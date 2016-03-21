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
#ifndef __IM501_COMM_H__
#define __IM501_COMM_H__

#define  iM401_I2C_ADDR                  0xAE
#define  iM501_I2C_ADDR                  0xE2

#define  iM501_I2C_REG                   0x10
#define  iM501_SPI_REG                   0x01
#define  iM501_I2C_SPI_REG               0x12//bit2 = 1 means I2C mode, default is SPI mode

#define  TO_DSP_CMD_ADDR                 (0x0FFFBFF8)
#define  TO_DSP_CMD_OFFSET_ATTR( x )     ( (x) & 0xFFFF )
#define  TO_DSP_CMD_OFFSET_STAT( x )     ( ((x) & 0xFF) << 24 )
#define  TO_DSP_CMD_REQ_START_BUF_TRANS   0x19
#define  TO_DSP_CMD_REQ_STOP_BUF_TRANS    0x1D
#define  TO_DSP_CMD_REQ_ENTER_PSM         0x0D

#define  TO_HOST_CMD_KEYWORD_DET          0x40
#define  TO_HOST_CMD_DATA_BUF_RDY         0x41

#define  TO_HOST_CMD_ADDR                (0x0FFFBFFC)
#define  HW_VOICE_BUF_START              (0x0FFF3EE0)  // DRAM voice buffer : 0x0FFF3EE0 ~ 0x0FFFBEDF = 32kB
#define  HW_VOICE_BUF_BANK_SIZE          (1024*2)  //2kB
#define  HW_BUF_RX_L                     (0x0FFFE000)  //1kB
#define  HW_BUF_RX_R                     (0x0FFFE400)  //1kB
#define  HW_BUF_RX_SIZE                  (2048)
#define  TO_HOST_CMD_OFFSET_ATTR( x )     ( (x) & 0xFFFF )
#define  TO_HOST_CMD_OFFSET_CMD( x )      ( ((x) & 0xFF) << 16 )
#define  TO_HOST_CMD_OFFSET_STAT( x )     ( ((x) & 0xFF) << 24 )
#define  GET_HOST_CMD_OFFSET_ATTR( x )    ( (x) & 0xFFFF )
#define  GET_HOST_CMD_OFFSET_CMD( x )     ( ((x)>>16) & 0xFF )
#define  GET_HOST_CMD_OFFSET_STAT( x )    ( ((x)>>24) & 0xFF )

#define  CHECK_LAST_PACK( x )            ( ((x)>>15) & 0x01 )
#define  GET_PACK_LEN( x )               ( (x) & 0x7F )
#define  GET_PACK_INDEX( x )             ( (x) & 0x7F )

#define  CMD_STAT_INIT                   0xFF
#define  CMD_STAT_DONE                   0x00

#define  IM501_I2C_CMD_DM_WR             0x2B //For burst mode, only can be 2 bytes
#define  IM501_I2C_CMD_DM_RD             0x27 //Normal W/R, can be 1,2,4 bytes. 
#define  IM501_I2C_CMD_IM_RD             0x0D //
#define  IM501_I2C_CMD_IM_WR             0x07 //
#define  IM501_I2C_CMD_REG_WR_1          0x48 //
#define  IM501_I2C_CMD_REG_WR_2          0x4A //
#define  IM501_I2C_CMD_REG_RD            0x46 //only support one byte read
#define  IM501_I2C_CMD_DM_WR_BST         0xA8 
#define  IM501_I2C_CMD_DM_RD_BST         0xA0 
#define  IM501_I2C_CMD_IM_WR_BST         0x88 
#define  IM501_I2C_CMD_IM_RD_BST         0x07 
#define  IM501_SPI_CMD_DM_WR             0x05 
#define  IM501_SPI_CMD_DM_RD             0x01
#define  IM501_SPI_CMD_IM_WR             0x04 
#define  IM501_SPI_CMD_IM_RD             0x00 
#define  IM501_SPI_CMD_REG_WR            0x06 
#define  IM501_SPI_CMD_REG_RD            0x02 
       
#define SUCCESS                          0u
#define NO_ERR                           0u
//#define NULL                             0u

#define SPI_BUS_ERR                      179u
#define I2C_BUS_ERR                      180u
#define TO_501_CMD_ERR                   181u 
        

//typedef struct {
//    unsigned short attri;
//    unsigned char  cmd_byte;
//    unsigned char  status;
//}To_Host_CMD ;
//
//typedef struct {
//    unsigned short attri;
//    unsigned char  cmd_byte_ext;
//    unsigned char  status;
//    unsigned char  cmd_byte;
//}To_501_CMD ;


typedef struct {
    unsigned int   attri    : 24;
    unsigned int   cmd_byte : 7;
    unsigned int   status   : 1;
}To_Host_CMD ;

typedef struct {
    unsigned int  attri        : 24 ;
    unsigned int  cmd_byte_ext : 7 ;
    unsigned int  status       : 1 ;
    
    unsigned char cmd_byte;
}To_501_CMD ;

typedef struct {
    unsigned int   length;
    unsigned int   index;
    unsigned char *pdata;
    unsigned char  done;
}VOICE_BUF ;

typedef struct {
  unsigned int   spi_speed;
  unsigned char  spi_mode;  
  unsigned char  gpio_irq; 
}VOICE_BUF_CFG;

//extern unsigned char iM401_Bypass( void );
//extern unsigned char iM401_Standby( void );

unsigned char MCU_Load_Vec( unsigned char firsttime );

unsigned char im501_read_reg_i2c( unsigned char reg_addr, unsigned char *pdata );
unsigned char im501_read_reg_spi( unsigned char reg_addr, unsigned char *pdata );

unsigned char im501_write_reg_i2c( unsigned char reg_addr, unsigned char data );
unsigned char im501_write_reg_spi( unsigned char reg_addr, unsigned char data );

unsigned char im501_read_dram_i2c( unsigned int mem_addr, unsigned char *pdata );
unsigned char im501_read_dram_spi( unsigned int mem_addr, unsigned char *pdata );

unsigned char im501_burst_read_dram_spi( unsigned int mem_addr, unsigned char **pdata, unsigned int data_len );

unsigned char im501_write_dram_i2c( unsigned int mem_addr, unsigned char *pdata );
unsigned char im501_write_dram_spi( unsigned int mem_addr, unsigned char *pdata );

unsigned char im501_switch_i2c_spi( unsigned char if_type, unsigned char spi_mode );

void im501_change_if_speed( unsigned char if_type, unsigned char speed_type );

unsigned char test_send_cmd_to_im501( void );

unsigned char Write_CMD_To_iM501( unsigned char cmd_index, unsigned short para );

void Wait_Keywords_Detect( unsigned char gpio_irq );

unsigned char Request_Enter_PSM( void );

void Service_To_iM501_IRQ( void );

void ISR_iM501_IRQ( void );

void Enable_Interrupt_For_iM501_IRQ( void );

void Disable_Interrupt_For_iM501_IRQ( void );

#endif
