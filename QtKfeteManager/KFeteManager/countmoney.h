#ifndef COUNTMONEY_H
#define COUNTMONEY_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QList>

class CountMoneyBefore : public QWidget
{
    Q_OBJECT
public:
    explicit CountMoneyBefore(QWidget *parent = nullptr);

signals:

public slots:
};


class CountMoneyAfter: public QWidget
{
    Q_OBJECT
public:
    explicit CountMoneyAfter(QWidget *parent = nullptr);

signals:

public slots:
};

struct MoneyForm : public QWidget
{
    Q_OBJECT

    explicit MoneyForm(QWidget *parent = nullptr);
    ~MoneyForm();

    qreal getCount();

    QList<QLabel *> *notesLabels;
    QList<QSpinBox *> *notesSpinBoxes;

    QList<QLabel *> *coinsLabels;
    QList<QSpinBox *> *coinsSpinBoxes;

    QVector<qreal> *notesValues;
    QVector<qreal> *coinsValues;
};


#endif // COUNTMONEY_H
