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
*                                        MICRIUM BOARD SUPPORT PACKAGE
*                                               
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include <includes.h>



/*
*********************************      Version Declaration       ****************************************
*/
const CPU_CHAR fw_version[]  = "[FW:H:V2.22]"; //fixed size string

#ifdef  BOARD_TYPE_AB01
const CPU_CHAR hw_version[]  = "[HW:V1.0]"; 
const CPU_CHAR hw_model[]    = "[AB01]"; 
#endif
#ifdef BOARD_TYPE_AB02
const CPU_CHAR hw_version[]  = "[HW:V1.0]"; 
const CPU_CHAR hw_model[]    = "[AB02]"; 
#endif
#ifdef  BOARD_TYPE_AB03
const CPU_CHAR hw_version[]  = "[HW:V1.0]"; 
const CPU_CHAR hw_model[]    = "[AB03]"; 
#endif
#ifdef  BOARD_TYPE_UIF
const CPU_CHAR hw_version[]  = "[HW:V2.0]"; 
const CPU_CHAR hw_model[]    = "[UIF02]"; 
#endif


OS_EVENT *Bsp_Ser_Tx_Sem_lock;  
OS_EVENT *Bsp_Ser_Rx_Sem_lock;

static  Pin PinBuzzer    =   BUZZER ;
CPU_INT08U Debug_COM_Sel = 0 ; //debug uart use:    0: DBGUART, 1: UART1, >1: debug muted




//////////////////////////////////////////////////////////////////////////


// Settings at 48/48MHz
#define AT91C_CKGR_MUL_SHIFT         16
#define AT91C_CKGR_OUT_SHIFT         14
#define AT91C_CKGR_PLLCOUNT_SHIFT     8
#define AT91C_CKGR_DIV_SHIFT          0

// Note: 
// MOSCXTST in CKGR_MOR takes 0x3F here, you can change it according to
// Crystal Oscillators Characteristics for quick startup; 
// PLLACOUNT in CKGR_PLLAR takes 0x3F here, you can change it according to 
// PLLA Characteristics for quick startup.
#define BOARD_OSCOUNT         (AT91C_CKGR_MOSCXTST & (0x3F << 8))
#define BOARD_PLLR ((1 << 29) | (0x7 << AT91C_CKGR_MUL_SHIFT) \
        | (0x0 << AT91C_CKGR_OUT_SHIFT) |(0x3F << AT91C_CKGR_PLLCOUNT_SHIFT) \
        | (0x1 << AT91C_CKGR_DIV_SHIFT))
//#define BOARD_MCKR ( AT91C_PMC_PRES_CLK_2 | AT91C_PMC_CSS_PLLA_CLK) //choose PLLA out 96M / 2 = 48M  //48000000
#define BOARD_MCKR ( AT91C_PMC_PRES_CLK | AT91C_PMC_CSS_PLLA_CLK) //choose PLLA out 96M / 1 = 96M  //96000000
// Define clock timeout
#define CLOCK_TIMEOUT           0xFF


/*
*********************************************************************************************************
*                                            REGISTER & BIT FIELD DEFINES
*********************************************************************************************************
*/

                                                                /* ---- POWER MANAGMENT CONTROLLER REGISTER DEFINES --- */
#define  BSP_REG_PMC_BASE_ADDR                           ((CPU_INT32U)(0x400E0400))

#define  BSP_REG_PMC_MOR                                 (*(CPU_REG32 *)(BSP_REG_PMC_BASE_ADDR + 0x0020))
#define  BSP_REG_PMC_PLLAR                               (*(CPU_REG32 *)(BSP_REG_PMC_BASE_ADDR + 0x0028))
#define  BSP_REG_PMC_MCKR                                (*(CPU_REG32 *)(BSP_REG_PMC_BASE_ADDR + 0x0030))
#define  BSP_REG_PMC_SR                                  (*(CPU_REG32 *)(BSP_REG_PMC_BASE_ADDR + 0x0068))

                                                                /* ------ POWER MANAGMENT CONTROLLER BIT DEFINES ------ */
#define  BSP_BIT_PMC_MCKR_CSS_MASK                       DEF_BIT_FIELD(2, 0)

#define  BSP_BIT_PMC_MCKR_CSS_SLOW                       DEF_BIT_MASK(0, 0)
#define  BSP_BIT_PMC_MCKR_CSS_MAIN                       DEF_BIT_MASK(1, 0)
#define  BSP_BIT_PMC_MCKR_CSS_PLLA                       DEF_BIT_MASK(2, 0)

#define  BSP_BIT_PMC_MCKR_PRES_MASK                      DEF_BIT_FIELD(3, 4)
#define  BSP_BIT_PMC_MCKR_PRES_1                         DEF_BIT_MASK(0, 4)
#define  BSP_BIT_PMC_MCKR_PRES_2                         DEF_BIT_MASK(1, 4)

#define  BSP_BIT_PMC_MCKR_UPLLDIV_MASK                   DEF_BIT_13
#define  BSP_BIT_PMC_MCKR_UPLLDIV_1                      DEF_BIT_NONE
#define  BSP_BIT_PMC_MCKR_UPLLDIV_2                      DEF_BIT_13

#define  BSP_BIT_PMC_MOR_MOSCXTEN                        DEF_BIT_00
#define  BSP_BIT_PMC_MOR_MOSCRCEN                        DEF_BIT_03
#define  BSP_BIT_PMC_MOR_KEY                             DEF_BIT_MASK(0x37, 16)
#define  BSP_BIT_PMC_MOR_MOSCXTST_VAL                    DEF_BIT_MASK(0x3F, 8)
#define  BSP_BIT_PMC_MOR_MOSCSEL                         DEF_BIT_24


#define  BSP_BIT_PMC_SR_MOSCXTS                          DEF_BIT_00
#define  BSP_BIT_PMC_SR_LOCKA                            DEF_BIT_01
#define  BSP_BIT_PMC_SR_MCKRDY                           DEF_BIT_03
#define  BSP_BIT_PMC_SR_LOCKU                            DEF_BIT_06
#define  BSP_BIT_PMC_SR_OSCSELS                          DEF_BIT_07
#define  BSP_BIT_PMC_SR_MOSCSELS                         DEF_BIT_16

#define  BSP_BIT_PMC_PLLR_DIV_MASK                       DEF_BIT_FIELD( 8,  0)          
#define  BSP_BIT_PMC_PLLR_CNT_MASK                       DEF_BIT_FIELD( 6,  8)
#define  BSP_BIT_PMC_PLLR_STMODE_MASK                    DEF_BIT_FIELD( 2, 14)
#define  BSP_BIT_PMC_PLLR_MUL_MASK                       DEF_BIT_FIELD(11, 16)

                                                                /* -------- SUPPLY CONTROLLER REGISTER DEFINES -------- */
#define  BSP_REG_SUPC_BASE_ADDR                          ((CPU_INT32U)(0x400E1210))

#define  BSP_REG_SUPC_CR                                 (*(CPU_REG32 *)(BSP_REG_SUPC_BASE_ADDR + 0x0000))
#define  BSP_REG_SUPC_SR                                 (*(CPU_REG32 *)(BSP_REG_SUPC_BASE_ADDR + 0x0014))

                                                                /* ----------- SUPPLY CONTROLLER BIT DEFINES --------- */
#define  BSP_BIT_SUPC_CR_XTALSEL                         DEF_BIT_03
#define  BSP_BIT_SUPC_CR_KEY                            ((CPU_INT32U)(0xA5000000))

#define  BSP_BIT_SUPC_SR_OSCSEL                          DEF_BIT_07

                                                                /* ----- ENHANCED EMBEDDED FLASH CONTROLLER (EEFC) ---- */
                                                                /* ----------- EEFC FLASH MODE REGISTER DEFINES ------- */

#define  BSP_REG_EFC0_BASE_ADDR                          ((CPU_INT32U)(0x400E0800))
#define  BSP_REG_EFC1_BASE_ADDR                          ((CPU_INT32U)(0x400E0A00))

