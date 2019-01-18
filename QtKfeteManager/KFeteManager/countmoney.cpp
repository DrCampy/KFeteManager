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
#include <QSettings>

#include "countmoney.h"
#include "databasemanager.h"
#include "clientlist.h"

CountMoney::CountMoney(QWidget *parent) : QWidget(parent){
    validate    = new QPushButton(tr("Valider"));
    cancel      = new QPushButton(tr("Annuler"));
    moneyForm   = new MoneyForm(this);
}

void CountMoney::load(QString settingsName){
    QSettings settings;
    settings.beginGroup(settingsName);
    QString notes = settings.value("notes", QVariant("")).toString();
    QString coins = settings.value("coins", QVariant("")).toString();
    settings.endGroup();

    QList<uint> notesList;
    for(auto it : notes.splitRef(";", QString::SkipEmptyParts)){
        notesList.append(it.toUInt());
    }
    QList<uint> coinsList;
    for(auto it : coins.splitRef(";", QString::SkipEmptyParts)){
        coinsList.append(it.toUInt());
    }

    if(coinsList.size() == moneyForm->coinsSpinBoxes->size()
            && notesList.size() == moneyForm->notesSpinBoxes->size()){
        moneyForm->setCount(notesList, coinsList);
    }
}

void CountMoney::save(QString settingsName){
    QList<uint> notes;
    QList<uint> coins;
    moneyForm->getCount(notes, coins);

    QSettings settings;
    settings.beginGroup(settingsName);

    QByteArray list = "";
    for(auto it : notes){
        list.append(';');
        list.append(QString::number(it));
    }
    settings.setValue("notes", list);

    list = "";
    for(auto it : coins){
        list.append(';');
        list.append(QString::number(it));
    }
    settings.setValue("coins", list);
    settings.endGroup();
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
    jobistsList->setMaximumSize(2000, 100);
    jobistsModel = new QStringListModel(this);
    jobistsModel->sort(0, Qt::AscendingOrder);
    jobistsList->setModel(jobistsModel);
    jobistsList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //Selection
    selectionCombo  = new QComboBox;
    add             = new QPushButton(tr("Ajouter"), this);
    comboLayout->addWidget(selectionCombo);
    comboLayout->addWidget(add);

    clientsModel  = new QSqlQueryModel(this);
    clientsModel->setQuery("SELECT Name FROM Clients ORDER BY 'name' ASC;");
    selectionCombo->setModel(clientsModel);

    //validate/cancel buttons
    mainLayout->addWidget(jobistsList);
    mainLayout->addLayout(comboLayout);
    mainLayout->addWidget(moneyForm);
    mainLayout->addLayout(buttonsLayout);

    connect(validate, SIGNAL(clicked()), this, SIGNAL(validated()));
    connect(cancel, SIGNAL(clicked()), this, SIGNAL(cancelled()));
    connect(add, SIGNAL(clicked()), this, SLOT(addJobist()));
    connect(jobistsList, SIGNAL(clicked(const QModelIndex &)), this, SLOT(removeJobist(const QModelIndex &)));
    load("count/before");
    QVariant openedSession = DatabaseManager::getCurrentSession();
    if(!openedSession.isNull()){
        QStringList jobistsList;
        QSqlQuery jobists;
        jobists.exec("SELECT name FROM HeldSession WHERE SessionTime = '"
                         + openedSession.toString() + "';");
        while(jobists.next()){
            jobistsList << jobists.value(0).toString();
        }
        addJobists(jobistsList);
    }
}

