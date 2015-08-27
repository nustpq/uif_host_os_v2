
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
*                                           DRIVER PACKAGE
*
*                                          Atmel AT91SAM7A3
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : uart.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

#include <bsp.h>
#include <string.h>
#include <pio.h>
#include <taskcomm.h>
#include "uart.h"
#include "kfifo.h"


OS_EVENT *DBGU_Tx_Sem_lock;  
OS_EVENT *DBGU_Rx_Sem_lock;



/*
*********************************************************************************************************
                                  UART Related Buffer Defines 
*********************************************************************************************************
*/

#if ( UART0_EN > 0 )
CPU_INT32U UART0_Send_Buf[UART0_SEND_QUEUE_LENGTH/sizeof(CPU_INT32U)];
CPU_INT32U UART0_Rece_Buf[UART0_RECE_QUEUE_LENGTH/sizeof(CPU_INT32U)];
CPU_INT08U UART0_Send_Buf_PDC[UART_PDC_LENGTH];
CPU_INT08U UART0_Rece_Buf_PDC[UART_PDC_LENGTH];
#endif

#if ( UART1_EN > 0 )
CPU_INT32U UART1_Send_Buf[UART1_SEND_QUEUE_LENGTH/sizeof(CPU_INT32U)];
CPU_INT32U UART1_Rece_Buf[UART1_RECE_QUEUE_LENGTH/sizeof(CPU_INT32U)];
CPU_INT08U UART1_Send_Buf_PDC[UART_PDC_LENGTH];
CPU_INT08U UART1_Rece_Buf_PDC[UART_PDC_LENGTH];
#endif

#if ( UART2_EN > 0 )
CPU_INT32U UART2_Send_Buf[UART2_SEND_QUEUE_LENGTH/sizeof(CPU_INT32U)];
CPU_INT32U UART2_Rece_Buf[UART2_RECE_QUEUE_LENGTH/sizeof(CPU_INT32U)];
CPU_INT08U UART2_Send_Buf_PDC[UART_PDC_LENGTH];
CPU_INT08U UART2_Rece_Buf_PDC[UART_PDC_LENGTH];
#endif

volatile bool uartin_start_cmd      = false;
volatile bool uartout_start_cmd     = false;

static  OS_EVENT *USART_Sem[3];

kfifo_t uart_tx_fifo[3];
kfifo_t uart_rx_fifo[3];

kfifo_t * const pUART_Send_kfifo[]=
{  
      &uart_tx_fifo[0],
      &uart_tx_fifo[1],
      &uart_tx_fifo[2]
          
};

kfifo_t * const pUART_Rece_kfifo[]=
{  
      &uart_rx_fifo[0],
      &uart_rx_fifo[1],
      &uart_rx_fifo[2]
          
};

static const AT91PS_USART  pUARTREG[] =
{
     AT91C_BASE_US0,    // USART0 REGS base addr
     AT91C_BASE_US1,    // USART1 REGS base addr
     AT91C_BASE_US2     // USART2 REGS base addr

};
static const Pin UART_PINS[] =
{
      PINS_USART0,
      PINS_USART1,
      PINS_USART2  
};

static const Pin UART1_PINS[] =
{
      PINS_USART1_HWSHS   
};

static const CPU_INT08U UART_PID[]=
{  
      AT91C_ID_US0,
      AT91C_ID_US1,
      AT91C_ID_US2
};



CPU_INT32U * const pUART_Send_Buf[]=
{  
      (CPU_INT32U *)UART0_Send_Buf,
      (CPU_INT32U *)UART1_Send_Buf,
      (CPU_INT32U *)UART2_Send_Buf
};

CPU_INT32U * const pUART_Rece_Buf[]=
{  
      (CPU_INT32U *)UART0_Rece_Buf,
      (CPU_INT32U *)UART1_Rece_Buf,
      (CPU_INT32U *)UART2_Rece_Buf
};

CPU_INT08U * const pUART_Send_Buf_PDC[]=
{  
      (CPU_INT08U *)UART0_Send_Buf_PDC,
      (CPU_INT08U *)UART1_Send_Buf_PDC,
      (CPU_INT08U *)UART2_Send_Buf_PDC
};

CPU_INT08U * const pUART_Rece_Buf_PDC[]=
{  
      (CPU_INT08U *)UART0_Rece_Buf_PDC,
      (CPU_INT08U *)UART1_Rece_Buf_PDC,
      (CPU_INT08U *)UART2_Rece_Buf_PDC
};


static const CPU_INT32U UART_Send_Buf_Size[]=
{  
      sizeof(UART0_Send_Buf),
      sizeof(UART1_Send_Buf),
      sizeof(UART2_Send_Buf)
};

static const CPU_INT32U UART_Rece_Buf_Size[]=
{  
      sizeof(UART0_Rece_Buf),
      sizeof(UART1_Rece_Buf),
      sizeof(UART2_Rece_Buf)
};



