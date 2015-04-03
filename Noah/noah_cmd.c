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
*                                        COMMUNICATION COMMANDS REALIZATION
*
*                                          Atmel AT91SAM7A3
*                                               on the
*                                      Unified EVM Interface Board
*
* Filename      : noah_cmd.c
* Version       : V1.0.0
* Programmer(s) : PQ
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

#include <includes.h>


#define MAXBUFLEN   MsgUARTBody_SIZE
 

extern EMB_BUF   Emb_Buf_Cmd;
extern EMB_BUF   Emb_Buf_Data;


/*
*********************************************************************************************************
*                                           Init_EMB_BUF()
*
* Description : Initialize EMB_BUF type data.
* Argument(s) : pEBuf :
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/
void  Init_EMB_BUF (EMB_BUF  *pEBuf)
{
    pEBuf->index   = 0;
    pEBuf->length  = 0;
    pEBuf->pdata   = NULL;
    pEBuf->state   = true;
}


/*
*********************************************************************************************************
*                                           Init_CMD_Read()
*
* Description : Initialize CMDREAD type data.
* Argument(s) : pCMD_Read : 
*               pOS_EVENT :
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/
void  Init_CMD_Read (CMDREAD   *pCMD_Read, 
                     OS_EVENT  *pOS_EVENT)
{
    
    pCMD_Read->state_mac    = CMD_STAT_SYNC1 ;
    pCMD_Read->pRecvPtr     = NULL;
    pCMD_Read->PcCmdCounter = 0 ;
    pCMD_Read->PcCmdDataLen = 0 ;  
    pCMD_Read->pEvent       = pOS_EVENT ; 
    
}


/*
*********************************************************************************************************
*                                           Noah_CMD_Read()
*
* Description : Initialize CMDREAD type data.
* Argument(s) : pCMD_Read :
*               ch        :
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/
void  Noah_CMD_Read (CMDREAD    *pCMD_Read,
                     CPU_INT08U  ch)
{ 
    
    CPU_INT08U   err;    
    OS_MEM_DATA  MemInfo ;
    NOAH_CMD    *pNoahCmd = NULL ; 
    
    CPU_INT08U   state_mac       = pCMD_Read->state_mac ;
    CPU_INT08U  *pRecvPtr        = pCMD_Read->pRecvPtr;
    CPU_INT16U   PcCmdCounter    = pCMD_Read->PcCmdCounter;
    CPU_INT16U   PcCmdDataLen    = pCMD_Read->PcCmdDataLen;
    
    switch( state_mac ) {   
        
        case CMD_STAT_SYNC1 :        
            if(ch == CMD_DATA_SYNC1)  {
                state_mac = CMD_STAT_SYNC2 ;
            }
        break;
        
        case CMD_STAT_SYNC2 :
            if(ch == CMD_DATA_SYNC2)  {             
                err =   OSMemQuery( pMEM_Part_MsgUART,&MemInfo );	                
                if( MemInfo.OSNFree > 1 && OS_ERR_NONE == err )  {
                    pRecvPtr = (void *)OSMemGet(pMEM_Part_MsgUART,&err);
                    if( NULL != pRecvPtr && OS_ERR_NONE == err )  {
                        state_mac     =  CMD_STAT_FLAG;
                        PcCmdCounter  = 0 ;                        
                    }
                } 
                
            } else {
              
                state_mac = CMD_STAT_SYNC1;                
            }
        break ;
        
        case CMD_STAT_FLAG :            
            *( pRecvPtr + PcCmdCounter++ ) = ch; //save in buf
       
            switch( GET_FRAME_TYPE(ch) )  {
                    case FRAM_TYPE_DATA :
                    case FRAM_TYPE_GDD_IIC :
                        state_mac =  CMD_STAT_LENGTH ;
                        break ;                
                    case FRAM_TYPE_ACK :
                    case FRAM_TYPE_NAK :
                    case FRAM_TYPE_EST :
                    case FRAM_TYPE_ESTA :
                        *( pRecvPtr + PcCmdCounter++ ) = 0; //set datalen = 0
                        state_mac =  CMD_STAT_CHECKSUM ;
                        break;                    
                    default :
                        break ;                        
            }
         
        break ;
        
        case CMD_STAT_LENGTH :            
            *( pRecvPtr + PcCmdCounter++ ) = ch;      
             PcCmdDataLen = ch ; // global
             state_mac    = CMD_STAT_DATA ;
          
        break ;
        
        case CMD_STAT_DATA :
            *( pRecvPtr + PcCmdCounter++ ) = ch;
            if( PcCmdCounter >= MAXBUFLEN ) { //check verflow             
               state_mac = CMD_STAT_SYNC1; 
               OSMemPut( pMEM_Part_MsgUART, pRecvPtr ); 
            } else if(PcCmdCounter >= PcCmdDataLen + 2) { // data over, the check sum will be followed              
                state_mac =  CMD_STAT_CHECKSUM ;
            }
        break ;
        
        case CMD_STAT_CHECKSUM :   
            pNoahCmd = (NOAH_CMD *)pRecvPtr;             
            pNoahCmd->checkSum = ch ;   //get check sum data            
            
            if( PcCmdCounter >= MAXBUFLEN ) { //check verflow            
                state_mac = CMD_STAT_SYNC1; 
                OSMemPut( pMEM_Part_MsgUART, pRecvPtr );
                
            }  else {         
                state_mac    = CMD_STAT_SYNC1 ; //reset state machine  
                PcCmdCounter = 0 ;  
                PcCmdDataLen = 0 ;
                
                err  = OSQPost( pCMD_Read->pEvent, pRecvPtr); // EVENT_MsgQ_PCUART2Noah  //Send valid CMD inf to Uart2task0 Messege Queue
                if( OS_ERR_NONE == err )  {              
                   pRecvPtr  = NULL;                 
                } else {  
                   OSMemPut( pMEM_Part_MsgUART, pRecvPtr );              
                }
//                Time_Stamp();
//                APP_TRACE_INFO(("\r\n::::: Noah_CMD_Read post data "));
              
            }
        break ;
        
        case CMD_STAT_FRAM :   
        break;
        
        default :
            state_mac     = CMD_STAT_SYNC1;
            PcCmdCounter  = 0 ;
        break ;
    }
    
    pCMD_Read->state_mac       = state_mac ;
    pCMD_Read->pRecvPtr        = pRecvPtr;
    pCMD_Read->PcCmdCounter    = PcCmdCounter;
    pCMD_Read->PcCmdDataLen    = PcCmdDataLen;    
    
}



/*
*********************************************************************************************************
*                                           CheckSum()
*
* Description : calculate check sum for a specified data 
* Argument(s) : init_data :  check sum data for previous data
*               pdata     :  pointer to the data address
*               length    :  data length 
* Return(s)   : checksum data: 1 byte 
*
* Note(s)     : None.
*********************************************************************************************************
*/
CPU_INT08U  CheckSum (CPU_INT08U   init_data, 
                      CPU_INT08U  *pdata, 
                      CPU_INT16U   length)
{
    
    CPU_INT16U i;
    CPU_INT08U checksum;
    
    checksum = init_data;   
    
    for( i = 0; i < length; i++ ) {      
	    if (checksum & 0x01) {
      	    checksum = (checksum >> 1) + 0x80 ;
            
        } else {
            checksum >>= 1;
            
        }
	    checksum += *pdata++;
        
    }
    
    return( checksum ) ;
    
}


