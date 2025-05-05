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
    explicit RegisterWindow(QWidget *parent=nullptr);
    bool isEmailValid(const QString &email);

private:
    QLineEdit *emailEdit;
    QPushButton *sendCodeButton;
    QLineEdit *codeEdit;
    QPushButton *verifyButton;
    Client *client;

private slots:
    void onSendCodeClicked();
    void onVerifyClicked();
};

#endif
