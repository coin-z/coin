/**
 * @file rect_viz_item.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "coin-viz/rect_viz_item.hpp"
#include <filament/FilamentAPI.h>
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
#include "rect_viz_item.hpp"


RectVizItem::RectVizItem(const std::shared_ptr<std::vector<Vertex>> &pts, const RectType type)
  : pts_(pts)
  , idx_mem_(std::make_shared<std::vector<idx_mem_t>>(std::vector<idx_mem_t>{0, 1, 2, 2, 3, 0}))
  , rect_type_(type)
{

}

RectVizItem::~RectVizItem()
{
}

void RectVizItem::update(filament::Engine *engine, const std::shared_ptr<std::vector<Vertex>> &ver, const float size)
{
    if(ver_buf_)
    {
        engine->destroy(ver_buf_);
    }
    if(idx_buf_)
    {
        engine->destroy(idx_buf_);
    }
    pts_ = ver;

    reload(engine);
}

void RectVizItem::add2Scene(filament::Engine *engine, filament::Scene *scene, filament::Material *mat)
{
    initMaterial(engine, nullptr);
    renderable_ = utils::EntityManager::get().create();
    scene->addEntity(renderable_);
    reload(engine);
}

void RectVizItem::reload(filament::Engine *engine)
{
    pts_->resize(4);
    ver_buf_ = filament::VertexBuffer::Builder()
        .vertexCount(pts_->size())
        .bufferCount(1)
        .attribute(filament::VertexAttribute::POSITION,
            0, filament::VertexBuffer::AttributeType::FLOAT3, 0, sizeof(Vertex))
        .build(*engine);
    ver_buf_->setBufferAt(*engine, 0,
        filament::VertexBuffer::BufferDescriptor(pts_->data(), pts_->size() * sizeof(Vertex)));

    idx_buf_ = filament::IndexBuffer::Builder()
        .indexCount(idx_mem_->size())
        .bufferType(filament::IndexBuffer::IndexType::USHORT)
        .build(*engine);

    idx_buf_->setBuffer(*engine,
        filament::IndexBuffer::BufferDescriptor(idx_mem_->data(), sizeof(idx_mem_t) * idx_mem_->size(), nullptr));

    filament::RenderableManager::PrimitiveType t = filament::RenderableManager::PrimitiveType::LINES;
    if(rect_type_ == RectType::LINE_STRIP)
    {
        t = filament::RenderableManager::PrimitiveType::LINE_STRIP;
    }
    else if(rect_type_ == RectType::TRIANGLE_STRIP)
    {
        t = filament::RenderableManager::PrimitiveType::TRIANGLE_STRIP;
    }
    else
    {
        t = filament::RenderableManager::PrimitiveType::LINE_STRIP;
    }

    filament::RenderableManager::Builder(1)
        .boundingBox({{-1, -1, -1}, {1, 1, 1}})
        .material(0, material_instance_)
        .geometry(0, t, ver_buf_, idx_buf_, 0, idx_mem_->size())
        .culling(false)
        .receiveShadows(false)
        .castShadows(false)
        .build(*engine, renderable_);
}


void RectVizItem::initMaterial(filament::Engine *engine, filament::Material *mat)
{
    material_ = mat;
    
    // 创建材质
    if(not mat)
    {
        material_ = filament::Material::Builder()
            .package((void*)RESOURCES_TRANSPARENTCOLOR_DATA, RESOURCES_TRANSPARENTCOLOR_SIZE)
            .build(*engine);
    }

    material_instance_ = material_->createInstance();
    material_instance_->setParameter("color", filament::math::float4(0.0f, 0.5f, 0.3f, 0.5f));
}
