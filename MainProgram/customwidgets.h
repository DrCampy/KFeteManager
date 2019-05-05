#ifndef CUSTOMWIDGETS_H
#define CUSTOMWIDGETS_H

#include <QDoubleSpinBox>
#include <QStyledItemDelegate>

class CustomDoubleSpinBox : public QDoubleSpinBox
{
public:
    explicit CustomDoubleSpinBox(QWidget *parent);
    QValidator::State validate(QString &input, int &pos) const;
};


class DateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateDelegate(QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const;
};

#endif // CUSTOMWIDGETS_H
