/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <includes.h>
#include "i2c_gpio.h" 



#define GPIO_I2C_DELAY  1  // 1ms timeout @ 48MHz MIP

#define GPIO_CS    5                         //GPIO pin for iM204 CS pin 
#define GPIO_SDA   6                         //GPIO pin for I2C SDA  
#define GPIO_SCL   7                         //GPIO pin for I2C SCL  
  
                                                             
static void delay( void )
{
    
    //unsigned int i = GPIO_I2C_DELAY;
    //while(i--);
    
}


  
static void i2c_start( void )  
{         
    GPIOPIN_Set_Fast( GPIO_SDA, 1 );
    GPIOPIN_Set_Fast( GPIO_SCL, 1 );    
    delay();      
   
    GPIOPIN_Set_Fast( GPIO_SDA, 0);
    delay();    
    
}  

                                                              

static void i2c_restart()
{
     GPIOPIN_Set_Fast( GPIO_SCL, 0 );     
     GPIOPIN_Set_Fast( GPIO_SDA, 1 );
     delay(); 
     GPIOPIN_Set_Fast( GPIO_SCL, 1 );
     delay();
     GPIOPIN_Set_Fast( GPIO_SDA, 0 );
     delay();     
}


static void i2c_stop( void )  
{  
    
    GPIOPIN_Set_Fast( GPIO_SCL, 0);               
    GPIOPIN_Set_Fast( GPIO_SDA, 0 );
    delay();
    GPIOPIN_Set_Fast( GPIO_SCL, 1);    
    delay();    
    GPIOPIN_Set_Fast( GPIO_SDA, 1 );
    delay();    
} 



static unsigned char i2c_read_ack( void )  
{  
    
    unsigned char data = 1; 
    
    GPIOPIN_Set_Fast( GPIO_SCL, 0); 
    GPIOPIN_Set_Fast( GPIO_SDA, 2 );    
               
    delay();
    GPIOPIN_Set_Fast( GPIO_SCL, 1); 
    
    GPIOPIN_Get_Fast(GPIO_SDA, &data);   
    delay();
    
    return data; 
    
}  



static void i2c_send_ack( void )  
{  
    GPIOPIN_Set_Fast( GPIO_SCL, 0);      
    GPIOPIN_Set_Fast( GPIO_SDA, 0 );            
    delay();  
    GPIOPIN_Set_Fast( GPIO_SCL, 1);             
    delay();     
}  


  
static unsigned char i2c_write_byte(unsigned char data)  
{  
    
    int i;
    unsigned char state;  
  
    for (i=7; i>=0; i--) {  
        GPIOPIN_Set_Fast( GPIO_SCL, 0);
        GPIOPIN_Set_Fast( GPIO_SDA, data & (1<<i) ? 1: 0 );        //从高位到低位依次准备数据进行发送        
        delay(); 
        GPIOPIN_Set_Fast( GPIO_SCL, 1);                    
        delay(); 
    }  
    state = i2c_read_ack();       
    return state;
    
}  

 
static unsigned char i2c_read_byte( void )  
{  
    int i;  
    unsigned char temp, data = 0;  
    
    GPIOPIN_Set_Fast( GPIO_SCL, 0);
    GPIOPIN_Set_Fast( GPIO_SDA, 2 );
    
    for (i=7; i>=0; i--) {  
        GPIOPIN_Set_Fast( GPIO_SCL, 0); 
        delay(); 
        GPIOPIN_Set_Fast( GPIO_SCL, 1); 
        GPIOPIN_Get_Fast(GPIO_SDA, &temp);
        data = (data <<1) | temp;      //从高位到低位依次准备数据进行读取                 
        delay();  
    }      
    i2c_send_ack();                 //向目标设备发送ACK信号  
    
    return data;  
} 




 

unsigned char I2C_GPIO_Read(unsigned char addr, unsigned char* buf, unsigned int len)  
{  
      unsigned int i;
      unsigned char state, data;  

#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif 
          
      OS_ENTER_CRITICAL();
      
      i2c_start();      //起始条件，开始数据通信  
      //发送地址和数据读写方向  
      data = (addr << 1) | 1;                    //低位为1，表示读数据  
      state = i2c_write_byte(data);
      if(state != 0) {
         OS_EXIT_CRITICAL();
         return 1;
      }
      //读入数据  
      for (i=0; i<len; i++) { 
          buf[i] = i2c_read_byte(); 
      }
      i2c_stop();                     //终止条件，结束数据通信 
      
      OS_EXIT_CRITICAL();
      
      return 0;
}  


