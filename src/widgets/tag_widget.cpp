#include "tag_widget.hpp"

namespace coin
{
TagLabel::TagLabel(const QString &text, QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Window);

    text_label = new QLabel(text);
    text_label->setContentsMargins(6, 0, 0, 0);
    text_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // 创建删除按钮
    auto *deleteButton = new QPushButton("X", this);
    deleteButton->setIcon(QIcon(":/icons/delete.png"));
    deleteButton->setFixedSize(16, 16);
    deleteButton->setObjectName("TagLabel-DeleteButton");
    deleteButton->setStyleSheet("QPushButton#TagLabel-DeleteButton {"
                                "border: none;"
                                "background: transparent;"
                                "}"
                                "QPushButton#TagLabel-DeleteButton:hover {"
                                "color: red;"
                                "}");
    connect(deleteButton, &QPushButton::clicked, this, &TagLabel::close);

    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(text_label, 1);
    layout->addWidget(deleteButton);
}

void TagLabel::close()
{
    if(not __m_isReadOnly)
    {
        deleteLater();
    }
}

QString TagLabel::text() const { return text_label->text(); }

void TagLabel::setReadOnly(const bool b) 
{
    __m_isReadOnly = b;
}

void TagLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto color = QColor(255, 255, 255);
    auto border_color = QColor(120, 120, 120);
    auto border_radius = 10;

    painter.setBrush(color);
    painter.setPen(border_color);
    painter.drawRoundedRect(rect(), border_radius, border_radius);
}


TagWidget::TagWidget(QWidget *parent) : QWidget(parent)
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    line_edit = new QLineEdit;
    line_edit->setPlaceholderText("Add Tag");
    line_edit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    line_edit->installEventFilter(this);
    connect(line_edit, &QLineEdit::returnPressed, this, &TagWidget::addTag);

    layout->addWidget(line_edit, 1);
    layout->addSpacing(5);

    setLayout(layout);
}

bool TagWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == line_edit && event->type() == QEvent::KeyPress)
    {
        auto key_event = static_cast<QKeyEvent *>(event);
        if (key_event->key() == Qt::Key_Enter || key_event->key() == Qt::Key_Return)
        {
            addTag();
            return true;
        }
    }
    return false;
}

QStringList TagWidget::getTagList() const
{
    QStringList tag_list;
    for (int i = 0; i < this->layout()->count() - 2; ++i)
    {
        auto tag_label = static_cast<TagLabel *>(this->layout()->itemAt(i)->widget());
        tag_list << tag_label->text();
    }
    return tag_list;
}

void TagWidget::clearAll()
{
    for (int i = 0; i < this->layout()->count() - 2; ++i)
    {
        auto tag_label = static_cast<TagLabel *>(this->layout()->itemAt(i)->widget());
        tag_label->close();
        tag_label->deleteLater();
    }
}

void TagWidget::setTagList(const QStringList& tag_list)
{
    clearAll();
    for (auto tag : tag_list)
    {
        auto tag_label = new TagLabel(tag, this);
        tag_label->setReadOnly(line_edit->isReadOnly());
        static_cast<QHBoxLayout *>(this->layout())->insertWidget(this->layout()->count() - 2, tag_label);
    }
    line_edit->clear();
}

void TagWidget::setReadOnly(const bool b)
{
    line_edit->setReadOnly(b);
    for (int i = 0; i < this->layout()->count() - 2; ++i)
    {
        auto tag_label = static_cast<TagLabel *>(this->layout()->itemAt(i)->widget());
        tag_label->setReadOnly(b);
    }
}

void TagWidget::addTag()
{
    auto tag_text = line_edit->text().trimmed();
    if (!tag_text.isEmpty())
    {
        auto tag_label = new TagLabel(tag_text, this);
        static_cast<QHBoxLayout *>(this->layout())->insertWidget(this->layout()->count() - 2, tag_label);
        line_edit->clear();
        emit tagAdded(tag_text);
    }
}


} // namespace coin
