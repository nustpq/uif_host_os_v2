#include  <board.h>
#include  <bsp.h>
#include  <at91sam3u4.h>
#include  "timer.h"
#include  <pio.h>



#define TIMER_C



static AT91PS_TC  pTCREG[] =
{
     AT91C_BASE_TC0,    
     AT91C_BASE_TC1,  
     AT91C_BASE_TC2    

};

static CPU_INT08U ppcer_offset[]=
{  
     AT91C_ID_TC0,
     AT91C_ID_TC1,
     AT91C_ID_TC2
};




#if 0

static __ramfunc void TIMER0_ISR( void )        
{
  
    CPU_INT32U status = pTCREG[0]->TC_SR;	
    
    if((status & AT91C_TC_CPCS) ) {        
        pTCREG[0]->TC_CCR = AT91C_TC_CLKEN +  AT91C_TC_SWTRG ; //restart timer  
        //CtrFlage.pcCmdTimeOver = 1; //set global flag
    }    
}


static __ramfunc void TIMER1_ISR( void )        
{
  
    CPU_INT32U status = pTCREG[1]->TC_SR;
	
    if((status & AT91C_TC_CPCS) ) {
       //pTCREG[0]->TC_CCR = AT91C_TC_CLKDIS;   //stop timer
       pTCREG[1]->TC_CCR = AT91C_TC_SWTRG | AT91C_TC_CLKEN;  //restart timer  
       
    }       
}


static __ramfunc void TIMER2_ISR( void )  
{
    CPU_INT32U status = pTCREG[2]->TC_SR;  
    
    if((status & AT91C_TC_CPCS) )  {      
        pTCREG[2]->TC_CCR = AT91C_TC_CLKDIS; //stop timer
        //CtrFlage.Time2Over = 1;  //set global flag
        
    }
}

static void (*pTimer_ISR[])(void)=
{
    TIMER0_ISR,
    TIMER1_ISR,
    TIMER2_ISR
  
};


#endif



void AT91_TC_Init( CPU_INT08U timer_index, CPU_INT08U timer_div, void (*handler)( void ), CPU_INT32U delay_us )
{
    CPU_INT16U DIV[5] = {         2,//1 us ~ 2730 us
                                  8,
                                  32,
                                  128,
                                  1024  // 22us - 1.4s maxium
                        };   
    CPU_INT32U counter_top =   MCK / (1000*1000) * delay_us / ( DIV[timer_div] )  ; 
    
    if( counter_top & 0xFFFF0000 ) {
        counter_top = 0xFFFF; // if exceeds TC_RC ...
       
    }
    AT91C_BASE_PMC->PMC_PCER = (1 << ppcer_offset[timer_index]);
    pTCREG[timer_index]->TC_CCR = AT91C_TC_CLKDIS;
    pTCREG[timer_index]->TC_IDR = 0xFFFFFFFF;
    pTCREG[timer_index]->TC_CMR = timer_div
                                | AT91C_TC_CPCSTOP
                                | AT91C_TC_CPCDIS
                                | AT91C_TC_WAVESEL_UP_AUTO
                                | AT91C_TC_WAVE;
    
    pTCREG[timer_index]->TC_RC  = (CPU_INT16U) counter_top ;
    
    if( handler != NULL ) {
        pTCREG[timer_index]->TC_IER = AT91C_TC_CPCS | AT91C_TC_COVFS;    
        
        BSP_IntVectSet( ppcer_offset[timer_index],
                        (CPU_FNCT_VOID)handler);        
        BSP_IntClr(ppcer_offset[timer_index] );     
        BSP_IntEn(ppcer_offset[timer_index]); 
        pTCREG[timer_index]->TC_CCR = AT91C_TC_CLKEN +  AT91C_TC_SWTRG ;
        
    }    
    
}









