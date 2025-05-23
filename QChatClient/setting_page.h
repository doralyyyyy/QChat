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

private:
    Client *client;

    QLabel *avatarLabel;
    QLabel *nicknameLabel;
    QLabel *emailLabel;

    QPushButton *changeNicknameButton;
    QPushButton *changeAvatarButton;
};

#endif // SETTING_PAGE_H
