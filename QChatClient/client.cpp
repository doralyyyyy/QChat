#include "client.h"
#include "main_window.h"
#include "register_window.h"
#include "login_window.h"
#include "email_login_window.h"

Client::Client(const QString &host, quint16 port, LoginWindow *loginWindow, QObject *parent)
    : QObject(parent), mainWindow(nullptr), serverHost(host), serverPort(port), loginWindow(loginWindow) {
    socket = new QTcpSocket(this);
    dbManager = new DatabaseManager;
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    socket->connectToHost(serverHost, serverPort);
}

Client::~Client() {
    socket->close();
}

void Client::showMessage(const QString &s) {
    QMessageBox msgBox;
    msgBox.setText(s);
    msgBox.exec();
}

void Client::onConnected() {
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
}

void Client::sendVerificationEmail(const QString& email) {
    if (socket && socket->isOpen()) {
        QString msg = "EMAIL:" + email + "\n";
        socket->write(msg.toUtf8());
        socket->flush();
    }
}

void Client::registerSuccess(){     // 注册通过验证
    registerWindow->close();
    mainWindow = new MainWindow(this);
    mainWindow->show();
}

void Client::loginSuccess(){       // 账号密码登录通过验证
    loginWindow->close();
    mainWindow = new MainWindow(this);
    mainWindow->show();
}

void Client::emailLoginSuccess(){   // 邮箱验证码登录通过验证
    emailLoginWindow->close();
    mainWindow = new MainWindow(this);
    mainWindow->show();
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

void Client::sendNonTextMessage(const QString &message) {    // 发送无需被显示的消息
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->write(message.toUtf8());
        socket->flush();        //确保消息被立即发送
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

        QFile out(savedPath);      // 自己保存一份文件
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
    QString msg = QString::fromUtf8(data).trimmed();

    if (msg=="REGISTER_OK") {                              // 收到注册反馈时
        registerSuccess();
    } else if (msg=="REGISTER_FAIL") {
        QMessageBox::warning(nullptr,"注册失败","注册失败，请重试");
    } else if (msg=="REGISTER_DUPLICATE") {
        QMessageBox::warning(nullptr,"注册失败","昵称或邮箱已被使用");
    } else if (msg=="LOGIN_OK") {                          // 收到登录反馈时
        loginSuccess();
    } else if (msg=="LOGIN_NOTFOUND") {
        QMessageBox::warning(nullptr,"登录失败","该昵称/邮箱不存在");
    } else if (msg=="LOGIN_FAIL") {
        QMessageBox::warning(nullptr,"登录失败","密码错误");
    } else if (msg.startsWith("CODE:")) {                    // 收到的是验证码时
        QMessageBox::information(nullptr, "提示", "验证码已发送至："+nowEmail);
        code = msg.mid(5).trimmed();
    } else if (msg == "EMAIL_NOTFOUND") {                        // 收到邮箱验证码登录反馈时
        QMessageBox::warning(nullptr, "错误", "该邮箱尚未注册！");
    } else {
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        mainWindow->updateMessage("[" + time + "] 对方：" + msg);
        dbManager->insertMessage("对方", "我", msg, time);
    }
}

void Client::onDisconnected() {
    showMessage("与服务器断开连接");
}
