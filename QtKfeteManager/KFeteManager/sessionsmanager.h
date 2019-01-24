#ifndef SESSIONSMANAGER_H
#define SESSIONSMANAGER_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QPlainTextEdit>
#include <QSqlQueryModel>

class SessionsManager : public QWidget
{
    Q_OBJECT
public:
    explicit SessionsManager(QWidget *parent = nullptr);
    ~SessionsManager();

private:
    QComboBox       *sessionSelector;
    QPlainTextEdit  *sessionDetails;
    QPushButton     *backButton;
    QPushButton     *validate1Button;
    QPushButton     *validate2Button;
    QPushButton     *validate3Button;
    QSqlQueryModel  *sessionsModel;
    void            refresh();

signals:
    void finished();

public slots:

private slots:
    void            writeDetails();

};

#endif // SESSIONSMANAGER_H
