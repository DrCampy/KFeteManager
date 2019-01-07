#ifndef CURRENTORDERMODEL_H
#define CURRENTORDERMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QItemSelectionModel>

#include "catalog.h"
#include "order.h"

//TODO refer to Order in order.h
class CurrentOrderModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum Action{plusItem, minusItem, deleteItem};

private:
    Order *order;
    //qreal total;
    //QMap<QString, unsigned int> *itemsCount;
    //QStringList *items;
    //Order::Price price;
    Action actionToPerform;
    QItemSelectionModel *activeSelection;
    void updateModel();

public:
    explicit CurrentOrderModel(int rows, QObject *parent=nullptr);
    ~CurrentOrderModel();
    void setPrice(Order::Price price);
    void setActiveSelection(QItemSelectionModel *selection);
    void setActionToPerform(Action action);
    qreal getTotal();
    Order getOrder();
    void clear();

public slots:
    void applyAction();
    void addArticle(QString articleName);
    void updatePrice();

signals:
    void updated();
};



#endif // CURRENTORDERMODEL_H
