#include "login_window.h"
#include "register_window.h"
#include "email_login_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>

LoginWindow::LoginWindow(Client* client, QWidget *parent) : QWidget(parent), client(client) {

    client->loginWindow=this;

    setWindowFlags(Qt::Window | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    accountEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    loginButton = new QPushButton("登录", this);
    switchLoginMethodButton = new QPushButton("切换为邮箱验证码登录", this);
    rememberMeCheckBox = new QCheckBox("记住我", this);
    registerButton = new QPushButton("没有账号？注册", this);
    registerButton->setObjectName("registerButton");

    accountEdit->setPlaceholderText("请输入昵称或邮箱");
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QWidget *panel = new QWidget(this);
    panel->setObjectName("loginPanel");  // 设置面板样式

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(panel);
    mainLayout->setContentsMargins(60, 40, 60, 40);

    QVBoxLayout *layout = new QVBoxLayout(panel);  // 将控件添加到panel的layout中
    layout->addWidget(accountEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);
    layout->addWidget(switchLoginMethodButton);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(rememberMeCheckBox);
    bottomLayout->addStretch();
    bottomLayout->addWidget(registerButton);
    layout->addLayout(bottomLayout);

    setLayout(mainLayout);
    setWindowTitle("登录");
    resize(420, 320);

    loadCredentials();

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(switchLoginMethodButton, &QPushButton::clicked, this, &LoginWindow::onSwitchMethodClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);

    setStyleSheet(R"(
        QWidget {
            border: 3px solid #dcdcdc;
            border-radius: 20px;
        }

        #loginPanel {
            background-color: rgba(255, 255, 255, 240);
            border-radius: 20px;
            padding: 25px;
            box-shadow: 0px 4px 15px rgba(0,0,0,0.1);
        }
        #topBar {
            background-color: #ffccff;
            border-top-left-radius: 20px;
            border-top-right-radius: 20px;
            padding: 10px;
            font-size: 16px;
            font-weight: bold;
            color: #555;
            text-align: center;
        }
        #closeButton {
            background-color: transparent;
            border: none;
            color: #ff4444;
            font-size: 18px;
        }
        QLineEdit {
            padding: 10px;
            border: 2px solid #ccc;
            border-radius: 12px;
            background: #ffffff;
            font-size: 14px;
        }

        QPushButton {
            padding: 10px;
            border: none;
            border-radius: 12px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #ff9a9e, stop:1 #fad0c4);
            color: white;
            font-size: 16px;
            font-weight: bold;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #fbc2eb, stop:1 #a6c1ee);
        }

        QCheckBox {
            color: #555;
            font-size: 14px;
        }

        QPushButton#registerButton {
            color: #007bff;
            background: transparent;
            text-decoration: underline;
            font-size: 14px;
        }
    )");
}

// 用于窗口拖动
QPoint mousePressPos;

void LoginWindow::mousePressEvent(QMouseEvent *event) {
    // 记录鼠标按下的位置
    if (event->button() == Qt::LeftButton) {
        mousePressPos = event->globalPosition().toPoint() - this->pos();
        event->accept();
    }
}

void LoginWindow::mouseMoveEvent(QMouseEvent *event) {
    // 通过鼠标移动来改变窗口位置
    if (event->buttons() & Qt::LeftButton) {
        this->move(event->globalPosition().toPoint() - mousePressPos);
        event->accept();
    }
}

void LoginWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QPixmap background(":/images/login_background.png");
    painter.drawPixmap(this->rect(), background);
}

void LoginWindow::onLoginClicked() {
    QString id = accountEdit->text().trimmed(); // 可为邮箱或昵称
    QString pwd = passwordEdit->text();

    if (id.isEmpty() || pwd.isEmpty()) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("提示");
        msgBox->setText("请填写完整信息！");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #fff3f3;
                border-radius: 15px;
                padding: 20px;
                box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
            }
            QLabel {
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
        return;
    }

    QString msg = "LOGIN:" + id + "|" + pwd;
    client->sendNonTextMessage(msg);

    if (rememberMeCheckBox->isChecked()) saveCredentials();   // 记忆账号密码信息
    else {
        QSettings settings("QChat", "QChatClient");
        settings.remove("account");
        settings.remove("password");
    }
}

void LoginWindow::onSwitchMethodClicked() {
    EmailLoginWindow *e = new EmailLoginWindow(client);
    this->close();
    e->show();
}

void LoginWindow::onRegisterClicked() {
    RegisterWindow *r = new RegisterWindow(client);
    this->close();
    r->show();
}

void LoginWindow::loadCredentials() {
    QSettings settings("QChat", "QChatClient");
    QString acc = settings.value("account").toString();
    QString pwd = settings.value("password").toString();
    if (!acc.isEmpty() && !pwd.isEmpty()) {
        accountEdit->setText(acc);
        passwordEdit->setText(pwd);
        rememberMeCheckBox->setChecked(true);
    }
}

void LoginWindow::saveCredentials() {
    QSettings settings("QChat", "QChatClient");
    settings.setValue("account", accountEdit->text());
    settings.setValue("password", passwordEdit->text());
}
