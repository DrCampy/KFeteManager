#include "order.h"
#include "clientlist.h"

Order::Order(Client *client)
{
    content = new QMap<Article, unsigned int>;
    this->client = client;
}

Order::~Order(){
    delete client;
    delete content;
}

void Order::addArticle(Article &a){
    if(!content->contains(a)){
        content->insert(a, 1);
    }else{
        content->insert(a, content->value(a)+1);
    }
    switch(price){
    case normal:
        total += a.getPrice();
        break;
    case reduced:
        total += a.getReducedPrice();
        break;
    case free:
        break;
    }

}

const QMap<Article, unsigned int> *Order::getContent() const{
    return content;
}

Client *Order::getClient() const{
    return client;
}

void Order::setPrice(Price price){
    this->price = price;
    if(price == free){
        total = 0;
        return;
    }

    //We are using java-style iterator because STL-style iterator won't compile?
    QMapIterator<Article, unsigned int> iterator(*content);
    while(iterator.hasNext()){
        switch(price){
        case normal:
            total += iterator.key().getPrice() * iterator.value();
            break;
        case reduced:
            total += iterator.key().getReducedPrice() * iterator.value();
            break;
        case free:
            //We never get here.
            break;
        }
    }
}

void Order::removeArticle(Article &a){
    if(content->contains(a)){
        unsigned int count = content->value(a);
        count--;
        if(count == 0){
            content->remove(a);
        }else{
            content->insert(a, count);
        }
        switch(price){
        case normal:
            total -= a.getPrice();
            break;
        case reduced:
            total -= a.getReducedPrice();
            break;
        case free:
            break;
        }
    }
}

void Order::deleteArticle(Article &a){
    if(content->contains(a)){
        unsigned int count = content->value(a);
        content->remove(a);
        switch(price){
        case normal:
            total -= a.getPrice()*count;
            break;
        case reduced:
            total -= a.getReducedPrice()*count;
            break;
        case free:
            break;
        }
    }
}

Order &Order::operator=(const Order &o){
    if(this == &o){
        return *this;
    }

    this->content = new QMap<Article, unsigned int>(*(o.getContent()));
    this->client = o.getClient();
    this->total = o.getTotal();
    this->price = o.price;

    return *this;
}

qreal Order::getTotal() const{

}


