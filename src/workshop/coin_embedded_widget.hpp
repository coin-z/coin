/**
 * @file NodeEmbeddedWidget.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QJsonObject>
#include <coin-node/package.hpp>
#include <widgets/terminal_widget.hpp>
#include <widgets/env_editor.hpp>


namespace coin::workshop
{

class OperationButton : public QPushButton
{
    Q_OBJECT
public:
    explicit OperationButton(const QString& text, QWidget* parent = nullptr);
    ~OperationButton() = default;
};

class OnNodeExitEvent : public QEvent
{
public:
    static const QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 1);
    OnNodeExitEvent(const int st);
    ~OnNodeExitEvent() = default;

    int state;
};

class OnNodeDataReadyEvent : public QEvent
{
public:
    static const QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 2);
    OnNodeDataReadyEvent(const QString& t);
    ~OnNodeDataReadyEvent() = default;
    QString text;
};

class NodeEmbeddedWidget : public QWidget
{
    Q_OBJECT
public:
    NodeEmbeddedWidget(const coin::node::PackageBaseInfo& info, QWidget* parent = nullptr);
    virtual ~NodeEmbeddedWidget();

    const QString name() const;
    const std::string node_comm_list() const;
    const std::string run_params() const;
    void set_run_params(const std::string& params);

    void load_env(char** env);

public slots:
    void changeNodeName(const QString& name);
    void loadPortInfo(const QJsonObject& info);

private slots:
    void start();
    void focuse_widget();

signals:
    void node_start();
    void node_stop();
    void node_exit(const int st);

private:
    QString name_;
    TerminalWidget* terminal_widget_;
    QLineEdit* start_params_editor_;
    coin::node::PackageBaseInfo info_;
    QPushButton* runBtn_;
    QJsonObject portInfo_; 

    QWidget* place_holder_;
    EnvEditor* env_editor_;

    void reset_focurse(QWidget* w);

protected:
    bool event(QEvent *event) override;
    virtual void paintEvent(QPaintEvent* event) override;
};

} // coin::workshop