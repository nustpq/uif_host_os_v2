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
*                                         On current measurement controls
*                                     Driver for ADC AD7799 on SPI interface
*                                          Atmel AT91SAM7A3
*                                                on the
*                                  Fortemedia SaturnII V2.1 Board
*
* Filename      : ad7799.c
* Version       : V1.00
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

#include <board.h>
#include <ucos_ii.h>
#include <pio.h>
#include <spi.h>
#include <gpio.h>
#include "ad7799.h"
#include "timer.h"
 

#define NPCS_AD7799   3 

AT91PS_SPI spi_adc = AT91C_BASE_SPI0 ;


/*
*********************************************************************************************************
*                                           AD7799_CMD_Write()
*
* Description : Write cmd data to AD7799 registers via SPI interface.
*
* Argument(s) : data    Byte need write to AD7799
*
* Return(s)   : None.
* 
* Caller(s)   : 
*
* Note(s)     : None.
*********************************************************************************************************
*/
static void AD7799_CMD_Write( unsigned char data )
{  
  
    SPI_Write( spi_adc, NPCS_AD7799, data );   
    
}


/*
*********************************************************************************************************
*                                           AD7799_CMD_Read()
*
* Description : Read AD7799 registers via SPI interface.
*
* Argument(s) : None. 
*
* Return(s)   : Byte return from AD7799
* 
* Caller(s)   : 
*
* Note(s)     : None.
*********************************************************************************************************
*/
static unsigned char AD7799_CMD_Read( void ) 
{  
    unsigned char ReadData;    
   
    SPI_Write(spi_adc, NPCS_AD7799, 0x00);   
    ReadData = SPI_Read(  spi_adc  );
     
    return ReadData;
}


/*
*********************************************************************************************************
*                                           Wait_For_RDY()
*
* Description : This function use to wait for the the previous operation is done and ADC is ready for next
*               operation.   500ms timeout.
*
* Argument(s) : None. 
*
* Return(s)   : 0  ADC is ready.
*               1  timeout and not ready 
* 
* Caller(s)   : 
*
* Note(s)     : None.
*********************************************************************************************************
*/
static unsigned int Wait_For_RDY(void)
{
    unsigned char status;
    unsigned int counter = 0 ; 
   
    do{      
       AD7799_CMD_Write(0x40);
       status = AD7799_CMD_Read();
       counter++; 
       OSTimeDly(1);
       
    }while ( (status & 0x80) && (counter < 500) );
    
    if(counter >= 500){        
        return 1;
        
    }
    
    return 0;
      
}


/*
*********************************************************************************************************
*                                           Init_AD7799()
*
* Description : Initialize AD7799 for ADC.
*
* Argument(s) : ADC_channel    Choose the ADC channel that need to operate. 
*
* Return(s)   :   =0     Init successfully
*                !=0     errors
*
* Caller(s)   : 
*
* Note(s)     : None.
*********************************************************************************************************
*/
static unsigned int Init_AD7799( unsigned char ADC_channel )
{
    unsigned int status = 0;    
   
    //SW reset
    AD7799_CMD_Write(0xFF);
    AD7799_CMD_Write(0xFF);
    AD7799_CMD_Write(0xFF);
    AD7799_CMD_Write(0xFF);
    OSTimeDly(1); //from spec, need wait at least 500us after reset 
    
    //check status REG
    AD7799_CMD_Write(0x40); 
    status = AD7799_CMD_Read();  
    //printf("\r\nstatus2 = 0x%X\r\n",status);
    if(status != 0x89)  {
      return 0x21;//error
    }    
  
    AD7799_CMD_Write(0x10);
    /* Writes to Communications Register Setting Next Operation as Write to
        CONFIGURATION Register*/      
    AD7799_CMD_Write(0x30);  //pga gain=1, in-amp not used, ADC input range(2.5V)
    AD7799_CMD_Write(0x20 + ADC_channel); //ref detection enabled, unbuffered mode   
 
    /***************** Internal  Calibration ****************/
    AD7799_CMD_Write(0x08);
    /* Writes to Communications Register Setting Next Operation as Write to
        Mode Register*/
    AD7799_CMD_Write(0x80);
    AD7799_CMD_Write(0x0F);
    /* Writes to Mode Register Initiating Internal Zero-Scale Calibration*/
    status = Wait_For_RDY();
    if(status!=0) return status;
    /* Wait for RDY pin to go low to indicate end of calibration cycle*/
   
    AD7799_CMD_Write(0x08);
    /* Writes to Communications Register Setting Next Operation as Write to
        Mode Register*/
    AD7799_CMD_Write(0xa0);
    AD7799_CMD_Write(0x0f);
    /* Writes to Mode Register Initiating Internal Full-Scale Calibration*/
    status = Wait_For_RDY();
    if(status!=0) return status;
    /* Wait for RDY pin to go low to indicate end of calibration cycle*/
    /*************************************************************/

    AD7799_CMD_Write(0x08);
    /* Writes to Communications Register Setting Next Operation as Write to
        Mode Register*/
    AD7799_CMD_Write(0x00);// Continuous-Conversion Mode
    AD7799_CMD_Write(0x0F);
    /* Mode Register[MD2(0),MD1(0),MD0(0),PSW(0),0(0),0(0),0(0),0(0),
     (0),(0),0(0),0(0),FS3(1),FS2(1),FS1(1),FS0(1)]*/
    /*Ä£Ê½0 Continuous-Conversion Mode.£¬Fadc=4.17HZ;*/
    
    return 0 ;
    
}


