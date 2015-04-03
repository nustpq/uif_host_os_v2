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
*                                     MICRIUM BOARD SUPPORT PACKAGE
*                                         INTERRUPT CONTROLLER
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
*               BSP_INT present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_INT_PRESENT
#define  BSP_INT_PRESENT


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


#ifdef   BSP_INT_MODULE
#define  BSP_INT_EXT
#else
#define  BSP_INT_EXT  extern
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

#define  BSP_INT_SRC_NBR_MAX                        29          /* Interrupt controller maximum source id number        */
#define  BSP_INT_SRC_NBR_TOTAL                      30          /* Total number of sources                              */


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

void         BSP_IntClr           (CPU_DATA       int_id);
void         BSP_IntDis           (CPU_DATA       int_id);
void         BSP_IntDisAll        (void);
void         BSP_IntEn            (CPU_DATA       int_id);
void         BSP_IntInit          (void);
void         BSP_IntVectSet       (CPU_DATA       int_id,
                                   CPU_FNCT_VOID  int_isr_fnct);

/*
*********************************************************************************************************
*                                  INTERRUPT HANDLERS FUNCTION PROTORYPES
*********************************************************************************************************
*/

void         BSP_IntHandlerSUPC   (void);
void         BSP_IntHandlerRSTC   (void);
void         BSP_IntHandlerRTC    (void);
void         BSP_IntHandlerRTT    (void);
void         BSP_IntHandlerWDT    (void);
void         BSP_IntHandlerPMC    (void);
void         BSP_IntHandlerEEFC0  (void);
void         BSP_IntHandlerEEFC1  (void);
void         BSP_IntHandlerUART   (void);
void         BSP_IntHandlerSMC    (void);
void         BSP_IntHandlerPIOA   (void);
void         BSP_IntHandlerPIOB   (void);
void         BSP_IntHandlerPIOC   (void);
void         BSP_IntHandlerUS0    (void);
void         BSP_IntHandlerUS1    (void);
void         BSP_IntHandlerUS2    (void);
void         BSP_IntHandlerUS3    (void);
void         BSP_IntHandlerHSMCI  (void);
void         BSP_IntHandlerTWI0   (void);
void         BSP_IntHandlerTWI1   (void);
void         BSP_IntHandlerSPI0   (void);
void         BSP_IntHandlerSSC0   (void);
void         BSP_IntHandlerTC0    (void);
void         BSP_IntHandlerTC1    (void);
void         BSP_IntHandlerTC2    (void);
void         BSP_IntHandlerPWM    (void);
void         BSP_IntHandlerAD12B  (void);
void         BSP_IntHandlerADC    (void);
void         BSP_IntHandlerDMAC   (void);
void         BSP_IntHandlerUDPHS  (void);



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
