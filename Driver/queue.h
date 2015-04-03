/****************************************Copyright (c)**************************************************
**                              
**
**--------------文件信息--------------------------------------------------------------------------------
** 文件名: queue.h
** 创建人:  
** 日  期:  
** 描  述: 数据队列头文件
**
**--------------当前版本修订-----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#define EN_QUEUE_WRITE            1     /* 禁止(0)或允许(1)FIFO发送数据       */
#define EN_QUEUE_WRITE_FRONT      1     /* 禁止(0)或允许(1)LIFO发送数据       */
#define EN_QUEUE_NDATA            1     /* 禁止(0)或允许(1)取得队列数据数目   */
#define EN_QUEUE_SIZE             1     /* 禁止(0)或允许(1)取得队列数据总容量 */
#define EN_QUEUE_FLUSH            1     /* 禁止(0)或允许(1)清空队列           */

#define QUEUE_FULL          8                           /* 队列满                                       */
#define QUEUE_EMPTY         4                           /* 无数据                                       */
#define QUEUE_OK            0                           /* 操作成功                                     */
#define QUEUE_FAIL          1                           /* 参数错误                                     */

#define Q_WRITE_MODE        1                           /* 操作成功                                     */
#define Q_WRITE_FRONT_MODE  2                           /* 操作成功                                     */

#ifndef QUEUE_DATA_TYPE
#define QUEUE_DATA_TYPE     CPU_INT08U
#endif

typedef struct {
  
    QUEUE_DATA_TYPE     *Out;                   /* 指向数据输出位置         */
    QUEUE_DATA_TYPE     *In;                    /* 指向数据输入位置         */
    QUEUE_DATA_TYPE     *End;                   /* 指向Buf的结束位置        */
    CPU_INT16U           NData;                  /* 队列中数据个数           */
    CPU_INT16U           MaxData;                /* 队列中允许存储的数据个数 */
    QUEUE_DATA_TYPE      Buf[1];                 /* 存储数据的空间           */
    
} DataQueue;



extern CPU_INT08U Queue_Create (void *Buf, CPU_INT32U SizeOfBuf );

extern CPU_INT08U Queue_Read(QUEUE_DATA_TYPE *Ret, void *Buf);
extern CPU_INT08U Queue_ReadBuf(QUEUE_DATA_TYPE *Ret, void *Buf, CPU_INT16U bytes_copy, CPU_INT16U *bytes_copied);
extern CPU_INT08U Queue_ReadOnly(QUEUE_DATA_TYPE *Ret, void *Buf);
    
extern CPU_INT08U Queue_Write(void *Buf, QUEUE_DATA_TYPE Data);
extern CPU_INT08U Queue_WriteBuf(QUEUE_DATA_TYPE *Ret, void *Buf, CPU_INT16U bytes_copy);
extern CPU_INT08U Queue_WriteFront(void *Buf, QUEUE_DATA_TYPE Data);

extern CPU_INT16U Queue_NData(void *Buf);
extern CPU_INT16U Queue_Size(void *Buf);

extern void       Queue_Flush(void *Buf);




/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
