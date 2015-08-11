#include <includes.h>

GPIOPIN   GpioPin  ;
GPIODIR   GpioDir  ;
GPIOPORT  GpioPort ;

unsigned int MonitCount = 0;
unsigned char setdir    = 0;



static Pin pinsGpios[]  = {          
    
    UIF_GPIO_0,     UIF_GPIO_1,      UIF_GPIO_2,     UIF_GPIO_3,     UIF_GPIO_4,
    UIF_GPIO_5,     UIF_GPIO_6,      UIF_GPIO_7,     UIF_GPIO_8,     UIF_GPIO_9,

    GPIO_AUDIO_RST, GPIO_FM_RST,     GPIO_CODEC_RST, //10 - 12

    I2C_SELECT1,    I2C_SELECT2,     I2C_SELECT3,  // 13 - 15

    BUZZER,         LED1,            LED2         //16 -18
                                  
};

//switch pins
static const Pin pinsSwitches[] = {  SW0,  SW1 };


    
    
static const unsigned int numGpios = PIO_LISTSIZE(pinsGpios);


OS_EVENT *GPIO_Sem_I2C_Mixer; //sem for I2C mixer

void GPIO_Init(void)
{  
  
    PIO_Configure( pinsSwitches,  PIO_LISTSIZE(pinsSwitches) );
    PIO_Configure( pinsGpios,     PIO_LISTSIZE(pinsGpios)     );   
 
    //PIO_InitializeInterrupts( AT91C_AIC_PRIOR_LOWEST );     
    //LED_Configure(LED_DS1);
    //LED_Configure(LED_DS2);
    
}



void GPIODIR_FLOAT( unsigned int pin  ) // 
{  
     unsigned int i; 
     
     for(i=0; i<numGpios; i++)
     {       
       if( pin>>i & 0x01 )
       {             
           pinsGpios[i].attribute  = PIO_DEFAULT ; 
           pinsGpios[i].type       = PIO_INPUT   ;            
           PIO_Configure(&pinsGpios[i], 1);
       }
       
     }

   
}

////ranfunc for a faster execution 
//void GPIOPIN_Set(unsigned int pin , unsigned int dat)
//{  
//    unsigned int i; 
//    
//    for( i=0; i < numGpios; i++ ) {       
//        if( (pin >> i) & 0x01 ) { 
//            pinsGpios[i].attribute  = PIO_PULLUP ;                   
//            if( (dat >> i) & 0x01 ) {
//              pinsGpios[i].type    = PIO_OUTPUT_1   ;  
//              
//            } else {
//              pinsGpios[i].type    = PIO_OUTPUT_0   ; 
//              
//            }
//            PIO_Configure(&pinsGpios[i], 1);
//            
//       }
//       
//     } 
//    
//}


//ranfunc for a faster execution 
unsigned char  GPIOPIN_Set(unsigned int pin , unsigned int dat)
{  


    if( pin >= PIO_LISTSIZE(pinsGpios) ) {
        return SET_GPIO_ERR;
        
    }
    
    //APP_TRACE_INFO(("\r\nSet GPIO[%d]=%d ", pin, dat));

    switch ( dat ) {
        
        case 0: 
           pinsGpios[pin].attribute  = PIO_PULLUP ;
           pinsGpios[pin].type       = PIO_OUTPUT_0   ;  
           //PIO_Clear(&pinsGpios[pin]);
           PIO_Configure(&pinsGpios[pin], 1);
        break;
                
        case 1: 
           pinsGpios[pin].attribute  = PIO_PULLUP ;
           pinsGpios[pin].type       = PIO_OUTPUT_1   ;  
           //PIO_Set(&pinsGpios[pin]);
           PIO_Configure(&pinsGpios[pin], 1);
        break;
        
        case 2: 
            pinsGpios[pin].attribute  = PIO_DEFAULT ; 
            pinsGpios[pin].type       = PIO_INPUT   ;            
            PIO_Configure(&pinsGpios[pin], 1);
        break;
        
        default:
            return SET_GPIO_ERR;
        break;      

    }
    
    return 0;
    
    
}


