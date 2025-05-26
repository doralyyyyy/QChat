#include "match_page.h"
#include "client.h"
#include "friend_list_page.h"
#include "qtimer.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

MatchPage::MatchPage(Client *client, QWidget *parent)
    : QWidget(parent), client(client) {
    client->matchPage = this;

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    statusLabel = new QLabel("点击开始匹配", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 20px; color: #333;");

    matchButton = new QPushButton("开始匹配", this);
    matchButton->setFixedSize(160, 50);
    matchButton->setStyleSheet(buttonStyle());
    connect(matchButton, &QPushButton::clicked, this, &MatchPage::startMatching);

    cancelButton = new QPushButton("取消匹配", this);
    cancelButton->setFixedSize(160, 40);
    cancelButton->setStyleSheet(buttonStyle());
    cancelButton->setVisible(false);
    connect(cancelButton, &QPushButton::clicked, this, &MatchPage::cancelMatching);

    mainLayout->addWidget(statusLabel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(matchButton);
    mainLayout->addWidget(cancelButton);

    addFriendButton=new QPushButton("加为好友", this);
    addFriendButton->setFixedSize(160, 40);
    addFriendButton->setStyleSheet(buttonStyle());
    addFriendButton->setVisible(false);
    connect(addFriendButton, &QPushButton::clicked, this, &MatchPage::addFriend);
    mainLayout->addWidget(addFriendButton);

    setLayout(mainLayout);
}

void MatchPage::sleep(int ms) {
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

QString MatchPage::buttonStyle() const {
    return R"(
        QPushButton {
            font-size: 16px;
            font-weight: bold;
            border-radius: 20px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #ff9a9e, stop:1 #fad0c4);
            color: white;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #fbc2eb, stop:1 #a6c1ee);
        }
    )";
}

void MatchPage::startMatching() {
    addFriendButton->setVisible(false);
    matchButton->setVisible(false);
    statusLabel->setText("正在匹配...");
    cancelButton->setVisible(true);

    sleep(3000);

    client->sendNonTextMessage("MATCH_REQUEST|"+client->nickname);
}

void MatchPage::cancelMatching() {
    statusLabel->setText("匹配已取消");
    addFriendButton->setVisible(false);
    cancelButton->setVisible(false);
    matchButton->setText("开始匹配");
    matchButton->setVisible(true);
}

void MatchPage::matchingFinished(const QString &result) {
    statusLabel->setText(result);
    cancelButton->setVisible(false);
    matchButton->setText("重新匹配");
    matchButton->setVisible(true);

    if (result.startsWith("匹配成功")) {
        int start=result.indexOf("对方昵称: ")+6;
        int end=result.indexOf("\n", start);
        lastMatchedUser=result.mid(start, end-start).trimmed();
        qDebug()<<lastMatchedUser;
        addFriendButton->setVisible(true);
    }
}

void MatchPage::stopMatching() {   // 切换界面时强制停止匹配
    statusLabel->setText("点击开始匹配");
    addFriendButton->setVisible(false);
    cancelButton->setVisible(false);
    matchButton->setText("开始匹配");
    matchButton->setVisible(true);
}

void MatchPage::addFriend() {
    if (!lastMatchedUser.isEmpty()) {
        client->friendListPage->addFriend(lastMatchedUser);
    }
}
