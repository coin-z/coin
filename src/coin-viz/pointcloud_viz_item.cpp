/**
 * @file pointcloud_viz_item.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "coin-viz/pointcloud_viz_item.hpp"

#include <filament/View.h>
#include <filament/Viewport.h>
#include <filament/Renderer.h>
#include <filament/RenderableManager.h>
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

#include <functional>
#include "pointcloud_viz_item.hpp"


PointCloudVizItem::PointCloudVizItem()
{
}

PointCloudVizItem::PointCloudVizItem(const std::shared_ptr<std::vector<Vertex>> &points, const float size)
  : point_vertices_(points), idx_mem_(nullptr), point_size_(size)
{
}

PointCloudVizItem::PointCloudVizItem(const size_t num_points, const float size)
  : point_vertices_(nullptr), idx_mem_(nullptr), point_size_(size)
{
    point_vertices_ = std::make_shared<std::vector<Vertex>>(num_points);
    for(size_t i = 0; i < point_vertices_->size(); i++)
    {
        (*point_vertices_)[i] = {{0, 0, 0}, 0};
    }
}

PointCloudVizItem::~PointCloudVizItem()
{
}

void PointCloudVizItem::update(filament::Engine *engine, const std::shared_ptr<std::vector<Vertex>>& points, const float size)
{
    if(ver_buf_)
    {
        engine->destroy(ver_buf_);
        ver_buf_ = nullptr;
    }
    if(idx_buf_)
    {
        engine->destroy(idx_buf_);
        idx_buf_ = nullptr;
    }

    {
        // buffer 大小发生变化，重新创建
        point_vertices_ = points;
        point_size_ = size;
    }

    reload(engine);
}

void PointCloudVizItem::releasePtr(void *buffer, size_t size, void *user)
{
}
void PointCloudVizItem::add2Scene(filament::Engine *engine, filament::Scene *scene, filament::Material *mat)
{
    initMaterial(engine, mat);
    renderable_ = utils::EntityManager::get().create();
    scene->addEntity(renderable_);
    reload(engine);
}

void PointCloudVizItem::reload(filament::Engine *engine)
{
    if(not point_vertices_)
    {
        return;
    }
    ver_buf_ = filament::VertexBuffer::Builder()
        .vertexCount(point_vertices_->size())
        .bufferCount(1)
        .attribute(filament::VertexAttribute::POSITION,
            0, filament::VertexBuffer::AttributeType::FLOAT3, 0, sizeof(Vertex))
        .attribute(filament::VertexAttribute::COLOR,
            0, filament::VertexBuffer::AttributeType::UBYTE4, sizeof(filament::math::float3), sizeof(Vertex))
        .normalized(filament::VertexAttribute::COLOR)
        .build(*engine);

    auto* ver_p = new BufferPtr<std::vector<Vertex>>(point_vertices_);
    auto release_ver_ptr = [](void* buffer, size_t size, void* user){
        delete static_cast<BufferPtr<std::vector<Vertex>>*>(user);
    };

    ver_buf_->setBufferAt(*engine, 0,
        filament::VertexBuffer::BufferDescriptor(ver_p->ptr->data(), ver_p->ptr->size() * sizeof(Vertex), release_ver_ptr, ver_p));

    if(not idx_mem_)
    {
        idx_mem_ = std::make_shared<std::vector<idx_mem_t>>();
    }
    if(idx_mem_->size() < point_vertices_->size())
    {
        idx_mem_->resize(point_vertices_->size());
        for(size_t i = 0; i < point_vertices_->size(); i++)
        { (*idx_mem_)[i] = i; }
    }

    auto* idx_p = new BufferPtr<std::vector<idx_mem_t>>(idx_mem_);
    auto release_idx_ptr = [](void* buffer, size_t size, void* user){
        delete static_cast<BufferPtr<std::vector<idx_mem_t>>*>(user);
    };
    idx_buf_ = filament::IndexBuffer::Builder()
        .indexCount(point_vertices_->size())
        .bufferType(filament::IndexBuffer::IndexType::UINT)
        .build(*engine);
    idx_buf_->setBuffer(*engine,
        filament::IndexBuffer::BufferDescriptor(idx_p->ptr->data(), sizeof(idx_mem_t) * point_vertices_->size(), release_idx_ptr, idx_p));
    
    material_instance_->setParameter("pointSize", point_size_);
    
    filament::RenderableManager::Builder(1)
        .boundingBox({{-100, -100, -100}, {100, 100, 100}})
        .material(0, material_instance_)
        .geometry(0, VizItem::primitive_type_, ver_buf_, idx_buf_, 0, point_vertices_->size())
        .culling(false)
        .receiveShadows(false)
        .castShadows(false)
        .build(*engine, renderable_);
}

void PointCloudVizItem::initMaterial(filament::Engine *engine, filament::Material *mat)
{
    filament::Material *material = mat;
    
    // if(not mat)
    {
        material = filament::Material::Builder()
            .package((void*)RESOURCES_POINTCLOUD_DATA, RESOURCES_POINTCLOUD_SIZE)
            .build(*engine);
    }

    material_instance_ = material->createInstance();
    material_instance_->setParameter("pointSize", point_size_);

}
