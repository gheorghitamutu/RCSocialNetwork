#include "databasemanagermysql.h"

DatabaseManagerMySQL::DatabaseManagerMySQL()
{
    CreateConnection();
    CreateUsersTable();
    CreatePostsTable();
    CreateCommentsTable();
    CreateMessagesTable();
    CreateRoomsTable();
    CreateLikesTable();
}

bool DatabaseManagerMySQL::CreateConnection()
{
    if(!QSqlDatabase::contains(DB_NAME))
    {
        db = QSqlDatabase::addDatabase(DB_MYSQL_TYPE, DB_NAME);
    }
    else
    {
        db = QSqlDatabase::database(DB_NAME);
    }

    db.setHostName(HOST_NAME);
    db.setPort(DB_PORT);
    db.setDatabaseName(DB_NAME);
    db.setUserName(USER_NAME);
    db.setPassword(DB_PASSWORD);

    if (!db.open())
    {
        qDebug() << QString("[Database] ") + db.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManagerMySQL::CloseConnection()
{
    if(db.isOpen())
    {
        db.close();
    }
    return true;
}

bool DatabaseManagerMySQL::CreateUsersTable()
{
    query = new QSqlQuery(db);

    if (!db.tables().contains(USERS_TABLE_NAME))
    {
        query->prepare( "CREATE TABLE " + QString(USERS_TABLE_NAME) + " ("
                        "id MEDIUMINT NOT NULL AUTO_INCREMENT, "
                        "email CHAR(50) NOT NULL, "
                        "password CHAR(50) NOT NULL, "
                        "user_status TINYINT(1), "
                        "admin TINYINT(1) DEFAULT 0, "
                        "date_created DATETIME NOT NULL, "
                        "date_updated DATETIME NOT NULL, "
                        "PRIMARY KEY (id), "
                        "UNIQUE KEY (email));" );
        if(!query->exec())
        {
            delete query;
            query = NULL;
            qDebug() << QString("[Database] ") + QString("Failed to create users table!");
            return false;
        }
        qDebug() << QString("[Database] ") + QString("Users table succesfully created!");
        db.commit();
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Users table exists!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::CreatePostsTable()
{
    query = new QSqlQuery(db);

    if (!db.tables().contains(POSTS_TABLE_NAME))
    {
        query->prepare( "CREATE TABLE " + QString(POSTS_TABLE_NAME) + " ("
                        "id_post MEDIUMINT NOT NULL AUTO_INCREMENT, "
                        "id_user MEDIUMINT NOT NULL, "
                        "post TEXT(1024) NOT NULL, "
                        "likes MEDIUMINT NOT NULL, "
                        "post_type MEDIUMINT NOT NULL DEFAULT 0, "
                        "date_created DATETIME NOT NULL, "
                        "date_updated DATETIME NOT NULL, "
                        "PRIMARY KEY (id_post));");
        if(!query->exec())
        {
            delete query;
            query = NULL;
            qDebug() << QString("[Database] ") + QString("Failed to create posts table!");
            return false;
        }
        qDebug() << QString("[Database] ") + QString("Posts table succesfully created!");
        db.commit();
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Posts table exists!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::AddPost(int id_user, QString post, int post_type)
{
    query = new QSqlQuery(db);

    query->prepare( "INSERT INTO " +
                    QString(POSTS_TABLE_NAME) +
                    " (id_user, post, likes, post_type, date_created, date_updated) "
                    "VALUES (?, ?, ?, ?, ?, ?)");

    query->addBindValue(id_user);
    query->addBindValue(post);
    query->addBindValue(0);
    query->addBindValue(post_type);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        delete query;
        query = NULL;
        return false;
    }

    qDebug() << QString("[Database] ") + QString("Added post \"")
             + post
             + QString("\" by user ")
             + QString::number(id_user)
             + QString(" succesfully!");

    db.commit();

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::UpdatePost(int id_post, QString updated_post, int post_type)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(POSTS_TABLE_NAME) +
                    " SET post = ?, post_type = ?, date_updated = ?"
                    " WHERE id_post = ?;");
    query->addBindValue(updated_post);
    query->addBindValue(post_type);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(id_post);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed updating post!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Post updated: ") + updated_post;
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::ChangePostType(int id_post, int post_type)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(POSTS_TABLE_NAME) +
                    " SET post_type = ?"
                    " WHERE id_post = ?");
    query->addBindValue(post_type);
    query->addBindValue(id_post);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed changing post type!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Post type changed to: ") + QString::number(post_type);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::DeletePost(int id_post)
{
    query = new QSqlQuery(db);
    query->prepare( "DELETE FROM " +
                    QString(POSTS_TABLE_NAME) +
                    " WHERE id_post = ?");
    query->addBindValue(id_post);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed deleting post ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Deleted post ID: ") + QString::number(id_post);
    }

    delete query;
    query = NULL;
    return true;
}

char *DatabaseManagerMySQL::GetLastTenPosts()
{
    query = new QSqlQuery(db);

    query->prepare( "SELECT * FROM ( SELECT * FROM " +
                    QString(POSTS_TABLE_NAME) +
                    " ORDER BY id_post DESC LIMIT 50 ) AS T ORDER by id_post DESC;");

    char* rows = new char[MAX_INPUT * 32];
    memset(rows, 0, MAX_INPUT * 32);

    char sep[3] = {1,1,1};

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed getting user ID!");
        delete query;
        query = NULL;
        return 0;
    }
    else
    {
        query->first();
        strcat(rows, query->value(0).toString().toLatin1().data());
        strncat(rows, sep, 3);
       // strcat(rows, QString::number(query->value(2).toInt()).toLatin1().data());
        strcat(rows, query->value(2).toString().toLatin1().data());
        strncat(rows, sep, 3);
        while(query->next())
        {
            strcat(rows, query->value(0).toString().toLatin1().data());
            strncat(rows, sep, 3);
            strcat(rows, query->value(2).toString().toLatin1().data());
            strncat(rows, sep, 3);
        }
    }

    qDebug() << QString("[Database] ") + QString("Get last 10 posts!");

    delete query;
    query = NULL;
    return rows;
}

bool DatabaseManagerMySQL::CheckEmailPassword(QString email, QString password)
{
    query = new QSqlQuery(db);

    query->prepare( "SELECT * FROM " +
                    QString(USERS_TABLE_NAME) +
                    " WHERE email = ? and password = ?");

    QByteArray passwordHash = QCryptographicHash::hash(password.toLocal8Bit(),
                                                       QCryptographicHash::Md5);

    query->addBindValue(email);
    query->addBindValue(passwordHash);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        delete query;
        query = NULL;
        return false;
    }

    qDebug() << QString("[Database] ") + QString("Login ")
             + email
             + QString(" succesfully!");
    db.commit();

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::CreateCommentsTable()
{
    query = new QSqlQuery(db);

    if (!db.tables().contains(COMMENTS_TABLE_NAME))
    {
        query->prepare( "CREATE TABLE " + QString(COMMENTS_TABLE_NAME) + " ("
                        "id_comment MEDIUMINT NOT NULL AUTO_INCREMENT, "
                        "id_post MEDIUMINT NOT NULL, "
                        "id_user MEDIUMINT NOT NULL, "
                        "comment TEXT(1024) NOT NULL, "
                        "likes MEDIUMINT NOT NULL, "
                        "date_created DATETIME NOT NULL, "
                        "date_updated DATETIME NOT NULL, "
                        "PRIMARY KEY (id_comment));");
        if(!query->exec())
        {
            delete query;
            query = NULL;
            qDebug() << QString("[Database] ") + QString("Failed to create comments table!");
            return false;
        }
        qDebug() << QString("[Database] ") + QString("Comments table succesfully created!");
        db.commit();
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Comments table exists!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::AddComment(int id_post, int id_user, QString comment)
{
    query = new QSqlQuery(db);

    query->prepare( "INSERT INTO " +
                    QString(COMMENTS_TABLE_NAME) +
                    " (id_post, id_user, comment, likes, date_created, date_updated)"
                    " VALUES (?, ?, ?, ?, ?, ?)");

    query->addBindValue(id_post);
    query->addBindValue(id_user);
    query->addBindValue(comment);
    query->addBindValue(0);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed to add comment!");
        delete query;
        query = NULL;
        return false;
    }

    qDebug() << QString("[Database] ") + QString("Added comment \"")
             + comment
             + QString("\" by user ")
             + QString::number(id_user)
             + QString(" succesfully!");

    db.commit();

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::UpdateComment(int id_comment, QString updated_comment)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(COMMENTS_TABLE_NAME) +
                    " SET comment = ?, date_updated = ?"
                    " WHERE id_comment = ?;");
    query->addBindValue(updated_comment);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(id_comment);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed updating comment!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Comment updated: ") + updated_comment;
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::UpdateCommentLikes(int id_comment, int like)
{
    query = new QSqlQuery(db);

    query->prepare( "SELECT likes FROM " +
                    QString(COMMENTS_TABLE_NAME) +
                    " WHERE id_comment = ?;");
    query->addBindValue(id_comment);

    int likes;

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed getting number of likes for this comment!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        query->first();
        likes = query->value(0).toInt();
        qDebug() << QString("[Database] ") + QString("This comment has ")
                    + QString::number(likes)
                    + QString(" likes!");
    }

    delete query;
    query = NULL;


    likes += like;

    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(COMMENTS_TABLE_NAME) +
                    " SET likes = ?"
                    " WHERE id_comment = ?;");
    query->addBindValue(likes);
    query->addBindValue(id_comment);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed changing the number of likes for this comment!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Comment with id ")
                    + QString::number(id_comment)
                    + (" has now ")
                    + QString::number(likes)
                    + QString(" likes!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::DeleteComment(int id_comment)
{
    query = new QSqlQuery(db);
    query->prepare( "DELETE FROM " +
                    QString(COMMENTS_TABLE_NAME) +
                    " WHERE id_comment = ?");
    query->addBindValue(id_comment);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed deleting comment ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Deleted comment ID: ") + QString::number(id_comment);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::CreateMessagesTable()
{
    query = new QSqlQuery(db);

    if (!db.tables().contains(MESSAGES_TABLE_NAME))
    {
        query->prepare( "CREATE TABLE " + QString(MESSAGES_TABLE_NAME) + " ("
                        "id_message MEDIUMINT NOT NULL AUTO_INCREMENT, "
                        "id_room MEDIUMINT NOT NULL, "
                        "id_user MEDIUMINT NOT NULL, "
                        "message TEXT(1024) NOT NULL, "
                        "date_created DATETIME NOT NULL, "
                        "PRIMARY KEY (id_message));");
        if(!query->exec())
        {
            delete query;
            query = NULL;
            qDebug() << QString("[Database] ") + QString("Failed to create messages table!");
            return false;
        }
        qDebug() << QString("[Database] ") + QString("Messages table succesfully created!");
        db.commit();
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Messages table exists!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::AddMessage(int id_room, int id_user, QString message)
{
    query = new QSqlQuery(db);

    query->prepare( "INSERT INTO " +
                    QString(MESSAGES_TABLE_NAME) +
                    " (id_room, id_user, message, date_created) "
                    "VALUES (?, ?, ?, ?)");

    query->addBindValue(id_room);
    query->addBindValue(id_user);
    query->addBindValue(message);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed to add message!");
        delete query;
        query = NULL;
        return false;
    }

    qDebug() << QString("[Database] ") + QString("Added message \"")
             + message
             + QString("\" by user ")
             + QString::number(id_user)
             + QString(" succesfully!");

    db.commit();

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::DeleteMessage(int id_message)
{
    query = new QSqlQuery(db);
    query->prepare( "DELETE FROM " +
                    QString(MESSAGES_TABLE_NAME) +
                    " WHERE id_message = ?");
    query->addBindValue(id_message);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed deleting message ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Deleted post ID: ") + QString::number(id_message);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::CreateRoomsTable()
{
    query = new QSqlQuery(db);

    if (!db.tables().contains(MYSQL_ROOMS_TABLE_NAME))
    {
        query->prepare( "CREATE TABLE " + QString(MYSQL_ROOMS_TABLE_NAME) + " ("
                        "id_room MEDIUMINT NOT NULL AUTO_INCREMENT, "
                        "id_owner MEDIUMINT NOT NULL, "
                        "room_name TEXT(1024) NOT NULL, "
                        "date_created DATETIME NOT NULL, "
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

bool DatabaseManagerMySQL::AddRoom(int id_room, int id_owner, QString room_name)
{
    query = new QSqlQuery(db);

    query->prepare( "INSERT INTO " +
                    QString(MYSQL_ROOMS_TABLE_NAME) +
                    " (id_room, id_owner, room_name date_created) "
                    "VALUES (?, ?, ?, ?)");

    query->addBindValue(id_room);
    query->addBindValue(id_owner);
    query->addBindValue(room_name);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed to add room!");
        delete query;
        query = NULL;
        return false;
    }

    qDebug() << QString("[Database] ") + QString("Added room \"")
             + QString::number(id_room)
             + QString("\" by owner ")
             + QString::number(id_owner)
             + QString(" succesfully!");

    db.commit();

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::DeleteRoom(int id_room, int id_owner)
{
    query = new QSqlQuery(db);
    query->prepare( "DELETE FROM " +
                    QString(MYSQL_ROOMS_TABLE_NAME) +
                    " WHERE id_room = ? AND id_owner = ?");
    query->addBindValue(id_room);
    query->addBindValue(id_owner);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed deleting room ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Deleted room ID: ") + QString::number(id_room);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::CreateLikesTable()
{
    query = new QSqlQuery(db);

    if (!db.tables().contains(LIKES_TABLE_NAME))
    {
        query->prepare( "CREATE TABLE " + QString(LIKES_TABLE_NAME) + " ("
                        "id_post MEDIUMINT NOT NULL AUTO_INCREMENT, "
                        "id_owner MEDIUMINT NOT NULL, "
                        "date_created DATETIME NOT NULL, "
                        "PRIMARY KEY (id_post, id_owner));");
        if(!query->exec())
        {
            delete query;
            query = NULL;
            qDebug() << QString("[Database] ") + QString("Failed to create likes table!");
            return false;
        }
        qDebug() << QString("[Database] ") + QString("Likes table succesfully created!");
        db.commit();
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Likes table exists!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::AddLike(int id_post, int id_owner)
{
    query = new QSqlQuery(db);

    query->prepare( "INSERT INTO " +
                    QString(LIKES_TABLE_NAME) +
                    " (id_post, id_owner, date_created) "
                    "VALUES (?, ?, ?)");

    query->addBindValue(id_post);
    query->addBindValue(id_owner);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed to add like!");
        delete query;
        query = NULL;
        return false;
    }

    qDebug() << QString("[Database] ") + QString("Added like on post \"")
             + QString::number(id_post)
             + QString("\" by owner ")
             + QString::number(id_owner)
             + QString(" succesfully!");

    db.commit();

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::DeleteLike(int id_post, int id_owner)
{
    query = new QSqlQuery(db);
    query->prepare( "DELETE FROM " +
                    QString(LIKES_TABLE_NAME) +
                    " WHERE id_post = ? AND id_owner = ?");
    query->addBindValue(id_post);
    query->addBindValue(id_owner);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed deleting like!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Deleted like on post: ") + QString::number(id_post);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::AddUser(QString email, QString password)
{
    query = new QSqlQuery(db);

    query->prepare( "INSERT INTO " +
                    QString(USERS_TABLE_NAME) +
                    " (email, password, user_status, date_created, date_updated)"
                    " VALUES (?, ?, ?, ?, ?)");

    QByteArray passwordHash = QCryptographicHash::hash(password.toLocal8Bit(),
                                                       QCryptographicHash::Md5);

    query->addBindValue(email);
    query->addBindValue(passwordHash);
    query->addBindValue(false);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        delete query;
        query = NULL;
        return false;
    }

    qDebug() << QString("[Database] ") + QString("Added user ")
             + email
             + QString(" succesfully!");
    db.commit();

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::UpdateUserEmail(int id_user, QString email)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(USERS_TABLE_NAME) +
                    " SET email = ?, date_updated = ?"
                    " WHERE id = ?;");
    query->addBindValue(email);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(id_user);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed changing user email!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("User with id ") + QString::number(id_user)
                    + (", email changed to: ") + email;
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::UpdateUserPassword(int id_user, QString password)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(USERS_TABLE_NAME) +
                    " SET password = ?, date_updated = ?"
                    " WHERE id = ?;");

    QByteArray passwordHash =
            QCryptographicHash::hash(
                password.toLocal8Bit(),
                QCryptographicHash::Md5);

    query->addBindValue(passwordHash);
    query->addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query->addBindValue(id_user);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed changing user password!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("User with id ")
                    + QString::number(id_user)
                    + (", password changed!");
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::DeleteUser(int id_user)
{
    query = new QSqlQuery(db);
    query->prepare( "DELETE FROM " +
                    QString(USERS_TABLE_NAME) +
                    " WHERE id = ?");
    query->addBindValue(id_user);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed deleting user ID!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("Deleted user ID: ") + QString::number(id_user);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::ChangeUserStatus(int id_user, bool logged)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(USERS_TABLE_NAME) +
                    " SET user_status = ?"
                    " WHERE id = ?");
    query->addBindValue(logged);
    query->addBindValue(id_user);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed changing user status!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("User status changed to: ")
                    + QString::number(logged);
    }

    delete query;
    query = NULL;
    return true;
}

bool DatabaseManagerMySQL::ChangeUserType(int id_user, int user_type)
{
    query = new QSqlQuery(db);

    query->prepare( "UPDATE " +
                    QString(USERS_TABLE_NAME) +
                    " SET admin = ?"
                    " WHERE id = ?");
    query->addBindValue(user_type);
    query->addBindValue(id_user);

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed changing user type!");
        delete query;
        query = NULL;
        return false;
    }
    else
    {
        qDebug() << QString("[Database] ") + QString("User type changed to: ")
                    + QString::number(user_type);
    }

    delete query;
    query = NULL;
    return true;
}

int DatabaseManagerMySQL::GetUserId(QString email)
{
    query = new QSqlQuery(db);

    query->prepare( "SELECT id FROM " +
                    QString(USERS_TABLE_NAME) +
                    " WHERE email = ?;");
    query->addBindValue(email);

    int userID = 0;

    if(!query->exec())
    {
        qDebug() << QString("[Database] ") + query->lastError().text();
        qDebug() << QString("[Database] ") + QString("Failed getting user ID!");
        delete query;
        query = NULL;
        return 0;
    }
    else
    {
        query->first();
        userID = query->value(0).toInt();
        qDebug() << QString("[Database] ") + QString("User ID: ")
                    + QString::number(userID);
    }

    delete query;
    query = NULL;
    return userID;
}

DatabaseManagerMySQL::~DatabaseManagerMySQL()
{
    if(query != NULL)
    {
        delete query;
        query = NULL;
    }
    if(db.isOpen())
    {
        db.close();
    }
}
