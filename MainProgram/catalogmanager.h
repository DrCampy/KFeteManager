#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QPushButton>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QLabel>

#include "customwidgets.h"

class CatalogManager : public QWidget
{
    Q_OBJECT
public:
    explicit CatalogManager(QWidget *parent = nullptr);

private:
    QSqlRelationalTableModel *sqlModel;
    int nameIndex, priceIndex, jobShareIndex, bPriceIndex, redPriceIndex,
    functionIndex, functionNameIndex;
    QListView *articlesView;
    //Form widgets
    CustomDoubleSpinBox *price, *bPrice, *jShare, *rPrice;
    QComboBox           *function;
    QDataWidgetMapper   *mapper;
    QPushButton         *validate, *deleteArticleButton;
    QLabel              *formTitle;

    //Function management widgets
    QPushButton *addFunctionButton;
    QPushButton *delFunctionButton;
    QLineEdit   *newFunctionLineEdit;
    QComboBox   *selectedFctCombo;
    void refreshFctModel();


signals:
    void validateEntry();
    void finished();

public slots:
    void selectionChanged();
    void entryModified();
    void entryValidated();
    void addFunction();
    void delFunction();
    void selectedFctChanged(int i);
    void createArticle();
    void deleteArticle();

};

#endif // CATALOGMANAGER_H
