#ifndef USERAUTH_H
#define USERAUTH_H

#include <QString>

class UserAuth {
public:
    static QString sm3Hash(const QString &input);
    static bool registerUser(const QString &username, const QString &password, const QString &role); // 添加 role 参数
    static bool loginUser(const QString &username, const QString &password);
};

#endif // USERAUTH_H
