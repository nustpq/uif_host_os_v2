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
*                                        iM501 Communication Related
*
*                                          Atmel AT91SAM7A3
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : im501_comm.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/


#include <includes.h>


VOICE_BUF  voice_buf_data;

static unsigned int im501_irq_counter;
static unsigned int im501_irq_gpio;
static unsigned int im501_key_words_detect;
static unsigned char voice_data_pkt_sn;



////////////////////////////////////////////////////////////////////////////////
/**********************************   For iM401 control test  **********************/
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


void im501_change_if_speed( unsigned char type )
{
    if( type == 0) { //change to low speed
        if(  Global_VEC_Cfg.if_type == 1 ) { //I2C interface 
            TWI_Init( 20 * 1000 );               
        } else if(Global_VEC_Cfg.if_type == 2 ) { //SPI 
            SPI_Init( 1000000,  Global_UIF_Setting[ UIF_TYPE_SPI - 1 ].attribute );       
        } 
        
    } else { //change to normal speed
        if(  Global_VEC_Cfg.if_type == 1 ) { //I2C interface 
            TWI_Init( Global_UIF_Setting[ UIF_TYPE_I2C - 1 ].speed * 1000 );               
        } else if(Global_VEC_Cfg.if_type == 2 ) { //SPI 
            SPI_Init(Global_UIF_Setting[ UIF_TYPE_SPI - 1 ].speed * 1000,  Global_UIF_Setting[ UIF_TYPE_SPI - 1 ].attribute );       
        }         
    }  
    
    
}


unsigned char im501_check_pll_ready( void )
{
    unsigned int data;
    unsigned char err;
     
    if( Global_VEC_Cfg.if_type == 1 ) { //I2C interface         
        err =  im501_read_dram_i2c(0x0FFFFF34, (unsigned char*)&data );         
    } else if(Global_VEC_Cfg.if_type == 2 ) { //SPI         
        err =  im501_read_dram_spi(0x0FFFFF34, (unsigned char*)&data );
    } else {
        err = 1;
    }
     
    if( err != 0 ) {
        return 1;   
    }
    
    if( data & 0x20 ) { //check bit5 == 1 ?        
        return 1; //ready
    }else {
        return 0; //not ready
    }   
    
}


