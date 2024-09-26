#include "super_panel.hpp"
#include <QPainter>
#include <QDebug>

#include <QTextEdit>
#include <QScrollArea>
#include <QScrollBar>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QInputDialog>

#include <widgets/image_viewer.hpp>

#include <iostream>
#include <regex>
#include <unistd.h>
#include <signal.h>

#include <widgets/terminal_widget.hpp>


static QString textToHtml(const QString& text, const QString head = "<font color=black>", const QString tail = "</font>")
{
    auto t = text;
    if(t.endsWith('\n'))
    {
        t = t.left(t.size() - 1);
    }
    auto lines = t.split('\n');
    QString html = "<p style='line-height: 1.0;'>" + head;
    size_t i = 0;
    for(i = 0; i < lines.size() - 1; i++)
    {
        html += lines[i] + "<br>";
    }
    html += lines[i] + (i == 0 ? "" : "<br>") + tail + "</p>";
    return html;
}

static QString ansiToHtml(const QString &ansiString)
{

    static QMap<QString, QString> colorMap{
        {"00", "black"},
        {"31", "red"},
        {"32", "green"},
        {"33", "yellow"},
        {"34", "blue"},
        {"35", "magenta"},
        {"36", "cyan"},
        {"37", "white"},
        {"39", "black"},
        {"40", "black"},
        {"41", "red"},
        {"42", "green"},
        {"43", "yellow"},
        {"44", "blue"},
        {"45", "magenta"},
        {"46", "cyan"},
        {"47", "white"},
        {"49", "black"},
    };

    QString htmlString;
    QTextStream stream(&htmlString);

    QString color = "black";

    QString html = "<p style='line-height: 1.5;'>";
    {
        bool is_recolor = true;
        bool is_start_ansi = false;
        QString ansi_str = "";
        QTextStream ansi_stream(&ansi_str);

        for(size_t i = 0; i < ansiString.length(); i++)
        {
            if(ansiString.at(i) == '\u001b')
            {
                ansi_str.clear();
                is_start_ansi = true;
                continue;
            }
            if(is_start_ansi && ansiString.at(i) == 'm')
            {
                auto info = ansi_str.split(';');
                QString color_code = info[info.size() - 1];
                is_start_ansi = false;
                ansi_str.clear();

                auto findColor = colorMap.find(color_code);
                if(findColor != colorMap.end())
                {
                    color = findColor.value();
                }
                is_recolor = true;
                continue;
            }
            if(is_start_ansi)
            {
                if(ansiString.at(i) != '[')
                { ansi_stream << ansiString.at(i); }
                continue;
            }

            if(is_recolor)
            {
                is_recolor = false;
                if(htmlString.isEmpty())
                {
                    stream << "<font color=" << color << ">";
                }
                else
                {
                    stream << "</font><font color=" << color << ">";
                }
            }
            
            if(ansiString.at(i) == '\n')
            {
                stream << "<br>";
            }
            else if(ansiString.at(i) != '\r')
            {
                stream << ansiString.at(i);
            }
        }
        stream << "</font>";
    }

    return htmlString;
}


