#ifndef CUSTOMWIDGETS_H
#define CUSTOMWIDGETS_H

#include <QDoubleSpinBox>

class CustomDoubleSpinBox : public QDoubleSpinBox
{
public:
    explicit CustomDoubleSpinBox(QWidget *parent);
    QValidator::State validate(QString &input, int &pos) const;
};

#endif // CUSTOMWIDGETS_H