/*
*********************************************************************************************************
*                                           MCU_Load_Vec()
*
* Description :  Load parameters stored in flash to iM401/iM501 and turn on / turn off PDM clock accordingly.
*
* Argument(s) :  firsttime     is used to identify this is a full power up - power down cycle(firsttime=0),
*                              or just a power down cycle for first stage(firsttime=1)                          
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char MCU_Load_Vec( unsigned char firsttime )
{     
    unsigned char err;
    unsigned char i,j;
    unsigned char index, *pChar, buf[6]; 
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
        if(  Global_VEC_Cfg.pdm_clk_off ) {
            I2C_Mixer(I2C_MIX_FM36_CODEC);
            FM36_PDMADC_CLK_OnOff(1); //enable PDM clock    
            I2C_Mixer(I2C_MIX_UIF_S);             
        }
        if( index != 0 ) {        
            Read_Flash_State(&flash_info, FLASH_ADDR_FW_VEC_STATE + AT91C_IFLASH_PAGE_SIZE * index  );
            if( flash_info.f_w_state != FW_DOWNLAD_STATE_FINISHED ) {    
              APP_TRACE_INFO(("\r\nvec data state error...\r\n"));
              return FW_VEC_SAVE_STATE_ERR;
            }   
            APP_TRACE_INFO(("Load vec[%d] (%d Bytes) ...\r\n",index,flash_info.bin_size));
            pChar = (unsigned char *)FLASH_ADDR_FW_VEC + index * FLASH_ADDR_FW_VEC_SIZE;
            
            if(  Global_VEC_Cfg.if_type == 1 ) { //I2C interface
                for( i = 0; i < flash_info.bin_size ;  ) {
                    data_num = *(pChar+i) ;
                    err =  TWID_Write(  dev_addr>>1, 0, 0, pChar + i + 1 , data_num, NULL); 
                    i += ( data_num + 1 );               
                    if ( err != SUCCESS )  {
                        return (I2C_BUS_ERR) ;
                    }
                }
            }else if( Global_VEC_Cfg.if_type == 2 ) { //SPI interface
                for( i = 0; i < flash_info.bin_size ;  ) {
                    data_num = *(pChar+i) ;
                    if(data_num > sizeof(buf)) {
                        return (SPI_BUS_ERR);
                    }
                    for(j = 0; j < data_num ; j++) {
                        buf[j] = *(pChar + i + 1 + j);
                    }                
                    err = SPI_WriteBuffer_API( &buf, data_num );                  
                    i += ( data_num + 1 );
                    if (err != SUCCESS) {
                        return (SPI_BUS_ERR);
                    }
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
        if(  Global_VEC_Cfg.if_type == 1 ) { //I2C interface
            for( i = 0; i < flash_info.bin_size ;  ) {
                data_num = *(pChar+i) ;
                err =  TWID_Write(  dev_addr>>1, 0, 0, pChar + i + 1 , data_num, NULL); 
                i += ( data_num + 1 );     
                if ( err != SUCCESS )  {
                    return(I2C_BUS_ERR) ;
                }
            }
        } else if( Global_VEC_Cfg.if_type == 2 ) { //SPI interface
            for( i = 0; i < flash_info.bin_size ;  ) {
                data_num = *(pChar+i) ; 
                if(data_num > sizeof(buf)) {
                    return (SPI_BUS_ERR);
                }
                for(j = 0; j < data_num ; j++) {
                    buf[j] = *(pChar + i + 1 + j);
                }                
                err = SPI_WriteBuffer_API( &buf, data_num ); 
                i += ( data_num + 1 );
                if (err != SUCCESS) {
                    return (SPI_BUS_ERR);
                }
            }           
        }        
    }
    
    if( Global_VEC_Cfg.pdm_clk_off ) {
       //OSTimeDly(30);//delay for iM501 test 
       // wait for iM501 PLL change stable
       unsigned int time_start = OSTimeGet();       
       im501_change_if_speed(0);
       while( im501_check_pll_ready() == 0 ) {
           if( (OSTimeGet() - time_start) > 500 ){ //max 500ms timeout
               break;
           }
       };
       im501_change_if_speed(1);
      
       I2C_Mixer(I2C_MIX_FM36_CODEC);
       FM36_PDMADC_CLK_OnOff(0); //disable PDM clock
       I2C_Mixer(I2C_MIX_UIF_S);          
    }     
     
    return err;     
    
}


/*
*********************************************************************************************************
*                                           im501_read_reg_i2c()
*
* Description :  read iM501 Reg via I2C, just read one byte.
*
* Argument(s) :  reg_addr      is the iM501 register adress(one byte length)
*
*                *pdata        is point to where read back data will be stored  
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_read_reg_i2c( unsigned char reg_addr, unsigned char *pdata )
{
    unsigned char err, state;
    unsigned char buf[2];
    
    err    =  NO_ERR;
    buf[0] =  IM501_I2C_CMD_REG_RD;
    buf[1] =  reg_addr;
    state  =  TWID_Write( iM501_I2C_ADDR>>1,
                         0, 
                         0, 
                         buf, 
                         sizeof(buf), 
                         NULL );     
    if (state != SUCCESS) {
        err = I2C_BUS_ERR;
        return err;
    } 
   
    state =  TWID_Read( iM501_I2C_ADDR>>1,
                        0, 
                        0, 
                        pdata, 
                        1, 
                        NULL );     
    if (state != SUCCESS) {
        err = I2C_BUS_ERR;        
    } 
    
    return err;
    
}


/*
*********************************************************************************************************
*                                           im501_read_reg_spi()
*
* Description :  read iM501 Reg via SPI, just read one byte.
*
* Argument(s) :  reg_addr      is the iM501 register adress(one byte length)
*
*                *pdata        is point to where read back data will be stored
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_read_reg_spi( unsigned char reg_addr, unsigned char *pdata )
{
    unsigned char err, state;
    unsigned char buf[2];
    unsigned char *pbuf;
    
    err  = NO_ERR;
    pbuf = (unsigned char *)Reg_RW_Data; //global usage
  
    buf[0] =  IM501_SPI_CMD_REG_RD;
    buf[1] =  reg_addr;
    
    state =  SPI_WriteReadBuffer_API(  pbuf, 
                                       buf, 
                                       1 , 
                                       sizeof(buf));
             
    if (state != SUCCESS) {
        err = SPI_BUS_ERR;
        //APP_TRACE_INFO(("\r\nSPI_ReadBuffer_API err = %d",state));
        return err;
    }              
    pbuf = pbuf + 1; //fix bug
    *pdata = *pbuf;
    
    return err;
    
}




/*
*********************************************************************************************************
*                                           im501_write_reg_i2c()
*
* Description :  write iM501 Reg via I2C, just write one byte.
*
* Argument(s) :  reg_addr      is the iM501 register adress(one byte length)
*
*                data          is the data will be write to im501
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_write_reg_i2c( unsigned char reg_addr, unsigned char data )
{
    unsigned char err, state;
    unsigned char buf[3];
    
    err    =  NO_ERR;
    buf[0] =  IM501_I2C_CMD_REG_WR_1;
    buf[1] =  reg_addr;
    buf[2] =  data;
    state  =  TWID_Write( iM501_I2C_ADDR>>1,
                         0, 
                         0, 
                         buf, 
                         sizeof(buf), 
                         NULL );     
    if (state != SUCCESS) {
        err = I2C_BUS_ERR;
        return err;
    } 
    
    return err;
    
}
/*
*********************************************************************************************************
*                                           im501_write_reg_spi()
*
* Description :  write iM501 Reg via SPI, just write one byte.
*
* Argument(s) :  reg_addr      is the iM501 register adress(one byte length)
*
*                data          is the data will be write to im501
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_write_reg_spi( unsigned char reg_addr, unsigned char data )
{
    unsigned char err, state;
    unsigned char buf[3];
    
    err    =  NO_ERR;
    buf[0] =  IM501_SPI_CMD_REG_WR;
    buf[1] =  reg_addr;
    buf[2] =  data;
      
    state =  SPI_WriteBuffer_API( buf, sizeof(buf) ); 
    if (state != SUCCESS) {
        err = I2C_BUS_ERR;
        return err;
    } 
    
    return err;
    
}



/*
*********************************************************************************************************
*                                           im501_read_dram_i2c()
*
* Description :  read iM501 DRAM via I2C, just read 4 bytes.
*
* Argument(s) :  mem_addr       is dram adress, total 3 bytes length and need 4bytes alignment
*
*                *pdata         is point to where read back data will be stored   
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_read_dram_i2c( unsigned int mem_addr, unsigned char *pdata )
{
    unsigned char err, state;
    unsigned char buf[4];
    
    err   =  NO_ERR;
    buf[0] =  IM501_I2C_CMD_DM_RD;
    buf[1] =  mem_addr & 0xFF;
    buf[2] =  (mem_addr>>8) & 0xFF;
    buf[3] =  (mem_addr>>16) & 0xFF;
    state =  TWID_Write( iM501_I2C_ADDR>>1,
                         0, 
                         0, 
                         buf, 
                         sizeof(buf),  
                         NULL );     
    if (state != SUCCESS) {
        err = I2C_BUS_ERR;
        return err;
    }   
    err = im501_read_reg_i2c(0x0A, pdata);
    if( err != NO_ERR ) {
        return err;
    }
    err = im501_read_reg_i2c(0x0B, pdata+1);
    if( err != NO_ERR ) {
        return err;
    }  
    err = im501_read_reg_i2c(0x0C, pdata+2);
    if( err != NO_ERR ) {
        return err;
    } 
    err = im501_read_reg_i2c(0x0D, pdata+3);
    if( err != NO_ERR ) {
        return err;
    }
    
    return err;
    
}

/*
*********************************************************************************************************
*                                           im501_read_dram_spi()
*
* Description :  burst read iM501 DRAM via SPI, just limited to 4 bytes.
*
* Argument(s) :  mem_addr       is dram adress, total 3 bytes length and need 4bytes alignment
*
*                *pdata         is point to where read back data will be stored 
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_read_dram_spi( unsigned int mem_addr, unsigned char *pdata )
{
    unsigned char err, state;
    unsigned char buf[6];
    unsigned char *pbuf;
    
    err   =  NO_ERR;
    pbuf = (unsigned char *)Reg_RW_Data; //global usage
  
    
    buf[0] =  IM501_SPI_CMD_DM_RD;
    buf[1] =  mem_addr & 0xFC;
    buf[2] =  (mem_addr>>8) & 0xFF;
    buf[3] =  (mem_addr>>16) & 0xFF;
    buf[4] =  2;
    buf[5] =  0;    

    state  =  SPI_WriteReadBuffer_API(  pbuf, 
                                       buf, 
                                       4 , 
                                       sizeof(buf) );
            
    if (state != SUCCESS) {
        err = SPI_BUS_ERR;
        //APP_TRACE_INFO(("\r\nSPI_ReadBuffer_API err = %d",state));
        return err;
    }   
        
    pbuf = pbuf + 1; //fix bug
    for (unsigned char i = 0; i<4; i++ ) {
      *(pdata+i) = *(pbuf+i);
    }
    
    return err;
    
}


/*
*********************************************************************************************************
*                                           im501_burst_read_dram_spi()
*
* Description :  burst read iM501 DRAM via SPI, just limited to 4095 bytes.
*
* Argument(s) :  mem_addr       is dram adress, total 3 bytes length and need 4bytes alignment
*
*                **pdata        is point to where the pointer pointing to read back data will be stored 
*
*                data_len       is data length to read in bytes, maxium 4095 bytes  
*
* Return(s)   :  error number.           
*
* Note(s)     :  Non-Reentrant function.   Reg_RW_Data is used.
*                Be care full this function use fixed buffer, and return a **pointer to.
*********************************************************************************************************
*/
unsigned char im501_burst_read_dram_spi( unsigned int mem_addr, unsigned char **pdata, unsigned int data_len )
{
    unsigned char  err, state;
    unsigned char  buf[6];
    unsigned char *pbuf;
    
    err   =  NO_ERR;
    pbuf = (unsigned char *)Reg_RW_Data; //global usage    
   
    buf[0] =  IM501_SPI_CMD_DM_RD;
    buf[1] =  mem_addr     & 0xFF;
    buf[2] =  (mem_addr>>8) & 0xFF;
    buf[3] =  (mem_addr>>16) & 0xFF;
    buf[4] =  (data_len>>1)   & 0xFF;
    buf[5] =  (data_len>>(1+8))& 0xFF;    

    state =  SPI_WriteReadBuffer_API(  pbuf, 
                                       buf, 
                                       data_len , 
                                       sizeof(buf) );
            
    if (state != SUCCESS) {
        err = SPI_BUS_ERR;
        //APP_TRACE_INFO(("\r\nSPI_ReadBuffer_API err = %d",state));
        return err;
    } 
          
    *pdata =  pbuf + 1; 
    
    Demo_Sine_Gen( *pdata, data_len, 16000, 1 );
    
    return err;
    
}


