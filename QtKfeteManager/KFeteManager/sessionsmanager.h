#ifndef SESSIONSMANAGER_H
#define SESSIONSMANAGER_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QPlainTextEdit>
#include <QSqlQueryModel>
#include <QDateTime>
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
    void            clear();

    void            validateAuto();
    void            validateManually();
    void            validateNoPay();
    void            saveData();
    static void     deleteSession(qlonglong id);
    static void     payJobist(QString jobist, qreal wage);

    //Datas
    QString text;
    qlonglong sessionID;
    QDateTime sessionTime, closingTime;
    QStringList jobists;

    //Name - Quantity
    QMap<QString, uint> normalSales, reducedSales, freeSales;

    //Name - amount (- note)
    QMultiMap<QString, QPair<qreal, QString>> clientMoves;
    QMap<QString, qreal> functionsBenefits;

    QList<QPair<qreal, QString>> cashRegisterMoves;
    qreal totalSales = 0, totJShare = 0, countLastSession = -1, minJShare = 0;
    QVariant  countBefore, countAfter;

signals:
    void finished();

public slots:

private slots:
    void            writeDetails();
    void            loadDatas();

};

#endif // SESSIONSMANAGER_H
