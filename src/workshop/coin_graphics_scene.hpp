/**
 * @file coin_graphics_scene.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <QtNodes/DataFlowGraphicsScene>


namespace coin::workshop
{
class CoinGraphicsScene : public QtNodes::DataFlowGraphicsScene
{
    Q_OBJECT
public:
    CoinGraphicsScene(QtNodes::DataFlowGraphModel &graphModel, QObject *parent = nullptr);
    virtual ~CoinGraphicsScene() = default;

private:
    void hovered_(QtNodes::NodeId const nodeId);
    void clicked_(QtNodes::NodeId const nodeId);
    void released_(QtNodes::NodeId const nodeId);
    
    virtual void onNodeObjectCreated(std::unique_ptr< QtNodes::NodeGraphicsObject >& object) override;
};
} // namespace coin::workshop
