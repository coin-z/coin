/**
 * @file pointcloud_viz_item.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <memory>
#include <vector>
#include <deque>
#include <map>
#include <mutex>
#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/Material.h>
#include <utils/Entity.h>

#include <coin-viz/viz_item.hpp>

class VizWidget;

class PointCloudVizItem : public VizItem
{
friend VizWidget;
using idx_mem_t = uint32_t;
public:

    using Vertex = VizItem::Vertex;

    PointCloudVizItem();
    PointCloudVizItem(const std::shared_ptr<std::vector<Vertex>>& points, const float size = 1.0f);
    
    PointCloudVizItem(const size_t num_points, const float size = 1.0f);

    virtual ~PointCloudVizItem() override;


private:
    virtual void add2Scene(filament::Engine *engine, filament::Scene *scene, filament::Material *mat) override;
    virtual void reload(filament::Engine *engine) override;

    void initMaterial(filament::Engine *engine, filament::Material* mat = nullptr);

    std::shared_ptr<std::vector<Vertex>> point_vertices_;
    std::shared_ptr<std::vector<idx_mem_t>> idx_mem_;

    float point_size_ = 1.0;

private:
    void update(filament::Engine *engine, const std::shared_ptr<std::vector<Vertex>>& points, const float size = 1.0f);

    void releasePtr(void* buffer, size_t size, void* user);

};
