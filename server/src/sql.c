#include "sql.h"
#include <stdlib.h>

sqlite3 *db;

int createSqlConnection(void) {


    char *err_msg = 0;
    int rc = sqlite3_open("user.db", &db);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    char *sql = "DROP TABLE IF EXISTS Users;"
                "CREATE TABLE Users(id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT, ip TEXT, port INT);"
                "INSERT INTO Users (username, password, ip, port) VALUES('test','pass','127.0.0.1', 4444);"
                "INSERT INTO Users (username, password, ip, port) VALUES('test1','pass1','127.0.0.1', 4444);"
                "INSERT INTO Users (username, password, ip, port) VALUES('test2','pass2','127.0.0.1', 4444);";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {

        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        closeSQLConnection();
        return 1;
    }



    return 0;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {

    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

    return 0;
}

sqlite3_stmt* getUserData(char* sqlStatement){

    char *err_msg;
    int rc ;//= sqlite3_exec(db, sqlStatement, callback, 0, &err_msg);

   if (rc != SQLITE_OK ) {

       fprintf(stderr, "Failed to select data\n");
       fprintf(stderr, "SQL error: %s\n", err_msg);

       sqlite3_free(err_msg);
       closeSQLConnection();
       exit(1);
       return NULL;
   }
    sqlite3_stmt* res;
    size_t t_len = strlen(sqlStatement);
    const char *error;
    rc = sqlite3_prepare_v2(db, sqlStatement, t_len, &res, &error);

    if (rc == SQLITE_OK){
//        int step = sqlite3_step(res);
//        if (step == SQLITE_ROW) {
//
//            printf("%s: ", sqlite3_column_text(res, 0));
//            printf("%s\n", sqlite3_column_text(res, 1));
//            printf("%s: ", sqlite3_column_text(res, 2));
//            printf("%s\n", sqlite3_column_text(res, 3));
//
//        int step = sqlite3_step(res);
//            printf("%s: ", sqlite3_column_text(res, 0));
//            printf("%s\n", sqlite3_column_text(res, 1));
//            printf("%s: ", sqlite3_column_text(res, 2));
//            printf("%s\n", sqlite3_column_text(res, 3));
//
//        }
       return res;
    }
    else
    {
        // zErrMsg may have an error message. If not, the database object
        // may have one. Either way, you can find out what went wrong.
        const char* error = sqlite3_errmsg(db);
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", error);

        sqlite3_free((void *)error);
        return NULL;
    }

}
void closeSQLConnection(){

    sqlite3_close(db);
}
