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
#include "/home/lsf/LSF/cruise_robot2/src/motordriven/src/spi.h"

typedef struct _option_
{
	char data;
	char addr;
}OPTION;

static OPTION parse_opt(int argc, char *argv[])
{
    char addr = 0;
    char data = 0;

    OPTION opt;
    while (1) {
        static const struct option lopts[] = {
            { "addr",   1, 0, 'a' },
            { "data",   1, 0, 'd' },
            { NULL, 0, 0, 0 },
        };
        int c;
        c = getopt_long(argc, argv, "a:d:", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
            case 'd':
                data = atoi(optarg);
                break;
            case 'a':
                addr = atoi(optarg);
                break;

            default:
                break;
        }
    }
    opt.addr = addr;
    opt.data = data;
    return opt;
}
int main(int argc, char *argv[]) {
            char tx[2];
            char rx[2];
            int fd = spi_init(1000000);
            OPTION OPT = parse_opt(argc,argv);
            tx[0] = OPT.addr;
            tx[1] = OPT.data;
            while(transfer(fd,tx,rx,2,1000000,0) <= 0);
    return 0;
}
