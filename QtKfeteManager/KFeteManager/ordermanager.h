#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <QWidget>
#include <QTableView>
#include <QSqlQueryModel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStyledItemDelegate>

class OrderManager : public QWidget
{
    Q_OBJECT
public:
    explicit OrderManager(QWidget *parent = nullptr);
    void refreshList();
    void showEvent(QShowEvent *event);
private:
    QPushButton *deleteButton;
    QPushButton *back;
    QPlainTextEdit *textEdit;
    QSqlQueryModel *orderModel;
    QTableView *orderView;

signals:
    void finished();

public slots:
    void writeOrderDetails();
    void deleteOrder();
};

class DateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateDelegate(QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const;
};

#endif // ORDERMANAGER_H
