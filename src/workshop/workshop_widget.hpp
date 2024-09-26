#pragma once
#include <QWidget>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtNodes/NodeDelegateModel>

#include <memory>
#include <map>

#include <coin-node/package.hpp>

#include <workshop/dataflow_graphmodel.hpp>
#include <workshop/coin_node_painter.hpp>
#include <workshop/coin_graphics_scene.hpp>

using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDelegateModelRegistry;


namespace coin::workshop
{

class WorkshopWidget : public QWidget
{

    Q_OBJECT
public:
    explicit WorkshopWidget(QWidget* parent = nullptr);
    ~WorkshopWidget();

    inline GraphicsView* view() { return view_; }

private:
    std::shared_ptr<NodeDelegateModelRegistry> registry;
    // DataFlowGraphModel* dataFlowGraphModel;
    CoinGraphicsScene* scene_;
    GraphicsView* view_;

    void __loadAllNodes(std::shared_ptr<NodeDelegateModelRegistry> registry);

    std::unique_ptr<NodeDelegateModel> dataModelMaker_(const coin::node::PackageBaseInfo& info);

    std::unique_ptr<QtNodes::AbstractNodePainter> setup_painter_();

private slots:
    void onDropObject(const QPointF& pos, const QString& path);
    void onNodeClicked(QtNodes::NodeId const nodeId);

public slots:
    void onAddNodeItem(const coin::node::PackageBaseInfo& node);
    void onSaveNodes();
    void onLoadNodes();

};

} // namespace coin::workshop
