#include "client.h"
#include "main_window.h"
#include "register_window.h"
#include "login_window.h"
#include "email_login_window.h"

Client::Client(const QString &host, quint16 port, LoginWindow *loginWindow, QObject *parent)
    : QObject(parent), mainWindow(nullptr), loginWindow(loginWindow), serverHost(host), serverPort(port) {
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    socket->connectToHost(serverHost, serverPort);
}

Client::~Client() {
    socket->close();
}

void Client::onConnected() {
    onlineList<<"Server";
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
}

bool Client::ifconnected(){
    return socket->state() == QTcpSocket::ConnectedState;
}

void Client::registerSuccess(){     // 注册通过验证
    registerWindow->close();
    dbManager = new DatabaseManager(email);
    mainWindow = new MainWindow(this);
    mainWindow->show();
}

void Client::loginSuccess(){       // 账号密码登录通过验证
    loginWindow->close();
    dbManager = new DatabaseManager(email);
    mainWindow = new MainWindow(this);
    mainWindow->show();
}

void Client::emailLoginSuccess(){   // 邮箱验证码登录通过验证
    emailLoginWindow->close();
    dbManager = new DatabaseManager(email);
    mainWindow = new MainWindow(this);
    mainWindow->show();
}

void Client::sendMessage(const QString &message) {
    if (socket->state() == QTcpSocket::ConnectedState) {
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        socket->write(message.toUtf8());
        socket->flush();        //确保消息被立即发送
        chatPage->updateMessage("["+time+"] 我："+message);   //显示自己发的消息
        dbManager->insertMessage("我","对方",message,time);  //数据存入数据库
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

void Client::sendNonTextMessage(const QString &message) {    // 发送无需被显示的消息
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->write(message.toUtf8());
        socket->flush();        //确保消息被立即发送
    } else {
        QMessageBox *msgBox = new QMessageBox(nullptr);
        msgBox->setWindowTitle("提示");
        msgBox->setText("通信失败，请检查您的网络连接");
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

void Client::sendFile(const QString& filePath) {
    if (socket->state() == QTcpSocket::ConnectedState) {
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
                chatPage->updateMessage("[" + time + "] 我：" + content);
                dbManager->insertMessage("我","对方",content,time);     // 标记为文件后存入数据库
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
            QList<QByteArray> messages = d.split('\n');
            for(const QByteArray& m : std::as_const(messages)) {
                if(!m.trimmed().isEmpty())
                    handleTextMessage(m);
            }
        }
    } else {
        f.data+=d;
        tryFinishFile(s);
    }
}

void Client::tryFinishFile(QTcpSocket* s) {
    FileInfo& f=fileMap[s];
    if(f.receiving && f.headerReceived && f.data.size()>=f.expectedSize) {
        QString subdir,saveName=f.name;
        if(f.name.contains("_avatar")) {
            subdir="avatars/";
        } else {
            subdir="received_";
        }
        int sepIndex=saveName.indexOf("|");
        if(sepIndex!=-1){
            interest=saveName.left(sepIndex);
            saveName=saveName.mid(sepIndex+1);
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
                if(f.name.contains("_self")) {
                    QPixmap pix(fullPath);
                    if(!pix.isNull()) {
                        avatar=pix;
                        qDebug()<<"自己头像加载成功："<<fullPath;
                    } else {
                        qDebug()<<"自己头像加载失败："<<fullPath;
                    }
                    settingPage->updateUserInfo();
                } else {
                    QString nickname=f.name.left(f.name.indexOf("_avatar"));
                    friendListPage->updateAvatar(nickname,fullPath);
                    qDebug()<<"收到别人头像："<<nickname<<fullPath;
                }
            } else {
                QString content="FILE|"+f.name;
                QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                chatPage->updateMessage("[" + time + "] 对方：" + content);
                dbManager->insertMessage("对方","我",content,time);
                qDebug()<<"收到普通文件："<<f.name;
            }
        } else {
            chatPage->updateMessage("文件保存失败："+f.name);
            qDebug()<<"文件写入失败："<<fullPath;
        }
        fileMap.remove(s);
    }
}

void Client::showStyledMessageBox(const QString& title, const QString& text, QMessageBox::Icon icon) {
    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setWindowTitle(title);
    msgBox->setText(text);
    msgBox->setIcon(icon);
    msgBox->setStandardButtons(QMessageBox::Ok);

    msgBox->setStyleSheet(R"(
        QMessageBox {
            background-color: #fff3f3;
            border-radius: 15px;
            padding: 20px;
            box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
        }
        QLabel {
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
    delete msgBox;
}


void Client::handleTextMessage(const QByteArray& data) {
    QString msg = QString::fromUtf8(data).trimmed();
    // qDebug()<<msg;
    if (msg=="REGISTER_OK") {                              // 收到注册反馈时
        registerSuccess();
    } else if (msg=="REGISTER_FAIL") {
        showStyledMessageBox("注册失败","注册失败，请重试",QMessageBox::Warning);
    } else if (msg=="REGISTER_DUPLICATE") {
        showStyledMessageBox("注册失败","昵称或邮箱已被使用",QMessageBox::Warning);
    } else if (msg.startsWith("LOGIN_SUCCESS|")) {     // 收到登录反馈时
        QStringList parts = msg.split("|");
        if (parts.size() >= 3) {
            nickname = parts[1];
            email = parts[2];
            loginSuccess();
        }
    } else if (msg=="LOGIN_NOTFOUND") {
        showStyledMessageBox("登录失败","该昵称/邮箱不存在",QMessageBox::Warning);
    } else if (msg=="LOGIN_FAIL") {
        showStyledMessageBox("登录失败","密码错误",QMessageBox::Warning);
    } else if (msg.startsWith("CODE:")) {                    // 收到的是验证码时
        showStyledMessageBox("提示", "验证码已发送至："+nowEmail,QMessageBox::Information);
        QString payload=msg.mid(QString("CODE:").length()).trimmed();
        QStringList parts=payload.split("|");
        code=parts[0];
        if(parts[1]!="") nickname=parts[1];
        if(parts[2]!="") password=parts[2];
    } else if (msg == "EMAIL_NOTFOUND") {                        // 收到邮箱验证码登录反馈时
        showStyledMessageBox( "错误", "该邮箱尚未注册",QMessageBox::Warning);
    } else if (msg=="FRIEND_NOT_FOUND") {                        // 收到好友添加反馈时
        showStyledMessageBox("错误", "未找到该用户",QMessageBox::Warning);
    } else if (msg.startsWith("ADD_FRIEND_SUCCESS|")) {
        QString friendName=msg.mid(QString("ADD_FRIEND_SUCCESS|").length()).trimmed();
        friendListPage->friendListData.append(friendName);
        QString msg="GET_AVATAR|"+friendName+'\n';
        qDebug()<<msg;
        sendNonTextMessage(msg);
        showStyledMessageBox("提示", "好友已添加",QMessageBox::Information);
    } else if (msg=="ALREADY_FRIENDS") {
        showStyledMessageBox( "错误", "好友已存在",QMessageBox::Warning);
    } else if (msg=="ADD_FRIEND_FAILED") {
        showStyledMessageBox( "错误", "不能添加自己为好友",QMessageBox::Warning);
    } else if (msg.startsWith("FRIEND_LIST|")){                      // 好友列表更新
        QString listStr=msg.mid(QString("FRIEND_LIST|").length());
        QStringList names=listStr.split(",",Qt::SkipEmptyParts);
        friendListPage->updateFriendList(names);
    } else if (msg=="CHANGE_NICKNAME_OK") {                        // 收到昵称修改反馈时
        nickname=newNickname;
        settingPage->updateUserInfo();
    } else if (msg=="CHANGE_NICKNAME_FAIL") {
        showStyledMessageBox( "错误", "该昵称已存在",QMessageBox::Warning);
    } else if (msg.startsWith("MATCH_RESULT|")) {                  // 收到匹配反馈时
        QStringList parts=msg.split("|");
        QString matchedUser=parts[1];
        QString matchedInterest=parts[2];
        QString result="匹配成功!\n对方昵称: "+matchedUser+"\n兴趣: "+matchedInterest;
        matchPage->matchingFinished(result);  // 你自己实现的界面更新方法
    } else if (msg=="MATCH_FAILED") {
        matchPage->matchingFinished("未找到合适的匹配对象");
    } else if (msg=="PASS") {
    } else {
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        chatPage->updateMessage("[" + time + "] 对方：" + msg);
        dbManager->insertMessage("对方", "我", msg, time);
    }
}

void Client::onDisconnected() {
    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setWindowTitle("提示");
    msgBox->setText("与服务器断开连接");
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
