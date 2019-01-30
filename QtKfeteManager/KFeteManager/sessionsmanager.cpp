#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QSqlQueryModel>
#include <QString>
#include <QDateTime>
#include <QLocale>
#include <QLineEdit>

#include "sessionsmanager.h"
#include "customwidgets.h"
#include "databasemanager.h"
#include "historiesmanager.h"

SessionsManager::SessionsManager(QWidget *parent) : QWidget(parent)
{
    sessionSelector     = new QComboBox(this);
    sessionDetails      = new QPlainTextEdit(this);
    backButton          = new QPushButton(tr("Retour"), this);
    validate1Button     = new QPushButton(tr("Valider automatiquement"), this);
    validate2Button     = new QPushButton(tr("Valider Manuellement"), this);
    validate3Button     = new QPushButton(tr("Valider sans payer"), this);
    sessionsModel       = new QSqlQueryModel();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttons    = new QHBoxLayout();

    sessionSelector->setItemDelegate(new DateDelegate(sessionSelector));
    sessionSelector->setModel(sessionsModel);
    sessionSelector->setCurrentIndex(-1); //selects nothing.

    sessionDetails->setReadOnly(true);

    buttons->addWidget(validate1Button);
    buttons->addWidget(validate2Button);
    buttons->addWidget(validate3Button);
    buttons->addWidget(backButton);

    mainLayout->addWidget(sessionSelector);
    mainLayout->addWidget(sessionDetails);
    mainLayout->addLayout(buttons);

    //Connects
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(finished()));
    connect(sessionSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(writeDetails()));
    refresh();
}

SessionsManager::~SessionsManager(){
    delete sessionsModel;
}

void SessionsManager::refresh(){
    sessionsModel->setQuery("SELECT openingTime FROM saleSessions WHERE state='closed' ORDER BY openingTime ASC;");
}

void Session::clear(){
    Id = 0;
    openTime = QDateTime();
    closeTime = QDateTime();
    jobists.clear();
    openAmount = QVariant();
    closeAmount = QVariant();
    cashIncome = 0;
    cashMoves.clear();
    accountMoves.clear();
    normalSales.clear();
    reducedSales.clear();
    freeSales.clear();
    functionsBenefits.clear();
    jobistShare = 0;
    jobistWage = 0;
}

