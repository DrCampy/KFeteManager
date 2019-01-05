#ifndef CATALOG_H
#define CATALOG_H

#include <QObject>
#include <QtGlobal>
#include <QHash>
#include <QString>
#include <QException>
#include <QVariant>

class Article
{
public:
    explicit Article(QString name);

    void create(qreal price = 0, qreal jobistShare = 0, qreal buyingPrice = 0, qreal reducedPrice = 0, QString function = QString());
    bool exists() const;

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
    bool operator==(const Article &a) const;
    bool operator<(const Article &a) const;
    operator QString() const{return this->getName();}

private:
    QString name;

};

#endif // CATALOG_H
