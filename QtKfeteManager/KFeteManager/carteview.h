#ifndef CARTEVIEW_H
#define CARTEVIEW_H

#include <QObject>
#include <QWidget>
#include <QButtonGroup>
#include <QWidgetAction>
#include <QToolButton>
#include <QLineEdit>
#include <QColor>

#include "catalog.h"
#include "cartemodel.h"

class CarteManager;
class CarteView;
class Searcher;

class CarteView : public QWidget
{
    Q_OBJECT
public:
    explicit CarteView(QWidget *parent = nullptr, bool inUse = true);
    void assignButton(int id, QString article);
    void setModel(CarteModel *model);
    void updateButton(unsigned int id);

private :
    QButtonGroup *carteButtons;
    CarteModel *model;
    QToolButton *search;
    Searcher *searcher;
    bool inUse; //This variable determines if we see the searchbar and if the buttons can be disabled.

public slots:
    void updateView();

friend CarteManager;
};

class Searcher : public QWidgetAction
{
    Q_OBJECT
public:
    explicit Searcher(const QStringList *list, QWidget *parent = nullptr);
    ~Searcher();
    QWidget *createWidget(QWidget *parent);
    void refreshList(QStringList *list);

private:
    const QStringList *list; //todo rename. Name is ambiguous
    QLineEdit *lineEdit;

public slots:
void returnPressed();

signals:
void articleSearched(QString);

};



#endif // CARTEVIEW_H
