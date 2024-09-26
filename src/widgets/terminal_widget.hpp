/**
 * @file terminal_widget.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextDocument>
#include <QEvent>
#include <QResizeEvent>

#include <widgets/float_widget.hpp>


namespace coin
{

class TerminalWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit TerminalWidget(QWidget* parent = nullptr);
    ~TerminalWidget() = default;

    static QString ansiToHtml(const QString &ansiString);

    void enableAutoResize();

signals:
    void contentsChanged();
    void dataReceived();
    void sizeChanged();

protected:
    bool event(QEvent *event) override;

private:
    bool enable_auto_resize_ = false;

public:
class OnDataReceived : public QEvent
{
public:
    static const QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 1);
    OnDataReceived();
    ~OnDataReceived() = default;
};

};
}