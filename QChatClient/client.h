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
#include <QSoundEffect>
#include "database_manager.h"

class MainWindow;
class RegisterWindow;
class LoginWindow;
class EmailLoginWindow;
class ChatPage;
class ChatPage2;
class FriendListPage;
class MatchPage;
class SettingPage;

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
    LoginWindow *loginWindow;
    ChatPage *chatPage;
    ChatPage2 *chatPage2;
    FriendListPage *friendListPage;
    MatchPage *matchPage;
    SettingPage *settingPage;
    QStringList onlineList;
    QString nickname;
    QString newNickname;
    QString email;
    QString password;
    QPixmap avatar;
    QString interest;
    QTcpSocket *socket;  // 与服务端的连接
    DatabaseManager *dbManager;      // 操纵数据库

    void registerSuccess();
    void loginSuccess();
    void emailLoginSuccess();
    void sendMessage(const QString &message);
    void sendNonTextMessage(const QString &message);
    void sendFile(const QString &filePath);
    void handleTextMessage(const QByteArray& data);
    void tryFinishFile(QTcpSocket* s);
    bool ifconnected();
    void showStyledMessageBox(const QString& title, const QString& text, QMessageBox::Icon icon);
    QString getNickname();
    QString getEmail();
    QString requestAvatar(const QString &nickname);


private slots:
    void onConnected();   // 客户端连接成功时的槽
    void onReadyRead();   // 客户端接收到消息时的槽
    void onDisconnected(); // 客户端断开连接时的槽

private:
    QString serverHost;  // 服务端地址
    quint16 serverPort;  // 服务端端口
};

#endif
