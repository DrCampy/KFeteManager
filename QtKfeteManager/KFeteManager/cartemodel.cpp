#include <QString>
#include <QPalette>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QSaveFile>
#include <QFile>

#include "cartemodel.h"

CarteModel::CarteModel(QObject *parent) : QObject(parent)
{
    //load carte from file carte.xml
}


ButtonDataWrapper::ButtonDataWrapper(QString name, QColor backgroundColor, QColor textColor) :
    name(name), backgroundColor(backgroundColor), textColor(textColor)
{
//Empty
};

QString ButtonDataWrapper::getName() const{
    return name;
}

QColor ButtonDataWrapper::getTextColor() const{
    return textColor;
}

QColor ButtonDataWrapper::getBackgroundColor() const{
    return backgroundColor;
}


bool CarteModel::exportCarte(QString filename) const{
    QSaveFile *file = new QSaveFile(filename);
    file->open(QIODevice::WriteOnly | QIODevice::Text);

    QXmlStreamWriter xml(file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("carte");
    for(auto it = table.begin(); it != table.end(); it++){
        xml.writeEmptyElement("boutton");
        xml.writeAttribute("button-id", QString::number(it.key()) );
        xml.writeAttribute("article-name", it.value().getName());
        xml.writeAttribute("background-color", it.value().getBackgroundColor().name());
        xml.writeAttribute("text-color", it.value().getTextColor().name());
        xml.writeEndElement();//boutton
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
bool CarteModel::importCarte(QString filename){
  QFile *file = new QFile(filename);
  if(!file->exists()){
      return false;
  }

  file->open(QIODevice::ReadOnly);

  QXmlStreamReader xml(file);
  unsigned int buttonID;
  QString articleName;
  QColor backgroundColor;
  QColor textColor;

  if(xml.readNextStartElement() && xml.name() == "carte"){
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
                                  if(catalog->contains(articleName)){
                                      table.insert(buttonID, ButtonDataWrapper(articleName, backgroundColor, textColor));
                                  }
                              }
                          }
                      }
                  }
              }
          }
      }
  }
  file->close();
  emit updateCarte();
  return xml.hasError();
}//end importCatalog
