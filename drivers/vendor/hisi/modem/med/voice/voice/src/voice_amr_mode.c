/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : med_amr_mode_ctrl.c
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��6��29��
  ����޸�   :
  ��������   :
  �����б�   :
              VOICE_AmrModeCtrlGetDtx
              VOICE_AmrModeCtrlChangeMode
              VOICE_AmrModeCtrlInit
              VOICE_UmtsAmrModeCtrlSetType
              VOICE_MsgUmtsMacModeChangeIndR
              VOICE_MsgUmtsMacModeSetIndIRP
  �޸���ʷ   :
  1.��    ��   : 2011��6��29��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "TdmacCodecInterface.h"
#include "voice_amr_mode.h"
#include "voice_proc.h"
#include "ucom_comm.h"
#include "om.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_AMR_MODE_SC

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
VOICE_AMR_MODE_CTRL_OBJ                 g_stVoiceAmrModeCtrlObj;                  /* AMRģʽ��������ʵ��, ��ϸ�μ�VOICE_AMR_MODE_CTRL_OBJ���� */

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : VOICE_AmrModeCtrlInit
 ��������  : AMRģʽ���Ƴ�ʼ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_AmrModeCtrlInit(VOS_VOID)
{
    VOICE_AMR_MODE_CTRL_OBJ              *pstObj = VOICE_AMR_MODE_CTRL_GetObjPtr();

    /* ȱʡ����: AMR2|12.2kbps */
    UCOM_MemSet(pstObj, 0, sizeof(VOICE_AMR_MODE_CTRL_OBJ));

    pstObj->enAmrType       = WTTFVOICE_AMR_TYPE_AMR2;
    pstObj->enCurrentMode   = WTTFVOICE_AMR_CODECMODE_TYPE_122K;
    pstObj->enTargetMode    = WTTFVOICE_AMR_CODECMODE_TYPE_122K;
    pstObj->enDtxEnable     = VOICE_AMR_DTX_DISABLE;
    pstObj->uhwNumInAcs     = 1;
    pstObj->aenAcs[0]       = WTTFVOICE_AMR_CODECMODE_TYPE_122K;
    pstObj->enAcsSet        = VOICE_AMR_ACS_SET_NO;

    OM_LogInfo(VOICE_AmrModeCtrlInit_Ok);
}

/*****************************************************************************
 �� �� ��  : VOICE_UmtsAmrModeCtrlSetType
 ��������  : ֪ͨMAC�µ�UMTS AMR type
 �������  : enCodecType - AMR|AMR2
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_UmtsAmrModeCtrlSetType(CODEC_ENUM_UINT16 enCodecType,VOS_UINT32 uwReceiverPid)
{
    VOICE_AMR_MODE_CTRL_OBJ            *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    WTTFVOICE_MAC_AMR_TYPE_REQ_STRU     stRspMsg;

    UCOM_MemSet(&stRspMsg, 0, sizeof(stRspMsg));


    /* ��ȡNB��WB���ͣ�������AMRNB��AMRNB2*/
    if (CODEC_AMRWB == enCodecType)
    {
        pstObj->enAmrType    = WTTFVOICE_AMR_TYPE_AMRWB;                         /*WB�²�����AMR��AMR2���������ΪAMR */
    }
    else
    {
        /* ��ȡAMR TYPE */
        pstObj->enAmrType
            = ((CODEC_AMR == enCodecType) ? WTTFVOICE_AMR_TYPE_AMR : WTTFVOICE_AMR_TYPE_AMR2);

    }

    /* �ظ���Ϣ׼�� */
    stRspMsg.enMsgName  = ID_VOICE_MAC_AMR_TYPE_REQ;
    stRspMsg.enAmrType  = pstObj->enAmrType;

    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stRspMsg,
                    sizeof(WTTFVOICE_MAC_AMR_TYPE_REQ_STRU));

    /* ��¼��־ */
    OM_LogInfo(VOICE_AmrModeCtrlSetType_Ok);
}
VOS_VOID VOICE_TDAmrModeCtrlSetType(CODEC_ENUM_UINT16 enCodecType)
{
    VOICE_AMR_MODE_CTRL_OBJ                *pstObj      = VOICE_AMR_MODE_CTRL_GetObjPtr();
    TDMACVOICE_VOICE_MAC_AMR_TYPE_REQ_STRU  stRspMsg;

    UCOM_MemSet(&stRspMsg, 0, sizeof(stRspMsg));


    /* ��ȡAMR TYPE */
    pstObj->enAmrType
        = ((CODEC_AMR == enCodecType) ? TDMACVOICE_AMR_TYPE_AMR : TDMACVOICE_AMR_TYPE_AMR2);

    /* �ظ���Ϣ׼�� */
    stRspMsg.enMsgName  = ID_VOICE_TDMAC_AMR_TYPE_REQ;
    stRspMsg.enAmrType  = pstObj->enAmrType;

    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_TDTTF,
                    &stRspMsg,
                    sizeof(TDMACVOICE_VOICE_MAC_AMR_TYPE_REQ_STRU));

    /* ��¼��־ */
    OM_LogInfo(VOICE_AmrModeCtrlSetType_Ok);
}

