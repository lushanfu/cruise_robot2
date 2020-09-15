#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <memory.h>
#include <getopt.h>
#include <fcntl.h>
#include "/home/lsf/LSF/cruise_robot2/src/motordriven/src/spi.h"
#include "/home/lsf/LSF/cruise_robot2/src/motordriven/src/common.h"


#define false 0
#define true 1
#define MAX_BUF_SIZE 1024
#define FIX_ARM_SIZE 0x10
#define FIX_READ_RAM_SIZE  0x09
#define MOV_READ_RAM_SIZE  0x09
#define MOV_ARM_SIZE 0x0e
#define NEW_READ_SZIE 2
char GPS_BUFF[MAX_BUF_SIZE];
char GPS_SEND_MSG[MAX_BUF_SIZE];
char USB_SEND_MSG[MAX_BUF_SIZE];
typedef struct _SOCKET_PARA{
    int conn_fd;
    char *msg;
}SOCKET_PARA;
typedef struct _OPTION
{
    uint32_t speed;
    char verb;
    uint32_t time;
    int spi_fd;
    int port;
}OPTION;
#pragma pack(1)
typedef struct _CMD_HEAD{
    short cmd;
    int pkg_id;
    char len;
    char reserve;
}CMD_HEAD;
#pragma pack()

uint8_t RAM_INF[128];
uint8_t CMD_PK_ID[128];
const uint8_t CMD_RAM_NUM[] = {1,1,1,1,2,1,1,1,2,4,3,3,1,3,3,2,1,1,1,1,2,1,1,1,2,4,3,3,1,1,1,1};
const uint8_t CMD_ADDR_MAP[] = {0x20,0x21,0x22,0x23,0x24,0x26,0x27,0x28,0xa0,0xa2,0xa6,0xa9,0xac,0xad,0xb0,0xb3,0x00,0x01,0x02,0x03,0x04,0x06,0x07,0x08,0x80,0x82,0x86,0x89,0x8c,0x8d,0x29,0x09};



OPTION OPT;
static void reset_ram()
{
    memset(RAM_INF,0,sizeof(RAM_INF));
    memset(CMD_PK_ID,0,sizeof(CMD_PK_ID));
    memset(GPS_SEND_MSG,0,sizeof(GPS_SEND_MSG));
    memset(USB_SEND_MSG,0,sizeof(USB_SEND_MSG));
    memset(GPS_BUFF,0,sizeof(GPS_BUFF));
}

static void wait_key()
{
    while(getchar() != 'q');
    exit(0);
}


static void us_sleep(unsigned us){
    struct timeval tv;
    tv.tv_sec = us/1000;
    tv.tv_usec = us%1000;
    select(0,NULL,NULL,NULL,&tv);
}

static int addr_to_index(int addr)
{
    int ix = -1;
    int size = sizeof(CMD_ADDR_MAP);
    for(ix = 0;ix < size;++ix)
    {
        if(CMD_ADDR_MAP[ix] == addr) break;
    }
    if(ix >= size) ix = -1;
    return ix;
}
static int cmd_to_addr(int cmd)
{
    int addr = -1;
    if(cmd >= 0 && cmd <= 0x0f)
    {
        addr = CMD_ADDR_MAP[cmd];
    }
    else if(cmd >=0x20 && cmd <= 0x2d )
    {
        addr = CMD_ADDR_MAP[cmd-0x20+FIX_ARM_SIZE];
    }
    else if(cmd >= 0x30 && cmd <= 0x31)
    {
        addr = CMD_ADDR_MAP[cmd-0x30+FIX_ARM_SIZE+MOV_ARM_SIZE];
    }
    return addr;
}

