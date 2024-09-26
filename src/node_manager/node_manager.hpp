#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QList>
#include <QVector>

#include <coin-commons/utils/type.hpp>
#include <coin-node/package.hpp>

#include <node_manager/display_widget.hpp>
#include "node_item.hpp"

namespace coin::node
{

class NodeTableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NodeTableWidget(QWidget* parent = nullptr);
    ~NodeTableWidget();

    void addNodeItem(NodeItemWidget* item);
    void addNodeItem(QWidget* item);
    void removeNodeItem(NodeItemWidget* item);
    QList<DisplayWidget*> items();

private:
    QVBoxLayout* __m_layout;

};

class NodeManagerWidget : public QWidget
{
    Q_OBJECT
public:

    explicit NodeManagerWidget(QWidget* parent = nullptr);
    ~NodeManagerWidget();

    QList<DisplayWidget*> nodes();

private:
    NodeTableWidget* __m_nodeTable;

private slots:
    void onAddNodeItem();
    void onEditNodeItem();
    void onRemoveNodeItem();

signals:
    void addNode(const coin::node::PackageBaseInfo&);

};

}