#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "qtheaders.h"
#include "databasemanagersqlite.h"

class ClientManager
{
public:
    ClientManager(int descriptor);
    ~ClientManager();

    void UpdateTimestamp();
    long long GetTimestamp();
    void SetLogged(bool logged);
    void SetDescriptor(int descriptor);
    int GetDescriptor();
    void SetSQLiteDB(DatabaseManagerSQLite* SQLiteDB);

private:
    int descriptor;
    bool logged;
    long long timestamp;



    DatabaseManagerSQLite* SQLiteDB;
};

#endif // CLIENTMANAGER_H
