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

#include "catalogmanager.h"
#include "databasemanager.h"

CatalogManager::CatalogManager(QWidget *parent) : QWidget(parent)
{
    QWidget *form = new QWidget(this);
    //Init all private members
    price       = new QDoubleSpinBox(form);
    bPrice      = new QDoubleSpinBox(form);
    jShare      = new QDoubleSpinBox(form);
    rPrice      = new QDoubleSpinBox(form);
    function    = new QComboBox(form);
    mapper      = new QDataWidgetMapper(this);
    validate    = new QPushButton(tr("Valider"));
    sqlModel    = new QSqlRelationalTableModel(this);
    QVBoxLayout *rightSidebar   = new QVBoxLayout();
    QHBoxLayout *mainLayout     = new QHBoxLayout(this);
    QFormLayout *formLayout     = new QFormLayout();
    QListView *articlesView     = new QListView(this);
    QSizePolicy qsp(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QFont font = this->font();
    font.setPointSize(13);
    articlesView->setFont(font);

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

    //sets size policy
    for(auto it: form->findChildren<QWidget *>()){
        it->setSizePolicy(qsp);
        it->setMaximumHeight(50);
    }

    //Configures the spinboxes
    for(auto it: form->findChildren<QDoubleSpinBox *>()){
        it->setSingleStep(0.1);
        it->setButtonSymbols(QAbstractSpinBox::NoButtons);
        it->setSuffix(tr(" €"));
    }

    //Layout items
    QLabel *formTitle = new QLabel(tr("Article sélectionné:"));
    formTitle->setFont(font);
    formLayout->addWidget(formTitle);
    formLayout->setAlignment(formTitle, Qt::AlignCenter);
    formLayout->addRow(tr("Prix de vente :"), price);
    formLayout->addRow(tr("Prix d'achat :"), bPrice);
    formLayout->addRow(tr("Part jobiste :"), jShare);
    formLayout->addRow(tr("Prix réduit :"), rPrice);
    formLayout->addRow(tr("Fonction :"), function);
    formLayout->addWidget(validate);

    for(auto it: form->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)){
        it->setFont(font);
        formLayout->labelForField(it)->setFont(font);
    }
    validate->setFont(font);

    //Functions management
    QWidget *fctManager = new QWidget(this);
    QFormLayout *fctManagerLayout = new QFormLayout();
    QPushButton *addFunction = new QPushButton(tr("Ajouter une fonction"), fctManager);
    QPushButton *delFunction = new QPushButton(tr("Supprimer"), fctManager);
    QLabel *addFunctionLabel = new QLabel(tr("Ajouter une fonction :"), fctManager);
    QLabel *delFunctionLabel = new QLabel(tr("Supprimer une fonction :"), fctManager);
    QLineEdit *newFunctionName = new QLineEdit(fctManager);
    QComboBox *selectedFct = new QComboBox(fctManager);
    fctManagerLayout->addRow(addFunctionLabel);
    fctManagerLayout->addRow(tr("Nom :"), newFunctionName);
    fctManagerLayout->addRow(addFunction);
    fctManagerLayout->addItem(new QSpacerItem(10, 80));
    fctManagerLayout->addRow(delFunctionLabel);
    fctManagerLayout->addRow(tr("Fonction à supprimer :"), selectedFct);
    fctManagerLayout->addRow(delFunction);

    newFunctionName->setObjectName("New function name");
    auto functions = sqlModel->relationModel(functionIndex);
    functions->select();
    selectedFct->setObjectName("Selected Function");
    selectedFct->setModel(functions);
    selectedFct->setModelColumn(functionNameIndex);
    selectedFct->setCurrentIndex(0);

    for(auto it: fctManager->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)){
        it->setFont(font);
        auto label = fctManagerLayout->labelForField(it);
        if(label){
            label->setFont(font);
        }
    }

    rightSidebar->addLayout(formLayout, 2);
    //rightSidebar->addItem(new QSpacerItem(10, 80));
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
    connect(addFunction, SIGNAL(clicked()), this, SLOT(addFunction()));
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
    auto fctName = this->findChild<QLineEdit *>("New function name");
    DatabaseManager::addFunction(fctName->text());
    fctName->clear();
    sqlModel->relationModel(functionIndex)->select();
    auto fctSelected = this->findChild<QComboBox *>("Selected Function");
    fctSelected->setCurrentIndex(0);
}

