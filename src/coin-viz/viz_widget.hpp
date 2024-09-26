/**
 * @file viz_widget.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "coin-viz/native_window.hpp"
#include "coin-viz/viz_item.hpp"

#include <coin-viz/viz-filament.hpp>

#include <QWidget>
#include <QWheelEvent>

#include <map>


class VizWidget : public NativeWindow
{
    Q_OBJECT
    using CameraManipulator = filament::camutils::Manipulator<float>;
public:
    VizWidget(QWidget *parent = nullptr);
    ~VizWidget();

    void addItem(const std::string &name, const std::shared_ptr<VizItem>& item);

    template<typename T, typename... Args>
    void updateItem(const std::string& name, Args&&... args);

private:
    filament::Engine    *engine_;
    filament::Skybox    *skybox_;
    filament::SwapChain *swap_chain_;
    filament::Renderer  *renderer_;
    filament::Material  *material_;
    filament::View      *view_;
    filament::Scene     *scene_;
    filament::Camera    *camera_;
    CameraManipulator   *camera_manipulator_ = nullptr;
    utils::Entity        camera_entity_;

private:
    std::map<std::string, std::shared_ptr<VizItem>> items_;


private:
    void updateCamera_();

    virtual void resize_impl(QResizeEvent *event) override;
    virtual void draw_impl() override;
    virtual void closeEvent(QCloseEvent *event) override;

    virtual void mouseLBtnClick(QMouseEvent *event) override;
    virtual void mouseLBtnRelease(QMouseEvent *event) override;
    virtual void mouseMove(QMouseEvent *event) override;
    virtual void wheel(QWheelEvent *event) override;
};

template <typename T, typename... Args>
inline void VizWidget::updateItem(const std::string &name, Args &&...args)
{
    auto itor = items_.find(name);
    if(itor != items_.end())
    {
        std::static_pointer_cast<T>(itor->second)->update(engine_, std::forward<Args>(args)...);
        updateCamera_();
    }
}
