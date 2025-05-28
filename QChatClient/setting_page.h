#ifndef SETTING_PAGE_H
#define SETTING_PAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "client.h"

class SettingPage : public QWidget {
    Q_OBJECT
public:
    explicit SettingPage(Client *client, QWidget *parent=nullptr);

    void updateUserInfo();

private slots:
    void changeAvatar();
    void changeBackground();
    void changeNickname();
    void changeInterest();
    void changePassword();

private:
    Client *client;

    QLabel *avatarLabel;
    QLabel *nicknameLabel;
    QLabel *emailLabel;
    QLabel *passwordLabel;
    QLabel *interestLabel;

    QPushButton *changeAvatarButton;
    QPushButton *changeBackgroundButton;
    QPushButton *changeNicknameButton;
    QPushButton *changeEmailButton;
    QPushButton *changePasswordButton;
    QPushButton *changeInterestButton;
};

#endif // SETTING_PAGE_H
