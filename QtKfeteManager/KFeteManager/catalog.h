#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QtGlobal>
#include <QHash>
#include <QString>
#include <QException>

class Article
{
public:
  explicit Article(qreal price, qreal jobistShare, qreal buyingPrice, qreal reducedPrice, QString name, QString function);

  qreal getPrice() const;
  void setPrice(qreal p);

  qreal getJobistShare() const;
  void setJobistShare(qreal js);

  qreal getBuyingPrice() const;
  void setBuyingPrice(qreal bp);

  qreal getReducedPrice() const;
  void setReducedPrice(qreal rp);

  QString getFunction() const;
  void setFunction(QString f);

  QString getName() const;
  static const size_t MAX_NAME_LENGTH = 30;

  Article &operator=(const Article &a);

private:
  qreal price;
  qreal jobistShare;
  qreal buyingPrice;
  qreal reducedPrice;
  QString name;
  QString function;

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
    bool hasArticle(QString &key) const;

    void addFunction(QString s);
    void deleteFunction(QString s);
    bool hasFunction(QString s) const;
    QStringList getFunctions() const;


private:
    /*Returns false if no error were raised, true otherwise*/
    bool exportCatalog(QString filename)const;
    bool importCatalog(QString filename);
    QHash<QString, Article> *container;
    QStringList *functions;

signals:

public slots:
};

class BadFunctionException : public QException
{
public:
    void raise() const override;
    BadFunctionException *clone() const override;
};


#endif // CATALOG_H
