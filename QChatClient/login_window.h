#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSettings>
#include <QPainter>
#include <QMouseEvent>
#include "client.h"

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(Client* client, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;  // 添加鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event) override;

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
