#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "qtheaders.h"
#include "databasemanagersqlite.h"

class ClientManager
{
public:
    ClientManager(int descriptor);
    ~ClientManager();

    void SetLogged(bool logged);
    void SetDescriptor(int descriptor);
    int GetDescriptor();
    void SetSQLiteDB(DatabaseManagerSQLite* SQLiteDB);
    void SetId(int id);
    int GetId();

private:
    int descriptor;
    bool logged;
    int id;
    DatabaseManagerSQLite* SQLiteDB;
};

#endif // CLIENTMANAGER_H