/*
*********************************************************************************************************
*                                           pcSendDateToBuf()
*
* Description : Code data as Noah protocol defines and send out to transmit task for transmission
* Argument(s) : *pOS_EVENT     :  pointer to the event that load the data 
*               frame_head     :  frame type flag
*               *pdat          :  pointer to thd data to send
*               data_length    :  data length in bytes
*               msg_post_mode  :  for urgent transmit use, 0 - OS_POST_OPT_NONE, 1 - OS_POST_OPT_FRONT
*               *pex_dat       :  extral data need to sent before data and after head bytes
*               ex_data_length :  extral data length
* Return(s)   : NO_ERR :   execute successfully
*               others :   =error code . 
*
* Note(s)     : None.
*********************************************************************************************************
*/
CPU_INT08U  pcSendDateToBuf (OS_EVENT    *pOS_EVENT, 
                             CPU_INT08U   frame_head, 
                             CPU_INT08U  *pdat, 
                             CPU_INT08U   data_length, 
                             CPU_INT08U   msg_post_mode,
                             CPU_INT08U  *pex_dat,
                             CPU_INT08U   ex_data_length
                             )
{
    
    CPU_INT08U  *pSendPtr;
    CPU_INT08U  *pMemPtr;    
    CPU_INT08U  err;
    CPU_INT08U  i, opt;
    CPU_INT08U  frame_type;    
    
    err         = 0;  
    pSendPtr    = NULL;
    pMemPtr     = NULL;
    opt         = ( msg_post_mode == 0 ) ? OS_POST_OPT_NONE : OS_POST_OPT_FRONT ; 
    frame_type  = GET_FRAME_TYPE( frame_head );
    
    if( ( data_length == 0 || pdat == NULL || (data_length + ex_data_length > NOAH_CMD_DATA_MLEN) )  && 
        (frame_type == FRAM_TYPE_DATA) )  {            
        err = SEND_DATA_LEN_ERR;    
        
    } else {  
        
        for( i = 0 ; i < 100 ; i++ ) { //delay 500ms waitting for free Mem 
            pMemPtr = (void *)OSMemGet(pMEM_Part_MsgUART,&err);
            if( OS_ERR_NONE == err ) {
                break;
            }
            if( OS_ERR_MEM_NO_FREE_BLKS == err ) {
                OSTimeDly(5);
            } else {   
                return err;    
            }
        }
        if( i >= 100 ) {
            return err;
        }
        pSendPtr  =  pMemPtr  ;   
      
        if( frame_type == FRAM_TYPE_DATA) {      
            *pSendPtr++ = frame_head;
            *pSendPtr++ = data_length + ex_data_length;         
             while(ex_data_length-- > 0) {
                *pSendPtr++ = *pex_dat++ ;                 
            }   
            while(data_length-- > 0) {
                *pSendPtr++ = *pdat++ ;                 
            }    
            *pSendPtr++ = 0; // here use 0 as checksum, and will do calcute sum in task uart tx  
            
        } else {
            *pSendPtr++ = frame_head;   
            *pSendPtr++ = frame_head; 
            
        } 
        
        err = OSQPostOpt( pOS_EVENT, pMemPtr, opt );   //EVENT_MsgQ_Noah2PCUART send data to Uart2task0 message queue       
        if( OS_ERR_NONE != err )  {   
            OSMemPut( pMEM_Part_MsgUART, pMemPtr ); 
            
        }
        
    }
    
    return  err;
    
}


