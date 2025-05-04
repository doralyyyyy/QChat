#include "server.h"
#include "main_window.h"

Server::Server(quint16 port,MainWindow *mainWindow, QObject *parent)
    : QObject(parent), socket(nullptr),mainWindow(mainWindow) {
    server = new QTcpServer(this);
    dbManager = new DatabaseManager;
    if (!server->listen(QHostAddress::Any, port)) {
        mainWindow->updateMessage("服务器无法启动");
    }
    else {
        mainWindow->updateMessage("服务器已启动并监听端口："+QString::number(port));
    };
    connect(server, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

Server::~Server() {
    if (socket) {
        socket->close();
    }
    server->close();
}

void Server::onNewConnection() {
    socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
    mainWindow->updateMessage("客户端已连接");
}

void Server::sendMessage(const QString &message) {
    if (socket&&socket->state() == QTcpSocket::ConnectedState) {
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        socket->write(message.toUtf8());
        socket->flush();        //确保消息被立即发送
        mainWindow->updateMessage("["+time+"] 我："+message);   //显示自己发的消息
        dbManager->insertMessage("我","对方",message,time);  //数据存入数据库
    }
}

void Server::sendFile(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        QString filename = QFileInfo(file).fileName();
        QString savedPath = QCoreApplication::applicationDirPath() + "/sent_" + filename;

        socket->write(("FILE:" + filename + ":" + QString::number(fileData.size()) + "\n").toUtf8());
        socket->write(fileData);
        socket->flush();
        file.close();

        // 自己保存一份文件
        QFile out(savedPath);
        if (out.open(QIODevice::WriteOnly)) {
            out.write(fileData);
            out.close();

            QString content="FILE|"+filename;
            QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            mainWindow->updateMessage("[" + time + "] 我：" + content);
            dbManager->insertMessage("我","对方",content,time);     // 标记为文件后存入数据库
        } else {
            mainWindow->updateMessage("文件保存失败：" + filename);
        }
    }
}

void Server::onReadyRead() {
    QTcpSocket* s=qobject_cast<QTcpSocket*>(sender());
    QByteArray d=s->readAll();
    FileInfo& f=fileMap[s];

    if (!f.receiving) {
        if (d.startsWith("FILE:")) {
            int idx=d.indexOf('\n');
            if (idx==-1) return;

            QList<QByteArray> parts=d.left(idx).mid(5).split(':');
            if (parts.size()!=2) return;

            f.name=QString::fromUtf8(parts[0]);
            f.expectedSize=parts[1].toInt();
            f.data=d.mid(idx+1);
            f.receiving=true;
            f.headerReceived=true;
            tryFinishFile(s);
        } else {
            handleTextMessage(d);
        }
    } else {
        f.data+=d;
        tryFinishFile(s);
    }
}

void Server::tryFinishFile(QTcpSocket* s) {
    FileInfo& f=fileMap[s];
    if (f.receiving && f.headerReceived && f.data.size() >= f.expectedSize) {
        QFile file(QCoreApplication::applicationDirPath() + "/received_" + f.name);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(f.data.left(f.expectedSize));
            file.close();

            QString content="FILE|"+f.name;
            QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            mainWindow->updateMessage("[" + time + "] 对方：" + content);
            dbManager->insertMessage("对方","我",content,time);     // 标记为文件后存入数据库
        } else {
            mainWindow->updateMessage("文件保存失败：" + f.name);
        }
        fileMap.remove(s);
    }
}

void Server::handleTextMessage(const QByteArray& data) {
    QString message = QString::fromUtf8(data);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    mainWindow->updateMessage("[" + time + "] 对方：" + message);
    dbManager->insertMessage("对方", "我", message, time);
}

void Server::onDisconnected() {
    mainWindow->updateMessage("客户端断开连接");
    socket->deleteLater();
}
