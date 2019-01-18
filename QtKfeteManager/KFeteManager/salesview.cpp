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
#include <QDebug>
#include <QToolButton>
#include <QListView>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QCompleter>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <customwidgets.h>

#include "catalog.h"
#include "salesview.h"
#include "cartemodel.h"
#include "carteview.h"

SalesView::SalesView(QWidget *parent) : QWidget(parent)
{    
    QHBoxLayout *topBar = new QHBoxLayout();
    QHBoxLayout *hBox = new QHBoxLayout();
    QVBoxLayout *mainVBox = new QVBoxLayout(this);
    QVBoxLayout *currentOrderVBox = new QVBoxLayout();

    totalLabel          = new QLabel(this);
    middleBar           = new MiddleBar(this);
    currentOrderModel   = new CurrentOrderModel(0, this);
    currentOrderView    = new QTableView(this);
    carteModel          = new CarteModel("./data/carte.xml", this);
    carteView           = new CarteView(this);

    //Links the carte model and view
    carteView->setModel(carteModel);

    //The currentOrderVBox contains the current order display on top and the total label bot.
    currentOrderVBox->addWidget(currentOrderView, 10);
    currentOrderVBox->addWidget(totalLabel, 1, Qt::AlignRight);
    
    //hBox contains the currentOrder display, the middle bar and the carte display
    hBox->addLayout(currentOrderVBox, 5);
    hBox->addWidget(middleBar, 1);
    hBox->addWidget(carteView, 7);
    
    //mainVBox contains the top bar then the rest of the window
    mainVBox->addLayout(topBar);
    mainVBox->addLayout(hBox, 6);
    
    //Configure table view. Selectable by row and hide vertical header
    currentOrderView->setModel(currentOrderModel);
    currentOrderView->verticalHeader()->hide();
    currentOrderView->setSelectionBehavior(QAbstractItemView::SelectRows);
    currentOrderView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    currentOrderView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    currentOrderView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    currentOrderView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    currentOrderViewResize();

    //TopBar
    QToolButton *deposit    = new QToolButton(this);
    QToolButton *withdraw   = new QToolButton(this);
    QToolButton *count      = new QToolButton(this);
    QToolButton *accounts   = new QToolButton(this);
    QPushButton *orders     = new QPushButton(tr("Commandes"), this);
    QPushButton *validate   = new QPushButton(tr("Valider"), this);

    //Configures the size of the tool buttons
    auto qsp = QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    deposit->setSizePolicy(qsp);
    withdraw->setSizePolicy(qsp);
    count->setSizePolicy(qsp);
    accounts->setSizePolicy(qsp);
    auto minSize = validate->minimumSize();
    deposit->setMinimumSize(minSize);
    withdraw->setMinimumSize(minSize);
    count->setMinimumSize(minSize);
    accounts->setMinimumSize(minSize);

    //accounts
    accounts->setText(tr("Comptes"));
    accounts->setPopupMode(QToolButton::InstantPopup);
    accounts->setArrowType(Qt::NoArrow);
    AccountSelector *accountSelector = new AccountSelector(this);
    accounts->addAction(accountSelector);

    //Deposit
    deposit->addAction(new QAction(tr("Sur un compte"), deposit));
    deposit->addAction(new QAction(tr("En caisse"), deposit));
    deposit->setText(tr("Dépôt"));
    deposit->setPopupMode(QToolButton::InstantPopup);
    deposit->setArrowType(Qt::NoArrow);
    connect(deposit->actions().at(0), SIGNAL(triggered()), this, SLOT(clientDeposit()));
    connect(deposit->actions().at(1), SIGNAL(triggered()), this, SLOT(cashDeposit()));

    //Withdraw
    withdraw->addAction(new QAction(tr("Sur un compte"), withdraw));
    withdraw->addAction(new QAction(tr("En caisse"), withdraw));
    withdraw->setText(tr("Retrait"));
    withdraw->setPopupMode(QToolButton::InstantPopup);
    withdraw->setArrowType(Qt::NoArrow);
    connect(withdraw->actions().at(0), SIGNAL(triggered()), this, SLOT(clientWithdraw()));
    connect(withdraw->actions().at(1), SIGNAL(triggered()), this, SLOT(cashWithdraw()));

    //count = count content of cash register
    count->addAction(new QAction(tr("Avant l'exercice"), count));
    count->addAction(new QAction(tr("Après l'exercice"), count));
    count->setText(tr("Compter la caisse"));
    count->setPopupMode(QToolButton::InstantPopup);
    count->setArrowType(Qt::NoArrow);
    connect(count->actions().at(0), SIGNAL(triggered()), this, SIGNAL(countBefore()));
    connect(count->actions().at(1), SIGNAL(triggered()), this, SIGNAL(countAfter()));

    topBar->addWidget(deposit, 1);
    topBar->addWidget(withdraw, 1);
    topBar->addWidget(count, 1);
    topBar->addWidget(accounts, 1);
    topBar->addWidget(orders, 1);
    topBar->addWidget(validate, 1);

    //Connects all the signals
    //From model to this
    connect(currentOrderModel, SIGNAL(updated()), this, SLOT(modelUpdated()));
    //From middleBar to this
    connect(middleBar, SIGNAL(actionPerformed()), this, SLOT(actionPerformed()));
    //From middleBar to currentOrderModel
    connect(middleBar, SIGNAL(priceChanged()), this, SLOT(priceUpdated()));
    connect(this, SIGNAL(updatePrice()), currentOrderModel, SLOT(updatePrice()));
    connect(this, SIGNAL(performAction()), currentOrderModel, SLOT(applyAction()));
    //Connect carteModel to currentOrderModel
    connect(carteModel, SIGNAL(articleClicked(QString)), this, SLOT(articleAdded(QString)));
    connect(this, SIGNAL(addArticle(QString)), currentOrderModel, SLOT(addArticle(QString)));
    //Connect the validate button
    connect(validate, SIGNAL(clicked()), this, SLOT(validateOrder()));
    //connects accountSelector
    connect(accountSelector, SIGNAL(clientSelected(Client)), this, SLOT(selectClient(Client)));
    connect(orders, SIGNAL(clicked()), this, SIGNAL(manageOrders()));

    modelUpdated();
}

