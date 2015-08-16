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
* Filename      : task_uart_tx.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

#include <includes.h>

//Mem
CPU_INT08U    MemPartition_MsgUART[MsgUARTQueue_SIZE][MsgUARTBody_SIZE];
OS_MEM      * pMEM_Part_MsgUART; //global
//Queue
void        * MsgQ_PCUART2Noah[MsgUARTQueue_SIZE];
void        * MsgQ_Noah2PCUART[MsgUARTQueue_SIZE];
//Event
OS_EVENT    * EVENT_MsgQ_PCUART2Noah;
OS_EVENT    * EVENT_MsgQ_Noah2PCUART;


CPU_INT32U  Tx_ReSend_Happens = 0;   // debug use, resend happen times, NOTE: only writable in this task
CPU_INT08U PcCmdTxID          = 0;   // Frame TXD ID
   

/*
*********************************************************************************************************
*                                    App_TaskUART_Tx()
*
* Description : Process UART Transmit related process between Audio Bridge and PC.
*               Wait for data message from other task that want to send to PC.
*
* Argument(s) : p_arg   Argument passed to 'App_TaskUART_Tx()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
void App_TaskUART_Tx( void *p_arg )
{    
    (void)p_arg;
    
    CPU_INT08U       errCode ;  
    CPU_INT08U      *pTaskMsgIN ;
    pNEW_CMD         pPcCmd ;    
    CPU_INT32U       counter ;
    CPU_INT32U       size ;
    CPU_INT32U       times ;
    CPU_INT08U       *pChar ;

    pTaskMsgIN  = NULL;
    pPcCmd      = NULL;  
    errCode     = UNKOW_ERR_RPT ;       

     
//    unsigned char tt[512];
//        for (unsigned int i = 0; i<512; i++) {
//            tt[i] = '0'+i;
//        }
////    while(1){
//
////       kfifo_put(pUART_Send_kfifo[PC_UART], (unsigned char *)&tt , 512) ;
////       UART_WriteStart( PC_UART );
////       OSTimeDly(1);
////                           
////    }
//    
//    while (DEF_TRUE) { 
//
//        // Noah to Uart transmit
//        pTaskMsgIN   = (INT8U *)OSQPend( EVENT_MsgQ_Noah2PCUART, 0, &errCode );
//        
//        if( pTaskMsgIN != NULL && OS_ERR_NONE == errCode )   {  
//          kfifo_put(pUART_Send_kfifo[PC_UART], (unsigned char *)&tt , 512) ;
//          UART_WriteStart( PC_UART );   
//        }
//    }  
//////////////////////////////////////////////////////////////////////////////      
    
    //pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, SET_FRAME_HEAD(PcCmdTxID,FRAM_TYPE_EST),  NULL, 0, 0, NULL, 0 ) ;  // send a EST package on startup   
    
    while (DEF_TRUE) { 

        // Noah to Uart transmit
        pTaskMsgIN   = (INT8U *)OSQPend( EVENT_MsgQ_Noah2PCUART, 0, &errCode );
        
        if( pTaskMsgIN != NULL && OS_ERR_NONE == errCode )   {  
//            Time_Stamp();
//            APP_TRACE_INFO(("\r\n:App_TaskUART_Tx : [%d] start ",pPcCmd->DataLen + 5));
                    
            pPcCmd   =  (pNEW_CMD)pTaskMsgIN ; 
            pChar    =  (unsigned char *)pTaskMsgIN ; 
            counter  =  (pPcCmd->data_len[0]<<16) + (pPcCmd->data_len[1]<<8) + pPcCmd->data_len[2];
            counter +=  8;
            
#if( false )            
            APP_TRACE_INFO(("\r\n############DATA==counter: %d ####################\r\n",counter)); 
            for(unsigned int i = 0; i< counter; i++ ){                            
                APP_TRACE_INFO(("%0X ", *(pChar+i) ));
                if(i%32 == 31) {
                    APP_TRACE_INFO(("\r\n"));
                }                
             }
            APP_TRACE_INFO(("\r\n##########################################\r\n"));
#endif      
            times   = counter / UART1_SEND_QUEUE_LENGTH ;
            counter = counter % UART1_SEND_QUEUE_LENGTH ;            
            if( times ) {
                for(unsigned int i = 0; i<times; i++ ) {
                    while(1) {                  
                        size = kfifo_get_free_space( pUART_Send_kfifo[PC_UART] );                    
                        if( size >= UART1_SEND_QUEUE_LENGTH ) {
                            kfifo_put(pUART_Send_kfifo[PC_UART], pChar, UART1_SEND_QUEUE_LENGTH) ;
                            pChar += UART1_SEND_QUEUE_LENGTH;
                            break;                    
                        }
                        //OSTimeDly(1);
                    }
                    UART_WriteStart( PC_UART ); //send data
                   
                }                
            }
            
            if( counter ) {                
                while(1) {                     
                    size = kfifo_get_free_space( pUART_Send_kfifo[PC_UART] );                    
                    if( size >= counter ) {
                        kfifo_put(pUART_Send_kfifo[PC_UART], pChar, counter) ;
                        break;                    
                    }
                    //OSTimeDly(1);
                }
                UART_WriteStart( PC_UART ); //send data 
             
            }            
            OSMemPut( pMEM_Part_MsgUART, pTaskMsgIN );
        }  
        
        ////OSTimeDly(5);		                                     	
	}
       
    
    
    
}




    