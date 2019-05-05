#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>
#include <QLineEdit>
/*
 *Interface de connexion avec le clavier.
 * Emet enteredPassword(QString) quand un mot de pass est entré.
 */
class LoginView : public QWidget
{
    Q_OBJECT
public:
    explicit LoginView(QWidget *parent = nullptr);

private:
  QLineEdit *passwordDisplay;

signals:
    void enteredPassword(QString);
    void quit();

public slots:
    void clicNumpad(int);
    void clicDelete(bool);
    void returnPressed();
};

#endif // LOGINVIEW_H



