/*
*********************************************************************************************************
*                               UIF BOARD APP PACKAGE
*
*                            (c) Copyright 2013 - 2016; Fortemedia Inc.; Nanjing, China
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
*                                        FM36 DSP Based Signal Path Setup
*
*                                          Atmel AT91SAM7A3
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : dsp.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/


#include <includes.h>


//Note: This routine do NOT support reentrance

static bool flag_power_lose       = true;

static unsigned short sr_saved               = 0;
static unsigned char  mic_num_saved          = 0;
static unsigned char  lin_sp_index_saved     = 0;
static unsigned char  start_slot_index_saved = 0;
static unsigned char  bit_length_saved       = 0;



static void Revert_patch_Endien( unsigned int *pData )
{
    
   unsigned char temp, *pChar;    
   
   pChar  = (unsigned char *)pData;   
   temp   = *pChar;
   *pChar = *(pChar+2);
   *(pChar+2) = temp;
   
}



////////////////////////    for FM36-600C on AB03      ////////////////////////

static unsigned int fm36_patch_code_3[] = 
{
    0x003F00,  
    0x3C0065,  
    0x43700A,  
    0x377201,  
    0x17F05E,  
    0x6800A5,  
    0x22629F,  
    0x3C0045,  
    0x43730A,  
    0x377261,  
    0x17F0BE,  
    0x6800A5,  
    0x22629F,  
    0x44000A,  
    0x19E78F,  
    0x81012A,  
    0x877200,  
    0x6800A1,  
    0x977200,  
    0x81013A,  
    0x877220,  
    0x6800A1,  
    0x977220,  
    0x81014A,  
    0x877240,  
    0x6800A1,  
    0x977240,  
    0x81011A,  
    0x19BFCF,  
    0x877210,  
    0x6000A1,  
    0x977210,  
    0x9101DA,  
    0x877230,  
    0x6000A1,  
    0x977230,  
    0x9101EA,  
    0x877250,  
    0x6000A1,  
    0x977250,  
    0x9101FA,  
    0x862450,  
    0x19C27F,  
    0x8A2C72,  
    0x7000AA,  
    0x877260,  
    0x6800A1,  
    0x977260,  
    0x8A2C82,  
    0x7000AA,  
    0x877280,  
    0x6800A1,  
    0x977280,  
    0x810BF1,  
    0x1831DF,  
    0x43F005,  
    0x3C0025,  
    0x3A2D50,  
    0x3B72A2,  
    0x1DC53F,  
    0x43F005,  
    0x19C8CF,  
    0x877270,  
    0x340088,  
    0x6000A1,  
    0x8B72A0,  
    0x7800A2,  
    0x977270,  
    0x877290,  
    0x6000A1,  
    0x8B72B0,  
    0x7800A2,  
    0x977290,  
    0x862550,  
    0x19A30F

};





//parameters for FM36 signal path on AB03
static unsigned short int fm36_para_table_3[][2] = 
{
  //patch settings
  {0x3FA0, 0x9E77},
  {0x3FB0, 0x3F00},
  {0x3FA1, 0x9BFB},
  {0x3FB1, 0x3F0E},
  {0x3FA2, 0x9C26},
  {0x3FB2, 0x3F1C},
  {0x3FA3, 0x831C},
  {0x3FB3, 0x3F2A},
  {0x3FA4, 0x9C8B},
  {0x3FB4, 0x3F36},
  {0x3FA5, 0x9A2F},
  {0x3FB5, 0x3F3D},    

  //////////////////////////////////////////////////////////////
  
  
  ///////////////// SP0 Format  ////////////////// 
  //reset in Config_SPx_Format()
  //{0x2260, 0x78df},//16bit TDM
  //{0x2260, 0x78ff},//32bit TDM
  //{0x2260, 0x7AFF},//32bit TDM,MSB first,Left alignment,8 slot 
  //{0x2260, 0x7AFD},//32bit TDM,MSB first,Left alignment,6 slot 
  // {0x2260, 0x78D9}, //16bit I2S,MSB first,Left alignment,2 slot 
  
  ///////////////// multi-function port  //////////////// 
  {0x2264, 0x01FC}, //use SP0 SP1 PDMI_012 PDMO_012
  
  //////////////////  PDM CLOCK SETTING   /////////////////
  {0x2265, 0x0000}, //ADC clock source, 0: ADC =PLL, 2: ADC=DCA, 3: ADC=1/2 DAC 
  //{0x2266, 0x0013}, //3.072Hz
  {0x2266, 0x001B}, //2.048M
  //{0x2266, 0x0033},//1.024

  //ADC
  //{0x2267, 0xBB80}, //3.072Hz
  {0x2267, 0x7d00}, //2.048M //clock to iM401
  //{0x2267, 0x3e80},//1.024

   //DAC
   //{0x226A, 0xBB80}, //3.072Hz
   //{0x226A, 0x7d00}, //2.048M 
   {0x226A, 0x3e80},//1.024   //clock from iM401
   
   ///////////////// SP1 Format  //////////////////
   //reset in Config_SPx_Format()
  //{0x2261, 0x78FF},//32bit TDM,MSB first,Left alignment,8 slot, Left alignment of word inside slot to meet AD1937
  //{0x2260, 0x78D9}, //16bit I2S,MSB first,Left alignment,2 slot 
  //{0x2268, 0xBB80}, 

  {0x2288, 0x0000},
  {0x2289, 0x7FFF},
  {0x2290, 0x7FFF}, 
  {0x22FD, 0x00DE}, //enable ADC/DAC interrupt
  //{0x2274, 0x0001},//mic revert  
  //{0x2303, 0x8000},
  //{0x2304, 0x4000},
  //{0x2305, 0x0000},
  {0x22FC, 0x8000}, //BYPASS ALL SIGNAL PORCESSING
  {0x226E, 0x000C}, //PLL 24.576MHz OSC 
  
  ////////////////////////////////////////////////////////////
  
  //select data interruption source :  
  // 0: PDM, 1: SP0, 2: SP1, 3: SP2
  {0x22B2, 0x0001},
  {0x22B3, 0x0001},
  {0x22B4, 0x0001},  
    //additional, input
    ////229A = 0x1 //Aux-in From SP0
    ////229B = 0x0 //Aux-in-L in slot0 
    ////229C = 0x1 //Aux-in-R in slot1
    //// 
    //////output, aux2 output same as aux1
    ////22C7 = 0x1018 //Aux-in-L
    ////22C8 = 0x1019 //Aux-in-R
    ////22B4 = 1 //SP0
    ////22D5 = 6 //slot6 
    ////22D6 = 7 //slot7  
  //lin source 
  {0x229A, 0x0002}, //Aux-in From SP1
  {0x229B, 0x0008}, //Aux-in-L in SP1 slot0 
  {0x229C, 0x0009}, //Aux-in-R in SP1 slot1
  
  //select output data source slot
  {0x22C1, 0x101A},
  {0x22C2, 0x101B},
  {0x22C3, 0x101C},
  {0x22C4, 0x101D},
  {0x22C5, 0x101E},
  {0x22C6, 0x101F},   
  {0x22C7, 0x1018}, //Aux-in-L
  {0x22C8, 0x1019}, //Aux-in-R
//  {0x22C7, 0x1020}, //default 0
//  {0x22C8, 0x1020}, //default 0
  
  //select data dest slot
  //If lineout is from TX0, offset is 0~7
  //If lineout is from TX1, offset is 8~15
  //If lineout is from TX2, offset is 16~23
  //If offset = 0x8000, means Zero output.
  {0x22D7, 0x0000},
  {0x22D8, 0x0001},
  {0x22D9, 0x0002},
  {0x22DA, 0x0003},
  {0x22DB, 0x0004},
  {0x22DC, 0x0005},  
  {0x22D5, 0x0006}, //slot6 
  {0x22D6, 0x0007}, //slot7
  
  //mic souce
  {0x2282, 0x0000},
  {0x2283, 0x0001},
  {0x2284, 0x0002},
  {0x2285, 0x0003},
  {0x2286, 0x0004},
  {0x2287, 0x0005},
  
  //set PDM out data
  {0x22B9, 0x3F40},
  {0x22BA, 0x3F42},
  {0x22BB, 0x3F44},
  {0x22BC, 0x3F46},
  {0x22BD, 0x3F48},
  {0x22BE, 0x3F4A}, 

  //{0x22B3, 0}, 
  {0x22FA, 0xFF}, 
  
  {0x22E5, 0x20}, //PDM DAC CLOCK As Input
  {0x22EB, 0x0006}, //Actual MIC number in system.
  
  {0x22F1, 0xD800}, //pwd reset mode., enable pwd bypass
  
  {0x22FB, 0 },  //run flag
  
  //{0x3FCF, 0x020},//PDMDAC CLOCK As Input
  
  /*******************  MIC Volume Control  ****************
  Volume setting [7:4]:
  0x0 ( 0 dB) 0x4 (-48 dB) 0x8 (-96 dB) 0xc (Not allowed)
  0x1 (-12 dB) 0x5 (-60 dB) 0x9 (-108 dB) 0xd (Not allowed)
  0x2 (-24 dB) 0x6 (-72 dB) 0xa (-120 dB) 0xe (+24 dB)
  0x3 (-36 dB) 0x7 (-84 dB) 0xb (-132 dB) 0xf (+12 dB)
  Volume setting [3:0]:
  0x0 ( 0 dB) 0x4 (-3.00 dB) 0x8 (-6.00 dB) 0xc (-9.00 dB)
  0x1 (-0.75 dB) 0x5 (-3.75 dB) 0x9 (-6.75 dB) 0xd (-9.75 dB)
  0x2 (-1.50 dB) 0x6 (-4.50 dB) 0xa (-7.50 dB) 0xe (-10.50 dB)
  0x3 (-2.25 dB) 0x7 (-5.25 dB) 0xb (-8.25 dB) 0xf (-11.25 dB)
  Real volume = Volume for [7:4] + Volume for [3:0]
  **********************************************************/  
  //{0x3FC6, 0x0000}, //MIC0,1 Volume Gain = 0dB
  //{0x3FC7, 0x0000}, //MIC2,3 Volume Gain = 0dB
  //{0x3FC8, 0x0000}, //MIC4,5 Volume Gain = 0dB
  
  /**************** MEMS Gain Control **********************/
  //{0x3F91, 0x0000}, //MEMS MIC TYPE, SW default setting as MEMS type
  //{0x3F92, 0x0000} //MEMS MIC gain = 0dB
  {0x3F92, 0x0555} //MEMS MIC gain = 6dB
  //{0x3F92, 0x0AAA} //MEMS MIC gain = 12dB //SW default setting
  //{0x3F92, 0x0FFF} //MEMS MIC gain = 18dB

} ;

