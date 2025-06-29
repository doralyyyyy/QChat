#ifndef FRIEND_LIST_PAGE_H
#define FRIEND_LIST_PAGE_H

#include "qlabel.h"
#include "friend_list_widget.h"
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QMap>

class Server;

class FriendListPage : public QWidget {
    Q_OBJECT
public:
    explicit FriendListPage(Server *c, QWidget *parent = nullptr);

    void updateListDisplay();

    QStringList friendListData;   // 好友列表
    FriendListWidget *friendList;

private:
    void setupUI();

private:
    Server *server;
    QLabel *emptyLabel;
};

#endif // FRIEND_LIST_PAGE_H
