#ifndef FRIEND_LIST_PAGE_H
#define FRIEND_LIST_PAGE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QMap>
#include "friend_list_widget.h"

class Client;

class FriendListPage : public QWidget {
    Q_OBJECT
public:
    explicit FriendListPage(Client *c, QWidget *parent = nullptr);
    void getFriendList();
    void updateFriendList(const QStringList &list);
    void updateAvatar(const QString &nickname, const QString &path);
    void addFriend(const QString &nickname);

    QStringList friendListData;   // 好友昵称列表
    FriendListWidget *friendList;     // 好友列表

private:
    void setupUI();
    void sleep(int ms);
    bool removeFriend(const QString &nickname);
    void updateListDisplay();

private slots:
    void showContextMenu(const QPoint &pos);
    void onAddFriend();

private:
    Client *client;
    QPushButton *addButton;
    QMap<QString, QString> avatarMap; // 昵称 -> 本地头像路径
};

#endif // FRIEND_LIST_PAGE_H