/*
*********************************************************************************************************
*                                           im501_write_dram_i2c()
*
* Description :  write iM501 DRAM via I2C, just write two bytes.
*
* Argument(s) :  mem_addr       is dram adress, total 3 bytes length and need 4bytes alignment
*
*                *pdata         is pointer to where to be written data stores 
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_write_dram_i2c( unsigned int mem_addr, unsigned char *pdata )
{
    unsigned char err, state;
    unsigned char buf[6];
    
    err   =  NO_ERR;
    buf[0] =  IM501_I2C_CMD_DM_WR;
    buf[1] =  mem_addr & 0xFF;
    buf[2] =  (mem_addr>>8) & 0xFF;
    buf[3] =  (mem_addr>>16) & 0xFF;
    buf[4] =  *pdata;
    buf[5] =  *(pdata+1);
    
    state =  TWID_Write( iM501_I2C_ADDR>>1,
                         0, 
                         0, 
                         buf, 
                         sizeof(buf),  
                         NULL );  
    
    if (state != SUCCESS) {
        err = I2C_BUS_ERR;
        return err;
    } 
       
    return err;
    
}


/*
*********************************************************************************************************
*                                           im501_write_dram_spi()
*
* Description :  write iM501 DRAM via SPI, just write 4 bytes.
*
* Argument(s) :  mem_addr       is dram adress, total 3 bytes length and need 4bytes alignment
*
*                *pdata         is pointer to where to be written data stores 
*
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_write_dram_spi( unsigned int mem_addr, unsigned char *pdata )
{
    unsigned char err, state;
    unsigned char buf[10];
    
    err    =  NO_ERR;
    
    buf[0] =  IM501_SPI_CMD_DM_WR;
    buf[1] =  mem_addr & 0xFF;
    buf[2] =  (mem_addr>>8) & 0xFF;
    buf[3] =  (mem_addr>>16) & 0xFF;
    buf[4] =  2; //2 words
    buf[5] =  0;
    buf[6] =  *pdata;
    buf[7] =  *(pdata+1);
    buf[8] =  *(pdata+2);
    buf[9] =  *(pdata+3);
      
    state =  SPI_WriteBuffer_API( buf, sizeof(buf) ); 
    if (state != SUCCESS) {
        err = I2C_BUS_ERR;
        return err;
    } 
    
    return err;
    
}


/*
*********************************************************************************************************
*                                           im501_switch_i2c_spi()
*
* Description :  change iM501 actived interface type 
*
* Argument(s) :  if_type     is for indicating which interface will be actived, SPI(if_type = 2) or  I2C(if_type = 1)
*
*                spi_mode    is for indicating  SPI format( 0~3 )
*
* Return(s)   :  error number.           
*
* Note(s)     :  
*********************************************************************************************************
*/
unsigned char im501_switch_i2c_spi( unsigned char if_type, unsigned char spi_mode )
{
    unsigned char err;
    unsigned char data;
    
    err = im501_read_reg_i2c( iM501_I2C_SPI_REG, &data );
    if( err != NO_ERR ) {
        return err;
    }
    if(( data & 0x04 ) && (if_type==1) ) { // I2C mode, need switch to SPI
        err = im501_write_reg_spi(iM501_I2C_SPI_REG, 0x80+(spi_mode&0x03));
        if( err != NO_ERR ) {
            return err;
        }
        err = im501_write_reg_spi(iM501_I2C_SPI_REG, spi_mode&0x03);
        if( err != NO_ERR ) {
            return err;
        }
    } else if( (!(data & 0x04 )) && (if_type == 2) ) { // SPI mode, need switch to I2C
        err = im501_write_reg_i2c(iM501_I2C_SPI_REG, 0x04);
        if( err != NO_ERR ) {
            return err;
        }
    }
    
    return err;
    
}




