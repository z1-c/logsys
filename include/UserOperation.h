#ifndef USEROPERATION_H
#define USEROPERATION_H

#include <QRunnable>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaObject>
#include <QDebug>

class MainThreadObject;

class UserOperation : public QRunnable {
public:
    UserOperation(MainThreadObject *mainThreadObject, const QString &username, const QString &password, const QString &operation);

    void run() override;

private:
    MainThreadObject *mainThreadObject;
    QString username;
    QString password;
    QString operation;
};

#endif // USEROPERATION_H
