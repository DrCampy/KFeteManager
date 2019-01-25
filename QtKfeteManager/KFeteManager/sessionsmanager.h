#ifndef SESSIONSMANAGER_H
#define SESSIONSMANAGER_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QPlainTextEdit>
#include <QSqlQueryModel>
#include <QDateTime>

typedef QList<QPair<qreal, QString>> CashMoves;
typedef QMultiMap<QString, QPair<qreal, QString>> AccountMoves;
typedef QMap<QString, uint> SalesList;
typedef QMap<QString, qreal> FunctionsBenefits;

struct Session{
    qlonglong           Id;
    QDateTime           openTime;
    QDateTime           closeTime;
    QStringList         jobists;
    QVariant            openAmount;
    QVariant            closeAmount;
    qreal               cashIncome;
    CashMoves           cashMoves;
    AccountMoves        accountMoves;
    SalesList           normalSales;
    SalesList           reducedSales;
    SalesList           freeSales;
    FunctionsBenefits   functionsBenefits;
    qreal               jobistShare;
    qreal               jobistWage;

    void                clear();
};

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

    Session session;
    //Datas
    QString text;
    qreal countLastSession = -1, minJShare = 0;

signals:
    void finished();

private slots:
    void            writeDetails();
    void            loadDatas();

};



#endif // SESSIONSMANAGER_H
