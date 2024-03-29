#include "../include/cmd.h"

void get_file_name(char* file_name, const char* cmd_path)
{
    int len = strlen(cmd_path);
    while (cmd_path[len] != '/' && len != -1)  // home/zhanghaonan/1.cc
    {
        len--;
    }
    len++;//1.cc
    int i = 0;
    while (cmd_path[len] != '\0')
    {
        file_name[i++] = cmd_path[len++];
    }
    file_name[i] = '\0';
}

char* convert_path(const char* path, MYSQL* conn, const char* root_id, const char* cur_dir_id)
{
    char* abs_path = (char*)malloc(RESULT_LEN);// 1000
    MYSQL_RES* res;             //sql_select类型
    MYSQL_ROW row;        
    if (path[0] == '/')         //strat with user root dir
    {
        res = sql_select(conn, "file", "id", root_id, 0);
        row = mysql_fetch_row(res);//数从结果集中取得一行，并作为枚举数组返回。
        mysql_free_result(res);//在我们执行完 SELECT 语句后，释放游标内存是一个很好的习惯。可以通过 PHP 函数 mysqli_free_result() 来实现内存的释放。
        strcpy(abs_path, row[5]);//C 库函数 char *strcpy(char *dest, const char *src) 把 src 所指向的字符串复制到 dest。
        if (strcmp(path, "/") == 0)
        {
            return abs_path;
        }
        strcat(abs_path, path);//把 src 所指向的字符串追加到 dest 所指向的字符串的结尾。
        return abs_path;
    }
    if (path[0] == '.' && path[1] == '.')       //start with parent dir
    {
        res = sql_select(conn, "file", "id", cur_dir_id, 0);//??sql_select()
        row = mysql_fetch_row(res);
        mysql_free_result(res);
        res = sql_select(conn, "file", "id", row[1], 0);//row[1]??
        row = mysql_fetch_row(res);
        mysql_free_result(res);
        if (atoi(row[1]) == -1)//把参数 str 所指向的字符串转换为一个整数（类型为 int 型）。
        {
            free(abs_path);
            return NULL;
        }
        strcpy(abs_path, row[5]);//row[5]
        if (strcmp(path, "..") == 0 || strcmp(path, "../") == 0)
        {
            return abs_path;
        }
        if (path[2] == '/')
        {
            char new_path[RESULT_LEN];
            int len = strlen(path);
            for (int i = 0; i + 3 <= len; i++)// 
            {
                new_path[i] = path[i + 3];//直接倒退../
            }
            convert_path(new_path, conn, root_id, row[0]);
        }
    }
    else        //start with cur dir
    {
        res = sql_select(conn, "file", "id", cur_dir_id, 0);
        row = mysql_fetch_row(res);
        mysql_free_result(res);
        if (strcmp(path, "./") == 0 || strcmp(path, ".") == 0)
        {
            strcpy(abs_path, row[5]);
            return abs_path; //cur_dir_id
        }
        if (path[0] == '.' && path[1] == '/')
        {
            char new_path[RESULT_LEN];
            int len = strlen(path);
            for (int i = 0; i + 2 <= len; i++)
            {
                new_path[i] = path[i + 2];//将./干掉
            }
            sprintf(abs_path, "%s/%s", row[5], new_path);
            return abs_path;
        }
        sprintf(abs_path, "%s/%s", row[5], path);
        return abs_path;
    }
}

