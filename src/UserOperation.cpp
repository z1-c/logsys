// useroperation.cpp
#include "UserOperation.h"
#include "MainThreadObject.h"
#include <QMutex>
#include <QMutexLocker>
#include <QMetaObject>
#include <QDebug>

extern QMutex dbMutex; // 确保全局互斥锁已声明

UserOperation::UserOperation(MainThreadObject *mainThreadObject, const QString &username, const QString &password, const QString &operation)
    : mainThreadObject(mainThreadObject), username(username), password(password), operation(operation) {}

void UserOperation::run() {
    qDebug() << "任务开始：" << operation;
    QMutexLocker locker(&dbMutex); // 确保线程安全
    if (operation == "register") {
        QMetaObject::invokeMethod(mainThreadObject, "performRegister", Qt::BlockingQueuedConnection, Q_ARG(QString, username), Q_ARG(QString, password));
    } else if (operation == "login") {
        QMetaObject::invokeMethod(mainThreadObject, "performLogin", Qt::BlockingQueuedConnection, Q_ARG(QString, username), Q_ARG(QString, password));
    } else if (operation == "upload") {
        QMetaObject::invokeMethod(mainThreadObject, "performUpload", Qt::BlockingQueuedConnection, Q_ARG(QString, username));
    } else if (operation == "query") {
        QMetaObject::invokeMethod(mainThreadObject, "performQuery", Qt::BlockingQueuedConnection, Q_ARG(QString, username));
    }
    qDebug() << "任务结束：" << operation;
}
