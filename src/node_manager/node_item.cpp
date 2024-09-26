#include "node_item.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

#include <QPainter>
#include <QPainterPath>

#include <coin-node/package.hpp>
#include <coin-node/system.hpp>

namespace coin::node
{

namespace impl
{

template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

}

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

NodeItemCreateWidget::NodeItemCreateWidget(QWidget *parent) : NodeItemBaseWidget(parent)
{
    auto* layout = static_cast<QVBoxLayout*>( this->layout() );
    // button
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setMargin(0);
    __m_createBtn = new QPushButton("create", this);
    __m_createBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    __m_cancelBtn = new QPushButton("cancel", this);
    __m_cancelBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btnLayout->addWidget(__m_createBtn);
    btnLayout->addWidget(__m_cancelBtn);
    layout->addLayout(btnLayout);

    connect(__m_createBtn, &QPushButton::clicked, this, &NodeItemCreateWidget::created);
    connect(__m_cancelBtn, &QPushButton::clicked, this, &NodeItemCreateWidget::canceled);

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

NodeItemCreateWidget::~NodeItemCreateWidget()
{
}

void NodeItemCreateWidget::paintEvent(QPaintEvent *event)
{

}

////////////////////////////////////////////////////////////////////////////////////////////

NodeItemEditWidget::NodeItemEditWidget(QWidget *parent) : NodeItemBaseWidget(parent)
{
    auto* layout = static_cast<QVBoxLayout*>( this->layout() );

    // 属性设置
    __m_name->setReadOnly(true);
    __m_cateList->setReadOnly(true);

    // button
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setMargin(0);
    btnLayout->setAlignment(Qt::AlignRight);
    __m_saveBtn = new QPushButton("save", this);
    __m_saveBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    __m_cancelBtn = new QPushButton("cancel", this);
    __m_cancelBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnLayout->addWidget(__m_saveBtn);
    btnLayout->addWidget(__m_cancelBtn);
    layout->addLayout(btnLayout);

    connect(__m_saveBtn, &QPushButton::clicked, this, &NodeItemEditWidget::__save);
    connect(__m_cancelBtn, &QPushButton::clicked, this, &NodeItemEditWidget::canceled);
}

NodeItemEditWidget::~NodeItemEditWidget()
{
}

void NodeItemEditWidget::__save()
{

    Package pkg({
        this->getName().toStdString(),
        this->getCategory().toStdString(),
        {},
        this->getDescInfo().toStdString()},
        SystemInfo::info().getParam<std::string>("workspace.path"));

    auto ret = pkg.save();
    if(not ret)
    {
        QMessageBox::warning(this, "warning", "save failed: " + QString::fromStdString(ret.reason()), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    emit saved();
}


////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////

NodeItemWidget::NodeItemWidget(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    __m_stackWidget = new QStackedWidget(this);

    __m_createWidget = new NodeItemCreateWidget(__m_stackWidget);
    __m_createWidget->setObjectName("create_widget");
    __m_editWidget = new NodeItemEditWidget(__m_stackWidget);
    __m_editWidget->setObjectName("edit_widget");
    __m_displayWidget = new NodeItemDisplayWidget(__m_stackWidget);
    __m_displayWidget->setObjectName("display_widget");

    __m_stackWidget->addWidget(__m_createWidget);
    __m_stackWidget->addWidget(__m_editWidget);
    __m_stackWidget->addWidget(__m_displayWidget);

    __m_stackWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout->addWidget(__m_stackWidget);

    connect(__m_createWidget, &NodeItemCreateWidget::created, this, &NodeItemWidget::nodeCreate_);
    connect(__m_createWidget, &NodeItemCreateWidget::canceled, this, &NodeItemWidget::nodeCancel_);
    connect(__m_editWidget, &NodeItemEditWidget::saved, this, &NodeItemWidget::saved_);
    connect(__m_editWidget, &NodeItemEditWidget::canceled, this, &NodeItemWidget::canceled_);
    connect(__m_displayWidget, &NodeItemDisplayWidget::edit, this, &NodeItemWidget::edit);
    connect(__m_displayWidget, &NodeItemDisplayWidget::remove, this, &NodeItemWidget::remove_);
    connect(__m_stackWidget, &QStackedWidget::currentChanged, this, [this](int idx){
        auto* widget = __m_stackWidget->widget(idx);
        widget->setFixedHeight(widget->sizeHint().height());
        __m_stackWidget->setFixedHeight(widget->sizeHint().height());
    });

}

NodeItemWidget::NodeItemWidget(const PackageBaseInfo &info, QWidget *parent) : QWidget(parent)
{
    // 设置按钮背景为深色，字体为白色
    auto* layout = new QVBoxLayout(this);

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    __m_stackWidget = new QStackedWidget(this);

    __m_createWidget = new NodeItemCreateWidget(__m_stackWidget);
    __m_createWidget->setObjectName(QString::fromStdString( info.name() ) + "/create_widget");
    __m_editWidget = new NodeItemEditWidget(__m_stackWidget);
    __m_editWidget->setObjectName(QString::fromStdString( info.name() ) + "/edit_widget");
    __m_displayWidget = new NodeItemDisplayWidget(__m_stackWidget);
    __m_displayWidget->setObjectName(QString::fromStdString( info.name() ) + "/display_widget");

    __m_stackWidget->addWidget(__m_createWidget);
    __m_stackWidget->addWidget(__m_editWidget);
    __m_stackWidget->addWidget(__m_displayWidget);

    __m_displayWidget->setName( QString::fromStdString( info.name() ));
    __m_displayWidget->setDescInfo( QString::fromStdString( info.description() ));
    __m_displayWidget->setCategory( QString::fromStdString( info.category() ));

    QStringList tags;
    for (const auto& tag : info.tags())
    {
        tags << QString::fromStdString(tag);
    }
    __m_displayWidget->setTagList(tags);

    __m_stackWidget->setCurrentIndex(impl::to_underlying(NodeItemWidgetState::Display));

    layout->addWidget(__m_stackWidget);

    connect(__m_createWidget, &NodeItemCreateWidget::created, this, &NodeItemWidget::nodeCreate_);
    connect(__m_createWidget, &NodeItemCreateWidget::canceled, this, &NodeItemWidget::nodeCancel_);
    connect(__m_editWidget, &NodeItemEditWidget::saved, this, &NodeItemWidget::saved_);
    connect(__m_editWidget, &NodeItemEditWidget::canceled, this, &NodeItemWidget::canceled_);
    connect(__m_displayWidget, &NodeItemDisplayWidget::edit, this, &NodeItemWidget::edit);
    connect(__m_displayWidget, &NodeItemDisplayWidget::remove, this, &NodeItemWidget::remove_);
    connect(__m_stackWidget, &QStackedWidget::currentChanged, this, [this](int idx){
        auto* widget = __m_stackWidget->widget(idx);
        widget->setFixedHeight(widget->sizeHint().height());
        __m_stackWidget->setFixedHeight(widget->sizeHint().height());
    });
}

NodeItemWidget::~NodeItemWidget()
{

}

void NodeItemWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // 绘制圆角矩形
    const float width = 5.0;
    painter.setPen(QPen(Qt::blue, width)); // 设置边框颜色，宽度
    QPainterPath path;
    QRectF r(rect().left() + width * 0.5, rect().top() + width * 0.5, rect().width() - (width), rect().height() - (width));
    path.addRoundedRect(r, 10.0, 10.0); // 设置圆角半径
    painter.drawPath(path);

    painter.setRenderHint(QPainter::Antialiasing, false);

}

void NodeItemWidget::resizeEvent(QResizeEvent *event)
{
    auto* widget = __m_stackWidget->currentWidget();
    widget->setFixedHeight(widget->sizeHint().height());
    __m_stackWidget->setFixedHeight(widget->sizeHint().height());
}

void NodeItemWidget::nodeCreate_()
{
    // check name or category is empty
    if (__m_createWidget->getName().isEmpty() || __m_createWidget->getCategory().isEmpty())
    {
        QMessageBox::warning(this, "warning", "name or category is empty");
        return;
    }

    // check if the package is already exist
    if (Package::isExist(
        __m_createWidget->getCategory().toStdString(),
        __m_createWidget->getName().toStdString(), 
        SystemInfo::info().getParam<std::string>("workspace.path")))
    {
        QMessageBox::warning(this, "warning", "package is already exist");
        return;
    }

    std::vector<std::string> tags;
    for (auto& tag : __m_createWidget->getTagList().toStdList())
    {
        tags.push_back(tag.toStdString());
    }
    Package pkg({
        __m_createWidget->getName().toStdString(),
        __m_createWidget->getCategory().toStdString(),
        tags,
        __m_createWidget->getDescInfo().toStdString()},
        SystemInfo::info().getParam<std::string>("workspace.path"));
    
    pkg.create();

    __m_displayWidget->setName(__m_createWidget->getName());
    __m_displayWidget->setCategory(__m_createWidget->getCategory());
    __m_displayWidget->setTagList(__m_createWidget->getTagList());
    __m_displayWidget->setDescInfo(__m_createWidget->getDescInfo());
    __m_stackWidget->setCurrentIndex(impl::to_underlying(NodeItemWidgetState::Display));

    emit created(pkg.info());
}

void NodeItemWidget::nodeCancel_()
{
    this->deleteLater();
    emit canceled();
}

void NodeItemWidget::saved_()
{
    __m_displayWidget->setName(__m_editWidget->getName());
    __m_displayWidget->setCategory(__m_editWidget->getCategory());
    __m_displayWidget->setTagList(__m_editWidget->getTagList());
    __m_displayWidget->setDescInfo(__m_editWidget->getDescInfo());

    __m_stackWidget->setCurrentIndex(impl::to_underlying(NodeItemWidgetState::Display));
}
void NodeItemWidget::canceled_()
{
    __m_stackWidget->setCurrentIndex(impl::to_underlying(NodeItemWidgetState::Display));
}

void NodeItemWidget::remove_()
{
    Package pkg({
        __m_displayWidget->getName().toStdString(),
        __m_displayWidget->getCategory().toStdString(),
        {},
        __m_displayWidget->getDescInfo().toStdString()},
        SystemInfo::info().getParam<std::string>("workspace.path"));
    
    pkg.remove();
    this->deleteLater();
    emit removed();
}

void NodeItemWidget::edit()
{
    __m_editWidget->setName(__m_displayWidget->getName());
    __m_editWidget->setCategory(__m_displayWidget->getCategory());
    __m_editWidget->setTagList(__m_displayWidget->getTagList());
    __m_editWidget->setDescInfo(__m_displayWidget->getDescInfo());
    __m_stackWidget->setCurrentIndex(impl::to_underlying( NodeItemWidgetState::Edit ));
}

}

