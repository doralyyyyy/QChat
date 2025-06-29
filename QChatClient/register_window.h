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
    Client *client;

    QVBoxLayout *mainLayout;
    QHBoxLayout *emailLayout;
    QHBoxLayout *codeLayout;
    QLineEdit *nicknameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLabel *errorLabel;
    QLineEdit *emailEdit;
    QPushButton *sendCodeButton;
    QLineEdit *codeEdit;
    QPushButton *verifyButton;
    QPushButton *backButton;
    QPushButton *showPasswordButton;
    QPushButton *showConfirmPasswordButton;
    QLabel *passwordStrengthLabel;


private slots:
    void onSendCodeClicked();
    void onVerifyClicked();
    void checkPasswordsMatch();
    void onBackToLoginClicked();
    void showMessage(const QString &title, const QString &text);
    QString getPasswordStrength(const QString &password);
};

#endif
