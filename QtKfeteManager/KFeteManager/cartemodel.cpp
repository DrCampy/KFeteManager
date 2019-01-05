#include <QString>
#include <QPalette>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QSaveFile>
#include <QFile>
#include <QErrorMessage>
#include <QMessageBox>
#include <QDebug>
#include <QDir>

#include "catalog.h"
#include "cartemodel.h"

CarteModel::CarteModel(QString filename, QObject *parent) : QObject(parent), filename(filename)
{
    this->articles = new QStringList();
    this->importCarte();
}

CarteModel::~CarteModel(){
    delete articles;
}

const ButtonDataWrapper *CarteModel::getButton(unsigned int id) const{
    auto res = table.find(id);
    if(res == table.end()){
        return nullptr;
    }else{
        return new ButtonDataWrapper(res.value());
    }
}

bool CarteModel::addEntry(unsigned int id, ButtonDataWrapper data){
    Article a(data.getName());
    if(a.exists()){
        //If a is a real article, sets the button and updates the list
        unsigned int entriesCount = nbEntries.value(data.getName(), 0);
        nbEntries.insert(data.getName(), entriesCount+1);
        table.insert(id, data);
        articles->append(data.getName());
        articles->removeDuplicates();
    }else{
        //If we add a non-existing entry, clears that button
        //Decrease by one the count of the entry that previously was at that button
        if(table.contains(id)){
            ButtonDataWrapper prevEntry = table.value(id);
            table.remove(id);
            unsigned int entriesCount = nbEntries.value(prevEntry.getName());
            if(entriesCount > 1){
                nbEntries.insert(prevEntry.getName(), entriesCount-1);
            }else{
                nbEntries.remove(prevEntry.getName());
            }
        }
    }
    emit modelUpdated();
    return exportCarte();
}

void CarteModel::buttonClicked(int id){
    if(table.contains(static_cast<unsigned int>(id))){
        emit articleClicked(table.find(static_cast<unsigned int>(id)).value().getName());
    }
}

bool CarteModel::exportCarte() const{
    QSaveFile *file = new QSaveFile(filename);
    file->open(QIODevice::WriteOnly | QIODevice::Text);

    QXmlStreamWriter xml(file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("carte");
    xml.writeAttribute("version", QString::number(CARTE_VERSION));
    for(auto it = table.begin(); it != table.end(); it++){
        xml.writeEmptyElement("boutton");
        xml.writeAttribute("button-id", QString::number(it.key()) );
        xml.writeAttribute("article-name", it.value().getName());
        xml.writeAttribute("background-color", it.value().getBackgroundColor().name());
        xml.writeAttribute("text-color", it.value().getTextColor().name());
    }
    xml.writeEndElement();//carte
    xml.writeEndDocument();

    file->commit();
    delete(file);
    return xml.hasError();
}

/*
 * Imports the content of the carte from the XML file "filename"
 *
 * Returns true if it went successful
 * Returns false in case of error
 *
*/
bool CarteModel::importCarte(){
  QFile *file = new QFile(filename);

  if(!file->exists()){
      QMessageBox ask;
      ask.setText(tr("Impossible d'ouvrir le fichier de carte"));
      ask.setInformativeText(tr("Le fichier n'existe pas. Le créer ?"));
      ask.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      ask.setDefaultButton(QMessageBox::No);
      int ret = ask.exec();
      if(ret == QMessageBox::Yes){
          this->exportCarte();
          return false;
      }
  }
  file->open(QIODevice::ReadOnly | QIODevice::Text);
  QXmlStreamReader xml(file);
  unsigned int buttonID;
  QString articleName;
  QColor backgroundColor;
  QColor textColor;

  if(xml.readNextStartElement() && xml.name() == "carte"){
      if(xml.attributes().hasAttribute("version")){
          if(xml.attributes().value("version").toUInt()>0){
              QErrorMessage error;
              QString errMessage = tr("Error: the carte file ");
              errMessage.append(filename);
              errMessage.append(tr(" has incorrect version number. \n Version is: \""));
              errMessage.append(xml.attributes().value("version"));
              errMessage.append(tr("\" but expected \""));
              errMessage.append(QString::number(CARTE_VERSION));
              errMessage.append("\".");
              error.showMessage(errMessage);
              error.exec();
              QString newfile = filename;
              newfile.append(".bad_version");
              file->rename(newfile);
              exportCarte();
              return importCarte();
          }
      }
      while (!xml.atEnd()){ //Tant qu'on est pas à la fin du fichier...
          while(xml.readNextStartElement()){//On lit le prochain tag
              if(xml.name() == "boutton"){ // Si le tag est "boutton", commence à importer un boutton
                  if(xml.attributes().hasAttribute("button-id")){//Vérifie si l'article a un nom
                      buttonID = xml.attributes().value("button-id").toUInt();
                      if(xml.attributes().hasAttribute("article-name")){
                          articleName = xml.attributes().value("article-name").toString();
                          if(xml.attributes().hasAttribute("background-color")){
                              backgroundColor = QColor(xml.attributes().value("background-color").toString());
                              if(xml.attributes().hasAttribute("text-color")){
                                  textColor = QColor(xml.attributes().value("text-color").toString());
                                  if(Article(articleName).exists()){
                                      table.insert(buttonID, ButtonDataWrapper(articleName, backgroundColor, textColor));
                                      unsigned int entriesCount = nbEntries.value(articleName, 0);
                                      nbEntries.insert(articleName, entriesCount+1);
                                  }
                              }
                          }
                      }
                  }
              }
          }
      }
  }
  //TODO check that ID is not out of bounds
  file->close();
  delete file;
  emit modelUpdated();
  return xml.hasError();
}//end importCatalog

QStringList *CarteModel::getArticlesList(){
    return new QStringList(this->nbEntries.keys());
}

ButtonDataWrapper::ButtonDataWrapper(QString name, QColor backgroundColor, QColor textColor) :
    name(name), backgroundColor(backgroundColor), textColor(textColor){/*Empty*/};

QString ButtonDataWrapper::getName() const{
    return name;
}

QColor ButtonDataWrapper::getTextColor() const{
    return textColor;
}

QColor ButtonDataWrapper::getBackgroundColor() const{
    return backgroundColor;
}

ButtonDataWrapper &ButtonDataWrapper::operator=(const ButtonDataWrapper &a){
    if(&a == this){
        return *this;
    }
    this->name = a.getName();
    this->backgroundColor = a.getBackgroundColor();
    this->textColor = a.getTextColor();
    return *this;
}

bool ButtonDataWrapper::operator==(const ButtonDataWrapper &a) const{
    if(getName() == a.getName() && getTextColor() == a.getTextColor() && getBackgroundColor() == a.getBackgroundColor())
        return true;
    return false;
}