//DMIC 4 steps PGA gain
unsigned char DMIC_PGA_Control( unsigned short gain ) 
{
    unsigned char  err  ;
    unsigned short data ;
    unsigned short mute ;
    
    mute = 0x3F;
    err = NO_ERR;
    
    switch ( gain ){
        case 0 : 
            data = 0x0000;
        break;
        case 6 : 
            data = 0x0555;
        break;
        case 12 : 
            data = 0x0AAA;
        break;
        case 18 : 
            data = 0x0FFF;
        break;
        case 1000 :
            mute = 0x3F3F;
        break;
        default:
            return FM36_DMIC_PGA_GAIN_ERR;
    }  
    
    if( mute == 0x3F ) {
      err = DM_SingleWrite( FM36_I2C_ADDR, 0x3F92, data ) ;
      if( OS_ERR_NONE != err ) {
          return FM36_WR_DM_ERR;;
      }   
    }    
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x3F96, mute ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }    
    
    return err;
    
}


//Setup External Lin data input source
//From : SP0_RX, SP1_RX
static unsigned char Config_Lin( unsigned char lin_sp_index, unsigned char start_slot_index )
{
    
    unsigned char err ;
    
    APP_TRACE_INFO(("\r\nConfig_Lin sp_index = %d, start_slot_index = %d\r\n", lin_sp_index, start_slot_index));
    
    //Aux-Source
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x229A, lin_sp_index+1 ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }
    
    //Aux-in-L
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x229B, (lin_sp_index<<3) + start_slot_index ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }
    
    //Aux-in-R
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x229C, (lin_sp_index<<3) + start_slot_index + 1 ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }    
   
    return err;
    
}

