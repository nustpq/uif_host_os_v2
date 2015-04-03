/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2006-2008; Micrium, Inc.; Weston, FL
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
*                                         TASK PACKAGE
*
*                                          Atmel AT91SAM7A3
*                                                on the
*                                  Fortemedia Saturn-II V2.1 Board
*
* Filename      : task_exe.c
* Version       : V1.00
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

#include <includes.h>


CPU_INT08U          databuf[ MAXDATABUF + 10 ] ;    // exe report data buf, raw write/read buf
FUNCSTK             FuncStk[ MAXFUNCNUM + 1 ] ;     // exe function parameter buf
BUFCTR              FuncStkCtr ;                    // exe function parameter buf  Control 
DATABUFCTR          DataBufCtr ;                    // exe report data buf         Control 


/*
*********************************************************************************************************
*                                    App_TaskExe()
*
* Description : based on PC Noah CMD, execute each atom functional operation .
*
* Argument(s) : p_arg   Argument passed to 'App_TaskExe()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
void App_TaskExe( void *p_arg )
{  
    (void)p_arg;
    
    CPU_INT08U      is_session ;
    CPU_INT08U      err ;
    CPU_INT08U      *msg ;
    CPU_INT08U      AckBuf[2];  //used for PcSendData ;
    CPU_INT16U      datlen;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
 
    err             = 0;   
    msg             = NULL;
    
    /*******************************/
    DataBufCtr.pBufStAddr = databuf ;
    DataBufCtr.pBufTop    = databuf ;    
    /******************************/

    while( DEF_TRUE ) {   
    
        msg = (CPU_INT08U *)( OSMboxPend(App_Noah_Mbox, 0, &err) ); //execute only if Got cmd function and session in stop state   
        
        APP_TRACE_DBG((" >EXE"));
        is_session  = (*msg) & 0x01 ;
        *msg        =  0; //
        
        if( is_session == 1 )  { //in session no interruption
            OS_ENTER_CRITICAL(); 
            
        }         
        for( FuncStkCtr.BufTail = 0; FuncStkCtr.BufTail < FuncStkCtr.BufHead; )  {
          
            err = FuncStk[FuncStkCtr.BufTail].Func(  FuncStk[FuncStkCtr.BufTail].prmt  ) ;//exe func            
            if( FuncStkCtr.BufTail++ >= MAXFUNCNUM ) {
                FuncStkCtr.BufTail = 0;
            }        
            if( err != 0) {                       
                AckBuf[0] = CMDDN_ERR_RPT ;
                AckBuf[1] = err ;           
                pcSendDateToBuf( FRAM_TYPE_DATA, AckBuf, 2, 0 ) ; //no need frame ID
                break ;
            }
        }        
        if( is_session == 1 ) {
            OS_EXIT_CRITICAL(); 
            
        }        
          
        //send exe data      
        datlen = DataBufCtr.pBufTop - DataBufCtr.pBufStAddr ;
        if(datlen) {       
            pcSendDateToBuf( FRAM_TYPE_DATA, DataBufCtr.pBufStAddr, datlen, 0 ) ; 
        }
        
        //reset buffer
        FuncStkCtr.BufTail = 0 ;
        FuncStkCtr.BufHead = 0 ; 
        DataBufCtr.pBufTop      = databuf;
        DataBufCtr.pBufStAddr   = databuf;
        
        if( is_session == 1 )  {         
            AckBuf[0] = REPORT_FINISH ;         
            pcSendDateToBuf( FRAM_TYPE_DATA, AckBuf, 1, 0 ) ;  
            
        } else {
            if(datlen==0)  {
                if(err == 0) {   // no data report
                    AckBuf[0] = CMDDN_ERR_RPT ;
                    AckBuf[1] = 0 ;            
                    pcSendDateToBuf( FRAM_TYPE_DATA, AckBuf, 2, 0 ) ;  
                    
                }
            }
            
        }    
       
       ////OSTimeDly(1); 
    }
    
    
}









    