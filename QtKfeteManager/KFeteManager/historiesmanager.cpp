#include <QString>
#include <QMap>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlDatabase>

#include <QDebug>

#include "historiesmanager.h"
#include "databasemanager.h"

void HistoriesManager::addSession(Session session){
    QSqlDatabase history = QSqlDatabase::database("history");
    QSqlQuery query(history);

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