// Set additional data besides MIC data
// M0..M5 + X0 + X1
// Max 8 slots
// Make sure last 2 slots folowed mic data closely
static unsigned char Config_SP0_Out( unsigned char mic_num )
{
    
    unsigned char err ;
    
    APP_TRACE_INFO(("\r\nConf FM36 Mic num = %d\r\n", mic_num));
    
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x22EB, mic_num ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }  
    
    //select output data source slot
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x22C1 + mic_num, 0x1018 ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }       
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x22C2 + mic_num, 0x1019 ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }     
  
    return err;
    
}


static unsigned char Config_SPx_Format( unsigned char bit_length )
{
    
    unsigned char  err ;
    unsigned short temp;
    
    APP_TRACE_INFO(("\r\nConf FM36 Bit length = %d\r\n", bit_length));
    
    if( bit_length == 32 ) {          
          temp = 0x78FF;//32bit TDM, 16bit data, MSB first,Left alignment,8 slot 
    } else {
          temp = 0x78DF;//16bit TDM,16bit data, MSB first,Left alignment,8 slot 
    }
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x2260, temp ) ; //SP0
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }       
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x2261, temp ) ; //SP1
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    } 
    
    return err;
    
}   



static unsigned char Config_SR( unsigned short sr )
{
    
    unsigned char err ;
    unsigned short temp ;
    
    APP_TRACE_INFO(("\r\nConf FM36 SR = %dkHz\r\n", sr));
      
    switch ( sr ) {
        case 8000 :
            temp = 0x1F40;
        break;        
        case 16000 :
            temp = 0x3E80;
        break;
        case 22050 :
            temp = 0x5622;
        break;
        case 24000 :
            temp = 0x5DC0;
        break;
        case 32000 :
            temp = 0x7D00;
        break;
        case 44100 :
            temp = 0xAC44;
        break;
        default: //48000 :
            temp = 0xBB80;
        break;   
    }
    
    //select output data source slot
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x2268, temp ) ; //Output Clock for ADC DPLL, PDM input sampling rate.
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }       
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x2269, temp ) ;// reference Clock for DAC DPLL,PDM output sampling rate.
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }     
  
    return err;
    
}  

