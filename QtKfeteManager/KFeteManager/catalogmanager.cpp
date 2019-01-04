#include <QWidget>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QListView>
#include <QTableView>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleValidator>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QPushButton>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>
#include <QDebug>
#include <QCompleter>
#include <QDoubleSpinBox>
#include <QSpacerItem>
#include <QInputDialog>
#include <QMessageBox>

#include "catalogmanager.h"
#include "databasemanager.h"

CatalogManager::CatalogManager(QWidget *parent) : QWidget(parent)
{
    //Init all private members
    price       = new QDoubleSpinBox(this);
    bPrice      = new QDoubleSpinBox(this);
    jShare      = new QDoubleSpinBox(this);
    rPrice      = new QDoubleSpinBox(this);
    function    = new QComboBox(this);
    mapper      = new QDataWidgetMapper(this);
    validate    = new QPushButton(tr("Valider"));
    sqlModel    = new QSqlRelationalTableModel(this);
    QVBoxLayout *rightSidebar   = new QVBoxLayout();
    QHBoxLayout *mainLayout     = new QHBoxLayout(this);
    QFormLayout *formLayout     = new QFormLayout();
    QListView   *articlesView   = new QListView(this);
    QLabel      *formTitle      = new QLabel(tr("Article sélectionné:"));

    QSizePolicy qsp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QFont font = this->font();
    font.setPointSize(13);

    //Imediately links the views and the models
    sqlModel->setTable("Articles");

    //A few indexes
    nameIndex           = sqlModel->record().indexOf("Name");
    priceIndex          = sqlModel->record().indexOf("sellPrice");
    jobShareIndex       = sqlModel->record().indexOf("jShare");
    bPriceIndex         = sqlModel->record().indexOf("bPrice");
    redPriceIndex       = sqlModel->record().indexOf("reducedPrice");
    functionIndex       = sqlModel->record().indexOf("function");
    {
        auto tmp = new QSqlTableModel();
        tmp->setTable("Functions");
        functionNameIndex = tmp->record().indexOf("name");
        delete tmp;
    }

    //Configures the sql model
    sqlModel->setRelation(functionIndex, QSqlRelation("Functions", "Id", "name"));
    sqlModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    sqlModel->select();

    //Configure widgets
    //LineEdits and combobox
    validate->setEnabled(false);

    //Views
    articlesView->setModel(sqlModel);
    articlesView->setModelColumn(nameIndex); //Sets the column to the name
    articlesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    function->setModel(sqlModel->relationModel(functionIndex));
    function->setModelColumn(functionNameIndex);

    //Mapper
    mapper->setModel(sqlModel);
    mapper->addMapping(price, priceIndex);
    mapper->addMapping(bPrice, bPriceIndex);
    mapper->addMapping(jShare, jobShareIndex);
    mapper->addMapping(rPrice, redPriceIndex);
    mapper->addMapping(function, functionIndex);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    //Layout items
    formLayout->addWidget(formTitle);
    formLayout->setAlignment(formTitle, Qt::AlignCenter);
    formLayout->addRow(tr("Prix de vente :"), price);
    formLayout->addRow(tr("Prix d'achat :"), bPrice);
    formLayout->addRow(tr("Part jobiste :"), jShare);
    formLayout->addRow(tr("Prix réduit :"), rPrice);
    formLayout->addRow(tr("Fonction :"), function);
    formLayout->addWidget(validate);

    //Functions management
    QFormLayout *fctManagerLayout = new QFormLayout();
    QLabel      *addFunctionLabel = new QLabel(tr("Ajouter une fonction :"), this);
    QLabel      *delFunctionLabel = new QLabel(tr("Supprimer une fonction :"), this);
    QPushButton *quitButton       = new QPushButton(tr("Quitter"));
    auto        *functionsModel   = sqlModel->relationModel(functionIndex);

    addFunctionButton       = new QPushButton(tr("Ajouter une fonction"), this);
    delFunctionButton       = new QPushButton(tr("Supprimer"), this);
    newFunctionLineEdit     = new QLineEdit(this);
    selectedFctCombo        = new QComboBox(this);

    fctManagerLayout->addRow(addFunctionLabel);
    fctManagerLayout->addRow(tr("Nom :"), newFunctionLineEdit);
    fctManagerLayout->addRow(addFunctionButton);
    fctManagerLayout->addRow(delFunctionLabel);
    fctManagerLayout->addRow(tr("Fonction à supprimer :"), selectedFctCombo);
    fctManagerLayout->addRow(delFunctionButton);
    functionsModel->select();
    selectedFctCombo->setModel(functionsModel);
    selectedFctCombo->setModelColumn(functionNameIndex);
    selectedFctCombo->setCurrentIndex(1); //Selects the first function
    articlesView->setFont(font);

    //Sets fonts
    for(auto it: this->findChildren<QWidget *>()){
        it->setFont(font);
        auto label = fctManagerLayout->labelForField(it);
        if(label){
            //label->setFont(font);
        }
        label = formLayout->labelForField(it);
        if(label){
            //label->setFont(font);
        }
    }

    //sets size policy
    for(auto it: this->findChildren<QLabel *>()){
        //it->setSizePolicy(qsp);
        //it->setMaximumHeight(50);
    }

    //Configures the spinboxes
    for(auto it: this->findChildren<QDoubleSpinBox *>()){
        //it->setSizePolicy(qsp);
        //it->setMaximumHeight(50);
        it->setSingleStep(0.1);
        it->setButtonSymbols(QAbstractSpinBox::NoButtons);
        it->setSuffix(tr(" €"));
    }

    rightSidebar->addWidget(quitButton);
    rightSidebar->addLayout(formLayout, 2);
    rightSidebar->addLayout(fctManagerLayout, 1);
    mainLayout->addWidget(articlesView, 3);
    mainLayout->addLayout(rightSidebar, 4);

    //Connects the signals and slots
    connect(price, SIGNAL(valueChanged(double)), this, SLOT(entryModified()));
    connect(bPrice, SIGNAL(valueChanged(double)), this, SLOT(entryModified()));
    connect(jShare, SIGNAL(valueChanged(double)), this, SLOT(entryModified()));
    connect(rPrice, SIGNAL(valueChanged(double)), this, SLOT(entryModified()));
    connect(function, SIGNAL(currentIndexChanged(int)), this, SLOT(entryModified()));
    connect(articlesView, SIGNAL(clicked(const QModelIndex)), mapper, SLOT(setCurrentModelIndex(const QModelIndex)));
    connect(articlesView, SIGNAL(clicked(const QModelIndex)), this, SLOT(entryNotModified()));
    connect(validate, SIGNAL(clicked()), this, SLOT(entryValidated()));
    connect(addFunctionButton, SIGNAL(clicked()), this, SLOT(addFunction()));
    connect(delFunctionButton, SIGNAL(clicked()), this, SLOT(delFunction()));
    connect(quitButton, SIGNAL(clicked()), this, SIGNAL(finished()));
    connect(selectedFctCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedFctChanged(int)));

}

