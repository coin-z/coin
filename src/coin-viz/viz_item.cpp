/**
 * @file viz_item.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "coin-viz/viz_item.hpp"
#include <filament/Engine.h>
#include <filament/View.h>
#include <filament/Viewport.h>
#include <filament/Scene.h>
#include <filament/Renderer.h>
#include <filament/RenderableManager.h>
#include <filament/Fence.h>
#include <filament/Camera.h>
#include <camutils/Manipulator.h>
#include <utils/EntityManager.h>
#include <utils/Entity.h>
#include <filament/Material.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/TransformManager.h>

#include <filament/IndexBuffer.h>
#include <filament/Texture.h>
#include <filament/TextureSampler.h>

#include <image/ImageSampler.h>
#include <image/LinearImage.h>

#include <resources.h>

#include <iostream>
#include "viz_item.hpp"


VizItem::VizItem()
  : primitive_type_(filament::RenderableManager::PrimitiveType::POINTS)
  , ver_buf_(nullptr)
  , idx_buf_(nullptr)
  , material_(nullptr)
  , material_instance_(nullptr)
{
}

VizItem::~VizItem()
{

}
void VizItem::release(filament::Engine *engine)
{
    std::cout << "release base viz." << std::endl;
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
    if(renderable_)
    {
        engine->destroy(renderable_);
    }
    if(material_)
    {
        engine->destroy(material_);
        material_ = nullptr;
    }
}
