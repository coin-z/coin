/**
 * @brief node_item_base.cpp
*/

#include <node_manager/node_item_base.hpp>

namespace coin::node
{

NodeItemBaseWidget::NodeItemBaseWidget(QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet(
                            "QPushButton {"
                            "    border: 2px solid white;"
                            "    font-weight: bold;"
                            "    border-radius: 10px;"
                            "    background-color: #4CAF50;"
                            "    color: white;"
                            "    padding: 10px 20px;"
                            "}"
                            "QPushButton:hover {"
                            "    background-color: #45a049;"
                            "}"
                            "QPushButton:pressed {"
                            "    background-color: red;"
                            "}"
                            "QLineEdit {"
                            "    color: white; background-color: #212121; font-size: 18px; font-weight: bold; border: 3px solid green; border-radius: 10px; padding: 10px;"
                            "}"
                            "QLabel {"
                            "    color: white; background-color: #212121; font-size: 18px; font-weight: bold; border: 0px solid green; border-radius: 10px; padding: 10px;"
                            "}"
                            );
    __m_layout = new QVBoxLayout(this);
    __m_layout->setMargin(0);
    __m_layout->setAlignment(Qt::AlignTop);

    auto* infoLayout = new QGridLayout();
    infoLayout->setMargin(0);
    // title
    auto* nameLabel = new QLabel("Name:", this);
    nameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    __m_name = new QLineEdit(this);
    infoLayout->addWidget(nameLabel, 0, 1);
    infoLayout->addWidget(__m_name, 0, 2);

    // info
    auto* cateLabel = new QLabel("Category:", this);
    cateLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    __m_cateList = new QLineEdit(this);
    infoLayout->addWidget(cateLabel, 1, 1);
    infoLayout->addWidget(__m_cateList, 1, 2);

    // tags
    auto* tagLabel = new QLabel("Tags:", this);
    tagLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    __m_tagList = new TagWidget(this);
    infoLayout->addWidget(tagLabel, 2, 1);
    infoLayout->addWidget(__m_tagList, 2, 2);

    // desc
    auto* descLabel = new QLabel("Description:", this);
    descLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    __m_descInfo = new QLineEdit(this);
    infoLayout->addWidget(descLabel, 3, 1);
    infoLayout->addWidget(__m_descInfo, 3, 2);

    __m_layout->addLayout(infoLayout);

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

NodeItemBaseWidget::~NodeItemBaseWidget()
{
}


void NodeItemBaseWidget::setName(const QString& name)
{
    __m_name->setText(name);
}
void NodeItemBaseWidget::setCategory(const QString& category)
{
    __m_cateList->setText(category);
}
void NodeItemBaseWidget::setTagList(const QStringList& tagList)
{
    __m_tagList->setTagList(tagList);
}
void NodeItemBaseWidget::setDescInfo(const QString& descInfo)
{
    __m_descInfo->setText(descInfo);
}

QString NodeItemBaseWidget::getName() const
{
    return __m_name->text();
}
QString NodeItemBaseWidget::getCategory() const
{
    return __m_cateList->text();
}
QStringList NodeItemBaseWidget::getTagList() const
{
    return __m_tagList->getTagList();
}
QString NodeItemBaseWidget::getDescInfo() const
{
    return __m_descInfo->text();
}

} // namespace coin::node

