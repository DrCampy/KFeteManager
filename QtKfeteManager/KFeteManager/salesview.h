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

class CarteView;
class TopBar;
class SalesView;
class MiddleBar;


class CarteView : public QWidget
{
    Q_OBJECT
public:
    explicit CarteView(Catalog *cat, QWidget *parent = nullptr);
    void assignButton(int id, QString article);
    bool exportCarte(QString filename) const;
    bool importCarte(QString filename);

signals:
    void clicArticle(QString);

private slots:
    void buttonClicked(int id); //TODO

public slots:

private :
    QButtonGroup *carteButtons;
    static const unsigned int NB_MENU_PAGES = 4, GRID_W = 5, GRID_H = 8;
    static const QStringList PAGES_NAMES;// = (QStringList() << "Bières" << "Snacks" << "Softs" << "Divers");
    QMap<int, QString> *lookupTable;
    Catalog *cat;

};

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

private:
    QPushButton *plusButton;
    QPushButton *minusButton;
    QPushButton *deleteButton;
    QButtonGroup *priceButtonsGroup;
    QPushButton *normalPriceButton;
    QPushButton *reducedPriceButton;
    QPushButton *freePriceButton;

private slots:
    void plusSlot();
    void minusSlot();
    void deleteSlot();
    void priceSlot(int id);

signals:
    void buttonPressed(CurrentOrderModel::Action);
    void priceChanged(CurrentOrderModel::Price);
};

/*
 * Interface principale avec la carte, les comptes,...
 */
class SalesView : public QWidget
{
    Q_OBJECT
public:
    explicit SalesView(QWidget *parent = nullptr );

private:

    CarteView *carteView;
    TopBar *topBar;
    Catalog *catalog;
    QLabel *totalLabel;
    MiddleBar *middleBar;

    QTableView *currentOrderView;
    CurrentOrderModel *currentOrderModel;

signals:
    void performAction(CurrentOrderModel::Action, QItemSelectionModel *);

public slots:
    void updateTotalLabel();
    void actionPerformed(CurrentOrderModel::Action action);

};

//TODO when validate is pressed, we have to select normal price button again.

/*
class Commande : public QWidget
{
  Q_OBJECT
public:
  explicit Commande(Catalog *cat, QWidget *parent = 0);
  enum Column{Quantite = 0, Article=1, SousTotal=2};
  enum Tarif{Normal, Reduit, Offert};

public slots:
  void receiveAction(int);
  void addArticle(QString);

private:
  void removeArticle(QString);
  void recalculerTotaux();
  void updateTotal();

  QLabel *totLabel, *tarifLabel;
  Catalog *cat;
  QMap<QString, QTreeWidgetItem*> *artList;
  QTreeWidget *treeWidget;
  Tarif tarif;

};*/
#endif // SALESVIEW_H


