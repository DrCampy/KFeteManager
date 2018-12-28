#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QList>
#include <QStringList>

/*   tables: Articles, Functions, Clients, SaleSessions, HeldSession, FunctionBenefits

     Articles: (Name, sellPrice, jShare, bPrice, reducedPrice, function)

     Functions: (Name, Id)

     Clients: (Name, phone, address, email, negLimit, isJobist, balance)

     SaleSessions: (OpeningTime, closingTime, openAmount, closeAmount, soldAmount)

     HeldSession: (Name, SessionTime)

     FunctionBenefits: (FctId, SessionTime)

     OrderDetails: (OrderId, sessionTime, orderNumber)

     OrderContent: (OrderId, Article, amount)

     OrderClient: (OrderId, Client)
      */

class DatabaseManager
{
private:
    static QStringList tables;


    static QStringList articlesFields, functionsFields, clientsFields,
            saleSessionsFields, heldSessionFields, functionBenefitsFields,
    OrderDetailsFields, OrderContentFields, OrderClientFields;

    static QList<QStringList> allNames;

public:
    static bool checkDatabase();
    static void openDatabase();
    static void closeDatabase();
    static void createDatabase();
};

#endif // DATABASEMANAGER_H
