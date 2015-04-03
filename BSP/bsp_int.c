/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2007-2009; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      MICRIUM BOARD SUPPORT PACKAGE
*                                         INTERRUPT CONTROLLER
*
* Filename      : bsp_int.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_INT_MODULE
#include <bsp.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR_TOTAL];


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

static  void  BSP_IntHandler       (CPU_DATA  int_id);
static  void  BSP_IntHandlerDummy  (void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntClr()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id      Interrupt to clear.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClr (CPU_DATA  int_id)
{

}


/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_DATA  int_id)
{
    if (int_id < (BSP_INT_SRC_NBR_MAX + 16)) {
        CPU_IntSrcDis(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}


/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_DATA  int_id)
{
    if (int_id < (BSP_INT_SRC_NBR_MAX + 16)) {
        CPU_IntSrcEn(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (CPU_DATA       int_id,
                      CPU_FNCT_VOID  isr)
{
    CPU_SR_ALLOC();


    if (int_id < (BSP_INT_SRC_NBR_MAX + 16)) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntPrioSet()
*
* Description : Assign ISR priority.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               prio        Priority to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntPrioSet (CPU_DATA    int_id,
                      CPU_INT08U  prio)
{
    CPU_SR_ALLOC();
    

    if (int_id < (BSP_INT_SRC_NBR_MAX + 16)) {
        CPU_CRITICAL_ENTER();
        CPU_IntSrcPrioSet(int_id + 16, prio);
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts:
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    CPU_DATA  int_id;


    for (int_id = 0; int_id < (BSP_INT_SRC_NBR_MAX + 16); int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler####()
*
* Description : Handle an interrupt.
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

void  BSP_IntHandlerSUPC          (void)  { BSP_IntHandler(BSP_PER_ID_SUPC );       }
void  BSP_IntHandlerRSTC          (void)  { BSP_IntHandler(BSP_PER_ID_RSTC);        }
void  BSP_IntHandlerRTC           (void)  { BSP_IntHandler(BSP_PER_ID_RTC);         }
void  BSP_IntHandlerRTT           (void)  { BSP_IntHandler(BSP_PER_ID_RTT);         }
void  BSP_IntHandlerWDT           (void)  { BSP_IntHandler(BSP_PER_ID_WDT);         }
void  BSP_IntHandlerPMC           (void)  { BSP_IntHandler(BSP_PER_ID_PMC);         }
void  BSP_IntHandlerEEFC0         (void)  { BSP_IntHandler(BSP_PER_ID_EEFC0);       }
void  BSP_IntHandlerEEFC1         (void)  { BSP_IntHandler(BSP_PER_ID_EEFC1);       }
void  BSP_IntHandlerUART          (void)  { BSP_IntHandler(BSP_PER_ID_UART);        }
void  BSP_IntHandlerSMC           (void)  { BSP_IntHandler(BSP_PER_ID_SMC);         }
void  BSP_IntHandlerPIOA          (void)  { BSP_IntHandler(BSP_PER_ID_PIOA);        }
void  BSP_IntHandlerPIOB          (void)  { BSP_IntHandler(BSP_PER_ID_PIOB);        }
void  BSP_IntHandlerPIOC          (void)  { BSP_IntHandler(BSP_PER_ID_PIOC);        }
void  BSP_IntHandlerUS0           (void)  { BSP_IntHandler(BSP_PER_ID_US0);         }
void  BSP_IntHandlerUS1           (void)  { BSP_IntHandler(BSP_PER_ID_US1);         }
void  BSP_IntHandlerUS2           (void)  { BSP_IntHandler(BSP_PER_ID_US2);         }
void  BSP_IntHandlerUS3           (void)  { BSP_IntHandler(BSP_PER_ID_US3);         }
void  BSP_IntHandlerHSMCI         (void)  { BSP_IntHandler(BSP_PER_ID_HSMCI);       }
void  BSP_IntHandlerTWI0          (void)  { BSP_IntHandler(BSP_PER_ID_TWI0);        }
void  BSP_IntHandlerTWI1          (void)  { BSP_IntHandler(BSP_PER_ID_TWI1);        }
void  BSP_IntHandlerSPI0          (void)  { BSP_IntHandler(BSP_PER_ID_SPI0);        }
void  BSP_IntHandlerSSC0          (void)  { BSP_IntHandler(BSP_PER_ID_SSC0);        }
void  BSP_IntHandlerTC0           (void)  { BSP_IntHandler(BSP_PER_ID_TC0);         }
void  BSP_IntHandlerTC1           (void)  { BSP_IntHandler(BSP_PER_ID_TC1);         }
void  BSP_IntHandlerTC2           (void)  { BSP_IntHandler(BSP_PER_ID_TC0);         }
void  BSP_IntHandlerPWM           (void)  { BSP_IntHandler(BSP_PER_ID_PWM);         }
void  BSP_IntHandlerAD12B         (void)  { BSP_IntHandler(BSP_PER_ID_AD12B);       }
void  BSP_IntHandlerADC           (void)  { BSP_IntHandler(BSP_PER_ID_ADC);         }
void  BSP_IntHandlerDMAC          (void)  { BSP_IntHandler(BSP_PER_ID_DMAC);        }
void  BSP_IntHandlerUDPHS         (void)  { BSP_IntHandler(BSP_PER_ID_UDPHS);       }


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_IntHandler()
*
* Description : Central interrupt handler.
*
* Argument(s) : int_id          Interrupt that will be handled.
*
* Return(s)   : none.
*
* Caller(s)   : ISR handlers.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandler (CPU_DATA  int_id)
{
    CPU_FNCT_VOID  isr;
    CPU_SR_ALLOC();

    
    CPU_CRITICAL_ENTER();                                       /* Tell uC/OS-II that we are starting an ISR            */
    OSIntEnter();
    CPU_CRITICAL_EXIT();

    if (int_id < (BSP_INT_SRC_NBR_MAX + 16)) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

    OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */
}


/*
*********************************************************************************************************
*                                        BSP_IntHandlerDummy()
*
* Description : Dummy interrupt handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy (void)
{

}