void SalesView::currentOrderViewResize(){
    currentOrderView->horizontalHeader()->resizeSection(0, 55);
    currentOrderView->horizontalHeader()->resizeSection(1, 90);
    currentOrderView->setMinimumWidth(230);
}

void SalesView::articleAdded(QString s){
    emit addArticle(s);
    currentOrderViewResize();
}

void SalesView::priceUpdated(){
    currentOrderModel->setPrice(middleBar->getSelectedPrice());
    emit updatePrice();
    currentOrderViewResize();
}

void SalesView::modelUpdated(){
    QString text = QString::number(currentOrderModel->getTotal(), 'f', 2);
    text.append(tr(" €"));
    this->totalLabel->setText(text);
    this->currentOrderView->setStyleSheet("");
}

/*
 *This function is used to relay the signal of actionperformed from the middle bar
 * to pass it to currentOrderModel along with the selection from the view.
 */
void SalesView::actionPerformed(){
    currentOrderModel->setActionToPerform(middleBar->getLastPerformedAction());
    currentOrderModel->setActiveSelection(currentOrderView->selectionModel());
    emit performAction();
    currentOrderViewResize();
}

void SalesView::selectClient(Client c){
    this->selectedClient = c;
}

CarteModel *SalesView::getCarteModel(){
    return carteModel;
}

bool SalesView::processOrder(const Order &o, Client c){
    bool process = false;
    //If the order is on a client's account, checks that the clients have enough funds.
    if(!c.isNull()){
        if(c.isJobist()){
            //Client does not have enough funds, check for it's limit
            if(c.getBalance() < o.getTotal()){
                //limit allow user to buy
                if(c.getBalance()-c.getLimit() > o.getTotal()){
                    QMessageBox messageBox;
                    messageBox.setText(tr("Solde bas"));
                    messageBox.setInformativeText(tr("Le solde du compte sélectionné est négatif. \n"
                                                     "Voulez-vous poursuivre quand même ?"));
                    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    messageBox.setDefaultButton(QMessageBox::No);
                    int ret = messageBox.exec();
                    process = (ret == QMessageBox::Yes);
                }else{
                    QMessageBox::warning(this, tr("Solde insuffisant"), tr("Le solde du compte sélectionné n'est pas assez élevé."));
                    process = false;
                }
            }else{
                process = true;
            }
        }else{
            if(c.getBalance() < o.getTotal()){
                //Displays error
                QMessageBox::warning(this, tr("Solde insuffisant"), tr("Le solde du compte sélectionné n'est pas assez élevé."));
                process = false;
            }else{
                process = true;
            }
        }
    }else{
        process = true;
    }

    if(!process){
        return false;
    }
    if(!c.isNull()){
        //removes money from account
        if(!c.exists()){
            return false;
        }
        if(process){
            c.deposit(-o.getTotal());
        }
    }
    return DatabaseManager::addOrder(o, c);
}

