// https://stackoverflow.com/questions/9367624/can-i-make-two-columns-unique-to-each-other

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#define DB_MYSQL_TYPE "QMYSQL"
#define DB_PORT 25
#define DB_NAME "test"
#define USERS_TABLE_NAME "users"
#define POSTS_TABLE_NAME "posts"
#define COMMENTS_TABLE_NAME "comments"
#define MESSAGES_TABLE_NAME "messages"
#define MYSQL_ROOMS_TABLE_NAME "rooms"
#define LIKES_TABLE_NAME "likes"

#define HOST_NAME "localhost"
#define USER_NAME "root"
#define DB_PASSWORD ""

#include "databasemanagersqlite.h"
#include "qtheaders.h"

class DatabaseManagerMySQL
{
public:
    DatabaseManagerMySQL();
    ~DatabaseManagerMySQL();

    bool CreateUsersTable();
    bool AddUser(QString email, QString password);
    bool UpdateUserEmail(int id_user, QString email);
    bool UpdateUserPassword(int id_user, QString password);
    bool ChangeUserStatus(int id_user, bool logged);
    bool ChangeUserType(int id_user, int user_type); // 0 - usual, 1 - admin
    bool DeleteUser(int id_user);
    int GetUserId(QString email);
    bool CheckEmailPassword(QString email, QString password);

    bool CreatePostsTable();
    bool AddPost(int id_user, QString post, int post_type);
    bool UpdatePost(int id_post, QString updated_post, int post_type);
    bool ChangePostType(int id_post, int post_type);
    bool DeletePost(int id_post);
    char *GetLastTenPosts();

    bool CreateCommentsTable();
    bool AddComment(int id_post, int id_user, QString comment);
    bool UpdateComment(int id_comment, QString updated_comment);
    bool UpdateCommentLikes(int id_comment, int like); // add or decrease by one
    bool DeleteComment(int id_comment);

    bool CreateMessagesTable();
    bool AddMessage(int id_room, int id_user, QString message);
    bool DeleteMessage(int id_message);

    bool CreateRoomsTable();
    bool AddRoom(int id_room, int id_owner, QString room_name);
    bool DeleteRoom(int id_room, int id_owner);

    bool CreateLikesTable();
    bool AddLike(int id_post, int id_owner); // primary key being boths args
    bool DeleteLike(int id_post, int id_owner);

    bool CreateConnection();
    bool CloseConnection();
private:
    QSqlQuery* query = NULL;
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