/******************************************************************************
 Function Description:  Delay us in session, 
                        Inquiry method is used instead of interruption, fater!
 
 @param:   delay_us,  1. Range :   1 us  ~ 2730 us.               
                      2. Delay time is not guaranteed  when < 10 us
                      3. One PIO_Set/Clear operation time is compensated.

 @return:  void.

    Note:   1. Compile None Optimizations, exe in  __ramfunc.
            2. This function must be called in a session noah cmd, or used between 
                OS_ENTER_CRITICAL() to OS_EXIT_CRITICAL() 
*******************************************************************************/

void  __ramfunc delay_us(CPU_INT32U delay_us)  
{   
  
    CPU_INT32U i ;
    CPU_INT32U counter_top;    
    
    if(  delay_us <=  1  ){
       __asm ("NOP");
       __asm ("NOP");
       __asm ("NOP");
       
    } else if( delay_us == 2 ) {       
      for ( i = 0 ; i< 6; i++) {
        __asm ("NOP");  
      } 
        
    } else {
        // MCK / (1000*1000) / ( DIV[timer_div] ) * delay_us ; = 48000000/1000000/2 *delay_us = 24*  *delay_us   
        counter_top =  (delay_us - 2) * 24 ;               
        if( counter_top & 0xFFFF0000 ) {
            counter_top = 0; // if exceeds TC_RC ...       
        }   
        pTCREG[2]->TC_RC  = counter_top;// if exceeds TC_RC ...
        pTCREG[2]->TC_CCR = AT91C_TC_CLKEN +  AT91C_TC_SWTRG ;
        while( !( pTCREG[2]->TC_SR & AT91C_TC_CPCS) ) ;  
        pTCREG[2]->TC_CCR = AT91C_TC_CLKDIS; 
    }  
   
    
}






/******************************************************************************
 Function Description:  Delay ms in session, 
                        Inquiry method is used instead of interruption, fater!
 
 @param:   delay_us,  1. Range :   1 ms  ~ 174 ms.               
                      2. Delay time is not guaranteed  when < 10 us ?

 @return:  void.

    Note:   1. Compile None Optimizations, exe in flash not __ramfunc.
            2. This function must be called in a session noah cmd, or used between 
                OS_ENTER_CRITICAL() to OS_EXIT_CRITICAL() 
*******************************************************************************/

void  delay_ms( CPU_INT32U delay_ms ) //
{   
    CPU_INT32U counter_top;
    CPU_INT32U counter_cycle;
    
    // MCK / (1000) / ( DIV[timer_div] ) * delay_ms ; = 48000000/1000/128 *delay_ms = 375 * delay_ms    
    counter_cycle   = delay_ms / 174 ;
    counter_top     = delay_ms - counter_cycle * 174 ;
    
    if( counter_cycle > 0 ) {     
        while( counter_cycle-- >0 ) { 
            pTCREG[1]->TC_RC  = 174 * 375 ;
            pTCREG[1]->TC_CCR = AT91C_TC_CLKEN +  AT91C_TC_SWTRG ;
            while( !(pTCREG[1]->TC_SR & AT91C_TC_CPCS) ) ;  
        }
        
    }             
    counter_top =  (counter_top - 0) * 375 ;   
    pTCREG[1]->TC_RC  = counter_top;
    pTCREG[1]->TC_CCR = AT91C_TC_CLKEN +  AT91C_TC_SWTRG ;
    
    while( !(pTCREG[1]->TC_SR & AT91C_TC_CPCS) ) ;  // real app use
    //while( pTCREG[1]->TC_CV < counter_top ) ;  //debug use, 
    
    pTCREG[1]->TC_CCR = AT91C_TC_CLKDIS;     
 
    
}





void Timer_Init( void )
{  
    //AT91_TC_Init(0, AT91C_TC_CLKS_TIMER_DIV5_CLOCK,pTimer_ISR[0], 1000*1000) ; // 1000ms interval for : Task_Transmit_Data
    
    AT91_TC_Init(1, AT91C_TC_CLKS_TIMER_DIV4_CLOCK, NULL, 1) ; // Init for function:  delay_ms() 

    AT91_TC_Init(2, AT91C_TC_CLKS_TIMER_DIV1_CLOCK, NULL, 1) ;     // Init for function:  delay_us() 
  
    
}
    
