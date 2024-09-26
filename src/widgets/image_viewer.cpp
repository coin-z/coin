/**
 * @file image_viewer.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "image_viewer.hpp"

#include <QGraphicsPixmapItem>


namespace coin
{

ImageViewer::ImageViewer(QWidget *parent) : QGraphicsView(parent), scaled(false)
{

    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::HighQualityAntialiasing, true);
    setViewportUpdateMode(FullViewportUpdate);

    // 创建场景和图像项
    scene = new QGraphicsScene(this);
    imageItem = new QGraphicsPixmapItem();
    scene->addItem(imageItem);

    // 设置视图的场景
    setScene(scene);

    // 设置视图属性
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 初始化视图
    setInteractive(true);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::SmoothPixmapTransform);
}

ImageViewer::~ImageViewer()
{}

void ImageViewer::openImage(const QString &filePath)
{
    // 加载图像并显示在场景中
    QPixmap image(filePath);
    imageItem->setPixmap(image);
    scene->setSceneRect(imageItem->pixmap().rect());
    scaled = false;
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    // 缩放因子，可以根据需要进行调整
    qreal scaleFactor = 1.2;

    // 鼠标滚轮的滚动方向，正数表示向前滚动，负数表示向后滚动
    int delta = event->angleDelta().y();

    // 根据滚轮方向进行缩放
    if (delta > 0) {
        scale(scaleFactor, scaleFactor);
        scaled = true;
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        scaled = true;
    }

    // 停止事件传播，防止事件继续传递给父类
    event->accept();
}

void ImageViewer::resizeEvent(QResizeEvent *event)
{
    if(not scaled)
    {
        // 更新视口矩形以适应窗口大小
        fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }

    // 停止事件传播，防止事件继续传递给父类
    event->accept();
}

}
