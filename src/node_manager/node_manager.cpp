#include "node_manager.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QDialog>

#include <QStyledItemDelegate>
#include <QSize>
#include <QColor>

#include <coin-node/system.hpp>
#include <coin-node/package.hpp>
#include <super_panel/super_panel.hpp>
#include <focuse_panel/focuse_panel.hpp>

namespace coin::node
{

namespace impl
{
} // namespace impl

NodeTableWidget::NodeTableWidget(QWidget *parent) : QWidget(parent)
  , __m_layout(new QVBoxLayout(this))
{
    __m_layout->setAlignment(Qt::AlignTop);
}

NodeTableWidget::~NodeTableWidget()
{}

void NodeTableWidget::addNodeItem(NodeItemWidget *item)
{
    __m_layout->addWidget(item);
}

void NodeTableWidget::addNodeItem(QWidget *item)
{
    __m_layout->addWidget(item);
}

void NodeTableWidget::removeNodeItem(NodeItemWidget *item)
{
    __m_layout->removeWidget(item);
}

QList<DisplayWidget *> NodeTableWidget::items()
{
    QList<DisplayWidget*> list;
    for(int i = 0; i < __m_layout->count(); i++)
    {
        list.append( static_cast<DisplayWidget*>( __m_layout->itemAt(i)->widget() ));
    }
    return list;
}

NodeManagerWidget::NodeManagerWidget(QWidget* parent) : QWidget(parent)
{

    auto* layout = new QVBoxLayout(this);

    __m_nodeTable = new NodeTableWidget(this);
    __m_nodeTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("background-color: #2A2A2A; border-radius: 10px; padding: 10px;");
    scrollArea->verticalScrollBar()->setStyleSheet(
                                     "QScrollBar:vertical {"
                                     "    background-color: #3C3C3C;"  // 金属灰色
                                     "    width: 10px;"
                                     "    margin: 0px 0px 0px 0px;"
                                     "}"
                                     "QScrollBar::handle:vertical {"
                                     "    background-color: #B0B0B0;"  // 滑块颜色
                                     "    border-radius: 5px;"
                                     "    min-height: 20px;"
                                     "}"
                                     "QScrollBar::add-line:vertical {"
                                     "    height: 0px;"
                                     "    subcontrol-position: bottom;"
                                     "    subcontrol-origin: margin;"
                                     "}"
                                     "QScrollBar::sub-line:vertical {"
                                     "    height: 0px;"
                                     "    subcontrol-position: top;"
                                     "    subcontrol-origin: margin;"
                                     "}");
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(__m_nodeTable);
    layout->addWidget(scrollArea);

    auto* createBtn = new QPushButton("Create", this);
    // createBtn->setStyleSheet("background-color: darkblue;");

    createBtn->setStyleSheet("QPushButton {"
                                "   border: 2px solid darkblue;"
                                "   font-weight: bold;"
                                "   border-radius: 10px;"
                                "   background-color: darkblue;"
                                "   color: white;"
                                "   padding: 10px 20px;"
                                "}"
                                "QPushButton:hover {"
                                "   background-color: blue;"
                                "}"
                                "QPushButton:pressed {"
                                "   background-color: #2A2A2A;"
                                "}");


    createBtn->setFont(QFont("Arial", 16));
    createBtn->setMinimumHeight(50);
    // 获取按钮的QPalette对象
    QPalette pal = createBtn->palette();

    // 设置字体颜色
    pal.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    createBtn->setPalette(pal);

    connect(createBtn, &QPushButton::clicked, this, &NodeManagerWidget::onAddNodeItem);
    layout->addWidget(createBtn);


    // 扫描工作目录
    auto pkgs = Package::scanPackage(SystemInfo::info().getParam<std::string>("workspace.path"));
    for(auto& pkg : pkgs)
    {
        auto display = new DisplayWidget(pkg->info(), this);
        connect(display, &DisplayWidget::detail, this, &NodeManagerWidget::onEditNodeItem);
        __m_nodeTable->addNodeItem(display);
    }

}

NodeManagerWidget::~NodeManagerWidget()
{

}

QList<DisplayWidget *> NodeManagerWidget::nodes()
{
    return __m_nodeTable->items();
}

void NodeManagerWidget::onEditNodeItem()
{
    auto display = qobject_cast<DisplayWidget*>(sender());
    if(display)
    {
        auto* itemWidget = new NodeItemWidget(display->info());
        itemWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        itemWidget->setStyleSheet("");
        coin::superpanel::SuperPanelWidget::superpanel().addPinWidget(itemWidget);

        FocusePanel::instance()->add_focuse_widget(QString::fromStdString("$$PACKAGE:" + display->info().name()), itemWidget, [itemWidget]{
            itemWidget->deleteLater();
        });

        // auto layout = new QHBoxLayout();
        // layout->addWidget(itemWidget);
        // QDialog* addItem = new QDialog();
        // addItem->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        // addItem->setLayout(layout);

        // connect(itemWidget, &NodeItemWidget::removed, [this, display, addItem]{
        //     display->deleteLater();
        //     addItem->close();
        // });

        // addItem->exec();
    }
}

void NodeManagerWidget::onRemoveNodeItem()
{
}

void NodeManagerWidget::onAddNodeItem()
{

    auto* itemWidget = new NodeItemWidget();
    auto layout = new QHBoxLayout();
    layout->addWidget(itemWidget);
    QDialog* addItem = new QDialog();
    addItem->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    addItem->setLayout(layout);

    connect(itemWidget, &NodeItemWidget::canceled, [this, addItem]{
        addItem->close();
    });

    connect(itemWidget, &NodeItemWidget::created, [this, addItem, itemWidget](coin::node::PackageBaseInfo info){

        auto display = new DisplayWidget(info, this);
        __m_nodeTable->addNodeItem(display);
        connect(display, &DisplayWidget::detail, this, &NodeManagerWidget::onEditNodeItem);

        addItem->close();

        emit addNode(info);
    });

    addItem->exec();
    // __m_nodeTable->addNodeItem(itemWidget);

    // connect(itemWidget, &NodeItemWidget::nodeCanceled, [this, itemWidget]{
    //     this->__m_nodeTable->removeNodeItem(itemWidget);
    // });

}


}