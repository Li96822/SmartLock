#ifndef __BREAK_H
#define __BREAK_H

void password(void);
//unsigned char match(unsigned char *line,unsigned char *mat);
//unsigned char Match(unsigned char *line,unsigned char *mat);
extern unsigned char table3[11];
extern unsigned char table4[6];
extern unsigned char Flag,temp2,Correctnum,CorrectFlag,Recordnum,RecordFlag;
extern unsigned int addr,addr1;
extern unsigned char Keytable[2];
unsigned char Match(unsigned char a,unsigned char b);
unsigned char MATCH(void);
void shuaka(void);
void Permission(void);
void finger(void);
void remote(void);
void Mima_kaisuo(void);
//unsigned char Match(unsigned char line[11],unsigned char mat[6]);
#endif