/*
*********************************************************************************************************
*                                           EMB_Data_Check()
*
* Description : Check the received data after Noah protocol decode if EMB format complete or not.
* Argument(s) : *pNoahCmd :  pointer to the NOAH_CMD type structure that contains the data 
*               *pEBuf    :  pointer to the EMB_BUF data where decoded data will be stored
*                delay    :  extra data in NOAH_CMD buf that not needed
* Return(s)   : NO_ERR :   execute successfully
*               others :   =error code . 
*
* Note(s)     : None.
*********************************************************************************************************
*/
CPU_INT08U  EMB_Data_Check (NOAH_CMD   *pNoahCmd, 
                            EMB_BUF    *pEBuf,
                            CPU_INT08U  delay)
{
  
    CPU_INT08U   err;
    CPU_INT16U   data_cmd_len;
    CPU_INT08U  *p_data_cmd;
   
    //Time_Stamp();                      
    //APP_TRACE_INFO(("\r\n::::: EMB_Data_Check "));
           
            
    err          = NO_ERR;
    p_data_cmd   = pNoahCmd->Data ;
    data_cmd_len = pNoahCmd->DataLen ;   
    
    p_data_cmd  += delay;
    data_cmd_len-= delay;
    
    if( pEBuf->state ) { //new data pack        
     
        if( *p_data_cmd++ == EMB_DATA_FRAME ) { //sync data          
            pEBuf->index   = *p_data_cmd++ ;
            pEBuf->index  += ((*p_data_cmd++)<<8) ; 
            pEBuf->length  = pEBuf->index; //reserve length
            if( pEBuf->length > EMB_BUF_SIZE ) {
                APP_TRACE_INFO(("EMB data length exceed the Max %d B\r\n",EMB_BUF_SIZE));
                return EMB_LEN_OERFLOW_ERR ;
            }
            pEBuf->pdata   = &(pEBuf->data[0]); 
            if( pEBuf->index > (data_cmd_len - 3) ) { // big data package
                pEBuf->index -= data_cmd_len - 3 ;
                pEBuf->state = false; //session not done.
            }
            memcpy( pEBuf->pdata, p_data_cmd, data_cmd_len-3 );
            pEBuf->pdata += data_cmd_len-3 ;
            
        } else {          
            err = EMB_FORMAT_ERR; 
            
        }
      
    } else { //next data pack

        if( pEBuf->index > data_cmd_len ) {
            pEBuf->index -=  data_cmd_len;        
            memcpy( pEBuf->pdata, p_data_cmd, data_cmd_len );
            pEBuf->pdata += data_cmd_len ;
        } else {
            memcpy( pEBuf->pdata, p_data_cmd, pEBuf->index );
            pEBuf->state = true;  //session done.           
        }
        
    }
    
    //Time_Stamp();
    //APP_TRACE_INFO(("\r\n::::: EMB_Data_Check end "));

    
    return err;
  
  
}



