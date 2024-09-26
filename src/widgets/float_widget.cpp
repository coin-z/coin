#include "float_widget.hpp"
#include <QPainter>
#include <QtDebug>

#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>


namespace coin
{

FloatWidget::FloatWidget(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);

    setMinimumSize(300, 300);

    layout->addWidget(new QWidget(this));

    // // 添加圆角效果到父窗口
    // QGraphicsDropShadowEffect *parentShadowEffect = new QGraphicsDropShadowEffect;
    // parentShadowEffect->setBlurRadius(10);
    // setGraphicsEffect(parentShadowEffect);

    setMouseTracking(true);
}

void FloatWidget::setWidget(QWidget *widget)
{
    this->layout()->addWidget(widget);
    // 居中显示子窗口并自适应大小
    this->layout()->setAlignment(Qt::AlignCenter);
    this->layout()->setContentsMargins(10, 10, 10, 10);
}

void FloatWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        // 判断鼠标是否进入窗口右下角
        const int edge = 5;
        auto cpos = event->pos();
        const auto leftBottom = QPoint(this->size().width() - edge, this->size().height() - edge);
        if(cpos.x() > leftBottom.x() && cpos.y() > leftBottom.y() )
        {
            setCursor(Qt::SizeFDiagCursor);
            __m_mouseOpt = MouseOpration::ResizeBottomRight;
        }
        else if(cpos.x() > leftBottom.x())
        {
            setCursor(Qt::SizeHorCursor);
            __m_mouseOpt = MouseOpration::ResizeHor;
        }
        else if(cpos.y() > leftBottom.y())
        {
            setCursor(Qt::SizeVerCursor);
            __m_mouseOpt = MouseOpration::ResizeVer;
        }
        else
        {
            setCursor(Qt::ArrowCursor);
            __m_mouseOpt = MouseOpration::Move;
        }

        event->accept();
    }
}

void FloatWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {

        auto cpos = event->pos();

        if(__m_mouseOpt == MouseOpration::Move)
        {
            move(event->globalPos() - m_dragPosition);
        }
        else if(__m_mouseOpt == MouseOpration::ResizeBottomRight)
        {
            // if(event->pos().x() > this->minimumWidth() && event->pos().y() > this->minimumHeight())
            {
                this->setFixedWidth(event->pos().x());
                this->setFixedHeight(event->pos().y());
            }
        }
        else if(__m_mouseOpt == MouseOpration::ResizeHor)
        {
            // if(event->pos().x() > this->minimumWidth())
            {
                this->setFixedWidth(event->pos().x());
            }
        }
        else if(__m_mouseOpt == MouseOpration::ResizeVer)
        {
            // if(event->pos().y() > this->minimumHeight())
            {
                this->setFixedHeight(event->pos().y());
            }
        }

        event->accept();
    }
    else if(event->buttons() == Qt::NoButton) {
        // 判断鼠标是否进入窗口右下角
        const int edge = 5;
        auto cpos = event->pos();
        const auto leftBottom = QPoint(this->size().width() - edge, this->size().height() - edge);
        if(cpos.x() > leftBottom.x() && cpos.y() > leftBottom.y() )
        {
            setCursor(Qt::SizeFDiagCursor);
        }
        else if(cpos.x() > leftBottom.x() && cpos.y() < edge )
        {
            setCursor(Qt::SizeBDiagCursor);
        }
        else if(cpos.x() > leftBottom.x() || cpos.x() < edge)
        {
            setCursor(Qt::SizeHorCursor);
        }
        else if(cpos.y() > leftBottom.y() || cpos.y() < edge)
        {
            setCursor(Qt::SizeVerCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void FloatWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
}

void FloatWidget::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制圆角矩形
    const float width = 5.0;
    painter.setPen(QPen(Qt::gray, width)); // 设置边框颜色，宽度
    painter.setBrush(QBrush(QColor("#2A2A2A")));
    QPainterPath path;
    QRectF r(rect().left() + width * 0.5, rect().top() + width * 0.5, rect().width() - (width), rect().height() - (width));
    path.addRoundedRect(r, 10.0, 10.0); // 设置圆角半径
    painter.drawPath(path);

    painter.setRenderHint(QPainter::Antialiasing, false);

    // update();

}

void FloatWidget::moveEvent(QMoveEvent *)
{ }

void FloatWidget::resizeEvent(QResizeEvent *)
{
    emit sizeChanged();
}

} // namespace coin
