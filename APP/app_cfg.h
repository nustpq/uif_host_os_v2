/*
*********************************************************************************************************
*                               UIF BOARD APP PACKAGE
*
*                            (c) Copyright 2013 - 2016; Fortemedia Inc.; Nanjing, China
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
*                                      APPLICATION CONFIGURATION
*
*                                          Atmel AT91SAM7A3
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : app_cfg.h
* Version       : V1.0.0
* IDE           : IAR for ARM 5.40
* Programmer(s) : PQ
*
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/



#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


#define  DBG_UART_METHOD_TASK_EN  //this define enable the DBG UART in task for speed up

/*
*********************************************************************************************************
*                                   ADDITIONAL uC/MODULE ENABLES
*********************************************************************************************************
*/
/* DEF_ENABLED = Present, DEF_DISABLED = Not Present  */

#define  APP_CFG_PROBE_OS_PLUGIN_EN                 DEF_DISABLED   /** PQ **/
#define  APP_CFG_PROBE_COM_EN                       DEF_DISABLED
#define  APP_CFG_PROBE_DEMO_INTRO_EN                DEF_DISABLED
//and need to disable APP_CFG_TRACE define in this head file       

/*
*********************************************************************************************************
*                                        TASK PRIORITIES
*********************************************************************************************************
*/


#define  APP_CFG_TASK_UART_TX_PRIO                         3
#define  APP_CFG_TASK_UART_TX_RULER_PRIO                   13
#define  APP_CFG_TASK_UART_RX_PRIO                         5
#define  APP_CFG_TASK_NOAH_PRIO                            4
#define  APP_CFG_TASK_NOAH_RULER_PRIO                      16
#define  APP_CFG_TASK_CMD_PARSE_PRIO                       2



#define  APP_CFG_TASK_USER_IF_PRIO                         10
#define  APP_CFG_TASK_JOY_PRIO                   (APP_CFG_TASK_USER_IF_PRIO+1)

#define  APP_CFG_TASK_SHELL_PRIO                           20
#define  APP_CFG_TASK_START_PRIO                           30

#define  APP_CFG_TASK_DBG_INFO_PRIO                        21

#define  APP_CFG_TASK_PROBE_STR_PRIO                       37
#define  PROBE_DEMO_INTRO_CFG_TASK_LED_PRIO                38

#define  OS_PROBE_TASK_PRIO                                40

#define  OS_PROBE_TASK_ID                                  40

#define  OS_TASK_TMR_PRIO                         (OS_LOWEST_PRIO - 2)


/*
*********************************************************************************************************
*                                        TASK STACK SIZES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE                     128
#define  APP_CFG_TASK_USER_IF_STK_SIZE                   256
#define  APP_CFG_TASK_JOY_STK_SIZE                       128
#define  APP_CFG_TASK_SHELL_STK_SIZE                     256
#define  APP_CFG_TASK_DBG_INFO_STK_SIZE                  128

#define  APP_CFG_TASK_UART_TX_STK_SIZE                   128
#define  APP_CFG_TASK_UART_TX_RULER_STK_SIZE             128
#define  APP_CFG_TASK_UART_RX_STK_SIZE                   128
#define  APP_CFG_TASK_NOAH_STK_SIZE                      128
#define  APP_CFG_TASK_NOAH_RULER_STK_SIZE                128
#define  APP_CFG_TASK_CMD_PARSE_STK_SIZE                 256

#define  PROBE_DEMO_INTRO_CFG_TASK_LED_STK_SIZE          256

#define  OS_PROBE_TASK_STK_SIZE                          512

/*
*********************************************************************************************************
*                               uC/Probe plug-in for uC/OS-II CONFIGURATION
*********************************************************************************************************
*/

#define  OS_PROBE_TASK                                     1    /* Task will be created for uC/Probe OS Plug-In             */
#define  OS_PROBE_TMR_32_BITS                              0    /* uC/Probe OS Plug-In timer is a 32-bit timer              */
#define  OS_PROBE_TMR_SEL                                  2    /* Select timer 2 for uC/Probe OS Plug-In timer             */
#define  OS_PROBE_HOOKS_EN                                 1    /* Hooks to update OS_TCB profiling members will be included*/

/*
*********************************************************************************************************
*                                      uC/OS-II DCC CONFIGURATION
*********************************************************************************************************
*/

#define  OS_CPU_ARM_DCC_EN                                 1

/*
*********************************************************************************************************
*                                        uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN               DEF_ENABLED


/*
*********************************************************************************************************
*                                          BSP CONFIGURATION
*********************************************************************************************************
*/

#define   BSP_CFG_SER_COMM_EN                      DEF_ENABLED
#define   BSP_CFG_SER_COMM_SEL                     BSP_SER_COMM_UART_DBG  //modified to a global varies: Debug_COM_Sel

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DEBUG                                 2

/////////////////////////////////////////////////////////////////
#define  BSP_CFG_TRACE_LEVEL           TRACE_LEVEL_OFF

#define  BSP_CFG_TRACE                 BSP_Ser_Printf   
#define  BSP_TRACE_INFO(x)            ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO)  ? (void)(BSP_CFG_TRACE x) : (void)0)
#define  BSP_TRACE_DBG(x)             ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DBG)   ? (void)(BSP_CFG_TRACE x) : (void)0)

////////////////////////////////////////////////////////////////
#define  APP_CFG_TRACE_LEVEL          TRACE_LEVEL_INFO     //PQ

#define  APP_CFG_TRACE                 BSP_Ser_Printf   ////Disable here is use uC Probe  
#define  APP_TRACE_INFO(x)            ((APP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)             ((APP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DEBUG) ? (void)(APP_CFG_TRACE x) : (void)0)

// for shell uart
#define  UART_SHELL_SEND_STR(x)                (void)( BSP_Ser_Printf x )      
#define  UART_SHELL_GET_BYTE(x)                      ( BSP_Ser_RdByte x )
#define  UART_SHELL_SEND_BYTE(x)               (void)( BSP_Ser_WrByte x )  

/*
*********************************************************************************************************
*                                    
*********************************************************************************************************
*/





#endif
