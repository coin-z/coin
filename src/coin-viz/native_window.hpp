/**
 * @file native_window.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <QWidget>
#include <memory>

// Qt widget that represents a completely blank native window, for use as a
// rendering target
class NativeWindow : public QWidget
{
    // Qt moc magic
    Q_OBJECT

public:
    // Constructor marked explicit to prevent other widgets from implicitly
    // casting to a native window
    explicit NativeWindow(QWidget *i_parent) noexcept;
    // Explicitly deleted default constructor
    NativeWindow() noexcept = delete;
    // Explicitly defaulted virtual destructor
    virtual ~NativeWindow() noexcept = default;
    // Return a null pointer here to tell Qt we're handling all drawing
    virtual QPaintEngine *paintEngine() const override final;
    // Initialization function, to be called after the widget has already been
    // set-up, from here we can access the native window ID
    virtual void init();

protected:
    // For derived classes to draw to this native window
    virtual void draw_impl();
    // For derived classes to handle a window resize
    virtual void resize_impl(QResizeEvent *event);

private:
    // Call this function to request a redraw of the window
    void request_draw();
    // This event will simply request a draw
    virtual void paintEvent(QPaintEvent *i_paint_event) override final;
    // This event will simply delegate to resize_impl after boilerplate check,
    // and then request a draw
    virtual void resizeEvent(QResizeEvent *i_resize_event) override final;
    // We need to intercept the update request, and call draw_impl when received
    virtual bool event(QEvent *i_event) override final;

    virtual void mouseLBtnClick(QMouseEvent *event);
    virtual void mouseLBtnRelease(QMouseEvent *event);
    virtual void mouseMove(QMouseEvent *event);
    virtual void wheel(QWheelEvent *event);

protected:
    // Has this window been initialized?
    bool m_is_init;

private:
    // Has a draw been requested?
    bool m_update_pending;
};
