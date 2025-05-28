#include "friend_list_page.h"
#include "server.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenu>
#include <QListWidgetItem>
#include <QPixmap>
#include <QIcon>

FriendListPage::FriendListPage(Server *c, QWidget *parent)
    : QWidget(parent), server(c) {
    server->friendListPage=this;

    setupUI();
    updateListDisplay();
}
void FriendListPage::setupUI() {
    emptyLabel=new QLabel("无账号",this);
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet("font-size: 35px; color: gray; background: transparent;");
    emptyLabel->hide(); // 默认隐藏
    friendList = new FriendListWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(emptyLabel); // 添加到主布局中（在 friendList 前）
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
        min-height: 100px;
    }

    QListWidget::item:icon {
        min-width: 96px;
        min-height: 96px;
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

void FriendListPage::updateListDisplay() {
    friendList->clear();
    friendListData = server->userDB->getFriends("Server");
    friendList->setFocusPolicy(Qt::NoFocus);  // 去除焦点框
    if (friendListData.isEmpty()) {
        emptyLabel->show();
        friendList->hide();
        return;
    }
    emptyLabel->hide();
    friendList->show();
    for (const QString &name : std::as_const(friendListData)) {
        QString path=server->userDB->getAvatar(name).trimmed();
        QPixmap pix(path);

        // 判断状态
        QString status = (name == server->nowClient)?"online":"offline";

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

        item->setIcon(QIcon(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        item->setSizeHint(QSize(64, 64));
        friendList->setItemAvatar(item,path);

        friendList->setIconSize(QSize(44, 44));
        friendList->addItem(item);
    }
}
