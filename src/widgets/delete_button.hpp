/**
 * delete_button.hpp
*/
#pragma once
#include <QPushButton>

class DeleteButton : public QPushButton
{

    Q_OBJECT
public:
    explicit DeleteButton(const QString& text, QWidget* parent = nullptr);
    virtual ~DeleteButton() = default;

};