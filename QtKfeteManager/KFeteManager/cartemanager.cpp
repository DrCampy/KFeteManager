#include <QWidget>
#include <QListView>
#include <QString>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QColorDialog>

#include "cartemodel.h"
#include "carteview.h"
#include "cartemanager.h"

/*
 * Left column : articles list
 * Middle column :
 *      Top : preview
 *      Middle : 2 color selectors for text and background
 *      Bottom : buttons to confirm (and cancel)
 * Right column : CarteView
 *
 */
CarteManager::CarteManager(CarteModel *carteModel, QWidget *parent) : QWidget(parent)
{

    QVBoxLayout         *middleColumn           = new QVBoxLayout();
    QHBoxLayout         *mainLayout             = new QHBoxLayout(this);
    QSqlTableModel      *articlesModel          = new QSqlTableModel(this);
    QColorDialog        *backColor              = new QColorDialog(this);
    QColorDialog        *textColor              = new QColorDialog(this);
    QPushButton         *quit                   = new QPushButton(this);
    int                 nameIndex               = articlesModel->record().indexOf("Name");

    articlesView        = new QListView(this);
    carteView           = new CarteView(false, this);
    previewButton       = new QPushButton(this);
    validateButton      = new QPushButton(this);
    this->carteModel    = carteModel;

    carteView->setModel(carteModel);
    articlesModel->setTable("Articles");
    articlesView->setModel(articlesModel);
    articlesView->setModelColumn(nameIndex);

    //Forming the middle column
    middleColumn->addWidget(previewButton);
    middleColumn->addWidget(backColor);
    middleColumn->addWidget(textColor);
    middleColumn->addWidget(validateButton);
    middleColumn->addWidget(quit);

    mainLayout->addWidget(articlesView, 1);
    mainLayout->addLayout(middleColumn, 1);
    mainLayout->addWidget(carteView, 2);

    connect(quit, SIGNAL(clicked()), this, SIGNAL(finished()));
}
