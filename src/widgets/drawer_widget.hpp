/**
 * @file drawer_widget.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QPushButton>
#include <QVBoxLayout>

class DrawerWidgetItem : public QWidget
{
    Q_OBJECT
public:
    explicit DrawerWidgetItem(const QString& title, QWidget* parent = nullptr);
    virtual ~DrawerWidgetItem();

    void setWidget(QWidget* widget);
    QWidget* reset_widget(QWidget* widget);
private slots:
    void change_display();
signals:
    void to_focuse();
private:
    QWidget* widget_;
};

class DrawerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawerWidget(QWidget* parent = nullptr);
    ~DrawerWidget();
    void addItem(DrawerWidgetItem* item);

private:
    QScrollArea* scroll_area_;
    QVBoxLayout* layout_;
};