static char control_cmd(int cmd,char**data,int *len)
{  
    uint8_t tx[2] = {0,0};
    uint8_t rx[2] = {0,0};
    uint8_t addr = 0;

    //hex_dump(*data,*len,4,"RCV");
    if(cmd >= 0x08 && cmd <= 0x0f)
    {
        int ram_num = CMD_RAM_NUM[cmd];
        uint8_t* id = CMD_PK_ID + cmd;
        for(int i = 1;i < ram_num;++i)
        {
            addr = CMD_ADDR_MAP[cmd]+i;    
            tx[0] = addr;
            tx[1] = (uint8_t)(*data)[i];
            while(transfer(OPT.spi_fd,tx,rx,2,OPT.speed,OPT.verb) <= 0);
        }    
        addr = CMD_ADDR_MAP[cmd];
        tx[0] = addr;
        tx[1] = (uint8_t)(*data)[0];
        tx[1] = tx[1] | ((*id)<<2);
        while(transfer(OPT.spi_fd,tx,rx,2,OPT.speed,OPT.verb) <= 0);
        *id = *id >= 63? 1 : *id + 1;
        return true;
    }
    else if(cmd >= 0x28 && cmd <= 0x2d)
    {
        int ram_num = CMD_RAM_NUM[cmd-0x20 + FIX_ARM_SIZE];
        uint8_t* id =CMD_PK_ID + (cmd-0x20+FIX_ARM_SIZE);
        for(int i = 1;i < ram_num;++i)
        {
            addr = CMD_ADDR_MAP[cmd-0x20+FIX_ARM_SIZE] + i;         
            tx[0] = addr;
            tx[1] = (uint8_t)(*data)[i];
            while(transfer(OPT.spi_fd,tx,rx,2,OPT.speed,OPT.verb) <= 0);
        }
        addr = CMD_ADDR_MAP[cmd-0x20+FIX_ARM_SIZE];         
        tx[0] = addr;
        tx[1] = (uint8_t)(*data)[0];
        tx[1] = tx[1] | ((*id)<<2);
        while(transfer(OPT.spi_fd,tx,rx,2,OPT.speed,OPT.verb) <= 0);
        *id = *id >= 63? 1 : *id + 1;
        return true;
    }

    return false;
}

static char monitor_cmd(int cmd,char **data,int *len)
{
    if(cmd >= 0x00 && cmd < 0x08)
    {
        int ram_num = CMD_RAM_NUM[cmd];
        int offset = cmd_to_addr(cmd);
        if(offset < 0) return false;
        char* addr = RAM_INF + offset;
        *data = (char*)malloc(ram_num);
        memset(*data,0,ram_num);
        memcpy(*data,addr,ram_num);
        *len = ram_num;
        return true;
    }
    else if(cmd >= 0x20 && cmd < 0x28){
        int ram_num = CMD_RAM_NUM[cmd-0x20+FIX_ARM_SIZE];
        int offset = cmd_to_addr(cmd);
        if(offset < 0) return false;
        char* addr = RAM_INF + offset; 
        *data = (char*)malloc(ram_num);
        memset(*data,0,ram_num);
        memcpy(*data,addr,ram_num);
        *len = ram_num;
        return true;

    }
    else if(cmd >= 0x30 && cmd <= 0x31)
    {
        int ram_num = CMD_RAM_NUM[cmd-0x30+FIX_ARM_SIZE+MOV_ARM_SIZE];
        int offset = cmd_to_addr(cmd);
        if(offset < 0) return false;
        char* addr = RAM_INF + offset; 
        *data = (char*)malloc(ram_num);
        memset(*data,0,ram_num);
        memcpy(*data,addr,ram_num);
        *len = ram_num;
        return true;
    }
    else if(cmd == 0xf0) //gps
    {
    	*data = GPS_SEND_MSG;
    	*len = MAX_BUF_SIZE;
    }
    else if(cmd == 0xf1)
    {
        *data = USB_SEND_MSG;
        *len = MAX_BUF_SIZE;
    }
    return false; 
}

static void thread_spi_read()
{
    uint8_t* buf = NULL;
    uint8_t addr[2];
    uint8_t msg[2];
    uint8_t data[2];
    int i = 0;
    while(true)
    {

        if(i >= 0 && i < FIX_READ_RAM_SIZE)
        {
            addr[0] = CMD_ADDR_MAP[0]+i;
            addr[1] = 0xff;
            transfer(OPT.spi_fd,addr,data,2,OPT.speed,0);
            buf = RAM_INF + addr[0];
            memset(buf,0,1);
            *buf = data[1];
            msg[0] = addr[0];
            msg[1] = data[1];
            if(OPT.verb > 0) hex_dump(msg,2,4,"RX/TX");
        }
        else if(i >= FIX_READ_RAM_SIZE && i < FIX_READ_RAM_SIZE + MOV_READ_RAM_SIZE )
        {
         
            addr[0] = CMD_ADDR_MAP[FIX_ARM_SIZE]+(i-FIX_READ_RAM_SIZE);
            addr[1] = 0xff;
            transfer(OPT.spi_fd,addr,data,2,OPT.speed,0);
            buf = RAM_INF + addr[0];
            memset(buf,0,1);
            *buf = data[1];
            msg[0] = addr[0];
            msg[1] = data[1];
            if(OPT.verb > 0) hex_dump(msg,2,4,"RX/TX");
        }
        else if(i >= FIX_READ_RAM_SIZE+MOV_READ_RAM_SIZE && i <= FIX_READ_RAM_SIZE+MOV_READ_RAM_SIZE+NEW_READ_SZIE)
        {

            addr[0] = CMD_ADDR_MAP[FIX_ARM_SIZE+MOV_ARM_SIZE+(i-FIX_READ_RAM_SIZE-MOV_READ_RAM_SIZE)];
            addr[1] = 0xff;
            transfer(OPT.spi_fd,addr,data,2,OPT.speed,0);
            buf = RAM_INF + addr[0];
            memset(buf,0,1);
            *buf = data[1];
            msg[0] = addr[0];
            msg[1] = data[1];
            if(OPT.verb > 0) hex_dump(msg,2,4,"RX/TX");
        }
        else if(i > FIX_READ_RAM_SIZE+MOV_READ_RAM_SIZE+NEW_READ_SZIE)
        {
            us_sleep(OPT.time);
            i = 0;
            continue;
        }
        i++;        
    }

}

