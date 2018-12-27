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
    Action actionToPerform;
    QItemSelectionModel *activeSelection;
    void updateModel();

public:
    explicit CurrentOrderModel(Catalog *catalog, int rows, QObject *parent=nullptr);
    ~CurrentOrderModel();
    void setPrice(Price price);
    void setActiveSelection(QItemSelectionModel *selection);
    void setActionToPerform(Action action);
    qreal getTotal();

public slots:
    void applyAction();
    void addArticle(QString articleName);
    void updatePrice();
    void clear();

signals:
    void updated();
};



#endif // CURRENTORDERMODEL_H
