/*
*********************************************************************************************************
*                                     MICRIUM BOARD SUPPORT SUPPORT
*
*                          (c) Copyright 2003-2009; Micrium, Inc.; Weston, FL
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
*                                      MICRIUM BOARD SUPPORT PACKAGE
*                                         SERIAL (UART) INTERFACE
*
* Filename      : bsp_ser.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_SER_MODULE
#include <bsp.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
                                                                /* -------- USART CONTROL REGISTER BIT DEFINES -------- */
#define  BSP_SER_US_CR_RSTRX                  DEF_BIT_02        /* Reset receiver                                       */
#define  BSP_SER_US_CR_RSTTX                  DEF_BIT_03        /* Reset Transmitter                                    */
#define  BSP_SER_US_CR_RXEN                   DEF_BIT_04        /* Receiver Enable                                      */
#define  BSP_SER_US_CR_RXDIS                  DEF_BIT_05        /* Receiver Disable                                     */
#define  BSP_SER_US_CR_TXEN                   DEF_BIT_06        /* Transmitter Enable                                   */
#define  BSP_SER_US_CR_TXDIS                  DEF_BIT_07        /* Transmitter Disable                                  */
#define  BSP_SER_US_CR_RSTSTA                 DEF_BIT_08        /* Reset Status Bits                                    */
#define  BSP_SER_US_CR_STTBRK                 DEF_BIT_09        /* Start Break                                          */
#define  BSP_SER_US_CR_STPBRK                 DEF_BIT_10        /* Stop Break                                           */
#define  BSP_SER_US_CR_STTTO                  DEF_BIT_11        /* Start Time-out                                       */
#define  BSP_SER_US_CR_SENDA                  DEF_BIT_12        /* Send Address                                         */
#define  BSP_SER_US_CR_RSTIT                  DEF_BIT_13        /* Reset Iterations                                     */
#define  BSP_SER_US_CR_RSTNACK                DEF_BIT_14        /* Reset Non Acknowledge                                */
#define  BSP_SER_US_CR_RETTO                  DEF_BIT_15        /* Rearm Time-out                                       */
#define  BSP_SER_US_CR_RTSEN                  DEF_BIT_18        /* Request to Send Enable                               */
#define  BSP_SER_US_CR_RTSDIS                 DEF_BIT_19        /* Request to Send Disable                              */

                                                                /* ----------- USART MODE REGISTER BIT DEFINES -------- */
#define  BSP_SER_US_MR_MODE_MASK              DEF_BIT_FIELD(4, 0)
#define  BSP_SER_US_MR_MODE_NORMAL            DEF_BIT_NONE
                                                                /* Clock source select mask                             */
#define  BSP_SER_US_MR_USCLKS_MASK            DEF_BIT_FIELD(2, 4)
#define  BSP_SER_US_MR_USCLKS_MCK             DEF_BIT_NONE
#define  BSP_SER_US_MR_USCLKS_MCK_DIV         DEF_BIT_MASK(1, 4)
#define  BSP_SER_US_MR_USCLKS_SCK             DEF_BIT_MASK(3, 4)
                                                                /* Character Length Mask                                */
#define  BSP_SER_US_MR_CHRL_MASK              DEF_BIT_FIELD(2, 6)
#define  BSP_SER_US_MR_CHRL_5                 DEF_BIT_NONE    
#define  BSP_SER_US_MR_CHRL_6                 DEF_BIT_MASK(1, 6)
#define  BSP_SER_US_MR_CHRL_7                 DEF_BIT_MASK(2, 6)
#define  BSP_SER_US_MR_CHRL_8                 DEF_BIT_MASK(3, 6)
                                                                /* Parity Mask                                          */
#define  BSP_SER_US_MR_PAR_MASK               DEF_BIT_FIELD(3, 9)
#define  BSP_SER_US_MR_PAR_EVEN               DEF_BIT_NONE     
#define  BSP_SER_US_MR_PAR_ODD                DEF_BIT_MASK(1, 9)
#define  BSP_SER_US_MR_PAR_FORCED_0           DEF_BIT_MASK(2, 9)
#define  BSP_SER_US_MR_PAR_FORCED_1           DEF_BIT_MASK(3, 9)
#define  BSP_SER_US_MR_PAR_NONE               DEF_BIT_MASK(4, 9)
#define  BSP_SER_US_MR_PAR_MULTI_DROP         DEF_BIT_MASK(5, 9)
                                                                /* Number of stop bits                                  */
