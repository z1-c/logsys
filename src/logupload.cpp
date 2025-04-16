#include "logupload.h"
#include "dbmanager.h"
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QTextStream>

bool LogUpload::uploadLog(const QString &filePath, const QString &username) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件：" << filePath;
        return false;
    }

    // 读取文件内容并转换为 UTF-8
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString content = in.readAll();
    QByteArray utf8Data = content.toUtf8();

    // 使用 Base64 编码作为临时加密方法
    QByteArray encryptedData = utf8Data.toBase64();

    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) {
        qDebug() << "数据库连接失败！";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO logs (filename, content, username) VALUES (:filename, :content, :username)");
    query.bindValue(":filename", file.fileName());
    query.bindValue(":content", encryptedData);
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "数据库插入失败：" << query.lastError().text();
        return false;
    }

    qDebug() << "日志上传成功！";
    return true;
}

QVector<QString> LogUpload::queryLogs(const QString &username, const QString &role) {
    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) {
        qDebug() << "数据库连接失败！";
        return {};
    }

    QSqlQuery query;
    if (role == "admin") {
        query.prepare("SELECT filename, content FROM logs");
    } else {
        query.prepare("SELECT filename, content FROM logs WHERE username = :username");
        query.bindValue(":username", username);
    }

    QVector<QString> logs;
    if (query.exec()) {
        while (query.next()) {
            QString filename = query.value(0).toString();
            QByteArray encryptedContent = query.value(1).toByteArray();
            QByteArray decryptedContent = QByteArray::fromBase64(encryptedContent);
            logs.append("文件名: " + filename + "\n内容: " + QString::fromUtf8(decryptedContent));
        }
    } else {
        qDebug() << "查询失败：" << query.lastError().text();
    }

    return logs;
}
