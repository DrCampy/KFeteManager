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


#include <QDebug>
#include <QSqlError>


#include "clientlist.h"
#include "clientmanager.h"

ClientManager::ClientManager(QWidget *parent) : QWidget(parent)
{
    clientModel     = new QSqlRelationalTableModel(this);
    clientList      = new QListView(this);

    QHBoxLayout *mainLayout     = new QHBoxLayout(this);
    QVBoxLayout *rightLayout    = new QVBoxLayout();
    QFormLayout *formLayout     = new QFormLayout();
    QLabel *formTitle           = new QLabel(tr("Article sélectionné :"), this);
    QDataWidgetMapper *mapper   = new QDataWidgetMapper(this);
    QPushButton *validateButton = new QPushButton(tr("Sauver"), this);
    QPushButton *deleteButton   = new QPushButton(tr("Supprimer client"), this);
    QPushButton *createButton   = new QPushButton(tr("Nouveau client"), this);
    QPushButton *quitButton     = new QPushButton(tr("Quitter"), this);

    //form widgets
    QLabel *clientName          = new QLabel(this);
    QLabel *balance             = new QLabel(this);
    QLineEdit *address          = new QLineEdit(this);
    QLineEdit *email            = new QLineEdit(this);
    QLineEdit *phone            = new QLineEdit(this);
    QDoubleSpinBox *limit       = new QDoubleSpinBox(this);
    QComboBox *isJobist         = new QComboBox(this);

    //validateButton->setEnabled(false);
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
    //isJobist->insertItem(0, tr("Non"));
    //isJobist->insertItem(1, tr("Oui"));
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
    connect(validateButton, SIGNAL(clicked()), mapper, SLOT(submit()));
    connect(clientList, SIGNAL(clicked(const QModelIndex)), mapper, SLOT(setCurrentModelIndex(const QModelIndex)));
    connect(createButton, SIGNAL(clicked()), this, SLOT(createClient()));
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
    record.setValue("isJobist", false);
    clientModel->insertRecord(-1, record);

    clientModel->select();
}