void SessionsManager::loadDatas(){
    QSqlQuery query;
    session.clear();
    //Loads session time
    session.Id = this->sessionSelector->currentData(Qt::EditRole).toLongLong();
    session.openTime.setSecsSinceEpoch(session.Id);

    //loads jobists
    query.prepare("SELECT name FROM heldSession WHERE sessionTime=:id ORDER BY name ASC;");
    query.bindValue(":id", session.Id);
    query.exec();
    while(query.next()){
        session.jobists << query.value(0).toString();
    }

    //Loads count before and after sales
    query.prepare("SELECT openAmount, closeAmount, closingTime FROM SaleSessions WHERE OpeningTime = :id;");
    query.bindValue(":id", session.Id);
    query.exec();
    if(query.first()){
        session.openAmount = query.value(0).toDouble();
        session.closeAmount = query.value(1).toDouble();
        session.closeTime.setSecsSinceEpoch(query.value(2).toLongLong());
    }

    //Tries to load the count at the end of the last session
    query.prepare("SELECT closeAmount FROM SaleSessions WHERE OpeningTime < :id ORDER BY OpeningTime DESC LIMIT 1;");
    query.bindValue(":id", session.Id);
    query.exec();
    if(query.first() && !query.value(0).isNull()){
        countLastSession = query.value(0).toDouble();
    }

    //load sold items and jobistShare
    //0 = article, 1 = nb_article, 2 = jobShare per article, 3 = price bought, 4 = price sold
    //5 = function name

    //Normal price sales
    query.prepare("SELECT OrderContent.article, SUM(OrderContent.quantity) AS tot_qtt, "
                  "Articles.jShare, Articles.bPrice, Articles.sellPrice AS soldPrice, "
                  "Functions.name AS Function "
                  "FROM Transactions "
                  "INNER JOIN OrderContent ON Transactions.Id = OrderContent.Id "
                  "INNER JOIN Articles ON OrderContent.article = Articles.name "
                  "INNER JOIN Functions ON Articles.function = Functions.Id "
                  "WHERE Transactions.sessionTime = :id AND OrderContent.Id IN (SELECT Id FROM IsOrder WHERE price = 'normal') "
                  "GROUP BY OrderContent.article "
                  "ORDER BY tot_qtt DESC;");
    query.bindValue(":id", session.Id);
    query.exec();
    while(query.next()){
        const QString name = query.value(0).toString();
        const uint nb = query.value(1).toUInt();
        const qreal artJShare = query.value(2).toDouble();
        const qreal boughtPrice = query.value(3).toDouble();
        const qreal soldPrice = query.value(4).toDouble();
        const QString function = query.value(5).toString();

        //Register sale
        session.normalSales.insert(name, nb);

        //Computes jobist share
        session.jobistShare += (artJShare * nb);

        //Computes function benefits
        session.functionsBenefits[function] += nb*(soldPrice - boughtPrice - artJShare);
    }

    //Reduced price sales
    query.prepare("SELECT OrderContent.article, SUM(OrderContent.quantity) AS tot_qtt, "
                  "Articles.jShare, Articles.bPrice, Articles.reducedPrice AS soldPrice, "
                  "Functions.name AS Function "
                  "FROM Transactions "
                  "INNER JOIN OrderContent ON Transactions.Id = OrderContent.Id "
                  "INNER JOIN Articles ON OrderContent.article = Articles.name "
                  "INNER JOIN Functions ON Articles.function = Functions.Id "
                  "WHERE Transactions.sessionTime = :id AND OrderContent.Id IN (SELECT Id FROM IsOrder WHERE price = 'reduced') "
                  "GROUP BY OrderContent.article "
                  "ORDER BY tot_qtt DESC;");
    query.bindValue(":id", session.Id);
    query.exec();
    while(query.next()){
        const QString name = query.value(0).toString();
        const uint nb = query.value(1).toUInt();
        const qreal artJShare = query.value(2).toDouble();
        const qreal boughtPrice = query.value(3).toDouble();
        const qreal soldPrice = query.value(4).toDouble();
        const QString function = query.value(5).toString();

        //Register sale
        session.reducedSales.insert(name, nb);

        //Computes jobist share
        session.jobistShare += (artJShare * nb);

        //Computes function benefits
        session.functionsBenefits[function] += nb*(soldPrice - boughtPrice - artJShare);
    }

    //free sales
    query.prepare("SELECT OrderContent.article, SUM(OrderContent.quantity) AS tot_qtt, "
                  "Articles.bPrice, Functions.name AS Function "
                  "FROM Transactions "
                  "INNER JOIN OrderContent ON Transactions.Id = OrderContent.Id "
                  "INNER JOIN Articles ON OrderContent.article = Articles.name "
                  "INNER JOIN Functions ON Articles.function = Functions.Id "
                  "WHERE Transactions.sessionTime = :id AND OrderContent.Id IN (SELECT Id FROM IsOrder WHERE price = 'free') "
                  "GROUP BY OrderContent.article "
                  "ORDER BY tot_qtt DESC;");
    query.bindValue(":id", session.Id);
    query.exec();
    while(query.next()){
        const QString name = query.value(0).toString();
        const uint nb = query.value(1).toUInt();
        const qreal boughtPrice = query.value(2).toDouble();
        const QString function = query.value(3).toString();

        //Register sale
        session.freeSales.insert(name, nb);

        //No jobist share

        //Computes function benefits
        session.functionsBenefits[function] -= nb*(boughtPrice);
    }

    //load cash moves
    query.prepare("SELECT CashMoves.client, Transactions.total, CashMoves.note "
                  "FROM Transactions "
                  "INNER JOIN CashMoves ON Transactions.Id = CashMoves.Id "
                  "WHERE Transactions.sessionTime = :id;");
    query.bindValue(":id", session.Id);
    query.exec();
    while(query.next()){
        if(query.value(0).isNull() || query.value(0).toString() == ""){
            //(cash register)
            session.cashMoves.append(QPair<qreal, QString>(query.value(1).toDouble(), query.value(2).toString()));
        }else{
            //(accounts)
            session.accountMoves.insert(query.value(0).toString(), QPair<qreal, QString>(query.value(1).toDouble(), query.value(2).toString()));
        }
    }

    //load total
    query.prepare("SELECT SUM(transactions.total) AS total "
                  "FROM Transactions "
                  "WHERE "
                  "Transactions.sessionTime = :id "
                  "AND "
                  "Transactions.Id "
                  "IN "
                  "(SELECT Id "
                  " FROM IsOrder "
                  " WHERE Id NOT IN (SELECT Id FROM OrderClient) "
                  " UNION "
                  " SELECT Id FROM CashMoves); ");
    query.bindValue(":id",session.Id);
    query.exec();
    if(query.first()){
        session.cashIncome = query.value(0).toDouble();
    }

    query.exec("SELECT value FROM Config where field = 'MinJShare';");
    if(query.first() && !query.value(0).isNull()){
        minJShare = query.value(0).toDouble();
    }
}

