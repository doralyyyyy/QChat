#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QByteArray>
#include <QObject>
#include <QString>

class MainWindow;

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(const QString &host, quint16 port, MainWindow *mainWindow, QObject *parent = nullptr);
    ~Client();

    void sendMessage(const QString &message);

private slots:
    void onConnected();   // 客户端连接成功时的槽
    void onReadyRead();   // 客户端接收到消息时的槽
    void onDisconnected(); // 客户端断开连接时的槽

private:
    QTcpSocket *socket;  // 与服务端的连接
    QString serverHost;  // 服务端地址
    quint16 serverPort;  // 服务端端口
    MainWindow *mainWindow;
};

#endif
