#include <QMap>

#include "order.h"
#include "clientlist.h"

Order::Order(Client *client)
{
    content = new QMap<Article, QPair<uint, qreal>>;
    this->client = client;
}

Order::Order(const Order &o){
    if(o.getClient() == nullptr){
        this->client = nullptr;
    }else{
        this->client = new Client(o.getClient()->getName());
    }

    this->content = new QMap<Article, QPair<uint, qreal>>(*(o.getContent()));

    this->total = o.total;
    this->setPrice(o.getPrice());
}

Order::~Order(){
    delete client;
    delete content;
}

void Order::addArticle(Article &a){
    if(a.exists()){
        unsigned int count = content->value(a).first+1;
        qreal subtotal = 0;
        switch(price){
        case normal:
            subtotal = a.getPrice();
            break;
        case reduced:
            subtotal = a.getReducedPrice();
            break;
        case free:
            break;
        }
        content->insert(a, QPair<uint, qreal>(count, subtotal*count));
        total += subtotal;
    }else{
        //In case we already had some of these articles but it suddenly was deleted from database
        deleteArticle(a);
    }
}

const Order::content_t *Order::getContent() const{
    return content;
}

Client *Order::getClient() const{
    return client;
}

void Order::setPrice(Order::Price price){
    this->price = price;
    total = 0;
    for(auto it : content->keys()){
        qreal subtotal = 0;
        switch(price){
        case normal:
            subtotal = it.getPrice() * content->value(it).first;
            break;
        case reduced:
            subtotal = it.getReducedPrice() * content->value(it).first;
            break;
        case free:
            //We never get here.
            break;
        }
        content->insert(it, QPair<uint, qreal>(content->value(it).first, subtotal));
        total += subtotal;
    }
}

Order::Price Order::getPrice() const{
    return this->price;
}

void Order::removeArticle(Article &a){
    if(!a.exists()){
        //In case article was deleted from db
        deleteArticle(a);
    }
    if(content->contains(a)){
        //Gets the current count for that article, minus 1.
        unsigned int count = content->value(a).first-1;

        //If we do not have this article anymore, removes it
        if(count == 0){
            //Adapts the total
            total -= content->value(a).second;
            content->remove(a);
            return;
        }

        //computes the new subtotal
        qreal subtotal = 0;
        switch(price){
        case normal:
            subtotal = a.getPrice();
            break;
        case reduced:
            subtotal = a.getReducedPrice();
            break;
        case free:
            break;
        }

        //Adapts the total
        total -= subtotal;
        //updates the new subtotal
        content->insert(a, QPair<uint, qreal>(count, subtotal*count));
        if(total < 1e-3 && total > -1e-3){
            total = 0;
        }
    }
}

void Order::deleteArticle(Article &a){
    //if we actually have that article in the order
    if(content->contains(a)){
        //adapts th total for that article
        total -= content->value(a).second;
        content->remove(a);
    }
    if(total < 1e-3 && total > -1e-3){
        total = 0;
    }
}

Order &Order::operator=(const Order &o){
    if(this == &o){
        return *this;
    }

    this->content = new content_t(*(o.getContent()));
    this->client = o.getClient();
    this->total = o.getTotal();
    this->price = o.price;

    return *this;
}

qreal Order::getTotal() const{
    return this->total;
}


