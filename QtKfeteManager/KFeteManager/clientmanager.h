#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QListView>

class ClientManager : public QWidget
{
    Q_OBJECT
public:
    explicit ClientManager(QWidget *parent = nullptr);

private:
    QSqlRelationalTableModel *clientModel;
    QListView *clientList;

signals:
    void finished();
public slots:

private slots:
    void createClient();
};

#endif // CLIENTMANAGER_H
