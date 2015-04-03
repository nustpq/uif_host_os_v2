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
* Filename      : task_noah.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/


#include <includes.h>



OS_EVENT            *ACK_Sem_PCUART;
volatile CPU_INT08U  Global_Conn_Ready = 0 ; //PC connection on/off state

extern EMB_BUF   Emb_Buf_Cmd;
extern EMB_BUF   Emb_Buf_Data;

/*
*********************************************************************************************************
*                                    App_TaskNoah()
*
* Description : Process Comminucation between PC and Audio Bridge.
*               Noah protocol layer related data parsing and processing.
*               This task wait message event from App_TaskUART_Rx(). Check if the Noah layer data is 
*               valid in the message. And send decoded data to App_TaskCMDParse().
*
* Argument(s) : p_arg   Argument passed to 'App_TaskNoah()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
void App_TaskNoah( void *p_arg )
{ 
    
    (void)p_arg;
    
    NOAH_CMD   *pNoahCmd ;    
    CPU_INT08U *pCmdBuf ;
    CPU_INT08U  rxID ; 
    CPU_INT08U  PcCmdRxID ;
    CPU_INT08U  sum ;
    CPU_INT08U  AckBuf[2];   
    CPU_INT08U  err ;
    CPU_INT08U *pTaskMsgIN ;  
    CPU_INT08U *pMsg ;     
    
    pTaskMsgIN  = NULL;
    pMsg        = NULL;      
    
    while( DEF_TRUE ) {     
        
        pTaskMsgIN  = (INT8U *)OSQPend( EVENT_MsgQ_PCUART2Noah, 0, &err );   
        
        if( pTaskMsgIN != NULL && OS_ERR_NONE == err )   {
//            Time_Stamp();
//            APP_TRACE_INFO(("\r\n:App_TaskNoah :  "));
            
            pCmdBuf  = pTaskMsgIN; // char point to the data buffer
            pNoahCmd = (NOAH_CMD *)pCmdBuf ; //change to NOAH_CMD type
            rxID     = GET_FRAME_ID( pNoahCmd->head ) ; //get frame ID, index       
            sum      = pNoahCmd->checkSum; //get check sum data  
            
            LED_Toggle(LED_DS2);           
            APP_TRACE_DBG(("\r\n<"));
            switch( GET_FRAME_TYPE( pNoahCmd->head ) ) {// GET_FRAME_TYPE(pNoahCmd->head)  get frame type 
              
                case FRAM_TYPE_DATA :  
                  
                    if( (sum == 0) || ( sum == CheckSum(0,pCmdBuf, pNoahCmd->DataLen + 2)) ) {   
                        
                        pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, SET_FRAME_HEAD(rxID,FRAM_TYPE_ACK), NULL, 0, 0, NULL, 0 ) ;  // ACK  
                        //OSTimeDly(1); //wait for ack sent
                        APP_TRACE_DBG(("< [0x%2x>0x%2x][",PcCmdRxID,rxID));
                        for(unsigned int i = 0; i<pNoahCmd->DataLen; i++){   
                            APP_TRACE_DBG((" %2X", *(unsigned char*)(pNoahCmd->Data+i) )); 
                        }
                        APP_TRACE_DBG((" ]")); 
                        
                        if( (CPU_INT08U)(PcCmdRxID + 0x40) == rxID ) { // check if frameID repeat  
                            PcCmdRxID = rxID ; //save this frameID
                            
                            APP_TRACE_DBG(("<"));
                            err = OSQPost( EVENT_MsgQ_Noah2CMDParse, pTaskMsgIN );   //send data to MsgQ_Noah2CMDParse message queue   
     
                            if( OS_ERR_NONE != err )  { 
                                AckBuf[0] = CMD_ERR_RPT ;
                                AckBuf[1] = CMD_ERR ;                
                                pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, AckBuf, 2, 0, NULL, 0 ) ; //send data: command error status                                 
                                OSMemPut( pMEM_Part_MsgUART, pTaskMsgIN );            
                            }                                               
                        } else {
                            APP_TRACE_DBG(("PcCmdRxID Err: expect 0x%X, get 0x%X",PcCmdRxID+0x40,rxID)); 
                        }
                        
                    } else {                
                        pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, SET_FRAME_HEAD(rxID,FRAM_TYPE_NAK), NULL, 0, 0, NULL, 0 ) ;  // NAK   
                    
                    }  
                break ;  
                
                case FRAM_TYPE_EST : //establish frame, reset all related buffer buffer          
                    
                    pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, SET_FRAME_HEAD(rxID,FRAM_TYPE_ESTA), NULL, 0, 1, NULL, 0 ) ;  // ESTA                      
                    OSSemPost(ACK_Sem_PCUART);  //end the resend pending--                     
                    OSTimeDly(10); //wait for the TX buffer is empty 
                    //while( Queue_NData(pUART_Send_Buf[PC_UART]) >0 ) {
                    while( kfifo_get_data_size(pUART_Send_kfifo[PC_UART]) ) {
                        OSTimeDly(1);                          
                    }                     
                    OSSemSet(ACK_Sem_PCUART, 0, &err);// clear the sem                 
                 case FRAM_TYPE_ESTA :
                  
                    PcCmdRxID = 0xC0 ; // ? why 0x40  make sure there can be many same setup frame
                    PcCmdTxID = rxID ; //                    
                    
                    Init_EMB_BUF( &Emb_Buf_Data ); //need reset this when Noah connection reset
                    Init_EMB_BUF( &Emb_Buf_Cmd ); //need reset this when Noah connection reset

                    //Reset all UART CMD related buffer and release mem 
                    do{  //reset mem used by  EVENT_MsgQ_Noah2PCUART                    
                        pMsg   = (INT8U *)OSQAccept( EVENT_MsgQ_Noah2PCUART, &err );
                        OSMemPut( pMEM_Part_MsgUART, pMsg ); 
                    } while ( pMsg != NULL && OS_ERR_NONE == err ) ; 
                    
                    do{  //reset mem used by  EVENT_MsgQ_PCUART2Noah                  
                        pMsg   = (INT8U *)OSQAccept( EVENT_MsgQ_PCUART2Noah, &err );
                        OSMemPut( pMEM_Part_MsgUART, pMsg ); 
                    } while ( pMsg != NULL && OS_ERR_NONE == err ) ; 
                       
                    Queue_Flush( pUART_Send_Buf[PC_UART] ); //clear uart send&rec data queue
                    Queue_Flush( pUART_Rece_Buf[PC_UART] );  
                    
                    //FuncStkCtr.BufTail = 0 ;   //clear exe function buffer
                    //FuncStkCtr.BufHead = 0 ;                 
                    Global_Conn_Ready = 1 ;
                    OSMemPut( pMEM_Part_MsgUART, pTaskMsgIN );
                    APP_TRACE_DBG(("EST/ESTA"));    
                break ;         
                        
                case FRAM_TYPE_ACK :
                  
                    if( rxID == PcCmdTxID ) {                       
                        OSSemPost(ACK_Sem_PCUART);                        
                    }  
                    OSMemPut( pMEM_Part_MsgUART, pTaskMsgIN );
                    APP_TRACE_DBG(("ACK"));
                break ;
                
                case FRAM_TYPE_NAK :
                        // dismiss NAK, there will be a resend if no ACK got
                    OSMemPut( pMEM_Part_MsgUART, pTaskMsgIN ); 
                    APP_TRACE_DBG(("NAK")); 
                break;               
                
                default :
                    OSMemPut( pMEM_Part_MsgUART, pTaskMsgIN );     
                break ;              
            } 
            
            //release mem 

//            Time_Stamp();
//            APP_TRACE_INFO(("\r\n:App_TaskNoah : end"));            
           
        }         
         
        ////OSTimeDly(2);   //use OSQPend(), no delay needed. 
        
    }        
    
    
}



    