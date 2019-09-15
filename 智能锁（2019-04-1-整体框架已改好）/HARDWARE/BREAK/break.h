#ifndef __BREAK_H
#define __BREAK_H	
#include "stm32f10x.h"
#include "sys.h"
//u8 match(unsigned char line[],unsigned char *mat);
void password(void);
extern unsigned char table3[11];
extern unsigned char table4[6];
extern unsigned char table1[6];
extern unsigned char Keytable[2];
extern u8 Flag,temp2,Correctnum,CorrectFlag,Recordnum,RecordFlag;
extern unsigned int addr,addr1;
unsigned char Match(unsigned char a,unsigned char b);
unsigned char MATCH(void);
void shuaka(void);
void Permission(void);
void finger(void);
void remote(void);
void Mima_kaisuo(void);
//unsigned char Match(unsigned char line[11],unsigned char mat[6]);


//u8 Match(u8 *line,u8 *mat);
#endif

