#include "email_login_window.h"

EmailLoginWindow::EmailLoginWindow(Client *client, QWidget *parent) : QWidget(parent), client(client) {
    emailEdit=new QLineEdit(this);
    sendCodeButton=new QPushButton("发送验证码", this);
    codeEdit=new QLineEdit(this);
    verifyButton=new QPushButton("确定", this);

    emailEdit->setPlaceholderText("请输入邮箱");
    codeEdit->setPlaceholderText("请输入验证码");

    QHBoxLayout *emailLayout=new QHBoxLayout;
    emailLayout->addWidget(emailEdit);
    emailLayout->addWidget(sendCodeButton);

    QHBoxLayout *codeLayout=new QHBoxLayout;
    codeLayout->addWidget(codeEdit);
    codeLayout->addWidget(verifyButton);

    QVBoxLayout *mainLayout=new QVBoxLayout(this);
    mainLayout->addLayout(emailLayout);
    mainLayout->addLayout(codeLayout);

    setLayout(mainLayout);
    setWindowTitle("注册");
    resize(300, 120);

    client->emailLoginWindow = this;

    connect(sendCodeButton, &QPushButton::clicked, this, &EmailLoginWindow::onSendCodeClicked);
    connect(codeEdit, &QLineEdit::returnPressed, this, &EmailLoginWindow::onVerifyClicked);
    connect(verifyButton, &QPushButton::clicked, this, &EmailLoginWindow::onVerifyClicked);
}

bool EmailLoginWindow::isEmailValid(const QString &email) {     // 判断邮件是否合法
    static const QRegularExpression re("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    return re.match(email).hasMatch();
}

void EmailLoginWindow::onSendCodeClicked() {
    QString email = emailEdit->text().trimmed();
    if (email.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入邮箱！");
        return;
    }
    if(!isEmailValid(email)) {
        QMessageBox::warning(this, "错误", "请输入有效的邮箱地址！");
        return;
    }
    QString msg="EMAIL_LOGIN:"+email;
    client->nowEmail=email;
    client->sendNonTextMessage(msg);
}

void EmailLoginWindow::onVerifyClicked() {
    QString code=codeEdit->text().trimmed();
    if (code.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入验证码！");
        return;
    }
    if (code!=client->code) {
        QMessageBox::warning(this, "提示", "验证码错误！");
        return;
    } else {
        client->emailLoginSuccess();
    }
}