/*
*********************************************************************************************************
*                                           Noah_CMD_Parse_Ruler()
*
* Description : Process decoded data from ruler based on Noah protocol 
* Argument(s) : *pNoahCmd      :  pointer to the NOAH_CMD type structure that contains the data 
*               *pSessionDone  :  pointer to data that indicate if one command session is finished
* Return(s)   : NO_ERR :   execute successfully
*               others :   =error code . 
*
* Note(s)     : communication with ruler
*               This routine do NOT support reentrance
*********************************************************************************************************
*/
CPU_INT08U  Noah_CMD_Parse_Ruler (NOAH_CMD    *pNoahCmd,                                 
                                  CPU_INT08U  *pSessionDone)
{
    
    CPU_INT08U    err;
    CPU_INT08U    index;
    EMB_BUF      *pEBuf_Data;         
    
    err         = NO_ERR ;  
    index       = 0;    
    pEBuf_Data  = &Emb_Buf_Data;  //Global var      
     
    switch( pNoahCmd->Data[0] )  {         
        case CMD_D_ACK : //CMD parse result 
            if( pNoahCmd->DataLen == 2 ) {
                err = pNoahCmd->Data[1];
            }
        break ;
        
        case CMD_G_ACK :
            if( pNoahCmd->DataLen == 2 ) {
                err = pNoahCmd->Data[1];
                *pSessionDone = 1 ; //session done , not data back
            }
        break ;        
 
        case RULER_CMD_READ_MIC_CALI_DATA :
            index += 1;        
        case RULER_CMD_RAED_RULER_INFO :  
            index += 1;
            err = EMB_Data_Check( pNoahCmd, pEBuf_Data, index );       
            if( err != OS_ERR_NONE ) {
                Init_EMB_BUF( pEBuf_Data ); 
            } else {
                if( pEBuf_Data->state ) { // EMB data complete               
                    *pSessionDone = 1 ; //session done , not data back 
                }                    
            }    
        break ;
        
        case RULER_CMD_RAED_RULER_STATUS :
            pEBuf_Data->data[0] = pNoahCmd->Data[1];
            pEBuf_Data->data[1] = pNoahCmd->Data[2];
            *pSessionDone = 1 ; 
        break ;

        case RULER_CMD_GET_RULER_TYPE :
            pEBuf_Data->data[0] = pNoahCmd->Data[1];
            *pSessionDone = 1 ; 
        break ;   
   
        case RULER_CMD_GET_RULER_VERSION :            
            pEBuf_Data->length = pNoahCmd->DataLen;            
            memcpy( pEBuf_Data->data, &(pNoahCmd->Data[1]), pNoahCmd->DataLen );
            pEBuf_Data->data[pNoahCmd->DataLen] = '0';
            *pSessionDone = 1 ; 
        break ;
    
        case RULER_CMD_SET_RULER :           
             pEBuf_Data->data[0] = pNoahCmd->Data[1];
            *pSessionDone = 1 ; 
        break;
        
        default :
            err = CMD_NOT_SURRPORT ;
        break ;
        
    }
       
    return( err ) ;
    
}


/*
*********************************************************************************************************
*                                           Send_DACK()
*
* Description : Package and send DACK command  
* Argument(s) : error_id :  error number as define
* Return(s)   : None. 
*
* Note(s)     : None.
*********************************************************************************************************
*/
void  Send_DACK (CPU_INT08U  error_id)
{
   
    CPU_INT08U   DAckBuf[2]; 
    
    DAckBuf[0] = CMD_D_ACK ;
    DAckBuf[1] = error_id ;
    APP_TRACE_DBG(("%2x ",error_id));
    pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, DAckBuf, 2, 0, NULL, 0 ) ; //send data: command error status  
   
    
}


/*
*********************************************************************************************************
*                                           Send_GACK()
*
* Description : Package and send GACK command  
* Argument(s) : error_id :  error number as define
* Return(s)   : None. 
*
* Note(s)     : None.
*********************************************************************************************************
*/
void  Send_GACK (CPU_INT08U  error_id)
{
  
    CPU_INT08U   GAckBuf[2]; 
    
    GAckBuf[0] = CMD_G_ACK ;
    GAckBuf[1] = error_id ;
    APP_TRACE_DBG(("%2x ",error_id));
    pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, GAckBuf, 2, 0, NULL, 0 ) ; 
     
    
}


