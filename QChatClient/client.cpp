#include "client.h"
#include "mainwindow.h"
#include <QDebug>
#include <QDateTime>

Client::Client(const QString &host, quint16 port, MainWindow *mainWindow, QObject *parent)
    : QObject(parent), serverHost(host), serverPort(port), mainWindow(mainWindow) {
    socket = new QTcpSocket(this);
    db = new DatabaseManager;
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    socket->connectToHost(serverHost, serverPort);
}

Client::~Client() {
    socket->close();
}

void Client::sendMessage(const QString &message) {
    if (socket->state() == QTcpSocket::ConnectedState) {
        QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        socket->write(message.toUtf8());
        socket->flush();        //确保消息被立即发送
        db->insertMessage("client","server",message,now);  //数据存入数据库
    }
}

void Client::onConnected() {
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    mainWindow->updateMessage("已连接到服务器");
}

void Client::onReadyRead() {
    QByteArray data = socket->readAll();
    QString message =QString::fromUtf8(data);
    QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    mainWindow->updateMessage("对方："+message);
    db->insertMessage("server","client",message,now);  //数据存入数据库
}

void Client::onDisconnected() {
    mainWindow->updateMessage("与服务器断开连接");
}
