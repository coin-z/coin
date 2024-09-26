/**
 * @file focuse_panel.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "focuse_panel.hpp"
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

FocuseWidget::FocuseWidget(QWidget* w, QWidget* parent) : QWidget(parent)
{
    QPushButton* cancel_btn = new QPushButton("CANCEL FOCUSE", this);
    cancel_btn->setStyleSheet("QPushButton {"
                                "   border: 2px solid #4CAF50;"
                                "   font-weight: bold;"
                                "   border-radius: 10px;"
                                "   background-color: #4CAF50;"
                                "   color: white;"
                                "   padding: 10px 20px;"
                                "}"
                                "QPushButton:hover {"
                                "   background-color: #45a049;"
                                "}"
                                "QPushButton:pressed {"
                                "   background-color: red;"
                                "}");
    connect(cancel_btn, &QPushButton::clicked, this, &FocuseWidget::cancel_focuse);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);
    
    layout->addWidget(w);
    layout->addWidget(cancel_btn);
}

FocuseWidget::~FocuseWidget()
{
}

//////////////////////////////////////////////////////////////////////

FocusePanelItem::FocusePanelItem(const QString& name, QWidget* parent) : QWidget(parent)
  , name_(name)
{
    // 设置当前 widget 样式为圆角、3px 边框
    // setStyleSheet("background-color: #212121; border-radius: 10px; border: 3px solid green;");
    auto* layout = new QVBoxLayout(this);
    auto* name_label = new QLabel(name, this);
    name_label->setStyleSheet("color: white; font-size: 18px; font-weight: bold; border: 3px solid green; border-radius: 10px; padding: 10px;");
    layout->addWidget(name_label);
    setLayout(layout);
}

FocusePanelItem::~FocusePanelItem()
{
}

void FocusePanelItem::add_focuse_widget(QWidget* w, const std::function<void()>& cancel_callback)
{
    auto* widget = new FocuseWidget(w, this);
    connect(widget, &FocuseWidget::cancel_focuse, this, [this, cancel_callback, widget]() {
        // 在当前 layout 中移除 widget
        layout()->removeWidget(widget);
        // 取消 focuse
        cancel_callback();
        widget->deleteLater();

        if(layout()->count() == 1)
        {
            emit item_isclear();
        }
    });
    layout()->addWidget(widget);

    if(w)
    {
        w->repaint();
    }
}

FocusePanel* FocusePanel::instance()
{
    static FocusePanel* panel = new FocusePanel();
    return panel;
}
FocusePanel::FocusePanel(QWidget *parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

FocusePanel::~FocusePanel()
{
}

void FocusePanel::add_focuse_widget(const QString& name, QWidget* w, const std::function<void()>& cancel_callback)
{
    auto item = items_.find(name);
    if(item == items_.end())
    {
        items_[name] = new FocusePanelItem(name, this);
        item = items_.find(name);
        layout()->addWidget(item->second);
        connect(item->second, &FocusePanelItem::item_isclear, this, [this]() {
            auto* w = static_cast<FocusePanelItem*>(sender());
            items_.erase(w->name());
            w->deleteLater();

            if(items_.empty())
            {
                emit item_isclear();
            }
        });
        
        emit add_newitem();
    }

    item->second->add_focuse_widget(w, cancel_callback);
}
