/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
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
*                                             EXAMPLE CODE
*
*                                            Atmel AT91SAM3U4
*                                                on the
*                                  Atmel AT91SAM3U-EK Development Board.
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_SCR_SIGN_ON                        1
#define  APP_SCR_VER_TICK_RATE                  2
#define  APP_SCR_CPU                            3
#define  APP_SCR_CTXSW                          4
 
#define  APP_SCR_FIRST                    APP_SCR_SIGN_ON
#define  APP_SCR_LAST                     APP_SCR_CTXSW


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
static  OS_STK       App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       App_TaskUserIF_Stk[APP_CFG_TASK_USER_IF_STK_SIZE];
static  OS_STK       App_TaskJoyStk[APP_CFG_TASK_JOY_STK_SIZE];
static  OS_STK       App_TaskGenieShellStk[APP_CFG_TASK_SHELL_STK_SIZE];

static  OS_STK       App_TaskUART_RxStk[APP_CFG_TASK_UART_RX_STK_SIZE];
static  OS_STK       App_TaskUART_TxStk[APP_CFG_TASK_UART_TX_STK_SIZE];
static  OS_STK       App_TaskUART_TxRulerStk[APP_CFG_TASK_UART_TX_RULER_STK_SIZE];
static  OS_STK       App_TaskNoahStk[APP_CFG_TASK_NOAH_STK_SIZE];
static  OS_STK       App_TaskNoahRulerStk[APP_CFG_TASK_NOAH_RULER_STK_SIZE];
static  OS_STK       App_TaskCMDParseStk[APP_CFG_TASK_CMD_PARSE_STK_SIZE];
static  OS_STK       App_TaskDebugInfoStk[APP_CFG_TASK_DBG_INFO_STK_SIZE];                                    


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void  App_EventCreate            (void); 
static  void  App_TaskStart              (void        *p_arg);
static  void  App_TaskCreate             (void);
        void  App_BufferCreate           (void);


/*
*********************************************************************************************************
*                                    LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                main()
*
* Description : The standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

int  main (void)
{
#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  err;
#endif    

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR     cpu_err;
#endif
    
    BSP_PreInit(); 
    
    CPU_Init();
    
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)"AT91SAM3UE",
                (CPU_ERR  *)&cpu_err);
#endif
    
    CPU_IntDis();                                               /* Disable all interrupts until we are ready to ...     */
                                                                /* ... accept them                                      */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"          */

    OSTaskCreateExt((void (*)(void *)) App_TaskStart,           /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&App_TaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    return (1);
}


/*
*********************************************************************************************************
*                                          AppTaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*
*               (2) Interrupts are enabled once the task starts because the I-bit of the CCR register was
*                   set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    (void)p_arg;
    CPU_INT32U counter;
    
    BSP_Ser_Init(115200);                        
    //APP_TRACE_INFO(("\n\r\n\r"));
   
    BSP_Init();
   //BSP_PostInit();                                             /* Initialize BSP functions                             */
     
                                       /* Initialize Serial port     */ 
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                               */
#endif
    
    Mem_Init();                                                 /* Initialize the Memory Managment module               */
    Math_Init();                                                /* Initialize the Mathematical module                   */
   
#if (APP_CFG_PROBE_COM_MODULE_EN == DEF_ENABLED) || \
    (APP_CFG_PROBE_OS_PLUGIN_EN  == DEF_ENABLED)
    App_ProbeInit();
#endif
    
     App_BufferCreate();      
  
     App_EventCreate();                                          /* Create Application Events                             */

     App_TaskCreate();                                           /* Create application tasks                              */

   
     while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.        */
                  
//        LED_Set( LED_DS1 );        
//        OSTimeDlyHMSM(0, 0, 0, 30);         
//        LED_Clear( LED_DS1 );   
//        OSTimeDlyHMSM(0, 0, 0, 200);   
//        LED_Set( LED_DS1 );        
//        OSTimeDlyHMSM(0, 0, 0, 30);         
//        LED_Clear( LED_DS1 );
//        OSTimeDlyHMSM(0, 0, 1, 740);         
        counter++;
        if(counter&0xFF) {
            LED_Set( LED_DS1 );   
        }
        if(counter&0x3F) {
            LED_Clear( LED_DS1 );
        }
        //Ruler_Port_LED_Service();

        OSTimeDly(10); 
    
        
    }
}




