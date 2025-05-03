#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QObject>
#include <QString>

class MainWindow;

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(quint16 port,MainWindow *mainWindow, QObject *parent = nullptr);
    ~Server();

    void sendMessage(const QString &message);

private slots:
    void onNewConnection();  // 新连接建立时的槽
    void onReadyRead();      // 当有数据可读时的槽
    void onDisconnected();   // 客户端断开连接时的槽

private:
    QTcpServer *server;       // 监听端口的服务器
    QTcpSocket *clientSocket; // 与客户端的连接
    MainWindow *mainWindow;
};

#endif
