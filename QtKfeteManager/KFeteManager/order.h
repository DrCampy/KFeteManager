#ifndef ORDER_H
#define ORDER_H

#include <QMap>
#include "catalog.h"
#include "clientlist.h"
#include "databasemanager.h"

class Order
{
public:
    enum Price{normal, reduced, free};
    Order(Client *client = nullptr);
    ~Order();
    const QMap<Article, unsigned int> *getContent() const;
    Client *getClient() const;
    void addArticle(Article &a);
    void removeArticle(Article &a);
    void deleteArticle(Article &a);
    qreal getTotal() const;
    void setPrice(Price price);
    Order &operator=(const Order &o);

private:
    void setClient(Client *client);
    QMap<Article, unsigned int> *content;
    Client *client = nullptr;
    qreal total = 0;
    Price price = normal;
};

#endif // ORDER_H
