#ifndef SALESVIEW_H
#define SALESVIEW_H

#include <QWidget>
#include <QString>
#include <QButtonGroup>
#include <QPushButton>
#include <QTableView>
#include "article.h"

class CarteView : public QWidget
{
    Q_OBJECT
public:
    explicit CarteView(QWidget *parent = nullptr);
    explicit CarteView(Catalog *cat, bool hasRights = false, QWidget *parent = nullptr);
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
    static const QStringList PAGES_NAMES; //defined in carte.cpp
    QMap<int, QString> *lookupTable;
    Catalog *cat;
    bool hasRights;

};

class CarteButton : public QPushButton{
    Q_OBJECT
    using QPushButton::QPushButton;
public:
    void changeColor();
    void allowColorChange(bool b = true);

public slots:
    void updateColor(QPalette);

private:
    bool canChangeColor = false;

protected:
    void mousePressEvent(QMouseEvent *e);
};

class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);
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
    QTableView *currentOrderView;
    CarteView *carteView;
    TopBar *topBar;
    //setModel()

public slots:
    void save();
};

const QStringList CarteView::PAGES_NAMES = (QStringList() << "Bières" << "Snacks" << "Softs" << "Divers");



/*
class Commande : public QWidget
{
  Q_OBJECT
public:
  explicit Commande(Catalog *cat, QWidget *parent = 0);
  enum Action{bPlus, bMoins, bSupprimer, bTarifNormal, bTarifReduit, bOffert};
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


