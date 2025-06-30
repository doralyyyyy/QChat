#include "server.h"
#include "chat_page.h"
#include "friend_list_page.h"
#include "qtimer.h"
#include <QBuffer>

Server::Server(quint16 port, QObject *parent)
    : QObject(parent), socket(nullptr) {
    server = new QTcpServer(this);
    userDB = new UserAuthDatabaseManager;
    dbManager = new DatabaseManager;
    nowEmail="";

    if (!server->listen(QHostAddress::Any, port)) {
        qDebug()<<"服务器无法启动";
    }
    else {
        qDebug()<<"服务器已启动并监听端口："+QString::number(port);
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
    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setWindowTitle("");
    msgBox->setText("客户端已连接");
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setStandardButtons(QMessageBox::Ok);

    msgBox->setStyleSheet(R"(
        QMessageBox {
            background-color: #fff3f3;
            border-radius: 15px;
            padding: 20px;
            box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
        }
        QLabel {
            background: transparent;
            font-size: 14px;
            color: #ff4444;
        }
        QPushButton {
            background-color: #ff9a9e;
            border: none;
            border-radius: 10px;
            padding: 8px;
            font-weight: bold;
            color: white;
        }
        QPushButton:hover {
            background-color: #fbc2eb;
        }
    )");

    // msgBox->exec();
}


void Server::sendMessage(const QString &message) {
    if (socket&&socket->state() == QTcpSocket::ConnectedState) {
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        socket->write(message.toUtf8());
        socket->flush();        //确保消息被立即发送
        chatPage->updateMessage("["+time+"] 我："+message);   //显示自己发的消息
        dbManager->insertMessage(nowEmail,"我","对方",message,time);  //数据存入数据库
    } else {
        QMessageBox *msgBox = new QMessageBox(nullptr);
        msgBox->setWindowTitle("提示");
        msgBox->setText("发送失败，请检查您的网络连接");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #fff3f3;
                border-radius: 15px;
                padding: 20px;
                box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
            }
            QLabel {
                background: transparent;
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff9a9e;
                border: none;
                border-radius: 10px;
                padding: 8px;
                font-weight: bold;
                color: white;
            }
            QPushButton:hover {
                background-color: #fbc2eb;
            }
        )");

        msgBox->exec();
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
                chatPage->updateMessage("[" + time + "] 我：" + content);
                dbManager->insertMessage(nowEmail,"我","对方",content,time);     // 标记为文件后存入数据库
            } else {
                chatPage->updateMessage("文件保存失败：" + filename);
            }
        }
    } else {
        QMessageBox *msgBox = new QMessageBox(nullptr);
        msgBox->setWindowTitle("提示");
        msgBox->setText("发送失败，请检查您的网络连接");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setStandardButtons(QMessageBox::Ok);

        msgBox->setStyleSheet(R"(
            QMessageBox {
                background-color: #fff3f3;
                border-radius: 15px;
                padding: 20px;
                box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
            }
            QLabel {
                background: transparent;
                font-size: 14px;
                color: #ff4444;
            }
            QPushButton {
                background-color: #ff9a9e;
                border: none;
                border-radius: 10px;
                padding: 8px;
                font-weight: bold;
                color: white;
            }
            QPushButton:hover {
                background-color: #fbc2eb;
            }
        )");

        msgBox->exec();
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
            if(subdir=="avatars/") {    // 发来更换头像请求
                QString nickname=f.name.left(f.name.indexOf("_avatar"));
                userDB->updateAvatarPath(nickname,fullPath);
                friendListPage->updateListDisplay();
            } else {
                QString content="FILE|"+f.name;
                QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                chatPage->updateMessage("[" + time + "] 对方：" + content);
                dbManager->insertMessage(nowEmail,"对方","我",content,time);
                qDebug()<<"收到普通文件："<<f.name;
            }
        } else {
            chatPage->updateMessage("文件保存失败："+f.name);
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
        QString password = userDB->getPasswordByEmail(email);
        nowClient=nickname;
        if (nowEmail!=email) nowChange = true;
        nowEmail=email;
        dbManager->init(nowEmail);
        friendListPage->updateListDisplay();
        sendVerificationCodeBack(code,nickname,password);
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
            nowClient=nick;
            if (nowEmail!=email) nowChange = true;
            nowEmail=email;
            dbManager->init(nowEmail);
            friendListPage->updateListDisplay();
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
                nowClient=nickname;
                if (nowEmail!=email) nowChange = true;
                nowEmail=email;
                dbManager->init(nowEmail);
                friendListPage->updateListDisplay();
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
    } else if (msg.startsWith("GET_SELF_AVATAR_AND_INTERESTS|")) {
        QString name=msg.mid(QString("GET_SELF_AVATAR_AND_INTERESTS|").length()).trimmed();
        QString path=userDB->getAvatar(name).trimmed();
        QByteArray data;
        QFile file(path);
        QString interest=userDB->getInterest(name).trimmed();
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
            QString header = "FILE:"+interest+"|"+name+"_self_avatar.png:"+QString::number(data.size()) + '\n';
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
            nowClient=newName;
            friendListPage->updateListDisplay();
        } else {
            socket->write("CHANGE_NICKNAME_FAIL");
            socket->flush();
        }
    } else if (msg.startsWith("CHANGE_PASSWORD|")) {
        QStringList parts = msg.mid(16).split('|');
        QString nickname = parts[0].trimmed();
        QString newPassword = parts[1].trimmed();
        userDB->changePasssword(nickname, newPassword);
    } else if (msg.startsWith("CHANGE_INTEREST|")) {
        QString newInterest = msg.section('|', 1);
        QString nickname = nowClient;

        if (!nickname.isEmpty()) {
            if (userDB->changeInterest(nickname, newInterest)) {
                qDebug() << nickname << "兴趣更新为：" << newInterest;
            } else {
                qDebug() << "更新兴趣失败：" << nickname;
            }
        }
        return;
    } else if (msg.startsWith("MATCH_REQUEST|")) {
        QString nickname=msg.section('|',1);
        qDebug()<<"Match:"+nickname;
        QString matchedUser,matchedInteret;
        if(userDB->findMatchingUser(nickname,matchedUser,matchedInteret)) {
            QString msg="MATCH_RESULT|"+matchedUser+"|"+matchedInteret+"\n";
            socket->write(msg.toUtf8());
            socket->flush();
        } else {
            socket->write("MATCH_FAILED");
            socket->flush();
        }
    } else if (msg.startsWith("FEEDBACK|")) {
        QString feedback=msg.section('|',1);
        sendFeedback(feedback);
    } else {
        QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        chatPage->updateMessage("[" + time + "] 对方：" + msg);
        dbManager->insertMessage(nowEmail,"对方", "我", msg, time);
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
    QString exePath=QCoreApplication::applicationDirPath()+"/../../../send_email.exe";   // python程序和主函数同级

    QStringList args;
    args<<email<<code;

    p->setProgram(exePath);
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

void Server::sendVerificationCodeBack(const QString &code,const QString &nickname,const QString &password) {   // 把验证码、昵称、密码发回客户端
    if (socket&&socket->state() == QTcpSocket::ConnectedState) {
        QString msg  = "CODE:"+code+"|"+nickname+"|"+password+"\n";
        socket->write(msg.toUtf8());
        socket->flush();        //确保消息被立即发送
    }
}

void Server::sendFeedback(const QString &feedback) {
    QProcess *p=new QProcess(this);

    // 设置环境变量，确保 Python 输出支持 UTF-8
    QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
    env.insert("PYTHONIOENCODING","utf-8");
    p->setProcessEnvironment(env);

    // 设置 Python 程序与参数
    QString exePath=QCoreApplication::applicationDirPath()+"/../../../send_feedback.exe";   // python程序和主函数同级

    QStringList args;
    args<<feedback;

    p->setProgram(exePath);
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
}

void Server::onDisconnected() {
    nowClient="Server";
    friendListPage->updateListDisplay();

    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setWindowTitle("");
    msgBox->setText("客户端断开连接");
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setStandardButtons(QMessageBox::Ok);

    msgBox->setStyleSheet(R"(
        QMessageBox {
            background-color: #fff3f3;
            border-radius: 15px;
            padding: 20px;
            box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
        }
        QLabel {
            background: transparent;
            font-size: 14px;
            color: #ff4444;
        }
        QPushButton {
            background-color: #ff9a9e;
            border: none;
            border-radius: 10px;
            padding: 8px;
            font-weight: bold;
            color: white;
        }
        QPushButton:hover {
            background-color: #fbc2eb;
        }
    )");

    // msgBox->exec();
    socket->deleteLater();
}
