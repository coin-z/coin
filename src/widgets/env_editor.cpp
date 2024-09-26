/**
 * @file env_editor.cpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "env_editor.hpp"

EnvEditor::EnvEditor(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);

    env_table_ = new QTableWidget(this);
    layout->addWidget(env_table_);
}

EnvEditor::~EnvEditor()
{

}

void EnvEditor::loadEnv(const QVector<std::pair<QString, QString>>& envs)
{
    env_table_->setRowCount(envs.size());
    env_table_->setColumnCount(2);
    for (int i = 0; i < envs.size(); i++)
    {
        env_table_->setItem(i, 0, new QTableWidgetItem(envs[i].first));
        env_table_->setItem(i, 1, new QTableWidgetItem(envs[i].second));
    }
}