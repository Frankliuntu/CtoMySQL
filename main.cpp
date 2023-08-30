#include <iostream>
#include <mysql.h>
#include <map>


//define some macro for mysql_real_connect
#define HOST "localhost"
#define USERNAME "leavevm0cl6"
#define PASSWORD "R08633005@ntu.edu.tw"//please change into your password
#define DATABASE "demo"

//enum for operate CRUD
enum CRUD {
    INSERT,
    SELECT,
    UPDATE,
    DELETE,
    ERROR
};


#pragma region Function Declaration

//print SELECT result
void PrintRowResult(MYSQL_RES *res_ptr);

//change query value into enum for further CRUD
CRUD ToEnum(char *Query);


#pragma endregion Function Declaration


int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "only take one sql clause" << std::endl;
        exit(1);
    }
    std::cout << argv[1] << std::endl;


    MYSQL my_connection;
    MYSQL_RES *res_ptr;
    int result;
    mysql_init(&my_connection);

    /*  mysql: This is a pointer to a MYSQL structure that will be filled with connection information.
     *         It's typically created using the mysql_init() function.
     *
     *  host: The hostname or IP address of the MySQL server you want to connect to.
     *
     *  user: The MySQL user name that will be used for authentication.
     *
     *  passwd: The password for the MySQL user.
     *
     *  db: The name of the MySQL database you want to connect to.
     *
     *  port: The port number on which the MySQL server is running.
     *        If set to 0, the default MySQL port (usually 3306) is used.
     *
     *  unix_socket: The path to a Unix socket that can be used to connect to the MySQL server.
     *               This can be set to NULL to use TCP/IP for the connection.
     *
     *  client_flag: A set of flags that determine various client-side behavior.
     *               These flags are combined using bitwise OR operations. Some common flags include:
     *               CLIENT_FOUND_ROWS: Return the number of found (matched) rows,
     *               not the number of affected rows, in mysql_affected_rows().
     * */

    /*The `client_flag` parameter in the `mysql_real_connect()` function is used to set various client-side flags that control the behavior of the MySQL client library. These flags are used to customize how the client interacts with the MySQL server during the connection and subsequent interactions. Here are some common `client_flag` values and their meanings:

1. CLIENT_FOUND_ROWS: This flag instructs MySQL to return the number of found (matched) rows instead of the number of affected rows in statements like `UPDATE`, `DELETE`, and `INSERT ... ON DUPLICATE KEY UPDATE`.

2. CLIENT_IGNORE_SIGPIPE: This flag tells MySQL to ignore the SIGPIPE signal that can be generated when writing to a closed socket. Using this flag can help prevent your application from crashing if the server disconnects unexpectedly.

3. CLIENT_MULTI_RESULTS: This flag indicates that the client supports multiple result sets for certain types of queries, such as stored procedures or queries that return multiple result sets.

4. CLIENT_MULTI_STATEMENTS: This flag enables the client to send multiple statements in a single string, separated by semicolons. The server executes each statement in sequence and returns multiple result sets.

5. CLIENT_REMEMBER_OPTIONS: This flag instructs MySQL to store connection options in the `MYSQL` structure. This can be useful if you need to reconnect later using the same options.

6. CLIENT_SSL: This flag indicates that the client wants to use SSL encryption for the connection to the server.

7. CLIENT_TRANSACTIONS: This flag enables the use of transactions. It's used in conjunction with the `BEGIN`, `COMMIT`, and `ROLLBACK` statements.

8. CLIENT_COMPRESS: This flag indicates that the client supports compression for the communication with the server.

These are just a few examples of the available `client_flag` values. The actual set of available flags might vary depending on the version of the MySQL client library you're using.

You can combine multiple flags using the bitwise OR operator (`|`). For example, if you want to set both `CLIENT_FOUND_ROWS` and `CLIENT_SSL`, you would use `client_flag = CLIENT_FOUND_ROWS | CLIENT_SSL`.

When setting `client_flag`, you're essentially specifying certain behaviors and features you want the MySQL client library to use during the connection and subsequent interactions with the MySQL server.
     */

    if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, DATABASE, 0, NULL, CLIENT_FOUND_ROWS)) {
        std::cout << "connection successful" << std::endl;
        CRUD returnCRUD = ToEnum(argv[1]);

        if (returnCRUD == ERROR) {
            std::cout << "Error input" << std::endl;
            mysql_close(&my_connection);
            exit(1);
        }

        result = mysql_query(&my_connection, argv[1]);

        if (!result) {
            switch (returnCRUD) {
                case INSERT: {
                    std::cout << "Insert successfully" << std::endl;
                    //mysql_affected_rows only work on "INSERT,UPDATE,DELETE
                    std::cout << mysql_affected_rows(&my_connection) << " row be affected" << std::endl;
                    break;
                }
                case SELECT: {
                    res_ptr = mysql_store_result(&my_connection);
                    if (res_ptr) {
                        PrintRowResult(res_ptr);
                    }
                    break;
                }
                case UPDATE: {
                    std::cout << "Update successfully" << std::endl;
                    std::cout << mysql_affected_rows(&my_connection) << " row be affected" << std::endl;
                    break;
                }
                case DELETE: {
                    std::cout << "Delete successfully" << std::endl;
                    std::cout << mysql_affected_rows(&my_connection) << " row be affected" << std::endl;
                    break;
                }
                default:
                    std::cout << "Unpredictable ERROR" << std::endl;
            }
        } else {
            const char *errorMSG = mysql_error(&my_connection);//mysql_error()fetch the errors from mysql
            switch (returnCRUD) {
                case INSERT: {
                    std::cout << "INSERT failed" << std::endl;
                    std::cerr << "ErrorMSG: " << errorMSG << std::endl;
                    break;
                }
                case SELECT: {
                    std::cout << "SELECT failed" << std::endl;
                    std::cerr << "ErrorMSG: " << errorMSG << std::endl;
                    break;
                }
                case UPDATE: {
                    std::cout << "UPDATE failed" << std::endl;
                    std::cerr << "ErrorMSG: " << errorMSG << std::endl;
                    break;
                }
                case DELETE: {
                    std::cout << "DELETE failed" << std::endl;
                    std::cerr << "ErrorMSG: " << errorMSG << std::endl;
                    break;
                }
                default: {
                    std::cerr << "ErrorMSG: " << errorMSG << std::endl;
                    break;
                }
            }
        }


    } else {
        std::cerr << "connection error occurred" << std::endl;
        mysql_close(&my_connection);
    }
    mysql_close(&my_connection);


    return 0;
}

#pragma region Function Implementation

void PrintRowResult(MYSQL_RES *res_ptr) {

    MYSQL_FIELD *result_field;
    while (result_field = mysql_fetch_field(res_ptr)) {
        std::cout << result_field->name << "\t";
    }
    std::cout << std::endl;
    std::cout << std::endl;


    int row = mysql_num_rows(res_ptr);
    int column = mysql_num_fields(res_ptr);

    for (int i = 1; i < row + 1; i++) {
        MYSQL_ROW result_row = mysql_fetch_row(res_ptr);
        for (int j = 0; j < column; j++) {
            if (result_row[j] == NULL)
                std::cout << "NULL" << "\t";
            else
                std::cout << result_row[j] << "\t";

        }
        std::cout << std::endl;


    }
    std::cout << row << " rows in set" << std::endl;
}

CRUD ToEnum(char *Query) {

    if ((strstr(Query, "INSERT")))
        return INSERT;
    else if ((strstr(Query, "SELECT")))
        return SELECT;
    else if ((strstr(Query, "UPDATE")))
        return UPDATE;
    else if ((strstr(Query, "DELETE")))
        return DELETE;
    else
        return ERROR;
}

#pragma endregion Function Implementation