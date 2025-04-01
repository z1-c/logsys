#include "logupload.h"
#include "dbmanager.h"
#include <QFile>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

bool LogUpload::uploadLog(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件：" << filePath;
        return false;
    }

    QByteArray fileData = file.readAll();
    qDebug() << "文件内容：" << fileData;

    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) {
        qDebug() << "数据库连接失败！";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO logs (filename, content) VALUES (:filename, :content)");
    query.bindValue(":filename", file.fileName());
    query.bindValue(":content", fileData.toHex());

    if (!query.exec()) {
        qDebug() << "数据库插入失败：" << query.lastError().text();
        return false;
    }

    qDebug() << "日志上传成功！";
    return true;
}
