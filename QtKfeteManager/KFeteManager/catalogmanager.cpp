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
#include <QSqlError>


#include "catalogmanager.h"
#include "catalog.h"
#include "databasemanager.h"

CatalogManager::CatalogManager(QWidget *parent) : QWidget(parent)
{
    //Init all private members
    price                       = new QDoubleSpinBox(this);
    bPrice                      = new QDoubleSpinBox(this);
    jShare                      = new QDoubleSpinBox(this);
    rPrice                      = new QDoubleSpinBox(this);
    function                    = new QComboBox(this);
    mapper                      = new QDataWidgetMapper(this);
    validate                    = new QPushButton(tr("Valider"), this);
    deleteArticleButton         = new QPushButton(tr("Supprimer"), this);
    sqlModel                    = new QSqlRelationalTableModel(this);
    articlesView                = new QListView(this);
    formTitle                   = new QLabel(tr("Article sélectionné : Aucun"), this);

    QVBoxLayout *rightSidebar   = new QVBoxLayout();
    QHBoxLayout *mainLayout     = new QHBoxLayout(this);
    QFormLayout *formLayout     = new QFormLayout();

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
    sqlModel->select();
    sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
    //Configure widgets
    deleteArticleButton->setEnabled(false);
    //LineEdits and combobox
    validate->setEnabled(false);

    //Views
    articlesView->setModel(sqlModel);
    articlesView->setModelColumn(nameIndex); //Sets the column to the name
    articlesView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    articlesView->setWrapping(true);
    sqlModel->setSort(nameIndex, Qt::AscendingOrder);
    function->setModel(sqlModel->relationModel(functionIndex));
    function->setModelColumn(functionNameIndex);

    //Mapper
    mapper->setModel(sqlModel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(price, priceIndex);
    mapper->addMapping(bPrice, bPriceIndex);
    mapper->addMapping(jShare, jobShareIndex);
    mapper->addMapping(rPrice, redPriceIndex);
    mapper->addMapping(function, functionIndex);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    //Layout items
    formLayout->addRow(formTitle);
    formLayout->addRow(tr("Prix de vente :"), price);
    formLayout->addRow(tr("Prix d'achat :"), bPrice);
    formLayout->addRow(tr("Part jobiste :"), jShare);
    formLayout->addRow(tr("Prix réduit :"), rPrice);
    formLayout->addRow(tr("Fonction :"), function);
    formLayout->addWidget(validate);
    formLayout->addWidget(deleteArticleButton);

    //Functions management
    QFormLayout *fctManagerLayout = new QFormLayout();
    QLabel      *addFunctionLabel = new QLabel(tr("Ajouter une fonction :"), this);
    QLabel      *delFunctionLabel = new QLabel(tr("Supprimer une fonction :"), this);
    QPushButton *quitButton       = new QPushButton(tr("Quitter"), this);
    auto        *functionsModel   = sqlModel->relationModel(functionIndex);

    addFunctionButton       = new QPushButton(tr("Ajouter"), this);
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

    QPushButton *addArticleButton = new QPushButton(tr("Créer un nouvel article"), this);

    //Sets fonts
    for(auto it: this->findChildren<QWidget *>()){
        it->setFont(font);
        auto label = fctManagerLayout->labelForField(it);
        if(label){
            label->setFont(font);
        }
        label = formLayout->labelForField(it);
        if(label){
            label->setFont(font);
        }
    }

    //Configures the spinboxes
    for(auto it: this->findChildren<QDoubleSpinBox *>()){
        it->setSingleStep(0.1);
        it->setButtonSymbols(QAbstractSpinBox::NoButtons);
        it->setSuffix(tr(" €"));
        it->setMinimum(-qInf());
        it->setMaximum(qInf());
    }

    QFrame *line1 = new QFrame(this);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    line1->setLineWidth(1);
    QFrame *line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    line2->setLineWidth(1);

    formLayout->setAlignment(Qt::AlignCenter);
    fctManagerLayout->setAlignment(Qt::AlignBottom);

    rightSidebar->addWidget(quitButton, 0, Qt::AlignTop);
    rightSidebar->addLayout(formLayout, 0);
    rightSidebar->addWidget(line1, 0, Qt::AlignHCenter);
    rightSidebar->addWidget(addArticleButton, 0, Qt::AlignHCenter);
    rightSidebar->addWidget(line2, 0, Qt::AlignHCenter);
    rightSidebar->addLayout(fctManagerLayout, 0);

    mainLayout->addWidget(articlesView, 3);
    mainLayout->addLayout(rightSidebar, 4);

    //Connects the signals and slots
    connect(price, SIGNAL(valueChanged(double)), this, SLOT(entryModified()));
    connect(bPrice, SIGNAL(valueChanged(double)), this, SLOT(entryModified()));
    connect(jShare, SIGNAL(valueChanged(double)), this, SLOT(entryModified()));
    connect(rPrice, SIGNAL(valueChanged(double)), this, SLOT(entryModified()));
    connect(function, SIGNAL(currentIndexChanged(int)), this, SLOT(entryModified()));
    connect(articlesView, SIGNAL(clicked(const QModelIndex)), mapper, SLOT(setCurrentModelIndex(const QModelIndex)));
    connect(articlesView, SIGNAL(clicked(const QModelIndex)), this, SLOT(selectionChanged()));
    connect(validate, SIGNAL(clicked()), this, SLOT(entryValidated()));
    connect(addFunctionButton, SIGNAL(clicked()), this, SLOT(addFunction()));
    connect(delFunctionButton, SIGNAL(clicked()), this, SLOT(delFunction()));
    connect(quitButton, SIGNAL(clicked()), this, SIGNAL(finished()));
    connect(selectedFctCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedFctChanged(int)));
    connect(addArticleButton, SIGNAL(clicked()), this, SLOT(createArticle()));
    connect(deleteArticleButton, SIGNAL(clicked()), this, SLOT(deleteArticle()));
}

