#ifndef EMAILLOGINWINDOW_H
#define EMAILLOGINWINDOW_H

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
#include <client.h>

class EmailLoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit EmailLoginWindow(Client *client, QWidget *parent=nullptr);
    bool isEmailValid(const QString &email);

private:
    QPushButton *backButton;
    QLineEdit *emailEdit;
    QPushButton *sendCodeButton;
    QLineEdit *codeEdit;
    QPushButton *verifyButton;
    Client *client;
    QVBoxLayout *mainLayout;
    QLineEdit *nicknameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QHBoxLayout *emailLayout;
    QHBoxLayout *codeLayout;

private slots:
    void onSendCodeClicked();
    void onVerifyClicked();
    void onBackToLoginClicked();
};

#endif
