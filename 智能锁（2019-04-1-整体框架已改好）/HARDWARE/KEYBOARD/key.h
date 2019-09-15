#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f10x.h"
#include "sys.h" 
#include "delay.h"

#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键2 
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 


void KEY_Init(void);

void fingler (void);

u8 keyScan(u8 mode);  		
extern u8 t;
extern u8 keyvalue;
//extern u32 readvalue;
#endif
