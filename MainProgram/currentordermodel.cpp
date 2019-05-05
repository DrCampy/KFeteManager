#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QItemSelectionModel>
#include <QVariant>
#include <QLocale>

#include "currentordermodel.h"

CurrentOrderModel::CurrentOrderModel(int rows, QObject *parent)
    : QStandardItemModel(rows, 3, parent)
{
    order = new Order();
    QStringList headers;
    headers << tr("Quantité") << tr("Article") << tr("Sous-total");
    this->setHorizontalHeaderLabels(headers);

}

CurrentOrderModel::~CurrentOrderModel(){
    delete order;
}

void CurrentOrderModel::updateModel(){
    //gets the conten of the order.
    auto orderContent = order->getContent();
    int previousRowCount = this->rowCount();
    if( previousRowCount != orderContent->size())
        this->setRowCount(orderContent->size());
    int i = 0;
    for(auto it : orderContent->keys()){
        QString itemName = it;
        uint itemQuantity = orderContent->value(it).first;
        qreal itemSubTotal = orderContent->value(it).second;
        if(this->item(i)){
            this->item(i, 0)->setText(QString::number(itemQuantity));
            this->item(i, 1)->setText(itemName);
            this->item(i, 2)->setText(QString::number(itemSubTotal, 'f', 2));
        }else{
            this->setItem(i, 0, new QStandardItem(QString::number(itemQuantity)));
            this->setItem(i, 1, new QStandardItem(itemName));
            this->setItem(i, 2, new QStandardItem(QString::number(itemSubTotal, 'f', 2)));
        }
        i++;
    }
    emit updated();
}


void CurrentOrderModel::setActiveSelection(QItemSelectionModel *selection){
    this->activeSelection = selection;
}

qreal CurrentOrderModel::getTotal(){
    return order->getTotal();
}

void CurrentOrderModel::addArticle(QString articleName){
    //creates the article
    Article a(articleName);

    //adds the article
    order->addArticle(a);

    //update the model
    updateModel();
}


//Does not update the whole model
//to allow the text to stay in red until next action
void CurrentOrderModel::clear(){
    delete order;
    order = new Order();
}

Order CurrentOrderModel::getOrder(){
    return *order;
}

void CurrentOrderModel::updatePrice(Order::Price price){
    order->setPrice(price);
    updateModel();
}

void CurrentOrderModel::applyAction(Action action){
    QModelIndexList selectedRows = activeSelection->selectedRows(1);

    for(auto it = selectedRows.begin(); it < selectedRows.end(); it++){
        QString selectedArticle = this->data(*it, Qt::DisplayRole).toString();
        Article a(selectedArticle);
        if(action == plusItem){
            //increments the quantity of that item in the order
            order->addArticle(a);
        }else if(action == minusItem){
            //Decrement the quantity of that item in the order
            order->removeArticle(a);
        }else if(action == deleteItem){
            //deletes the article from the order
            order->deleteArticle(a);
        }
    }
    updateModel();
}
