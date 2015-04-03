/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2006-2009; Micrium, Inc.; Weston, FL
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
*                                     MICRIUM BOARD SUPPORT PACKAGE
*                                      POWER MANAGEMENT CONTROLLER
*
* Filename      : bsp_pmc.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_PMC_MODULE
#include <bsp.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
                                                                /* ------------ PMC REGISTER DEFINES DEFINE ----------- */
#define  BSP_PMC_BASE_ADDR                          (CPU_INT32U)(0x400E0400)
#define  BSP_PMC_BASE_REG                           ((BSP_PMC_REG_PTR)(BSP_PMC_BASE_ADDR))

                                                                /* --------- SUPPLY CONTROLLER REGISTER DEFINES ------- */
#define  BSP_REG_PMC_SUPC_SR                       (*(CPU_REG32 *)0x400E1224)
#define  BSP_REG_PMC_SUPC_MR                       (*(CPU_REG32 *)0x400E1224)

                                                                /* ------ SUPPLY CONTROLLER REGISTER BIT DEFINES ------ */
#define  BSP_BIT_PMC_SUPC_SR_OSCSEL                 DEF_BIT_07
#define  BSP_BIT_PMC_SUPC_MR_OSCBYPASS              DEF_BIT_20  /* Oscillator Bypass                                    */

                                                                /* --------- MASTER CLOCK REGISTER BIT DEFINES -------- */
#define  BSP_BIT_PMC_MCKR_CSS_MASK                  DEF_BIT_FIELD(2, 0)
#define  BSP_BIT_PMC_MCKR_CSS_SLOW                  DEF_BIT_MASK(0, 0)
#define  BSP_BIT_PMC_MCKR_CSS_MAIN                  DEF_BIT_MASK(1, 0)
#define  BSP_BIT_PMC_MCKR_CSS_PLLA                  DEF_BIT_MASK(2, 0)
#define  BSP_BIT_PMC_MCKR_CSS_UPLL                  DEF_BIT_MASK(3, 0)

#define  BSP_BIT_PMC_MCKR_PRES_MASK                 DEF_BIT_FIELD(3, 4)
#define  BSP_BIT_PMC_MCKR_PRES_3                    DEF_BIT_MASK(7, 4)

#define  BSP_BIT_PMC_MCKR_UPLLDIV_2                 DEF_BIT_13
                                                                /* --------- MAIN OSCILLATOR REGISTER BIT DEFINES ----- */
#define  BSP_BIT_PMC_CKGR_MCFR_MAINFRDY             DEF_BIT_16
#define  BSP_BIT_PMC_CKGR_MCFR_MAINF_MASK           DEF_BIT_FIELD(16 , 0)

                                                                /* -- UTMI CLOCK CONFIGURATION REGISTER BIT DEFINES --- */

#define  BSP_BIT_PMC_UCKR_UPLLCOUNT_VAL             DEF_BIT_FIELD(4, 20)
#define  BSP_BIT_PMC_UCKR_UPLLEN                    DEF_BIT_16  /* UTMI PLL enable                                      */


                                                                /* ---------------- FIXED VALUES DEFINES -------------- */
#define  BSP_VAL_PMC_REG_TO                         ((CPU_INT16U)(0xFFFF))
#define  BSP_VAL_PMC_UTMI_PLL_MUL                          40   /* Fixed built-in multiplier of 40                      */


                                                               /* --------- PMC STATUS REGISTER BIT DEFINES ----------- */
#define  BSP_BIT_PMC_SR_LOCKU                       DEF_BIT_06


/*
*********************************************************************************************************
*                                       EXTERN  GLOBAL VARIABLES
*********************************************************************************************************
*/


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

