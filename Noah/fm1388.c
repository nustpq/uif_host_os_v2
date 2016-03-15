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
*                                        FM1388 Control RELATED OPERATIONS
*
*                                          Atmel AT91SAM3U4C
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : fm1388.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/


#include <includes.h>
#include "FM1388.h"


FM1388_CODE_TYPE FM1388_Code[CODE_NUM];

void Reverse_Endian( unsigned char *pdata, unsigned char size ) 
{
    
    unsigned char i;
    unsigned char temp;
    
    for( i = 0 ; i< size>>1 ; i++ ) {
        
      temp = *( pdata+i );
      *( pdata+i ) = *( pdata + size -1 - i );
      *( pdata + size -1 - i ) = temp;
      
    }     
    
}


void Reverse_Code( unsigned char *pdata, unsigned char size ) 
{
    
    unsigned char i;
    
    for( i = 0 ; i< size/SPI_DATA_SIZE ; i++ ) {      
        Reverse_Endian( pdata, SPI_DATA_SIZE ) ;
        pdata+= SPI_DATA_SIZE;      
    }  
    size %= SPI_DATA_SIZE ;
    if( size ) {
        Reverse_Endian( pdata, size ) ;
    }
    
}
        
        
unsigned char FM1388_Write_I2C_Reg( unsigned char reg_addr, unsigned short reg_data )
{
    
    unsigned char buf[] = {  reg_data>>8, reg_data };     
    unsigned char err   = TWID_Write( FM1388_I2C_ADDR>>1, reg_addr, 1 , buf, sizeof(buf), NULL);       
    return err; 
    
}


unsigned char FM1388_Write_I2C( unsigned int mem_addr, unsigned short data )
{
   
    unsigned char err;
    unsigned char buf[2];
    buf[0] = (unsigned char)(mem_addr>>8);
    buf[1] = (unsigned char)(mem_addr>>0);     
    err   = TWID_Write( FM1388_I2C_ADDR>>1, 1, 1 , buf, sizeof(buf), NULL);       
    if( OS_ERR_NONE != err ){
        return err;
    }
    buf[0] = (unsigned char)(mem_addr>>24);
    buf[1] = (unsigned char)(mem_addr>>16);     
    err   = TWID_Write( FM1388_I2C_ADDR>>1, 2, 1 , buf, sizeof(buf), NULL);       
    if( OS_ERR_NONE != err ){
        return err;
    }
    buf[0] = (unsigned char)(data>>8);
    buf[1] = (unsigned char)(data>>0);     
    err   = TWID_Write( FM1388_I2C_ADDR>>1, 3, 1 , buf, sizeof(buf), NULL);       
    if( OS_ERR_NONE != err ){
        return err;
    }
    buf[0] = (unsigned char)(data>>8);
    buf[1] = (unsigned char)(data>>0);     
    err   = TWID_Write( FM1388_I2C_ADDR>>1, 4, 1 , buf, sizeof(buf), NULL);       
    if( OS_ERR_NONE != err ){
        return err;
    }
    buf[0] = 0;
    buf[1] = 1;     
    err   = TWID_Write( FM1388_I2C_ADDR>>1, 0, 1 , buf, sizeof(buf), NULL);       
    if( OS_ERR_NONE != err ){
        return err;
    }
    return err;
    
}





unsigned char FM1388_Read_I2C( unsigned int mem_addr, unsigned char *pdata, unsigned char size )
{
   
    unsigned char err;
    unsigned char buf[4];
    
    buf[0] = (unsigned char)(mem_addr>>8);
    buf[1] = (unsigned char)(mem_addr>>0);     
    err   = TWID_Write( FM1388_I2C_ADDR>>1, 1, 1 , buf, sizeof(buf), NULL);       
    if( OS_ERR_NONE != err ){
        return err;
    }
    buf[0] = (unsigned char)(mem_addr>>24);
    buf[1] = (unsigned char)(mem_addr>>16);     
    err   = TWID_Write( FM1388_I2C_ADDR>>1, 2, 1 , buf, sizeof(buf), NULL);       
    if( OS_ERR_NONE != err ){
        return err;
    }
    buf[0] = 0;
    buf[1] = 2;     
    err   = TWID_Write( FM1388_I2C_ADDR>>1, 0, 1 , buf, sizeof(buf), NULL);       
    if( OS_ERR_NONE != err ){
        return err;
    }
        
    err   =  TWID_Read( FM1388_I2C_ADDR>>1, 3, 1, &buf[0],2, NULL) ;
    if( OS_ERR_NONE != err ){
        return err;
    }     
    
    err   =  TWID_Read( FM1388_I2C_ADDR>>1, 4, 1, &buf[2],2, NULL) ;
    if( OS_ERR_NONE != err ){
        return err;
    } 
   
    *pdata++ = buf[1];
    *pdata++ = buf[0];
    if( size != 2 ) {
        *pdata++ = buf[3];
        *pdata++ = buf[2];
    }
    
    return err; 
    
}