static void msg_process(SOCKET_PARA *para) {
    char ret = -1;
    int  len = 0;
    char *msg = NULL;
    char *s_msg = NULL;
    CMD_HEAD *head = (CMD_HEAD*)malloc(sizeof(CMD_HEAD));
    memset(head,0,sizeof(CMD_HEAD));
    memcpy(head,para->msg,sizeof(CMD_HEAD));
    
    //if(head->cmd == 0x31 || head->cmd == 0x30)
    //{
    //    printf("cmd id is %d\n\n",head->cmd);
    //}
    if((head->cmd >= 0 && head->cmd <= 0x07) || (head->cmd >= 0x20 && head->cmd <= 0x27)||(head->cmd >= 0x30 && head->cmd <= 0x31))
    {
        ret = monitor_cmd(head->cmd,&msg,&len);
        s_msg = (char*)malloc(sizeof(head)+len);
        memset(s_msg,0,sizeof(head)+len);
        memcpy(s_msg,head,sizeof(head));
        memcpy(s_msg+sizeof(head),msg,len);

    }
    else if((head->cmd >= 0x08 && head->cmd <= 0x0f) || (head->cmd >= 0x28 && head->cmd <= 0x2d))
    {
        len = head->len;
        msg = (char*)malloc(len);
        memset(msg,0,len);
        memcpy(msg,para->msg + sizeof(head),len);
        ret = control_cmd(head->cmd,&msg,&len);
        s_msg = (char*)malloc(sizeof(head)); 
        memset(s_msg,0,sizeof(head));
        memcpy(s_msg,head,sizeof(head));
        free(msg);

    }
    
    send(para->conn_fd, s_msg,len+sizeof(head),0);
    close(para->conn_fd);
    free(s_msg);
    free(head);

}

