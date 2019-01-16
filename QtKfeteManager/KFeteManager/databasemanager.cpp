#include <QObject>
#include <QList>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QFile>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QDateTime>
#include <QSettings>

#include "databasemanager.h"
#include "order.h"
#include "clientlist.h"
#include "catalog.h"

QStringList DatabaseManager::tables =
        QStringList() << "Articles" << "Functions" << "Clients" << "SaleSessions"
                      <<"HeldSession" << "Transactions" << "IsOrder" << "OrderContent"
                      << "CashMoves" << "OrderClient" << "Config";

QStringList DatabaseManager::articlesFields =
        QStringList() << "Name" << "sellPrice" << "jShare"
                      << "bPrice" << "reducedPrice" << "function";

QStringList DatabaseManager::functionsFields =
        QStringList() << "name" << "Id";

QStringList DatabaseManager::clientsFields =
        QStringList() << "Name" << "phone" << "address" << "email"
                      << "negLimit" << "isJobist" << "balance";

QStringList DatabaseManager::saleSessionsFields =
        QStringList() << "OpeningTime" << "closingTime" << "state"
                      << "openAmount"  << "closeAmount";

QStringList DatabaseManager::heldSessionFields =
        QStringList() << "Name" << "SessionTime";

QStringList DatabaseManager::TransactionsFields =
        QStringList() << "Id" << "sessionTime" << "lineNumber"
                      << "processTime" << "total";

QStringList DatabaseManager::IsOrderFields =
        QStringList() << "Id" << "price";

QStringList DatabaseManager::OrderContentFields =
        QStringList() << "Id" << "article" << "quantity";

QStringList DatabaseManager::OrderClientFields =
        QStringList() << "Id" << "client";

QStringList DatabaseManager::CashMovesFields =
        QStringList() << "Id" << "client";

QStringList DatabaseManager::configFields =
        QStringList() << "Field" << "value";

QList<QStringList> DatabaseManager::allNames = {DatabaseManager::articlesFields,
                                                DatabaseManager::functionsFields,
                                                DatabaseManager::clientsFields,
                                                DatabaseManager::saleSessionsFields,
                                                DatabaseManager::heldSessionFields,
                                                DatabaseManager::TransactionsFields,
                                                DatabaseManager::IsOrderFields,
                                                DatabaseManager::OrderContentFields,
                                                DatabaseManager::OrderClientFields,
                                                DatabaseManager::CashMovesFields,
                                                DatabaseManager::configFields
                                               };

void        DatabaseManager::openDatabase               (){
    //Creates default sql database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./data/KFeteManagerDB.sqlite");

    bool ok = db.open();
    if(!ok){
        qDebug() << "Error opening database";
    }
    QSqlQuery query;
    bool res = query.exec("PRAGMA foreign_keys=ON;");
    if(!res){
        qDebug()<<"Error seting PRAGMA foreign_keys=ON";
    }
    query.exec(QString("REPLACE INTO Functions(Id, name) VALUES(0,'").append(QObject::tr("Pas de fonction")).append("');"));

    //Creates a new temporary database to allow for translation in string of the boolean value.
    query.exec("CREATE TEMP TABLE BooleanYesNo( BoolValue INT PRIMARY KEY, string TEXT NOT NULL);");
    query.prepare("INSERT INTO BooleanYesNo VALUES(:value, :string)");
    query.bindValue(":value", 0);
    query.bindValue(":string", QObject::tr("Non"));
    query.exec();
    query.bindValue(":value", 1);
    query.bindValue(":string", QObject::tr("Oui"));
    query.exec();
}

