#ifndef COUNTMONEY_H
#define COUNTMONEY_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QList>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>

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
    QListWidget *jobistsList;
    QList<JobistTag> *jobistsModel;
    QPushButton *add;


signals:
    void validated(qreal, QList<Client>);

private slots:
    void addJobist();
    void removeJobist(QString name);
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

//Item delegate to paint the names of the jobists in the list and also
//Add a button that user have to click to remove the jobist.
class JobistDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit JobistDelegate(QWidget *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    //QSize sizeHint(const QStyleOptionViewItem &option,
    //               const QModelIndex &index) const;
};

class JobistTag
{

private:
    QString name;

public:
    explicit JobistTag(QString name = "");
    void setName(QString name);
    void paint(QPainter *painter, const QStyleOptionViewItem &option);
    bool operator==(const JobistTag &tag) const;
};
Q_DECLARE_METATYPE(JobistTag)

#endif // COUNTMONEY_H
