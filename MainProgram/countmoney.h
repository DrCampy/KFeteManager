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
#include <QSqlQueryModel>

#include "clientlist.h"

class CountMoney;
class CountMoneyBefore;
class CountMoneyAfter;
class JobistTag;
class JobistDelegate;
struct MoneyForm;

struct MoneyForm : public QWidget
{
    Q_OBJECT

public:
    explicit MoneyForm(QWidget *parent = nullptr);
    ~MoneyForm();
    qreal getTotal();
    void setCount(const QList<uint> &notes, const QList<uint> &coins);
    void getCount(QList<uint> &notes, QList<uint> &coins);

private:
    QList<QLabel *> *notesLabels;
    QList<QSpinBox *> *notesSpinBoxes;

    QList<QLabel *> *coinsLabels;
    QList<QSpinBox *> *coinsSpinBoxes;

    QVector<qreal> *notesValues;
    QVector<qreal> *coinsValues;

    QLocale *locale;

    friend CountMoney;
};

class CountMoney : public QWidget
{
    Q_OBJECT
public:
    explicit CountMoney(QWidget *parent = nullptr);
    void load(QString settingsName);
    void save(QString settingsName);
    qreal getTotal(){return moneyForm->getTotal();}

protected:
    QPushButton *validate;
    QPushButton *cancel;
    MoneyForm *moneyForm;

signals:
    void cancelled();
    void validated();

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
    QList<Client> getJobists();
    void setJobists(QList<Client> clients);
    void refresh();

private:
    QComboBox *selectionCombo;
    QListView *jobistsList;
    QStringListModel *jobistsModel;
    QPushButton *add;
    QSqlQueryModel *clientsModel;

private slots:
    void addJobist();
    void addJobists(QStringList jobists);
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

#endif // COUNTMONEY_H
