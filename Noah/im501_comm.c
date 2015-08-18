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


static unsigned int im501_irq_counter;
static unsigned int im501_irq_gpio;
static unsigned char voice_data_pkt_sn;
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
        if(  Global_VEC_Cfg.pdm_clk_off ) {
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
    
    if(  Global_VEC_Cfg.pdm_clk_off ) { 
       OSTimeDly(30);//delay for iM501 test
       I2C_Mixer(I2C_MIX_FM36_CODEC);
       FM36_PDMADC_CLK_Onoff(0); //disable PDM clock
       I2C_Mixer(I2C_MIX_UIF_S);          
    }     
     
    return err;     
    
}



















/*
*********************************************************************************************************
*                                           im501_read_reg_i2c()
*
* Description :  read iM501 Reg via I2C, just read 1 byte.
* Argument(s) :  reg_addr :  register adress, 1 byte length
*                *pdata : pointer to where read data stores            
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
* Description :  read iM501 Reg via SPI, just read 1 byte.
* Argument(s) :  reg_addr :  register adress, 1 byte length
*                *pdata : pointer to where read data stores            
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
* Description :  write iM501 Reg via I2C, just write 1 byte.
* Argument(s) :  reg_addr :  register adress, 1 byte length
*                data : 1 byte data need write to reg            
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
* Description :  write iM501 Reg via SPI, just write 1 byte.
* Argument(s) :  reg_addr :  register adress, 1 byte length
*                data : 1 byte data need write to reg            
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
    buf[0] =  IM501_I2C_CMD_REG_WR_1;
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
* Argument(s) :  mem_addr :  register adress, 3 bytes length, 4bytes alignment
*                *pdata : pointer to where read data stores            
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
* Argument(s) :  mem_addr :  register adress, 3 bytes length, 4bytes alignment
*                *pdata : pointer to where read data stores  
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
    buf[1] =  mem_addr & 0xFF;
    buf[2] =  (mem_addr>>8) & 0xFF;
    buf[3] =  (mem_addr>>16) & 0xFF;
    buf[4] =  2;
    buf[5] =  0;    

    state =  SPI_WriteReadBuffer_API(  pbuf, 
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
* Description :  burst read iM501 DRAM via SPI, just limited to 2048-8-1 bytes.
* Argument(s) :  mem_addr :  register adress, 3 bytes length, 4bytes alignment
*                **pdata : pointer to where read data buffer point stores 
*                data_len : teh data length to be read in bytes  
* Return(s)   :  error number.           
*
* Note(s)     :  Non-Reentrant function.
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
    
    if( data_len > (EMB_BUF_SIZE-1-13) ) {
        return PARA_SET_ERR;
    }
    
    buf[0] =  IM501_SPI_CMD_DM_RD;
    buf[1] =  mem_addr & 0xFF;
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
        
     
    return err;
    
}


/*
*********************************************************************************************************
*                                           im501_write_dram_i2c()
*
* Description :  write iM501 DRAM via I2C, just write 2 bytes.
* Argument(s) :  mem_addr :  register adress, 3 bytes length, 4bytes alignment
*                *pdata : pointer to where to be written data stores            
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
* Argument(s) :  mem_addr :  register adress, 3 bytes length, 4bytes alignment
*                *pdata : pointer to where to be written data stores 
* Return(s)   :  error number.           
*
* Note(s)     :  None.
*********************************************************************************************************
*/
unsigned char im501_write_dram_spi( unsigned int mem_addr, unsigned char *pdata )
{
    unsigned char err, state;
    unsigned char buf[8];
    
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



//if_type = 0 : set to SPI mode
//if_type = 1 : set to I2C mode
unsigned char im501_switch_i2c_spi( unsigned char if_type, unsigned char spi_mode )
{
    unsigned char err;
    unsigned char data;
    
    err = im501_read_reg_i2c( iM501_I2C_SPI_REG, &data );
    if( err != NO_ERR ) {
        return err;
    }
    if(( data & 0x04 ) && (if_type==0) ) { // I2C mode, need switch to SPI
        err = im501_write_reg_spi(iM501_I2C_SPI_REG, 0x80+(spi_mode&0x03));
        if( err != NO_ERR ) {
            return err;
        }
        err = im501_write_reg_spi(iM501_I2C_SPI_REG, spi_mode&0x03);
        if( err != NO_ERR ) {
            return err;
        }
    } else if( (!(data & 0x04 )) && (if_type != 0) ) { // SPI mode, need switch to I2C
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

    err = im501_switch_i2c_spi(0, 0); //switch to SPI mode 0
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





//this routine is called by iM501 IRQ service

VOICE_BUF  voice_buf_data;

unsigned char parse_to_host_command( To_Host_CMD cmd )
{
    unsigned char err;
    unsigned char *pbuf;
         
    switch( cmd.cmd_byte ) {
        
        case 0x81 : //Reuest host to read To-Host Buffer 1st package
            voice_buf_data.length   = cmd.attri & 0x7FFF;
            voice_buf_data.index    = 1;
            voice_buf_data.done     = (cmd.attri>>15) & 0x01;
            err = im501_burst_read_dram_spi( HW_BUF_RX_L,  &pbuf,  voice_buf_data.length );
            if( err != NO_ERR ){ 
                return err;
            }
        break;
        
        case 0x82 : //Reuest host to read To-Host Buffer  
            voice_buf_data.index    =  cmd.attri & 0x7FFF;
            voice_buf_data.done     = (cmd.attri>>15) & 0x01;
            err = im501_burst_read_dram_spi( HW_BUF_RX_L,  &pbuf,  voice_buf_data.length );
            if( err != NO_ERR ){ 
                return err;
            }
        break;
        
        case 0x83 : //Reuest host to turn PDM CLKI (PDMADC CLK for FM36)
            I2C_Mixer(I2C_MIX_FM36_CODEC);
            err = FM36_PDMADC_CLK_Onoff(1); //Enable PDM clock
            I2C_Mixer(I2C_MIX_UIF_S); 
            if( err != NO_ERR ){ 
                return err;
            }
        break;
        
        default:
            err = MODE_NOT_SUPPORT;           
        break;
        
    }
        
    if( err != NO_ERR ) {
        voice_buf_data.pdata = pbuf ;
        err = pcSendDateToBuffer( EVENT_MsgQ_Noah2PCUART, 
                                      (pPCCMDDAT)&voice_buf_data,
                                      voice_data_pkt_sn, 
                                      PC_CMD_READ_VOICE_BUFFER ) ;  
        
    }
    
    return err;
    
}



unsigned char send_to_dsp_command( To_501_CMD cmd )
{
    unsigned char err;
    
    err = im501_write_dram_spi( TO_DSP_CMD_ADDR, (unsigned char *)&cmd );
    if( err != NO_ERR ){ 
        return err;
    }
    err = im501_write_reg_spi( 0x01, cmd.cmd_byte );
    if( err != NO_ERR ){ 
        return err;
    }
    
    OSTimeDly(1); //wait for DSP execute the cmd
    
    err = im501_read_dram_spi( TO_DSP_CMD_ADDR, (unsigned char *)&cmd );
    if( err != NO_ERR ){ 
        return err;
    }
    if( cmd.status != 0 ) {
        err = cmd.status;
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



unsigned char Write_CMD_To_iM501( unsigned char cmd_index, unsigned int para )
{
    unsigned char err;
    To_501_CMD cmd;
    
    cmd.cmd_byte = cmd_index;
    cmd.attri    = para & 0xFFFF ;
    
    err = send_to_dsp_command( cmd );
    
    return err;
    
}


void ISR_iM501_IRQ( void )
{
    if( Check_GPIO_Intrrupt( im501_irq_gpio ) ) {                       
        im501_irq_counter++;        
    }
}



unsigned char Read_iM501_Voice_Buffer( unsigned char gpio_irq, unsigned int timeout_ms, unsigned char pkt_sn )
{
    unsigned char err;
    unsigned int time_start;
    
    err = NO_ERR;    
    im501_irq_counter = 0;
    im501_irq_gpio = gpio_irq ;
    voice_data_pkt_sn = pkt_sn ;
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
            if( voice_buf_data.done ) {
                break;
            }
        }
        if( (OSTimeGet() - time_start) >= timeout_ms ) { //timeout hit
            break;  
        }
        //OSTimeDly(5); //for test 
    }
    
    Disable_GPIO_Interrupt( gpio_irq );
    
    return err;
    
}