/*
*********************************************************************************************************
*                                    UART_Init()
*
* Description : Initialize UART related setting and int isr    .
*
* Argument(s) : uart_index : 0 ~ 3  
*               isr_handler : uart int isr
*               baud: uart baud rate speed
*             
* Return(s)   : error: =0 is no orror
*
* Note(s)     : 
*
*********************************************************************************************************
*/
CPU_INT08U UART_Init( CPU_INT08U uart_index,void (*isr_handler)( void ),CPU_INT32U baud )
{
  
    CPU_INT08U error ;     
    error    =  0    ;        
     
    if( uart_index < 3 ) { //for USART0  USART1  USART2  
      
        AT91C_BASE_PMC->PMC_PCER      = 1 << UART_PID[uart_index]; //power on first //PQ
         //PQ
        if( uart_index == PC_UART ) { 
            PIO_Configure( UART1_PINS, PIO_LISTSIZE(UART1_PINS) );
            USART_Configure(   pUARTREG[uart_index], USART_MODE_ASYNCHRONOUS_HW, baud,  MCK  ); 
        } else {
            PIO_Configure( &UART_PINS[uart_index], PIO_LISTSIZE(UART_PINS[uart_index]) );
            USART_Configure( pUARTREG[uart_index], USART_MODE_ASYNCHRONOUS, baud,  MCK  );        
        }
        pUARTREG[uart_index]->US_IDR  = 0xFFFFFFFF;           
        pUARTREG[uart_index]->US_CR   = AT91C_US_STTTO;
        pUARTREG[uart_index]->US_RTOR = UART_TIMEOUT_BIT;   
        pUARTREG[uart_index]->US_TCR  = 0;
        pUARTREG[uart_index]->US_RCR  = 0;
        
        if ( isr_handler == NULL ) {
          OSSemDel ( USART_Sem[uart_index], OS_DEL_ALWAYS,  &error ) ;          
          USART_Sem[uart_index] = OSSemCreate(1); 
          
        } else {  //if ISR used          
            pUARTREG[uart_index]->US_RPR  = (unsigned int)pUART_Rece_Buf_PDC[uart_index];
            pUARTREG[uart_index]->US_RCR  = UART_PDC_LENGTH;
            pUARTREG[uart_index]->US_PTCR = AT91C_PDC_RXTEN;        
             
            BSP_IntVectSet((CPU_INT08U   )UART_PID[uart_index],
                          // (CPU_INT08U   )AT91C_AIC_PRIOR_LOWEST+1,
                          // (CPU_INT08U   )BSP_INT_SCR_TYPE_INT_HIGH_LEVEL_SENSITIVE,
                           (CPU_FNCT_VOID)isr_handler); 
            IRQ_ConfigureIT(UART_PID[uart_index], AT91C_AIC_PRIOR_LOWEST-1, NULL);
            IRQ_EnableIT(UART_PID[uart_index]);
                                   
            pUARTREG[uart_index]->US_IER = AT91C_US_ENDRX | AT91C_US_TIMEOUT;       
           
        }	        
        USART_SetTransmitterEnabled( pUARTREG[uart_index], 1 );         
        USART_SetReceiverEnabled(    pUARTREG[uart_index], 1 );	
        
        ////////////////////////////////////////////////////////////////////////    
//        if (Queue_Create((void *)pUART_Send_Buf[uart_index], UART_Send_Buf_Size[uart_index] ) == QUEUE_FAIL) {
//              error++;
//              
//        }
//        if (Queue_Create((void *)pUART_Rece_Buf[uart_index], UART_Rece_Buf_Size[uart_index] ) == QUEUE_FAIL)  {
//              error++;
//        }
        kfifo_init_static(pUART_Send_kfifo[uart_index], (unsigned char *)pUART_Send_Buf[uart_index], UART_Send_Buf_Size[uart_index]);
        kfifo_init_static(pUART_Rece_kfifo[uart_index], (unsigned char *)pUART_Rece_Buf[uart_index], UART_Rece_Buf_Size[uart_index]);
         
    } else {// for DBG_USART
     
        DBGU_Configure( baud, MCK);         
        OSSemSet (DBGU_Rx_Sem_lock, 1,  &error) ;
        OSSemSet (DBGU_Tx_Sem_lock, 1,  &error) ;
    }
      
    return error ;
    
} 



