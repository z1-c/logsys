// src/userauth.cpp
#include "userauth.h"
#include "dbmanager.h"
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QDebug>

QString UserAuth::sm3Hash(const QString &input) {
    QByteArray hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha3_256);
    return hash.toHex();
}

bool UserAuth::registerUser(const QString &username, const QString &password, const QString &role) {
    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) return false;

    QString hashedPassword = sm3Hash(password);
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash, role) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    query.addBindValue(role);

    return query.exec();
}

bool UserAuth::loginUser(const QString &username, const QString &password) {
    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) return false;

    QString hashedPassword = sm3Hash(password);
    QSqlQuery query;
    query.prepare("SELECT password_hash, role FROM users WHERE username = ?");
    query.addBindValue(username);
    query.exec();

    if (query.next()) {
        QString storedRole = query.value(1).toString();
        qDebug() << "用户角色：" << storedRole;
        return query.value(0).toString() == hashedPassword;
    }
    return false;
}
