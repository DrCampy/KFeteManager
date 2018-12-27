#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>


class DatabaseManager : public QObject
{
    Q_OBJECT
private:
    static QStringList tables;
    tables << "Articles" << "Functions" << "Clients"
                   << "SaleSessions" <<"HeldSession" <<"FunctionBenefits";

    static QStringList articlesFields, functionsFields, clientsFields,
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
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    static bool checkDatabase();
    static void openDatabase();
    static void closeDatabase();
};

#endif // DATABASEMANAGER_H
