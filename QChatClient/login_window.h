#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QSettings>
#include "client.h"

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent=nullptr);

private:
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *switchLoginMethodButton;
    QCheckBox *rememberMeCheckBox;
    QPushButton *registerButton;
    Client *client;

    void loadCredentials();
    void saveCredentials();

private slots:
    void onLoginClicked();
    void onSwitchMethodClicked();
    void onRegisterClicked();
};

#endif
