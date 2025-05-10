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
    QVBoxLayout *layout=new QVBoxLayout(&dialog);
    QDateTimeEdit *edit=new QDateTimeEdit(QDateTime::currentDateTime(),&dialog);
    edit->setCalendarPopup(true);
    layout->addWidget(edit);

    QPushButton *okBtn=new QPushButton("确定");
    layout->addWidget(okBtn);
    connect(okBtn,&QPushButton::clicked,&dialog,&QDialog::accept);

    if(dialog.exec()==QDialog::Accepted) {
        QDateTime time=edit->dateTime();
        qint64 delay=QDateTime::currentDateTime().msecsTo(time);
        if(delay>0) {
            QString text=selectedText;
            QTimer::singleShot(delay,this,[=]() {
                QMessageBox::information(listWidget,"定时提醒","提醒内容："+text);
            });
        }
    }
}
