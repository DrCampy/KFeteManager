#include "credentialsmanager.h"
#include <QProcess>
#include <QDebug>


/**
 * @brief CredentialsManager::exists checks if an entry exists for that user.
 * @param user: the user to check for existence.
 * @return true if the user exists, false otherwise
 */
bool CredentialsManager::exists(QString user){
    QProcess *credentials = new QProcess();
    credentials->setProgram("./credentials");
    credentials->setArguments({"EXISTS", "accounts", user});
    credentials->start(QProcess::ReadOnly);
    if(!credentials->waitForFinished(10000)){
        qDebug() << "Wait for finished returned false.";
        return false;
    }
    QByteArray errors = credentials->readAllStandardError();
    if(errors.length() > 0){
        qDebug() << errors;
        return false;
    }
    if(credentials->exitStatus() == QProcess::NormalExit && credentials->exitCode() == 0){
        return true;
    }
    return false;
}
