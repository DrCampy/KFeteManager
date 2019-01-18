#include "customwidgets.h"
#include <QDebug>

CustomDoubleSpinBox::CustomDoubleSpinBox(QWidget *parent) : QDoubleSpinBox (parent){}

QValidator::State CustomDoubleSpinBox::validate(QString &input, int &pos) const{
    QLocale locale;
    if(pos != 0 && (input[pos-1] == "." || input[pos-1] == ",")){
        input.replace(pos-1, 1, locale.decimalPoint());
    }

    return QDoubleSpinBox::validate(input, pos);
}