unsigned char  GPIOPIN_Get(unsigned int pin , unsigned char *pdat)
{  


    if( pin >= PIO_LISTSIZE(pinsGpios) ) {
        return SET_GPIO_ERR;
        
    }
       
    *pdat = PIO_Get(&pinsGpios[pin]);       

    
    return 0;
    
    
}


void  __ramfunc GPIOPIN_Set_Fast( unsigned char pin , unsigned char data )
{    
     
    switch( data ) {
        
        case 0 : //output 0
            
            pinsGpios[pin].pio->PIO_CODR = pinsGpios[pin].mask; 
            pinsGpios[pin].pio->PIO_OER  = pinsGpios[pin].mask;
        break;
        
        case 1 : //output 1
            pinsGpios[pin].pio->PIO_SODR = pinsGpios[pin].mask;
            pinsGpios[pin].pio->PIO_OER  = pinsGpios[pin].mask;
        break; 
        
        case 2 : //input        
            pinsGpios[pin].pio->PIO_ODR  = pinsGpios[pin].mask;
        break;
        
        default:
        break;             
   } 
   
}


void  __ramfunc GPIOPIN_Get_Fast( unsigned char pin, unsigned char * pdata )
{
    unsigned int reg ;
    
    reg = pinsGpios[pin].pio->PIO_PDSR;    

    if ((reg & pinsGpios[pin].mask) == 0) {

        *pdata = 0 ;
    }
    else {

        *pdata = 1 ;
    }
    
}


void GPIOPIN_Init_Fast( unsigned int pin )
{
    
    pinsGpios[pin].pio->PIO_IDR = pinsGpios[pin].mask;
    
    //pull up
    pinsGpios[pin].pio->PIO_PPUER = pinsGpios[pin].mask;  //enable
    //pinsGpios[pin].pio->PIO_PPUDR = pinsGpios[pin].mask;
    
    //multi-drive OP
    //pinsGpios[pin].pio->PIO_MDER = pinsGpios[pin].mask;  //enable
    pinsGpios[pin].pio->PIO_MDDR = pinsGpios[pin].mask;
    
    // Enable filter(s)
    pinsGpios[pin].pio->PIO_IFER = pinsGpios[pin].mask;  //enable
    //pinsGpios[pin].pio->PIO_IFDR = pinsGpios[pin].mask;
    
    pinsGpios[pin].pio->PIO_PER = pinsGpios[pin].mask;  
    
}






//// additional time delay :  +10us
//// so, the critical time delay is 11us
void  __ramfunc GPIOPIN_Set_Session( unsigned int pin , unsigned int dat )
{    
    unsigned int i; 
    
    for( i=0; i < 28; i++ ) {  //here 28 is used instead of numGpios for speed up !
      
        if( pin & 0x01<<i ) { 
                            
            if( dat  & 0x01<<i ) {
                //PIO_Set( &pinsGpios[i]);
                pinsGpios[i].pio->PIO_SODR = pinsGpios[i].mask;
            } else {
                //PIO_Clear( &pinsGpios[i]);
                pinsGpios[i].pio->PIO_CODR = pinsGpios[i].mask;              
            }              
        }         
     }     
}



typedef struct __MONITCTR 
{
    unsigned char DataType ;
    unsigned short  DataNum ;
    
}MONITCTR ;


unsigned int GPIOPIN_Read(void)
{
  /*pq
    GpioPin.portStt.porta = GPIO_ReadPin(PA) ;
    GpioPin.portStt.portc = GPIO_ReadPin(PC) ;
    GpioPin.portStt.portg = GPIO_ReadPin(PG) ;
    GpioPin.portStt.portj = GPIO_ReadPin(PJ) ;
    
    return(GpioPin.pinStt & PINMASK) ;
  */
  return 0;
}


