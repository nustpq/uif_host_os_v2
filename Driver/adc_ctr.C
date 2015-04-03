/*
*********************************************************************************************************
*                                           Atmel AT91SAM7A3
*                                                on the
*                                        Saturn Test Plateform REV2.1
*
*                          (c) Copyright 2003-2011; Fortemedia(Nanjing) Co., Ltd
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
*                                         On Chip ADC control related
*
*                                          Atmel AT91SAM7A3
*                                                on the
*                                  Fortemedia SaturnII V2.1 Board
*
* Filename      : adc_trc.c
* Version       : V1.00
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

#include <board.h>
#include <pio.h>
#include <gpio.h>
#include <adc.h>
#include <adc_ctr.h>

#define ADC_CHANNEL_4 4
#define BOARD_ADC_FREQ     1000000

static const Pin pinsADC = PIN_ADC0_ADC4 ; //use adc4



//set 16 to 1 MUX channel
static void Set_ADC_MUX_Channel( unsigned char adc_channel )
{    
    unsigned char i; 
    
    for(i=0; i<4; i++) {
      
        if( adc_channel & (0x01<<i) ) {
           PIO_Set( &pinsControls[2+i] );
           
        } else {
           PIO_Clear( &pinsControls[2+i] );
           
        }        
    }  
  
}



void Init_ADC_Voltage( void )
{  
  
    PIO_Configure( &pinsADC, 1 );
      
    ADC_Initialize(   AT91C_BASE_ADC0,
                      AT91C_ID_ADC,
                      AT91C_ADC_TRGEN_DIS,
                      0,
                      AT91C_ADC_SLEEP_NORMAL_MODE,
                      AT91C_ADC_LOWRES_10_BIT,
                      BOARD_MCK,
                      BOARD_ADC_FREQ,
                      20,
                      2000);

    ADC_EnableChannel(AT91C_BASE_ADC0, ADC_CHANNEL_4);//init adc channel 4
    
}



unsigned int Get_Voltage( unsigned char times, unsigned char adc_channel )
{
    unsigned int total_data = 0;
    unsigned int i ;
    
    
    Set_ADC_MUX_Channel( adc_channel ); //set 16 to 1 MUX channel
    
    for( i=0; i<times; i++ ) {
      
     //   ADC_StartConversion( AT91C_BASE_ADC0 );        
     //   while(!( ADC_GetStatus(AT91C_BASE_ADC0) & AT91C_ADC_EOC4 ) ) ;         
        total_data = total_data + ADC_GetConvertedData(AT91C_BASE_ADC0, ADC_CHANNEL_4);
        
    }   
    
    total_data = total_data / times ;
    //printf("voltage_adc_data = 0x%8X, times:: %d \r\n", total_data,times );
    
    return  total_data;
 
}


  





