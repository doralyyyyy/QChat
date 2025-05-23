#include "friend_list_page.h"
#include "client.h"
#include "qtimer.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenu>
#include <QListWidgetItem>
#include <QPixmap>
#include <QIcon>

FriendListPage::FriendListPage(Client *c, QWidget *parent)
    : QWidget(parent), client(c) {
    client->friendListPage=this;

    setupUI();
    getFriendList();
}

void FriendListPage::sleep(int ms) {
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

void FriendListPage::setupUI() {
    friendList = new QListWidget(this);
    friendList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(friendList, &QListWidget::customContextMenuRequested, this, &FriendListPage::showContextMenu);

    addButton = new QPushButton("添加好友", this);
    connect(addButton, &QPushButton::clicked, this, &FriendListPage::onAddFriend);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(addButton);
    layout->addWidget(friendList);
    setLayout(layout);
}

void FriendListPage::getFriendList() {
    QString msg="GET_FRIEND_LIST|"+client->nickname;
    client->sendNonTextMessage(msg);
}

void FriendListPage::updateFriendList(const QStringList &list) {
    friendListData = list;

    for (const QString &name : list) {
        QString avatarPath = QCoreApplication::applicationDirPath()+"/avatars/"+name+"_avatar.png";
        qDebug()<<avatarPath<<"__name";
        if (!QFileInfo::exists(avatarPath)) {
            QString msg = "GET_AVATAR|" + name + '\n';
            qDebug() << msg;
            client->sendNonTextMessage(msg);
        } else {
            updateAvatar(name,avatarPath);
        }
    }
    updateListDisplay();
}

void FriendListPage::updateAvatar(const QString &nickname, const QString &path) {
    avatarMap[nickname] = path;
    updateListDisplay();
}

void FriendListPage::updateListDisplay() {
    friendList->clear();
    qDebug()<<"1";
    for (const QString &name : std::as_const(friendListData)) {
        qDebug()<<"2";
        QListWidgetItem *item = new QListWidgetItem(name);
        if (avatarMap.contains(name)) {
            qDebug()<<"3";
            QPixmap pix(avatarMap[name]);
            if (!pix.isNull()) {
                qDebug()<<"4";
                item->setIcon(QIcon(pix.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            }
        }
        friendList->addItem(item);
    }
}

void FriendListPage::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = friendList->itemAt(pos);
    if (!item) return;

    QMenu menu(this);
    QAction *removeAction = menu.addAction("删除好友");
    QAction *selectedAction = menu.exec(friendList->viewport()->mapToGlobal(pos));
    if (selectedAction == removeAction) {
        QString nickname = item->text();
        if(removeFriend(nickname)) delete item;
    }
}

void FriendListPage::onAddFriend() {
    bool ok;
    QString name = QInputDialog::getText(this, "添加好友", "好友昵称：", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        addFriend(name);
    }
}

void FriendListPage::addFriend(const QString &nickname) {
    QString msg = "ADD_FRIEND|" + client->nickname + "|" + nickname;
    client->sendNonTextMessage(msg);
}

bool FriendListPage::removeFriend(const QString &nickname) {
    if(nickname=="Server"){
        QMessageBox::warning(nullptr, "错误", "无法删除服务器");
        return false;
    }
    friendListData.removeAll(nickname);
    QString msg = "REMOVE_FRIEND|" + client->nickname + "|" + nickname;
    client->sendNonTextMessage(msg);
    QMessageBox::information(nullptr, "提示", "删除成功");
    return true;
}