/*****************************************************************************
 �� �� ��  : VOICE_AmrModeCtrlGetDtx
 ��������  : ��ȡUMTS AMR DTX���ܱ�־
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOICE_AMR_DTX_ENUM_UINT16
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOICE_AMR_DTX_ENUM_UINT16 VOICE_AmrModeCtrlGetDtx(VOS_VOID)
{
    return g_stVoiceAmrModeCtrlObj.enDtxEnable;
}

/*****************************************************************************
 �� �� ��  : VOICE_AmrModeCtrlChangeMode
 ��������  : ��ȡ��ǰʹ�õ�AMR����ģʽ, ��ÿ20ms����һ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : CODEC_AMR_RATE_MODE_ENUM_U16
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT16 VOICE_AmrModeCtrlChangeMode(VOS_VOID)
{
    VOICE_AMR_MODE_CTRL_OBJ            *pstObj = VOICE_AMR_MODE_CTRL_GetObjPtr();
    VOS_INT16                           shwCnt = 0;
    VOS_INT16                           shwFlag;

    if (VOICE_AMR_ACS_SET_NO == VOICE_AMR_MODE_CTRL_GetAcsSet())
    {
        OM_LogWarning(VOICE_AmrModeCtrlChangeMode_AcsNotSet);
    }

    /* ֡����, ȡֵ0,1 */
    pstObj->uhwFrmCnt += 1;
    if (pstObj->uhwFrmCnt >= 2)
    {
        pstObj->uhwFrmCnt = 0;
    }

    shwFlag = ((WTTFVOICE_AMR_TYPE_AMR2    == pstObj->enAmrType)
              || (WTTFVOICE_AMR_TYPE_AMRWB == pstObj->enAmrType))
              && (0 == pstObj->uhwFrmCnt);

    /* ��ΪAMR2��AMRWB���֡(40ms)����һ������ */
    if ( VOS_TRUE == shwFlag )
    {
        return pstObj->enCurrentMode;
    }

    /* ��ǰ�Ѿ���Ŀ��������ֱ�ӷ��� */
    if (pstObj->enTargetMode == pstObj->enCurrentMode)
    {
        return pstObj->enCurrentMode;
    }

    /* ���ҵ�ǰ������ACS�е�λ�� */
    for (shwCnt = 0; shwCnt < (VOS_INT16)(pstObj->uhwNumInAcs); shwCnt++)
    {
        if (pstObj->aenAcs[shwCnt] == pstObj->enCurrentMode)
        {
            break;
        }
    }

    /* ����ǰ���ʲ���ACS����ǿ��Ϊ��Сֵ */
    if (shwCnt >= (VOS_INT16)(pstObj->uhwNumInAcs))
    {
        shwCnt = 0;
        pstObj->enCurrentMode = pstObj->aenAcs[shwCnt];
        OM_LogWarning(VOICE_AmrModeCtrlChangeMode_InvalidTarget);
    }

    /* ������Ҫ���� */
    if (pstObj->enTargetMode > pstObj->enCurrentMode)
    {
        shwCnt += 1;
        if (shwCnt >= (VOS_INT16)(pstObj->uhwNumInAcs))
        {
            /* ��ǰ�Ѿ��������������ģʽ�򲻱� */
            OM_LogWarning(VOICE_AmrModeCtrlChangeMode_TargetOverflowAcsTop);
        }
        else
        {
            pstObj->enCurrentMode = pstObj->aenAcs[shwCnt];
        }
    }
    /* ������Ҫ��С */
    else
    {
        shwCnt -= 1;
        if (shwCnt < 0)
        {
            /* ��ǰ�Ѿ�����С��������ģʽ�򲻱� */
            OM_LogWarning(VOICE_AmrModeCtrlChangeMode_TargetOverflowAcsBottom);
        }
        else
        {
            pstObj->enCurrentMode = pstObj->aenAcs[shwCnt];
        }
    }

    return pstObj->enCurrentMode;

}

