#pragma once
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

namespace coin
{
class TagLabel : public QWidget
{
    Q_OBJECT
public:
    explicit TagLabel(const QString &text, QWidget *parent = nullptr);

    void close();

    QString text() const;

    void setReadOnly(const bool b);

    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *text_label = nullptr;
    QPushButton *close_button = nullptr;

    bool __m_isReadOnly = false;
};

class TagWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TagWidget(QWidget *parent = nullptr);

    bool eventFilter(QObject *object, QEvent *event) override;

    QStringList getTagList() const;

    void clearAll();

    void setTagList(const QStringList& tag_list);

    void setReadOnly(const bool b);

private slots:
    void addTag();

private:
    QLineEdit *line_edit;
signals:
    void tagAdded(const QString &tag_text);
};

} // namespace coin
