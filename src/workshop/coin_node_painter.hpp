/**
 * @file node_painter.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <QtGui/QPainter>
#include <QGraphicsObject>

#include <QtNodes/AbstractNodePainter>
#include <QtNodes/Definitions>

namespace QtNodes {
class BasicGraphicsScene;
class GraphModel;
class NodeGeometry;
class NodeGraphicsObject;
class NodeState;
} // namespace QtNodes

namespace coin::workshop {
/// @ Lightweight class incapsulating paint code.
class NODE_EDITOR_PUBLIC CoinNodePainter : public QtNodes::AbstractNodePainter
{
public:
    using paint_callback_t = std::function<void(QPainter *painter, QtNodes::NodeGraphicsObject&)>;

    void paint(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const override;

    void drawNodeRect(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const;

    void drawConnectionPoints(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const;

    void drawFilledConnectionPoints(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const;

    void drawNodeCaption(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const;

    void drawEntryLabels(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const;

    void drawResizeRect(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const;

    void draw_title_bar_(QPainter* painter, QtNodes::NodeGraphicsObject& ngo) const;
};
} // namespace coin::workshop
