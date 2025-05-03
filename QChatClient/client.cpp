#include "client.h"
#include "mainwindow.h"
#include <QDebug>

Client::Client(const QString &host, quint16 port, MainWindow *mainWindow, QObject *parent)
    : QObject(parent), serverHost(host), serverPort(port), mainWindow(mainWindow) {
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    socket->connectToHost(serverHost, serverPort);
}

Client::~Client() {
    socket->close();
}

void Client::sendMessage(const QString &message) {
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->write(message.toUtf8());
        socket->flush();        //确保消息被立即发送
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
    mainWindow->updateMessage("对方："+message);
}

void Client::onDisconnected() {
    mainWindow->updateMessage("与服务器断开连接");
}
