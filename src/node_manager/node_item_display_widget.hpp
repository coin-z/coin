/**
 * @brief node_item_display_widget.hpp
*/

#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QStackedWidget>

#include <qtermwidget5/qtermwidget.h>
#include "widgets/tag_widget.hpp"

#include <coin-node/package.hpp>

#include <node_manager/node_item_base.hpp>

#include <QtDebug>

namespace coin::node
{

class NodeItemDisplayWidget : public NodeItemBaseWidget
{
    Q_OBJECT
public:
    explicit NodeItemDisplayWidget(QWidget* parent = nullptr);
    ~NodeItemDisplayWidget();

private:
    EditButton *__m_editBtn;
    DeleteButton *__m_deleteBtn;
    QPushButton *__m_openBtn;
    BuildButton *__m_buildBtn;
    QPushButton *__m_packageBtn;

    QStackedWidget *__m_stackWidget;

signals:
    void edit();
    void remove();
    void open();
    void build();
    void package();

private slots:
    void __remove();
    void __open();
    void __build();

};

} // coin::node
