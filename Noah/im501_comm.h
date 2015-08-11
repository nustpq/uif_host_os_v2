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
#define  TO_HOST_CMD_ADDR                (0x0FFFBFFC)
#define  TO_HOST_CMD_OFFSET_ATTR( x )    ( (x) & 0xFFFF )
#define  TO_HOST_CMD_OFFSET_CMD( x )     ( ((x) & 0xFF) << 16 )
#define  TO_HOST_CMD_OFFSET_STAT( x )    ( ((x) & 0xFF) << 24 )
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




//extern unsigned char iM401_Bypass( void );
//extern unsigned char iM401_Standby( void );

unsigned char MCU_Load_Vec( unsigned char firsttime );
unsigned char iM401_Load_Vec( void );

#endif
