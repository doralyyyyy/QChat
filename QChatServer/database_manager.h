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
    explicit DatabaseManager();

    void init(const QString &tableName);
    QString emailToTable(const QString &email);
    void insertMessage(const QString &tableName,const QString &sender,const QString &receiver,const QString &message,const QString &timestamp);
    QVector<QString> loadMessages(const QString &tableName);
    void closeDatabase();

private:
    QSqlDatabase db;

};

#endif