unsigned char FM36_PWD_Bypass( void )
{
    
    unsigned char  err ;    
    APP_TRACE_INFO(("\r\nPower down FM36 to bypass SP0<-->SP1\r\n"));
     
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x3FEF, 0x2000 ) ; //pwd
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }  
    
    return err;
    
}  

unsigned char FM36_PDMADC_CLK_Onoff( unsigned char onoff )
{
    unsigned char  err ;  
    
    APP_TRACE_INFO(("\r\nEnable/Disbale FM36 ADC PDM CLK: %d",onoff));    

    if( onoff ) {  //Normal operation 
        
        err = DM_SingleWrite( FM36_I2C_ADDR, 0x3FCF, 0x20 ) ;  //turn on clk
        if( OS_ERR_NONE != err ) {
            return FM36_WR_DM_ERR;
        }          
        err = DM_SingleWrite( FM36_I2C_ADDR, 0x3F90, 0 ) ;  //power up MIC0-5
        if( OS_ERR_NONE != err ) {
            return FM36_WR_DM_ERR;
        }  
        OSTimeDly(5);  //depop time
        err = DM_SingleWrite( FM36_I2C_ADDR, 0x3F96, 0x3F ) ;  //unmute MIC0-5
        if( OS_ERR_NONE != err ) {
            return FM36_WR_DM_ERR;
        }  
        
    } else {    //PDMCLK_OFF
        
        err = DM_SingleWrite( FM36_I2C_ADDR, 0x3F96, 0x3F3F ) ;  //mute MIC0-5
        if( OS_ERR_NONE != err ) {
            return FM36_WR_DM_ERR;
        }     
        OSTimeDly(5); //wait data 0 to cyclebuffer
        err = DM_SingleWrite( FM36_I2C_ADDR, 0x3F90, 0x3F ) ;  //power down MIC0-5 
        if( OS_ERR_NONE != err ) {
            return FM36_WR_DM_ERR;
        } 
        err = DM_SingleWrite( FM36_I2C_ADDR, 0x3FCF, 0x24 ) ;  //turn off clk
        if( OS_ERR_NONE != err ) {
            return FM36_WR_DM_ERR;
        } 
        
    }
    
    return err;
    
}  

unsigned char FM36_PDMADC_CLK_Set( unsigned char pdm_clk_mhz )
{
    
    unsigned char  err ;
    unsigned short data1,data2,data3 ;
    
    APP_TRACE_INFO(("\r\nConf FM36 PDMADC Clock = %dMHz\r\n", pdm_clk_mhz));
      
    switch ( pdm_clk_mhz ) {
        case 4 : //4.096
            data3 = 0x0F;
            data2 = 0x7D80;
            data1 = 0xFA00;
        break;  
        case 3 : //3.072
            data3 = 0x13;
            data2 = 0x3E80;
            data1 = 0xBB80;
        break;
        case 2 : //2.048
            data3 = 0x1B;
            data2 = 0x3E80;
            data1 = 0x7D00;
        break;
        case 1 : //1.024
            data3 = 0x33;
            data2 = 0x3E80;
            data1 = 0x3E80;
        break; 
        default ://2.048
            APP_TRACE_INFO(("Not supported PDM CLK, reset to default 2.048MHz\r\n"));
            data3 = 0x1B;
            data2 = 0x3E80;
            data1 = 0x7D00;
        break;
    }
    
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x3FD5, data1 ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x3FCD, data2 ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x3FED, data3 ) ;
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }
    
    err = DM_LegacyRead( FM36_I2C_ADDR, 0x3FE4,(unsigned char *)&data1 ) ;
    if( OS_ERR_NONE != err ) {
        err = FM36_RD_DM_ERR;
        return err ;
    }     
    err = DM_SingleWrite( FM36_I2C_ADDR, 0x3FE4, data1) ;// must rewrite 3fe4 to setting PLL.
    if( OS_ERR_NONE != err ) {
        return FM36_WR_DM_ERR;;
    }     
  
    return err;
    
}

