#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QSqlError>
#include <QDebug>

class DatabaseManager {
public:
    explicit DatabaseManager(const QString &user);

    void insertMessage(const QString &sender,const QString &receiver,const QString &message,const QString &timestamp);
    void closeDatabase();
    QVector<QString> loadMessages();

private:
    QSqlDatabase db;

    void init();
};

#endif