void Reset_FM1388( void )
{
     GPIOPIN_Set(3,0); //normal mode
     GPIOPIN_Set(1,1); //LDO enable
     GPIOPIN_Set(6,0); //reset pin low
     OSTimeDly(1);  //delay 1ms
     GPIOPIN_Set(6,1); //reset pin high
     
}


unsigned char Check_Codec( void )
{
    
    unsigned char  i, err;
    unsigned short temp;
        
    for ( i = 0; i < 10; i++ ) {
        err = FM1388_Read_I2C( CHECK_DSP_ADDR, (unsigned char*)&temp, 2 );
        if( OS_ERR_NONE != err ){
            return err;
        }
        if( temp == CHECK_RUN_VALUE) {
            break;
        }
    }
    if( i >= 10 ) {
        return 1; //err
    }
    return err;
    
}


unsigned char Load_Vec_Codec( void )
{
    
    unsigned char err;
    for (unsigned int i = 0; i < sizeof(VEC_INIT)>>2; i++ ) {
        err = FM1388_Write_I2C_Reg( VEC_INIT[i][0],VEC_INIT[i][1] );
        if( OS_ERR_NONE != err ){
            return err;
        }
    }
    return err;
    
}

unsigned char Load_Vec_Type2( void )
{
    
    unsigned char err;
    for (unsigned int i = 0; i < sizeof(VEC_TYPE2)>>2; i++ ) {
        err = FM1388_Write_I2C( VEC_TYPE2[i][0] + TYPE2_BASE_ADDR,VEC_TYPE2[i][1] );
        if( OS_ERR_NONE != err ){
            return err;
        }
    }
    return err;
    
}

unsigned char Load_Vec_Cfg( void )
{
    
    unsigned char err;
    for (unsigned int i = 0; i < sizeof(VEC_CFG)>>2; i++ ) {
        err = FM1388_Write_I2C( VEC_CFG[i][0] + SW_BASE_ADDR,VEC_CFG[i][1] );
        if( OS_ERR_NONE != err ){
            return err;
        }
    }
    return err;
    
}

unsigned char Run_DSP( void )
{
    
    unsigned char err;
    for (unsigned int i = 0; i < sizeof(VEC_RUN)>>3; i++ ) {
        err = FM1388_Write_I2C( VEC_RUN[i][0],VEC_RUN[i][1] );
        if( OS_ERR_NONE != err ){
            return err;
        }
    }
    return err;
        
}


unsigned char Check_Run( void )
{
    
    unsigned char  err;
    unsigned short temp[2];
        
    err = FM1388_Read_I2C( FRAME_COUNTER, (unsigned char*)&temp[0],4 );
    if( OS_ERR_NONE != err ){
        return err;
    }
    OSTimeDly(1);
    err = FM1388_Read_I2C( FRAME_COUNTER, (unsigned char*)&temp[1],4 );
    if( OS_ERR_NONE != err ){
        return err;
    }
       
    if( temp[1] <= temp[0] ) {
         err = 2;
    }
    return err;
    
}


void Init_Code( void )
{
    FM1388_Code[0].start_addr =  FM1388_CODE_ADDR_A;    
    FM1388_Code[0].pdata =  (unsigned char*)&FM1388_CODE_DAT_A;
    FM1388_Code[0].data_length =  sizeof(FM1388_CODE_DAT_A);
    
    FM1388_Code[1].start_addr =  FM1388_CODE_ADDR_B;    
    FM1388_Code[1].pdata =  (unsigned char*)&FM1388_CODE_DAT_B;
    FM1388_Code[1].data_length =  sizeof(FM1388_CODE_DAT_B);
    
    FM1388_Code[2].start_addr =  FM1388_CODE_ADDR_C;    
    FM1388_Code[2].pdata =  (unsigned char*)&FM1388_CODE_DAT_C;
    FM1388_Code[2].data_length =  sizeof(FM1388_CODE_DAT_C);
    
    FM1388_Code[3].start_addr =  FM1388_CODE_ADDR_D;    
    FM1388_Code[3].pdata =  (unsigned char*)&FM1388_CODE_DAT_D;
    FM1388_Code[3].data_length =  sizeof(FM1388_CODE_DAT_D);
}



