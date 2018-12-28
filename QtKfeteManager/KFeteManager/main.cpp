#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "mainwindow.h"
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!QDir("./data").exists()){
        QDir().mkdir("./data");
    }
    //Opens database
    DatabaseManager::openDatabase();

    //Checks if database is OK
    if(!DatabaseManager::checkDatabase()){
        //If database not OK, proposes to create it.
        QMessageBox ask;
        ask.setText(QObject::tr("La base de donnée semble corrompue."));
        ask.setInformativeText(QObject::tr("La recréer peut résoudre le problème mais peut causer la perte des données. La recréer ?"));
        ask.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        ask.setDefaultButton(QMessageBox::No);
        int ret = ask.exec();
        if(ret == QMessageBox::No){
            return EXIT_FAILURE;
        }
        DatabaseManager::createDatabase();

    }

    //runs application
    MainWindow w;
    w.show();

    //Executes application
    int ret = a.exec();

    //Closes Database
    DatabaseManager::closeDatabase();

    return ret;
}