static OPTION parse_opt(int argc, char *argv[])
{
    uint32_t speed = 1000000;
    int port = 100;
    char verb = 0;
    uint32_t time = 20;
    OPTION opt;
    while (1) {
        static const struct option lopts[] = {
            { "speed",   1, 0, 's' },
            { "port",   1, 0, 'p' },
            { "verb",   1, 0, 'v' },
            { "time",   1, 0, 't'},
            { NULL, 0, 0, 0 },
        };
        int c;
        c = getopt_long(argc, argv, "s:p:v:t:", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
            case 's':
                speed = atoi(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'v':
                verb = atoi(optarg);
                break;
            case 't':
                time = atoi(optarg);
                break;

            default:
                break;
        }
    }
    opt.port = port;
    opt.speed = speed;
    opt.verb = verb;
    opt.time = time;
    return opt;
}



int send_cmd(int fd,char* cmd,int size)
{
    char at[40];
    memset(at,0,sizeof(at));
    memcpy(at,cmd,size);
    return write(fd,at,strlen(at));
}

void thread_get_gps(void)
{   
    //char at[20];
    int nread = -1;
    int fd1 = open("/dev/ttyUSB2",O_RDWR); 
    int fd2 = open("/dev/ttyUSB3",O_RDWR);
    if(fd1 == -1 || fd2 == -1) return;
    if(set_opt(fd1,4800, 8, 'N', 1, 100) == -1 || set_opt(fd2,4800, 8, 'N', 1, 100) == -1) return;
    //send_cmd(fd2,"AT+QGPSCFG=\"gpsnmeatype\",2",sizeof("AT+QGPSCFG=\"gpsnmeatype\",2"));
    //memset(at,0,20);
    //memcpy(at,"AT+QGPS=1\r",sizeof("AT+QGPS=1\r"));
    //int nwrite = write(fd2,at,sizeof("AT+QGPS=1\r"));
    
    int nwrite = send_cmd(fd2,"AT+QGPS=1\r",sizeof("AT+QGPS=1\r"));
    char* head = NULL;
    char* end = NULL;
    const char dollor = '$';
    while(nwrite>=0) 
    {
    	memset(GPS_BUFF,0,MAX_BUF_SIZE);
    	nread = read(fd1, GPS_BUFF, MAX_BUF_SIZE);
        int ix = prase_RMC(GPS_BUFF);
        head = GPS_BUFF+ix;
        end = strpbrk(head+sizeof("$GPRMC"),&dollor);        
        int size = end == NULL? (GPS_BUFF+nread) - head : end - head;
        memset(GPS_SEND_MSG,0,sizeof(GPS_SEND_MSG));
        memcpy(GPS_SEND_MSG,head,size);
        GPS_SEND_MSG[size] = '\0';
    	if (nread > 0){
    		printf("receive GPS DATALen=%d\n",size); 
    		GPS_BUFF[nread] = '\0';
    		printf( "reveive GPS msg is:%s\n\n", GPS_SEND_MSG); 

    	}
    	us_sleep(1000);
     
    }
}
void thread_get_usb(void)
{
    int nread; 
    int fd = open("/dev/ttyUSB0",O_RDWR);
    if (fd>0 && set_opt(fd,9600,8,'N',1,34) >= 0)
    {
        printf("init Serial Port success\n\n");
    }
    else 
    {
        printf("init Serial Port fail!\n");
    }
    char usb_buf[] = {0xDD,0xA5,0x03,0x00,0xFF,0xFD,0x77};  
    hex_dump(usb_buf,sizeof(usb_buf),32,"USB_BUFF");
    while(1)
    {
        if(write(fd,usb_buf,strlen(usb_buf)) > 0)
        {
            printf("write cmd success\n\n");
            memset(USB_SEND_MSG,0,MAX_BUF_SIZE);
            nread = read(fd,USB_SEND_MSG,MAX_BUF_SIZE);
            if(nread < 34) continue;
            printf("\nLen %d\n",nread);
            hex_dump(USB_SEND_MSG,nread,128,"USB_SEND_MSG");
            //USB_SEND_MSG[nread+1]='\0';
            //printf("\n%s",USB_SEND_MSG);           
        }
        us_sleep(10);
    }

}


void thread_camera_cmd(void)
{
    int nread; 
    int fd = open("/dev/ttyUSB0",O_RDWR);
    if (fd>0 && set_opt(fd,9600,8,'N',1,34) >= 0)
    {
        printf("init Serial Port success\n\n");
    }
    else 
    {
        printf("init Serial Port fail!\n");
    }

    char usb_buf[] = {0xDD,0xA5,0x03,0x00,0xFF,0xFD,0x77};  
    hex_dump(usb_buf,sizeof(usb_buf),32,"USB_BUFF");
    while(1)
    {
        if(write(fd,usb_buf,strlen(usb_buf)) > 0)
        {
            printf("send cmd success\n\n");           
        }
        us_sleep(10);
    }

}

int main(int argc, char *argv[]) {

    reset_ram();
    OPT = parse_opt(argc,argv);
    OPT.spi_fd = spi_init(OPT.speed);   
    char buff[MAX_BUF_SIZE];
    int recv_len = -1;
    int conn_fd = -1;
    int ret = -1;
    int server_ip_port = OPT.port;
    pthread_t t_spi_read,t_wait_key,t_gps,t_usb;
    struct sockaddr_in t_sockaddr;
    memset(&t_sockaddr, 0, sizeof(t_sockaddr));
    t_sockaddr.sin_family = AF_INET;
    t_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    t_sockaddr.sin_port = htons(server_ip_port);
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        fprintf(stderr, "socket error %s errno: %d\n", strerror(errno), errno);
    }

    ret = bind(listen_fd,(struct sockaddr *) &t_sockaddr,sizeof(t_sockaddr));
    if (ret < 0) {
        fprintf(stderr, "bind socket error %s errno: %d\n", strerror(errno), errno);
    }

    ret = listen(listen_fd, 1024);
    if (ret < 0) {
        fprintf(stderr, "listen error %s errno: %d\n", strerror(errno), errno);
    }
    pthread_create(&t_spi_read,NULL,(void*)thread_spi_read,NULL);
    pthread_create(&t_gps,NULL,(void*)thread_get_gps,NULL);
    pthread_create(&t_usb,NULL,(void*)thread_get_usb,NULL);
    //pthread_create(&t_wait_key,NULL,(void*)wait_key,NULL);
    for(;;) {
        conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
        if(conn_fd < 0) {
            fprintf(stderr, "accpet socket error: %s errno :%d\n", strerror(errno), errno);
            continue;
        }

        recv_len = recv(conn_fd, buff, MAX_BUF_SIZE, 0);
        if (recv_len < 0) {
            fprintf(stderr, "recv error %s errno: %d\n", strerror(errno), errno);
            continue;
        }
        printf("socket connect success\n\n");
        SOCKET_PARA *para = malloc(sizeof(SOCKET_PARA));
        para->msg = buff;
        para->conn_fd = conn_fd; 
        msg_process(para); 
        conn_fd = -1;
        free(para);
    }

    close(listen_fd);
    listen_fd = -1;

    return 0;
}
