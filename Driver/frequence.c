#include <stdio.h>
#include <string.h>
#include <usart.h>
#include <pio.h>
#include "frequence.h"


#define FREQUENCY_CMD_DELAY_MS   50 //10ms delay

static const char FuncShape[][4] = {  "SIN", "SQU", "TRI"  };



static CPU_INT08U Freq_Gen_Adj_TypeA( void *pdata ) // for F80-A
{
  
	    FRQADJ   *FrqAdj = (FRQADJ *)pdata;
        CPU_FP32  temp ;         
            
        //check power ack
        memset(combuf, 0, PDC_LEN);
        USART_SendByte(SIGNAL_POWER_UART,0x0D);           
        USART_SendStr(SIGNAL_POWER_UART,"*IDN?");
        USART_SendByte(SIGNAL_POWER_UART,0x00);
        if( USART_Read_Timeout(SIGNAL_POWER_UART,combuf,PDC_LEN,1000) != 0) {          
            return (FRQ_MODULE_ERR); 
            
        }
        if( strstr((const char*)combuf,"SAMPLE") == NULL  )   {          
            return (FRQ_MODULE_ERR);  
            
        }        
        USART_SendByte(SIGNAL_POWER_UART,0x0D);  
	    sprintf( (char *)combuf,"FUNC:SHAP %s",FuncShape[FrqAdj->FrqType]);       
        USART_SendStr(SIGNAL_POWER_UART, combuf);  
        USART_SendByte(SIGNAL_POWER_UART,0x00); 
        OSTimeDly( FREQUENCY_CMD_DELAY_MS );
        
        //choose FM 
        USART_SendByte(SIGNAL_POWER_UART,0x0D); 
        sprintf( (char *)combuf,"FM:STAT ON" );        
        USART_SendStr(SIGNAL_POWER_UART, combuf);      
        USART_SendByte(SIGNAL_POWER_UART,0x00);
        OSTimeDly( FREQUENCY_CMD_DELAY_MS );
            
        USART_SendByte(SIGNAL_POWER_UART,0x0D);  
	    sprintf( (char *)combuf,"FREQ %d HZ",FrqAdj->Frq);       
        USART_SendStr(SIGNAL_POWER_UART, combuf);  
        USART_SendByte(SIGNAL_POWER_UART,0x00);
        OSTimeDly( FREQUENCY_CMD_DELAY_MS );
         
        USART_SendByte(SIGNAL_POWER_UART,0x0D);
        temp = ((float)FrqAdj->Volt)/1000/2 ; 
        sprintf( (char *)combuf,"VOLT %f",temp);	
        USART_SendStr(SIGNAL_POWER_UART, combuf); 
        USART_SendByte(SIGNAL_POWER_UART,0x00);
        OSTimeDly( FREQUENCY_CMD_DELAY_MS );        
        
        USART_SendByte(SIGNAL_POWER_UART,0x0D);
        temp /= 2 ;
        sprintf( (char *)combuf,"VOLT:OFFSET %f",temp);	
        USART_SendStr(SIGNAL_POWER_UART, combuf); 
        USART_SendByte(SIGNAL_POWER_UART,0x00);
        
        OSTimeDly(200);        
        //check applied ?
        USART_SendByte(SIGNAL_POWER_UART,0x0D);      
        USART_SendStr(SIGNAL_POWER_UART,"APPL?");
        USART_SendByte(SIGNAL_POWER_UART,0x00);
        if( USART_Read_Timeout(SIGNAL_POWER_UART,combuf,PDC_LEN,1000) != 0 )  {          
            return (FRQ_MODULE_ERR); 
            
        } 
        if( strstr((const char *)&combuf[1],FuncShape[FrqAdj->FrqType] ) == NULL )  {          
            return (FRQ_MODULE_ERR);  
            
        }     
        
        return 0 ;
}



