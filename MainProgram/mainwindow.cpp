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
#include <QSettings>
#include <QDateTime>
#include <QSplashScreen>

#include "mainwindow.h"
#include "loginview.h"
#include "salesview.h"
#include "catalogmanager.h"
#include "cartemanager.h"
#include "clientmanager.h"
#include "countmoney.h"
#include "ordermanager.h"
#include "sessionsmanager.h"

/**
 * @brief MainWindow::MainWindow constructs the main window.
 * Manages all the menus in the menu bar, the clock and it's timer and the switching between
 * all the views used during operation of the program.
 *
 * @param parent is the parent widget.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    center = new QStackedWidget(this);
    clockLabel = new QLabel(this);
    QTimer *clockTimer = new QTimer(this);
    clockTimer->start(5000);
    connect(clockTimer, SIGNAL(timeout()), this, SLOT(updateClock()));

    this->setMenuBar(new QMenuBar()); //automatically akes ownership

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *action = fileMenu->addAction(tr("&Fermer la session de vente"));
    connect(action, SIGNAL(triggered()), this, SLOT(closeSession()));

    QMenu *editMenu = menuBar()->addMenu(tr("&Editer"));
    action = editMenu->addAction(tr("Modifier la &carte"));
    connect(action, SIGNAL(triggered()), this, SLOT(editCarte()));

    action = editMenu->addAction(tr("Modifier le c&atalogue"));
    connect(action, SIGNAL(triggered()), this, SLOT(editCatalog()));

    action = editMenu->addAction(tr("Gérer les C&lients"));
    connect(action, SIGNAL(triggered()), this, SLOT(editClient()));

    QMenu *managementMenu = menuBar()->addMenu(tr("&Gestion"));
    action = managementMenu->addAction(tr("Effectuer les &paiements"));
    connect(action, SIGNAL(triggered()), this, SLOT(payJobists()));

    managementMenu->addAction(tr("Statistiques financières"));
    connect(action, SIGNAL(triggered()), this, SLOT(statistics()));

    QMenu *aboutMenu = menuBar()->addMenu(tr("À propos"));
    action = aboutMenu->addAction(tr("À propos du logiciel"));
    connect(action, SIGNAL(triggered()), this, SLOT(about()));

    action = aboutMenu->addAction(tr("À propos de Qt"));
    connect(action, SIGNAL(triggered()), this, SLOT(aboutQT()));

    //Check that we have an open session
    QVariant openSession = DatabaseManager::getCurrentSession();
    if(!openSession.isNull()){
        QDateTime time; time.setSecsSinceEpoch(openSession.toLongLong());
        //If current session is older than 12 hours proposes to create a new one
        if(time.secsTo(QDateTime::currentDateTime()) > 12*3600){
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("Ouvrir une nouvelle session ?"),
                                  tr("La session actuelle a été ouverte il y a plus de 12 heures, "
                                     "voulez-vous en ouvrir une nouvelle ?"));
            if(reply == QMessageBox::Yes){
                QSqlQuery query(QString("SELECT closeAmount FROM SaleSessions WHERE openingTime = ") + QString::number(openSession.toLongLong()) + ";");
                if(query.first()){
                    if(query.value(0).isNull()){
                        reply = QMessageBox::question(this, tr("Ouvrir une nouvelle session ?"),
                                                      tr("La caisse n'a pas été comptée à la fin de l'exercice précédent. ") + tr("Fermer la session quand même ?"));
                    }else{
                        qreal closeAmount = query.value(0).toDouble();
                        reply = QMessageBox::question(this, tr("Ouvrir une nouvelle session ?"),
                                                      tr("Le total en caisse à la fin de l'xercice précédent était de : ") + locale().toCurrencyString(closeAmount) + ". "+tr("Valider ?"));
                    }
                   if(reply == QMessageBox::Yes){
                       DatabaseManager::closeSession();
                   }
                }
            }
        }
    }


    statusBar()->addWidget(clockLabel);
    SalesView *salesView = new SalesView(center);
    /*00*/center->addWidget(salesView);
    /*01*/center->addWidget(new CatalogManager(center));
    /*02*/center->addWidget(new CarteManager(salesView->getCarteModel(), center));
    /*03*/center->addWidget(new ClientManager(center));
    /*04*/center->addWidget(new CountMoneyBefore(center));
    /*05*/center->addWidget(new CountMoneyAfter(center));
    /*06*/center->addWidget(new OrderManager(this));
    /*07*/center->addWidget(new SessionsManager(this));

    center->setCurrentIndex(0);
    this->setCentralWidget(center);

    updateClock();

    connect(center->widget(1), SIGNAL(finished()), this, SLOT(backToSales()));
    connect(center->widget(2), SIGNAL(finished()), this, SLOT(backToSales()));
    connect(center->widget(3), SIGNAL(finished()), this, SLOT(backToSales()));
    connect(center->widget(6), SIGNAL(finished()), this, SLOT(backToSales()));
    connect(center->widget(7), SIGNAL(finished()), this, SLOT(backToSales()));

    connect(center->widget(4), SIGNAL(cancelled()), this, SLOT(backToSales()));
    connect(center->widget(5), SIGNAL(cancelled()), this, SLOT(backToSales()));
    connect(center->widget(5), SIGNAL(validated()), this, SLOT(countAfterFinished()));
    connect(center->widget(4), SIGNAL(validated()), this, SLOT(countBeforeFinished()));

    connect(center->widget(0), SIGNAL(countBefore()), this, SLOT(countBefore()));
    connect(center->widget(0), SIGNAL(countAfter()), this, SLOT(countAfter()));
    connect(center->widget(0), SIGNAL(manageOrders()), this, SLOT(manageOrders()));
    readSettings();

    //If there is no open session or we closed it sooner, create a new one.
    if(DatabaseManager::getCurrentSession().isNull()){
        createNewSession();
    }

}

