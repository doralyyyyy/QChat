#include "register_window.h"
#include "login_window.h"

RegisterWindow::RegisterWindow(Client *client, QWidget *parent) : QWidget(parent), client(client){
    mainLayout = new QVBoxLayout(this);

    // 输入框和控件
    nicknameEdit = new QLineEdit(this);
    nicknameEdit->setPlaceholderText("请输入昵称");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);

    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("请确认密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    // 错误提示标签
    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color:red");

    // 邮箱 + 按钮
    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("请输入邮箱");
    sendCodeButton = new QPushButton("发送验证码", this);
    emailLayout = new QHBoxLayout;
    emailLayout->addWidget(emailEdit);
    emailLayout->addWidget(sendCodeButton);

    // 验证码 + 按钮
    codeEdit = new QLineEdit(this);
    codeEdit->setPlaceholderText("请输入验证码");
    verifyButton = new QPushButton("确定", this);
    codeLayout = new QHBoxLayout;
    codeLayout->addWidget(codeEdit);
    codeLayout->addWidget(verifyButton);

    //返回按钮
    backButton = new QPushButton("返回登录", this);
    connect(backButton, &QPushButton::clicked, this, &RegisterWindow::onBackToLoginClicked);

    // 加入布局
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

    // 设置样式
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

    QLabel {
        font-size: 12px;
        color: #ff4444;
        margin-top: 5px;
        margin-bottom: 5px;
    }

    QHBoxLayout {
        margin: 5px 0;
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
    LoginWindow *loginWindow = new LoginWindow(client);  // 返回登录界面
    loginWindow->show();
    this->close();
}

bool RegisterWindow::isEmailValid(const QString &email) {
    static const QRegularExpression re("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    return re.match(email).hasMatch();
}

void RegisterWindow::onSendCodeClicked() {
    QString email = emailEdit->text().trimmed();
    if (email.isEmpty()) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("提示");
        msgBox->setText("请输入邮箱");
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
    if (!isEmailValid(email)) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("提示");
        msgBox->setText("请输入有效的邮箱地址");
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
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("错误");
        msgBox->setText("请填写完整信息");
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
    if (pwd != pwd2) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("错误");
        msgBox->setText("两次密码不一致");
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
    if (code.isEmpty()) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("提示");
        msgBox->setText("请输入验证码");
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
    if (code != client->code) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("提示");
        msgBox->setText("验证码错误");
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

    QString msg = "REGISTER:" + nick + "|" + pwd + "|" + email + "|" + code;
    client->sendNonTextMessage(msg.toUtf8());
    client->nickname=nick;
    client->email=email;
}