/*
*********************************************************************************************************
*                                       Init_FM36_AB03()
*
* Description : Initialize FM36 DSP on AB03 board.
* Argument(s) : sr        : sample rate : 8000 ~ 48000 
*               bit_length: 16bit/32bit mode
*               mic_num   : 0~6
*               lin_sp_index  : line in data source: 0 ~ 1
*               start_slot_index: line in data slot: 0 ~ 7
* Return(s)   : NO_ERR :   execute successfully
*               others :   =error code .  
*
* Note(s)     : None.
*********************************************************************************************************
*/
unsigned char Init_FM36_AB03( unsigned short sr, unsigned char mic_num, unsigned char lin_sp_index, unsigned char start_slot_index, unsigned char bit_length )
{
    unsigned int   i;
    unsigned short temp, temp2 ;
    unsigned short addr, val; 
    unsigned char  err ;     
   
    if( sr == sr_saved  &&  \
        mic_num == mic_num_saved && \
        lin_sp_index == lin_sp_index_saved && \
        start_slot_index == start_slot_index_saved && \
        bit_length == bit_length_saved  ) {    
        APP_TRACE_INFO(("No need Re-Init FM36\r\n"));
        return NO_ERR;        
    } else {
        sr_saved = sr ;
        mic_num_saved = mic_num ;
        lin_sp_index_saved = lin_sp_index;
        start_slot_index_saved = start_slot_index ;
        bit_length_saved = bit_length;
    }   
    
    Pin_Reset_FM36();  
    
//    err = HOST_SingleWrite_2(FM36_I2C_ADDR, 0x0C, 2); //reset
//    if( OS_ERR_NONE != err ) {
//        return FM36_WR_HOST_ERR;
//    }    
//    err = DM_LegacyRead( FM36_I2C_ADDR, 0x22FB,(unsigned char *)&temp ) ;
//    if( OS_ERR_NONE != err ) {
//        err = FM36_RD_DM_ERR;
//        return err ;
//    }  
//    APP_TRACE_INFO(("0x22FB = 0x%X\r\n", temp));     
//    err = DM_LegacyRead( FM36_I2C_ADDR, 0x2306,(unsigned char *)&temp ) ;
//    if( OS_ERR_NONE != err ) {
//        err = FM36_RD_DM_ERR;
//        return err ;
//    }  
//    APP_TRACE_INFO(("0x2306 = 0x%X\r\n", temp));     
//
//    err = HOST_SingleWrite_2(FM36_I2C_ADDR, 0x10, 0); //select FM36 DSP Core 0
//    if( OS_ERR_NONE != err ) {
//        return FM36_WR_HOST_ERR;
//    }
    
    //check chip type by rom id 
    err = CM_LegacyRead( FM36_I2C_ADDR, 0x2FFF,(unsigned char *)&temp ) ;
    if( OS_ERR_NONE != err ) {
        err = FM36_RD_CM_ERR;
        return FM36_RD_CM_ERR ;
    }
    APP_TRACE_INFO(("ROM ID = 0x%X\r\n", temp));
    
    if( temp != FM36_ROM_ID_2 ) { 
        return FM36_CHECK_ROMID_ERR; //dsp type error
    }    
    
    //load FM36-600C patch code
    if( flag_power_lose ) { //only need download patch once after one power cycle            
        flag_power_lose = false;
        addr = (unsigned short)fm36_patch_code_3[0];
        for( i = 1; i < sizeof(fm36_patch_code_3)/4; i++) { 
            Revert_patch_Endien(&fm36_patch_code_3[i]);    
            err = PM_SingleWrite( FM36_I2C_ADDR, addr++, (unsigned char*)&fm36_patch_code_3[i],0 ) ;
              if( OS_ERR_NONE != err ) {
                  err = FM36_WR_PM_ERR;
                  return err ;
              }         
        }
    } 
    
    //load parameters
    for( i = 0; i < sizeof(fm36_para_table_3)/4; i++) {            
        addr = fm36_para_table_3[i][0];
        val  = fm36_para_table_3[i][1];
        
        if( addr == 0x22FB ) {  //if run chip, do sth before            
            err = Config_SP0_Out( mic_num );
            if( OS_ERR_NONE != err ) {
                return err ;
            }
            err = Config_Lin( lin_sp_index, start_slot_index );
            if( OS_ERR_NONE != err ) {
                return err ;
            }
            err = Config_SPx_Format( bit_length );
            if( OS_ERR_NONE != err ) {
                return err ;
            }
            err = Config_SR( sr );
            if( OS_ERR_NONE != err ) {
                return err ;
            }            
        } 
        
        err = DM_SingleWrite( FM36_I2C_ADDR, addr, val ) ;
        if( OS_ERR_NONE != err ) {
            err = FM36_WR_DM_ERR;
            return err ;
        }
        if( addr == 0x22FB ) {  //if run chip, delay
            OSTimeDly(100);            
        } 
    } 
          
    //check running status
    err = DM_LegacyRead( FM36_I2C_ADDR, 0x2306,(unsigned char *)&temp2 ) ;
    if( OS_ERR_NONE != err ) {
        err = FM36_RD_DM_ERR;
        return err ;
    }
    APP_TRACE_INFO(("0x2306 = 0x%X\r\n", temp2));
    OSTimeDly(100);
    err = DM_LegacyRead( FM36_I2C_ADDR, 0x22FB,(unsigned char *)&temp ) ;
    if( OS_ERR_NONE != err ) {
        err = FM36_RD_DM_ERR;
        return err ;
    }
    APP_TRACE_INFO(("0x22FB = 0x%X\r\n", temp));
    if( temp != 0x5A5A ) {   
        return FM36_CHECK_FLAG_ERR;
    }
    err = DM_LegacyRead( FM36_I2C_ADDR, 0x2306,(unsigned char *)&temp ) ;
    if( OS_ERR_NONE != err ) {
        err = FM36_RD_DM_ERR;
        return err ;
    }
    APP_TRACE_INFO(("0x2306 = 0x%X\r\n", temp));
    if( temp == temp2 ) {
        APP_TRACE_INFO(("FM36 frame counter stopped !"));
        return FM36_CHECK_COUNTER_ERR;
    } 
    
    if( Global_UIF_Setting[ UIF_TYPE_FM36_PATH - 1 ].attribute == ATTRI_FM36_PATH_PWD_BP ) {
        FM36_PWD_Bypass();
        
    }
    
    return err;
    
}



