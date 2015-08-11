/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                              (c) Copyright 2009; Micrium, Inc.; Weston, FL
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
*                                      MICRIUM BOARD SUPPORT PACKAGE
*
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : ES
*               : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT



/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/


#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif

/*
*********************************************************************************************************
*                                        DEFAULT CONFIGURATION
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*
* Note(s) : (1) The following common software files are located in the following directories :
*
*               (a) \<Custom Library Directory>\lib*.*
*
*               (b) (1) \<CPU-Compiler Directory>\cpu_def.h
*
*                   (2) \<CPU-Compiler Directory>\<cpu>\<compiler>\cpu*.*
*
*                           where
*                                   <Custom Library Directory>      directory path for custom   library      software
*                                   <CPU-Compiler Directory>        directory path for common   CPU-compiler software
*                                   <cpu>                           directory name for specific processor (CPU)
*                                   <compiler>                      directory name for specific compiler
*
*           (2) Compiler MUST be configured to include the '\<Custom Library Directory>\uC-LIB\',
*               '\<CPU-Compiler Directory>\' directory, & the specific CPU-compiler directory as
*               additional include path directories.
*
*           (3) NO compiler-supplied standard library functions SHOULD be used.
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>

#include  <cpu.h>

#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_mem.h>

#include  <app_cfg.h>

#include  <bsp_int.h>
#include  <bsp_gpio.h>
#include  <bsp_pmc.h>
//#include  <bsp_ser.h>
#include  <bsp_os.h>

#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <cpu.h>
#include  <at91sam3u4.h>
#include  <app_cfg.h>
#include  <ucos_ii.h>
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdbool.h>
#include  <string.h>
#include  <probe_com_cfg.h>

#include  <taskcomm.h>
#include  <kfifo.h>

#include  <board.h>
#include  <uart.h>
#include  <pio.h>
#include  <pio_it.h>
#include  <gpio.h>
#include  <led.h>
#include  <timer.h>
#include  <eefc.h>
#include  <flashd.h>
#include  <twid.h>
#include  <spi.h>
#include  <i2c_gpio.h>
#include  <ruler.h>
#include  <emb.h>
#include  <mem_basic.h>
#include  <uif.h>
#include  <dsp.h>
#include  <im501_comm.h>
#include  <noah_cmd.h>
#include  <commands.h>
#include  <codec.h>
#include  <xmodem.h>
#include  <dma.h>
#include  <dmad.h>



#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


/*
*********************************************************************************************************
                                  Board type defines 
*********************************************************************************************************
*/

//#define  BOARD_TYPE_AB01  
//#define  BOARD_TYPE_AB02  
//#define  BOARD_TYPE_AB03  
#define  BOARD_TYPE_UIF


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
#define  BSP_MAIN_XTAL_FREQ                   12000000          /* Onboard Main crystal Osc. frequency (hz)           */
#define  BSP_SLOW_XTAL_FREQ                      32768          /* onboard Slow crystal Osc. frequency (hz)           */


                                                                /* ------------------- CLK SOURCES ------------------ */
#define  BSP_CLK_SRC_SLOW                            0          /* Slow  clock                                        */
#define  BSP_CLK_SRC_MAIN                            1          /* Main  clock                                        */
#define  BSP_CLK_SRC_PLLA                            2          /* PLLA   clock                                        */
#define  BSP_CLK_SRC_UPLL                            3          /* 1/2 UPLL   clock                                        */



/* ------------ INTERRUPT SOURCE TYPE --------------- */
#define  BSP_INT_SCR_TYPE_INT_HIGH_LEVEL_SENSITIVE   0          /* Internal Interrupt High Level Sensitive            */
#define  BSP_INT_SCR_TYPE_INT_POSITIVE_EDGE_TRIG     1          /* Internal Interrupt Positive Edge Trigerred         */
#define  BSP_INT_SCR_TYPE_EXT_LOW_LEVEL_SENSITIVE    2          /* External Interrupt Low Level Sensitive             */
#define  BSP_INT_SCR_TYPE_EXT_HIGH_LEVEL_SENSITIVE   3          /* External Interrupt Negative Edge Trigerred         */
#define  BSP_INT_SCR_TYPE_EXT_NEGATIVE_EDGE_TRIG     4          /* External Interrupt High Level Sensitive            */
#define  BSP_INT_SCR_TYPE_EXT_POSITIVE_EDGE_TRIG     5          /* External Interrupt Positive Edge triggered         */

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         BSP_LowLevelInit  (void);
void         BSP_PreInit       (void);
void         BSP_PostInit      (void);


/*
*********************************************************************************************************
*                                             LED SERVICES
*********************************************************************************************************
*/

void         BSP_LED_On          (CPU_INT08U led);
void         BSP_LED_Off         (CPU_INT08U led);
void         BSP_LED_Toggle      (CPU_INT08U led);


/*
*********************************************************************************************************
*                                         SYSTEM CLOCK FUNCTIONS
*********************************************************************************************************
*/

CPU_INT32U   BSP_CPU_Freq (void);
CPU_INT32U   BSP_MclkFreq (void);

/*
*********************************************************************************************************
*                                         PUSH BUTTON SERVICES
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_PB_GetStatus  (CPU_INT08U pb);


/*
*********************************************************************************************************
*                                              ERROR CHECKING
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            PLL FUNCTIONS
*********************************************************************************************************
*/

CPU_INT32U   BSP_PLL_GetFreq         (void);
CPU_BOOLEAN  BSP_PLL_Cfg             (CPU_INT16U  pll_mul,
                                      CPU_INT08U  pll_div,
                                      CPU_INT08U  pll_out,
                                      CPU_INT08U  pll_ctr);

/*
*********************************************************************************************************
*                                             RS-232 SERVICES
*********************************************************************************************************
*/

void         BSP_Ser_Init            (CPU_INT32U     baud_rate);
void         BSP_Ser_WrByte          (CPU_CHAR       tx_byte);
void         BSP_Ser_WrStr           (CPU_CHAR      *tx_str);
CPU_INT08U   BSP_Ser_RdByte          (void);
void         BSP_Ser_RdStr           (CPU_CHAR      *rx_str,
                                      CPU_INT32U     len);
void         BSP_Ser_Printf          (CPU_CHAR      *format, ...);



/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/
extern CPU_INT08U Debug_COM_Sel;
extern OS_EVENT *Bsp_Ser_Tx_Sem_lock;  
extern OS_EVENT *Bsp_Ser_Rx_Sem_lock;

extern const CPU_CHAR fw_version[];
extern const CPU_CHAR hw_version[];
extern const CPU_CHAR hw_model[];

extern void Head_Info( void );
extern void Beep( INT32U beep_cycles);
extern void Head_Info( void );
extern void Beep( INT32U beep_cycles);
extern volatile CPU_INT08U  Flag_Reset_Pin_Trigger;
//extern void PDM_Pattern_Gen( INT8U type );
extern void Time_Stamp( void );
extern void  Get_Flash_Info (void);



extern volatile CPU_INT08U  Flag_Reset_Pin_Trigger  ;

#define AT91C_BASE_TWI      AT91C_BASE_TWI0
#define AT91C_ID_TWI        AT91C_ID_TWI0
#define AT91C_IFLASH        AT91C_IFLASH0
#define AT91C_AIC_PRIOR_LOWEST               (15) // (AIC) Lowest priority level
#define AT91C_AIC_PRIOR_HIGHEST              (0) // (AIC) Highest priority level
typedef void( *IntFunc )( void );


#endif                                                          /* End of module include.                               */
