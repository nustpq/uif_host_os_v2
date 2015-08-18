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
*                                           TASK PACKAGE
*
*                                          Atmel AT91SAM7A3
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : task_user_if.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/


#include <includes.h>

#define DBG_UART_BUF_SIZE    256

CPU_INT08U DBG_UART_Send_Buffer[ DBG_UART_Send_Buf_Size ];

CPU_INT16U debug_uart_fifo_data_max ;
CPU_INT16U debug_uart_fifo_oveflow_counter ;

kfifo_t DBG_UART_Send_kFIFO;



/*
*********************************************************************************************************
*                                         App_TaskDebugInfo()
*
* Description : Check DBG_UART_Send_Buffer[] and Send debug data if kFIFO buffer is not empty  
*
* Argument(s) : p_arg       Argument passed to 'App_TaskUserIF()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
void  App_TaskDebugInfo (void *p_arg)
{
  
    CPU_INT16U   i, size ;
    CPU_CHAR     uart_data[ DBG_UART_BUF_SIZE ] ;
    
    (void)p_arg;    
    
    debug_uart_fifo_data_max = 0;
    debug_uart_fifo_oveflow_counter = 0;
    
    kfifo_init_static( &DBG_UART_Send_kFIFO, (unsigned char *)DBG_UART_Send_Buffer, DBG_UART_Send_Buf_Size);
   
    
    while ( DEF_TRUE ) {                   /* Task body, always written as an infinite loop.           */   
         
        size  = kfifo_get_data_size( &DBG_UART_Send_kFIFO ); 
        if( size ) { 
            if( size > debug_uart_fifo_data_max ) {
                debug_uart_fifo_data_max = size;
            }
            size = size < DBG_UART_BUF_SIZE ?  size  :  DBG_UART_BUF_SIZE;
            kfifo_get( &DBG_UART_Send_kFIFO, (unsigned char *)uart_data, size ) ; 
             
            for( i = 0; i < size; i++ ){                 
                BSP_Ser_WrByte( uart_data[i] ) ;                   
            }
             
        } else {
            OSTimeDly(1);
            
        }
    }    
    
    
}





void BSP_Ser_WrStr_To_Buffer( char *p_str )
{
      
    CPU_INT16U len;
    CPU_INT16U temp;

    len  = strlen( p_str );
    temp = kfifo_get_free_space( &DBG_UART_Send_kFIFO );
    
    if( len > temp ) {
        len = temp ;      
        debug_uart_fifo_oveflow_counter++ ;        
    }
    kfifo_put( &DBG_UART_Send_kFIFO, (unsigned char *)p_str,  len); 
    
}




    