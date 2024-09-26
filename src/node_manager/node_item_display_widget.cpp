/**
 * @brief node_item_display_widget.cpp
*/

#include <node_manager/node_item_display_widget.hpp>
#include <coin-node/package.hpp>
#include <coin-node/system.hpp>
#include <super_panel/super_panel.hpp>


namespace coin::node
{


NodeItemDisplayWidget::NodeItemDisplayWidget(QWidget *parent) : NodeItemBaseWidget(parent)
{
    auto* layout = static_cast<QVBoxLayout*>( this->layout() );

    __m_name->setReadOnly(true);
    __m_cateList->setReadOnly(true);
    __m_tagList->setReadOnly(true);
    __m_descInfo->setReadOnly(true);

    __m_terminal = new QTermWidget(this);
    layout->addWidget(__m_terminal);
    __m_terminal->hide();

    __m_stackWidget = new QStackedWidget(this);

    // button
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setMargin(0);
    btnLayout->setAlignment(Qt::AlignLeft);
    __m_editBtn = new EditButton("edit", this);
    __m_editBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(__m_editBtn, &EditButton::clicked, this, &NodeItemDisplayWidget::edit);
    btnLayout->addWidget(__m_editBtn);


    __m_deleteBtn = new DeleteButton("delete", this);
    __m_deleteBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(__m_deleteBtn, &DeleteButton::clicked, this, &NodeItemDisplayWidget::__remove);
    btnLayout->addWidget(__m_deleteBtn);

    __m_openBtn = new QPushButton("open", this);
    __m_openBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(__m_openBtn, &QPushButton::clicked, this, &NodeItemDisplayWidget::__open);
    btnLayout->addWidget(__m_openBtn);

    __m_buildBtn = new BuildButton("build", this);
    __m_buildBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(__m_buildBtn, &BuildButton::clicked, this, &NodeItemDisplayWidget::__build);
    btnLayout->addWidget(__m_buildBtn);

    __m_packageBtn = new QPushButton("package", this);
    __m_packageBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(__m_packageBtn, &QPushButton::clicked, this, &NodeItemDisplayWidget::package);
    btnLayout->addWidget(__m_packageBtn);

    auto* optWidget = new QWidget(this);
    optWidget->setContentsMargins(0, 0, 0, 0);
    optWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    optWidget->setLayout(btnLayout);

    __m_stackWidget->addWidget(optWidget);

    auto* infoLabel = new QLabel("Confirm to delete?");
    auto* confirmBtn = new QPushButton("confirm", this);
    connect(confirmBtn, &QPushButton::clicked, this, &NodeItemDisplayWidget::remove);
    auto* cancelBtn = new QPushButton("cancel", this);
    connect(cancelBtn, &QPushButton::clicked, [this]{
        __m_stackWidget->setCurrentIndex(0);
    });
    auto* confirmWidget = new QWidget(this);
    confirmWidget->setContentsMargins(0, 0, 0, 0);
    auto* confirmLayout = new QHBoxLayout();
    confirmLayout->setMargin(0);
    confirmLayout->addWidget(infoLabel);
    confirmLayout->addWidget(confirmBtn);
    confirmLayout->addWidget(cancelBtn);
    confirmWidget->setLayout(confirmLayout);
    __m_stackWidget->addWidget(confirmWidget);

    layout->addWidget(__m_stackWidget);
}

NodeItemDisplayWidget::~NodeItemDisplayWidget()
{
}

void NodeItemDisplayWidget::__remove()
{
    __m_stackWidget->setCurrentIndex(1);
}

void NodeItemDisplayWidget::__open()
{

    Package pkg({
        this->getName().toStdString(),
        this->getCategory().toStdString(),
        {},
        this->getDescInfo().toStdString()},
        SystemInfo::info().getParam<std::string>("workspace.path"));

    
    auto cmd = "cd " + pkg.path() + " && " + pkg.command("open");
    coin::superpanel::SuperPanelWidget::superpanel().addCommand(QString::fromStdString(cmd));


}
void NodeItemDisplayWidget::__build()
{

    Package pkg({
        this->getName().toStdString(),
        this->getCategory().toStdString(),
        {},
        this->getDescInfo().toStdString()},
        SystemInfo::info().getParam<std::string>("workspace.path"));

    
    auto cmd = "cd " + pkg.path() + " && " + pkg.command("build");


    coin::superpanel::SuperPanelWidget::superpanel().addCommandLine(QString::fromStdString(cmd));

}

} // coin::node
