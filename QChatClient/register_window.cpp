#include "register_window.h"

RegisterWindow::RegisterWindow(Client *client, QWidget *parent) : QWidget(parent), client(client){
    mainLayout=new QVBoxLayout(this);

    nicknameEdit=new QLineEdit(this);
    nicknameEdit->setPlaceholderText("请输入昵称");

    passwordEdit=new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);

    confirmPasswordEdit=new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("请确认密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    // 错误提示标签
    errorLabel=new QLabel(this);
    errorLabel->setStyleSheet("color:red");

    // 邮箱 + 按钮
    emailEdit=new QLineEdit(this);
    emailEdit->setPlaceholderText("请输入邮箱");
    sendCodeButton=new QPushButton("发送验证码", this);
    emailLayout=new QHBoxLayout;
    emailLayout->addWidget(emailEdit);
    emailLayout->addWidget(sendCodeButton);

    // 验证码 + 按钮
    codeEdit=new QLineEdit(this);
    codeEdit->setPlaceholderText("请输入验证码");
    verifyButton=new QPushButton("确定", this);
    codeLayout=new QHBoxLayout;
    codeLayout->addWidget(codeEdit);
    codeLayout->addWidget(verifyButton);

    // 加入布局
    mainLayout->addWidget(nicknameEdit);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(confirmPasswordEdit);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(emailLayout);
    mainLayout->addLayout(codeLayout);

    setLayout(mainLayout);
    setWindowTitle("注册");
    resize(320, 140);

    client->registerWindow = this;

    connect(sendCodeButton, &QPushButton::clicked, this, &RegisterWindow::onSendCodeClicked);
    connect(codeEdit, &QLineEdit::returnPressed, this, &RegisterWindow::onVerifyClicked);
    connect(verifyButton, &QPushButton::clicked, this, &RegisterWindow::onVerifyClicked);
    connect(passwordEdit,&QLineEdit::textChanged,this,&RegisterWindow::checkPasswordsMatch);
    connect(confirmPasswordEdit,&QLineEdit::textChanged,this,&RegisterWindow::checkPasswordsMatch);

}

void RegisterWindow::checkPasswordsMatch() {
    if (passwordEdit->text() != confirmPasswordEdit->text()) {
        errorLabel->setText("两次输入的密码不一致");
    } else {
        errorLabel->clear();
    }
}

bool RegisterWindow::isEmailValid(const QString &email) {    // 判断是否为有效的邮箱地址
    static const QRegularExpression re("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    return re.match(email).hasMatch();
}

void RegisterWindow::onSendCodeClicked() {
    QString email=emailEdit->text().trimmed();
    if(email.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入邮箱！");
        return;
    }
    if(!isEmailValid(email)) {
        QMessageBox::warning(this, "错误", "请输入有效的邮箱地址！");
        return;
    }
    QString msg="Email:"+email;
    client->nowEmail=email;
    client->sendNonTextMessage(msg);
}

void RegisterWindow::onVerifyClicked() {
    QString nick=nicknameEdit->text().trimmed();
    QString pwd=passwordEdit->text();
    QString pwd2=confirmPasswordEdit->text();
    QString email=emailEdit->text().trimmed();
    QString code=codeEdit->text().trimmed();
    if (nick.isEmpty()||pwd.isEmpty()||pwd2.isEmpty()||email.isEmpty()||code.isEmpty()) {
        QMessageBox::warning(this,"错误","请填写完整信息");
        return;
    }
    if (pwd!=pwd2) {
        QMessageBox::warning(this,"错误","两次密码不一致");
        return;
    }
    if(code.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入验证码！");
        return;
    }
    if(code!=client->code) {
        QMessageBox::warning(this, "提示", "验证码错误！");
        return;
    }
    QString msg="REGISTER:"+nick+"|"+pwd+"|"+email+"|"+code;
    client->sendNonTextMessage(msg.toUtf8());
}
