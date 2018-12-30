#include <QWidget>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

#include "catalogmanager.h"


CatalogManager::CatalogManager(QWidget *parent) : QWidget(parent)
{
    //Init all private members
    price       = new QLineEdit(this);
    bPrice      = new QLineEdit(this);
    jShare      = new QLineEdit(this);
    rPrice      = new QLineEdit(this);
    function    = new QComboBox(this);
    mapper      = new QDataWidgetMapper(this);
    validate    = new QPushButton(tr("Valider"));
    sqlModel    = new QSqlRelationalTableModel(this);
    QSqlTableModel *fctModel    = new QSqlTableModel(this);

    QVBoxLayout *rightSidebar   = new QVBoxLayout();
    QHBoxLayout *mainLayout     = new QHBoxLayout(this);
    QVBoxLayout *formLayout     = new QVBoxLayout();
    QDoubleValidator *v         = new QDoubleValidator(this);
    QListView *articlesView     = new QListView(this);

    //Imediately links the views and the models
    sqlModel->setTable("Articles");
    fctModel->setTable("Functions");
    fctModel->select();

    //A few indexes
    int nameIndex           = sqlModel->record().indexOf("Name");
    int priceIndex          = sqlModel->record().indexOf("sellPrice");
    int jobShareIndex       = sqlModel->record().indexOf("jShare");
    int bPriceIndex         = sqlModel->record().indexOf("bPrice");
    int redPriceIndex       = sqlModel->record().indexOf("reducedPrice");
    int functionIndex       = sqlModel->record().indexOf("function");
    int functionMode        = fctModel->record().indexOf("name");

    //Configure widgets
    //LineEdits and combobox
    v->setNotation(QDoubleValidator::StandardNotation);
    price->setValidator(new QDoubleValidator());
    bPrice->setValidator(v);
    jShare->setValidator(v);
    rPrice->setValidator(v);
    validate->setEnabled(false);

    //Model
    sqlModel->setRelation(functionIndex, QSqlRelation("Functions", "Id", "name"));
    sqlModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    //Views
    //articlesView->setModelColumn(nameIndex); //Sets the column to the name
    articlesView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //Mapper
    mapper->setModel(sqlModel);
    mapper->addMapping(price, priceIndex);
    mapper->addMapping(bPrice, bPriceIndex);
    mapper->addMapping(jShare, jobShareIndex);
    mapper->addMapping(rPrice, redPriceIndex);
    mapper->addMapping(function, functionIndex);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    function->setModel(sqlModel);
    function->setModelColumn(functionIndex);
    function->setItemDelegate(new QSqlRelationalDelegate(function));

    /*QCompleter *completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    completer->setModel(fctModel);
    completer->setCompletionColumn(functionMode);
    function->setCompleter(completer);*/



    //Layout items
    formLayout->addWidget(new QLabel(tr("Prix de vente :"), this), 0, Qt::AlignBottom);
    formLayout->addWidget(price, 0, Qt::AlignTop);

    formLayout->addWidget(new QLabel(tr("Prix d'achat :"), this), 0, Qt::AlignBottom);
    formLayout->addWidget(bPrice, 0, Qt::AlignTop);

    formLayout->addWidget(new QLabel(tr("Part jobiste :"), this), 0, Qt::AlignBottom);
    formLayout->addWidget(jShare, 0, Qt::AlignTop);

    formLayout->addWidget(new QLabel(tr("Prix réduit :"), this), 0, Qt::AlignBottom);
    formLayout->addWidget(rPrice, 0, Qt::AlignTop);

    formLayout->addWidget(new QLabel(tr("Fonction :"), this), 0, Qt::AlignBottom);
    formLayout->addWidget(function, 0, Qt::AlignTop);

    formLayout->addWidget(validate);

    rightSidebar->addLayout(formLayout);
    mainLayout->addWidget(articlesView);
    mainLayout->addLayout(rightSidebar);

    //TODO do not work
    setTabOrder(price, bPrice);
    setTabOrder(bPrice, jShare);
    setTabOrder(jShare, rPrice);
    //setTabOrder(rPrice, function);
    //setTabOrder(function, validate);
    //this->setTabOrder(validate, price);

    articlesView->setModel(sqlModel);
    sqlModel->select();

    connect(price, SIGNAL(editingFinished()), this, SLOT(entryModified()));
    connect(bPrice, SIGNAL(editingFinished()), this, SLOT(entryModified()));
    connect(jShare, SIGNAL(editingFinished()), this, SLOT(entryModified()));
    connect(rPrice, SIGNAL(editingFinished()), this, SLOT(entryModified()));
    connect(function, SIGNAL(currentIndexChanged(int)), this, SLOT(entryModified()));
    connect(validate, SIGNAL(clicked()), this, SLOT(entryValidated()));
    connect(articlesView, SIGNAL(clicked(const QModelIndex)), mapper, SLOT(setCurrentModelIndex(const QModelIndex)));
}

void CatalogManager::entryModified(){
    isEntryModified = true;
    validate->setEnabled(isEntryModified);
}

void CatalogManager::entryValidated(){
    mapper->submit();
    sqlModel->submitAll();
    isEntryModified = false;
    validate->setEnabled(false);

}
