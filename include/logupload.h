#ifndef LOGUPLOAD_H
#define LOGUPLOAD_H

#include <QString>

class LogUpload {
public:
    static bool uploadLog(const QString &filePath);
};

#endif // LOGUPLOAD_H
