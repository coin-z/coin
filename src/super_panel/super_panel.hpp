#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QLabel>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QProcess>
#include <QTimer>
#include <memory>

#include <widgets/float_widget.hpp>
#include <super_panel/command_processor.hpp>

#include <coin-commons/executor/executor.hpp>

namespace coin::superpanel
{

class TableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TableWidget(QWidget* parent = nullptr);
    ~TableWidget() = default;
    // void paintEvent(QPaintEvent* event) override;
};

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ScrollArea(QWidget* parent = nullptr);
    ~ScrollArea() = default;

// protected:
//     void paintEvent(QPaintEvent* event) override;
};

class SuperPanelWidget : public QWidget
{
    Q_OBJECT
public:

    SuperPanelWidget(const SuperPanelWidget&) = delete;
    const SuperPanelWidget& operator = (const SuperPanelWidget&) = delete;

    virtual ~SuperPanelWidget();

    static SuperPanelWidget& superpanel();

    FloatWidget* addMessageWidget();

    void addCommand(const QString& cmd, const QStringList& args);
    void addCommand(const QString& cmdLine);
    void addCommandLine(const QString& cmdLine);
    void addHttpRequest(const QString& url, const QString& context);

    void addPinWidget(QWidget* widget);

    void paintEvent(QPaintEvent *event) override;

private:
    SuperPanelWidget(QWidget* parent = nullptr);
    QListWidget* __m_chatMessageList;
    QLineEdit* __m_msgEditLine;
    QPushButton* __m_sendBtn;

    QVBoxLayout* __m_tableLayout;
    QHBoxLayout* __m_conversitionLayout;

    ScrollArea* __m_scrollArea;

    QTimer __m_scrollTimer;

    coin::Executor __m_terminal;
    CommandProcessor __m_commandProcessor;

    QWidget* __m_pinWidget;

    QMap<QProcess*, std::shared_ptr<QProcess> > __m_cmdList;

private slots:
    void processExit(int exitCode, QProcess::ExitStatus exitStatus);
    void launchProcess_(QLineEdit* w);
    void updateScrollBarPos();

};
    
} // namespace coin::floatwindow