/**********************************   For iM401 control **********************/
/*
static unsigned char iM401_Standby_Cmd[][3]=
{
    0x91,0x00,0x00,
    0x61,0x8b,0x81,
    0x60,0x00,0xc0,
    0x90,0x00,0x00,
    0x65,0x00,0x05,
    0x65,0x00,0x04,
    0x57,0x0f,0xff,
    0x5d,0x80,0x00,
    0x82,0x07,0x04,  
    0xc1,0x22,0x02,
    0x65,0x00,0x01,
    0x65,0x00,0x19,
    0x65,0x00,0x18,
    0x64,0x00,0x01,
    0x5c,0x00,0x3a,
    0x5d,0x00,0x00,
    0x50,0x11,0xc0,
    0x54,0x80,0x00,
    0x54,0x00,0x00,
    0x46,0x00,0x90,
    0x2a,0x8a,0x2f
};


static unsigned char iM401_Bypass_Cmd[][3]=
{
   {0x8A,  0x00,  0x22},
   {0xC0,  0x00,  0x40},
   {0xC1,  0x22,  0x02},
   {0x80,  0x0E,  0x06},
   {0x81,  0x00,  0xD3},
   {0x82,  0x07,  0x04},
   {0x73,  0x80,  0x40},
   {0x74,  0x02,  0x00},
   {0x83,  0x00,  0x00},
   {0x84,  0x01,  0xF4},
   {0x8A,  0xC0,  0x22},
   {0x91,  0x00,  0x00},
   {0x61,  0x8B,  0x81},
   {0x60,  0x00,  0xC0},
   {0x90,  0x00,  0x00},
   {0x65,  0x00,  0x05},
   {0x65,  0x00,  0x04},
   {0x5D,  0x80,  0x00},
   {0x82,  0x07,  0x04},
   {0xC1,  0x22,  0x02},
   {0x65,  0x00,  0x01},
   {0x65,  0x00,  0x19},
   {0x65,  0x00,  0x18},
   {0x64,  0x00,  0x01},
   {0x5C,  0x00,  0x3a},
   {0x5D,  0x00,  0x00},
   {0x50,  0x11,  0xC0},
   {0x54,  0x80,  0x00},
   {0x54,  0x00,  0x00},
   {0x46,  0x00,  0x90},
   {0x2A,  0x8A,  0x10},
   {0x82,  0x07,  0x01},
   {0x2D,  0x6A,  0xAF},
   {0x2D,  0x6E,  0xAF},
   {0x64,  0x00,  0x03},
   {0x46,  0x00,  0x91},
   {0x91,  0x00,  0x00}
};



unsigned char iM401_Standby( void )
{
    
    unsigned char  i, state ;    
    APP_TRACE_INFO(("\r\nSend CMD to enter iM401 standby mode...\r\n"));
    
    for( i = 0; i < sizeof(iM401_Standby_Cmd)/3; i++ ) { 
        state =  TWID_Write(  iM401_I2C_ADDR>>1, 0, 0, iM401_Standby_Cmd[i], 3, NULL);     
        if ( state != SUCCESS )  {
            return(I2C_BUS_ERR) ;
        } 
    }       
    return 0;
    
}   


unsigned char iM401_Bypass( void )
{
    
    unsigned char  i, state ;    
    APP_TRACE_INFO(("\r\nSend CMD to enter iM401 bypass mode...\r\n"));
    
    for( i = 0; i < sizeof(iM401_Bypass_Cmd)/3; i++ ) { 
        state =  TWID_Write(  iM401_I2C_ADDR>>1, 0, 0, iM401_Bypass_Cmd[i], 3, NULL);     
        if ( state != SUCCESS )  {
            return(I2C_BUS_ERR) ;
        }
    }       
    return 0;
    
}
*/
////////////////////////////////////////////////////////////////////////////////




