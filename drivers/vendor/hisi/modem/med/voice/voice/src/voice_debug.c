/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : VOICE_debug.c
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��7��4��
  ����޸�   :
  ��������   : ���Թ���
  �����б�   :
              VOICE_DbgCheckAndLoop
              VOICE_DbgInfoHookCfg
              VOICE_DbgSendHook
              VOICE_MsgOmSetHookReqIRP
              VOICE_MsgOmSetLoopReqIRP
  �޸���ʷ   :
  1.��    ��   : 2011��7��4��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "om.h"
#include "ucom_config.h"
#include "ucom_nv.h"
#include "voice_debug.h"
#include "voice_proc.h"
#include "voice_pcm.h"
#include "med_drv_timer_hifi.h"
#include "ucom_pcm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_DEBUG_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* ����������� */
CODEC_LOOP_STRU   g_stVoiceDbgLoop = {CODEC_SWITCH_OFF, CODEC_LOOP_BUTT};

/* HOOKʹ�ܡ���ȡĿ�ꡢ֡�� */
VOICE_HOOK_STRU   g_stVoiceDbgHookObj = {CODEC_SWITCH_OFF, 0, 0};

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : VOICE_DbgInit
 ��������  : ����ģ���ʼ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��17��
    ��    ��   : ��ׯ�� 59026
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_DbgInit( CODEC_NV_PARA_VOICE_TRACE_CFG_STRU *pstVoiceTraceCfg )
{
    CODEC_LOOP_STRU                      *pstLoopObj  = VOICE_DbgGetLoopFLagPtr();
    VOICE_HOOK_STRU                      *pstHookCtrl = VOICE_DbgGetHookPtr();

    pstLoopObj->enLoopEnable    = (pstVoiceTraceCfg->stLoopCfg).uhwEnable;
    pstLoopObj->enLoopType      = (pstVoiceTraceCfg->stLoopCfg).uhwType;

    pstHookCtrl->enHookEnable   = (pstVoiceTraceCfg->stVoiceHookCfg).uhwEnable;
    pstHookCtrl->usHookTarget   = (pstVoiceTraceCfg->stVoiceHookCfg).uhwTarget;
    pstHookCtrl->ulFrameTick    = 0;

}

