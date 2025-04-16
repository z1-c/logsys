// include/logupload.h
#ifndef LOGUPLOAD_H
#define LOGUPLOAD_H

#include <QString>
#include <QVector>

class LogUpload {
public:
    static bool uploadLog(const QString &filePath, const QString &username);
    static QVector<QString> queryLogs(const QString &username, const QString &role);
};

#endif // LOGUPLOAD_H
