#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QSqlQueryModel>
#include <QString>
#include <QDateTime>
#include <QLocale>

#include "sessionsmanager.h"
#include "customwidgets.h"

SessionsManager::SessionsManager(QWidget *parent) : QWidget(parent)
{
    sessionSelector     = new QComboBox(this);
    sessionDetails      = new QPlainTextEdit(this);
    backButton          = new QPushButton(tr("Retour"), this);
    validate1Button     = new QPushButton(tr("Valider automatiquement"), this);
    validate2Button     = new QPushButton(tr("Valider Manuellement"), this);
    validate3Button     = new QPushButton(tr("Valider sans payer"), this);
    sessionsModel       = new QSqlQueryModel();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttons    = new QHBoxLayout();

    sessionSelector->setItemDelegate(new DateDelegate(sessionSelector));
    sessionSelector->setModel(sessionsModel);
    sessionSelector->setCurrentIndex(-1); //selects nothing.

    sessionDetails->setReadOnly(true);

    buttons->addWidget(validate1Button);
    buttons->addWidget(validate2Button);
    buttons->addWidget(validate3Button);
    buttons->addWidget(backButton);

    mainLayout->addWidget(sessionSelector);
    mainLayout->addWidget(sessionDetails);
    mainLayout->addLayout(buttons);

    //Connects
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(finished()));
    connect(sessionSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(writeDetails()));
    refresh();
}

SessionsManager::~SessionsManager(){
    delete sessionsModel;
}

void SessionsManager::refresh(){
    sessionsModel->setQuery("SELECT openingTime FROM saleSessions WHERE state='closed' ORDER BY openingTime ASC;");
}