unsigned char test_send_cmd_to_im501( void )
{
   
    unsigned char err;
    unsigned char *pdata;

//    pdata = (void *)OSMemGet(pMEM_Part_MsgUART,&err);
//    if( (NULL == pdata) || (OS_ERR_NONE != err) )  {
//        return 1;
//    }

    err = im501_switch_i2c_spi(2, 0); //switch to SPI mode 0
    err = im501_write_reg_spi(0x0F,5); //turn on DSP clock and on hold dsp
    
 //   for (;;) {
       err = im501_burst_read_dram_spi( 0x0ffe0000,  &pdata, 2000 );
       unsigned int k = 0;
       if( err == NO_ERR ){            
//            APP_TRACE_INFO(("\r\n-----------%8d--- Dump Data(Hex) ------------------------",k++));            
//            for( unsigned int i = 0; i < 128 ; i++ ) {         
//                if( i%16 == 0 ) {
//                    APP_TRACE_INFO(("\r\n"));
//                }
//                APP_TRACE_INFO((" %02X ",*pdata++));
//            }              
//            APP_TRACE_INFO(("\r\n---------------------------------------------------------------\r\n"));
            //OSTimeDly(1);
       } else {
            return 2; 
       }


//    }   
    
    
}






unsigned char parse_to_host_command( To_Host_CMD cmd )
{
    unsigned char err; 
    unsigned int address;
       
    switch( cmd.cmd_byte ) {
        
        case 0x40 : //Infom host Keywords detected
            im501_key_words_detect = 1 ;
        break;
        
        case 0x41 : //Reuest host to read To-Host Buffer-Fast
//            voice_buf_data.length   = (cmd.attri & 0xFFFF ) << 1;  //sample to bytes
//            address = HW_VOICE_BUF_START;
//            global_rec_spi_fast = 1;
//            err = fetch_voice_data( address, voice_buf_data.length ); 
//            if( err != NO_ERR ){
//                return err;
//            }
        break;
        
        case 0x42 : //Reuest host to read To-Host Buffer-RealTime            
//            voice_buf_data.length   = ( (cmd.attri & 0xFF0000 )>>16 ) << 1;  //sample to bytes
//            address = HW_VOICE_BUF_START + (cmd.attri & 0xFFFF);
//            global_rec_spi_fast = 0;  
//            err = fetch_voice_data( address,  voice_buf_data.length );
//            if( err != NO_ERR ){ 
//                return err;
//            }
        break;
                       
        default:
            err = 2;           
        break;
        
    }
            
    return err;
    
}