#define  BSP_SER_US_MR_NBSTOP_MASK            DEF_BIT_FIELD(2, 12)
#define  BSP_SER_US_MR_NBSTOP_1               DEF_BIT_NONE     
#define  BSP_SER_US_MR_NBSTOP_1_5             DEF_BIT_MASK(1, 12)
#define  BSP_SER_US_MR_NBSTOP_2               DEF_BIT_MASK(2, 12)


                                                                /* ------------ USART INTERRUPTS BIT DEFINES ---------- */
#define  BSP_SER_US_INT_RXRDY                 DEF_BIT_00        /* RXRDY Interrupt Enable                               */
#define  BSP_SER_US_INT_TXRDY                 DEF_BIT_01        /* TXRDY Interrupt Enable                               */
#define  BSP_SER_US_INT_RXBRK                 DEF_BIT_02        /* Receiver Break Interrupt Enable                      */
#define  BSP_SER_US_INT_ENDRX                 DEF_BIT_03        /* End of Receive Transfer Interrupt Enable             */
#define  BSP_SER_US_INT_ENDTX                 DEF_BIT_04        /* End of Transmit Interrupt Enable                     */
#define  BSP_SER_US_INT_OVRE                  DEF_BIT_05        /* Overrun Error Interrupt Enable                       */
#define  BSP_SER_US_INT_FRAME                 DEF_BIT_06        /* Framing Error Interrupt Enable                       */
#define  BSP_SER_US_INT_PARE                  DEF_BIT_07        /* Parity Error Interrupt Enable                        */
#define  BSP_SER_US_INT_TIMEOUT               DEF_BIT_08        /* Time-out Interrupt Enable                            */
#define  BSP_SER_US_INT_TXEMPTY               DEF_BIT_09        /* TXEMPTY Interrupt Enable                             */
#define  BSP_SER_US_INT_ITERATION             DEF_BIT_10        /* Iteration Interrupt Enable                           */
#define  BSP_SER_US_INT_TXBUFE                DEF_BIT_11        /* Buffer Empty Interrupt Enable                        */
#define  BSP_SER_US_INT_RXBUFF                DEF_BIT_12        /* Buffer Full Interrupt Enable                         */
#define  BSP_SER_US_INT_NACK                  DEF_BIT_13        /* Non Acknowledge Interrupt Enable                     */
#define  BSP_SER_US_INT_CTSIC                 DEF_BIT_19        /* Clear to Send Input Change Interrupt Enable          */
#define  BSP_SER_US_INT_ALL                  (DEF_BIT_FIELD(14, 0) | \
                                              BSP_SER_US_INT_CTSIC)

                                                                 /* ----------------- USART I/O DEFINES --------------- */
#define  BSP_SER_GPIOA_DBG_PINS              (DEF_BIT_11 | \
                                              DEF_BIT_12)

#define  BSP_SER_GPIOA_US1_PINS              (DEF_BIT_20 | \
                                              DEF_BIT_21)

                                                                 /* ------------- USART BASE ADDRESS DEFINES ---------- */

#define  BSP_SER_DBG_BASE_ADDR               (CPU_INT32U)(0x400E0600)
#define  BSP_SER_US1_BASE_ADDR               (CPU_INT32U)(0x40094000)



#define  BSP_SER_REG_TO                      (CPU_INT32U)(0x0000FFFF)



/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

typedef struct BSP_SER_REG {
    CPU_REG32  US_CR;                                         /* Control Register                                       */
    CPU_REG32  US_MR;                                         /* Mode Register                                          */
    CPU_REG32  US_IER;                                        /* Interrupt Enable Register                              */
    CPU_REG32  US_IDR;                                        /* Interrupt Disable Register                             */
    CPU_REG32  US_IMR;                                        /* Output Disable Registerr                               */
    CPU_REG32  US_CSR;                                        /* Output Status Register                                 */
    CPU_REG32  US_RHR;                                        /* Receiver Holding Register                              */
    CPU_REG32  US_THR;                                        /* Transmit Holding Register                              */
    CPU_REG32  US_BRGR;                                       /* Baud Rate Generator Register                           */
}  BSP_SER_REG, * BSP_SER_REG_PTR;


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  BSP_OS_SEM   BSP_SerTxWait;
static  BSP_OS_SEM   BSP_SerRxWait;
static  BSP_OS_SEM   BSP_SerLock;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void        BSP_SerWrByteUnlocked   (CPU_INT08U  c);
static  CPU_INT08U  BSP_SerRdByteUnlocked   (void);

