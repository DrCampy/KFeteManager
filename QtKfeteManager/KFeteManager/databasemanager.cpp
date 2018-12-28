#include <QObject>
#include <QList>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

#include "databasemanager.h"

QStringList DatabaseManager::tables =
        QStringList() << "Articles" << "Functions" << "Clients" << "SaleSessions"
                      <<"HeldSession" <<"FunctionBenefits" << "OrderDetails"
                     << "OrderContent" << "OrderClient";

QStringList DatabaseManager::articlesFields =
        QStringList() << "Name" << "sellPrice" << "jShare"
                      << "bPrice" << "reducedPrice" << "function";

QStringList DatabaseManager::functionsFields =
        QStringList() << "Name" << "Id";

QStringList DatabaseManager::clientsFields =
        QStringList() << "Name" << "phone" << "address" << "email"
                      << "negLimit" << "isJobist" << "balance";

QStringList DatabaseManager::saleSessionsFields =
        QStringList() << "OpeningTime" << "closingTime" << "openAmount"
                      << "closeAmount" << "soldAmount";

QStringList DatabaseManager::heldSessionFields =
        QStringList() << "Name" << "SessionTime";

QStringList DatabaseManager::functionBenefitsFields =
        QStringList() << "FctId" << "SessionTime" << "amount";

QStringList DatabaseManager::OrderDetailsFields =
        QStringList() << "OrderID" << "sessionTime" << "orderNumber";

QStringList DatabaseManager::OrderContentFields =
        QStringList() << "OrderId" << "Article" << "amount";

QStringList DatabaseManager::OrderClientFields =
        QStringList() << "OrderId" << "Client";

QList<QStringList> DatabaseManager::allNames = {DatabaseManager::articlesFields,
                                           DatabaseManager::functionsFields,
                                           DatabaseManager::clientsFields,
                                           DatabaseManager::saleSessionsFields,
                                           DatabaseManager::heldSessionFields,
                                           DatabaseManager::functionBenefitsFields,
                                           DatabaseManager::OrderDetailsFields,
                                           DatabaseManager::OrderContentFields,
                                           DatabaseManager::OrderClientFields
                                          };

void DatabaseManager::openDatabase(){
    //Creates default sql database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./data/KFeteManagerDB.sqlite");

    bool ok = db.open();
    if(!ok){
        qDebug() << "Error opening database";
    }
    QSqlQuery foreign_keys;
    bool res = foreign_keys.exec("PRAGMA foreign_keys=ON;");
    if(!res){
        qDebug()<<"Error seting PRAGMA foreign_keys=ON";
    }
}

bool DatabaseManager::checkDatabase(){
    /*
     Checks if the scheme of the database is correct

     Returns:
            True   if the database seems OK.
            False  if there is a problem.
    */

    if(allNames.size() != tables.length()){
        qDebug() << "Error: allNames is not the same length as tables.";
        qDebug() << allNames.size();
        return false;
    }

    //Check if default database opened.
    if(!QSqlDatabase::contains()){
        return false;
    }

    //Loads the database.
    QSqlDatabase db = QSqlDatabase::database();

    //Checks if the default database is open
    if(!db.isOpen()){
        return false;
    }

    //Checks if the database contains the expected tables
    QStringList foundTables = db.tables();
    for(auto it = tables.constBegin(); it < tables.constEnd(); it++){
        if(!foundTables.contains(*it)){
            //The database lacks one of the expected tables
            qDebug() << "Table " << *it << " not found.";
            return false;
        }
        QSqlRecord fields = db.record(*it);
        int pos = it-tables.constBegin();
        for(auto it2 = allNames[pos].constBegin(); it2 < allNames[pos].constEnd(); it2++){
            if(!fields.contains(*it2)){
                //One of the tables does not have all the expected fields.
                qDebug() << "Field " << *it2 << " from table " << *it << " not found.";
                return false;
            }
        }
    }
    return true;
}

