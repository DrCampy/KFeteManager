#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QListView>
#include <QStringListModel>
#include <QPainter>
#include <QComboBox>
#include <QSqlQueryModel>
#include <QDebug>

#include "countmoney.h"
#include "databasemanager.h"
#include "clientlist.h"

CountMoney::CountMoney(QWidget *parent) : QWidget(parent){
    validate    = new QPushButton(tr("Valider"));
    cancel      = new QPushButton(tr("Annuler"));
    moneyForm   = new MoneyForm(this);
}

CountMoneyBefore::CountMoneyBefore(QWidget *parent) : CountMoney(parent){
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *comboLayout = new QHBoxLayout();
    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    buttonsLayout->addWidget(validate);
    buttonsLayout->addWidget(cancel);
    //Selecting jobists
    //Display
    jobistsList = new QListView(this);
    jobistsModel = new QStringListModel(this);
    jobistsModel->sort(0, Qt::AscendingOrder);
    jobistsList->setModel(jobistsModel);
    jobistsList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //Selection
    selectionCombo  = new QComboBox;
    add             = new QPushButton(tr("Ajouter"), this);
    comboLayout->addWidget(selectionCombo);
    comboLayout->addWidget(add);

    QSqlQueryModel *clientsModel  = new QSqlQueryModel(this);
    clientsModel->setQuery("SELECT Name FROM Clients ORDER BY 'name' ASC;");
    selectionCombo->setModel(clientsModel);

    //validate/cancel buttons
    mainLayout->addWidget(jobistsList);
    mainLayout->addLayout(comboLayout);
    mainLayout->addWidget(moneyForm);
    mainLayout->addLayout(buttonsLayout);

    connect(cancel, SIGNAL(clicked()), this, SIGNAL(cancelled()));
    connect(add, SIGNAL(clicked()), this, SLOT(addJobist()));
    connect(jobistsList, SIGNAL(clicked(const QModelIndex &)), this, SLOT(removeJobist(const QModelIndex &)));
}

void CountMoneyBefore::addJobist(){
    QString name = this->selectionCombo->currentText();
    QStringList list = jobistsModel->stringList();
    list << name;
    list.removeDuplicates();
    jobistsModel->setStringList(list); //appends the added name.
    jobistsModel->sort(0, Qt::AscendingOrder);
}

void CountMoneyBefore::removeJobist(const QModelIndex &index){
    QStringList list = jobistsModel->stringList();
    list.removeAll(index.data().toString());
    jobistsModel->setStringList(list);
}

CountMoneyAfter::CountMoneyAfter(QWidget *parent) : CountMoney(parent){

}

MoneyForm::MoneyForm(QWidget *parent) : QWidget (parent)
{
    //TODO use QLocale to manage euro symbol. Maybe a way to store it in another way in the DB.
    QLabel *notesTitleLabel = new QLabel(tr("Billets :"), this);
    QLabel *coinsTitleLabel = new QLabel(tr("Pièces :"), this);

    QString currency = DatabaseManager::getCurrency();
    QStringList notes = DatabaseManager::getNotes();
    QStringList coins = DatabaseManager::getCoins();

    notesLabels = new QList<QLabel *>();
    coinsLabels = new QList<QLabel *>();
    notesValues = new QVector<qreal>();
    coinsValues = new QVector<qreal>();
    notesSpinBoxes = new QList<QSpinBox *>();
    coinsSpinBoxes = new QList<QSpinBox *>();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *gridNotes = new QGridLayout();
    QGridLayout *gridCoins = new QGridLayout();

    //Creates the necessary labels and spinBoxes for the notes
    for(auto it : notes){
        notesLabels->append(new QLabel(currency + it, this));
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setMaximum(INT_MAX);
        spinBox->setMinimum(0);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox->setSingleStep(1);
        notesSpinBoxes->append(spinBox);
        notesValues->append(it.toDouble());
    }

    //Creates the necessary labels and spinBoxes for the coins
    for(auto it : coins){
        coinsLabels->append(new QLabel(currency + it, this));
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setMaximum(INT_MAX);
        spinBox->setMinimum(0);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox->setSingleStep(1);
        coinsSpinBoxes->append(spinBox);
        coinsValues->append(it.toDouble());
    }

    //layout the widgets
    //Notes
    gridNotes->addWidget(notesTitleLabel, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignBottom);
    qDebug() << notesLabels->size();
    for(int i = 0; i < notesLabels->size(); i++){
        gridNotes->addWidget(notesLabels->at(i), 1+(i/4), (i%4)*2, 1, 1);
        gridNotes->addWidget(notesSpinBoxes->at(i), 1+(i/4), (i%4)*2+1, 1, 1);
    }
    //Coins
    gridCoins->addWidget(coinsTitleLabel, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignBottom);
    for(int i = 0; i < coinsLabels->size(); i++){
        gridCoins->addWidget(coinsLabels->at(i), 1+(i/4), (i%4)*2, 1, 1);
        gridCoins->addWidget(coinsSpinBoxes->at(i), 1+(i/4), (i%4)*2+1, 1, 1);
    }

    mainLayout->addLayout(gridCoins);
    mainLayout->addLayout(gridNotes);
}

MoneyForm::~MoneyForm(){
    delete notesLabels;
    delete notesSpinBoxes;
    delete notesValues;

    delete coinsLabels;
    delete coinsSpinBoxes;
    delete coinsValues;
}

qreal MoneyForm::getCount(){
    qreal total = 0;

    //for notes
    for(int i = 0; i < notesValues->size(); i++){
        total += notesSpinBoxes->at(i)->value()*notesValues->at(i);
    }

    //for coins
    for(int i = 0; i < coinsValues->size(); i++){
        total += coinsSpinBoxes->at(i)->value()*coinsValues->at(i);
    }

    return total;
}
