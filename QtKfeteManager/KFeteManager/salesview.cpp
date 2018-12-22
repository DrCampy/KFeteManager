#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QCloseEvent>
#include <QStringList>
#include <QSizePolicy>
#include <QMap>
#include <QApplication>
#include <QColorDialog>
#include <QColor>
#include <QPalette>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>

#include "article.h"
#include "salesview.h"

SalesView::SalesView(QWidget *parent) : QWidget(parent)
{
  //setAttribute(Qt::WA_DeleteOnClose); // deletes the widget when closed

  QHBoxLayout *hBox = new QHBoxLayout();
  QVBoxLayout *vBox = new QVBoxLayout(this);
  TopBar *topBar = new TopBar();
  //Commande *commande = new Commande(cat);
  //carte = new Carte(cat, hasRights);
  //QTimer *saveTimer = new QTimer(this);

  //saveTimer->start(60000);//run a timer to save files every 60 seconds
  //cat->importCatalog(FICHIER_CATALOGUE);
  carteView = new CarteView(this);

  vBox->addWidget(topBar, 1);
  hBox->addWidget(currentOrderView, 7);
  //hBox->addWidget(commande, 7);
  hBox->addWidget(carteView, 8);
  vBox->addLayout(hBox, 6);


  //connect(carteView, SIGNAL(clicArticle(QString)), commande, SLOT(addArticle(QString)) );
}

TopBar::TopBar(QWidget *parent) : QWidget(parent)
{
  QHBoxLayout *hBox = new QHBoxLayout(this);
  QWidget *bar = new QWidget(this);
  QButtonGroup *bGroup = new QButtonGroup(this);

  QStringList buttonsNames;
  buttonsNames << tr("Dépôt") << tr("Retrait") << tr("Ouvrir la Caisse") << tr("Compter la Caisse")
               << tr("Comptes") << tr("Commandes") << tr("Valider");

  QPushButton *tmp;
  for(auto it = buttonsNames.begin(); it != buttonsNames.end(); it++){
      tmp = new QPushButton(*it, bar);
      tmp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      hBox->addWidget(tmp);
      bGroup->addButton(tmp, (it-buttonsNames.begin()));
    }

  hBox->setSpacing(0);
}



CarteView::CarteView(QWidget *parent) : QWidget (parent)
{

}

CarteView::CarteView(Catalog *cat, bool hasRights, QWidget *parent) : QWidget(parent)
{
  if(PAGES_NAMES.size() != NB_MENU_PAGES){
    throw std::out_of_range("From class Carte : NB_MENU_PAGES not less or equal PAGES_NAMES.size()");
    }
  this->hasRights = hasRights;
  this->cat = cat; //initialise the articles catalog to refer to

  QVBoxLayout *vBox = new QVBoxLayout(this); //first of all widgets
  QHBoxLayout *hBox = new QHBoxLayout; //Layout for page name buttons
  hBox->setSpacing(0);
  vBox->setSpacing(0);
  QWidget *hBar = new QWidget;
  QStackedLayout *sLayout = new QStackedLayout; //main layout with pages inside
  QButtonGroup *tabs = new QButtonGroup(this);
  carteButtons = new QButtonGroup(this);

  lookupTable = new QMap<int, QString>();

  QWidget *pagesWidget = new QWidget[NB_MENU_PAGES]; //widget to store pages
  QGridLayout *pagesLayout[5]; // pages

  //set a size policy for the buttons to expand at max
  QSizePolicy qsp(QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::PushButton);
  CarteButton *tmp;
  QPalette defaultPalette = QPushButton(nullptr).palette();
  defaultPalette.setColor(QPalette::Button, Qt::lightGray);
  for(size_t i = 0; i < NB_MENU_PAGES; i++){
      pagesLayout[i] = new QGridLayout(&pagesWidget[i]);
      pagesLayout[i]->setSpacing(0);
      //Fills pages
      for(unsigned int h = 0; h < GRID_H; h++){
          for(unsigned int w = 0; w < GRID_W; w++){
            //w+(h*GRID_W)+(GRID_H*GRID_W*i)
              //Generating buttons for the carte
            tmp = new CarteButton(&pagesWidget[i]);
            tmp->setSizePolicy(qsp);
            tmp->updateColor(defaultPalette);
            tmp->allowColorChange(hasRights);
            carteButtons->addButton(tmp, w + (h*GRID_W) + (GRID_H*GRID_W*i));
            pagesLayout[i]->addWidget(tmp, h, w);
          }
      }

      //insert pages in QStackedLayout
      sLayout->addWidget(&pagesWidget[i]);

      //Generating tabs
      //Insert buttons in buttonGroup for tabs
      tmp = new CarteButton(PAGES_NAMES.at(i), hBar);
      tmp->setMinimumHeight(25);
      tmp->setMaximumHeight(80);
      tmp->setSizePolicy(qsp);
      tmp->updateColor(defaultPalette);
      tmp->allowColorChange(hasRights);
      hBox->addWidget(tmp, Qt::AlignCenter);
      tabs->addButton(tmp, i);
  }

  vBox->addLayout(sLayout, 8);
  vBox->addLayout(hBox, 1);

  connect(tabs, SIGNAL(buttonClicked(int)), sLayout, SLOT(setCurrentIndex(int)));
  connect(carteButtons, SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)) );
  tabs->button(0)->click();
}

void CarteView::buttonClicked(int id){
  if(lookupTable->contains(id)){
          emit clicArticle( *(lookupTable->find(id)) );
    }
}

void CarteButton::mousePressEvent(QMouseEvent *e){
  if(e->modifiers() == Qt::ShiftModifier && this->canChangeColor){
      if(this->text() != "")
      changeColor();
      return;
    }
  QPushButton::mousePressEvent(e);
}

