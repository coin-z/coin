/**
 * @file image_viewer.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <QWidget>
#include <QWheelEvent>
#include <QGraphicsView>


namespace coin
{


class ImageViewer : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();

    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


public slots:
    void openImage(const QString &filePath);

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *imageItem;
    bool scaled;

};
}
