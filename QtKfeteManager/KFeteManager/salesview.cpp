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
#include <QPalette>
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
    topBar            = new TopBar(this);
    catalog           = new Catalog(this);
    carteView         = new CarteView(catalog, this);
    currentOrderModel = new CurrentOrderModel(catalog, 0, this);
    totalLabel        = new QLabel(this);
    middleBar         = new MiddleBar(this);


    //The currentOrderVBox contains the current order display on top and the total label bot.
    currentOrderVBox->addWidget(currentOrderView, 10);
    currentOrderVBox->addWidget(totalLabel, 1);

    //hBox contains the currentOrder display, the middle bar and the carte display
    hBox->addLayout(currentOrderVBox, 7);
    hBox->addWidget(middleBar, 1);
    hBox->addWidget(carteView, 7);

    //mainVBox contains the top bar then the rest of the window
    mainVBox->addWidget(topBar, 1);
    mainVBox->addLayout(hBox, 6);

    //Configure table view. Selectable by row and hide vertical header
    QHeaderView *vHeader = currentOrderView->verticalHeader();
    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(4);
    currentOrderView->setColumnWidth(0, 3);
    currentOrderView->setColumnWidth(1, 31);
    currentOrderView->setColumnWidth(2, 6);
    currentOrderView->setSelectionBehavior(QAbstractItemView::SelectRows);
    currentOrderView->verticalHeader()->hide();
    currentOrderView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    currentOrderView->setModel(currentOrderModel);

    //Connects all the signals
    //From model to this
    connect(currentOrderModel, SIGNAL(updated()), this, SLOT(updateTotalLabel()));
    //From middleBar to this
    connect(middleBar, SIGNAL(buttonPressed(CurrentOrderModel::Action)), this, SLOT(actionPerformed(CurrentOrderModel::Action)));
    //From carteView to currentOrderModel
    connect(carteView, SIGNAL(clicArticle(QString)), currentOrderModel, SLOT(addArticle(QString)));
    //From middleBar to currentOrderModel
    connect(middleBar, SIGNAL(priceChanged()), currentOrderModel, SLOT(changePrice()));
    //From this to currentOrderModel
    connect(this, SIGNAL(performAction(CurrentOrderModel::Action, QItemSelectionModel *)), currentOrderModel, SLOT(applyAction(CurrentOrderModel::Action, QItemSelectionModel *)));

}

void SalesView::updateTotalLabel(){
    QString text = QString::number(currentOrderModel->getTotal(), 'f', 2);
    text.append(tr(" €"));
    this->totalLabel->setText(text);
}

/*
 *This function is used to relay the signal of actionperformed from the middle bar
 * to pass it to currentOrderModel along with the selection from the view.
 */
