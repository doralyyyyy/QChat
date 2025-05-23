#include "server.h"
#include "main_window.h"
#include <QBuffer>

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

void Server::sleep(int ms) {
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
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
    } else {
        QMessageBox::warning(nullptr, "提示", "发送失败，请检查您的网络连接！");
    }
}

void Server::sendFile(const QString& filePath) {
    if (socket&&socket->state() == QTcpSocket::ConnectedState) {
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
    } else {
        QMessageBox::warning(nullptr, "提示", "发送失败，请检查您的网络连接！");
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
            QList<QByteArray> messages = d.split('\n');
            for(const QByteArray& m : std::as_const(messages)) {
                if(!m.trimmed().isEmpty())
                    handleTextMessage(s, m);
            }
        }
    } else {        // 文件可能较大，要多次获取
        f.data+=d;
        tryFinishFile(s);
    }
}

void Server::tryFinishFile(QTcpSocket* s) {
    FileInfo& f=fileMap[s];
    if(f.receiving && f.headerReceived && f.data.size()>=f.expectedSize) {
        QString subdir,saveName=f.name;
        if(f.name.contains("_avatar")) {
            subdir="avatars/";
        } else {
            subdir="received_";
        }
        QString base=QCoreApplication::applicationDirPath();
        QString dir=base+"/"+subdir;
        QDir().mkpath(dir);
        QString fullPath=dir+saveName;
        QFile file(fullPath);
        qDebug()<<"尝试保存文件："<<fullPath;
        if(file.open(QIODevice::WriteOnly)) {
            file.write(f.data.left(f.expectedSize));
            file.close();
            qDebug()<<"写入成功："<<fullPath;
            if(subdir=="avatars/") {
                QString nickname=f.name.left(f.name.indexOf("_avatar"));
                userDB->updateAvatarPath(nickname,fullPath);
            } else {
                QString content="FILE|"+f.name;
                QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                mainWindow->updateMessage("[" + time + "] 对方：" + content);
                dbManager->insertMessage("对方","我",content,time);
                qDebug()<<"收到普通文件："<<f.name;
            }
        } else {
            mainWindow->updateMessage("文件保存失败："+f.name);
            qDebug()<<"文件写入失败："<<fullPath;
        }
        fileMap.remove(s);
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
            socket->flush();
            return;
        }
        QString code = generateCode();
        QString nickname = userDB->getNicknameByEmail(email);
        sendVerificationCodeBack(code,nickname);
        sendVerificationCode(email, code);
    } else if (msg.startsWith("REGISTER:")) {          // 收到注册消息
        QStringList parts=msg.mid(9).split('|');
        if (parts.size()!=4) {
            socket->write("REGISTER_FAIL");
            socket->flush();
            return;
        }
        QString nick=parts[0], pwd=parts[1], email=parts[2];
        if (userDB->nicknameExists(nick) || userDB->emailExists(email)) {
            socket->write("REGISTER_DUPLICATE");
            socket->flush();
            return;
        }
        if (userDB->addUser(nick, pwd, email)) {
            socket->write("REGISTER_OK");
            socket->flush();
        } else {
            socket->write("REGISTER_FAIL");
            socket->flush();
        }
    } else if (msg.startsWith("LOGIN:")) {           // 收到登录消息
        QStringList parts=msg.mid(6).split('|');
        if (parts.size()!=2) {
            socket->write("LOGIN_FAIL");
            socket->flush();
            return;
        }
        QString id=parts[0], pwd=parts[1];
        QSqlQuery q(userDB->db);
        q.prepare("SELECT nickname, email, password FROM users WHERE nickname=? OR email=?");
        q.addBindValue(id);
        q.addBindValue(id);
        if (q.exec() && q.next()) {
            QString nickname = q.value(0).toString();
            QString email = q.value(1).toString();
            QString correctPassword = q.value(2).toString();
            if (pwd == correctPassword) {
                QString reply = "LOGIN_SUCCESS|" + nickname + "|" + email;
                socket->write(reply.toUtf8());
                socket->flush();
            } else {
                socket->write("LOGIN_FAIL");
                socket->flush();
            }
        } else {
            socket->write("LOGIN_NOTFOUND");
            socket->flush();
        }
    } else if (msg.startsWith("ADD_FRIEND|")) {
        QStringList parts = msg.split("|");
        if (parts.size() == 3) {
            QString user = parts[1].trimmed();
            QString friendName = parts[2].trimmed();
            if (userDB->nicknameExists(friendName)) {
                if (userDB->areFriends(user, friendName)) {
                    socket->write("ALREADY_FRIENDS");
                    socket->flush();
                } else if (userDB->addFriend(user, friendName)) {
                    socket->write("ADD_FRIEND_SUCCESS|"+friendName.toUtf8());
                    socket->flush();
                } else {
                    socket->write("ADD_FRIEND_FAILED");
                    socket->flush();
                }
            } else {
                socket->write("FRIEND_NOT_FOUND");
                socket->flush();
            }
        }
    } else if (msg.startsWith("REMOVE_FRIEND|")) {
        QStringList parts = msg.split("|");
        if (parts.size() == 3) {
            QString user = parts[1].trimmed();
            QString friendName = parts[2].trimmed();
            userDB->removeFriend(user, friendName);
        }
    } else if (msg.startsWith("GET_FRIEND_LIST|")) {
        QString user = msg.mid(QString("GET_FRIEND_LIST|").length()).trimmed();
        QStringList list = userDB->getFriends(user);
        QString result = "FRIEND_LIST|"+list.join(",")+'\n';
        socket->write(result.toUtf8());
        socket->flush();
    } else if (msg.startsWith("GET_AVATAR|")) {
        QString name=msg.mid(QString("GET_AVATAR|").length()).trimmed();
        QString path=userDB->getAvatar(name).trimmed();
        QByteArray data;
        QFile file(path);
        if (file.exists()) {
            qDebug() << "Avatar file exists:" << path;
            if (file.open(QIODevice::ReadOnly)) {
                data = file.readAll();
                qDebug() << "Avatar file opened and read successfully";
            } else {
                qDebug() << "Failed to open avatar file:" << path;
            }
        } else {
            qDebug() << "Avatar file does not exist:" << path;
        }
        if (!data.isEmpty()) {
            QString header = "FILE:" + name + "_avatar.png:" + QString::number(data.size()) + '\n';
            socket->write(header.toUtf8() + data);
            socket->flush();
            sleep(1000);
            socket->write("PASS");
            socket->flush();
        } else {
            qDebug() << "Failed to load avatar from path:" << path;
        }
    } else if (msg.startsWith("GET_SELF_AVATAR|")) {
        QString name=msg.mid(QString("GET_SELF_AVATAR|").length()).trimmed();
        QString path=userDB->getAvatar(name).trimmed();
        QByteArray data;
        QFile file(path);
        if (file.exists()) {
            qDebug() << "Avatar file exists:" << path;
            if (file.open(QIODevice::ReadOnly)) {
                data = file.readAll();
                qDebug() << "Avatar file opened and read successfully";
            } else {
                qDebug() << "Failed to open avatar file:" << path;
            }
        } else {
            qDebug() << "Avatar file does not exist:" << path;
        }
        if (!data.isEmpty()) {
            QString header = "FILE:" + name + "_self_avatar.png:" + QString::number(data.size()) + '\n';
            socket->write(header.toUtf8() + data + '\n');
            socket->flush();
        } else {
            qDebug() << "Failed to load avatar from path:" << path;
        }
    } else if (msg.startsWith("CHANGE_NICKNAME|")) {
        QStringList parts = msg.mid(16).split('|');
        QString oldName = parts[0].trimmed();
        QString newName = parts[1].trimmed();
        if (userDB->changeNickname(oldName, newName)) {
            socket->write("CHANGE_NICKNAME_OK");
            socket->flush();
        } else {
            socket->write("CHANGE_NICKNAME_FAIL");
            socket->flush();
        }
    } else {
        QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        mainWindow->updateMessage("[" + time + "] 对方：" + msg);
        dbManager->insertMessage("对方", "我", msg, time);
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

void Server::sendVerificationCodeBack(const QString &code,const QString &nickname) {   // 把验证码发回客户端
    if (socket&&socket->state() == QTcpSocket::ConnectedState) {
        QString msg  = "CODE:"+code+"|"+nickname+"\n";
        socket->write(msg.toUtf8());
        socket->flush();        //确保消息被立即发送
    }
}

void Server::onDisconnected() {
    QMessageBox::information(nullptr, "", "客户端断开连接");
    socket->deleteLater();
}
