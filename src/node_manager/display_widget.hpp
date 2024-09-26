/**
 * @file display_widget.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QDrag>

#include <coin-node/package.hpp>

namespace coin::node
{

class DisplayWidget : public QLabel
{

    Q_OBJECT
public:
    DisplayWidget(const coin::node::PackageBaseInfo& info, QWidget* parent = nullptr);
    ~DisplayWidget();

    inline const coin::node::PackageBaseInfo& info() const { return info_; }

protected:

    void mousePressEvent(QMouseEvent *event) override;

private:
    QMenu* menu_;
    coin::node::PackageBaseInfo info_;

signals:
    void detail();

};

} // namespace coin::node