namespace coin::superpanel
{

TableWidget::TableWidget(QWidget* parent) : QWidget(parent)
{ }

// void TableWidget::paintEvent(QPaintEvent* event)
// {
//     Q_UNUSED(event);
//     // 在这里添加绘制代码，例如绘制透明背景或其他内容
//     QPainter painter(this);

//     // 设置透明度，范围是0.0（完全透明）到1.0（完全不透明）
//     // painter.setOpacity(0.0);

//     // 绘制透明背景或其他内容
//     painter.fillRect(rect(), Qt::red);  // 使用白色背景，你可以使用其他颜色或渐变
// }

ScrollArea::ScrollArea(QWidget *parent) : QScrollArea(parent)
{
    // viewport()->setAttribute(Qt::WA_TranslucentBackground);
}

// void ScrollArea::paintEvent(QPaintEvent *event)
// {
//     QPainter painter(viewport());
//     painter.setOpacity(1.0);
//     painter.fillRect(viewport()->rect(), Qt::red);
//     QScrollArea::paintEvent(event);
// }

SuperPanelWidget::SuperPanelWidget(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* pinLayout = new QHBoxLayout();
    __m_pinWidget = new QWidget(this);
    __m_pinWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    __m_pinWidget->setLayout(new QVBoxLayout(__m_pinWidget));
    pinLayout->addWidget(__m_pinWidget);
    auto* table = new TableWidget();
    // table->setAttribute(Qt::WA_TranslucentBackground);
    table->setStyleSheet("background-color: #2a2a2a; border: 2px solid #4CAF50; border-radius: 10px; padding: 10px;");
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    __m_tableLayout = new QVBoxLayout(table);
    __m_tableLayout->setAlignment(Qt::AlignTop);
    __m_tableLayout->setMargin(0);

    __m_scrollArea = new ScrollArea(this);
    __m_scrollArea->setStyleSheet("background-color: white; border: 0; border-radius: 0px; padding: 0px;");


    __m_scrollArea->verticalScrollBar()->setStyleSheet(
                                     "QScrollBar:vertical {"
                                     "    background-color: #3C3C3C;"  // 金属灰色
                                     "    width: 10px;"
                                     "    margin: 0px 0px 0px 0px;"
                                     "}"
                                     "QScrollBar::handle:vertical {"
                                     "    background-color: #B0B0B0;"  // 滑块颜色
                                     "    border-radius: 5px;"
                                     "    min-height: 20px;"
                                     "}"
                                     "QScrollBar::add-line:vertical {"
                                     "    height: 0px;"
                                     "    subcontrol-position: bottom;"
                                     "    subcontrol-origin: margin;"
                                     "}"
                                     "QScrollBar::sub-line:vertical {"
                                     "    height: 0px;"
                                     "    subcontrol-position: top;"
                                     "    subcontrol-origin: margin;"
                                     "}");

    __m_scrollArea->setWidgetResizable(true);
    __m_scrollArea->setWidget(table);
    // __m_scrollArea->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    connect(__m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, [this, table](int){
        for(auto it = 0; it < __m_tableLayout->count(); it++)
        {
            auto w = static_cast<QWidget*>(__m_tableLayout->itemAt(it)->widget());
            w->repaint();
        }
        table->repaint();
        __m_scrollArea->repaint();
    });
    layout->addWidget(__m_scrollArea);

    __m_conversitionLayout = new QHBoxLayout();
    __m_conversitionLayout->setAlignment(Qt::AlignBottom);
    layout->addLayout(__m_conversitionLayout);

    QLineEdit* textInput = new QLineEdit(this);
    textInput->setStyleSheet("background-color: white; border: 2px solid #4CAF50; border-radius: 10px; padding: 10px;");
    QPushButton* sendBtn = new QPushButton("Hit Me", this);
    sendBtn->setStyleSheet("QPushButton {"
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
    sendBtn->setShortcut(QKeySequence::InsertParagraphSeparator);
    sendBtn->setShortcut(Qt::Key_Enter);
    sendBtn->setShortcut(Qt::Key_Return);
    connect(sendBtn, &QPushButton::clicked, std::bind(&SuperPanelWidget::launchProcess_, this, textInput));

    __m_conversitionLayout->addWidget(textInput);
    __m_conversitionLayout->addWidget(sendBtn);

    __m_commandProcessor.setupProcessor("image", [this](const std::string& cmd){
        auto img = new ImageViewer();
        img->openImage(QString::fromStdString(cmd));
        __m_tableLayout->addWidget(img);
    });

    __m_commandProcessor.setupExecutorProcessor([this](const std::string& cmd){
        addCommandLine(QString::fromStdString(cmd));
    });

    __m_commandProcessor.setupUnknownProcessor([this](const std::string& cmd, const std::string& args){
        qDebug() << QString::fromStdString(cmd) << QString::fromStdString(args);
        // addCommandLine("source " + QString(getenv("COIN_ROOT")) + "/" + "setup.bash &&" + QString::fromStdString(cmd) + " && echo ${COIN_ROOT}\n");
        addCommandLine(QString::fromStdString(cmd));
    });

}

SuperPanelWidget::~SuperPanelWidget()
{
    __m_terminal.shutdown();
}

void SuperPanelWidget::addPinWidget(QWidget* widget)
{
    __m_pinWidget->layout()->addWidget(widget);
}

void SuperPanelWidget::launchProcess_(QLineEdit* w)
{
    if(w->text().isEmpty())
    {
        return;
    }

    addCommand(w->text());

    w->clear();
}