void SessionsManager::writeDetails(){
    loadDatas();
    text.clear();
    text.reserve(1500);
    QLocale locale;

    //Builds the text summary
    //Header
    text += "<h1><p>" + tr("Session de vente du ") + locale.toString(session.openTime) + "</p></h1>";
    if(!session.jobists.isEmpty()){
        text += "<i><p>" + tr("Session fermée le ") + locale.toString(session.closeTime) + tr(" et tenue par :") + "<br>";
        for(auto it : session.jobists){
            text += it + "<br>"; //Adds jobists
        }
        text += "</p></i>";
    }else{
        text += "<i><p>" + tr("Session fermée le ") + locale.toString(session.closeTime) + ".</i></p>";
    }


    //Summary of the state of the cash register
    text += "<h2>" + tr("État de caisse :") + "</h2>";
    if(countLastSession > 0){
        text += "<p>" + tr("À la fin de l'éxercice précédent : ") + locale.toCurrencyString(countLastSession);
    }
    text += "<p>" + tr("Au début : ") + (session.openAmount.isNull()?"<em>"+tr("Non comptée")+"</em>":locale.toCurrencyString(session.openAmount.toDouble())) + "</p>";
    text += "<p>" + tr("Total des recettes : ") + locale.toCurrencyString(session.cashIncome) + "</p>";
    text += "<p>" + tr("À la fin : ") + (session.closeAmount.isNull()?"<em>"+tr("Non comptée")+"</em>":locale.toCurrencyString(session.closeAmount.toDouble())) + "</p>";
    if(!session.closeAmount.isNull()){
        text += "<p>" + tr("Solde final théorique : ") + locale.toCurrencyString(session.openAmount.toDouble() + session.cashIncome) + "</p>";
    }

    //Cash moves

    text += "<h2>" + tr("Mouvements en caisse :") + "</h2>";
    if(session.cashMoves.isEmpty()){
        text += "<em>" + tr("Pas de mouvements.") + "</em>";
    }else{
        text += "<ul>";
        for(auto it : session.cashMoves){
            text += "<li>";
            if(it.first < 0){
                text += tr("Retrait de ") + locale.toCurrencyString(-it.first) + " en caisse : " + it.second;
            }else{
                text += tr("Dépôt de ") + locale.toCurrencyString(it.first) + " en caisse : " + it.second;
            }
            text += "</li>";
        }
        text += "</ul>";
    }

    text += "<h2>" + tr("Mouvements sur les comptes :") + "</h2>";
    if(session.accountMoves.isEmpty()){
        text += "<em>" + tr("Pas de mouvements.") + "</em>";
    }else{
        for(auto client : session.accountMoves.uniqueKeys()){
            text += "<li>" + tr("Sur le compte de : ") + client + "<ul>";
            for(auto moves : session.accountMoves.values(client)){
                text += "<li>";
                if(moves.first < 0){
                    text += tr("Retrait de ") + locale.toCurrencyString(-moves.first) + " : " + moves.second;
                }else{
                    text += tr("Dépôt de ") + locale.toCurrencyString(moves.first) + " : " + moves.second;
                }
                text += "</li>";
            }
            text += "</ul>";
        }
    }

    //Normal price sales
    text += "<h2>" + tr("Ventes au tarif normal :") + "</h2>";
    text += "<ul>";
    for(auto item : session.normalSales.keys()){
        text += "<li>" + item + " x " + QString::number(session.normalSales.value(item)) + "</li>";
    }
    text += "</ul>";

    //Reduced sales
    if(!session.reducedSales.isEmpty()){
        text += "<h2>" + tr("Ventes au tarif réduit :") + "</h2>";
        text += "<ul>";
        for(auto item : session.reducedSales.keys()){
            text += "<li>" + item + " x " + QString::number(session.reducedSales.value(item)) + "</li>";
        }
        text += "</ul>";
    }

    //Free sales
    if(!session.freeSales.isEmpty()){
        text += "<h2>" + tr("Ventes offertes :") + "</h2>";
        text += "<ul>";
        for(auto item : session.freeSales.keys()){
            text += "<li>" + item + " x " + QString::number(session.freeSales.value(item)) + "</li>";
        }
        text += "</ul>";
    }

    //Functions
    text += "<h2>" + tr("Bénéfices des différentes fonctions :") + "</h2>";
    text += "<ul>";
    for(auto function : session.functionsBenefits.keys()){
        text += "<li>" + function + tr(" : ") + locale.toCurrencyString(session.functionsBenefits.value(function)) + "</li>";
    }
    text += "</ul>";

    //Jobists share
    text += "<h2>" + tr("Part des jobistes : ") + "</h2>";
    text += "<p>" + locale.toCurrencyString(session.jobistShare);
    if(session.jobistShare < minJShare){
        text += tr(" (part jobiste minimale : ") + locale.toCurrencyString(minJShare) + ")";
    }
    text += "</p>";


    sessionDetails->clear();
    sessionDetails->appendHtml(text);
}

