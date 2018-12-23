#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QItemSelectionModel>
#include <QVariant>
#include <QLocale>

#include "currentordermodel.h"

CurrentOrderModel::CurrentOrderModel(Catalog *catalog, int rows, QObject *parent)
    : QStandardItemModel(rows, 3, parent)
{
    //Assign catalog
    this->catalog = catalog;
    price = normal;
    total = 0;
    itemsCount = new QMap<QString, unsigned int>();
    items = new QStringList();

    QStringList headers;
    headers << tr("Quantité") << tr("Article") << tr("Sous-total");
    this->setHorizontalHeaderLabels(headers);
}

CurrentOrderModel::~CurrentOrderModel(){
    delete itemsCount;
    delete items;
}

void CurrentOrderModel::updateModel(){
    this->setRowCount(items->size());
    total = 0;
    for(auto it = items->begin(); it < items->end(); it++){
        QString itemName = *it;
        unsigned int itemQuantity= itemsCount->value(itemName);
        qreal subTotal= itemQuantity;

        if(price == normal){
             subTotal *= catalog->getArticle(itemName).getPrice();
        }else if(price == reduced){
             subTotal *= catalog->getArticle(itemName).getReducedPrice();
        }else{
            subTotal *= 0;
        }

        total += subTotal;

        this->setItem(it-items->begin(), 0, new QStandardItem(QString(itemQuantity)));
        this->setItem(it-items->begin(), 1, new QStandardItem(itemName));
        this->setItem(it-items->begin(), 2, new QStandardItem(QString::number(subTotal, 'f', 2)));
    }
}

void CurrentOrderModel::setPrice(Price price){
    this->price = price;
}

void CurrentOrderModel::setActiveSelection(QItemSelectionModel *selection){
    this->activeSelection = selection;
}

void CurrentOrderModel::setActionToPerform(Action action){
    this->actionToPerform = action;
}

qreal CurrentOrderModel::getTotal(){
    return this->total;
}

void CurrentOrderModel::applyAction(){

    QModelIndexList selectedRows = activeSelection->selectedRows(1);

    for(auto it = selectedRows.begin(); it < selectedRows.end(); it++){
        QString selectedArticle = this->data(*it, Qt::DisplayRole).toString();
        if(actionToPerform == plusItem){
            //Increment quantity of that item
            itemsCount->insert(selectedArticle, itemsCount->value(selectedArticle));
        }else if(actionToPerform == minusItem){
            //Decrement the quantity of that item
            long int newItemCount = static_cast<long int>(itemsCount->value(selectedArticle)-1);
            if(newItemCount >= 0){
                //If we still have that item to display, updates its quantity
                itemsCount->insert(selectedArticle, static_cast<unsigned int>(newItemCount));
            }else{
                //If we are below 1 item we delete it
                itemsCount->remove(selectedArticle);
                items->removeAll(selectedArticle);
            }
        }else if(actionToPerform == deleteItem){
            itemsCount->remove(selectedArticle);
            items->removeAll(selectedArticle);
        }
    }
    updateModel();

}

void CurrentOrderModel::addArticle(QString articleName){
    //If we already have that item, increase it's quantity
    if(items->contains(articleName, Qt::CaseSensitive)){
        itemsCount->insert(articleName, itemsCount->value(articleName)+1);
    }else{
        //If we do not have that item yet, adds it to the current order
        items->append(articleName);
        itemsCount->insert(articleName, 1);
        items->sort();
    }
    updateModel();
}

void CurrentOrderModel::updatePrice(){
    updateModel();
}

void CurrentOrderModel::clear(){
    this->items->empty();
    this->itemsCount->empty();
    updateModel();
}