#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_DBG)
static  void        BSP_SerDBG_ISR_Handler  (void);
#endif

#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_01)
static  void        BSP_SerUS1_ISR_Handler  (void);
#endif

static  void        BSP_SerUSx_ISR_Handler  (CPU_INT32U  reg_addr);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          BSP_Ser_Init()
*
* Description : Initialize a serial port for communication.
*
* Argument(s) : baud_rate           The desire RS232 baud rate.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_Init (CPU_INT32U  baud_rate)
{   
    CPU_INT32U     mclk_freq;
    CPU_INT08U     per_id;
    BSP_SER_REG   *p_ser_reg;
    
      
    BSP_OS_SemCreate(&BSP_SerTxWait, 0, "Serial Tx Wait");
    BSP_OS_SemCreate(&BSP_SerRxWait, 0, "Serial Rx Wait");
    BSP_OS_SemCreate(&BSP_SerLock  , 1, "Serial Lock");
 
    mclk_freq   = BSP_SysClkFreqGet(BSP_SYS_CLK_ID_MCLK);       /* Get peripheral clock frequency                       */

#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_DBG)
    BSP_GPIO_Cfg(BSP_GPIO_PORT_A,
                 (BSP_SER_GPIOA_DBG_PINS),
                  BSP_GPIO_OPT_PER_SEL_A); 
    per_id    = BSP_PER_ID_UART;
    p_ser_reg = (BSP_SER_REG *)BSP_SER_DBG_BASE_ADDR;

    BSP_IntVectSet(BSP_PER_ID_UART,  BSP_SerDBG_ISR_Handler);    
#endif    

#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_01)
    BSP_GPIO_Cfg(BSP_GPIO_PORT_A,
                 (BSP_SER_GPIOA_US1_PINS),
                  BSP_GPIO_OPT_PER_SEL_A); 
    per_id    = BSP_PER_ID_US1;
    p_ser_reg = (BSP_SER_REG *)BSP_SER_US1_BASE_ADDR;

    BSP_IntVectSet(BSP_PER_ID_US1,  BSP_SerUS1_ISR_Handler);
    
#endif    
    
    BSP_PerClkEn(per_id);                                       /* Enable the DBGU peripheral clock                     */

    p_ser_reg->US_IDR = BSP_SER_US_INT_ALL;  

    p_ser_reg->US_CR  = BSP_SER_US_CR_RXEN                      /* Enable the receiver                                  */
                      | BSP_SER_US_CR_TXEN;                     /* Enable the transmitter                               */

    p_ser_reg->US_MR  = BSP_SER_US_MR_MODE_NORMAL               /* RS232C mode selected                                 */
                      | BSP_SER_US_MR_USCLKS_MCK                /* USART input CLK is MCK                               */
                      | BSP_SER_US_MR_CHRL_8                    /* 8 bit data to be sent                                */
                      | BSP_SER_US_MR_PAR_NONE                  /* No parity bit selected                               */
                      | BSP_SER_US_MR_NBSTOP_1;                 /* 1 stop bit selected                                  */

    p_ser_reg->US_BRGR = (CPU_INT16U)((mclk_freq / baud_rate) / 16);


    BSP_IntEn(per_id);
}


