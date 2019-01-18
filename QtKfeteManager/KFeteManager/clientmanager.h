#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QListView>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QDataWidgetMapper>

#include <customwidgets.h>
class ClientManager : public QWidget
{
    Q_OBJECT
public:
    explicit ClientManager(QWidget *parent = nullptr);

private:
    QSqlRelationalTableModel    *clientModel;
    QListView                   *clientList;
    QPushButton                 *validateButton;
    QLabel                      *clientName;
    QLabel                      *balance;
    QLineEdit                   *address;
    QLineEdit                   *email;
    QLineEdit                   *phone;
    CustomDoubleSpinBox         *limit;
    QComboBox                   *isJobist;
    QDataWidgetMapper           *mapper;


signals:
    void finished();
public slots:

private slots:
    void createClient();
    void deleteClient();
    void selectionChanged();
    void dataChanged();
    void validatePressed();
};

#endif // CLIENTMANAGER_H
