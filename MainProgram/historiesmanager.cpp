#include <QString>
#include <QMap>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlDatabase>

#include <QDebug>

#include "historiesmanager.h"
#include "databasemanager.h"

void HistoriesManager::addSession(Session session){
    //TODO add clients if necessary ??????
    QSqlDatabase history = getDatabase();
    QSqlQuery query(history);
    QSqlQuery insertClients(history);
    QSqlQuery insertArticle(history);
    bool success = true;
    success &= history.transaction();

    //Inserts all metadata of session
    query.prepare("INSERT INTO Sessions(OpenTime, closeTime, openAmount, closeAmount, incomes, jobistShare, jobistWage) "
                  "VALUES (:ot, :ct, :oa, :ca, :i, :js, :jw);");
    query.bindValue(":ot", session.openTime);
    query.bindValue(":ct", session.closeTime);
    query.bindValue(":oa", session.openAmount);
    query.bindValue(":ca", session.closeAmount);
    query.bindValue(":i", session.cashIncome);
    query.bindValue(":js", session.jobistShare);
    query.bindValue(":jw", session.jobistWage);
    success &= query.exec();

    //Inserts cash moves
    query.prepare("INSERT INTO CashMoves(Session, amount, note) VALUES(:sess, :amount, :note);");
    query.bindValue(":sess", session.Id);
    for(auto it : session.cashMoves){
        query.bindValue(":amount", it.first);
        query.bindValue(":note", it.second);
        success &= query.exec();
    }

    //Inserts account moves
    insertClients.prepare("INSERT OR IGNORE INTO Clients(Name) VALUES(:client);");
    query.prepare("INSERT INTO AccountMoves(Session, Client, amount, note) "
                  "VALUES(:sess, :client, :amount, :note);");
    query.bindValue(":sess", session.Id);
    for(auto it : session.accountMoves.keys()){ //TODO Is a multimap. We have to correct that
        insertClients.bindValue(":client", it); //ensures all clients are in db
        success &= insertClients.exec();
        query.bindValue(":client", it);
        query.bindValue(":amount", session.accountMoves.value(it).first);
        query.bindValue(":note", session.accountMoves.value(it).second);
        success &= query.exec();
    }

    //Inserts functions benefits
    query.prepare("INSERT INTO FunctionsBenefits(Session, function, amount) VALUES(:sess, :fun, :amount);");
    query.bindValue(":sess", session.Id);
    for(auto it : session.functionsBenefits.keys()){
        query.bindValue(":fun", it);
        query.bindValue(":amount", session.functionsBenefits.value(it));
        success &= query.exec();
    }

    //Add all items to table items
    insertArticle.prepare("INSERT OR IGNORE INTO Articles(Name) VALUES(:art);");
    query.prepare("INSERT INTO Sales(Session, Article, quantity, price) "
                  "VALUES(:sess, :art, :qtt, :price);");
    query.bindValue(":sess", session.Id);
    query.bindValue(":price", "normal"); //Normal Sales
    for(auto article : session.normalSales.keys()){
        insertArticle.bindValue(":art", article);
        success &= insertArticle.exec();
        query.bindValue(":art", article);
        query.bindValue(":qtt", session.normalSales.value(article));
        success &= query.exec();
    }

    query.bindValue(":price", "reduced"); //reduced sales
    for(auto article : session.reducedSales.keys()){
        insertArticle.bindValue(":art", article);
        success &= insertArticle.exec();
        query.bindValue(":art", article);
        query.bindValue(":qtt", session.reducedSales.value(article));
        success &= query.exec();
    }

    query.bindValue(":price", "free"); //free sales
    for(auto article : session.freeSales.keys()){
        insertArticle.bindValue(":art", article);
        success &= insertArticle.exec();
        query.bindValue(":art", article);
        query.bindValue(":qtt", session.freeSales.value(article));
        success &= query.exec();
    }

    //Adds jobists
    query.prepare("INSERT INTO Jobists(Session, Client) VALUES(:sess, :jobist);");
    query.bindValue(":sess", session.Id);
    for(auto jobist : session.jobists){
        query.bindValue(":jobist", jobist);
        success &= query.exec();
    }

    //Terminate transaction
    if(success){
        history.commit();
    }else{
        history.rollback();
    }
}

bool HistoriesManager::openHistoryDb(){
    QSqlDatabase history = QSqlDatabase::addDatabase("QSQLITE", "history");
    history.setDatabaseName("./data/history.sqlite");
    bool ok = history.open();
    if(!ok){
        qDebug() << "Impossible to open history database.";
        return false;
    }

    QSqlQuery query(history);
    return query.exec("PRAGMA foreign_keys = ON;");
}

void HistoriesManager::closeHistoryDb(){
    if(QSqlDatabase::contains("history")){
        QSqlDatabase history = QSqlDatabase::database("history");
        QSqlQuery query("PRAGMA optimize;", history);
        history.close();
    }
}

void HistoriesManager::createHistoryDb(){
    if(QSqlDatabase::contains("history")){
        QSqlDatabase history = QSqlDatabase::database("history");
        DatabaseManager::executeScript(":/create-history.sql", history);
    }
}

QSqlDatabase HistoriesManager::getDatabase(){
    return QSqlDatabase::database("history");
}