/*
*********************************************************************************************************
*                                           EMB_Data_Build()
*
* Description :  Code the data as EMB format for specified command 
* Argument(s) : *pEBuf    :  pointer to the EMB_BUF type structure where built new data will be stored
*                cmd_type :  commmand type that need the EMB data
* Return(s)   : NO_ERR :   execute successfully
*               others :   =error code . 
*
* Note(s)     : None.
*********************************************************************************************************
*/
static CPU_INT08U  EMB_Data_Build (EMB_BUF     *pEBuf, 
                                   CPU_INT08U   cmd_type,
                                   PCCMDDAT     *pPCCMD)
{
    
    CPU_INT08U   err;
    CPU_INT32S   pos;  
    CPU_INT08U  *pChar;
    emb_builder  builder;
    CPU_INT08U   ver_buf[25];  //sizeof(Audio_Version) + szieof(fw_version)
        
    err      =  NO_ERR ;
    pChar    =  &(pEBuf->data[0]);    
    *pChar   =  EMB_DATA_FRAME;
    pChar   +=  3;
    
    switch( cmd_type ){      

        case DATA_AB_STATUS :       	
            pos = emb_init_builder(pChar, EMB_BUF_SIZE, cmd_type, &builder);
            pos = emb_append_attr_uint(&builder, pos, 1, Global_Bridge_POST);
            pos = emb_append_attr_uint(&builder, pos, 2, *(CPU_INT32U *)(&Global_Ruler_State) ); 
            pos = emb_append_attr_uint(&builder, pos, 3, *(CPU_INT32U *)(&Global_Mic_State));    
            pos = emb_append_end(&builder, pos);
            pEBuf->data[1] = pos & 0xFF;    
            pEBuf->data[2] = (pos>>8) & 0xFF; 
            pEBuf->length = pos + 3;            
        break;
          
        case DATA_AB_INFO : 
            pos = emb_init_builder(pChar, EMB_BUF_SIZE, cmd_type, &builder);
            pos = emb_append_attr_string(&builder, pos, 1, hw_model);
            pos = emb_append_attr_string(&builder, pos, 2, hw_version); 
            strcpy( (char*)ver_buf, (char*)fw_version );  
            strcat( (char*)ver_buf, (char*)Audio_Version ); 
            pos = emb_append_attr_string(&builder, pos, 3, (const char*)ver_buf);    
            pos = emb_append_end(&builder, pos);
            pEBuf->data[1] = pos & 0xFF;    
            pEBuf->data[2] = (pos>>8) & 0xFF; 
            pEBuf->length = pos + 3;          
        break;
        
        case DATA_UIF_RAW_RD :
            pos = emb_init_builder(pChar, EMB_BUF_SIZE, cmd_type, &builder);
            pos = emb_append_attr_uint(&builder, pos, 1, pPCCMD->raw_read.if_type);
            pos = emb_append_attr_uint(&builder, pos, 2, pPCCMD->raw_read.dev_addr); 
            pos = emb_append_attr_uint(&builder, pos, 3, pPCCMD->raw_read.data_len_read);            
            pos = emb_append_attr_binary(&builder, pos, 4, pPCCMD->raw_read.pdata_read, pPCCMD->raw_read.data_len_read);
            pos = emb_append_end(&builder, pos);
            pEBuf->data[1] = pos & 0xFF;    
            pEBuf->data[2] = (pos>>8) & 0xFF; 
            pEBuf->length = pos + 3;        
        
        break;
        
        default:
            err = CMD_NOT_SURRPORT ;    
            
        break ;
        
    }     
 
    return err;
  
}


