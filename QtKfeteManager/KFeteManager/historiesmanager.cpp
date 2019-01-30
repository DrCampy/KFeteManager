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
    bool success = true;
    success &= history.transaction();
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

    query.prepare("INSERT INTO CashMoves(Session, amount, note) VALUES(:sess, :amount, :note);");
    query.bindValue(":sess", session.Id);
    //Inserts cash moves
    for(auto it : session.cashMoves){
        query.bindValue(":amount", it.first);
        query.bindValue(":note", it.second);
        success &= query.exec();
    }

    //Inserts account moves
    query.prepare("INSERT INTO AccountMoves(Session, Client, amount, note) "
                  "VALUES(:sess, :client, :amount, :note);");
    query.bindValue(":sess", session.Id);
    for(auto it : session.accountMoves.keys()){
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
