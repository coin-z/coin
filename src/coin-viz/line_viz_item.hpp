/**
 * @file line_viz_item.hpp
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
#include <map>
#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/Material.h>
#include <utils/Entity.h>
#include <filament/RenderableManager.h>

#include <coin-viz/viz_item.hpp>

class VizWidget;

class LineVizItem : public VizItem
{
friend VizWidget;
using idx_mem_t = uint16_t;
public:

    struct Vertex
    {
        filament::math::float3 position;
        uint32_t color;
    };

    enum class LineType
    {
        LINES,
        LINE_STRIP
    };
    

    LineVizItem(const std::shared_ptr<std::vector<Vertex>>& line, const LineType type = LineType::LINES);
    virtual ~LineVizItem() override;

private:
    virtual void add2Scene(filament::Engine *engine, filament::Scene *scene, filament::Material *mat) override;
    virtual void reload(filament::Engine *engine) override;


private:
    std::shared_ptr<std::vector<Vertex>> line_;
    filament::VertexBuffer* ver_buf_;
    filament::IndexBuffer* idx_buf_;
    std::shared_ptr<std::vector<idx_mem_t>> idx_mem_;

    filament::RenderableManager::PrimitiveType line_type_;

private:
    void update(filament::Engine *engine, const std::shared_ptr<std::vector<Vertex>>& ver, const float size = 1.0f);
};

