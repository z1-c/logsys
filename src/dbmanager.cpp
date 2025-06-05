#include "dbmanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

QMutex dbMutex;  // 全局互斥锁

QSqlDatabase DBManager::getDatabase() {
    static QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("logsys.db");
    return db;
}

bool DBManager::initDatabase() {
    QSqlDatabase db = getDatabase();
    if (!db.open()) {
        qDebug() << "数据库打开失败！";
        return false;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE NOT NULL, "
               "password_hash TEXT NOT NULL, "
               "role TEXT DEFAULT 'user')");

    query.exec("CREATE TABLE IF NOT EXISTS logs ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "filename TEXT NOT NULL, "
               "content BLOB NOT NULL, "
               "signature TEXT NOT NULL)"
               "content TEXT NOT NULL, "
               "username TEXT NOT NULL)");

    query.exec("CREATE TABLE IF NOT EXISTS audit_logs ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT NOT NULL, "
               "operation TEXT NOT NULL, "
               "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)");

    return true;
}
