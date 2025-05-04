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
#include "database_manager.h"

class MainWindow;

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(quint16 port,MainWindow *mainWindow, QObject *parent = nullptr);
    ~Server();

    struct FileInfo {
        QString name;
        QByteArray data;
        int expectedSize=0;
        bool receiving=false;
        bool headerReceived=false;
    };
    QMap<QTcpSocket*, FileInfo> fileMap;

    void sendMessage(const QString &message);
    void sendFile(const QString &filePath);
    void handleTextMessage(const QByteArray& data);
    void tryFinishFile(QTcpSocket* s);

private slots:
    void onNewConnection();  // 新连接建立时的槽
    void onReadyRead();      // 当有数据可读时的槽
    void onDisconnected();   // 客户端断开连接时的槽

private:
    QTcpServer *server;       // 监听端口的服务器
    QTcpSocket *socket;       // 与客户端的连接
    MainWindow *mainWindow;
    DatabaseManager *dbManager;      // 操纵数据库

};

#endif
