#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QList>
#include <QStringList>
#include <QSqlQuery>

#include <clientlist.h>

/*   tables: Articles, Functions, Clients, SaleSessions, HeldSession, FunctionBenefits

     Articles: (Name, sellPrice, jShare, bPrice, reducedPrice, function)

     Functions: (name, Id)

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

class Client;
class Article;
class Order;

class DatabaseManager
{
private:
    static QStringList tables;

    static QStringList articlesFields, functionsFields, clientsFields,
    saleSessionsFields, heldSessionFields, TransactionsFields, IsOrderFields,
    OrderContentFields, OrderClientFields, CashMovesFields, configFields;

    static QList<QStringList> allNames;

public:

    //Database Management
    static bool             checkDatabase           ();
    static void             openDatabase            ();
    static void             closeDatabase           ();
    static void             createDatabase          ();

    //TODO find where they should go. Class Function in catalog.h ?
    static uint             addFunction             (QString name);
    static uint             hasFunction             (QString name);
    static QList<QString>   getFunctions            ();
    static void             delFunction             (QString name);
    static bool             addOrder                (const Order &o, Client c);
    static bool             deposit                 (qreal amount, Client c = Client());

    //Miscelaneous
    static QVariant         getCurrentSession           ();
    static bool             closeSession                (QVariant closeAmount);
    static bool             newSession                  (QVariant openAmount, QList<Client> holdingSession = QList<Client>());
    static bool             setCurrentSessionOpenAmount (qreal count);
    static bool             setCurrentSessionjobists    (QList<Client> jobists);

    static QStringList      getNotes                    ();
    static QStringList      getCoins                    ();
    static QString          getCurrency                 ();

    /*
     * Managing articles
     */
    //General purpose
    static bool         updateArticleField          (const Article &a, QString field, QVariant value);
    static QVariant     getArticleField             (const Article &a, QString field);
    static void         delArticle                  (const Article &a);
    static void         extractArticle              (const Article &a, qreal &price, qreal &jShare, qreal &bPrice, qreal &redPrice, QString &function);
    static QStringList  articlesList                ();

    //getters
    static qreal        getArticlePrice             (const Article &a);
    static qreal        getArticleReducedPrice      (const Article &a);
    static qreal        getArticleJobistShare       (const Article &a);
    static qreal        getArticleBuyingPrice       (const Article &a);
    static QString      getArticleFunction          (const Article &a);

    //setters
    static bool         updateArticlePrice          (const Article &a, qreal value);
    static bool         updateArticleReducedPrice   (const Article &a, qreal value);
    static bool         updateArticleJobistShare    (const Article &a, qreal value);
    static bool         updateArticleBuyingPrice    (const Article &a, qreal value);
    static bool         updateArticleFunction       (const Article &a, QString value);

    /*
     * Managing clients
     */
    //general purpose
    static bool         hasClient                   (const Client &c);
    static bool         addClient                   (const Client &c, QString phone, QString address, QString email, qreal negLimit, bool isJobist, qreal balance = 0);
    static void         delClient                   (const Client &c);

    //getters
    static QString      getClientPhone              (const Client &c);
    static QString      getClientAddress            (const Client &c);
    static QString      getClientEmail              (const Client &c);
    static qreal        getClientLimit              (const Client &c);
    static bool         isClientJobist              (const Client &c);
    static qreal        getClientBalance            (const Client &c);

    //setters
    static bool         updateClientPhone           (const Client &c, QString value);
    static bool         updateClientAddress         (const Client &c, QString value);
    static bool         updateClientEmail           (const Client &c, QString value);
    static bool         updateClientLimit           (const Client &c, qreal value);
    static bool         updateIsClientJobist        (const Client &c, bool value);
    static bool         updateClientBalance         (const Client &c, qreal value);

    static bool         hasArticle                  (const Article &a);
    static bool         addArticle                  (const Article &a, qreal price, qreal jShare, qreal bPrice, qreal redPrice, QString function);

private:
    static bool         executeScript               (QString filename, QSqlQuery &query);

    static bool         updateClientField           (const Client &c, QString field, QVariant value);
    static QVariant     getClientField              (const Client &c, QString field);
};

#endif // DATABASEMANAGER_H