void SalesView::validateOrder(){
    if(this->processOrder(this->currentOrderModel->getOrder(), selectedClient)){
        this->middleBar->resetPrice();
        this->currentOrderView->setStyleSheet("color : red;");
        this->currentOrderModel->clear();
        this->selectedClient = Client("");
    }
}

void SalesView::cashDeposit(){
    CustomSelectorPopup *popup =
            new CustomSelectorPopup(nullptr, CustomSelectorPopup::Amount);

    popup->setTitle(tr("Effectuer un dépôt en caisse"));

    QVariant *amount = new QVariant();

    if(popup->ask(amount)){
        DatabaseManager::addDeposit(amount->toDouble());
    }
}

void SalesView::clientDeposit(){
    CustomSelectorPopup *popup =
            new CustomSelectorPopup(nullptr,
                                    CustomSelectorPopup::Amount |
                                    CustomSelectorPopup::Client);

    popup->setTitle(tr("Effectuer un dépôt sur un compte"));

    QVariant *amount = new QVariant();
    QVariant *client = new QVariant();
    if(popup->ask(amount, client)){
        DatabaseManager::addDeposit(amount->toDouble(), Client(client->toString()));
    }
}

void SalesView::cashWithdraw(){
    CustomSelectorPopup *popup =
            new CustomSelectorPopup(nullptr, CustomSelectorPopup::Amount);

    popup->setTitle(tr("Effectuer un retrait en caisse"));

    QVariant *amount = new QVariant();
    if(popup->ask(amount)){
        DatabaseManager::addDeposit(-amount->toDouble());
    }
}

void SalesView::clientWithdraw(){
    CustomSelectorPopup *popup =
            new CustomSelectorPopup(nullptr,
                                    CustomSelectorPopup::Amount |
                                    CustomSelectorPopup::Client);

    popup->setTitle(tr("Effectuer un retrait sur un compte"));

    QVariant *amount = new QVariant();
    QVariant *client = new QVariant();
    if(popup->ask(amount, client)){
        DatabaseManager::addDeposit(-amount->toDouble(), Client(client->toString()));
    }
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
        selectedPrice = Order::normal;
    }else if(id == 1){
        selectedPrice = Order::reduced;
    }else if(id == 2){
        selectedPrice = Order::free;
    }
    emit(priceChanged());
}

void MiddleBar::resetPrice(){
    this->normalPriceButton->click();
}

CurrentOrderModel::Action MiddleBar::getLastPerformedAction() const{
    return lastPerformedAction;
}

Order::Price MiddleBar::getSelectedPrice() const{
    return selectedPrice;
}

//Constructor
AccountSelector::AccountSelector(QWidget *parent) : QWidgetAction(parent){
    //Initializes the model from the SQL database
    clientsModel = new QSqlQueryModel(this);
};

QWidget *AccountSelector::createWidget(QWidget *parent){
    QWidget *mainWidget = new QWidget(parent);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    balance = new QLabel(tr("Solde : "), mainWidget);
    clientsModel->setQuery("SELECT Name, balance FROM Clients ORDER BY Name ASC;");
    //Creates searchbar and configure it
    searchBar = new QLineEdit(mainWidget);
    searchBar->setPlaceholderText(tr("Rechercher"));
    searchBar->setFocus(); //So that when users have clicked the button he can imediately type

    //Creates listview and configure it
    listView = new QListView(mainWidget);
    listView->setModel(clientsModel);
    listView->setModelColumn(0);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers); //Listview cannot be edited

    mainLayout->addWidget(searchBar);
    mainLayout->addWidget(listView);
    mainLayout->addWidget(balance);

    lastCreatedWidget = mainWidget;

    connect(searchBar, SIGNAL(textChanged(QString)), this, SLOT(searchBarUpdated(QString )));
    connect(searchBar, SIGNAL(returnPressed()), this, SLOT(searchBarReturnPressed()));
    connect(listView, SIGNAL(activated(const QModelIndex &)), this, SLOT(listViewItemActivated(const QModelIndex &)));
    connect(listView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(clientHighlighted(const QModelIndex &)));

    return mainWidget;
}

