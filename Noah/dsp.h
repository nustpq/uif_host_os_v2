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
#ifndef __DSP_H__
#define __DSP_H__



#define  FM36_I2C_ADDR    0xC0
#define  FM36_ROM_ID_1    0x36D0
#define  FM36_ROM_ID_2    0x600C
#define  iM401_I2C_ADDR   0xAE

extern unsigned char ReInit_FM36( unsigned short sr );


extern unsigned char Init_FM36_AB03( unsigned short sr,                                   
                                     unsigned char mic_num,
                                     unsigned char lin_sp_index, 
                                     unsigned char start_slot_index,
                                     unsigned char bit_length );

extern unsigned char Init_FM36( unsigned short sr );

extern unsigned char DMIC_PGA_Control( unsigned short gain );

extern unsigned char iM401_Bypass( void );
extern unsigned char iM401_Standby( void );

extern unsigned char FM36_PWD_Bypass( void );

#endif
