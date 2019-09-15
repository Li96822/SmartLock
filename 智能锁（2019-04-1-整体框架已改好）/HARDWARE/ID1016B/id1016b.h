#ifndef __ID1016B_H
#define __ID1016B_H
#include "stm32f10x.h"
void cleanRx4Buffer(void);
int id1016bMatch(unsigned char* s,unsigned char* d,int len);
int id1016bInit(void);
int getImage(void);
int fingerDetect(void);
int storeFinger(int num);
int generateFinger0(void);
int generateFinger1(void);
int generateFinger2(void);
int mergeFinger(void);
int addFinger(int num);
int getStatus(int num);
int delAllFinger(void);
int delSomeFinger(unsigned char begin,unsigned char end);
int searchFinger(void);
#endif
