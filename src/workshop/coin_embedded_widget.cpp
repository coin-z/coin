/**
 * @file NodeEmbeddedWidget.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "coin_embedded_widget.hpp"
#include <QVBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPainter>
#include <QCoreApplication>
#include <sys/types.h>
#include <signal.h>
#include <QtDebug>
#include <widgets/drawer_widget.hpp>
#include <focuse_panel/focuse_panel.hpp>



namespace coin::workshop
{

OperationButton::OperationButton(const QString& text, QWidget* parent)
  : QPushButton(text, parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setStyleSheet("QPushButton {"
                                "   border: 2px solid #0288D1;"
                                "   font-size: 12px;"
                                "   font-weight: bold;"
                                "   border-radius: 3px;"
                                "   background-color: #0288D1;"
                                "   color: white;"
                                "   padding: 1px 2px;"
                                "}"
                                "QPushButton:hover {"
                                "   background-color: #FF9800;"
                                "}"
                                "QPushButton:pressed {"
                                "   background-color: #212121;"
                                "}"
                                "QPushButton:checked {"
                                "   background-color: #FF5722;"
                                "}");
}


OnNodeExitEvent::OnNodeExitEvent(const int st) : QEvent(OnNodeExitEvent::EventType)
  , state(st)
{
    
}
OnNodeDataReadyEvent::OnNodeDataReadyEvent(const QString &t) : QEvent(OnNodeDataReadyEvent::EventType)
  , text(t)
{
}
NodeEmbeddedWidget::NodeEmbeddedWidget(const coin::node::PackageBaseInfo& info, QWidget *parent)
  : QWidget(parent)
  , info_(info)
  , terminal_widget_(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    terminal_widget_ = new TerminalWidget(this);

    auto* drawer = new DrawerWidget(this);
    
    // 终端输出显示
    auto* terminal_item = new DrawerWidgetItem("Terminal", this);
    terminal_item->setWidget(terminal_widget_);
    connect(terminal_item, &DrawerWidgetItem::to_focuse, this, &NodeEmbeddedWidget::focuse_widget);
    drawer->addItem(terminal_item);

    // 启动参数配置
    auto* start_params_item = new DrawerWidgetItem("Run Params", this);
    start_params_editor_ = new QLineEdit(this);
    // 设置样式为圆角、白底、黑字，粗体字、12px
    start_params_editor_->setStyleSheet("border-radius: 10px; background-color: white; color: black; font-weight: bold; font-size: 12px;");
    // start_params_editor_->setText(QString::fromStdString(info_.startParams()));
    start_params_item->setWidget(start_params_editor_);
    drawer->addItem(start_params_item);

    // 环境变量编辑器
    env_editor_ = new EnvEditor(this);
    auto* env_item = new DrawerWidgetItem("Environment Editor", this);
    env_item->setWidget(env_editor_);
    drawer->addItem(env_item);

    layout->addWidget(drawer);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setAlignment(Qt::AlignRight);
    layout->addLayout(btnLayout);

    runBtn_ = new OperationButton("Run", this);
    runBtn_->setCheckable(true);
    connect(runBtn_, &QPushButton::clicked, this, &NodeEmbeddedWidget::start);
    btnLayout->addWidget(runBtn_);

    auto reloadBtn = new OperationButton("Reload", this);
    btnLayout->addWidget(reloadBtn);
}
bool NodeEmbeddedWidget::event(QEvent *event)
{
    if(event->type() == OnNodeExitEvent::EventType)
    {
        runBtn_->setText("Run");
        runBtn_->setChecked(false);
        emit node_exit(static_cast<OnNodeExitEvent*>(event)->state);
        return true;
    }
    else if(event->type() == OnNodeDataReadyEvent::EventType)
    {
        auto text = static_cast<OnNodeDataReadyEvent*>(event)->text;
        terminal_widget_->append(text);
        return true;
    }
    else if(event->type() == QEvent::Paint)
    {
        paintEvent(static_cast<QPaintEvent*>(event));
        return true;
    }
    return QWidget::event(event);
}
NodeEmbeddedWidget::~NodeEmbeddedWidget()
{

}
const QString NodeEmbeddedWidget::name() const
{
    return name_;
}
void NodeEmbeddedWidget::load_env(char** env)
{
    QVector<std::pair<QString, QString>> envs;
    while(*env)
    {
        const auto item = std::string(*env);
        auto pos = item.find('=');
        if (pos != std::string::npos)
        {
            auto key_item = item.substr(0, pos);
            envs.append({key_item.c_str(), item.substr(pos + 1).c_str()});
        }
        env++;
    }
    env_editor_->loadEnv(envs);
}
const std::string NodeEmbeddedWidget::node_comm_list() const
{
    std::string list;
    for(auto itor = portInfo_.begin(); itor != portInfo_.end(); itor++)
    {
        list += itor.key().toStdString();
        list += ":";
        list += itor.value().toString().toStdString();
        list += ";";
    }
    return std::move(list);
}
const std::string NodeEmbeddedWidget::run_params() const
{
    return start_params_editor_->text().toStdString();
}
void NodeEmbeddedWidget::set_run_params(const std::string& params)
{
    start_params_editor_->setText(QString::fromStdString(params));
}
void NodeEmbeddedWidget::start()
{
    if(runBtn_->isChecked())
    {
        runBtn_->setText("Stop");
        emit node_start();
    }
    else
    {
        emit node_stop();
        runBtn_->setText("Run");
    }
}
void NodeEmbeddedWidget::focuse_widget()
{
    auto* focuse_label = new QLabel("Foucused Now", this);
    focuse_label->setAlignment(Qt::AlignCenter);
    focuse_label->setStyleSheet("background-color: gray;");
    focuse_label->setGeometry(0, 0, 10, 10);
    
    auto* drawer_item = static_cast<DrawerWidgetItem*>(sender());
    auto* w = drawer_item->reset_widget(focuse_label);
    FocusePanel::instance()->add_focuse_widget(name_, w, [=](){
        drawer_item->reset_widget(w);
    });
}

void NodeEmbeddedWidget::reset_focurse(QWidget* w)
{

}

void NodeEmbeddedWidget::changeNodeName(const QString& name)
{
    name_ = name;
}

void NodeEmbeddedWidget::loadPortInfo(const QJsonObject &info)
{
    portInfo_ = info;
}

void NodeEmbeddedWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QSize size = this->size();

    QPen p(QColor(58, 58, 58), 3);
    painter.setPen(p);

    QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(2.0, size.height()));

    gradient.setColorAt(0.0, QColor(58, 58, 58));
    gradient.setColorAt(0.10, QColor(80, 80, 80));
    gradient.setColorAt(0.90, QColor(64, 64, 64));
    gradient.setColorAt(1.0, QColor(58, 58, 58));

    painter.setBrush(gradient);

    QRectF boundary(0, 0, size.width(), size.height());

    double const radius = 0.0;

    painter.drawRoundedRect(boundary, radius, radius);
}

} // namespace coin::workshop
