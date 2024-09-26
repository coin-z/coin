#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QLabel>
#include <QTextEdit>

namespace coin
{

class FloatWidget : public QWidget {
    Q_OBJECT
public:
    FloatWidget(QWidget* parent = nullptr);
    virtual ~FloatWidget() = default;

    void setWidget(QWidget* widget);
signals:
    void sizeChanged();
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    // void mouseEvent(QMouseEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void moveEvent(QMoveEvent*) override;
    virtual void resizeEvent(QResizeEvent*) override;
    virtual void paintEvent(QPaintEvent *event) override;
private:
    enum class MouseOpration {
        NoOpt,
        Move,
        ResizeBottomRight,
        ResizeHor,
        ResizeVer
    };

    QPoint m_dragPosition;
    MouseOpration __m_mouseOpt = MouseOpration::NoOpt;
};
    
} // namespace coin

