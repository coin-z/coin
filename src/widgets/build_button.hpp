/**
 * @brief build_button.hpp
*/

#pragma once
#include <QPushButton>

class BuildButton : public QPushButton
{

    Q_OBJECT
public:
    explicit BuildButton(const QString& text, QWidget* parent = nullptr);
    virtual ~BuildButton() = default;

};
