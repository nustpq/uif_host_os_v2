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
*                                         GENERAL PURPOSE I/O
*
* Filename      : bsp_gpio.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GPIO_MODULE
#include <bsp.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


#define  BSP_GPIO_PORT_A_BASE                  0x400E0C00
#define  BSP_GPIO_PORT_B_BASE                  0x400E0E00
#define  BSP_GPIO_PORT_C_BASE                  0x400E1000


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

typedef struct BSP_GPIO_REG {
    CPU_REG32	 PIO_PER;                                       /* PIO Enable Register                                */
    CPU_REG32	 PIO_PDR;                                       /* PIO Disable Register                               */
    CPU_REG32	 PIO_PSR;                                       /* PIO Status Register                                */
    CPU_REG32	 RESERVED0;                                     
    CPU_REG32	 PIO_OER;                                       /* Output Enable Register                             */
    CPU_REG32	 PIO_ODR;                                       /* Output Disable Registerr                           */
    CPU_REG32	 PIO_OSR;                                       /* Output Status Register                             */
    CPU_REG32	 RESERVED1;                                     
    CPU_REG32	 PIO_IFER;                                      /* Input Filter Enable Register                       */
    CPU_REG32	 PIO_IFDR;                                      /* Input Filter Disable Register                      */
    CPU_REG32	 PIO_IFSR;                                      /* Input Filter Status Register                       */
    CPU_REG32	 RESERVED2;                                     
    CPU_REG32	 PIO_SODR;                                      /* Set Output Data Register                           */
    CPU_REG32	 PIO_CODR;                                      /* Clear Output Data Register                         */
    CPU_REG32	 PIO_ODSR;                                      /* Output Data Status Register                        */
    CPU_REG32	 PIO_PDSR;                                      /* Pin Data Status Register                           */
    CPU_REG32	 PIO_IER;                                       /* Interrupt Enable Register                          */
    CPU_REG32	 PIO_IDR;                                       /* Interrupt Disable Register                         */
    CPU_REG32	 PIO_IMR;                                       /* Interrupt Mask Register                            */
    CPU_REG32	 PIO_ISR;                                       /* Interrupt Status Register                          */
    CPU_REG32	 PIO_MDER;                                      /* Multi-driver Enable Register                       */
    CPU_REG32	 PIO_MDDR;                                      /* Multi-driver Disable Register                      */
    CPU_REG32	 PIO_MDSR;                                      /* Multi-driver Status Register                       */
    CPU_REG32	 RESERVED3;                                     
    CPU_REG32	 PIO_PPUDR;                                     /* Pull-up Disable Register                           */
    CPU_REG32	 PIO_PPUER;                                     /* Pull-up Enable Register                            */
    CPU_REG32	 PIO_PPUSR;                                     /* Pull-up Status Register                            */
    CPU_REG32	 RESERVED4;                                     
    CPU_REG32	 PIO_ABSR;                                      /* AB Select Status Register                          */
    CPU_REG32	 RESERVED5[3];                                  
    CPU_REG32    PIO_SCIFSR;                                    /* System Clock Clith Input Fileter Select Register   */
    CPU_REG32    PIO_DIFSR;                                     /* Debouncing Input Filter Select Register            */
    CPU_REG32    PIO_IFDGSR;                                    /* Glitch or Debouncing Input Filter Clock Sel Reg    */
    CPU_REG32    PIO_SCDR;                                      /* Slow Clock Divider Debouncing Register             */
    CPU_REG32    RESERVED6[4];
    CPU_REG32	 PIO_OWER;                                      /* Output Write Enable Register                       */
    CPU_REG32	 PIO_OWDR;                                      /* Output Write Disable Register                      */
    CPU_REG32	 PIO_OWSR;                                      /* Output Write Status Register                       */
    CPU_REG32    RESERVED7;
    CPU_REG32    PIO_AIMER;                                     /* Additional Interrupt Modes Enable Register         */
    CPU_REG32    PIO_AIMDR;                                     /* Additional Interrupt Modes Disables Register       */
    CPU_REG32    PIO_AIMMR;                                     /* Additional Interrupt Modes Mask Register           */
    CPU_REG32	 RESERVED8;                                     
    CPU_REG32    PIO_ESR;                                       /* Edge Select Register                               */
    CPU_REG32    PIO_LSR;                                       /* Level Select Register                              */
    CPU_REG32    PIO_ELSR;                                      /* Edge/Level Status Register                         */
    CPU_REG32	 RESERVED9;                                     
    CPU_REG32    PIO_FELLSR;                                    /* Falling Edge/Low Level Select Register             */
    CPU_REG32    PIO_REHLSR;                                    /* Rising Edge/ High Level Select Register            */
    CPU_REG32    PIO_FRLHSR;                                    /* Fall/Rise - Low/High Status Register               */
    CPU_REG32    RESERVED10;                                    
    CPU_REG32    PIO_LOCKSR;                                    /* Lock Status Register                               */
    CPU_REG32    RESERVED11[6];                                 
    CPU_REG32    PIO_VER;                                       /* PIO VERSION REGISTER                               */
    CPU_REG32    RESERVED12[8];                                 
    CPU_REG32    PIO_KER;                                       /* Keypad Controller Enable Register                  */
    CPU_REG32    PIO_KRCR;                                      /* Keypad Controller Row Column Register              */
    CPU_REG32    PIO_KDR;                                       /* Keypad Controller Debouncing Register              */
    CPU_REG32	 RESERVED13;                                    
    CPU_REG32    PIO_KIER;                                      /* Keypad Controller Interrupt Enable Register        */
    CPU_REG32    PIO_KIDR;                                      /* Keypad Controller Interrupt Disable Register       */
    CPU_REG32    PIO_KIMR;                                      /* Keypad Controller Interrupt Mask Register          */
    CPU_REG32    PIO_KSR;                                       /* Keypad Controller Status Register                  */
    CPU_REG32    PIO_KKPR;                                      /* Keypad Controller Key Press Register               */
    CPU_REG32    PIO_KKRR;                                      /* Keypad Controller Key Release Register             */
}  BSP_GPIO_REG, * BSP_GPIO_REG_PTR;


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



