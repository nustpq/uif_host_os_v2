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
*                                      TASKS HEADERS CONFIGURATION
*
*                                          Atmel AT91SAM7A3
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : taskcomm.h
* Version       : V1.0.0
* IDE           : IAR for ARM 5.40
* Programmer(s) : PQ
*
*********************************************************************************************************
* Note(s)       :  defines used for communication between tasks
*********************************************************************************************************
*/

#ifndef _TASKCOMM_H_
#define _TASKCOMM_H_


#define  MSG_TYPE_MASK          0xFF000000
#define  MSG_TYPE_RESET         0xA0000000
#define  MSG_TYPE_SWITCH        0xB0000000
#define  MSG_TYPE_PORT_DET      0xC0000000
#define  MCU_SW_RESET_PATTERN   0xA5000005


/////////  UART Message Storage Area  //////////////////
#define MsgUARTQueue_SIZE       5//10   //memory partition block numbers
#define MsgUARTBody_SIZE        4200//4096 // package header(8B) + data(4096) + EMB Ext data
#define EMB_BUF_SIZE            (MsgUARTBody_SIZE-8)// 8 bytes are for package header reserved
#define MAX_DATA_SIZE           4096 //suggest maximum data size

//
#define MAX_RESEND_TIMES        3
#define DBG_UART_Send_Buf_Size  4096 //kFIFO



extern OS_MEM       *pMEM_Part_MsgUART;
extern CPU_INT08U    MemPartition_MsgUART[MsgUARTQueue_SIZE][MsgUARTBody_SIZE];

//Msg from PC Uart to Noah
extern void     * MsgQ_PCUART2Noah[MsgUARTQueue_SIZE];
extern OS_EVENT * EVENT_MsgQ_PCUART2Noah;
//Msg from Noah to PC Uart
extern void     * MsgQ_Noah2PCUART[MsgUARTQueue_SIZE];
extern OS_EVENT * EVENT_MsgQ_Noah2PCUART;

//Msg from Ruler Uart to Noah
extern void     * MsgQ_RulerUART2Noah[MsgUARTQueue_SIZE];
extern OS_EVENT * EVENT_MsgQ_RulerUART2Noah;
//Msg from Noah to Ruler Uart
extern void     * MsgQ_Noah2RulerUART[MsgUARTQueue_SIZE];
extern OS_EVENT * EVENT_MsgQ_Noah2RulerUART;

//Msg from Noah to CMD Parse
extern void     * MsgQ_Noah2CMDParse[MsgUARTQueue_SIZE];
extern OS_EVENT * EVENT_MsgQ_Noah2CMDParse;

extern OS_EVENT *App_UserIF_Mbox; 
extern OS_EVENT *ACK_Sem_PCUART;
extern OS_EVENT *ACK_Sem_RulerUART;
extern OS_EVENT *Done_Sem_RulerUART;
extern OS_EVENT *UART_MUX_Sem_lock;

extern CPU_INT08U DBG_UART_Send_Buffer[];

extern CPU_INT08U       PcCmdTxID;
extern CPU_INT08U       PcCmdTxID_Ruler[];
extern CPU_INT32U       Tx_ReSend_Happens ;
extern CPU_INT32U       Tx_ReSend_Happens_Ruler ;

extern volatile CPU_INT08U  Global_Conn_Ready;
extern volatile CPU_INT08U  Global_Idle_Ready;
extern unsigned int         test_counter1, test_counter2,test_counter3, test_counter4, test_counter5 ;
extern CPU_INT16U debug_uart_fifo_data_max ;
extern CPU_INT16U debug_uart_fifo_oveflow_counter ;



void App_TaskUART_Tx      ( void *pdata ) ;
void App_TaskUART_Tx_Ruler( void *pdata ) ;
void App_TaskUART_Rx      ( void *pdata ) ;
void App_TaskNoah         ( void *p_arg ) ;
void App_TaskNoah_Ruler   ( void *p_arg ) ;
void App_TaskGenieShell   ( void *p_arg ) ;
void App_TaskUserIF       ( void *p_arg ) ;
void App_TaskJoy          ( void *p_arg ) ;
void App_TaskCMDParse     ( void *p_arg ) ;
void App_TaskDebugInfo    ( void *p_arg ) ;

void Task_ReCreate_Shell( void );
void Port_Detect_Enable( unsigned char on_off );





#endif


