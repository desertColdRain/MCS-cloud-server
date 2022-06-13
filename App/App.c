#include <stdlib.h>
#include "sgx_urts.h"
#include <time.h>
#include "../build/Enclave_u.h"
#include "App.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <mysql/mysql.h>

#define MAX_BUF_LEN 100
#define DATA_SIZE (5 * 1024 * 1024)
#define BLOCK_SIZE (100 * 1024)
#define PORT 8888 //端口号
#define BACKLOG 5 //最大监听数
static char *server_groups[] = {"embedded", "server", "this_program_server", (char *)NULL};
sgx_enclave_id_t global_eid = 0;
MYSQL mysql;
MYSQL_RES *res;
MYSQL_ROW row;
char *host = "localhost";
char *user = "debian-sys-maint";
char *password = "123456";
char *database = "mcs_blockchain";

void o_print_str(const char *str)
{
        printf("%s", str);
}

int initialize_enclave(void)
{
        sgx_launch_token_t token = {0};
        sgx_status_t ret = SGX_ERROR_UNEXPECTED;
        int updated = 0;

        /* Call sgx_create_enclave to initialize an enclave instance */
        /* Debug Support: set 2nd parameter to 1 */
        ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
        if (ret != SGX_SUCCESS)
        {
                printf("main: error %#x,failed to create enclave\n", ret);
                return -1;
        }
        return 0;
}
// using socket to communicate with miner
void socket_server()
{

        sgx_status_t re = SGX_ERROR_UNEXPECTED;
        int iSocketFD = 0; // socket句柄
        int iRecvLen = 0;  //接收成功后的返回值
        int new_fd = 0;    //建立连接后的句柄

        struct sockaddr_in stLocalAddr = {0};  //本地地址信息结构图，下面有具体的属性赋值
        struct sockaddr_in stRemoteAddr = {0}; //对方地址信息
        socklen_t socklen = 0;

        iSocketFD = socket(AF_INET, SOCK_STREAM, 0); //建立socket
        if (0 > iSocketFD)
        {
                printf("创建socket失败！\n");
                return 0;
        }

        stLocalAddr.sin_family = AF_INET;                /*该属性表示接收本机或其他机器传输*/
        stLocalAddr.sin_port = htons(PORT);              /*端口号*/
        stLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY); /*IP，括号内容表示本机IP*/

        //绑定地址结构体和socket
        if (0 > bind(iSocketFD, (void *)&stLocalAddr, sizeof(stLocalAddr)))
        {
                printf("绑定失败！\n");
                return 0;
        }

        //开启监听 ，第二个参数是最大监听数
        if (0 > listen(iSocketFD, BACKLOG))
        {
                printf("监听失败！\n");
                return 0;
        }

        printf("iSocketFD: %d\n", iSocketFD);
        //在这里阻塞知道接收到消息，参数分别是socket句柄，接收到的地址信息以及大小
        while (1)
        {
                char buf[4096] = {0}; //
                new_fd = accept(iSocketFD, (void *)&stRemoteAddr, &socklen);
                // printf("Hello, let us watch for the signal%c\n",new_fd);
                if (0 > new_fd)
                {
                        printf("接收失败！\n");
                        return 0;
                }
                else
                {
                        printf("接收成功！\n");
                        //发送内容，参数分别是连接句柄，内容，大小，其他信息（设为0即可）
                        int flag = 1;
                        iRecvLen = recv(new_fd, buf, sizeof(buf), 0);
                        printf("new_fd: %d\n", new_fd);
                        printf("buf [0]%c\n:", buf[0]);
                        if (buf[0] == 'Z')
                        {
                                int j = 0;
                                char *miner_pk;
                                char *miner_sig;
                                char *cloud_pk;
                                char *message;
                                printf("This is a account registration signal\n");
                                const char *delim = "*";
                                char *token;
                                char *signal;
                                char *username;
                                // 获取第一个子字符串
                                token = strtok(buf, delim);
                                /* 继续获取其他的子字符串 */
                                while (token != NULL)
                                {
                                        if (j == 0)
                                                signal = token;
                                        if (j == 1)
                                                miner_pk = token;
                                        if (j == 2)
                                                miner_sig = token;
                                        if (j == 3)
                                                message = token;
                                        if (j == 4)
                                                cloud_pk = token;
                                        if (j == 5)
                                                username = token;
                                        // printf("Str = %s\n", token);
                                        j++;
                                        token = strtok(NULL, delim);
                                }
                                printf("username:%s\n", username);
                                printf("miner pk:%s\n", miner_pk);
                                printf("miner sig:%s\n", miner_sig);
                                printf("cloud pk:%s\n", cloud_pk);
                                // notes: the function in enclave.c return values is re, not the ret = verify_signature(global_eid,&re,message,miner_pk,miner_sig);
                                verify_signature(global_eid, &re, message, miner_pk, miner_sig);
                                if (re == SGX_SUCCESS)
                                {
                                        printf("The first step that verify signature completed!\n");
                                        // if the key is in revoke list, send NO to client
                                        if (key_in_revoke(miner_pk) == 0)
                                        {
                                                printf("The public key has already existed in revoke key list\n");
                                                send(new_fd, "SB", sizeof("SB"), 0);
                                                printf("verify failed");
                                        }
                                        else
                                        {
                                                printf("The public key does not appeared in revoke list\n");
                                                // otherwise check whether ket is in alteration key list
                                                if (key_in_alteration(miner_pk) == 0)
                                                {
                                                        printf("The public key has already exist in the alteration list!\n");
                                                        send(new_fd, "SB", sizeof("SB"), 0);
                                                        printf("verify failed");
                                                }
                                                else
                                                {
                                                        printf("The public key does not appeard in key alteration list! Congratulations, you have passed the authentication!\n");
                                                        send(new_fd, "OK", sizeof("OK"), 0);
                                                        printf("verify successful\n");
                                                        char sqlcmd[1024];
                                                        char *str1 = "insert into alteration_key_list values(\"";
                                                        sprintf(sqlcmd, "%s%s%s%s%s", str1, username, "\",\"", miner_pk, "\",1.1)");
                                                        printf("sqlcmd:%s\n", sqlcmd);
                                                        query_data(sqlcmd, mysql);
                                                        printf("end\n");
                                                }
                                        }
                                }
                                // verify signatures failed
                                else
                                {
                                        send(new_fd, "SB", sizeof("SB"), 0);
                                        printf("verify failed");
                                }
                        }
                        // the signal of key alteration
                        else if (buf[0] == 'Y')
                        {
                                printf("This is a key alteration signal\n");
                                int j = 0;
                                char *old_pk;
                                char *alteration_sig;
                                char *new_pk;
                                char *alteration_message;
                                const char *delim = "*";
                                char *token;
                                char *signal;
                                char *cloud;
                                char *username;
                                // 获取第一个子字符串
                                token = strtok(buf, delim);
                                /* 继续获取其他的子字符串 */
                                while (token != NULL)
                                {
                                        if (j == 0)
                                                signal = token;
                                        if (j == 1)
                                                old_pk = token;
                                        if (j == 2)
                                                new_pk = token;
                                        if (j == 3)
                                                alteration_sig = token;
                                        if (j == 4)
                                                alteration_message = token;
                                        if (j == 5)
                                                cloud = token;
                                        if (j == 6)
                                                username = token;
                                        // printf("Str = %s\n", token);
                                        j++;
                                        token = strtok(NULL, delim);
                                }
                                printf("Old pk after decode %s\n", old_pk);
                                printf("New pk after decode %s\n", new_pk);
                                printf("Alteration sig after decode %s\n", alteration_sig);
                                printf("Alteration message after decode %s\n", alteration_message);
                                printf("Cloud pk after decode %s\n", cloud);
                                printf("Username after decode %s\n", username);
                                // notes: the function in enclave.c return values is re, not the ret = verify_signature(global_eid,&re,message,miner_pk,miner_sig);
                                // sb sgx
                                verify_signature(global_eid, &re, alteration_message, old_pk, alteration_sig);
                                if (re == SGX_SUCCESS)
                                {
                                        // whether old pk in revoke list
                                        printf("Your signature has been verifyed!\n");
                                        if (key_in_revoke(old_pk) == 1)
                                        {
                                                printf("There is not exist in revoke list of old pk!\n");
                                                // if key in revoke list, check whether pld pk in alteration key list, if exists, update public key in alteration list
                                                // and put old key into revoke list if not return false
                                                if (key_in_alteration(old_pk) == 0)
                                                {
                                                        printf("The old key exist in alteration key list!\n");
                                                        char sqlcmd1[1024];
                                                        sprintf(sqlcmd1, "%s%s%s%s%s", "update alteration_key_list set public_key=\"", new_pk, "\" where username=\"",
                                                                username, "\"");
                                                        printf("sqlcmd:%s\n", sqlcmd1);
                                                        query_data(sqlcmd1, mysql);
                                                        char sqlcmd2[1024];
                                                        sprintf(sqlcmd2, "%s%s%s%s%s", "insert into revoke_key_list values(\"", username, "\",\"", old_pk, "\")");
                                                        printf("sqlcmd2 %s\n", sqlcmd2);
                                                        query_data(sqlcmd2, mysql);
                                                        send(new_fd, "OK", sizeof("OK"), 0);
                                                        printf("verify successful\n");
                                                }
                                        }
                                        // whether pk is in the revoke key list
                                }
                                else
                                {
                                        printf("The old pk exist in revoke key list!\n");
                                        send(new_fd, "SB", sizeof("SB"), 0);
                                        printf("verify failed");
                                }
                        }
                        // the signal of applying a task, anonymous authentication
                        else if (buf[0] == 'A')
                        {
                                printf("This is a key alteration signal\n");
                                int j = 0;
                                // the hash of task content
                                char *contentHash;
                                // the public key of applier
                                char *applier_public_key;
                                // the signature of contentHash by applier
                                char *applier_signature;
                                // the requirements of trust
                                float trust_requirements;
                                // username
                                char *username;
                                const char *delim = "*";
                                char *token;
                                char *signal;
                                // 获取第一个子字符串
                                token = strtok(buf, delim);
                                /* 继续获取其他的子字符串 */
                                while (token != NULL)
                                {
                                        if (j == 0)
                                                signal = token;
                                        if (j == 1)
                                                contentHash = token;
                                        if (j == 2)
                                                applier_public_key = token;
                                        if (j == 3)
                                                applier_signature = token;
                                        if (j == 4)
                                                trust_requirements = atof(token);
                                        if (j == 5)
                                                username = token;
                                        // printf("Str = %s\n", token);
                                        j++;
                                        token = strtok(NULL, delim);
                                }
                                printf("The hash of content%s\n", contentHash);
                                printf("The applier public key: %s\n", applier_public_key);
                                printf("The applier signature:%s\n", applier_signature);
                                printf("The trust requirement:%f\n", trust_requirements);
                                printf("Username after decode %s\n", username);
                                // notes: the function in enclave.c return values is re, not the ret = verify_signature(global_eid,&re,message,miner_pk,miner_sig);
                                // sb sgx
                                verify_signature(global_eid, &re, contentHash, applier_public_key, applier_signature);
                                if (re == SGX_SUCCESS)
                                {
                                        // whether the key is in use
                                        printf("Your signature has been verifyed!\n");
                                        if (key_in_alteration(applier_public_key) == 0)
                                        {
                                                printf("The key is in use!\n");
                                                char sqlcmd[1024];
                                                sprintf(sqlcmd, "%s%s%s", "select trust_value from alteration_key_list where username=\"",
                                                        username, "\"");
                                                printf("sqlcmd:%s\n", sqlcmd);
                                                query_data(sqlcmd, mysql);
                                                while (row = mysql_fetch_row(res))
                                                { // mysql_fetch_row取结果集的下一行
                                                        // The trust value meets the requirement

                                                        if (atof(row[0]) >= trust_requirements)
                                                        {
                                                                printf("The trust value %s\n", row[0]);
                                                                send(new_fd, row[0], sizeof(row[0]), 0);
                                                                printf("The trust value meets the requirement\n");
                                                        }
                                                        else
                                                        {
                                                                send(new_fd, "NO", sizeof("NO"), 0);
                                                                printf("The trust value does not meets the requirements\n");
                                                        }
                                                }
                                        }
                                        // whether pk is in the revoke key list
                                }
                                else
                                {
                                        printf("The old pk exist in revoke key list!\n");
                                        send(new_fd, "NO", sizeof("NO"), 0);
                                        printf("verify failed");
                                }
                        }
                        // This is a trust evaluation signal
                        else if (buf[0] == 'T')
                        {
                                printf("This is a trust evaluation signal\n");
                                int j = 0;
                                char *score;
                                char *username;
                                const char *delim = "*";
                                char *token;
                                char *signal;
                                // 获取第一个子字符串
                                token = strtok(buf, delim);
                                /* 继续获取其他的子字符串 */
                                while (token != NULL)
                                {
                                        if (j == 0)
                                                signal = token;
                                        if (j == 1)
                                                username = token;
                                        if (j == 2)
                                                score = token;
                                        j++;
                                        token = strtok(NULL, delim);
                                }
                                printf("The username%s\n", username);
                                printf("The score is: %s\n", score);
                                char sqlcmd[1024];
                                sprintf(sqlcmd, "%s%s%s%s%s", "update alteration_key_list set trust_value = trust_value+", score, ",trust_value_times = trust_value_times + 1 where username =\"", username, "\"");
                                printf("sqlcmd:%s\n", sqlcmd);
                                query_data(sqlcmd, mysql);
                                send(new_fd, "OK", sizeof("OK"), 0);
                                printf("The trust value meets the requirement\n");
                        }
                        if (0 >= iRecvLen) //对端关闭连接 返回0
                        {
                                printf("接收失败或者对端关闭连接！\n");
                        }
                        /*else{
                                printf("buf: %s\n", buf);
                        }*/
                }
        }
        close(new_fd);
        close(iSocketFD);
}
// return 0 represents the pk is in revoke list and 1 not
int key_in_revoke(char *pk)
{
        printf("You have entered the key in revoke function\n");
        char *sqlcmd = "select * from revoke_key_list";
        // MYSQL_RES *res;
        query_data(sqlcmd, mysql);
        int t;
        printf("666\n");
        // MYSQL_ROW row;
        while (row = mysql_fetch_row(res))
        { // mysql_fetch_row取结果集的下一行
                for (t = 0; t < mysql_num_fields(res); t++)
                { //结果集的列的数量
                        printf("%s\t", row[t]);
                        if (t == 1 && strcmp(row[1], pk) == 0)
                                return 0;
                }
                printf("\n");
        }
        printf("We have completed the revoke key list\n");
        return 1;
}

