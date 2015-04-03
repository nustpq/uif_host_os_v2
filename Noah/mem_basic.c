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
*                                        FM DSP W/R RELATED OPERATIONS
*
*                                          Atmel AT91SAM7A3
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : mem_basic.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/


#include <includes.h>

//FM36 unlock_mmreg is different from previous dsp
unsigned char DSP_PM_Type = 1 ;  // 1: FM36; 0 : others DSP



/*******************************   Unlock PM  *******************************/ 
/**  this operation must be executed once before Read_PM() after reset DSP **/
unsigned char DM_Write( unsigned short dm_addr,unsigned short dm_val ) 
{
    return DM_SingleWrite(0xC0, dm_addr, dm_val);
}
static void Unlock_PM( unsigned char dsp_type )
{
    unsigned short unlock_mmreg;
    
    if( dsp_type == 0 ) {      
        unlock_mmreg = 0x3FD6; // the MREG address is 0x3FEE for FM31/FM32/FM33/FM34 
    } else {
        unlock_mmreg = 0x3FEE; // the MREG address is 0x3FEE for FM36 
    }
       
    DM_Write( unlock_mmreg, 0xECF3); 
    DM_Write( unlock_mmreg, 0x3807);
    DM_Write( unlock_mmreg, 0x79AD);

}




/************************  DM/ PM/ CM Single Write ******************************/
unsigned char DM_SingleWrite(unsigned char dev_addr,unsigned short dm_addr,unsigned short dm_val)
{
    unsigned char state ;    
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_DM_WR, (dm_addr>>8)&0xFF, dm_addr&0xFF, (dm_val>>8)&0xff, dm_val&0xff};
    
    state =  TWID_Write( dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);  
   
    return( state );
}


unsigned char PM_SingleWrite(unsigned char dev_addr,unsigned short dm_addr,unsigned char *pdata, unsigned int xor_key)
{
    unsigned char state ;      
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_PM_WR,(dm_addr>>8)&0xFF, dm_addr&0xFF, *(pdata++)^(xor_key>>16), *(pdata++)^(xor_key>>8),*(pdata++)^(xor_key) };
    
    state =  TWID_Write( dev_addr>>1, 0, 0, buf, sizeof(buf), NULL); 
    
    return(state);
}


unsigned char CM_SingleWrite(unsigned char dev_addr,unsigned short dm_addr,unsigned char *pdata)
{
    unsigned char state ;   
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_CM_WR, (dm_addr>>8)&0xFF, dm_addr&0xFF, *(pdata++),*(pdata++) };
    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL); 
   
    return(state);
}



/************************  DM/ PM/ CM Single Read *****************************/

unsigned char DM_LegacyRead(unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal)
{
    unsigned char state ;    
    //APP_TRACE_INFO( ("\r\nRead DM [0x%4X] = ", dm_addr) ); //some    
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_DM_RD,(dm_addr>>8)&0xFF, dm_addr&0xFF};
    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);     
    if (state != SUCCESS)
    {
        return(state) ;
    }    

    buf[2] = FM_CMD_HOST_RD;
    buf[3] = 0x25; 
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS)
    {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal++, 1, NULL) ;
    if (state != SUCCESS)
    {
        return(state) ;
    }
       
    buf[3] = 0x26; 
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS)
    {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
    if (state != SUCCESS)
    {
        return(state) ;
    }
    
    //APP_TRACE_INFO( (  " 0x%4X \r\n", *(unsigned short *)(pVal-2)  ) ); //some    
    return(state);
}


//not support
unsigned char DM_LegacyReadReStart(unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal)
{       
    return DM_LegacyRead( dev_addr, dm_addr, pVal ) ; 
}