MainWindow::~MainWindow()
{
}

/**
 * @brief MainWindow::updateClock updates the time displayed by the clock with the current date and time.
 */
void MainWindow::updateClock(){
    date = QDateTime(QDateTime::currentDateTime());
    clockLabel->setText(date.toString("ddd d/M/yyyy H:mm"));
}

/**
 * @brief MainWindow::writeSettings saves the window settings inside QSettings.
 *
 * Parameters saved are : window position, window size and if the window is maximized.
 */
void MainWindow::writeSettings(){
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("isMaximized", isMaximized());
    settings.endGroup();
}

/**
 * @brief MainWindow::readSettings reads the window settings from QSettings.
 *
 * Parameters loaded are : window size, window position and if the window is maximized.
 */
void MainWindow::readSettings(){
    QSettings settings;
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    if(settings.value("isMaximized", false) == true){
        setWindowState(windowState() | Qt::WindowMaximized);
    }
    settings.endGroup();
}

/**
 * @brief MainWindow::closeEvent Intercepts any close event to allow us to save the window settings before leaving.
 * @param event is the event catched.
 */
void MainWindow::closeEvent(QCloseEvent *event){
    writeSettings();
    event->accept();
}

/**
 * @brief MainWindow::closeSession closes a salesession inside the main database.
 */
void MainWindow::closeSession(){
    DatabaseManager::closeSession();
    this->close();
}

/**
 * @brief MainWindow::manageDB [WIP] will be used to perform some operations on the database.
 */
void MainWindow::manageDB(){
}

/**
 * @brief MainWindow::payJobists displays the interface allowing user to pay the jobists.
 */
void MainWindow::payJobists(){
    center->setCurrentIndex(7);
}

/**
 * @brief MainWindow::backToSales is called when user wants to go back to the main sales interface.
 *
 * This could change in the future for a stack-based interfaces history management.
 */
void MainWindow::backToSales(){
    center->setCurrentIndex(0);
}

/**
 * @brief MainWindow::editCatalog displays the interface allowing the user to edit the catalog.
 */
void MainWindow::editCatalog(){
    center->setCurrentIndex(1);
}

/**
 * @brief MainWindow::editCarte displays the interface allowing the user to edit the carte.
 */
void MainWindow::editCarte(){
    center->setCurrentIndex(2);
}