#define  BSP_REG_EFC0_FMR                                (*(CPU_REG32 *)(BSP_REG_EFC0_BASE_ADDR  + 0x00))
#define  BSP_REG_EFC1_FMR                                (*(CPU_REG32 *)(BSP_REG_EFC1_BASE_ADDR  + 0x00))

                                                                /* ----------- EEFC FLASH MODE BIT DEFINES ------------ */
#define  BSP_BIT_EFCx_FMR_FWS_MASK                       DEF_BIT_MASK(4 , 8)
#define  BSP_BIT_EFCx_FMR_FWS_1                          DEF_BIT_MASK(0 , 8)
#define  BSP_BIT_EFCx_FMR_FWS_2                          DEF_BIT_MASK(1 , 8)
#define  BSP_BIT_EFCx_FMR_FWS_3                          DEF_BIT_MASK(2 , 8)
#define  BSP_BIT_EFCx_FMR_FWS_4                          DEF_BIT_MASK(3,  8)

                                                                /* ------------- BUS MATRIX REGISTER DEFINES ---------- */
#define  BSP_REG_MATRIX_BASE_ADDR                        ( (CPU_INT32U )(0x400E0200))
                                                              
#define  BSP_REG_MATRIX_SCFGx(slave_nbr)                 (*(CPU_REG32 *)(BSP_REG_MATRIX_BASE_ADDR + 0x40 + slave_nbr * 0x4))

#define  BSP_REG_BIT_MATRIX_SCFGx_DEFMSTR_TYPE_MSK       DEF_BIT_FIELD(2, 16)
#define  BSP_REG_BIT_MATRIX_SCFGx_DEFMSTR_TYPE_NONE      DEF_BIT_MASK(0, 16)
#define  BSP_REG_BIT_MATRIX_SCFGx_DEFMSTR_TYPE_LAST      DEF_BIT_MASK(1, 16)
#define  BSP_REG_BIT_MATRIX_SCFGx_DEFMSTR_TYPE_FIXED     DEF_BIT_MASK(2, 16)

#define  BSP_REG_BIT_MATRIX_SCFGx_FIXED_DEFMSTR_MASK     DEF_BIT_FIELD(3, 18)

                                                                /* --------------- WATCHDOG REGISTER DEFINES ---------- */
#define  BSP_REG_WDT_BASE_ADDR                           ((CPU_INT32U )(0x400E1250))
#define  BSP_REG_WDT_CR                                  (*(CPU_REG32 *)(BSP_REG_WDT_BASE_ADDR + 0x00))
#define  BSP_REG_WDT_MR                                  (*(CPU_REG32 *)(BSP_REG_WDT_BASE_ADDR + 0x04))
#define  BSP_REG_WDT_SR                                  (*(CPU_REG32 *)(BSP_REG_WDT_BASE_ADDR + 0x08))

                                                                /* ---- WATCHDOG TIMER MODE REGISTER BIT DEFINES ---- */
                                                                /* Watchdog disable                                   */
#define  BSP_BIT_WDT_MR_WDDIS                            DEF_BIT_15         

                                                                /* ------- RESET CONTROLLER REGISTER DEFINES -------- */
#define  BSP_REG_RSTC_BASE_ADDR                          ((CPU_INT32U )(0x400E1200))
#define  BSP_REG_RSTC_CR                                 (*(CPU_REG32 *)(BSP_REG_RSTC_BASE_ADDR + 0x00))
#define  BSP_REG_RSTC_MR                                 (*(CPU_REG32 *)(BSP_REG_RSTC_BASE_ADDR + 0x04))
#define  BSP_REG_RSTC_SR                                 (*(CPU_REG32 *)(BSP_REG_RSTC_BASE_ADDR + 0x08))

                                                                /* --- RESET CONTROLLER MODE REGISTER BIT DEFINES --- */
#define  BSP_BIT_RSTC_MR_ERSTL_MASK                      DEF_BIT_FIELD(4, 8)
#define  BSP_BIT_RSTC_MR_URSTEN                          DEF_BIT_00
#define  BSP_BIT_RSTC_MR_URSTIEN                         DEF_BIT_04
#define  BSP_BIT_RSTC_MR_ERSTL_16                        DEF_BIT_MASK(2, 8)
#define  BSP_BIT_RSTC_KEY_VAL                            (CPU_INT32U)(0xA5000000)

                                                                /* - RESET CONTROLLER CONTROL  REGISTER BIT DEFINES - */
#define  BSP_BIT_RSTC_CR_EXTRST                          DEF_BIT_03
                                                                /* -- RESET CONTROLLER STATUS REGISTER BIT DEFINES -- */
#define  BSP_BIT_RSTC_SR_NRSTL                           DEF_BIT_16

#define  BSP_VAL_MAX_TO                                  0xFFFF

#define  BSP_GPIOA_RXD0                                 DEF_BIT_19           /* UART0 (A): COM PORT 0                                    */
#define  BSP_GPIOA_TXD0                                 DEF_BIT_18           /* UART0 (A): COM PORT 0                                    */                                 
#define  BSP_GPIOA_DRXD                                 DEF_BIT_11          /* DBGU  (A): Debug Port                                    */
#define  BSP_GPIOA_DTXD                                 DEF_BIT_12          /* DBGU  (A): Debug Port 

/*
*********************************************************************************************************
*                                            GPIO DEFINES
*********************************************************************************************************
*/

#define  BSP_GPIOA_DBG                     (BSP_GPIOA_DRXD      |\
                                            BSP_GPIOA_DTXD)

#define  BSP_GPIOA_UART0                   (BSP_GPIOA_RXD0      |\
                                            BSP_GPIOA_TXD0)

                                                                /* ------------------ GPIOA PINS DEFINES -------------- */
#define  BSP_GPIOA_PB1                                   DEF_BIT_18
#define  BSP_GPIOA_PB2                                   DEF_BIT_19
#define  BSP_GPIOA_PB_GRP                                DEF_BIT_FIELD(2, 18)

                                                                /* -------------------- GPIOB Pins ------------------ */
#define  BSP_GPIOB_LED1                                  DEF_BIT_05
#define  BSP_GPIOB_LED2                                  DEF_BIT_06
#define  BSP_GPIOB_LED_GRP                               DEF_BIT_FIELD(2, 5)


                                                               /* --------------- TIMEOUT VALUES ------------------- */
#define  BSP_PLL_MAX_TIMEOUT                    0xFFFF
#define  BSP_MAIN_OSC_MAX_START_UP_VAL          0xFFFF
#define  BSP_MAIN_OSC_MAINF_MAX_TIMEOUT_VAL     0xFFFF
#define  BSP_MCLK_MAX_TIMEOUT                   0xFFFF
                                                                /* ------------ PERIPHERALS CONSTRAINS  ------------- */
#define  BSP_PLL_MAX_MUL                          2048          /* Maximum value for the PLL Multiplier               */
#define  BSP_AIC_INT_PRIO_MAX                        7          /* Highest Interrupt level in the AIC                 */
#define  BSP_PER_ID_MAX                             31          /* Number of peripherals.                             */
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

static  void  BSP_ClkInit             (void);

static  void  BSP_LED_Init            (void);

static  void  BSP_PB_Init             (void);

static  void  BSP_BP_Init             (void);

/*
***********************************************************************************************************
***********************************************************************************************************
**                                        GLOBAL FUNCTIONS
***********************************************************************************************************
***********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP_LowLevelInit()
*
* Description : Board Support Package Low Level Initialization.
*
* Argument(s) : none.
*
* Returns(s)  : none
*
* Caller(s)   : Startup code.
*
* Note(s)     : (1) This function can be used to initalize external memories and bring the CPU to the 
*                   maximum operating frequency.
*********************************************************************************************************
*/

