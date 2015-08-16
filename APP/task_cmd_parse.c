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
* Filename      : task_cmd_parse.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

#include <includes.h>


//Queue
void       * MsgQ_Noah2CMDParse[MsgUARTQueue_SIZE];
//Event
OS_EVENT   * EVENT_MsgQ_Noah2CMDParse;

EMB_BUF   Emb_Buf_Cmd;
EMB_BUF   Emb_Buf_Data;


/*
*********************************************************************************************************
*                                    App_TaskCMDParse()
*
* Description : This task wait message event from App_TaskNoah().
*               Check if the EMB data is valid in the message. And decode EMB data and parse command and data.
*               Execute the command and return result.          
*
* Argument(s) : p_arg   Argument passed to 'App_TaskCMDParse()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
void App_TaskCMDParse( void *p_arg )
{ 
    
    (void)p_arg;
    
    CPU_INT08U   err ;
    CPU_INT08U  *pTaskMsgIN ;
    pNEW_CMD     pNewCmd ; 
    EMB_BUF      *pEBuf;
    
    pTaskMsgIN  = NULL;
    
//    pEBuf         = &Emb_Buf_Data;    
//    Init_EMB_BUF( pEBuf ); //need reset this when Noah connection reset
//    pEBuf         = &Emb_Buf_Cmd;    
//    Init_EMB_BUF( pEBuf ); //need reset this when Noah connection reset
    
    while( DEF_TRUE ) {     
        
        pTaskMsgIN  = (INT8U *)OSQPend( EVENT_MsgQ_Noah2CMDParse, 0, &err ); 
        
        if( pTaskMsgIN != NULL && OS_ERR_NONE == err )   { 
            
            LED_Set( LED_DS2 );
            
            pNewCmd  = (pNEW_CMD)pTaskMsgIN ; //change to NOAH CMD type           
            err = EMB_Data_Parse( pNewCmd );           
            OSMemPut( pMEM_Part_MsgUART, pTaskMsgIN );  //release mem
            Send_Report( pNewCmd->pkt_sn, err );
            
            LED_Clear( LED_DS2 );
            
        }
        
        
    }
    
    
}
                    
        
    