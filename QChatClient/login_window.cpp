#include "login_window.h"
#include "register_window.h"
#include "email_login_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {

    accountEdit=new QLineEdit(this);
    passwordEdit=new QLineEdit(this);
    loginButton=new QPushButton("登录", this);
    switchLoginMethodButton=new QPushButton("切换为邮箱验证码登录", this);
    rememberMeCheckBox=new QCheckBox("记住我", this);
    registerButton=new QPushButton("没有账号？注册", this);

    accountEdit->setPlaceholderText("请输入昵称或邮箱");
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->addWidget(accountEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);
    layout->addWidget(switchLoginMethodButton);

    QHBoxLayout *bottomLayout=new QHBoxLayout;
    bottomLayout->addWidget(rememberMeCheckBox);
    bottomLayout->addStretch();
    bottomLayout->addWidget(registerButton);
    layout->addLayout(bottomLayout);

    setLayout(layout);
    setWindowTitle("登录");
    resize(320, 180);

    client = new Client("localhost", 11455, this);   // 连接穿透网，11455端口

    loadCredentials();

    connect(loginButton,&QPushButton::clicked,this,&LoginWindow::onLoginClicked);
    connect(switchLoginMethodButton,&QPushButton::clicked,this,&LoginWindow::onSwitchMethodClicked);
    connect(registerButton,&QPushButton::clicked,this,&LoginWindow::onRegisterClicked);
}

void LoginWindow::onLoginClicked() {
    QString id=accountEdit->text().trimmed(); // 可为邮箱或昵称
    QString pwd=passwordEdit->text();

    if (id.isEmpty()||pwd.isEmpty()) {
        QMessageBox::warning(this,"提示","请填写完整信息！");
        return;
    }

    QString msg="LOGIN:"+id+"|"+pwd;
    client->sendNonTextMessage(msg);

    if(rememberMeCheckBox->isChecked()) saveCredentials();   // 记忆账号密码信息
    else {
        QSettings settings("QChat","QChatClient");;
        settings.remove("account");
        settings.remove("password");
    }
}

void LoginWindow::onSwitchMethodClicked() {
    EmailLoginWindow *e=new EmailLoginWindow(client);
    this->close();
    e->show();
}

void LoginWindow::onRegisterClicked() {
    RegisterWindow *r=new RegisterWindow(client);
    this->close();
    r->show();
}

void LoginWindow::loadCredentials() {
    QSettings settings("QChat","QChatClient");;
    QString acc=settings.value("account").toString();
    QString pwd=settings.value("password").toString();
    if(!acc.isEmpty()&&!pwd.isEmpty()) {
        accountEdit->setText(acc);
        passwordEdit->setText(pwd);
        rememberMeCheckBox->setChecked(true);
    }
}

void LoginWindow::saveCredentials() {
    QSettings settings("QChat","QChatClient");;
    settings.setValue("account",accountEdit->text());
    settings.setValue("password",passwordEdit->text());
}
