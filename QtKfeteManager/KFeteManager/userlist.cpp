#include <QString>
#include <QRegularExpression>
#include <QMap>
#include <QtGlobal>

#include "userlist.h"

UserList::UserList(QObject *parent) : QObject(parent)
{
    userList = new QMap<QString, User>();
}

UserList::~UserList(){
    delete userList;
}

void UserList::addUser(User u){
    if(hasUser(u.getName())){
        throw new DuplicateUserException();
    }

    userList->insert(u.getName(), u);
}

void UserList::delUser(QString userName){
    userList->remove(userName);
}

bool UserList::hasUser(QString userName){
    return userList->contains(userName);
}

bool UserList::importList(QString dbName){
    //TODO implement
    return true;
}

bool UserList::exportList(QString dbName){
    return true;
}

User::User(QString name, QString Phone, QString adresse, QString email,
               qreal limite, bool isJobiste/*=false*/) : name(name){

  setAddress(adresse);
  setEmail(email);
  setJobist(isJobiste);
  setLimit(limite);
  setPhone(Phone);
}

QString User::getName() const{
    return this->name;
}

void User::setPhone(QString tel){
  tel.remove(QRegularExpression("^(0-9./)"));
  this->Phone = tel;
}

QString User::getPhone() const{
  return this->Phone;
}

void User::setAddress(QString adr){
  adr.remove(QRegularExpression("^(a-zA-Z0-9,.)"));
  this->address = adr;
}

QString User::getAddress() const{
  return this->address;
}

void User::setEmail(QString email){
  //email.remove(QRegularExpression("^(a-zA-Z0-9.@-_)"));
  this->email = email;
}

QString User::getEmail() const{
  return this->email;
}

void User::setJobist(bool isJobist){
  this->bIsJobist = isJobist;
}

bool User::isJobist() const{
  return this->bIsJobist;
}

void User::setLimit(qreal limit){
  if(!isJobist() || limit > 0){
      this->limit = 0;
    }else if(limit < LIMIT_MIN){
      this->limit = LIMIT_MIN;
    }else{
      this->limit = limit;
    }
}

qreal User::getLimit() const{
  return this->limit;
}

User::ResultatOperation User::deposit(qreal montant, PriorityLevel level/* = Normal*/){
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
}

qreal User::getBalance() const{
  return this->balance;
}

User &User::operator=(const User &u){
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

DuplicateUserException *DuplicateUserException::clone() const{
    return new DuplicateUserException(*this);
}

void DuplicateUserException::raise() const{
    throw *this;
}
