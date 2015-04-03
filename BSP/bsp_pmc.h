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
*                                         BOARD SUPPORT PACKAGE
*                                       POWER MANAGMENT CONTROLLER
*
*
* Filename      : bsp_pmc.h
* Version       : V1.00
* Programmer(s) : FT
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

#ifndef  BSP_PMC_PRESENT
#define  BSP_PMC_PRESENT


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/


#ifdef   BSP_PMC_MODULE
#define  BSP_PMC_EXT
#else
#define  BSP_PMC_EXT  extern
#endif

/*
*********************************************************************************************************
*                                        DEFAULT CONFIGURATION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

                                                                /* ------------- SLOW CLK GENERATOR DEFINES ----------- */
#define  BSP_PMC_SLOW_CLK_INT_RC_FREQ_HZ               32000    /* RC internal frequency (hz)                           */
#define  BSP_PMC_SLOW_CLK_CRYSTAL_FREQ_HZ              32768    /* 32768 slow clk oscilator                             */
#define  BSP_PMC_SLOW_CLK_EXT_CLK_FREQ_HZ                  0    /* External Crystal Frequency                           */

                                                                /* ------------------ MAIN XTAL DEFINES --------------- */
#define  BSP_PMC_MAIN_XTAL_FREQ_HZ                  18432000L   /* Onboard crystal frequency (hz).                      */

                                                                /* --------------------- PERIPHRALS ID's -------------------- */
                                                                /* ----- INSTANCE DESCRIPTION ------- | NVIC INT | PMC CTRL | */
#define  BSP_PER_ID_SUPC                                   0    /* Supply controller                  |     X    |          | */
#define  BSP_PER_ID_RSTC                                   1    /* Reset  controller                  |     X    |          | */
#define  BSP_PER_ID_RTC                                    2    /* Real Time Clock                    |     X    |          | */
#define  BSP_PER_ID_RTT                                    3    /* Real Time Timer                    |     X    |          | */
#define  BSP_PER_ID_WDT                                    4    /* Watchdog  Timer                    |     X    |          | */
#define  BSP_PER_ID_PMC                                    5    /* Power Managment controller         |     X    |          | */
#define  BSP_PER_ID_EEFC0                                  6    /* Enhanced Embedded Flash Ctrl 0     |     X    |          | */
#define  BSP_PER_ID_EEFC1                                  7    /* Enhanced Embedded Flash Ctrl 1     |     X    |          | */
#define  BSP_PER_ID_UART                                   8    /* Universal Asyncronous Rx/Tx  0     |     X    |     X    | */ 
#define  BSP_PER_ID_SMC                                    9    /* Static Memory Controller           |     X    |     X    | */ 
#define  BSP_PER_ID_PIOA                                  10    /* Parallel I/O Controller A          |     X    |     X    | */ 
#define  BSP_PER_ID_PIOB                                  11    /* Parallel I/O Controller B          |     X    |     X    | */ 
#define  BSP_PER_ID_PIOC                                  12    /* Parallel I/O Controller C          |     X    |     X    | */ 
#define  BSP_PER_ID_US0                                   13    /* USART 0                            |     X    |     X    | */
#define  BSP_PER_ID_US1                                   14    /* USART 1                            |     X    |     X    | */
#define  BSP_PER_ID_US2                                   15    /* USART 2                            |     X    |     X    | */
#define  BSP_PER_ID_US3                                   16    /* USART 3                            |     X    |     X    | */
#define  BSP_PER_ID_HSMCI                                 17    /* High Speed MCI                     |     X    |     X    | */
#define  BSP_PER_ID_TWI0                                  18    /* Two Wire Interface  0              |     X    |     X    | */
#define  BSP_PER_ID_TWI1                                  19    /* Two Wire Interface  1              |     X    |     X    | */
#define  BSP_PER_ID_SPI0                                  20    /* Serial Peripheral Interface 0      |     X    |     X    | */
#define  BSP_PER_ID_SSC0                                  21    /* Synchronous Serial Controller 0    |     X    |     X    | */
#define  BSP_PER_ID_TC0                                   22    /* Timer/Counter 0                    |     X    |     X    | */
#define  BSP_PER_ID_TC1                                   23    /* Timer/Counter 1                    |     X    |     X    | */
#define  BSP_PER_ID_TC2                                   24    /* Timer/counter 2                    |     X    |     X    | */
#define  BSP_PER_ID_PWM                                   25    /* Pulse Width Modulation Ctrl        |     X    |     X    | */
#define  BSP_PER_ID_AD12B                                 26    /* 12-BHit ADC Controller             |     X    |     X    | */
#define  BSP_PER_ID_ADC                                   27    /* 10-bit ADC Controller              |     X    |     X    | */
#define  BSP_PER_ID_DMAC                                  28    /* DMA Controller                     |     X    |     X    | */
#define  BSP_PER_ID_UDPHS                                 29    /* USB Device High Speed              |     X    |     X    | */  


#define  BSP_PER_ID_PMC_CTRL_MIN                   BSP_PER_ID_UART
#define  BSP_PER_ID_PMC_CTRL_MAX                   BSP_PER_ID_UDPHS

                                                                /* ------------------ SYSTEM CLOCK IDs ---------------- */
#define  BSP_SYS_CLK_ID_CPU                                0
#define  BSP_SYS_CLK_ID_MCLK                               1
#define  BSP_SYS_CLK_ID_UTMI                               2


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
                                                                /* ---------------- SYSTEM CLOCK FREQUENCY ------------ */
void         BSP_SysClkEn         (CPU_INT08U  sys_clk);
void         BSP_SysClkDis        (CPU_INT08U  sys_clk);
CPU_INT32U   BSP_SysClkFreqGet    (CPU_INT08U  sys_clk);

void         BSP_PerClkEn         (CPU_INT08U  per_id);
void         BSP_PerClkDis        (CPU_INT08U  per_id);
CPU_INT32U   BSP_PerClkFreq       (CPU_INT08U  per_id);


/*
*********************************************************************************************************
*                                              ERROR CHECKING
*********************************************************************************************************
*/

            
/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
