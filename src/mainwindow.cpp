#include "mainwindow.hpp"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QDir>
#include <QFileDialog>
#include <QTabWidget>
#include <QTextEdit>
#include <QDrag>
#include <QDockWidget>

#include <iostream>
#include <filesystem>

#include <coin-node/system.hpp>

#include "workshop/workshop_widget.hpp"
#include "super_panel/super_panel.hpp"
#include "focuse_panel/focuse_panel.hpp"

#include "coin-viz/viz_widget.hpp"
#include "coin-viz/viz_item.hpp"
#include "coin-viz/pointcloud_viz_item.hpp"
#include "coin-viz/line_viz_item.hpp"
#include "coin-viz/rect_viz_item.hpp"
#include "coin-viz/cube_viz_item.hpp"

/**
 * @brief Construct a new Main Window:: Main Window object
 * mainwindow has four main components:
 * 1. a node manager side bar
 * 2. a workshop area
 * 3. a super panel for assit development
 * 4. a float super assist panel
*/

using namespace coin;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 夜光蓝：#186FAF、#00BFFF、#D1EEEE、#FFFFFF、#0F1A20
    SystemInfo::info().load();

    this->resize(
        SystemInfo::info().getParam<int>("mainwindow.width"), 
        SystemInfo::info().getParam<int>("mainwindow.height")
    );
    auto pale = this->palette();
    pale.setBrush(QPalette::Window, QBrush(QColor(0x0A, 0x2D, 0x52)));
    // pale.setBrush(QPalette::Window, QBrush(QColor(0x2A, 0x2A, 0x2A)));
    this->setPalette(pale);

    // 检查当前是否已经设置了工作空间
    auto ws_path = SystemInfo::info().getParam<std::string>("workspace.path");
    if(ws_path.empty() or not std::filesystem::exists(ws_path)) {
        // 如果没有设置工作空间，则弹出对话框，让用户选择工作空间
        auto dir = QFileDialog::getExistingDirectory(this, "Select a workspace", QDir::currentPath());
        if(dir != "") {
            SystemInfo::info().setParam("workspace.path", dir.toStdString());
            ws_path = dir.toStdString();
        }
        else
        {
            // 如果用户没有选择工作空间，则退出程序
            this->close();
        }
    }

    auto* center = new QWidget(this);
    setCentralWidget(center);
    auto* layout = new QHBoxLayout(center);

    auto* workshop = new ::workshop::WorkshopWidget(center);
    layout->addWidget(workshop);

    // 创建状态栏
    QStatusBar *statusBar = new QStatusBar();
    this->setStatusBar(statusBar);

    // 在状态栏上添加标签
    QLabel *statusLabel = new QLabel("workspace: " + QString::fromStdString( ws_path ));
    statusBar->addWidget(statusLabel);

    // node manager
    m_nodeManager = new ::node::NodeManagerWidget(center);

    // super panel
    auto* superPanel = &coin::superpanel::SuperPanelWidget::superpanel();
    m_floatWidget = new FloatWidget(this);
    m_floatWidget->setFixedSize(SystemInfo::info().getParam<int>("superpanel.width"), 
                                SystemInfo::info().getParam<int>("superpanel.height"));
    m_floatWidget->setWidget(superPanel);

    m_floatWidget->move(SystemInfo::info().getParam<int>("superpanel.x"), SystemInfo::info().getParam<int>("superpanel.y"));

    // m_floatWidget->hide();

    // focuse panel
    m_floatFocusePanel = new FloatWidget(this);
    m_floatFocusePanel->setFixedSize(SystemInfo::info().getParam<int>("focusepanel.width"),
                                     SystemInfo::info().getParam<int>("focusepanel.height"));
    m_floatFocusePanel->setWidget(FocusePanel::instance());
    m_floatFocusePanel->move(SystemInfo::info().getParam<int>("focusepanel.x"), SystemInfo::info().getParam<int>("focusepanel.y"));
    connect(FocusePanel::instance(), &FocusePanel::item_isclear, [this]{
        m_floatFocusePanel->hide();
    });
    connect(FocusePanel::instance(), &FocusePanel::add_newitem, [this]{
        m_floatFocusePanel->show();
    });

    m_floatFocusePanel->hide();

    connect(m_nodeManager, &::node::NodeManagerWidget::addNode, workshop, &::workshop::WorkshopWidget::onAddNodeItem);

    // control panel
    m_controlPanel = new coin::control_panel::ControlPanel(this);
    connect(m_controlPanel, &coin::control_panel::ControlPanel::loadNodes, workshop, &::workshop::WorkshopWidget::onLoadNodes);
    connect(m_controlPanel, &coin::control_panel::ControlPanel::saveNodes, workshop, &::workshop::WorkshopWidget::onSaveNodes);

}

MainWindow::~MainWindow()
{
    SystemInfo::info().setParam<int>("mainwindow.width", width());
    SystemInfo::info().setParam<int>("mainwindow.height", height());

    // Super panel
    SystemInfo::info().setParam<int>("superpanel.x", m_floatWidget->pos().x());
    SystemInfo::info().setParam<int>("superpanel.y", m_floatWidget->pos().y());
    SystemInfo::info().setParam<int>("superpanel.width", m_floatWidget->width());
    SystemInfo::info().setParam<int>("superpanel.height", m_floatWidget->height());

    // Focuse panel
    SystemInfo::info().setParam<int>("focusepanel.x", m_floatFocusePanel->pos().x());
    SystemInfo::info().setParam<int>("focusepanel.y", m_floatFocusePanel->pos().y());
    SystemInfo::info().setParam<int>("focusepanel.width", m_floatFocusePanel->width());
    SystemInfo::info().setParam<int>("focusepanel.height", m_floatFocusePanel->height());

    SystemInfo::info().save();

    this->close();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{

    const size_t space = 50;
    m_nodeManager->resize(300, event->size().height() - space * 2);
    m_nodeManager->move(20, space);
    m_nodeManager->update();

    // 如果 floatwidget 位置超出当前窗口范围则移动到窗口边界
    if(m_floatWidget->pos().x() + m_floatWidget->width() > event->size().width()) {
        m_floatWidget->move(event->size().width() - m_floatWidget->width(), m_floatWidget->pos().y());
    }
    if(m_floatWidget->pos().y() + m_floatWidget->height() > event->size().height()) {
        m_floatWidget->move(m_floatWidget->pos().x(), event->size().height() - m_floatWidget->height());
    }
    m_floatWidget->update();

    // 设置 control panel 位置在右下角
    m_controlPanel->move(event->size().width() - m_controlPanel->width() - 20, event->size().height() - m_controlPanel->height() - 20);
    m_controlPanel->update();

}
