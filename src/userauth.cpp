#include "userauth.h"
#include "dbmanager.h"
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QDebug>

#include <gmssl/sm3.h>  // 确保引入GMSSL库

QString UserAuth::sm3Hash(const QString &input) {
    QByteArray data = input.toUtf8();
    unsigned char hash[32];  // SM3输出为256位（32字节）
    sm3(data.constData(), data.size(), hash);
    return QString::fromLatin1(hash, 32).toHex();  // 转换为16进制字符串
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
