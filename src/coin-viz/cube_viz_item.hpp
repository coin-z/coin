/**
 * @file cube_viz_item.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-20
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
#include <map>
#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/Material.h>
#include <utils/Entity.h>
#include <filament/RenderableManager.h>

#include <coin-viz/viz_item.hpp>

class VizWidget;

class CubeVizItem : public VizItem
{
friend VizWidget;
using idx_mem_t = uint16_t;
public:

    struct Vertex
    {
        filament::math::float3 position;
    };

    using color_t = filament::math::float4;

    enum class CubeType
    {
        LINES,
        TRIANGLES
    };
    
    CubeVizItem(const std::shared_ptr<std::vector<Vertex>>& pts, const color_t& c, const CubeType type = CubeType::LINES);
    virtual ~CubeVizItem() override;

private:
    virtual void add2Scene(filament::Engine *engine, filament::Scene *scene, filament::Material *mat) override;
    virtual void reload(filament::Engine *engine) override;
    virtual void release(filament::Engine *engine) override;

private:
    std::shared_ptr<std::vector<Vertex>> pts_;
    std::shared_ptr<std::vector<idx_mem_t>> idx_mem_;
    color_t color_;
    CubeType cube_type_;

private:
    void update(filament::Engine *engine, const std::shared_ptr<std::vector<Vertex>>& ver, const float size = 1.0f);
    void initMaterial(filament::Engine *engine, filament::Material *mat);

};
