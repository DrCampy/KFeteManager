#include <QString>
#include <QRegularExpression>
#include <QMap>
#include <QtGlobal>

#include "databasemanager.h"
#include "clientlist.h"

Client::Client(QString name) : name(name){}

void Client::create(QString phone, QString address, QString email, qreal limit, bool isJobist, qreal balance){

    if(!isJobist && limit < 0){
        limit = 0;
    }else if(limit < LIMIT_MIN){
        limit = LIMIT_MIN;
    }

    DatabaseManager::addClient(*this, phone, address, email, limit, isJobist, balance);
}

QString Client::getName() const{
    return this->name;
}

void Client::setPhone(QString tel){
  tel.remove(QRegularExpression("^(0-9./)"));
  DatabaseManager::updateClientPhone(*this, tel);
}

QString Client::getPhone() const{
  return DatabaseManager::getClientPhone(*this);
}

void Client::setAddress(QString adr){
  adr.remove(QRegularExpression("^(a-zA-Z0-9,.)"));
  DatabaseManager::updateClientAddress(*this, adr);
}

QString Client::getAddress() const{
  return DatabaseManager::getClientAddress(*this);
}

void Client::setEmail(QString email){
  DatabaseManager::updateClientEmail(*this, email);
}

QString Client::getEmail() const{
  return DatabaseManager::getClientEmail(*this);
}

void Client::setJobist(bool isJobist){
  DatabaseManager::updateIsClientJobist(*this, isJobist);
}

bool Client::isJobist() const{
  return DatabaseManager::isClientJobist(*this);
}

void Client::setLimit(qreal limit){
  if(!isJobist() || limit > 0){
     limit = 0;
  }else if(limit < LIMIT_MIN){
     limit = LIMIT_MIN;
  }
  DatabaseManager::updateClientLimit(*this, limit);
}

bool Client::exists() const{
    return DatabaseManager::hasClient(*this);
}

qreal Client::getLimit() const{
  return DatabaseManager::getClientLimit(*this);
}

/*Client::ResultatOperation Client::deposit(qreal montant, PriorityLevel level){
  qreal tmp = getBalance()-montant;
  if(tmp >= 0 || level == Admin){
      balance -= montant;
    }else if( (tmp >= this->getLimit()) && (tmp < 0) && level == Limit ){
      balance -= montant;
    }else if( tmp >= 0){
      balance -= montant;
    }else if( (tmp >= this->getLimit()) && (tmp < 0) && level == Normal){
      return Negatif;
    }else{
      return Echec;
    }
  return Reussi;
}*/

qreal Client::getBalance() const{
  return DatabaseManager::getClientBalance(*this);
}

Client &Client::operator=(const Client &u){
    if(this == &u){
        return *this;
    }
    this->name = u.getName();
    return *this;
}

bool Client::operator==(const Client &c) const{
    return(c.getName() == this->getName());
}

bool Client::operator<(const Client &c) const{
    return this->getName() < c.getName();
}

DuplicateClientException *DuplicateClientException::clone() const{
    return new DuplicateClientException(*this);
}

void DuplicateClientException::raise() const{
    throw *this;
}

NotJobistException *NotJobistException::clone() const{
    return new NotJobistException(*this);
}

void NotJobistException::raise() const{
    throw *this;
}