//To be triggered when the content of the form changes
//Enables the validate button
void CatalogManager::entryModified(){
    validate->setEnabled(true);
}

//Used to submit datas from the form as well as
//disable the validate button
void CatalogManager::entryValidated(){
    bool tmp = mapper->submit();
    if(tmp == true){
        qDebug() << "Validate returned true";
    }else{
        qDebug() << "Validate returned false";
    }
    validate->setEnabled(false);

}

//Used to keep the validate button disables in case we change the index in the ListView
// and update the title of the form
void CatalogManager::selectionChanged(){
    validate->setEnabled(false);
    QString formTitleStr(tr("Article sélectionné : "));
    QString articleName = articlesView->currentIndex().data().toString();
    if(articleName == ""){
        formTitleStr.append("Aucun");
    }else{
        formTitleStr.append(articleName);
    }
    formTitle->setText(formTitleStr);
    deleteArticleButton->setEnabled(true);
}

void CatalogManager::addFunction(){
    DatabaseManager::addFunction(newFunctionLineEdit->text());
    newFunctionLineEdit->clear();
    refreshFctModel();
    this->selectionChanged();
}

void CatalogManager::delFunction(){
    //ask for confirmation
    bool *ok = new bool();
    QString input = QInputDialog::getText(this, tr("Veuillez confirmer la suppression"),
                                          tr("Pour confirmer la suppression de la fonction, "
                                             "veuillez entrer son nom : ") + selectedFctCombo->currentText(),
                                          QLineEdit::Normal, QString(), ok);
    if(*ok == false){
        delete ok;
        return;
    }
    if(input == selectedFctCombo->currentText()){
        DatabaseManager::delFunction(input);
    }else{
        QMessageBox::warning(this, tr("Erreur"), tr("Le nom entré est incorrect. La fonction n'est pas supprimée."));
    }
    refreshFctModel();
    delete ok;
}

void CatalogManager::refreshFctModel(){
    int selectedFunctionIndex = function->currentIndex();
    int selectedFctSelectedIndex = selectedFctCombo->currentIndex();
    sqlModel->relationModel(functionIndex)->select();
    selectedFctCombo->setCurrentIndex(selectedFctSelectedIndex);
    function->setCurrentIndex(selectedFunctionIndex);
    this->selectionChanged();
}


void CatalogManager::selectedFctChanged(int i){
    if(i == 0){
        delFunctionButton->setEnabled(false);
    }else{
        delFunctionButton->setEnabled(true);
    }
}

void CatalogManager::createArticle(){
    QString input = QInputDialog::getText(
                this, tr("Créer un nouvel article"),
                tr("Veuillez entrer un nom pour le nouvel article."),
                QLineEdit::Normal);
    Article a(input);
    a.create();
    auto selection = articlesView->currentIndex();
    sqlModel->select();
    articlesView->setCurrentIndex(selection);
    mapper->setCurrentModelIndex(selection);
}

void CatalogManager::deleteArticle(){
    bool *ok = new bool();
    QString input = QInputDialog::getText(this, tr("Veuillez confirmer la suppression"),
                                          tr("Pour confirmer la suppression de l'article, "
                                          "veuillez entrer son nom :") + articlesView->currentIndex().data().toString(),
                                          QLineEdit::Normal, QString(), ok);
    if(*ok == false){
        delete ok;
        return;
    }

    if(input == articlesView->currentIndex().data().toString()){
        sqlModel->removeRow(articlesView->currentIndex().row());
    }else{
        QMessageBox::warning(this, tr("Erreur"), tr("Le nom entré est incorrect. L'article n'est pas supprimé."));
    }
    sqlModel->select();
    delete ok;
}
