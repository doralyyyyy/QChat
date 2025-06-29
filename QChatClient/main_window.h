 #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "chat_page2.h"
#include "client.h"
#include "chat_page.h"
#include "friend_list_page.h"
#include "match_page.h"
#include "setting_page.h"
#include "tray_manager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Client *client, QWidget *parent = nullptr);

    void closeEvent(QCloseEvent *event) override;

private slots:
    void showChatPage();
    void showFriendListPage();
    void showMatchPage();
    void showSettingPage();
    void sleep(int ms);

private:
    Client *client;
    QWidget *mainWidget;
    QStackedWidget *stackedWidget;
    QPushButton *chatButton;
    QPushButton *friendListButton;
    QPushButton *matchButton;
    QPushButton *settingButton;
    TrayManager *trayManager;
    ChatPage *chatPage;
    ChatPage2 *chatPage2;
    QString chatPartner;
    QString chatPartnerStatus;
    FriendListPage *friendListPage;
    MatchPage *matchPage;
    SettingPage *settingPage;
};

#endif // MAINWINDOW_H
