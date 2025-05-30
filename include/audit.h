#ifndef AUDIT_H
#define AUDIT_H

#include <QString>

class Audit {
public:
    static void logOperation(const QString &username, const QString &operation);
};

#endif // AUDIT_H