//To be triggered when the content of the form changes
//Enables the validate button
void CatalogManager::entryModified(){
    validate->setEnabled(true);
}

//Used to submit datas from the form as well as
//disable the validate button
void CatalogManager::entryValidated(){
    validate->setEnabled(false);
    mapper->submit();
}

//Used to keep the validate button disables in case we change the index in the ListView
void CatalogManager::entryNotModified(){
    validate->setEnabled(false);
}

void CatalogManager::addFunction(){
    DatabaseManager::addFunction(newFunctionLineEdit->text());
    newFunctionLineEdit->clear();
    refreshFctModel();
    this->entryNotModified();
}

void CatalogManager::delFunction(){
    //ask for confirmation
    QString input = QInputDialog::getText(this, tr("Veuillez confirmer la suppression"),
                                          tr("Pour confirmer la suppression de la fonction, "
                                             "veuillez entrer son nom :").append(selectedFctCombo->currentText()), QLineEdit::Normal);
    if(input == selectedFctCombo->currentText()){
        DatabaseManager::delFunction(input);
    }else{
        QMessageBox::warning(this, tr("Erreur"), tr("Le nom entré est incorrect. La fonction n'est pas supprimée."));
    }
    refreshFctModel();
}

void CatalogManager::refreshFctModel(){
    int selectedFunctionIndex = function->currentIndex();
    int selectedFctSelectedIndex = selectedFctCombo->currentIndex();
    sqlModel->relationModel(functionIndex)->select();
    selectedFctCombo->setCurrentIndex(selectedFctSelectedIndex);
    function->setCurrentIndex(selectedFunctionIndex);
    this->entryNotModified();
}


void CatalogManager::selectedFctChanged(int i){
    if(i == 0){
        delFunctionButton->setEnabled(false);
    }else{
        delFunctionButton->setEnabled(true);
    }
}

