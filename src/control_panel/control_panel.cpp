/**
 * @file control_panel.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "control_panel.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

namespace coin::control_panel
{
ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumSize(200, 60);
    auto layout = new QHBoxLayout(this);
    auto loadBtn = new QPushButton("Load", this);
    setBtnStyle(loadBtn);
    layout->addWidget(loadBtn);

    auto saveBtn = new QPushButton("Save", this);
    setBtnStyle(saveBtn);
    layout->addWidget(saveBtn);
    
    connect(loadBtn, &QPushButton::clicked, this, &ControlPanel::loadNodes);
    connect(saveBtn, &QPushButton::clicked, this, &ControlPanel::saveNodes);
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::setBtnStyle(QPushButton *btn)
{
    btn->setStyleSheet("QPushButton {"
                                "   border: 2px solid #072A24;"
                                "   font-weight: bold;"
                                "   border-radius: 10px;"
                                "   background-color: #072A24;"
                                "   color: white;"
                                "   padding: 10px 20px;"
                                "}"
                                "QPushButton:hover {"
                                "   background-color: #689C97;"
                                "}"
                                "QPushButton:pressed {"
                                "   background-color: #2A2A2A;"
                                "}");
}

} // namespace coin::control_panel

