#include <QApplication>
#include <QDir>

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
    if(DatabaseManager::checkDatabase()){
        //runs application
        MainWindow w;
        w.show();
    }

    return a.exec();
}
