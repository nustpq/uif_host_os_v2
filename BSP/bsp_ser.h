/*
*********************************************************************************************************
*                                     MICRIUM BOARD SUPPORT SUPPORT
*
*                          (c) Copyright 2003-2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        MICIUM BOARD SUPPORT PACKAGE
*                                         SERIAL (UART) INTERFACE
*
* Filename      : bsp_ser.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP_SER present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_SER_PRESENT
#define  BSP_SER_PRESENT


/*
*********************************************************************************************************
*                                              EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_SER_MODULE
#define  BSP_SER_EXT
#else
#define  BSP_SER  extern
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

#define  BSP_SER_COMM_UART_00                     0
#define  BSP_SER_COMM_UART_01                     1
#define  BSP_SER_COMM_UART_02                     2
#define  BSP_SER_COMM_UART_03                     3
#define  BSP_SER_COMM_UART_DBG                    2


/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/


void        BSP_Ser_Init    (CPU_INT32U   baud_rate);

CPU_INT08U  BSP_Ser_RdByte  (void);

void        BSP_SerRdStr   (CPU_CHAR    *p_str,
                            CPU_INT16U   len);

void        BSP_Ser_WrByte  (CPU_INT08U   c);

void        BSP_SerWrStr   (CPU_CHAR    *tx_str);

void        BSP_Ser_Printf  (CPU_CHAR    *format, ...);



/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#ifndef  BSP_CFG_SER_COMM_SEL
#error  "BSP_CFG_SER_COMM_SEL   not  #define'd in 'app_cfg.h'                     "
#error  "                                 [     ||  BSP_SER_COMM_UART_DBG]        "
#error  "                                 [     ||  BSP_SER_COMM_UART_UART01]     "

#elif   (BSP_CFG_SER_COMM_SEL != BSP_SER_COMM_UART_DBG) && \
        (BSP_CFG_SER_COMM_SEL != BSP_SER_COMM_UART_01)

#error  "BSP_CFG_SER_COMM_SEL  illegally #define'd in 'app_cfg.h'                 "
#error  "                                 [     ||  BSP_SER_COMM_UART_DBG]        "
#error  "                                 [MUST be  BSP_SER_COMM_UART_01]         "
#endif
#endif    
