#include "user_auth_database_manager.h"

UserAuthDatabaseManager::UserAuthDatabaseManager(QObject *parent) : QObject(parent) {
    db=QSqlDatabase::addDatabase("QSQLITE","UserAuthConnection");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/user.db");
    if (!db.open()) {
        qWarning() << "Database open error:" << db.lastError().text();
    } else {
        init();
    }
}

void UserAuthDatabaseManager::init() {
    QSqlQuery q(db);
    q.exec("CREATE TABLE IF NOT EXISTS users ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "nickname TEXT UNIQUE,"
           "password TEXT,"
           "email TEXT UNIQUE,"
           "avatar TEXT,"
           "interests TEXT)");

    q.exec("CREATE TABLE IF NOT EXISTS friends ("
           "user_id INTEGER,"
           "friend_id INTEGER,"
           "UNIQUE(user_id, friend_id))");

    q.exec("SELECT COUNT(*) FROM users WHERE nickname='Server'");
    if (q.next() && q.value(0).toInt()==0) {
        q.prepare("INSERT INTO users (nickname,password,email,avatar,interests) VALUES (?,?,?,?,?)");
        q.addBindValue("Server");
        q.addBindValue("server");
        q.addBindValue("server@qchat.com");
        q.addBindValue(":/images/Server.png");
        q.addBindValue("系统账号");
        q.exec();
    }
}

bool UserAuthDatabaseManager::addUser(const QString &nickname,const QString &password,const QString &email) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO users (nickname,password,email,avatar,interests) VALUES (?,?,?,?,?)");
    q.addBindValue(nickname);
    q.addBindValue(password);
    q.addBindValue(email);
    q.addBindValue(":/images/default.png");
    q.addBindValue("");
    if (!q.exec()) {
        qWarning() << "Add user failed:" << q.lastError().text();
        return false;
    }

    int newUserId=-1, serverId=-1;
    q.prepare("SELECT id FROM users WHERE nickname=?");
    q.addBindValue(nickname);
    if (q.exec() && q.next()) newUserId=q.value(0).toInt();

    q.prepare("SELECT id FROM users WHERE nickname='Server'");
    if (q.exec() && q.next()) serverId=q.value(0).toInt();

    if (newUserId>0 && serverId>0 && newUserId!=serverId) {
        QSqlQuery q2(db);
        q2.prepare("INSERT OR IGNORE INTO friends (user_id, friend_id) VALUES (?,?)");
        q2.addBindValue(newUserId); q2.addBindValue(serverId); q2.exec();
        q2.addBindValue(serverId); q2.addBindValue(newUserId); q2.exec();
    }

    return true;
}

bool UserAuthDatabaseManager::checkLogin(const QString &id,const QString &password) {
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM users WHERE (nickname=? OR email=?) AND password=?");
    q.addBindValue(id);
    q.addBindValue(id);
    q.addBindValue(password);
    if (!q.exec()) return false;
    if (q.next()) return q.value(0).toInt()>0;
    return false;
}

QString UserAuthDatabaseManager::getNicknameByEmail(const QString &e) {
    QSqlQuery q(db);
    q.prepare("SELECT nickname FROM users WHERE email=?");
    q.addBindValue(e);
    if (q.exec() && q.next()) return q.value(0).toString();
    return "";
}

QString UserAuthDatabaseManager::getPasswordByEmail(const QString &e) {
    QSqlQuery q(db);
    q.prepare("SELECT password FROM users WHERE email=?");
    q.addBindValue(e);
    if (q.exec() && q.next()) return q.value(0).toString();
    return "";
}

bool UserAuthDatabaseManager::emailExists(const QString &email) {
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM users WHERE email=?");
    q.addBindValue(email);
    if (!q.exec()) return false;
    if (q.next()) return q.value(0).toInt()>0;
    return false;
}

bool UserAuthDatabaseManager::nicknameExists(const QString &nickname) {
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM users WHERE nickname=?");
    q.addBindValue(nickname);
    if (!q.exec()) return false;
    if (q.next()) return q.value(0).toInt()>0;
    return false;
}

int UserAuthDatabaseManager::getUserId(const QString &nickname) {
    QSqlQuery q(db);
    q.prepare("SELECT id FROM users WHERE nickname=?");
    q.addBindValue(nickname);
    if (q.exec() && q.next()) return q.value(0).toInt();
    return -1;
}

QList<QString> UserAuthDatabaseManager::getFriends(const QString &nickname) {
    QList<QString> res;
    int id=getUserId(nickname);
    if (id==-1) return res;
    QSqlQuery q(db);
    q.prepare("SELECT u.nickname FROM friends f JOIN users u ON f.friend_id=u.id WHERE f.user_id=?");
    q.addBindValue(id);
    if (!q.exec()) return res;
    while (q.next()) res.append(q.value(0).toString());
    return res;
}

QString UserAuthDatabaseManager::getAvatar(const QString &nickname) {
    QSqlQuery q(db);
    q.prepare("SELECT avatar FROM users WHERE nickname=?");
    q.addBindValue(nickname);
    if (q.exec() && q.next()) return q.value(0).toString();
    return "";
}

bool UserAuthDatabaseManager::updateUserInfo(const QString &nickname,const QString &newNickname,const QString &newPassword,const QString &newAvatar) {
    QSqlQuery q(db);
    q.prepare("UPDATE users SET nickname=?, password=?, avatar=? WHERE nickname=?");
    q.addBindValue(newNickname);
    q.addBindValue(newPassword);
    q.addBindValue(newAvatar);
    q.addBindValue(nickname);
    return q.exec();
}

