/**
 * @file display_widget.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "display_widget.hpp"
#include <QPropertyAnimation>

#include <QMenu>
#include <QAction>
#include <QMimeData>

#include <QtDebug>

namespace coin::node
{
DisplayWidget::DisplayWidget(const coin::node::PackageBaseInfo& info, QWidget *parent) : QLabel(parent)
  , info_(info)
{
    // 设置圆角
    setStyleSheet("border: 3px solid blue; border-radius: 10px; background-color: lightgray;");
    // 设置文本粗体黑色
    setFont(QFont("Microsoft YaHei", 12));
    setAlignment(Qt::AlignCenter);

    setText( QString::fromStdString(info.name()) );

    menu_ = new QMenu(this);
    QAction *detailAction = new QAction("Detail", menu_);

    menu_->addAction(detailAction);

    connect(detailAction, &QAction::triggered, this, &DisplayWidget::detail);

}

DisplayWidget::~DisplayWidget()
{
}

void DisplayWidget::mousePressEvent(QMouseEvent *event)
{
    // 捕获鼠标点击事件，防止事件传递到下层窗口
    if(event->button() == Qt::RightButton)
    {
        // 弹出菜单
        menu_->exec(mapToGlobal(QPoint( this->width(), this->height() )));
        event->accept();
    }
    else if(event->button() == Qt::LeftButton)
    {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setImageData(this->grab());
        QString nodeUrl = QString::fromStdString(info_.url());
        mimeData->setData("path", nodeUrl.toLocal8Bit());
        drag->setMimeData(mimeData);

        drag->setPixmap(this->grab());
        
        drag->setHotSpot(QPoint(drag->pixmap().width() / 2,
                            drag->pixmap().height() / 2));


        Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
    }
}

} // namespace coin::node
