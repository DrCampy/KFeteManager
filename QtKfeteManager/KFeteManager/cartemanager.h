#ifndef CARTEMANAGER_H
#define CARTEMANAGER_H

#include <QWidget>
#include <QListView>
#include <QString>
#include <QPushButton>
#include <QColorDialog>

#include "cartemodel.h"
#include "carteview.h"

class CarteManager;

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

    int selectedIndex = 0;

    QString text;
    QColor backgroundColor;
    QColor textColor;

    QColor defaultBackgroundColor;
    QColor defaultTextColor;

    void loadPreview();

signals:
    void finished();

public slots:
    void selectButton(int);
    void confirmChanges();
    void selectedArticle(const QModelIndex &index);
};

#endif // CARTEMANAGER_H