typedef struct BSP_PMC_REG {
    CPU_REG32  PMC_SCER;                                        /* System Clock Enable Register                       */
    CPU_REG32  PMC_SCDR;                                        /* System Clock Disable Register                      */
    CPU_REG32  PMC_SCSR;                                        /* System Clock Status Register                       */
    CPU_REG32  RESERVED0[1];                                    /*                                                    */
    CPU_REG32  PMC_PCER;                                        /* Peripheral Clock Enable Register                   */
    CPU_REG32  PMC_PCDR;                                        /* Peripheral Clock Disable Register                  */
    CPU_REG32  PMC_PCSR;                                        /* Peripheral Clock Status Register                   */
    CPU_REG32  CKGR_UCKR;                                       /* UTMI Clock register                                */
    CPU_REG32  CKGR_MOR;                                        /* Main Oscillator Register                           */
    CPU_REG32  CKGR_MCFR;                                       /* Main Clock  Frequency Register                     */
    CPU_REG32  CKGR_PLLAR;                                      /* PLL A Register                                     */
    CPU_REG32  RESERVED1;                                       /* PLL B Register                                     */
    CPU_REG32  PMC_MCKR;                                        /* Master Clock Register                              */
    CPU_REG32  RESERVED2[3];                                    
    CPU_REG32  PMC_PCKR[3];                                     /* Programmable Clock Register                        */
    CPU_REG32  RESERVED3[5];                                    
    CPU_REG32  PMC_IER;                                         /* Interrupt Enable Register                          */
    CPU_REG32  PMC_IDR;                                         /* Interrupt Disable Register                         */ 
    CPU_REG32  PMC_SR;                                          /* Status Register                                    */
    CPU_REG32  PMC_IMR;                                         /* Interrupt Mask Register                            */
    CPU_REG32  PMC_FSMR;                                        /* Fast Startupt Mode Register                        */
    CPU_REG32  PMC_FSPR;                                        /* Fast Startup Polarity Register                     */
    CPU_REG32  PMC_FOCR;                                        /* Fault Output Clear register                        */
} BSP_PMC_REG, *BSP_PMC_REG_PTR;


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


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  CPU_INT32U   BSP_PMC_SlowOscFreqGet (void);
static  CPU_INT32U   BSP_PMC_PLL_FreqGet    (void);
static  CPU_INT32U   BSP_PMC_MainOscFreqGet (void);


/*
*********************************************************************************************************
*********************************************************************************************************
**                                        GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            BSP_PerClkDis()
*
* Description : Disable the corresponding peripheral clock
*
* Argument(s) : per_id        The peripheral id to be disabled (see note #1)
*                             BSP_PER_ID_UART
*                             BSP_PER_ID_SMC
*                                     .
*                                     .
*                                     .
*                             BSP_PER_ID_UDPHS
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The peripherals clk id are defined in the bsp_pmc.h file.
*********************************************************************************************************
*/

void  BSP_PerClkDis (CPU_INT08U  per_id)
{
                                                                /* ------------------ ARGUMENTS CHECK ---------------- */
    if (per_id < BSP_PER_ID_PMC_CTRL_MIN) {
        return;
    }
    
    if (per_id >  BSP_PER_ID_PMC_CTRL_MAX) {
        return;
    }
    
    BSP_PMC_BASE_REG->PMC_PCDR = DEF_BIT(per_id);
}


/*
*********************************************************************************************************
*                                            BSP_PerClkEn()
*
* Description : Enable the corresponding peripheral clock
*
* Argument(s) : per_id        The peripheral id to be enabled (see note #1)
*                             BSP_PER_ID_UART
*                             BSP_PER_ID_SMC
*                                     .
*                                     .
*                                     .
*                             BSP_PER_ID_UDPHS
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The peripherals clk id are defined in the bsp_pmc.h file.
*********************************************************************************************************
*/

void  BSP_PerClkEn (CPU_INT08U per_id)
{
                                                                /* ------------------ ARGUMENTS CHECK ----------------- */
    if (per_id < BSP_PER_ID_PMC_CTRL_MIN) {
        return;
    }
    
    if (per_id >  BSP_PER_ID_PMC_CTRL_MAX) {
        return;
    }
 
    BSP_PMC_BASE_REG->PMC_PCER = DEF_BIT(per_id);
}


/*
*********************************************************************************************************
*                                        BSP_PerClkFreq()
*
* Description : Return the peripheral clock frequency.
*
* Argument(s) : per_freq    The peripheral clock id.
*                             BSP_PER_ID_UART
*                             BSP_PER_ID_SMC
*                                     .
*                                     .
*                                     .
*                             BSP_PER_ID_UDPHS
*
* Return(s)   : The frequency of the peripheral clock in hertz.
* 
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_INT32U  BSP_PerClkFreq (CPU_INT08U  per_id)
{
    CPU_INT32U  per_freq;
    
    
    if (per_id < BSP_PER_ID_PMC_CTRL_MIN) {
        return (0);
    }
    
    if (per_id >  BSP_PER_ID_PMC_CTRL_MAX) {
        return (0);
    }
 
    per_freq = BSP_SysClkFreqGet(BSP_SYS_CLK_ID_MCLK);
    
    return (per_freq);
}


/*
*********************************************************************************************************
*                                           BSP_SysClkEn()
*
* Description : This function enables a specic system clk
*
* Argument(s) : sys_clk    The system clk id.
*                              BSP_SYS_CLK_ID_CPU        CPU Clock.
*                              BSP_SYS_CLK_ID_MCLK       Master clock.
*                              BSP_SYS_CLK_ID_UTMI       USB UTMI clock.
*
* Return  (s) : none.
*
* Caller  (s) : Application
*
* Note(s)     : none.              
*********************************************************************************************************
*/