void SuperPanelWidget::updateScrollBarPos()
{
    if(__m_scrollArea->verticalScrollBar()->isVisible())
    {
        __m_scrollArea->verticalScrollBar()->setValue(__m_scrollArea->verticalScrollBar()->maximum() + 100);
    }
}

void SuperPanelWidget::processExit(int exitCode, QProcess::ExitStatus exitStatus)
{
    sender()->deleteLater();
}

SuperPanelWidget &SuperPanelWidget::superpanel()
{
    static SuperPanelWidget *sp = new SuperPanelWidget;
    return *sp;
}

FloatWidget* SuperPanelWidget::addMessageWidget()
{
    auto msg = new FloatWidget(this);
    msg->setFixedHeight(100);
    __m_tableLayout->addWidget(msg);
    return msg;
}

void SuperPanelWidget::addCommand(const QString &cmd, const QStringList &args)
{
    // 合并命令与参数为命令行
    QString cmdLine = cmd + " " + args.join(" ");
    addCommand(cmdLine);
}

void SuperPanelWidget::addCommand(const QString &cmdLine)
{
    __m_commandProcessor.invokeProcessor(cmdLine.toStdString());
}

void SuperPanelWidget::addCommandLine(const QString &cmdLine)
{
    auto makeMessageItem = [this]() -> TerminalWidget*{

        auto term = new TerminalWidget(this);
        term->enableAutoResize();

        // 添加圆角效果到父窗口，避免由于透明效果引起的花屏
        QGraphicsDropShadowEffect *parentShadowEffect = new QGraphicsDropShadowEffect;
        parentShadowEffect->setBlurRadius(10);
        term->setGraphicsEffect(parentShadowEffect);

        __m_tableLayout->addWidget(term);

        connect(term, &TerminalWidget::contentsChanged, [=](){
            QTimer::singleShot(100, [=]() {
                updateScrollBarPos();
            });
        });

        connect(term, &TerminalWidget::sizeChanged, [this]{
            QTimer::singleShot(100, [=]() {
                updateScrollBarPos();
            });
        });

        QTimer::singleShot(100, [=]() {
            updateScrollBarPos();
        });

        return term;
    };
    {
        auto input = makeMessageItem();
        input->setAlignment(Qt::AlignVCenter);
        QTimer::singleShot(0, [=]() {
            // input->insertHtml(textToHtml(cmdLine, "<font color=blue><b>", "</b></font>"));
            input->append(cmdLine.trimmed());
        });

        auto output = makeMessageItem();

        connect(output, &TerminalWidget::dataReceived, [=](){
            std::string text;
            do {
                text = __m_terminal.read_front();
                std::regex re(R"(^\[\bsudo\]\s\bpassword\s\bfor\s.+:\s*$)");
                if(std::regex_match(text, re))
                {
                    // 启动密码对话框
                    bool ok = false;
                    QInputDialog input_dialog(this);
                    input_dialog.setTextEchoMode(QLineEdit::Password);
                    input_dialog.setWindowTitle("Password Required");
                    input_dialog.setLabelText("Please enter Root password:");
                    input_dialog.exec();
                    ok = input_dialog.result();
                    if(ok)
                    {
                        QString word = input_dialog.textValue();
                        qDebug() << "get password:" << word;
                        __m_terminal.send(word.toStdString() + "\n");
                    }
                    else
                    {
                        __m_terminal.send_signal(SIGINT);
                    }
                }

                auto t = QString::fromStdString(text).trimmed();
                if(not t.isEmpty())
                {
                    output->append(t);
                }
            } while (not text.empty());
            // auto html = ansiToHtml(QString::fromStdString(text));
            // output->insertHtml(html);
        });

        __m_terminal.set_receive_notification([=]{
            QCoreApplication::postEvent(output, new TerminalWidget::OnDataReceived());
        });

        __m_terminal.send(cmdLine.toStdString() + "\n");        
    }
}

void SuperPanelWidget::addHttpRequest(const QString &url, const QString &context)
{
}

void SuperPanelWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // 在这里添加绘制代码，例如绘制透明背景或其他内容
    QPainter painter(this);

    // 设置透明度，范围是0.0（完全透明）到1.0（完全不透明）
    // painter.setOpacity(0.0);

    // 绘制透明背景或其他内容
    // painter.fillRect(rect(), Qt::white);  // 使用白色背景，你可以使用其他颜色或渐变

    painter.drawRoundedRect(rect(), 10, 10);

}

} // namespace coin::superpanel
