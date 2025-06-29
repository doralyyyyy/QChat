 #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "chat_page2.h"
#include "server.h"
#include "chat_page.h"
#include "friend_list_page.h"
#include "tray_manager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Server *server, QWidget *parent = nullptr);

    void closeEvent(QCloseEvent *event) override;

private slots:
    void showChatPage();
    void showFriendListPage();
    void sleep(int ms);

private:
    Server *server;
    QWidget *mainWidget;
    QStackedWidget *stackedWidget;
    QPushButton *chatButton;
    QPushButton *friendListButton;
    TrayManager *trayManager;
    ChatPage *chatPage;
    ChatPage2 *chatPage2;
    QString chatPartner;
    FriendListPage *friendListPage;
};

#endif // MAINWINDOW_H