void DatabaseManager::createDatabase(){
    QSqlQuery querry;
    int res = querry.exec("CREATE TABLE IF NOT EXISTS "
                          "Articles("
                          "Name TEXT NOT NULL PRIMARY KEY,"
                          "sellPrice NUMERIC NOT NULL,"
                          "jShare NUMERIC NOT NULL,"
                          "bPrice NUMERIC NOT NULL,"
                          "reducedPrice NUMERIC NOT NULL,"
                          "function INTEGER DEFAULT 0 REFERENCES Functions(Id) ON UPDATE CASCADE ON DELETE SET DEFAULT);"
                          );
    if(!res){
        qDebug() << "Error Creating Articles";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("CREATE TABLE IF NOT EXISTS "
                      "Functions("
                      "Name TEXT NOT NULL UNIQUE,"
                      "Id INTEGER NOT NULL UNIQUE,"
                      "PRIMARY KEY(Name, Id));"
                      );
    if(!res){
        qDebug() << "Error Creating Functions";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("CREATE TABLE IF NOT EXISTS "
                      "Clients("
                      "Name TEXT NOT NULL PRIMARY KEY,"
                      "phone TEXT,"
                      "address TEXT,"
                      "email TEXT,"
                      "negLimit NUMERIC,"
                      "isJobist INTEGER,"
                      "balance NUMERIC NOT NULL);"
                      );
    if(!res){
        qDebug() << "Error Creating Clients";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("CREATE TABLE IF NOT EXISTS "
                      "SaleSessions("
                      "OpeningTime DATE NOT NULL PRIMARY KEY,"
                      "closingTime DATE,"
                      "openAmount NUMERIC,"
                      "closeAmount NUMERIC,"
                      "soldAmount NUMERIC);"
                      );
    if(!res){
        qDebug() << "Error Creating SaleSessions";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("CREATE TABLE IF NOT EXISTS "
                      "HeldSession("
                      "Name TEXT NOT NULL REFERENCES Clients(Name) ON DELETE CASCADE ON UPDATE CASCADE,"
                      "SessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE ON UPDATE CASCADE,"
                      "PRIMARY KEY(Name, SessionTime));"
                      );
    if(!res){
        qDebug() << "Error Creating HeldSession";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("CREATE TABLE IF NOT EXISTS "
                      "FunctionBenefits("
                      "FctId INTEGER NOT NULL DEFAULT 0 REFERENCES Functions(Id) ON DELETE SET DEFAULT ON UPDATE CASCADE,"
                      "SessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE NO ACTION ON UPDATE CASCADE,"
                      "amount NUMERIC,"
                      "PRIMARY KEY(FctId, SessionTime));"
                      );
    if(!res){
        qDebug() << "Error Creating FunctionBenefits";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("CREATE TABLE IF NOT EXISTS "
                      "OrderDetails("
                      "OrderId INTEGER PRIMARY KEY,"
                      "sessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE,"
                      "orderNumber INTEGER NOT NULL,"
                      "UNIQUE(sessionTime, orderNumber));"
                      );
    if(!res){
        qDebug() << "Error Creating OrderDetails";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("CREATE TABLE IF NOT EXISTS "
                      "OrderContent("
                      "OrderId INTEGER NOT NULL REFERENCES OrderDetails(OrderId) ON DELETE CASCADE,"
                      "Article TEXT NOT NULL REFERENCES Articles(Name) ON DELETE NO ACTION,"
                      "amount INTEGER NOT NULL CHECK(amount > 0),"
                      "PRIMARY KEY(OrderId, Article));"
                      );
    if(!res){
        qDebug() << "Error Creating OrderContent";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("CREATE TABLE IF NOT EXISTS "
                      "OrderClient("
                      "OrderId INTEGER NOT NULL REFERENCES OrderDetails(OrderId) ON DELETE CASCADE,"
                      "Client TEXT NOT NULL REFERENCES Clients(Name) ON DELETE NO ACTION,"
                      "PRIMARY KEY(OrderId, Client));"
                      );
    if(!res){
        qDebug() << "Error Creating OrderClient";
        qDebug() << querry.lastError().text();
        return;
    }

    res = querry.exec("INSERT OR REPLACE INTO Functions VALUES('Unknown Function', 0);");

    if(!res){
        qDebug() << "Error inserting default function";
        qDebug() << querry.lastError().text();
        return;
    }
}

void DatabaseManager::closeDatabase(){
    if(QSqlDatabase::contains()){
        QSqlDatabase::database().close();
    }

}