void CountMoneyBefore::addJobists(QStringList jobists){
    QStringList list = jobistsModel->stringList();
    for(auto it : jobists){
        if(Client(it).exists()){
            list << it;
        }
    }
    list.removeDuplicates();
    jobistsModel->setStringList(list); //appends the added name.
    jobistsModel->sort(0, Qt::AscendingOrder);
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

QList<Client> CountMoneyBefore::getJobists(){
    QList<Client> jobists;
    for(auto it : jobistsModel->stringList()){
        if(Client(it).exists())
            jobists.append(Client(it));
    }
    return jobists;
}

void CountMoneyBefore::refresh(){
    clientsModel->setQuery("SELECT Name FROM Clients ORDER BY 'name' ASC;");
}

CountMoneyAfter::CountMoneyAfter(QWidget *parent) : CountMoney(parent){
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonsLayout = new QHBoxLayout();


    buttonsLayout->addWidget(validate);
    buttonsLayout->addWidget(cancel);

    //validate/cancel buttons
    mainLayout->addWidget(moneyForm);
    mainLayout->addLayout(buttonsLayout);

    connect(validate, SIGNAL(clicked()), this, SIGNAL(validated()));
    connect(cancel, SIGNAL(clicked()), this, SIGNAL(cancelled()));
    load("count/after");
}

void MoneyForm::setCount(const QList<uint> &notes, const QList<uint> &coins){
    if(notesSpinBoxes->size() != notes.size()
       || coinsSpinBoxes->size() != coins.size()){
        return;
    }
    for(int i = 0; i < notes.size(); i++){
        notesSpinBoxes->at(i)->setValue(static_cast<int>(notes.at(i)));
    }
    for(int i = 0; i < coins.size(); i++){
        coinsSpinBoxes->at(i)->setValue(static_cast<int>(coins.at(i)));
    }
}

void MoneyForm::getCount(QList<uint> &notes, QList<uint> &coins){
    notes.empty();
    coins.empty();

    for(auto it : *notesSpinBoxes){
        notes.append(static_cast<uint>(it->value()));
    }
    for(auto it : *coinsSpinBoxes){
        coins.append(static_cast<uint>(it->value()));
    }
}

MoneyForm::MoneyForm(QWidget *parent) : QWidget (parent)
{
    //TODO could use formlayout
    QLocale locale;
    QLabel *notesTitleLabel = new QLabel(tr("Billets :"), this);
    QLabel *coinsTitleLabel = new QLabel(tr("Pièces :"), this);

    QStringList notes = DatabaseManager::getNotes();
    QStringList coins = DatabaseManager::getCoins();

    notesLabels = new QList<QLabel *>();
    coinsLabels = new QList<QLabel *>();
    notesValues = new QVector<qreal>();
    coinsValues = new QVector<qreal>();
    notesSpinBoxes = new QList<QSpinBox *>();
    coinsSpinBoxes = new QList<QSpinBox *>();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *gridCoins = new QGridLayout();
    QGridLayout *gridNotes = new QGridLayout();

    //Creates the necessary labels and spinBoxes for the notes
    for(auto it : notes){
        locale.toCurrencyString(it.toInt(), locale.currencySymbol());
        notesLabels->append(new QLabel(locale.toCurrencyString(it.toInt(), locale.currencySymbol()), this));
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
        coinsLabels->append(new QLabel(locale.toCurrencyString(it.toDouble(), locale.currencySymbol(), 2), this));
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
    for(int i = 0; i < notesLabels->size(); i++){
        gridNotes->addWidget(notesLabels->at(i), 1+(i/4), (i%4)*2, 1, 1, Qt::AlignRight);
        gridNotes->addWidget(notesSpinBoxes->at(i), 1+(i/4), (i%4)*2+1, 1, 1, Qt::AlignLeft);
    }
    //Coins
    gridCoins->addWidget(coinsTitleLabel, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignBottom);
    for(int i = 0; i < coinsLabels->size(); i++){
        gridCoins->addWidget(coinsLabels->at(i), 1+(i/4), (i%4)*2, 1, 1, Qt::AlignRight);
        gridCoins->addWidget(coinsSpinBoxes->at(i), 1+(i/4), (i%4)*2+1, 1, 1, Qt::AlignLeft);
    }

    mainLayout->addLayout(gridNotes);
    mainLayout->addLayout(gridCoins);
}

MoneyForm::~MoneyForm(){
    delete notesLabels;
    delete notesSpinBoxes;
    delete notesValues;

    delete coinsLabels;
    delete coinsSpinBoxes;
    delete coinsValues;
}

qreal MoneyForm::getTotal(){
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