unsigned char iM401_Load_Vec( void )
{     
    unsigned char err;
    unsigned char i, index, *pChar;  
    FLASH_INFO    flash_info;   
    
    err = NO_ERR;
    APP_TRACE_INFO(("\r\nLoad iM401 vec stored in MCU flash...\r\n"));    
   
    if( Global_VEC_Cfg.flag != 0x55 ) {
        APP_TRACE_INFO(("\r\nvec conf data error!\r\n")); 
        err = FW_VEC_SET_CFG_ERR;
        return err;
    }
        
    index = Global_VEC_Cfg.vec_index_a ;
    if( index != 0 ) {        
        Read_Flash_State(&flash_info, FLASH_ADDR_FW_VEC_STATE + AT91C_IFLASH_PAGE_SIZE * index  );
        if( flash_info.f_w_state != FW_DOWNLAD_STATE_FINISHED ) {
          err = FW_VEC_SAVE_STATE_ERR;
          APP_TRACE_INFO(("\r\nvec data state error...\r\n"));
          return err;
        }       
        APP_TRACE_INFO(("Load vec[%d] (%d Bytes) ...\r\n",index,flash_info.bin_size));
        pChar = (unsigned char *)FLASH_ADDR_FW_VEC + index * FLASH_ADDR_FW_VEC_SIZE;
        for( i = 0; i < flash_info.bin_size / 3; i++ ) { 
            err =  TWID_Write(  iM401_I2C_ADDR>>1, 0, 0, pChar + i*3 , 3, NULL);     
            if ( err != SUCCESS )  {
                return(I2C_BUS_ERR) ;
            }
        } 
    } 
    
    OSTimeDly( Global_VEC_Cfg.delay );

    index = Global_VEC_Cfg.vec_index_b ;
    if( index != 0 ) {        
        Read_Flash_State(&flash_info, FLASH_ADDR_FW_VEC_STATE + AT91C_IFLASH_PAGE_SIZE * index );
        if( flash_info.f_w_state != FW_DOWNLAD_STATE_FINISHED ) {
          err = FW_VEC_SAVE_STATE_ERR;
          APP_TRACE_INFO(("\r\nvec data state error...\r\n"));
          return err;
        }       
        APP_TRACE_INFO(("Load vec[%d] (%d Bytes) ...\r\n",index,flash_info.bin_size));
        pChar = (unsigned char *)FLASH_ADDR_FW_VEC + index * FLASH_ADDR_FW_VEC_SIZE;
        for( i = 0; i < flash_info.bin_size / 3; i++ ) { 
            err =  TWID_Write(  iM401_I2C_ADDR>>1, 0, 0, pChar + i*3 , 3, NULL);     
            if ( err != SUCCESS )  {
                return(I2C_BUS_ERR) ;
            }
        } 
    } 
    
    return err;     
    
}


