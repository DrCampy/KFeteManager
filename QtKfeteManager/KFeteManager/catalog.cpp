#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <iterator>
#include <QFile>
#include <QHash>
#include <fstream>
#include <QException>
#include <QString>
#include <QDebug>
#include "catalog.h"


Article::Article(qreal price, qreal jobistShare, qreal buyingPrice, qreal reducedPrice, QString name, QString function) : name(name){
  setPrice(price);
  setJobistShare(jobistShare);
  setBuyingPrice(buyingPrice);
  setReducedPrice(reducedPrice);
  setFunction(function);
}

qreal Article::getPrice() const{
  return this->price;
}

void Article::setPrice(qreal p){
  this->price = static_cast<qreal>(qRound(p*100))/100;
}

qreal Article::getJobistShare() const{
  return this->jobistShare;
}

void Article::setJobistShare(qreal js){
  this->jobistShare = static_cast<qreal>(qRound(js*100))/100;
}

qreal Article::getBuyingPrice() const{
  return this->buyingPrice;
}

void Article::setBuyingPrice(qreal bp){
  this->buyingPrice= static_cast<qreal>(qRound(bp*100))/100;
}

qreal Article::getReducedPrice() const{
  return this->reducedPrice;
}

void Article::setReducedPrice(qreal rp){
  this->reducedPrice = static_cast<qreal>(qRound(rp*100))/100;
}

QString Article::getName() const{
  return this->name;
}

void Article::setFunction(QString f){
    this->function = f.toLower();
}

QString Article::getFunction() const{
    return function;
}

Article &Article::operator=(const Article &a){
    if(&a == this){
        return *this;
    }
    this->name = a.getName();
    this->setPrice(a.getPrice());
    this->setJobistShare(a.getJobistShare());
    this->setBuyingPrice(a.getBuyingPrice());
    this->setReducedPrice(a.getReducedPrice());
    this->setFunction(a.getFunction());
    return *this;
}

bool Article::operator==(const Article &a) const{
    return(a.getName() == this->getName());
}

bool Article::operator<(const Article &a) const{
    return(this->getName() < a.getName());
}

/*
 *
 * CLASS CATALOG
 *
*/
Catalog::Catalog(QObject *parent):QObject(parent){
    container = new QHash<QString, Article>();
    functions = new QStringList();
}
Catalog::~Catalog(){
  delete container;
    delete functions;
}

void Catalog::addArticle(Article &a){
    QString name = a.getName();
    name.truncate(Article::MAX_NAME_LENGTH);
    if(!functions->contains(a.getFunction())){
        qDebug() << "Error: unknown function" << a.getFunction();
        throw new BadFunctionException();
    }
    container->insert(name, a);
}

void Catalog::deleteArticle(QString &key){
  container->remove(key);
}

bool Catalog::hasArticle(QString &key) const{
  return container->contains(key);
}

Article Catalog::getArticle(QString &key) const{
  auto it = container->find(key);
  return it.value();
}

/*
 * Adds a function to be used by articles. Will be stored in all lower case
 */

void Catalog::addFunction(QString s){
    s = s.toLower();
    if(!hasFunction(s)){
        functions->append(s);
    }
}

void Catalog::deleteFunction(QString s){
    s = s.toLower();
    functions->removeAll(s);
}

bool Catalog::hasFunction(QString s) const{
    s = s.toLower();
    return functions->contains(s);
}

QStringList Catalog::getFunctions()const{
    return QStringList(*functions);
}

bool Catalog::exportCatalog(QString filename) const{

}

//Return true if success
bool Catalog::importCatalog(QString filename){

}//end importCatalog


BadFunctionException *BadFunctionException::clone() const{
    return new BadFunctionException(*this);
}

void BadFunctionException::raise() const{
    throw *this;
}