/*
*********************************************************************************************************
*                                      App_BufferCreate()
*
* Description : Create the application uart buffer
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TasStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  App_BufferCreate (void)
{
  
    CPU_INT08U  err;
    
    //APP_TRACE_INFO(("Creating Application Buffer...\r\n"));
    
#if (OS_MEM_EN > 0)
    
     pMEM_Part_MsgUART = OSMemCreate( MemPartition_MsgUART, MsgUARTQueue_SIZE, MsgUARTBody_SIZE, &err );
     if(OS_ERR_NONE != err) {
        while(1);  
     }   
     
#if (OS_MEM_NAME_EN > 0)
   OSMemNameSet(pMEM_Part_MsgUART, "MEM_Part_MsgUART", &err); 
#endif
   
#endif           
       
}



/*
*********************************************************************************************************
*                                      App_TaskEvent()
*
* Description : Create the application Events
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TasStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_EventCreate (void)
{
    
    //APP_TRACE_INFO(("Creating Application Events...\r\n"));
      
#if (OS_EVENT_NAME_EN  > 0 )
    CPU_INT08U  err;
#endif

    App_UserIF_Mbox     = OSMboxCreate((void *)0);   /* Create MBOX for comm between App_TaskUserIF() and App_TaskJoy()    */
   // App_Noah_Ruler_Mbox = OSMboxCreate((void *)0);   /* Create MBOX for comm App_TaskUserIF()to App_TaskNoah_Ruler()       */    
    ACK_Sem_PCUART      = OSSemCreate(0);            /* Create Sem for the ACK from PC, after UART data sent               */    
    ACK_Sem_RulerUART   = OSSemCreate(0);            /* Create Sem for the ACK from Ruler, after UART data sent            */    
    Done_Sem_RulerUART  = OSSemCreate(0);            /* Create Sem for the Ruler operation caller, after operation done    */    
    EVENT_MsgQ_PCUART2Noah     = OSQCreate(&MsgQ_PCUART2Noah[0],MsgUARTQueue_SIZE);             /* Message queue from PC   */  
    EVENT_MsgQ_Noah2PCUART     = OSQCreate(&MsgQ_Noah2PCUART[0],MsgUARTQueue_SIZE);             /* Message queue to PC     */  
    EVENT_MsgQ_RulerUART2Noah  = OSQCreate(&MsgQ_RulerUART2Noah[0],MsgUARTQueue_SIZE);          /* Message queue from Ruler*/  
    EVENT_MsgQ_Noah2RulerUART  = OSQCreate(&MsgQ_Noah2RulerUART[0],MsgUARTQueue_SIZE);          /* Message queue to Ruler  */  
    EVENT_MsgQ_Noah2CMDParse   = OSQCreate(&MsgQ_Noah2CMDParse[0],MsgUARTQueue_SIZE);   /* Message queue to Task CMD Prase */ 
    Bsp_Ser_Tx_Sem_lock = OSSemCreate(1); 
    Bsp_Ser_Rx_Sem_lock = OSSemCreate(1); 
    DBGU_Tx_Sem_lock    = OSSemCreate(1); 
    DBGU_Tx_Sem_lock    = OSSemCreate(1);     
    GPIO_Sem_I2C_Mixer  = OSSemCreate(1); ; //sem for I2C mixer
    UART_MUX_Sem_lock   = OSSemCreate(1); 
    //ruler UART MUX //if error then halt MCU
    if( NULL == UART_MUX_Sem_lock )  while(1); //last Event creat failure means OS_MAX_EVENTS is not enough