void SessionsManager::writeDetails(){
    QString text; text.reserve(1500);
    qlonglong sessionID;
    QDateTime sessionTime;
    QSqlQuery query;
    QStringList jobists;
    QLocale locale;
    //Name - Quantity
    QMap<QString, uint> normalSales, reducedSales, freeSales;

    //Name - amount
    QMultiMap<QString, QPair<qreal, QString>> clientMoves;
    QMap<QString, qreal> functionsBenefits;

    QList<QPair<qreal, QString>> cashRegisterMoves;
    qreal totalSales = 0, totJShare = 0;
    QVariant  countBefore, countAfter;
    //Loads session time
    sessionID = this->sessionSelector->currentData(Qt::EditRole).toLongLong();
    sessionTime.setSecsSinceEpoch(sessionID);

    //loads jobists
    query.prepare("SELECT name FROM heldSession WHERE sessionTime=:id ORDER BY name ASC;");
    query.bindValue(":id", sessionID);
    query.exec();
    while(query.next()){
        jobists << query.value(0).toString();
    }

    //Loads count before and after sales
    query.prepare("SELECT openAmount, closeAmount FROM SaleSessions WHERE OpeningTime = :id;");
    query.bindValue(":id", sessionID);
    query.exec();
    if(query.first()){
        countBefore = query.value(0).toDouble();
        countAfter = query.value(1).toDouble();
    }

    //load sold items and jobistShare
    //0 = article, 1 = nb_article, 2 = jobShare per article, 3 = price bought, 4 = price sold
    //5 = function name

    //Normal price sales
    query.prepare("SELECT OrderContent.article, SUM(OrderContent.quantity) AS tot_qtt, "
                  "Articles.jShare, Articles.bPrice, Articles.sellPrice AS soldPrice, "
                  "Functions.name AS Function "
                  "FROM Transactions "
                  "INNER JOIN OrderContent ON Transactions.Id = OrderContent.Id "
                  "INNER JOIN Articles ON OrderContent.article = Articles.name "
                  "INNER JOIN Functions ON Articles.function = Functions.Id "
                  "WHERE Transactions.sessionTime = :id AND OrderContent.Id IN (SELECT Id FROM IsOrder WHERE price = 'normal') "
                  "GROUP BY OrderContent.article "
                  "ORDER BY tot_qtt DESC;");
    query.bindValue(":id", sessionID);
    query.exec();
    while(query.next()){
        const QString name = query.value(0).toString();
        const uint nb = query.value(1).toUInt();
        const qreal artJShare = query.value(2).toDouble();
        const qreal boughtPrice = query.value(3).toDouble();
        const qreal soldPrice = query.value(4).toDouble();
        const QString function = query.value(5).toString();

        //Register sale
        normalSales.insert(name, nb);

        //Computes jobist share
        totJShare += (artJShare * nb);

        //Computes function benefits
        functionsBenefits[function] += nb*(soldPrice - boughtPrice - artJShare);
    }

    //Reduced price sales
    query.prepare("SELECT OrderContent.article, SUM(OrderContent.quantity) AS tot_qtt, "
                  "Articles.jShare, Articles.bPrice, Articles.reducedPrice AS soldPrice, "
                  "Functions.name AS Function "
                  "FROM Transactions "
                  "INNER JOIN OrderContent ON Transactions.Id = OrderContent.Id "
                  "INNER JOIN Articles ON OrderContent.article = Articles.name "
                  "INNER JOIN Functions ON Articles.function = Functions.Id "
                  "WHERE Transactions.sessionTime = :id AND OrderContent.Id IN (SELECT Id FROM IsOrder WHERE price = 'reduced') "
                  "GROUP BY OrderContent.article "
                  "ORDER BY tot_qtt DESC;");
    query.bindValue(":id", sessionID);
    query.exec();
    while(query.next()){
        const QString name = query.value(0).toString();
        const uint nb = query.value(1).toUInt();
        const qreal artJShare = query.value(2).toDouble();
        const qreal boughtPrice = query.value(3).toDouble();
        const qreal soldPrice = query.value(4).toDouble();
        const QString function = query.value(5).toString();

        //Register sale
        reducedSales.insert(name, nb);

        //Computes jobist share
        totJShare += (artJShare * nb);

        //Computes function benefits
        functionsBenefits[function] += nb*(soldPrice - boughtPrice - artJShare);
    }

    //free sales
    query.prepare("SELECT OrderContent.article, SUM(OrderContent.quantity) AS tot_qtt, "
                  "Articles.bPrice, Functions.name AS Function "
                  "FROM Transactions "
                  "INNER JOIN OrderContent ON Transactions.Id = OrderContent.Id "
                  "INNER JOIN Articles ON OrderContent.article = Articles.name "
                  "INNER JOIN Functions ON Articles.function = Functions.Id "
                  "WHERE Transactions.sessionTime = :id AND OrderContent.Id IN (SELECT Id FROM IsOrder WHERE price = 'free') "
                  "GROUP BY OrderContent.article "
                  "ORDER BY tot_qtt DESC;");
    query.bindValue(":id", sessionID);
    query.exec();
    while(query.next()){
        const QString name = query.value(0).toString();
        const uint nb = query.value(1).toUInt();
        const qreal boughtPrice = query.value(2).toDouble();
        const QString function = query.value(3).toString();

        //Register sale
        freeSales.insert(name, nb);

        //No jobist share

        //Computes function benefits
        functionsBenefits[function] -= nb*(boughtPrice);
    }

    //load cash moves
    query.prepare("SELECT CashMoves.client, Transactions.total, CashMoves.note "
                  "FROM Transactions "
                  "INNER JOIN CashMoves ON Transactions.Id = CashMoves.Id "
                  "WHERE Transactions.sessionTime = :id;");
    query.bindValue(":id", sessionID);
    query.exec();
    while(query.next()){
        if(query.value(0).isNull()){
            //(cash register)
            cashRegisterMoves.append(QPair<qreal, QString>(query.value(1).toDouble(), query.value(2).toString()));
        }else{
            //(accounts)
            clientMoves.insert(query.value(0).toString(), QPair<qreal, QString>(query.value(1).toDouble(), query.value(2).toString()));
        }
    }

    //load total
    query.prepare("SELECT SUM(transactions.total) AS total "
                  "FROM Transactions "
                  "WHERE "
                  "Transactions.sessionTime = :id "
                  "AND "
                  "Transactions.Id "
                  "IN "
                  "(SELECT Id "
                  " FROM IsOrder "
                  " WHERE Id NOT IN (SELECT Id FROM OrderClient) "
                  " UNION "
                  " SELECT Id FROM CashMoves); ");
    query.bindValue(":id",sessionID);
    query.exec();
    if(query.first()){
        totalSales = query.value(0).toDouble();
    }

    //Builds the text summary
    //Header
    text += "<h1><p>" + tr("Session de vente du ") + locale.toString(sessionTime) + "</p></h1>";
    if(!jobists.isEmpty()){
        text += "<i><p>" + tr("Session tenue par :") + "</br>";
        for(auto it : jobists){
            text += it + "<br/>"; //Adds jobists
        }
        text += "</p></i>";
    }

    //Summary of the state of the cash register
    text += "<h2>" + tr("État de caisse :") + "</h2>";
    text += "<p>" + tr("Au début : ") + (countBefore.isNull()?"<em>"+tr("Non comptée")+"</em>":locale.toCurrencyString(countBefore.toDouble())) + "</p>";
    text += "<p>" + tr("Total des recettes : ") + locale.toCurrencyString(totalSales) + "</p>";
    text += "<p>" + tr("À la fin : ") + (countAfter.isNull()?"<em>"+tr("Non comptée")+"</em>":locale.toCurrencyString(countAfter.toDouble())) + "</p>";
    if(!countAfter.isNull()){
        text += "<p>" + tr("Solde final théorique : ") + locale.toCurrencyString(countBefore.toDouble() + totalSales) + "</p>";
    }

    //Cash moves

    text += "<h2>" + tr("Mouvements en caisse :") + "</h2>";
    if(cashRegisterMoves.isEmpty()){
        text += "<em>" + tr("Pas de mouvements.") + "</em>";
    }else{
        text += "<ul>";
        for(auto it : cashRegisterMoves){
            text += "<li>";
            if(it.first < 0){
                text += tr("Retrait de ") + locale.toCurrencyString(-it.first) + " en caisse : " + it.second;
            }else{
                text += tr("Dépôt de ") + locale.toCurrencyString(it.first) + " en caisse : " + it.second;
            }
            text += "</li>";
        }
        text += "</ul>";
    }

    text += "<h2>" + tr("Mouvements sur les comptes :") + "</h2>";
    if(clientMoves.isEmpty()){
        text += "<em>" + tr("Pas de mouvements.") + "</em>";
    }else{
        for(auto client : clientMoves.uniqueKeys()){
            text += "<li>" + tr("Sur le compte de : ") + client + "<ul>";
            for(auto moves : clientMoves.values(client)){
                text += "<li>";
                if(moves.first < 0){
                    text += tr("Retrait de ") + locale.toCurrencyString(-moves.first) + " : " + moves.second;
                }else{
                    text += tr("Dépôt de ") + locale.toCurrencyString(moves.first) + " : " + moves.second;
                }
                text += "</li>";
            }
            text += "</ul>";
        }
    }

    //Normal price sales
    text += "<h2>" + tr("Ventes au tarif normal :") + "</h2>";
    text += "<ul>";
    for(auto item : normalSales.keys()){
        text += "<li>" + item + " x " + QString::number(normalSales.value(item)) + "</li>";
    }
    text += "</ul>";

    //Reduced sales
    if(!reducedSales.isEmpty()){
        text += "<h2>" + tr("Ventes au tarif réduit :") + "</h2>";
        text += "<ul>";
        for(auto item : reducedSales.keys()){
            text += "<li>" + item + " x " + QString::number(reducedSales.value(item)) + "</li>";
        }
        text += "</ul>";
    }

    //Free sales
    if(!freeSales.isEmpty()){
        text += "<h2>" + tr("Ventes offertes :") + "</h2>";
        text += "<ul>";
        for(auto item : freeSales.keys()){
            text += "<li>" + item + " x " + QString::number(freeSales.value(item)) + "</li>";
        }
        text += "</ul>";
    }

    //Functions
    text += "<h2>" + tr("Bénéfices des différentes fonctions :") + "</h2>";
    text += "<ul>";
    for(auto function : functionsBenefits.keys()){
        text += "<li>" + function + tr(" : ") + locale.toCurrencyString(functionsBenefits.value(function)) + "</li>";
    }
    text += "</ul>";

    //Jobists share
    text += "<h2>" + tr("Part des jobistes : ") + "</h2>";
    text += locale.toCurrencyString(totJShare);

    sessionDetails->clear();
    sessionDetails->appendHtml(text);
}


//NOTE TODO refresh account button once validate is pressed.
//Note display jobist share (and functions benef ?) in order summary
//Not update order summary when using keyboard arrows