unsigned char send_to_dsp_command( To_501_CMD cmd )
{
    unsigned char err;
    unsigned int  i;
    
    err = im501_write_dram_spi( TO_DSP_CMD_ADDR, (unsigned char *)&cmd );
    if( err != 0 ){ 
        return err;
    }
    
    err = im501_write_reg_spi( 0x01, cmd.cmd_byte ); //generate interrupt to DSP
    if( err != 0 ){ 
        return err;
    }
    
    for( i = 0; i< 50; i++ ) {   //wait for (50*100us = 5ms) to check if DSP finished 
        err = im501_read_dram_spi( TO_DSP_CMD_ADDR, (unsigned char *)&cmd );
        if( err != 0 ){ 
            return err;
        }
        if( cmd.status != 0 ) {
            err = TO_501_CMD_ERR;
        } else {
            err = 0;
        }
        delay_us(100); //??
    }
    
    return err;
    
}



unsigned char resp_to_host_command( void )
{
    unsigned char err;
    To_Host_CMD   cmd;
    
    err = im501_read_dram_spi( TO_HOST_CMD_ADDR, (unsigned char *)&cmd );
    if( err != NO_ERR ){ 
        return err;
    }
    
    err = parse_to_host_command( cmd );
    if( err != NO_ERR ){ 
        return err;
    }
    
    cmd.status = 0;
    err = im501_write_dram_spi( TO_HOST_CMD_ADDR, (unsigned char *)&cmd );
    if( err != NO_ERR ){ 
        return err;
    }
    
    return err;
    
}





