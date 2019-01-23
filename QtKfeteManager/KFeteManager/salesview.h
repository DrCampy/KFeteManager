#ifndef SALESVIEW_H
#define SALESVIEW_H

#include <QWidget>
#include <QString>
#include <QButtonGroup>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QLabel>
#include <QAbstractListModel>
#include <QSqlTableModel>
#include <QListView>
#include <QComboBox>
#include <QDialog>
#include <customwidgets.h>


#include "catalog.h"
#include "currentordermodel.h"
#include "carteview.h"
#include "cartemodel.h"
#include "currentorderview.h"

class SalesView;

/*
 * Interface principale avec la carte, les comptes,...
 */
class SalesView : public QWidget
{
    Q_OBJECT
public:
    explicit SalesView(QWidget *parent = nullptr);
    CarteModel *getCarteModel();

private:

    CarteModel          *carteModel;
    CarteView           *carteView;
    QLabel              *totalLabel;
    CurrentOrderView    *currentOrderView;
    Client              selectedClient = Client("");
    CurrentOrderModel   *currentOrderModel;
    QToolButton         *deposit;
    QToolButton         *withdraw;
    QToolButton         *count;
    QToolButton         *accounts;
    QPushButton         *orders;
    QPushButton         *validate;

    void currentOrderViewResize();
    bool processOrder(const Order &o, Client c);

signals:
    void countBefore();
    void countAfter();
    void manageOrders();

public slots:
    void validateOrder();
    void selectClient(Client c = Client());

private slots:
    void cashDeposit();
    void clientDeposit();
    void cashWithdraw();
    void clientWithdraw();

};

//QAction used in the topbar to select a client account
class AccountSelector : public QWidgetAction
{
    Q_OBJECT
public:
    explicit AccountSelector(QWidget *parent = nullptr);
    QWidget *createWidget(QWidget *parent);

private:
    void refreshList();
    QSqlQueryModel *clientsModel;
    QLineEdit *searchBar;
    QListView *listView;
    QWidget *lastCreatedWidget;
    QLabel *balance;

public slots:
    void searchBarUpdated(QString text);
    void searchBarReturnPressed();
    void listViewItemActivated(const QModelIndex &index);
    void clientHighlighted(const QModelIndex &index);

signals:
    void clientSelected(Client);

};

class ClientComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit ClientComboBox(QWidget *parent = nullptr);

private slots:
    void refresh();

};

class CustomSelectorPopup : protected QDialog
{
    Q_OBJECT
public:
    enum SelectionFlag{
        Amount = 0x1,
        Client = 0x2,
        Note   = 0x4,
    };
    Q_DECLARE_FLAGS(SelectionFlags, SelectionFlag) //Declare the flags as flags for Qt

    explicit CustomSelectorPopup(QWidget *parent = nullptr, SelectionFlags flags = Amount);
    bool ask(QVariant *note, QVariant *amount = nullptr, QVariant *client = nullptr);
    void setTitle(QString title = QString());

private:
    QLabel              *accountLabel   = nullptr;
    ClientComboBox      *clientCombo    = nullptr;

    QLabel              *amountLabel    = nullptr;
    CustomDoubleSpinBox *amount         = nullptr;

    QLabel              *noteLabel      = nullptr;
    QLineEdit           *note           = nullptr;

    SelectionFlags      flags;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(CustomSelectorPopup::SelectionFlags) //Allows flags to be OR'ed.

#endif // SALESVIEW_H


