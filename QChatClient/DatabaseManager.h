#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>

class DatabaseManager {
public:
    DatabaseManager();

    void insertMessage(const QString &sender,const QString &receiver,const QString &message,const QString &timestamp);
    void closeDatabase();
    QVector<QString> loadMessages();

private:
    QSqlDatabase db;

    void init();
};

#endif
