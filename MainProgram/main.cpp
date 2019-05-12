#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "databasemanager.h"
#include "historiesmanager.h"

/**
 * @brief qMain starts the program.
 * Opens the different databases, performs checks (procedure will change).
 * Loads the QSettings for the application, runs the application then close databases.
 *
 * @param argc Number of arguments
 * @param argv Arguments
 * @return EXIT_SUCCESS in case of success. EXIT_FAILURE otherwise.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if(!QDir("./data").exists()){
        QDir().mkdir("./data");
    }

    //Opens databases
    DatabaseManager::openDatabase();
    HistoriesManager::openHistoryDb();

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
        if(!DatabaseManager::checkDatabase()){
            qDebug() << "Unable to create Database";
            return EXIT_FAILURE;
        }
    }

    app.setOrganizationName("Morgan Diepart");
    app.setApplicationName("KFeteManager");

    //runs application
    MainWindow w;
    w.show();

    //Executes application
    int ret = app.exec();

    //Closes Database
    DatabaseManager::closeDatabase();
    HistoriesManager::closeHistoryDb();

    return ret;
}
