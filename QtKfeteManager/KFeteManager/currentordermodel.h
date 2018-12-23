#ifndef CURRENTORDERMODEL_H
#define CURRENTORDERMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QItemSelectionModel>

#include "catalog.h"

class CurrentOrderModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum Action{plusItem, minusItem, deleteItem};
    enum Price{normal, reduced, free};

private:
    qreal total;
    Catalog *catalog;
    QMap<QString, unsigned int> *itemsCount;
    QStringList *items;
    Price price;
    void updateModel();


public:
    explicit CurrentOrderModel(Catalog *catalog, int rows, QObject *parent=nullptr);
    ~CurrentOrderModel();
    qreal getTotal();

public slots:
    void applyAction(Action action, QItemSelectionModel *selection);
    void addArticle(QString articleName);
    void changePrice(Price price);
    void clear();

signals:
    void updated();

};

#endif // CURRENTORDERMODEL_H