void  BSP_LowLevelInit (void)
{
                                                               /* -------- FLASH CONTROLLER CONFIGURATION ---------- */
    BSP_REG_EFC0_FMR = BSP_BIT_EFCx_FMR_FWS_4;                 /* Flash wait states = 2                              */
    BSP_REG_EFC1_FMR = BSP_BIT_EFCx_FMR_FWS_4;
    
                                                               /* ---------- WATCHDOG TIMER CONFIGURATION ---------- */
    BSP_REG_WDT_MR   = BSP_BIT_WDT_MR_WDDIS;                   /* Disable the Watchdog Timer                         */
    
                                                               /* -------- RESET CONTROLLER INITIALIZATION --------- */
    BSP_REG_RSTC_MR  =  BSP_BIT_RSTC_MR_ERSTL_16               /* ... External Reset Length = 16 Slow Cycles         */
                     |  BSP_BIT_RSTC_MR_URSTEN                 /* ... User Reset Enable                              */
                     |  BSP_BIT_RSTC_KEY_VAL;                  /* ... Key Password                                   */
                        
    BSP_REG_RSTC_CR  = BSP_BIT_RSTC_CR_EXTRST                  /* ... Peripheral Reset                               */
                     | BSP_BIT_RSTC_KEY_VAL;                   /* ... Key Password                                   */
  
    
    BSP_ClkInit();                                              /* Initialize Clocks.                                */
    

                                                                /* -------------- BUS MATRIX CONFIGURATION ---------- */
    DEF_BIT_SET(BSP_REG_MATRIX_SCFGx(0), DEF_BIT_MASK(2, 16) |
                                         DEF_BIT_MASK(1, 18));
    
    DEF_BIT_SET(BSP_REG_MATRIX_SCFGx(1), DEF_BIT_MASK(2, 16) |
                                         DEF_BIT_MASK(1, 18));

    DEF_BIT_SET(BSP_REG_MATRIX_SCFGx(3), DEF_BIT_MASK(2, 16) |
                                         DEF_BIT_MASK(0, 18));

}



/*
*********************************************************************************************************
*                                         BSP_CPU_Freq()
*
* Description : This function returns the processor clock frequency.
*
* Argument(s) : none.
*
* Returns(s)  : The CPU clock frequency in hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_Freq (void)
{
    CPU_INT32U  cpu_freq;
    
    
    cpu_freq = BSP_SysClkFreqGet(BSP_SYS_CLK_ID_CPU);    

    return (cpu_freq);
}


/*
*********************************************************************************************************
*                                          BSP_Clk_Init()
*
* Description : Initialize the AT91SAM3U clks.
*                   (1) Configure the slow clock.
*                   (2) Configure & initialize the main clock.
*                   (3) Switch Master clock source to main clock.
*                   (4) Configure the PLLA & USB frequency:
*                       PLLA_freq    =  (MUL * XTAL_FREQ) / DIV         
*                                    =  ( 8  * 12000000 ) / 2   
*                                    = 48 Mhz        
* 
*                       USB_freq     = XTAL_FREQ * UTMI_PLL_MUL / DIV
*                                    = 12000000  * 40           / 1
*                                    = 480 Mhz
*    
*                   (5) Switch Master clock source to PLLA clock.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : BSP_LowLevelInit()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_ClkInit (void)
{
    CPU_INT32U  reg_val;
    CPU_INT16U  reg_to;
    CPU_INT32U  mclk_freq;
                                                                // -------------- SLOW CLOCK CONFIGURATION ------------                   
    BSP_REG_SUPC_CR = BSP_BIT_SUPC_CR_XTALSEL                   // Select the 32 Khz XTAL osc. for slow clock output    
                    | BSP_BIT_SUPC_CR_KEY;
    
    reg_to = BSP_VAL_MAX_TO;                                    // Wait until the selection is stabilized               
    
    while (DEF_BIT_IS_CLR(BSP_REG_SUPC_SR, BSP_BIT_SUPC_SR_OSCSEL) &&
          (reg_to > 0)) {
        reg_to--;
    } 
                                                                // -------------- MAIN CLOCK CONFIGURATION ------------ 

    BSP_REG_PMC_MOR = BSP_BIT_PMC_MOR_MOSCXTEN                  // ... Main Crystal Oscillator is enable                
                    | BSP_BIT_PMC_MOR_MOSCXTST_VAL              // ... Maximum Main crystal oscillator start-up time    
                    | BSP_BIT_PMC_MOR_MOSCRCEN                  // ... Main On Chip RC oscillator enable                
                    | BSP_BIT_PMC_MOR_KEY;                      // ... Key Passsword                                    
    
    reg_to = BSP_VAL_MAX_TO;
                                                                // Wait until Main XTAL oscillator is stabilized        
    while ((DEF_BIT_IS_CLR(BSP_REG_PMC_SR, BSP_BIT_PMC_SR_MOSCXTS)) && 
           (reg_to > 0)) {
        reg_to--;
    }
    
    BSP_REG_PMC_MOR = BSP_BIT_PMC_MOR_MOSCXTEN                  // ... Main Crystal Oscillator is enable                
                    | BSP_BIT_PMC_MOR_MOSCXTST_VAL              // ... Maximum Main crystal oscillator start-up time    
                    | BSP_BIT_PMC_MOR_MOSCRCEN                  // ... Main On Chip RC oscillator enable                
                    | BSP_BIT_PMC_MOR_MOSCSEL                   //... The main Crystal Oscillator is selected           
                    | BSP_BIT_PMC_MOR_KEY;                      // ... Key Passsword                                    
    
    reg_to = BSP_VAL_MAX_TO;
                                                                // Wait until Main XTAL oscillator is stabilized       
    while ((DEF_BIT_IS_CLR(BSP_REG_PMC_SR, BSP_BIT_PMC_SR_MCKRDY)) && 
           (reg_to > 0)) {
        reg_to--;
    }

                                                                // ----------------- PLLA CONFIGURATION --------------- 
                                                                // Switch the Master and CPU clock the the main clock   
    reg_val = BSP_REG_PMC_MCKR;
    DEF_BIT_CLR(reg_val, BSP_BIT_PMC_MCKR_CSS_MASK);
    DEF_BIT_SET(reg_val, BSP_BIT_PMC_MCKR_CSS_MAIN);
        
    BSP_REG_PMC_MCKR = reg_val;
        
    reg_to = BSP_VAL_MAX_TO;                                    //Wait until the MCLK is ready is stabilized           
    
    while (DEF_BIT_IS_CLR(BSP_REG_PMC_SR, BSP_BIT_PMC_SR_MCKRDY) &&
          (reg_to > 0)) {
        reg_to--;
    } 
        
    DEF_BIT_CLR(reg_val, BSP_BIT_PMC_MCKR_PRES_MASK);
    DEF_BIT_SET(reg_val, BSP_BIT_PMC_MCKR_PRES_1);

    BSP_REG_PMC_MCKR = reg_val;

    while (DEF_BIT_IS_CLR(BSP_REG_PMC_SR, BSP_BIT_PMC_SR_MCKRDY) &&
          (reg_to > 0)) {
        reg_to--;
    } 
    
                                                                // ... Divider      =   1                               
                                                                //... PLLA Counter = 255                               
                                                                // ... Start Mode   = Normal Startup                    
                                                                //... Multiplier   =  8                                
                                                                // ... PLLA_freq    =  (MUL * XTAL_FREQ) / DIV          
                                                                // ...              =  ( 8  * 12000000 ) / 2            
                                                                // ...              = 48 Mhz                            
    BSP_REG_PMC_PLLAR =  ((      1u  << 0 ) & BSP_BIT_PMC_PLLR_DIV_MASK)  //PQ   96MHz
                      |  ((    255u  << 8 ) & BSP_BIT_PMC_PLLR_CNT_MASK) 
                      |  ((      2u  << 14) & BSP_BIT_PMC_PLLR_STMODE_MASK)
                      |  (((8u - 1u) << 16) & BSP_BIT_PMC_PLLR_MUL_MASK)
                      | DEF_BIT_29;
    
    reg_to = BSP_VAL_MAX_TO;
    
                                                                // Wait until PLLA is stabilized                       
    while ((DEF_BIT_IS_CLR(BSP_REG_PMC_SR, BSP_BIT_PMC_SR_LOCKA)) && 
           (reg_to > 0)) {
        reg_to--;
    }
    
                                                                //---------- CPU, MASTER & USB CONFIGURATION  --------
    
    reg_val = BSP_REG_PMC_MCKR;
    DEF_BIT_CLR(reg_val, BSP_BIT_PMC_MCKR_PRES_MASK);
    DEF_BIT_SET(reg_val, BSP_BIT_PMC_MCKR_PRES_1);

    BSP_REG_PMC_MCKR = reg_val;

    while (DEF_BIT_IS_CLR(BSP_REG_PMC_SR, BSP_BIT_PMC_SR_MCKRDY) &&
          (reg_to > 0)) {
        reg_to--;
    } 

    DEF_BIT_CLR(reg_val, BSP_BIT_PMC_MCKR_CSS_MASK);
    DEF_BIT_SET(reg_val, BSP_BIT_PMC_MCKR_CSS_PLLA);
        
    BSP_REG_PMC_MCKR = reg_val;
        
    reg_to = BSP_VAL_MAX_TO;                                //Wait until the MCLK is ready is stabilized          
    
    while (DEF_BIT_IS_CLR(BSP_REG_PMC_SR, BSP_BIT_PMC_SR_MCKRDY) &&
          (reg_to > 0)) {
        reg_to--;
    }
    mclk_freq=BSP_SysClkFreqGet(BSP_SYS_CLK_ID_MCLK);  
 
    
    mclk_freq=BSP_SysClkFreqGet(BSP_SYS_CLK_ID_CPU);  
}


/*
*********************************************************************************************************
*                                    BSP_MclkFreq()
*
* Description : Return the master clock (MCLK) frequency.
*
* Argument(s) : none
*
* Return(s)   : The CPU clock frequency in hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_MclkFreq (void)
{
    CPU_INT32U  mclk_freq;
    
    
    mclk_freq = BSP_SysClkFreqGet(BSP_SYS_CLK_ID_MCLK);
    
    return  (mclk_freq); 
}


/*
*********************************************************************************************************
*                                          BSP_PreInit()
*
* Description : System Pre-Initialization. This function is called before the OS is initializaed.
*
* Argument(s) : none.
*
* Return(s)   : none.

* Caller(s)   : Application.
*
* Note(s)     : (1) This function should be used to initialized peripherals drivers that doesn't interact
*                   with the OS.               
*********************************************************************************************************
*/

