#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

static int ZJF_Mail_int2char_FUN(int a)
{
    if(a <= 25)
    {
        return 'A'+(a-0);
    }
    else if((a > 25)&&(a <= 51))
    {
        return 'a'+(a-26);
    }
    else if((a > 51)&&(a < 61))
    {
        return '0'+(a-52);
    }
    else if(a == '=')
    {
        return a;
    }
    else if(a == 62)
    {
        return '+';
    }
    else if(a == 63)
    {
        return '/';
    }
    return 0;
}

static int ZJF_Mail_LittleToBig_FUN(char *str1,int len)
{
    int i    = 0;
    int temp = 0;
    for(i = 0; i < len; i++)
    {
        temp |= (str1[i] << (len-i-1)*8);
    }
    //printf("%#x \n",temp);
    return temp;
}

static int ZJF_Mail_Base64Encod_FUN(char * data,char *output,int len,int *oputlen)
{

    int i,j,k;
    char strtemp[4] = { 0 };  /*每次拷贝三个字节出来，存储*/
    int temp = 0;
    char * pbuffer = (char * )malloc(len/3*4+4);

    for(i = 0,j = 0; i < len/3; i++)
    {
        memcpy(strtemp,data+3*i,3);
        temp = ZJF_Mail_LittleToBig_FUN(strtemp,3);

        pbuffer[j++] = (temp>>18)&0x3f;   //0x3f  111111
        pbuffer[j++] = (temp>>12)&0x3f;
        pbuffer[j++] = (temp>>6)&0x3f;
        pbuffer[j++] = (temp>>0)&0x3f;

        if(j == 75)
        {
            pbuffer[j++] = '\n';
        }
    }

    if(len%3 == 1)
    {
        memcpy(strtemp,data+3*i,1);
        temp = (int)strtemp[0];
        pbuffer[j++] = temp>>2;
        pbuffer[j++] = (temp&0x03)<<4;
        pbuffer[j++] = '=';
        pbuffer[j++] = '=';
    }
    else if(len%3 == 2)
    {
        memcpy(strtemp,data+3*i,2);
        temp = ZJF_Mail_LittleToBig_FUN(strtemp,2);
        pbuffer[j++] = temp>>10;
        pbuffer[j++] = (temp>>4)&0x3f;
        pbuffer[j++] = (temp<<2)&0x3f;
        pbuffer[j++] = '=';
    }
    pbuffer[j++] = '\0';

    for(i = 0;i < j-1;i++)
    {
        //printf("%c ",ZJF_Mail_int2char_FUN(pbuffer[i]));
        output[i]= ZJF_Mail_int2char_FUN(pbuffer[i]);
    }
    printf("\n");
    *oputlen = j-1;
    //printf("data len = %d \n",j-1);
    if(pbuffer != NULL)
    {
        free(pbuffer);
        pbuffer = NULL;
    }

    return 0;

}


#define IP_ADDR      ("smtp.sina.com")
static ZJF_Mail_GetPeerAddrInfo_FUN(struct sockaddr_in *PeerAddr)
{
    struct hostent *Addr;
    char ip[32] = { 0 };

    Addr = gethostbyname(IP_ADDR);
    if(NULL == Addr)
    {
        perror("err:");
        printf("get ip info err !!! \n");
        return -1;
    }

    memset(PeerAddr, 0, sizeof(struct sockaddr_in));
    PeerAddr->sin_family = AF_INET;
    PeerAddr->sin_port   = htons(25);

    inet_ntop(Addr->h_addrtype, Addr->h_addr_list[0], ip, sizeof(ip));
    PeerAddr->sin_addr.s_addr = inet_addr(ip);
    printf("ip:%s len: %d \n",ip,strlen(ip));

    return 0;
}


static int ZJF_Mail_OpenSocket_FUN(struct sockaddr *addr)
{
    int sockfd = 0;
    sockfd=socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("socket failed !!! \n");
        return -1;
    }
    if(connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
    {
        printf("connect failed !!! \n");
        return -1;
    }
    return sockfd;
}

static int ZJF_Send_FUN()
{

}



/*HELO */
static int ZJF_Mail_HELO_FUN(int socket)
{
    char buf[320] = { 0 };
    int ret       = 0;

    sprintf(buf,"%s","EHLO ZJF \r\n");
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 320, 0);
    printf("注册返回消息:len = %d  rcv:%s \n",ret,buf);
    if(ret <= 0)
    {
        return -1;
    }

    return 0;
}


/*AUTH LOGIN 身份认证*/
static int ZJF_Mail_AuthLogin_FUN(int socket)
{
    char buf[320] = { 0 };
    int ret       = 0;

    sprintf(buf,"%s","AUTH LOGIN\r\n");
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 320, 0);
    if(ret <= 0)
    {
        return -1;
    }
    printf("身份认证返回消息:len = %d rcv:%s \n",ret,buf);
    return 0;
}


