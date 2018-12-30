#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QDateTime>
#include <QMenuBar>

#include "salesview.h"
#include "accountmanagementview.h"
#include "loginview.h"

namespace Ui {
class MainWindow;
}

class PushLabel : public QLabel{
    Q_OBJECT
public:
    PushLabel(QWidget *parent = Q_NULLPTR);
protected:
    void mouseReleaseEvent(QMouseEvent *eve);
signals:
    void clearAccountSelection();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    SalesView *sales;
    QDateTime date;
    QLabel *clockLabel;
    PushLabel *accountLabel;

public slots:
    void updateClock();
    void updateAccountLabel(QString);
    //void enteredPassword(QString);
    void receiveRandomEvent(); //TODO remove

    //From the menu
    void closeSession();
    void manageDB();
    //void editCarte();
    //void editCatalog();
    //void payJobists();
    //void statistics();

};

#endif // MAINWINDOW_H
