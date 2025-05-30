#include "MainThreadObject.h"
#include "dbmanager.h"
#include "userauth.h"
#include "logupload.h"
#include "audit.h"
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

extern QMutex dbMutex;  // 声明为extern

void MainThreadObject::performRegister(const QString &username, const QString &password) {
    QMutexLocker locker(&dbMutex);
    if (UserAuth::registerUser(username, password, "user")) {
        qDebug() << username << "注册成功！";
        Audit::logOperation(username, "注册");
    } else {
        qDebug() << username << "注册失败！";
    }
}

void MainThreadObject::performLogin(const QString &username, const QString &password) {
    qDebug() << "主线程 - 登录：" << username;
    QMutexLocker locker(&dbMutex);
    if (UserAuth::loginUser(username, password)) {
        qDebug() << username << "登录成功！";
        Audit::logOperation(username, "登录");
    } else {
        qDebug() << username << "登录失败！";
    }
}

void MainThreadObject::performUpload(const QString &username) {
    qDebug() << "主线程 - 日志上传：" << username;
    QMutexLocker locker(&dbMutex);
    if (LogUpload::uploadLog("test.log", username)) {
        qDebug() << username << "日志上传成功！";
        Audit::logOperation(username, "上传日志");
    } else {
        qDebug() << username << "日志上传失败！";
    }
}

void MainThreadObject::performQuery(const QString &username) {
    qDebug() << "主线程 - 日志查询：" << username;
    QMutexLocker locker(&dbMutex);
    QVector<QString> logs = LogUpload::queryLogs(username, "user");
    qDebug() << username << "查询到的日志：";
    for (const auto &log : logs) {
        qDebug() << log;
    }
    Audit::logOperation(username, "查询日志");
}