/*
*********************************************************************************************************
*                                    UART_WriteStart()
*
* Description : Copy data from queue pUART_Send_Buf to pUART_Send_Buf_PDC and start UART PDC, if PDC is 
*               not working and there is some data in the queue.
*
* Argument(s) : uart_index : 0 ~ 3  
*             
* Return(s)   : error: =0 is no orror
*                      =1 is no data in queue
*
* Note(s)     : Just check queue and start PDC sending
*
*********************************************************************************************************
*/
CPU_INT08U UART_WriteStart( CPU_INT08U uart_index )
{
    CPU_INT16U counter ;
    CPU_INT08U error;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    error = 0;
    
    OS_ENTER_CRITICAL();    
    
   
    if ((pUARTREG[uart_index]->US_TCR == 0) && (pUARTREG[uart_index]->US_TNCR == 0)) {
      
//        if( Queue_ReadBuf(  pUART_Send_Buf_PDC[uart_index], 
//                            pUART_Send_Buf[uart_index], 
//                            UART_PDC_LENGTH, 
//                            &byte_send) == QUEUE_OK ) {
        counter  = kfifo_get_data_size(pUART_Send_kfifo[uart_index]); 
        if( counter ) { 
            counter = counter < UART_PDC_LENGTH ? counter : UART_PDC_LENGTH ;
            kfifo_get(pUART_Send_kfifo[uart_index], pUART_Send_Buf_PDC[uart_index], counter) ;    
                            
            pUARTREG[uart_index]->US_TPR      =  (unsigned int) pUART_Send_Buf_PDC[uart_index];
            pUARTREG[uart_index]->US_TCR      =  counter;                  
            pUARTREG[uart_index]->US_PTCR     =  AT91C_PDC_TXTEN ; //start PDC
            pUARTREG[uart_index]->US_IER      =  AT91C_US_ENDTX  ; //enable PDC tx INT
            
        } else {
            error = 1;  
            test_counter5++;            
        } 
    }
    
    OS_EXIT_CRITICAL();
    
    return error;
}


CPU_INT08U UART_ReWriteStart( CPU_INT08U uart_index )
{
    CPU_INT16U counter ;
    CPU_INT08U error;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    error = 0;
    
    OS_ENTER_CRITICAL();    
       
    if ((pUARTREG[uart_index]->US_TCR == 0) && (pUARTREG[uart_index]->US_TNCR == 0)) {
      
//        if( Queue_ReadBuf(  pUART_Send_Buf_PDC[uart_index], 
//                            pUART_Send_Buf[uart_index], 
//                            UART_PDC_LENGTH, 
//                            &byte_send) == QUEUE_OK ) {
        counter  = kfifo_get_data_size(pUART_Send_kfifo[uart_index]); 
        if( counter ) { 
            counter = counter < UART_PDC_LENGTH ? counter : UART_PDC_LENGTH ;
            kfifo_get(pUART_Send_kfifo[uart_index], pUART_Send_Buf_PDC[uart_index], counter) ;    
                            
            pUARTREG[uart_index]->US_TPR      =  (unsigned int) pUART_Send_Buf_PDC[uart_index];
            pUARTREG[uart_index]->US_TCR      =  counter;                  
            pUARTREG[uart_index]->US_PTCR     =  AT91C_PDC_TXTEN ; //start PDC
            pUARTREG[uart_index]->US_IER      =  AT91C_US_ENDTX  ; //enable PDC tx INT
            
        } else {
            error = 1;  
            test_counter5++;            
        } 
    }
    
    OS_EXIT_CRITICAL();
    
    return error;
}

/*
*********************************************************************************************************
*                                    UART_Write()
*
* Description : Add data to queue. and copy data from queue pUART_Send_Buf to pUART_Send_Buf_PDC and 
*               start UART PDC, if PDC is not working and there is some data in the queue.
*
* Argument(s) : uart_index : 0 ~ 3  
*               data : byte need to be added to uart send queue  
*  
* Return(s)   : error: =0 is no orror
*                      =1 is no data in queue
*                      =2 is no data in queue
*
* Note(s)     : 
*
*********************************************************************************************************
*/
CPU_INT08U UART_Write(CPU_INT08U uart_index,  CPU_INT08U data )
{
    CPU_INT16U byte_send;
    CPU_INT08U error;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    error = 0;
    
    OS_ENTER_CRITICAL();
    
    if( Queue_Write((void *)pUART_Send_Buf[uart_index], data) != QUEUE_OK ) {  /* 数据入队 */
        error = 1 ; //?
    }      
    
    if ((pUARTREG[uart_index]->US_TCR == 0) && (pUARTREG[uart_index]->US_TNCR == 0)) {      
        if( Queue_ReadBuf(  pUART_Send_Buf_PDC[uart_index], 
                            pUART_Send_Buf[uart_index], 
                            UART_PDC_LENGTH, 
                            &byte_send) == QUEUE_OK ) {       
            pUARTREG[uart_index]->US_TPR      =  (unsigned int) pUART_Send_Buf_PDC[uart_index];
            pUARTREG[uart_index]->US_TCR      =  byte_send;                  
            pUARTREG[uart_index]->US_PTCR     =  AT91C_PDC_TXTEN ; //start PDC
            pUARTREG[uart_index]->US_IER      =  AT91C_US_ENDTX  ; //enable PDC tx INT
            
        } else {
            error = 2;  //??
            
        }         
      
    }
    OS_EXIT_CRITICAL();
    
    return error;
}