int key_in_alteration(char *pk)
{
        char *sqlcmd = "select * from alteration_key_list";
        // MYSQL_RES *res;
        query_data(sqlcmd, mysql);
        int t;
        // MYSQL_ROW row;
        while (row = mysql_fetch_row(res))
        { // mysql_fetch_row取结果集的下一行
                for (t = 0; t < mysql_num_fields(res); t++)
                { //结果集的列的数量
                        if (t == 1 && strcmp(row[1], pk) == 0)
                        {
                                printf("match success\n");
                                return 0;
                        }
                        printf("row%d%s\t", t, row[t]);
                }
                printf("\n");
        }
        return 1;
}

void mysql_initial(char *host, char *user, char *password, char *database)
{

        int t, r;
        mysql_library_init(0, NULL, server_groups); //初始化MYSQL数据库
        mysql_init(&mysql);                         //初始化MYSQL标识符，用于连接
        if (!mysql_real_connect(&mysql, host, user, password, database, 0, NULL, 0))
        {
                fprintf(stderr, "无法连接到数据库，错误原因是:%s\n", mysql_error(&mysql));
        }
        else
        {
                puts("数据库连接成功");
                //首先向数据库中插入一条记录
                printf("\n");
        }

        // mysql_free_result(res);//操作完毕，查询结果集
        // mysql_close(&mysql);//关闭数据库连接

        // mysql_library_end();//关闭MySQL库
}
// insert, update and query data, type = 0 denotes query data from mysql, type = 1 denotes insert, update or delete data

