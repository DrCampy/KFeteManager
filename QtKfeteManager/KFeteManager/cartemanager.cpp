#include <QWidget>
#include <QListView>
#include <QString>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QColorDialog>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QDebug>

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
    QPushButton         *quit                   = new QPushButton(tr("Quitter"), this);
    QPushButton         *changeBackColor        = new QPushButton(tr("Changer la couleur de fond"), this);
    QPushButton         *changeTextColor        = new QPushButton(tr("Changer la couleur du texte"), this);
    QSizePolicy         qsp(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::PushButton);
    int                 nameIndex               = articlesModel->record().indexOf("Name");

    QFrame *line1 = new QFrame(this);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    line1->setLineWidth(1);
    QFrame *line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    line2->setLineWidth(1);

    articlesView        = new QListView(this);
    carteView           = new CarteView(this, false);
    previewButton       = new QPushButton(this);
    validateButton      = new QPushButton(tr("Valider"), this);
    this->carteModel    = carteModel;


    defaultTextColor = previewButton->palette().color(QWidget::backgroundRole());
    defaultBackgroundColor = previewButton->palette().color(QWidget::foregroundRole());

    qDebug() << "Texte : " << defaultTextColor << " Background : " << defaultBackgroundColor;

    carteView->setModel(carteModel);
    articlesModel->setTable("Articles");
    articlesView->setModel(articlesModel);
    articlesView->setModelColumn(nameIndex);
    articlesView->setEditTriggers(QListView::NoEditTriggers);
    previewButton->setSizePolicy(qsp);
    previewButton->setMaximumSize(150, 150);

    //Forming the middle column
    middleColumn->addWidget(previewButton, 1);
    middleColumn->addWidget(line1);
    middleColumn->addWidget(changeBackColor);
    middleColumn->addWidget(changeTextColor);
    middleColumn->addWidget(line2);
    middleColumn->addWidget(validateButton);
    middleColumn->addWidget(quit);
    mainLayout->addWidget(articlesView, 2);
    mainLayout->addLayout(middleColumn, 1);
    mainLayout->addWidget(carteView, 3);


    articlesModel->select();
    connect(quit, SIGNAL(clicked()), this, SIGNAL(finished()));
    connect(carteView->carteButtons, SIGNAL(buttonClicked(int)), this, SLOT(selectButton(int)));
    connect(articlesView, SIGNAL(activated(const QModelIndex)), this, SLOT(selectedArticle(const QModelIndex)));

}

void CarteManager::selectButton(int i){
    selectedIndex = i;
    auto details = carteModel->getButton(static_cast<unsigned int>(selectedIndex));
    if(details){
        backgroundColor = details->getBackgroundColor();
        textColor = details->getTextColor();
        text = details->getName();
    }else{
        backgroundColor = defaultBackgroundColor;
        textColor = defaultTextColor;
        text = tr("<Vide>");
    }
    loadPreview();
}

//TODO unselect articlesList
void CarteManager::loadPreview(){
    QString ss;
    ss.append("color: ");
    ss.append(backgroundColor.name());
    ss.append("; background-color: ");
    ss.append(textColor.name());
    ss.append(";");// border: none;");
    previewButton->setStyleSheet(ss);
    previewButton->setText(text);
}

void CarteManager::confirmChanges(){

}

void CarteManager::selectedArticle(const QModelIndex &index){
    this->text = index.data().toString();
    loadPreview();
}
