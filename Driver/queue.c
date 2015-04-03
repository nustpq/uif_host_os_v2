/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2006-2008; Micrium, Inc.; Weston, FL
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
*                                         UART data buffering queue related
*
*                                          Atmel AT91SAM7A3
*                                                on the
*                                     Unified EVM Interface Board
*
* Filename      : queue.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

#include <includes.h>



/*********************************************************************************************************
** 函数名称: Queue_Create
** 功能描述: 初始化数据队列
** 输  　入: Buf      ：为队列分配的存储空间地址
**           SizeOfBuf：为队列分配的存储空间大小（字节）
**           ReadEmpty：为队列读空时处理程序
**           WriteFull：为队列写满时处理程序
** 输  　出: QUEUE_FAIL:参数错误
**           QUEUE_OK:成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
CPU_INT08U Queue_Create(void *Buf,  CPU_INT32U SizeOfBuf )
{
    DataQueue *Queue;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    if (Buf != NULL && SizeOfBuf >= (sizeof(DataQueue))) {       /* 判断参数是否有效 */
    
        Queue = (DataQueue *)Buf;

        OS_ENTER_CRITICAL();
                                                                /* 初始化结构体数据 */
        Queue->MaxData = (SizeOfBuf - (CPU_INT32U)(((DataQueue *)0)->Buf)) / 
                         sizeof(QUEUE_DATA_TYPE);               /* 计算队列可以存储的数据数目 */
        Queue->End = Queue->Buf + Queue->MaxData;               /* 计算数据缓冲的结束地址 */
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;
        
        OS_EXIT_CRITICAL();

        return QUEUE_OK;
        
    } else {
        return QUEUE_FAIL;
    }
}


/*********************************************************************************************************
** 函数名称: QueueRead
** 功能描述: 获取队列中的数据
** 输  　入: Ret:存储返回的消息的地址
**           Buf:指向队列的指针
** 输  　出: QUEUE_FAIL     ：参数错误
**           QUEUE_OK   ：收到消息
**           QUEUE_EMPTY：无消息
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
CPU_INT08U Queue_Read(QUEUE_DATA_TYPE *Ret, void *Buf)
{
    CPU_INT08U err;
    DataQueue *Queue;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    err = QUEUE_FAIL;
    
    if (Buf != NULL) {                                         /* 队列是否有效 */
                                                             /* 有效 */
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData > 0){                                   /* 队列是否为空 */
                                                                 /* 不空         */
            *Ret = Queue->Out[0];                               /* 数据出队     */
            Queue->Out++;                                       /* 调整出队指针 */
            if (Queue->Out >= Queue->End) {            
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* 数据减少      */
            err = QUEUE_OK;
            
        } else {                                               /* 空              */          
            err = QUEUE_EMPTY;        
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}

/*******************************************************************************/
CPU_INT08U Queue_ReadBuf( QUEUE_DATA_TYPE *Ret, void *Buf, CPU_INT16U bytes_copy, CPU_INT16U *bytes_copied)
{
    CPU_INT08U err;
    DataQueue *Queue;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    err = QUEUE_FAIL;
    
    if (Buf == NULL)  {                                         
         return err;
    }
    
    Queue = (DataQueue *)Buf; 
    
    OS_ENTER_CRITICAL();
        
    if (Queue->NData >= bytes_copy ) {                        
            *bytes_copied = bytes_copy ; 
            while( bytes_copy-- > 0 ){      
                *Ret++ = *(Queue->Out)++;          
                if (Queue->Out >= Queue->End)  {   //>= ??? Queue->End never used ?
                    Queue->Out = Queue->Buf;
                }
                Queue->NData--;
            }         
            err = QUEUE_OK;
        
    } else if (Queue->NData > 0) {                               
            *bytes_copied = Queue->NData ; 
            while( Queue->NData > 0 ) { 
                *Ret++ = *(Queue->Out)++;  
                Queue->NData--;
                if (Queue->Out >= Queue->End) {
                    Queue->Out = Queue->Buf;
                }            
            }
            err = QUEUE_OK;
        
    } else  {                                                      
            *bytes_copied = 0;
            err = QUEUE_EMPTY;      
    }
    
    OS_EXIT_CRITICAL();
   
    return err;
}
/*********************************************************************************************************
** 函数名称: QueueReadOnly
** 功能描述: 获取队列中的数据,但不改变原数据队列
** 输  　入: Ret:存储返回的消息的地址
**           Buf:指向队列的指针
** 输  　出: QUEUE_FAIL     ：参数错误
**           QUEUE_OK   ：收到消息
**           QUEUE_EMPTY：无消息
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
CPU_INT08U Queue_ReadOnly(QUEUE_DATA_TYPE *Ret, void *Buf)
{
    CPU_INT08U err;
    DataQueue *Queue;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    err = QUEUE_FAIL;
    if (Buf != NULL) {                                         /* 队列是否有效 */
                                                               /* 有效 */
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData > 0) {                                  /* 队列是否为空 */
                                                                /* 不空         */
            *Ret = Queue->Out[0];                               /* 数据出队     */
            err = QUEUE_OK;
            
        }  else  {                                                  
          
            err = QUEUE_EMPTY;
        }
        
        OS_EXIT_CRITICAL();
    }
    return err;
}

