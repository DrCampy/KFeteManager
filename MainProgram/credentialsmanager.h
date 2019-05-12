#ifndef CREDENTIALSMANAGER_H
#define CREDENTIALSMANAGER_H

#include <QString>


class CredentialsManager
{
public:
    CredentialsManager();
    static bool exists(QString user);
    static bool check(QString user, QString password);
    static bool add(QString user, QString password);
    static bool remove(QString user);
    static bool update(QString user, QString password);


private:
    const QString PROGRAM = "./credentials";

};

#endif // CREDENTIALSMANAGER_H