void AccountSelector::searchBarUpdated(QString text){
    //Finds the best match
    auto match = clientsModel->match(clientsModel->index(0, 0), Qt::DisplayRole, QVariant(text), 1, Qt::MatchContains | Qt::MatchWrap);

    //If we have a match, select it
    if(!match.isEmpty()){
        listView->selectionModel()->select(match.at(0), QItemSelectionModel::ClearAndSelect);
        listView->scrollTo(match.at(0), QAbstractItemView::PositionAtTop); //Scrolls so that the match is on top of the list
    }

    //Todo check if it cannot be automated
    //Manually calls for clientHighlighted to update label.
    clientHighlighted(match.at(0));
}

void AccountSelector::searchBarReturnPressed(){
    //When user press enter on the searchbar, we are done.
    auto selection = listView->selectionModel()->selectedIndexes();
    if(!selection.isEmpty()){
        emit(clientSelected(Client(selection.first().data().toString())));
    }
    lastCreatedWidget->parentWidget()->hide();
}

void AccountSelector::listViewItemActivated(const QModelIndex &index){
    //If user clicks on a client in the list, emit the corresponding signal
    emit clientSelected(Client(index.data().toString()));
    lastCreatedWidget->parentWidget()->hide();
}

void AccountSelector::clientHighlighted(const QModelIndex &index){
    //Updates the label indicating the balance of the highlighted client
    this->balance->setText(tr("Solde : ") + QString::number(clientsModel->data(clientsModel->index(index.row(), 1)).toDouble(), 'f', 2));
}

ClientComboBox::ClientComboBox(QWidget *parent) : QComboBox (parent)
{
    //this->setEditable(true);
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery("SELECT name FROM Clients;");
    this->setModel(model);
    model->setHeaderData(0, Qt::Horizontal, tr("Nom"));
    this->setCurrentIndex(-1);
    connect(this, SIGNAL(editTextChanged(const QString &)), this, SLOT(refresh()));
}

void ClientComboBox::refresh(){
    this->setCurrentIndex(findText(currentText()));
    auto selection = this->view()->selectionModel()->selectedIndexes();
    if(!selection.isEmpty()){
        this->view()->scrollTo(selection.first(), QAbstractItemView::PositionAtTop);
    }
}

CustomSelectorPopup::CustomSelectorPopup(QWidget *parent, SelectionFlags flags) :
    QDialog (parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    this->flags = flags;
    setMinimumSize(400, 130);
    setMaximumSize(400, 130);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);

    //If we have to ask for a client
    if(flags & Client){
        accountLabel = new QLabel(tr("Compte :"), this);
        clientCombo = new ClientComboBox(this);
        mainLayout->addWidget(accountLabel, 0, Qt::AlignBottom);
        mainLayout->addWidget(clientCombo, 0, Qt::AlignTop);
    }

    //If we have to ask both, insert space
    if(flags & (Amount|Client) ){
        mainLayout->addSpacing(15);
    }

    //If we have to ask for an amount
    if(flags & Amount){
        amountLabel = new QLabel(tr("Montant :"), this);
        amount = new CustomDoubleSpinBox(this);
        mainLayout->addWidget(amountLabel, 0, Qt::AlignBottom);
        mainLayout->addWidget(amount, 0, Qt::AlignTop);
    }

    //Adds buttons
    QHBoxLayout *buttonsLayout  = new QHBoxLayout();
    QPushButton *validate       = new QPushButton(tr("Valider"), this);
    QPushButton *cancel         = new QPushButton(tr("Annuler"), this);
    buttonsLayout->addStretch(0);
    buttonsLayout->addWidget(validate, 1, Qt::AlignRight);
    buttonsLayout->addWidget(cancel, 1, Qt::AlignLeft);
    mainLayout->addLayout(buttonsLayout);

    connect(validate, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

void CustomSelectorPopup::setTitle(QString title){
    setWindowTitle(title);
}

bool CustomSelectorPopup::ask(QVariant *amount, QVariant *client){
    this->exec();

    if(this->result() == QDialog::Accepted){
        if(flags & Amount && amount){
            amount->setValue(this->amount->value());
        }
        if(flags & Client && client){
            client->setValue(this->clientCombo->currentText());
        }
        this->deleteLater();
        return true;
    }
    this->deleteLater();
    return false;
}

