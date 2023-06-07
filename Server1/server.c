#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "modbus.h"
#include "sqlite3.h"

#define CONNECT_MAX 8
#define BUFFER_SIZE 1024  

//--------------------------------------------------全局变量-----------------------------------------------------------------
static int sockfd;

int client_sockfds[CONNECT_MAX]; 

// static int server_send_flag = 0;    //服务器向客户端广播发送信息标志；0：不发送；1：向小车发送
unsigned char *send_ADU;
int ADU_len = 0;    //封装ADU的长度
unsigned char temp_data[1024];    //用来临时存储ADU数据
unsigned server_funcode = 0x00;     //用来存储服务端即将进行的功能码，0x00默认为不做任何功能。
char recv_buf[BUFFER_SIZE]; 

unsigned char Carid = 0xff;     //存储服务器最近收到信息的公交编号

unsigned char Carid1 = 0x01; //模拟公交编号，不同模拟公交此处编号不同！！！！！！！！！！！！！！！！！！！！！！！！！！
unsigned char Pstation1[6] = {0x05,0x00, 0x04, 0x03, 0x02, 0x01}; //存储该路线站点信息表！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！


unsigned char Pstation2[4] = {0x05,0x00, 0x02, 0x01};   //存储该路线站点信息表！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
unsigned char Carid2 = 0x02;   //模拟公交编号，不同模拟公交此处编号不同！！！！！！！！！！！！！！！！！！！！！！！！！！

unsigned char Nstation1 = 0xee; //记录下一站，避免多车同时进站
unsigned char Nstation2 = 0xff; //记录下一站，避免多车同时进站
int Nstation_flag = 0;  //是否发送过特殊情况指令   0：没有    1：有
unsigned char special_Carid = 0xff;
int send_client_station_pos = 1;


//--------------------------------------------------初始化-----------------------------------------------------------------
// 服务器socket初始化进入监听状态,等待客户端连接
void server_init(void)
{
    struct sockaddr_in my_addr;
    int err_log;

    //创建socket
    //family代表一个协议族，比较熟知的就是AF_INET，PF_PACKET等；
    //第二个参数是协议类型，常见类型是SOCK_STREAM,SOCK_DGRAM, SOCK_RAW, SOCK_PACKET等；
    //第三个参数是具体的协议，对于标准套接字来说，其值是0，对于原始套接字来说就是具体的协议值
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("socket");
        exit(-1);
    }

    // 将空间my_addr的前sizeof(my_addr)字节置零
    bzero(&my_addr, sizeof(my_addr));         
    my_addr.sin_family = AF_INET;
    //注意：端口号小于1024，在Linux下需要root用户运行
    my_addr.sin_port   = htons(502);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);//任意ip地址
    // my_addr.sin_addr.s_addr = inet_addr("192.168.1.99");//指定ip地址
    
    /*绑定函数，将addrlen长度 structsockaddr类型的myaddr地址与sockfd文件描述符绑定到一起，
    在sockaddr中主要包含服务器端的协议族类型，网络地址和端口号等*/
    err_log = bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if( err_log != 0){
        perror("binding");
        close(sockfd);        
        exit(-1);
    }
    
    /*监听函数，server端不能及时的处理已经建立的连接，这时就会将connect连接放在等待队列中缓存
    起来。这个等待队列的长度有listen中的backlog参数来设定。*/
    err_log = listen(sockfd, 10);
    if(err_log != 0){
        perror("listen");
        close(sockfd);        
        exit(-1);
    }
}

//数据库初始化
void my_sql_init()
{
    sqlite3* db;

    sqlite3_open("Transit.db",&db);//新建数据库

    char* sql1 = "create table station (id1 tinyint,carinfo tinyint,prev tinyint,next tinyint);";
    char* sql2 = "create table distance (id1 tinyint,id2 tinyint,distance tinyint);";
    char* sql3 = "create table carstat(id tinyint, prev tinyint, carspeed tinyint, prevdistance tinyint);";

    char* errmsg=NULL;
    sqlite3_exec(db,sql1,NULL,NULL,&errmsg);
    sqlite3_exec(db,sql2,NULL,NULL,&errmsg);
    sqlite3_exec(db,sql3,NULL,NULL,&errmsg);

    if (errmsg!=NULL)
    {
        printf("%s\n",errmsg);
    }

    sqlite3_close(db);//关闭数据库
}

