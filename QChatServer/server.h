#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QtGlobal>
#include <QProcess>
#include <QMessageBox>
#include "database_manager.h"
#include "user_auth_database_manager.h"

class MainWindow;
class ChatPage;
class FriendListPage;

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(quint16 port, QObject *parent = nullptr);
    ~Server();

    struct FileInfo {
        QString name;
        QByteArray data;
        int expectedSize=0;
        bool receiving=false;
        bool headerReceived=false;
    };
    QMap<QTcpSocket*, FileInfo> fileMap;
    MainWindow *mainWindow;
    ChatPage *chatPage;
    FriendListPage *friendListPage;
    QString nowClient;
    QString nowEmail;
    bool nowChange;
    QTcpSocket *socket;       // 与客户端的连接
    UserAuthDatabaseManager *userDB;   // 用户数据库

    void sendVerificationCode(const QString &email, const QString &code);
    void sendVerificationCodeBack(const QString &code,const QString &nickname="",const QString &password="");
    void sendMessage(const QString &message);
    void sendFile(const QString &filePath);
    void handleTextMessage(QTcpSocket *socket, const QByteArray& data);
    void tryFinishFile(QTcpSocket* s);
    void sleep(int ms);
    QString generateCode();
    void sendFeedback(const QString& feedback);

private slots:
    void onNewConnection();  // 新连接建立时的槽
    void onReadyRead();      // 当有数据可读时的槽
    void onDisconnected();   // 客户端断开连接时的槽

private:
    QTcpServer *server;       // 监听端口的服务器
    DatabaseManager *dbManager;      // 聊天记录数据库

};

#endif