/*****************************************************************************
 �� �� ��  : VOICE_MsgOmSetLoopReqIRP
 ��������  : ���û�����ر���
 �������  : pstOsaMsg��ָ����Ϣ���ݵ�ָ��
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��10��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_UINT32 VOICE_MsgOmSetLoopReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                            uwRet       = UCOM_RET_SUCC;
    MSG_OM_VOICE_LOOP_REQ_STRU           *pstPrim     = VOS_NULL;
    CODEC_LOOP_STRU                      *pstLoopObj  = VOICE_DbgGetLoopFLagPtr();
    MSG_CODEC_CNF_STRU                    stRspPrim;

    UCOM_MemSet(&stRspPrim, 0, sizeof(stRspPrim));

    /*ԭ��ID�Ϸ����ж������Ѿ�����Ϣ״̬����֤���˴������ж� */
    pstPrim = (MSG_OM_VOICE_LOOP_REQ_STRU *)pstOsaMsg;

    /*�����Ϸ����ж�*/
    if((pstPrim->stLoop.enLoopEnable > CODEC_SWITCH_ON)
        || (pstPrim->stLoop.enLoopType > CODEC_LOOP_CODEC))
    {
        uwRet = UCOM_RET_ERR_PARA;
    }
    else
    {
        /*���û�������*/
        pstLoopObj->enLoopType          = pstPrim->stLoop.enLoopType;

        /*���û��ش򿪻�ر�*/
        pstLoopObj->enLoopEnable        = pstPrim->stLoop.enLoopEnable;
    }

    /* �����ϢID��ִ�н�� */
    stRspPrim.usMsgName                 = ID_VOICE_OM_SET_LOOP_CNF;
    stRspPrim.enExeRslt                 = (VOS_UINT16)uwRet;

    /* ��SDT�ظ���ϢID_VOICE_OM_SET_LOOP_CNF */
    OM_COMM_SendTranMsg(&stRspPrim, sizeof(stRspPrim));

    /* ��¼��־��Ϣ */
    OM_LogInfo(VOICE_MsgSetLoopReq_Ok);

    return uwRet;
}
VOS_UINT32 VOICE_MsgOmSetHookReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                            uwRet      = UCOM_RET_SUCC;
    MSG_OM_VOICE_HOOK_REQ_STRU           *pstPrim    = VOS_NULL;
    VOICE_HOOK_STRU                      *pstHook    = VOICE_DbgGetHookPtr();
    MSG_CODEC_CNF_STRU                    stRspPrim;

    UCOM_MemSet(&stRspPrim, 0, sizeof(stRspPrim));

    /*ԭ��ID�Ϸ����ж������Ѿ�����Ϣ״̬����֤���˴������ж� */
    pstPrim = (MSG_OM_VOICE_HOOK_REQ_STRU *)pstOsaMsg;

    /*�����Ϸ����ж�*/
    if ((CODEC_SWITCH_BUTT <= pstPrim->enHookEnable)
        || (VOICE_HOOK_BUTT <= pstPrim->usHookTarget))
    {
        uwRet = UCOM_RET_ERR_PARA;

        /* ��¼��־��Ϣ */
        OM_LogInfo(VOICE_MsgSetHookReq_Fail);
    }
    else
    {
        /* ��ԭ���л�ȡ���ݹ�ȡ��ر���ֵ����ȫ�ֱ��� */
        pstHook->enHookEnable   = pstPrim->enHookEnable;
        pstHook->usHookTarget   = pstPrim->usHookTarget;
        pstHook->ulFrameTick    = 0;

        /* ֪ͨOMģ��ͬ���������ݹ�ȡ������Ϣ */
        /* PC Voice������V9R1�ϲ�ʵ�֣���ʱ���� */
        /* VOICE_DbgInfoHookCfg(); */

        uwRet = UCOM_RET_SUCC;

        /* ��¼��־��Ϣ */
        OM_LogInfo(VOICE_MsgSetHookReq_Ok);
    }

    /* �����ϢID��ִ�н�� */
    stRspPrim.usMsgName = ID_VOICE_OM_SET_HOOK_CNF;
    stRspPrim.enExeRslt = (VOS_UINT16)uwRet;

    /* ��SDT�ظ���ϢID_VOICE_OM_SET_HOOK_CNF */
    OM_COMM_SendTranMsg(&stRspPrim, sizeof(stRspPrim));

    return uwRet;
}

