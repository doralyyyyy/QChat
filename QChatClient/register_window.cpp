#include "register_window.h"
#include "login_window.h"
#include <QTimer>
#include <QRegularExpression>
#include <QHBoxLayout>
#include <QMessageBox>

RegisterWindow::RegisterWindow(Client *client, QWidget *parent) : QWidget(parent), client(client) {
    mainLayout = new QVBoxLayout(this);

    nicknameEdit = new QLineEdit(this);
    nicknameEdit->setPlaceholderText("请输入昵称");

    // 密码框 + 显示按钮
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    showPasswordButton = new QPushButton("显示", this);
    showPasswordButton->setCheckable(true);
    showPasswordButton->setFixedWidth(50);
    connect(showPasswordButton, &QPushButton::toggled, this, [=](bool checked){
        passwordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        showPasswordButton->setText(checked ? "隐藏" : "显示");
    });
    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->addWidget(passwordEdit);
    passwordLayout->addWidget(showPasswordButton);

    // 确认密码框 + 显示按钮
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setPlaceholderText("请确认密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    showConfirmPasswordButton = new QPushButton("显示", this);
    showConfirmPasswordButton->setCheckable(true);
    showConfirmPasswordButton->setFixedWidth(50);
    connect(showConfirmPasswordButton, &QPushButton::toggled, this, [=](bool checked){
        confirmPasswordEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        showConfirmPasswordButton->setText(checked ? "隐藏" : "显示");
    });
    QHBoxLayout *confirmLayout = new QHBoxLayout;
    confirmLayout->addWidget(confirmPasswordEdit);
    confirmLayout->addWidget(showConfirmPasswordButton);

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
    mainLayout->addLayout(passwordLayout);
    mainLayout->addLayout(confirmLayout);
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
            border-radius: 20px;
        }

        QLineEdit {
            padding: 10px;
            border: 2px solid #ccc;
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

        QPushButton:checked {
            background-color: #aaa;
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
    // 密码强度
    passwordStrengthLabel = new QLabel(this);
    passwordStrengthLabel->setText("密码强度：");
    passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #666;");
    mainLayout->addWidget(passwordStrengthLabel);

    connect(passwordEdit, &QLineEdit::textChanged, this, [=](const QString &text){
        QString level = getPasswordStrength(text);
        QString color;
        if (level == "弱") color = "#ff4444";
        else if (level == "中") color = "#ffaa00";
        else color = "#00cc66";

        passwordStrengthLabel->setText("密码强度：" + level);
        passwordStrengthLabel->setStyleSheet("font-size: 12px; color: " + color + ";");
    });

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
            box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
        }
        QLabel {
            background: transparent;
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

QString RegisterWindow::getPasswordStrength(const QString &pwd) {
    int score = 0;

    static const QRegularExpression upperCasePattern("[A-Z]");
    static const QRegularExpression digitPattern("[0-9]");
    static const QRegularExpression specialCharPattern("[^a-zA-Z0-9]");

    if (pwd.contains(upperCasePattern)) score++;
    if (pwd.contains(digitPattern)) score++;
    if (pwd.contains(specialCharPattern)) score++;
    if (pwd.length() >= 8) score++;

    if (score <= 1) return "弱";
    else if (score == 2 || score == 3) return "中";
    else return "强";
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
    if (getPasswordStrength(pwd) == "弱") {
        showMessage("提示", "密码强度过低，请设置更复杂的密码（包含数字、大小写、符号等）");
        return;
    }

    QString msg = "REGISTER:" + nick + "|" + pwd + "|" + email + "|" + code;
    client->sendNonTextMessage(msg.toUtf8());
    client->nickname = nick;
    client->email = email;
    client->password = pwd;
}
