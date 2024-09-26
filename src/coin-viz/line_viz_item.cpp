/**
 * @file line_viz_item.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "coin-viz/line_viz_item.hpp"

#include <filament/View.h>
#include <filament/Viewport.h>
#include <filament/Renderer.h>
#include <filament/Fence.h>
#include <filament/Camera.h>
#include <camutils/Manipulator.h>
#include <utils/EntityManager.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/TransformManager.h>

#include <filament/IndexBuffer.h>
#include <filament/Texture.h>
#include <filament/TextureSampler.h>

#include <image/ImageSampler.h>
#include <image/LinearImage.h>

#include <resources.h>


LineVizItem::LineVizItem(const std::shared_ptr<std::vector<Vertex>> &line, const LineType type)
  : line_(line)
{
    if(type == LineType::LINE_STRIP)
    {
        VizItem::primitive_type_ = filament::RenderableManager::PrimitiveType::LINE_STRIP;
    }
    else if(type == LineType::LINES)
    {
        VizItem::primitive_type_ = filament::RenderableManager::PrimitiveType::LINES;
    }
}

LineVizItem::~LineVizItem()
{
}

void LineVizItem::update(filament::Engine *engine, const std::shared_ptr<std::vector<Vertex>> &ver, const float size)
{
    if(ver_buf_)
    {
        engine->destroy(ver_buf_);
    }
    if(idx_buf_)
    {
        engine->destroy(idx_buf_);
    }
    line_ = ver;

    reload(engine);

}

void LineVizItem::add2Scene(filament::Engine *engine, filament::Scene *scene, filament::Material *mat)
{
    material_ = mat;
    renderable_ = utils::EntityManager::get().create();
    scene->addEntity(renderable_);
    reload(engine);
}

void LineVizItem::reload(filament::Engine *engine)
{
    const size_t index_cnt = line_->size();
    ver_buf_ = filament::VertexBuffer::Builder()
        .vertexCount(index_cnt)
        .bufferCount(1)
        .attribute(filament::VertexAttribute::POSITION,
            0, filament::VertexBuffer::AttributeType::FLOAT3, 0, sizeof(Vertex))
        .attribute(filament::VertexAttribute::COLOR,
            0, filament::VertexBuffer::AttributeType::UBYTE4, sizeof(filament::math::float3), sizeof(Vertex))
        .normalized(filament::VertexAttribute::COLOR)
        .build(*engine);

    auto* ver_p = new BufferPtr<std::vector<Vertex>>(line_);
    auto release_ver_ptr = [](void* buffer, size_t size, void* user){
        delete static_cast<BufferPtr<std::vector<Vertex>>*>(user);
    };
    ver_buf_->setBufferAt(*engine, 0,
        filament::VertexBuffer::BufferDescriptor(ver_p->ptr->data(), ver_p->ptr->size() * sizeof(Vertex), release_ver_ptr, ver_p));

    idx_buf_ = filament::IndexBuffer::Builder()
        .indexCount(index_cnt)
        .bufferType(filament::IndexBuffer::IndexType::USHORT)
        .build(*engine);
    
    if(not idx_mem_ || idx_mem_->size() != ver_p->ptr->size())
    {
        idx_mem_ = std::make_shared<std::vector<idx_mem_t>>(ver_p->ptr->size());
        for(size_t i = 0; i < ver_p->ptr->size(); i++)
        { (*idx_mem_)[i] = i; }
    }

    auto idx_p = new BufferPtr<std::vector<idx_mem_t>>(idx_mem_);
    auto release_idx_ptr = [](void* buffer, size_t size, void* user){
        delete static_cast<BufferPtr<std::vector<idx_mem_t>>*>(user);
    };

    idx_buf_->setBuffer(*engine,
        filament::IndexBuffer::BufferDescriptor(idx_mem_->data(), sizeof(idx_mem_t) * ver_p->ptr->size(), release_idx_ptr, idx_p));

    // if(!mat)
    {
        // material = filament::Material::Builder()
        //     .package(engine->package())
        //     .name("line")
        //     .build(*engine);
    }

    filament::RenderableManager::Builder(1)
        .boundingBox({{-1, -1, -1}, {1, 1, 1}})
        .material(0, material_->getDefaultInstance())
        .geometry(0, VizItem::primitive_type_, ver_buf_, idx_buf_, 0, ver_p->ptr->size())
        .culling(false)
        .receiveShadows(false)
        .castShadows(false)
        .build(*engine, renderable_);
}