void  BSP_PreInit (void)
{    
    BSP_LED_Init();                                             /* Initialize the I/Os for the LEDs                   */

    BSP_PB_Init();                                              /* Initialize the I/Os for the Push Buttons           */

    BSP_BP_Init();   
}


/*
*********************************************************************************************************
*                                          BSP_PostInit()
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Argument(s) : none.
*
* Return(s)   : none.

* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_PostInit (void)
{    
    BSP_IntInit();                                              /* Initialize the Interrupt Controller                */
}


/*
*********************************************************************************************************
*                                        BSP_LED_Init()
*
* Description : This function initializes the LED I/O pins.
*
* Argument(s) : none
*
* Returns     : none
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Init (void)
{
    BSP_PerClkEn(BSP_PER_ID_PIOB);
        
    BSP_GPIO_Cfg(BSP_GPIO_PORT_B,          
                 BSP_GPIOB_LED_GRP,
                 BSP_GPIO_OPT_PIN_EN |                          /* PIO controls the I/O line                         */
                 BSP_GPIO_OPT_OUT_EN);                          /* Output enable                                     */

    BSP_LED_Off(0);                                             /* Turn OFF all the LEDs                             */
}


/*
*********************************************************************************************************
*                                            BSP_LED_Off()
*
* Description : This function is used turn off any or all of the LEDs on the board.
*
* Argument(s) : led    is the number of the LED to turn OFF
*                      0    indicates that you want ALL the LEDs to be OFF
*                      1    turns OFF LED1 on the board
*                      2    turns OFF LED2 on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Off (CPU_INT08U led)
{
    switch (led) {
        case 0:
             BSP_GPIO_Set(BSP_GPIO_PORT_B, BSP_GPIOB_LED_GRP);
             break;

        case 1:
             BSP_GPIO_Set(BSP_GPIO_PORT_B, BSP_GPIOB_LED1);
             break;

        case 2:
             BSP_GPIO_Set(BSP_GPIO_PORT_B, BSP_GPIOB_LED2);
             break;

        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : This function is used turn on any or all of the LEDs on the board.
*
* Argument(s) : led    is the number of the LED to control
*                      0    indicates that you want ALL the LEDs to be ON
*                      1    turns ON LED1 on the board
*                      2    turns ON LED2 on the board
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_On (CPU_INT08U led)
{
    switch (led) {
        case 0:
             BSP_GPIO_Clr(BSP_GPIO_PORT_B, BSP_GPIOB_LED_GRP);     
             break;

        case 1:
             BSP_GPIO_Clr(BSP_GPIO_PORT_B, BSP_GPIOB_LED1);     
             break;

        case 2:
             BSP_GPIO_Clr(BSP_GPIO_PORT_B, BSP_GPIOB_LED2);     
             break;

        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                             BSP_LED_Toggle()
*
* Description : This function is used to toggle any or all the LEDs on the board.
*
* Argument(s) : led    is the number of the LED to toggle
*                      0    indicates that you want ALL the LEDs to be toggle
*                      1    toggles LED1 on the board
*                      2    toggles LED2 on the board
*                      3    toggles LED3 on the board (the power LED)
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (CPU_INT08U led)
{
    switch (led) {
        case 0:
             BSP_GPIO_Toggle(BSP_GPIO_PORT_B, BSP_GPIOB_LED_GRP);     
             break;

        case 1:
             BSP_GPIO_Toggle(BSP_GPIO_PORT_B, BSP_GPIOB_LED1);     
             break;

        case 2:
             BSP_GPIO_Toggle(BSP_GPIO_PORT_B, BSP_GPIOB_LED2);     
             break;

        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                         BSP_PB_GetStatus()
*
* Description : This function is used to get the status of any push button on the board.
*
* Argument(s) : pb    is the number of the push button to probe
*                     1    probe the push button B1
*                     2    probe the push button B2
*
* Return(s)   : DEF_ON     if the push button is pressed
*               DEF_OFF    if the push button is not pressed
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_PB_GetStatus (CPU_INT08U pb)
{
    CPU_BOOLEAN  status;
    CPU_INT32U   reg_val;


    status  = DEF_OFF;
    reg_val = BSP_GPIO_StatusGet(BSP_GPIO_PORT_A);

    switch (pb) {
        case 1:
            if (DEF_BIT_IS_CLR(reg_val, BSP_GPIOA_PB1)) {
                status  = DEF_ON;
            }
            break;

        case 2:
            if (DEF_BIT_IS_CLR(reg_val, BSP_GPIOA_PB2)) {
                status  = DEF_ON;
            }
            break;  

        default:
            break;
    }

    return (status);
}


/*
*********************************************************************************************************
*                                         BSP_PB_Init()
*
* Description : This function initializes the I/O for the PBs.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_PB_Init (void)
{
    BSP_PerClkEn(BSP_PER_ID_PIOA);

    BSP_GPIO_Cfg(BSP_GPIO_PORT_A,
                 BSP_GPIOA_PB_GRP,
                 BSP_GPIO_OPT_PIN_EN |
                 BSP_GPIO_OPT_PULLUP_EN);
}

/*
*********************************************************************************************************
*                                         BSP_BP_Init()
*
* Description : This function initializes the I/O for the Buzzer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_BP_Init (void)
{
//    BSP_PerClkEn(BSP_PER_ID_PIOA);
//
//    BSP_GPIO_Cfg(BSP_GPIO_PORT_A,
//                 BSP_GPIOA_PB_GRP,
//                 BSP_GPIO_OPT_PIN_EN |
//                 BSP_GPIO_OPT_PULLUP_EN);
}



/*
*********************************************************************************************************
*                                         BSP_Reset_ISR_Handler()
*
* Description : Handle pin reset peripheral interrupts.
*
* Argument(s) : none.
*
* Note(s)  :
*********************************************************************************************************
*/
volatile CPU_INT08U  Flag_Reset_Pin_Trigger = 0 ;

