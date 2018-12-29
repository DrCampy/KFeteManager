#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QException>
#include <QtGlobal>

class Client;
class ClientList;
class DuplicateClientException;
/*
class ClientList : public QObject
{
    Q_OBJECT
public:
    explicit ClientList(QObject *parent = nullptr);
    ~ClientList();
    void addClient(Client u);
    void delClient(QString clientName);
    bool hasClient(QString clientName);
    Client getClient(QString clientName);

private:
    QMap<QString, Client> *clientList;
    bool exportList();
    bool importList();


signals:
    void listUpdated();

public slots:

};*/

class Client
{
public:

  //enum ResultatOperation {Echec, Negatif, Reussi};

  /*
   *Niveaux de priorité pour les opérations de retrait :
   * Admin : Ajuste toujours la bamance du montant indiqué, même si on descend sous la limit
   * Negatif : Permet de descendre en négatif, jusqu'à la limit
   * Normal : Permet de descendre jusqu'à 0
   */
  /*enum PriorityLevel {Admin, Limit, Normal};*/

  Client(QString name, QString Phone = "", QString address = "", QString email = "", qreal limit = 0, bool isJobist = false);
  Client(Client &c);

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

  //ResultatOperation deposit(qreal montant, PriorityLevel level = Normal);
  qreal getBalance() const;

  Client &operator=(const Client &u);

  bool operator==(const Client &c) const;
  bool operator<(const Client &c) const;
  operator QString() const{return this->getName();}


private:
  QString name;
  static constexpr qreal LIMIT_MIN = -10;

};

class DuplicateClientException : public QException
{
public:
    void raise() const override;
    DuplicateClientException *clone() const override;
};

class NotJobistException : public QException
{
public:
    void raise() const override;
    NotJobistException *clone() const override;
};

#endif // CLIENTLIST_H
