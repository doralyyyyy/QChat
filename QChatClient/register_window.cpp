#include "register_window.h"
#include "login_window.h"
#include <QTimer>
#include <QRegularExpression>

RegisterWindow::RegisterWindow(Client *client, QWidget *parent) : QWidget(parent), client(client) {
    mainLayout = new QVBoxLayout(this);

    nicknameEdit = new QLineEdit(this);
    nicknameEdit->setPlaceholderText("请输入昵称");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);

    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("请确认密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color:red");

    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("请输入邮箱");
    sendCodeButton = new QPushButton("发送验证码", this);
    emailLayout = new QHBoxLayout;
    emailLayout->addWidget(emailEdit);
    emailLayout->addWidget(sendCodeButton);

    codeEdit = new QLineEdit(this);
    codeEdit->setPlaceholderText("请输入验证码");
    verifyButton = new QPushButton("确定", this);
    codeLayout = new QHBoxLayout;
    codeLayout->addWidget(codeEdit);
    codeLayout->addWidget(verifyButton);

    backButton = new QPushButton("返回登录", this);
    connect(backButton, &QPushButton::clicked, this, &RegisterWindow::onBackToLoginClicked);

    mainLayout->addWidget(backButton);
    mainLayout->addWidget(nicknameEdit);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(confirmPasswordEdit);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(emailLayout);
    mainLayout->addLayout(codeLayout);

    setLayout(mainLayout);
    setWindowTitle("注册");
    resize(420, 400);

    client->registerWindow = this;

    connect(sendCodeButton, &QPushButton::clicked, this, &RegisterWindow::onSendCodeClicked);
    connect(codeEdit, &QLineEdit::returnPressed, this, &RegisterWindow::onVerifyClicked);
    connect(verifyButton, &QPushButton::clicked, this, &RegisterWindow::onVerifyClicked);
    connect(passwordEdit, &QLineEdit::textChanged, this, &RegisterWindow::checkPasswordsMatch);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &RegisterWindow::checkPasswordsMatch);

    // 样式
    setStyleSheet(R"(
        QWidget {
            background-color: #f8f8f8;
            border-radius: 20px;
            padding: 20px;
            margin: 0;
        }

        QLineEdit {
            padding: 10px;
            border: 2px solid #ddd;
            border-radius: 12px;
            background: #ffffff;
            font-size: 14px;
        }

        QLineEdit:focus {
            border-color: #ff9a9e;
        }

        QPushButton {
            padding: 10px;
            border: none;
            border-radius: 12px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff9a9e, stop:1 #fad0c4);
            color: white;
            font-size: 16px;
            font-weight: bold;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fbc2eb, stop:1 #a6c1ee);
        }

        QPushButton#backButton {
            color: #007bff;
            background: transparent;
            font-size: 14px;
            text-decoration: underline;
        }

        QLabel {
            font-size: 12px;
            color: #ff4444;
            margin-top: 5px;
            margin-bottom: 5px;
        }
    )");
}

void RegisterWindow::checkPasswordsMatch() {
    if (passwordEdit->text() != confirmPasswordEdit->text()) {
        errorLabel->setText("两次输入的密码不一致");
    } else {
        errorLabel->clear();
    }
}

void RegisterWindow::onBackToLoginClicked() {
    LoginWindow *loginWindow = new LoginWindow(client);
    loginWindow->show();
    this->close();
}

bool RegisterWindow::isEmailValid(const QString &email) {
    static const QRegularExpression re("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    return re.match(email).hasMatch();
}

void RegisterWindow::showMessage(const QString &title, const QString &text) {
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(title);
    msgBox->setText(text);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setStyleSheet(R"(
        QMessageBox {
            background-color: #fff3f3;
            border-radius: 15px;
            padding: 20px;
        }
        QLabel {
            background:transparent;
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
}

void RegisterWindow::onSendCodeClicked() {
    QString email = emailEdit->text().trimmed();
    if (email.isEmpty()) {
        showMessage("提示", "请输入邮箱");
        return;
    }
    if (!isEmailValid(email)) {
        showMessage("提示", "请输入有效的邮箱地址");
        return;
    }
    QString msg = "EMAIL:" + email;
    client->nowEmail = email;
    client->sendNonTextMessage(msg);
}

void RegisterWindow::onVerifyClicked() {
    QString nick = nicknameEdit->text().trimmed();
    QString pwd = passwordEdit->text();
    QString pwd2 = confirmPasswordEdit->text();
    QString email = emailEdit->text().trimmed();
    QString code = codeEdit->text().trimmed();

    if (nick.isEmpty() || pwd.isEmpty() || pwd2.isEmpty() || email.isEmpty() || code.isEmpty()) {
        showMessage("错误", "请填写完整信息");
        return;
    }
    if (pwd != pwd2) {
        showMessage("错误", "两次密码不一致");
        return;
    }
    if (code != client->code) {
        showMessage("提示", "验证码错误");
        return;
    }

    QString msg = "REGISTER:" + nick + "|" + pwd + "|" + email + "|" + code;
    client->sendNonTextMessage(msg.toUtf8());
    client->nickname = nick;
    client->email = email;
    client->password = pwd;
}
