#include "audit.h"
#include "dbmanager.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

void Audit::logOperation(const QString &username, const QString &operation) {
    qDebug() << "记录审计日志：" << username << operation;
    QSqlDatabase db = DBManager::getDatabase();
    if (!db.open()) {
        qDebug() << "数据库连接失败！";
        return;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO audit_logs (username, operation) VALUES (:username, :operation)");
    query.bindValue(":username", username);
    query.bindValue(":operation", operation);
    if (!query.exec()) {
        qDebug() << "审计日志记录失败：" << query.lastError().text();
    } else {
        qDebug() << "审计日志记录成功！";
    }
}
