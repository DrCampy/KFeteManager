#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QMap>
#include <QDebug>
#include <QToolButton>
#include <QCompleter>
#include <QLineEdit>

#include "carteview.h"
#include "cartemodel.h"

CarteView::CarteView(bool searchBar, QWidget *parent) : QWidget(parent)
{
    //Checks the number of pages
    if(PAGES_NAMES.size() != NB_MENU_PAGES){
        throw std::out_of_range("From class Carte : NB_MENU_PAGES not less or equal PAGES_NAMES.size()");
    }

    QVBoxLayout *vBox       = new QVBoxLayout(this); //Main Layout
    QHBoxLayout *hBox       = new QHBoxLayout(); //Layout for page pages buttons
    QStackedLayout *sLayout = new QStackedLayout(); //main layout with pages inside
    QButtonGroup *tabs      = new QButtonGroup(this);
    carteButtons            = new QButtonGroup(this);
    this->searchBar         = searchBar;
    hBox->setSpacing(0);
    vBox->setSpacing(0);

    //set a size policy for the buttons to expand at max
    QSizePolicy qsp(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::PushButton);

    QPushButton *tmp;
    QPalette defaultPalette = QPushButton(nullptr).palette();
    defaultPalette.setColor(QPalette::Button, Qt::lightGray);
    for(int i = 0; i < static_cast<int>(NB_MENU_PAGES); i++){
        QWidget *page = new QWidget(this);
        QGridLayout *gridLayout = new QGridLayout(page);
        gridLayout->setSpacing(0);
        //Fills pages
        for(int h = 0; h < static_cast<int>(GRID_H); h++){
            for(int w = 0; w < static_cast<int>(GRID_W); w++){
                //w+(h*GRID_W)+(GRID_H*GRID_W*i)
                //Generating buttons for the carte
                tmp = new QPushButton(page);
                tmp->setSizePolicy(qsp);
                carteButtons->addButton(tmp, w + (h*static_cast<int>(GRID_W)) + (static_cast<int>(GRID_H)*static_cast<int>(GRID_W)*i));
                gridLayout->addWidget(tmp, static_cast<int>(h), static_cast<int>(w));
            }
        }

        //insert pages in QStackedLayout
        sLayout->addWidget(page);

        //Generating tabs
        //Insert buttons in buttonGroup for tabs
        tmp = new QPushButton(PAGES_NAMES.at(i), this);
        tmp->setMinimumHeight(25);
        tmp->setMaximumHeight(80);
        tmp->setSizePolicy(qsp);
        tmp->setCheckable(true);
        hBox->addWidget(tmp, Qt::AlignCenter);
        tabs->addButton(tmp, i);
    }
    if(searchBar){
        //Inserts the search button
        search = new QToolButton(this);
        search->setText(tr("&Rechercher"));
        search->setPopupMode(QToolButton::InstantPopup);
        search->setArrowType(Qt::NoArrow);
        search->setShortcut(Qt::CTRL+Qt::Key_F);
        search->setMinimumHeight(25);
        search->setMaximumHeight(80);
        search->setSizePolicy(qsp);
        hBox->addWidget(search);
    }
    vBox->addLayout(sLayout, 8);
    vBox->addLayout(hBox, 1);

    tabs->setExclusive(true);

    connect(tabs, SIGNAL(buttonClicked(int)), sLayout, SLOT(setCurrentIndex(int)));
    tabs->button(0)->click();
}

void CarteView::updateButton(unsigned int id){
    auto details = model->getButton(id);
    if(details == nullptr){
        //disables button;
        carteButtons->button(static_cast<int>(id))->setDisabled(true);
    }else{
        QAbstractButton *button = carteButtons->button(static_cast<int>(id));
        button->setDisabled(false);
        QString ss;
        ss.append("color: ");
        ss.append(details->getTextColor().name());
        ss.append("; background-color: ");
        ss.append(details->getBackgroundColor().name());
        //ss.append(";");
        ss.append("; border: none;");
        button->setStyleSheet(ss);
        button->setText(details->getName());
        qDebug() << ss;
    }
}

void CarteView::updateView(){
    for(unsigned int i = 0; i < NB_MENU_PAGES*GRID_H*GRID_W; i++){
        updateButton(i);
    }
}

void CarteView::setModel(CarteModel *model){
    this->model = model;
    if(searchBar){
        Searcher *searcher = new Searcher(model->getArticlesList(), this);
        search->addAction(searcher);
        connect(searcher, SIGNAL(articleSearched(QString)), model, SIGNAL(articleClicked(QString)));
    }
    connect(model, SIGNAL(modelUpdated()), this, SLOT(updateView()));
    connect(carteButtons, SIGNAL(buttonClicked(int)), model, SLOT(buttonClicked(int)) );
    updateView();
}

Searcher::Searcher(const QStringList *list, QWidget *parent) : QWidgetAction (parent){
    this->list = list;
}

QWidget *Searcher::createWidget(QWidget *parent){
    QCompleter *completer = new QCompleter(*list, parent);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit = new QLineEdit(parent);
    lineEdit->setCompleter(completer);
    lineEdit->setFocus(Qt::MenuBarFocusReason);
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    return lineEdit;
}

void Searcher::returnPressed(){
    emit articleSearched(lineEdit->text());
}
