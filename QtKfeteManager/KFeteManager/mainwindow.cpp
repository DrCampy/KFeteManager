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
#include "clientmanager.h"
#include "countmoney.h"

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
    action = fileMenu->addAction(tr("Gérer la base de données"), this, "SLOT(manageDB())");

    QMenu *editMenu = menuBar()->addMenu(tr("&Editer"));
    action = editMenu->addAction(tr("Editer la &carte"));
    action = editMenu->addAction(tr("Editer le c&atalogue"));
    action = editMenu->addAction(tr("Editer les C&lients"));

    QMenu *managementMenu = menuBar()->addMenu(tr("&Gestion"));
    action = managementMenu->addAction(tr("Effectuer les &paiements"));
    managementMenu->addAction(tr("Statistiques financières"));

    //TODO button about

    //Check that we have an open session
    QVariant openSession = DatabaseManager::getCurrentSession();

    //TODO do not ask but open the view to count cash.
    //Set the current session only after cash register counted.
    //Display a message to ask if the last session is older than 12 hours.
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
    center->addWidget(new ClientManager(center));
    center->addWidget(new CountMoneyBefore(center));
    center->addWidget(new CountMoneyAfter(center));

    center->setCurrentIndex(0);
    this->setCentralWidget(center);

    updateClock();
    updateAccountLabel("");

    //TODO fixme
    connect(accountLabel, SIGNAL(clearAccountSelection()), this, SLOT(receiveRandomEvent()));
    connect(center->widget(1), SIGNAL(finished()), this, SLOT(backToSales()));
    connect(center->widget(2), SIGNAL(finished()), this, SLOT(backToSales()));
    connect(center->widget(3), SIGNAL(finished()), this, SLOT(backToSales()));
    connect(action, SIGNAL(triggered()), this, SLOT(closeSession()));
    connect(action, SIGNAL(triggered()), this, SLOT(manageDB()));
    connect(action, SIGNAL(triggered()), this, SLOT(editCarte()));
    connect(action, SIGNAL(triggered()), this, SLOT(editCatalog()));
    connect(action, SIGNAL(triggered()), this, SLOT(editClient()));
    connect(action, SIGNAL(triggered()), this, SLOT(payJobists()));
    connect(action, SIGNAL(triggered()), this, SLOT(statistics()));
    connect(center->widget(0), SIGNAL(countBefore()), this, SLOT(countBefore()));
    connect(center->widget(0), SIGNAL(countAfter()), this, SLOT(countAfter()));
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
    if((eve->button() == Qt::LeftButton) && this->rect().contains(posMouse)){
        emit clearAccountSelection();
    }
}

void MainWindow::closeSession(){
    emit receiveRandomEvent();
}

void MainWindow::manageDB(){
    emit receiveRandomEvent();
}

void MainWindow::backToSales(){
    center->setCurrentIndex(0);
}

void MainWindow::editCatalog(){
    center->setCurrentIndex(1);
}

void MainWindow::editCarte(){
    center->setCurrentIndex(2);
}

void MainWindow::editClient(){
    center->setCurrentIndex(3);
}

void MainWindow::countBefore(){
    disconnect(center->widget(4), SIGNAL(validated(qreal, QList<Client>)), this, SLOT(newSessionCreated(qreal, QList<Client>)));
    center->setCurrentIndex(4);
    connect(center->widget(4), SIGNAL(validated(qreal, QList<Client>)), this, SLOT(countBeforeFinished(qreal, QList<Client>)));
}

void MainWindow::countAfter(){
    center->setCurrentIndex(5);
    connect(center->widget(5), SIGNAL(validated(qreal)), this, SLOT(countAfterFinished(qreal)));

}

void MainWindow::createNewSession(){
    disconnect(center->widget(4), SIGNAL(validated(qreal, QList<Client>)), this, SLOT(countBeforeFinished(qreal, QList<Client>)));
    center->setCurrentIndex(4);
    connect(center->widget(4), SIGNAL(validated(qreal, QList<Client>)), this, SLOT(newSessionCreated(qreal, QList<Client>)));
}

void MainWindow::countBeforeFinished(qreal count, QList<Client> jobists){
    center->setCurrentIndex(0);
    DatabaseManager::setCurrentSessionjobists(jobists);
    DatabaseManager::setCurrentSessionOpenAmount(count);
}

void MainWindow::countAfterFinished(qreal count){
    center->setCurrentIndex(0);
    DatabaseManager::closeSession(count);
}

void MainWindow::newSessionCreated(qreal count, QList<Client> jobists){
    center->setCurrentIndex(0);
    DatabaseManager::newSession(count, jobists);
}
