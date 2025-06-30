#include "database_manager.h"

DatabaseManager::DatabaseManager(const QString &user) {
    QString safeUser = user;
    safeUser.replace("@","_at_").replace(".","_dot_").replace("/","_slash_");
    QString connectionName = safeUser + "_ChatConnection";

    if (QSqlDatabase::contains(connectionName)) {    // 确保只创建一个连接
        db = QSqlDatabase::database(connectionName);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }

    db.setDatabaseName(QCoreApplication::applicationDirPath() + QString("/%1_chat.db").arg(safeUser));  // 数据库创建于Debug的debug内

    init();
}

void DatabaseManager::init() {
    if (db.isOpen()) {
        QSqlQuery q(db);
        q.exec("CREATE TABLE IF NOT EXISTS messages(id INTEGER PRIMARY KEY AUTOINCREMENT,sender TEXT, receiver TEXT,message TEXT,timestamp TEXT)");
    }
}

void DatabaseManager::insertMessage(const QString &sender, const QString &receiver, const QString &message, const QString &timestamp) {
    if (db.isOpen()) {
        QSqlQuery q(db);
        q.prepare("INSERT INTO messages(sender,receiver,message,timestamp) VALUES(?,?,?,?)");
        q.addBindValue(sender);
        q.addBindValue(receiver);
        q.addBindValue(message);
        q.addBindValue(timestamp);

        if (!q.exec()) {
            qDebug() << "插入消息失败: " << q.lastError().text();
        }
    } else {
        qDebug() << "数据库没有打开";
    }
}

QVector<QString> DatabaseManager::loadMessages() {
    QVector<QString> history;
    if (db.isOpen()) {
        QSqlQuery q(db);
        if (!q.exec("SELECT sender, receiver, message, timestamp FROM messages")) {
            qDebug() << "历史记录读取失败：" << q.lastError().text();
        } else {
            while (q.next()) {
                QString s = q.value(0).toString();
                // QString r = q.value(1).toString();
                QString m = q.value(2).toString();
                QString t = q.value(3).toString();
                QString line = QString("[%1] %2 ：%3").arg(t, s, m);
                history.append(line);
            }
        }
    } else {
        qDebug() << "数据库没有打开";
    }
    return history;
}

void DatabaseManager::closeDatabase() {
    QString connName = db.connectionName();
    if (db.isOpen()) {
        db.close();
    }
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connName);
}
