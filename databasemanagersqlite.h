#ifndef DATABASEMANAGERSQLITE_H
#define DATABASEMANAGERSQLITE_H

#include "qtheaders.h"

#define DB_TYPE "QSQLITE"
#define FRIENDS_TABLE_NAME "friends"
#define ROOMS_TABLE_NAME "rooms"
#define SETTINGS_TABLE_NAME "settings"
#define DB_PATH ""

class DatabaseManagerSQLite
{
public:
    DatabaseManagerSQLite();
    ~DatabaseManagerSQLite();
    bool CreateConnection(int db_name);
    bool CloseConnection();

    bool CreateFriendsTable();
    bool AddFriend(int id_friend, int friend_type);
    bool ChangeFriendsStatus(int friend_status);
    bool DeleteFriend(int id_friend);

    bool CreateRoomsTable();
    bool AddRoom(int id_room);
    bool DeleteRoom(int id_room);

    bool CreateSettingsTable();
    bool ChangeUserName(QString name);
    bool AddSettings(QString name);

    bool DeleteDatabase(int db_name); // try using this with current db only

    bool CreateAllTables();
private:
    QSqlDatabase db;
    QSqlQuery* query = NULL;
};

#endif // DATABASEMANAGERSQLITE_H
