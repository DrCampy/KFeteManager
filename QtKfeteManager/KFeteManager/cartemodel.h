#ifndef CARTEMODEL_H
#define CARTEMODEL_H

#include <QObject>
#include <QPalette>
#include <QString>
#include <QMap>

#include "catalog.h"

class CarteModel;
class ButtonDataWrapper;

class CarteModel : public QObject
{
    Q_OBJECT
public:
    explicit CarteModel(QString filename, QObject *parent = nullptr);
    ~CarteModel();
    const ButtonDataWrapper *getButton(unsigned int id) const;
    bool addEntry(unsigned int id, ButtonDataWrapper data);
    QStringList *getArticlesList();

private:
    static const unsigned int CARTE_VERSION=0;
    bool exportCarte() const;
    bool importCarte();
    QString filename;
    QMap<unsigned int, ButtonDataWrapper> table;
    QMap<QString, unsigned int> nbEntries;
    QStringList *articles;


signals:
    void modelUpdated();
    void articleClicked(QString);

public slots:
    void buttonClicked(int id);

};

class ButtonDataWrapper
{
public:
    explicit ButtonDataWrapper(QString name = "", QColor backgroundColor = "", QColor textColor = "");
    QString getName() const;
    QColor getBackgroundColor() const;
    QColor getTextColor() const;

    ButtonDataWrapper &operator=(const ButtonDataWrapper &a);
    bool operator==(const ButtonDataWrapper &a) const;
private:
    QString name;
    QColor backgroundColor, textColor;

};

#endif // CARTEMODEL_H
