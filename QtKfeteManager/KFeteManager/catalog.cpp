#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <iterator>
#include <QFile>
#include <QHash>
#include <fstream>
#include <cmath>
#include <QString>

#include "catalog.h"


Article::Article(qreal price, qreal jobistShare, qreal buyingPrice, qreal reducedPrice, QString name) : name(name){
  setPrice(price);
  setJobistShare(jobistShare);
  setBuyingPrice(buyingPrice);
  setReducedPrice(reducedPrice);
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

Article &Article::operator=(const Article &a){
    if(&a == this){
        return *this;
    }
    this->name = a.getName();
    this->setPrice(a.getPrice());
    this->setJobistShare(a.getJobistShare());
    this->setBuyingPrice(a.getBuyingPrice());
    this->setReducedPrice(a.getReducedPrice());
    return *this;
}


/*
 *
 * CLASS CATALOG
 *
*/
Catalog::Catalog(QObject *parent):QObject(parent){
    container = new QHash<QString, Article>();
}
Catalog::~Catalog(){
  delete container;
}

void Catalog::addArticle(Article &a){
    QString name = a.getName();
    name.truncate(Article::MAX_NAME_LENGTH);
    container->insert(name, a);
}

void Catalog::deleteArticle(QString &key){
  container->remove(key);
}

bool Catalog::contains(QString &key) const{
  return container->contains(key);
}

Article Catalog::getArticle(QString &key) const{
  auto it = container->find(key);
  return it.value();
}

bool Catalog::exportCatalog(QString filename) const{
  QString *str = new QString;
  QXmlStreamWriter stream(str);
  stream.setAutoFormatting(true);
  stream.writeStartDocument();
  stream.writeStartElement("articles-list");
  for(auto i = container->begin(); i != container->end(); i++){
      stream.writeStartElement("article");
      stream.writeAttribute("nom", i.value().getName());
      stream.writeTextElement("prix", QString::number(i.value().getPrice()));
      stream.writeTextElement("part-jobistes", QString::number(i.value().getJobistShare()));
      stream.writeTextElement("prix-achat", QString::number(i.value().getBuyingPrice()));
      stream.writeTextElement("prix-reduit", QString::number(i.value().getReducedPrice()));
      stream.writeEndElement();//article
  }
  stream.writeEndElement();//articles-list
  stream.writeEndDocument();

  std::ofstream outfile;
  outfile.open(filename.toStdString().c_str());
  outfile << str->toStdString();
  outfile.close();
  delete str;
  return !stream.hasError();
}

//Return true if success
bool Catalog::importCatalog(QString filename){
  QFile file(filename);
  if(!file.exists()){
      return true;
  }
  std::ifstream infile;
  infile.open(filename.toStdString().c_str());
  std::string stds((std::istreambuf_iterator<char>(infile)),
                   std::istreambuf_iterator<char>());

  infile.close();
  QString s(stds.c_str());
  QXmlStreamReader xml(s);
  QString nom;
  bool prixSet = false, partJobistSet = false, prixAchatSet = false, prixReduitSet = false;
  qreal prix = 0, partJobist = 0, prixAchat = 0, prixReduit = 0;
  Article *a;

  if(xml.readNextStartElement() && xml.name() == "articles-list"){
      while (!xml.atEnd()){ //Tant qu'on est pas à la fin du fichier...
          while(xml.readNextStartElement()){//On lit le prochain tag
              if(xml.name() == "article"){ // Si le tag est "Article", commence à importer un article
                  if(xml.attributes().hasAttribute("nom")){//Vérifie si l'article a un nom
                      nom = xml.attributes().value("nom").toString();
                      while (xml.readNextStartElement() && !(prixSet && partJobistSet && prixAchatSet && prixReduitSet)) {
                          if( (xml.name() == "prix") && !prixSet){
                              prix = xml.readElementText().toDouble();
                              prixSet = true;
                          }else if( (xml.name() == "part-jobistes") && !partJobistSet){
                              partJobist = xml.readElementText().toDouble();
                              partJobistSet = true;
                          }else if( (xml.name() == "prix-achat") && !prixAchatSet){
                              prixAchat = xml.readElementText().toDouble();
                              prixAchatSet = true;
                          }else if( (xml.name() == "prix-reduit") && !prixReduitSet){
                              prixReduit = xml.readElementText().toDouble();
                              prixReduitSet = true;
                          }else{
                              xml.raiseError("Element inconnu dans le fichier xml");
                          }

                      }//end while readNextStartElement
                      if(prixSet && partJobistSet && prixAchatSet){ //if every elt was init, insert new article
                          a = new Article(prix, partJobist, prixAchat, prixReduit, nom);
                          this->addArticle(*a);
                          delete a;
                          prixSet = false;
                          partJobistSet =false;
                          prixAchatSet = false;
                          prixReduitSet = false;
                      }//end if sets
                  }//end if has attribute nom
              }//end if name == article
          }//end while readNextElement
      }//end while !hasEnd
  }//end if name == articles-list
  return !xml.hasError();
}//end importCatalog
