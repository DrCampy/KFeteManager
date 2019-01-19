#ifndef CURRENTORDERMODEL_H
#define CURRENTORDERMODEL_H

#include <QStandardItemModel>
#include <QString>
#include <QItemSelectionModel>

#include "order.h"

class CurrentOrderModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum Action{plusItem, minusItem, deleteItem};
    explicit CurrentOrderModel(int rows, QObject *parent=nullptr);
    ~CurrentOrderModel();
    qreal getTotal();
    Order getOrder();
    void clear();

private:
    Order *order;
    QItemSelectionModel *activeSelection;
    void updateModel();

public slots:
    void setActiveSelection(QItemSelectionModel *selection);
    void addArticle(QString articleName);
    void updatePrice(Order::Price price);
    void applyAction(CurrentOrderModel::Action action);

signals:
    void updated();

};

#endif // CURRENTORDERMODEL_H
