#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include "database_manager.h"

class MainWindow;
class RegisterWindow;
class LoginWindow;
class EmailLoginWindow;

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(const QString &host, quint16 port, LoginWindow *loginWindow = nullptr, QObject *parent = nullptr);
    ~Client();

    struct FileInfo {
        QString name;
        QByteArray data;
        int expectedSize=0;
        bool receiving=false;
        bool headerReceived=false;
    };
    QMap<QTcpSocket*, FileInfo> fileMap;
    QString code;          // 验证码
    QString nowEmail;      // 邮箱
    MainWindow *mainWindow;
    RegisterWindow *registerWindow;
    EmailLoginWindow *emailLoginWindow;

    void sendVerificationEmail(const QString& email);
    void sendVerificationCode(const QString& code);
    void registerSuccess();
    void loginSuccess();
    void emailLoginSuccess();
    void sendMessage(const QString &message);
    void sendNonTextMessage(const QString &message);
    void sendFile(const QString &filePath);
    void handleTextMessage(const QByteArray& data);
    void tryFinishFile(QTcpSocket* s);
    void showMessage(const QString &message);    // 弹出提示框

private slots:
    void onConnected();   // 客户端连接成功时的槽
    void onReadyRead();   // 客户端接收到消息时的槽
    void onDisconnected(); // 客户端断开连接时的槽

private:
    QTcpSocket *socket;  // 与服务端的连接
    QString serverHost;  // 服务端地址
    quint16 serverPort;  // 服务端端口
    LoginWindow *loginWindow;
    DatabaseManager *dbManager;      // 操纵数据库
};

#endif
