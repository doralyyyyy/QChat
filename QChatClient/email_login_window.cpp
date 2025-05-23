#include "email_login_window.h"
#include "login_window.h"

EmailLoginWindow::EmailLoginWindow(Client *client, QWidget *parent) : QWidget(parent), client(client) {
    emailEdit = new QLineEdit(this);
    sendCodeButton = new QPushButton("发送验证码", this);
    codeEdit = new QLineEdit(this);
    verifyButton = new QPushButton("确定", this);

    emailEdit->setPlaceholderText("请输入邮箱");
    codeEdit->setPlaceholderText("请输入验证码");

    backButton = new QPushButton("返回登录", this);
    connect(backButton, &QPushButton::clicked, this, &EmailLoginWindow::onBackToLoginClicked);

    // 设置布局
    QHBoxLayout *emailLayout = new QHBoxLayout;
    emailLayout->addWidget(emailEdit);
    emailLayout->addWidget(sendCodeButton);

    QHBoxLayout *codeLayout = new QHBoxLayout;
    codeLayout->addWidget(codeEdit);
    codeLayout->addWidget(verifyButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(backButton);
    mainLayout->addLayout(emailLayout);
    mainLayout->addLayout(codeLayout);

    setLayout(mainLayout);
    setWindowTitle("邮箱验证码登录");
    resize(300, 150);

    client->emailLoginWindow = this;

    connect(sendCodeButton, &QPushButton::clicked, this, &EmailLoginWindow::onSendCodeClicked);
    connect(codeEdit, &QLineEdit::returnPressed, this, &EmailLoginWindow::onVerifyClicked);
    connect(verifyButton, &QPushButton::clicked, this, &EmailLoginWindow::onVerifyClicked);

    // 设置界面样式
    setStyleSheet(R"(
        QWidget {
            border: 3px solid #dcdcdc;
            border-radius: 20px;
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

        QHBoxLayout {
            margin: 5px 0;
        }
    )");
}

bool EmailLoginWindow::isEmailValid(const QString &email) {     // 判断邮件是否合法
    static const QRegularExpression re("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    return re.match(email).hasMatch();
}

void EmailLoginWindow::onBackToLoginClicked() {             // 返回登录界面
    LoginWindow *loginWindow = new LoginWindow(client);
    loginWindow->show();
    this->close();
}
void EmailLoginWindow::onSendCodeClicked() {
    QString email = emailEdit->text().trimmed();
    if (email.isEmpty()) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("提示");
        msgBox->setText("请输入邮箱");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #ffe6e6;
                border-radius: 15px;
                padding: 15px;
            }
            QLabel {
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff7f7f;
                border: none;
                border-radius: 10px;
                color: white;
                font-size: 14px;
                padding: 8px;
            }
            QPushButton:hover {
                background-color: #ff4c4c;
            }
        )");

        msgBox->exec();
        return;
    }
    if (!isEmailValid(email)) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("错误");
        msgBox->setText("请输入有效的邮箱地址");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #ffe6e6;
                border-radius: 15px;
                padding: 15px;
            }
            QLabel {
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff7f7f;
                border: none;
                border-radius: 10px;
                color: white;
                font-size: 14px;
                padding: 8px;
            }
            QPushButton:hover {
                background-color: #ff4c4c;
            }
        )");

        msgBox->exec();
        return;
    }
    QString msg = "EMAIL_LOGIN:" + email;
    client->nowEmail = email;
    client->sendNonTextMessage(msg);
    client->email=email;
}

void EmailLoginWindow::onVerifyClicked() {
    QString code = codeEdit->text().trimmed();
    if (code.isEmpty()) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("提示");
        msgBox->setText("请输入验证码");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #ffe6e6;
                border-radius: 15px;
                padding: 15px;
            }
            QLabel {
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff7f7f;
                border: none;
                border-radius: 10px;
                color: white;
                font-size: 14px;
                padding: 8px;
            }
            QPushButton:hover {
                background-color: #ff4c4c;
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
                background-color: #ffe6e6;
                border-radius: 15px;
                padding: 15px;
            }
            QLabel {
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff7f7f;
                border: none;
                border-radius: 10px;
                color: white;
                font-size: 14px;
                padding: 8px;
            }
            QPushButton:hover {
                background-color: #ff4c4c;
            }
        )");

        msgBox->exec();
        return;
    } else {
        client->emailLoginSuccess();
    }
}