unsigned char PM_LegacyRead(unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal)
{
    unsigned char state ;   
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_PM_RD, (dm_addr>>8)&0xFF, dm_addr&0xFF}; 
    
    Unlock_PM(DSP_PM_Type);    
      
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);     
    if (state != SUCCESS)
    {
        return(state) ;
    }

    buf[2] = FM_CMD_HOST_RD;
    buf[3] = 0x24; 
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS)
    {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
    if (state != SUCCESS)
    {
        return(state) ;
    }
   
    buf[3] = 0x25;
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS)
    {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
    if (state != SUCCESS)
    {
        return(state) ;
    }

    buf[3] = 0x26;
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS)
    {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
    if (state != SUCCESS)
    {
        return(state) ;
    }    

    return(state);
}

    
unsigned char CM_LegacyRead(unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal)
{
    unsigned char state ;    
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_CM_RD, (dm_addr>>8)&0xFF, dm_addr&0xFF}; 
    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);     
    if (state != SUCCESS)
    {
        return(state) ;
    }

    buf[2] = FM_CMD_HOST_RD;
    buf[3] = 0x25;
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS)
    {
        return(state) ;
    }   
    state =  TWID_Read(  dev_addr>>1, 0, 0, pVal++,1, NULL) ;
    if (state != SUCCESS)
    {
        return(state) ;
    }

    buf[3] = 0x26;
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS)
    {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
    if (state != SUCCESS)
    {
        return(state) ;
    }  
    
    *pVal = 0 ;   // CM  only use high two bytes
    return(state);
}



/*********************     DM / PM Burst Read / Write     **********************/
unsigned char DM_BurstWrite(  unsigned char dev_addr,
                              struct{unsigned short dm_addr;unsigned short dm_val;} *pDat,
                              unsigned char DatNum )
{
   
    unsigned char state ;
    unsigned char i ;
   
    unsigned char buf[] = { 0x00, 0x1C,0,0 };
    state =  TWID_Write( dev_addr>>1, 0, 0 , buf, 2, NULL);                 
    if (state != SUCCESS) {
        return(state);
    }
    
    for( i=0; (i < DatNum)&&(state == SUCCESS);  i++ )  { 
        buf[0] = (pDat[i].dm_addr>>8)&0xFF;
        buf[1] = pDat[i].dm_addr&0xFF ;
        buf[2] = (pDat[i].dm_val>>8)&0xFF ;
        buf[3] = pDat[i].dm_val&0xFF ;
        state =  TWID_Write(  dev_addr>>1, 0xFCF33B, 3 , buf, sizeof(buf), NULL);         
    }
   
    return(state);	
}


unsigned char DM_BurstWrite_s(unsigned char dev_addr,unsigned short StAddr,unsigned char DatNum,void *pDat)
{
    unsigned char state ;
    unsigned char *pDmDat   = (unsigned char *)pDat ; 
    unsigned short data_num = (DatNum-1) * 2 - 1; 

    unsigned char buf[] = {0x3F, 0xE8, (data_num>>8)&0xFF, data_num&0xFF}; //????
    state =  TWID_Write(  dev_addr>>1, 0xFCF33B, 3 , buf, sizeof(buf), NULL);   
    if (state != SUCCESS) {
        return(state);
    } 
    
    buf[0] =(StAddr>>8)&0xFF;
    buf[1] =(StAddr)&0xFF;
    buf[2] =*pDmDat++;
    buf[3] =*pDmDat++;

    state =  TWID_Write(  dev_addr>>1, 0xFCF33B, 3 , buf, sizeof(buf), NULL);
    if (state != SUCCESS) {
        return(state);
    }    

    state =  TWID_Write(  dev_addr>>1, 0xFCF3B8, 3 , pDmDat, data_num +1 , NULL);     
    if (state != SUCCESS) {
        return(state);
    }

    return(state);	
}



unsigned char PM_BurstWrite_s(unsigned char dev_addr,unsigned short StAddr,unsigned char DatNum,void *pDat) //fake burst
{
    unsigned char state ;
    unsigned char *pDmDat = (unsigned char *)pDat ; 
    unsigned short data_num = (DatNum-1) * 3 - 1; 

    unsigned char buf[] = {0x3F, 0xE8, (data_num>>8)&0xFF, data_num&0xFF,0};
    state =  TWID_Write(  dev_addr>>1, 0xFCF33B, 3 , buf, 4, NULL);   
    if (state != SUCCESS) {
        return(state);
    } 
    
    buf[0] =(StAddr>>8)&0xFF; 
    buf[1] =(StAddr)&0xFF;
    buf[2] = *pDmDat++;
    buf[3] = *pDmDat++;
    buf[4] = *pDmDat++;    
    state =  TWID_Write(  dev_addr>>1, 0xFCF30D, 3 , buf, sizeof(buf), NULL);
    if (state != SUCCESS) {
        return(state);
    }    

    state =  TWID_Write(  dev_addr>>1, 0xFCF388, 3 , pDmDat, data_num + 1 , NULL);     
    if (state != SUCCESS) {
        return(state);
    }

    return(state);	
}


