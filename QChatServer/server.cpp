#include "server.h"
#include "main_window.h"

Server::Server(quint16 port,MainWindow *mainWindow, QObject *parent)
    : QObject(parent), socket(nullptr),mainWindow(mainWindow) {
    server = new QTcpServer(this);
    dbManager = new DatabaseManager;
    userDB = new UserAuthDatabaseManager;
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
    QMessageBox::information(nullptr, "", "客户端已连接");
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
            handleTextMessage(s, d);
        }
    } else {        // 文件可能较大，要多次获取
        f.data+=d;
        tryFinishFile(s);
    }
}

void Server::handleTextMessage(QTcpSocket* socket, const QByteArray& data) {
    QString msg=QString::fromUtf8(data).trimmed();

    if (msg.startsWith("EMAIL:")) {                    // 收到注册时的邮件
        QString email=msg.mid(6).trimmed();
        QString code=generateCode();
        sendVerificationCodeBack(code);
        sendVerificationCode(email, code);
    } else if (msg.startsWith("EMAIL_LOGIN:")) {       // 收到邮件-验证码登录的邮件
        QString email = msg.mid(12).trimmed();
        if (!userDB->emailExists(email)) {
            socket->write("EMAIL_NOTFOUND");           // 告诉客户端邮箱不存在
            return;
        }
        QString code = generateCode();
        sendVerificationCodeBack(code);
        sendVerificationCode(email, code);
    } else if (msg.startsWith("REGISTER:")) {          // 收到注册消息
        QStringList parts=msg.mid(9).split('|');
        if (parts.size()!=4) {
            socket->write("REGISTER_FAIL");
            return;
        }

        QString nick=parts[0], pwd=parts[1], email=parts[2];

        if (userDB->nicknameExists(nick) || userDB->emailExists(email)) {
            socket->write("REGISTER_DUPLICATE");
            return;
        }
        if (userDB->addUser(nick, pwd, email)) {
            socket->write("REGISTER_OK");
        } else {
            socket->write("REGISTER_FAIL");
        }
    } else if (msg.startsWith("LOGIN:")) {           // 收到登录消息
        QStringList parts=msg.mid(6).split('|');
        if (parts.size()!=2) {
            socket->write("LOGIN_FAIL");
            return;
        }

        QString id=parts[0], pwd=parts[1];
        if (userDB->checkLogin(id, pwd)) {
            socket->write("LOGIN_OK");
        } else if (!userDB->nicknameExists(id)&&!userDB->emailExists(id)) {
            socket->write("LOGIN_NOTFOUND");
        } else {
            socket->write("LOGIN_FAIL");
        }
    } else {
        QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        mainWindow->updateMessage("[" + time + "] 对方：" + msg);
        dbManager->insertMessage("对方", "我", msg, time);
    }
}

void Server::tryFinishFile(QTcpSocket* s) {               // 先判断文件获取是否结束，若结束，进行后处理
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

QString Server::generateCode() {      // 随机生成六位数验证码
    QString code;
    for (int i=0;i<6;++i) code += QChar('0'+rand()%10);
    return code;
}

void Server::sendVerificationCode(const QString &email, const QString &code) {     // 把验证码发到邮箱
    QProcess *p=new QProcess(this);

    // 设置环境变量，确保 Python 输出支持 UTF-8
    QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
    env.insert("PYTHONIOENCODING","utf-8");
    p->setProcessEnvironment(env);

    // 设置 Python 程序与参数
    QString python="python";
    QString scriptPath=QCoreApplication::applicationDirPath()+"/../../../send_email.py";   // python程序和主函数同级

    QStringList args;
    args<<scriptPath<<email<<code;

    p->setProgram(python);
    p->setArguments(args);
    p->setProcessChannelMode(QProcess::MergedChannels); // 合并标准输出与错误输出

    connect(p,&QProcess::readyReadStandardOutput,[=](){
        QByteArray out=p->readAllStandardOutput();
        qDebug()<<"标准输出:"<<QString::fromUtf8(out);
    });

    connect(p,&QProcess::readyReadStandardError,[=](){
        QByteArray err=p->readAllStandardError();
        qDebug()<<"错误输出:"<<QString::fromUtf8(err);
    });

    p->start();

    if (!p->waitForStarted()) {
        qDebug()<<"Python 子进程启动失败";
    } else {
        qDebug()<<"验证码已尝试发送至"<<email<<"，内容为"<<code;
    }
}

void Server::sendVerificationCodeBack(const QString &code) {   // 把验证码发回客户端
    if (socket&&socket->state() == QTcpSocket::ConnectedState) {
        QString msg  = "CODE:" + code + "\n";
        socket->write(msg.toUtf8());
        socket->flush();        //确保消息被立即发送
    }
}

void Server::onDisconnected() {
    QMessageBox::information(nullptr, "", "客户端断开连接");
    socket->deleteLater();
}
