#include "database_manager.h"

DatabaseManager::DatabaseManager() {
    if (QSqlDatabase::contains("ChatConnection")) {
        db = QSqlDatabase::database("ChatConnection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "ChatConnection");
    }

    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/chat.db");
}

QString DatabaseManager::emailToTable(const QString &email) {
    QString safe = email;
    safe.replace("@", "_at_").replace(".", "_dot_").replace("/", "_slash_");
    return "m" + safe;
}

void DatabaseManager::init(const QString &email) {
    QString tableName = emailToTable(email);
    if (db.open()) {
        QSqlQuery q(db);
        QString sql = QString("CREATE TABLE IF NOT EXISTS %1 (id INTEGER PRIMARY KEY AUTOINCREMENT, sender TEXT, receiver TEXT, message TEXT, timestamp TEXT)").arg(tableName);
        if (!q.exec(sql)) {
            qDebug() << "表创建失败:" << q.lastError().text();
        }
    } else {
        qDebug() << "数据库打开失败";
    }
}

void DatabaseManager::insertMessage(const QString &email, const QString &sender, const QString &receiver, const QString &message, const QString &timestamp) {
    QString tableName = emailToTable(email);
    if (db.open()) {
        QSqlQuery q(db);
        QString sql = QString("INSERT INTO %1(sender,receiver,message,timestamp) VALUES(?,?,?,?)").arg(tableName);
        q.prepare(sql);
        q.addBindValue(sender);
        q.addBindValue(receiver);
        q.addBindValue(message);
        q.addBindValue(timestamp);
        if (!q.exec()) {
            qDebug() << "插入失败：" << q.lastError().text();
        }
    } else {
        qDebug() << "数据库没有打开";
    }
}

QVector<QString> DatabaseManager::loadMessages(const QString &email) {
    QString tableName = emailToTable(email);
    QVector<QString> history;
    if (db.open()) {
        QSqlQuery q(db);
        QString sql = QString("SELECT sender, receiver, message, timestamp FROM %1").arg(tableName);
        if (!q.exec(sql)) {
            qDebug() << "读取失败：" << q.lastError().text();
        } else {
            while (q.next()) {
                QString s = q.value(0).toString();
                QString m = q.value(2).toString();
                QString t = q.value(3).toString();
                history.append(QString("[%1] %2 ：%3").arg(t, s, m));
            }
        }
    } else {
        qDebug() << "数据库没有打开";
    }
    return history;
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
        QSqlDatabase::removeDatabase("ChatConnection");
    }
}
