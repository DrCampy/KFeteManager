#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QCloseEvent>
#include <QStringList>
#include <QSizePolicy>
#include <QMap>
#include <QApplication>
#include <QColorDialog>
#include <QColor>
#include <QTableView>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>
#include <QStandardItemModel>
#include <QLabel>
#include <QHeaderView>

#include "catalog.h"
#include "salesview.h"

//TODO see QCompleter and QDataWidgetMapper (seen at http://doc.qt.io/qt-5/modelview.html)

const QStringList CarteView::PAGES_NAMES = (QStringList() << tr("Bières") << tr("Snacks") << tr("Softs") << tr("Divers"));

SalesView::SalesView(QWidget *parent) : QWidget(parent)
{
    //setAttribute(Qt::WA_DeleteOnClose); // deletes the widget when closed
    
    QHBoxLayout *hBox = new QHBoxLayout();
    QVBoxLayout *mainVBox = new QVBoxLayout(this);
    QVBoxLayout *currentOrderVBox = new QVBoxLayout();

    catalog           = new Catalog(this);
    carteView         = new CarteView(catalog, this);
    topBar            = new TopBar(this);
    totalLabel        = new QLabel(this);
    middleBar         = new MiddleBar(this);
    currentOrderView  = new QTableView(this);
    currentOrderModel = new CurrentOrderModel(catalog, 0, this);
    
    //The currentOrderVBox contains the current order display on top and the total label bot.
    currentOrderVBox->addWidget(currentOrderView, 10);
    currentOrderVBox->addWidget(totalLabel, 1, Qt::AlignRight);
    
    //hBox contains the currentOrder display, the middle bar and the carte display
    hBox->addLayout(currentOrderVBox, 5);
    hBox->addWidget(middleBar, 1);
    hBox->addWidget(carteView, 7);
    
    //mainVBox contains the top bar then the rest of the window
    mainVBox->addWidget(topBar, 1);
    mainVBox->addLayout(hBox, 6);
    
    //Configure table view. Selectable by row and hide vertical header
    QHeaderView *vHeader = currentOrderView->verticalHeader();
    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(20);
    currentOrderView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    currentOrderView->setSelectionBehavior(QAbstractItemView::SelectRows);
    currentOrderView->verticalHeader()->hide();
    currentOrderView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    currentOrderView->setModel(currentOrderModel);
    
    //Connects all the signals
    //From model to this
    connect(currentOrderModel, SIGNAL(updated()), this, SLOT(updateTotalLabel()));
    //From middleBar to this
    connect(middleBar, SIGNAL(actionPerformed()), this, SLOT(actionPerformed()));
    //From carteView to currentOrderModel
    connect(carteView, SIGNAL(clicArticle(QString)), currentOrderModel, SLOT(addArticle(QString)));
    //From middleBar to currentOrderModel
    connect(middleBar, SIGNAL(priceChanged()), currentOrderModel, SLOT(updatePrice()));

    updateTotalLabel();
}

void SalesView::updateTotalLabel(){
    QString text = QString::number(currentOrderModel->getTotal(), 'f', 2);
    text.append(tr(" €"));
    this->totalLabel->setText(text);
}

void SalesView::resizeEvent(QResizeEvent *) {
    currentOrderView->setColumnWidth(0, 15*currentOrderView->width()/100);
    currentOrderView->setColumnWidth(1, 65*currentOrderView->width()/100);
    currentOrderView->setColumnWidth(2, 20*currentOrderView->width()/100);
}

/*
 *This function is used to relay the signal of actionperformed from the middle bar
 * to pass it to currentOrderModel along with the selection from the view.
 */
void SalesView::actionPerformed(){
    currentOrderModel->setActionToPerform(middleBar->getLastPerformedAction());
    currentOrderModel->setActiveSelection(currentOrderView->selectionModel());
    performAction();
}

