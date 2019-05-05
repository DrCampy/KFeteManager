#include <QPushButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>

#include "order.h"
#include "currentordermodel.h"
#include "currentorderview.h"

CurrentOrderView::CurrentOrderView(QWidget *parent) : QWidget(parent)
{
    //Defines the way the button will expand
    QSizePolicy qsp(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::PushButton);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *viewLayout = new QVBoxLayout();
    QVBoxLayout *buttonsLayout = new QVBoxLayout();
    buttonsLayout->setSpacing(0);

    //Creates buttons
    plusButton         = new QPushButton("+", this);
    minusButton        = new QPushButton("-", this);
    deleteButton       = new QPushButton(tr("Supprimer"), this);
    priceButtonsGroup  = new QButtonGroup(this);
    normalPriceButton  = new QPushButton(tr("Tarif Normal"), this);
    reducedPriceButton = new QPushButton(tr("Tarif Réduit"), this);
    freePriceButton    = new QPushButton(tr("Gratuit"), this);
    tableView          = new QTableView(this);
    totalLabel         = new QLabel(this);

    //The viewLayout contains the current order display on top and the total label bot.
    viewLayout->addWidget(tableView, 10);
    viewLayout->addWidget(totalLabel, 1, Qt::AlignRight);

    buttonsLayout->addWidget(plusButton);
    buttonsLayout->addWidget(minusButton);
    buttonsLayout->addWidget(deleteButton);
    buttonsLayout->addWidget(normalPriceButton);
    buttonsLayout->addWidget(reducedPriceButton);
    buttonsLayout->addWidget(freePriceButton);

    mainLayout->addLayout(viewLayout, 5);
    mainLayout->addLayout(buttonsLayout, 1);

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

    connect(plusButton, SIGNAL(clicked()), this, SLOT(plusSlot()));
    connect(minusButton, SIGNAL(clicked()), this, SLOT(minusSlot()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSlot()));
    connect(priceButtonsGroup, SIGNAL(buttonPressed(int)), this, SLOT(priceSlot(int)));
}

void CurrentOrderView::plusSlot(){
    if(!model){
        return;
    }
    model->setActiveSelection(tableView->selectionModel());
    model->applyAction(CurrentOrderModel::plusItem);
    resize();
}

void CurrentOrderView::minusSlot(){
    if(!model){
        return;
    }
    model->setActiveSelection(tableView->selectionModel());
    model->applyAction(CurrentOrderModel::minusItem);
    resize();
}

void CurrentOrderView::deleteSlot(){
    if(!model){
        return;
    }
    model->setActiveSelection(tableView->selectionModel());
    model->applyAction(CurrentOrderModel::deleteItem);
    resize();
}

void CurrentOrderView::priceSlot(int id){
    if(!model){
        return;
    }
    if(id == 0){
        model->updatePrice(Order::normal);
    }else if(id == 1){
        model->updatePrice(Order::reduced);
    }else if(id == 2){
        model->updatePrice(Order::free);
    }
    updateLabel();
}

void CurrentOrderView::setModel(CurrentOrderModel *model){
    this->model = model;
    tableView->setModel(this->model);
    tableView->verticalHeader()->hide();
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    connect(model, SIGNAL(updated()), this, SLOT(updateLabel()));
    resize();
    updateLabel();
}

void CurrentOrderView::resize(){
    tableView->horizontalHeader()->resizeSection(0, 55);
    tableView->horizontalHeader()->resizeSection(1, 90);
    tableView->setMinimumWidth(230);
}

void CurrentOrderView::validated(){
    this->tableView->setStyleSheet("color: red;");
}

void CurrentOrderView::updateLabel(){
    if(!model){
        this->totalLabel->setText(tr("Total : ") + locale().toCurrencyString(0.0, locale().currencySymbol(), 2));
    }else{
        this->totalLabel->setText(tr("Total : ") + locale().toCurrencyString(model->getTotal(), locale().currencySymbol(), 2));
        switch(model->getOrder().getPrice()){
        case Order::normal:
            this->normalPriceButton->setChecked(true);
            break;
        case Order::reduced:
            this->reducedPriceButton->setChecked(true);
            break;
        case Order::free:
            this->freePriceButton->setChecked(true);
            break;
        }
    }
    this->tableView->setStyleSheet("");
}
