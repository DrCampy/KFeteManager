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
        QStringList() << "Name" << "Id";

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
    executeScript("./data/scripts/create.sql", querry);
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

bool DatabaseManager::hasArticle(const Article &a){
    QSqlQuery query;
    query.prepare("SELECT Name FROM Articles WHERE Name=?;");
    query.addBindValue(a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Finding article " << a << " failed.";
        qDebug() << query.lastError().text();
        return false;
    }
    if(query.size() == 1){
        return true;
    }else if(query.size() == 0){
        return false;
    }

    //this should never happen because name is a primary key.
    qDebug() << "Error: found " << query.size() << " entries for article " << a << " but expected only one.";
    return true;
}

/*
 * Adds an article to the database.
 * If the database already contains the article,
 * updates the value of the existing entry.
 */
bool DatabaseManager::addArticle(const Article &a, qreal price, qreal jShare, qreal bPrice, qreal redPrice, QString function){
    QSqlQuery query;
    query.prepare("INSERT OR UPDATE INTO Articles(Name, sellPrice, jShare, bPrice, reducedPrice, function)"
                  "VALUES(?, ?, ?, ?, ?, ?);");
    query.addBindValue(a.getName());
    query.addBindValue(price);
    query.addBindValue(jShare);
    query.addBindValue(bPrice);
    query.addBindValue(redPrice);
    query.addBindValue(function);
    int ret = query.exec();
    if(!ret){
        qDebug() << "Inserting article " << a << " failed.";
        qDebug() << query.lastError().text();
    }
    return ret;
}

void DatabaseManager::delArticle(const Article &a){
    QSqlQuery query;
    query.prepare("DELETE FROM Articles WHERE Name=?;");
    query.addBindValue(a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Deleting article " << a << " failed.";
        qDebug() << query.lastError().text();
    }
}

qreal DatabaseManager::getArticlePrice(const Article &a){
    QSqlQuery query;
    query.prepare("SELECT sellPrice FROM Articles WHERE Name=?;");
    query.addBindValue(a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting price of article " << a << " failed.";
        qDebug() << query.lastError().text();
        return qQNaN();
    }
    if(query.size() == 0)
        return qQNaN();
    return query.value(0).toReal();
}

qreal DatabaseManager::getArticleReducedPrice(const Article &a){
    QSqlQuery query;
    query.prepare("SELECT reducedPrice FROM Articles WHERE Name=?;");
    query.addBindValue(a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting reduced price of article " << a << " failed.";
        qDebug() << query.lastError().text();
        return qQNaN();
    }
    if(query.size() == 0)
        return qQNaN();
    return query.value(0).toReal();
}

qreal DatabaseManager::getArticleJobistShare(const Article &a){
    QSqlQuery query;
    query.prepare("SELECT jShare FROM Articles WHERE Name=?;");
    query.addBindValue(a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting jobist share of article " << a << " failed.";
        qDebug() << query.lastError().text();
        return qQNaN();
    }
    if(query.size() == 0)
        return qQNaN();
    return query.value(0).toReal();
}

qreal DatabaseManager::getArticleBuyingPrice(const Article &a){
    QSqlQuery query;
    query.prepare("SELECT bPrice FROM Articles WHERE Name=?;");
    query.addBindValue(a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting buying price of article " << a << " failed.";
        qDebug() << query.lastError().text();
        return qQNaN();
    }
    if(query.size() == 0)
        return qQNaN();
    return query.value(0).toReal();
}

QString DatabaseManager::getArticleFunction(const Article &a){
    QSqlQuery query;
    query.prepare("SELECT Functions.Name "
                  "FROM( SELECT Name, function  FROM Articles WHERE Name=?) "
                  "INNER JOIN Functions ON Articles.function=Functions.Name);");
    query.addBindValue(a.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting function of article " << a << " failed.";
        qDebug() << query.lastError().text();
        return "";
    }
    if(query.size() == 0)
        return "";
    return query.value(0).toString();
}