/*********************     DM / PM Fast Read / Write     **********************/
unsigned char PM_FastWrite_s(unsigned char dev_addr,unsigned short StAddr,unsigned char DatNum,void *pDat )
{
    unsigned char state ;
    unsigned char *pDmDat   = (unsigned char *)pDat ; 
    unsigned short data_num = (DatNum) * 3   ;   
    
    state = HOST_SingleWrite_2(dev_addr, 0x0F, data_num);      
    if (state != SUCCESS) {
        return(state);
    }    
    
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_PM_WR,(StAddr>>8)&0xFF, (StAddr)&0xFF, *(pDmDat++),*(pDmDat++),*(pDmDat++) } ; //??
    state =  TWID_Write( dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);  //???
    
    if (state != SUCCESS) {
        return(state);
    }   

    return(state);	
}



unsigned char DM_FastRead(unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal)
{
    unsigned char state ;
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_DM_RD,(dm_addr>>8)&0xFF, dm_addr&0xFF};    
    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);     
    if (state != SUCCESS) {
        return(state) ;
    }      
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal,2, NULL) ;
     
    return(state);
}


// not support specially, just used the previous one
unsigned char DM_FastReadReStart(unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal) 
{
    return DM_FastRead(dev_addr, dm_addr,pVal); 
}




//check DSP IDMA control port stauts, wait until not busy
unsigned char Check_IDMA( unsigned char dev_addr )
{
     unsigned char i;
     unsigned char temp;
     unsigned char state ;
     const unsigned char timeout = 100 ;
     
     for ( i = 0; i< timeout ; i++ ) {
        state = HOST_LegacyRead( dev_addr, 0x20, &temp ) ;
        if( state != 0 ) return state ; //host read error
        if(!( temp&0x02) ) break ;  
        
     }     
     
     if(i == timeout ) {
        return 0x44 ;//error timeout
        
     }else{         
        return 0 ; 
        
     }
    
}


/*****************     DM / PM / CM  MEM_Block_Read / Write [I2C]    **********/

//read
unsigned char MEM_Block_LegacyRead(     unsigned char dev_addr, 
                                        unsigned char mem_type, 
                                        unsigned short start_addr,
                                        unsigned char num, 
                                        unsigned char *pVal     )
{
   
    unsigned char state = 0xFF ;
    unsigned char i     = 0;
    unsigned int cmd[]  = {FM_CMD_DM_RD, FM_CMD_PM_RD, FM_CMD_CM_RD}; //cmd for read : DM, PM, CM 
    unsigned char buf[] = {FM_CMD_SYN_0, FM_CMD_SYN_1, 0,(start_addr>>8)&0xFF, start_addr&0xFF};  
     
    if( mem_type == MEM_TYPE_PM ) {      
        Unlock_PM(DSP_PM_Type);
    }
    
    buf[2] = cmd[mem_type] ;
    
    for(i = 0; i < num ; i++) {  
      
        buf[0] = (start_addr>>8)&0xFF;
        buf[1] = start_addr&0xFF ; 
        start_addr ++ ;
        
        state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);     
        if (state != SUCCESS) {          
            return(state) ;
        }
        
        buf[2] = FM_CMD_HOST_RD;
        buf[3] = 0x26;      
        state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
        if (state != SUCCESS) {
            return(state) ;
        }   
        state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
        if (state != SUCCESS) {
            return(state) ;
        }
        
        buf[3] = 0x25;
        state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
        if (state != SUCCESS) {
            return(state) ;
        }   
        state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
        if (state != SUCCESS) {
            return(state) ;
        }
        
        //////////////////////////////////////////
        if( mem_type == MEM_TYPE_PM ) {
          
            buf[3] = 0x24;
            state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
            if (state != SUCCESS) {
                return(state) ;
            }   
            state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
            if (state != SUCCESS) {
                return(state) ;
            }    
        }
        
    }
    
    return (state);
    
}


