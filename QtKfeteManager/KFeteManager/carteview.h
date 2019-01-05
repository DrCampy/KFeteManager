#ifndef CARTEVIEW_H
#define CARTEVIEW_H

#include <QObject>
#include <QWidget>
#include <QButtonGroup>
#include <QWidgetAction>
#include <QToolButton>
#include <QLineEdit>

#include "catalog.h"
#include "cartemodel.h"

class CarteView : public QWidget
{
    Q_OBJECT
public:
    explicit CarteView(bool searchBar, QWidget *parent = nullptr);
    void assignButton(int id, QString article);
    void setModel(CarteModel *model);
    void updateButton(unsigned int id);

private :
    static const unsigned int NB_MENU_PAGES = 4, GRID_W = 5, GRID_H = 8;
    static const QStringList PAGES_NAMES;
    QButtonGroup *carteButtons;
    CarteModel *model;
    QToolButton *search;
    bool searchBar;

public slots:
    void updateView();

};

class Searcher : public QWidgetAction
{
    Q_OBJECT
public:
    explicit Searcher(const QStringList *list, QWidget *parent = nullptr);
    QWidget *createWidget(QWidget *parent);

private:
    const QStringList *list;
    QLineEdit *lineEdit;

public slots:
void returnPressed();

signals:
void articleSearched(QString);

};



#endif // CARTEVIEW_H
