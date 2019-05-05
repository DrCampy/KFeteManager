#ifndef HISTORIESMANAGER_H
#define HISTORIESMANAGER_H

#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QMap>

#include "sessionsmanager.h"

class HistoriesManager
{
public:
    static bool         openHistoryDb();
    static void         addSession(Session session);
    static void         closeHistoryDb();
    static void         createHistoryDb();
    static QSqlDatabase getDatabase();
};

#endif // HISTORIESMANAGER_H