static void  BSP_Reset_ISR_Handler (void)
{
    //check if NRST interruption assert //PQ
    if( AT91C_BASE_RSTC->RSTC_RSR & AT91C_RSTC_URSTS ) {       
        Flag_Reset_Pin_Trigger++ ;           
    } 

}

/*
*********************************************************************************************************
*                                       BSP_ResetInit()
*
* Description : (1) Initialize NRST interruption:
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  BSP_ResetInit (void)
{   
       
   AT91C_BASE_RSTC->RSTC_RMR = (CPU_INT32U)(0xA5 << 24) | AT91C_RSTC_URSTIEN ;  //enable NRST interrupt    
   AT91C_BASE_RSTC->RSTC_RSR; // read RSR, clear previous data   
   
   //BSP_IntEn(AT91C_ID_RSTC);
   BSP_IntVectSet(AT91C_ID_RSTC,  BSP_Reset_ISR_Handler);
               
   IRQ_ConfigureIT(AT91C_ID_RSTC, AT91C_AIC_PRIOR_HIGHEST, (IntFunc)0);
   IRQ_EnableIT(AT91C_ID_RSTC);
  
}
/*
*********************************************************************************************************
*                                             BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/
void  BSP_Init (void)
{
      
    AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;       /* Disable the Watchdog Timer                               */
    
    BSP_PostInit();                                             /* Initialize BSP functions    BSP_IntInit();                             */
    BSP_OS_TmrTickInit(OS_TICKS_PER_SEC);                       /* Initialize the uC/OS-II ticker                       */ 
    
    GPIO_Init();   
     
    BSP_ResetInit();                                     /* Enable the hardware reset button  used interrupt         */
         
    // Configure IIC    
    TWI_Init( TWCK ); //It seems that the TWI will auto desrease SCK if loading increase           
    //Init_CODEC_5620();
    
    // Init Internal Flash
    FLASHD_Initialize( MCK );
//    //flash_test();
//    FLASHD_Erase(0x100000);
//    FLASHD_Write(0x100000,temp,10);
//    FLASHD_Erase(0x100000);
    
    // Config USART
    UART_Init(PC_UART,       ISR_PC_UART,  1000000 );    //To PC 
    //UART_Init(SIGNAL_POWER_UART, NULL,         9600   );    //To Power Supply  & Signal Generator    
    UART_Init(AUDIO_UART,    NULL,         115200 );    //To Audio_DP and Audio_DC  
          
    //Config Timer
    Timer_Init();  
          
    //Init ADCs
  //  Init_ADC_Voltage() ; 
 //   Init_ADC_Current() ;   
    
                            /* Initialize uC/OS-II's Tick Rate and DEBUG UART                          */

}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Get the processor clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/
CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    CPU_INT32U  mclk_css;
    CPU_INT32U  mclk_div;
    CPU_INT32U  cpu_freq;

    
    mclk_css = (AT91C_BASE_PMC->PMC_MCKR     ) & 0x00000003;    /* Get the master clk source                          */
    mclk_div = (AT91C_BASE_PMC->PMC_MCKR >> 2) & 0x00000003;    /* Get the master clk preescaler                      */
    mclk_div = DEF_BIT(mclk_div);
    
    switch (mclk_css) {

        case BSP_CLK_SRC_SLOW:                                 /* Slow clock                                         */
             cpu_freq = BSP_SLOW_XTAL_FREQ;
             break;

        case BSP_CLK_SRC_MAIN:                                 /* Main clock                                         */
             cpu_freq = BSP_MAIN_XTAL_FREQ;
             break;

        case BSP_CLK_SRC_PLLA:                                  /* PLL clock                                          */
             cpu_freq = BSP_PLL_GetFreq();
             break;

        //case BSP_CLK_SRC_RESERVED:
        default:
             cpu_freq = 0;
             break;             
    }

    cpu_freq = cpu_freq / mclk_div;

    return (cpu_freq);
}

/*
*********************************************************************************************************
*********************************************************************************************************
**                                             PLL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            BSP_PLL_GetFreq()
*
* Description : Enable  and configure the PLL
*
* Argument(s) : none.
*
* Return(s)   : DEF_TRUE      If the PLL is enabled and stabilized
*               DEF_FALSE     If the PLL could not be stabilized during a period of time
*                             specified by BSP_PLL_MAX_TIMEOUT
*                               
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/
            
CPU_INT32U  BSP_PLL_GetFreq (void)
{
    CPU_INT08U  pll_div;
    CPU_INT16U  pll_mul;
    CPU_INT32U  freq;
  
               
    pll_div = ((AT91C_BASE_PMC->PMC_PLLAR >>  0) & 0x0FF);
    pll_mul = ((AT91C_BASE_PMC->PMC_PLLAR >> 16) & 0x7FF);

    if (pll_div != 0) {
        freq = (BSP_MAIN_XTAL_FREQ * (pll_mul + 1)) / pll_div;    
    }
    
    return (freq);        
}

/*
*********************************************************************************************************
*                                               BSP_PLL_Set()
*
* Description : Enable  and configure the PLL
*
* Argument(s) : pll_div    The pll divider.            
*
*               pll_mul    The pll Multipler.   
*
*               pll_ctr    The number of slow clock cycles before the LOCKA bit ise set in PMC_SR.
*
*               pll_out    : PLL frequency optimization parameter (see note #1)
*
* Return(s)   : DEF_TRUE   If the PLL is enabled and stabilized
*               DEF_FALSE  If the PLL could not be stabilized during a period of time
*                          specified by BSP_PLL_MAX_TIMEOUT
*                               
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/
            
CPU_BOOLEAN  BSP_PLL_Cfg (CPU_INT16U  pll_mul,
                          CPU_INT08U  pll_div,
                          CPU_INT08U  pll_out,
                          CPU_INT08U  pll_ctr)
{
    CPU_INT32U  timeout;
    
                                                                /* ---------------- ARGUMENTS CHECKING -------------- */
    if (pll_mul > BSP_PLL_MAX_MUL) {
        return (DEF_FALSE);
    }
    
    if ((pll_out != 0x00) && 
        (pll_out != 0x01)) {
        return (DEF_FALSE);
    }
    
    AT91C_BASE_PMC->PMC_PLLAR  = ((pll_div       ) << 0 )
                              | ((pll_ctr & 0x3F) << 8 )
                              | ((pll_out & 0x03) << 14)
                              | ((pll_mul - 1   ) << 16);
                             
    
    timeout = BSP_PLL_MAX_TIMEOUT;

    while ((timeout > 0) &&     //wait until PLL is locked
          (DEF_BIT_IS_CLR(AT91C_BASE_PMC->PMC_SR, DEF_BIT_02))) {
        timeout--;       
    }
        
    if (timeout == 0) {
        return (DEF_FALSE);
    } else {
        return (DEF_TRUE);  
    }
                 
}




/*
*********************************************************************************************************
*********************************************************************************************************
**                                       Serial Port Communications
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                   BSP_Ser_Init()
*
* Description : Initialize a serial port for communication.
*
* Argument(s) : baud_rate: Desire baud rate for serial communication.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  BSP_Ser_Init (CPU_INT32U baud_rate)
{
    CPU_INT32U  pclk_freq;


    pclk_freq                  = BSP_CPU_ClkFreq();             /* Get the UART input clock frequency                       */

