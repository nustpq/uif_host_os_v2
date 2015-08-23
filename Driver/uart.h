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


#ifndef UART_H
#define UART_H

#include <queue.h>


#define UART0_EN                    1u
#define UART1_EN                    1u
#define UART2_EN                    1u

///////////////////////////////////////////////////////////////////////////////

#define AUDIO_UART                  0
#define PC_UART                     1 
#define RULER_UART                  2

//#define CHIP_UART                   3 // debug usart can be used to access DSP


#define UART0_SEND_QUEUE_LENGTH     64     //Audio MCU
#define UART0_RECE_QUEUE_LENGTH     64      

#define UART1_SEND_QUEUE_LENGTH     (1024)   //PC 
#define UART1_RECE_QUEUE_LENGTH     (1024)     

#define UART2_SEND_QUEUE_LENGTH     4     // not used
#define UART2_RECE_QUEUE_LENGTH     4    

#define UART_PDC_LENGTH             128//32    // 32B transfer time is ok for a interruption 

#define UART_TIMEOUT_BIT            (50 * 10) // 500 bit=50*10bit,  timeout in 50 Bytes' time, due to IT6322 slow response  

#define USART_MODE_ASYNCHRONOUS (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE)
#define USART_MODE_ASYNCHRONOUS_HW (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_USMODE_HWHSH)

///////////////////////////////////////////////////////////////////////////////



#if ( UART0_EN > 0 )
extern CPU_INT32U UART0_Send_Buf[UART0_SEND_QUEUE_LENGTH/sizeof(CPU_INT32U)];
extern CPU_INT32U UART0_Rece_Buf[UART0_RECE_QUEUE_LENGTH/sizeof(CPU_INT32U)];
#endif

#if ( UART1_EN > 0 )
extern CPU_INT32U UART1_Send_Buf[UART1_SEND_QUEUE_LENGTH/sizeof(CPU_INT32U)];
extern CPU_INT32U UART1_Rece_Buf[UART1_RECE_QUEUE_LENGTH/sizeof(CPU_INT32U)];
#endif

#if ( UART2_EN > 0 )
extern CPU_INT32U UART2_Send_Buf[UART2_SEND_QUEUE_LENGTH/sizeof(CPU_INT32U)];
extern CPU_INT32U UART2_Rece_Buf[UART2_RECE_QUEUE_LENGTH/sizeof(CPU_INT32U)];
#endif

extern volatile bool uartin_start_cmd;
extern volatile bool uartout_start_cmd;

extern CPU_INT32U * const pUART_Send_Buf[];
extern CPU_INT32U * const pUART_Rece_Buf[];

extern kfifo_t uart_tx_fifo[];
extern kfifo_t uart_rx_fifo[];

extern kfifo_t * const pUART_Send_kfifo[];
extern kfifo_t * const pUART_Rece_kfifo[];

extern CPU_INT08U UART_Init(CPU_INT08U uart_index,   void (*isr_handler)( void ), CPU_INT32U baud);
extern void ISR_PC_UART( void );
extern void ISR_Ruler_UART( void );
extern void UART_Rx_ReStart( CPU_INT08U uart_index ) ;
extern CPU_INT08U UART_WriteStart( CPU_INT08U uart_index );
extern CPU_INT08U UART_Read( CPU_INT08U uart_index, QUEUE_DATA_TYPE *pdata );
extern CPU_INT08U UART_Write(CPU_INT08U uart_index,  CPU_INT08U data );
extern void USART_Configure(    AT91S_USART *usart,
                                CPU_INT32U mode,
                                CPU_INT32U baudrate,
                                CPU_INT32U masterClock);
extern void USART_SetTransmitterEnabled(AT91S_USART *usart, CPU_INT08U enabled);
extern void USART_SetReceiverEnabled(AT91S_USART *usart,  CPU_INT08U enabled);

 
extern void DBGU_Configure( CPU_INT32U baud_rate, CPU_INT32U mclk ) ;
extern void DBGU_Write_Byte( CPU_CHAR tx_byte );
extern CPU_INT08U  DBGU_Read_Byte ( CPU_INT08U *pdata, CPU_INT32U timeout_ms );
extern CPU_INT08U  DBGU_Read_Buff ( CPU_INT08U *pdata, CPU_INT32U size, CPU_INT32U timeout_ms );
extern void DBGU_Write_Buffer( CPU_INT08U * pBuf,  CPU_INT32U size );
extern void DBGU_Read_NULL_RHR( void  );


extern void USART_SendByte( CPU_INT08U uart_index, CPU_INT08U data );
extern void USART_SendStr(  CPU_INT08U uart_index, CPU_INT08U * pStr );
extern CPU_INT08U USART_SendBuf( CPU_INT08U uart_index, CPU_INT08U * pBuf,  CPU_INT32U size);
extern CPU_INT08U USART_Read_Timeout( CPU_INT08U uart_index, void *buffer, CPU_INT32U size, CPU_INT32U timeout_ms );
extern CPU_INT08U USART_SendBuf_Status( CPU_INT08U uart_index);
extern CPU_INT08U USART_Start_Ruler_Bootloader( void );

extern OS_EVENT *DBGU_Tx_Sem_lock;  
extern OS_EVENT *DBGU_Rx_Sem_lock;


#endif



