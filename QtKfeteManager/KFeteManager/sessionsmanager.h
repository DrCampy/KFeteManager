#ifndef SESSIONSMANAGER_H
#define SESSIONSMANAGER_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSqlQuery>
#include <QPlainTextEdit>
#include <QSqlQueryModel>
#include <QDateTime>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVariant>
#include <QFormLayout>

typedef QList<QPair<qreal, QString>> CashMoves;
typedef QMultiMap<QString, QPair<qreal, QString>> AccountMoves;
typedef QMap<QString, uint> SalesList;
typedef QMap<QString, qreal> FunctionsBenefits;

/**
 * @brief The Session struct contains all the variables needed to represent a Session in the history.
 */
struct Session{
    qlonglong           Id; /**< ID of the represented session.*/
    QDateTime           openTime; /**< Date and time the session was opened.*/
    QDateTime           closeTime; /**< Date and time the session was closed.*/
    QStringList         jobists; /**< List of the jobists holding the session.*/
    QVariant            openAmount; /**< Money in the cash register at the time of opening.*/
    QVariant            closeAmount; /**< Money in the cash register at the time of closing.*/
    qreal               cashIncome; /**< Theoretical cash incomes due to sales.*/
    CashMoves           cashMoves; /**< List of cash moves in the cash register that are not linked to any account. */
    AccountMoves        accountMoves; /**< List of cash moves in the cash register linked to accounts.*/
    SalesList           normalSales; /**< List of articles sold at normal price.*/
    SalesList           reducedSales; /**< List of articles sold at reduced price.*/
    SalesList           freeSales; /**< List of articles sold for free.*/
    FunctionsBenefits   functionsBenefits; /**< List of benefits for each function, per function.*/
    qreal               jobistShare; /**< Actual earnings of the jobists for the session.*/
    qreal               jobistWage; /**< Actual amount paid to the jobists (in total).*/

    void                clear();
};

/**
 * @brief The SessionsManager class contains the view to manage sessions.
 *
 * It allows user to display a session's content and validate it and pay (or not) the jobists.
 * Inherits QWidget.
 */
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

    void            saveData();
    static void     deleteSession(qlonglong id);
    static void     payJobist(QMap<QString, qreal> wages);

    Session session;
    //Datas
    QString text;
    qreal countLastSession = -1, minJShare = 0;

signals:
    void finished();

private slots:
    void            writeDetails();
    void            loadDatas();
    void            validateAuto();
    void            validateManually();
    void            validateNoPay();

};


/**
 * @brief The WageSelector class implements a dialog box allowing the user to choose freely the wage for each jobist of a session.
 */
class WageSelector : protected QDialog
{
    Q_OBJECT
public :
    explicit WageSelector(const Session *session, QWidget *parent = nullptr);
    bool ask(QMap<QString, qreal> *wages);
private :
    QList<QDoubleSpinBox*> spinBoxesList;
    QLabel *total;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QFormLayout *formLayout;

private slots:
    void updateTotal();


};

#endif // SESSIONSMANAGER_H
