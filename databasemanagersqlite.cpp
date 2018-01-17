#include "databasemanagersqlite.h"

DatabaseManagerSQLite::DatabaseManagerSQLite()
{

}

DatabaseManagerSQLite::~DatabaseManagerSQLite()
{
    if(db.isOpen())
    {
        db.close();
    }
    if(query != NULL)
    {
        delete query;
        query = NULL;
    }
}

bool DatabaseManagerSQLite::CreateConnection(int db_name)
{
    if(!QSqlDatabase::contains(QString::number(db_name)))
    {
        db = QSqlDatabase::addDatabase("QSQLITE", QString::number(db_name));
        qDebug() << QString("[Database] ") + QString("SQLite Database ")
                 + QString::number(db_name)
                 + QString(" created!");
    }
    else
    {
        db = QSqlDatabase::database(QString::number(db_name));
        qDebug() << QString("[Database] ") + QString("SQLite Database ")
                 + QString::number(db_name)
                 + QString(" exists!");
    }
    QString path = QString(DB_PATH) + QString::number(db_name) + QString(".db");
    qDebug() << QString("[Database] ") + path;
    db.setDatabaseName(path);
    db.open();

    return CreateAllTables();
}

bool DatabaseManagerSQLite::CreateFriendsTable()
{
    query = new QSqlQuery(db);

    if (!db.tables().contains(FRIENDS_TABLE_NAME)) {
        query->prepare( "CREATE TABLE " + QString(FRIENDS_TABLE_NAME) + " ("
                        "id_friend MEDIUMINT NOT NULL, "
                        "friend_type MEDIUMINT NOT NULL, "
                        "friend_status TINYINT(1), "
                        "date_added DATETIME NOT NULL, "
                        "PRIMARY KEY (id_friend));");
        if(!query->exec())
        {
            delete query;
            query = NULL;
            qDebug() << QString("[Database] ") + QString("Failed to create friends table!");
            return false;
        }
        qDebug() << QString("[Database] ") + QString("Friends table succesfully created!");
        db.commit();
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Friends table exists!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::AddFriend(int id_friend, int friend_type)
{
    query = new QSqlQuery(db);
    query->prepare( "INSERT INTO " +
                    QString(FRIENDS_TABLE_NAME) +
                    " (id_friend, friend_type, friend_status, date_added) "
                    "VALUES (?, ?, ?, ?)");
    query->addBindValue(id_friend);
    query->addBindValue(friend_type);
    query->addBindValue(0);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text() + QString("Failed adding friend ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Friend ID ") + QString::number(id_friend)
                    + QString( " as type ") + QString::number(friend_type)
                    + QString( " with status 0!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::ChangeFriendsStatus(int id_friend, int friend_status)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(FRIENDS_TABLE_NAME) +
                    " SET friend_status = ?, "
                    "date_added = ? "
                    "WHERE id_friend = ?;");
    query->addBindValue(friend_status);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(id_friend);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text() + QString("Failed changing friend status!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Friend status changed to: ")
                    + QString::number(friend_status);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::DeleteFriend(int id_friend)
{
    query = new QSqlQuery(db);
    query->prepare( "DELETE FROM " +
                    QString(FRIENDS_TABLE_NAME) +
                    " WHERE id_friend = ?");
    query->addBindValue(id_friend);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text() + QString("Failed deleting friend ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("Deleted friend ID: ")
                    + QString::number(id_friend);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::CreateRoomsTable()
{
    query = new QSqlQuery(db);

   if (!db.tables().contains(ROOMS_TABLE_NAME))
   {
        query->prepare( "CREATE TABLE " + QString(ROOMS_TABLE_NAME) + " ("
                        "id_room MEDIUMINT NOT NULL, "
                        "date_added DATETIME NOT NULL, "
                        "PRIMARY KEY (id_room));");
        if(!query->exec())
        {
            delete query;
            query = NULL;
            qDebug() << QString("[Database] ") + QString("Failed to create rooms table!");
            return false;
        }
        qDebug() << QString("[Database] ") + QString("Rooms table succesfully created!");
        db.commit();
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Rooms table exists!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::AddRoom(int id_room)
{
    query = new QSqlQuery(db);
    query->prepare( "INSERT INTO " +
                    QString(ROOMS_TABLE_NAME) +
                    " (id_room, date_added)"
                    " VALUES (?, ?)");
    query->addBindValue(id_room);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text() + QString("Failed adding room ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Room ID: ") + QString::number(id_room);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::DeleteRoom(int id_room)
{
    query = new QSqlQuery(db);
    query->prepare( "DELETE FROM " +
                    QString(ROOMS_TABLE_NAME) +
                    " WHERE id_room = ?");
    query->addBindValue(id_room);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text() + QString("Failed deleting room ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Deleted room ID: ")
                    + QString::number(id_room);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::AddSettings(QString name)
{
    query = new QSqlQuery(db);
    query->prepare( "INSERT INTO " +
                    QString(SETTINGS_TABLE_NAME) +
                    " (name, date_created, date_updated) "
                    "VALUES (?, ?, ?)");
    query->addBindValue(name);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text() + QString("Failed adding user name!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("User name: ") + name;
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::DeleteDatabase(int db_name)
{
    CreateConnection(db_name);
    db.close();

    QString db_path = QString(DB_PATH)
            + QString::number(db_name)
            + QString(".db");
    QSqlDatabase::removeDatabase(db_path);

    QFile file(db_path);
    file.remove();

    qDebug() << QString("[Database] ") + QString("Deleted database: ")
                + QString::number(db_name);

    return true;
}

bool DatabaseManagerSQLite::CreateAllTables()
{
    return CreateFriendsTable() && CreateRoomsTable() && CreateSettingsTable();
}

bool DatabaseManagerSQLite::CreateSettingsTable()
{
    query = new QSqlQuery(db);

    if (!db.tables().contains(SETTINGS_TABLE_NAME))
    {
        query->prepare( "CREATE TABLE " + QString(SETTINGS_TABLE_NAME) + " ("
                        "name CHAR(50) NOT NULL, "
                        "date_created DATETIME NOT NULL, "
                        "date_updated DATETIME NOT NULL);");
        if(!query->exec())
        {
            delete query;
            query = NULL;
            qDebug() << QString("[Database] ") + QString("Failed to create settings table!");
            return false;
        }
        qDebug() << QString("[Database] ") + QString("Settings table succesfully created!");
        db.commit();
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Settings table exists!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerSQLite::ChangeUserName(QString name)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(SETTINGS_TABLE_NAME) +
                    " SET name = ?, date_updated = ?;");
    query->addBindValue(name);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed changing user name!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("User name changed to: ") + name;
    }

    delete query;
    query = NULL;
    return true;
}
