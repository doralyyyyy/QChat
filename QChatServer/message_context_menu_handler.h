#ifndef MESSAGECONTEXTMENUHANDLER_H
#define MESSAGECONTEXTMENUHANDLER_H

#include <QObject>
#include <QListWidget>
#include <QMenu>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QPointer>
#include <QLabel>

class MessageContextMenuHandler : public QObject {
    Q_OBJECT
public:
    MessageContextMenuHandler(QListWidget *listWidget, QObject *parent=nullptr);

private slots:
    void showContextMenu(const QPoint &pos);
    void translateMessage();
    void searchMessage();
    void copyMessage();
    void speakMessage();
    void remindMessage();
    void handleTranslationFinished(QNetworkReply *reply);

private:
    QListWidget *listWidget;
    QString selectedText;
    QNetworkAccessManager *manager;
    QPointer<QProgressDialog> progressDialog;
};

#endif