//write
unsigned char MEM_Block_SingleWrite(    unsigned char dev_addr,                                        
                                        unsigned char mem_type, 
                                        unsigned short start_addr,
                                        unsigned char num, 
                                        unsigned char *pVal )
{

    unsigned char state ;
    unsigned char i     ;
    unsigned int cmd[]   = { FM_CMD_DM_WR, FM_CMD_PM_WR, FM_CMD_CM_WR }; //cmd for write : DM, PM, CM 
    unsigned char buf[8] ;
    unsigned char  data_length  = 2; //for PM 3, CM 2, DM 2
    
    buf[0] = FM_CMD_SYN_0  ;
    buf[1] = FM_CMD_SYN_1  ;
    buf[2] = cmd[mem_type] ;
        
    for(i = 0; i < num ; i++)  { 
        
        buf[3] = (start_addr>>8)&0xFF;
        buf[4] = start_addr&0xFF;
        buf[5] = *pVal++;
        buf[6] = *pVal++;
        if( mem_type == MEM_TYPE_PM ){
            buf[7] = *pVal++;
            data_length  = 3;
        }
       
        state =  TWID_Write( dev_addr>>1, 0, 0 , buf, data_length+5, NULL);       
        if (state != SUCCESS) {
            return(state) ;
        } 
        start_addr++ ;
        //OSTimeDly(10); //fixed issue on fast write DSP
    }    
    return (state);    

}






/*********************     HOST Register Read / Write     *********************/
unsigned char HOST_SingleWrite_1(unsigned char dev_addr,unsigned char host_addr,unsigned char host_val)
{
     unsigned char state ;    
     unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_HOST_WR_1, host_addr, host_val};  
     
     state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);       
     
     return(state);
}


unsigned char HOST_SingleWrite_2(unsigned char dev_addr,unsigned char host_addr,unsigned short host_val)
{
    unsigned char state ;
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_HOST_WR_2, host_addr, (host_val>>8)&0xff, host_val&0xff}; 
    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);   
     
    return(state);
}


unsigned char HOST_LegacyRead(unsigned char dev_addr, unsigned char host_addr,unsigned char *pVal)
{
    unsigned char state;   
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_HOST_RD, host_addr};
    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);  
    if (state != SUCCESS)
    {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal,1, NULL) ;
    if (state != SUCCESS)
    {
        return(state) ;
    }
    
    return(state);
}


/*********************     DSP Register Read / Write     **********************/
unsigned char DSP_SingleWrite_1(unsigned char dev_addr,unsigned char dsp_addr,unsigned char dsp_val)
{
    unsigned char state ;
   
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_DSP_WR_1, dsp_addr, dsp_val};    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);  
     
    return(state);
}


unsigned char DSP_SingleWrite_2(unsigned char dev_addr,unsigned char dsp_addr,unsigned short dsp_val)
{
    unsigned char state ;
    
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_DSP_WR_2, dsp_addr, (dsp_val>>8)&0xff, dsp_val&0xff};    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);  
     
    return(state);
}


unsigned char DSP_LegacyRead(unsigned char dev_addr, unsigned char dsp_addr,unsigned char *pVal)
{  
    unsigned char state;    
    unsigned char buf[] = { FM_CMD_SYN_0, FM_CMD_SYN_1, FM_CMD_DSP_RD, dsp_addr}; 
    
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, sizeof(buf), NULL);     
    if (state != SUCCESS) {
        return(state) ;
    }  
     
    buf[2] = FM_CMD_HOST_RD;
    buf[3] = 0x4B;      
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS) {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal++,1, NULL) ;
    if (state != SUCCESS) {
        return(state) ;
    }
        
    buf[3] = 0x4A;      
    state =  TWID_Write(  dev_addr>>1, 0, 0, buf, 4, NULL); 
    if (state != SUCCESS) {
        return(state) ;
    }   
    state =  TWID_Read( dev_addr>>1, 0, 0, pVal,1, NULL) ;
    if (state != SUCCESS) {
        return(state) ;
    } 
     
    return(state);
  
}

 






/******************    USART Read/ Write Related    **************************/

unsigned char DM_SingleWrite_uart(unsigned char dev_addr,unsigned short dm_addr,unsigned short dm_val)
{  
    unsigned char buf[] = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_DM_WR,(dm_addr>>8)&0xff,dm_addr&0xff,(dm_val>>8)&0xff,dm_val&0xff };    
    dev_addr = dev_addr ;    
   
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;
  
    return( 0 );
    
}


unsigned char DM_LegacyRead_uart(unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal)
{
    
    unsigned char buf[]    = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_DM_RD, (dm_addr>>8)&0xFF, dm_addr&0xFF };
    unsigned char err_code = 0 ;
    dev_addr = dev_addr ;   
    
    DBGU_Read_NULL_RHR();
    
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;        
    
    buf[2]  = FM_CMD_HOST_RD ;     
    buf[3]  = 0x25 ;    
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal++, 1000);    
    if(err_code != 0 ) {
      return( err_code ); 
    }
        
    buf[3] = 0x26 ;
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal,1000);  
    
    return( err_code );       
    
}


