#include "message_bubble_widget.h"
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QFile>

MessageBubbleWidget::MessageBubbleWidget(const QString &time, const QString &sender, const QString &content, bool isSelf, QWidget *parent)
    : QWidget(parent), isSelf(isSelf) {
    labelTop = new QLabel(time);
    labelBottom = new QLabel(sender + "：" + (content.startsWith("FILE|") ? content.mid(5) : content));
    labelTop->setStyleSheet("color: gray;");
    labelBottom->setWordWrap(true);

    bubbleLayout = new QVBoxLayout;
    bubbleLayout->addWidget(labelTop);
    bubbleLayout->addWidget(labelBottom);
    bubbleLayout->setSpacing(2);
    bubbleLayout->setContentsMargins(10, 6, 10, 6);

    bubble = new QWidget;
    bubble->setLayout(bubbleLayout);
    bubble->setStyleSheet(QString("background-color:%1; border-radius:8px;").arg(isSelf ? "#dcf8c6" : "#e1f5fe"));
    bubble->setMaximumWidth(400);

    hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(10, 5, 10, 5);
    hLayout->setSpacing(5);

    if (content.startsWith("FILE|")) {
        setupFileMessage(content);
    } else {
        setupTextMessage();
    }

    QWidget *w = new QWidget;
    w->setLayout(hLayout);
    setLayout(hLayout);
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