TopBar::TopBar(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *hBox = new QHBoxLayout(this);
    QWidget *bar = new QWidget(this);
    QButtonGroup *bGroup = new QButtonGroup(this);
    
    QStringList buttonsNames;
    buttonsNames << tr("Dépôt") << tr("Retrait") << tr("Ouvrir la Caisse") << tr("Compter la Caisse")
                 << tr("Comptes") << tr("Commandes") << tr("Valider");
    
    QPushButton *tmp;
    for(auto it = buttonsNames.begin(); it != buttonsNames.end(); it++){
        tmp = new QPushButton(*it, bar);
        tmp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        hBox->addWidget(tmp);
        bGroup->addButton(tmp, (it-buttonsNames.begin()));
    }
    
    hBox->setSpacing(0);
}


MiddleBar::MiddleBar(QWidget *parent) : QWidget(parent)
{
    //Defines the way the button will expand
    QSizePolicy qsp(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::PushButton);
    QVBoxLayout *column = new QVBoxLayout(this);
    column->setSpacing(0);

    //Creates ad hoc buttons
    plusButton         = new QPushButton("+", this);
    minusButton        = new QPushButton("-", this);
    deleteButton       = new QPushButton(tr("Supprimer"), this);
    priceButtonsGroup  = new QButtonGroup(this);
    normalPriceButton  = new QPushButton(tr("Tarif Normal"), this);
    reducedPriceButton = new QPushButton(tr("Tarif Réduit"), this);
    freePriceButton    = new QPushButton(tr("Gratuit"), this);
    
    column->addWidget(plusButton);
    column->addWidget(minusButton);
    column->addWidget(deleteButton);
    column->addWidget(normalPriceButton);
    column->addWidget(reducedPriceButton);
    column->addWidget(freePriceButton);

    //Adds the price buttons to group and config group
    priceButtonsGroup->addButton(normalPriceButton, 0);
    priceButtonsGroup->addButton(reducedPriceButton, 1);
    priceButtonsGroup->addButton(freePriceButton, 2);
    
    normalPriceButton->setCheckable(true);
    reducedPriceButton->setCheckable(true);
    freePriceButton->setCheckable(true);
    priceButtonsGroup->setExclusive(true);
    
    //Configures buttons to appear flat
    plusButton->setAutoFillBackground(true);
    plusButton->setSizePolicy(qsp);
    minusButton->setAutoFillBackground(true);
    minusButton->setSizePolicy(qsp);
    deleteButton->setAutoFillBackground(true);
    deleteButton->setSizePolicy(qsp);
    normalPriceButton->setAutoFillBackground(true);
    normalPriceButton->setSizePolicy(qsp);
    reducedPriceButton->setAutoFillBackground(true);
    reducedPriceButton->setSizePolicy(qsp);
    freePriceButton->setAutoFillBackground(true);
    freePriceButton->setSizePolicy(qsp);
    //Configures internal state
    lastPerformedAction = CurrentOrderModel::plusItem;
    resetPrice();
    
    connect(plusButton, SIGNAL(clicked()), this, SLOT(plusSlot()));
    connect(minusButton, SIGNAL(clicked()), this, SLOT(minusSlot()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSlot()));
    connect(priceButtonsGroup, SIGNAL(buttonPressed(int)), this, SLOT(priceSlot(int)));
    

}

void MiddleBar::plusSlot(){
    lastPerformedAction = CurrentOrderModel::plusItem;
    emit(actionPerformed());
}

void MiddleBar::minusSlot(){
    lastPerformedAction = CurrentOrderModel::minusItem;
    emit(actionPerformed());
}

void MiddleBar::deleteSlot(){
    lastPerformedAction = CurrentOrderModel::deleteItem;
    emit(actionPerformed());
}

void MiddleBar::priceSlot(int id){
    if(id == 0){
        selectedPrice = CurrentOrderModel::normal;
    }else if(id == 1){
        selectedPrice = CurrentOrderModel::reduced;
    }else if(id == 2){
        selectedPrice = CurrentOrderModel::free;
    }
    emit(priceChanged());
}

void MiddleBar::resetPrice(){
    this->normalPriceButton->click();
}

CurrentOrderModel::Action MiddleBar::getLastPerformedAction() const{
    return lastPerformedAction;
}

CurrentOrderModel::Price MiddleBar::getSelectedPrice() const{
    return selectedPrice;
}