unsigned char PM_SingleWrite_uart(unsigned char dev_addr,unsigned short dm_addr,unsigned char *pdata, unsigned int xor_key)
{
   
    unsigned char buf[] = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_PM_WR,(dm_addr>>8)&0xFF, dm_addr&0xFF, *(pdata++)^(xor_key>>16),*(pdata++)^(xor_key>>8),*(pdata++)^(xor_key)};   
    dev_addr = dev_addr ;    
   
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;
  
    return( 0 );    
 
}


unsigned char PM_LegacyRead_uart( unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal)
{
  
    unsigned char buf[]    = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_PM_RD, (dm_addr>>8)&0xFF, dm_addr&0xFF };
    unsigned char err_code = 0 ;
  
    dev_addr = dev_addr ;   
    
    Unlock_PM(DSP_PM_Type);
     
    DBGU_Read_NULL_RHR();
    
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;        
    
    buf[2]  = FM_CMD_HOST_RD ;     
    buf[3]  = 0x26 ;    
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal++, 1000);    
    if(err_code != 0 ) {
      return( err_code ); 
    }
        
    buf[3] = 0x25 ;
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal++,1000);  
    if(err_code != 0 ) {
      return( err_code ); 
    }
    
    buf[3] = 0x24 ;
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal,1000); 
    
    return( err_code ); 
    
    

}



unsigned char CM_SingleWrite_uart(unsigned char dev_addr,unsigned short dm_addr,unsigned char *pdata)
{
   
    unsigned char buf[] = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_CM_WR,(dm_addr>>8)&0xFF, dm_addr&0xFF, *(pdata++),*(pdata++) };
    dev_addr = dev_addr ;    
   
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;
  
    return( 0 );   
    
}

 

unsigned char CM_LegacyRead_uart(unsigned char dev_addr, unsigned short dm_addr,unsigned char *pVal)
{
       
    unsigned char buf[]    = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_CM_RD, (dm_addr>>8)&0xFF, dm_addr&0xFF };
    unsigned char err_code = 0 ;
  
    dev_addr = dev_addr ;   
   
     
    DBGU_Read_NULL_RHR();
    
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;        
    
    buf[2]  = FM_CMD_HOST_RD ;     
    buf[3]  = 0x25 ;    
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal++, 1000);    
    if(err_code != 0 ) {
      return( err_code ); 
    }
        
    buf[3] = 0x26 ;
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal++,1000);  
    
    *pVal = 0 ; // CM  only use high two bytes
    
    return( err_code ); 
}




 
unsigned char HOST_SingleWrite_1_uart(unsigned char dev_addr,unsigned char host_addr,unsigned char host_val)
{  
  
    unsigned char buf[] = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_HOST_WR_1,host_addr, host_val};    
    dev_addr = dev_addr ;    
   
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;
  
    return( 0 );
    
}


unsigned char HOST_SingleWrite_2_uart(unsigned char dev_addr,unsigned char host_addr,unsigned short host_val)
{
  
    unsigned char buf[] = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_HOST_WR_2,host_addr, (host_val>>8)&0xff, host_val&0xff};    
    dev_addr = dev_addr ;    
   
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;
  
    return( 0 );    
    
}


unsigned char HOST_LegacyRead_uart(unsigned char dev_addr, unsigned char host_addr,unsigned char *pVal)
{
  
    unsigned char buf[]    = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_HOST_RD, host_addr };
    unsigned char err_code = 0 ;
    dev_addr = dev_addr ;   
    
    DBGU_Read_NULL_RHR();    
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;  
   
    err_code = DBGU_Read_Byte( pVal, 1000);    
    
    return( err_code );     
    
}


 
unsigned char DSP_SingleWrite_1_uart(unsigned char dev_addr,unsigned char dsp_addr,unsigned char dsp_val)
{
  
    unsigned char buf[] = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_DSP_WR_1, dsp_addr, dsp_val};    
    dev_addr = dev_addr ;    
   
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;
  
    return( 0 );
    
}


unsigned char DSP_SingleWrite_2_uart(unsigned char dev_addr,unsigned char dsp_addr,unsigned short dsp_val)
{

    unsigned char buf[] = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_DSP_WR_2, dsp_addr, (dsp_val>>8)&0xff, dsp_val&0xff};     
    dev_addr = dev_addr ;    
   
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;
  
    return( 0 ); 
    
}


