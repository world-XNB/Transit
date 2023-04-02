#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define BUFFER_SIZE 1024
static int sockfd;

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
    struct sockaddr_in server_addr;

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
    bzero(&server_addr,sizeof(server_addr)); 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(502);

    
    // 将IP地址从字符串格式转换成网络地址格式，支持Ipv4和Ipv6.
    // 将点分十进制的ip地址转化为用于网络传输的数值格式
    inet_pton(AF_INET, "10.200.20.4", &server_addr.sin_addr);

    // 请求连接函数
    int err_log = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));      // 主动连接服务器
    if(err_log != 0){
        perror("connect");
        close(sockfd);
        exit(-1);
    }
    
    while(1){
        //发送信息
        char send_buf[BUFFER_SIZE] = "";
        printf("send:");

        fgets(send_buf,sizeof(send_buf),stdin);
        *(strchr(send_buf,'\n')) = '\0';    //将第一个出现的'\n'赋值为'\0'

        send(sockfd, send_buf, strlen(send_buf), 0);

        //接受信息
        // char recv_buf[BUFFER_SIZE] = "";
        // bzero(recv_buf , BUFFER_SIZE);
        // int server_sockfd = 3;
        // long byte_num = recv(server_addr, recv_buf, BUFFER_SIZE, 0); 
        // printf("%ld",byte_num);
        // if (byte_num > 0)  
        // {  
        //     if(byte_num > BUFFER_SIZE)  
        //     {  
        //         byte_num = BUFFER_SIZE;  
        //     }  
        //     recv_buf[byte_num] = '\0';  
        //     printf("服务端消息：%s", recv_buf);  
        // }  
        // else if(byte_num < 0)  
        // {  
        //     printf("从服务器接受消息出错.\n");  
        // }  
    }

    close(sockfd);
    return 0;
}
