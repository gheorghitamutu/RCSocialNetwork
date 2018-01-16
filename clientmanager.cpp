#include "clientmanager.h"

ClientManager::ClientManager(int descriptor)
{
    SetDescriptor(descriptor);
    SetLogged(false);
    SetSQLiteDB(NULL);
}

ClientManager::~ClientManager()
{
    if(this->SQLiteDB != NULL)
    {
        delete this->SQLiteDB;
    }
}

void ClientManager::SetLogged(bool logged)
{
    this->logged = logged;
}

void ClientManager::SetDescriptor(int descriptor)
{
    this->descriptor = descriptor;
}

int ClientManager::GetDescriptor()
{
    return this->descriptor;
}

void ClientManager::SetSQLiteDB(DatabaseManagerSQLite *SQLiteDB)
{
    if(this->SQLiteDB != NULL)
    {
        delete this->SQLiteDB;
    }
    this->SQLiteDB = SQLiteDB;
}

void ClientManager::SetId(int id)
{
    this->id = id;
}

int ClientManager::GetId()
{
    return this->id;
}