/*****************************************************************************
 �� �� ��  : VOICE_MsgUmtsMacModeSetIndIRP
 ��������  : UMTS AMR����ģʽ������Ϣ����
 �������  : pstOsaMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 VOICE_MsgUmtsMacModeSetIndIRP(VOS_VOID *pstOsaMsg)
{
    VOICE_AMR_MODE_CTRL_OBJ                  *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    WTTFVOICE_MAC_VOICE_MODE_SET_IND_STRU    *pstSetMsg = VOS_NULL;
    WTTFVOICE_MAC_MODE_SET_RSP_STRU           stRspMsg;
    VOS_UINT16                                enAmrMode;
    VOS_UINT16                                uhwModeNum;
    VOS_UINT16                                uhwCnt;
    VOS_UINT16                                uhwCodecModeTypeButt;

    UCOM_MemSet(&stRspMsg, 0, sizeof(stRspMsg));

    /* ��Ϣ��ȡ */
    pstSetMsg = (WTTFVOICE_MAC_VOICE_MODE_SET_IND_STRU*)pstOsaMsg;

    /* �ظ���Ϣ׼�� */
    UCOM_MemSet(&stRspMsg, 0, sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));
    stRspMsg.enMsgName  = ID_VOICE_MAC_MODE_SET_RSP;
    stRspMsg.usOpId     = pstSetMsg->usOpId;

    stRspMsg.enResult   = WTTFVOICE_RSLT_FAIL;

    /* ������� */
    /* enCodecTypeȡֵ��Χ��飻ACS��AMR MODE�������: ACS��������һ���Ϸ�AMR MODE*/
    uhwModeNum  = pstSetMsg->usCodecModeCnt;
    if (   (0 == uhwModeNum)
        || (pstSetMsg->enCodecType >= WTTFVOICE_AMR_BANDWIDTH_TYPE_BUTT))
    {
        /* ������ϢID_VOICE_MAC_MODE_SET_RSP��WTTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_WTTF,
                        &stRspMsg,
                        sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));

        OM_LogError(VOICE_MsgMacModeSetInd_InvalidAcs);

        return UCOM_RET_ERR_PARA;
    }

    /* ACS�Ϸ��Լ�� */
    if(WTTFVOICE_AMR_BANDWIDTH_TYPE_NB == pstSetMsg->enCodecType)
    {
        uhwCodecModeTypeButt = WTTFVOICE_AMR_CODECMODE_TYPE_BUTT;
    }
    else
    {
        uhwCodecModeTypeButt = WTTFVOICE_AMRWB_CODECMODE_TYPE_BUTT;
    }

    for (uhwCnt = 0; uhwCnt < uhwModeNum; uhwCnt++)
    {
        if (pstSetMsg->aenCodecModes[uhwCnt] >= uhwCodecModeTypeButt)
        {
            /* ������ϢID_VOICE_MAC_MODE_SET_RSP��WTTF */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            UCOM_PID_PS_WTTF,
                            &stRspMsg,
                            sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));

            OM_LogError(VOICE_MsgMacModeSetInd_InvalidMode);

            return UCOM_RET_ERR_PARA;
        }
    }

    /* SID�Ϸ��Լ�� */
    if (pstSetMsg->usSidEnable >= VOICE_AMR_DTX_BUTT)
    {
        /* ������ϢID_VOICE_MAC_MODE_SET_RSP��WTTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_WTTF,
                        &stRspMsg,
                        sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));

        OM_LogError(VOICE_MsgMacModeSetInd_InvalidSid);

        return UCOM_RET_ERR_PARA;
    }

    /* ��ȡ���ò��� */
    enAmrMode               = pstSetMsg->aenCodecModes[uhwModeNum-1];           /* Ĭ��ȡ���ֵ��Ϊ��ǰAMR MODE */
    pstObj->enCurrentMode   = enAmrMode;
    pstObj->enTargetMode    = enAmrMode;
    pstObj->uhwNumInAcs     = uhwModeNum;
    pstObj->enDtxEnable     = pstSetMsg->usSidEnable;
    UCOM_MemCpy(pstObj->aenAcs,
                pstSetMsg->aenCodecModes,
                (VOS_UINT32)(uhwModeNum * sizeof(VOS_UINT16)));

    /* �ظ�RSP��Ϣ */
    stRspMsg.enCurrMode = enAmrMode;
    stRspMsg.enAmrType  = pstObj->enAmrType;
    stRspMsg.enResult   = WTTFVOICE_RSLT_SUCC;

    /* ������ϢID_VOICE_MAC_MODE_SET_RSP��WTTF */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_WTTF,
                    &stRspMsg,
                    sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));

    /* ��¼�Ѿ��յ���MAC�·���AMR����ģʽ������Ϣ */
    VOICE_AMR_MODE_CTRL_SetAcsSet(VOICE_AMR_ACS_SET_YES);

    OM_LogInfo(VOICE_MsgMacModeSetInd_Ok);

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 �� �� ��  : VOICE_MsgUmtsMacModeChangeIndR
 ��������  : �ı�AMR ����ģʽ
 �������  : pstOsaMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT16
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 VOICE_MsgUmtsMacModeChangeIndR(VOS_VOID *pstOsaMsg)
{
    VOICE_AMR_MODE_CTRL_OBJ                    *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    WTTFVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU   *pstIndMsg = VOS_NULL;
    WTTFVOICE_MAC_MODE_CHANGE_RSP_STRU          stRspMsg;
    VOS_UINT16                                  enAmrMode;
    VOS_UINT16                                  uhwCnt;
    VOS_UINT16                                  uhwCodecModeTypeButt;

    /* ��Ϣ��ȡ */
    pstIndMsg = (WTTFVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU*)pstOsaMsg;

    /* �ظ���Ϣ׼�� */
    UCOM_MemSet(&stRspMsg, 0, sizeof(WTTFVOICE_MAC_MODE_CHANGE_RSP_STRU));
    stRspMsg.enMsgName  = ID_VOICE_MAC_MODE_CHANGE_RSP;
    stRspMsg.usOpId     = pstIndMsg->usOpId;

    /* ������� */

    /* ���Ŀ��AMR MODE�Ƿ��ڵ�ǰACS�� */
    if (WTTFVOICE_AMR_BANDWIDTH_TYPE_NB == pstIndMsg->enCodecType)
    {
        uhwCodecModeTypeButt = WTTFVOICE_AMR_CODECMODE_TYPE_BUTT;
    }
    else if (WTTFVOICE_AMR_BANDWIDTH_TYPE_WB == pstIndMsg->enCodecType)
    {
        uhwCodecModeTypeButt = WTTFVOICE_AMRWB_CODECMODE_TYPE_BUTT;
    }
    else
    {
        OM_LogError(VOICE_MsgDoChangeAmrModeInd_InvalidCodecType);
        return UCOM_RET_ERR_PARA;
    }

    enAmrMode = uhwCodecModeTypeButt;

    for (uhwCnt = 0; uhwCnt < pstObj->uhwNumInAcs; uhwCnt++)
    {
        if ( pstIndMsg->enTargetMode == pstObj->aenAcs[uhwCnt] )
        {
            enAmrMode = pstIndMsg->enTargetMode;
            break;
        }
    }

    if (uhwCodecModeTypeButt  == enAmrMode)
    {
        stRspMsg.enResult   = WTTFVOICE_RSLT_FAIL;

        /* ������ϢID_VOICE_MAC_MODE_CHANGE_RSP��WTTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_WTTF,
                        &stRspMsg,
                        sizeof(WTTFVOICE_MAC_MODE_CHANGE_RSP_STRU));

        OM_LogError(VOICE_MsgDoChangeAmrModeInd_InvalidMode);

        return UCOM_RET_ERR_PARA;
    }

    /* ���ܲ��� */
    pstObj->enTargetMode    = pstIndMsg->enTargetMode;

    /* �ظ�RSP��Ϣ */
    stRspMsg.enCurrMode     = pstObj->enCurrentMode;
    stRspMsg.enTargetMode   = pstObj->enTargetMode;
    stRspMsg.enResult       = WTTFVOICE_RSLT_SUCC;

    /* ������ϢID_VOICE_MAC_MODE_CHANGE_RSP��WTTF */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_WTTF,
                    &stRspMsg,
                    sizeof(WTTFVOICE_MAC_MODE_CHANGE_RSP_STRU));

    OM_LogInfo(VOICE_MsgDoChangeAmrModeInd_Ok);

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgTDmacModeSetIndIRP(VOS_VOID *pstOsaMsg)
{
    VOICE_AMR_MODE_CTRL_OBJ                *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    TDMACVOICE_MAC_VOICE_MODE_SET_IND_STRU *pstSetMsg = VOS_NULL;
    TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU  stRspMsg;
    VOS_UINT16                              enAmrMode;
    VOS_UINT16                              uhwModeNum;
    VOS_UINT16                              uhwCnt;
    VOS_UINT16                              uhwCodecModeTypeButt;

    UCOM_MemSet(&stRspMsg, 0, sizeof(stRspMsg));

    /* ��Ϣ��ȡ */
    pstSetMsg = (TDMACVOICE_MAC_VOICE_MODE_SET_IND_STRU*)pstOsaMsg;

    /* �ظ���Ϣ׼�� */
    UCOM_MemSet(&stRspMsg, 0, sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));
    stRspMsg.enMsgName  = ID_VOICE_TDMAC_MODE_SET_RSP;
    stRspMsg.usOpId     = pstSetMsg->usOpId;

    stRspMsg.enResult   = TDMACVOICE_RSLT_FAIL;

    /* ������� */
    /* enCodecTypeȡֵ��Χ��飻ACS��AMR MODE�������: ACS��������һ���Ϸ�AMR MODE*/
    uhwModeNum  = pstSetMsg->usCodecModeCnt;
    if (   (0 == uhwModeNum)
        || (pstSetMsg->enCodecType >= TDMACVOICE_AMR_BANDWIDTH_TYPE_BUTT))
    {
        /* ������ϢID_CODEC_MAC_MODE_SET_RSP��TD TTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_TDTTF,
                        &stRspMsg,
                        sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));

        OM_LogError(VOICE_MsgMacModeSetInd_InvalidAcs);

        return UCOM_RET_ERR_PARA;
    }

    uhwCodecModeTypeButt = TDMACVOICE_AMR_CODECMODE_TYPE_BUTT;

    for (uhwCnt = 0; uhwCnt < uhwModeNum; uhwCnt++)
    {
        if (pstSetMsg->aenCodecModes[uhwCnt] >= uhwCodecModeTypeButt)
        {
            /* ������ϢID_VOICE_MAC_MODE_SET_RSP��TD TTF */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            UCOM_PID_PS_TDTTF,
                            &stRspMsg,
                            sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));

            OM_LogError(VOICE_MsgMacModeSetInd_InvalidMode);

            return UCOM_RET_ERR_PARA;
        }
    }

    /* SID�Ϸ��Լ�� */
    if (pstSetMsg->usSidEnable >= VOICE_AMR_DTX_BUTT)
    {
        /* ������ϢID_VOICE_MAC_MODE_SET_RSP��TD TTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_TDTTF,
                        &stRspMsg,
                        sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));

        OM_LogError(VOICE_MsgMacModeSetInd_InvalidSid);

        return UCOM_RET_ERR_PARA;
    }

    /* ��ȡ���ò��� */
    enAmrMode               = pstSetMsg->aenCodecModes[uhwModeNum-1];           /* Ĭ��ȡ���ֵ��Ϊ��ǰAMR MODE */
    pstObj->enCurrentMode   = enAmrMode;
    pstObj->enTargetMode    = enAmrMode;
    pstObj->uhwNumInAcs     = uhwModeNum;
    pstObj->enDtxEnable     = pstSetMsg->usSidEnable;
    UCOM_MemCpy(pstObj->aenAcs,
                pstSetMsg->aenCodecModes,
                (VOS_UINT32)(uhwModeNum * sizeof(VOS_UINT16)));

    /* �ظ�RSP��Ϣ */
    stRspMsg.enCurrMode = enAmrMode;
    stRspMsg.enAmrType  = pstObj->enAmrType;
    stRspMsg.enResult   = TDMACVOICE_RSLT_SUCC;

    /* ������ϢID_CODEC_MAC_MODE_SET_RSP��TD TTF */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_TDTTF,
                    &stRspMsg,
                    sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));

    /* ��¼�Ѿ��յ���MAC�·���AMR����ģʽ������Ϣ */
    VOICE_AMR_MODE_CTRL_SetAcsSet(VOICE_AMR_ACS_SET_YES);

    OM_LogInfo(VOICE_MsgMacModeSetInd_Ok);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgTDMacModeChangeIndR(VOS_VOID *pstOsaMsg)
{
    VOICE_AMR_MODE_CTRL_OBJ                    *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    TDMACVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU  *pstIndMsg = VOS_NULL;
    TDMACVOICE_VOICE_MAC_MODE_CHANGE_RSP_STRU   stRspMsg;
    VOS_UINT16                                  enAmrMode;
    VOS_UINT16                                  uhwCnt;
    VOS_UINT16                                  uhwCodecModeTypeButt;

    /* ��Ϣ��ȡ */
    pstIndMsg = (TDMACVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU*)pstOsaMsg;

    /* �ظ���Ϣ׼�� */
    UCOM_MemSet(&stRspMsg, 0, sizeof(TDMACVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU));
    stRspMsg.enMsgName  = ID_VOICE_TDMAC_MODE_CHANGE_RSP;
    stRspMsg.usOpId     = pstIndMsg->usOpId;

    /* ������� */

    /* ���Ŀ��AMR MODE�Ƿ��ڵ�ǰACS�� */
    if (TDMACVOICE_AMR_BANDWIDTH_TYPE_NB == pstIndMsg->enCodecType)
    {
        uhwCodecModeTypeButt = TDMACVOICE_AMR_CODECMODE_TYPE_BUTT;
    }
    else
    {
        OM_LogError(VOICE_MsgDoChangeAmrModeInd_InvalidCodecType);
        return UCOM_RET_ERR_PARA;
    }

    enAmrMode = uhwCodecModeTypeButt;

    for (uhwCnt = 0; uhwCnt < pstObj->uhwNumInAcs; uhwCnt++)
    {
        if ( pstIndMsg->enTargetMode == pstObj->aenAcs[uhwCnt] )
        {
            enAmrMode = pstIndMsg->enTargetMode;
            break;
        }
    }

    if (uhwCodecModeTypeButt  == enAmrMode)
    {
        stRspMsg.enResult   = TDMACVOICE_RSLT_FAIL;

        /* ������ϢID_CODEC_MAC_MODE_CHANGE_RSP��TD TTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_TDTTF,
                        &stRspMsg,
                        sizeof(TDMACVOICE_VOICE_MAC_MODE_CHANGE_RSP_STRU));

        OM_LogError(VOICE_MsgDoChangeAmrModeInd_InvalidMode);

        return UCOM_RET_ERR_PARA;
    }

    /* ���ܲ��� */
    pstObj->enTargetMode    = pstIndMsg->enTargetMode;

    /* �ظ�RSP��Ϣ */
    stRspMsg.enCurrMode     = pstObj->enCurrentMode;
    stRspMsg.enTargetMode   = pstObj->enTargetMode;
    stRspMsg.enResult       = TDMACVOICE_RSLT_SUCC;

    /* ������ϢID_CODEC_MAC_MODE_CHANGE_RSP��TD TTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_TDTTF,
                        &stRspMsg,
                        sizeof(TDMACVOICE_VOICE_MAC_MODE_CHANGE_RSP_STRU));

    OM_LogInfo(VOICE_MsgDoChangeAmrModeInd_Ok);

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

