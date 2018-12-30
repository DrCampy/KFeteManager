#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QPushButton>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>

class CatalogManager : public QWidget
{
    Q_OBJECT
public:
    explicit CatalogManager(QWidget *parent = nullptr);

private:
    //Form widgets
    QLineEdit *price, *bPrice, *jShare, *rPrice;
    QComboBox *function;
    QDataWidgetMapper *mapper;
    bool isEntryModified = false;
    QPushButton *validate;
    QSqlRelationalTableModel *sqlModel;
signals:
    void validateEntry();
public slots:
    void entryModified();
    void entryValidated();
};

#endif // CATALOGMANAGER_H
