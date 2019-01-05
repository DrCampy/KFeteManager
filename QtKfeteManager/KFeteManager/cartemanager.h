#ifndef CARTEMANAGER_H
#define CARTEMANAGER_H

#include <QWidget>
#include <QListView>
#include <QString>
#include <QPushButton>
#include <QColorDialog>

#include "cartemodel.h"
#include "carteview.h"

class CarteManager : public QWidget
{
    Q_OBJECT
public:
    explicit CarteManager(CarteModel *carteModel, QWidget *parent = nullptr);

private:
    QPushButton     *previewButton;
    QPushButton     *validateButton;
    QListView       *articlesView;
    CarteModel      *carteModel;
    CarteView       *carteView;

signals:
    void finished();

public slots:
};

#endif // CARTEMANAGER_H
