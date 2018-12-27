#include <QApplication>
#include <QDir>

#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!QDir("./data").exists()){
        QDir().mkdir("./data");
    }
    MainWindow w;
    w.show();

    return a.exec();
}