int resolve_ls(char*** result, int *n, const char* path, MYSQL* conn, const char* cur_dir_id, const char* root_id)
{
    int i;
    char* abs_path;
    MYSQL_RES* res;
    MYSQL_ROW row;
    if (strlen(path) == 0)   //only ls
    {
        res = sql_select(conn, "file", "dir_id", cur_dir_id, 0);
        if (res == NULL)
        {
            *n = 0;
            *result = NULL;
            return 1;
        }
    }
    else        //ls [FILE]
    {
        abs_path = convert_path(path, conn, root_id, cur_dir_id);       //free after use
        if (abs_path == NULL)
        {
            return -1;
        }
        res = sql_select(conn, "file","file_path", abs_path, 0);
        free(abs_path);
        abs_path = NULL;
        if (res == NULL)
        {
            return -1;
        }

        row = mysql_fetch_row(res);
        if (atoi(row[2]) == 0)  //is dir
        {
            mysql_free_result(res);
            res = sql_select(conn, "file", "dir_id", row[0], 0);
            if (res == NULL)        //empty dir
            {
                *n = 0;
                *result = NULL;
                return 1;
            }
        }
        else    //is file
        {
            *n = 1;
            *result = (char**)malloc(sizeof(char*));
            (*result)[0] = (char*)malloc(RESULT_LEN);
            sprintf((*result)[0], "%-30s%s", row[3], row[4]);
            mysql_free_result(res);
            return 1;
        }
    }

    *n = mysql_num_rows(res);
    *result = (char**)malloc(*n * sizeof(char*));
    for (i = 0; i < *n; i++)
    {
        (*result)[i] = (char*)malloc(RESULT_LEN);
        row = mysql_fetch_row(res);
        //whether file is dir
        if (atoi(row[2]) == 0)
        {
            sprintf((*result)[i], "%-30s%s", row[3], "dir");
        }
        else
        {
            sprintf((*result)[i], "%-30s%s", row[3], row[4]);
        }
    }
    mysql_free_result(res);
    return 1;
}

int resolve_pwd(char*** result, int *n, MYSQL* conn, const char* cur_dir_id, int name_len)
{
    MYSQL_RES* res;
    MYSQL_ROW row;
    res = sql_select(conn, "file", "id",  cur_dir_id, 0);
    row = mysql_fetch_row(res);
    *n = 1;
    *result = (char**)malloc(sizeof(char*));
    (*result)[0] = (char*)malloc(RESULT_LEN);
    if (row[5][8 + name_len + 1] == '\0')
    {
        mysql_free_result(res);
        strcpy((*result)[0], "/");
        return 1;
    }
    else
    {
        for (int i = 8 + name_len + 1; row[5][i - 1] != '\0'; i++)
        {
            (*result)[0][i - 8 - name_len - 1] = row[5][i];
        }
        mysql_free_result(res);
    return 1;
    }
}

int resolve_cd(char*** result, int *n, const char* cmd_path, MYSQL* conn, char* cur_dir_id, const char* root_id)
{
    MYSQL_RES* res;
    MYSQL_ROW row;
    char* abs_path;
    abs_path = convert_path(cmd_path, conn, root_id, cur_dir_id);
    if (abs_path == NULL)
    {
        return -2;
    }
    res = sql_select(conn, "file", "file_path", abs_path, 0);
    free(abs_path);
    abs_path = NULL;
    if (res == NULL)
    {
        return -2;
    }

    row = mysql_fetch_row(res);
    mysql_free_result(res);
    if (atoi(row[2]) == 0)      //is dir
    {
        strcpy(cur_dir_id, row[0]);
        int ret;
        ret = resolve_ls(result, n, "", conn, cur_dir_id, root_id);
        return ret;
    }
    else    //is file
    {
        return -2;
    }
}

int resolve_mkdir(char*** result, int *n, const char* user_name, const char* cmd_path, MYSQL* conn, const char* cur_dir_id, const char* root_id)
{
    int ret;
    MYSQL_RES* res;

    char file_name[FILE_NAME_LEN];
    get_file_name(file_name, cmd_path);

    char* abs_path;
    abs_path = convert_path(cmd_path, conn, root_id, cur_dir_id);
    if (abs_path == NULL)
    {
        return -4;
    }
    res = sql_select(conn, "file", "file_path", abs_path, 0);
    free(abs_path);
    abs_path = NULL;
    if (res == NULL)
    {
        ret = sql_insert_file_trans(conn, user_name, cur_dir_id, 0, file_name, 0, NULL);
        if (ret == -1)
        {
            return -4;
        }
        ret = resolve_ls(result, n, "", conn, cur_dir_id, root_id);
        return ret;
    }
    else
    {
        mysql_free_result(res);
        return -4;
    }
}

