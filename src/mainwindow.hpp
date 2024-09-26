#pragma once
#include <QMainWindow>
#include <QResizeEvent>
#include <QTimer>

#include <widgets/float_widget.hpp>
#include <node_manager/node_manager.hpp>
#include <control_panel/control_panel.hpp>


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    coin::node::NodeManagerWidget* m_nodeManager;
    coin::FloatWidget* m_floatWidget;
    coin::FloatWidget* m_floatFocusePanel;
    coin::control_panel::ControlPanel* m_controlPanel;
};
