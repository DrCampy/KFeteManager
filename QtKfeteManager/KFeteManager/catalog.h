#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QtGlobal>

class Article
{
public:
  explicit Article(qreal price, qreal partJobiste, qreal prixAchat, qreal prixReduit, QString nom);

  qreal getPrice() const;
  void setPrice(qreal p);

  qreal getPartJobiste() const;
  void setPartJobiste(qreal pj);

  qreal getPrixAchat() const;
  void setPrixAchat(qreal pa);

  qreal getPrixReduit() const;
  void setPrixReduit(qreal pr);

  QString getNom() const;
  static const size_t MAX_NAME_LENGTH = 30;

private:
  qreal price;
  qreal partJobiste;
  qreal prixAchat;
  qreal prixReduit;
  const QString nom;

};

class Catalog : public QObject
{
    Q_OBJECT
public:
    explicit Catalog(QObject *parent = nullptr);
    ~Catalog();
    void addArticle(Article &a);
    void deleteArticle(QString &key);
    Article getArticle(QString &key) const;
    bool contains(QString &key) const;

private:
    /*Returns false if no error were raised, true otherwise*/
    bool exportCatalog(QString filename)const;
    bool importCatalog(QString filename);
    typedef QHash<QString, Article*> container_t;
    container_t container;

signals:

public slots:
};

#endif // CATALOG_H