void  BSP_SysClkEn (CPU_INT08U  sys_clk)
{
    CPU_INT32U  reg_to;

    
    switch (sys_clk) {                
        case BSP_SYS_CLK_ID_UTMI:                               /* Enable UTMI Clock ...                                */
                                                                /* ... Set the UTMI PLL startup time                    */
                                                                /* ... Enable the UTMI PLL                              */
             BSP_PMC_BASE_REG->CKGR_UCKR = BSP_BIT_PMC_UCKR_UPLLCOUNT_VAL
                                         | BSP_BIT_PMC_UCKR_UPLLEN;    
     
             reg_to = BSP_VAL_PMC_REG_TO;
                                                                /* Wait until PLL is enabled                            */
             while (DEF_BIT_IS_CLR(BSP_PMC_BASE_REG->PMC_SR, BSP_BIT_PMC_SR_LOCKU) &&
                    (reg_to > 0)) {
                 reg_to--;
             }        
             break;

        case BSP_SYS_CLK_ID_CPU:                               /* CPU & Master are always enabled                      */
        case BSP_SYS_CLK_ID_MCLK:
        default:
              break;
    }
}


/*
*********************************************************************************************************
*                                         BSP_SysClkDis()
*
* Description : Disable a specific system clock.
*
* Argument(s) : sys_clk    The system clk id.
*                              BSP_SYS_CLK_ID_CPU        CPU Clock.
*                              BSP_SYS_CLK_ID_MCLK       Master clock.
*                              BSP_SYS_CLK_ID_UTMI       USB UTMI clock.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_SysClkDis (CPU_INT08U sys_clk)
{
    CPU_INT16U  reg_to;
    
    
    switch (sys_clk) {                

        case  BSP_SYS_CLK_ID_UTMI:                              /* Disable UTMI Clock ...                               */
                                                                /* ... Disable the UTMI PLL                             */
              BSP_PMC_BASE_REG->CKGR_UCKR = DEF_BIT_NONE;
     
              reg_to = BSP_VAL_PMC_REG_TO;
                                                                /* Wait until PLL is enabled                            */
              while (DEF_BIT_IS_SET(BSP_PMC_BASE_REG->PMC_SR, BSP_BIT_PMC_SR_LOCKU) &&
                    (reg_to > 0)) {
                  reg_to--;
              }
              break;

        case  BSP_SYS_CLK_ID_CPU:                               /* CPU & Master clock cannot be disabled                */
        case  BSP_SYS_CLK_ID_MCLK:
        default:
              break; 
    }
}


/*
*********************************************************************************************************
*                                         BSP_SysClkFreqGet()
*
* Description : Get the frequency of one of the master clock.
*
* Argument(s) : sys_clk    System Clock id.
*                              BSP_SYS_CLK_ID_CPU
*                              BSP_SYS_CLK_ID_MCLK
*                              BSP_SYS_CLK_ID_UPLL
*
* Return(s)   : The specific system clock frequency in hertz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_SysClkFreqGet (CPU_INT08U  sys_clk)
{    
    CPU_INT08U  mclk_css;
    CPU_INT08U  mclk_pres;
    CPU_INT08U  mclk_div;
    CPU_INT32U  clk_freq;
    
    
    mclk_css  = (CPU_INT08U)(BSP_PMC_BASE_REG->PMC_MCKR & BSP_BIT_PMC_MCKR_CSS_MASK);
    
    clk_freq = 0;
    
    switch (mclk_css) {
        case BSP_BIT_PMC_MCKR_CSS_SLOW:
             clk_freq = BSP_PMC_SlowOscFreqGet();
             break;
             
        case BSP_BIT_PMC_MCKR_CSS_MAIN:
             clk_freq = BSP_PMC_MainOscFreqGet();
             break;

        case BSP_BIT_PMC_MCKR_CSS_PLLA:
             //clk_freq = BSP_PMC_PLL_FreqGet();
             clk_freq = BSP_PLL_GetFreq();  //PQ Modified
             break;

        case BSP_BIT_PMC_MCKR_CSS_UPLL:
             clk_freq = BSP_PMC_MainOscFreqGet() * BSP_VAL_PMC_UTMI_PLL_MUL;
             break;
             
        default:
            break;
             
    }
    
    mclk_pres = ((BSP_PMC_BASE_REG->PMC_MCKR &  BSP_BIT_PMC_MCKR_PRES_MASK) >> 4);
    
    if (mclk_pres == BSP_BIT_PMC_MCKR_PRES_3) {
        mclk_div = 6;
    } else {
        mclk_div = DEF_BIT(mclk_pres);
    }


    clk_freq /= mclk_div;

    switch (sys_clk) {                
        case  BSP_SYS_CLK_ID_CPU:                               /* CPU & Master clock have the same frequency            */
        case  BSP_SYS_CLK_ID_MCLK:
        default:
              break; 

        case  BSP_SYS_CLK_ID_UTMI:
              if (DEF_BIT_IS_SET(BSP_PMC_BASE_REG->PMC_MCKR, BSP_BIT_PMC_MCKR_UPLLDIV_2)) {
                  clk_freq /= 2;
              }
              break;

    }

    return (clk_freq);
}


