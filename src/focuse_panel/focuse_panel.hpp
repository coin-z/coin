/**
 * @file focuse_panel.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <QWidget>

#include <map>

class FocuseWidget : public QWidget
{
    Q_OBJECT
public:
    FocuseWidget(QWidget* w, QWidget* parent = nullptr);
    virtual ~FocuseWidget();

signals:
    void cancel_focuse();
};

class FocusePanelItem : public QWidget
{
    Q_OBJECT
public:
    explicit FocusePanelItem(const QString& name, QWidget* parent = nullptr);
    ~FocusePanelItem();

    void add_focuse_widget(QWidget* w, const std::function<void()>& cancel_callback);

    inline const QString name() const { return name_; }
signals:
    void item_isclear();

private:
    const QString name_;
};

class FocusePanel : public QWidget
{
    Q_OBJECT
public:
    FocusePanel(const FocusePanel&) = delete;
    FocusePanel& operator=(const FocusePanel&) = delete;
    virtual ~FocusePanel();

    static FocusePanel* instance();

    void add_focuse_widget(const QString& name, QWidget* w, const std::function<void()>& cancel_callback);
signals:
    void item_isclear();
    void add_newitem();
private:
    explicit FocusePanel(QWidget *parent = nullptr);

private:
    std::map<QString, FocusePanelItem*> items_;
};
