#include "main_window.h"
#include "close_confirm_dialog.h"
#include "qsettings.h"

MainWindow::MainWindow(Client *client, QWidget *parent)
    : QMainWindow(parent), client(client) {
    this->setWindowTitle("QChat");
    this->resize(800,600);
    client->mainWindow=this;

    // 创建左侧按钮
    chatButton=new QPushButton("聊天窗口");
    friendListButton=new QPushButton("好友列表");
    matchButton=new QPushButton("好友匹配");
    settingButton=new QPushButton("设置");

    chatButton->setMinimumHeight(40);
    friendListButton->setMinimumHeight(40);
    matchButton->setMinimumHeight(40);
    settingButton->setMinimumHeight(40);

    QVBoxLayout *leftLayout=new QVBoxLayout;
    leftLayout->addWidget(chatButton);
    leftLayout->addWidget(friendListButton);
    leftLayout->addWidget(matchButton);
    leftLayout->addWidget(settingButton);
    leftLayout->addStretch();

    QWidget *leftWidget=new QWidget;
    leftWidget->setLayout(leftLayout);

    // 创建右侧页面区
    stackedWidget=new QStackedWidget;
    chatPage=new ChatPage(client);
    friendListPage=new FriendListPage(client);
    matchPage=new MatchPage(client);
    settingPage=new SettingPage(client);

    stackedWidget->addWidget(chatPage);
    stackedWidget->addWidget(friendListPage);
    stackedWidget->addWidget(matchPage);
    stackedWidget->addWidget(settingPage);

    // 整体布局
    QHBoxLayout *mainLayout=new QHBoxLayout;
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(stackedWidget,1);

    mainWidget=new QWidget;
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

    // 默认显示聊天页
    stackedWidget->setCurrentWidget(chatPage);

    // 显示托盘
    trayManager=new TrayManager(this);
    trayManager->show();

    // 信号槽连接
    connect(chatButton,&QPushButton::clicked,this,&MainWindow::showChatPage);
    connect(friendListButton,&QPushButton::clicked,this,&MainWindow::showFriendListPage);
    connect(matchButton,&QPushButton::clicked,this,&MainWindow::showMatchPage);
    connect(settingButton,&QPushButton::clicked,this,&MainWindow::showSettingPage);
}

void MainWindow::sleep(int ms) {
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

void MainWindow::showChatPage() {
    chatPage->refresh();
    stackedWidget->setCurrentWidget(chatPage);
}

void MainWindow::showFriendListPage() {
    stackedWidget->setCurrentWidget(friendListPage);
}

void MainWindow::showMatchPage() {
    stackedWidget->setCurrentWidget(matchPage);
}

void MainWindow::showSettingPage() {
    QString msg="GET_SELF_AVATAR|"+client->nickname+'\n';  // 获取自己头像
    client->sendNonTextMessage(msg);
    stackedWidget->setCurrentWidget(settingPage);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings("MyCompany", "QChat");
    if (settings.value("close_behavior/skipPrompt", false).toBool()) {
        QString choice = settings.value("close_behavior/choice", "minimize").toString();
        if (choice == "exit") {
            event->accept();
        } else {
            event->ignore();
            this->hide();
        }
        return;
    }

    CloseConfirmDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.dontAskAgain()) {
            settings.setValue("close_behavior/skipPrompt", true);
            settings.setValue("close_behavior/choice", dlg.getAction() == CloseConfirmDialog::Exit ? "exit" : "minimize");
        }

        if (dlg.getAction() == CloseConfirmDialog::Exit)
            event->accept();
        else {
            event->ignore();
            this->hide();
        }
    } else {
        event->ignore();
    }
}