bool        DatabaseManager::checkDatabase              (){
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

void        DatabaseManager::createDatabase             (){
    QSqlQuery querry;
    executeScript(":/create-script.sql", querry);

    //TODO remove
    //Insert notes and coins
    QSqlQuery query;
    query.exec("INSERT INTO Config(field, value) VALUES('currency', '€') WHERE Field='currency';");
    query.exec("INSERT INTO Config(field, value) VALUES('notes', '500;200;100;50;20;10;5');");
    query.exec("INSERT INTO Config(field, value) VALUES('coins', '2;1;0.5;0.2;0.1;0.05;0.02;0.01');");
}

bool        DatabaseManager::executeScript              (QString filename, QSqlQuery &query){
    //opens file
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString script = file.readAll();
    file.close();

    //Each statement begins with '--Statement'
    QVector<QStringRef> statements = script.splitRef("--Statement", QString::SkipEmptyParts);

    //for each statement
    for(auto it = statements.begin(); it < statements.end(); it++){
        QString finalStatement;
        //breaks down the statement into lines
        QVector<QStringRef> lines = (*it).split('\n', QString::SkipEmptyParts);
        //for each line, split comments
        for(auto it2 = lines.begin(); it2 < lines.end(); it2++){

            //If we are a line that starts with a comment, skip it
            if((*it2).startsWith("--")){
                continue;
            }
            auto linePart = (*it2).split("--", QString::SkipEmptyParts);

            //The instruction part is at index 0. The rest is only comments.
            finalStatement.append(linePart.at(0));
            finalStatement.append(' ');
        }
        finalStatement = finalStatement.trimmed();
        if(finalStatement.isEmpty() || finalStatement == ";"){
            continue;
        }

        //Appends final ';'
        //finalStatement.append(';');

        //Executes statement
        int ret = query.exec(finalStatement);
        if(!ret){
            qDebug() << "Error processing statement number " << it-statements.begin()+1 << " from file " << filename <<".";
            qDebug() << finalStatement;
            qDebug() << "Details: " << query.lastError().text();
            return false;
        }
    }
    return true;
}

void        DatabaseManager::closeDatabase              (){
    if(QSqlDatabase::contains()){
        QSqlDatabase::database().close();
    }
}

//Articles management
//GENERAL PURPOSE FUNCTIONS
bool        DatabaseManager::hasArticle                 (const Article &a){
    QSqlQuery query;
    query.prepare("SELECT Name FROM Articles WHERE Name=:name;");
    query.bindValue(":name", a.getName());
    bool ret = query.exec();
    if(!ret){
        qDebug() << "Finding article " << a << " failed.";
        qDebug() << query.lastError().text();
        return false;
    }
    return query.next();
}

/*
 * Adds an article to the database.
 * If the database already contains the article,
 * updates the value of the existing entry.
 */
bool        DatabaseManager::addArticle                 (const Article &a, qreal price, qreal jShare, qreal bPrice, qreal redPrice, QString function){
    QSqlQuery query;
    uint f = hasFunction(function);
    query.prepare("INSERT OR REPLACE INTO Articles(Name, sellPrice, jShare, bPrice, reducedPrice, function)"
                  " VALUES(:name, :price, :jShare, :bPrice, :redPrice, :f);");
    query.bindValue(":price", price);
    query.bindValue(":jShare", jShare);
    query.bindValue(":bPrice", bPrice);
    query.bindValue(":redPrice", redPrice);
    query.bindValue(":f", f);
    query.bindValue(":name", a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Inserting article " << a << " failed.";
        qDebug() << query.lastError().text();
    }

    return ret;
}

bool        DatabaseManager::updateArticleField         (const Article &a, QString field, QVariant value){
    QSqlQuery query;
    query.prepare("UPDATE Articles SET " + field + "=:value WHERE Name=:name;");

    //Manages the particular case of the fields function
    if(field == "function"){
        query.bindValue(":value", "(SELECT Id FROM Functions WHERE name='" + value.toString() + "')");
    }else{
        query.bindValue(":value", value);
    }

    query.bindValue(":name", a.getName());
    bool ret = query.exec();
    if(!ret){
        qDebug() << "Error updating field " << field << " with value " << value << " for article " << a;
        qDebug() << query.lastError().text();
    }
    return ret;
}

QVariant    DatabaseManager::getArticleField            (const Article &a, QString field){
    QSqlQuery query;

    //Manages the particular case of the fields function
    if(field == "function"){
        query.prepare("SELECT Functions.Name "
                      "FROM( SELECT Name, function FROM Articles WHERE Name=:name) "
                      "INNER JOIN Functions ON Articles.function=Functions.Id);");

    }else{
        query.prepare("SELECT " + field + " FROM Articles WHERE Name=:name;");
    }

    query.bindValue(":name", a.getName());

    bool ret = query.exec();
    if(!ret){
        qDebug() << "Error getting field " << field << " of article " << a;
        qDebug() << query.lastError().text();
        return QVariant();
    }
    if(!query.next()){
        return QVariant();
    }else{
        return query.value(0);
    }
}

void        DatabaseManager::delArticle                 (const Article &a){
    QSqlQuery query;
    query.prepare("DELETE FROM Articles WHERE Name=:name;");
    query.bindValue(":name", a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Deleting article " << a << " failed.";
        qDebug() << query.lastError().text();
    }
}

void        DatabaseManager::extractArticle             (const Article &a, qreal &price, qreal &jShare, qreal &bPrice, qreal &redPrice, QString &function){
    QSqlQuery query;

    //Manages the particular case of the fields function
    query.prepare("SELECT Articles.sellPrice, Articles.jShare, Articles.bPrice, Articles.reducedPrice, Functions.name "
                  "FROM( SELECT * FROM Articles WHERE Name=:name) "
                  "INNER JOIN Functions ON Articles.function=Functions.id);");

    query.bindValue(":name", a.getName());

    bool ret = query.exec();
    if(!ret){
        qDebug() << "Error getting article " << a;
        qDebug() << query.lastError().text();
        return;
    }
    if(query.next()){
        price    = query.value("Articles.sellPrice").toReal();
        jShare   = query.value("Articles.jShare").toReal();
        bPrice   = query.value("Articles.bPrice").toReal();
        redPrice = query.value("Articles.reducedPrice").toReal();
        function = query.value("Functions.name").toString();
    }
}

QStringList DatabaseManager::articlesList               (){
    QSqlQuery query;

    //Executes the query
    bool ret = query.exec("SELECT Name FROM Articles;");

    QStringList list;
    if(!ret){
        qDebug() << "Error getting articles list";
        qDebug() << query.lastError().text();
        return list;
    }

    //Creates the list
    while(query.next()){
        list.append(query.value(0).toString());
    }

    return list;
}
//ACCESSORS FUNCTIONS
qreal       DatabaseManager::getArticlePrice            (const Article &a){
    QVariant ret = getArticleField(a, "sellPrice");
    if(ret.isNull())
        return qQNaN();
    else
        return ret.toReal();
}

qreal       DatabaseManager::getArticleReducedPrice     (const Article &a){
    QVariant ret = getArticleField(a, "reducedPrice");
    if(ret.isNull())
        return qQNaN();
    else
        return ret.toReal();
}

qreal       DatabaseManager::getArticleJobistShare      (const Article &a){
    QVariant ret = getArticleField(a, "jShare");
    if(ret.isNull())
        return qQNaN();
    else
        return ret.toReal();
}

qreal       DatabaseManager::getArticleBuyingPrice      (const Article &a){
    QVariant ret = getArticleField(a, "bPrice");
    if(ret.isNull())
        return qQNaN();
    else
        return ret.toReal();
}

QString     DatabaseManager::getArticleFunction         (const Article &a){
    QVariant ret = getArticleField(a, "function");
    if(ret.isNull())
        return "";
    else
        return ret.toString();
}

//SETTERS FUNCTIONS

bool        DatabaseManager::updateArticlePrice          (const Article &a, qreal value){
    return updateArticleField(a, "sellPrice", value);
}

bool        DatabaseManager::updateArticleReducedPrice   (const Article &a, qreal value){
    return updateArticleField(a, "reducedPrice", value);
}

bool        DatabaseManager::updateArticleJobistShare    (const Article &a, qreal value){
    return updateArticleField(a, "jShare", value);

}

bool        DatabaseManager::updateArticleBuyingPrice    (const Article &a, qreal value){
    return updateArticleField(a, "bPrice", value);
}

bool        DatabaseManager::updateArticleFunction       (const Article &a, QString value){
    return updateArticleField(a, "function", value);
}


//Client management
bool        DatabaseManager::updateClientField          (const Client &c, QString field, QVariant value){
    QSqlQuery query;
    query.prepare("UPDATE Clients SET " + field + "=:value WHERE Name=:name;");
    query.bindValue(":value", value);
    query.bindValue(":name", c.getName());
    bool ret = query.exec();
    if(!ret){
        qDebug() << "Error updating field " << field << " with value " << value << " for client " << c;
        qDebug() << query.lastError().text();
    }
    return ret;
}

QVariant    DatabaseManager::getClientField             (const Client &c, QString field){
    QSqlQuery query;

    query.prepare("SELECT " + field + " FROM Clients WHERE Name=:name;");
    query.bindValue(":name", c.getName());

    bool ret = query.exec();
    if(!ret){
        qDebug() << "Error getting field " << field << " of client " << c;
        qDebug() << query.lastError().text();
        return QVariant();
    }

    if(!query.next()){
        return QVariant();
    }else{
        return query.value(0);
    }
}

bool        DatabaseManager::hasClient                  (const Client &c){
    QSqlQuery query;
    query.prepare("SELECT Name FROM Clients WHERE Name=:name;");
    query.bindValue(":name", c.getName(), QSql::Out);
    int ret = query.exec();
    if(!ret){
        qDebug() << "Finding client" << c << " faile    d.";
        qDebug() << query.lastError().text();
        return false;
    }
    return query.next();
}

bool        DatabaseManager::addClient                  (const Client &c, QString phone, QString address, QString email, qreal negLimit, bool isJobist, qreal balance){
    QSqlQuery query;
    query.prepare("INSERT OR UPDATE INTO Clients(Name, phone, address, email, negLimit, isJobist, balance) VALUES(?,?,?,?,?,?);");
    query.addBindValue(c.getName());
    query.addBindValue(phone);
    query.addBindValue(address);
    query.addBindValue(email);
    query.addBindValue(negLimit);
    query.addBindValue(isJobist);
    query.addBindValue(balance);
    int ret = query.exec();
    if(!ret){
        qDebug() << "Inserting " << c << " failed.";
        qDebug() << query.lastError().text();
    }
    return ret;
}

void        DatabaseManager::delClient                  (const Client &c){
    QSqlQuery query;
    query.prepare("DELETE FROM Clients WHERE Name=:name;");
    query.bindValue(":name", c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Deleting client" << c << " failed.";
        qDebug() << query.lastError().text();
    }
}


//ACCESSORS FUNCTIONS
QString     DatabaseManager::getClientPhone             (const Client &c){
    QVariant ret = getClientField(c, "phone");
    if(ret.isNull()){
        return "";
    }
    return ret.toString();
}

QString     DatabaseManager::getClientAddress           (const Client &c){
    QVariant ret = getClientField(c, "address");
    if(ret.isNull()){
        return "";
    }
    return ret.toString();
}

QString     DatabaseManager::getClientEmail             (const Client &c){
    QVariant ret = getClientField(c, "email");
    if(ret.isNull()){
        return "";
    }
    return ret.toString();
}

qreal       DatabaseManager::getClientLimit             (const Client &c){
    QVariant ret = getClientField(c, "negLimit");
    if(ret.isNull()){
        return qQNaN();
    }
    return ret.toReal();
}

bool        DatabaseManager::isClientJobist             (const Client &c){
    QVariant ret = getClientField(c, "isJobist");
    if(ret.isNull()){
        return false;
    }
    //SQlite does not support bool. Stores integer 1 for of true, 0 for false.
    return (ret.toInt() == 1);
}

qreal       DatabaseManager::getClientBalance           (const Client &c){
    QVariant ret = getClientField(c, "phone");
    if(ret.isNull()){
        return qQNaN();
    }
    return ret.toReal();
}

//SETTERS FUNCTIONS
bool        DatabaseManager::updateClientPhone          (const Client &c, QString value){
    return updateClientField(c, "phone", value);
}

bool        DatabaseManager::updateClientAddress        (const Client &c, QString value){
    return updateClientField(c, "address", value);
}

bool        DatabaseManager::updateClientEmail          (const Client &c, QString value){
    return updateClientField(c, "email", value);
}

bool        DatabaseManager::updateClientLimit          (const Client &c, qreal value){
    return updateClientField(c, "negLimit", value);
}

bool        DatabaseManager::updateIsClientJobist       (const Client &c, bool value){
    return updateClientField(c, "isJobist", value);
}

bool        DatabaseManager::updateClientBalance        (const Client &c, qreal value){
    return updateClientField(c, "balance", value);
}

/*
 * Inserts a new function to a database.
 * If the database already contains this function
 * then the functions return the ID already allocated to that function.
 * Comparison of functions is NOT CasE SeNsiTIvE
 */
uint        DatabaseManager::addFunction                (QString name){
    name = name.trimmed();
    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO Functions(name) VALUES(?);");
    query.addBindValue(name);
    int ret = query.exec();
    if(!ret){
        qDebug() << "Inserting function " << name << " failed.";
        qDebug() << query.lastError().text();
        return 0;
    }
    query.prepare("SELECT Id FROM Functions WHERE name=?;");
    query.addBindValue(name, QSql::Out);
    ret = query.exec();
    if(!ret){
        qDebug() << "Getting id of function " << name << " failed.";
        qDebug() << query.lastError().text();
        return 0;
    }
    if(query.next()){
        return query.value(0).toUInt();
    }
    return 0;
}

uint        DatabaseManager::hasFunction                (QString name){
    name = name.trimmed();
    QSqlQuery query;
    query.prepare("SELECT Id FROM Functions WHERE name=:name;");
    query.bindValue(":name", name);
    bool ret = query.exec();
    if(!ret){
        qDebug() << "Finding function " << name << " failed.";
        qDebug() << query.lastError().text();
        return 0;
    }

    if(query.next()){
        uint val = query.value(0).toUInt();
        return val;
    }
    return 0;
}

QList<QString> DatabaseManager::getFunctions            (){
    QSqlQuery query;
    int ret = query.exec("SELECT name FROM Functions;");
    if(!ret){
        qDebug() << "Listing functions failed.";
        qDebug() << query.lastError().text();
        return QList<QString>();
    }
    QList<QString> list;
    while(query.next()){
        list.append(query.value(0).toString());
    }
    return list;
}

void        DatabaseManager::delFunction                (QString name){
    QSqlQuery query;
    query.prepare("DELETE FROM Functions WHERE name=?;");
    query.addBindValue(name);
    int ret = query.exec();
    if(!ret){
        qDebug() << "Deleting function failed.";
        qDebug() << query.lastError().text();
    }
}

/*
 * Managing Orders
 */

bool        DatabaseManager::addOrder                   (const Order &o, Client c){
    QSqlQuery query;

    //Fetch the session time.
    QVariant sessionTime = getCurrentSession();
    if(sessionTime.isNull()){
        qDebug() << "No open session.";
        return false;
    }

    bool success = true;
    QSqlDatabase::database().transaction(); //starts a transaction

    //Fetch the next order line
    success &= query.exec("SELECT value FROM Config WHERE Field='CurrentSessionOrderId';");
    query.next();
    unsigned int orderNumber = query.value(0).toUInt();

    //Adds the order as transaction (not to be confused with sql transaction...)
    QDateTime time = QDateTime::currentDateTime();
    query.prepare("INSERT INTO Transactions(sessionTime, lineNumber, processTime, total) "
                  "VALUES(:session, :line, :time, :total);");
    query.bindValue(":session", sessionTime.toULongLong());
    query.bindValue(":line", orderNumber);
    query.bindValue(":time", time.toSecsSinceEpoch());
    query.bindValue(":total", o.getTotal());
    success &= query.exec();

    unsigned long long int orderID = query.lastInsertId().toULongLong();

    //Adds the order as order
    query.prepare("INSERT INTO IsOrder(Id, price) VALUES(:id, :price);");
    query.bindValue(":id", orderID);
    switch(o.getPrice()){
    case Order::normal :
        query.bindValue(":price", "normal");
        break;
    case Order::reduced :
        query.bindValue(":price", "reduced");
        break;
    case Order::free :
        query.bindValue(":price", "free");
        break;
    }
    success &= query.exec();

    //Adds order content
    for(auto it:o.getContent()->keys()){
        query.prepare("INSERT INTO OrderContent(Id, article, quantity)"
                      "VALUES (:id, :article, :quantity);");
        query.bindValue(":id", orderID);
        query.bindValue(":article", it.getName());
        query.bindValue(":quantity", o.getContent()->value(it).first);
        success &= query.exec();
    }

    //Adds the order client
    if(!c.isNull()){
        query.prepare("INSERT INTO OrderClient(Id, client) VALUES(:id, :client);");
        query.bindValue(":id", orderID);
        query.bindValue(":client", c.getName());
        success &= query.exec();
    }

    //Increments the next order line
    query.prepare("UPDATE Config SET value=:line WHERE Field='CurrentSessionOrderId';");
    query.bindValue(":line", orderNumber+1);
    success &= query.exec();
    if(success){
        QSqlDatabase::database().commit();
    }else{
        QSqlDatabase::database().rollback();
    }

    return success;
}

//Miscelaneous
QVariant    DatabaseManager::getCurrentSession          (){
    QSqlQuery query;

    //Fetch the session time.
    query.exec("SELECT value FROM Config WHERE Field='CurrentSession';");
    if(!query.next()){
        qDebug() << "Could not fetch current session time.";
        return QVariant(QString()); //Return null value
    }
    return query.value(0);
}

bool        DatabaseManager::closeSession               (QVariant closeAmount){
    //Get current session
    QVariant currentSession = getCurrentSession();

    //If there is no open session
    if(currentSession.isNull()){
        //If we had no closeAmount we are done
        //Otherwise return false because we could not set the closeAmount.
        return closeAmount.isNull();
    }

    //The session we have is an existing session because of the foreign key.

    //Current time
    qint64 now = QDateTime::currentDateTime().toSecsSinceEpoch();

    //Updates the session and close it.
    QSqlQuery query;
    bool success;
    QSqlDatabase::database().transaction();
    success = query.exec("UPDATE Config SET value = NULL WHERE Field='CurrentSession'"); //Remove the current session from Config

    //Closes the session by setting the correct values
    if(closeAmount.isNull()){
        //If we have no closeAmount we still want to keep the closeAmount already in the database.
        query.prepare("UPDATE SaleSessions SET state='closed', "
                      "closingTime=:closeTime WHERE (OpeningTime = :session AND state='opened');"); //Close the session
        query.bindValue(":closeTime", now);
        query.bindValue(":session", currentSession.toLongLong());
        success &= query.exec();
    }else{
        query.prepare("UPDATE SaleSessions SET state='closed', closeAmount = :amount, "
                      "closingTime=:closeTime WHERE (OpeningTime = :session AND state='opened');"); //Close the session
        query.bindValue(":amount", closeAmount);
        query.bindValue(":closeTime", now);
        query.bindValue(":session", currentSession.toLongLong());
        success &= query.exec();
    }

    if(success){
        QSqlDatabase::database().commit();
        QSettings settings;
        settings.setValue("count/after/notes", "");
        settings.setValue("count/after/coins", "");
        settings.setValue("count/before/notes", "");
        settings.setValue("count/before/coins", "");
    }else{
        QSqlDatabase::database().rollback();
    }
    return success;
}

bool        DatabaseManager::newSession                 (QVariant openAmount, QList<Client> holdingSession){
    QDateTime now = QDateTime::currentDateTime();

    QSqlQuery query;
    bool success = true;
    QSqlDatabase::database().transaction();

    //Creates the new session
    query.prepare("INSERT INTO SaleSessions(openingTime, openAmount) VALUES(:time, :amount);");
    query.bindValue(":time", now.toSecsSinceEpoch());
    query.bindValue(":amount", openAmount);
    success &= query.exec();

    //Sets the created session as current session in the program.
    query.prepare("UPDATE Config SET Value=:time WHERE field='CurrentSession';");
    query.bindValue(":time", now.toSecsSinceEpoch());
    success &= query.exec();

    //Sets the next (first) order number to be 1.
    success &= query.exec("UPDATE Config SET Value=1 WHERE field='CurrentSessionOrderId';");

    //Adds the jobists holding the session
    query.prepare("INSERT INTO heldSession(Name, SessionTime) VALUES(:jobist, :session);");
    query.bindValue(":session", now.toSecsSinceEpoch(), QSql::In);
    for(auto it : holdingSession){
        query.bindValue(":jobist", it.getName(), QSql::In);
        success &= query.exec();
    }

    if(success){
        QSqlDatabase::database().commit();
    }else{
        QSqlDatabase::database().rollback();
    }

    return success;
}

bool        DatabaseManager::setCurrentSessionOpenAmount(qreal count){
    QSqlQuery query;

    //Fetch the session time.
    QVariant sessionTime = getCurrentSession();
    if(sessionTime.isNull()){
        qDebug() << "No open session.";
        return false;
    }

    //Sets openAmount
    query.prepare("UPDATE SaleSession SET openAmount=:amount WHERE OpeningTime = :time;");
    query.bindValue(":amount", count);
    query.bindValue(":time", sessionTime);
    bool success = query.exec();

    return success;
}

bool        DatabaseManager::setCurrentSessionjobists   (QList<Client> jobists){
    QSqlQuery query;
    bool success = true;

    QSqlDatabase::database().transaction();
    //Fetch the session time.
    QVariant sessionTime = getCurrentSession();
    if(sessionTime.isNull()){
        qDebug() << "No open session.";
        return false;
    }
    query.prepare("DELETE FROM heldSession WHERE SessionTime = :session;");
    query.bindValue(":session", sessionTime);
    success &= query.exec();

    //Adds the jobists holding the session
    query.prepare("INSERT INTO heldSession(name, SessionTime) VALUES(:jobist, :session);");
    query.bindValue(":session", sessionTime, QSql::In);
    for(auto it : jobists){
        query.bindValue(":jobist", it.getName(), QSql::In);
        success &= query.exec();
    }

    if(success){
        QSqlDatabase::database().commit();
    }else{
        QSqlDatabase::database().rollback();
    }

    return success;
}

bool        DatabaseManager::setCurrentSessionCloseAmount(qreal count){
    QSqlQuery query;

    //Fetch the session time.
    QVariant sessionTime = getCurrentSession();
    if(sessionTime.isNull()){
        qDebug() << "No open session.";
        return false;
    }

    //Sets closeAmount
    query.prepare("UPDATE SaleSession SET closeAmount=:amount WHERE OpeningTime = :time;");
    query.bindValue(":amount", count);
    query.bindValue(":time", sessionTime);
    bool success = query.exec();

    return success;
}

QStringList DatabaseManager::getNotes                   (){
    QSqlQuery query;
    if(!query.exec("SELECT value FROM Config WHERE Field='notes';") || !query.next()){
        return QStringList();
    }else{
        return query.value(0).toString().split(";", QString::SkipEmptyParts);
    }
}

QStringList DatabaseManager::getCoins                   (){
    QSqlQuery query;
    if(!query.exec("SELECT value FROM Config WHERE Field='coins';") || !query.next()){
        return QStringList();
    }else{
        return query.value(0).toString().split(";", QString::SkipEmptyParts);
    }
}

QString     DatabaseManager::getCurrency                (){
    QSqlQuery query;
    if(!query.exec("SELECT value FROM Config WHERE Field='currency';") || !query.next()){
        return QString();
    }else{
        return query.value(0).toString();
    }
}

bool        DatabaseManager::deposit                    (qreal amount, Client c){
    QSqlQuery query;

    if( (!c.isNull() && !c.exists()) || (amount <= 0.01) ){
        return false;
    }

    //Fetch the session time.
    QVariant sessionTime = getCurrentSession();
    if(sessionTime.isNull()){
        qDebug() << "No open session.";
        return false;
    }

    bool success = true;
    QSqlDatabase::database().transaction(); //starts a transaction

    //Fetch the next order line
    success &= query.exec("SELECT value FROM Config WHERE Field='CurrentSessionOrderId';");
    query.next();
    unsigned int orderNumber = query.value(0).toUInt();

    //Adds the order as transaction (not to be confused with sql transaction...)
    QDateTime time = QDateTime::currentDateTime();
    query.prepare("INSERT INTO Transactions(sessionTime, lineNumber, processTime, total) "
                  "VALUES(:session, :line, :time, :total);");
    query.bindValue(":session", sessionTime.toULongLong());
    query.bindValue(":line", orderNumber);
    query.bindValue(":time", time.toSecsSinceEpoch());
    query.bindValue(":total", amount);
    success &= query.exec();

    unsigned long long int orderID = query.lastInsertId().toULongLong();

    //Adds the order as order
    query.prepare("INSERT INTO CashMoves(Id, client) VALUES(:id, :client);");
    query.bindValue(":id", orderID);
    query.bindValue(":client", c.getName());
    success &= query.exec();

    //Increments the next order line
    query.prepare("UPDATE Config SET value=:line WHERE Field='CurrentSessionOrderId';");
    query.bindValue(":line", orderNumber+1);
    success &= query.exec();

    if(!c.isNull()){
        query.prepare("UPDATE Clients SET balance = balance + :amount WHERE Name = :name;");
        query.bindValue(":amount", amount);
        if(!c.isNull()){
            query.bindValue(":name", c.getName());
        }else{
            query.bindValue(":name", QVariant());
        }
        success &= query.exec();
    }

    if(success){
        QSqlDatabase::database().commit();
    }else{
        QSqlDatabase::database().rollback();
    }

    return success;
}
