#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

#include "countmoney.h"
#include "databasemanager.h"
#include "clientlist.h"

CountMoneyBefore::CountMoneyBefore(QWidget *parent) : QWidget(parent)
{

}

MoneyForm::MoneyForm(QWidget *parent) : QWidget (parent)
{
    QLabel *notesTitleLabel = new QLabel(tr("Billets :"), this);
    QLabel *coinsTitleLabel = new QLabel(tr("Pièces :"), this);

    QString currency = DatabaseManager::getCurrency();
    QStringList notes = DatabaseManager::getNotes();
    QStringList coins = DatabaseManager::getCoins();
    notesValues = new QVector<qreal>();
    coinsValues = new QVector<qreal>();

    QGridLayout *gridNotes = new QGridLayout();
    QGridLayout *gridCoins = new QGridLayout();

    //Creates the necessary labels and spinBoxes for the notes
    for(auto it : notes){
        notesLabels->append(new QLabel(currency + it, this));
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setMaximum(INT_MAX);
        spinBox->setMinimum(0);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox->setSingleStep(1);
        notesSpinBoxes->append(spinBox);
        notesValues->append(it.toDouble());
    }

    //Creates the necessary labels and spinBoxes for the coins
    for(auto it : coins){
        coinsLabels->append(new QLabel(currency + it, this));
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setMaximum(INT_MAX);
        spinBox->setMinimum(0);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox->setSingleStep(1);
        coinsSpinBoxes->append(spinBox);
        coinsValues->append(it.toDouble());
    }

    //layout the widgets
    //Notes
    gridNotes->addWidget(notesTitleLabel, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignBottom);
    for(int i = 0; i < notesLabels->size(); i++){
        gridNotes->addWidget(notesLabels->at(i), 1+(i/4), (i%4)*2, 1, 1);
        gridNotes->addWidget(notesSpinBoxes->at(i), 1+(i/4), (i%4)*2+1, 1, 1);
    }
    //Coins
    gridCoins->addWidget(coinsTitleLabel, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignBottom);
    for(int i = 0; i < coinsLabels->size(); i++){
        gridCoins->addWidget(coinsLabels->at(i), 1+(i/4), (i%4)*2, 1, 1);
        gridCoins->addWidget(coinsSpinBoxes->at(i), 1+(i/4), (i%4)*2+1, 1, 1);
    }
}

MoneyForm::~MoneyForm(){
    delete notesLabels;
    delete notesSpinBoxes;
    delete notesValues;

    delete coinsLabels;
    delete coinsSpinBoxes;
    delete coinsValues;
}

qreal MoneyForm::getCount(){
    qreal total = 0;

    //for notes
    for(int i = 0; i < notesValues->size(); i++){
        total += notesSpinBoxes->at(i)->value()*notesValues->at(i);
    }

    //for coins
    for(int i = 0; i < coinsValues->size(); i++){
        total += coinsSpinBoxes->at(i)->value()*coinsValues->at(i);
    }

    return total;
}