static CPU_INT08U Freq_Gen_Adj_TypeB( void *pdata )// for F80-B and F40
{
  
	    FRQADJ   *FrqAdj = (FRQADJ *)pdata;
        CPU_FP32  temp ;         
        unsigned int voltage_div    = 2 ;
        unsigned int delay_ms_add   = 0 ; 
        
        memset(combuf, 0, PDC_LEN);   
        USART_SendStr(SIGNAL_POWER_UART,"*IDN?");
        USART_SendByte(SIGNAL_POWER_UART,0x0A);
        if( USART_Read_Timeout(SIGNAL_POWER_UART,combuf,PDC_LEN,1000) != 0 )  {          
            return (FRQ_MODULE_ERR); 
            
        }        
        if(  strstr( (const char *)combuf,"SAMPLE") == NULL  )  {   
            if( strstr( (const char *)combuf,"SHENGPU")== NULL )  {
               return (FRQ_MODULE_ERR);     
               
            } else  {
               voltage_div    = 2;   
               delay_ms_add   = 50; //add ms delay for F40
               
            }
            
        }            
	    sprintf( (char *)combuf,"FUNC:SHAP %s",FuncShape[FrqAdj->FrqType]);       
        USART_SendStr(SIGNAL_POWER_UART, combuf);  
        USART_SendByte(SIGNAL_POWER_UART,0x0A);
        OSTimeDly( FREQUENCY_CMD_DELAY_MS );
        
         //choose FM  
        sprintf( (char *)combuf,"FM:STAT ON" );        
        USART_SendStr(SIGNAL_POWER_UART, combuf);      
        USART_SendByte(SIGNAL_POWER_UART,0x0A);
        OSTimeDly( FREQUENCY_CMD_DELAY_MS );
         
	    sprintf( (char *)combuf,"FREQ %d HZ",FrqAdj->Frq);       
        USART_SendStr(SIGNAL_POWER_UART, combuf);  
        USART_SendByte(SIGNAL_POWER_UART,0x0A);        
        OSTimeDly( FREQUENCY_CMD_DELAY_MS );
        OSTimeDly( delay_ms_add ) ;
         
        temp = ((float)FrqAdj->Volt)/1000/voltage_div ; 
        sprintf( (char *)combuf,"VOLT %f",temp);	
        USART_SendStr(SIGNAL_POWER_UART, combuf); 
        USART_SendByte(SIGNAL_POWER_UART,0x0A);        
        OSTimeDly( FREQUENCY_CMD_DELAY_MS );
        OSTimeDly( delay_ms_add ) ;
                 
        temp /= 2 ;
        sprintf( (char *)combuf,"VOLT:OFFSET %f",temp);	
        USART_SendStr(SIGNAL_POWER_UART, combuf); 
        USART_SendByte(SIGNAL_POWER_UART,0x0A);        
        OSTimeDly(200);
          
        USART_SendStr(SIGNAL_POWER_UART,"APPL?");
        USART_SendByte(SIGNAL_POWER_UART,0x0A);  
        if( USART_Read_Timeout(SIGNAL_POWER_UART,combuf,PDC_LEN,1000) != 0 ) {          
            return (FRQ_MODULE_ERR); 
            
        }    
        if( strstr( (const char *)&combuf[0],FuncShape[FrqAdj->FrqType] ) == NULL )  {          
            return (FRQ_MODULE_ERR); 
            
        } 
        // printf(">>>>Set Frequency GOOD!"); 
  
        return (0) ;
}


//There are two types of Function generator that need different commands
CPU_INT08U  CTR_FrqAdj( void *pdata )
{
  
#if 1
    CPU_INT08U status;
    PIO_Set( &pinsControls[0] );//choose USRT0 to signal generator 
    
    status = Freq_Gen_Adj_TypeA(pdata);    
    if(status != 0)
    {   
       status = Freq_Gen_Adj_TypeB(pdata); 
    }
    
    return status;
    
#else   //Freq control disabled
    
    return (FRQ_MODULE_ERR);
    
#endif
  
}


 
CPU_INT08U CTR_FrqCtr( void *pdata )
{
        /*
        FRQCTR *FrqCtr = (FRQCTR *)pdata;
        PIO_Set( UART0_SEL );//choose USRT0 to signal generator 
  
        USART_SendByte(SIGNAL_POWER_UART,0x0D);       
        sprintf( combuf,"FM:STAT %s",FuncOnoff[FrqCtr->Ctr] );        
        USART_SendStr(SIGNAL_POWER_UART, combuf);      
        USART_SendByte(SIGNAL_POWER_UART,0x00);       
   
        return 0 ; 
        */ 
  
        // not support onoff control so far
        return (FRQ_MODULE_NOT_EXIST);
        
}

