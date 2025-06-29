#include "match_page.h"
#include "client.h"
#include "friend_list_page.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QEventLoop>

MatchPage::MatchPage(Client *client, QWidget *parent)
    : QWidget(parent), client(client) {
    client->matchPage = this;

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 背景GIF
    gifLabel = new QLabel(this);
    gifLabel->setScaledContents(false);
    gifLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gifLabel->setAlignment(Qt::AlignCenter);
    gifLabel->lower();  // 放到最底层

    movie = new QMovie(":/gifs/matching.GIF");
    connect(movie, &QMovie::frameChanged, this, &MatchPage::updateCroppedFrame);
    movie->start();
    movie->stop();

    statusLabel = new QLabel("点击开始匹配", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 20px; color: #333;");
    statusLabel->raise();

    matchButton = new QPushButton("开始匹配", this);
    matchButton->setFixedSize(160, 50);
    matchButton->setStyleSheet(buttonStyle());
    connect(matchButton, &QPushButton::clicked, this, &MatchPage::startMatching);

    cancelButton = new QPushButton("取消匹配", this);
    cancelButton->setFixedSize(160, 40);
    cancelButton->setStyleSheet(buttonStyle());
    cancelButton->setVisible(false);
    connect(cancelButton, &QPushButton::clicked, this, &MatchPage::cancelMatching);

    addFriendButton = new QPushButton("加为好友", this);
    addFriendButton->setFixedSize(160, 40);
    addFriendButton->setStyleSheet(buttonStyle());
    addFriendButton->setVisible(false);
    connect(addFriendButton, &QPushButton::clicked, this, &MatchPage::addFriend);

    mainLayout->addWidget(statusLabel, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(40);
    mainLayout->addWidget(matchButton, 0, Qt::AlignHCenter);
    mainLayout->addWidget(cancelButton, 0, Qt::AlignHCenter);
    mainLayout->addWidget(addFriendButton, 0, Qt::AlignHCenter);

    setLayout(mainLayout);

    matchTimer = new QTimer(this);
    matchTimer->setSingleShot(true);
    connect(matchTimer, &QTimer::timeout, this, [this]() {
        this->client->sendNonTextMessage("MATCH_REQUEST|" + this->client->nickname);
    });
}

QString MatchPage::buttonStyle() const {
    return R"(
        QPushButton {
            font-size: 16px;
            font-weight: bold;
            border-radius: 20px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff9a9e, stop:1 #fad0c4);
            color: white;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fbc2eb, stop:1 #a6c1ee);
        }
    )";
}

void MatchPage::startMatching() {
    addFriendButton->setVisible(false);
    matchButton->setVisible(false);
    cancelButton->setVisible(true);
    statusLabel->setText("正在匹配...");

    if (movie->state() != QMovie::Running) {
        movie->start();
    }

    matchTimer->start(3000);
}

void MatchPage::cancelMatching() {
    statusLabel->setText("匹配已取消");
    if (movie->state() == QMovie::Running) {
        movie->stop();
        movie->jumpToFrame(0);
        updateCroppedFrame(-1);  // 刷新
    }

    cancelButton->setVisible(false);
    addFriendButton->setVisible(false);
    matchButton->setText("开始匹配");
    matchButton->setVisible(true);
    matchTimer->stop();
}

void MatchPage::matchingFinished(const QString &result) {
    statusLabel->setText(result);
    if (movie->state() == QMovie::Running) {
        movie->stop();
        movie->jumpToFrame(0);
        updateCroppedFrame(-1);  // 刷新
    }

    cancelButton->setVisible(false);
    matchButton->setText("重新匹配");
    matchButton->setVisible(true);

    if (result.startsWith("匹配成功")) {
        int start = result.indexOf("对方昵称: ") + 6;
        int end = result.indexOf("\n", start);
        lastMatchedUser = result.mid(start, end - start).trimmed();
        addFriendButton->setVisible(true);
    }
}

void MatchPage::stopMatching() {
    statusLabel->setText("点击开始匹配");
    if (movie->state() == QMovie::Running) {
        movie->stop();
        movie->jumpToFrame(0);
        updateCroppedFrame(-1);  // 刷新
    }

    cancelButton->setVisible(false);
    addFriendButton->setVisible(false);
    matchButton->setText("开始匹配");
    matchButton->setVisible(true);
    matchTimer->stop();
}

void MatchPage::addFriend() {
    if (!lastMatchedUser.isEmpty()) {
        client->friendListPage->addFriend(lastMatchedUser);
    }
}

void MatchPage::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    gifLabel->resize(size());
    updateCroppedFrame(-1);
}

void MatchPage::updateCroppedFrame(int) {
    if (!movie) return;

    QPixmap original = movie->currentPixmap();
    if (original.isNull()) return;

    // 裁剪左右各40像素
    int cropLeft = 40;
    int cropRight = 40;
    int width = original.width() - cropLeft - cropRight;
    int height = original.height();

    if (width <= 0 || height <= 0) return;

    QPixmap cropped = original.copy(cropLeft, 0, width, height);

    QPixmap scaled = cropped.scaled(gifLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    gifLabel->setPixmap(scaled);
}
