#ifndef __GPIO_H__
#define __GPIO_H__


#define I2C_MIX_UIF_M        1
#define I2C_MIX_UIF_S        2
#define I2C_MIX_FM36_CODEC   3


typedef struct __PORTS
{                          
    unsigned char porta ;
    unsigned char portc ;
    unsigned char portg ;
    unsigned char portj ;
}PORTS ;

typedef union __GPIOPINSTATE
{
    PORTS   portStt ;     // pin
    unsigned int pinStt ;
}GPIOPIN ;
typedef union __GPIODIRSTATE
{
    PORTS  portDir ;     // dir
    unsigned int pinDir ;
}GPIODIR ;
typedef union __GPIOPORT
{                           
    PORTS  portDat ;     // port
    unsigned int pinDat ; 
}GPIOPORT ;


extern void GPIO_Init(void);
extern void GPIODIR_FLOAT( unsigned int pin  ) ;

extern void GPIODIR_Set(unsigned int pin ) ;
extern unsigned char GPIOPIN_Set(unsigned int pin , unsigned int dat) ;
extern unsigned char  GPIOPIN_Get(unsigned int pin , unsigned char *pdat);
extern void GPIOPIN_Set_Session(unsigned int pin , unsigned int dat) ;

extern void Ruler_PowerOnOff( unsigned char switches ) ;
extern unsigned int Get_Switches( void );
extern unsigned int Get_Port_Detect( void );


extern void UART1_Mixer( unsigned char index ) ;
extern void UART2_Mixer( unsigned char index ) ;
extern unsigned char I2C_Mixer( unsigned char index ) ;
    
extern void Pin_Reset_Codec( void );
extern void Pin_Reset_FM36( void ) ;
extern void Pin_Reset_Audio_MCU( void );
extern void Enable_FPGA( void ) ;
extern void Disable_FPGA( void );
extern void Init_FPGA( unsigned int channels );
extern void Ruler_Power_Switch( unsigned char onoff );

extern void GPIOPIN_Init_Fast( unsigned int pin );
extern void GPIOPIN_Get_Fast( unsigned char pin, unsigned char * pdata );
extern void GPIOPIN_Set_Fast( unsigned char pin , unsigned char data );

extern OS_EVENT *GPIO_Sem_I2C_Mixer;

extern void Config_GPIO_Interrupt( unsigned char gpio_index, CPU_FNCT_VOID isr_handler );
extern void Disable_GPIO_Interrupt( unsigned char gpio_index );
extern unsigned char Check_GPIO_Intrrupt( unsigned char gpio_index );

#endif





