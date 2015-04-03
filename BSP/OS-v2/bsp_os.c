/*
*********************************************************************************************************
*                                     MICRIUM BOARD SUPPORT SUPPORT
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
*                                    MICRIUM BOARD SUPPORT PACKAGE
*                                             OS LAYER
*
* Filename      : bsp_os.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_OS_MODULE
#include <bsp.h>

#if (APP_CFG_PROBE_COM_EN == DEF_ENABLED)
#include <app_probe.h>
#endif

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_REG_DWT_CR            (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT        (*(CPU_REG32 *)0xE0001004)


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



/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*********************************************************************************************************
*                             uC/Probe PLUG-IN FOR uC/OS-II FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       OSProbe_TmrInit()
*
* Description : Select & initialize a timer for use with the uC/Probe Plug-In for uC/OS-II.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
    (OS_PROBE_HOOKS_EN          == 1          )
void  OSProbe_TmrInit (void)
{
    BSP_REG_DWT_CR |= DEF_BIT_00;
}
#endif


/*
*********************************************************************************************************
*                                        OSProbe_TmrRd()
*
* Description : Read the current counts of a 16-bit free running timer.
*
* Argument(s) : none.
*
* Return(s)   : The 16 bit counts (in a 32 bit variable) of the timer.
*********************************************************************************************************
*/

#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
    (OS_PROBE_HOOKS_EN          == 1          )
CPU_INT32U  OSProbe_TmrRd (void)
{
    CPU_INT32U  tmr_cnts;
    
    
    tmr_cnts = BSP_REG_DWT_CYCCNT;
    
    return (tmr_cnts);    
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
**                                     BSP OS LOCKS FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      BSP_OS_SemCreate()
*
* Description : Creates a sempahore to lock/unlock
*
* Argument(s) : p_sem        Pointer to a BSP_OS_SEM structure
*  
*               sem_val      Initial value of the semaphore.
*
*               p_sem_name   Pointer to the semaphore name.
*
* Return(s)   : DEF_OK       if the semaphore was created.
*               DEF_FAIL     if the sempahore could not be created.
*     
* Caller(s)   : Application.
*
* Note(s)     : none.
*
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OS_SemCreate (BSP_OS_SEM       *p_sem,
                               BSP_OS_SEM_VAL    sem_val,
                               CPU_CHAR         *p_sem_name)
{
    OS_EVENT    *p_event;

#if (OS_EVENT_NAME_EN > 0)
    CPU_INT08U  err;
#endif
    
    p_event = OSSemCreate(sem_val);
    
    if (p_event == (OS_EVENT *)0) {
        return (DEF_FAIL);    
    }
    
    *p_sem = (BSP_OS_SEM)(p_event);
    
#if (OS_EVENT_NAME_EN > 0)
    OSEventNameSet((OS_EVENT *)p_event, 
                   (INT8U    *)p_sem_name, 
                   (INT8U    *)&err);
#endif
    

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                     BSP_OS_SemWait()
*
* Description : Wait on a semaphore to become available
*
* Argument(s) : sem          sempahore handler
*  
*               dly_ms       delay in miliseconds to wait on the semaphore
*
* Return(s)   : DEF_OK       if the semaphore was acquire
*               DEF_FAIL     if the sempahore could not be acquire
*     
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OS_SemWait (BSP_OS_SEM  *p_sem,
                             CPU_INT32U   dly_ms)
{
    CPU_INT08U  err;
    CPU_INT32U  dly_ticks;
    OS_EVENT    *p_event;
    
    
    dly_ticks  = ((dly_ms * DEF_TIME_NBR_mS_PER_SEC) / OS_TICKS_PER_SEC);
    p_event    = *p_sem;
    
    OSSemPend((OS_EVENT   *)p_event,
              (CPU_INT32U  )dly_ticks,
              (CPU_INT08U *)&err);

    if (err != OS_ERR_NONE) {
       return (DEF_FAIL);
    }

    return (DEF_OK);
}

/*
*********************************************************************************************************
*                                      BSP_OS_SemPost()
*
* Description : Post a semaphore
*
* Argument(s) : sem          Semaphore handler
*  
* Return(s)   : DEF_OK     if the semaphore was posted.
*               DEF_FAIL      if the sempahore could not be posted.
*     
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OS_SemPost (BSP_OS_SEM  *p_sem)
{
    CPU_INT08U  err;
    OS_EVENT    *p_event;
    
    
    p_event    = *p_sem;        
    err        = OSSemPost((OS_EVENT *)p_event);
    
    if (err != OS_ERR_NONE) {
        return (DEF_FAIL);
    }

    return (DEF_OK);
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                     uC/OS-II TIMER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    BSP_TimeDlyMs()
*
* Description : This function delay the exceution for specifi amount of miliseconds
*
* Argument(s) : dly_ms       Delay in miliseconds
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*               
*********************************************************************************************************
*/

void   BSP_OS_TimeDlyMs (CPU_INT32U  dly_ms)
{
    CPU_INT16U  os_ms;
    CPU_INT08U  os_sec;
    

    if (dly_ms > 10000) {                                       /* Limit delays to 10 seconds.                        */
        dly_ms = 10000;
    }

    os_sec = dly_ms / 1000;
    os_ms  = dly_ms % 1000;

    OSTimeDlyHMSM(0, 0, os_sec, os_ms);    
}


/*
*********************************************************************************************************
*                                            BSP_OS_TmrTickInit()
*
* Description : Initialize uC/OS-II's tick source.
*
* Argument(s) : ticks_per_sec              Number of ticks per second.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void   BSP_OS_TmrTickInit (CPU_INT32U  tick_rate)
{
    CPU_INT32U  cnts;
    CPU_INT32U  cpu_freq;
    

    cpu_freq = BSP_SysClkFreqGet(BSP_SYS_CLK_ID_MCLK);
    cnts     = (cpu_freq / tick_rate);

    OS_CPU_SysTickInit(cnts);
        
}
