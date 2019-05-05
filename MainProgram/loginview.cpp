#include <QButtonGroup>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "loginview.h"

LoginView::LoginView(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose); // deletes the widget when closed


    QButtonGroup *numpad = new QButtonGroup(this);
    QVBoxLayout *vLayout = new QVBoxLayout(), *mainVLayout = new QVBoxLayout(this);
    QHBoxLayout *hLayout = new QHBoxLayout(), *mainHLayout = new QHBoxLayout();
    QGridLayout *gridLayout = new QGridLayout();

    //Password display line
    passwordDisplay = new QLineEdit(this);
    passwordDisplay->setEchoMode(QLineEdit::Password);
    passwordDisplay->setAlignment(Qt::AlignCenter);
    passwordDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //Creating the numpad and buttons
    QPushButton *tmp, *delButton = new QPushButton("C", this);
    delButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QStringList buttonsLabels = {"1", "2", "3", "4", "5", "6", "7", "8", "9", tr("Annuler"), "0", tr("Valider")};
    QList<int> buttonsIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 11}; //Buttons IDs are their numerical value. 10 for cancel, 11 for validate

    //Fills the numpad
    for(unsigned int y = 0; y < 4; y++){
        for(unsigned int x = 0; x < 3; x++){
            tmp = new QPushButton(buttonsLabels.at( static_cast<int>(x+(3*y)) ));
            tmp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            gridLayout->addWidget(tmp, static_cast<int>(y), static_cast<int>(x));
            numpad->addButton(tmp, buttonsIDs.at( static_cast<int>(x+(3*y)) ));
            if(x+(3*y) == 11){ //"Validate" button
                tmp->setShortcut(QKeySequence(Qt::Key_Enter));
            }
        }
    }

    gridLayout->setSpacing(0);

    hLayout->addWidget(passwordDisplay, 6);
    hLayout->addWidget(delButton, 1);


    vLayout->addLayout(hLayout, 1);
    vLayout->addLayout(gridLayout, 8);

    mainHLayout->addSpacerItem(new QSpacerItem(350, 350, QSizePolicy::Expanding, QSizePolicy::Expanding));
    mainHLayout->addLayout(vLayout, Qt::AlignCenter);
    mainHLayout->addSpacerItem(new QSpacerItem(350, 350, QSizePolicy::Expanding, QSizePolicy::Expanding));

    mainVLayout->addSpacerItem(new QSpacerItem(150, 150, QSizePolicy::Expanding, QSizePolicy::Expanding));
    mainVLayout->addLayout(mainHLayout, Qt::AlignCenter);
    mainVLayout->addSpacerItem(new QSpacerItem(150, 150, QSizePolicy::Expanding, QSizePolicy::Expanding));


    connect(delButton, SIGNAL(clicked(bool)), this, SLOT(clicDelete(bool)));
    connect(passwordDisplay, SIGNAL(returnPressed()), this, SLOT(returnPressed()) );
    connect(numpad, SIGNAL(buttonClicked(int)), this, SLOT(clicgridLayout(int)) );

}

void LoginView::clicNumpad(int id){
    if( (id >= 0) && (id <= 9) ){
        passwordDisplay->insert(QString::number(id));
    }else if( id == 11){ //Valider
        emit returnPressed();
    }else{
        emit quit();
    }
}

void LoginView::returnPressed(){
    QString str(passwordDisplay->text());
    passwordDisplay->clear();
    emit enteredPassword(str);
}

void LoginView::clicDelete(bool){
    passwordDisplay->backspace();
}
