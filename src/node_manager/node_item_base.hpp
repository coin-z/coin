/**
 * @brief node_item_base.hpp
*/

#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QStackedWidget>
#include <qtermwidget5/qtermwidget.h>
#include "widgets/tag_widget.hpp"

#include <widgets/edit_button.hpp>
#include <widgets/delete_button.hpp>
#include <widgets/build_button.hpp>

namespace coin::node
{
    
class NodeItemBaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NodeItemBaseWidget(QWidget* parent = nullptr);
    ~NodeItemBaseWidget();

    void setName(const QString& name);
    void setCategory(const QString& category);
    void setTagList(const QStringList& tagList);
    void setDescInfo(const QString& descInfo);

    QString getName() const;
    QString getCategory() const;
    QStringList getTagList() const;
    QString getDescInfo() const;

    virtual void setReadOnly(const bool b) { }

protected:
    QLineEdit   *__m_name;
    QLineEdit   *__m_cateList;
    TagWidget   *__m_tagList;
    QLineEdit   *__m_descInfo;

    QTermWidget *__m_terminal = nullptr;

private:
    QVBoxLayout *__m_layout;

};
} // namespace coin::node