/*
*********************************************************************************************************
*                                           AD7799_GetConvertedData()
*
* Description : Get AD7799 last convert data
*
* Argument(s) : None.
*
* Return(s)   :  A/D convertion result
* 
* Caller(s)   : 
*
* Note(s)     : None.
*********************************************************************************************************
*/
static unsigned int AD7799_GetConvertedData( void )
{
    unsigned int  ConverData ;
    unsigned char status ; 
    
    status = Wait_For_RDY(); 
    
    if( status != 0 ) {
        return 0;
    }
    
    AD7799_CMD_Write(0x58);
    /* Writes to Communications Register Setting Next Operation as Continuous
        Read From Data Register*/            
    ConverData=0;
    ConverData=AD7799_CMD_Read();
    ConverData<<=8;
    ConverData+=AD7799_CMD_Read();
    ConverData<<=8;
    ConverData+=AD7799_CMD_Read();    
        
    return( ConverData );
}




/*
*********************************************************************************************************
*                                           Init_ADC_Current()
*
* Description : Initialize the ADC for current measurement.
*
* Argument(s) : None.
*
* Return(s)   : None.
* 
* Caller(s)   : 
*
* Note(s)     : None.
*********************************************************************************************************
*/
void Init_ADC_Current( void )
{  
  
  SPI_Init( spi_adc, NPCS_AD7799, 1000000, MCK ) ;   // Configure SPI 
  
  //Init_AD7799(0);
  
}



/*
*********************************************************************************************************
*                                           Get_Current()
*
* Description : Current measurement using AD7799
*
* Argument(s) : channel       Choose the ADC channel that need to operate. 
*                                 channel = 0 : AIN1 : PWR_IN2 : VDDC
*                                 channel = 1 : AIN2 : PWR_IN3 : VDDA
*                                 channel = 2 : AIN3 : PWR_IN1 : VDDIO
*
*               repeat_times   This number used to calculate a average test data
* 
*               *pData         Point to the address the data is stored
*
* Return(s)   :   =0     Init successfully
*                !=0     errors
*
* Caller(s)   : 
*
* Note(s)     : Auto measurement range switching...
*********************************************************************************************************
*/

#define MAX_REPEAT_TIMES   ( 50 )
#define RANGE_LIMIT        ( 19.8/2500 ) * 0xFFFFFF  //range switch point :  0.5mA * 0.36 ohm * 110 = 19.8 mV

unsigned char Get_Current( unsigned char channel, unsigned char repeat_times, unsigned char *pData)
{
    unsigned char status = 0;
    unsigned int i, range_flag ;  
    unsigned int total, average;
    unsigned int adc_result[MAX_REPEAT_TIMES];
    
    if( channel > 2 ) { //channel is limited to 0 ~ 2 
      return 0x22;//error
      
    }      
    range_flag = 0x80000000 ; //0.33 Ohm, larger current range      
    if( repeat_times > MAX_REPEAT_TIMES || repeat_times==0 ) {
      return 0x23; //error
      
    } 
    
    OSTimeDly(100); //delay_ms(100);    ?????
    status = Init_AD7799( channel );
    if(status!=0) {
        return status; //init failed, return ;
        
    }
    
    adc_result[0] = AD7799_GetConvertedData(); //first test data discard
    adc_result[0] = AD7799_GetConvertedData(); 
    
    if(adc_result[0] < RANGE_LIMIT )  {
       range_flag = 0x00000000 ; //220 Ohm, small current range
       PIO_Set( &pinsControls[6+channel] );//switch to 220 Ohm : 0.1uA ~ 100uA range
       OSTimeDly(1000); //delay_ms(1000);
       adc_result[0] = AD7799_GetConvertedData();
    }
    //printf("adc_result[0] = 0x%8X\r\n",adc_result[0] );
    total = adc_result[0];
    for(i=1; i<repeat_times; i++) {
        adc_result[i] = AD7799_GetConvertedData();
        total += adc_result[i];
        //printf("adc_result[%d] = 0x%8X\r\n",i,adc_result[i] );
    }
    
    // reset R to 0.33//220 after test
     // OSTimeDlyHMSM(0,0,3,0); //delay_ms(100);    ?????
    OSTimeDly(2000); //delay_ms(2000);   
    if(range_flag == 0) {
        PIO_Clear( &pinsControls[6+ channel] );//switch to 0.33//220 Ohm : 0.1mA ~ 30mA range 
        
    }
    
    average = total/repeat_times + range_flag;
    *(pData++) = average>>0  & 0xFF;
    *(pData++) = average>>8  & 0xFF;
    *(pData++) = average>>16 & 0xFF;
    *pData     = average>>24 & 0xFF;
    
    //printf("adc_data = 0x%8X\r\n",average );
     
    return status ;
     
}
