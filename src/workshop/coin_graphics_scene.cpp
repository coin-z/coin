/**
 * @file coin_graphics_scene.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "coin_graphics_scene.hpp"
#include "coin_node_datamodel.hpp"
#include <QtNodes/internal/NodeGraphicsObject.hpp>

namespace coin::workshop
{
CoinGraphicsScene::CoinGraphicsScene(QtNodes::DataFlowGraphModel &graphModel, QObject *parent)
  : QtNodes::DataFlowGraphicsScene(graphModel, parent)
{
    // connect(this, &CoinGraphicsScene::nodeHoverMove, this, &CoinGraphicsScene::hovered_);
    // connect(this, &CoinGraphicsScene::nodePressed, this, &CoinGraphicsScene::clicked_);
    // connect(this, &CoinGraphicsScene::nodeReleased, this, &CoinGraphicsScene::released_);
}

void CoinGraphicsScene::onNodeObjectCreated(std::unique_ptr< QtNodes::NodeGraphicsObject >& object)
{
    QtNodes::AbstractGraphModel &model = object->graphModel();
    auto data_model = (static_cast<QtNodes::DataFlowGraphModel&>(model)).delegateModel<CoinNodeDataModel>(object->nodeId());
    auto obj_ptr = object.get();
    connect(data_model, &CoinNodeDataModel::work_state_changed, [obj_ptr](const CoinNodeDataModel::NodeWorkState&){
        obj_ptr->update();
    });
}

void CoinGraphicsScene::hovered_(QtNodes::NodeId const nodeId)
{
    auto object = nodeGraphicsObject(nodeId);
    if (object)
    {
    }
}

void CoinGraphicsScene::clicked_(QtNodes::NodeId const nodeId)
{
    auto object = nodeGraphicsObject(nodeId);
    if (object)
    {

    }
}

void CoinGraphicsScene::released_(QtNodes::NodeId const nodeId)
{
    auto object = nodeGraphicsObject(nodeId);
    if (object)
    {

    }   
}
} // namespace coin::workshop
