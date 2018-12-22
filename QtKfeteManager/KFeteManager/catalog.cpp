#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <iterator>
#include <QFile>
#include <QHash>
#include <fstream>
#include <cmath>
#include <QString>

#include "catalog.h"


Article::Article(qreal price, qreal partJobiste, qreal prixAchat, qreal prixReduit, QString nom) : nom(nom){
  setPrice(price);
  setPartJobiste(partJobiste);
  setPrixAchat(prixAchat);
  setPrixReduit(prixReduit);
}

qreal Article::getPrice() const{
  return this->price;
}

void Article::setPrice(qreal p){
  this->price = qRound(p*100)/100;
}

qreal Article::getPartJobiste() const{
  return this->partJobiste;
}

void Article::setPartJobiste(qreal pj){
  this->partJobiste = qRound(pj*100)/100;
}

qreal Article::getPrixAchat() const{
  return this->prixAchat;
}

void Article::setPrixAchat(qreal pa){
  this->prixAchat = qRound(pa*100)/100;
}

qreal Article::getPrixReduit() const{
  return this->prixReduit;
}

void Article::setPrixReduit(qreal pr){
  this->prixReduit = qRound(pr*100)/100;
}

QString Article::getNom() const{
  return this->nom;
}


/*
 *
 * CLASS CATALOG
 *
*/

Catalog::~Catalog(){
  for(container_t::iterator it = container.begin(); it != container.end(); it++){
      delete it.value();
  }
}

void Catalog::addArticle(Article &a){
  QString str = a.getNom();
  str.resize(std::min(str.size(), static_cast<int>(Article::MAX_NAME_LENGTH)));
  Article *a2 = new Article(a.getPrice(), a.getPartJobiste(), a.getPrixAchat(), a.getPrixReduit(), str);
  container.insert(a2->getNom(), a2);
}

void Catalog::deleteArticle(QString &key){
  delete container.find(key).value();
  container.remove(key);
}

bool Catalog::contains(QString &key) const{
  return container.contains(key);
}

Article Catalog::getArticle(QString &key) const{
  container_t::const_iterator it = container.find(key);
  return **it;
}

bool Catalog::exportCatalog(QString filename) const{
  QString *str = new QString;
  QXmlStreamWriter stream(str);
  stream.setAutoFormatting(true);
  stream.writeStartDocument();
  stream.writeStartElement("articles-list");
  for(auto i = container.begin(); i != container.end(); i++){
      stream.writeStartElement("article");
      stream.writeAttribute("nom", i.value()->getNom());
      stream.writeTextElement("prix", QString::number(i.value()->getPrice()));
      stream.writeTextElement("part-jobistes", QString::number(i.value()->getPartJobiste()));
      stream.writeTextElement("prix-achat", QString::number(i.value()->getPrixAchat()));
      stream.writeTextElement("prix-reduit", QString::number(i.value()->getPrixReduit()));
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
