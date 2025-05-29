#include "message_context_menu_handler.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QGuiApplication>
#include <QClipboard>
#include <QProcess>
#include <QVBoxLayout>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QTimer>

MessageContextMenuHandler::MessageContextMenuHandler(QListWidget *list, QObject *parent)    // 右键消息后产生的菜单
    : QObject(parent), listWidget(list), manager(new QNetworkAccessManager(this)) {
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listWidget, &QListWidget::customContextMenuRequested,this, &MessageContextMenuHandler::showContextMenu);
    connect(manager, &QNetworkAccessManager::finished,this, &MessageContextMenuHandler::handleTranslationFinished);
}

void MessageContextMenuHandler::showContextMenu(const QPoint &pos) {
    QListWidgetItem *item = listWidget->itemAt(pos);
    if (!item) return;

    selectedText = item->data(Qt::UserRole).toString();
    if (selectedText.isEmpty()) return;

    QMenu menu;
    QAction *actCopy = menu.addAction("复制");
    QAction *actSpeak = menu.addAction("朗读");
    QAction *actTranslate = menu.addAction("翻译");
    QAction *actSearch = menu.addAction("搜索");
    QAction *actRemind = menu.addAction("提醒");

    menu.setStyleSheet(R"(
        QMenu {
            font-size: 14px;
            background-color: #ffffff;
            border-radius: 10px;
            padding: 4px;
        }

        QMenu::item {
            padding: 4px 12px;
            min-height: 22px;
            font-size: 14px;
            border-radius: 5px;
            color: #333333;
        }

        QMenu::item:selected {
            background-color: #ff9a9e;
            color: white;
        }

        QMenu::item:hover {
            background-color: #fbc2eb;
        }
    )");

    QAction *chosen = menu.exec(listWidget->viewport()->mapToGlobal(pos));
    if (chosen == actTranslate) translateMessage();
    else if (chosen == actSearch) searchMessage();
    else if (chosen == actCopy) copyMessage();
    else if (chosen == actSpeak) speakMessage();
    else if (chosen == actRemind) remindMessage();
}

void MessageContextMenuHandler::translateMessage() {
    // 创建并显示进度对话框
    progressDialog = new QProgressDialog("正在翻译中，请稍候...", QString(), 0, 0, listWidget);
    progressDialog->setWindowTitle("翻译中");
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setCancelButton(nullptr); // 不显示取消按钮
    progressDialog->setMinimumDuration(0);    // 立即显示
    progressDialog->show();

    QString api = QString("https://api.mymemory.translated.net/get?q=%1&langpair=en|zh-CN").arg(QUrl::toPercentEncoding(selectedText));
    manager->get(QNetworkRequest(QUrl(api)));
}

void MessageContextMenuHandler::handleTranslationFinished(QNetworkReply *reply) {
    if (progressDialog) {
        progressDialog->close();
        progressDialog = nullptr;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QString translated = doc["responseData"]["translatedText"].toString();

    QMessageBox::information(listWidget,"结果",translated);
}

void MessageContextMenuHandler::searchMessage() {
    QString url = "https://www.baidu.com/s?wd=" + QUrl::toPercentEncoding(selectedText);
    QDesktopServices::openUrl(QUrl(url));
}

void MessageContextMenuHandler::copyMessage() {
    if(selectedText.isEmpty()) {
        return;
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(selectedText);
}

void MessageContextMenuHandler::speakMessage() {
    QString text = selectedText;
    if (text.isEmpty()) return;

#ifdef Q_OS_WIN
    QString escaped = text;
    escaped.replace("\"", "");
    QString script = QString("Add-Type -AssemblyName System.speech; (New-Object System.Speech.Synthesis.SpeechSynthesizer).Speak('%1')").arg(escaped);
    QProcess::startDetached("powershell", QStringList() << "-Command" << script);
#elif defined(Q_OS_MAC)
    QProcess::startDetached("say", QStringList() << text);
#elif defined(Q_OS_LINUX)
    QProcess::startDetached("espeak", QStringList() << text);
#endif
}

void MessageContextMenuHandler::remindMessage() {
    QDialog dialog;
    dialog.setWindowTitle("设置提醒时间");
    dialog.setFixedSize(260, 140);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLabel *label = new QLabel("提醒时间:");
    label->setStyleSheet("font-size: 13px;");

    QDateTimeEdit *edit = new QDateTimeEdit(QDateTime::currentDateTime(), &dialog);
    edit->setCalendarPopup(true);
    edit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    edit->setFixedHeight(28);
    edit->setStyleSheet(R"(
        QDateTimeEdit {
            border: 1px solid #ccc;
            border-radius: 8px;
            padding: 4px 8px;
            font-size: 12px;
        }
    )");

    QPushButton *okBtn = new QPushButton("确定", &dialog);
    okBtn->setFixedHeight(30);
    okBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #ff9a9e;
            border: none;
            border-radius: 10px;
            padding: 6px;
            font-weight: bold;
            color: white;
        }
        QPushButton:hover {
            background-color: #fbc2eb;
        }
    )");

    layout->addWidget(label);
    layout->addWidget(edit);
    layout->addStretch();
    layout->addWidget(okBtn);
    dialog.setLayout(layout);

    // 圆角 + 白底
    dialog.setStyleSheet(R"(
        QDialog {
            background-color: #ffffff;
            border-radius: 15px;
        }
    )");

    connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    if (dialog.exec() == QDialog::Accepted) {
        QDateTime time = edit->dateTime();
        qint64 delay = QDateTime::currentDateTime().msecsTo(time);
        if (delay > 0) {
            QString text = selectedText;
            QTimer::singleShot(delay, this, [=]() {
                QMessageBox *msgBox = new QMessageBox(listWidget);
                msgBox->setWindowTitle("定时提醒");
                msgBox->setText("提醒内容：" + text);
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
            });
        }
    }
}

