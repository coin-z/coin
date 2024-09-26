/**
 * @file control_panel.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <QWidget>
#include <QPushButton>

namespace coin::control_panel
{

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    ControlPanel(QWidget* parent = nullptr);
    ~ControlPanel();

signals:
    void loadNodes();
    void saveNodes();

private:
    void setBtnStyle(QPushButton* btn);

};

} // namespace coin::control_panel

