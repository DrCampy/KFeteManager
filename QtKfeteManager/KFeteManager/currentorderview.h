#ifndef CURRENTORDERVIEW_H
#define CURRENTORDERVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QTableView>
#include <QLabel>

#include "order.h"
#include "currentordermodel.h"

class CurrentOrderView : public QWidget
{
    Q_OBJECT
    public:
        explicit CurrentOrderView(QWidget *parent = nullptr);
        CurrentOrderModel::Action getLastPerformedAction() const;
        Order::Price getSelectedPrice() const;
        void resetPrice();
        void setModel(CurrentOrderModel *model);
        void validated();
    private:
        void resize();
        QPushButton *plusButton;
        QPushButton *minusButton;
        QPushButton *deleteButton;
        QButtonGroup *priceButtonsGroup;
        QPushButton *normalPriceButton;
        QPushButton *reducedPriceButton;
        QPushButton *freePriceButton;
        CurrentOrderModel *model;
        QTableView *tableView;
        QLabel *totalLabel;

    private slots:
        void plusSlot();
        void minusSlot();
        void deleteSlot();
        void priceSlot(int id);
        void updateLabel();

    signals:
        void priceChanged(Order::Price);
        void actionPerformed(CurrentOrderModel::Action);
};

#endif // CURRENTORDERVIEW_H
