#include "client.h"
#include "main_window.h"

Client::Client(const QString &host, quint16 port, MainWindow *mainWindow, QObject *parent)
    : QObject(parent), serverHost(host), serverPort(port), mainWindow(mainWindow) {
    socket = new QTcpSocket(this);
    dbManager = new DatabaseManager;
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    socket->connectToHost(serverHost, serverPort);
}

Client::~Client() {
    socket->close();
}

void Client::onConnected() {
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    mainWindow->updateMessage("已连接到服务器");
}

void Client::sendMessage(const QString &message) {
    if (socket->state() == QTcpSocket::ConnectedState) {
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        socket->write(message.toUtf8());
        socket->flush();        //确保消息被立即发送
        mainWindow->updateMessage("["+time+"] 我："+message);   //显示自己发的消息
        dbManager->insertMessage("我","对方",message,time);  //数据存入数据库
    }
}

void Client::sendFile(const QString& filePath) {
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

void Client::onReadyRead() {
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

void Client::tryFinishFile(QTcpSocket* s) {
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

void Client::handleTextMessage(const QByteArray& data) {
    QString message = QString::fromUtf8(data);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    mainWindow->updateMessage("[" + time + "] 对方：" + message);
    dbManager->insertMessage("对方", "我", message, time);
}

void Client::onDisconnected() {
    mainWindow->updateMessage("与服务器断开连接");
}
