#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QStatusBar>
#include <QMouseEvent>
#include <QTimer>

#include "mainwindow.h"
#include "loginview.h"
#include "salesview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    clockLabel = new QLabel(this);
    accountLabel = new PushLabel(this);
    accountLabel->setToolTip(tr("Cliquez pour supprimer"));
    QTimer *clockTimer = new QTimer(this);
    clockTimer->start(5000);
    connect(clockTimer, SIGNAL(timeout()), this, SLOT(updateClock()));

    updateClock();
    updateAccountLabel("");

    statusBar()->addWidget(clockLabel);
    statusBar()->addPermanentWidget(accountLabel);

    this->setCentralWidget(new SalesView(this));

    //TODO fixme
    connect(accountLabel, SIGNAL(clearAccountSelection()), this, SLOT(receiveRandomEvent()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateClock(){
    date = QDateTime(QDateTime::currentDateTime());
    clockLabel->setText(date.toString("ddd d/M/yyyy H:mm"));
}

void MainWindow::updateAccountLabel(QString account){
    QString s("Compte sélectionné : ");
    if(account.size() == 0){
        s.append("<i>");
        s.append(tr("Aucun"));
        s.append("</i>");
    }
    else{
        s.push_front("<font color=#dd2828>");
        s.append(account);
        s.append("</font>");
    }
    accountLabel->setText(s);
}

//TODO remove
void MainWindow::receiveRandomEvent(){
    statusBar()->showMessage("Event !", 1000);
}

PushLabel::PushLabel(QWidget *parent) : QLabel(parent){}

void PushLabel::mouseReleaseEvent(QMouseEvent *eve){
    QPoint posMouse = eve->pos();
    if( (eve->button() == Qt::LeftButton) && this->rect().contains(posMouse)){
        emit clearAccountSelection();
    }
}
