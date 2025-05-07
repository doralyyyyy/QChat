#ifndef USER_AUTH_DATABASE_MANAGER_H
#define USER_AUTH_DATABASE_MANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication>

class UserAuthDatabaseManager {
public:
    UserAuthDatabaseManager();
    bool addUser(const QString &nickname,const QString &password,const QString &email);
    bool checkLogin(const QString &id,const QString &password);
    bool emailExists(const QString &email);
    bool nicknameExists(const QString &nickname);

private:
    QSqlDatabase db;
    void init();
};

#endif
