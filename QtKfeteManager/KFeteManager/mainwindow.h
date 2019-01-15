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
    QStackedWidget *center;
    QDateTime date;
    QLabel *clockLabel;
    PushLabel *accountLabel;

public slots:
    void updateClock();
    void updateAccountLabel(QString);
    //void enteredPassword(QString);
    void receiveRandomEvent(); //TODO remove
    void backToSales();

    //From the menu
    void closeSession();
    void manageDB();
    void editCarte();
    void editCatalog();
    void editClient();
    void countBefore();
    void countAfter();
    void countBeforeFinished(qreal count, QList<Client> jobists);
    void countAfterFinished(qreal count);
    void newSessionCreated(qreal count, QList<Client> jobists);
    void createNewSession();
    //void payJobists();
    //void statistics();

};

#endif // MAINWINDOW_H
