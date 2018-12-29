#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QList>
#include <QStringList>
#include <QSqlQuery>

#include "catalog.h"
#include "clientlist.h"


/*   tables: Articles, Functions, Clients, SaleSessions, HeldSession, FunctionBenefits

     Articles: (Name, sellPrice, jShare, bPrice, reducedPrice, function)

     Functions: (Name, Id)

     Clients: (Name, phone, address, email, negLimit, isJobist, balance)

     SaleSessions: (OpeningTime, closingTime, openAmount, closeAmount, soldAmount)

     HeldSession: (Name, SessionTime)

     OrderDetails: (OrderId, sessionTime, orderNumber)

     OrderContent: (OrderId, Article, amount)

     OrderClient: (OrderId, Client)

     Config: (Field, value)
      */

/*
 * Here we decide to do one database request for each data we need.
 * If it was to be too slow we could choose to do one database
 * request to get the article or client or...
 * then proceed as usual.
 * The problem then would be that the article or so could be out of date
 * with old data from the database.
 *
 * We might also keep a transparent cache. A call to client.get***() would get the whole client and store it.
 * Further calls to the client would get from the cache.
 * changes on the client would get imediately repercuted on db and entry updated/deleted from cache.
 */

class DatabaseManager
{
private:
    static QStringList tables;


    static QStringList articlesFields, functionsFields, clientsFields,
    saleSessionsFields, heldSessionFields,
    OrderDetailsFields, OrderContentFields, OrderClientFields, configFields;

    static QList<QStringList> allNames;

public:
    static bool checkDatabase();
    static void openDatabase();
    static void closeDatabase();
    static void createDatabase();
    static bool executeScript(QString filename, QSqlQuery &query);

protected:
    //Managing articles
    static bool hasArticle(const Article &a);
    static bool addArticle(const Article &a, qreal price, qreal jShare, qreal bPrice, qreal redPrice, QString function);
    static void delArticle(const Article &a);
    static qreal getArticlePrice(const Article &a);
    static qreal getArticleReducedPrice(const Article &a);
    static qreal getArticleJobistShare(const Article &a);
    static qreal getArticleBuyingPrice(const Article &a);
    static QString getArticleFunction(const Article &a);

    //Clients: (Name, phone, address, email, negLimit, isJobist, balance)
    //Managing clients
    static bool hasClient(const Client &c);
    static bool addClient(const Client &c, QString phone, QString address, QString email, qreal negLimit, bool isJobist);
    static void delClient(const Client &c);
    static QString getClientPhone(const Client &c);
    static QString getClientAddress(const Client &c);
    static QString getClientEmail(const Client &c);
    static qreal getClientLimit(const Client &c);
    static bool isClientJobist(const Client &c);
    static qreal getClientBalance(const Client &c);

    static unsigned long int addFunction(QString name);

    //Declaring friends classes
    friend Article;
    friend Client;
};

#endif // DATABASEMANAGER_H