void  RecordGpio(
               unsigned char pTime,  /*读取数据的时间间隔,单位为uS*/
               unsigned short  dTime,  /*记录数据时间长度,单位为mS*/
               void *p               /*存放数据位置*/
               )
{
  
    /*pq
    MONITCTR *pMonitCtr = (MONITCTR *)DataBufCtr.pBufTop ;
    unsigned char *pDataSt ;
    unsigned short tdelay ;
    DataBufCtr.pBufTop += 3 ;
    pMonitCtr->DataType = DATA_TYPE_MONIT ;

    pDataSt  = DataBufCtr.pBufTop ;
    MonitCount = 0;
    pinb = 0x76;
    
    TIMER_Open_8bit(0,T_INT_EN|T_PRS_8, pTime-2) ;

    while(dTime)
    {
        tdelay = ((dTime>4000)? 4000:dTime) ;
        dTime -= tdelay ;
        CtrFlage.Time3Over = 0 ;
        
        TIMER_Open_16bit(3,T_INT_EN|T_PRS_1024, tdelay) ;
        
        while(!CtrFlage.Time3Over) ;
    }

    TCCR0B &= 0xf8 ;      // 关闭定时器0
    OCR0A = 0;            
    OCR0B = 0;    
    *(unsigned int *)DataBufCtr.pBufTop = MonitCount ;
    DataBufCtr.pBufTop += 4 ;
    *(DataBufCtr.pBufTop++) = PINB ;
    *(DataBufCtr.pBufTop++) = PINC ;
    *(DataBufCtr.pBufTop++) = PIND ;
      DataBufCtr.pBufTop++ ;
    pMonitCtr->DataNum = (DataBufCtr.pBufTop - pDataSt)/8 ;
  
    */
}





void Ruler_Power_Switch( unsigned char onoff )
{

//    pinsGpios[5].type = (onoff == 0) ? PIO_OUTPUT_0 : PIO_OUTPUT_1 ;
//    PIO_Configure(&pinsGpios[5], 1);   
    
}




//Ruler MCU selector
void UART1_Mixer( unsigned char index )
{      
//    unsigned char i; 
//    //OSTimeDly(1000);
//    if( index<= 3) {      
//        for( i=0; i<=1; i++) {        
//            pinsGpios[i].type = (index & (1<<i) ) == 0 ? PIO_OUTPUT_0 : PIO_OUTPUT_1 ;
//            PIO_Configure(&pinsGpios[i], 1);   
//      
//        }      
//    
//    }   
//    //OSTimeDly(50);
  
}


//Audio MCU selector
void UART2_Mixer( unsigned char index )
{  
    
#ifdef   BOARD_TYPE_AB01   
    unsigned char i; 
  
    if( index<= 3 ) {      
        for( i=2; i<=3; i++) {        
            pinsGpios[12+i].type = (index & (1<<(i-2)) ) == 0 ? PIO_OUTPUT_0 : PIO_OUTPUT_1 ;
            PIO_Configure(&pinsGpios[i], 1);   
      
        }      
    
    }   
#endif  
}



static unsigned char I2C_Mix_Index_Save =  0;  
//I2C bus selector
unsigned char I2C_Mixer( unsigned char index )
{  
#ifdef BOARD_TYPE_UIF 
    
    unsigned char err = 0;
    
    if( index > 3 || index < 1 ) {
        return 1;
    }
    
    OSSemPend( GPIO_Sem_I2C_Mixer, 1000, &err );
    if( OS_ERR_NONE != err ) {  
        APP_TRACE_INFO(( "I2C_Mixer OSSemPend err [0x%X]\r\n", err ));        
        return err;    
    }
    
    if( I2C_Mix_Index_Save == index ) { //no need re-set 
        OSSemPost( GPIO_Sem_I2C_Mixer );
        return err;
    }    
    I2C_Mix_Index_Save = index ;    
    
//    if( index <= 3) {  
//        GPIOPIN_Set_Fast(13, 1);//disable all I2C channels
//        GPIOPIN_Set_Fast(14, 1);//disable all I2C channels
//        GPIOPIN_Set_Fast(15, 1);//disable all I2C channels        
//        GPIOPIN_Set_Fast(12+index, 0);//enable index I2C channels 
//    }
    for( unsigned char i = 1; i <= 3; i++ ) {
        pinsGpios[12+i].type = (index == i) ? PIO_OUTPUT_0 : PIO_OUTPUT_1 ; ////lowe level truned to high after NPN to control switch OE pin
    }
    PIO_Configure(&pinsGpios[13], 3);     
    APP_TRACE_INFO(("\r\nI2C_Mixer switch to: %d ", index ));  
    
    OSSemPost( GPIO_Sem_I2C_Mixer );
    
    //OSTimeDly(1);
#endif
    
}