/*
*********************************************************************************************************
*                                                BSP_Ser_Printf()
*
* Description : Formatted outout to the serial port.
*               This funcion reads a string from a serial port. This call blocks until a
*               character appears at the port and the last character is a Carriage
*               Return (0x0D).
*
* Argument(s) : Format string follwing the C format convention.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_Printf (CPU_CHAR *format, ...)
{
    CPU_CHAR  buf[128u + 1u];

    va_list   vArgs;

    va_start(vArgs, format);
    vsprintf((char *)buf, (char const *)format, vArgs);
    va_end(vArgs);

    BSP_SerWrStr((CPU_CHAR*)&buf[0]);
}


/*
*********************************************************************************************************
*                                                BSP_SerRdByte()
*
* Description : Receive a single byte.
*
* Argument(s) : none.
*
* Return(s)   : The received byte
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Ser_RdByte (void)
{
    CPU_INT08U  rx_byte;
    
    
    BSP_OS_SemWait(&BSP_SerLock, 0);

    rx_byte = BSP_SerRdByteUnlocked();
        
    BSP_OS_SemPost(&BSP_SerLock); 
    
    return (rx_byte);
}


/*
*********************************************************************************************************
*                                             BSP_SerRdByteUnlocked()
*
* Description : Receive a single byte.
*
* Argument(s) : none.
*
* Return(s)   : The received byte
*
* Caller(s)   : BSP_Ser_RdByte()
*               BSP_Ser_RdStr()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT08U  BSP_SerRdByteUnlocked (void)
{
    CPU_INT08U      rx_byte;      
    BSP_SER_REG   *p_ser_reg;
        
#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_DBG)
    p_ser_reg = (BSP_SER_REG *)BSP_SER_DBG_BASE_ADDR;
#endif    

#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_01)
    p_ser_reg = (BSP_SER_REG *)BSP_SER_US1_BASE_ADDR;
#endif       
    
    p_ser_reg->US_IER = BSP_SER_US_INT_RXRDY; 

    BSP_OS_SemWait(&BSP_SerRxWait, 0);

    rx_byte = (CPU_INT08U)(p_ser_reg->US_RHR & 0x00FF);

    p_ser_reg->US_IDR = BSP_SER_US_INT_RXRDY; 

    return (rx_byte);
}

/*
*********************************************************************************************************
*                                              BSP_SerRdStr()
*
* Description : This function reads a string from a UART.
*
* Argument(s) : p_str      A pointer to a buffer at which the string can be stored.
*
*               len         The size of the string that will be read.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_SerRdStr (CPU_CHAR    *p_str,
                    CPU_INT16U   len)
{
    CPU_CHAR     rx_data;
    CPU_CHAR     rx_buf_ix;
    CPU_BOOLEAN  err;


    rx_buf_ix = 0;
    p_str[0]  = 0;

    err = BSP_OS_SemWait(&BSP_SerLock, 0);                      /* Obtain access to the serial interface              */
    
    if (err != DEF_OK ) {
        return;
    }

    while (DEF_TRUE)
    {        
        rx_data = BSP_SerRdByteUnlocked();
        
        if ((rx_data == ASCII_CHAR_CARRIAGE_RETURN) ||          /* Is it '\r' or '\n' character  ?                    */
            (rx_data == ASCII_CHAR_LINE_FEED      )) {
                          
            BSP_SerWrByteUnlocked((CPU_INT08U)ASCII_CHAR_LINE_FEED);
            BSP_SerWrByteUnlocked((CPU_INT08U)ASCII_CHAR_CARRIAGE_RETURN);

            p_str[rx_buf_ix] = 0;                              /* set the null character at the end of the string     */
            break;                                             /* exit the loop                                       */
        }

        if (rx_data == ASCII_CHAR_BACKSPACE) {                 /* Is backspace character                              */
            if (rx_buf_ix > 0) {
                BSP_SerWrByteUnlocked((CPU_INT08U)ASCII_CHAR_BACKSPACE);
                BSP_SerWrByteUnlocked((CPU_INT08U)ASCII_CHAR_SPACE);
                BSP_SerWrByteUnlocked((CPU_INT08U)ASCII_CHAR_BACKSPACE);
                
                rx_buf_ix--;                                   /* Decrement the index                                 */
                p_str[rx_buf_ix] = 0;
            }
        }

        if (ASCII_IsPrint(rx_data)) {                           /* Is it a printable character ... ?                  */
            BSP_SerWrByteUnlocked((CPU_INT08U)rx_data);         /* Echo-back                                          */
            p_str[rx_buf_ix] = rx_data;                         /* Save the received character in the buffer          */
            rx_buf_ix++;                                        /* Increment the buffer index                         */
            if (rx_buf_ix >= len) {
               rx_buf_ix = len;
            }
        }
    }
    
    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface            */
}


