#include <QString>
#include <QRegularExpression>
#include <QMap>
#include <QtGlobal>

#include "databasemanager.h"
#include "clientlist.h"

Client::Client(QString name) : name(name){}

Client::Client(const Client &c){
    this->name = c.getName();
}

void Client::create(QString phone, QString address, QString email, qreal limit, bool isJobist, qreal balance){
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
  DatabaseManager::updateClientLimit(*this, limit);
}

bool Client::exists() const{
    if(isNull()){
        return false;
    }
    return DatabaseManager::hasClient(*this);
}

qreal Client::getLimit() const{
  return DatabaseManager::getClientLimit(*this);
}

bool Client::deposit(qreal amount){
    return DatabaseManager::clientDeposit(amount, *this);
}

qreal Client::getBalance() const{
  return DatabaseManager::getClientBalance(*this);
}

bool Client::isNull() const{
    return this->getName() == "";
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
