#ifndef ORDER_H
#define ORDER_H

#include <QMap>
#include <QPair>
#include "catalog.h"
#include "clientlist.h"
#include "databasemanager.h"

class Order
{
public:
    typedef QMap<Article, QPair<uint, qreal>> content_t;
    enum Price{normal, reduced, free};
    Order(Client *client = nullptr);
    Order(const Order &o);
    ~Order();
    const content_t *getContent() const;
    Client *getClient() const;
    void addArticle(Article &a);
    void removeArticle(Article &a);
    void deleteArticle(Article &a);
    qreal getTotal() const;
    void setPrice(Price price);
    Price getPrice();
    Order &operator=(const Order &o);
//TODO
    //bool process(); //to process the order on the client account
private:
    void setClient(Client *client);
    content_t *content;
    Client *client = nullptr;
    qreal total = 0;
    Price price = normal;
};

#endif // ORDER_H
