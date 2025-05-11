#include "message_bubble_widget.h"
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QFile>

MessageBubbleWidget::MessageBubbleWidget(const QString &time, const QString &sender, const QString &content, bool isSelf, QWidget *parent)
    : QWidget(parent), isSelf(isSelf) {
    labelTop = new QLabel(time);
    labelBottom = new QLabel(content.startsWith("FILE|") ? content.mid(5) : content); // 只显示消息内容
    labelTop->setStyleSheet("color: gray; font-size: 10px;");
    labelBottom->setWordWrap(true);
    labelBottom->setStyleSheet("font-size: 14px; color: #333;");

    bubbleLayout = new QVBoxLayout;
    bubbleLayout->addWidget(labelTop);
    bubbleLayout->addWidget(labelBottom);
    bubbleLayout->setSpacing(4); // 更细致的间距
    bubbleLayout->setContentsMargins(15, 10, 15, 10);

    // 背景颜色和圆角
    bubble = new QWidget;
    bubble->setLayout(bubbleLayout);
    bubble->setStyleSheet(QString("background-color:%1; border-radius:12px;").arg(isSelf ? "#dcf8c6" : "#e1f5fe"));
    bubble->setMaximumWidth(350);  // 限制最大宽度

    hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(15, 10, 15, 10);  // 改变外部间距
    hLayout->setSpacing(8);

    if (content.startsWith("FILE|")) {
        setupFileMessage(content);
    } else {
        setupTextMessage();
    }

    QWidget *w = new QWidget;
    w->setLayout(hLayout);
    setLayout(hLayout);

    (void)sender;     // 去除警告
}

void MessageBubbleWidget::setupTextMessage() {
    if (isSelf) {
        // 如果是自己发的消息，显示在右边
        hLayout->addStretch();
        hLayout->addWidget(bubble, 0, Qt::AlignRight);
    } else {
        // 如果是别人发的消息，显示在左边
        hLayout->addWidget(bubble, 0, Qt::AlignLeft);
        hLayout->addStretch();
    }
}

void MessageBubbleWidget::setupFileMessage(const QString &content) {
    QString fname = content.mid(5);
    QString tryPath1 = QCoreApplication::applicationDirPath() + "/received_" + fname;
    QString tryPath2 = QCoreApplication::applicationDirPath() + "/sent_" + fname;
    QString path;
    if (QFile::exists(tryPath1)) path = tryPath1;
    else if (QFile::exists(tryPath2)) path = tryPath2;

    QPushButton *btn = new QPushButton("打开");
    btn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff9a9e, stop:1 #fad0c4);
            border-radius: 10px;
            color: white;
            padding: 6px 10px;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fbc2eb, stop:1 #a6c1ee);
        }
    )");

    connect(btn, &QPushButton::clicked, this, [path]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });

    if (isSelf) {
        // 如果是自己发的消息，显示在右边
        hLayout->addStretch();
        hLayout->addWidget(btn, 0, Qt::AlignBottom);
        hLayout->addWidget(bubble, 0, Qt::AlignRight);
    } else {
        // 如果是别人发的消息，显示在左边
        hLayout->addWidget(bubble, 0, Qt::AlignLeft);
        hLayout->addWidget(btn, 0, Qt::AlignBottom);
        hLayout->addStretch();
    }
}
