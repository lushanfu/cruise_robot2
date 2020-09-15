#ifndef  _GPS_H_
#define  _GPS_H_

int prase_RMC(char* msg);
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop,int nword);




#endif
