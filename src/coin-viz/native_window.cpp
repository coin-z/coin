/**
 * @file native_window.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "coin-viz/native_window.hpp"
#include <mutex>
#include <QApplication>
#include <QResizeEvent>

#include <QtDebug>

NativeWindow::NativeWindow(QWidget *i_parent) noexcept
    : QWidget(i_parent), m_is_init(false), m_update_pending(false)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
}

QPaintEngine *NativeWindow::paintEngine() const
{
    return nullptr;
}

void NativeWindow::request_draw()
{
    if (m_update_pending == false)
    {
        m_update_pending = true;
        QApplication::postEvent(this, new QEvent{QEvent::UpdateRequest});
    }
}

void NativeWindow::paintEvent(QPaintEvent * /*i_paint_event*/)
{
    // Register a request to draw our window
    request_draw();
}

void NativeWindow::resizeEvent(QResizeEvent *i_resize_event)
{
    QWidget::resizeEvent(i_resize_event);

    // Get our previous and current window sizes
    const auto old_size = i_resize_event->oldSize();
    const auto size = i_resize_event->size();

    // Don't resize to invalid negative dimensions
    if (size.width() < 0 || size.height() < 0)
        return;

    resize_impl(i_resize_event);

    // Qt automatically requests an update if we resize the window to larger
    // dimensions, but if we resize smaller, we need to manually request the
    // update
    if (size.width() < old_size.width() || size.height() < old_size.height())
    {
        request_draw();
    }
}

bool NativeWindow::event(QEvent *i_event)
{
    switch (i_event->type())
    {
    // If an update has been requested, we need to draw
    case QEvent::UpdateRequest:
    {
        // Only draw if the window is visible
        if (isVisible())
            draw_impl();
        // Set this to false now that we've satisfied the request
        m_update_pending = false;
        return true;
    }
    // mouse event
    case QEvent::MouseButtonPress:

        if(static_cast<QMouseEvent*>(i_event)->button() == Qt::MouseButton::LeftButton)
        {
            mouseLBtnClick(static_cast<QMouseEvent*>(i_event));
            return true;
        }

    break;
    case QEvent::MouseButtonRelease:

        if(static_cast<QMouseEvent*>(i_event)->button() == Qt::MouseButton::LeftButton)
        {
            mouseLBtnRelease(static_cast<QMouseEvent*>(i_event));
            return true;
        }

    break;
    case QEvent::MouseMove:
        mouseMove(static_cast<QMouseEvent*>(i_event));
        request_draw();
        return true;
    break;
    case QEvent::Wheel:
        wheel(static_cast<QWheelEvent*>(i_event));
        request_draw();
    // All other events should have default behavior
    default:
        return QWidget::event(i_event);
    }

    return false;
}

void NativeWindow::mouseLBtnClick(QMouseEvent *event)
{
}

void NativeWindow::mouseLBtnRelease(QMouseEvent *event)
{
}

void NativeWindow::mouseMove(QMouseEvent *event)
{
}

void NativeWindow::wheel(QWheelEvent *event)
{
}

void NativeWindow::init()
{
    // Quick exit to avoid locking
    if (m_is_init)
        return;
    // Initialize using the native window id, exactly once in a thread safe manor
    static std::once_flag is_init;
    std::call_once(is_init, [this]
                   {
    m_is_init = true; });
}

//------------------------------------------------------------------------------
//----------------Default implementations do nothing----------------------------
//------------------------------------------------------------------------------
void NativeWindow::draw_impl()
{
    request_draw();
}
void NativeWindow::resize_impl(QResizeEvent *event)
{
}
