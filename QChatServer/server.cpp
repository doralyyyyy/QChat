#include "server.h"
#include "mainwindow.h"
#include <QDebug>

Server::Server(quint16 port,MainWindow *mainWindow, QObject *parent)
    : QObject(parent), clientSocket(nullptr),mainWindow(mainWindow) {
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, port)) {
        mainWindow->updateMessage("服务器无法启动");
    }
    else {
        mainWindow->updateMessage("服务器已启动并监听端口："+QString::number(port));
    }
    connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

Server::~Server() {
    if (clientSocket) {
        clientSocket->close();
    }
    server->close();
}

void Server::sendMessage(const QString &message) {
    if (clientSocket&&clientSocket->state() == QTcpSocket::ConnectedState) {
        clientSocket->write(message.toUtf8());
        clientSocket->flush();        //确保消息被立即发送
    }
}

void Server::onNewConnection() {
    clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
    mainWindow->updateMessage("客户端已连接");
}

void Server::onReadyRead() {
    clientSocket=qobject_cast<QTcpSocket*>(sender());    //找到发消息的那个客户端
    QByteArray data = clientSocket->readAll();
    QString message = QString::fromUtf8(data);
    mainWindow->updateMessage("对方："+message);
}

void Server::onDisconnected() {
    mainWindow->updateMessage("客户端断开连接");
    clientSocket=qobject_cast<QTcpSocket*>(sender());
    clientSocket->deleteLater();
}