/**
 * @brief MainWindow::editClient displays the interface allowing the user to edit the clients data.
 */
void MainWindow::editClient(){
    center->setCurrentIndex(3);
}

/**
 * @brief MainWindow::countBefore displays the interface allowing the user to register the amount of cash available before begining the sales.
 * Also refreshes the list of jobists shown in the dropdown list of the interface.
 */
void MainWindow::countBefore(){
    center->setCurrentIndex(4);
    auto cmb = dynamic_cast<CountMoneyBefore *>(center->widget(4));
    if(cmb){
        cmb->refresh();
    }
}

/**
 * @brief MainWindow::countAfter displays the interface allowing the user to register the amount of cash in the cash register after the sales.
 */
void MainWindow::countAfter(){
    center->setCurrentIndex(5);    
}

/**
 * @brief MainWindow::createNewSession display the interface allowing the user to register the amount of cash before the sales.
 * After the call to this function, the interface will trigger newSessionCreated() when user presses OK button.
 */
void MainWindow::createNewSession(){
    disconnect(center->widget(4), SIGNAL(validated()), this, SLOT(countBeforeFinished()));
    center->setCurrentIndex(4);
    connect(center->widget(4), SIGNAL(validated()), this, SLOT(newSessionCreated()));
}

/**
 * @brief MainWindow::countBeforeFinished is called when the user presses OK on the interface to register the cash before the sales.
 * Saves the jobists for the current session and the cash total in the main database.
 */
void MainWindow::countBeforeFinished(){
    center->setCurrentIndex(0);
    auto cmb = dynamic_cast<CountMoneyBefore *>(center->widget(4));
    if(cmb){
        DatabaseManager::setCurrentSessionjobists(cmb->getJobists());
        QVariant total = (cmb->getTotal()>=0.01?cmb->getTotal():QVariant());
        DatabaseManager::setCurrentSessionOpenAmount(total);
        cmb->save("count/before");
    }
}

/**
 * @brief MainWindow::countAfterFinished
 */
void MainWindow::countAfterFinished(){
    center->setCurrentIndex(0);
    auto cma = dynamic_cast<CountMoneyAfter *>(center->widget(5));
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Voulez-vous quitter ?"),
                                  tr("Voulez-vous fermer la sessin de vente actuelle et fermer le programme ?"));
    if(cma && (reply == QMessageBox::Yes)){
        QVariant total = (cma->getTotal()>=0.01?cma->getTotal():QVariant());
        DatabaseManager::closeSession(total);
        this->close();
    }else if(cma){
        //Saves the details of the count
        cma->save("count/after");
        QVariant total = (cma->getTotal()>=0.01?cma->getTotal():QVariant());
        DatabaseManager::setCurrentSessionCloseAmount(total);
    }
}

void MainWindow::newSessionCreated(){
    center->setCurrentIndex(0);
    auto cmb = dynamic_cast<CountMoneyBefore *>(center->widget(4));
    if(cmb){
        QVariant total = (cmb->getTotal()>=0.01?cmb->getTotal():QVariant());
        DatabaseManager::newSession(total, cmb->getJobists());
        cmb->save("count/before");
    }
    disconnect(center->widget(4), SIGNAL(validated()), this, SLOT(newSessionCreated()));
    connect(center->widget(4), SIGNAL(validated()), this, SLOT(countBeforeFinished()));

}

void MainWindow::manageOrders(){
    center->setCurrentIndex(6);
}

void MainWindow::about(){
    QIcon icon;
    const QString text = tr("<h2>À propos du logiciel KFeteManager</h2><br/>"
                            "<i>Développé sous license libre par <a href='mailto:morgandiepart@gmail.com'> Morgan Diepart</a>.</i><br/>"
                            "Code source disponible sur le dépôt Github à l'adresse  <a href='https://github.com/DrCampy/KFeteManager'>https://github.com/DrCampy/KFeteManager</a><br/>"
                            "<small>Version 0.1.0</small>");
    QMessageBox::about(this, tr("À propos du logiciel"), text);
}

void MainWindow::aboutQT(){
    QMessageBox::aboutQt(this, tr("À propos de Qt"));
}