/*********************************************************************************************************
** 函数名称: Queue_Write
** 功能描述: FIFO方式发送数据
** 输  　入: Buf :指向队列的指针
**           Data:消息数据
** 输  　出: QUEUE_FAIL   :参数错误
**           QUEUE_FULL:队列满
**           QUEUE_OK  :发送成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_WRITE
#define EN_QUEUE_WRITE      0
#endif

#if EN_QUEUE_WRITE > 0

CPU_INT08U Queue_Write(void *Buf, QUEUE_DATA_TYPE Data)
{
    CPU_INT08U err;
    DataQueue *Queue;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    err = QUEUE_FAIL;
    if (Buf != NULL) {                                                    /* 队列是否有效 */
    
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData) {                           /* 队列是否满  */
                                                                      /* 不满        */
            Queue->In[0] = Data;                                        /* 数据入队    */
            Queue->In++;                                                /* 调整入队指针*/
            if (Queue->In >= Queue->End) {     
                Queue->In = Queue->Buf;
            }
            Queue->NData++;                                             /* 数据增加    */
            err = QUEUE_OK;
            
        } else {      
                                                                        /* 满           */
            err = QUEUE_FULL;       
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}


/******************************************************************************/

CPU_INT08U Queue_WriteBuf(QUEUE_DATA_TYPE *Ret, void *Buf, CPU_INT16U bytes_copy)
{
    CPU_INT08U err;
    DataQueue *Queue;

#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    err = QUEUE_FAIL;
    if (Buf == NULL | bytes_copy == 0)  {                                          /* 队列是否有效 */    
         return err;
    }    
    Queue = (DataQueue *)Buf;  
    err = QUEUE_OK;
    
    OS_ENTER_CRITICAL();  
     
    while( bytes_copy-- > 0 ){ 
              
         if(Queue->NData < Queue->MaxData) {   
                 *(Queue->In)++ = *Ret++ ; 
                 Queue->NData++ ;
                 if (Queue->In >= Queue->End)  {
                    Queue->In = Queue->Buf;
                 } 
                 
          } else { //full, discard the rest data
                 //Queue->Out++ ;
                 //Queue->NData-- ;
                 //*(Queue->In)++ = *Ret++ ;            
                 //if (Queue->In >= Queue->End)  {
                 //   Queue->In = Queue->Buf;
                 //} 
                 err = QUEUE_FULL;
           }
    }
    
    OS_EXIT_CRITICAL();

    return err;
}

#endif

/*********************************************************************************************************
** 函数名称: QueueWriteFront
** 功能描述: LIFO方式发送数据
** 输  　入: Buf:指向队列的指针
**           Data:消息数据
** 输　  出: QUEUE_FULL:队列满
**           QUEUE_OK:发送成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_WRITE_FRONT
#define EN_QUEUE_WRITE_FRONT    0
#endif

#if EN_QUEUE_WRITE_FRONT > 0

CPU_INT08U Queue_WriteFront(void *Buf, QUEUE_DATA_TYPE Data)
{
    CPU_INT08U err;
    DataQueue *Queue;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    err = QUEUE_FAIL;
    if (Buf != NULL)                                                    /* 队列是否有效 */
    {
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData)                              /* 队列是否满  */
        {                                                               /* 不满 */
            Queue->Out--;                                               /* 调整出队指针 */
            if (Queue->Out < Queue->Buf)
            {
                Queue->Out = Queue->End - 1;
            }
            Queue->Out[0] = Data;                                       /* 数据入队     */
            Queue->NData++;                                             /* 数据数目增加 */
            err = QUEUE_OK;
        }
        else
        {                                                               /* 满           */
            err = QUEUE_FULL;        
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}

#endif

/*********************************************************************************************************
** 函数名称: QueueNData
** 功能描述: 取得队列中数据数
** 输  　入: Buf:指向队列的指针
** 输  　出: 消息数
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_NDATA
#define EN_QUEUE_NDATA    0
#endif

#if EN_QUEUE_NDATA > 0

CPU_INT16U Queue_NData(void *Buf)
{
    CPU_INT16U temp;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    temp = 0;                                                   /* 队列无效返回0 */
    if (Buf != NULL)
    {
        OS_ENTER_CRITICAL();
        temp = ((DataQueue *)Buf)->NData;
        OS_EXIT_CRITICAL();
    }
    return temp;
}

#endif

/*********************************************************************************************************
** 函数名称: QueueSize
** 功能描述: 取得队列总容量
** 输  　入: Buf:指向队列的指针
** 输  　出: 队列总容量
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_SIZE
#define EN_QUEUE_SIZE    0
#endif

#if EN_QUEUE_SIZE > 0

CPU_INT16U Queue_Size(void *Buf)
{
    CPU_INT16U temp;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    temp = 0;                                                   /* 队列无效返回0 */
    if (Buf != NULL)
    {
        OS_ENTER_CRITICAL();
        temp = ((DataQueue *)Buf)->MaxData;
        OS_EXIT_CRITICAL();
    }
    return temp;
}

#endif

/*********************************************************************************************************
** 函数名称: OSQFlush
** 功能描述: 清空队列
** 输　  入: Buf:指向队列的指针
** 输  　出: 无
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_FLUSH
#define EN_QUEUE_FLUSH    0
#endif

#if EN_QUEUE_FLUSH > 0

void Queue_Flush(void *Buf)
{
    DataQueue *Queue;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    if (Buf != NULL )                                                /* 队列是否有效 */
    {                                                               /* 有效         */
        Queue = (DataQueue *)Buf;
        OS_ENTER_CRITICAL();
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;                                           /* 数据数目为0 */
        OS_EXIT_CRITICAL();
    }
}

#endif












/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