/*  the function returns MYSQL_RES type, which can be used as follow, res represents the MYSQL_RES type
        int t;
       MYSQL_ROW row;
         while(row=mysql_fetch_row(res)){//mysql_fetch_row取结果集的下一行
                for(t=0;t<mysql_num_fields(res);t++){//结果集的列的数量
                        printf("%s\t",row[t]);
                        }
                printf("\n");
        }
*/
void query_data(char *sqlcmd, MYSQL mysql)
{
        // MYSQL_RES *res;
        // MYSQL_ROW row;
        // sprintf(sqlcmd,"%s","insert into friends (name,telephone) Values ('xx','xx')");
        // mysql_query(&mysql,sqlcmd);
        // sprintf(sqlcmd,"%s","select * from friends");
        int t;
        t = mysql_real_query(&mysql, sqlcmd, (unsigned int)strlen(sqlcmd));
        if (t)
        {
                printf("查询数据库失败%s\n", mysql_error(&mysql));
        }
        else
        {
                // execute the insert operation
                res = mysql_store_result(&mysql); //返回查询的全部结果集
        }
}

int main(int argc, char **argv)
{

        sgx_status_t status;
        sgx_status_t ret = SGX_SUCCESS;
        /* Initialize the enclave */
        printf("start initialize_enclave\n");
        initialize_enclave();
        printf("stop initialize_enclave\n");
        if (initialize_enclave() < 0)
        {
                printf("Enter a character before exit ...\n");
                getchar();
                return -1;
        }

        // init pairing of system
        char param[10240];
        FILE *fp = stdin;
        if (argc > 1)
        {
                fp = fopen(argv[1], "r");
                if (!fp)
                {
                        printf("Error opening %s\n", argv[1]);
                }
        }
        else
        {
                printf("Usage: .\\MPAC-MLDP <param_path>\n");
                sgx_destroy_enclave(global_eid);
                exit(-1);
        }
        size_t count = fread(param, 1, 10240, fp);
        if (!count)
        {
                printf("Input error\n");
                sgx_destroy_enclave(global_eid);
                return -1;
        }
        fclose(fp);
        unsigned char *digest;

        e_pairing_init(global_eid, &ret, param, count);
        printf("e_pairing_init end\n");

        // socket_server();

        mysql_initial(host, user, password, database);
        char *sqlcmd = "select * from student";
        socket_server();
        // query_data(sqlcmd,mysql);
        sgx_destroy_enclave(global_eid);
        return 0;
}
