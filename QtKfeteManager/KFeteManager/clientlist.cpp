#include <QString>
#include <QRegularExpression>
#include <QMap>
#include <QtGlobal>

#include "clientlist.h"
/*
ClientList::ClientList(QObject *parent) : QObject(parent)
{
    clientList = new QMap<QString, Client>();
}

ClientList::~ClientList(){
    delete clientList;
}

void ClientList::addClient(Client u){
    if(hasClient(u.getName())){
        throw new DuplicateClientException();
    }

    clientList->insert(u.getName(), u);
}

void ClientList::delClient(QString clientName){
    clientList->remove(clientName);
}

bool ClientList::hasClient(QString clientName){
    return clientList->contains(clientName);
}

bool ClientList::importList(){
    //TODO implement
    return true;
}

bool ClientList::exportList(){
    return true;
}*/

Client::Client(QString name, QString Phone, QString adresse, QString email,
               qreal limite, bool isJobiste) : name(name){

  setAddress(adresse);
  setEmail(email);
  setJobist(isJobiste);
  setLimit(limite);
  setPhone(Phone);
}

/*Client::Client(Client &c){
    *this = c;
}*/

QString Client::getName() const{
    return this->name;
}

void Client::setPhone(QString tel){
  tel.remove(QRegularExpression("^(0-9./)"));
  //this->Phone = tel;
}

QString Client::getPhone() const{
  //return this->Phone;
}

void Client::setAddress(QString adr){
  adr.remove(QRegularExpression("^(a-zA-Z0-9,.)"));
  //this->address = adr;
}

QString Client::getAddress() const{
  //return this->address;
}

void Client::setEmail(QString email){
  //this->email = email;
}

QString Client::getEmail() const{
  //return this->email;
}

void Client::setJobist(bool isJobist){
  //this->bIsJobist = isJobist;
}

bool Client::isJobist() const{
  //return this->bIsJobist;
}

void Client::setLimit(qreal limit){
  /*if(!isJobist() || limit > 0){
      this->limit = 0;
    }else if(limit < LIMIT_MIN){
      this->limit = LIMIT_MIN;
    }else{
      this->limit = limit;
    }*/
}

qreal Client::getLimit() const{
  //return this->limit;
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
  //return this->balance;
}

Client &Client::operator=(const Client &u){
    if(this == &u){
        return *this;
    }
    this->name = u.getName();
    this->setPhone(u.getPhone());
    this->setAddress(u.getAddress());
    this->setEmail(u.getEmail());
    this->setLimit(u.getLimit());
    this->setJobist(u.isJobist());
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
