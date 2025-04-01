#include "dbmanager.h"
#include <QSqlQuery>
#include <QDebug>

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

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE NOT NULL, "
               "password_hash TEXT NOT NULL)");

    query.exec("CREATE TABLE IF NOT EXISTS logs ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "filename TEXT NOT NULL, "
               "content TEXT NOT NULL)");

    return true;
}
