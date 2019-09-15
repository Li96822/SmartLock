#ifndef __L620_H
#define __L620_H
#include "stm32f10x.h"


extern unsigned char IPtable[48];
extern u8 timebuffer[7];
extern u16 year;
extern u8 month,date,hour,min,sec;
extern u8 Usercount;
extern u8 UserName[8];
extern char pSend[48];
void Clear_Buffer(void);//Çå¿Õ»º´æ	
void L620_Init(void);
void strcpymessage(unsigned char *s,unsigned char *des,unsigned char i,unsigned char len);
void getTime(unsigned char *timebuffer);
unsigned char receivedData(void);
unsigned char getUserMessage(void);
unsigned char delUserMessage(void);
unsigned char L620Match(unsigned char *s,unsigned char *d,unsigned char len);
unsigned char IPMessage(void);
unsigned char Length(unsigned char *P);
#endif

