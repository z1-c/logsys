#ifndef MAINTHREADOBJECT_H
#define MAINTHREADOBJECT_H

#include <QObject>

class MainThreadObject : public QObject {
    Q_OBJECT
public:
    explicit MainThreadObject(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void performRegister(const QString &username, const QString &password);
    void performLogin(const QString &username, const QString &password);
    void performUpload(const QString &username);
    void performQuery(const QString &username);
};

#endif // MAINTHREADOBJECT_H