bool UserAuthDatabaseManager::addFriend(const QString &user1,const QString &user2) {
    int id1=getUserId(user1);
    int id2=getUserId(user2);
    if (id1<=0||id2<=0||id1==id2) return false;
    QSqlQuery q(db);
    q.prepare("INSERT OR IGNORE INTO friends (user_id, friend_id) VALUES (?,?)");
    q.addBindValue(id1); q.addBindValue(id2); q.exec();
    q.addBindValue(id2); q.addBindValue(id1); q.exec();
    return true;
}

bool UserAuthDatabaseManager::areFriends(const QString &user, const QString &friendName) {
    int userId=getUserId(user);
    int friendId=getUserId(friendName);
    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM friends WHERE user_id=? AND friend_id=?");
    q.addBindValue(userId);
    q.addBindValue(friendId);
    return q.exec() && q.next(); // 若查到一行，说明是好友
}

bool UserAuthDatabaseManager::removeFriend(const QString &user1,const QString &user2) {
    int id1=getUserId(user1);
    int id2=getUserId(user2);
    if (id1<=0||id2<=0||id1==id2) return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM friends WHERE (user_id=? AND friend_id=?) OR (user_id=? AND friend_id=?)");
    q.addBindValue(id1); q.addBindValue(id2);
    q.addBindValue(id2); q.addBindValue(id1);
    return q.exec();
}

bool UserAuthDatabaseManager::changeNickname(const QString& oldName, const QString& newName) {
    QSqlQuery q(db);

    // 先检查新昵称是否已存在
    q.prepare("SELECT COUNT(*) FROM users WHERE nickname=?");
    q.addBindValue(newName);
    if (!q.exec()) {
        qDebug() << "检查新昵称失败:" << q.lastError().text();
        return false;
    }
    if (!q.next()) {
        qDebug() << "读取新昵称检查结果失败";
        return false;
    }
    if (q.value(0).toInt() > 0) {
        qDebug() << "新昵称已存在:" << newName;
        return false;
    }
    // 执行更新
    q.prepare("UPDATE users SET nickname=? WHERE nickname=?");
    q.addBindValue(newName);
    q.addBindValue(oldName);
    if (!q.exec()) {
        qDebug() << "更新失败:" << q.lastError().text();
        return false;
    }
    if (q.numRowsAffected() == 0) {
        qDebug() << "更新未生效，可能是 oldName 不存在:" << oldName;
        return false;
    }
    qDebug() << "更新成功:" << oldName << "=>" << newName;
    return true;
}

bool UserAuthDatabaseManager::changePasssword(const QString& nickname, const QString& newPassword){
    QSqlQuery q(db);
    q.prepare("UPDATE users SET password=? WHERE nickname=?");
    q.addBindValue(newPassword);
    q.addBindValue(nickname);
    if (!q.exec()) {
        qDebug() << "密码更新失败:" << q.lastError().text();
        return false;
    }
    return q.numRowsAffected() > 0;
}

bool UserAuthDatabaseManager::updateAvatarPath(const QString& nickname, const QString& path) {
    QSqlQuery q(db);
    q.prepare("UPDATE users SET avatar=? WHERE nickname=?");
    q.addBindValue(path);
    q.addBindValue(nickname);
    if (!q.exec()) {
        qDebug() << "头像路径更新失败:" << q.lastError().text();
        return false;
    }
    return q.numRowsAffected() > 0;
}

bool UserAuthDatabaseManager::changeInterest(const QString &nickname, const QString &interest) {
    QSqlQuery q(db);
    q.prepare("UPDATE users SET interests=? WHERE nickname=?");
    q.addBindValue(interest);
    q.addBindValue(nickname);
    if (!q.exec()) {
        qDebug() << "更新兴趣失败:" << q.lastError().text();
        return false;
    }
    return q.numRowsAffected() > 0;
}

QString UserAuthDatabaseManager::getInterest(const QString &nickname) {
    QSqlQuery q(db);
    q.prepare("SELECT interests FROM users WHERE nickname=?");
    q.addBindValue(nickname);
    if (q.exec() && q.next()) {
        return q.value(0).toString();
    }
    return QString();
}

bool UserAuthDatabaseManager::findMatchingUser(const QString &nickname, QString &matchedUser, QString &matchedInterest) {
    QString myInterest=getInterest(nickname);
    if(myInterest.isEmpty()) return false;

    QStringList myList=myInterest.split(u'，',Qt::SkipEmptyParts);
    if(myList.isEmpty()) return false;

    QSqlQuery q(db);
    q.prepare("SELECT nickname, interests FROM users WHERE nickname != ? AND interests != ''");
    q.addBindValue(nickname);
    if(!q.exec()) return false;

    while(q.next()) {
        QString other=q.value(0).toString();
        QString interestStr=q.value(1).toString();
        if(areFriends(nickname,other)) continue;

        QStringList otherList=interestStr.split(u'，',Qt::SkipEmptyParts);
        for(const QString &i:std::as_const(myList)) {
            if(otherList.contains(i)) {
                matchedUser=other;
                matchedInterest=interestStr;
                return true;
            }
        }
    }
    return false;
}
