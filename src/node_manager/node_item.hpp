#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QStackedWidget>

#include <coin-node/package.hpp>

#include <node_manager/node_item_display_widget.hpp>

#include <QtDebug>

namespace coin::node
{


class NodeItemCreateWidget : public NodeItemBaseWidget
{
    Q_OBJECT
public:
    explicit NodeItemCreateWidget(QWidget* parent = nullptr);
    ~NodeItemCreateWidget();

private:
    QPushButton *__m_createBtn;
    QPushButton *__m_cancelBtn;

signals:
    void created();
    void canceled();


protected:
    void paintEvent(QPaintEvent *event) override;
    
};

class NodeItemEditWidget : public NodeItemBaseWidget
{
    Q_OBJECT
public:
    explicit NodeItemEditWidget(QWidget* parent = nullptr);
    ~NodeItemEditWidget();

private:
    QPushButton *__m_saveBtn;
    QPushButton *__m_cancelBtn;

signals:
    void saved();
    void canceled();

private slots:
    void __save();

};

class NodeItemWidget : public QWidget
{

    Q_OBJECT
public:
    explicit NodeItemWidget(QWidget* parent = nullptr);
    explicit NodeItemWidget(const PackageBaseInfo& info, QWidget* parent = nullptr);
    ~NodeItemWidget();

private:
    /**
     * A node should has information:
     *   1. Node name
     *   2. Node category list
     *   3. Description
     *   4. Open, Build, Package operation
    */

   enum class NodeItemWidgetState
   {
       Create,
       Edit,
       Display
   };

   NodeItemCreateWidget *__m_createWidget;
   NodeItemEditWidget *__m_editWidget;
   NodeItemDisplayWidget *__m_displayWidget;

   QStackedWidget *__m_stackWidget;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


private:
    /**
     * @brief create package
    */
    void nodeCreate_();
    /**
     * @brief cancel create package
    */
    void nodeCancel_();
    /**
     * @brief edit package information
    */
    void edit();
    /**
     * @brief save edit of package
    */
    void saved_();
    /**
     * @brief cancel edit of package
    */
    void canceled_();

    void remove_();

signals:
    void nodeCreated(const QString& name, const QString& category, const QStringList& tagList, const QString& descInfo);
    void nodeSaved(const QString& name, const QString& category, const QStringList& tagList, const QString& descInfo);
    void nodeDeleted(const QString& name, const QString& category);

    void canceled();
    void created(PackageBaseInfo);
    void removed();
    void saved();


};

}