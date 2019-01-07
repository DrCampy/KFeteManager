#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QStatusBar>
#include <QMouseEvent>
#include <QTimer>
#include <QStackedWidget>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>

#include "mainwindow.h"
#include "loginview.h"
#include "salesview.h"
#include "catalogmanager.h"
#include "cartemanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    center = new QStackedWidget(this);
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

    //Check that we have an open session
    QVariant openSession = DatabaseManager::getCurrentSession();

    //if openSession is null we have no open session. Asks if we have to open one.
    if(openSession.isNull()){
        QMessageBox messageBox;
        messageBox.setText(tr("Aucune session n'est actuellement ouverte."));
        messageBox.setInformativeText(tr("Voulez-vous ouvrir une nouvelle session de vente ?"));
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        messageBox.setDefaultButton(QMessageBox::Yes);
        int ret = messageBox.exec();
        if(ret == QMessageBox::Yes){
            DatabaseManager::newSession(QVariant());
        }
    }

    statusBar()->addWidget(clockLabel);
    statusBar()->addPermanentWidget(accountLabel);
    SalesView *salesView = new SalesView(center);
    center->addWidget(salesView);
    center->addWidget(new CatalogManager(center));
    center->addWidget(new CarteManager(salesView->getCarteModel(), center));
    center->setCurrentIndex(0);
    this->setCentralWidget(center);



    updateClock();
    updateAccountLabel("");

    //TODO fixme
    connect(accountLabel, SIGNAL(clearAccountSelection()), this, SLOT(receiveRandomEvent()));
    connect(center->widget(1), SIGNAL(finished()), this, SLOT(backToSales()));
    connect(center->widget(2), SIGNAL(finished()), this, SLOT(backToSales()));
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

void MainWindow::editCarte(){
    center->setCurrentIndex(2);
}

void MainWindow::editCatalog(){
    center->setCurrentIndex(1);
}

void MainWindow::backToSales(){
    center->setCurrentIndex(0);
}
