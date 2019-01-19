#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QDateTime>
#include <QMenuBar>
#include <QStackedWidget>

#include "salesview.h"
#include "loginview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QStackedWidget *center;
    QDateTime date;
    QLabel *clockLabel;
    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);

public slots:
    void updateClock();
    //void enteredPassword(QString);
    void backToSales();

    //From the menu
    void closeSession();
    void manageDB();
    void editCarte();
    void editCatalog();
    void editClient();
    void countBefore();
    void countAfter();
    void countBeforeFinished();
    void countAfterFinished();
    void newSessionCreated();
    void createNewSession();
    void manageOrders();
    void about();
    void aboutQT();
    //void payJobists();
    //void statistics();

};

#endif // MAINWINDOW_H