/*****************************************************************************
 �� �� ��  : VOICE_DbgCheckAndLoop
 ��������  : ����Ƿ񻷻�
 �������  : uhwPos:     ����λ��
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��10��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ע��ģ��

*****************************************************************************/
VOS_VOID VOICE_DbgCheckAndLoop(CODEC_LOOP_ENUM_UINT16 uhwPos)
{
    VOICE_PROC_CODEC_STRU           *pstCodec   = VOICE_ProcGetCodecPtr();
    CODEC_LOOP_STRU                 *pstLoopObj = VOICE_DbgGetLoopFLagPtr();

    if ( (CODEC_SWITCH_OFF == pstLoopObj->enLoopEnable)
        || (uhwPos != pstLoopObj->enLoopType) )
    {
        return;
    }

    if (CODEC_LOOP_CODEC == uhwPos)
    {
        /* ��������ת������ */

        pstCodec->stDecInObj.enTafFlag     = CODEC_TAF_NONE;    /* EFR/HR/FRʼ���ޱ߽� */
        pstCodec->stDecInObj.enBfiFlag     = CODEC_BFI_NO;      /* EFR/HR/FRʼ��Ϊ��֡ */
        pstCodec->stDecInObj.enHrUfiFlag   = CODEC_BFI_NO;      /* HRʼ��Ϊ��֡ */

        /* EFR/HR/FR֡����ƥ�� */
        if (CODEC_SP_SPEECH == pstCodec->stEncOutObj.enSpFlag)
        {
            pstCodec->stDecInObj.enSidFlag = CODEC_SID_SPEECH;  /* ���յ�SPEECH֡ */
        }
        else
        {
            pstCodec->stDecInObj.enSidFlag = CODEC_SID_VALID;   /* ���յ�SID֡ */
        }

        /* AMR֡����ƥ�� */
        switch (pstCodec->stEncOutObj.enAmrFrameType)
        {
            case CODEC_AMR_TYPE_TX_SPEECH_GOOD:
            {
                pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_SPEECH_GOOD;
                break;
            }
            case CODEC_AMR_TYPE_TX_SID_FIRST:
            {
                pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_SID_FIRST;
                break;
            }
            case CODEC_AMR_TYPE_TX_SID_UPDATE:
            {
                pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_SID_UPDATE;
                break;
            }
            default:
            {
                pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_NO_DATA;
            }
        }

        /* �������� */
        UCOM_MemCpy((void*)&g_stVoiceCodecData.astDecSerial[0].asDecSerial[0],
                    (void*)&g_stVoiceCodecData.asEncSerial[0],
                    (VOICE_CODED_FRAME_WITH_OBJ_LEN * sizeof(VOS_UINT16)));

    }
    else if (CODEC_LOOP_DMA == uhwPos)
    {
        /* DMA���� */
        UCOM_MemCpy((void*)g_psVoicePcmSpkOut,
                    (void*)g_psVoicePcmMicIn,
                    (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES*UCOM_PCM_I2S_CHANNEL_NUM);
    }
    else
    {
        /* ����ǰ���� if (CODEC_LOOP_BEFORE_CODEC == uhwPos) */
        UCOM_MemCpy((void*)&g_stVoiceCodecData.asLineInBuff[0],
                    (void*)&g_stVoiceCodecData.asLineOutBuff[0],
                    (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : VOICE_DbgInfoHookCfg
 ��������  : ֪ͨDataTransferģ�����ݹ�ȡ���ò���
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��4��
    ��    ��   : ��ׯ�� 59026
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_DbgInfoHookCfg(VOS_VOID)
{
    VOICE_HOOK_STRU                      *pstHook    = VOICE_DbgGetHookPtr();
    MSG_CODEC_OM_HOOK_IND_STRU           stInfoMsg;

    /*��ԭ���л�ȡ���ݹ�ȡ��ر���ֵ����ȫ�ֱ���*/
    stInfoMsg.usMsgId      = ID_VOICE_OM_SET_HOOK_IND;
    stInfoMsg.usReserve    = 0;
    stInfoMsg.enHookEnable = pstHook->enHookEnable;
    stInfoMsg.usHookTarget = pstHook->usHookTarget;

    UCOM_SendOsaMsg(DSP_PID_VOICE, ACPU_PID_PCVOICE, &stInfoMsg, sizeof(stInfoMsg));

}

/*****************************************************************************
 �� �� ��  : VOICE_DbgSendHook
 ��������  : ��ȡָ��λ�����ݽ����ϱ�
 �������  : uhwPos:  ��ȡ����λ��
             pvData:  ��ȡ����buffer��ַ
             uhwLen:  ��ȡ����ʵ�����ݳ���(16bit)
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��16��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ע��ģ��

*****************************************************************************/
VOS_VOID VOICE_DbgSendHook(
                VOS_UINT16              uhwPos,
                VOS_VOID               *pvData,
                VOS_UINT16              uhwLen)
{
    VOS_UINT16                     uhwHookLen  = 0;
    MSG_VOICE_OM_HOOK_STRU         stHookHead;
    UCOM_DATA_BLK_STRU             astBlk[2];
    VOICE_HOOK_STRU               *pstHookObj = VOICE_DbgGetHookPtr();

    if (CODEC_SWITCH_OFF == pstHookObj->enHookEnable )
    {
        return;
    }

    if(uhwPos == (uhwPos & (pstHookObj->usHookTarget)))
    {
        uhwHookLen = (uhwLen > VOICE_MAX_HOOK_LEN) ? VOICE_MAX_HOOK_LEN : uhwLen;

        /* ���ԭ������ */
        stHookHead.usMsgId      = (VOS_UINT16)ID_VOICE_OM_HOOK_IND;
        stHookHead.usHookTarget = uhwPos;
        stHookHead.ulFrameTick  = pstHookObj->ulFrameTick;
        stHookHead.ulTimeStamp  = DRV_TIMER_ReadSysTimeStamp();
        stHookHead.usHookLength = (VOS_UINT16)(uhwHookLen * 2);

        /* ����ַ�ͳ��� */
        astBlk[0].pucData       = (VOS_UCHAR *)&stHookHead;
        astBlk[0].uwSize        = sizeof(stHookHead);

        /* ����ַ�ͳ��� */
        astBlk[1].pucData       = (VOS_UCHAR *)pvData;
        astBlk[1].uwSize        = uhwHookLen * 2;


        /* �ϱ�HOOK��Ϣ */
        OM_COMM_SendTrans(astBlk, 2);
    }
}


VOS_VOID VOICE_DbgSaveCodecObj(
                VOS_INT16                          *pshwCodedBuf,
                VOICE_DBG_CODEC_MODE_ENUM_UINT16 uhwCodecMode,
                VOS_VOID                           *pstCodec )
{
    VOS_UINT32 *puwDbgBuf;
    VOS_UINT32  uwFreeSize;

    /* ��ȡ�����������βλ�� */
    puwDbgBuf = (VOS_UINT32 *)&pshwCodedBuf[VOICE_CODED_FRAME_LENGTH];

    /* �ڱ��������ĩβ��� Magic Number */
    *puwDbgBuf = VOICE_DBG_CODEC_TITLE_MAGIC_NUM;
    puwDbgBuf++;

    /* ���ݱ����ģʽд���Ӧ�� Magic Number */
    switch(uhwCodecMode)
    {
        case VOICE_DBG_CODEC_MODE_ENCODE:
        {
            *puwDbgBuf = VOICE_DBG_CODEC_ENCODE_MAGIC_NUM;
            break;
        }
        case VOICE_DBG_CODEC_MODE_DECODE:
        {
            *puwDbgBuf = VOICE_DBG_CODEC_DECODE_MAGIC_NUM;
            break;
        }
        default:
        {
            /* �����ģʽ����,���ظ�д��TITLE */
            *puwDbgBuf = VOICE_DBG_CODEC_TITLE_MAGIC_NUM;
            break;
        }
    }
    puwDbgBuf++;

    /* ����ʣ���д��ռ� */
    uwFreeSize = (((VOICE_CODED_FRAME_WITH_OBJ_LEN - VOICE_CODED_FRAME_LENGTH) * sizeof(VOS_INT16)) - (2*sizeof(VOS_UINT32)))
                  - sizeof(GPHY_VOICE_RX_DATA_IND_STRU);

    if((sizeof(VOICE_PROC_CODEC_STRU)) < uwFreeSize)
    {
        /* д������ṹ�� */
        UCOM_MemCpy(puwDbgBuf, pstCodec, sizeof(VOICE_PROC_CODEC_STRU));
    }
    else
    {
        /* �ռ䲻�� */
        OM_LogError(VOICE_DbgSaveCodecObj_CannotWriteVOICE_PROC_CODEC_STRUtoDbgBuf);
    }

}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

