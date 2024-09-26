/**
 * @file env_editor.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>

class EnvEditor : public QWidget
{
    Q_OBJECT
public:
    explicit EnvEditor(QWidget* parent = nullptr);
    ~EnvEditor();

    void loadEnv(const QVector<std::pair<QString, QString>>& envs);
private:
    QTableWidget *env_table_;
};