/*
*********************************************************************************************************
*                                    UART_Read()
*
* Description : read data from uart receive queue
*
* Argument(s) : uart_index : 0 ~ 3  
*               *pdata : the read date will be in the location this pointer point to  
*  
* Return(s)   : error: =0 is no orror
*                      =1 is no data is read
*
* Note(s)     : 
*
*********************************************************************************************************
*/
CPU_INT08U UART_Read( CPU_INT08U uart_index, QUEUE_DATA_TYPE *pdata )
{
    CPU_INT08U error;
     
    error = 0 ;   
    
    if ( Queue_Read( pdata, (void *)pUART_Rece_Buf[uart_index]) != QUEUE_OK )  {
        error = 1;
        
    }    
    
    return error;
} 


/*
*********************************************************************************************************
*                                    ISR_PC_UART()
*
* Description : PC uart (uart0) interruption service subroutine
*
* Argument(s) : no
*  
* Return(s)   : no
*
* Note(s)     : 
*
*********************************************************************************************************
*/
void ISR_PC_UART( void )
{
    CPU_INT32U status; 
    CPU_INT16U counter;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif   
    
    OS_ENTER_CRITICAL();
    
    //AT91C_BASE_AIC->AIC_IVR = 0;     /* Write the IVR, as required in Protection Mode */
    status = pUARTREG[PC_UART]->US_CSR;  
             
    if ( status & AT91C_US_ENDTX  )   {  //Transmit INT         
//        if( Queue_ReadBuf(  pUART_Send_Buf_PDC[PC_UART],
//                            pUART_Send_Buf[PC_UART], 
//                            UART_PDC_LENGTH, 
//                            &byte_send) == QUEUE_OK ) {  //move data from PDC buf to queue
        counter  = kfifo_get_data_size(pUART_Send_kfifo[PC_UART]); 
        if( counter ) { 
            counter = counter < UART_PDC_LENGTH ? counter : UART_PDC_LENGTH ;
            kfifo_get(pUART_Send_kfifo[PC_UART], pUART_Send_Buf_PDC[PC_UART], counter) ; 
           
            pUARTREG[PC_UART]->US_TPR = (CPU_INT32U) pUART_Send_Buf_PDC[PC_UART];
            pUARTREG[PC_UART]->US_TCR = counter;       
            pUARTREG[PC_UART]->US_PTCR = AT91C_PDC_TXTEN; //start PDC
                    
        } else {
            uartout_start_cmd = true;
            pUARTREG[PC_UART]->US_IDR      =  AT91C_US_ENDTX  ; //disable PDC tx INT
            //pUARTREG[PC_UART]->US_PTCR  =   AT91C_PDC_TXTDIS; //stop PDC
            
        }
        
    }
    
    if (status & AT91C_US_ENDRX  )   {  // receive INT
    
        kfifo_put(pUART_Rece_kfifo[PC_UART], pUART_Rece_Buf_PDC[PC_UART], UART_PDC_LENGTH) ;
        
        if( UART_PDC_LENGTH < kfifo_get_free_space( pUART_Rece_kfifo[PC_UART]) ) {        
            pUARTREG[PC_UART]->US_RPR  = (CPU_INT32U) pUART_Rece_Buf_PDC[PC_UART];
            pUARTREG[PC_UART]->US_RCR  = UART_PDC_LENGTH;
            pUARTREG[PC_UART]->US_PTCR = AT91C_PDC_RXTEN;
            pUARTREG[PC_UART]->US_IER  = AT91C_US_ENDRX | AT91C_US_TIMEOUT; //PQ
            
        } else {
            uartin_start_cmd = true; 
            pUARTREG[PC_UART]->US_IDR  = AT91C_US_ENDRX | AT91C_US_TIMEOUT; //PQ
            
        }
        
    }
    
    if (status & AT91C_US_TIMEOUT)   {  //receive timeout

        kfifo_put(pUART_Rece_kfifo[PC_UART], pUART_Rece_Buf_PDC[PC_UART] , UART_PDC_LENGTH - pUARTREG[PC_UART]->US_RCR) ;
        
        if( UART_PDC_LENGTH < kfifo_get_free_space( pUART_Rece_kfifo[PC_UART] )) {  
            pUARTREG[PC_UART]->US_RPR  = (CPU_INT32U) pUART_Rece_Buf_PDC[PC_UART];
            pUARTREG[PC_UART]->US_RCR  = UART_PDC_LENGTH;
            pUARTREG[PC_UART]->US_PTCR = AT91C_PDC_RXTEN;
            
            pUARTREG[PC_UART]->US_CR   = AT91C_US_STTTO; //restart timeout counter
            pUARTREG[PC_UART]->US_RTOR = UART_TIMEOUT_BIT;
            pUARTREG[PC_UART]->US_IER  = AT91C_US_ENDRX | AT91C_US_TIMEOUT; //PQ
            
        } else {
            uartin_start_cmd = true; 
            pUARTREG[PC_UART]->US_IDR  = AT91C_US_ENDRX | AT91C_US_TIMEOUT; //PQ
        }
      
    }

    OS_EXIT_CRITICAL();
    
    BSP_IntClr( UART_PID[PC_UART] );
    
}


