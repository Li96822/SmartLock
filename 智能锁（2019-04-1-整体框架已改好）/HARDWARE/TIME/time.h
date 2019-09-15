#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
void TIM2_Int_Init(u32 arr,u16 psc);
void TIM3_Int_Init(u32 arr,u16 psc);
void TIM7_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u32 arr,u16 psc);
extern u8 UnlockRecord[400];
extern u8 CorrectRecord[400];
extern u8 m1,k1;
extern u16 n1,l1;
extern unsigned char mm;
#endif