//#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_DBG)  
    if( Debug_COM_Sel == 0 ) {
      
    AT91C_BASE_PIOA->PIO_PDR   |= BSP_GPIOA_DBG;                 /* Set GPIOA pins 9 and 10 as DBGU UART pins                */
    AT91C_BASE_PIOA->PIO_ABSR  &= ~BSP_GPIOA_DBG;   //PQ              /* Select GPIOA attached peripheral (DBGU)                  */
    
    AT91C_BASE_PMC->PMC_PCER   = (1 << AT91C_ID_DBGU);
     
    AT91C_BASE_DBGU->DBGU_CR   =  AT91C_US_RSTRX 
                               |  AT91C_US_RSTTX ;             /*  Reset the receiver  and transmitter                      */
      
    AT91C_BASE_DBGU->DBGU_IDR  = AT91C_US_RXRDY                 /* Disable Rx interrupts                                    */
                               | AT91C_US_TXRDY;                /* Disable Tx interrupt                                     */   
    
    AT91C_BASE_DBGU->DBGU_CR   = AT91C_US_RXEN                  /* Enable the receiver                                      */
                               | AT91C_US_TXEN   ;              /* Enable the transmitter                                   */
                      
      
    AT91C_BASE_DBGU->DBGU_MR   = AT91C_US_USMODE_NORMAL         /* Normal mode selected                                     */
                               | AT91C_US_PAR_NONE;             /* No parity bit selected                                   */

                                                                /* Set the DBGU baud rate                                   */
    AT91C_BASE_DBGU->DBGU_BRGR = (CPU_INT16U)((pclk_freq) / baud_rate / 16);

   // AT91C_BASE_PMC->PMC_PCER   = (1 << AT91C_ID_DBGU);           /* Enable the DBGU peripheral clock                         */
//#endif
    } else {
//#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_UART_01)    
                                                                /* Set GPIOA pins 0 & 1 as US0 pins                         */
    AT91C_BASE_PIOA->PIO_PDR   = BSP_GPIOA_UART0;
    AT91C_BASE_PIOA->PIO_ABSR  &= ~BSP_GPIOA_UART0;
    
    AT91C_BASE_US0->US_CR      = AT91C_US_RSTRX 
                               | AT91C_US_RSTTX ;               /* Reset the receiver  and transmitter                      */
    
                                                                /* ---------------------- SETUP US0 ----------------------- */
    AT91C_BASE_US0->US_IDR     = 0xFFFFFFFF;  //disbale all int
    AT91C_BASE_US0->US_CR      = AT91C_US_RXEN  | AT91C_US_TXEN;  /* Enable the receiver  and transmitter                   */                           
                                
    AT91C_BASE_US0->US_MR      = AT91C_US_USMODE_NORMAL         /* RS232C mode selected                                     */
                               | AT91C_US_CLKS_CLOCK            /* USART input CLK is MCK                                   */
                               | AT91C_US_CHRL_8_BITS           /* 8 bit data to be sent                                    */
                               | AT91C_US_PAR_NONE              /* No parity bit selected                                   */
                               | AT91C_US_NBSTOP_1_BIT;         /* 1 stop bit selected                                      */
                                                                /* Set the USART baud rate                                  */
    AT91C_BASE_US0->US_BRGR    = (CPU_INT16U)((pclk_freq) / baud_rate / 16);
  
    AT91C_BASE_US0->US_PTCR    = AT91C_PDC_TXTDIS               //Disable previous PDC settings
                               | AT91C_PDC_RXTDIS;
                                                                /* ---------------- INITIALIZE AIC FOR US0 ---------------- */
    AT91C_BASE_PMC->PMC_PCER   = (1 << AT91C_ID_US0);           /* Enable the US0 peripheral clock                          */
//#endif    
    }
    
}

/*
*********************************************************************************************************
*                                                BSP_Ser_WrByte()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : tx_byte     The character to output.
*
* Return(s)   : none.
*
* Note(s)     : (1) This functino blocks until room is available in the UART for the byte to be sent.
*********************************************************************************************************
*/

void BSP_Ser_WrByte(CPU_CHAR tx_byte)
{
  
    unsigned char   err;  
    
    OSSemPend( Bsp_Ser_Tx_Sem_lock, 0, &err );  
    
//#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_DBG) 
    if( Debug_COM_Sel == 0 ) {   
      
        while ((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY) == 0);  /*  Wait for room in the transmit register.                 */
        AT91C_BASE_DBGU->DBGU_THR = tx_byte;
//#endif    
        
    } else if( Debug_COM_Sel == 1 ){
      
//#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_01) 
        while ((AT91C_BASE_US0->US_CSR & AT91C_US_TXRDY) == 0);  /*  Wait for room in the transmit register.                 */
        AT91C_BASE_US0->US_THR    = tx_byte;
    
//#endif    
    }  else {
    
        // DBG UART used as CHIP_UART connection now
    }
    
    OSSemPost( Bsp_Ser_Tx_Sem_lock );

}

/*
*********************************************************************************************************
*                                                BSP_Ser_WrStr()
*
* Description : Write a character string to a serial port.
*
* Argument(s) : tx_str      A character string.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  BSP_Ser_WrStr (CPU_CHAR *tx_str)
{
    while ((*tx_str) != 0) {
//        if (*tx_str == '\n') {
//            BSP_Ser_WrByte('\n');
//            BSP_Ser_WrByte('\r');
//            tx_str++;
//        } else {
            BSP_Ser_WrByte(*tx_str++);
//        }        
    }
}

/*
*********************************************************************************************************
*                                                BSP_Ser_RdByte()
*
* Description : Read a byte from a serial port and echo byte to port.
*
* Argument(s) : none.
*
* Return(s)   : A byte containing the value of the received charcater.
*
* Note(s)     : (1) This function blocks until a character appears at the port.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Ser_RdByte (void)
{
    CPU_INT08U      rx_byte;
    unsigned char   err; 
    
    OSSemPend( Bsp_Ser_Rx_Sem_lock, 0, &err );  

     if( Debug_COM_Sel == 0 ) {   
       
        //#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_DBG) 
        while ((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY) == 0) {     /*  Wait for a byte to show up.                         */
            OSTimeDly(2);
        }
        rx_byte = (CPU_INT08U)(AT91C_BASE_DBGU->DBGU_RHR & 0x00FF);     /* Read the character.                                  */
    //#endif  
        
     } else if( Debug_COM_Sel == 1 ){
   
    //#if (BSP_CFG_SER_COMM_SEL == BSP_SER_COMM_UART_01) 
        while ((AT91C_BASE_US0->US_CSR & AT91C_US_RXRDY) == 0) {     /*  Wait for a byte to show up.                         */
            OSTimeDly(2);
        }
        rx_byte = (CPU_INT08U)(AT91C_BASE_US0->US_RHR & 0x00FF);     /* Read the character.                                  */
    //#endif   
        
     } else {
       
        // DBG UART used as CHIP_UART connection now
     }
     
     OSSemPost( Bsp_Ser_Rx_Sem_lock );
    
    return (rx_byte);
}

