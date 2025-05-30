#include <QCoreApplication>
#include "dbmanager.h"
#include "userauth.h"
#include "logupload.h"
#include "audit.h"
#include "MainThreadObject.h"
#include "UserOperation.h" // 包含 UserOperation 的定义
#include <QThreadPool>
#include <QRunnable>
#include <QMutex>
#include <QMetaObject>
#include <QDebug>

extern QMutex dbMutex; // 确保全局互斥锁已声明

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    qDebug() << "程序启动";
    if (!DBManager::initDatabase()) {
        qDebug() << "数据库初始化失败，程序退出";
        return -1;
    }
    qDebug() << "数据库初始化成功";

    MainThreadObject mainThreadObject;
    QThreadPool pool;
    pool.setMaxThreadCount(10);

    UserOperation *registerOp1 = new UserOperation(&mainThreadObject, "testuser", "mypassword", "register");
    pool.start(registerOp1);

    UserOperation *loginOp1 = new UserOperation(&mainThreadObject, "testuser", "mypassword", "login");
    pool.start(loginOp1);

    UserOperation *uploadOp = new UserOperation(&mainThreadObject, "testuser", "mypassword", "upload");
    pool.start(uploadOp);

    UserOperation *queryOp1 = new UserOperation(&mainThreadObject, "testuser", "mypassword", "query");
    pool.start(queryOp1);

    pool.waitForDone();
    qDebug() << "所有任务完成";

    return a.exec();
}