int resolve_gets(char* file_md5, char* file_name, char* file_size, const char* path, MYSQL* conn, const char* root_id, const char* cur_dir_id)
{
    char* abs_path;
    MYSQL_RES* res;
    MYSQL_ROW row;

    abs_path = convert_path(path, conn, root_id, cur_dir_id);
    if (abs_path == NULL)
    {
        return -1;
    }
    res = sql_select(conn, "file", "file_path", abs_path, 0);
    free(abs_path);
    abs_path = NULL;
    if (res == NULL)
    {
        return -1;
    }

    row = mysql_fetch_row(res);
    mysql_free_result(res);
    if (atoi(row[2]) == 0)          //is dir
    {
        //to be complete
        return -1;
    }
    else        //is file
    {
        strcpy(file_size, row[4]);
        strcpy(file_name, row[3]);
        strcpy(file_md5, row[6]);
        return 1;
    }
}

int resolve_puts(const char* cmd_path, MYSQL* conn, const char* root_id, const char* cur_dir_id)
{
    MYSQL_RES* res;
    char* abs_path;

    char file_name[FILE_NAME_LEN];
    get_file_name(file_name, cmd_path);

    abs_path = convert_path(file_name, conn, root_id, cur_dir_id);
    if (abs_path == NULL)
    {
        return -1;
    }
    res = sql_select(conn, "file", "file_path", abs_path, 0);
    free(abs_path);
    abs_path = NULL;
    if (res == NULL)
    {
        return 0;
    }
    else
    {
        mysql_free_result(res);
#ifdef _DEBUG
        printf("insert file failed: file exist\n");
#endif
        return -1;          //file exist
    }
}

int resolve_rm(const char* cmd_path, int abs_flag, MYSQL* conn, const char* user_name, const char* root_id, const char* cur_dir_id)
{
    char* abs_path;
    int ret, num;
    MYSQL_RES* res;
    MYSQL_ROW row;

    if (abs_flag == 0)
    {
        abs_path = convert_path(cmd_path, conn, root_id, cur_dir_id);
    }
    else
    {
        abs_path = (char*)malloc(RESULT_LEN);
        strcpy(abs_path, cmd_path);
    }
    if (abs_path == NULL)
    {
        return -3;
    }
    res = sql_select(conn, "file", "file_path", abs_path, 0);
    if (res == NULL)
    {
        return -3;
    }

    row = mysql_fetch_row(res);
    mysql_free_result(res);
    if (atoi(row[2]) == 0)      //is dir
    {
        ret = sql_delete_file(conn, user_name, abs_path);
        if (ret == -1)
        {
            return -3;
        }
        char regexp[QUERY_LEN] = "^";
        strcat(regexp, abs_path);
        free(abs_path);
        abs_path = NULL;
        res = sql_select(conn, "file", "file_path", regexp, 1);
        if (res == NULL)
        {
            return 3;
        }
        num = mysql_num_rows(res);
        for (int i = 0; i < num; i++)
        {
            row = mysql_fetch_row(res);
            resolve_rm(row[5], 1, conn, user_name, root_id, cur_dir_id);
        }
        mysql_free_result(res);
        return 3;
    }
    else            //is file
    {
        char file_md5[MD5_LEN];
        strcpy(file_md5, row[6]);
        res = sql_select(conn, "file", "file_md5", file_md5, 0);
        num = mysql_num_rows(res);
        mysql_free_result(res);

        ret = sql_delete_file(conn, user_name, abs_path);
        free(abs_path);
        abs_path = NULL;
        if (ret == -1)
        {
            return -3;
        }

        if(num == 1)       //last file
        {
            char path_name[RESULT_LEN] = "../netdisk/";
            strcat(path_name, file_md5);
            ret = remove(path_name);
            if (ret == -1)
            {
                return -3;
            }
#ifdef _DEBUG
            printf("%s is removed from disk\n", path_name);
#endif
            return 3;
        }
    }
}

void cmd_interpret(const char*cmd, char* prefix, char* cmd_path)
{
    int i = 0, j, k;
    while (cmd[i] != ' ' && cmd[i] != '\0')
    {
        i++;
    }
    if (cmd[i] == ' ')
    {
        strncpy(prefix, cmd, i);
        prefix[i] = '\0';
        j = i;
        while (cmd[j] != '\0')
        {
            j++;
        }
        int len = j - i;
        for (k = 0; k < len; k++, i++)
        {
            cmd_path[k] = cmd[i + 1];
        }
    }
    else
    {
        strcpy(prefix, cmd);
        cmd_path[0] = '\0';
    }
#ifdef _DEBUG
    printf("prefix: %s\n", prefix);
    printf("cmd_path: %s\n", cmd_path);
#endif
}