/*
*********************************************************************************************************
*                                                BSP_Ser_RdStr()
*
* Description : Read a string from a serial port.
*
* Argument(s) : rx_str      A pointer to a buffer at which the string can be stored.
*               len         The size of the string that will be read.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  BSP_Ser_RdStr (CPU_CHAR    *rx_str,
                     CPU_INT32U   len)
{
    CPU_CHAR  input;
    CPU_CHAR  input_ix;


    input_ix  = 0;
    rx_str[0] = 0;

    while (DEF_TRUE)
    {
        input = BSP_Ser_RdByte();

        if ((input == '\r') ||
            (input == '\n')) {
            BSP_Ser_Printf("\n");
            rx_str[input_ix] = 0;
            break;
        }

        if (input == '\b') {
            if (input_ix > 0) {
                BSP_Ser_Printf("\b \b");
                input_ix--;
                rx_str[input_ix] = 0;
            }
        }

        if (ASCII_IS_PRINT(input)) {
            BSP_Ser_Printf("%c", input);
            rx_str[input_ix] = input;
            input_ix++;
            if (input_ix >= len) {
               input_ix = len;
            }
        }
    }
}

/*
*********************************************************************************************************
*                                                BSP_Ser_Printf()
*
* Description : Formatted outout to the serial port.
*
* Argument(s) : format      Format string follwing the C format convention.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  BSP_Ser_Printf (CPU_CHAR *format, ...)
{
    static  CPU_CHAR  buffer[80 + 1];
            va_list   vArgs;


    va_start(vArgs, format);
    vsprintf((char *)buffer, (char const *)format, vArgs);
    va_end(vArgs);

    BSP_Ser_WrStr((CPU_CHAR*) buffer);
    //BSP_Ser_WrStr_To_Buffer((CPU_CHAR*) buffer);
}



/*
*********************************************************************************************************
*                                         Beep()
*
* Description : Beep Buzzer
*
* Argument(s) : beep times.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskJoy()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void Beep( INT32U beep_cycles)
{
  
   INT32U i ;
   
   for(i = 0; i< beep_cycles; i++)  {
    
        PIO_Clear(&PinBuzzer); //beep on
        LED_Clear(LED_DS1); 
        LED_Set(LED_DS2);
        OSTimeDly(250);           
        PIO_Set(&PinBuzzer); //beep off
        LED_Clear(LED_DS2); 
        LED_Set(LED_DS1);
        OSTimeDly(250); //delay_ms(250);  
    
    }
   
}


/*
*********************************************************************************************************
*                                         get_os_state()
*
* Description : get_os_state
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

const CPU_INT08S os_stat_desp[][6] = {"Sem  ","MBox ","Q    ","Suspd","Mutex","Flag ","  *  ","Multi","Ready"} ;

CPU_INT08S* get_os_state( INT8U os_state ) 
{
  
  CPU_INT08U i = 0 ;

  for(i=0;i<8;i++) {
    if(os_state == (1<<i) ) {     
      return  (CPU_INT08S*)os_stat_desp[i];      
    }
  }
  
  return  (CPU_INT08S*)os_stat_desp[i];  
  
}


/*
*********************************************************************************************************
*                                         Get_Task_Info()
*
* Description : Print task related information
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Get_Task_Info (void)
{
    OS_TCB    *ptcb;
    CPU_INT08U index = 1 ;
    
    if (OSRunning == OS_TRUE) {
     
        APP_TRACE_INFO(("------------------------------------------------------   DEBUG INFORMATION   -------------------------------------------------------\r\n"));
        APP_TRACE_INFO(("|-------------------------   T  A  S  K   --------------------------------------|----------------   S  T  A  C  K   ---------------|\r\n"));
        APP_TRACE_INFO(("| ID  |    Name    | Priority | CtxSwCtr | State | Delay |      Waitting On     |  Point@ | Cur. | Max. | Size | Starts@ | Ends@   |\r\n"));
        ptcb = OSTCBList;                                  /* Point at first TCB in TCB list               */
        while (ptcb->OSTCBPrio != OS_TASK_IDLE_PRIO) {     /* Go through all TCBs in TCB list              */          
            //APP_TRACE_INFO(( "|%4d ",ptcb->OSTCBId ));  //same as ptcb->OSTCBPrio
            APP_TRACE_INFO(( "|%2d ",index++ ));
            APP_TRACE_INFO(( "%13.13s",ptcb->OSTCBTaskName ));
            APP_TRACE_INFO(( "      %2d ",ptcb->OSTCBPrio ));
            APP_TRACE_INFO(( "  %10d ",ptcb->OSTCBCtxSwCtr ));            
            APP_TRACE_INFO(( "   %s  ",get_os_state( ptcb->OSTCBStat )  ));
            APP_TRACE_INFO(( " %5d ",ptcb->OSTCBDly ));
            APP_TRACE_INFO(( " %22.22s ", (INT32U)(ptcb->OSTCBEventPtr) == 0 ?  (INT8U *)" " : ptcb->OSTCBEventPtr->OSEventName ));  
              
            APP_TRACE_INFO(( " %08X ",ptcb->OSTCBStkPtr ));
            APP_TRACE_INFO(( " %4d ",(ptcb->OSTCBStkBase - ptcb->OSTCBStkPtr)*4 ));
            APP_TRACE_INFO(( " %5d ",ptcb->OSTCBStkUsed ));
            APP_TRACE_INFO(( " %5d ",ptcb->OSTCBStkSize * 4 ));
            APP_TRACE_INFO(( " %08X ",ptcb->OSTCBStkBase ));
            APP_TRACE_INFO(( " %08X ",ptcb->OSTCBStkBottom ));           
            APP_TRACE_INFO(( " |\r\n" ));     
            ptcb = ptcb->OSTCBNext;                        /* Point at next TCB in TCB list                */          
        }
        APP_TRACE_INFO(("------------------------------------------------------------------------------------------------------------------------------------\r\n"));  
         
    }
}