unsigned char I2C_GPIO_Read_iM205(unsigned char addr, unsigned char reg, unsigned char *val)
{
    
      unsigned char state, data;  

#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif 
       
      APP_TRACE_INFO(("\r\nI2C_GPIO_Read_iM205(0x%0X,0x%0X,)", addr, reg));

      OS_ENTER_CRITICAL();
      
      i2c_start();      //起始条件，开始数据通信      
      //发送地址和数据读写方向  
      data = (addr << 1) | 0;                    //低位为0，表示写数据  
      state = i2c_write_byte(data);  
      if(state != 0) {
           OS_EXIT_CRITICAL();
           //APP_TRACE_INFO(("\r\nwrite byte err1!"));
           return 1;
      }

     
      //写入数据  
      state = i2c_write_byte( reg ); 
      if(state != 0) {
           OS_EXIT_CRITICAL();
           //APP_TRACE_INFO(("\r\nwrite byte err2!"));
           return 1;
      }
      
      i2c_restart();
      //发送地址和数据读写方向  
      data = (addr << 1) | 1;                    //低位为1，表示读数据  
      state = i2c_write_byte(data);
      if(state != 0) {
         OS_EXIT_CRITICAL();
         //APP_TRACE_INFO(("\r\n read byte err3!"));
         return 1;
      }
      //读入数据       
      *val = i2c_read_byte(); 
      
      i2c_stop();                     //终止条件，结束数据通信 
      
      OS_EXIT_CRITICAL();
      
      return 0;
    
}
/*  
I2C写操作  
addr：目标设备地址  
buf：写缓冲区  
len：写入字节的长度  
*/  
unsigned char I2C_GPIO_Write (unsigned char addr, unsigned char* buf, unsigned int len)  
{  
    
    unsigned int i;  
    unsigned char state, data;  
    
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif 
        
       
    OS_ENTER_CRITICAL();    
    i2c_start();                        //起始条件，开始数据通信  
    //发送地址和数据读写方向  
    data = (addr << 1) | 0;                    //低位为0，表示写数据  
    state = i2c_write_byte(data);  
    if(state != 0) {
         OS_EXIT_CRITICAL();
         return 1;
    }
    //写入数据  
    for (i=0; i<len; i++) { 
       state = i2c_write_byte(buf[i]); 
       if(state != 0) {
           OS_EXIT_CRITICAL();
           return 1;
       }
    }
    i2c_stop();                     //终止条件，结束数据通信
    
    OS_EXIT_CRITICAL();
   
    return 0;
} 


unsigned char I2C_GPIO_Write_iM205 (unsigned char addr, unsigned char reg, unsigned char val ) 
{
    
    unsigned char state, data;  
    
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif 
        
    APP_TRACE_INFO(("\r\nI2C_GPIO_Write_iM205(0x%0X,0x%0X, 0x%0X)", addr, reg, val));
   
    OS_ENTER_CRITICAL();    
    i2c_start();                        //起始条件，开始数据通信  
    //发送地址和数据读写方向  
    data = (addr << 1) | 0;                    //低位为0，表示写数据  
    state = i2c_write_byte(data);  
    if(state != 0) {
         OS_EXIT_CRITICAL();
         //APP_TRACE_INFO(("\r\n write byte err1!"));
         return 1;
    }
      
    
    //写入数据  
    state = i2c_write_byte( reg ); 
    if(state != 0) {
         OS_EXIT_CRITICAL();
         //APP_TRACE_INFO(("\r\n write byte err2!"));
         return 1;
    }
    
    i2c_restart();
    //发送地址和数据读写方向  
    data = (addr << 1) | 0;                    //低位为0，表示写数据  
    state = i2c_write_byte(data);  
    if(state != 0) {
         OS_EXIT_CRITICAL();
         //APP_TRACE_INFO(("\r\n write byte err3!"));
         return 1;
    }
    
    //写入数据  
    state = i2c_write_byte( val ); 
    if(state != 0) {
         OS_EXIT_CRITICAL();
         //APP_TRACE_INFO(("\r\n write byte err4!"));
         return 1;
    }
    
    i2c_stop();                     //终止条件，结束数据通信
    
    OS_EXIT_CRITICAL();
   
    return 0;      
     
}




void I2C_GPIO_Init ( unsigned int speed )  
{ 
    
    GPIOPIN_Init_Fast( GPIO_SDA );
    GPIOPIN_Init_Fast( GPIO_SCL );
    GPIOPIN_Set_Fast( GPIO_SDA, 1 );
    GPIOPIN_Set_Fast( GPIO_SCL, 1 );
    
}



////////////   iM204 Mode/Gain Control via CS pin ///////////

unsigned char CS_GPIO_Write (unsigned char mode_toggle_num, unsigned char gain_toggle_num)  
{  
    
    unsigned int i;   
    
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif 
        
       
    OS_ENTER_CRITICAL();    
  
    for ( i=0; i< mode_toggle_num; i++ ) {  
        GPIOPIN_Set_Fast( GPIO_CS, 1);
        delay(); 
        GPIOPIN_Set_Fast( GPIO_CS, 0);                    
        delay(); 
    }
        
    OS_EXIT_CRITICAL();
    
    OSTimeDly(10);  //10ms quiet period between 
    
    OS_ENTER_CRITICAL();    
  
    for ( i=0; i< gain_toggle_num; i++ ) {  
        GPIOPIN_Set_Fast( GPIO_CS, 1);
        delay(); 
        GPIOPIN_Set_Fast( GPIO_CS, 0);                    
        delay(); 
    }
        
    OS_EXIT_CRITICAL();
   
    return 0;
}


void CS_GPIO_Init ( void )  
{ 
    
    GPIOPIN_Init_Fast( GPIO_CS );   
    GPIOPIN_Set_Fast(  GPIO_CS, 0 );
    
}