#if (OS_EVENT_NAME_EN > 0)    
    
   OSEventNameSet(App_UserIF_Mbox,      "Joy->UserI/F Mbox",   &err);
  // OSEventNameSet(App_Noah_Ruler_Mbox,  "UserI/F->NoahRulerMbox",     &err);
   OSEventNameSet(ACK_Sem_PCUART,       "PCUART_Tx_ACK_Sem",    &err);  
   OSEventNameSet(ACK_Sem_RulerUART,    "RulerUART_Tx_ACK_Sem", &err); 
   OSEventNameSet(Done_Sem_RulerUART,   "Done_Sem_RulerUART",   &err);
   OSEventNameSet(EVENT_MsgQ_PCUART2Noah,      "EVENT_MsgQ_PCUART2Noah",      &err);
   OSEventNameSet(EVENT_MsgQ_Noah2PCUART,      "EVENT_MsgQ_Noah2PCUART",      &err); 
   OSEventNameSet(EVENT_MsgQ_RulerUART2Noah,   "EVENT_MsgQ_RulerUART2Noah",   &err);
   OSEventNameSet(EVENT_MsgQ_Noah2RulerUART,   "EVENT_MsgQ_Noah2RulerUART",   &err); 
   OSEventNameSet(EVENT_MsgQ_Noah2CMDParse,    "EVENT_MsgQ_Noah2CMDParse",    &err);
   OSEventNameSet(Bsp_Ser_Tx_Sem_lock,  "Bsp_Ser_Tx_Sem_lock",  &err);
   OSEventNameSet(Bsp_Ser_Rx_Sem_lock,  "Bsp_Ser_Tx_Sem_lock",  &err);
   OSEventNameSet(DBGU_Tx_Sem_lock,     "DBGU_Tx_Sem_lock",     &err);
   OSEventNameSet(DBGU_Rx_Sem_lock,     "DBGU_Rx_Sem_lock",     &err);
   OSEventNameSet(UART_MUX_Sem_lock,    "UART_MUX_Sem_lock",    &err);
   
#endif
   
}