/*
*********************************************************************************************************
*                                           EMB_Data_Parse()
*
* Description : Decode EMB data and do the command in the data.
* Argument(s) : *pEBuf_Cmd    :  pointer to the EMB_BUF type structure that contains the data 
* Return(s)   : NO_ERR :   execute successfully
*               others :   =error code . 
*
* Note(s)     : This routine do NOT support reentrance
*********************************************************************************************************
*/
CPU_INT08U  EMB_Data_Parse (EMB_BUF  *pEBuf_Cmd) 
{
    
    CPU_INT08U    err; 
    CPU_INT08U    cmd_type; 
    CPU_INT32S    temp, temp2;      
    emb_t         root;
    PCCMDDAT      PCCmd;
    EMB_BUF      *pEBuf_Data;
    CPU_INT08U    buf[3];
    CPU_INT08U   *pdata;
    CPU_INT16U    data_length;
    const void   *pBin;
    
    err  =  NO_ERR;    
    pEBuf_Data = &Emb_Buf_Data; 
   
    emb_attach( &(pEBuf_Cmd->data[0]), pEBuf_Cmd->length, &root );        
    cmd_type = emb_get_id(&root);   
    
//    Time_Stamp();
//    APP_TRACE_INFO(("\r\n::::: EMB_Data_Parse: cmd type=%d ",cmd_type));
  
    switch( cmd_type )  {
        
        case PC_CMD_SET_AUDIO_CFG : 
            Send_DACK(err);
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.audio_cfg.type = (CPU_INT08U)temp;            
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.audio_cfg.sr = (CPU_INT16U)temp;            
            temp = emb_get_attr_int(&root, 3, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.audio_cfg.channels = (CPU_INT08U)temp; 
            temp = emb_get_attr_int(&root, 4, 0);            
            PCCmd.audio_cfg.lin_ch_mask = (CPU_INT08U)temp; 
            temp = emb_get_attr_int(&root, 5, 0);            
            PCCmd.audio_cfg.bit_length = (CPU_INT08U)temp; 
            err = Setup_Audio( &PCCmd.audio_cfg );
            Send_GACK(err);
   
        break ;
        
        case PC_CMD_START_AUDIO :
            Send_DACK(err);
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.start_audio.type = (CPU_INT08U)temp;             
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.start_audio.padding = (CPU_INT08U)temp; 
            err = Ruler_Active_Control(1);              
            if( err != NO_ERR ) { Send_GACK(err); break; }            
            err = Start_Audio( PCCmd.start_audio );
            Send_GACK(err);
        break ;
        
        case PC_CMD_STOP_AUDIO :
            Send_DACK(err);  
            err = Ruler_Active_Control(0);                 
            if( err != NO_ERR ) { Send_GACK(err); break; }
            err = Stop_Audio(); 
            Send_GACK(err);
        break ;    
        
        case PC_CMD_RESET_AUDIO :
            Send_DACK(err);           
            err = Reset_Audio(); 
            Send_GACK(err);
        break ; 
                
        ////////////////////////////////////////////////////////////////////////        
        
        case PC_CMD_SET_IF_CFG :
            Send_DACK(err);
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.interface_cfg.if_type = (CPU_INT08U)temp;             
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.interface_cfg.speed = (CPU_INT16U)temp;   
            temp = emb_get_attr_int(&root, 3, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.interface_cfg.attribute = (CPU_INT08U)temp; 
            err = Setup_Interface( &PCCmd.interface_cfg );
            Send_GACK(err);
        break ;
        
        case PC_CMD_RAW_WRITE :
//            Time_Stamp();
//            APP_TRACE_INFO(("\r\n::::: PC_CMD_RAW_WRITE "));
//            
            Send_DACK(err);            
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.raw_write.if_type = (CPU_INT08U)temp;             
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.raw_write.dev_addr = (CPU_INT08U)temp; 
            temp = emb_get_attr_int(&root, 3, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.raw_write.data_len = (CPU_INT32U)temp;  
            pBin = emb_get_attr_binary(&root, 4, (int*)&temp);
            if(pBin == NULL ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.raw_write.pdata = (CPU_INT08U *)pBin; 
            err = Raw_Write( &PCCmd.raw_write );
            Send_GACK(err);
//            Time_Stamp();
//            APP_TRACE_INFO(("\r\n::::: PC_CMD_RAW_WRITE end "));
        
        break ;
        
        case PC_CMD_RAW_READ :
            Send_DACK(err);           
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.raw_read.if_type = (CPU_INT08U)temp;             
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.raw_read.dev_addr = (CPU_INT08U)temp;            
            temp = emb_get_attr_int(&root, 3, -1);           
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.raw_read.data_len_read = (CPU_INT32U)temp;  
            
            temp = emb_get_attr_int(&root, 4, -1);
            if(temp == -1 ) { 
                //Send_GACK(EMB_CMD_ERR);  break; 
                temp = 0;};
            PCCmd.raw_read.data_len_write = (CPU_INT32U)temp;  
            pBin = emb_get_attr_binary(&root, 5, (int*)&temp);
            if(pBin == NULL ) { 
                //Send_GACK(EMB_CMD_ERR);  break; 
            }            
            PCCmd.raw_read.pdata_write = (CPU_INT08U *)pBin;             
            err = Raw_Read( &PCCmd.raw_read );
            if( err != NO_ERR ) { Send_GACK(err); break; }             
            err = EMB_Data_Build( pEBuf_Data, DATA_UIF_RAW_RD, &PCCmd );                    
            pdata = pEBuf_Data->data;
            data_length = pEBuf_Data->length;
            //APP_TRACE_INFO(("\r\n::::: data_length %d", data_length));
            while( data_length > 0 ){ 
                temp = data_length > (NOAH_CMD_DATA_MLEN-2) ? (NOAH_CMD_DATA_MLEN-2) : data_length ;  
                //APP_TRACE_INFO(("\r\n::::: temp %d", temp));
                err = pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, pdata, temp, 0, NULL, 0 ) ; 
                //Dump_Data(pdata,temp);
                if( OS_ERR_NONE != err ) { break;}                    
                data_length -= temp;
                pdata += temp;        
            }      
            Send_GACK(err);           
        break ;
        
//        case PC_CMD_BURST_WRITE :
//            Send_DACK(err);            
//            temp = emb_get_attr_int(&root, 1, -1);
//            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
//            PCCmd.burst_write.if_type = (CPU_INT08U)temp;             
//            temp = emb_get_attr_int(&root, 2, -1);
//            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
//            PCCmd.burst_write.dev_addr = (CPU_INT08U)temp;              
//            temp = emb_get_attr_int(&root, 3, -1);
//            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
//            PCCmd.burst_write.mem_addr_l = (CPU_INT16U)temp;    
//            temp = emb_get_attr_int(&root, 4, -1);
//            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
//            PCCmd.burst_write.mem_addr_h = (CPU_INT16U)temp;             
//            temp = emb_get_attr_int(&root, 5, -1);
//            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
//            PCCmd.burst_write.mem_addr_len = (CPU_INT08U)temp;            
//            temp = emb_get_attr_int(&root, 6, -1);
//            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
//            PCCmd.burst_write.data_len = (CPU_INT32U)temp;             
//            pBin = emb_get_attr_binary(&root, 7, (int*)&PCCmd.burst_write.data_len);
//            if(pBin == NULL ) { Send_GACK(EMB_CMD_ERR);  break; }
//            PCCmd.burst_write.pdata = (CPU_INT08U *)pBin;             
//            err = Write_Burst( PCCmd.burst_write );
//            Send_GACK(err);
//        break ;
          
//        case PC_CMD_BURST_READ :
//            Send_DACK(err);
//           
//            Send_GACK(err);
//        break ;
//                
//        case PC_CMD_SESSION :
//            Send_DACK(err);
//           
//            Send_GACK(err);
//        break ;
//                
//        case PC_CMD_DELAY :
//            Send_DACK(err);
//           
//            Send_GACK(err);
//        break ;
    
        case PC_CMD_MCU_FLASH_WRITE :
            Send_DACK(err);            
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.mcu_flash.addr_index = (CPU_INT08U)temp;             
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.mcu_flash.data_len = (CPU_INT32U)temp;        
            pBin = emb_get_attr_binary(&root, 3, (int*)&temp);
            if(pBin == NULL ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.mcu_flash.pdata = (CPU_INT08U *)pBin;
            pBin = emb_get_attr_binary(&root, 4, (int*)&temp);
            if(pBin == NULL ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.mcu_flash.pStr = (CPU_INT08U *)pBin;          
            err = Save_DSP_VEC(  &PCCmd.mcu_flash );    
            Send_GACK(err);
        break ;
        
        case PC_CMD_SET_VEC_CFG :
            Send_DACK(err);            
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.set_vec_cfg.vec_index_a = (CPU_INT08U)temp;             
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.set_vec_cfg.vec_index_b = (CPU_INT08U)temp;
            temp = emb_get_attr_int(&root, 3, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }
            PCCmd.set_vec_cfg.delay = (CPU_INT32U)temp;                
            err = Set_DSP_VEC( &PCCmd.set_vec_cfg );    
            Send_GACK(err);
        break ;
        ////////////////////////////////////////////////////////////////////////        
        
        case PC_CMD_RAED_RULER_INFO : 
            Send_DACK(err);             
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }            
            err = Read_Ruler_Info( temp );              
            if( err != NO_ERR ) { Send_GACK(err); break; }  
            emb_attach( &(pEBuf_Data->data[0]), pEBuf_Data->length, &root );
            emb_get_node_replace(&root, 1, temp);
            buf[0] = EMB_DATA_FRAME ;
            buf[1] = (pEBuf_Data->length) & 0xFF;    
            buf[2] = ((pEBuf_Data->length)>>8) & 0xFF;  
            err = pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, buf, sizeof(buf), 0, NULL, 0 ) ;            
            if( OS_ERR_NONE == err ) {  
                pdata = pEBuf_Data->data;
                data_length = pEBuf_Data->length;
                while( data_length > 0 ){        
                    temp = data_length > NOAH_CMD_DATA_MLEN ? NOAH_CMD_DATA_MLEN : data_length ; 
                    err = pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, pdata, temp, 0, NULL, 0 ) ; 
                    if( OS_ERR_NONE != err ) { break;}
                    data_length -= temp;
                    pdata += temp;
                } 
            }
            Send_GACK(err);          
        break ;
       
        case PC_CMD_WRITE_RULER_INFO : 
            Send_DACK(err); 
            temp = emb_get_attr_int(&root, 1, -1);            
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }            
            err = Write_Ruler_Info( temp ); 
            Send_GACK(err);             
        break ;

        ////////////////////////////////////////////////////////////////////////
        case PC_CMD_READ_MIC_CALI_DATA :  
            Send_DACK(err);             
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; } 
            temp2 = emb_get_attr_int(&root, 2, -1);            
            if(temp2 == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }            
            err = Read_Mic_Cali_Data( temp, temp2 );             
            if( err != NO_ERR ) { Send_GACK(err); break; }  
            emb_attach( &(pEBuf_Data->data[0]), pEBuf_Data->length, &root );
            emb_get_node_replace(&root, 1, temp);
            emb_get_node_replace(&root, 2, temp2);
            buf[0] = EMB_DATA_FRAME ;
            buf[1] = (pEBuf_Data->length) & 0xFF;    
            buf[2] = ((pEBuf_Data->length)>>8) & 0xFF;  
            err = pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, buf, sizeof(buf), 0, NULL, 0 ) ;            
            if( OS_ERR_NONE == err ) {  
                pdata = pEBuf_Data->data;
                data_length = pEBuf_Data->length;
                while( data_length > 0 ){        
                    temp = data_length > NOAH_CMD_DATA_MLEN ? NOAH_CMD_DATA_MLEN : data_length ; 
                    err = pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, pdata, temp, 0, NULL, 0 ) ; 
                    if( OS_ERR_NONE != err ) { break;}
                    data_length -= temp;
                    pdata += temp;
                }  
            }
            Send_GACK(err);            
        break ;

        case PC_CMD_WRITE_MIC_CALI_DATA :   
            Send_DACK(err); 
            temp = emb_get_attr_int(&root, 1, -1);            
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }   
            temp2 = emb_get_attr_int(&root, 2, -1);            
            if(temp2 == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }          
            err = Write_Mic_Cali_Data( temp, temp2 ); 
            Send_GACK(err);              
        break ;
        ////////////////////////////////////////////////////////////////////////
        case PC_CMD_TOGGLE_MIC : 
            Send_DACK(err);
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR); break; }
            PCCmd.toggle_mic.ruler_id = (CPU_INT08U)temp;
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR); break; }
            PCCmd.toggle_mic.mic_id = (CPU_INT08U)temp;
            temp = emb_get_attr_int(&root, 3, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR); break; }
            PCCmd.toggle_mic.on_off = (CPU_INT08U)temp;      
            err = Toggle_Mic( &PCCmd.toggle_mic ) ;
            Send_GACK(err);    
        break ;
        
        case PC_CMD_RESET_MIC :
            Send_DACK(err);
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR); break; }            
            err = Reset_Mic_Mask( (unsigned int*)&temp ) ;
            Send_GACK(err);    
        break ;
  
        case PC_CMD_SET_VOLUME :
            Send_DACK(err);
            temp = emb_get_attr_int(&root, 1, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR); break; }
            PCCmd.set_volume.mic = (CPU_INT32U)temp;
            temp = emb_get_attr_int(&root, 2, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR); break; }
            PCCmd.set_volume.lout = (CPU_INT32U)temp;
            temp = emb_get_attr_int(&root, 3, -1);
            if(temp == -1 ) { Send_GACK(EMB_CMD_ERR); break; }
            PCCmd.set_volume.spk = (CPU_INT32U)temp;      
            err = Set_Volume( &PCCmd.set_volume ) ;
            Send_GACK(err);    
        break ;
        
        case PC_CMD_READ_AB_STATUS : 
             Send_DACK(err);             
             err = EMB_Data_Build( pEBuf_Data, DATA_AB_STATUS, NULL );               
             pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, pEBuf_Data->data, pEBuf_Data->length, 0, NULL, 0 ) ; 
             Send_GACK(err);            
        break ;
  
        case PC_CMD_RAED_AB_INFO :   
             Send_DACK(err);             
             err = EMB_Data_Build( pEBuf_Data, DATA_AB_INFO, NULL );             
             pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, pEBuf_Data->data, pEBuf_Data->length, 0, NULL, 0 ) ; 
             Send_GACK(err);            
        break ;      
        ////////////////////////////////////////////////////////////////////////
        case PC_CMD_RAW_DATA_TRANS :   
             Send_DACK(err);  
             
             Send_GACK(err);              
        break ;  
       
        case PC_CMD_DOWNLOAD_RULER_FW :
             Send_DACK(err);   
             temp = emb_get_attr_int(&root, 1, -1);            
             if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }  
             unsigned char *pBin,*pStr;
             unsigned int   size; 
             pBin = (unsigned char *)emb_get_attr_binary(&root, 2, (int *)&size);            
             if(pBin == NULL ) { Send_GACK(EMB_CMD_ERR);  break; }              
             pStr = (unsigned char *)emb_get_attr_string(&root, 3);            
             if(pStr == NULL ) { Send_GACK(EMB_CMD_ERR);  break; }            
             err = Save_Ruler_FW( temp, pBin, pStr, size );           
             Send_GACK(err);             
        break ; 
        
        case PC_CMD_UPDATE_RULER_FW :   
             Send_DACK(err);   
             temp = emb_get_attr_int(&root, 1, -1);            
             if(temp == -1 ) { Send_GACK(EMB_CMD_ERR);  break; }                  
             err = Update_Ruler_FW( temp );          
             Send_GACK(err);             
        break ;  
        
        case PC_CMD_UPDATE_AB_FW :   
             Send_DACK(err);  
             
             Send_GACK(err);               
        break ;         
        
        default :            
            err = CMD_NOT_SURRPORT ;
            Send_DACK(err);
            
        break ;
        
    }
    
    return err;

}