void UART_Rx_ReStart( CPU_INT08U uart_index ) 
{
    CPU_INT16U counter; 
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    if( uartin_start_cmd ) { 
   
        OS_ENTER_CRITICAL();
        
        counter = kfifo_get_free_space( pUART_Rece_kfifo[uart_index] );
        
        if( UART_PDC_LENGTH < counter ) {                      
            uartin_start_cmd = false; 
            pUARTREG[uart_index]->US_RPR  = (CPU_INT32U) pUART_Rece_Buf_PDC[uart_index];
            pUARTREG[uart_index]->US_RCR  = UART_PDC_LENGTH;
            pUARTREG[uart_index]->US_PTCR = AT91C_PDC_RXTEN;   
            
            pUARTREG[PC_UART]->US_IER  = AT91C_US_ENDRX | AT91C_US_TIMEOUT; //PQ
        }   
        
        OS_EXIT_CRITICAL();
    }
}

/*
*********************************************************************************************************
*                                    ISR_Ruler_UART()
*
* Description : Ruler uart (uart1) interruption service subroutine
*
* Argument(s) : no
*  
* Return(s)   : no
*
* Note(s)     : 
*
*********************************************************************************************************
*/
void ISR_Ruler_UART( void )
{
    CPU_INT32U status; 
    CPU_INT16U byte_send;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif   
 
    byte_send = 0;
    
    OS_ENTER_CRITICAL();
    
    //AT91C_BASE_AIC->AIC_IVR = 0;  //?????   /* Write the IVR, as required in Protection Mode */
    status = pUARTREG[RULER_UART]->US_CSR;  
 
    if ( status & AT91C_US_ENDTX  )   {  //Transmit INT         
        if( Queue_ReadBuf(  pUART_Send_Buf_PDC[RULER_UART],
                            pUART_Send_Buf[RULER_UART], 
                            UART_PDC_LENGTH, 
                            &byte_send) == QUEUE_OK ) {  //move data from PDC buf to queue
            pUARTREG[RULER_UART]->US_TPR = (CPU_INT32U) pUART_Send_Buf_PDC[RULER_UART];
            pUARTREG[RULER_UART]->US_TCR = byte_send;       
            pUARTREG[RULER_UART]->US_PTCR = AT91C_PDC_TXTEN; //start PDC
                    
        } else {
            pUARTREG[RULER_UART]->US_IDR      =  AT91C_US_ENDTX  ; //disable PDC tx INT
            //pUARTREG[RULER_UART]->US_PTCR  =   AT91C_PDC_TXTDIS; //stop PDC
        }
        
    }
    
    if (status & AT91C_US_ENDRX  )   {  // receive INT
        Queue_WriteBuf( pUART_Rece_Buf_PDC[RULER_UART],  
                        pUART_Rece_Buf[RULER_UART], 
                        UART_PDC_LENGTH );//move data from queue to PDC buf
        pUARTREG[RULER_UART]->US_RPR  = (CPU_INT32U) pUART_Rece_Buf_PDC[RULER_UART];
        pUARTREG[RULER_UART]->US_RCR  = UART_PDC_LENGTH;
        pUARTREG[RULER_UART]->US_PTCR = AT91C_PDC_RXTEN;
        
    }
    
    if (status & AT91C_US_TIMEOUT)   {  //receive timeout
        Queue_WriteBuf( pUART_Rece_Buf_PDC[RULER_UART], 
                        pUART_Rece_Buf[RULER_UART], 
                        (UART_PDC_LENGTH - pUARTREG[RULER_UART]->US_RCR) );  //move data from queue to PDC buf        
        pUARTREG[RULER_UART]->US_RPR  = (CPU_INT32U) pUART_Rece_Buf_PDC[RULER_UART];
        pUARTREG[RULER_UART]->US_RCR  = UART_PDC_LENGTH;
        pUARTREG[RULER_UART]->US_PTCR = AT91C_PDC_RXTEN;
        
        pUARTREG[RULER_UART]->US_CR   = AT91C_US_STTTO; //restart timeout counter
        pUARTREG[RULER_UART]->US_RTOR = UART_TIMEOUT_BIT;
      
    }

    OS_EXIT_CRITICAL();
    
    BSP_IntClr( UART_PID[RULER_UART] );
    
}


