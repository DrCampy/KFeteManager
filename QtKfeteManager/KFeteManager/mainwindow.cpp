#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QStatusBar>
#include <QMouseEvent>
#include <QTimer>

#include <QMenuBar>
#include <QMenu>

#include "mainwindow.h"
#include "loginview.h"
#include "salesview.h"
#include "catalogmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    clockLabel = new QLabel(this);
    accountLabel = new PushLabel(this);
    accountLabel->setToolTip(tr("Cliquez pour supprimer"));
    QTimer *clockTimer = new QTimer(this);
    clockTimer->start(5000);
    connect(clockTimer, SIGNAL(timeout()), this, SLOT(updateClock()));

    this->setMenuBar(new QMenuBar()); //Takes ownership

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *action = fileMenu->addAction(tr("&Fermer la session de vente"));
    connect(action, SIGNAL(triggered()), this, SLOT(closeSession()));

    action = fileMenu->addAction(tr("Gérer la base de données"), this, "SLOT(manageDB())");
    connect(action, SIGNAL(triggered()), this, SLOT(manageDB()));

    QMenu *editMenu = menuBar()->addMenu(tr("&Editer"));
    action = editMenu->addAction(tr("Editer la &carte"));
    connect(action, SIGNAL(triggered()), this, SLOT(editCarte()));

    action = editMenu->addAction(tr("Editer le c&atalogue"));
    connect(action, SIGNAL(triggered()), this, SLOT(editCatalog()));

    QMenu *managementMenu = menuBar()->addMenu(tr("&Gestion"));
    action = managementMenu->addAction(tr("Effectuer les &paiements"));
    connect(action, SIGNAL(triggered()), this, SLOT(payJobists()));

    managementMenu->addAction(tr("Statistiques financières"));
    connect(action, SIGNAL(triggered()), this, SLOT(statistics()));

    //TODO button about

    updateClock();
    updateAccountLabel("");

    statusBar()->addWidget(clockLabel);
    statusBar()->addPermanentWidget(accountLabel);

    sales = new SalesView(this);
    //this->setCentralWidget(sales);
    this->setCentralWidget(new CatalogManager(this));
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

void MainWindow::closeSession(){
    emit receiveRandomEvent();
}

void MainWindow::manageDB(){
    emit receiveRandomEvent();
}
