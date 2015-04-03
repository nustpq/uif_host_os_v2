#include <stdio.h>
#include <string.h>
#include <usart.h>
#include <pio.h>
#include "power.h"



CPU_INT08U combuf[PDC_LEN];

static Pin pinsPowerCtr[] = { PWR_CTRL1,    PWR_CTRL2,   PWR_CTRL3 };



#define POWER_SUPPLY_CMD_DELAY_MS   50 //50ms delay

CPU_INT08U CTR_PowerAdj( void *pdata )
{  
 
	    POWERADJ  *PowerAdj = (POWERADJ *)pdata ;
        PIO_Clear( &pinsControls[0] );//choose USRT0 to power supply   
        
        if( (PowerAdj->Ch > 3) || (PowerAdj->Ch < 1) )  {		
          return (POWER_MODULE_NOT_EXIST);   
        }        
     
        memset(combuf, 0, PDC_LEN); //clear buf
        USART_SendStr(SIGNAL_POWER_UART,"*IDN?\r\n");
        if( USART_Read_Timeout(SIGNAL_POWER_UART,combuf,PDC_LEN,1000) != 0 )  {           
          return (POWER_MODULE_ERR + 0x10); 
        }        
        if( (unsigned char *)strstr((const char*)combuf,"IT6322") == NULL ) {     
          return (POWER_MODULE_ERR + 0x20);
        }
        OSTimeDly( POWER_SUPPLY_CMD_DELAY_MS );
        sprintf((char *)combuf,"INST:NSEL %d\r\n",PowerAdj->Ch); 		
        USART_SendStr(SIGNAL_POWER_UART, combuf);  
        OSTimeDly( POWER_SUPPLY_CMD_DELAY_MS );
        
        //check power ack 
        memset(combuf, 0, PDC_LEN);
        USART_SendStr(SIGNAL_POWER_UART,"INST:NSEL?\r\n");        
        if( USART_Read_Timeout(SIGNAL_POWER_UART,combuf,PDC_LEN,1000) != 0 )  {           
            return (POWER_MODULE_ERR + 0x30); 
          
        }       
        if(  combuf[1] != PowerAdj->Ch + 0x30 )  {
            return (POWER_MODULE_ERR + 0x40);
                
        }            
 
        OSTimeDly( POWER_SUPPLY_CMD_DELAY_MS );
       
	    sprintf((char *)combuf,"VOLT %dmV\r\n",PowerAdj->Voltage);     
        USART_SendStr(SIGNAL_POWER_UART, combuf);
        OSTimeDly( POWER_SUPPLY_CMD_DELAY_MS );
        sprintf((char *)combuf,"CURR 100mA\r\n");	
        USART_SendStr(SIGNAL_POWER_UART, combuf);  
        OSTimeDly( POWER_SUPPLY_CMD_DELAY_MS );
        sprintf((char *)combuf,"OUTP %d\r\n",1);
  	    USART_SendStr(SIGNAL_POWER_UART, combuf);
        
        OSTimeDly(200);
        //printf("\r\n>>Set power OK!");  
        
        return 0 ;
}





/********************** Power On/Off Control****************************************/

#if( false )   // use power supply inner on off commands------------------------

static CPU_INT08U doPowerControl( CPU_INT08U channel,  CPU_INT08U onoff )  
{
    PIO_Clear( &pinsControls[0] );//choose USRT0 to power supply  
    
    sprintf(combuf,"INST:NSEL %d\r\n",channel); 		
    USART_SendStr(SIGNAL_POWER_UART, combuf);
    OSTimeDly( POWER_SUPPLY_CMD_DELAY_MS );
    //check power ack
    USART_SendStr(SIGNAL_POWER_UART,"INST:NSEL?\r\n");
    if( USART_Read_Timeout(SIGNAL_POWER_UART,combuf,PDC_LEN,1000) != 0 ) {
      
            return (POWER_MODULE_ERR + 0x70);  
    }
    if(  combuf[0] != channel + 0x30 ) {
      
            return (POWER_MODULE_ERR + 0x80); 
    } 
    OSTimeDly( POWER_SUPPLY_CMD_DELAY_MS );
    sprintf(combuf,"OUTP %d\r\n",onoff);
    USART_SendStr(SIGNAL_POWER_UART, combuf); 
    return 0 ;
}


CPU_INT08U CTR_PowerCtr( void *pdata )
{
    unsigned char result;
    POWERCTR  *PowerCtr = (POWERCTR  *)pdata ; 
    
    if(PowerCtr->define & 0xf0){
      
        return (POWER_MODULE_NOT_EXIST);
    }    
    
    if (PowerCtr->define & 0x02) {
      
        result = doPowerControl(1, PowerCtr->Ctr & 0x02 );
        if(  result != 0 )
            return result;
         
    }
    if(PowerCtr->define & 0x04){
     
        result = doPowerControl(2, PowerCtr->Ctr & 0x04 );
        if(  result != 0 )
            return result;
    }
    if(PowerCtr->define & 0x08){
 
        result = doPowerControl(3, PowerCtr->Ctr & 0x08 );
        if(  result != 0 )
            return result;
    }    
   
           
    return(0) ;
}

#else  //use on board MOS switcher on off control-------------------------------

CPU_INT08U CTR_PowerCtr( void *pdata )
{   

    POWERCTR  *PowerCtr = (POWERCTR  *)pdata ; 
    
    if(PowerCtr->define & 0xf0){
      
        return (POWER_MODULE_NOT_EXIST);
    }      
    if (PowerCtr->define & 0x02) {
       
        if (PowerCtr->Ctr & 0x02){
          
           PIO_Clear(&pinsPowerCtr[0]);                   
        }
        else{
          
           PIO_Set(&pinsPowerCtr[0]);         
        }
        
    }    
    if (PowerCtr->define & 0x04) {
       
        if (PowerCtr->Ctr & 0x04){
          
           PIO_Clear(&pinsPowerCtr[1]);	
        }
        else{
          
           PIO_Set(&pinsPowerCtr[1]);
        }

    }    
    if (PowerCtr->define & 0x08) {
       
        if (PowerCtr->Ctr & 0x08){
          
           PIO_Clear(&pinsPowerCtr[2]);                     
        }
        else{
          
           PIO_Set(&pinsPowerCtr[2]);
        }
    } 
               
    return(0) ;   
}



#endif