void SessionsManager::validateAuto(){
    //Pay jobists

    //Wage in hundredth
    uint share= static_cast<uint>(((session.jobistShare > minJShare)?session.jobistShare:minJShare)*100);
    share /= static_cast<uint>(session.jobists.size());
    qreal wage = share/100;
    session.jobistWage = wage*session.jobists.size();
    for(auto jobist : session.jobists){
        payJobist(jobist, wage/session.jobists.size());
    }

    //Save datas
    saveData();

    //Delete session
    deleteSession(session.Id);
}

void SessionsManager::validateManually(){

}

void SessionsManager::validateNoPay(){

}

void SessionsManager::saveData(){
    //Save session
    HistoriesManager::addSession(this->session);

    QSqlDatabase historyDb = HistoriesManager::getDatabase();
    //Save orders
    //Get clients that ordered something
    QSqlQuery mainClientQuery;
    QSqlQuery mainItemsQuery;
    QSqlQuery histClientQuery(historyDb);

    mainItemsQuery.prepare("SELECT OrderContent.article, SUM(OrderContent.quantity) AS tot_qtt "
                           "FROM Transactions "
                           "INNER JOIN OrderContent ON Transactions.Id = OrderContent.Id "
                           "INNER JOIN Articles ON OrderContent.article = Articles.name "
                           "INNER JOIN OrderClient ON Transactions.Id = OrderClient.Id "
                           "WHERE Transactions.sessionTime = :id AND OrderClient.client = :client "
                           "GROUP BY OrderContent.article "
                           "ORDER BY tot_qtt DESC");
    mainItemsQuery.bindValue(":id", session.Id);
    mainClientQuery.prepare("SELECT DISTINCT client FROM OrderClient WHERE Id IN (SELECT Id FROM Transactions WHERE sessionTime = :id);");
    mainClientQuery.bindValue(":id", session.Id);
    mainClientQuery.exec();

    //Starts a transaction with history db
    historyDb.transaction();
    bool success = true;

    //Adds/update history entries for each client
    while(mainClientQuery.next()){
        //ensure client is in db
        QString client = mainClientQuery.value(0).toString();
        histClientQuery.prepare("INSERT OR IGNORE INTO Clients VALUES(:client);");
        histClientQuery.bindValue(":client", client);
        success &= histClientQuery.exec();

        //get items ordered by that client of that client
        mainItemsQuery.bindValue(":client", client);
        success &= mainItemsQuery.exec();

        //Ensures item is in table
        QSqlQuery itemInTable(historyDb);
        itemInTable.prepare("INSERT OR IGNORE INTO ClientHistory(Client, Article) VALUES(:client, :article);");
        itemInTable.bindValue(":client", client);
        //Increment item
        QSqlQuery incItemCount(historyDb);
        incItemCount.prepare("UPDATE ClientHistory SET quantity = quantity + :quantity WHERE Article = :article AND Client = :client);");
        incItemCount.bindValue(":client", client);

        while(mainItemsQuery.next()){
            QString article = mainItemsQuery.value(0).toString();
            uint quantity = mainItemsQuery.value(1).toUInt();

            itemInTable.bindValue(":article", article);
            success &= itemInTable.exec();
            incItemCount.bindValue(":article", article);
            incItemCount.bindValue(":quantity", quantity);
            success &= incItemCount.exec();
        }
    }

    if(success){
        historyDb.commit();
    }else{
        historyDb.rollback();
    }
}

void SessionsManager::deleteSession(qlonglong id){
    QSqlQuery query;
    query.prepare("DELETE FROM SalesSessions WHERE OpeningTime = :id;");
    query.bindValue(":id", id);
    query.exec();
}

void SessionsManager::payJobist(QString jobist, qreal wage){

}

