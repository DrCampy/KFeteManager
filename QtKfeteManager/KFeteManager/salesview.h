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
#include <QDoubleSpinBox>


#include "catalog.h"
#include "currentordermodel.h"
#include "carteview.h"
#include "cartemodel.h"

class SalesView;
class MiddleBar;


class MiddleBar : public QWidget
{
Q_OBJECT
public:
    explicit MiddleBar(QWidget *parent = nullptr);
    CurrentOrderModel::Action getLastPerformedAction() const;
    Order::Price getSelectedPrice() const;
    void resetPrice();

private:
    QPushButton *plusButton;
    QPushButton *minusButton;
    QPushButton *deleteButton;
    QButtonGroup *priceButtonsGroup;
    QPushButton *normalPriceButton;
    QPushButton *reducedPriceButton;
    QPushButton *freePriceButton;

    CurrentOrderModel::Action lastPerformedAction;
    Order::Price selectedPrice;

private slots:
    void plusSlot();
    void minusSlot();
    void deleteSlot();
    void priceSlot(int id);

signals:
    void actionPerformed();
    void priceChanged();
};

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

    CarteModel  *carteModel;
    CarteView   *carteView;
    QLabel      *totalLabel;
    MiddleBar   *middleBar;
    Client      selectedClient = Client("");

    QTableView *currentOrderView;
    CurrentOrderModel *currentOrderModel;

    void currentOrderViewResize();
    bool processOrder(const Order &o, Client c);

signals:
    void performAction();
    void updatePrice();
    void addArticle(QString);
    void countBefore();
    void countAfter();
    void manageOrders();

public slots:
    void modelUpdated();
    void actionPerformed();
    void priceUpdated();
    void articleAdded(QString);
    void validateOrder();
    void selectClient(Client c);

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
        Client = 0x2
    };
    Q_DECLARE_FLAGS(SelectionFlags, SelectionFlag) //Declare the flags as flags for Qt

    explicit CustomSelectorPopup(QWidget *parent = nullptr, SelectionFlags flags = Amount);
    bool ask(QVariant *amount, QVariant *client = nullptr);
    void setTitle(QString title = QString());

private:
    QLabel *accountLabel = nullptr;
    ClientComboBox *clientCombo = nullptr;
    QLabel *amountLabel = nullptr;
    QDoubleSpinBox *amount = nullptr;
    SelectionFlags flags;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(CustomSelectorPopup::SelectionFlags) //Allows flags to be OR'ed.

#endif // SALESVIEW_H


