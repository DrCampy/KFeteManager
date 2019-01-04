#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QPushButton>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QDoubleSpinBox>
class CatalogManager : public QWidget
{
    Q_OBJECT
public:
    explicit CatalogManager(QWidget *parent = nullptr);

private:
    QSqlRelationalTableModel *sqlModel;
    int nameIndex, priceIndex, jobShareIndex, bPriceIndex, redPriceIndex,
    functionIndex, functionNameIndex;

    //Form widgets
    QDoubleSpinBox      *price, *bPrice, *jShare, *rPrice;
    QComboBox           *function;
    QDataWidgetMapper   *mapper;
    QPushButton         *validate;

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
    void entryNotModified();
    void entryModified();
    void entryValidated();
    void addFunction();
    void delFunction();
    void selectedFctChanged(int i);

};

#endif // CATALOGMANAGER_H