/*
**********************************************************************************************************
**********************************************************************************************************
**                                        GLOBAL FUNCTIONS
**********************************************************************************************************
**********************************************************************************************************
*/

/*
*
*********************************************************************************************************
*                                           BSP_GPIO_Cfg()
*
* Description : This function configures the I/O pins in the Parallel Input/Output controller.
*
* Argument(s) : gpio_port           The PIO controller port.
*                                       BSP_GPIO_PORT_A       
*                                       BSP_GPIO_PORT_B       
*                                       BSP_GPIO_PORT_C   
*                                       BSP_GPIO_PORT_D       
*                                       BSP_GPIO_PORT_E
*
*               gpio_pin            Port pin to be configured.
*
*               gpio_cfg_opt        PIO configuration options
*                                       BSP_GPIO_OPT_PER_EN             Peripheral is active on the I/O Line               
*                                       BSP_GPIO_OPT_OUT_EN             Enables the Output in the I/O Line                 
*                                       BSP_GPIO_OPT_FILTER_EN          Enables the input glitch filter on the I/O line    
*                                       BSP_GPIO_OPT_INT_EN             Enables input change interrupt on the I/O Line     
*                                       BSP_GPIO_OPT_MULTI_DRV_EN       Enables multi drive on the I/O Line                
*                                       BSP_GPIO_OPT_PULL_UP_EN         Enables Pull-up on the Pin                         
*                                       BSP_GPIO_OPT_PER_SEL_A          Assigns the I/O line to the peripheral A function  
*                                       BSP_GPIO_OPT_PER_SEL_B          Assigns the I/O line to the peripheral B function  
*                                       BSP_GPIO_OPT_OUT_WR_EN          Enables writing PIO_ODSR for the I/O line          
* Return(s)   : none.
*                               
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_GPIO_Cfg (CPU_INT08U   gpio_port,
                    CPU_INT32U   gpio_pins,     
                    CPU_INT16U   gpio_opt)
{
    BSP_GPIO_REG  *p_pio_ctrl_reg;
    

    p_pio_ctrl_reg = ((BSP_GPIO_REG *)0);
    
    switch (gpio_port) {
        case BSP_GPIO_PORT_A:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)( BSP_GPIO_PORT_A_BASE); 
             break;

        case BSP_GPIO_PORT_B:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)( BSP_GPIO_PORT_B_BASE);         
             break;

        case BSP_GPIO_PORT_C:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)( BSP_GPIO_PORT_C_BASE);         
             break;

        case BSP_GPIO_PORT_D:
        case BSP_GPIO_PORT_E:
             break;             
             
        default:
             break;             
    }
    
    if (p_pio_ctrl_reg == ((BSP_GPIO_REG *)0)) {
        return;
    }
    
    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_PIN_EN)) {
        p_pio_ctrl_reg->PIO_PER  = gpio_pins;
    } else {
        p_pio_ctrl_reg->PIO_PDR  = gpio_pins;    
    }

    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_OUT_EN)) {
        p_pio_ctrl_reg->PIO_OER  = gpio_pins;
    } else {
        p_pio_ctrl_reg->PIO_ODR  = gpio_pins;    
    }

    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_FILTER_EN)) {
        p_pio_ctrl_reg->PIO_IFER = gpio_pins;
    } else {
        p_pio_ctrl_reg->PIO_IFDR = gpio_pins;    
    }
    
    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_INT_EN)) {
        p_pio_ctrl_reg->PIO_IER  = gpio_pins;
    } else {
        p_pio_ctrl_reg->PIO_IDR  = gpio_pins;    
    }

    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_MULTI_DRV_EN)) {
        p_pio_ctrl_reg->PIO_MDER = gpio_pins;
    } else {
        p_pio_ctrl_reg->PIO_MDDR = gpio_pins;    
    }
    
    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_PULLUP_EN)) {
        p_pio_ctrl_reg->PIO_PPUER = gpio_pins;
    } else {
        p_pio_ctrl_reg->PIO_PPUDR = gpio_pins;    
    }
   
    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_PER_SEL_A)) {
        DEF_BIT_CLR(p_pio_ctrl_reg->PIO_ABSR, gpio_pins);
    }
    
    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_PER_SEL_B)) {
        DEF_BIT_SET(p_pio_ctrl_reg->PIO_ABSR, gpio_pins);
    }
    
    if (DEF_BIT_IS_SET(gpio_opt, BSP_GPIO_OPT_OUT_WR_EN)) {
        p_pio_ctrl_reg->PIO_OWER = gpio_pins;
    } else {
        p_pio_ctrl_reg->PIO_OWDR = gpio_pins;    
    }
}


/*
*********************************************************************************************************
*                                           BSP_GPIO_IntClr()
*
* Description : This function clears the interrupt for a specific GPIO
*
* Argument(s) : gpio_port            The PIO controller port.
*                                       BSP_GPIO_PORT_A       
*                                       BSP_GPIO_PORT_B       
*                                       BSP_GPIO_PORT_C       
*                                       BSP_GPIO_PORT_D       
*                                       BSP_GPIO_PORT_E
*
*
* Return(s)   : none.
*                               
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_GPIO_IntClr  (CPU_INT08U   gpio_port)
{
    BSP_GPIO_REG  *p_pio_ctrl_reg;
    CPU_INT32U     isr_status; 


    p_pio_ctrl_reg = ((BSP_GPIO_REG *)0);
          
  
    switch (gpio_port) {
        case BSP_GPIO_PORT_A:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_A_BASE); 
             break;

        case BSP_GPIO_PORT_B:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_B_BASE);         
             break;

        case BSP_GPIO_PORT_C:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_C_BASE);         
             break;

        case BSP_GPIO_PORT_D:
        case BSP_GPIO_PORT_E:
             break;

        default:
             break;             
    }
  
  
    if (p_pio_ctrl_reg == (BSP_GPIO_REG *)0) {
        return;
    }

    isr_status = p_pio_ctrl_reg->PIO_ISR;                       /* Clear interrupt flag.                              */
   (void)&isr_status;
}


