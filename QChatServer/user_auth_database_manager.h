#ifndef USERAUTHDATABASEMANAGER_H
#define USERAUTHDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QDebug>

class UserAuthDatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit UserAuthDatabaseManager(QObject *parent=nullptr);

    bool addUser(const QString &nickname,const QString &password,const QString &email);
    bool checkLogin(const QString &id,const QString &password);
    bool emailExists(const QString &email);
    bool nicknameExists(const QString &nickname);
    QString getNicknameByEmail(const QString&e);

    int getUserId(const QString &nickname);
    QString getAvatar(const QString &nickname);
    QList<QString> getFriends(const QString &nickname);

    bool areFriends(const QString &user, const QString &friendName);
    bool updateUserInfo(const QString &nickname,const QString &newNickname,const QString &newPassword,const QString &newAvatar);
    bool addFriend(const QString &user1,const QString &user2);
    bool removeFriend(const QString &user1,const QString &user2);
    bool changeNickname(const QString& oldName, const QString& newName);
    bool updateAvatarPath(const QString& nickname, const QString& path);
    bool changeInterest(const QString &nickname, const QString &interest);
    QString getInterest(const QString &nickname);
    bool findMatchingUser(const QString &nickname, QString &matchedUser, QString &matchedInterest);

    QSqlDatabase db;

private:
    void init();
};

#endif // USERAUTHDATABASEMANAGER_H
