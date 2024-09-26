/**
 * @brief edit_button.hpp
*/

#pragma once
#include <QPushButton>

class EditButton : public QPushButton
{

    Q_OBJECT
public:
    explicit EditButton(const QString& text, QWidget* parent = nullptr);
    EditButton() = default;
    virtual ~EditButton() = default;

}; // EditButton
