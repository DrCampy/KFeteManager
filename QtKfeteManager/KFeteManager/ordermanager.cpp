#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlRecord>
#include <QDateTime>
#include <QHeaderView>

#include "ordermanager.h"
#include "databasemanager.h"
#include "customwidgets.h"

OrderManager::OrderManager(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *mainLayout     = new QHBoxLayout(this);
    QVBoxLayout *rightLayout    = new QVBoxLayout();

    deleteButton    = new QPushButton(tr("Supprimer"), this);
    back            = new QPushButton(tr("Retour"), this);
    orderModel      = new QSqlQueryModel(this);
    textEdit        = new QPlainTextEdit(this);
    orderView       = new QTableView(this);

    refreshList();

    orderView->setModel(orderModel);
    orderView->verticalHeader()->hide();
    orderView->hideColumn(0);
    orderView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    orderView->setSelectionBehavior(QAbstractItemView::SelectRows);
    orderView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    //orderView->horizontalHeader()->resizeSection(0, 20);
    //orderView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    orderView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    orderModel->setHeaderData(1, Qt::Horizontal, tr("N°"));
    orderModel->setHeaderData(2, Qt::Horizontal, tr("Heure de commande"));

    textEdit->setReadOnly(true);

    orderView->setItemDelegateForColumn(2, new DateDelegate(this));

    rightLayout->addWidget(textEdit);
    rightLayout->addWidget(deleteButton);
    rightLayout->addWidget(back);

    mainLayout->addWidget(orderView);
    mainLayout->addLayout(rightLayout);

    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteOrder()));
    connect(back, SIGNAL(clicked()), this, SIGNAL(finished()));
    connect(orderView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(writeOrderDetails()));
}

void OrderManager::deleteOrder(){
    QString text;
    QSqlQuery query;
    auto selectedRows = orderView->selectionModel()->selectedRows();
    if(selectedRows.size() == 1){
        //qlonglong id = orderModel->data(selectedRows.at(0));
        qlonglong id = selectedRows.at(0).data().toLongLong();
        DatabaseManager::cancelOrder(id);
        refreshList();
    }
}

void OrderManager::refreshList(){
    orderModel->setQuery("SELECT id, lineNumber, processTime FROM Transactions "
                         "WHERE sessionTime = (SELECT value FROM Config WHERE field='CurrentSession');");
    orderModel->sort(1, Qt::AscendingOrder);
}

void OrderManager::writeOrderDetails(){
    QString text;
    QSqlQuery query;
    qlonglong id = orderView->selectionModel()->selectedRows().at(0).data().toLongLong();

    query.prepare("SELECT total FROM Transactions WHERE id=:id;");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    qreal total = query.value(0).toDouble();

    //Determines if the order is a deposit or an actual order
    query.prepare("SELECT * FROM IsOrder WHERE id=:id;");
    query.bindValue(":id", id);
    query.exec();
    if(query.next()){
        //It is an order
        //Get client (will be empty QVariant if no client)
        query.prepare("SELECT client FROM OrderClient WHERE Id = :id;");
        query.bindValue(":id", id);
        query.exec();
        if(query.next()){
            text.append(tr("Sur le compte de : ") + query.value(0).toString() + "\n");
        }
        //Get order Content
        query.prepare("SELECT article, quantity FROM OrderContent WHERE Id = :id;");
        query.bindValue(":id", id);
        query.exec();
        text.append(tr("Contenu : ") + "\n");
        while(query.next()){
            text.append(QString::number(query.value(1).toUInt()) + " x " + query.value(0).toString() + "\n");
        }
        //TODO Display if order is reduced price or other.
        text.append("\n" + tr("Total : ") + QString::number(total, 'f', 2));
    }else{
        //It is a deposit
        query.prepare("SELECT client FROM CashMoves WHERE Id = :id;");
        query.bindValue(":id", id);
        query.exec();
        query.next();
        QVariant client = query.value(0);
        if(total < 0){
            text.append(tr("Retrait "));
        }else{
            text.append(tr("Dépôt "));
        }
        if(Client(client.toString()).isNull()){
            text.append(tr("en caisse :") + "\n");
        }else{
            text.append(tr("sur un compte : ") + "\n");
            text.append(tr("Client : ") + client.toString() + "\n");
        }
        text.append(tr("Montant : ") + QString::number(qAbs(total)) + "\n");
    }
    textEdit->setPlainText(text);
}

void OrderManager::showEvent(QShowEvent *event){
    QWidget::showEvent(event);
    refreshList();
}
