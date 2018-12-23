#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QtGlobal>
#include <QHash>
#include <QString>

class Article
{
public:
  explicit Article(qreal price, qreal jobistShare, qreal buyingPrice, qreal reducedPrice, QString name);

  qreal getPrice() const;
  void setPrice(qreal p);

  qreal getJobistShare() const;
  void setJobistShare(qreal js);

  qreal getBuyingPrice() const;
  void setBuyingPrice(qreal bp);

  qreal getReducedPrice() const;
  void setReducedPrice(qreal rp);

  QString getName() const;
  static const size_t MAX_NAME_LENGTH = 30;

  Article &operator=(const Article &a);

private:
  qreal price;
  qreal jobistShare;
  qreal buyingPrice;
  qreal reducedPrice;
  QString name;

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
    QHash<QString, Article> *container;

signals:

public slots:
};

#endif // CATALOG_H
