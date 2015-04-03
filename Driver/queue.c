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
** ��������: Queue_Create
** ��������: ��ʼ�����ݶ���
** ��  ����: Buf      ��Ϊ���з���Ĵ洢�ռ��ַ
**           SizeOfBuf��Ϊ���з���Ĵ洢�ռ��С���ֽڣ�
**           ReadEmpty��Ϊ���ж���ʱ�������
**           WriteFull��Ϊ����д��ʱ�������
** ��  ����: QUEUE_FAIL:��������
**           QUEUE_OK:�ɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
CPU_INT08U Queue_Create(void *Buf,  CPU_INT32U SizeOfBuf )
{
    DataQueue *Queue;
    
#if OS_CRITICAL_METHOD == 3u   /* Allocate storage for CPU status register   */
    OS_CPU_SR  cpu_sr = 0u;
#endif
    
    if (Buf != NULL && SizeOfBuf >= (sizeof(DataQueue))) {       /* �жϲ����Ƿ���Ч */
    
        Queue = (DataQueue *)Buf;

        OS_ENTER_CRITICAL();
                                                                /* ��ʼ���ṹ������ */
        Queue->MaxData = (SizeOfBuf - (CPU_INT32U)(((DataQueue *)0)->Buf)) / 
                         sizeof(QUEUE_DATA_TYPE);               /* ������п��Դ洢��������Ŀ */
        Queue->End = Queue->Buf + Queue->MaxData;               /* �������ݻ���Ľ�����ַ */
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
** ��������: QueueRead
** ��������: ��ȡ�����е�����
** ��  ����: Ret:�洢���ص���Ϣ�ĵ�ַ
**           Buf:ָ����е�ָ��
** ��  ����: QUEUE_FAIL     ����������
**           QUEUE_OK   ���յ���Ϣ
**           QUEUE_EMPTY������Ϣ
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    
    if (Buf != NULL) {                                         /* �����Ƿ���Ч */
                                                             /* ��Ч */
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData > 0){                                   /* �����Ƿ�Ϊ�� */
                                                                 /* ����         */
            *Ret = Queue->Out[0];                               /* ���ݳ���     */
            Queue->Out++;                                       /* ��������ָ�� */
            if (Queue->Out >= Queue->End) {            
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* ���ݼ���      */
            err = QUEUE_OK;
            
        } else {                                               /* ��              */          
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
** ��������: QueueReadOnly
** ��������: ��ȡ�����е�����,�����ı�ԭ���ݶ���
** ��  ����: Ret:�洢���ص���Ϣ�ĵ�ַ
**           Buf:ָ����е�ָ��
** ��  ����: QUEUE_FAIL     ����������
**           QUEUE_OK   ���յ���Ϣ
**           QUEUE_EMPTY������Ϣ
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    if (Buf != NULL) {                                         /* �����Ƿ���Ч */
                                                               /* ��Ч */
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData > 0) {                                  /* �����Ƿ�Ϊ�� */
                                                                /* ����         */
            *Ret = Queue->Out[0];                               /* ���ݳ���     */
            err = QUEUE_OK;
            
        }  else  {                                                  
          
            err = QUEUE_EMPTY;
        }
        
        OS_EXIT_CRITICAL();
    }
    return err;
}

/*********************************************************************************************************
** ��������: Queue_Write
** ��������: FIFO��ʽ��������
** ��  ����: Buf :ָ����е�ָ��
**           Data:��Ϣ����
** ��  ����: QUEUE_FAIL   :��������
**           QUEUE_FULL:������
**           QUEUE_OK  :���ͳɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    if (Buf != NULL) {                                                    /* �����Ƿ���Ч */
    
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData) {                           /* �����Ƿ���  */
                                                                      /* ����        */
            Queue->In[0] = Data;                                        /* �������    */
            Queue->In++;                                                /* �������ָ��*/
            if (Queue->In >= Queue->End) {     
                Queue->In = Queue->Buf;
            }
            Queue->NData++;                                             /* ��������    */
            err = QUEUE_OK;
            
        } else {      
                                                                        /* ��           */
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
    if (Buf == NULL | bytes_copy == 0)  {                                          /* �����Ƿ���Ч */    
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
** ��������: QueueWriteFront
** ��������: LIFO��ʽ��������
** ��  ����: Buf:ָ����е�ָ��
**           Data:��Ϣ����
** �䡡  ��: QUEUE_FULL:������
**           QUEUE_OK:���ͳɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    if (Buf != NULL)                                                    /* �����Ƿ���Ч */
    {
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData)                              /* �����Ƿ���  */
        {                                                               /* ���� */
            Queue->Out--;                                               /* ��������ָ�� */
            if (Queue->Out < Queue->Buf)
            {
                Queue->Out = Queue->End - 1;
            }
            Queue->Out[0] = Data;                                       /* �������     */
            Queue->NData++;                                             /* ������Ŀ���� */
            err = QUEUE_OK;
        }
        else
        {                                                               /* ��           */
            err = QUEUE_FULL;        
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}

#endif

/*********************************************************************************************************
** ��������: QueueNData
** ��������: ȡ�ö�����������
** ��  ����: Buf:ָ����е�ָ��
** ��  ����: ��Ϣ��
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    
    temp = 0;                                                   /* ������Ч����0 */
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
** ��������: QueueSize
** ��������: ȡ�ö���������
** ��  ����: Buf:ָ����е�ָ��
** ��  ����: ����������
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    
    temp = 0;                                                   /* ������Ч����0 */
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
** ��������: OSQFlush
** ��������: ��ն���
** �䡡  ��: Buf:ָ����е�ָ��
** ��  ����: ��
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    
    if (Buf != NULL )                                                /* �����Ƿ���Ч */
    {                                                               /* ��Ч         */
        Queue = (DataQueue *)Buf;
        OS_ENTER_CRITICAL();
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;                                           /* ������ĿΪ0 */
        OS_EXIT_CRITICAL();
    }
}

#endif












/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