unsigned char FM1388_Write_SPI( unsigned int addr, unsigned char *pdata,  unsigned int size )
{
    unsigned char  err;
    unsigned char  j;
    unsigned char  buf[ FM1388_DATA_PACK_SIZE + 6 ];
     
    for( j=0; j<(size/FM1388_DATA_PACK_SIZE);j++ ) {
        buf[0] = FM1388_SPI_BURST_WRITE_CMD; 
        buf[1]= (addr >>24) &0xFF;
        buf[2]= (addr >>16) &0xFF;
        buf[3]= (addr >>8) &0xFF;
        buf[4]= (addr) &0xFF;
        memcpy(&buf[5], pdata, FM1388_DATA_PACK_SIZE);       
        Reverse_Code( &buf[5], FM1388_DATA_PACK_SIZE ) ;  
        err = SPI_WriteBuffer_API( &buf, sizeof(buf));              
        if( OS_ERR_NONE != err ){
            return err;
        }
        addr += FM1388_DATA_PACK_SIZE;
        pdata+= FM1388_DATA_PACK_SIZE;
    }
        
    size %= FM1388_DATA_PACK_SIZE;
    if( size ) {
            buf[0] = FM1388_SPI_BURST_WRITE_CMD; 
            buf[1]= (addr >>24) &0xFF;
            buf[2]= (addr >>16) &0xFF;
            buf[3]= (addr >>8) &0xFF;
            buf[4]= (addr) &0xFF;
            memcpy(&buf[5], pdata, size);       
            Reverse_Code( &buf[5], size ) ;
            if( size % SPI_DATA_SIZE ) {
                for( j=0; j<(size % SPI_DATA_SIZE);j++ ) {
                    buf[5+size+j] = 0;    
                }
            }
    }
    err = SPI_WriteBuffer_API( &buf, sizeof(buf));              
    if( OS_ERR_NONE != err ){
        return err;
    }   
    return err;   
 
    
}


unsigned char Load_Code( void )
{
   
    unsigned char  err;
    unsigned char  i;
    
    Init_Code();
    
    for( i=0; i<1; i++ ) {  //should be 4 segament from 4 dat files, used just 1 segament due to MCU flash limitation  
    //for( i=0; i<CODE_NUM; i++ ) { 
        err = FM1388_Write_SPI( FM1388_Code[i].start_addr, FM1388_Code[i].pdata, FM1388_Code[i].data_length );
    }  
    
    return err;
    
}




unsigned char FM1388_Init( void )
{
    unsigned char err;
    
    Reset_FM1388(); //reset FM1388
    
    OSTimeDly(1); //delay 1ms
    
    err = Load_Vec_Codec(); 
    if( OS_ERR_NONE != err ){
        return err;
    }  
    err = Check_Codec();
    if( OS_ERR_NONE != err ){
        return err;
    }
    
    err = Load_Code(); //download FM1388 code
    if( OS_ERR_NONE != err ){
        return err;
    }
    
    err = Load_Vec_Type2();
    if( OS_ERR_NONE != err ){
        return err;
    }
    err = Load_Vec_Cfg();
    if( OS_ERR_NONE != err ){
        return err;
    }
    
    err = Run_DSP();    
    if( OS_ERR_NONE != err ){
        return err;
    }
    err = Check_Run();
    if( OS_ERR_NONE != err ){
        return err;
    }   
    return err;
    
}


unsigned char Get_DOA_Value( unsigned char *pValue )
{
    
    unsigned char  err;
      
    err = FM1388_Read_I2C( DOA_REG_ADDR, pValue,2 );
    if( OS_ERR_NONE != err ){
        return err;
    }

    return err;
    
}


unsigned char FM1388_DOA_Test( void )
{
    unsigned char err;
    unsigned short doa_data;
    
    err = FM1388_Init();
    if( OS_ERR_NONE != err ){
        APP_TRACE_INFO(("\r\nFM1388 Init error: %d",err));
        return err;
    }
    for(;;) {
        err = Get_DOA_Value((unsigned char *)&doa_data);
        if( OS_ERR_NONE != err ){
            APP_TRACE_INFO(("\r\nGet DOA Value error: %d",err));
            break;
        } else {
            APP_TRACE_INFO(("\r\nDOA =  %d", doa_data));
        }
    }
    return err;
    
}
