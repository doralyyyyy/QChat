#include "main_window.h"
#include "close_confirm_dialog.h"
#include "qsettings.h"

MainWindow::MainWindow(Server *server, QWidget *parent)
    : QMainWindow(parent), server(server) {
    this->setWindowTitle("QChatServer");
    this->resize(800,600);
    server->mainWindow=this;

    //创建左侧按钮
    chatButton = new QPushButton;
    friendListButton = new QPushButton;

    // 设置固定大小
    chatButton->setFixedSize(80, 80);
    friendListButton->setFixedSize(80, 80);

    // 设置图标
    chatButton->setIcon(QIcon(":/icons/chat_icon.png"));
    friendListButton->setIcon(QIcon(":/icons/friend_list_icon.png"));

    chatButton->setToolTip("聊天窗口");
    friendListButton->setToolTip("好友列表");

    QSize iconSize(48, 48);
    chatButton->setIconSize(iconSize);
    friendListButton->setIconSize(iconSize);

    // 设置样式
    QString buttonStyle = R"(
        QPushButton {
            background-color: transparent;
            border: none;
        }
        QPushButton:hover {
            background-color: rgba(255, 0, 0, 30%);
            border-radius: 10px;
        }
    )";
    chatButton->setStyleSheet(buttonStyle);
    friendListButton->setStyleSheet(buttonStyle);

    // 左侧布局
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setSpacing(20);           // 按钮之间的间距
    leftLayout->setContentsMargins(15, 20, 15, 20); // 上下左右边距
    leftLayout->addWidget(chatButton);
    leftLayout->addWidget(friendListButton);
    leftLayout->addStretch();

    QWidget *leftWidget=new QWidget;
    leftWidget->setLayout(leftLayout);

    // 创建右侧页面区
    chatPartner="";
    stackedWidget=new QStackedWidget;
    chatPage=new ChatPage(server);
    chatPage2=new ChatPage2(server);
    friendListPage=new FriendListPage(server);

    stackedWidget->addWidget(chatPage);
    stackedWidget->addWidget(chatPage2);
    stackedWidget->addWidget(friendListPage);

    // 整体布局
    QHBoxLayout *mainLayout=new QHBoxLayout;
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(stackedWidget,1);

    mainWidget=new QWidget;
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

    // 默认显示好友页
    stackedWidget->setCurrentWidget(friendListPage);

    // 显示托盘
    trayManager=new TrayManager(this);
    trayManager->show();

    // 信号槽连接
    connect(chatButton,&QPushButton::clicked,this,&MainWindow::showChatPage);
    connect(friendListButton,&QPushButton::clicked,this,&MainWindow::showFriendListPage);
    connect(friendListPage->friendList, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item){
        chatPartner=item->text().simplified().split(' ').value(0);
        chatButton->click();
    });
}

void MainWindow::sleep(int ms) {
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

void MainWindow::showChatPage() {
    if(chatPartner == "") {
        QMessageBox::critical(this,"错误","请选择聊天对象");
    } else if(chatPartner == server->nowClient) {
        chatPage->refresh();
        stackedWidget->setCurrentWidget(chatPage);
        chatPage->initMessage();
        chatPage->chatTitleLabel->setText(chatPartner);
    } else {
        stackedWidget->setCurrentWidget(chatPage2);
        chatPage2->chatTitleLabel->setText(chatPartner);
    }
}

void MainWindow::showFriendListPage() {
    stackedWidget->setCurrentWidget(friendListPage);
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

        if (dlg.getAction() == CloseConfirmDialog::Exit) {
            event->accept();
        } else {
            event->ignore();
            this->hide();
        }
    } else {
        event->ignore();
    }
}