/*
*********************************************************************************************************
*                                        BSP_PMC_MainOscFreqGet()
*
* Description : Return the Frequency of the main oscillator.
*
* Argument(s) : none
*
* Return(s)   : The frequency of the Main Oscillator.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The Main Oscillator features a Main Clock frequency counter that provides the quartz frequency
*                   connected to the Main Oscillator. Generally, this value is known by the system designer; however,
*                   it is useful for the boot program to configure the device with the correct clock speed,
*                   independently of the application.
*********************************************************************************************************
*/

CPU_INT32U  BSP_PMC_MainOscFreqGet (void)
{
    CPU_INT16U  mclk_to;
    CPU_INT16U  main_clk_ctr;
    CPU_INT32U  main_clk_freq;


    mclk_to = BSP_VAL_PMC_REG_TO;
    
    while ((mclk_to > 0) && 
           (DEF_BIT_IS_CLR(BSP_PMC_BASE_REG->CKGR_MCFR, BSP_BIT_PMC_CKGR_MCFR_MAINFRDY))) {
        mclk_to--;       
    }

    if (mclk_to == 0) {
        main_clk_freq = 0;
    } else {
        main_clk_ctr   = (BSP_PMC_BASE_REG->CKGR_MCFR & BSP_BIT_PMC_CKGR_MCFR_MAINF_MASK);
        main_clk_freq  = BSP_PMC_SlowOscFreqGet();
        main_clk_freq *= main_clk_ctr;
        main_clk_freq /= 16;
    }    
    
    return (main_clk_freq);
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                            PLL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          BSP_PMC_PLL_FreqGet()
*
* Description : This function return the frequency of the PLL
*
* Argument(s) : none.
*
* Return(s)   : The PLL frequency. (If the PLL is disabled this function returns 0)
*                                              
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/
            
static  CPU_INT32U  BSP_PMC_PLL_FreqGet (void)
{
    CPU_INT08U  pll_div;
    CPU_INT16U  pll_mul;
    CPU_INT32U  pll_freq;


    pll_div  = ((BSP_PMC_BASE_REG->CKGR_PLLAR >>  0) & 0x0FF);
    pll_mul  = ((BSP_PMC_BASE_REG->CKGR_PLLAR >> 16) & 0x7FF);
    pll_freq = 0;

    if (pll_div != 0) {
        pll_freq  = ((BSP_PMC_MainOscFreqGet() / 100) * (pll_mul + 1)) / pll_div;
        pll_freq *= 100;
    }
    
    return (pll_freq);        
}

/*
*********************************************************************************************************
*                                           BSP_SlowOscFreqGet()
*
* Description : This function gets the frequency of the slow clk
*
* Argument(s) : none.
*     
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT32U   BSP_PMC_SlowOscFreqGet (void)
{
    CPU_INT32U  osc_freq;
    
    if (DEF_BIT_IS_CLR(BSP_REG_PMC_SUPC_SR,  BSP_BIT_PMC_SUPC_SR_OSCSEL)) {
        osc_freq = BSP_PMC_SLOW_CLK_INT_RC_FREQ_HZ;

    } else if (DEF_BIT_IS_CLR(BSP_REG_PMC_SUPC_MR,  BSP_BIT_PMC_SUPC_MR_OSCBYPASS)) {
        osc_freq = BSP_PMC_SLOW_CLK_CRYSTAL_FREQ_HZ;

    } else {
        osc_freq = BSP_PMC_SLOW_CLK_EXT_CLK_FREQ_HZ;        
    }

    return (osc_freq);
}
