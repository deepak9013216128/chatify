#ifndef SQL_H
#define SQL_H

#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

int createSqlConnection(void);
sqlite3_stmt *getUserData(char *sqlStatement);
void closeSQLConnection();
#endif // SQL_H
