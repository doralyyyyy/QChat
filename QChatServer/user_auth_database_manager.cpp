#include "user_auth_database_manager.h"

UserAuthDatabaseManager::UserAuthDatabaseManager() {
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
           "email TEXT UNIQUE)");
}

bool UserAuthDatabaseManager::addUser(const QString &nickname,const QString &password,const QString &email) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO users (nickname,password,email) VALUES (?,?,?)");
    q.addBindValue(nickname);
    q.addBindValue(password);
    q.addBindValue(email);
    if (!q.exec()) {
        qWarning() << "Add user failed:" << q.lastError().text();
        return false;
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