/*
*********************************************************************************************************
*                                           BSP_GPIO_Set()
*
* Description : This function sets the data to be driven on the I/O line
*
* Argument(s) : gpio_port           The PIO controller port.
*                                       BSP_GPIO_PORT_A       
*                                       BSP_GPIO_PORT_B       
*                                       BSP_GPIO_PORT_C       
*                                       BSP_GPIO_PORT_D       
*                                       BSP_GPIO_PORT_E
*
*               gpio_pin            Port pin(s) to be set.
*
* Return(s)   : none.
*                               
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_GPIO_Set  (CPU_INT08U   gpio_port,
                     CPU_INT32U   gpio_pin)
{
    BSP_GPIO_REG  *p_pio_ctrl_reg;
    

    p_pio_ctrl_reg = ((BSP_GPIO_REG *)0);
    
   
    switch (gpio_port) {
        case BSP_GPIO_PORT_A:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_A_BASE); 
             break;

        case BSP_GPIO_PORT_B:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_B_BASE);         
             break;

        case BSP_GPIO_PORT_C:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_C_BASE);         
             break;

        case BSP_GPIO_PORT_D:
        case BSP_GPIO_PORT_E:
             break;             
             
        default:
             break;             
    }

    
    if ( p_pio_ctrl_reg == ((BSP_GPIO_REG *)0)) {
        return;
    }
      
     p_pio_ctrl_reg->PIO_SODR = gpio_pin;
    
}


/*
*********************************************************************************************************
*                                           BSP_GPIO_Clr()
*
* Description : This function clears the data to be driven on the I/O line
*
* Argument(s) : gpio_port            The PIO controller port.
*                                       BSP_GPIO_PORT_A       
*                                       BSP_GPIO_PORT_B       
*                                       BSP_GPIO_PORT_C    
*                                       BSP_GPIO_PORT_D       
*                                       BSP_GPIO_PORT_E
*
*
*               gpio_pin             Port pin(s) to be set.
*
* Return(s)   : none.
*                               
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_GPIO_Clr  (CPU_INT08U   gpio_port,
                     CPU_INT32U   gpio_pin)
{
    BSP_GPIO_REG  *p_pio_ctrl_reg;
    

    p_pio_ctrl_reg = ((BSP_GPIO_REG *)0);
          
  
    switch (gpio_port) {
        case BSP_GPIO_PORT_A:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_A_BASE); 
             break;

        case BSP_GPIO_PORT_B:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_B_BASE);         
             break;

        case BSP_GPIO_PORT_C:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_C_BASE);         
             break;

        case BSP_GPIO_PORT_D:
        case BSP_GPIO_PORT_E:
             break;             
                            
        default:
             break;             
    }
  
  
    if (p_pio_ctrl_reg == (BSP_GPIO_REG *)0) {
        return;
    }
    
    p_pio_ctrl_reg->PIO_CODR = gpio_pin;
}


/*
*********************************************************************************************************
*                                           BSP_GPIO_StatusGet()
*
* Description : This function returns the status of the pin(s).
*
* Argument(s) : gpio_port           The PIO controller port.
*                                       BSP_GPIO_PORT_A       
*                                       BSP_GPIO_PORT_B       
*                                       BSP_GPIO_PORT_C       
*                                       BSP_GPIO_PORT_D       
*                                       BSP_GPIO_PORT_E
*
*               gpio_pin            Port pin(s) to be read.
*
* Return(s)   : The status of the pin(s)
*                               
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/
CPU_INT32U  BSP_GPIO_StatusGet  (CPU_INT08U   gpio_port)

{
    BSP_GPIO_REG  *p_pio_ctrl_reg;  
    CPU_INT32U     pin_status;
    

    p_pio_ctrl_reg = ((BSP_GPIO_REG *)0);          
    pin_status     = DEF_BIT_NONE;
    
    
    switch (gpio_port) {
        case BSP_GPIO_PORT_A:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_A_BASE); 
             break;

        case BSP_GPIO_PORT_B:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_B_BASE);         
             break;

        case BSP_GPIO_PORT_C:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_C_BASE);         
             break;

        case BSP_GPIO_PORT_D:
        case BSP_GPIO_PORT_E:
             break;             
                            
        default:
             break;             
    }
    
    
    if (p_pio_ctrl_reg == (BSP_GPIO_REG *)0) {
        return (pin_status);
    }
      
    pin_status = p_pio_ctrl_reg->PIO_PDSR;

    return (pin_status);
}



/*
*********************************************************************************************************
*                                           BSP_GPIO_Toggle()
*
* Description : This function toggle a I/O pin(s)
*
* Argument(s) : gpio_port           The PIO controller port.
*                                      BSP_GPIO_PORT_A       
*                                      BSP_GPIO_PORT_B       
*                                      BSP_GPIO_PORT_C       
*                                      BSP_GPIO_PORT_D       
*                                      BSP_GPIO_PORT_E
*
*               gpio_pin            Port pin(s) to be toggled
*
* Return(s)   : none.
*                               
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_GPIO_Toggle (CPU_INT08U   gpio_port,
                       CPU_INT32U   gpio_pin)
{
    BSP_GPIO_REG  *p_pio_ctrl_reg;
    CPU_INT32U     pin_set;
    CPU_INT32U     pin_clr;
    

    p_pio_ctrl_reg = ((BSP_GPIO_REG *)0);
        
    switch (gpio_port) {
        case BSP_GPIO_PORT_A:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_A_BASE); 
             break;

        case BSP_GPIO_PORT_B:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_B_BASE);         
             break;

        case BSP_GPIO_PORT_C:
             p_pio_ctrl_reg = (BSP_GPIO_REG *)(BSP_GPIO_PORT_C_BASE);         
             break;

        case BSP_GPIO_PORT_D:
        case BSP_GPIO_PORT_E:
             break;             
                            
        default:
             break;             
    }    
    
    if (p_pio_ctrl_reg == (BSP_GPIO_REG *)0) {
        return ;
    }
      
    pin_clr                  =  (p_pio_ctrl_reg->PIO_PDSR) & gpio_pin;    
    pin_set                  = ~(p_pio_ctrl_reg->PIO_PDSR) & gpio_pin;
    p_pio_ctrl_reg->PIO_CODR = pin_clr;
    p_pio_ctrl_reg->PIO_SODR = pin_set;    
    
}
