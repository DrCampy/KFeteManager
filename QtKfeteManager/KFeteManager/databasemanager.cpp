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

#include "databasemanager.h"

QStringList DatabaseManager::tables =
        QStringList() << "Articles" << "Functions" << "Clients" << "SaleSessions"
                      <<"HeldSession" << "OrderDetails" << "OrderContent"
                     << "OrderClient" << "Config";

QStringList DatabaseManager::articlesFields =
        QStringList() << "Name" << "sellPrice" << "jShare"
                      << "bPrice" << "reducedPrice" << "function";

QStringList DatabaseManager::functionsFields =
        QStringList() << "name" << "Id";

QStringList DatabaseManager::clientsFields =
        QStringList() << "Name" << "phone" << "address" << "email"
                      << "negLimit" << "isJobist" << "balance";

QStringList DatabaseManager::saleSessionsFields =
        QStringList() << "OpeningTime" << "closingTime" << "openAmount"
                      << "closeAmount" << "soldAmount";

QStringList DatabaseManager::heldSessionFields =
        QStringList() << "Name" << "SessionTime";

QStringList DatabaseManager::OrderDetailsFields =
        QStringList() << "OrderID" << "sessionTime" << "orderNumber";

QStringList DatabaseManager::OrderContentFields =
        QStringList() << "OrderId" << "Article" << "amount";

QStringList DatabaseManager::OrderClientFields =
        QStringList() << "OrderId" << "Client";

QStringList DatabaseManager::configFields =
        QStringList() << "Field" << "value";

QList<QStringList> DatabaseManager::allNames = {DatabaseManager::articlesFields,
                                                DatabaseManager::functionsFields,
                                                DatabaseManager::clientsFields,
                                                DatabaseManager::saleSessionsFields,
                                                DatabaseManager::heldSessionFields,
                                                DatabaseManager::OrderDetailsFields,
                                                DatabaseManager::OrderContentFields,
                                                DatabaseManager::OrderClientFields,
                                                DatabaseManager::configFields
                                               };

void DatabaseManager::openDatabase(){
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
    executeScript(":/create-script.sql", querry);
}

bool DatabaseManager::executeScript(QString filename, QSqlQuery &query){
    //opens file
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString script = file.readAll();
    file.close();

    QVector<QStringRef> statements = script.splitRef(";", QString::SkipEmptyParts);

    //for each statement
    for(auto it = statements.begin(); it < statements.end(); it++){
        QString finalStatement;
        QVector<QStringRef> lines = (*it).split('\n', QString::SkipEmptyParts);
        //for each line split comments
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

        if(finalStatement.isEmpty()){
            continue;
        }

        //Appends final ';'
        finalStatement.append(';');

        //Executes statement
        int ret = query.exec(finalStatement);
        if(!ret){
            qDebug() << "Error processing statement number " << it-statements.begin()+1 << " from file " << filename <<".";
            qDebug() << "Details: " << query.lastError().text();
            return false;
        }
    }
    return true;
}

void DatabaseManager::closeDatabase(){
    if(QSqlDatabase::contains()){
        QSqlDatabase::database().close();
    }

}


//Articles management
//GENERAL PURPOSE FUNCTIONS
bool        DatabaseManager::hasArticle(const Article &a){
    QSqlQuery query;
    query.prepare("SELECT Name FROM Articles WHERE Name=:name;");
    query.bindValue(":name", a.getName());
    bool ret = query.exec();//QString("SELECT Name FROM Articles WHERE Name='").append(a).append("';"));
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
    query.prepare(QString("UPDATE Articles SET ").append(field).append("=:value WHERE Name=:name;"));

    //Manages the particular case of the fields function
    if(field == "function"){
        query.bindValue(":value", QString("(SELECT Id FROM Functions WHERE name='").append(value.toString()).append("')"));
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
        query.prepare(QString("SELECT ").append(field).append(" FROM Articles WHERE Name=:name;"));
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
    query.prepare("UPDATE Clients SET ?=? WHERE Name=?;");
    query.addBindValue(field);
    query.addBindValue(value);
    query.addBindValue(c.getName());
    bool ret = query.exec();
    if(!ret){
        qDebug() << "Error updating field " << field << " with value " << value << " for client " << c;
        qDebug() << query.lastError().text();
    }
    return ret;
}

QVariant    DatabaseManager::getClientField             (const Client &c, QString field){
    QSqlQuery query;

    query.prepare(QString("SELECT ").append(field).append(" FROM Clients WHERE Name=:name;"));
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
uint        DatabaseManager::addFunction            (QString name){
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

uint DatabaseManager::hasFunction(QString name){
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

QList<QString> DatabaseManager::getFunctions(){
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

void DatabaseManager::delFunction(QString name){
    QSqlQuery query;
    query.prepare("DELETE FROM Functions WHERE name=?;");
    query.addBindValue(name);
    int ret = query.exec();
    if(!ret){
        qDebug() << "Deleting function failed.";
        qDebug() << query.lastError().text();
    }
}
