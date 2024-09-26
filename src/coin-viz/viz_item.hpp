/**
 * @file viz_item.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-05
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

#include <coin-viz/viz-filament.hpp>

#include <iostream>


class VizWidget;
class VizItem
{
friend VizWidget;
public:
    struct Vertex {
        filament::math::float3 position;
        uint32_t color;
    };

    VizItem();
    virtual ~VizItem();

protected:
    filament::RenderableManager::PrimitiveType primitive_type_;

    filament::VertexBuffer* ver_buf_;
    filament::IndexBuffer* idx_buf_;

    utils::Entity renderable_;
    filament::Material* material_;
    filament::MaterialInstance* material_instance_;

private:
    virtual void add2Scene(filament::Engine *engine, filament::Scene *scene, filament::Material *mat) = 0;
    virtual void reload(filament::Engine *engine) = 0;
protected:
    virtual void release(filament::Engine *engine);
};

template<typename ItemT>
class BufferPtr
{
    typedef void(*release_t)(void*, size_t, void*);
public:
    explicit BufferPtr(const std::shared_ptr<ItemT>& p) : ptr(p) { }
    ~BufferPtr() = default;

    void release()
    {
        delete this;
    }

    std::shared_ptr<ItemT> ptr;
};
