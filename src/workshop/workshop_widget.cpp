#include "workshop_widget.hpp"

#include <QtDebug>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QDialog>
#include <QInputDialog>

#include <filesystem>
#include <coin-node/system.hpp>
#include <coin-node/package.hpp>

#include <QUndoStack>
#include <QUndoCommand>

#include <QtNodes/DefaultNodePainter>
#include "QtNodes/internal/NodeGraphicsObject.hpp"
#include "coin_embedded_widget.hpp"
#include "coin_node_datamodel.hpp"

namespace coin::workshop
{
static void setNodeStyle()
{
    QtNodes::GraphicsViewStyle::setStyle(
        R"(
        {
  "GraphicsViewStyle": {
    "BackgroundColor": [53, 53, 53],
    "FineGridColor": [60, 60, 60],
    "CoarseGridColor": [25, 25, 25]
  }}
  )");

    QtNodes::NodeStyle::setNodeStyle(
        R"(
        {
  "NodeStyle": {
    "NormalBoundaryColor": [255, 255, 255],
    "SelectedBoundaryColor": [255, 165, 0],
    "GradientColor0": "gray",
    "GradientColor1": [80, 80, 80],
    "GradientColor2": [64, 64, 64],
    "GradientColor3": [58, 58, 58],
    "ShadowColor": [20, 20, 20],
    "FontColor" : "white",
    "FontColorFaded" : "gray",
    "ConnectionPointColor": [169, 169, 169],
    "FilledConnectionPointColor": "cyan",
    "ErrorColor": "red",
    "WarningColor": [128, 128, 0],

    "PenWidth": 1.0,
    "HoveredPenWidth": 1.5,

    "ConnectionPointDiameter": 8.0,

    "Opacity": 0.8
  }}
  )");

    QtNodes::ConnectionStyle::setConnectionStyle(
        R"(
        {
  "ConnectionStyle": {
    "ConstructionColor": "gray",
    "NormalColor": "darkcyan",
    "SelectedColor": [100, 100, 100],
    "SelectedHaloColor": "orange",
    "HoveredColor": "lightcyan",

    "LineWidth": 3.0,
    "ConstructionLineWidth": 2.0,
    "PointDiameter": 10.0,

    "UseDataDefinedColors": false
  }
  }
  )");
}


WorkshopWidget::WorkshopWidget(QWidget *parent) : QWidget(parent)
{
    setNodeStyle();
    scene_ = new CoinGraphicsScene(CoinDataFlowGraphModel::graphModel());

    scene_->setNodePainter(setup_painter_());

    connect(scene_, &CoinGraphicsScene::nodeClicked, this, &WorkshopWidget::onNodeClicked);

    __loadAllNodes(CoinDataFlowGraphModel::graphModel().dataModelRegistry());

    view_ = new GraphicsView(scene_);
    view_->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);

    auto* layout = new QHBoxLayout(this);
    layout->addWidget(view_);

    connect(view_, &GraphicsView::dropObject, this, &WorkshopWidget::onDropObject);

    auto workspace = SystemInfo::info().getParam<std::string>("workspace.workspace");
    if(not workspace.empty())
    {
        scene_->loadFromFile(QString::fromStdString(workspace));
    }
}

WorkshopWidget::~WorkshopWidget()
{
}

std::unique_ptr<QtNodes::AbstractNodePainter> WorkshopWidget::setup_painter_()
{
    auto painter = std::make_unique<CoinNodePainter>();
    return std::move(painter);
}

void WorkshopWidget::__loadAllNodes(std::shared_ptr<NodeDelegateModelRegistry> registry)
{
    // 在工作目录下查找全部开发目录模块
    auto ws_path = SystemInfo::info().getParam<std::string>("workspace.path");
    auto node_list = coin::node::Package::scanPackage(ws_path);

    for(auto& node : node_list)
    {
        // 加载节点信息文件
        registry->registerModel<CoinNodeDataModel>(
            std::bind(&WorkshopWidget::dataModelMaker_, this, node->info()),
            QString::fromStdString(node->info().category())
        );
    }
}

std::unique_ptr<NodeDelegateModel> WorkshopWidget::dataModelMaker_(const coin::node::PackageBaseInfo &info)
{
    auto model = std::make_unique<CoinNodeDataModel>( info );

    auto embeddedWidget = static_cast<NodeEmbeddedWidget*>(model->embeddedWidget());
    embeddedWidget->loadPortInfo(model->portConfigInfo());

    embeddedWidget->setStyleSheet("background-color: #2A2A2A; border-radius: 10px; padding: 10px;");

    connect(model.get(), &CoinNodeDataModel::nodeNameChanged, 
            embeddedWidget, &NodeEmbeddedWidget::changeNodeName);
    connect(model.get(), &CoinNodeDataModel::portInfoChanged,
            embeddedWidget, &NodeEmbeddedWidget::loadPortInfo);

    return model;
}

void WorkshopWidget::onSaveNodes()
{
    auto file = scene_->save();
    SystemInfo::info().setParam<std::string>("workspace.workspace", file.toStdString());
}

void WorkshopWidget::onLoadNodes()
{
    scene_->load();
}

void WorkshopWidget::onAddNodeItem(const coin::node::PackageBaseInfo &node)
{
    // 加载节点信息文件
    auto ws_path = SystemInfo::info().getParam<std::string>("workspace.path");
    CoinDataFlowGraphModel::graphModel().dataModelRegistry()->registerModel<CoinNodeDataModel>(
        std::bind(&WorkshopWidget::dataModelMaker_, this, node),
        QString::fromStdString(node.category())
    );
}

void WorkshopWidget::onDropObject(const QPointF& pos, const QString& path)
{
    // 在工作目录下查找全部开发目录模块
    auto node_path = SystemInfo::info().getParam<std::string>("workspace.path") + "/" + path.toStdString();
    coin::node::Package pkg(node_path);

    bool ok;
    QString node_name = QInputDialog::getText(this, tr("Input"),
                                         tr("Node Name:"), QLineEdit::Normal,
                                         QString::fromStdString(pkg.info().name()), &ok);

    if(not ok || node_name.isEmpty())
    {
        return;
    }
    
    // 检查该节点是否已经存在，如果已经存在则不可添加
    auto nodeIds = CoinDataFlowGraphModel::graphModel().allNodeIds();
    for(auto &id : nodeIds)
    {
        auto model = CoinDataFlowGraphModel::graphModel().delegateModel<CoinNodeDataModel>(id);
        if(model)
        {
            if(model->nodeName() == node_name)
            {
                QMessageBox::warning(this, tr("Warning"), tr("Node <%0> already exists!").arg(node_name));
                return;
            }
        }
    }

    auto nodeId = scene_->addNode(QString::fromStdString(pkg.info().name()), view_->mapToScene(pos.x(), pos.y()));
    auto model = CoinDataFlowGraphModel::graphModel().delegateModel<CoinNodeDataModel>(nodeId);
    model->setName(node_name);
}

void WorkshopWidget::onNodeClicked(QtNodes::NodeId const nodeId)
{
    auto object = scene_->nodeGraphicsObject(nodeId);
}

} // coin::workshop