bool DatabaseManager::hasClient(const Client &c){
    QSqlQuery query;
    query.prepare("SELECT Name FROM Clients WHERE Name=?;");
    query.addBindValue(c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Finding client" << c << " failed.";
        qDebug() << query.lastError().text();
        return false;
    }
    if(query.size() == 1){
        return true;
    }else if(query.size() == 0){
        return false;
    }

    //this should never happen because name is a primary key.
    qDebug() << "Error: found " << query.size() << " entries for client " << c << " but expected only one.";
    return true;
}

bool DatabaseManager::addClient(const Client &c, QString phone, QString address, QString email, qreal negLimit, bool isJobist){
    QSqlQuery query;
    query.prepare("INSERT OR UPDATE INTO Clients(Name, phone, address, email, negLimit, isJobist) VALUES(?,?,?,?,?);");
    query.addBindValue(c.getName());
    query.addBindValue(phone);
    query.addBindValue(address);
    query.addBindValue(email);
    query.addBindValue(negLimit);
    query.addBindValue(isJobist);
    int ret = query.exec();
    if(!ret){
        qDebug() << "Inserting " << c << " failed.";
        qDebug() << query.lastError().text();
    }
    return ret;
}

void DatabaseManager::delClient(const Client &c){
    QSqlQuery query;
    query.prepare("DELETE FROM Clients WHERE Name=?;");
    query.addBindValue(c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Deleting client" << c << " failed.";
        qDebug() << query.lastError().text();
    }
}

QString DatabaseManager::getClientPhone(const Client &c){
    QSqlQuery query;
    query.prepare("SELECT phone FROM Clients WHERE Name=?;");
    query.addBindValue(c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting phone of client " << c << " failed.";
        qDebug() << query.lastError().text();
        return "";
    }
    if(query.size() == 0)
        return "";
    return query.value(0).toString();
}


QString DatabaseManager::getClientAddress(const Client &c){
    QSqlQuery query;
    query.prepare("SELECT address FROM Clients WHERE Name=?;");
    query.addBindValue(c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting address of client " << c << " failed.";
        qDebug() << query.lastError().text();
        return "";
    }
    if(query.size() == 0)
        return "";
    return query.value(0).toString();
}

QString DatabaseManager::getClientEmail(const Client &c){
    QSqlQuery query;
    query.prepare("SELECT email FROM Clients WHERE Name=?;");
    query.addBindValue(c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting email of client " << c << " failed.";
        qDebug() << query.lastError().text();
        return "";
    }
    if(query.size() == 0)
        return "";
    return query.value(0).toString();
}

qreal DatabaseManager::getClientLimit(const Client &c){
    QSqlQuery query;
    query.prepare("SELECT negLimit FROM Clients WHERE Name=?;");
    query.addBindValue(c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting limit of client " << c << " failed.";
        qDebug() << query.lastError().text();
        return qQNaN();
    }
    if(query.size() == 0)
        return qQNaN();
    return query.value(0).toReal();
}

bool DatabaseManager::isClientJobist(const Client &c){
    QSqlQuery query;
    query.prepare("SELECT isJobist FROM Clients WHERE Name=?;");
    query.addBindValue(c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting limit of client " << c << " failed.";
        qDebug() << query.lastError().text();
        return false;
    }
    if(query.size() == 0)
        return false;

    //SQlite does not support bool. Stores integer 1 for of true, 0 for false.
    return (query.value(0).toInt() == 1);
}

qreal DatabaseManager::getClientBalance(const Client &c){
    QSqlQuery query;
    query.prepare("SELECT balance FROM Clients WHERE Name=?;");
    query.addBindValue(c.getName());
    int ret = query.exec();
    if(!ret){
        qDebug() << "Getting balance of client " << c << " failed.";
        qDebug() << query.lastError().text();
        return qQNaN();
    }
    if(query.size() == 0)
        return qQNaN();
    return query.value(0).toReal();
}

/*
 * Inserts a new function to a database.
 * If the database already contains this function
 * then the functions return the ID already allocated to that function.
 * Comparison of functions is NOT CasE SeNsiTIvE
 */
unsigned long int DatabaseManager::addFunction(QString name){
    name = name.trimmed();

}