/*
*********************************************************************************************************
*                                    USART_Configure()
*
* Description : USART configuration
*
* Argument(s) : 
            /// \param usart  Pointer to the USART peripheral to configure.
            /// \param mode  Desired value for the USART mode register (see the datasheet).
            /// \param baudrate  Baudrate at which the USART should operate (in Hz).
            /// \param masterClock  Frequency of the system master clock (in Hz).
* Return(s)   : no
*
* Note(s)     : 
*
*********************************************************************************************************
*/
void USART_Configure(   AT91S_USART *usart,
                        CPU_INT32U mode,
                        CPU_INT32U baudrate,
                        CPU_INT32U masterClock )
{
    // Reset and disable receiver & transmitter
    usart->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX
                   | AT91C_US_RXDIS | AT91C_US_TXDIS;
    // Configure mode
    usart->US_MR = mode;

    // Configure baudrate, asynchronous mode, no oversampling   
    
    if (    ((mode & AT91C_US_SYNC) == 0)&&
            ((mode & AT91C_US_OVER) == 0)   ) {    
        usart->US_BRGR = (masterClock / baudrate) / 16;
    }
    // TODO other modes
    
}


/*
*********************************************************************************************************
*                                    USART_SetTransmitterEnabled()
*
* Description : Enables or disables the transmitter of an USART peripheral.
*
* Argument(s) : 
                /// \param usart  Pointer to an USART peripheral
                /// \param enabled  If true, the transmitter is enabled; otherwise it is disabled.
* Return(s)   : no
*
* Note(s)     : 
*
*********************************************************************************************************
*/
void USART_SetTransmitterEnabled( AT91S_USART *usart, CPU_INT08U enabled )
{
    if (enabled) {
        usart->US_CR = AT91C_US_TXEN;
        
    }   else {
        usart->US_CR = AT91C_US_TXDIS;
        
    }
    
}



/*
*********************************************************************************************************
*                                    USART_SetReceiverEnabled()
*
* Description : Enables or disables the receiver of an USART peripheral.
*
* Argument(s) : 
                /// \param usart  Pointer to an USART peripheral
                /// \param enabled  If true, the receiver is enabled; otherwise it is disabled.
* Return(s)   : no
*
* Note(s)     : 
*
*********************************************************************************************************
*/
void USART_SetReceiverEnabled( AT91S_USART *usart,  CPU_INT08U enabled )
{
    if (enabled) {
        usart->US_CR = AT91C_US_RXEN;
        
    } else {
        usart->US_CR = AT91C_US_RXDIS;
        
    }
}




