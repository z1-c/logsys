#include "userauth.h"
#include "dbmanager.h"
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QDebug>

QString UserAuth::sm3Hash(const QString &input) {
    QByteArray hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha3_256);
    return hash.toHex();
}

bool UserAuth::registerUser(const QString &username, const QString &password) {
    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) return false;

    QString hashedPassword = sm3Hash(password);
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    
    return query.exec();
}

bool UserAuth::loginUser(const QString &username, const QString &password) {
    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) return false;

    QString hashedPassword = sm3Hash(password);
    QSqlQuery query;
    query.prepare("SELECT password_hash FROM users WHERE username = ?");
    query.addBindValue(username);
    query.exec();

    if (query.next()) {
        return query.value(0).toString() == hashedPassword;
    }
    return false;
}
