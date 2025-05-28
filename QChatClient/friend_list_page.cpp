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
    friendList = new FriendListWidget(this);
    friendList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(friendList, &QListWidget::customContextMenuRequested, this, &FriendListPage::showContextMenu);

    addButton = new QPushButton("添加好友", this);
    connect(addButton, &QPushButton::clicked, this, &FriendListPage::onAddFriend);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(addButton);
    layout->addWidget(friendList);
    setLayout(layout);

    this->setStyleSheet(R"(
    QWidget {
        background-color: #f0f0f0;
    }

    QListWidget {
        background-color: #ffffff;
        border: none;
        border-radius: 10px;
        font-size: 14px;
        padding: 5px;
    }

    QListWidget::item {
        padding: 8px;
        margin: 4px;
        border-radius: 6px;
        color: #000000;
    }

    QListWidget::item:hover {
        background-color: #fbc2eb;
        color: white;
    }

    QPushButton {
        padding: 10px;
        border: none;
        border-radius: 12px;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #ff9a9e, stop:1 #fad0c4);
        color: white;
        font-size: 16px;
        font-weight: bold;
    }

    QPushButton:hover {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #fbc2eb, stop:1 #a6c1ee);
    }

    QMenu {
        background-color: #ffffff;
        border-radius: 8px;
        padding: 4px;
    }

    QMenu::item {
        padding: 10px 20px;
        color: #555;
        font-size: 14px;
        border-radius: 6px;
    }

    QMenu::item:selected {
        background-color: #fbc2eb;
        color: white;
    }

    QMenu::item:pressed {
        background-color: #a6c1ee;
    }
)");
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
    friendList->setFocusPolicy(Qt::NoFocus);  // 去除焦点框
    for (const QString &name : std::as_const(friendListData)) {
        // 判断状态
        QString status = (name == "Server")?"online":"offline";

        // 获取图标字符 + 中文文本
        QString icon, text;
        if(status == "online") {
            icon = "●";
            text = "在线";
        } else if(status == "offline") {
            icon = "○";
            text = "离线";
        } else if(status == "busy") {
            icon = "❗";
            text = "忙碌";
        } else if(status == "away") {
            icon = "◐";
            text = "离开";
        } else if(status == "invisible") {
            icon = "◑";
            text = "隐身";
        } else {
            icon = "?";
            text = "未知";
        }

        // 文字设置
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        QFontMetrics fm(font);
        int maxWidth=450; // 最大昵称宽度
        int nameWidth=fm.horizontalAdvance(name);
        int spaceWidth=fm.horizontalAdvance(" ");
        int neededSpaces=(maxWidth-nameWidth)/spaceWidth;
        QString fullText="  "+name+QString(" ").repeated(neededSpaces)+icon+" "+text;
        QListWidgetItem *item = new QListWidgetItem(fullText);
        item->setFont(font);

        if (avatarMap.contains(name)) {
            qDebug()<<name+"c";
            QPixmap pix(avatarMap[name]);
            if (!pix.isNull()) {
                qDebug()<<name+"d";
                friendList->setIconSize(QSize(44,44));
                item->setIcon(QIcon(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
                friendList->setItemAvatar(item,avatarMap[name]);
            }
        }
        item->setSizeHint(QSize(64, 64));
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
        QString nickname = item->text().simplified().split(' ').value(0);
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
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("错误");
        msgBox->setText("无法删除服务器");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #fff3f3;
                border-radius: 15px;
                padding: 20px;
                box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
            }
            QLabel {
                background: transparent;
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff9a9e;
                border: none;
                border-radius: 10px;
                padding: 8px;
                font-weight: bold;
                color: white;
            }
            QPushButton:hover {
                background-color: #fbc2eb;
            }
        )");

        msgBox->exec();
        return false;
    }
    friendListData.removeAll(nickname);
    QString msg = "REMOVE_FRIEND|" + client->nickname + "|" + nickname;
    client->sendNonTextMessage(msg);
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("提示");
    msgBox->setText("删除成功");
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setStandardButtons(QMessageBox::Ok);

    msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #fff3f3;
                border-radius: 15px;
                padding: 20px;
                box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
            }
            QLabel {
                background: transparent;
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff9a9e;
                border: none;
                border-radius: 10px;
                padding: 8px;
                font-weight: bold;
                color: white;
            }
            QPushButton:hover {
                background-color: #fbc2eb;
            }
        )");

    msgBox->exec();
    return true;
}