void ISR_iM501_IRQ( void )
{    
    if( Check_GPIO_Intrrupt( im501_irq_gpio ) ) {         
        im501_irq_counter++;
      
    }
    
}


void Check_KeyWords_Detect_Status( void )
{
    
    unsigned char err;
    VOICE_BUF_CFG voice_buf_cfg;
    
    if ( im501_irq_counter ) {
        
        im501_irq_counter--;
        
        err = resp_to_host_command();             
        if( err != NO_ERR ){            
            return;
        }  
    
        if( im501_key_words_detect == 1 ) {
            
            Disable_GPIO_Interrupt( im501_irq_gpio ); //disable IRQ interrupt
                    
            I2C_Mixer(I2C_MIX_FM36_CODEC);
            FM36_PDMADC_CLK_OnOff(1); //Enable PDM clock
            I2C_Mixer(I2C_MIX_UIF_S); 
                    
            Disable_SPI_Port(); //disabled host mcu SPI
                    
            voice_buf_cfg.spi_mode = Global_UIF_Setting[1].speed;
            voice_buf_cfg.spi_speed = Global_UIF_Setting[1].attribute;
            voice_buf_cfg.gpio_irq = im501_irq_gpio;    
            
            Rec_Voice_Buffer_Start( &voice_buf_cfg ); //send CMD to Audio MCU                    
        }  
        
    }
    
}

void Wait_Keywords_Detect( unsigned char gpio_irq )
{
    im501_irq_counter = 0;
    im501_key_words_detect = 0 ;
    im501_irq_gpio = gpio_irq ; 
    //set gpio interruption
    Config_GPIO_Interrupt( im501_irq_gpio, ISR_iM501_IRQ );     
  
    //PP_TRACE_INFO(("\r\n::Record voice buffer time cost: %d ms\r\n",time_rec));
    
    
}


unsigned char Request_Enter_PSM( void )
{
    
    unsigned char err;
        
    err = Write_CMD_To_iM501( TO_DSP_CMD_REQ_ENTER_PSM, 0 );
    if( err != NO_ERR ){ 
        return err;
    }
    
    I2C_Mixer(I2C_MIX_FM36_CODEC);
    FM36_PDMADC_CLK_OnOff(0); //Disable PDM clock
    I2C_Mixer(I2C_MIX_UIF_S);
    
    Wait_Keywords_Detect(2);
    return err;
    
}