unsigned char MCU_Load_Vec( unsigned char firsttime )
{     
    unsigned char err;
    unsigned char i, index, *pChar; 
    unsigned char dev_addr, data_num;
    FLASH_INFO    flash_info;   
    
    err = NO_ERR;
    
    if( Global_VEC_Cfg.type == 41 ) {
        APP_TRACE_INFO(("\r\nLoad iM401 vec stored in MCU flash...\r\n"));
        dev_addr = iM401_I2C_ADDR;        
    } else if(Global_VEC_Cfg.type == 51 ) {
        APP_TRACE_INFO(("\r\nLoad iM501 vec stored in MCU flash...\r\n"));
        dev_addr = iM501_I2C_ADDR;       
    } else {
        APP_TRACE_INFO(("\r\nvec conf data error!\r\n")); 
        return FW_VEC_SET_CFG_ERR;         
    }
    
    if( Global_VEC_Cfg.flag != 0x55 ) {
        APP_TRACE_INFO(("\r\nvec conf data error!\r\n")); 
        return FW_VEC_SET_CFG_ERR;
    }
    
    if( firsttime == 0 ) {   //not first time pwd
        
        index = Global_VEC_Cfg.vec_index_a ; //Power up
        if( Global_VEC_Cfg.type == 51 ) {
            I2C_Mixer(I2C_MIX_FM36_CODEC);
            FM36_PDMADC_CLK_Onoff(1); //enable PDM clock    
            I2C_Mixer(I2C_MIX_UIF_S); 
        }
        if( index != 0 ) {        
            Read_Flash_State(&flash_info, FLASH_ADDR_FW_VEC_STATE + AT91C_IFLASH_PAGE_SIZE * index  );
            if( flash_info.f_w_state != FW_DOWNLAD_STATE_FINISHED ) {    
              APP_TRACE_INFO(("\r\nvec data state error...\r\n"));
              return FW_VEC_SAVE_STATE_ERR;;
            }   
            APP_TRACE_INFO(("Load vec[%d] (%d Bytes) ...\r\n",index,flash_info.bin_size));
            pChar = (unsigned char *)FLASH_ADDR_FW_VEC + index * FLASH_ADDR_FW_VEC_SIZE;
            for( i = 0; i < flash_info.bin_size ;  ) {
                data_num = *(pChar+i) ;
                err =  TWID_Write(  dev_addr>>1, 0, 0, pChar + i + 1 , data_num, NULL); 
                i += ( data_num + 1 );               
                if ( err != SUCCESS )  {
                    return(I2C_BUS_ERR) ;
                }
            } 
        }  
        
        OSTimeDly( Global_VEC_Cfg.delay );  //Delay mSecond
        
    }
    
    index = Global_VEC_Cfg.vec_index_b ; // Power down
    if( index != 0 ) {        
        Read_Flash_State(&flash_info, FLASH_ADDR_FW_VEC_STATE + AT91C_IFLASH_PAGE_SIZE * index );
        if( flash_info.f_w_state != FW_DOWNLAD_STATE_FINISHED ) {
          err = FW_VEC_SAVE_STATE_ERR;
          APP_TRACE_INFO(("\r\nvec data state error...\r\n"));
          return err;
        }       
        APP_TRACE_INFO(("Load vec[%d] (%d Bytes) ...\r\n",index,flash_info.bin_size));
        pChar = (unsigned char *)FLASH_ADDR_FW_VEC + index * FLASH_ADDR_FW_VEC_SIZE;
        for( i = 0; i < flash_info.bin_size ;  ) {
            data_num = *(pChar+i) ;
            err =  TWID_Write(  dev_addr>>1, 0, 0, pChar + i + 1 , data_num, NULL); 
            i += ( data_num + 1 );     
            if ( err != SUCCESS )  {
                return(I2C_BUS_ERR) ;
            }
        } 
    }
    
    if(Global_VEC_Cfg.type == 51 ) {    
       I2C_Mixer(I2C_MIX_FM36_CODEC);
       FM36_PDMADC_CLK_Onoff(0); //disable PDM clock
       I2C_Mixer(I2C_MIX_UIF_S);          
    }     
     
    return err;     
    
}
