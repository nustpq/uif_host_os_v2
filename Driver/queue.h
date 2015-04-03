/****************************************Copyright (c)**************************************************
**                              
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
** �ļ���: queue.h
** ������:  
** ��  ��:  
** ��  ��: ���ݶ���ͷ�ļ�
**
**--------------��ǰ�汾�޶�-----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#define EN_QUEUE_WRITE            1     /* ��ֹ(0)������(1)FIFO��������       */
#define EN_QUEUE_WRITE_FRONT      1     /* ��ֹ(0)������(1)LIFO��������       */
#define EN_QUEUE_NDATA            1     /* ��ֹ(0)������(1)ȡ�ö���������Ŀ   */
#define EN_QUEUE_SIZE             1     /* ��ֹ(0)������(1)ȡ�ö������������� */
#define EN_QUEUE_FLUSH            1     /* ��ֹ(0)������(1)��ն���           */

#define QUEUE_FULL          8                           /* ������                                       */
#define QUEUE_EMPTY         4                           /* ������                                       */
#define QUEUE_OK            0                           /* �����ɹ�                                     */
#define QUEUE_FAIL          1                           /* ��������                                     */

#define Q_WRITE_MODE        1                           /* �����ɹ�                                     */
#define Q_WRITE_FRONT_MODE  2                           /* �����ɹ�                                     */

#ifndef QUEUE_DATA_TYPE
#define QUEUE_DATA_TYPE     CPU_INT08U
#endif

typedef struct {
  
    QUEUE_DATA_TYPE     *Out;                   /* ָ���������λ��         */
    QUEUE_DATA_TYPE     *In;                    /* ָ����������λ��         */
    QUEUE_DATA_TYPE     *End;                   /* ָ��Buf�Ľ���λ��        */
    CPU_INT16U           NData;                  /* ���������ݸ���           */
    CPU_INT16U           MaxData;                /* ����������洢�����ݸ��� */
    QUEUE_DATA_TYPE      Buf[1];                 /* �洢���ݵĿռ�           */
    
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