/*
*********************************************************************************************************
*                                         BSP_Ser_USx_ISR_Handler()
*
* Description : Generic Serial ISR.
*
* Argument(s) : reg_addr      The base address of the AT91SAM3U USART registers.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Ser_DBG_ISR_Handler()
*               BSP_Ser_US1_ISR_Handler()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static void  BSP_SerUSx_ISR_Handler (CPU_INT32U  reg_addr)
{
    CPU_REG32     rx_data;
    BSP_SER_REG  *p_ser_reg;
    
    p_ser_reg = (BSP_SER_REG *)reg_addr;    
    
    if (DEF_BIT_IS_SET(p_ser_reg->US_CSR, BSP_SER_US_INT_RXRDY)) {
        BSP_OS_SemPost(&BSP_SerRxWait);
        p_ser_reg->US_IDR = BSP_SER_US_INT_RXRDY;
    }
    
    if (DEF_BIT_IS_SET(p_ser_reg->US_CSR, BSP_SER_US_INT_TXEMPTY)) {
        BSP_OS_SemPost(&BSP_SerTxWait);    
        p_ser_reg->US_IDR = BSP_SER_US_INT_TXEMPTY;
    }    
}


/*
*********************************************************************************************************
*                                         BSP_SerDBG_ISR_Handler()
*                                         BSP_SerUS1_ISR_Handler
*
* Description : Specific UARTx [DBG, 01 ] ISRs
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_DBG)
static  void  BSP_SerDBG_ISR_Handler  (void)
{
    BSP_SerUSx_ISR_Handler(BSP_SER_DBG_BASE_ADDR);
}
#endif

#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_01)
static  void  BSP_SerUS1_ISR_Handler  (void)
{
    BSP_SerUSx_ISR_Handler(BSP_SER_US1_BASE_ADDR);
}
#endif


/*
*********************************************************************************************************
*                                                BSP_SerWrByte()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : c       The character to output.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function blocks until room is available in the UART for the byte to be sent.
*********************************************************************************************************
*/

void  BSP_Ser_WrByte (CPU_INT08U  c)
{
    BSP_OS_SemWait(&BSP_SerLock, 0);
    
    BSP_SerWrByteUnlocked(c);
    
    BSP_OS_SemPost(&BSP_SerLock);
}


/*
*********************************************************************************************************
*                                                BSP_SerWrByteUnlocked ()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : c        The character to output.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Ser_WrByte()
*               BSP_Ser_WrStr()
*
* Note(s)     : (1) This function blocks until room is available in the UART for the byte to be sent.
*********************************************************************************************************
*/

static void  BSP_SerWrByteUnlocked (CPU_INT08U  tx_byte)
{
    BSP_SER_REG   *p_ser_reg;
    
    
#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_DBG)
    p_ser_reg = (BSP_SER_REG *)BSP_SER_DBG_BASE_ADDR;
#endif    

#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_01)
    p_ser_reg = (BSP_SER_REG *)BSP_SER_US1_BASE_ADDR;
#endif    

    p_ser_reg->US_THR = tx_byte;
    p_ser_reg->US_IER = BSP_SER_US_INT_TXEMPTY;

    BSP_OS_SemWait(&BSP_SerTxWait, 0);

    p_ser_reg->US_IDR = BSP_SER_US_INT_TXEMPTY;
}


/*
*********************************************************************************************************
*                                                BSP_SerWrStr()
*
* Description : Transmits a string.
*
* Argument(s) : tx_str      The string that will be transmitted.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/


void  BSP_SerWrStr (CPU_CHAR * tx_str)
{
   BSP_OS_SemWait(&BSP_SerLock, 0);

    while ((*tx_str) != 0) {
        BSP_SerWrByteUnlocked(*tx_str);

        if (*tx_str == ASCII_CHAR_LINE_FEED) {
            BSP_SerWrByteUnlocked(ASCII_CHAR_CARRIAGE_RETURN);
        }
        tx_str++;
    }

    BSP_OS_SemPost(&BSP_SerLock);
    /*  while ((*tx_str) != 0) {
        if (*tx_str == '\n') {
            BSP_Ser_WrByte('\n');
            BSP_Ser_WrByte('\r');
            tx_str++;
        } else {
            BSP_Ser_WrByte(*tx_str++);
        }        
    }*/
}