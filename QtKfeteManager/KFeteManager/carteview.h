#ifndef CARTEVIEW_H
#define CARTEVIEW_H

#include <QObject>
#include <QWidget>
#include <QButtonGroup>

#include "catalog.h"
#include "cartemodel.h"

class CarteView : public QWidget
{
    Q_OBJECT
public:
    explicit CarteView(QWidget *parent = nullptr);
    void assignButton(int id, QString article);
    void setModel(CarteModel *model);
    void updateButton(unsigned int id);

private :
    static const unsigned int NB_MENU_PAGES = 4, GRID_W = 5, GRID_H = 8;
    static const QStringList PAGES_NAMES;
    QButtonGroup *carteButtons;
    CarteModel *model;

public slots:
    void updateView();

};



#endif // CARTEVIEW_H
