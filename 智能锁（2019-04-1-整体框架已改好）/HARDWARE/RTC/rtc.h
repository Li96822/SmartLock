#ifndef __RTC_H
#define __RTC_H	
#include "stm32f10x.h"
//Mini STM32开发板
//RTC实时时钟 驱动代码			 
//正点原子@ALIENTEK
//2010/6/6

//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;

extern _calendar_obj calendar;	//日历结构体

extern u8 const mon_table[12];	//月份日期数据表
extern u8 time[6];

u8 RTC_Init(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);        //初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);//平年,闰年判断

u8 RTC_Get(void);         //更新时间   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间
void PasswordTime(void);
void FingerTime(void);
void ShuakaTime(void);
void PasswordTime1(void);
void FingerTime1(void);
#endif
