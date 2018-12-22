#ifndef CARTEMODEL_H
#define CARTEMODEL_H

#include <QObject>
#include <QPalette>
#include <QString>
#include <QMap>

#include "catalog.h"

class ButtonDataWrapper;

class CarteModel : public QObject
{
    Q_OBJECT
public:
    explicit CarteModel(QObject *parent = nullptr);
    ButtonDataWrapper getButton(unsigned int id);

private:
    bool exportCarte(QString filename) const;
    bool importCarte(QString filename);

    Catalog *catalog;

    //TODO implement
    bool updateCarte();

    QMap<unsigned long int, ButtonDataWrapper> table;


signals:
    void modelUpdated();

public slots:

};

class ButtonDataWrapper
{
public:
    explicit ButtonDataWrapper(QString name, QColor backgroundColor, QColor textColor);
    QString getName() const;
    QColor getBackgroundColor() const;
    QColor getTextColor() const;
private:
    QString name;
    QColor backgroundColor, textColor;

};

#endif // CARTEMODEL_H