unsigned char Record_iM501_Voice_Buffer( unsigned char gpio_irq, unsigned int timeout_ms )
{
    unsigned char err;
    unsigned int time_start, time_rec;
    
    err = NO_ERR;    
    im501_irq_counter = 0;
    im501_irq_gpio = gpio_irq ;   
    time_start = OSTimeGet(); //save start time
    
    //set gpio interruption
    Config_GPIO_Interrupt( gpio_irq, ISR_iM501_IRQ );     
  
    //
    while(1) {  
   
        if ( im501_irq_counter ) {
            //APP_TRACE_INFO(("::ISR_iM501_IRQ : %d\r\n",im501_irq_counter)); //for test
            im501_irq_counter--;
            err = resp_to_host_command();             
            if( err != NO_ERR ){ 
                break;
            }
            if(voice_buf_data.index == 1) {
                time_rec = OSTimeGet(); 
            }
            if( voice_buf_data.done ) {
                time_rec = OSTimeGet() - time_rec;
                break;
            }
        }
        if( (OSTimeGet() - time_start) >= timeout_ms ) { //timeout hit
            err = TIME_OUT;
            break;  
        }
        //OSTimeDly(5); //for test 
    }
    
    Disable_GPIO_Interrupt( gpio_irq );
    APP_TRACE_INFO(("\r\n::Record voice buffer time cost: %d ms\r\n",time_rec));
    
    return err;
    
}


unsigned char Write_CMD_To_iM501( unsigned char cmd_index, unsigned short para )
{
    
    unsigned char err;
    To_501_CMD    cmd;
    
    cmd.cmd_byte = cmd_index;//((cmd_index & 0x3F) << 2) | 0x01; //D[1] : "1", interrupt DSP. This bit generates NMI (non-mask-able interrupt), D[0]: "1" generate mask-able interrupt
    cmd.attri    = para & 0xFFFF ;
    cmd.status   = 1;
    err = send_to_dsp_command( cmd );
    
    return err;
    
}



static FLASH_INFO  flash_info_voice_buf;

unsigned char save_voice_buffer_to_flash( VOICE_BUF  *p_voice_buf_data )
{  
    unsigned char err; 
    unsigned int flash_addr;    
              
    err  = NO_ERR;
    flash_addr = FLASH_ADDR_FW_BIN + ( p_voice_buf_data->index - 1 ) * p_voice_buf_data->length;
    
    LED_Toggle(LED_DS2);     
    err = FLASHD_Write_Safe( flash_addr, p_voice_buf_data->pdata, p_voice_buf_data->length );     
    if(err != NO_ERR ) {                     
        APP_TRACE_INFO(("ERROR: Write MCU flash failed!\r\n"));
        return err;
    }
    
    if( p_voice_buf_data->index == 1) { //first package
        Read_Flash_State(&flash_info_voice_buf, FLASH_ADDR_FW_STATE);
        if( flash_info_voice_buf.flag != 0x55 ) {
            flash_info_voice_buf.flag = 0x55;
            flash_info_voice_buf.f_w_counter = 0;
            flash_info_voice_buf.s_w_counter = 0;
        }
        flash_info_voice_buf.bin_size  = p_voice_buf_data->length;
        flash_info_voice_buf.f_w_state = FW_DOWNLAD_STATE_UNFINISHED ;
        if( p_voice_buf_data->done == 0) { //first not last  
           strcpy(flash_info_voice_buf.bin_name, (char const*)("iM501 voice buffer:broken"));    
           err = Write_Flash_State( &flash_info_voice_buf,  FLASH_ADDR_FW_STATE );
           if(err != NO_ERR ) {                     
                APP_TRACE_INFO(("ERROR: Write MCU flash failed!\r\n"));
                return err;
            }
        }
    }   
           
    flash_info_voice_buf.bin_size   += p_voice_buf_data->length ;   
    
    if( p_voice_buf_data->done == 1 ) { //last package
        flash_info_voice_buf.f_w_state = FW_DOWNLAD_STATE_FINISHED ;  
        flash_info_voice_buf.f_w_counter++;
        flash_info_voice_buf.s_w_counter++;        
        strcpy(flash_info_voice_buf.bin_name, (char const*)("iM501 voice buffer"));    
        err = Write_Flash_State( &flash_info_voice_buf,  FLASH_ADDR_FW_STATE ); 
    }
    
    return err;  
    
}
/*
unsigned char save_voice_buffer_to_flash( VOICE_BUF  *p_voice_buf_data )
{  
    unsigned char err; 
    unsigned int flash_addr;    
    FLASH_INFO   flash_info, flash_info_pre;
          
    err   = NO_ERR;
    flash_addr = FLASH_ADDR_FW_BIN + ( p_voice_buf_data->index - 1 ) * p_voice_buf_data->length;
    
    Read_Flash_State(&flash_info, FLASH_ADDR_FW_STATE);          
    if( p_voice_buf_data->index == 1) { //first package
        flash_info.bin_size = 0;
    }            
    LED_Toggle(LED_DS2);    
    err = FLASHD_Write_Safe( flash_addr, p_voice_buf_data->pdata, p_voice_buf_data->length ); 
    
    if(err != NO_ERR ) {                     
        APP_TRACE_INFO(("ERROR: Write MCU flash failed!\r\n"));
        return err;
    }
    if( flash_info.flag != 0x55 ) {
        flash_info.flag = 0x55;
        flash_info.f_w_counter = 0;
        flash_info.s_w_counter = 0;
    }
    if( p_voice_buf_data->done == 1 ) {
        flash_info.f_w_state = FW_DOWNLAD_STATE_FINISHED ;
    } else {
        flash_info.f_w_state = FW_DOWNLAD_STATE_UNFINISHED ;
    }
    flash_info.f_w_counter++;
    flash_info.s_w_counter++;
    flash_info.bin_size   += (p_voice_buf_data->length) ;
    strcpy(flash_info.bin_name, (char const*)("iM501 voice buffer")); 
    if( flash_info_pre != flash_info) {   
      err = Write_Flash_State( &flash_info,  FLASH_ADDR_FW_STATE ); 
    }
    return err;  
    
}
*/

