#ifndef USERLIST_H
#define USERLIST_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QException>
#include <QtGlobal>

class User;
class UserList;
class DuplicateUserException;

class UserList : public QObject
{
    Q_OBJECT
public:
    explicit UserList(QObject *parent = nullptr);
    ~UserList();
    void addUser(User u);
    void delUser(QString userName);
    bool hasUser(QString userName);
    User getUser(QString userName);

private:
    QMap<QString, User> *userList;
    bool exportList(QString dbName);
    bool importList(QString dbName);


signals:
    void listUpdated();

public slots:

};

class User
{
public:

  enum ResultatOperation {Echec, Negatif, Reussi};

  /*
   *Niveaux de priorité pour les opérations de retrait :
   * Admin : Ajuste toujours la bamance du montant indiqué, même si on descend sous la limit
   * Negatif : Permet de descendre en négatif, jusqu'à la limit
   * Normal : Permet de descendre jusqu'à 0
   */
  enum PriorityLevel {Admin, Limit, Normal};

  User(QString name, QString Phone = "", QString address = "", QString email = "", qreal limit = 0, bool isJobist = false);

  QString getName() const;

  void setPhone(QString tel);
  QString getPhone() const;

  void setAddress(QString adr);
  QString getAddress() const;

  void setEmail(QString email);
  QString getEmail() const;

  void setJobist(bool isJobist);
  bool isJobist() const;

  void setLimit(qreal limit);
  qreal getLimit() const;

  ResultatOperation deposit(qreal montant, PriorityLevel level = Normal);
  qreal getBalance() const;

  User &operator=(const User &u1);


private:
  QString name;
  QString Phone;
  QString address;
  QString email;

  bool bIsJobist;
  qreal balance = 0.0, limit = 0.0;
  static constexpr qreal LIMIT_MIN = -10;

};

class DuplicateUserException : public QException
{
public:
    void raise() const override;
    DuplicateUserException *clone() const override;
};

#endif // USERLIST_H
