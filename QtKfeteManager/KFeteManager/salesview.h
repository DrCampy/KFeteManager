#ifndef SALESVIEW_H
#define SALESVIEW_H

#include <QWidget>
#include <QString>
#include <QButtonGroup>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QLabel>

#include "catalog.h"
#include "currentordermodel.h"
#include "carteview.h"
#include "cartemodel.h"

class TopBar;
class SalesView;
class MiddleBar;

class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);
};

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
    explicit SalesView(QWidget *parent = nullptr );
    CarteModel *getCarteModel();

private:

    CarteModel  *carteModel;
    CarteView   *carteView;
    TopBar      *topBar;
    QLabel      *totalLabel;
    MiddleBar   *middleBar;

    QTableView *currentOrderView;
    CurrentOrderModel *currentOrderModel;
    void currentOrderViewResize();


signals:
    void performAction();
    void updatePrice();
    void addArticle(QString);

public slots:
    void updateTotalLabel();
    void actionPerformed();
    void priceUpdated();
    void articleAdded(QString);

};

//TODO when validate is pressed, we have to select normal price button again.

#endif // SALESVIEW_H