/*发送账号*/
static int ZJF_Mail_User_FUN(int socket)
{
    char buf[320]    = { 0 };
    char outbuf[120] = { 0 };
    int ret          = 0;
    int len          = 0;

    sprintf(buf,"%s","zjf535214685");
    ZJF_Mail_Base64Encod_FUN(buf,outbuf,strlen(buf),&len);
    sprintf(buf,"%s\r\n",outbuf);
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 320, 0);

    if(ret <= 0)
    {
        return -1;
    }
    printf("发送账号返回消息:len = %d rcv:%s \n",ret,buf);
    return 0;
}


/*发送密码*/
static int ZJF_Mail_Password_FUN(int socket)
{
    char buf[320]    = { 0 };
    char outbuf[120] = { 0 };
    int ret          = 0;
    int len          = 0;

    sprintf(buf,"%s","zjf19911018");
    ZJF_Mail_Base64Encod_FUN(buf,outbuf,strlen(buf),&len);
    sprintf(buf,"%s\r\n",outbuf);
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 320, 0);
    if(ret <= 0)
    {
        return -1;
    }
    printf("发送密码返回消息:len = %d rcv:%s \n",ret,buf);

    return 0;
}


/*发送发件人*/
static int ZJF_Mail_From_FUN(int socket)
{
    char buf[320] = { 0 };
    int ret       = 0;

    sprintf(buf,"%s","MAIL FROM: <zjf535214685@sina.com>\r\n");
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 320, 0);
    if(ret <= 0)
    {
        return -1;
    }
    printf("发送收件人返回消息:len = %d rcv:%s \n",ret,buf);
    return 0;
}



/*发送收件人*/
static int ZJF_Mail_RcptTo_FUN(int socket)
{
    char buf[320] = { 0 };
    int ret       = 0;

    sprintf(buf,"%s","RCPT TO:<2853109004@qq.com>\r\n");
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 320, 0);
    if(ret <= 0)
    {
        return -1;
    }
    printf("发送收件人返回消息:len = %d rcv:%s \n",ret,buf);
    return 0;
}


/*发送准备开始发送邮件*/
static int ZJF_Mail_DataReady_FUN(int socket)
{
    char buf[320] = { 0 };
    int ret       = 0;

    sprintf(buf,"%s","DATA\r\n");
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 320, 0);
    if(ret <= 0)
    {
        return -1;
    }
    printf("发送准备开始发送消息返回消息:len = %d rcv:%s \n",ret,buf);
    return 0;
}



/*发送邮件*/
static int ZJF_Mail_DataSend_FUN(int socket)
{
    char Email[] = "From: \"zjf\"<zjf535214685@sina.com>\r\n" \
                           "To: \"dasiy\"<2853109004@qq.com>\r\n" \
                           "Subject: Dear pig!\r\n\r\n" \
                           "Dear pig, if you see this email, that I have succeeded!";

    char buf[1200] = { 0 };
    int ret        = 0;

    sprintf(buf,"%s\r\n.\r\n",Email);
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 1200, 0);
    if(ret <= 0)
    {
        return -1;
    }
    printf("发送消息体返回消息:len = %d rcv:%s \n",ret,buf);
    return 0;
}

/*断开连接QUIT*/
static int ZJF_Mail_QUIT_FUN(int socket)
{
    char buf[320] = { 0 };
    int ret       = 0;

    sprintf(buf,"%s","QUIT\r\n");
    send(socket, buf, strlen(buf), 0);
    ret = recv(socket, buf, 320, 0);
    if(ret <= 0)
    {
        return -1;
    }
    printf("发送结束消息返回消息:len = %d rcv:%s \n",ret,buf);
    return 0;
}

#define MAX_BUFF_SIZE  (2048)
int main()
{
    struct sockaddr_in PeerAddr;
    int socket               = -1;
    char rbuf[MAX_BUFF_SIZE] = { 0 };
    int  ret                 = 0;

    if(ZJF_Mail_GetPeerAddrInfo_FUN(&PeerAddr) < 0)
    {
        return -1;
    }

    for( ; ; )
    {
        socket = ZJF_Mail_OpenSocket_FUN((struct sockaddr *)&PeerAddr);
        if(socket < 0)
        {
            sleep(5);
            continue;
        }

        while(1)
        {
            ret = recv(socket,rbuf,MAX_BUFF_SIZE,0);
            if(ret > 0)
            {
                printf("Revc: %s \n",rbuf);
                break;
            }
            sleep(2);
            continue;
        }

        if(ZJF_Mail_HELO_FUN(socket) < 0)
        {
            continue;
        }
        if(ZJF_Mail_AuthLogin_FUN(socket) < 0)
        {
            continue;
        }
        if(ZJF_Mail_User_FUN(socket) < 0)
        {
            continue;
        }

        if(ZJF_Mail_Password_FUN(socket) < 0)
        {
            continue;
        }
        if(ZJF_Mail_From_FUN(socket) < 0)
        {
            continue;
        }
        if(ZJF_Mail_RcptTo_FUN(socket) < 0)
        {
            continue;
        }

        if(ZJF_Mail_DataReady_FUN(socket) < 0)
        {
            continue;
        }
        if(ZJF_Mail_DataSend_FUN(socket) < 0)
        {
            continue;
        }
        if(ZJF_Mail_QUIT_FUN(socket) < 0)
        {
            continue;
        }

        close(socket);
        break;
    }

    return 0;
}