/*
*********************************************************************************************************
*                                           AB_Status_Change_Report()
*
* Description : Check ruler status, and report to PC only if any ruler's  attach/detach  status changed 
* Argument(s) : None.
* Return(s)   : NO_ERR :   execute successfully
*               others :   =error code . 
* Note(s)     : None.
*********************************************************************************************************
*/
static CPU_INT08U Ruler_State_Previous[4];    //ruler previous status 

CPU_INT08U  AB_Status_Change_Report (void)
{
    
    CPU_INT08U    err; 
    EMB_BUF      *pEBuf;   
    CPU_INT08U    flag; 
    CPU_INT08U    i;   
    
    err   = NO_ERR;   
    pEBuf = &Emb_Buf_Cmd;
    flag  = 0;    

    for( i = 0; i < 4; i++ ) {
        if( (Ruler_State_Previous[i] == 0) && (Global_Ruler_State[i] > 1) ||
            (Ruler_State_Previous[i] > 1)  && (Global_Ruler_State[i] == 0) ) {
            flag = 1; 
            Ruler_State_Previous[i] =  Global_Ruler_State[i];   
        }
    }
    
    if( flag == 0 ) {//no state changed
       return err;
    }
    
    if( Global_Conn_Ready == 0 || Global_Idle_Ready == 0) { //no connection setup, or commu busy
        return err;
    } 
    Global_Idle_Ready = 0 ;
    
    err = EMB_Data_Build( pEBuf, DATA_AB_STATUS, NULL );  
    if( err != NO_ERR ) {
        return err;
    }
    
    err = pcSendDateToBuf( EVENT_MsgQ_Noah2PCUART, FRAM_TYPE_DATA, pEBuf->data, pEBuf->length, 0, NULL, 0 ) ;   
    
    return err;
  
}

    