//站点信息表初始化
void my_sql_station_init()
{
    sqlite3* db;

    sqlite3_open("Transit.db",&db);

    char sql_ju[200]="";
    sprintf(sql_ju,"select * from station;");
    char* errmsg=NULL;
    int r=0,c=0;
    char** table1=NULL;
    sqlite3_get_table(db,sql_ju,&table1,&r,&c,&errmsg);
    if(errmsg!=NULL)
        printf("reemsg:%s\n",errmsg);
    if (r!=0)
    {
        //已经初始化过，不在初始化
        return;
    }
    
    char sql1[200]="";
    char sql2[200]="";
    char sql3[200]="";
    char sql4[200]="";
    char sql5[200]="";
    char sql6[200]="";

    sprintf(sql1,"insert into station values (0,3,32,20);");
    sprintf(sql2,"insert into station values (1,3,4,255);");
    sprintf(sql3,"insert into station values (2,3,9,2);");
    sprintf(sql4,"insert into station values (3,1,16,4);");
    sprintf(sql5,"insert into station values (4,1,0,8);");
    sprintf(sql6,"insert into station values (5,3,255,1);");

    errmsg=NULL;
    sqlite3_exec(db,sql1,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    errmsg=NULL;
    sqlite3_exec(db,sql2,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    errmsg=NULL;
    sqlite3_exec(db,sql3,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    errmsg=NULL;
    sqlite3_exec(db,sql4,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    sqlite3_exec(db,sql5,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    sqlite3_exec(db,sql6,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }

    sqlite3_close(db);
}

//站点距离初始化
void my_sql_distance_init()
{
    sqlite3* db;

    sqlite3_open("Transit.db",&db);

    char sql_ju[200]="";
    sprintf(sql_ju,"select * from distance;");
    char* errmsg=NULL;
    int r=0,c=0;
    char** table1=NULL;
    sqlite3_get_table(db,sql_ju,&table1,&r,&c,&errmsg);
    if(errmsg!=NULL)
        printf("reemsg:%s\n",errmsg);
    if (r!=0)
    {
        //已经初始化过，不在初始化
        return;
    }
    
    char sql1[200]="";
    char sql2[200]="";
    char sql3[200]="";
    char sql4[200]="";
    char sql5[200]="";
    char sql6[200]="";

    // distance(id1 tinyint,id2 tinyint,distance tinyint);
    sprintf(sql1,"insert into distance values (0,2,1);");
    sprintf(sql2,"insert into distance values (0,4,4);");
    sprintf(sql3,"insert into distance values (1,2,5);");
    sprintf(sql4,"insert into distance values (2,3,5);");
    sprintf(sql5,"insert into distance values (3,4,2);");
    sprintf(sql6,"insert into distance values (0,5,4);");

    errmsg=NULL;
    sqlite3_exec(db,sql1,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    errmsg=NULL;
    sqlite3_exec(db,sql2,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    errmsg=NULL;
    sqlite3_exec(db,sql3,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    errmsg=NULL;
    sqlite3_exec(db,sql4,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    sqlite3_exec(db,sql5,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    sqlite3_exec(db,sql6,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }

    sqlite3_close(db);
}

//模拟公交状态初始化
void my_sql_carstat_init()
{
    sqlite3* db;

    sqlite3_open("Transit.db",&db);

    char sql_ju[200]="";
    sprintf(sql_ju,"select * from carstat;");
    char* errmsg=NULL;
    int r=0,c=0;
    char** table1=NULL;
    sqlite3_get_table(db,sql_ju,&table1,&r,&c,&errmsg);
    if(errmsg!=NULL)
        printf("reemsg:%s\n",errmsg);
    if (r!=0)
    {
        //已经初始化过，不在初始化
        return;
    }
    
    char sql1[200]="";
    char sql2[200]="";

    // carstat(id tinyint, prev tinyint, carspeed tinyint, prevdistance tinyint);
    sprintf(sql1,"insert into carstat values (1,0,0,0);");
    sprintf(sql2,"insert into carstat values (2,0,0,0);");

    errmsg=NULL;
    sqlite3_exec(db,sql1,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    errmsg=NULL;
    sqlite3_exec(db,sql2,NULL,NULL,&errmsg);
    if (errmsg!=NULL)
    {
        printf("注册插入错误：%s\n",errmsg);
    }
    
    sqlite3_close(db);
}

//--------------------------------------------------功能函数-----------------------------------------------------------------
//软件中断函数，kill -l查看系统信号列表
static void sig_dispose(int sig)
{
    if(SIGINT == sig){
        close(sockfd);
        puts("\nclose!");
        exit(0);
    }
}


// 封装发送的MODBUS_ADU
void modbus_ADU_data()
{
    struct MDBOS_ADU adu;

    if(server_funcode == 0x42)  //发送对小车的控制信息
    {
        unsigned char control = 0x18;
        adu = modbus_req_ADU_control(control);
    }
    else if(server_funcode == 0x43)    //此时是响应：返回站点信息； 请求：客户端->服务器 发送站点信息请求
    {
        unsigned char temp_buf[4];   //3字节的站点信息(经过公交车次（1字节），最近公交时间（1字节），距几站（1字节）)
        temp_buf[0] = 0x01; //1字节8位：从低到高依次为一路到八路公交，对应位置1代表要经过本站
        temp_buf[1] = 0x02;
        temp_buf[2] = 0x03;
        temp_buf[3] = 0x03;

        sqlite3* db;
        sqlite3_open("Transit.db",&db);

        char sql[200]="";
        sprintf(sql,"select * from station;");

        char* errmsg=NULL;
        char** table1=NULL;
        int r=0,c=0;
        sqlite3_get_table(db,sql,&table1,&r,&c,&errmsg);
        if(errmsg!=NULL)
            printf("select:%s\n",errmsg);
        if (r!=0)
        {
            int i=0,j=0;
            char* data[r][c];

            for ( i = 1; i <= r; i++)
            {
                for ( j = 0; j < c; j++)
                {
                    data[i][j]=table1[i*c+j];
                    // printf("%s\t",data[i][j]);
                }
                // printf("\n");
            }
            for (int i = 0; i < c; ++i)
            {
                temp_buf[i]=atoi(data[send_client_station_pos][i]);
                // printf("%d\t",temp_buf[i]);
            }
        }
        sqlite3_close(db);

        adu = modbus_res_ADU_station(temp_buf);

        send_client_station_pos++;
        if (send_client_station_pos == r+1)
        {
             send_client_station_pos = 1;
        }
    }
    else if(server_funcode == 0x45)    //此时是响应：返回站点信息； 请求：客户端->服务器 发送站点信息请求
    {
        adu = modbus_req_ADU_control(Carid);
    }
    else if (server_funcode == 0x46)    //请求 服务器->模拟公交 模拟公交启动
    {
        adu = modbus_req_ADU_start();
    }
    else if (server_funcode == 0x47)    //请求 服务器->模拟公交 模拟公交结束
    {
        adu = modbus_req_ADU_end();
    }

    unsigned short lenth = *(unsigned short*)adu.head.lenth;    //将两字节数据长度拼接——lenth为以下字节长度
    for (int i = 0; i < lenth; ++i)  //!!!不这样进行中间存储在malloc send_ADU之后adu.data[0]会被修改
    {
        temp_data[i] = adu.data[i];
    }

    ADU_len = 6 + lenth;  //ADU长度

    send_ADU = (unsigned char *)malloc(ADU_len*sizeof(unsigned char));
    
    send_ADU[0] = adu.head.TMIdent[0];
    send_ADU[1] = adu.head.TMIdent[1];
    send_ADU[2] = adu.head.PIdent[0];
    send_ADU[3] = adu.head.PIdent[1];
    send_ADU[4] = adu.head.lenth[1];
    send_ADU[5] = adu.head.lenth[0];
    send_ADU[6] = adu.head.UIdent;
    send_ADU[7] = adu.fun_code;

    int t;
    for (int i = 0; i < lenth; ++i)
    {
        t = 8 + i;
        send_ADU[t] = temp_data[i];
    }

    // printf("封装ADU:");   
    // for (int i = 0; i < ADU_len; ++i)
    // {
    //     printf("%x\t",send_ADU[i]);
    // }
    // printf("\n");
}

//封装给客户端发送信息函数
//@param i:调用时传递的客户端连接下标号
void send_to_client(int i)
{
    modbus_ADU_data();
    if (client_sockfds[i] != 0)
    {
        send(client_sockfds[i], send_ADU, ADU_len, 0);
        send(client_sockfds[i], send_ADU, ADU_len, 0);

        // printf("向客户端(%d)发送ADU(%d):\n",client_sockfds[i],ADU_len);   
        // for (int i = 0; i < ADU_len; ++i)
        // {
        //     printf("%x\t",send_ADU[i]);
        // }
        // printf("\n\n");
    } 
}

// 刷新当前输入公交的下一站名称
//@param Pstation:当前公交编号    Pstation:当前公交经过的上一站
void Nstation_fun(unsigned char Pstation,unsigned char Carid)
{
    if (Carid == Carid1)
    {
        for (int i = 0; i < 6; ++i)
        {
            if (Pstation == Pstation1[i])
            {
                int j = i + 1;
                if(j == 6)
                    j = 0;
                Nstation1 = Pstation1[j];
            }
        }
    }
    else if (Carid == Carid2)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (Pstation == Pstation2[i])
            {
                int j = i+1;
                if (j == 4)
                    j = 0;
                Nstation2 = Pstation2[j];
            }
        }
    }
}

//接受数据处理函数——根据接受到的ADU的功能码执行相应功能
void recv_buf_funcode()
{
    if (recv_buf[7] == 0x41)//此时是请求：小车—>服务器 小车经过上一站的名称，当前车速，距上一站的距离
    {
        unsigned char Carspeed, Pstation, Pdistance;
        Carspeed = recv_buf[8];
        Pstation = recv_buf[9];
        Pdistance = recv_buf[10];
        Carid = recv_buf[11];

        // server_funcode =0x43;    //利用小车定时向客户端发送站点信息

        printf("Carspeed:%d\tPstation:%x\tPdistance:%d\tCarid:%x\n\n", Carspeed, Pstation, Pdistance,Carid);

        Nstation_fun(Pstation, Carid);
        printf("Nstation1:%x\tNstation2:%x\n",Nstation1,Nstation2);
        if (Nstation1 == Nstation2&&Nstation_flag==0)     //判断是否需要特殊指令
        {
            special_Carid = Carid;  
            printf("special_Carid:%x\tNstation1:%x\tNstation2:%x\n",special_Carid,Nstation1,Nstation2);
            server_funcode = 0x45;
            Nstation_flag = 1;
        }
        else if (Nstation2 != Nstation1&&Nstation_flag==1)  //  下一站相同的另一辆车已经离站
        {
            printf("ok\n");
            server_funcode = 0x46;
            Nstation_flag = 0;
        }
        else if (Nstation_flag == 0&&Nstation1!=Nstation2)
        {
             server_funcode =0x43;    //利用小车定时向客户端发送站点信息
        }

        // 更新数据库中小车状态
        sqlite3* db;

        sqlite3_open("Transit.db",&db);//新建数据库
        char sql_update[200] = "";
        sprintf(sql_update ,"update carstat set prev = \"%d\" ,carspeed = \"%d\" ,prevdistance = \"%d\" where id = \"%d\";", Pstation, Carspeed, Pdistance, Carid);

        char* errmsg=NULL;
        sqlite3_exec(db,sql_update,NULL,NULL,&errmsg);

        if (errmsg!=NULL)
        {
            printf("%s\n",errmsg);
        }

        sqlite3_close(db);//关闭数据库
    }
    else
    {

    }
}

// 进行客户端连接,并进行数据处理
void data_hand()
{
    char input_buf[BUFFER_SIZE];  

    //标准输入时有回车，所以此时应该加“\n”
    char QUIT_CMD[10]=".quit\n";
    char START_CMD[10]=".start\n";
    char END_CMD[10]=".end\n";

    fd_set readset;   //fd_set变量
    int max_fd=-1;  //最大监听的文件描述符个数
    struct timeval tv;  //超时时间设置
    
    while(1)
    {   
        //初始化fd_set变量
        FD_ZERO(&readset);

        //监视文件描述符0的变化，及标准输入的变化
        FD_SET(STDIN_FILENO, &readset);

        if(max_fd < STDIN_FILENO)
        {
            max_fd = STDIN_FILENO;
        }
        
        //检测服务器socket文件标识符的变化
        FD_SET(sockfd, &readset);
        if (max_fd < sockfd)
        {
            max_fd = sockfd;
        }
        
        //检测客户端连接标识符变化
        for (int i = 0; i < CONNECT_MAX; i++)
        {
            if (client_sockfds[i] !=0)
            {
                FD_SET(client_sockfds[i], &readset);  
                if(max_fd < client_sockfds[i])  
                {  
                    max_fd = client_sockfds[i];  
                } 
            }
        }

        //超时时间 
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        //调用select函数. 若有控制台输入数据或则服务器文件标识符可读或则有新客户端连接，则返回大于0的整数，如果没有输入数据而引发超时，返回0.
        //ret 为未状态发生变化的文件描述符的个数  
        int ret = select(max_fd + 1, &readset, NULL, NULL, &tv);  
        if(ret < 0)  
        {  
            perror("select 出错\n");  
            continue;  
        }  
        else if(ret == 0)  
        {  
            // printf("select 超时\n");  
            // continue;  
        }
        else
        {
            // 检测标准输入是否有待读入数据
            if(FD_ISSET(STDIN_FILENO, &readset))  
            {  
                printf("发送消息:"); 
                bzero(input_buf, BUFFER_SIZE);  
                fgets(input_buf, BUFFER_SIZE, stdin);  
                printf("%s",input_buf);

                //输入“.quit"则退出服务器  
                if(strcmp(input_buf, QUIT_CMD) == 0)  
                {  
                    printf("服务器已退出!!!\n");
                    exit(0);  
                } 
                else if (strcmp(input_buf, START_CMD) == 0)
                {
                    printf("模拟公交系统启动...\n");
                    server_funcode = 0x46;
                }
                else if(strcmp(input_buf, END_CMD) == 0)
                {
                    printf("模拟公交结束...\n");
                    server_funcode = 0x47;
                }
            }  

            // 检测有新的连接请求  
            if(FD_ISSET(sockfd, &readset))  
            {  
                struct sockaddr_in client_address;  
                socklen_t address_len;  
                int client_sock_fd = accept(sockfd, (struct sockaddr *)&client_address, &address_len);  
                printf("new connection client_sock_fd = %d\n", client_sock_fd);  
                if(client_sock_fd > 0)  
                {  
                    int index = -1;  
                    for(int i = 0; i < CONNECT_MAX; i++)  
                    {  
                        if(client_sockfds[i] == 0)  
                        {  
                            index = i;  
                            client_sockfds[i] = client_sock_fd;  
                            break;  
                        }  
                    }  
                    if(index >= 0)  
                    {  
                        printf("新客户端(%d)加入成功 %s:%d\n", index, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));  
                    }  
                    else  
                    {  
                        bzero(input_buf, BUFFER_SIZE);  
                        strcpy(input_buf, "服务器加入的客户端数达到最大值,无法加入!\n");  
                        send(client_sock_fd, input_buf, BUFFER_SIZE, 0);  
                        printf("客户端连接数达到最大值，新客户端加入失败 %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));  
                    }  
                }  
            }  

            // 处理每个客户端连接是否有待读入信息
            for(int i =0; i < CONNECT_MAX; i++)  
            {  
                if(client_sockfds[i] !=0)  
                {  
                    if(FD_ISSET(client_sockfds[i], &readset))  
                    {  
                        //处理某个客户端过来的消息  
                        bzero(recv_buf, BUFFER_SIZE);  
                        long byte_num = recv(client_sockfds[i], recv_buf, BUFFER_SIZE, 0);  
                        if (byte_num > 0)  
                        {  
                            if(byte_num > BUFFER_SIZE)  
                            {  
                                byte_num = BUFFER_SIZE;  
                            }  
                            recv_buf[byte_num] = '\0';  
                            recv_buf_funcode();
                            
                            // printf("客户端（%d）十六进制：", i);
                            // for(int count = 0; count < byte_num ; count++)
                            // {
                            //     printf("%x ",recv_buf[count]);
                            // }

                            printf("\n客户端(%d):%s", i, recv_buf);
                        }  
                        else if(byte_num < 0)  
                        {  
                            printf("从客户端(%d)接受消息出错.\n", i);  
                        }  
                        else  
                        {  
                            FD_CLR(client_sockfds[i], &readset);  
                            client_sockfds[i] = 0;  
                            printf("客户端(%d)退出了\n", i);  
                        }  
                    }  
                }
            }            
        }

        // 发送信息部分
        for (int i = 0; i < CONNECT_MAX; i++)
        {
            if(server_funcode != 0x00)
            {
                send_to_client(i);
            }
        }
        server_funcode = 0x00;  //  复位功能码，准备下一个功能码
    }   
}

int main(int argc, char *argv[])
{    
    printf("服务器启动...\n");
    //结束服务器程序——进程收到SIGINT信号的时候，用sig_dispose进行处理
    signal(SIGINT,sig_dispose);

    // 数据库初始化
    printf("数据库初始化...\n");
    my_sql_init();
    my_sql_station_init();
    my_sql_distance_init();
    my_sql_carstat_init();

    // 服务端socket初始化
    printf("网络初始化\n");
    server_init();

    // 数据处理
    data_hand();

    free(send_ADU);

    // 关闭套接字
    close(sockfd);     
    
    return 0;
}