void SalesView::actionPerformed(CurrentOrderModel::Action action){
    emit(performAction(action, currentOrderView->selectionModel()));
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

CarteView::CarteView(Catalog *cat, QWidget *parent) : QWidget(parent)
{
    //Checks the number of pages
    if(PAGES_NAMES.size() != NB_MENU_PAGES){
        throw std::out_of_range("From class Carte : NB_MENU_PAGES not less or equal PAGES_NAMES.size()");
    }

    //initialise the articles catalog of article to refer to
    this->cat = cat;

    QVBoxLayout *vBox = new QVBoxLayout(this); //first of all layouts
    QHBoxLayout *hBox = new QHBoxLayout(); //Layout for page name buttons

    hBox->setSpacing(0);
    vBox->setSpacing(0);
    QWidget *hBar = new QWidget;
    QStackedLayout *sLayout = new QStackedLayout; //main layout with pages inside
    QButtonGroup *tabs = new QButtonGroup(this);
    carteButtons = new QButtonGroup(this);

    lookupTable = new QMap<int, QString>();

    QWidget *pagesWidget = new QWidget[NB_MENU_PAGES]; //widget to store pages
    QGridLayout *pagesLayout[5]; // pages

    //set a size policy for the buttons to expand at max
    QSizePolicy qsp(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::PushButton);
    QPushButton *tmp;
    QPalette defaultPalette = QPushButton(nullptr).palette();
    defaultPalette.setColor(QPalette::Button, Qt::lightGray);
    for(int i = 0; i < static_cast<int>(NB_MENU_PAGES); i++){
        pagesLayout[i] = new QGridLayout(&pagesWidget[i]);
        pagesLayout[i]->setSpacing(0);
        //Fills pages
        for(int h = 0; h < static_cast<int>(GRID_H); h++){
            for(int w = 0; w < static_cast<int>(GRID_W); w++){
                //w+(h*GRID_W)+(GRID_H*GRID_W*i)
                //Generating buttons for the carte
                tmp = new QPushButton(&pagesWidget[i]);
                tmp->setSizePolicy(qsp);
                carteButtons->addButton(tmp, w + (h*static_cast<int>(GRID_W)) + (static_cast<int>(GRID_H)*static_cast<int>(GRID_W)*i));
                pagesLayout[i]->addWidget(tmp, static_cast<int>(h), static_cast<int>(w));
            }
        }

        //insert pages in QStackedLayout
        sLayout->addWidget(&pagesWidget[i]);

        //Generating tabs
        //Insert buttons in buttonGroup for tabs
        tmp = new QPushButton(PAGES_NAMES.at(i), hBar);
        tmp->setMinimumHeight(25);
        tmp->setMaximumHeight(80);
        tmp->setSizePolicy(qsp);
        hBox->addWidget(tmp, Qt::AlignCenter);
        tabs->addButton(tmp, i);
    }

    vBox->addLayout(sLayout, 8);
    vBox->addLayout(hBox, 1);

    connect(tabs, SIGNAL(buttonClicked(int)), sLayout, SLOT(setCurrentIndex(int)));
    connect(carteButtons, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)) );
    tabs->button(0)->click();
}

void CarteView::buttonClicked(int id){
    if(lookupTable->contains(id)){
        emit clicArticle( *(lookupTable->find(id)) );
    }
}

MiddleBar::MiddleBar(QWidget *parent) : QWidget(parent)
{
    //Creates ad hoc buttons
    plusButton         = new QPushButton("+", this);
    minusButton        = new QPushButton("-", this);
    deleteButton       = new QPushButton(tr("Supprimer"), this);
    priceButtonsGroup  = new QButtonGroup(this);
    normalPriceButton  = new QPushButton(tr("Tarif Normal"), this);
    reducedPriceButton = new QPushButton(tr("Tarif Réduit"), this);
    freePriceButton    = new QPushButton(tr("Gratuit"), this);

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
    minusButton->setAutoFillBackground(true);
    deleteButton->setAutoFillBackground(true);
    normalPriceButton->setAutoFillBackground(true);
    reducedPriceButton->setAutoFillBackground(true);
    freePriceButton->setAutoFillBackground(true);

    connect(plusButton, SIGNAL(clicked()), this, SLOT(plusSlot()));
    connect(minusButton, SIGNAL(clicked()), this, SLOT(minusSlot()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSlot()));
    connect(priceButtonsGroup, SIGNAL(buttonPressed(int)), this, SLOT(priceSlot(int)));

}

void MiddleBar::plusSlot(){
    emit(buttonPressed(CurrentOrderModel::plusItem));
}

void MiddleBar::minusSlot(){
    emit(buttonPressed(CurrentOrderModel::minusItem));
}

void MiddleBar::deleteSlot(){
    emit(buttonPressed(CurrentOrderModel::deleteItem));
}

void MiddleBar::priceSlot(int id){
    if(id == 0){
        emit(priceChanged(CurrentOrderModel::normal));
    }else if(id == 1){
        emit(priceChanged(CurrentOrderModel::reduced));
    }else if(id == 2){
        emit(priceChanged(CurrentOrderModel::free));
    }
}


