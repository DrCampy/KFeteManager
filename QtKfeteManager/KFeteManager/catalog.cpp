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
#include "databasemanager.h"

Article::Article(QString name) : name(name){
}

void Article::create(qreal price, qreal jobistShare, qreal buyingPrice, qreal reducedPrice, QString function){
    DatabaseManager::addArticle(*this, price, jobistShare, buyingPrice, reducedPrice, function);
}

bool Article::exists() const{
    return DatabaseManager::hasArticle(*this);
}

qreal Article::getPrice() const{
    return DatabaseManager::getArticlePrice(*this);
}

void Article::setPrice(qreal p){
    DatabaseManager::updateArticlePrice(*this, p);
}

qreal Article::getJobistShare() const{
    return DatabaseManager::getArticleJobistShare(*this);
}

void Article::setJobistShare(qreal js){
    DatabaseManager::updateArticleJobistShare(*this, js);
}

qreal Article::getBuyingPrice() const{
    return DatabaseManager::getArticlePrice(*this);
}

void Article::setBuyingPrice(qreal bp){
    DatabaseManager::updateArticleBuyingPrice(*this, bp);
}

qreal Article::getReducedPrice() const{
    return DatabaseManager::getArticleReducedPrice(*this);
}

void Article::setReducedPrice(qreal rp){
    DatabaseManager::updateArticleReducedPrice(*this, rp);
}

QString Article::getName() const{
    return this->name;
}

void Article::setFunction(QString f){
    DatabaseManager::updateArticleFunction(*this, f);
}

QString Article::getFunction() const{
    return DatabaseManager::getArticleFunction(*this);
}

Article &Article::operator=(const Article &a){
    if(&a == this){
        return *this;
    }
    this->name = a.getName();
    return *this;
}

bool Article::operator==(const Article &a) const{
    return(a.getName() == this->getName());
}

bool Article::operator<(const Article &a) const{
    return(this->getName() < a.getName());
}
