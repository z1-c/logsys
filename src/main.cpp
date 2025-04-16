// src/main.cpp
#include <QCoreApplication>
#include "dbmanager.h"
#include "userauth.h"
#include "logupload.h"
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // 初始化数据库
    if (!DBManager::initDatabase()) return -1;

    // 测试用户注册
    if (UserAuth::registerUser("testuser", "mypassword", "user")) { // 添加角色参数
        qDebug() << "普通用户注册成功！";
    } else {
        qDebug() << "普通用户注册失败！";
    }

    if (UserAuth::registerUser("adminuser", "adminpassword", "admin")) { // 添加角色参数
        qDebug() << "管理员用户注册成功！";
    } else {
        qDebug() << "管理员用户注册失败！";
    }

    // 测试用户登录
    if (UserAuth::loginUser("testuser", "mypassword")) {
        qDebug() << "普通用户登录成功！";
    } else {
        qDebug() << "普通用户登录失败！";
    }

    if (UserAuth::loginUser("adminuser", "adminpassword")) {
        qDebug() << "管理员用户登录成功！";
    } else {
        qDebug() << "管理员用户登录失败！";
    }

    // 测试日志上传
    if (LogUpload::uploadLog("test.log", "testuser")) {
        qDebug() << "日志上传成功！";
    } else {
        qDebug() << "日志上传失败！";
    }

    // 测试日志查询
    QVector<QString> userLogs = LogUpload::queryLogs("testuser", "user");
    qDebug() << "普通用户查询到的日志：";
    for (const auto &log : userLogs) {
        qDebug() << log;
    }

    QVector<QString> adminLogs = LogUpload::queryLogs("adminuser", "admin");
    qDebug() << "管理员查询到的日志：";
    for (const auto &log : adminLogs) {
        qDebug() << log;
    }

    return a.exec();
}