/*
*********************************************************************************************************
*                                      App_TaskCreate()
*
* Description : Create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TasStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
         
#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  err;
#endif
    
#ifndef DBG_UART_METHOD_TASK_EN        
    APP_TRACE_INFO(("Creating Application Tasks...\r\n"));    
#else
    OSTaskCreateExt((void (*)(void *)) App_TaskDebugInfo,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskDebugInfoStk[APP_CFG_TASK_DBG_INFO_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_DBG_INFO_PRIO,
                    (INT16U          ) APP_CFG_TASK_DBG_INFO_PRIO,
                    (OS_STK         *)&App_TaskDebugInfoStk[0],
                    (INT32U          ) APP_CFG_TASK_DBG_INFO_STK_SIZE,
                    (void *)0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_DBG_INFO_PRIO, "Debug_Info", &err);
#endif  
#endif
////////////////////////////////////////////////////////////////////////////// 
    
    
    OSTaskCreateExt((void (*)(void *)) App_TaskJoy,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskJoyStk[APP_CFG_TASK_JOY_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_JOY_PRIO,
                    (INT16U          ) APP_CFG_TASK_JOY_PRIO,
                    (OS_STK         *)&App_TaskJoyStk[0],
                    (INT32U          ) APP_CFG_TASK_JOY_STK_SIZE,
                    (void *)0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_JOY_PRIO, "Keyboard", &err);
#endif
    
   
    OSTaskCreateExt((void (*)(void *)) App_TaskGenieShell,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskGenieShellStk[APP_CFG_TASK_SHELL_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_SHELL_PRIO,
                    (INT16U          ) APP_CFG_TASK_SHELL_PRIO,
                    (OS_STK         *)&App_TaskGenieShellStk[0],
                    (INT32U          ) APP_CFG_TASK_SHELL_STK_SIZE,
                    (void *)0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_SHELL_PRIO, "Genie_shell", &err);
#endif

    
    
////////////////////////////////////////////////////////////////////////////// 
    
    
    OSTaskCreateExt((void (*)(void *)) App_TaskUART_Tx,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskUART_TxStk[APP_CFG_TASK_UART_TX_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_UART_TX_PRIO,
                    (INT16U          ) APP_CFG_TASK_UART_TX_PRIO,
                    (OS_STK         *)&App_TaskUART_TxStk[0],
                    (INT32U          ) APP_CFG_TASK_UART_TX_STK_SIZE,
                    (void *)0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_UART_TX_PRIO, "Uart_tx", &err);
#endif
    
     OSTaskCreateExt((void (*)(void *)) App_TaskUART_Rx,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskUART_RxStk[APP_CFG_TASK_UART_RX_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_UART_RX_PRIO,
                    (INT16U          ) APP_CFG_TASK_UART_RX_PRIO,
                    (OS_STK         *)&App_TaskUART_RxStk[0],
                    (INT32U          ) APP_CFG_TASK_UART_RX_STK_SIZE,
                    (void *)0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_UART_RX_PRIO, "Uart_rx", &err);
#endif
    
    
    
    
//    OSTaskCreateExt((void (*)(void *)) App_TaskNoah,
//                    (void           *) 0,
//                    (OS_STK         *)&App_TaskNoahStk[APP_CFG_TASK_NOAH_STK_SIZE - 1],
//                    (INT8U           ) APP_CFG_TASK_NOAH_PRIO,
//                    (INT16U          ) APP_CFG_TASK_NOAH_PRIO,
//                    (OS_STK         *)&App_TaskNoahStk[0],
//                    (INT32U          ) APP_CFG_TASK_NOAH_STK_SIZE,
//                    (void *)0,
//                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
//
//#if (OS_TASK_NAME_EN > 0)
//    OSTaskNameSet(APP_CFG_TASK_NOAH_PRIO, "Noah", &err);
//#endif
    
      
    OSTaskCreateExt((void (*)(void *)) App_TaskUserIF,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskUserIF_Stk[APP_CFG_TASK_USER_IF_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_USER_IF_PRIO,
                    (INT16U          ) APP_CFG_TASK_USER_IF_PRIO,
                    (OS_STK         *)&App_TaskUserIF_Stk[0],
                    (INT32U          ) APP_CFG_TASK_USER_IF_STK_SIZE,
                    (void *)0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_USER_IF_PRIO, "User I/F", &err);
#endif
 
 
 ///////////////////////////////////////////////////////////////////////////////
//    
//     OSTaskCreateExt((void (*)(void *)) App_TaskUART_Tx_Ruler,
//                    (void           *) 0,
//                    (OS_STK         *)&App_TaskUART_TxRulerStk[APP_CFG_TASK_UART_TX_RULER_STK_SIZE - 1],
//                    (INT8U           ) APP_CFG_TASK_UART_TX_RULER_PRIO,
//                    (INT16U          ) APP_CFG_TASK_UART_TX_RULER_PRIO,
//                    (OS_STK         *)&App_TaskUART_TxRulerStk[0],
//                    (INT32U          ) APP_CFG_TASK_UART_TX_RULER_STK_SIZE,
//                    (void *)0,
//                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
//
//#if (OS_TASK_NAME_EN > 0)
//    OSTaskNameSet(APP_CFG_TASK_UART_TX_RULER_PRIO, "Uart_tx_ruler", &err);
//#endif
//    
//    
//    OSTaskCreateExt((void (*)(void *)) App_TaskNoah_Ruler,
//                    (void           *) 0,
//                    (OS_STK         *)&App_TaskNoahRulerStk[APP_CFG_TASK_NOAH_RULER_STK_SIZE - 1],
//                    (INT8U           ) APP_CFG_TASK_NOAH_RULER_PRIO,
//                    (INT16U          ) APP_CFG_TASK_NOAH_RULER_PRIO,
//                    (OS_STK         *)&App_TaskNoahRulerStk[0],
//                    (INT32U          ) APP_CFG_TASK_NOAH_RULER_STK_SIZE,
//                    (void *)0,
//                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
//
//#if (OS_TASK_NAME_EN > 0)
//    OSTaskNameSet(APP_CFG_TASK_NOAH_RULER_PRIO, "Noah_Ruler", &err);
//#endif
    
////////////////////////////////////////////////////////////////////////////////
    
    OSTaskCreateExt((void (*)(void *)) App_TaskCMDParse,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskCMDParseStk[APP_CFG_TASK_CMD_PARSE_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_CMD_PARSE_PRIO,
                    (INT16U          ) APP_CFG_TASK_CMD_PARSE_PRIO,
                    (OS_STK         *)&App_TaskCMDParseStk[0],
                    (INT32U          ) APP_CFG_TASK_CMD_PARSE_STK_SIZE,
                    (void *)0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_CMD_PARSE_PRIO, "CMD_Parse", &err);
#endif 
    
////////////////////////////////////////////////////////////////////////////////
    

    
}


/*
*********************************************************************************************************
*                                    Task_ReCreate_Shell()
*
* Description : Create task shell again after changed the shell UART port in 'AppTaskUserIF()'.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskKbd()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
void Task_ReCreate_Shell( void )
{

#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  err;
#endif   
    
    OSTaskCreateExt((void (*)(void *)) App_TaskGenieShell,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskGenieShellStk[APP_CFG_TASK_SHELL_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_SHELL_PRIO,
                    (INT16U          ) APP_CFG_TASK_SHELL_PRIO,
                    (OS_STK         *)&App_TaskGenieShellStk[0],
                    (INT32U          ) APP_CFG_TASK_SHELL_STK_SIZE,
                    (void *)0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_SHELL_PRIO, "Genie_shell", &err);
#endif

    
}
