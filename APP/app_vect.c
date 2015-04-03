/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                     CORTEX M3 APPLICATION EXCEPTION VECTORS
*                                             
*
*
* Filename      : app_vect-v5.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

typedef  union {
    CPU_FNCT_VOID   Fnct;
    void           *Ptr;
} APP_INTVECT_ELEM;



/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#pragma language=extended
#pragma segment="CSTACK"

static  void       App_NMI_ISR        (void);

static  void       App_Fault_ISR      (void);

static  void       App_BusFault_ISR   (void);

static  void       App_UsageFault_ISR (void);

static  void       App_MemFault_ISR   (void);

static  void       App_Spurious_ISR   (void);

extern  void       __iar_program_start(void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  EXCEPTION / INTERRUPT VECTOR TABLE
*
* Note(s) : (1) The Cortex-M3 may have up to 256 external interrupts, which are the final entries in the
*               vector table.  The AT91SAM3U has 30 external interrupt vectors.
*
*           (2)  The name "__vector_table" has special meaning for C-SPY: 
*                it is where the SP start value is found, and the NVIC vector table register (VTOR) is 
*                 initialized to this address if != 0.
*********************************************************************************************************
*/

#pragma language=extended
#pragma segment="CSTACK"
#pragma section = ".vectors"
#pragma location = ".vectors"

const  APP_INTVECT_ELEM  __vector_table[] = {
    { .Ptr = (void *)__sfe( "CSTACK" )},                        /*  0, SP start value.                                  */
    __iar_program_start,                                        /*  1, PC start value.                                  */
    App_NMI_ISR,                                                /*  2, NMI.                                             */
    App_Fault_ISR,                                              /*  3, Hard Fault.                                      */
    App_MemFault_ISR,                                           /*  4, Memory Management.                               */
    App_BusFault_ISR,                                           /*  5, Bus Fault.                                       */
    App_UsageFault_ISR,                                         /*  6, Usage Fault.                                     */
    App_Spurious_ISR,                                           /*  7, Reserved.                                        */
    App_Spurious_ISR,                                           /*  8, Reserved.                                        */
    App_Spurious_ISR,                                           /*  9, Reserved.                                        */
    App_Spurious_ISR,                                           /* 10, Reserved.                                        */
    App_Spurious_ISR,                                           /* 11, SVCall.                                          */
    App_Spurious_ISR,                                           /* 12, Debug Monitor.                                   */
    App_Spurious_ISR,                                           /* 13, Reserved.                                        */
    OS_CPU_PendSVHandler,                                       /* 14, PendSV Handler.                                  */
    OS_CPU_SysTickHandler,                                      /* 15, uC/OS-II Tick ISR Handler.                       */

    BSP_IntHandlerSUPC,                                         /* 16 +  0 Supply controller                            */
    BSP_IntHandlerRSTC,                                         /* 16 +  1 Reset  controller                            */
    BSP_IntHandlerRTC,                                          /* 16 +  2 Real Time Clock                              */
    BSP_IntHandlerRTT,                                          /* 16 +  3 Real Time Timer                              */
    BSP_IntHandlerWDT,                                          /* 16 +  4 Watchdog  Timer                              */
    BSP_IntHandlerPMC,                                          /* 16 +  5 Power Managment controller                   */
    BSP_IntHandlerEEFC0,                                        /* 16 +  6 Enhanced Embedded Flash Ctrl 0               */
    BSP_IntHandlerEEFC1,                                        /* 16 +  7 Enhanced Embedded Flash Ctrl 1               */
    BSP_IntHandlerUART ,                                        /* 16 +  8 Universal Asyncronous Rx/Tx                  */
    BSP_IntHandlerSMC,                                          /* 16 +  9 Static Memory Controller                     */
    BSP_IntHandlerPIOA,                                         /* 16 + 10 Parallel I/O Controller A                    */
    BSP_IntHandlerPIOB,                                         /* 16 + 11 Parallel I/O Controller B                    */
    BSP_IntHandlerPIOC,                                         /* 16 + 12 Parallel I/O Controller C                    */
    BSP_IntHandlerUS0,                                          /* 16 + 13 USART 0                                      */
    BSP_IntHandlerUS1,                                          /* 16 + 14 USART 1                                      */
    BSP_IntHandlerUS2,                                          /* 16 + 15 USART 2                                      */
    BSP_IntHandlerUS2,                                          /* 16 + 16 USART 3                                      */
    BSP_IntHandlerHSMCI,                                        /* 16 + 17 High Speed MCI                               */
    BSP_IntHandlerTWI0,                                         /* 16 + 18 Two Wire Interface  0                        */
    BSP_IntHandlerTWI1,                                         /* 16 + 19 Two Wire Interface  1                        */
    BSP_IntHandlerSPI0,                                         /* 16 + 20 Serial Peripheral Interface 0                */
    BSP_IntHandlerSSC0,                                         /* 16 + 21 Synchronous Serial Controller 0              */
    BSP_IntHandlerTC0,                                          /* 16 + 22 Timer/Counter 0                              */
    BSP_IntHandlerTC1,                                          /* 16 + 23 Timer/Counter 1                              */
    BSP_IntHandlerTC2,                                          /* 16 + 24 Timer/counter 2                              */
    BSP_IntHandlerPWM,                                          /* 16 + 25 Pulse Width Modulation Ctrl                  */
    BSP_IntHandlerAD12B,                                        /* 16 + 26 12-BHit ADC Controller                       */
    BSP_IntHandlerADC,                                          /* 16 + 27 10-bit ADC Controller                        */
    BSP_IntHandlerDMAC,                                         /* 16 + 28 DMA Controller                               */
    BSP_IntHandlerUDPHS,                                        /* 16 + 29 USB Device High Speed                        */
    App_Spurious_ISR,                                           /* 16 + 30 not used                                     */
};

/*
*********************************************************************************************************
*                                           __low_level_init()
*
* Description : Perform low-level initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : IAR startup code.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int __low_level_init( void )
{
    unsigned int * src = __section_begin(".vectors");

    BSP_LowLevelInit();

    AT91C_BASE_NVIC->NVIC_VTOFFR = ((unsigned int)(src)) | (0x0 << 7);
    
    return 1; 
}


/*
*********************************************************************************************************
*                                            App_NMI_ISR()
*
* Description : Handle Non-Maskable Interrupt (NMI).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : (1) Since the NMI is not being used, this serves merely as a catch for a spurious
*                   exception.
*********************************************************************************************************
*/

static  void  App_NMI_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}

/*
*********************************************************************************************************
*                                             App_Fault_ISR()
*
* Description : Handle hard fault.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_Fault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_BusFault_ISR()
*
* Description : Handle bus fault.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_BusFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                          App_UsageFault_ISR()
*
* Description : Handle usage fault.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_UsageFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_MemFault_ISR()
*
* Description : Handle memory fault.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_MemFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_Spurious_ISR()
*
* Description : Handle spurious interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_Spurious_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}
