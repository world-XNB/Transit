#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define CONNECT_MAX 8
#define BUFFER_SIZE 1024  

#define QUIT_CMD ".quit"  

static int sockfd;

int client_fds[CONNECT_MAX]; 

//软件中断函数，kill -l查看系统信号列表
static void sig_dispose(int sig)
{
    if(SIGINT == sig){
        close(sockfd);
        puts("\nclose!");
        exit(0);
    }
}

int main(int argc, char *argv[])
{    
    struct sockaddr_in my_addr;
    int err_log;

    char input_msg[BUFFER_SIZE];  
    char recv_msg[BUFFER_SIZE]; 

    //进程收到SIGINT信号的时候，用sig_dispose进行处理
    signal(SIGINT,sig_dispose);
    
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

    fd_set server_fd_set;   //fd_set变量
    int max_fd=-1;  //最大监听的文件描述符个数
    struct timeval tv;  //超时时间设置
    
    while(1)
    {   
        //初始化fd_set变量
        FD_ZERO(&server_fd_set);

        //监视文件描述符0的变化，及标准输入的变化
        FD_SET(STDIN_FILENO, &server_fd_set);
        if(max_fd < STDIN_FILENO)
        {
            max_fd = STDIN_FILENO;
        }
        
        //检测服务器socket文件标识符的变化
        FD_SET(sockfd, &server_fd_set);
        if (max_fd < sockfd)
        {
            max_fd = sockfd;
        }
        
        //客户端连接
        for (int i = 0; i < CONNECT_MAX; i++)
        {
            if (client_fds[i] !=0)
            {
                FD_SET(client_fds[i], &server_fd_set);  
                if(max_fd < client_fds[i])  
                {  
                    max_fd = client_fds[i];  
                } 
            }
        }

        //超时时间 
        tv.tv_sec = 20;
        tv.tv_usec = 0;

        //调用select函数. 若有控制台输入数据或则服务器文件标识符可读或则有新客户端连接，则返回大于0的整数，如果没有输入数据而引发超时，返回0.
        int ret = select(max_fd + 1, &server_fd_set, NULL, NULL, &tv);  
        if(ret < 0)  
        {  
            perror("select 出错\n");  
            continue;  
        }  
        else if(ret == 0)  
        {  
            printf("select 超时\n");  
            continue;  
        }
        else
        {
            //ret 为未状态发生变化的文件描述符的个数  
            if(FD_ISSET(STDIN_FILENO, &server_fd_set))  
            {  
                printf("发送消息：\n");  
                bzero(input_msg, BUFFER_SIZE);  
                fgets(input_msg, BUFFER_SIZE, stdin);  
                //输入“.quit"则退出服务器  
                if(strcmp(input_msg, QUIT_CMD) == 0)  
                {  
                    exit(0);  
                }  
                for(int i = 0; i < CONNECT_MAX; i++)  
                {  
                    if(client_fds[i] != 0)  
                    {  
                        printf("client_fds[%d]=%d\n", i, client_fds[i]);  
                        send(client_fds[i], input_msg, BUFFER_SIZE, 0);  
                    }  
                }  
            }  
            if(FD_ISSET(sockfd, &server_fd_set))  
            {  
                //有新的连接请求  
                struct sockaddr_in client_address;  
                socklen_t address_len;  
                int client_sock_fd = accept(sockfd, (struct sockaddr *)&client_address, &address_len);  
                printf("new connection client_sock_fd = %d\n", client_sock_fd);  
                if(client_sock_fd > 0)  
                {  
                    int index = -1;  
                    for(int i = 0; i < CONNECT_MAX; i++)  
                    {  
                        if(client_fds[i] == 0)  
                        {  
                            index = i;  
                            client_fds[i] = client_sock_fd;  
                            break;  
                        }  
                    }  
                    if(index >= 0)  
                    {  
                        printf("新客户端(%d)加入成功 %s:%d\n", index, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));  
                    }  
                    else  
                    {  
                        bzero(input_msg, BUFFER_SIZE);  
                        strcpy(input_msg, "服务器加入的客户端数达到最大值,无法加入!\n");  
                        send(client_sock_fd, input_msg, BUFFER_SIZE, 0);  
                        printf("客户端连接数达到最大值，新客户端加入失败 %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));  
                    }  
                }  
            }  
            for(int i =0; i < CONNECT_MAX; i++)  
            {  
                if(client_fds[i] !=0)  
                {  
                    if(FD_ISSET(client_fds[i], &server_fd_set))  
                    {  
                        //处理某个客户端过来的消息  
                        bzero(recv_msg, BUFFER_SIZE);  
                        long byte_num = recv(client_fds[i], recv_msg, BUFFER_SIZE, 0);  
                        if (byte_num > 0)  
                        {  
                            if(byte_num > BUFFER_SIZE)  
                            {  
                                byte_num = BUFFER_SIZE;  
                            }  
                            recv_msg[byte_num] = '\0';  
                            printf("客户端(%d):%s\n", i, recv_msg);  
                        }  
                        else if(byte_num < 0)  
                        {  
                            printf("从客户端(%d)接受消息出错.\n", i);  
                        }  
                        else  
                        {  
                            FD_CLR(client_fds[i], &server_fd_set);  
                            client_fds[i] = 0;  
                            printf("客户端(%d)退出了\n", i);  
                        }  
                    }  
                }
            }
        }
    }   
    // 关闭套接字
    close(sockfd);       
    
    return 0;
}