unsigned char DSP_LegacyRead_uart(unsigned char dev_addr, unsigned char dsp_addr,unsigned char *pVal)
{  
    
    unsigned char buf[]    = { FM_CMD_SYN_0,FM_CMD_SYN_1,FM_CMD_DSP_RD, dsp_addr };
    unsigned char err_code = 0 ;
    dev_addr = dev_addr ;   
    
    DBGU_Read_NULL_RHR();
    
    DBGU_Write_Buffer( buf, sizeof(buf) ) ;        
    
    buf[2]  = FM_CMD_HOST_RD ;     
    buf[3]  = 0x4B ;    
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal++, 1000);    
    if(err_code != 0 ) {
      return( err_code ); 
    }
        
    buf[3] = 0x4A ;
    DBGU_Write_Buffer( buf, 4 ) ;      
    err_code = DBGU_Read_Byte( pVal,1000);  
    
    return( err_code ); 
  
}




 
    
/*****************     DM / PM / CM  MEM_Block_Read / Write [UART]   **********/

unsigned char MEM_Block_LegacyRead_uart( unsigned char  dev_addr, 
                                         unsigned char  mem_type, 
                                         unsigned short start_addr,
                                         unsigned char  num, 
                                         unsigned char *pVal     )
{
   
    unsigned char err_code  = 0 ;
    unsigned char i         = 0;
    unsigned int cmd[]      = { FM_CMD_DM_RD, FM_CMD_PM_RD, FM_CMD_CM_RD}; //cmd for read : DM, PM, CM 
    unsigned char buf[]     = { FM_CMD_SYN_0, FM_CMD_SYN_1, 0, 0, 0};  
    
    dev_addr = dev_addr ;  
    
    DBGU_Read_NULL_RHR();
    
    if( mem_type == MEM_TYPE_PM ) {      
        Unlock_PM(DSP_PM_Type);
    }    
        
    for(i = 0; i < num ; i++) { 
      
        buf[2]  =  cmd[mem_type] ;
        buf[3]  = (start_addr>>8)&0xFF;
        buf[4]  =  start_addr&0xFF ; 
        start_addr ++ ;
        DBGU_Write_Buffer( buf, 5 ) ;        
    
        buf[2]  = FM_CMD_HOST_RD ;     
        buf[3]  = 0x26 ;          
        DBGU_Write_Buffer( buf, 4 ) ;      
        err_code = DBGU_Read_Byte( pVal++, 1000);    
        if(err_code != 0 ) {
            return( err_code ); 
        }
        
        buf[3] = 0x25 ;
        DBGU_Write_Buffer( buf, 4 ) ;      
        err_code = DBGU_Read_Byte( pVal++,1000);  
        if(err_code != 0 ) {
            return( err_code ); 
        }
        
        if( mem_type == MEM_TYPE_PM ) {
            buf[3] = 0x24 ;
            DBGU_Write_Buffer( buf, 4 ) ;      
            err_code = DBGU_Read_Byte( pVal++,1000); 
            if(err_code != 0 ) {
                return( err_code ); 
            }
        }
    
    }
    
    return( err_code ); 
      
    
}


//write
unsigned char MEM_Block_SingleWrite_uart(   unsigned char dev_addr,                                             
                                            unsigned char mem_type, 
                                            unsigned short start_addr,
                                            unsigned char num, 
                                            unsigned char *pVal )
{
    
    unsigned char i            = 0;
    unsigned int cmd[]         = {FM_CMD_DM_RD, FM_CMD_PM_RD, FM_CMD_CM_RD}; //cmd for write : DM, PM, CM 
    unsigned char buf[]        = { FM_CMD_SYN_0, FM_CMD_SYN_1, 0, 0, 0, 0, 0, 0};
    unsigned char data_length  = 7;
    
    dev_addr = dev_addr ;    
    buf[2] = cmd[mem_type] ;
      
    for(i = 0; i < num ; i++)  {
        
        buf[3] = (start_addr>>8)&0xFF;
        buf[4] = start_addr&0xFF;
        buf[5] = *pVal++;
        buf[6] = *pVal++;
        if( mem_type == MEM_TYPE_PM ){
            buf[7] = *pVal++;
            data_length  = 8;
        }
        start_addr++ ;
        DBGU_Write_Buffer( buf, data_length ) ;
    }
    
    return( 0 );  

}
