/**
 * @file terminal_widget.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "widgets/terminal_widget.hpp"
#include <iostream>

#include <QGraphicsDropShadowEffect>
#include <QTextDocument>
#include <QScrollBar>
#include <QTimer>
#include "terminal_widget.hpp"

#include <QtDebug>


namespace coin
{

TerminalWidget::TerminalWidget(QWidget *parent) : QTextEdit(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setAlignment(Qt::AlignCenter);
    this->setStyleSheet("background-color: white; border: 2px solid #4CAF50; border-radius: 8px; padding: 10px;");
    this->setReadOnly(true);
    // this->insertHtml("<html><body style='word-wrap: break-word;'>"
    //                 "</body></html>");

    // 设置文本左对齐
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(blockFormat);
    setTextCursor(cursor);

    // connect(this->document(), &QTextDocument::contentsChanged, [=](){
    //     if(enable_auto_resize_)
    //     {
    //         this->document()->setTextWidth(this->viewport()->width());
    //         auto docHeight = this->document()->size().height() + 25;

    //         // 设置超出一定高度后显示滚动条
    //         const int maxHeight = 500;
    //         if (docHeight > maxHeight) {
    //             this->setFixedHeight(maxHeight);
    //             // this->setFixedHeight(maxHeight + 10);
    //         } else {
    //             this->setFixedHeight(docHeight);
    //             // this->setFixedHeight(docHeight + 10);
    //         }            
    //     }
    //     QScrollBar *vScrollBar = this->verticalScrollBar();
    //     vScrollBar->setValue(vScrollBar->maximum());

    //     emit contentsChanged();
    // });
}

QString TerminalWidget::ansiToHtml(const QString &ansiString)
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

void TerminalWidget::enableAutoResize()
{
    enable_auto_resize_ = true;
}

bool TerminalWidget::event(QEvent *event)
{
    if(event->type() == OnDataReceived::EventType)
    {
        emit dataReceived();
        return true;
    }
    // else if(event->type() == QEvent::Resize)
    // {
    //     if(enable_auto_resize_)
    //     {
    //         this->document()->setTextWidth(this->viewport()->width());
    //         auto docHeight = this->document()->size().height() + 25;

    //         // 设置超出一定高度后显示滚动条
    //         const int maxHeight = 500;
    //         if (docHeight > maxHeight) {
    //             this->setFixedHeight(maxHeight);
    //             // this->setFixedHeight(maxHeight + 10);
    //         } else {
    //             this->setFixedHeight(docHeight);
    //             // this->setFixedHeight(docHeight + 10);
    //         }            
    //     }
    //     QScrollBar *vScrollBar = this->verticalScrollBar();
    //     vScrollBar->setValue(vScrollBar->maximum());

    //     emit contentsChanged();
    // }
    return QObject::event(event);
}

TerminalWidget::OnDataReceived::OnDataReceived() : QEvent(EventType) {}

}