#include "viz_widget.hpp"
/**
 * @file viz_widget.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "coin-viz/viz_widget.hpp"
#include <resources.h>

#include <QtDebug>

namespace flm = filament::math;
VizWidget::VizWidget(QWidget *parent) : NativeWindow(parent)
{
    // create engine
    engine_ = filament::Engine::create();
    swap_chain_ = engine_->createSwapChain((void*)winId());
    renderer_ = engine_->createRenderer();

    view_ = engine_->createView();
    scene_ = engine_->createScene();
    view_->setScene(scene_);
    view_->setViewport(filament::Viewport(0, 0, width(), height()));

    // skybox init
    skybox_ = filament::Skybox::Builder().color({0.1, 0.125, 0.25, 1.0}).build(*engine_);
    scene_->setSkybox(skybox_);

    view_->setPostProcessingEnabled(false);

    // default material
    material_ = filament::Material::Builder()
        .package((void*)RESOURCES_BAKEDCOLOR_DATA, RESOURCES_BAKEDCOLOR_SIZE)
        .build(*engine_);

    // camera init
    camera_entity_ = utils::EntityManager::get().create();
    camera_ = engine_->createCamera(camera_entity_);
    camera_->setExposure(16.0f, 1 / 125.0f, 100.0f);
    camera_->lookAt({100, 100, 100}, {0, 0, 0}, {0, 1, 0});
    camera_->setProjection(60, (float) width() / height(), 0.0001, 200);
    view_->setCamera(camera_);

    camera_manipulator_ = CameraManipulator::Builder()
        .orbitHomePosition(2, 2, 2)
        .upVector(0, 1, 0)
        .targetPosition(0, 0, 0)
        .flightMoveDamping(15.0)
        .farPlane(100)
        .build(filament::camutils::Mode::ORBIT);

    camera_manipulator_->setViewport(width(), height());

}

VizWidget::~VizWidget()
{

}

void VizWidget::addItem(const std::string &name, const std::shared_ptr<VizItem>& item)
{
    auto itor = items_.find(name);
    if(itor != items_.end())
    {
        itor->second->release(engine_);
    }
    items_.emplace(name, item);
    item->add2Scene(engine_, scene_, material_);
}

void VizWidget::resize_impl(QResizeEvent *event)
{
    if(!m_is_init) return;
    const auto pixelRatio = devicePixelRatio();
    const uint32_t w = static_cast<uint32_t>(event->size().width() * pixelRatio);
    const uint32_t h = static_cast<uint32_t>(event->size().height() * pixelRatio);
    view_->setViewport({0, 0, w, h});
    updateCamera_();
    NativeWindow::resize_impl(event);
}

void VizWidget::draw_impl()
{
    NativeWindow::draw_impl();
    if(renderer_->beginFrame(swap_chain_))
    {
        renderer_->render(view_);
        renderer_->endFrame();
    }
    updateCamera_();
}

void VizWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    std::cout << "viz widget closed" << std::endl;
    for(auto itor = items_.begin(); itor != items_.end(); itor++)
    {
        itor->second->release(engine_);
    }
    items_.clear();

    engine_->destroy(skybox_);
    engine_->destroy(material_);

    engine_->destroyCameraComponent(camera_entity_);
    utils::EntityManager::get().destroy(camera_entity_);

}

void VizWidget::updateCamera_()
{
    const auto pixelRatio = devicePixelRatio();
    const uint32_t w = static_cast<uint32_t>(width() * pixelRatio);
    const uint32_t h = static_cast<uint32_t>(height() * pixelRatio);

    view_->setViewport({0, 0, w, h});

    constexpr float k_zoom = 1.5f;
    const float aspect = (float)w / (float)h;
    // camera_->setProjection(filament::Camera::Projection::ORTHO,
    //                             -aspect * k_zoom,
    //                             aspect * k_zoom,
    //                             -k_zoom,
    //                             k_zoom,
    //                             0, 100);
    camera_->setProjection(60, aspect, 0.1, 1000);

    flm::float3 eye, target, up;
    camera_manipulator_->getLookAt(&eye, &target, &up);
    camera_->lookAt(eye, target, up);

    update();
    NativeWindow::draw_impl();
}

void VizWidget::mouseLBtnClick(QMouseEvent *event)
{
    camera_manipulator_->grabBegin(event->pos().x(), event->pos().y(), false);
}

void VizWidget::mouseLBtnRelease(QMouseEvent *event)
{
    camera_manipulator_->grabEnd();
}

void VizWidget::mouseMove(QMouseEvent *event)
{
    camera_manipulator_->grabUpdate(event->pos().x(), event->pos().y());
    updateCamera_();
}

void VizWidget::wheel(QWheelEvent *event)
{
    // camera_manipulator_->setScaling(0.1 * event->angleDelta().y(), 0.1 * event->angleDelta().y(), 0.1 * event->angleDelta().y());
    camera_manipulator_->scroll(0, 0, 0.5 * event->angleDelta().y());
    updateCamera_();
}
