/**
 * @file cube_viz_item.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "coin-viz/cube_viz_item.hpp"
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
#include "cube_viz_item.hpp"


CubeVizItem::CubeVizItem(const std::shared_ptr<std::vector<Vertex>> &pts, const color_t& c, const CubeType type)
  : pts_(pts)
  , idx_mem_(std::make_shared<std::vector<idx_mem_t>>(std::vector<idx_mem_t>{
        0, 1, 2, 0, 2, 3, //前面
        0, 5, 1, 0, 4, 5, //上面
        0, 3, 7, 0, 7, 4, //右面
        6, 5, 4, 6, 4, 7, //后面
        6, 3, 2, 6, 7, 3, //下面
        6, 2, 1, 6, 1, 5 //左面
    }))
  , color_(c)
  , cube_type_(type)
{

}

CubeVizItem::~CubeVizItem()
{
}

void CubeVizItem::update(filament::Engine *engine, const std::shared_ptr<std::vector<Vertex>> &ver, const float size)
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

void CubeVizItem::add2Scene(filament::Engine *engine, filament::Scene *scene, filament::Material *mat)
{
    initMaterial(engine, nullptr);
    renderable_ = utils::EntityManager::get().create();
    scene->addEntity(renderable_);
    reload(engine);
}

void CubeVizItem::reload(filament::Engine *engine)
{
    pts_->resize(8);
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
    if(cube_type_ == CubeType::LINES)
    {
        t = filament::RenderableManager::PrimitiveType::LINE_STRIP;
    }
    else if(cube_type_ == CubeType::TRIANGLES)
    {
        t = filament::RenderableManager::PrimitiveType::TRIANGLES;
    }
    else
    {
        t = filament::RenderableManager::PrimitiveType::LINES;
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

void CubeVizItem::release(filament::Engine *engine)
{
    std::cout << "release cub viz." << std::endl;
    VizItem::release(engine);
    if(material_instance_)
    {
        engine->destroy(material_instance_);
    }
    // if(material_)
    // {
    //     engine->destroy(material_);
    // }
}
void CubeVizItem::initMaterial(filament::Engine *engine, filament::Material *mat)
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
    material_instance_->setParameter("color", color_);
}
