#ifndef __TIMER_H__
#define __TIMER_H__


#include  <cpu.h>

/*
//#define DELAY_US_METHOD_INT

extern void TIMER_Init(void);
extern int  TIMER_Get(void);
extern void TIMER_Release(short timer);

extern void TIMER_Start(short timer,short over);
extern void TIMER_Pause(short timer);
extern void TIMER_Resume(short timer);
extern int  TIMER_Quire(short timer);

extern void Delay_(void) ;
extern void AT91_PIT_Init( void );
*/

extern void AT91_TC_Init( CPU_INT08U timer_index, CPU_INT08U timer_div, void (*handler)( void ), CPU_INT32U delay_us );
extern void Timer_Init( void );
extern void delay_us(CPU_INT32U delay_us) ;
extern void delay_ms(CPU_INT32U delay_ms) ;




#endif
