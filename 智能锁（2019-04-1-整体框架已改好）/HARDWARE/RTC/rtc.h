#ifndef __RTC_H
#define __RTC_H	
#include "stm32f10x.h"
//Mini STM32������
//RTCʵʱʱ�� ��������			 
//����ԭ��@ALIENTEK
//2010/6/6

//ʱ��ṹ��
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//������������
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;

extern _calendar_obj calendar;	//�����ṹ��

extern u8 const mon_table[12];	//�·��������ݱ�
extern u8 time[6];

u8 RTC_Init(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u8 Is_Leap_Year(u16 year);//ƽ��,�����ж�

u8 RTC_Get(void);         //����ʱ��   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//����ʱ��
void PasswordTime(void);
void FingerTime(void);
void ShuakaTime(void);
void PasswordTime1(void);
void FingerTime1(void);
#endif
