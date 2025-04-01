#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DBManager {
public:
    static bool initDatabase();
    static QSqlDatabase getDatabase();
};

#endif // DBMANAGER_H

