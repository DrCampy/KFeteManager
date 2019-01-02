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
    //Form widgets
    QDoubleSpinBox *price, *bPrice, *jShare, *rPrice;
    QComboBox *function;
    QDataWidgetMapper *mapper;
    QPushButton *validate;
    QSqlRelationalTableModel *sqlModel;

    int nameIndex, priceIndex, jobShareIndex, bPriceIndex, redPriceIndex,
    functionIndex, functionNameIndex;

signals:
    void validateEntry();

public slots:
    void entryNotModified();
    void entryModified();
    void entryValidated();
    void addFunction();
};

#endif // CATALOGMANAGER_H
