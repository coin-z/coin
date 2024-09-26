/**
 * @file node_painter.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "coin_node_painter.hpp"

#include <cmath>

#include <QtCore/QMargins>

#include <QtNodes/AbstractGraphModel>
// #include <QtNodes/AbstractNodeGeometry>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/internal/ConnectionGraphicsObject.hpp>
#include <QtNodes/ConnectionIdUtils>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QtNodes/NodeState>
#include <QtNodes/StyleCollection>

#include "coin_node_datamodel.hpp"
#include "coin_embedded_widget.hpp"

namespace coin::workshop {

void CoinNodePainter::paint(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const
{
    // TODO?
    //AbstractNodeGeometry & geometry = ngo.nodeScene()->nodeGeometry();
    //geometry.recomputeSizeIfFontChanged(painter->font());

    drawNodeRect(painter, ngo);

    drawConnectionPoints(painter, ngo);

    drawFilledConnectionPoints(painter, ngo);

    drawNodeCaption(painter, ngo);

    drawEntryLabels(painter, ngo);

    drawResizeRect(painter, ngo);

    draw_title_bar_(painter, ngo);
}

void CoinNodePainter::drawNodeRect(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const
{
    QtNodes::AbstractGraphModel &model = ngo.graphModel();

    QtNodes::NodeId const nodeId = ngo.nodeId();

    QtNodes::AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QSize size = geometry.size(nodeId);

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, QtNodes::NodeRole::Style));

    QtNodes::NodeStyle nodeStyle(json.object());

    auto color = ngo.isSelected() ? nodeStyle.SelectedBoundaryColor : nodeStyle.NormalBoundaryColor;

    if (ngo.nodeState().hovered()) {
        QPen p(color, nodeStyle.HoveredPenWidth);
        painter->setPen(p);
    } else {
        QPen p(color, nodeStyle.PenWidth);
        painter->setPen(p);
    }

    QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(2.0, size.height()));

    gradient.setColorAt(0.0, nodeStyle.GradientColor0);
    gradient.setColorAt(0.10, nodeStyle.GradientColor1);
    gradient.setColorAt(0.90, nodeStyle.GradientColor2);
    gradient.setColorAt(1.0, nodeStyle.GradientColor3);

    painter->setBrush(gradient);

    QRectF boundary(0, 0, size.width(), size.height());

    double const radius = 3.0;

    painter->drawRoundedRect(boundary, radius, radius);
}

void CoinNodePainter::drawConnectionPoints(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const
{
    QtNodes::AbstractGraphModel &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();
    QtNodes::AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, QtNodes::NodeRole::Style));
    QtNodes::NodeStyle nodeStyle(json.object());

    auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

    float diameter = nodeStyle.ConnectionPointDiameter;
    auto reducedDiameter = diameter * 0.6;

    for (QtNodes::PortType portType : {QtNodes::PortType::Out, QtNodes::PortType::In}) {
        size_t const n = model
                             .nodeData(nodeId,
                                       (portType == QtNodes::PortType::Out) ? QtNodes::NodeRole::OutPortCount
                                                                   : QtNodes::NodeRole::InPortCount)
                             .toUInt();

        for (QtNodes::PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            QPointF p = geometry.portPosition(nodeId, portType, portIndex);

            auto const &dataType = model.portData(nodeId, portType, portIndex, QtNodes::PortRole::DataType)
                                       .value<QtNodes::NodeDataType>();

            double r = 1.0;

            QtNodes::NodeState const &state = ngo.nodeState();

            if (auto const *cgo = state.connectionForReaction()) {
                QtNodes::PortType requiredPort = cgo->connectionState().requiredPort();

                if (requiredPort == portType) {
                    QtNodes::ConnectionId possibleConnectionId = makeCompleteConnectionId(cgo->connectionId(),
                                                                                 nodeId,
                                                                                 portIndex);

                    bool const possible = model.connectionPossible(possibleConnectionId);

                    auto cp = cgo->sceneTransform().map(cgo->endPoint(requiredPort));
                    cp = ngo.sceneTransform().inverted().map(cp);

                    auto diff = cp - p;
                    double dist = std::sqrt(QPointF::dotProduct(diff, diff));

                    if (possible) {
                        double const thres = 40.0;
                        r = (dist < thres) ? (2.0 - dist / thres) : 1.0;
                    } else {
                        double const thres = 80.0;
                        r = (dist < thres) ? (dist / thres) : 1.0;
                    }
                }
            }

            if (connectionStyle.useDataDefinedColors()) {
                painter->setBrush(connectionStyle.normalColor(dataType.id));
            } else {
                painter->setBrush(nodeStyle.ConnectionPointColor);
            }

            painter->drawEllipse(p, reducedDiameter * r, reducedDiameter * r);
        }
    }

    if (ngo.nodeState().connectionForReaction()) {
        ngo.nodeState().resetConnectionForReaction();
    }
}

void CoinNodePainter::drawFilledConnectionPoints(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const
{
    QtNodes::AbstractGraphModel &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();
    QtNodes::AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, QtNodes::NodeRole::Style));
    QtNodes::NodeStyle nodeStyle(json.object());

    auto diameter = nodeStyle.ConnectionPointDiameter;

    for (QtNodes::PortType portType : {QtNodes::PortType::Out, QtNodes::PortType::In}) {
        size_t const n = model
                             .nodeData(nodeId,
                                       (portType == QtNodes::PortType::Out) ? QtNodes::NodeRole::OutPortCount
                                                                   : QtNodes::NodeRole::InPortCount)
                             .toUInt();

        for (QtNodes::PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            QPointF p = geometry.portPosition(nodeId, portType, portIndex);

            auto const &connected = model.connections(nodeId, portType, portIndex);

            if (!connected.empty()) {
                auto const &dataType = model
                                           .portData(nodeId, portType, portIndex, QtNodes::PortRole::DataType)
                                           .value<QtNodes::NodeDataType>();

                auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();
                if (connectionStyle.useDataDefinedColors()) {
                    QColor const c = connectionStyle.normalColor(dataType.id);
                    painter->setPen(c);
                    painter->setBrush(c);
                } else {
                    painter->setPen(nodeStyle.FilledConnectionPointColor);
                    painter->setBrush(nodeStyle.FilledConnectionPointColor);
                }

                painter->drawEllipse(p, diameter * 0.4, diameter * 0.4);
            }
        }
    }
}

void CoinNodePainter::drawNodeCaption(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const
{
    QtNodes::AbstractGraphModel &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();
    QtNodes::AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    if (!model.nodeData(nodeId, QtNodes::NodeRole::CaptionVisible).toBool())
        return;

    QString const name = model.nodeData(nodeId, QtNodes::NodeRole::Caption).toString();

    QFont f = painter->font();
    f.setBold(true);

    QPointF position = geometry.captionPosition(nodeId);

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, QtNodes::NodeRole::Style));
    QtNodes::NodeStyle nodeStyle(json.object());

    painter->setFont(f);
    painter->setPen(nodeStyle.FontColor);
    painter->drawText(position, name);

    f.setBold(false);
    painter->setFont(f);
}

void CoinNodePainter::drawEntryLabels(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const
{
    QtNodes::AbstractGraphModel &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();
    QtNodes::AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, QtNodes::NodeRole::Style));
    QtNodes::NodeStyle nodeStyle(json.object());

    for (QtNodes::PortType portType : {QtNodes::PortType::Out, QtNodes::PortType::In}) {
        unsigned int n = model.nodeData<unsigned int>(nodeId,
                                                      (portType == QtNodes::PortType::Out)
                                                          ? QtNodes::NodeRole::OutPortCount
                                                          : QtNodes::NodeRole::InPortCount);

        for (QtNodes::PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            auto const &connected = model.connections(nodeId, portType, portIndex);

            QPointF p = geometry.portTextPosition(nodeId, portType, portIndex);

            if (connected.empty())
                painter->setPen(nodeStyle.FontColorFaded);
            else
                painter->setPen(nodeStyle.FontColor);

            QString s;

            if (model.portData<bool>(nodeId, portType, portIndex, QtNodes::PortRole::CaptionVisible)) {
                s = model.portData<QString>(nodeId, portType, portIndex, QtNodes::PortRole::Caption);
            } else {
                auto portData = model.portData(nodeId, portType, portIndex, QtNodes::PortRole::DataType);

                s = portData.value<QtNodes::NodeDataType>().name;
            }

            painter->drawText(p, s);
        }
    }
}

void CoinNodePainter::drawResizeRect(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const
{
    QtNodes::AbstractGraphModel &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();
    QtNodes::AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    if (model.nodeFlags(nodeId) & QtNodes::NodeFlag::Resizable) {
        painter->setBrush(Qt::gray);

        painter->drawEllipse(geometry.resizeHandleRect(nodeId));
    }
}
void CoinNodePainter::draw_title_bar_(QPainter* painter, QtNodes::NodeGraphicsObject& ngo) const
{
    QtNodes::AbstractGraphModel &model = ngo.graphModel();

    QtNodes::NodeId const nodeId = ngo.nodeId();

    QtNodes::AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QSize size = geometry.size(nodeId);

    auto work_state = (static_cast<QtNodes::DataFlowGraphModel&>(model)).delegateModel<CoinNodeDataModel>(nodeId)->work_state();


    float const edge_spacing = 5;
    QRectF boundary(edge_spacing, edge_spacing, size.width() - (edge_spacing * 2.0f), 20);

    // const auto title_height = ngo.nodeScene()->nodeGeometry().title_height();
    const auto title_height = geometry.title_bar_height();
    painter->drawLine(0, title_height, size.width(), title_height);

    {
        QPen p(Qt::white, 3.0f);
        painter->setPen(p);
    
        QBrush b(Qt::gray, Qt::SolidPattern);
        if(work_state == CoinNodeDataModel::NodeWorkState::Crash)
        {
            b.setColor(Qt::red);
        }
        else if(work_state == CoinNodeDataModel::NodeWorkState::Working)
        {
            b.setColor(Qt::green);
        }
        painter->setBrush(b);
    }

    const QPointF status_pos = {
        boundary.x() + 10.0f,
        boundary.y() + (boundary.height() * 0.5)
    };
    painter->drawEllipse(status_pos, 6.0f, 6.0f);

    {
        QPen p(Qt::white, 5.0f);
        painter->setPen(p);
    }
    painter->drawText(status_pos.x() + 20, title_height * 0.65, model.nodeData<NodeEmbeddedWidget*>(nodeId, QtNodes::NodeRole::Widget)->name());
}

} // namespace QtNodes
