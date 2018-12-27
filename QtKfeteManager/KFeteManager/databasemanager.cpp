#include <QObject>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>
#include <QStringList>

#include "databasemanager.h"

QStringList DatabaseManager::tables =
        QStringList() << "Articles" << "Functions" << "Clients"
                      << "SaleSessions" <<"HeldSession" <<"FunctionBenefits";

QStringList DatabaseManager::articlesFields =
        QStringList() << "Name" << "sellPrice" << "jShare"
                      << "reducedPrice" << "function";

QStringList DatabaseManager::functionsFields =
        QStringList() << "Name" << "ID";

QStringList DatabaseManager::clientsFields =
        QStringList() << "Name" << "phone" << "address" << "email"
                      << "limit" << "isJobist" << "balance";

QStringList DatabaseManager::saleSessionsFields =
        QStringList() << "OpeningTime" << "closingTime" << "jShare"
                      << "openAmount" << "closeAmount" << "soldAmount";

QStringList DatabaseManager::heldSessionFields =
        QStringList() << "Name" << "SessionTime";

QStringList DatabaseManager::functionBenefitsFields =
        QStringList() << "FctID" << "SessionTime";

QStringList DatabaseManager::allNames[] = {DatabaseManager::articlesFields,
                                         DatabaseManager::functionsFields,
                                         DatabaseManager::clientsFields,
                                         DatabaseManager::saleSessionsFields,
                                         DatabaseManager::heldSessionFields,
                                         DatabaseManager::functionBenefitsFields
                                        };
DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{


}


bool DatabaseManager::checkDatabase(){
    /*
     Checks if the scheme of the database is correct

     Returns:
            True   if the database seems OK.
            False  if ther is a problem.

     tables: Articles, Functions, Clients, SaleSessions, HeldSession, FunctionBenefits

     Articles: (Name, sellPrice, jShare, reducedPrice, function)

     Functions: (Name, ID)

     Clients: (Name, phone, address, email, limit, isJobist, balance)

     SaleSessions: (OpeningTime, closingTime, jShare, openAmount, closeAmount, soldAmount)

     HeldSession: (Name, SessionTime)

     FunctionBenefits: (FctID, SessionTime)
      */

    QStringList tables;
    tables << "Articles" << "Functions" << "Clients"
                   << "SaleSessions" <<"HeldSession" <<"FunctionBenefits";

    QStringList articlesFields, functionsFields, clientsFields,
            saleSessionsFields, heldSessionFields, functionBenefitsFields;

    articlesFields << "Name" << "sellPrice" << "jShare"
                   << "reducedPrice" << "function";

    functionsFields << "Name" << "ID";

    clientsFields << "Name" << "phone" << "address" << "email"
                  << "limit" << "isJobist" << "balance";

    saleSessionsFields << "OpeningTime" << "closingTime" << "jShare"
                       << "openAmount" << "closeAmount" << "soldAmount";

    heldSessionFields << "Name" << "SessionTime";

    functionBenefitsFields << "FctID" << "SessionTime";

    QStringList allNames[]={articlesFields, functionsFields, clientsFields, saleSessionsFields,
                        heldSessionFields, functionBenefitsFields};

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
            return false;
        }
        auto fields = db.record(*it);

        for(auto it2 = allNames[it-tables.constBegin()].constBegin();
            it2 < allNames[it-tables.constBegin()].constEnd(); it2++){
            if(!fields.contains(*it2)){
                //One of the tables does not have all the expected fields.
                return false;
            }
        }
    }
    return true;
}