/*
*********************************************************************************************************
*                                         Get_Run_Time()
*
* Description : Print run time information
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void Get_Run_Time( void )
{
  
    INT32U time   ;
    INT8U  sec ;
    INT8U  min ;
    INT8U  hour ;
    INT8U  day ;   
    
    time = OSTime / 1000L ;
    sec  = time % 60 ;
    min  = time / 60 %60 ;
    hour = time / 3600 % 24 ;
    day  = time / 3600 / 24 ;
    
    APP_TRACE_INFO(("OS Running Time  =  %02d days : %02d hours : %02d min : %02d sec\r\n", day,hour,min, sec )); 

}


void Time_Stamp( void )
{  
    
    INT32U time   ;
    INT8U  sec ;
    INT8U  min ;
    INT8U  hour ;
    INT16U  msec ;
    
    msec = OSTime % 1000L ;
    time = OSTime / 1000L ;
    sec  = time % 60 ;
    min  = time / 60 %60 ;
    hour = time / 3600 % 24 ;
    
    APP_TRACE_INFO(("\r\n[%02d:%02d:%02d.%03d] ", hour,min, sec, msec )); 

}



void  Get_Flash_Info (void)
{
    unsigned char i = 0;
    FLASH_INFO flash_info; 
    
    APP_TRACE_INFO(("\r\n"));
    APP_TRACE_INFO(("------------------------------------------------------   Flash INFORMATION   -------------------------------------------------------\r\n"));
    for(i=0; i<FLASH_ADDR_FW_VEC_NUM; i++ ) {        
        Read_Flash_State(&flash_info, i==0 ? FLASH_ADDR_FW_STATE : (FLASH_ADDR_FW_VEC_STATE + AT91C_IFLASH_PAGE_SIZE * i) );
        if(flash_info.flag == 0x55 ) {
            APP_TRACE_INFO(("------- Flash Seg[%d] >>>\r\n",i)); 
            APP_TRACE_INFO(("Flash Write Cycle:       State_Page = %d cycles,  Data_Page = %d cycles\r\n", flash_info.s_w_counter,flash_info.f_w_counter ));
            APP_TRACE_INFO(("Bin File:                \"%s\" (%d Bytes), [0x%0X, %s]\r\n", (flash_info.f_w_state == FW_DOWNLAD_STATE_FINISHED ? flash_info.bin_name : " ?? "), flash_info.bin_size, flash_info.f_w_state,(flash_info.f_w_state == FW_DOWNLAD_STATE_FINISHED ? "OK" : "Error")));
        }
    }
    APP_TRACE_INFO(("------------------------------------------------------------------------------------------------------------------------------------\r\n"));  

}


/*
*********************************************************************************************************
*                                         Head_Info()
*
* Description : Print Head information
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskUserIF()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void Head_Info ( void )
{ 
        
    APP_TRACE_INFO(("\r\n\r\n")); 
    APP_TRACE_INFO(("-----------------------------------------------------------\r\n"));
    APP_TRACE_INFO(("----                    Fortemedia                    -----\r\n"));
    APP_TRACE_INFO(("----             EVM Unified Interface Board          -----\r\n"));
    APP_TRACE_INFO(("----   %20s-%s             -----\r\n", hw_model, fw_version));
    APP_TRACE_INFO(("----   Compile date:  %12s, %8s, by PQ   -----\r\n", __DATE__, __TIME__));
    APP_TRACE_INFO(("-----------------------------------------------------------\r\n"));
    APP_TRACE_INFO(("\r\n"));                
    //APP_TRACE_INFO(("------------------------------------------------------------------------------------------------------------------------------------\r\n"));  
    APP_TRACE_INFO(("Micrium uC/OS-II on the Atmel AT91SAM7A3. Version : V%d.%d \r\n",(OSVersion()/ 100),(OSVersion() % 100)  ));
    APP_TRACE_INFO(("CPU Usage = %d%%, CPU Speed = %3d MHz, Tick_Per_Second = %6d ticks/sec  \r\n", OSCPUUsage,  (BSP_CPU_ClkFreq() / 1000000L),OS_TICKS_PER_SEC ));
    APP_TRACE_INFO(("#Ticks = %8d, #CtxSw = %8d \r\n", OSTime, OSCtxSwCtr )); 
    Get_Run_Time();
    APP_TRACE_INFO(("\r\n"));  
    APP_TRACE_INFO(("-------------------------------------------------   GLOBAL VARIABLES STATUS   ------------------------------------------------------\r\n")); 
    APP_TRACE_INFO(("MEM_Part_MsgUART :         %7d(Max%2d) / %2d   of the memory partiation used\r\n", pMEM_Part_MsgUART->OSMemNBlks - pMEM_Part_MsgUART->OSMemNFree, pMEM_Part_MsgUART->OSMemNBlks - pMEM_Part_MsgUART->OSMemNFreeMin,  pMEM_Part_MsgUART->OSMemNBlks)); 
    APP_TRACE_INFO(("Tx_ReSend_Happens:         %7d   times happened\r\n", Tx_ReSend_Happens ));
    APP_TRACE_INFO(("Tx_ReSend_Happens_Ruler:   %7d   times happened\r\n", Tx_ReSend_Happens_Ruler ));
    APP_TRACE_INFO(("TWI_Sem_lock:              %7d   ( default 1 )\r\n", TWI_Sem_lock->OSEventCnt ));   
    //APP_TRACE_INFO(("TWI_Sem_done:              %7d   ( default 0 )\r\n", TWI_Sem_done->OSEventCnt ));
    APP_TRACE_INFO(("UART_MUX_Sem_lock:         %7d   ( default 1 )\r\n", UART_MUX_Sem_lock->OSEventCnt ));
    APP_TRACE_INFO(("Done_Sem_RulerUART:        %7d   ( default 0 )\r\n", Done_Sem_RulerUART->OSEventCnt ));
    APP_TRACE_INFO(("Global_Ruler_State[3..0]:        [%d - %d - %d - %d]\r\n", Global_Ruler_State[3],Global_Ruler_State[2],Global_Ruler_State[1],Global_Ruler_State[0] ));
    APP_TRACE_INFO(("Global_Ruler_Type[3..0] :        [%X - %X - %X - %X]\r\n", Global_Ruler_Type[3],Global_Ruler_Type[2],Global_Ruler_Type[1],Global_Ruler_Type[0] ));
    APP_TRACE_INFO(("Global_Mic_Mask[3..0][] :        [%X - %X - %X - %X]\r\n", Global_Mic_Mask[3],Global_Mic_Mask[2],Global_Mic_Mask[1],Global_Mic_Mask[0] ));
    APP_TRACE_INFO(("Test Counter:            test_counter1, 2, 3, 4  =  %4d,%4d,%4d,%4d\r\n",  test_counter1, test_counter2,test_counter3, test_counter4));
    APP_TRACE_INFO(("Test Counter:  UART_WriteStart Failed :  %4d  times\r\n",   test_counter5));
    APP_TRACE_INFO(("DBG_UART_FIFO: [Max_Usage: %d/%d ]  [FIFO Overflow Hit: %d times]\r\n", debug_uart_fifo_data_max,DBG_UART_Send_Buf_Size,debug_uart_fifo_oveflow_counter ));
    APP_TRACE_INFO(("\r\n"));
    Get_Task_Info ();
    Get_Flash_Info ();
    //APP_TRACE_INFO(("\r\n")); 

}



/*
*********************************************************************************************************
*                                         PDM_Pattern_Gen()
*
* Description : Debug_Send_PDM_Pattern_ for CP2240
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskUserIF()
*
* Note(s)     : none.
*********************************************************************************************************
*/
 
//#define PDM_DAT         0   //PA26
//#define PDM_CLK         1   //PA27
//#define PATTERN_DATA    0xD4
//#define IDLE_DATA       0xCC
//#define PATTERN_WIDTH   1 // 1bytes
//#define REPEAT_TIMES    150
//
//
//
//void __ramfunc PDM_Out( unsigned int data, unsigned int times )
//{
//   
//    unsigned int i, j ; 
//    
//    Pin PinsPDM[]   = {GPIO_PA26, GPIO_PA27}; //faster
//    
//    PinsPDM[PDM_DAT].pio->PIO_OER = PinsPDM[PDM_DAT].mask; //set pin as output
//    PinsPDM[PDM_CLK].pio->PIO_OER = PinsPDM[PDM_CLK].mask;
//    PinsPDM[PDM_DAT].pio->PIO_PPUER = PinsPDM[PDM_DAT].mask; //set pull up 
//    PinsPDM[PDM_CLK].pio->PIO_PPUER = PinsPDM[PDM_CLK].mask;
//
//    
//    PinsPDM[PDM_DAT].pio->PIO_CODR = PinsPDM[PDM_DAT].mask;
//    PinsPDM[PDM_CLK].pio->PIO_CODR = PinsPDM[PDM_CLK].mask;
//    
//    for( i = 0; i < times; i++ ) {
//    
//        for( j = 0; j < (8*PATTERN_WIDTH) ; j++)  {
//              
//              PinsPDM[PDM_CLK].pio->PIO_CODR = PinsPDM[PDM_CLK].mask;
//              if( (data<<j)&0x80 ) {              
//                 PinsPDM[PDM_DAT].pio->PIO_SODR = PinsPDM[PDM_DAT].mask;
//              } else {                 
//                 PinsPDM[PDM_DAT].pio->PIO_CODR = PinsPDM[PDM_DAT].mask;
//              }      
//              PinsPDM[PDM_CLK].pio->PIO_SODR = PinsPDM[PDM_CLK].mask;
//          
//               __asm ("NOP");
//               __asm ("NOP");
//               __asm ("NOP");
//               __asm ("NOP");
//               __asm ("NOP");
//               __asm ("NOP");
//             
//        }
//  
//    }
//    
//   PinsPDM[PDM_DAT].pio->PIO_ODR = PinsPDM[PDM_DAT].mask; //set pin  as input
//   PinsPDM[PDM_CLK].pio->PIO_ODR = PinsPDM[PDM_CLK].mask;
//   PinsPDM[PDM_DAT].pio->PIO_PPUDR = PinsPDM[PDM_DAT].mask; //disable pull up 
//   PinsPDM[PDM_CLK].pio->PIO_PPUDR = PinsPDM[PDM_CLK].mask;
//   
//}
//
//
//void   PDM_Pattern_Gen( INT8U type )
//{
//       
//#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
//    OS_CPU_SR  cpu_sr = 0u;
//#endif
//    
//  
//    OS_ENTER_CRITICAL();
//    
//    if( type == 0 ) {
//        PDM_Out(PATTERN_DATA, REPEAT_TIMES);
//    } else {
//        PDM_Out(IDLE_DATA, 0xFFFFFFFF );
//    }
//  
//    OS_EXIT_CRITICAL();
//  
//}