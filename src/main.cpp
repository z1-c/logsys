#include <QCoreApplication>
#include "dbmanager.h"
#include "userauth.h"
#include "logupload.h"
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // 初始化数据库
    if (!DBManager::initDatabase()) return -1;

    // 测试用户注册 & 登录
    if (UserAuth::registerUser("testuser", "mypassword")) {
        qDebug() << "用户注册成功！";
    } else {
        qDebug() << "用户注册失败！";
    }

    if (UserAuth::loginUser("testuser", "mypassword")) {
        qDebug() << "登录成功！";
    } else {
        qDebug() << "登录失败！";
    }

    // 测试日志上传
    if (LogUpload::uploadLog("test.log")) {
        qDebug() << "日志上传成功！";
    } else {
        qDebug() << "日志上传失败！";
    }

    return a.exec();
}