unsigned char fetch_voice_buffer_from_flash( unsigned char pkt_sn )
{
    unsigned char err;
    unsigned int  flash_addr; 
    unsigned int  i, index;
    unsigned int  addr;
    unsigned int  times, counter;    
    FLASH_INFO   flash_info;
    
    err   = NO_ERR;      
    flash_addr = FLASH_ADDR_FW_BIN ;    
    Read_Flash_State(&flash_info, FLASH_ADDR_FW_STATE);     
    
    //check if voice buffer data broken
    if( (flash_info.flag != 0x55) || (flash_info.f_w_state != FW_DOWNLAD_STATE_FINISHED) ) { //voice buffer broken
        voice_buf_data.index  = 1;
        voice_buf_data.pdata  = (unsigned char *)flash_addr;
        voice_buf_data.done   = 2; //2 means error data
        voice_buf_data.length = 0;
        err = pcSendDateToBuffer( EVENT_MsgQ_Noah2PCUART, 
                                      (pPCCMDDAT)&voice_buf_data,
                                      pkt_sn, 
                                      PC_CMD_FETCH_VOICE_BUFFER ) ; 
       return err; 
    }
    
    //start fetch voice buffer
    times   = flash_info.bin_size / MAX_DATA_SIZE ;
    counter = flash_info.bin_size % MAX_DATA_SIZE ;
    index   = 1;
    
    for ( i = 0; i < times; i++ ) {   
        voice_buf_data.index  = index++;
        voice_buf_data.pdata  = (unsigned char *)(flash_addr + (i*MAX_DATA_SIZE));
        voice_buf_data.done   = (counter || (i < times)) ? 0 : 1; //0:unfinised, 1:done
        voice_buf_data.length = MAX_DATA_SIZE;
        err = pcSendDateToBuffer( EVENT_MsgQ_Noah2PCUART, 
                                      (pPCCMDDAT)&voice_buf_data,
                                      pkt_sn, 
                                      PC_CMD_FETCH_VOICE_BUFFER ) ;  
    }
    
    if( counter ) {   
        voice_buf_data.index  = index++;
        voice_buf_data.pdata  = (unsigned char *)(flash_addr + (i*MAX_DATA_SIZE));
        voice_buf_data.done   = 1; //done
        voice_buf_data.length = counter;
        err = pcSendDateToBuffer( EVENT_MsgQ_Noah2PCUART, 
                                      (pPCCMDDAT)&voice_buf_data,
                                      pkt_sn, 
                                      PC_CMD_FETCH_VOICE_BUFFER ) ;         
    }
    
    return err;
    
}
