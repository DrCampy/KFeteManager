#ifndef COUNTMONEY_H
#define COUNTMONEY_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QList>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QPushButton>
#include <QListView>
#include <QStringListModel>

#include "clientlist.h"

class JobistTag;
class JobistDelegate;
struct MoneyForm;

class CountMoney : public QWidget
{
    Q_OBJECT
public:
    explicit CountMoney(QWidget *parent = nullptr);

protected:
    MoneyForm *moneyForm;
    QPushButton *validate;
    QPushButton *cancel;

signals:
    void cancelled();

};

/*
 * Interface that requests a list of jobists
 * and the count of the cash register.
 */
class CountMoneyBefore : public CountMoney
{
    Q_OBJECT
public:
    explicit CountMoneyBefore(QWidget *parent = nullptr);

private:
    QComboBox *selectionCombo;
    QListView *jobistsList;
    QStringListModel *jobistsModel;
    QPushButton *add;


signals:
    void validated(qreal, QList<Client>);

private slots:
    void addJobist();
    void removeJobist(const QModelIndex &);
};


class CountMoneyAfter: public CountMoney
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

public:
    explicit MoneyForm(QWidget *parent = nullptr);
    ~MoneyForm();
    qreal getCount();

private:
    QList<QLabel *> *notesLabels;
    QList<QSpinBox *> *notesSpinBoxes;

    QList<QLabel *> *coinsLabels;
    QList<QSpinBox *> *coinsSpinBoxes;

    QVector<qreal> *notesValues;
    QVector<qreal> *coinsValues;
};

#endif // COUNTMONEY_H
