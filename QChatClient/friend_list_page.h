#ifndef FRIEND_LIST_PAGE_H
#define FRIEND_LIST_PAGE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QMap>

class Client;

class FriendListPage : public QWidget {
    Q_OBJECT
public:
    explicit FriendListPage(Client *c, QWidget *parent = nullptr);
    void getFriendList();
    void updateFriendList(const QStringList &list);
    void updateAvatar(const QString &nickname, const QString &path);

    QStringList friendListData;   // 好友列表

private:
    void setupUI();
    void sleep(int ms);
    void addFriend(const QString &nickname);
    bool removeFriend(const QString &nickname);
    void updateListDisplay();

private slots:
    void showContextMenu(const QPoint &pos);
    void onAddFriend();

private:
    Client *client;
    QListWidget *friendList;
    QPushButton *addButton;
    QMap<QString, QString> avatarMap; // 昵称 -> 本地头像路径
};

#endif // FRIEND_LIST_PAGE_H
