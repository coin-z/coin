/**
 * @file drawer_widget.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "drawer_widget.hpp"
#include <QVBoxLayout>

DrawerWidgetItem::DrawerWidgetItem(const QString& title, QWidget* parent)
    : QWidget(parent)
    , widget_(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* top_layout = new QHBoxLayout();
    top_layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(top_layout);
    QPushButton* btn = new QPushButton(title, this);
    btn->setStyleSheet("color: white; font-size: 18px; font-weight: bold; border: 3px solid green; border-radius: 10px; padding: 10px;");
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    top_layout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, &DrawerWidgetItem::change_display);

    QPushButton* fbtn = new QPushButton("F", this);
    fbtn->setStyleSheet("color: white; font-size: 18px; font-weight: bold; border: 3px solid green; border-radius: 10px; padding: 10px;");
    fbtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    top_layout->addWidget(fbtn);
    connect(fbtn, &QPushButton::clicked, this, &DrawerWidgetItem::to_focuse);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

DrawerWidgetItem::~DrawerWidgetItem()
{

}

void DrawerWidgetItem::setWidget(QWidget* widget)
{
    widget_ = widget;
    this->layout()->addWidget(widget);
}

QWidget* DrawerWidgetItem::reset_widget(QWidget* widget)
{
    this->layout()->removeWidget(widget_);
    this->layout()->addWidget(widget);

    auto* w = widget_;
    widget_ = widget;
    return w;
}

void DrawerWidgetItem::change_display()
{
    if(not widget_) return;
    if (widget_->isVisible())
    {
        widget_->hide();
    }
    else
    {
        widget_->show();
    }
}


DrawerWidget::DrawerWidget(QWidget* parent)
    : QWidget(parent)
    , scroll_area_(nullptr)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);
    scroll_area_ = new QScrollArea(this);
    layout->addWidget(scroll_area_);
    scroll_area_->setWidgetResizable(true);

    auto* w = new QWidget(this);
    scroll_area_->setWidget(w);

    layout_ = new QVBoxLayout();
    layout_->setAlignment(Qt::AlignTop);
    w->setLayout(layout_);
}

DrawerWidget::~DrawerWidget()
{

}

void DrawerWidget::addItem(DrawerWidgetItem* item)
{
    layout_->addWidget(item);
}
