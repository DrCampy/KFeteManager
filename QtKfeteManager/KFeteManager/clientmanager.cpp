#include <QSqlRelationalTableModel>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QListView>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QSqlRecord>
#include <QInputDialog>
#include <QMessageBox>

#include <QDebug>
#include <QSqlError>


#include "clientlist.h"
#include "clientmanager.h"
#include "databasemanager.h"

ClientManager::ClientManager(QWidget *parent) : QWidget(parent)
{
    clientModel         = new QSqlRelationalTableModel(this);
    clientList          = new QListView(this);
    validateButton      = new QPushButton(tr("Sauver"), this);
    //form widgets
    clientName          = new QLabel(this);
    balance             = new QLabel(this);
    address             = new QLineEdit(this);
    email               = new QLineEdit(this);
    phone               = new QLineEdit(this);
    limit               = new QDoubleSpinBox(this);
    isJobist            = new QComboBox(this);
    mapper              = new QDataWidgetMapper(this);

    QHBoxLayout *mainLayout     = new QHBoxLayout(this);
    QVBoxLayout *rightLayout    = new QVBoxLayout();
    QFormLayout *formLayout     = new QFormLayout();
    QLabel *formTitle           = new QLabel(tr("Article sélectionné :"), this);
    QPushButton *deleteButton   = new QPushButton(tr("Supprimer client"), this);
    QPushButton *createButton   = new QPushButton(tr("Nouveau client"), this);
    QPushButton *quitButton     = new QPushButton(tr("Quitter"), this);



    limit->setObjectName("limit-spinbox");
    isJobist->setObjectName("isJobist-combobox");

    validateButton->setEnabled(false);
    clientModel->setTable("Clients");
    limit->setMinimum(-10);
    limit->setMaximum(0);

    int nameIndex               = clientModel->record().indexOf("Name");
    int phoneIndex              = clientModel->record().indexOf("phone");
    int addressIndex            = clientModel->record().indexOf("address");
    int emailIndex              = clientModel->record().indexOf("email");
    int limitIndex              = clientModel->record().indexOf("negLimit");
    int isJobistIndex           = clientModel->record().indexOf("isJobist");
    int balanceIndex            = clientModel->record().indexOf("balance");

    clientModel->setEditStrategy(QSqlTableModel::OnRowChange);
    clientModel->setSort(nameIndex, Qt::AscendingOrder);
    clientModel->setRelation(isJobistIndex, QSqlRelation("BooleanYesNo", "BoolValue", "string"));
    clientModel->select();

    clientList->setModel(clientModel);
    clientList->setModelColumn(nameIndex);
    clientList->setEditTriggers(QListView::NoEditTriggers);
    isJobist->setModel(clientModel->relationModel(isJobistIndex));
    isJobist->setModelColumn(1);

    formLayout->addWidget(formTitle);
    formLayout->addRow(tr("Nom :"), clientName);
    formLayout->addRow(tr("Solde :"), balance);
    formLayout->addRow(tr("Adresse :"), address);
    formLayout->addRow(tr("E-mail :"), email);
    formLayout->addRow(tr("Téléphone :"), phone);
    formLayout->addRow(tr("Limite :"), limit);
    formLayout->addRow(tr("Jobiste :"), isJobist);

    mapper->setModel(clientModel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->addMapping(clientName, nameIndex, "text");
    mapper->addMapping(balance, balanceIndex, "text");
    mapper->addMapping(address, addressIndex);
    mapper->addMapping(email, emailIndex);
    mapper->addMapping(phone, phoneIndex);
    mapper->addMapping(limit, limitIndex);
    mapper->addMapping(isJobist, isJobistIndex);//, "currentIndex");

    rightLayout->addWidget(quitButton);
    rightLayout->addLayout(formLayout);
    rightLayout->addWidget(validateButton);
    rightLayout->addWidget(deleteButton);
    rightLayout->addWidget(createButton);

    mainLayout->addWidget(clientList);
    mainLayout->addLayout(rightLayout);

    connect(quitButton, SIGNAL(clicked()), this, SIGNAL(finished()));
    connect(validateButton, SIGNAL(clicked()), this, SLOT(validatePressed()));
    connect(clientList, SIGNAL(clicked(const QModelIndex)), mapper, SLOT(setCurrentModelIndex(const QModelIndex)));
    connect(clientList, SIGNAL(clicked(const QModelIndex)), this, SLOT(selectionChanged()));
    connect(createButton, SIGNAL(clicked()), this, SLOT(createClient()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteClient()));

    connect(address, SIGNAL(editingFinished()), this, SLOT(dataChanged()));
    connect(email, SIGNAL(editingFinished()), this, SLOT(dataChanged()));
    connect(phone, SIGNAL(editingFinished()), this, SLOT(dataChanged()));
    connect(limit, SIGNAL(valueChanged(double)), this, SLOT(dataChanged()));
    connect(isJobist, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
}

void ClientManager::createClient(){
    QString input = QInputDialog::getText(
                this, tr("Créer un nouveau client"),
                tr("Veuillez entrer un nom pour le nouveau client."),
                QLineEdit::Normal);
    QSqlRecord record(clientModel->record());
    record.setValue("name", input);
    record.setValue("email", "");
    record.setValue("negLimit", 0);
    record.setValue("balance", 0);
    record.setValue("address", "");
    record.setValue("phone", "");
    record.setValue("isJobist", tr("Non"));
    clientModel->insertRecord(-1, record);
    clientModel->select();
}

void ClientManager::deleteClient(){
    //ask for confirmation
    auto index = clientList->currentIndex();
    bool *ok = new bool();
    QString input = QInputDialog::getText(this, tr("Veuillez confirmer la suppression"),
                                          tr("Pour confirmer la suppression du compte client, "
                                             "veuillez entrer son nom : ").append(index.data().toString()),
                                          QLineEdit::Normal, QString(), ok);
    if(*ok == false){
        delete ok;
        return;
    }
    if(input == index.data()){
        DatabaseManager::delClient(Client(index.data().toString()));
        clientModel->select();
    }else{
        QMessageBox::warning(this, tr("Erreur"), tr("Le nom entré est incorrect. La fonction n'est pas supprimée."));
    }
    delete ok;
}

void ClientManager::selectionChanged(){
    validateButton->setEnabled(false);
}

void ClientManager::dataChanged(){
    validateButton->setEnabled(true);
}

void ClientManager::validatePressed(){
    bool jobist = ((isJobist->currentIndex() == 0)?false:true);
    qreal limitValue = limit->value();
    if(!jobist && limitValue < 0){
        limit->setValue(0);
    }
    this->validateButton->setEnabled(false);
    mapper->submit();
}
