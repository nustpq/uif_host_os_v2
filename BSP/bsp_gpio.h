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
*                                         GENERAL PURPOSE I/O
*
*
* Filename      : bsp_int.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP_GPIO present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_GPIO_PRESENT
#define  BSP_GPIO_PRESENT


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


#ifdef   BSP_GPIO_MODULE
#define  BSP_GPIO_EXT
#else
#define  BSP_GPIO_EXT  extern
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
                                                                /* ---------- PIO CONTROLLER DEFINES  --------------- */                                                                    
                                                                /* ---------------- PIO PORTS DEFINES --------------- */
#define  BSP_GPIO_PORT_A                                  1
#define  BSP_GPIO_PORT_B                                  2
#define  BSP_GPIO_PORT_C                                  3
#define  BSP_GPIO_PORT_D                                  4
#define  BSP_GPIO_PORT_E                                  5

#define  BSP_GPIO_OPT_NONE                       DEF_BIT_NONE
#define  BSP_GPIO_OPT_PIN_EN                     DEF_BIT_00     /* Peripheral is active on the I/O Line               */
#define  BSP_GPIO_OPT_OUT_EN                     DEF_BIT_01     /* Enables the Output in the I/O Line                 */
#define  BSP_GPIO_OPT_FILTER_EN                  DEF_BIT_02     /* Enables the input glitch filter on the I/O line    */
#define  BSP_GPIO_OPT_INT_EN                     DEF_BIT_03     /* Enables input change interrupt on the I/O Line     */
#define  BSP_GPIO_OPT_MULTI_DRV_EN               DEF_BIT_04     /* Enables multi drive on the I/O Line                */
#define  BSP_GPIO_OPT_PULLUP_EN                  DEF_BIT_05     /* Enables Pull-up on the Pin                         */
#define  BSP_GPIO_OPT_PER_SEL_A                  DEF_BIT_06     /* Assigns the I/O line to the peripheral A function  */
#define  BSP_GPIO_OPT_PER_SEL_B                  DEF_BIT_07     /* Assigns the I/O line to the peripheral B function  */
#define  BSP_GPIO_OPT_OUT_WR_EN                  DEF_BIT_08     /* Enables writing PIO_ODSR for the I/O line          */


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

void         BSP_GPIO_Cfg         (CPU_INT08U   gpio_port,
                                   CPU_INT32U   gpio_pin,
                                   CPU_INT16U   gpio_opt);
void         BSP_GPIO_Clr         (CPU_INT08U   gpio_port,
                                   CPU_INT32U   gpio_pin);
CPU_INT32U   BSP_GPIO_StatusGet   (CPU_INT08U   gpio_port);
void         BSP_GPIO_Toggle      (CPU_INT08U   gpio_port,
                                   CPU_INT32U   gpio_pin);
void         BSP_GPIO_Set         (CPU_INT08U   gpio_port,
                                   CPU_INT32U   gpio_pin);
void         BSP_GPIO_IntClr      (CPU_INT08U   gpio_port);


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