//------------------------------------------------------------------------------
/// Sends the contents of a data buffer through the specified USART peripheral.
/// This function returns immediately (1 if the buffer has been queued, 0
/// otherwise); poll the ENDTX and TXBUFE bits of the USART status register
/// to check for the transfer completion.
/// \param usart  Pointer to an USART peripheral.
/// \param buffer  Pointer to the data buffer to send.
/// \param size  Size of the data buffer (in bytes).
//------------------------------------------------------------------------------
static unsigned char USART_WriteBuffer(     AT91S_USART *usart,
                                            void *buffer,
                                            unsigned int size)
{
    // Check if the first PDC bank is free
    if ((usart->US_TCR == 0) && (usart->US_TNCR == 0)) {
        usart->US_TPR = (unsigned int) buffer;
        usart->US_TCR = size;
        usart->US_PTCR = AT91C_PDC_TXTEN; //start PDC
        return 1;
    }
    // Check if the second PDC bank is free
    else if (usart->US_TNCR == 0) {
        usart->US_TNPR = (unsigned int) buffer;
        usart->US_TNCR = size;
        return 1;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
/// Reads data from an USART peripheral, filling the provided buffer until it
/// becomes full. This function returns immediately with 1 if the buffer has
/// been queued for transmission; otherwise 0.
/// \param usart  Pointer to an USART peripheral.
/// \param buffer  Pointer to the buffer where the received data will be stored.
/// \param size  Size of the data buffer (in bytes).
//------------------------------------------------------------------------------
static unsigned char USART_ReadBuffer(      AT91S_USART *usart,
                                            void *buffer,
                                            unsigned int size)
{
    // Check if the first PDC bank is free
    if ((usart->US_RCR == 0) && (usart->US_RNCR == 0)) {
        usart->US_RPR = (unsigned int) buffer;
        usart->US_RCR = size;
        usart->US_PTCR = AT91C_PDC_RXTEN;
        return 1;
    }
    // Check if the second PDC bank is free
    else if (usart->US_RNCR == 0) {
        usart->US_RNPR = (unsigned int) buffer;
        usart->US_RNCR = size;
        return 1;
    }
    else {
        return 0;
    }
    
}



CPU_INT08U USART_Read_Timeout( CPU_INT08U uart_index, void *buffer, CPU_INT32U size, CPU_INT32U timeout_ms )
{ 
  
    CPU_INT32U counter ;
    CPU_INT08U err ;
    
    counter    = timeout_ms; //wait    for resp   
    err        = 0 ;
    OSSemPend( USART_Sem[uart_index], 0, &err );

    pUARTREG[uart_index]->US_RCR = 0;
    pUARTREG[uart_index]->US_RNCR= 0;
    pUARTREG[uart_index]->US_CR  = AT91C_US_STTTO; //Restart timeout 
    
    while( USART_ReadBuffer(pUARTREG[uart_index],buffer,size) == 0 ){//wait until PDC is free     
        OSTimeDly(1);  /////////?????? 
       
    }        
    
    while ( (pUARTREG[uart_index]->US_CSR & AT91C_US_TIMEOUT) == 0 ) { //wait for data is received       
        OSTimeDly(1);      
        if( --counter == 0) {    ////wait 100ms no data 
            err = UART_RD_TIMEOUT_ERR; //time out
            break;
        }     
               
    }
    
    pUARTREG[uart_index]->US_PTCR = AT91C_PDC_RXTDIS; //disable PDC receive
 
    OSSemPost( USART_Sem[uart_index]);
    
    return err ;

}
  

void USART_SendByte( CPU_INT08U uart_index, CPU_INT08U data )
{

    CPU_INT08U err ;   
    
    OSSemPend( USART_Sem[uart_index], 0, &err );
    
    while ( (pUARTREG[uart_index]->US_CSR & AT91C_US_TXEMPTY) == 0 ) {       
       OSTimeDly(1);
       
    };  
  
    pUARTREG[uart_index]->US_THR = data;
    
    OSSemPost( USART_Sem[uart_index]);
 
}



CPU_INT08U USART_SendBuf( CPU_INT08U uart_index, CPU_INT08U * pBuf,  CPU_INT32U size)
{      
  
    CPU_INT08U err ;
    
    OSSemPend( USART_Sem[uart_index], 0, &err );    
    //memcpy(pUART_Send_Buf[uart_index], pBuf, size); //in case of 
    //err = USART_WriteBuffer( pUARTREG[uart_index], pUART_Send_Buf[uart_index], size ); 
    err = USART_WriteBuffer( pUARTREG[uart_index], pBuf, size ); 
    OSSemPost( USART_Sem[uart_index]);
    err = (err == 0) ? 1 : 0 ;
    
    return err;
    
}


CPU_INT08U USART_SendBuf_Status( CPU_INT08U uart_index)
{      
  
    CPU_INT08U err ;    

    if( (pUARTREG[uart_index]->US_PTCR == 0) && (pUARTREG[uart_index]->US_TNCR == 0 )) {
        err = 0 ;
        
    } else {        
        err = 1;
        
    }
    
    return err;
    
}


void USART_SendStr( CPU_INT08U uart_index, CPU_INT08U * pStr )
{       
    CPU_INT08U err ;
    
    OSSemPend( USART_Sem[uart_index], 0, &err );
    
    memcpy(pUART_Send_Buf[uart_index], pStr, strlen((char const *)pStr)); //in case of 
     
    USART_WriteBuffer( pUARTREG[uart_index], pUART_Send_Buf[uart_index], strlen((char const *)pStr) ); 
    
    OSSemPost( USART_Sem[uart_index]);
    
}



////////////////////////////////////////////////////////////////////////////////


void DBGU_Configure( CPU_INT32U baud_rate, CPU_INT32U mclk ) 
{    
      
    AT91C_BASE_PIOA->PIO_PDR   |= (DEF_BIT_11|DEF_BIT_12);                 /* Set GPIOA pins 9 and 10 as DBGU UART pins                */
    AT91C_BASE_PIOA->PIO_ABSR  |= (DEF_BIT_11|DEF_BIT_12);                 /* Select GPIOA attached peripheral (DBGU)                  */

     
    AT91C_BASE_DBGU->DBGU_CR   =  AT91C_US_RSTRX 
                               |  AT91C_US_RSTTX ;             /*  Reset the receiver  and transmitter                      */
    
    AT91C_BASE_DBGU->DBGU_IDR  = AT91C_US_RXRDY                 /* Disable Rx interrupts                                    */
                               | AT91C_US_TXRDY;                /* Disable Tx interrupt                                     */

    AT91C_BASE_DBGU->DBGU_CR   = AT91C_US_RXEN                  /* Enable the receiver                                      */
                               | AT91C_US_TXEN ;                /* Enable the transmitter                                   */
                              
    
    AT91C_BASE_DBGU->DBGU_MR   = AT91C_US_USMODE_NORMAL         /* Normal mode selected                                     */
                               | AT91C_US_PAR_NONE;             /* No parity bit selected                                   */

                                                                /* Set the DBGU baud rate                                   */
    AT91C_BASE_DBGU->DBGU_BRGR = (CPU_INT16U)( MCK / baud_rate / 16);

    AT91C_BASE_PMC->PMC_PCER   = (1 << AT91C_ID_DBGU);           /* Enable the DBGU peripheral clock                         */ 
    
    
    
}



void DBGU_Write_Byte(CPU_CHAR tx_byte)
{
  
    CPU_INT08U   err;  
    
    OSSemPend( DBGU_Tx_Sem_lock, 0, &err );   

    while ((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY) == 0); /*  Wait for room in the transmit register.                 */
  
    AT91C_BASE_DBGU->DBGU_THR = tx_byte;

    OSSemPost( DBGU_Tx_Sem_lock );

}



CPU_INT08U  DBGU_Read_Byte ( CPU_INT08U *pdata, CPU_INT32U timeout_ms )
{
    
    CPU_INT08U  err; 
    err   = 0 ;
    
    OSSemPend( DBGU_Rx_Sem_lock, 0, &err );  
    
    while ((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY) == 0) {     /*  Wait for a byte to show up.                         */
        OSTimeDly(1);
        if(timeout_ms-- == 0 ) {
          err = 1 ; //timeout error
          break;
        }
    }
    *pdata = (CPU_INT08U)(AT91C_BASE_DBGU->DBGU_RHR & 0x00FF);     /* Read the character.                                  */

    OSSemPost( DBGU_Rx_Sem_lock );
    
    return err;
}


//used to clear previous data in 
void DBGU_Read_NULL_RHR( void  )
{    
   (CPU_INT08U)(AT91C_BASE_DBGU->DBGU_RHR & 0x00FF);     /* Read the character.                                  */

}



void DBGU_Write_Buffer( CPU_INT08U * pBuf,  CPU_INT32U size )
{
  
    CPU_INT08U   err;  
    CPU_INT32U   i ;
    
    OSSemPend( DBGU_Tx_Sem_lock, 0, &err ); 
    
    for (i = 0; i < size; i++ ){
      
      while ((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY) == 0); /*  Wait for room in the transmit register.      */  
      AT91C_BASE_DBGU->DBGU_THR = *( pBuf + i );
    }
    
    OSSemPost( DBGU_Tx_Sem_lock );

}

CPU_INT08U  DBGU_Read_Buff ( CPU_INT08U *pdata, CPU_INT32U size, CPU_INT32U timeout_ms )
{
    CPU_INT32U   i ;
    CPU_INT08U  err; 
    err   = 0 ;
    
    OSSemPend( DBGU_Rx_Sem_lock, 0, &err );  
    
    for (i = 0; i < size ; i ++ ) {      
        while ((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY) == 0) {     /*  Wait for a byte to show up.                         */
            OSTimeDly(1);
            if(timeout_ms-- == 0 ) {
                  err = 1 ; //timeout error
                  OSSemPost( DBGU_Rx_Sem_lock );
                  return err;
            }
        }
        *(pdata+i) = (CPU_INT08U)(AT91C_BASE_DBGU->DBGU_RHR & 0x00FF);     /* Read the character.                                  */
    
    }
    
    OSSemPost( DBGU_Rx_Sem_lock );
    
    return err;
}



CPU_INT08U USART_Start_Ruler_Bootloader( void )
{
   
    unsigned char Buf[9];
    AT91S_USART *usart = pUARTREG[RULER_UART];
   
    usart->US_PTCR = AT91C_PDC_TXTDIS; //stop PDC
    if ( (usart->US_TCR != 0) || (usart->US_TNCR != 0) ) {
        return 1; //err
    }    
    memset(Buf,'d',sizeof(Buf)); 
    
    usart->US_TPR = (unsigned int) Buf;
    usart->US_TCR = sizeof(Buf);      
        
    Ruler_Power_Switch(0);     
    OSTimeDly(50);
    Ruler_Power_Switch(1);   
    OSTimeDly(200);
    usart->US_PTCR = AT91C_PDC_TXTEN; //start PDC
    OSTimeDly(800);
    if( USART_Read_Timeout( RULER_UART, Buf, 3, 5000 ) == 0 ) {
        if(Buf[0] == 'c' || Buf[0] == 'C' ) {
            return 0;
        }
    }
    return 2;
    
}


////////////////////////////////////////////////////////////////////////////////



void  memcpy_simple(CPU_INT08U * pDst, CPU_INT08U * pSrc, CPU_INT32U size) //without safty check !
{
	for(;size > 0; size--){
		*pDst++ = *pSrc++;
	}
    
}

