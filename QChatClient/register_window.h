#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTcpSocket>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "client.h"

class RegisterWindow : public QWidget {
    Q_OBJECT

public:
    explicit RegisterWindow(Client *client, QWidget *parent=nullptr);
    bool isEmailValid(const QString &email);

private:
    QLineEdit *emailEdit;
    QPushButton *sendCodeButton;
    QLineEdit *codeEdit;
    QPushButton *verifyButton;
    Client *client;
    QVBoxLayout *mainLayout;
    QLineEdit *nicknameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLabel *errorLabel;
    QHBoxLayout *emailLayout;
    QHBoxLayout *codeLayout;
    QPushButton *backButton;

private slots:
    void onSendCodeClicked();
    void onVerifyClicked();
    void checkPasswordsMatch();
    void onBackToLoginClicked();
};

#endif
