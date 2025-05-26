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
    void changeNickname();
    void changeAvatar();
    void changeInterest();

private:
    Client *client;

    QLabel *avatarLabel;
    QLabel *nicknameLabel;
    QLabel *emailLabel;
    QLabel *interestLabel;

    QPushButton *changeNicknameButton;
    QPushButton *changeAvatarButton;
    QPushButton *changeInterestButton;
};

#endif // SETTING_PAGE_H