unsigned int Get_Switches( void )
{
      
    unsigned char i     =   0 ;
    unsigned int  value =   0 ;
  
    for( i=0; i<PIO_LISTSIZE( pinsSwitches ); i++ ) {      
        value <<= 1;
        value +=PIO_Get( &pinsSwitches[i] );
    }
    
    return value;
  
}


//GPIO 0~9 detection  ?/PQ  ??
unsigned int Get_Port_Detect( void )
{
    unsigned char i;
    unsigned int  value =   0 ;   

    for( i=0; i<8; i++ ) {      
        value <<= 1;
        value +=PIO_Get( &pinsGpios[i] );
    }
           
    return value;   

}



//note: turn off and on to update a ruler firmware, hwo about other ruler connections , issue ???
void Ruler_PowerOnOff( unsigned char switches ) 
{
    
//    if( switches == 0 ) { //power off
//        PIO_Clear(&pinsGpios[5]);  
//    } else { //power on
//        PIO_Set(&pinsGpios[5]);  
//    }
     
}

void Pin_Reset_Codec( void )
{

    PIO_Clear(&pinsGpios[12]);
    OSTimeDly(10) ;
    PIO_Set(&pinsGpios[12]);
    OSTimeDly(10) ;
     
}

void Pin_Reset_FM36( void )
{
    
    PIO_Clear(&pinsGpios[11]);
    OSTimeDly(20) ;
    PIO_Set(&pinsGpios[11]);
    OSTimeDly(50) ;
     
}


void Pin_Reset_Audio_MCU( void )
{

    PIO_Clear(&pinsGpios[10]);
    OSTimeDly(10) ;
    PIO_Set(&pinsGpios[10]);
    OSTimeDly(10) ;
     
}

void Pin_Reset_FM1388( void )
{

//    PIO_Clear(&pinsGpios[12]);
//    OSTimeDly(20) ;
//    PIO_Set(&pinsGpios[12]);
//    OSTimeDly(50) ;
     
}



void Enable_FPGA( void )
{
    
    //PIO_Set(&pinsGpios[4]);   
     
}


void Disable_FPGA( void )
{
    
    //PIO_Clear(&pinsGpios[4]);    
     
}



//Note: This routine do NOT support reentrance
//SPI simulation for FPGA control timing requirement
//31 bits control 31 mics on 4 rulers, 1- enable, 0- disable
//channels[7..0]   : ruler0_mic[7..0]
//channels[15..8]  : ruler1_mic[7..0]
//channels[23..16] : ruler2_mic[7..0]
//channels[31..24] : ruler3_mic[7..0]
void Init_FPGA( unsigned int channels )
{
   
//    unsigned int i ;
//    APP_TRACE_DBG(("Init FPGA...[0x%0X] \r\n",channels));
//    PIO_Set(&pinsGpios[20]); //cs 
//    PIO_Set(&pinsGpios[21]); //data 
//    PIO_Set(&pinsGpios[22]); //clock
//    for ( i = 0; i < 32; i++ ) {        
//       PIO_Clear(&pinsGpios[20]); //cs, delay compensation
//       PIO_Clear(&pinsGpios[22]); //clock       
//       if( (channels<<i) & 0x80000000 ) {
//           PIO_Set(&pinsGpios[21]); //data 
//       } else {
//           PIO_Clear(&pinsGpios[21]); //data 
//       }
//       PIO_Set(&pinsGpios[22]); //clock
//    }    
//    PIO_Set(&pinsGpios[20]); //cs 
    
}

