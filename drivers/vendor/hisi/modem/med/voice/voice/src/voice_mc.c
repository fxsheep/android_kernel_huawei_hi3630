

/*****************************************************************************
   1 ͷ�ļ�����
******************************************************************************/
#include "OmCodecInterface.h"
#include "hifidrvinterface.h"
#include "ImsCodecInterface.h"

#include "codec_typedefine.h"
#include "ucom_comm.h"
#include "ucom_nv.h"
#include "ucom_mem_dyn.h"
#include "om.h"
#include "med_drv_sio.h"
#include "drv_mailbox.h"

#include "voice_log.h"
#include "voice_proc.h"
#include "voice_pcm.h"
#include "voice_mc.h"
#include "voice_debug.h"
#include "voice_amr_mode.h"
#include "voice_diagnose.h"

#include "med_drv_timer_hifi.h"
#include "VosPidDef.h"
#include "CodecInterface.h"
#include "mlib_interface.h"
#include "ucom_pcm.h"

#include "voice_jb_interface.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_MC_C

/* ��VOICE�������������PID���� */

#define PID_UNSET                       (0)

/* PID��Χ����� */
#define MODEM0_BEGIN                    (WUEPS_PID_MAC)
#define MODEM0_END                      (PS_PID_IMSVA)

#define MODEM1_BEGIN                    (I1_WUEPS_PID_USIM)
#define MODEM1_END                      (I1_DSP_PID_APM)

#define CODEC_BEGIN                     (DSP_PID_VOICE_RESERVED)
#define CODEC_END                       (DSP_PID_HIFI_OM)

#define OM_BEGIN                        (WUEPS_PID_OM)
#define OM_END                          (WUEPS_PID_OM)


/*****************************************************************************
   2 ȫ�ֱ�������
******************************************************************************/
/* PIDת����,��0֧�ֶ�ģ����һֻ֧��G */
VOS_UINT32                              g_auwModemMcPidTbl[VOICE_MC_MODEM_NUM_BUTT][VOICE_MC_INDEX_BUTT]
                                     = {{I0_WUEPS_PID_VC,I0_DSP_PID_GPHY,
                                         DSP_PID_WPHY,DSP_PID_TDPHY,
                                         WUEPS_PID_MAC,TPS_PID_MAC,
                                         UCOM_PID_DSP_IMSA},
                                       {I1_WUEPS_PID_VC,I1_DSP_PID_GPHY,
                                        PID_UNSET,PID_UNSET,
                                        PID_UNSET,PID_UNSET,
                                        PID_UNSET}};

/* �������ģ������״̬ȫ�ֱ��� */
VOICE_MC_OBJS_STRU                      g_stVoiceMcObjs;

/*****************************************************************************
   3 �ⲿȫ�ֱ�������
******************************************************************************/

/*****************************************************************************
   4 ����ʵ��
******************************************************************************/

/*****************************************************************************
 �� �� ��  : VOICE_McInit
 ��������  : ��������ĳ�ʼ������ʹ����������Ŀ�ʼ����һ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��10��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ע��ģ��

*****************************************************************************/
VOS_VOID VOICE_McInit(VOS_VOID)
{
    CODEC_NV_PARA_VOICE_TRACE_CFG_STRU  stVoiceTraceCfg;
    EN_OPENDSP_CONFIG_STRU              stOpenDspConfig;
    VOICE_MC_OBJS_STRU                 *pstObj         = VOICE_McGetMcObjsPtr();
    VOICE_MC_MODEM_STRU                *pstMcModemObj0 = VOICE_McGetModemObjPtr(VOICE_MC_MODEM0);
    VOICE_MC_MODEM_STRU                *pstMcModemObj1 = VOICE_McGetModemObjPtr(VOICE_MC_MODEM1);
    VOICE_DIAG_NV_STRU                  stVoiceDiagCfg;

    UCOM_MemSet(pstObj, 0, sizeof(VOICE_MC_OBJS_STRU));
    UCOM_MemSet(&stOpenDspConfig, 0, sizeof(EN_OPENDSP_CONFIG_STRU));

    /* ��ǰ��ǰ̨ͨ���� */
    pstObj->enActiveModemNo = VOICE_MC_MODEM_NUM_BUTT;

    /* ��ʼ��ͨ·���� */
    VOICE_McModemObjInit(pstMcModemObj0);
    VOICE_McModemObjInit(pstMcModemObj1);

    /* ��ȡSmartPA������ */
    UCOM_NV_Read(en_NV_SmartPACfg,
                 &(pstObj->stSmartPaCfg),
                 sizeof(pstObj->stSmartPaCfg));

    /* ���õ�ǰProfile */
    UCOM_NV_Read(en_NV_OpenDSP_Config,
                &stOpenDspConfig,
                sizeof(EN_OPENDSP_CONFIG_STRU));

    MLIB_SelectProfile(stOpenDspConfig.ucProfile);

    /* AMR����ģʽ���Ƴ�ʼ�� */
    VOICE_AmrModeCtrlInit();

    /* �źŴ����ʼ�� */
    VOICE_ProcInit();

    /* ����IO��ʼ�� */
    VOICE_PcmInit();

    /* ��ȡVOICE TRACE���NV�� */
    UCOM_NV_Read(en_NV_VoiceTraceCfg,
                 &stVoiceTraceCfg,
                 sizeof(stVoiceTraceCfg));

    /* ���Թ��ܳ�ʼ�� */
    VOICE_DbgInit(&stVoiceTraceCfg);

    /* ��ȡ�����������NV�� */
    UCOM_NV_Read(en_NV_VoiceDiagnoseCfg,
                 &stVoiceDiagCfg,
                 sizeof(stVoiceDiagCfg));

    /* ��ʼ�������������ģ�� */
    VOICE_DiagInit(&stVoiceDiagCfg);

    #ifdef _MED_ERRORLOG
    /* ��ʼ��ErrorLog */
    VOICE_ErrLogInit(&stVoiceTraceCfg.stErrlogCfg);
    #endif

    /* TD 5ms ��֡�ж� */
    /* ����V9R1 <<Hi6620V100 SOC�߼��������_DMA������жϷ���.xlsx>>,����Ϊtds_stu_hifi_int��ID����� */
    VOS_ConnectInterrupt(DRV_TDSCDMA_CTRL_SYNC_INTR, VOICE_SyncTdSubFrm5msIsr);

    /* ��ʼ��JB */
    VOICE_JB_Init();

    return;
}
VOS_VOID VOICE_McDestory( )
{
    /* ����Proc */
    VOICE_ProcDestroy(VOICE_ProcGetObjsPtr());
}

#if 0
/*****************************************************************************
 �� �� ��  : VOICE_McSndStopCallback
 ��������  : ����sound�ӿڵĻص�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��10��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ע��ģ��

*****************************************************************************/
VOS_VOID VOICE_McSndStopCallback(VOS_VOID)
{
    CODEC_INOUT_STRU                    *pstInout = VOICE_McGetInoutPtr();

    if(CODEC_INOUT_SOUND == pstInout->usTxIn)
    {
        pstInout->usTxIn = CODEC_INOUT_DEFAULT;
    }

    if(CODEC_INOUT_SOUND == pstInout->usRxIn)
    {
        pstInout->usRxIn = CODEC_INOUT_DEFAULT;
    }

    /* �ϱ�VOICE_VOICE_VC_SOUND_END_INDԭ��,V3R2�汾��ʵ��SOUND���� */

    return;
}

/*****************************************************************************
 �� �� ��  : VOICE_McConfigSnd
 ��������  : ����sound�ӿڵĻص�����
 �������  : sTxVolume:      ����sound����������С,��λdb
             sRxVolume:      ����sound����������С,��λdb
             usSoundId:      sound���
             usSoundLoopCnt: sound����ѭ������
 �������  : ��
 �� �� ֵ  : VOICE_RET_OK          �ɹ�
             VOICE_RET_ERROR       ʧ��
             VOICE_RET_ERR_PARAM   ��������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��10��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ע��ģ��

*****************************************************************************/
VOS_UINT32 VOICE_McConfigSnd(
                VOS_INT16               sTxVolume,
                VOS_INT16               sRxVolume,
                VOS_UINT16              usSoundId,
                VOS_UINT16              usSoundLoopCnt)
{
    VOICE_PROC_SND_CFG_STRU          stSoundConfig;
    VOS_UINT32                          uwRet        = UCOM_RET_SUCC;
    VOS_UINT16                          usSoundSwich = CODEC_SWITCH_ON;
    CODEC_INOUT_STRU                     *pstInout     = VOICE_McGetInoutPtr();

    /* ���������ж����������л��������� */
    if((CODEC_INOUT_SOUND == pstInout->usTxIn)
      && (CODEC_INOUT_SOUND == pstInout->usRxIn))
    {
        stSoundConfig.enTxRx = VOICE_TXRX_TXRX;
    }
    else if(CODEC_INOUT_SOUND == pstInout->usTxIn)
    {
        stSoundConfig.enTxRx = VOICE_TXRX_TX;
    }
    else if(CODEC_INOUT_SOUND == pstInout->usRxIn)
    {
        stSoundConfig.enTxRx = VOICE_TXRX_RX;
    }
    else
    {
        usSoundSwich = CODEC_SWITCH_OFF;
    }

    /* ����ѭ��������������С */
    stSoundConfig.usRptCnt   = usSoundLoopCnt;
    stSoundConfig.usSndId    = usSoundId;
    stSoundConfig.sTxVolume  = sTxVolume;
    stSoundConfig.sRxVolume  = sRxVolume;
    stSoundConfig.funcSndInd = VOICE_McSndStopCallback;
    uwRet                    = VOICE_ProcConfigSound(usSoundSwich, &stSoundConfig);

    return uwRet;
}
#endif

/*****************************************************************************

 ��Ϣ��Ӧ������������

 �� �� ��  : VOICE_Msg[�������][��Ϣ��д][I(dle)\R(unning)\P(ause)]
 ��������  : [I\R\P]��ʾ��Ϣ��Ӧ���������������״̬,�������ʹ��
 �������  : VOS_VOID *pstOsaMsg
 �� �� ֵ  : VOS_UINT32

*****************************************************************************/


VOS_UINT32 VOICE_MsgVcSetVolReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                      uwRet;
    VOS_UINT32                      uwSenderPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo;              /* ͨ����ö�� */
    VOICE_MC_MODEM_STRU            *pstModem   = VOS_NULL;
    VCVOICE_SET_VOLUME_REQ_STRU    *pstVolPrim = VOS_NULL;

    /*ԭ��ID�Ϸ����ж������Ѿ�����Ϣ״̬����֤���˴������ж� */
    pstVolPrim  = (VCVOICE_SET_VOLUME_REQ_STRU*)pstOsaMsg;

    /* ����senderPid��ȡͨ���� */
    uwSenderPid = pstVolPrim->uwSenderPid;
    enModemNo   = VOICE_McGetModemNum(uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ��ȡͨ·���� */
    pstModem    = VOICE_McGetModemObjPtr(enModemNo);

    /* �������Ϸ��� */
    if (   (pstVolPrim->enVolTarget >= VCVOICE_VOLUME_TARGET_BUTT)
        || (pstVolPrim->sVolValue   >  VOICE_PROC_GAIN_MAX)
        || (pstVolPrim->sVolValue   <  VOICE_PROC_GAIN_MIN))
    {
        uwRet = UCOM_RET_ERR_PARA;
    }
    else
    {
        /* ����ʹ�ܲ��� */
        /* ���� */
        if ( VCVOICE_VOLUME_TARGET_UP == pstVolPrim->enVolTarget )
        {
            pstModem->shwVolumeTx = pstVolPrim->sVolValue;
        }
        /* ���� */
        else
        {
            pstModem->shwVolumeRx = pstVolPrim->sVolValue;
        }

        uwRet = UCOM_RET_SUCC;
    }

    if ( UCOM_RET_SUCC == uwRet)
    {
        OM_LogInfo(VOICE_MsgSetVolReq_Ok);
    }
    else
    {
        OM_LogError(VOICE_MsgSetVolReq_ParamError);
    }

    /*  �ظ���ϢID_VOICE_VC_SET_VOLUME_CNF��NAS */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_VOLUME_CNF, uwSenderPid, uwRet);

    return uwRet;
}


VOS_UINT32 VOICE_MsgVcSetDevReqI(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo;              /* ͨ����ö�� */
    VCVOICE_SET_DEVICE_REQ_STRU        *pstDevice;

    pstDevice = (VCVOICE_SET_DEVICE_REQ_STRU*)pstOsaMsg;

    /* ����senderPid��ȡͨ���� */
    enModemNo = VOICE_McGetModemNum(pstDevice->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* �����豸ģʽ */
    uwRet = VOICE_McSetDev(pstDevice->usDeviceMode, VOS_FALSE, enModemNo);

    /* �ظ�NASִ�н�� */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_DEVICE_CNF, pstDevice->uwSenderPid, uwRet);

    return uwRet;
}
VOS_UINT32 VOICE_MsgVcSetDevReqRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo;              /* ͨ����ö�� */
    VCVOICE_SET_DEVICE_REQ_STRU        *pstDevice;

    pstDevice = (VCVOICE_SET_DEVICE_REQ_STRU*)pstOsaMsg;

    /* ����senderPid��ȡͨ���� */
    enModemNo = VOICE_McGetModemNum(pstDevice->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* �����豸ģʽ */
    uwRet = VOICE_McSetDev(pstDevice->usDeviceMode, VOS_TRUE, enModemNo);

    /* �ظ�NASִ�н�� */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_DEVICE_CNF, pstDevice->uwSenderPid, uwRet);

    return uwRet;
}
VOS_UINT32 VOICE_MsgVcStartReqI(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                      uwRet;
    VOS_UINT32                      uwJbStartRet = UCOM_RET_SUCC;
    VCVOICE_START_REQ_STRU         *pstMsg       = VOS_NULL;

    pstMsg      = (VCVOICE_START_REQ_STRU  *)pstOsaMsg;

    /* ��ʼ�������� */
    uwRet = VOICE_McStart(pstMsg);

    /* LTEģʽ�£�����JB */
    if(CODEC_NET_MODE_L == pstMsg->enMode)
    {
        uwJbStartRet = VOICE_JB_Start(VCVOICE_TYPE_AMRWB);
        if(UCOM_RET_SUCC != uwJbStartRet)
        {
            OM_LogError(VOICE_MsgStartReq_JbInitFail);
        }
    }

    /* �ظ�NASִ�н�� */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_START_CNF, pstMsg->uwSenderPid, uwRet);

    OM_LogInfo(VOICE_MsgStartReq_Ok);

    return uwRet;
}
VOS_UINT32 VOICE_MsgVcSetCodecReqR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo;              /* ͨ����ö�� */
    VCVOICE_SET_CODEC_REQ_STRU         *pstSetCodec;

    /*ԭ��ID�Ϸ����ж������Ѿ�����Ϣ״̬����֤���˴������ж� */
    pstSetCodec = (VCVOICE_SET_CODEC_REQ_STRU*)pstOsaMsg;

    /* ����senderPid��ȡͨ���� */
    enModemNo   = VOICE_McGetModemNum(pstSetCodec->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ���������� */
    uwRet = VOICE_McSetCodec(pstSetCodec, enModemNo);

    /* �ظ���ϢID_VOICE_VC_SET_CODEC_CNF��NAS */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_CODEC_CNF, pstSetCodec->uwSenderPid, uwRet);

    /* ��¼��־��Ϣ */
    OM_LogInfo(VOICE_MsgSetCodecReq_Ok);

    return uwRet;
}
VOS_UINT32 VOICE_MsgVcSetCodecReqP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                      uwRet;
    VOS_UINT32                      uwSenderPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo;              /* ͨ����ö�� */
    VCVOICE_SET_CODEC_REQ_STRU     *pstSetCodec;

    /*ԭ��ID�Ϸ����ж������Ѿ�����Ϣ״̬����֤���˴������ж� */
    pstSetCodec = (VCVOICE_SET_CODEC_REQ_STRU*)pstOsaMsg;

    /* ����senderPid��ȡͨ���� */
    uwSenderPid = pstSetCodec->uwSenderPid;
    enModemNo   = VOICE_McGetModemNum(uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ���������� */
    uwRet = VOICE_McSetCodec(pstSetCodec, enModemNo);

    if (UCOM_RET_SUCC == uwRet)
    {
        /* �ָ�����Ϊ����״̬ */
        VOICE_McResume(pstSetCodec->enCodecType, enModemNo);
    }

    /* �ظ���ϢID_VOICE_VC_SET_CODEC_CNF��NAS */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_CODEC_CNF, uwSenderPid, uwRet);

    /* ��¼��־��Ϣ */
    OM_LogInfo(VOICE_MsgSetCodecReq_Ok);

    return uwRet;
}


VOS_UINT32 VOICE_MsgVcStopReqRP(VOS_VOID *pstOsaMsg)
{
    VOICE_MC_MODEM_NUM_ENUM_UINT16    enModemNo;
    VCVOICE_STOP_REQ_STRU            *pstStop;

    VOS_UINT32  uwRet = UCOM_RET_SUCC;

    /* ��ȡSenderPid */
    pstStop     = (VCVOICE_STOP_REQ_STRU *)pstOsaMsg;

    /* ����SenderPid��ȡͨ���� */
    enModemNo = VOICE_McGetModemNum(pstStop->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ֹͣ����������� */
    if (enModemNo == VOICE_McGetForeGroundNum())
    {
        VOICE_McStop(enModemNo);

        /* ���õ�ǰ����Modem�� */
        VOICE_McSetForeGroundNum(VOICE_MC_MODEM_NUM_BUTT);
    }

    /* ����������ά�ɲ⣬����ǰmodem��suspendʱ�����0 */
    g_auwSuspendBeginTs[enModemNo] = 0;

    /* �ͷ�JB */
    VOICE_JB_Free();

    /* ���������ʼ�� */
    VOICE_McModemObjInit(VOICE_McGetModemObjPtr(enModemNo));

    /* �ظ���ϢID_VOICE_VC_STOP_CNF��NAS */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_STOP_CNF, pstStop->uwSenderPid, uwRet);

    OM_LogInfo(VOICE_MsgStopReq_OK);

    return uwRet;
}
VOS_UINT32 VOICE_MsgOmSetNvReqRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    MSG_CODEC_CNF_STRU                  stRspPrim;
    MSG_OM_MLIB_PARA_SET_STRU          *pstOmPara;
    MLIB_PARA_STRU                     *pstMlibPara;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          uwBufSize;

    pstOmPara = (MSG_OM_MLIB_PARA_SET_STRU*)pstOsaMsg;

    /* �������ݱ��ĵĿռ� */
    uwBufSize = sizeof(MLIB_PARA_STRU) + pstOmPara->uwSize;
    pucBuf = (VOS_UINT8*)UCOM_MemAlloc(uwBufSize);
    UCOM_MemSet(pucBuf, 0, uwBufSize);
    pstMlibPara = (MLIB_PARA_STRU*)pucBuf;

    /* �������ݱ���ͷ */
    pstMlibPara->uwChnNum     = 2;
    pstMlibPara->uwSampleRate = VOICE_ProcGetSampleRate();
    pstMlibPara->uwFrameLen   = ( (pstMlibPara->uwSampleRate == 16000) ? CODEC_FRAME_LENGTH_WB: CODEC_FRAME_LENGTH_NB);
    pstMlibPara->uwResolution = 16;
    pstMlibPara->enDevice     = VOICE_McGetDeviceMode();
    pstMlibPara->enParaSource = MLIB_PARA_SOURCE_TUNER;
    pstMlibPara->uwDataSize   = pstOmPara->uwSize;

    /* �������ݱ����� */
    UCOM_MemCpy(pstMlibPara->aucData, pstOmPara->aucData, pstOmPara->uwSize);

    /* �����ݱ��ķ�������Ӧģ�� */
    uwRet = MLIB_PathModuleSet(
                pstOmPara->uwPathID,
                pstOmPara->uwModuleID,
                pstMlibPara,
                uwBufSize);

    /* ���ظ���ϢID */
    stRspPrim.usMsgName = ID_VOICE_OM_SET_NV_CNF;

    /* ���ִ�н�� */
    stRspPrim.enExeRslt = (VOS_UINT16)uwRet;

    /* ��SDT�ظ���ϢID_VOICE_OM_SET_NV_CNF */
    OM_COMM_SendTranMsg(&stRspPrim, sizeof(stRspPrim));

    /* �ͷű����ڴ� */
    UCOM_MemFree(pucBuf);

    return uwRet;
}


VOS_UINT32 VOICE_MsgOmGetNvReqRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    MSG_OM_MLIB_PARA_GET_STRU          *pstOmPara;
    MSG_OM_MLIB_PARA_GET_STRU          *pstGetPara;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          uwSize;

    pstOmPara = (MSG_OM_MLIB_PARA_GET_STRU*)pstOsaMsg;

    uwSize = sizeof(MSG_OM_MLIB_PARA_GET_STRU) + pstOmPara->uwSize;
    pucBuf = (VOS_UINT8*) UCOM_MemAlloc(uwSize);

    pstGetPara = (MSG_OM_MLIB_PARA_GET_STRU*)pucBuf;

    UCOM_MemCpy(pstGetPara, pstOmPara, sizeof(MSG_OM_MLIB_PARA_GET_STRU));
    UCOM_MemCpy(pstGetPara->aucData, pstOmPara->aucData, pstOmPara->uwSize);

    uwRet = MLIB_PathModuleGet(
                pstOmPara->uwPathID,
                pstOmPara->uwModuleID,
                pstGetPara->aucData,
                pstOmPara->uwSize);

    if(UCOM_RET_SUCC == uwRet)
    {
        pstGetPara->usMsgName = ID_VOICE_OM_GET_NV_CNF;

        OM_COMM_SendTranMsg(pstGetPara, uwSize);
    }

    UCOM_MemFree(pucBuf);

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgOmQueryStatusReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                      uwRet    = UCOM_RET_SUCC;
    MSG_VOICE_OM_QUERY_RSP_STRU     stQueryMsg;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enActiveModemNo = VOICE_McGetForeGroundNum();
    MsgBlock                       *pstMsg    = VOS_NULL;
    VOICE_PROC_CODEC_STRU          *pstCodec;

    /* ��ǰ̨ͨ·�������� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enActiveModemNo)
    {
        return uwRet;
    }

    pstCodec    = VOICE_McGetModemCodecPtr(enActiveModemNo);
    pstMsg      = (MsgBlock*)pstOsaMsg;

    UCOM_MemSet(&stQueryMsg, 0, sizeof(stQueryMsg));

    stQueryMsg.usMsgId       = ID_VOICE_OM_QUERY_STATUS_CNF;
    stQueryMsg.usState       = VOICE_McGetVoiceState(pstMsg->uwSenderPid);
    stQueryMsg.enNetMode     = VOICE_McGetNetMode(pstMsg->uwSenderPid);
    stQueryMsg.enCodecType   = pstCodec->enCodecType;
    stQueryMsg.enDtxMode     = (VOS_UINT16)pstCodec->stEncInObj.enDtxMode;
    stQueryMsg.enAmrMode     = (VOS_UINT16)pstCodec->stEncInObj.enAmrMode;
    stQueryMsg.stInout       = *(VOICE_McGetInoutPtr());

    /*����OM_COMM_SendTranMsg�ӿڽ���Transͷ��䣬�����͸�SDT*/
    OM_COMM_SendTranMsg(&stQueryMsg, sizeof(MSG_VOICE_OM_QUERY_RSP_STRU));

    OM_LogInfo(VOICE_MsgQueryStatusReq_Ok);
    return uwRet;
}


VOS_UINT32 VOICE_MsgPhySuspendCmdR(VOS_VOID *pstOsaMsg)
{
    VOICE_MC_MODEM_NUM_ENUM_UINT16    enModemNo;
    MsgBlock                         *pstSuspend = VOS_NULL;

    /* ��ȡSenderPid */
    pstSuspend     = (MsgBlock *)pstOsaMsg;

    /* ����SenderPid��ȡͨ���� */
    enModemNo = VOICE_McGetModemNum(pstSuspend->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* �����ǰΪǰ̨��ֹͣ�豸 */
    if (enModemNo == VOICE_McGetForeGroundNum())
    {
        /* ����������豸ֹͣ */
        VOICE_McStop(enModemNo);
    }

    /*���������״̬��Ϊ��ͣ*/
    VOICE_McSetVoiceState(enModemNo, VOICE_MC_STATE_PAUSE);

    /* ��¼��־ */
    OM_LogInfo(VOICE_MsgSuspendCmd_OK);

    /* ����ErrorLog������ */
    VOICE_ErrLogReset();

    /* ��¼suspend��Ϣʱ��� */
    g_auwSuspendBeginTs[enModemNo] = DRV_TIMER_ReadSysTimeStamp();

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgWphySyncIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT16                  uhwCfn;
    VOS_UINT32                  uwSyncOffset;
    VOS_UINT32                  uwRet      = UCOM_RET_FAIL;
    WPHY_VOICE_SYNC_IND_STRU   *pstSyncMsg = VOS_NULL;
    VOS_UINT32                  uwTimerLen;
    VOS_UINT16                  uhwModemNo;
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU        *pstTimer   = VOICE_McGetTimerPtr();
    VOS_UINT16                  uhwActiveModemNo = VOICE_McGetForeGroundNum();

    /* ��WPHY_VOICE_SYNC_IND_STRU�ṹ������Ϣ����*/
    pstSyncMsg  = (WPHY_VOICE_SYNC_IND_STRU*)pstOsaMsg;

    /* ��ȡͨ·�� */
    uhwModemNo  = VOICE_McGetModemNum(pstSyncMsg->uwSenderPid);

    /* ��ǰ����ǰ̨�������� */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* ��ȡ��Ϣ����ʱ�̵�Cfn�� */
    uhwCfn      = pstSyncMsg->uhwCfn;

    /* ����Sync��ϢΪż��֡������Ϊ�쳣 */
    if (0 == (uhwCfn % 2))
    {
        /* ��¼�쳣 */
        uwRet = UCOM_RET_ERR_MSG;
    }
    else
    {
        /* ��ȡSOC����������ȡ��ǰʱ��������*/
        pstDmaFlag->uwSyncTimestamp = DRV_TIMER_ReadSysTimeStamp();

        /* ������Ϣ����ʱ�̾��뱾֡֡ͷ�ľ��벢���棬��λ0.1ms */
        uwSyncOffset = (((pstSyncMsg->uhwSlotOffset * VOICE_CHIP_NUMBER_BY_SLOT)
                + pstSyncMsg->uhwChipOffset) * 100) / VOICE_CHIP_NUMBER_BY_FRAME;

        /* �������г�ͻ��⣬��Ҫ����ͬ����Ϣ��ǰʱ�� */
        pstDmaFlag->uwSyncOffset = uwSyncOffset;

        /*������DMA��־Ϊδ������������3ms��ʱ��������UMTS����DMA����ʱ��*/
        if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
        {
            /*��ʱ������uwTimerLen = VOICE_UMTS_TX_BEGIN_TIMESTAMP - uwSyncOffset;*/
            uwTimerLen = VOICE_UMTS_TX_BEGIN_TIMESTAMP - uwSyncOffset;

            /*����3ms��ʱ��,������ʱ��ָ�뱣��Ϊg_stVoiceMcObjs.stTimer.pTimer3ms*/
            DRV_TIMER_Start(&(pstTimer->uwTimer3ms),
                            uwTimerLen,
                            DRV_TIMER_MODE_ONESHOT,
                            VOICE_Timer3msIsr,
                            0);
        }

        /*������DMA��־Ϊδ������������6ms��ʱ��������UMTS����DMA����ʱ��*/
        if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
        {
            /*��ʱ������ = VOICE_UMTS_RX_BEGIN_TIMESTAMP - uwSyncOffset;*/
            uwTimerLen = VOICE_UMTS_RX_BEGIN_TIMESTAMP - uwSyncOffset;

            /*����6ms��ʱ��,������ʱ��ָ�뱣��Ϊg_stVoiceMcObjs.stTimer.uwTimer6ms*/
            DRV_TIMER_Start(&(pstTimer->uwTimer6ms),
                            uwTimerLen,
                            DRV_TIMER_MODE_ONESHOT,
                            VOICE_Timer6msIsr,
                            0);
        }

        uwRet = UCOM_RET_SUCC;
    }

    /* ��¼��־ */
    OM_LogFreqInfo(VOICE_MsgWphySyncInd_Done, VOICE_LOG_PERIOD);

    return uwRet;
}


VOS_UINT32 VOICE_MsgTdSyncIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwTimerLen;
    VOS_UINT16                          uhwModemNo;
    VOS_UINT32                          uwSyncOffset = 0;                     /*��ǰ�յ��жϵ�ʱ����ż��֡��ʼʱ�̵�һ��*/
    VOICE_VOICE_TD_SYNC_IND_STRU       *pstSyncMsg;
    VOICE_MC_DMA_FLAG_STRU             *pstDmaFlag   = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU                *pstTimer     = VOICE_McGetTimerPtr();


    /* ��TDPHY_MED_SYNC_IND_STRU�ṹ������Ϣ����*/
    pstSyncMsg  = (VOICE_VOICE_TD_SYNC_IND_STRU*)pstOsaMsg;

    if (NULL == pstSyncMsg)
    {
        return UCOM_RET_ERR_STATE;
    }

    /* ��ȡͨ·�� */
    uhwModemNo  = VOICE_McGetModemNum(pstSyncMsg->uwSenderPid);

    /* ��ǰ����ǰ̨�������� */
    if (VOICE_MC_CODEC_RANGE != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /*�жϵ�ǰ��������Ƿ���running̬*/
    if (VOICE_MC_STATE_RUNNING != VOICE_McGetVoiceState(pstSyncMsg->uwSenderPid))
    {
        return UCOM_RET_ERR_STATE;
    }

    /* ��ȡSOC����������ȡ��ǰʱ��������*/
    pstDmaFlag->uwSyncTimestamp = DRV_TIMER_ReadSysTimeStamp();
    pstDmaFlag->uwSyncOffset    = 0;

    /*������DMA��־Ϊδ��������������ʱ��������TDS-CDMA����DMA����ʱ��*/
    if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
    {
        /*��ʱ������uwTimerLen = VOICE_TDS_TX_BEGIN_TIMESTAMP - uwSyncOffset;*/
        uwTimerLen = VOICE_TDS_TX_BEGIN_TIMESTAMP - uwSyncOffset;

        /*����0.3ms��ʱ��,������ʱ��ָ�뱣��Ϊg_stMedMcObjs.stTimer.uwTdTxTimer*/
        DRV_TIMER_Start(&(pstTimer->uwTdTxTimer),
                          uwTimerLen,
                          DRV_TIMER_MODE_ONESHOT,
                          VOICE_TdsTxTimerIsr,
                          0);
    }

    /*������DMA��־Ϊδ��������������ʱ��������TDS-CDMA����DMA����ʱ��*/
    if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
    {
        /*��ʱ������ = VOICE_TDS_RX_BEGIN_TIMESTAMP - uwSyncOffset;*/
        uwTimerLen = VOICE_TDS_RX_BEGIN_TIMESTAMP - uwSyncOffset;

        /*����8ms��ʱ��,������ʱ��ָ�뱣��Ϊg_stMedMcObjs.stTimer.uwTdRxTimer*/
        DRV_TIMER_Start(&(pstTimer->uwTdRxTimer),
                          uwTimerLen,
                          DRV_TIMER_MODE_ONESHOT,
                          VOICE_TdsRxTimerIsr,
                          0);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgWphyRxDataIndR(VOS_VOID *pstOsaMsg)
{
    WPHY_VOICE_RX_DATA_IND_STRU    *pstDecMsg        = VOS_NULL;
    VOS_UINT16                      uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                      uhwModemNo;

    /* ��ID_WPHY_VOICE_RX_DATA_IND��Ϣ��ʽ���� */
    pstDecMsg = (WPHY_VOICE_RX_DATA_IND_STRU*)pstOsaMsg;

    /* ��ȡͨ·�� */
    uhwModemNo  = VOICE_McGetModemNum(pstDecMsg->uwSenderPid);

    /* ��ǰ����ǰ̨�������� */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* �����յ�������ѹ����뻺������� */
    VOICE_ProcAddDecSerialWrite(pstDecMsg, sizeof(WPHY_VOICE_RX_DATA_IND_STRU));

    #ifdef _MED_ERRORLOG
    /* ��֡ */
    if (VOICE_ERRLOG_FQI_QUALITY_BAD == pstDecMsg->enQualityIdx)
    {
        /* ErrorLog ��¼�¼� */
        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_WCDMA_BBP_DATA_BAD);
    }

    /* ErrorLog ��¼�¼� */
    VOICE_ErrLogEventRec(VOICE_ERRLOG_EVENTS_WCDMA_DEC_INT_RECEIVE);

    #endif

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgGphyRxDataIndR(VOS_VOID *pstOsaMsg)
{
    GPHY_VOICE_RX_DATA_IND_STRU *pstDecMsg        = VOS_NULL;
    VOICE_PROC_CODEC_STRU       *pstCodec         = VOICE_ProcGetCodecPtr();
    VOS_UINT16                   uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                   uhwModemNo;

    pstDecMsg   = (GPHY_VOICE_RX_DATA_IND_STRU*)pstOsaMsg;

    /* ��ȡͨ·�� */
    uhwModemNo  = VOICE_McGetModemNum(pstDecMsg->uwSenderPid);

    /* ��ǰ����ǰ̨�������� */
    if (uhwActiveModemNo != uhwModemNo)
    {
        OM_LogWarning(VOICE_MsgGphyRxDataInd_ModemMismatch);
        return UCOM_RET_SUCC;
    }

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* �жϸ�֡�������Ƿ��SetCodec�е���Ϣһ�£�����һ��������֡��Ϣ */
    if ( pstDecMsg->enCodecType != pstCodec->enCodecType )
    {
        OM_LogWarning(VOICE_MsgGphyRxDataInd_CodecTypeMismatch);
        VOICE_McSetGsmCodecTypeMismatch(uhwModemNo, VOICE_PROC_IS_YES);

        /* ������ֱ�ӷ��� */
        return UCOM_RET_ERR_STATE;
    }
    else
    {
        VOICE_McSetGsmCodecTypeMismatch(uhwModemNo, VOICE_PROC_IS_NO);
    }

    /* �����յ�������ѹ����뻺������� */
    VOICE_ProcAddDecSerialWrite(pstDecMsg, sizeof(GPHY_VOICE_RX_DATA_IND_STRU));

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgTDphyRxDataIndR(VOS_VOID *pstOsaMsg)
{
    TDPHY_VOICE_RX_DATA_IND_STRU       *pstDecMsg        = MED_NULL;
    VOS_UINT16                          uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                          uhwModemNo;

    /* ��ID_TDPHY_MED_RX_DATA_IND��Ϣ��ʽ���� */
    pstDecMsg   = (TDPHY_VOICE_RX_DATA_IND_STRU*)pstOsaMsg;

    /* ��ȡͨ���� */
    uhwModemNo  = VOICE_McGetModemNum(pstDecMsg->uwSenderPid);

    /* ��ǰ����ǰ̨���������� */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /*�жϵ�ǰ��������Ƿ���running̬*/
    if (VOICE_MC_STATE_RUNNING != VOICE_McGetVoiceState(pstDecMsg->uwSenderPid))
    {
        return UCOM_RET_ERR_STATE;
    }

    /* �����յ�������ѹ����뻺������� */
    VOICE_ProcAddDecSerialWrite(pstDecMsg, sizeof(TDPHY_VOICE_RX_DATA_IND_STRU));

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgImsaRxDataIndR( VOS_VOID *pstOsaMsg )
{
    IMSA_VOICE_RX_DATA_IND_STRU        *pstImsaMsg       = MED_NULL;
    VOS_UINT16                          uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                          uhwModemNo;
    VOS_UINT32                          uwRet;

    /* ��IMSA_VOICE_RX_DATA_IND��Ϣ��ʽ���� */
    pstImsaMsg = (IMSA_VOICE_RX_DATA_IND_STRU*)pstOsaMsg;

    /* ��ȡͨ���� */
    uhwModemNo  = VOICE_McGetModemNum(pstImsaMsg->uwSenderPid);

    /* ��ǰ����ǰ̨���������� */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /*�жϵ�ǰ��������Ƿ���running̬*/
    if (VOICE_MC_STATE_RUNNING != VOICE_McGetVoiceState(pstImsaMsg->uwSenderPid))
    {
        return UCOM_RET_ERR_STATE;
    }

    /* �����б���������� */
    uwRet = VOICE_ProcCfgImsCodec(pstImsaMsg->usCodecType,
                                  pstImsaMsg->usDtxEnable,
                                  pstImsaMsg->usRateMode);

    /* ��������ƥ�䣬��Ϊ��֡���� */
    if (UCOM_RET_SUCC != uwRet)
    {
        OM_LogWarning(VOICE_MsgImsaRxDataInd_CodecTypeMismatch);

        /* ��QualificationΪbad */
        pstImsaMsg->usQualityIdx = CODEC_AMR_FQI_QUALITY_BAD;
    }

    /* �����͸���̬JB��AJB */
    VOICE_JB_AddPkt(pstOsaMsg);

    return uwRet;
}


VOS_UINT32 VOICE_MsgImsaCfgReqR( VOS_VOID *pstOsaMsg )
{
    IMSA_VOICE_CFG_REQ_STRU            *pstImsaCfgMsg    = MED_NULL;
    VOICE_IMSA_CFG_CNF_STRU             stImsaCfnMsg;
    VOS_UINT32                          uwRet;

    /* ��ID_IMSA_VOICE_CFG_REQ��Ϣ��ʽ���� */
    pstImsaCfgMsg = (IMSA_VOICE_CFG_REQ_STRU*)pstOsaMsg;

    if ((VOICE_JB_PKT_TIME_20MS == pstImsaCfgMsg->ulTransTime)
      ||(VOICE_JB_PKT_TIME_40MS == pstImsaCfgMsg->ulTransTime))
    {
        /* ���ô��ʱ�� */
        uwRet = VOICE_JB_SetSjbPktTimeTx(pstImsaCfgMsg->ulTransTime );
        VOICE_JB_SetLtePktTime(pstImsaCfgMsg->ulTransTime);
    }
    else
    {
        uwRet = UCOM_RET_ERR_MSG;
    }

    /* �ظ���ϢID_VOICE_IMSA_CFG_CNF */
    stImsaCfnMsg.usMsgId    = ID_VOICE_IMSA_CFG_CNF;
    stImsaCfnMsg.ulResult   = uwRet;

    UCOM_SendOsaMsg(DSP_PID_VOICE_RT,
                    pstImsaCfgMsg->uwSenderPid,
                    &stImsaCfnMsg,
                    sizeof(VOICE_IMSA_CFG_CNF_STRU));

    return uwRet;
}
VOS_UINT32 VOICE_MsgGphyUpdateParaCmdRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                       uwRet;
    GPHY_VOICE_UPDATE_PARA_CMD_STRU *pstUpdateMsg;
    VOICE_PROC_CODEC_STRU           *pstCodec;
    VOS_UINT16                       uhwModemNo;

    /* ��GPHY_VOICE_UPDATE_PARA_CMD_STRU�ṹ������Ϣ���� */
    pstUpdateMsg = (GPHY_VOICE_UPDATE_PARA_CMD_STRU*)pstOsaMsg;

    /* ��ȡͨ·�� */
    uhwModemNo   = VOICE_McGetModemNum(pstUpdateMsg->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ��ȡ�������� */
    pstCodec     = VOICE_McGetModemCodecPtr(uhwModemNo);

    /* ����VOICE_ProcCfgGsmUlCodec�������ݽ��б���������� */
    uwRet = VOICE_ProcCfgGsmUlCodec(pstUpdateMsg,uhwModemNo);

    /* �����������Ͳ�ƥ�䣬�ϱ���Ӧ�ľ�����Ϣ */
    if (pstUpdateMsg->enCodecType != pstCodec->enCodecType)
    {
        VOICE_McSetGsmCodecTypeMismatch(uhwModemNo,VOICE_PROC_IS_YES);
        OM_LogWarning(VOICE_MsgUpdateGsmParaCmd_CodecTypeMismatch);
    }
    else
    {
        VOICE_McSetGsmCodecTypeMismatch(uhwModemNo,VOICE_PROC_IS_NO);
        OM_LogInfo1(VOICE_MsgUpdateGsmParaCmd_Ok, uwRet);
    }

    return uwRet;

}


VOS_UINT32 VOICE_MsgGphyPlayIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    VOICE_GPHY_PLAY_CNF_STRU            stCnfMsg;
    VOS_UINT16                          uhwNetMode;         /* ����ģʽ */
    VOS_UINT16                          uhwModemNo;
    VOS_UINT16                          uhwActiveModemNo = VOICE_McGetForeGroundNum();
    MsgBlock                           *pstMsg           = VOS_NULL;

    /* ��ȡ��SenderPid */
    pstMsg        = (MsgBlock *)pstOsaMsg;

    /* ��ȡͨ���� */
    uhwModemNo    = VOICE_McGetModemNum(pstMsg->uwSenderPid);

    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);

    if(   (CODEC_NET_MODE_G  == uhwNetMode)
       && (VOICE_PROC_IS_YES == VOICE_McGetGsmCodecTypeMismatch(uhwModemNo)))
    {
        OM_LogWarning(VOICE_MsgPlayInd_CodecTypeMismatch);
        uwRet = UCOM_RET_FAIL;
    }
    else
    {
        /* ������������ */
        uwRet = VOICE_McPlay();
    }

    /* �ظ���ϢID_VOICE_GPHY_PLAY_CNF */
    stCnfMsg.uhwMsgId = ID_VOICE_GPHY_PLAY_CNF;
    stCnfMsg.uwRslt   = uwRet;

    /* �ظ���ϢID_VOICE_GPHY_PLAY_CNF */
    UCOM_SendOsaMsg(DSP_PID_VOICE_RT,
                    pstMsg->uwSenderPid,
                    &stCnfMsg,
                    sizeof(VOICE_GPHY_PLAY_CNF_STRU));

    /* ��¼��־ */
    OM_LogInfo(VOICE_MsgPlayInd_Done);

    return uwRet;
}


VOS_UINT32 VOICE_MsgGphyRecordIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                  uwRet;
    VOICE_GPHY_RECORD_CNF_STRU  stCnfMsg;
    VOS_UINT16                  uhwNetMode;         /* ����ģʽ */
    VOS_UINT16                  uhwModemNo;
    VOS_UINT16                  uhwActiveModemNo = VOICE_McGetForeGroundNum();
    MsgBlock                   *pstMsg      = VOS_NULL;

    /* ��ȡ��SenderPid */
    pstMsg        = (MsgBlock *)pstOsaMsg;

    /* ��ȡͨ���� */
    uhwModemNo    = VOICE_McGetModemNum(pstMsg->uwSenderPid);

    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    if(   (CODEC_NET_MODE_G  == uhwNetMode)
       && (VOICE_PROC_IS_YES == VOICE_McGetGsmCodecTypeMismatch(uhwModemNo)))
    {
        OM_LogWarning(VOICE_MsgRecordInd_CodecTypeMismatch);
        uwRet = UCOM_RET_FAIL;
    }
    else
    {
        /* ���������ɼ� */
        uwRet = VOICE_McRecord();
    }

    /* �ظ���ϢID_VOICE_GPHY_RECORD_CNF */
    stCnfMsg.uhwMsgId = ID_VOICE_GPHY_RECORD_CNF;
    stCnfMsg.uwRslt   = uwRet;

    /* �ظ���ϢID_VOICE_GPHY_RECORD_CNF */
    UCOM_SendOsaMsg(DSP_PID_VOICE_RT,
                    pstMsg->uwSenderPid,
                    &stCnfMsg,
                    sizeof(VOICE_GPHY_RECORD_CNF_STRU));

    /* ��¼��־ */
    OM_LogInfo(VOICE_MsgRecordInd_Done);

    return uwRet;
}



VOS_UINT32 VOICE_MsgGphySyncIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                  uwSyncOffset;
    VOS_UINT32                  uwRet      = UCOM_RET_SUCC;
    GPHY_VOICE_SYNC_IND_STRU   *pstSyncMsg = VOS_NULL;
    VOS_UINT32                  uwTimerLen;
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU        *pstTimer   = VOICE_McGetTimerPtr();
    VOS_UINT32                  uwTxDataOffset;
    VOS_UINT16                  uhwModemNo;
    VOS_UINT16                  uhwActiveModemNo = VOICE_McGetForeGroundNum();

    /* ��GPHY_VOICE_SYNC_IND_STRU�ṹ������Ϣ����*/
    pstSyncMsg  = (GPHY_VOICE_SYNC_IND_STRU*)pstOsaMsg;

    /* ��ȡͨ·�� */
    uhwModemNo  = VOICE_McGetModemNum(pstSyncMsg->uwSenderPid);

    /* ��ǰ����ǰ̨�������� */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* ��ȡSOC����������ȡ��ǰʱ��������*/
    pstDmaFlag->uwSyncTimestamp = DRV_TIMER_ReadSysTimeStamp();

    /* ������Ϣ�յ�ʱ�̾�����Ϣ�е�12֡��0ʱ϶�ľ��룬��λ0.1ms */
    uwSyncOffset = ((VOICE_GSM_QB_PARA * pstSyncMsg->uwQb) / (5000 * 100));

    /* �������г�ͻ��⣬��Ҫ����ͬ����Ϣ��ǰʱ�̺�GPHY�ŵ�����ʱ�� */
    pstDmaFlag->uwSyncOffset   = uwSyncOffset;
    pstDmaFlag->uwChanCodeTime = (VOS_UINT32)pstSyncMsg->uhwChanCodeTime;

    /* ����GPHYҪ���ϱ��������ݵ�ʱ��ƫ�� */
    if(pstSyncMsg->uhwChanCodeTime <= (VOICE_GSM_RX_DMA_TIMESTAMP + uwSyncOffset))
    {
        uwTxDataOffset = pstSyncMsg->uhwChanCodeTime + VOICE_GSM_ONE_FRAME_OFFSET;
    }
    else
    {
        uwTxDataOffset = pstSyncMsg->uhwChanCodeTime;
    }

    /*������DMA��־Ϊδ���������������ж�ʱ��������GSM����DMA����ʱ��*/
    if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
    {
        /*��ʱ������*/
        uwTimerLen = (uwTxDataOffset - VOICE_GSM_TX_DMA_TIMESTAMP) - uwSyncOffset;

        /*����3ms��ʱ��,������ʱ��ָ�뱣��Ϊg_stVoiceMcObjs.stTimer.pTimer3ms*/
        DRV_TIMER_Start(&(pstTimer->uwTimer3ms),
                        uwTimerLen,
                        DRV_TIMER_MODE_ONESHOT,
                        VOICE_Timer3msIsr,
                        0);

    }

    /*������DMA��־Ϊδ������������20ms��ʱ��������GSM����DMA����ʱ��*/
    if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
    {
        /*��ʱ������*/
        uwTimerLen = (uwTxDataOffset - VOICE_GSM_RX_DMA_TIMESTAMP) - uwSyncOffset;

        /*����20ms��ʱ��,������ʱ��ָ�뱣��Ϊg_stVoiceMcObjs.stTimer.uwTimer6ms*/
        DRV_TIMER_Start(&(pstTimer->uwTimer6ms),
                        uwTimerLen,
                        DRV_TIMER_MODE_ONESHOT,
                        VOICE_Timer6msIsr,
                        0);
    }

    /* ��¼��־ */
    OM_LogInfo(VOICE_MsgGphySyncInd_Done);

    return uwRet;
}


VOS_UINT32 VOICE_MsgGphyChannelQualityIndR( VOS_VOID *pstOsaMsg )
{
    GPHY_VOICE_CHANNEL_QUALITY_IND_STRU *pstChannelQualityMsg = VOS_NULL;
    VOS_UINT16                           uhwActiveModemNo     = VOICE_McGetForeGroundNum();
    VOS_UINT16                           uhwModemNo;
    VOICE_DIAG_NV_STRU                  *pstDiagCfg           = VOICE_DiagGetCfgPtr();
    VOICE_DIAG_CHANNEL_QUALITY_STRU     *pstDiagChanQa        = VOICE_DiagGetChannelQualityPtr();
    VOS_UINT32                           uwDiagChanQaSize;
    VOS_UINT32                           uwOsaMsgTextSize;
    VOS_INT16                            shwGSMChanQaLev;

    /* ����������ϼ��ģ���б�����ŵ�������Ϣ */
    UCOM_MemSet(pstDiagChanQa, 0, sizeof(VOICE_DIAG_CHANNEL_QUALITY_STRU));

    pstChannelQualityMsg = (GPHY_VOICE_CHANNEL_QUALITY_IND_STRU*)pstOsaMsg;
    uwOsaMsgTextSize     = sizeof(GPHY_VOICE_CHANNEL_QUALITY_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* ��ȡͨ·�� */
    uhwModemNo  = VOICE_McGetModemNum(pstChannelQualityMsg->uwSenderPid);

    /* ��ǰ����ǰ̨�������� */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ��⵱ǰ�ź����� */
    shwGSMChanQaLev = (VOS_INT16)pstChannelQualityMsg->uhwRxLevVal - VOICE_DIAG_GSM_RX_LEVEL_OFFSET;

    if(shwGSMChanQaLev <= pstDiagCfg->stChanQaPara.shwGsmChanQaThd)
    {
        pstDiagChanQa->uhwIsBadCell = VOICE_PROC_IS_YES;

        OM_LogWarning(VOICE_MsgGphyChannelQualityIndR_BadCell);
    }

    /* ������ϱ���Ϣ��С���ܳ���Ԥ����С */
    uwDiagChanQaSize = VOICE_DIAG_CHANNEL_QUALITY_LEN * sizeof(VOS_UINT32);

    if(uwOsaMsgTextSize > uwDiagChanQaSize)
    {
        OM_LogError(VOICE_MsgGphyChannelQualityIndR_Overflow);

        return UCOM_RET_FAIL;
    }

    /* ����GPHY�ŵ�������Ϣ�������������ϼ�� */
    UCOM_MemCpy(pstDiagChanQa->auwChannelQuality,
                &pstChannelQualityMsg->uhwMsgId,
                uwOsaMsgTextSize);

    return UCOM_RET_SUCC;

}
VOS_UINT32 VOICE_MsgWphyChannelQualityIndR( VOS_VOID *pstOsaMsg  )
{
    WPHY_VOICE_CHANNEL_QUALITY_IND_STRU *pstChannelQualityMsg = VOS_NULL;
    VOS_UINT16                           uhwActiveModemNo     = VOICE_McGetForeGroundNum();
    VOS_UINT16                           uhwModemNo;
    VOICE_DIAG_NV_STRU                  *pstDiagCfg           = VOICE_DiagGetCfgPtr();
    VOICE_DIAG_CHANNEL_QUALITY_STRU     *pstDiagChanQa        = VOICE_DiagGetChannelQualityPtr();
    VOS_UINT32                           uwDiagChanQaSize;
    VOS_UINT32                           uwOsaMsgTextSize;
    VOS_INT16                            shwUMTSChanQaLev;
    VOS_UINT16                           uhwServCellId;

    /* ����������ϼ��ģ���б�����ŵ�������Ϣ */
    UCOM_MemSet(pstDiagChanQa, 0, sizeof(VOICE_DIAG_CHANNEL_QUALITY_STRU));

    pstChannelQualityMsg = (WPHY_VOICE_CHANNEL_QUALITY_IND_STRU*)pstOsaMsg;
    uwOsaMsgTextSize     = sizeof(WPHY_VOICE_CHANNEL_QUALITY_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* ��ȡͨ·�� */
    uhwModemNo  = VOICE_McGetModemNum(pstChannelQualityMsg->uwSenderPid);

    /* ��ǰ����ǰ̨�������� */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ��⵱ǰ�ŵ����� */
    uhwServCellId = pstChannelQualityMsg->uhwServCellId;
    if(uhwServCellId >= WPHY_VOICE_WCDMA_MAX_CELL_NUM)
    {
        OM_LogError(VOICE_MsgWphyChannelQualityIndR_ServCellId_Err);

        return UCOM_RET_FAIL;
    }

    shwUMTSChanQaLev = pstChannelQualityMsg->astCellInfo[uhwServCellId].shwCpichEcN0;

    if((shwUMTSChanQaLev >> 3) <= pstDiagCfg->stChanQaPara.shwUmtsChanQaThd)
    {
        pstDiagChanQa->uhwIsBadCell = VOICE_PROC_IS_YES;

        OM_LogWarning(VOICE_MsgWphyChannelQualityIndR_BadCell);
    }

    uwDiagChanQaSize = VOICE_DIAG_CHANNEL_QUALITY_LEN * sizeof(VOS_UINT32);

    /* ������ϱ���Ϣ��С���ܳ���Ԥ����С */
    if(uwOsaMsgTextSize > uwDiagChanQaSize)
    {
        OM_LogError(VOICE_MsgWphyChannelQualityIndR_Overflow);

        return UCOM_RET_FAIL;
    }

    /* ����WPHY�ŵ�������Ϣ�������������ϼ�� */
    UCOM_MemCpy(pstDiagChanQa->auwChannelQuality,
                &pstChannelQualityMsg->uhwMsgId,
                uwOsaMsgTextSize);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgVoicePlayIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32  uwRet;

    /* ������������ */
    uwRet = VOICE_McPlay();

    /* ��¼��־ */
    OM_LogInfo(VOICE_MsgPlayInd_Done);

    return uwRet;
}


VOS_UINT32 VOICE_MsgVoiceRecordIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32  uwRet;

    /* ���������ɼ� */
    uwRet = VOICE_McRecord();

    /* ��¼��־ */
    OM_LogInfo(VOICE_MsgRecordInd_Done);

    return uwRet;
}


VOS_UINT32 VOICE_MsgVoiceTxProcReqR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32           uwRet;

    /* ��¼�Ѿ��յ��˱�����Ϣ,���ڼ���������� */
    g_stDiagDmaIsr.uhwTxDmaIsrFlag = VOICE_DIAG_TRUE;

    /* �������д���ǰ��׼������, ����AMR���ʵ��� */
    uwRet = VOICE_McPreTxProc();
    if(uwRet != UCOM_RET_SUCC)
    {
        return uwRet;

    }

    /* ��I2S���������ϲ����ݷֽ���������� */
    uwRet = VOICE_PcmHybrid2Stereo(g_psVoicePcmMicIn,
                                   VOICE_PcmGetMcInBufPtr(),
                                   VOICE_PcmGetRcInBufPtr(),
                                   CODEC_PCM_FRAME_LENGTH);
    if (UCOM_RET_SUCC != uwRet)
    {
        return  UCOM_RET_FAIL;
    }

    /* �������б��� */
    uwRet = VOICE_McTxEnc();
    if (UCOM_RET_SUCC != uwRet)
    {
        return  UCOM_RET_FAIL;
    }

    /* �������б����� */
    VOICE_McPostTxProc();


    /* ��������PP���� */
    uwRet = VOICE_McTxProc();
    if(uwRet != UCOM_RET_SUCC)
    {
        return uwRet;

    }

    return uwRet;
}


VOS_UINT32 VOICE_MsgVoiceRxProcReqR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32 uwRet;

    /* ��¼�Ѿ��յ��˽�����Ϣ,���ڼ���������� */
    g_stDiagDmaIsr.uhwRxDmaIsrFlag = VOICE_DIAG_TRUE;

    /* ��֡���д��� */
    uwRet = VOICE_McRxProc();

    return uwRet;
}


VOS_UINT32 VOICE_MsgVcForeGroundReqRP( VOS_VOID *pstOsaMsg  )
{
    VOS_UINT32                       uwRet;
    VOS_UINT32                       uwSenderPid;                /* nas��PID */
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enModemNo;
    CODEC_NET_MODE_ENUM_UINT16       enNetMode;
    VOICE_PROC_CODEC_STRU           *pstCodec;
    VCVOICE_FOREGROUND_REQ_STRU     *pstForeGroundReq = VOS_NULL;
    VOICE_PROC_OBJS_STRU            *pstProc          = VOICE_ProcGetObjsPtr();
    VOICE_MC_DMA_FLAG_STRU          *pstDmaFlag = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                       uwUlTimer;
    VOS_UINT32                       uwDlTimer;

    /* ��ȡsenderPid */
    pstForeGroundReq = (VCVOICE_FOREGROUND_REQ_STRU*)pstOsaMsg;
    uwSenderPid      = pstForeGroundReq->uwSenderPid;

    /* ����senderPid��ȡͨ���� */
    enModemNo = VOICE_McGetModemNum(uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ����ͨ����,��ȡ��Ӧmodem���� */
    enNetMode     = VOICE_McGetModemNetMode(enModemNo);

    /* �Ѿ���foreground��,�ظ�nas�ɹ� */
    if(enModemNo == VOICE_McGetForeGroundNum())
    {
        VOICE_McSendCnfMsgToVc(ID_VOICE_VC_FOREGROUND_CNF,
                               uwSenderPid,
                               VCVOICE_EXECUTE_RSLT_SUCC);

        return UCOM_RET_SUCC;
    }

    /* ��һ��������ǰ̨״̬������һ·����Ϊ��̨ */
    if(VOICE_MC_MODEM_NUM_BUTT > VOICE_McGetForeGroundNum())
    {
        VOICE_McSetBackGround(VOICE_McGetForeGroundNum());
    }

    /* PROCȫ�ֱ����е�codecָ����Ϊ��ͨ·��codec */
    pstProc->pstCodec = VOICE_McGetModemCodecPtr(enModemNo);

    /* ����PMC��IOͨ��������PP������� */
    uwRet = VOICE_McUpdate(enModemNo);

    /* �ظ�nasʧ�� */
    if(UCOM_RET_SUCC != uwRet)
    {
        pstProc->pstCodec = VOS_NULL;

        VOICE_McSendCnfMsgToVc(ID_VOICE_VC_FOREGROUND_CNF,
                               uwSenderPid,
                               VCVOICE_EXECUTE_RSLT_FAIL);

        return UCOM_RET_FAIL;
    }

    /* TD��5ms֡ͬ���ж� */
    if(CODEC_NET_MODE_TD == enNetMode)
    {
        /* ����V9R1 <<Hi6620V100 SOC�߼��������_DMA������жϷ���.xlsx>>,����Ϊtds_stu_hifi_int��ID����� */
        VOS_EnableInterrupt(DRV_TDSCDMA_CTRL_SYNC_INTR);

        /* ʹ�����μĴ�����ʹ��״̬Ϊ������״̬ */
        UCOM_RegBitWr(DRV_TDSCDMA_CTRL_EN_ADDR, UCOM_BIT0, UCOM_BIT0, 1);
    }

    /* ��������DMA */
    if(CODEC_NET_MODE_L == enNetMode)
    {
        /*������DMA��־Ϊδ������������5ms��ʱ��������IMS����DMA����ʱ��*/
        if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
        {
            /* ��������DMA */
            DRV_TIMER_Start(&uwDlTimer,
                            VOICE_IMS_RX_BEGIN_TIMESTAMP,
                            DRV_TIMER_MODE_ONESHOT,
                            VOICE_Timer6msIsr,
                            0);
        }

        /*������DMA��־Ϊδ������������4ms��ʱ��������IMS����DMA����ʱ��*/
        if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
        {
            /* ��������DMA */
            DRV_TIMER_Start(&uwUlTimer,
                            VOICE_IMS_TX_BEGIN_TIMESTAMP,
                            DRV_TIMER_MODE_ONESHOT,
                            VOICE_Timer3msIsr,
                            0);
        }

        /* IMS�����������������,Ĭ������wbΪ23.05��nbΪ12.2 */
        VOICE_ProcCfgImsCodec((pstProc->pstCodec)->enCodecType,CODEC_DTX_DISABLE,CODEC_AMR_RATE_MODE_122K);
    }

    /* ����ͨ·״̬Ϊǰ̨ */
    VOICE_McSetModemState(enModemNo, VOICE_MC_MODEM_STATE_FOREGROUND);

    /* ���õ�ǰ����Modem�� */
    VOICE_McSetForeGroundNum(enModemNo);

    /* FA͵֡���������� */
    VOICE_ProcFacchFrameCntReset();

    /* �������������г�ʼ����־����Ϊδ��ʼ�� */
    pstCodec = VOICE_ProcGetCodecPtr();
    pstCodec->usIsDecInited = VOICE_PROC_IS_NO;
    pstCodec->usIsEncInited = VOICE_PROC_IS_NO;

    /* ֪ͨ������˳���Ĭ */
    VOICE_McModemInformToPhy(enNetMode,
                            enModemNo,
                            VOICE_MC_FOREGROUND_OPT);

    /* ����Ӧnas��CNF��Ϣ */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_FOREGROUND_CNF,
                           uwSenderPid,
                           VCVOICE_EXECUTE_RSLT_SUCC);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgVcBackGroundReqRP( VOS_VOID *pstOsaMsg  )
{
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enModemNo;
    VOICE_MC_MODEM_STATE_ENUM_UINT16 enModemState;
    VCVOICE_BACKGROUND_REQ_STRU     *pstBackGroundReq = VOS_NULL;


    /* ��ȡsenderPid */
    pstBackGroundReq = (VCVOICE_BACKGROUND_REQ_STRU*)pstOsaMsg;

    /* ����senderPid��ȡͨ���� */
    enModemNo     = VOICE_McGetModemNum(pstBackGroundReq->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ����ͨ����,��ȡ��Ӧmodem���� */
    enModemState  = VOICE_McGetModemState(enModemNo);

    /* �Ѿ���background��,�ظ�nas�ɹ� */
    if( VOICE_MC_MODEM_STATE_BACKGROUND == enModemState )
    {
        VOICE_McSendCnfMsgToVc(ID_VOICE_VC_BACKGROUND_CNF, pstBackGroundReq->uwSenderPid, VCVOICE_EXECUTE_RSLT_SUCC);

        return UCOM_RET_SUCC;
    }

    /* ����Ϊ��̨ģʽ */
    VOICE_McSetBackGround(enModemNo);

    /* ����Ӧnas��CNF��Ϣ */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_BACKGROUND_CNF, pstBackGroundReq->uwSenderPid, VCVOICE_EXECUTE_RSLT_SUCC);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgVcGroundQryReqRP( VOS_VOID *pstOsaMsg )
{
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enModemNo;
    VOICE_MC_MODEM_STATE_ENUM_UINT16 enModemState;
    VCVOICE_GROUND_QRY_STRU         *pstGroundQry = VOS_NULL;

    /* ��ȡsenderPid */
    pstGroundQry     = (VCVOICE_GROUND_QRY_STRU*)pstOsaMsg;

    /* ����senderPid��ȡͨ���� */
    enModemNo        = VOICE_McGetModemNum(pstGroundQry->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* ����ͨ����,��ȡ��Ӧmodem���� */
    enModemState     = VOICE_McGetModemState(enModemNo);

    /* ͨ·״̬֪ͨNAS */
    VOICE_McSendQryCnfMsgToVc(ID_VOICE_VC_GROUND_RSP, pstGroundQry->uwSenderPid, enModemState);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgGUPhyBackGroundCnfRP( VOS_VOID *pstOsaMsg )
{
    DSP_VOICE_RESULT_ENUM_UINT16     enResult;
    GUPHY_VOICE_BACKGROUND_CNF_STRU *pstCnf = VOS_NULL;

    pstCnf   = (GUPHY_VOICE_BACKGROUND_CNF_STRU *)pstOsaMsg;

    enResult = pstCnf->enResult;

    if(DSP_VOICE_RESULT_SUCC == enResult)
    {
        OM_LogInfo(VOICE_MsgPhyBackGroundCnf_SUCC);
    }
    else
    {
        OM_LogError1(VOICE_MsgPhyBackGroundCnf_FAIL, pstCnf->uwSenderPid);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgGUPhyForeGroundCnfRP( VOS_VOID *pstOsaMsg )
{
    DSP_VOICE_RESULT_ENUM_UINT16     enResult;
    GUPHY_VOICE_FOREGROUND_CNF_STRU *pstCnf = VOS_NULL;

    pstCnf   = (GUPHY_VOICE_FOREGROUND_CNF_STRU *)pstOsaMsg;

    enResult = pstCnf->enResult;

    if(DSP_VOICE_RESULT_SUCC == enResult)
    {
        OM_LogInfo(VOICE_MsgPhyForeGroundCnf_SUCC);
    }
    else
    {
        OM_LogError1(VOICE_MsgPhyForeGroundCnf_FAIL, pstCnf->uwSenderPid);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgTdPhyBackGroundCnfRP( VOS_VOID *pstOsaMsg )
{
    TDPHY_VOICE_RESULT_ENUM_UINT16   enResult;
    TDPHY_VOICE_BACKGROUND_CNF_STRU *pstCnf = VOS_NULL;

    pstCnf   = (TDPHY_VOICE_BACKGROUND_CNF_STRU *)pstOsaMsg;

    enResult = pstCnf->enResult;

    if(TDPHY_VOICE_RESULT_SUCC == enResult)
    {
        OM_LogInfo(VOICE_MsgPhyBackGroundCnf_SUCC);
    }
    else
    {
        OM_LogError1(VOICE_MsgPhyBackGroundCnf_FAIL, pstCnf->uwSenderPid);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgTdPhyForeGroundCnfRP( VOS_VOID *pstOsaMsg )
{
    TDPHY_VOICE_RESULT_ENUM_UINT16   enResult;
    TDPHY_VOICE_FOREGROUND_CNF_STRU *pstCnf = VOS_NULL;

    pstCnf   = (TDPHY_VOICE_FOREGROUND_CNF_STRU *)pstOsaMsg;

    enResult = pstCnf->enResult;

    if(TDPHY_VOICE_RESULT_SUCC == enResult)
    {
        OM_LogInfo(VOICE_MsgPhyForeGroundCnf_SUCC);
    }
    else
    {
        OM_LogError1(VOICE_MsgPhyForeGroundCnf_FAIL, pstCnf->uwSenderPid);
    }

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 �� �� ��  : VOICE_McSendQryCnfMsgToVc
 ��������  : �ظ�Nasͨ·״̬
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��24��
    ��    ��   : ·���л�
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_McSendQryCnfMsgToVc(VOS_UINT16 uhwMsgId,VOS_UINT32 uwReceiverPid, VOS_UINT32 uwRet)
{
    VCVOICE_GROUND_RSP_STRU    stCnfMsg;

    /* �����Ϣ */
    UCOM_MemSet(&stCnfMsg, 0, sizeof(VCVOICE_GROUND_RSP_STRU));

    /* ������Ϣ�ͷ���ֵ */
    stCnfMsg.usMsgName        = uhwMsgId;
    stCnfMsg.enState          = (VOS_UINT16)uwRet;

    /* ����uwReceiverPid,�ظ���Ϣ����ӦNAS */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stCnfMsg,
                    sizeof(VCVOICE_GROUND_RSP_STRU));
}

/*****************************************************************************
 �� �� ��  : VOICE_McLinkInformToPhy
 ��������  : ֪ͨ�����ͨ·״̬
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_UINT16
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��23��
    ��    ��   : ·���л�
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_McModemInformToPhy(
                CODEC_NET_MODE_ENUM_UINT16 enNetMode,
                VOICE_MC_MODEM_NUM_ENUM_UINT16 enModemNo,
                VOICE_MC_MODEM_OPT_ENUM_UINT16 enModemOpt
                )
{
    VOS_UINT32                      uwReceiverPid;              /* ��Ҫ֪ͨ�������PID */
    VOS_UINT16                      uhwMsgId;

    switch( enNetMode )
    {
        case CODEC_NET_MODE_G:
        {
            if( VOICE_MC_FOREGROUND_OPT == enModemOpt )
            {
                uhwMsgId = ID_VOICE_GPHY_FOREGROUND_REQ;
            }
            else
            {
                uhwMsgId = ID_VOICE_GPHY_BACKGROUND_REQ;
            }
            uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_GPHY_INDEX];
            VOICE_McSendModemMsgToGUPhy(enModemOpt,uwReceiverPid,uhwMsgId);
        }
        break;
        case CODEC_NET_MODE_W:
        {
            if( VOICE_MC_FOREGROUND_OPT == enModemOpt )
            {
                uhwMsgId = ID_VOICE_WPHY_FOREGROUND_REQ;
            }
            else
            {
                uhwMsgId = ID_VOICE_WPHY_BACKGROUND_REQ;
            }
            uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_WPHY_INDEX];
            VOICE_McSendModemMsgToGUPhy(enModemOpt,uwReceiverPid,uhwMsgId);
        }
        break;
        case CODEC_NET_MODE_TD:
        {
            if( VOICE_MC_FOREGROUND_OPT == enModemOpt )
            {
                uhwMsgId = ID_VOICE_TDPHY_FOREGROUND_REQ;
            }
            else
            {
                uhwMsgId = ID_VOICE_TDPHY_BACKGROUND_REQ;
            }
            uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_TDPHY_INDEX];
            VOICE_McSendModemMsgToTDPhy(enModemOpt,uwReceiverPid,uhwMsgId);
        }
        break;
        default:
        {

        }
    }


}
VOS_VOID VOICE_McSendModemMsgToTDPhy(
                VOICE_MC_MODEM_OPT_ENUM_UINT16 enLinkOpt,
                VOS_UINT32                     uwReceiverPid,
                VOS_UINT16                     uhwMsgId
                )
{
    switch(enLinkOpt)
    {
        case VOICE_MC_BACKGROUND_OPT:
        {
            VOICE_TDPHY_BACKGROUND_REQ_STRU stBackGroundReqMsg;
            /* �����Ϣ */
            UCOM_MemSet(&stBackGroundReqMsg, 0, sizeof(VOICE_TDPHY_BACKGROUND_REQ_STRU));

            stBackGroundReqMsg.usMsgId = uhwMsgId;

            /* ����uwReceiverPid,�ظ���Ϣ����ӦTD����� */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                        uwReceiverPid,
                        &stBackGroundReqMsg,
                        sizeof(VOICE_TDPHY_BACKGROUND_REQ_STRU));
        }
        break;
        case VOICE_MC_FOREGROUND_OPT:
        {
            VOICE_TDPHY_FOREGROUND_REQ_STRU stForeGroundReqMsg;
            /* �����Ϣ */
            UCOM_MemSet(&stForeGroundReqMsg, 0, sizeof(VOICE_TDPHY_FOREGROUND_REQ_STRU));

            stForeGroundReqMsg.usMsgId = uhwMsgId;

            /* ����uwReceiverPid,�ظ���Ϣ����ӦTD����� */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                        uwReceiverPid,
                        &stForeGroundReqMsg,
                        sizeof(VOICE_TDPHY_FOREGROUND_REQ_STRU));
        }
        break;
        default:
        {
        }

    }
}


VOS_VOID VOICE_McSendModemMsgToGUPhy(
                VOICE_MC_MODEM_OPT_ENUM_UINT16 enLinkOpt,
                VOS_UINT32 uwReceiverPid,
                VOS_UINT16 uhwMsgId)
{
    switch(enLinkOpt)
    {
        case VOICE_MC_BACKGROUND_OPT:
        {
            VOICE_GUPHY_BACKGROUND_REQ_STRU stBackGroundReqMsg;
             /* �����Ϣ */
            UCOM_MemSet(&stBackGroundReqMsg, 0, sizeof(VOICE_GUPHY_BACKGROUND_REQ_STRU));

            stBackGroundReqMsg.usMsgId = uhwMsgId;

            /* ����uwReceiverPid,�ظ���Ϣ����ӦGU����� */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            uwReceiverPid,
                            &stBackGroundReqMsg,
                            sizeof(VOICE_GUPHY_BACKGROUND_REQ_STRU));
        }
        break;
        case VOICE_MC_FOREGROUND_OPT:
        {
            VOICE_GUPHY_FOREGROUND_REQ_STRU stForeGroundReqMsg;
             /* �����Ϣ */
            UCOM_MemSet(&stForeGroundReqMsg, 0, sizeof(VOICE_GUPHY_FOREGROUND_REQ_STRU));

            stForeGroundReqMsg.usMsgId = uhwMsgId;

            /* ����uwReceiverPid,�ظ���Ϣ����ӦGU����� */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            uwReceiverPid,
                            &stForeGroundReqMsg,
                            sizeof(VOICE_GUPHY_FOREGROUND_REQ_STRU));
        }
        break;
        default:
        {

        }
    }
}


VOS_UINT16 VOICE_McGetModemNum( VOS_UINT32 uwSenderPid )
{
    VOS_UINT16    enRet;

    /* ����PID�ж�ͨ���� */
    if (( (MODEM0_BEGIN <= uwSenderPid) && (MODEM0_END >= uwSenderPid))
        || (DSP_PID_TDPHY == uwSenderPid))
    {
        enRet = VOICE_MC_MODEM0_RANGE;
    }
    else if ((MODEM1_BEGIN <= uwSenderPid) && (MODEM1_END >= uwSenderPid))
    {
        enRet = VOICE_MC_MODEM1_RANGE;
    }
    else if ((CODEC_BEGIN <= uwSenderPid) && (CODEC_END >= uwSenderPid))
    {
        enRet = VOICE_MC_CODEC_RANGE;
    }
    else if ((OM_BEGIN <= uwSenderPid) && (OM_END >= uwSenderPid))
    {
        enRet = VOICE_MC_OM_RANGE;
    }
    else
    {
        enRet = VOICE_MC_PID_RANGE_NUM_BUTT;
    }

    return enRet;
}
VOS_UINT32 VOICE_McHandleFsmErr(VOS_UINT16 uhwSenderPid, VOS_UINT16 uhwMsgId)
{
    VOS_UINT32  uwRet = UCOM_RET_SUCC;

    /* ��ǰ FSM_ACT_TBL_ITEM �е� PID ���嶼Ϊ 0, ��δ����SenderPid */
    switch(uhwMsgId)
    {
        case ID_VC_VOICE_START_REQ:
        {
            VOICE_McSendCnfMsgToVc(ID_VOICE_VC_START_CNF, uhwSenderPid, UCOM_RET_ERR_STATE);
        }
        break;
        case ID_VC_VOICE_STOP_REQ:
        {
            VOICE_McSendCnfMsgToVc(ID_VOICE_VC_STOP_CNF, uhwSenderPid, UCOM_RET_ERR_STATE);
        }
        break;
        case ID_VC_VOICE_SET_CODEC_REQ:
        {
            VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_CODEC_CNF, uhwSenderPid, UCOM_RET_ERR_STATE);
        }
        break;
        default:
        {

            /* ����δ������ */
            uwRet = UCOM_RET_ERR_MSG;
        }
    }

    return uwRet;
}


VOS_UINT32 VOICE_McHandleRtFsmErr(VOS_UINT16 uhwSenderPid, VOS_UINT16 uhwMsgId)
{
    /* �ݲ������κ��쳣 */

    return UCOM_RET_ERR_MSG;
}
VOS_UINT32 VOICE_McSetDev(VOS_UINT16 uhwDevMode, VOS_UINT32 uwUpdate, VOS_UINT16 uhwModemNo)
{
    VOS_UINT32                    uwRet;
    CODEC_NET_MODE_ENUM_UINT16    enNetMode;
    VOICE_MC_OBJS_STRU           *pstObj         = VOICE_McGetMcObjsPtr();
    VOICE_MC_DMA_FLAG_STRU       *pstDmaFlag     = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                    uwUlTimer;
    VOS_UINT32                    uwDlTimer;

    /* ��ȡ������ʽ */
    enNetMode   = VOICE_McGetModemNetMode(uhwModemNo);

    /* У��͸����豸ģʽ */
    if(uhwDevMode >= VCVOICE_DEV_MODE_BUTT )
    {
        OM_LogError(VOICE_MsgSetDevReq_ParamError);

        return UCOM_RET_ERR_PARA;
    }

    /* �����豸ģʽ */
    VOICE_McSetDeviveMode(uhwDevMode);

    /*ͨ������*/
    VOICE_McSetDevChannel(uhwDevMode);

    /* ���´������ */
    uwRet = UCOM_RET_SUCC;
    if(VOS_TRUE == uwUpdate)
    {
        /* ǰ̨״̬����Ҫ���� */
        if (uhwModemNo == VOICE_McGetForeGroundNum())
        {
            uwRet = VOICE_ProcUpdate(uhwDevMode, enNetMode);
        }

        /* ��ȡNV�� */
        UCOM_NV_Read(en_NV_SmartPACfg,
                     &(pstObj->stSmartPaCfg),
                     sizeof(pstObj->stSmartPaCfg));

        /* ���������SMART PA���ã���������SMART PA SIO */
        if (   (VCVOICE_DEV_MODE_HANDFREE == VOICE_McGetDeviceMode())
            && (VOICE_SMART_PA_EN_ENABLE  == VOICE_McGetSmartPaEn()))
        {
            /* �л�������, ������DMA����������MicIn��SpeakOut��EcRef��Ӧ��DMA */
            pstDmaFlag->usMicInRun   = CODEC_SWITCH_OFF;

            pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

            /* VOLTE�£��л�Ϊ����ʱ����������������DMA����������ȡSmartPa������ */
            if(CODEC_NET_MODE_L == enNetMode)
            {
                /* ��������DMA */
                DRV_TIMER_Start(&uwDlTimer,
                                VOICE_IMS_RX_BEGIN_TIMESTAMP,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer6msIsr,
                                0);

                /* ��������DMA */
                DRV_TIMER_Start(&uwUlTimer,
                                VOICE_IMS_TX_BEGIN_TIMESTAMP,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer3msIsr,
                                0);
            }
        }
        else
        {
            /* �رս���Smart PA�ķ����ź� */
            UCOM_PCM_SmartPaStop();
        }
    }

    OM_LogInfo(VOICE_MsgSetDevReq_Ok);

    return uwRet;
}
VOS_UINT32 VOICE_McSetCodec(VCVOICE_SET_CODEC_REQ_STRU *pstMsg, VOS_UINT16 uhwModemNo)
{
    VOS_UINT32                  uwRet;
    VOS_UINT16                  uhwNvUpdateFlag;
    VOS_UINT16                  uhwCodecType;
    VOS_UINT16                  uhwNetMode    = VOICE_McGetModemNetMode(uhwModemNo);
    VOS_UINT16                  uhwModemState = VOICE_McGetModemState(uhwModemNo); /*ͨ·״̬ */
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag    = VOICE_McGetDmaFlagPtr();
    VCVOICE_DEV_MODE_ENUM_UINT16        enDevMode;                /* �����豸���� */
    VOS_UINT32                          swSampleRate;

    /* ��ȡ���������� */
    uhwCodecType = pstMsg->enCodecType;

    /* Ԥ����: �Ϸ��Լ�� + AMR2->AMR */
    uwRet = VOICE_McPreCfgCodecType(pstMsg->enMode, &uhwCodecType);
    if (UCOM_RET_SUCC != uwRet)
    {
        return uwRet;
    }

    /* �����������Լ�������ʽ */
    VOICE_McSetCodecType(pstMsg->enMode, uhwCodecType, uhwModemNo);

    /* �ж��Ƿ�Ϊ��խ���л� */
    uhwNvUpdateFlag = VOICE_ProcCheckUpdate(uhwCodecType);

    /*����ģʽΪLʱ����ʼ��JitterBuffer*/
    if ( CODEC_NET_MODE_L == pstMsg->enMode )
    {
        uwRet = VOICE_JB_Start(pstMsg->enCodecType);

        /* ��ʼ��ʧ�� */
        if (uwRet != UCOM_RET_SUCC)
        {
            OM_LogError(VOICE_McSetCodecRegAjbCodec);
            return uwRet;
        }
    }

    /* ǰ̨��Ҫ�������������� */
    if(VOICE_McGetForeGroundNum() == uhwModemNo)
    {
        /* ֻ�ڲ����ʱ仯ʱ����PPģ����� */
        if(VOICE_PROC_IS_YES == uhwNvUpdateFlag)
        {
            uwRet = VOICE_McUpdate(uhwModemNo);

            if (UCOM_RET_SUCC != uwRet)
            {
                return uwRet;
            }
        }
        else
        {
            swSampleRate = VOICE_ProcGetSampleRate();
            enDevMode    = VOICE_McGetDeviceMode();

            /* ��������IOģ��(����ͨ��8K/16K����) */
            uwRet = VOICE_PcmUpdate(swSampleRate, enDevMode);

            if(uwRet != UCOM_RET_SUCC)
            {
                return uwRet;
            }

            /* ����DMA */
            pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;
            pstDmaFlag->usSpkOutRun = CODEC_SWITCH_OFF;
        }

    }

    /* ����: ֪ͨ������ */
    VOICE_McPostCfgCodecType(pstMsg->enCodecType, uhwModemNo);

    /* RAT�仯֪ͨ�����ǰ��̨״̬ */
    if (uhwNetMode != pstMsg->enMode)
    {
       VOICE_McModemInformToPhy(pstMsg->enMode, uhwModemNo, uhwModemState);

       /* LTE��SRVCC����suspend��Ϣ������ͳ���л�ʱ�� */
       if(uhwNetMode != CODEC_NET_MODE_L)
       {
           /* ��������л�ʱ���Ƿ���� */
           VOICE_DiagSuspend(uhwModemNo, pstMsg->enMode, uhwNetMode);
       }
    }

    return UCOM_RET_SUCC;
}



VOS_UINT32 VOICE_McStart(VCVOICE_START_REQ_STRU *pstMsg)
{
    VOS_UINT32                       uwRet;
    VOS_UINT16                       uhwCodecType;
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enModemNo;                                    /* ͨ����ö�� */
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enActiveModemNo = VOICE_McGetForeGroundNum(); /* ǰ̨ͨ���� */
    VOICE_PROC_OBJS_STRU            *pstProc         = VOICE_ProcGetObjsPtr();

    /* ������ʽ��� */
    if (CODEC_NET_MODE_BUTT <= pstMsg->enMode)
    {
        return UCOM_RET_ERR_STATE;
    }

    /* ��ȡ���������� */
    uhwCodecType = pstMsg->enCodecType;

    /* ��ǰ��ǰ̨,����ʧ�� */
    if (VOICE_MC_MODEM_NUM_BUTT > enActiveModemNo)
    {
        return UCOM_RET_FAIL;

    }

    /* Ԥ����: �Ϸ��Լ�� + AMR2->AMR */
    uwRet = VOICE_McPreCfgCodecType(pstMsg->enMode, &uhwCodecType);
    if (UCOM_RET_SUCC != uwRet)
    {
       return uwRet;
    }

    /* ��ȡͨ���� */
    enModemNo   = VOICE_McGetModemNum(pstMsg->uwSenderPid);

    /* ��enModemNo���б��� */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* PROCȫ�ֱ����е�codecָ����Ϊ��ǰͨ·��codec */
    pstProc->pstCodec = VOICE_McGetModemCodecPtr(enModemNo);

    /* �����������Լ�������ʽ */
    VOICE_McSetCodecType(pstMsg->enMode, uhwCodecType, enModemNo);

    /* ���´������ */
    uwRet = VOICE_McUpdate(enModemNo);

    if (UCOM_RET_SUCC != uwRet)
    {
        pstProc->pstCodec = VOS_NULL;
        return uwRet;
    }

    /* ����: ֪ͨ������ */
    VOICE_McPostCfgCodecType(pstMsg->enCodecType, enModemNo);

    /* �л������������״̬ */
    VOICE_McSetVoiceState(enModemNo, VOICE_MC_STATE_RUNNING);

    /* ����uwSenderPid����ͨ·����״̬Ϊǰ̨ */
    VOICE_McSetModemState(enModemNo, VOICE_MC_MODEM_STATE_FOREGROUND);

    /* ���õ�ǰ����Modem�� */
    VOICE_McSetForeGroundNum(enModemNo);

    /* ����ǰ����Modem�ŵ����������ϱ���־��Ϊ0 */
    UCOM_MemSet(VOICE_DiagGetReportFlag(enModemNo),
                0,
                (VOICE_DIAG_CHECK_POINT_NUM * sizeof(VOS_UINT16)));

    return uwRet;

}


VOICE_MC_STATE_ENUM_UINT16 VOICE_McGetVoiceState(VOS_UINT32 uwSenderPid)
{
    VOICE_MC_STATE_ENUM_UINT16      enMcVoiceState;
    VOICE_MC_PID_RANGE_ENUM_UINT16  enPidRange;

    enPidRange  = VOICE_McGetModemNum(uwSenderPid);

    switch(enPidRange)
    {
        case VOICE_MC_MODEM0_RANGE:
        case VOICE_MC_MODEM1_RANGE:
        {
            enMcVoiceState = VOICE_McGetModemVoiceState(enPidRange);
        }
        break;
        case VOICE_MC_CODEC_RANGE:
        case VOICE_MC_OM_RANGE:
        {
            enMcVoiceState = VOICE_McGetForeGroundVoiceState();
        }
        break;
        default:
        {
            enMcVoiceState = VOICE_MC_STATE_IDLE;
        }
    }

    return enMcVoiceState;
}


VOICE_MC_STATE_ENUM_UINT16 VOICE_McGetForeGroundVoiceState(VOS_VOID)
{
    VOICE_MC_STATE_ENUM_UINT16  enVoiceState = VOICE_MC_STATE_IDLE;
    VOICE_MC_MODEM_STRU        *pstMcModem   = VOS_NULL;

    pstMcModem = VOICE_McGetForeGroundObj();

    if(VOS_NULL != pstMcModem)
    {
        enVoiceState = pstMcModem->enState;
    }

    return enVoiceState;
}

/*****************************************************************************
 �� �� ��  : VOICE_McGetForeGroundNetMode
 ��������  : ��ȡǰ̨����ģʽ
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_UINT16
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��2��19��
    ��    ��   : g002017040
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT16 VOICE_McGetForeGroundNetMode( VOS_UINT16 uhwActiveModemNo )
{
    CODEC_NET_MODE_ENUM_UINT16  enNetMode    = CODEC_NET_MODE_BUTT;

    /* ȡǰ̨������ʽ */
    if (VOICE_MC_MODEM_NUM_BUTT > uhwActiveModemNo)
    {
        enNetMode = VOICE_McGetModemNetMode(uhwActiveModemNo);
    }

    return enNetMode;
}
VOS_UINT32 VOICE_McApiIsIdle(VOS_VOID)
{
    /* ����ͨ·��ΪIDLE,�����µ� */
    if ((VOICE_MC_STATE_IDLE == VOICE_McGetModemVoiceState(VOICE_MC_MODEM0)) &&
         (VOICE_MC_STATE_IDLE == VOICE_McGetModemVoiceState(VOICE_MC_MODEM1)))
    {
        return UCOM_RET_SUCC;
    }

    return UCOM_RET_FAIL;
}
VOICE_MC_MODEM_STRU* VOICE_McGetForeGroundObj(VOS_VOID)
{
    VOICE_MC_MODEM_STRU   *pstModem            = VOS_NULL;
    VOS_UINT16             uhwActiveModemNo    = VOICE_McGetForeGroundNum();

    if (VOICE_MC_MODEM_NUM_BUTT > uhwActiveModemNo)
    {
        pstModem = VOICE_McGetModemObjPtr(uhwActiveModemNo);
    }

    return pstModem;
}


VOS_VOID VOICE_McSetBackGround(VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo)
{
    VOICE_PROC_OBJS_STRU            *pstProc          = VOICE_ProcGetObjsPtr();
    CODEC_NET_MODE_ENUM_UINT16       enNetMode;

    enNetMode     = VOICE_McGetModemNetMode(enModemNo);

    /* ����ͨ·״̬Ϊbackground */
    VOICE_McSetModemState(enModemNo, VOICE_MC_MODEM_STATE_BACKGROUND);

    /* ���õ�ǰ����Modem�� */
    VOICE_McSetForeGroundNum(VOICE_MC_MODEM_NUM_BUTT);

    /* PROCȫ�ֱ����е�codecָ����NULL */
    pstProc->pstCodec = VOS_NULL;

    /* ֹͣdma��sio */
    VOICE_McStop(enModemNo);

    /* ֪ͨ����㾲Ĭ */
    VOICE_McModemInformToPhy(enNetMode,enModemNo,VOICE_MC_BACKGROUND_OPT);
}
CODEC_NET_MODE_ENUM_UINT16 VOICE_McGetNetMode(VOS_UINT32 uwSenderPid)
{
    CODEC_NET_MODE_ENUM_UINT16      enMcNetMode     = CODEC_NET_MODE_BUTT;
    VOICE_MC_PID_RANGE_ENUM_UINT16  enPidRange;
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enActiveModemNo = VOICE_McGetForeGroundNum();
    enPidRange  = VOICE_McGetModemNum(uwSenderPid);

    switch(enPidRange)
    {
        case VOICE_MC_MODEM0_RANGE:
        case VOICE_MC_MODEM1_RANGE:
        {
            enMcNetMode = VOICE_McGetModemNetMode(enPidRange);
        }
        break;
        case VOICE_MC_CODEC_RANGE:
        case VOICE_MC_OM_RANGE:
        {
            if (VOICE_MC_MODEM_NUM_BUTT > enActiveModemNo)
            {
                enMcNetMode = VOICE_McGetForeGroundNetMode(enActiveModemNo);
            }
        }
        break;
        default:
        {
        }
    }

    return enMcNetMode;
}

/*****************************************************************************
 �� �� ��  : VOICE_McResume
 ��������  : ���������ϵͳ�л���ɺ���������ָ�
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��8��11��
    ��    ��   : ��ׯ�� 59026
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_McResume(VCVOICE_TYPE_ENUM_UINT16 enCodecType, VOS_UINT16 uhwModemNo)
{
    VOICE_McSetVoiceState(uhwModemNo, VOICE_MC_STATE_RUNNING);

    OM_LogInfo(VOICE_McDoResume_OK);
}


VOS_VOID VOICE_McStop(VOS_UINT16 uhwModemNo)
{
    VCVOICE_DEV_MODE_ENUM_UINT16        enDevMode;     /* �豸ģʽ */
    CODEC_NET_MODE_ENUM_UINT16          enNetMode;     /* ������ʽ */
    VOICE_PCM_BUFFER_STRU              *pstPcmBuffTmp  = VOS_NULL;
    VOS_INT16                          *pshwLineOut    = VOICE_ProcGetLineOutBufPtr();

    /* ��ȡTimer */
    VOICE_MC_TIMER_STRU *pstTimer   = VOICE_McGetTimerPtr();

    /* ��ս��뻺���е����� */
    VOICE_ProcInitDecSerialWrite();

    /* ��ձ���ǰPCM����buff */
    UCOM_MemSet(pshwLineOut,
                0,
                (CODEC_PCM_MAX_FRAME_LENGTH * sizeof(VOS_UINT16)));

    /* ��ջ����е�PCM���� */
    pstPcmBuffTmp   = (&g_stVoicePcmBuff);
    UCOM_MemSet(pstPcmBuffTmp,  0, sizeof(g_stVoicePcmBuff));

    /* �����֡����ǰ�����DecSerial���� */
    UCOM_MemSet(VOICE_ProcGetPrevDecDataPtr(), 0, GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));

    /*ֹͣMIC��DMA����*/
    DRV_DMA_Stop(VOICE_MC_DMAC_CHN_MIC);

    /*ֹͣSPK��DMA����*/
    DRV_DMA_Stop(VOICE_MC_DMAC_CHN_SPK);

    /*ֹͣSmart PA��DMA*/
    UCOM_PCM_SmartPaStop();

    /*��ʼ���������DMA��ر�־*/
    UCOM_MemSet(VOICE_McGetDmaFlagPtr(), 0, sizeof(VOICE_MC_DMA_FLAG_STRU));

    /* ��ȡ�豸ģʽ */
    enDevMode = VOICE_McGetDeviceMode();

    /* PC VOICEģʽ��ֹͣ20ms��ʱ�� */
    if (VCVOICE_DEV_MODE_PCVOICE == enDevMode)
    {
        DRV_TIMER_StopPrecise(&(pstTimer->uwTimer20ms));
        DRV_TIMER_StopPrecise(&(pstTimer->uwTimer20msRx));

    }
    else
    {
        /*ֹͣSIO�豸*/
        DRV_SIO_Voice_Close();
    }

    /* ��ȡ������ʽ */
    enNetMode = VOICE_McGetModemNetMode(uhwModemNo);

    /* TDȡ��5ms֡ͬ���ж� */
    if((CODEC_NET_MODE_TD == enNetMode) && (VOICE_MC_STATE_RUNNING == VOICE_McGetModemVoiceState(uhwModemNo)))
    {
        /* ����V9R1 <<Hi6620V100 SOC�߼��������_DMA������жϷ���.xlsx>>,����Ϊtds_stu_hifi_int��ID����� */
        VOS_DisableInterrupt(DRV_TDSCDMA_CTRL_SYNC_INTR);

        /* ȥʹ�����μĴ�����ʹ��״̬Ϊ����״̬ */
        UCOM_RegBitWr(DRV_TDSCDMA_CTRL_EN_ADDR, UCOM_BIT0, UCOM_BIT0, 0);

    }

    /* ����������ģ��ͳ�Ƶ�֡��Ϣ */
    VOICE_DiagReset();

    /* ֹͣ�������ģ��30ms���ڶ�ʱ�� */
    DRV_TIMER_StopPrecise(&(pstTimer->uwTimer30msDiag));

}


VOS_VOID VOICE_McModemObjInit( VOICE_MC_MODEM_STRU *pstMcModemObj )
{
    VOICE_PROC_CODEC_STRU  *pstCodec;
    VOICE_DEC_SERIAL_STRU  *pstDecBuf = VOICE_ProcGetDecBufPtr();

    if (VOS_NULL == pstMcModemObj)
    {
        return ;
    }

    UCOM_MemSet(pstMcModemObj,0,sizeof(VOICE_MC_MODEM_STRU));

    /* ͨ·״̬��ΪUNSET */
    pstMcModemObj->enModemState = VOICE_MC_MODEM_STATE_UNSET;

    /* ����״̬��ΪIDLE */
    pstMcModemObj->enState      = VOICE_MC_STATE_IDLE;

    /* ������ʽ��ΪCODEC_NET_MODE_BUTT */
    pstMcModemObj->enNetMode    = CODEC_NET_MODE_BUTT;

    /* ������ʼ�� */
    pstCodec = &pstMcModemObj->stCodec;

    /* ������������������� */
    pstCodec->stEncOutObj.pshwEncSerial = VOICE_ProcGetEncBufPtr();
    pstCodec->stDecInObj.pshwDecSerial  = pstDecBuf->asDecSerial;
}
VOS_UINT32 VOICE_McPreTxProc( VOS_VOID )
{
    VOS_UINT32  uwRet            = UCOM_RET_SUCC;
    VOS_UINT16  uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16  uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);

    /* ����UMTSģʽ��,������AMR���ʿ��Ʋ��� */
    if ((CODEC_NET_MODE_W == uhwNetMode)
         ||(CODEC_NET_MODE_TD == uhwNetMode))
    {
        /* ����AMR���ʿ��Ƽ����º��� */
        uwRet = VOICE_ProcCfgUmtsUlCodec();
        if (uwRet == UCOM_RET_FAIL)
        {
            return  UCOM_RET_FAIL;
        }
    }

    return uwRet;
}


VOS_UINT32 VOICE_McTxEnc( VOS_VOID )
{
    VOS_UINT32                          uwRet       = UCOM_RET_SUCC;
    VOS_INT16                          *pshwLineOut = VOICE_ProcGetLineOutBufPtr();
    VOICE_PROC_CODEC_STRU              *pstCodec    = VOICE_ProcGetCodecPtr();
    VOS_INT16                          *pshwEncBuf  = VOICE_ProcGetEncBufPtr();

    /* 1 ������������˵�PCM���ݹ�ȡ�ϱ���OM */
    VOICE_DbgSendHook(VOICE_HOOK_CODECIN_PCM, pshwLineOut, (VOS_UINT16)CODEC_PCM_FRAME_LENGTH);

    /* 2 ��ʼһ֡�������ݵı��� */
    uwRet = VOICE_ProcEncode(pshwLineOut, pshwEncBuf);

    if (UCOM_RET_SUCC != uwRet)
    {
        OM_LogError(VOICE_McDoEnc_ProcEncodeError);
    }

    /* G711ֱ����PCM����������Ҫ�ϱ� */
    if (CODEC_G711 != pstCodec->enCodecType)
    {
        /* 3 ��¼�����ṹ��, ��PC����ʹ�� */
        VOICE_DbgSaveCodecObj(pshwEncBuf, VOICE_DBG_CODEC_MODE_ENCODE, pstCodec);

        /* 4 ���������������ݹ�ȡ�ϱ���OM */
        VOICE_DbgSendHook(VOICE_HOOK_TX_STRM, pshwEncBuf, VOICE_CODED_FRAME_WITH_OBJ_LEN);
    }

    if (UCOM_RET_SUCC == uwRet)
    {
        OM_LogFreqInfo(VOICE_McDoEnc_Done, VOICE_LOG_PERIOD);
    }

    return uwRet;

}
VOS_UINT32 VOICE_McTxProc(VOS_VOID)
{
    VOS_UINT32                          uwRet       = UCOM_RET_SUCC;
    VOS_INT16                          *pshwMicIn   = g_psVoicePcmMicIn;
    VOS_INT16                          *pshwMcIn    = VOICE_PcmGetMcInBufPtr();
    VOS_INT16                          *pshwRcIn    = VOICE_PcmGetRcInBufPtr();
    VOS_INT16                          *pshwLineOut = VOICE_ProcGetLineOutBufPtr();
    VOICE_MC_DMA_FLAG_STRU             *pstDmaFlag  = VOICE_McGetDmaFlagPtr();
    VOICE_HOOK_STRU                    *pstHook     = VOICE_DbgGetHookPtr();
    VOS_UINT16                          uhwActModem = VOICE_McGetForeGroundNum();
    VOS_INT16                          *pshwSpkOutTemp = VOICE_PcmGetSpkOutBufTempPtr();
    VOS_INT16                           shwVolumeTx;
    VOICE_MC_MODEM_STRU                *pstModem;
    VOICE_MC_OBJS_STRU                 *pstVoiceMcObj;
    VOS_UINT32                          uwSmpRate;

    /* ��ȡmc���ƽṹ�� */
    pstVoiceMcObj               = VOICE_McGetMcObjsPtr();

    /* ǰ����ʼ, ������DMA�л� */
    pstDmaFlag->usMicInSwEnable = CODEC_SWITCH_OFF;

    /* ���±���ͨ��֡�� */
    pstHook->ulFrameTick        = pstHook->ulFrameTick + 1;

    /* ����Active Modem Number��ȡ����������Ϣ */
    pstModem    = VOICE_McGetModemObjPtr(uhwActModem);
    shwVolumeTx = pstModem->shwVolumeTx;

    /* 1 ����Mic�����PCM���ݹ�ȡ�ϱ���OM */
    VOICE_DbgSendHook(VOICE_HOOK_MICIN_PCM,
                      pshwMicIn,
                      (VOS_UINT16)((VOS_UINT16)CODEC_PCM_FRAME_LENGTH*DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr())));

    /* 2 ��������ǰ���� */
    uwRet = VOICE_ProcTxPp(pshwMcIn, pshwRcIn, pshwLineOut);
    if (UCOM_RET_SUCC != uwRet)
    {
        OM_LogError(VOICE_McDoPp_ProcPreError);
    }
    else
    {
        OM_LogFreqInfo(VOICE_McDoPp_Done, VOICE_LOG_PERIOD);
    }

    /* ����������������TxPP���֮�� */
    uwRet = VOICE_ProcVolume(shwVolumeTx, pshwLineOut, pshwLineOut);
    if (UCOM_RET_SUCC != uwRet)
    {
        OM_LogError(VOICE_ProcVolume_InvalidVolume);
    }

    /* ���д�����Ϻ����ÿ��л���־ */
    pstDmaFlag->usMicInSwEnable  = CODEC_SWITCH_ON;

    if (VOS_NULL != pstVoiceMcObj->pfunCallBack)
    {
        /* ��ȡ��ǰ������ */
        uwSmpRate = VOICE_ProcGetSampleRate();

        /* �����������ݷ��͸�audio_recorder */
        (VOS_VOID)pstVoiceMcObj->pfunCallBack(pshwLineOut, pshwSpkOutTemp, uwSmpRate);

        OM_LogInfo1(VOICE_ProcVolume_InvalidVolume,uwSmpRate);

    }

    return uwRet;
}


VOS_UINT32 VOICE_McPostTxProc( VOS_VOID )
{
    VOS_UINT16  uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16  uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);

    /* �������б����� */
    switch(uhwNetMode)
    {
        case CODEC_NET_MODE_W:
        {

            /* ��ǰ������ʽΪUMTSʱ,�������б�������WPHY */
            VOICE_McTxDataToWphy();
        }
        break;
        case CODEC_NET_MODE_G:
        {

            /* ��ǰ������ʽΪGSMʱ,�������б�������GPHY */
            VOICE_McTxDataToGphy();

            /* ����ʱ���ͻ��� */
            VOICE_McGsmChkMicConflick();
        }
        break;
        case CODEC_NET_MODE_TD:
        {

            /* ��ǰ������ʽΪTDʱ,�������б�������TDPHY */
            VOICE_McTxDataToTDphy();

            /* ����ʱ���ͻ��� */
            VOICE_McChkMicConflick();
        }
        break;
        case CODEC_NET_MODE_L:
        {
            /* ��ǰ������ʽΪLTEʱ,�������б�������IMSA */
            VOICE_McTxDataToImsa();
        }
        break;
        default:
        {
            OM_LogWarning(VOICE_McPostTxProc_NetModeMisMatch);
        }
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_McRxPreProc(VOS_UINT16 *puhwRxFrmLost, VOS_INT16 *pshwVolumeRx, VOS_INT16 **ppshwDecData)
{
    VOS_UINT32                   uwRet           = UCOM_RET_SUCC;
    VOS_UINT16                   uhwRxFrmLost    = VOICE_PROC_IS_NO;
    VOICE_DEC_SERIAL_STRU       *pstDecBuf       = VOICE_ProcGetDecBufPtr();
    VOICE_PROC_CODEC_STRU       *pstCodec        = VOICE_ProcGetCodecPtr();
    VOS_UINT16                   uhwActModem     = VOICE_McGetForeGroundNum();
    VOS_INT16                    shwVolumeRx;
    VOICE_MC_MODEM_STRU         *pstModem;
    VOICE_DEC_SERIAL_STRU        *pstDecWrite    = VOICE_ProcGetDecSerialWritePtr();
    VOS_UINT16                   uhwNetMode      = VOICE_McGetForeGroundNetMode(uhwActModem);
    VOS_UINT32                   uwTimeStamp     = 0;
    VOS_UINT32                   uwDuration      = 0;
    VOS_INT16                   *pshwDecData     = VOS_NULL;

    /* LTE�Ҳ�ʹ��AJB���߾�̬JB���� */
    if ((0 == VOICE_JB_IsAjbEnable()) && (CODEC_NET_MODE_L == uhwNetMode))
    {
        uwRet = VOICE_JB_GetSjbPkt();
        if(UCOM_RET_SUCC != uwRet)
        {
            return uwRet;
        }
    }

    /* ʹ��AJBʱ,��������Щ���� */
    if ((CODEC_NET_MODE_L != uhwNetMode) || (0 == VOICE_JB_IsAjbEnable()))
    {
        /* ��ȡSOC����������ȡ��ǰʱ�� */
        uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

        /* �����жϽ���ʱ�����ͣ��ʱ�� */
        uwDuration = UCOM_COMM_CycSub(uwTimeStamp, pstDecBuf->uwTimeStamp, 0xffffffff);

        /*�����뻺��BUFF�д�����֡���ݣ��ҵ�ǰ�뻺���ǰһ֡��ʱ����30ms����ȡ��һ֡�������ݽ��н���*/
        if ((((uwDuration * VOICE_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq()) > VOICE_LAST_FRAME_OFFSET)
            && (pstDecWrite == (pstDecBuf + VOICE_CODED_DECODE_BUF_FRAME_NUM)))
        {
            UCOM_MemCpy(pstDecBuf, pstDecBuf + 1, sizeof(VOICE_DEC_SERIAL_STRU));

            VOICE_ProcSubDecSerialWritePtr();
        }

        /* ���ж�֡���,LTE�����²���� */
        uhwRxFrmLost = VOICE_McDetectRxFrmLost();
    }

    /* ��ȡ��ǰ����Modem���������� */
    pstModem    = VOICE_McGetModemObjPtr(uhwActModem);
    shwVolumeRx = pstModem->shwVolumeRx;

    /* �������н������ */
    uwRet = VOICE_ProcCfgDlCodec(pstModem->enNetMode,
                                 pstDecBuf->asDecSerial,
                                 uhwRxFrmLost,
                                 &pshwDecData);

    if (UCOM_RET_SUCC != uwRet)
    {
        /* ������һ�δ洢�������ݵĵ�ַ */
        VOICE_ProcSubDecSerialWrite(uhwRxFrmLost);

        OM_LogError(VOICE_ProcCfgDlCodecError);
        return uwRet;
    }

    /* ErrorLog 20ms ���ڴ��� */
    VOICE_ErrLogEventCycle(VOICE_ERRLOG_CHECK_PERIOD);

    /* 1 ����󻷻�@����֮ǰ: ʹ����һ֡�ı��������Ǳ��εĴ��������� */
    VOICE_DbgCheckAndLoop(CODEC_LOOP_CODEC);

    /* G711ֱ����PCM����������Ҫ�ϱ� */
    if (CODEC_G711 != pstCodec->enCodecType)
    {
    	/* 2.1 ��¼�����ṹ��, ��PC����ʹ�� */
    	VOICE_DbgSaveCodecObj(pshwDecData, VOICE_DBG_CODEC_MODE_DECODE, pstCodec);

    	/* 2.2 �������������ݹ�ȡ�ϱ���OM */
	VOICE_DbgSendHook(VOICE_HOOK_RX_STRM, pshwDecData, VOICE_CODED_FRAME_WITH_OBJ_LEN );
    }

    /* ������״̬, �������쳣������ErrorLog��ӡ */
    VOICE_ErrLogCheckDecStatus(&pstCodec->stDecInObj);

    /* RxProc�л���Ҫ�õ�����������������ǰ����������ٴ���������ֵ */
    *puhwRxFrmLost = uhwRxFrmLost;
    *pshwVolumeRx  = shwVolumeRx;
    *ppshwDecData  = pshwDecData;
    return uwRet;
}


VOS_UINT32 VOICE_McRxProc(VOS_VOID)
{
    VOS_UINT32                   uwRet           = UCOM_RET_SUCC;
    VOS_UINT16                   uhwRxFrmLost    = VOICE_PROC_IS_NO;
    VOS_INT16                   *pshwLineIn      = VOICE_ProcGetLineInBufPtr();
    VOS_INT16                   *pshwSpkOut      = VOICE_PcmGetSpkOutBufPtr();
    VOS_INT16                   *pshwSpkOutTemp  = VOICE_PcmGetSpkOutBufTempPtr();
    CODEC_INOUT_STRU            *pstInOut        = VOICE_McGetInoutPtr();
    VOICE_MC_DMA_FLAG_STRU      *pstDmaFlag      = VOICE_McGetDmaFlagPtr();
    VOS_UINT16                   uhwActModem     = VOICE_McGetForeGroundNum();
    VOS_INT16                    shwVolumeRx;
    VOS_INT16                   *pshwDecData     = VOS_NULL;
    VOS_UINT16                   uhwNetMode      = VOICE_McGetForeGroundNetMode(uhwActModem);
    VOS_UINT32                   uwAjbOutDataLen;

    /* ����G�����Ͳ�ƥ���򲻽��н������ */
    if (   (CODEC_NET_MODE_G  == uhwNetMode)
           && (VOICE_PROC_IS_YES == VOICE_McGetGsmCodecTypeMismatch(uhwActModem)))
    {
        /* �������SpeakOut����buff */
        UCOM_MemSet(pshwSpkOut, 0, CODEC_PCM_MAX_FRAME_LENGTH * 2 * sizeof(VOS_INT16));
        UCOM_MemSet(pshwSpkOutTemp, 0, CODEC_PCM_MAX_FRAME_LENGTH * sizeof(VOS_INT16));

        return uwRet;
    }

    /* ����ǰ���� */
    uwRet = VOICE_McRxPreProc(&uhwRxFrmLost, &shwVolumeRx, &pshwDecData);
    if(UCOM_RET_SUCC != uwRet)
    {
        return uwRet;
    }

    if ((CODEC_INOUT_DEFAULT == pstInOut->usRxIn)
    || (CODEC_INOUT_DATA == pstInOut->usRxIn))
    {
        /* ������δ���,������DMA���� */
        pstDmaFlag->usSpkOutSwEnable = CODEC_SWITCH_OFF;

        /* 3 ��ʼһ֡�������ݵĽ��룬�����LTE����Ӧ��AJB�����AJB��ȡ */
        if ((VOICE_JB_IsAjbEnable()) && (CODEC_NET_MODE_L == uhwNetMode))
        {
            uwRet = (VOS_UINT32)VOICE_JB_GetAjbPcmData(VOICE_JB_PKT_TIME_20MS, pshwLineIn, &uwAjbOutDataLen);
        }
        else
        {
            uwRet = VOICE_ProcDecode(pshwDecData, pshwLineIn);

            /* ������һ�δ洢�������ݵĵ�ַ */
            VOICE_ProcSubDecSerialWrite(uhwRxFrmLost);

            /* 4 �������������������PCM���ݹ�ȡ�ϱ���OM,���ʹ��AJB������AJB���� */
            VOICE_DbgSendHook(VOICE_HOOK_CODECOUT_PCM, pshwLineIn, (VOS_UINT16)CODEC_PCM_FRAME_LENGTH);
        }

        if (UCOM_RET_SUCC != uwRet)
        {
            OM_LogError(VOICE_MsgDoDec_ProcDecodeError);
        }

        /* 5 ����������ǰ��������һ�ε����б���ǰ���ݻ��������н��봦 */
        VOICE_DbgCheckAndLoop(CODEC_LOOP_BEFORE_CODEC);

        /* 6 ����������������RxPP֮ǰ */
        uwRet = VOICE_ProcVolume(shwVolumeRx, pshwLineIn, pshwLineIn);
        if (UCOM_RET_SUCC != uwRet)
        {
            OM_LogError(VOICE_ProcVolume_InvalidVolume);
        }

        /* 7 ��ʼ��������Ԥ����,����������ӡ��˲����������ӵ�*/
        uwRet = VOICE_ProcRxPp(pshwLineIn, pshwSpkOutTemp);

        /* ͳһ������� */
        if (UCOM_RET_SUCC != uwRet)
        {
            OM_LogError(VOICE_McDoPp_ProcPostError);
        }
        else
        {
            OM_LogFreqInfo(VOICE_McDoPp_Done, VOICE_LOG_PERIOD);
        }

        /* ��Ԥ�����Ժ���������ΪI2S˫�������� */
        VOICE_PcmStereo2Hybrid(pshwSpkOutTemp,pshwSpkOut, CODEC_PCM_FRAME_LENGTH);

        /* 8 ������DMA��������һ�ε�����Mic�ɼ������ݻ��������� */
        VOICE_DbgCheckAndLoop(CODEC_LOOP_DMA);

        pstDmaFlag->usSpkOutSwEnable = CODEC_SWITCH_ON;

    }

    /* 10 ������Spk��PCM���ݹ�ȡ�ϱ���OM */
    VOICE_DbgSendHook(VOICE_HOOK_SPKOUT_PCM,
                      pshwSpkOut,
                      (VOS_UINT16)((VOS_UINT16)CODEC_PCM_FRAME_LENGTH*DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr())));

    OM_LogFreqInfo(VOICE_McDoDec_Done, VOICE_LOG_PERIOD);

    return uwRet;

}


VOS_UINT32 VOICE_McPlay(VOS_VOID)
{
    VOS_UINT32              uwRet;
    VOS_UINT32              uwSampleRate;
    CODEC_INOUT_STRU       *pstInout    = VOICE_McGetInoutPtr();
    VOICE_MC_DMA_FLAG_STRU *pstDmaFlag  = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU    *pstTimer    = VOICE_McGetTimerPtr();
    VOICE_MC_OBJS_STRU     *pstObj      = VOICE_McGetMcObjsPtr();

    /* ������ֻ�ģʽ,�������ʽDMA����,���򲻽��ж��� */
    if (CODEC_INOUT_DEFAULT == pstInout->usRxOut)
    {
        pstDmaFlag->usSpkOutSwEnable = CODEC_SWITCH_ON;

        /* ����SPK�����в�����ʽDMA���� */
        uwRet = VOICE_PcmSpkOutStartLoopDMA(VOICE_MC_DMAC_CHN_SPK, VOICE_SpkDmaIsr);

        if (   (VCVOICE_DEV_MODE_HANDFREE == VOICE_McGetDeviceMode())
            && (VOICE_SMART_PA_EN_ENABLE  == VOICE_McGetSmartPaEn()))
        {
            uwSampleRate = VOICE_ProcGetSampleRate();

            /* ����ģʽ�£�����SmartPA�����в�����ʽDMA���� */
            UCOM_PCM_SmartPaStartLoopDma((VOS_UINT16)(CODEC_PCM_FRAME_LENGTH_BYTES * UCOM_PCM_I2S_CHANNEL_NUM),
                                         uwSampleRate,
                                         pstObj->stSmartPaCfg.uhwIsMaster,
                                         UCOM_PCM_GetSmartPaDmacChn(),
                                         VOICE_EcRefDmaIsr);
        }

        /* ��־��ǰSPK��DMA״̬Ϊ������ */
        pstDmaFlag->usSpkOutRun      = CODEC_SWITCH_ON;

        /* �����֡����ǰ�����DecSerial���� */
        UCOM_MemSet(VOICE_ProcGetPrevDecDataPtr(), 0, GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));

        /* ��¼��־ */
        OM_LogInfo(VOICE_McStartPcmTrans_SpkOut);
    }
    else if (CODEC_INOUT_DATA == pstInout->usRxOut)
    {
        /* ��ֹͣ20ms��ʱ�� */
        DRV_TIMER_StopPrecise(&(pstTimer->uwTimer20msRx));

        /* ���Լ���������Ϣ */
        uwRet = DRV_TIMER_Start(&(pstTimer->uwTimer01msRx),
                                1,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer20msRxIsr,
                                0);

        /* ��������20ms��ʱ��,��λ0.1ms */
        uwRet = DRV_TIMER_StartPrecise(&(pstTimer->uwTimer20msRx),
                                200,
                                DRV_TIMER_MODE_PERIODIC,
                                VOICE_Timer20msRxIsr,
                                0);

        /* PC Voiceģʽ����DMA�����ڽ����ֱ�ӽ��� */
        uwRet = UCOM_RET_SUCC;
    }
    else
    {
        uwRet = UCOM_RET_ERR_PARA;
    }

    /* ����30ms�����Զ�ʱ�������ڼ��������DMA�ж��Ƿ�ʱ�յ� */
    DRV_TIMER_StartPrecise(&(pstTimer->uwTimer30msDiag),
                        300,
                        DRV_TIMER_MODE_PERIODIC,
                        VOICE_Timer30msDiagIsr,
                        0);

    return uwRet;
}
VOS_UINT32 VOICE_McRecord(VOS_VOID)
{
    VOS_UINT32              uwRet;
    CODEC_INOUT_STRU       *pstInout    = VOICE_McGetInoutPtr();
    VOICE_MC_DMA_FLAG_STRU *pstDmaFlag  = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU    *pstTimer    = VOICE_McGetTimerPtr();

    /* ��Ϊ�ֻ�ģʽ,������������ʽDMA���� */
    if (CODEC_INOUT_DEFAULT == pstInout->usTxIn)
    {
        /* ����MIC�����вɼ���ʽDMA���� */
        uwRet = VOICE_PcmMicInStartLoopDMA(VOICE_MC_DMAC_CHN_MIC, VOICE_MicDmaIsr);

        /* ��־��ǰMIC��DMA״̬Ϊ������ */
        pstDmaFlag->usMicInRun = CODEC_SWITCH_ON;

        /* ��¼��־ */
        OM_LogInfo(VOICE_McStartPcmTrans_MicIn);
    }
    /* ��ΪPC VOICEģʽ��������20ms�����Զ�ʱ�����ڶ�ʱ�����ٽ�������DMA��������*/
    else if (CODEC_INOUT_DATA == pstInout->usTxIn)
    {
        /* ��ֹͣ20ms��ʱ�� */
        DRV_TIMER_StopPrecise(&(pstTimer->uwTimer20ms));

        /* ���Լ���������Ϣ */
        uwRet = DRV_TIMER_Start(&(pstTimer->uwTimer01msTx),
                                1,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer20msIsr,
                                0);

        /* ��������20ms��ʱ�� */
        uwRet = DRV_TIMER_StartPrecise(&(pstTimer->uwTimer20ms),
                        200,
                        DRV_TIMER_MODE_PERIODIC,
                        VOICE_Timer20msIsr,
                        0);
    }
    else
    {
        uwRet = UCOM_RET_ERR_PARA;
    }

    return uwRet;
}


VOS_UINT32 VOICE_McUpdate(VOS_UINT16 uhwModemNo)
{
    VOS_UINT32                          uwRet;
    VCVOICE_DEV_MODE_ENUM_UINT16        enDevMode;                /* �����豸���� */
    CODEC_NET_MODE_ENUM_UINT16          enNetMode;                /* ����ģʽ */
    VOICE_MC_DMA_FLAG_STRU             *pstDmaFlag     = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                          swSampleRate   = VOICE_ProcGetSampleRate();

    /* ע��: ����Proc�е�PPģ���ʼ��ʱ������Pcmģ���е�Frame Length����,
       ���PcmUpdate����ProcUpdate */
    /* ��ȡ�豸ģʽ */
    enDevMode = VOICE_McGetDeviceMode();

    /* ��ȡ����ģʽ */
    enNetMode = VOICE_McGetModemNetMode(uhwModemNo);

    /* ��������IOģ��(����ͨ��8K/16K����) */
    uwRet = VOICE_PcmUpdate(swSampleRate, enDevMode);

    if(uwRet != UCOM_RET_SUCC)
    {
        return uwRet;
    }

    /* ����DMA */
    pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;
    pstDmaFlag->usSpkOutRun = CODEC_SWITCH_OFF;

    /* �����źŴ���ģ��(PP��������) */
    uwRet = VOICE_ProcUpdate(enDevMode,enNetMode);

    if(uwRet != UCOM_RET_SUCC)
    {
        return uwRet;
    }

    return uwRet;
}
VOS_UINT32 VOICE_McPreCfgCodecType(VOS_UINT16 uhwNetMode, VOS_UINT16 *puhwCodecType)
{
    VOS_UINT16 uhwCodecType = *puhwCodecType;

    /* ������Ч���ж� */
    if (uhwNetMode >= CODEC_NET_MODE_BUTT)
    {
        OM_LogError(VOICE_McCheckStartMsg_InvalidNetMode);
        return UCOM_RET_ERR_PARA;
    }

    if (uhwCodecType >= CODEC_BUTT)
    {
        OM_LogError(VOICE_McCheckStartMsg_InvalidCodecType);
        return UCOM_RET_ERR_PARA;
    }

    if (CODEC_NET_MODE_W == uhwNetMode)
    {
        /* WCDMA ������ */
        if (   (uhwCodecType != CODEC_AMR)
            && (uhwCodecType != CODEC_AMR2)
            && (uhwCodecType != CODEC_AMRWB) )
        {
            OM_LogError(VOICE_McCheckStartMsg_UnmatchableWcdmaCodec);
            return UCOM_RET_ERR_PARA;
        }

        /* �ڲ�AMRͳһ���� */
        if (CODEC_AMR2 == uhwCodecType)
        {
            *puhwCodecType = CODEC_AMR;
        }
    }
    else if (CODEC_NET_MODE_TD == uhwNetMode)
    {
        /* TDS-CDMA ֻ֧��AMR������ */
        if ( (uhwCodecType != CODEC_AMR)
            && (uhwCodecType != CODEC_AMR2))
        {
            OM_LogError(VOICE_McCheckStartMsg_UnmatchableTDCodec);
            return UCOM_RET_ERR_PARA;
        }

        /* ����AMR���� */
        VOICE_TDAmrModeCtrlSetType(uhwCodecType);

        /* �ڲ�AMRͳһ���� */
        if (CODEC_AMR2 == uhwCodecType)
        {
            *puhwCodecType = CODEC_AMR;
        }
    }
    else if(CODEC_NET_MODE_G == uhwNetMode)
    {
        /* GSM ������ */
        if (   (CODEC_AMR2 == uhwCodecType)
            || (CODEC_G711 == uhwCodecType))
        {
            OM_LogError(VOICE_McCheckStartMsg_UnmatchableGsmCodec);
            return UCOM_RET_ERR_PARA;
        }
    }
    else
    {
        /* IMS ������ */
        if (   (uhwCodecType != CODEC_AMR)
            && (uhwCodecType != CODEC_AMR2)
            && (uhwCodecType != CODEC_AMRWB)
            && (uhwCodecType != CODEC_G711))
        {
            OM_LogError(VOICE_McCheckStartMsg_UnmatchableImsCodec);
            return UCOM_RET_ERR_PARA;
        }

        /* �ڲ�AMRͳһ���� */
        if (CODEC_AMR2 == uhwCodecType)
        {
            *puhwCodecType = CODEC_AMR;
        }
    }

    return UCOM_RET_SUCC;
}
VOS_VOID VOICE_McSetCodecType(VOS_UINT16 uhwNetMode, VOS_UINT16 uhwCodecType, VOS_UINT16 uhwModemNo)
{
    VOICE_PROC_CODEC_STRU          *pstCodec = VOS_NULL;

    /* ����ģʽ���� */
    VOICE_McSetModemNetMode(uhwModemNo, uhwNetMode);

    /* ���������͸��� */
    pstCodec = VOICE_McGetModemCodecPtr(uhwModemNo);
    pstCodec->enCodecType = uhwCodecType;
    pstCodec->usIsEncInited  = VOICE_PROC_IS_NO;
    pstCodec->usIsDecInited  = VOICE_PROC_IS_NO;
}


VOS_VOID VOICE_McPostCfgCodecType(VOS_UINT16 uhwCodecType,VOS_UINT16 uhwModemNo)
{
    CODEC_NET_MODE_ENUM_UINT16      enNetMode;              /* ������ʽ */
    VOS_UINT32                      uwReceiverPid;
    VOICE_MC_MODEM_STRU            *pstModem   = VOS_NULL;
    VOICE_MC_DMA_FLAG_STRU         *pstDmaFlag = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                      uwUlTimer;
    VOS_UINT32                      uwDlTimer;

    /* ��ȡͨ·���� */
    pstModem  = VOICE_McGetModemObjPtr(uhwModemNo);

    /* ��ȡ������ʽ */
    enNetMode = pstModem->enNetMode;

    /* ����������ʽ���д��� */
    switch(enNetMode)
    {
       case CODEC_NET_MODE_G:
       {
            /* ֪ͨGPHY�����������¼� */
            uwReceiverPid = g_auwModemMcPidTbl[uhwModemNo][VOICE_MC_GPHY_INDEX];
            VOICE_McInformGphySetCodec(uwReceiverPid);
       }
       break;
       case CODEC_NET_MODE_W:
       {
            /* ����AMR����(AMR / AMR2 / AMRWB) */
            uwReceiverPid = g_auwModemMcPidTbl[uhwModemNo][VOICE_MC_WTTF_INDEX];
            VOICE_UmtsAmrModeCtrlSetType(uhwCodecType,uwReceiverPid);
       }
       break;
       case CODEC_NET_MODE_TD:
       {
            /* ����V9R1 <<Hi6620V100 SOC�߼��������_DMA������жϷ���.xlsx>>,����Ϊtds_stu_hifi_int��ID����� */
            /* TDʹ��5ms֡ͬ���ж� */
            VOS_EnableInterrupt(DRV_TDSCDMA_CTRL_SYNC_INTR);

            /* ʹ�����μĴ�����ʹ��״̬Ϊ������״̬ */
            UCOM_RegBitWr(DRV_TDSCDMA_CTRL_EN_ADDR, UCOM_BIT0, UCOM_BIT0, 1);
       }
       break;
       case CODEC_NET_MODE_L:
       {
            /* ��������DMA */
            /*������DMA��־Ϊδ������������5ms��ʱ��������IMS����DMA����ʱ��*/
            if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
            {
                /* ��������DMA */
                DRV_TIMER_Start(&uwDlTimer,
                                VOICE_IMS_RX_BEGIN_TIMESTAMP,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer6msIsr,
                                0);
            }

            /*������DMA��־Ϊδ������������4ms��ʱ��������IMS����DMA����ʱ��*/
            if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
            {
                /* ��������DMA */
                DRV_TIMER_Start(&uwUlTimer,
                                VOICE_IMS_TX_BEGIN_TIMESTAMP,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer3msIsr,
                                0);
            }

            /* IMS�����������������,Ĭ������wbΪ23.05��nbΪ12.2 */
            VOICE_ProcCfgImsCodec(uhwCodecType,CODEC_DTX_DISABLE,CODEC_AMR_RATE_MODE_122K);
       }
       break;
       default:
       {
       }
    }
}
VOS_UINT16 VOICE_McDetectRxFrmLost(VOS_VOID)
{
    VOS_UINT16              uhwRxFrmLost     = VOICE_PROC_IS_NO;
    VOICE_DEC_SERIAL_STRU  *pshwDecBuf       = VOICE_ProcGetDecBufPtr();
    VOICE_DEC_SERIAL_STRU  *pDecSerialWrite  = VOICE_ProcGetDecSerialWritePtr();

    /* ��������ж�֡ */
    if(pDecSerialWrite <= pshwDecBuf)
    {
        /* ���ж�֡ */
        uhwRxFrmLost = VOICE_PROC_IS_YES;

        /* ��¼�쳣 */
        OM_LogWarning(VOICE_McRxFrmLost);
    }

    return(uhwRxFrmLost);
}
VOS_VOID VOICE_McTxDataToWphy(VOS_VOID)
{
    VOICE_WPHY_TX_DATA_IND_STRU     stEncRslt;
    VOS_UINT32                      uwReceiverPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo  = VOICE_MC_MODEM_NUM_BUTT;

    /* ��ȡǰ̨modem��ͨ���� */
    enModemNo = VOICE_McGetForeGroundNum();

    if(VOICE_MC_MODEM_NUM_BUTT == enModemNo)
    {
        return ;
    }

    UCOM_MemSet(&stEncRslt, 0, sizeof(stEncRslt));

    stEncRslt.uhwMsgId   = (VOS_UINT16)ID_VOICE_WPHY_TX_DATA_IND;

    /* ���ABC�������ȼ�AMR�������� */
    UCOM_MemCpy((VOICE_WPHY_AMR_SERIAL_STRU*)&stEncRslt.enQualityIdx,
                VOICE_ProcGetEncBufPtr(),
                sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /* ��ȡPID */
    uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_WPHY_INDEX];

    /* ID_VOICE_WPHY_TX_DATA_IND��Ϣ���͸�WPHY */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stEncRslt,
                    sizeof(VOICE_WPHY_TX_DATA_IND_STRU));

    return;
}


VOS_VOID VOICE_McTxDataToGphy(VOS_VOID)
{
    VOICE_GPHY_TX_DATA_IND_STRU     stEncRslt;
    VOS_UINT32                      uwReceiverPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo   = VOICE_MC_MODEM_NUM_BUTT;
    VOICE_PROC_CODEC_STRU          *pstCodec    = VOICE_ProcGetCodecPtr();

    /* ��ȡǰ̨modem��ͨ���� */
    enModemNo = VOICE_McGetForeGroundNum();

    if(VOICE_MC_MODEM_NUM_BUTT == enModemNo)
    {
        return ;
    }

    if (VOICE_PROC_IS_YES == VOICE_McGetGsmCodecTypeMismatch(enModemNo))
    {
        return ;
    }

    UCOM_MemSet(&stEncRslt, 0, sizeof(stEncRslt));

    /* ���ID_VOICE_GPHY_TX_DATA_IND��Ϣ�������� */
    stEncRslt.uhwMsgId        = (VOS_UINT16)ID_VOICE_GPHY_TX_DATA_IND;
    stEncRslt.enSpFlag        = pstCodec->stEncOutObj.enSpFlag;
    stEncRslt.enVadFlag       = pstCodec->stEncOutObj.enVadFlag;
    stEncRslt.enAmrFrameType  = pstCodec->stEncOutObj.enAmrFrameType;
    stEncRslt.enAmrMode       = pstCodec->stEncOutObj.enAmrMode;
    stEncRslt.enCodecType     = pstCodec->enCodecType;

    /* ��������������� */
    UCOM_MemCpy(stEncRslt.auhwData,
                VOICE_ProcGetEncBufPtr(),
                (GPHY_VOICE_DATA_LEN * sizeof(VOS_UINT16)));

    /* ��ȡPID */
    uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_GPHY_INDEX];

    /* ��GPHY����ID_VOICE_GPHY_TX_DATA_IND��Ϣ */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stEncRslt,
                    sizeof(VOICE_GPHY_TX_DATA_IND_STRU));

    return;
}
VOS_VOID VOICE_McTxDataToTDphy(VOS_VOID)
{
    VOICE_TDPHY_TX_DATA_IND_STRU      stEncRslt;
    VOS_UINT32                        uwReceiverPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16    enModemNo  = VOICE_MC_MODEM_NUM_BUTT;
    VOICE_PROC_CODEC_STRU            *pstCodec   = VOICE_ProcGetCodecPtr();

    /* ��ȡǰ̨modem��ͨ���� */
    enModemNo = VOICE_McGetForeGroundNum();

    if(VOICE_MC_MODEM_NUM_BUTT == enModemNo)
    {
        return ;
    }

    UCOM_MemSet(&stEncRslt, 0, sizeof(stEncRslt));

    stEncRslt.uhwMsgId                 = (VOS_UINT16)ID_VOICE_TDPHY_TX_DATA_IND;
    stEncRslt.uhwFrameType             = (VOS_UINT16)((pstCodec->stEncOutObj).enAmrFrameType);

    /* ���ABC�������ȼ�AMR�������� */
    UCOM_MemCpy((VOICE_WPHY_AMR_SERIAL_STRU*)&stEncRslt.enQualityIdx,
                VOICE_ProcGetEncBufPtr(),
                sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /* ��ȡPID */
    uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_TDPHY_INDEX];

    /* ID_MED_TDPHY_TX_DATA_IND��Ϣ���͸�TDPHY */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stEncRslt,
                    sizeof(VOICE_TDPHY_TX_DATA_IND_STRU));

    return;
}


VOS_VOID VOICE_McTxDataToImsa(VOS_VOID)
{
    VOICE_IMSA_TX_DATA_IND_STRU     stEncRslt;
    VOS_UINT32                      uwReceiverPid;
    VOS_INT16                      *pEncBuf         = VOICE_ProcGetEncBufPtr();
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo       = VOICE_MC_MODEM_NUM_BUTT;

    /* ��ȡǰ̨modem��ͨ���� */
    enModemNo = VOICE_McGetForeGroundNum();

    /* modem�źϷ��Լ�� */
    if(VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        return ;
    }

    /* ��ȡPID */
    uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_IMSA_INDEX];

    /* PID�Ϸ��Լ�� */
    if (PID_UNSET == uwReceiverPid)
    {
        OM_LogWarning1(VOICE_McTxReceivePid_UNSET,enModemNo);
        return ;
    }

    UCOM_MemSet(&stEncRslt, 0, sizeof(VOICE_IMSA_TX_DATA_IND_STRU));

    /* �������б������� */
    UCOM_MemCpy(&(stEncRslt.usStatus),
                &pEncBuf[0],
                sizeof(IMS_UL_DATA_OBJ_STRU));

    stEncRslt.usMsgId = ID_VOICE_IMSA_TX_DATA_IND;

    VOICE_JB_SendImsaPkt(&stEncRslt);

    OM_LogFreqInfo(VOICE_McTxDataToImsa_Done, VOICE_LOG_PERIOD);

    return;
}
VOS_VOID VOICE_McChkMicConflick(VOS_VOID)
{
    VOS_UINT32                  uwTimeStamp      = 0;
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag       = VOICE_McGetDmaFlagPtr();
    VOS_UINT16                  uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                  uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    VOS_UINT32                  uwMaxEncOffset;

    /* �����ǰ����ģʽUMTSģ��������г�ͻ��� */
    if (CODEC_NET_MODE_W == uhwNetMode)
    {
        uwMaxEncOffset = VOICE_UMTS_MAX_ENC_OFFSET;
    }

    /* TD����ģʽ�½�������ʱ���ͻ��� */
    else if(CODEC_NET_MODE_TD== uhwNetMode)
    {
        uwMaxEncOffset = VOICE_TDS_MAX_ENC_OFFSET;
    }
    else
    {
        return;
    }

    /* ��ȡSOC����������ȡ��ǰʱ�� */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* ����ʱ�̾��뱾֡ͷλ���Ƿ񳬹�9.5ms */
    if (((UCOM_COMM_CycSub(uwTimeStamp, pstDmaFlag->uwSyncTimestamp, 0xffffffff)
        * VOICE_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq())
        >= (uwMaxEncOffset - pstDmaFlag->uwSyncOffset))
    {
        /* DMA�л���ͻ, ������DMA */
        pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;

        pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

        /* ��¼�쳣 */
        OM_LogWarning(VOICE_McChkMicConflick_MicInDmaRest);

        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_MIC_RESET);
    }
}


VOS_VOID VOICE_McGsmChkMicConflick(VOS_VOID)
{
    VOS_UINT32                  uwTimeStamp      = 0;
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag       = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                  uwTimeDiff;
    VOS_UINT32                  uwTimeOffset;


    /* ��ȡSOC����������ȡ��ǰʱ�� */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* ���㵱ǰʱ�̾���ͬ��ʱ�̵�ʱ���,ģ20ms����λ0.1ms */
    uwTimeDiff = ((UCOM_COMM_CycSub(uwTimeStamp, pstDmaFlag->uwSyncTimestamp, 0xffffffff)
                 * VOICE_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq()) + pstDmaFlag->uwSyncOffset;

    /* ���㵱ǰʱ����������Ҫ���ϱ�TX_DATA��ʱ��ƫ�ƣ���λ0.1ms */
    uwTimeOffset = ((uwTimeDiff + VOICE_GSM_ONE_FRAME_OFFSET) - pstDmaFlag->uwChanCodeTime) % VOICE_GSM_ONE_FRAME_OFFSET;

    /* ����ʱ���Ƿ񳬹�GPhyҪ���ϱ�TX_DATA������ʱ�� */
    if (uwTimeOffset < VOICE_GSM_MIN_OFFSET)
    {
        /* DMA�л���ͻ, ������DMA */
        pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;

        pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

        /* ��¼�쳣 */
        OM_LogWarning(VOICE_McChkMicRightConflick_MicInDmaRest);

        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_MIC_RESET);
    }

    /* ����ʱ���Ƿ�����GPhyҪ���ϱ�TX_DATA������ʱ�� */
    if (uwTimeOffset > VOICE_GSM_MAX_OFFSET)
    {
        /* DMA�л���ͻ, ������DMA */
        pstDmaFlag->usMicInRun   = CODEC_SWITCH_OFF;

        pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

        /* ��¼�쳣 */
        OM_LogWarning(VOICE_McChkMicLeftConflick_MicInDmaRest);

        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_MIC_RESET);
    }
}
VOS_VOID VOICE_McChkSpkConflick(VOS_VOID)
{
    VOICE_MC_DMA_FLAG_STRU          *pstDmaFlag       = VOICE_McGetDmaFlagPtr();
    CODEC_INOUT_STRU                *pstInout         = VOICE_McGetInoutPtr();
    VOS_UINT16                       uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOICE_GPHY_SYNC_REQ_STRU         stSyncReqMsg;
    UCOM_MemSet(&stSyncReqMsg, 0, sizeof(stSyncReqMsg));

    /* ����ǰģʽΪ��PC VOICEģʽ(���ֻ�ģʽ)������ܴ�������ʱ���ͻ */
    /* PC VOICE�½������DMA�Ļ��Ʊ�֤�˲���������в���ʱ���ͻ */
    if (CODEC_INOUT_DATA != pstInout->usRxOut)
    {
        /* ��SpkOut��DMA�ж�ʱ�̼�⵽��δ���յ��µ��������ݣ�����Ϊ����ʱ���ͻ */
        if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutSwEnable)
        {
            /* ��������ʱ���ͻ, ������������ʱ��*/
            pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;

            pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

            /* ��¼�쳣 */
            OM_LogWarning(VOICE_McChkSpkConflick_SpkOutDmaRest);

            /* ��ǰ̨���� */
            if(VOICE_MC_MODEM_NUM_BUTT <= uhwActiveModemNo)
            {
                /* ErrorLog ��¼�¼� */
                VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_SPK_RESET);
                return ;
            }

            /* ErrorLog ��¼�¼� */
            VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_SPK_RESET);
        }
        /* ��δ���ֳ�ͻ,���־��֡����������ʹ�� */
        else
        {
            pstDmaFlag->usSpkOutSwEnable = CODEC_SWITCH_OFF;
        }
    }
}
VOS_VOID VOICE_McTdChkMicConflick(VOS_VOID)
{
    VOS_UINT32                          uwTimeStamp = 0;
    VOICE_MC_DMA_FLAG_STRU             *pstDmaFlag;

    pstDmaFlag  = VOICE_McGetDmaFlagPtr();

    /* ��ȡSOC����������ȡ��ǰʱ�� */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* ����ʱ�̾��뱾֡ͷλ���Ƿ񳬹�16.7ms */
    if (((UCOM_COMM_CycSub(uwTimeStamp, pstDmaFlag->uwSyncTimestamp, 0xffffffff)
        * VOICE_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq())
        >= (VOICE_TDS_MAX_ENC_OFFSET - pstDmaFlag->uwSyncOffset))
    {
        /* DMA�л���ͻ, ������DMA */
        pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;
        pstDmaFlag->usSpkOutRun = CODEC_SWITCH_OFF;

        /* ��¼�쳣 */
        OM_LogWarning(VOICE_McChkMicConflick_MicInDmaRest);

        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_MIC_RESET);
    }
}


VOS_VOID VOICE_McSetDevChannel(VOS_UINT16 uhwDevMode)
{
    CODEC_INOUT_STRU    *pstInout = VOICE_McGetInoutPtr();

    /* ���ݳ���ģʽ������������ͨ�����������ͨ�� */
    if (VCVOICE_DEV_MODE_PCVOICE != uhwDevMode)
    {
        pstInout->usTxIn        = CODEC_INOUT_DEFAULT;
        pstInout->usRxOut       = CODEC_INOUT_DEFAULT;
    }
    else
    {
        pstInout->usTxIn        = CODEC_INOUT_DATA;
        pstInout->usRxOut       = CODEC_INOUT_DATA;
    }

}

/*****************************************************************************
 �� �� ��  : VOICE_McInformGphySetCodec
 ��������  : ֪ͨGPHY��������ͱ�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2010��4��10��
    ��    ��   : ��ׯ�� 59026
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_McInformGphySetCodec(VOS_UINT32 uwReceiverPid)
{
    VOICE_GPHY_SET_CODEC_IND_STRU   stSetCodecInd;

    UCOM_MemSet(&stSetCodecInd, 0, sizeof(stSetCodecInd));

    /* �����ϢID */
    stSetCodecInd.uhwMsgId    = ID_VOICE_GPHY_SET_CODEC_IND;

    /* ������Ϣ����ϢĿ�ĵ�ַΪPID_GPHY����ϢIDΪID_CODEC_GPHY_SET_CODEC_IND */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,/* UCOM_PID_DSP_GPHY */
                    &stSetCodecInd,
                    sizeof(VOICE_GPHY_SET_CODEC_IND_STRU));

    return;
}
VOS_VOID VOICE_McSendCnfMsgToVc(VOS_UINT16 uhwMsgId,VOS_UINT32 uwReceiverPid, VOS_UINT32 uwRet)
{
    VCVOICE_OP_RSLT_STRU    stCnfMsg;

    /* �����Ϣ */
    UCOM_MemSet(&stCnfMsg, 0, sizeof(VCVOICE_OP_RSLT_STRU));

    /* ������Ϣ�ͷ���ֵ */
    stCnfMsg.usMsgName        = uhwMsgId;
    stCnfMsg.enExeRslt        = (VOS_UINT16)uwRet;

    /* ����uwReceiverPid,�ظ���Ϣ����ӦNAS */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stCnfMsg,
                    sizeof(VCVOICE_OP_RSLT_STRU));
}
VOS_UINT32 VOICE_McRegisterCaptureCallBack(VOICE_MC_CALLBACK pfunCallBack)
{
    if(VOS_NULL == pfunCallBack)
    {
        return UCOM_RET_FAIL;
    }
    else
    {
        VOICE_McSetCaptureCallBack(pfunCallBack);
    }

    return UCOM_RET_SUCC;
}


VOS_VOID VOICE_McUnRegisterCaptureCallBack(VOS_VOID)
{
    VOICE_McSetCaptureCallBack(VOS_NULL);
}

/*****************************************************************************
 �� �� ��  : VOICE_Timer3msIsr
 ��������  : W��3ms��ʱ���жϴ�����,ע�����DRV�㱻����
 �������  : uwTimer   -- ��ʱ�����
             uwPara    -- �жϺ������,ע��ʱ����
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_Timer3msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    GPHY_VOICE_RECORD_IND_STRU  stRecordIndMsg;

    UCOM_MemSet(&stRecordIndMsg, 0, sizeof(stRecordIndMsg));

    /* ����UMTS��������3ms��ʱ,�ֻ�ģʽʱ��������DMA��ʽ���˻�PC VOICE�µ�20ms��ʱ */
    /* �����ϢID */
    stRecordIndMsg.uhwMsgId = ID_VOICE_VOICE_RECORD_IND;

    /* ���������ɼ���ϢID_CODEC_CODEC_RECORD_IND��PID_VOICE_RT,��ʼ���������ɼ� */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    DSP_PID_VOICE_RT,
                    &stRecordIndMsg,
                    sizeof(GPHY_VOICE_RECORD_IND_STRU));

    return;
}

/*****************************************************************************
 �� �� ��  : VOICE_Timer6msIsr
 ��������  : W��6ms��ʱ���жϴ�����,ע�����DRV�㱻����
 �������  : uwTimer   -- ��ʱ�����
             uwPara    -- �жϺ������,ע��ʱ����
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_Timer6msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    GPHY_VOICE_PLAY_IND_STRU    stPlayIndMsg;

    UCOM_MemSet(&stPlayIndMsg, 0, sizeof(stPlayIndMsg));

    /* ����UMTS��������6ms��ʱ,�ֻ�ģʽʱ��������DMA��ʽ����,PC VOICEģʽʱ�������� */
    /* �����ϢID */
    stPlayIndMsg.uhwMsgId = ID_VOICE_VOICE_PLAY_IND;

    /* ��������������ϢID_CODEC_CODEC_PLAY_IND��PID_VOICE_RT,��ʼ������������ */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    DSP_PID_VOICE_RT,
                    &stPlayIndMsg,
                    sizeof(GPHY_VOICE_PLAY_IND_STRU));

    return;
}

/*****************************************************************************
 �� �� ��  : VOICE_Timer20msIsr
 ��������  : 20ms���ڶ�ʱ���жϴ�����,ע�����DRV�㱻����
 �������  : uwTimer   -- ��ʱ�����
             uwPara    -- �жϺ������,ע��ʱ����
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_Timer20msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    /* ��������Mic��DMA���� */
    VOICE_PcmStartDMAFromDataIn(VOICE_MC_DMAC_CHN_MIC);

    return;
}
VOS_VOID VOICE_Timer20msRxIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    /* ������������� */
    VOICE_PcmStartDMAToDataOut(VOICE_MC_DMAC_CHN_SPK);

    return;
}

/*****************************************************************************
 �� �� ��  : VOICE_SyncTdSubFrm5msIsr
 ��������  : TD 5ms ��֡�ж�
 �������  :
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_SyncTdSubFrm5msIsr(VOS_VOID)
{
    VOICE_VOICE_TD_SYNC_IND_STRU    stSyncInd;
    VOS_UINT32                      uwFrmCnt = 0;


    /* ��Ϊ5ms�ж��ǵ�ƽ�жϣ���Ҫ�Ƚ������жϲ��� */
    UCOM_RegBitWr(DRV_TDSCDMA_CTRL_INT_CLEAR_ADDR, UCOM_BIT0, UCOM_BIT0, 1);

    /* ��ȡ֡�жϼĴ������ݣ�����16λ��Ч */
    uwFrmCnt = UCOM_RegRd(DRV_TDSCDMA_FRM_CNT_REG_ADDR);
    uwFrmCnt = uwFrmCnt & 0xffff;

    /* ������ƣ���4i(i������)��5ms�жϲ�����ʱ���ż��֡֡ͷʱ��һ�£���ʱMED��MED_RT����ͬ����Ϣ������ʹ�ú귴������������� */
    if (0 == (uwFrmCnt & 0x3))
    {
        stSyncInd.uhwMsgId = ID_VOICE_VOICE_TD_SYNC_IND;
        /* ���ͱ�����ϢID_MED_MED_TD_SYNC_IND��PID_MED_RT,��ʼ�������� */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        DSP_PID_VOICE_RT,
                        &stSyncInd,
                        sizeof(VOICE_VOICE_TD_SYNC_IND_STRU));
    }
    return;
}


VOS_VOID VOICE_TdsTxTimerIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    GPHY_VOICE_RECORD_IND_STRU            stRecordIndMsg;

    UCOM_MemSet(&stRecordIndMsg, 0, sizeof(stRecordIndMsg));

    if (VOICE_MC_STATE_RUNNING == VOICE_McGetForeGroundVoiceState())
    {
        /* ����TDS��������0.3ms��ʱ */
        /* �����ϢID */
        stRecordIndMsg.uhwMsgId = ID_VOICE_VOICE_RECORD_IND;

        /* ���������ɼ���ϢID_VOICE_VOICE_RECORD_IND��DSP_PID_VOICE_RT,��ʼ���������ɼ� */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        DSP_PID_VOICE_RT,
                        &stRecordIndMsg,
                        sizeof(GPHY_VOICE_RECORD_IND_STRU));
    }
    else
    {
        OM_LogWarning(VOICE_TimerTxIsr_WarningState);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : VOICE_TdsRxTimerIsr
 ��������  : W��6ms��ʱ���жϴ�����,ע�����DRV�㱻����
 �������  : uwTimer   -- ��ʱ�����
             uwPara    -- �жϺ������,ע��ʱ����
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID VOICE_TdsRxTimerIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    GPHY_VOICE_PLAY_IND_STRU              stPlayIndMsg;

    UCOM_MemSet(&stPlayIndMsg, 0, sizeof(stPlayIndMsg));

    if (VOICE_MC_STATE_RUNNING == VOICE_McGetForeGroundVoiceState())
    {
        /* ����TDS-CDMA��������6ms��ʱ */
        /* �����ϢID */
        stPlayIndMsg.uhwMsgId = ID_VOICE_VOICE_PLAY_IND;

        /* ��������������ϢID_VOICE_VOICE_PLAY_IND��PID_VOICE_RT,��ʼ������������ */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        DSP_PID_VOICE_RT,
                        &stPlayIndMsg,
                        sizeof(GPHY_VOICE_PLAY_IND_STRU));
    }
    else
    {
        OM_LogWarning(VOICE_TimerRxIsr_WarningState);
    }

    return;
}
VOS_VOID VOICE_Timer30msDiagIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    HIFI_ERR_EVT_VOICE_CONTROL_FAULT_STRU stVoiceCtrlErr;
    VOS_UINT16                            uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOICE_PROC_CODEC_STRU                *pstCodec         = VOICE_McGetModemCodecPtr(uhwActiveModemNo);

    UCOM_MemSet(&stVoiceCtrlErr, 0, sizeof(stVoiceCtrlErr));

   /* û���յ�����DMA�жϻ�����DMA�ж� */
    if((VOICE_DIAG_FALSE == g_stDiagDmaIsr.uhwTxDmaIsrFlag)
     ||(VOICE_DIAG_FALSE == g_stDiagDmaIsr.uhwRxDmaIsrFlag))
    {
        stVoiceCtrlErr.uhwErrorReason = VOICE_CONTROL_FAULT;
        stVoiceCtrlErr.uhwDevMode     = VOICE_McGetDeviceMode();
        stVoiceCtrlErr.uhwActiveModem = uhwActiveModemNo;
        stVoiceCtrlErr.uhwNetMode     = VOICE_McGetModemNetMode(uhwActiveModemNo);
        stVoiceCtrlErr.uhwCodecType   = pstCodec->enCodecType;
        stVoiceCtrlErr.uhwTxDmaErr    = g_stDiagDmaIsr.uhwTxDmaIsrFlag;
        stVoiceCtrlErr.uhwRxDmaErr    = g_stDiagDmaIsr.uhwRxDmaIsrFlag;

        /* �ϱ��������̴���澯 */
        VOICE_DiagAlarmReport(uhwActiveModemNo,
                              VOICE_CONTROL_FAULT,
                              &stVoiceCtrlErr,
                              sizeof(stVoiceCtrlErr),
                              HIFI_ERR_LOG_VOICE_CONTROL_FAULT);
    }
    else
    {
        /* ��ձ��μ�¼ */
        g_stDiagDmaIsr.uhwTxDmaIsrFlag = VOICE_DIAG_FALSE;
        g_stDiagDmaIsr.uhwRxDmaIsrFlag = VOICE_DIAG_FALSE;
    }

    return;
}
VOS_UINT32 VOICE_MsgApNoteCCPUResetRP(VOS_VOID *pstOsaMsg)
{

    HIFI_AP_CCPU_RESET_CNF_STRU    stCnfMsg;

    /* ���1ͨ·����ͨ��״̬����ֹͣͨ��״̬ */
    if (VOICE_MC_STATE_RUNNING == VOICE_McGetModemVoiceState(VOICE_MC_MODEM0))
    {
        /* ֹͣ����������� */
        VOICE_McStop(VOICE_MC_MODEM0);

        /* ���õ�ǰ����Modem�� */
        VOICE_McSetForeGroundNum(VOICE_MC_MODEM_NUM_BUTT);

        /* ���������ʼ�� */
        VOICE_McModemObjInit(VOICE_McGetModemObjPtr(VOICE_MC_MODEM0));
    }

    /* ���2ͨ·����ͨ��״̬����ֹͣͨ��״̬ */
    if (VOICE_MC_STATE_RUNNING == VOICE_McGetModemVoiceState(VOICE_MC_MODEM1))
    {
        /* ֹͣ����������� */
        VOICE_McStop(VOICE_MC_MODEM1);

        /* ���õ�ǰ����Modem�� */
        VOICE_McSetForeGroundNum(VOICE_MC_MODEM_NUM_BUTT);

        /* ���������ʼ�� */
        VOICE_McModemObjInit(VOICE_McGetModemObjPtr(VOICE_MC_MODEM1));
    }

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    /* �ظ���ϢID_AP_HIFI_CCPU_RESET_REQ */
    stCnfMsg.uhwMsgId   = ID_HIFI_AP_CCPU_RESET_CNF;
    stCnfMsg.uhwResult  = VOS_OK;

    UCOM_SendDrvMsg(DSP_PID_VOICE,
                ACPU_PID_OM,
                &stCnfMsg,
                sizeof(HIFI_AP_CCPU_RESET_CNF_STRU));

    /* ��¼��־ */
    OM_LogInfo(VOICE_MsgApNoteCCPUReset_SUCC);

    return VOS_OK;

}


VOS_UINT32 VOICE_MsgVoiceLoopReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32              uwRet;
    VCVOICE_LOOP_REQ_STRU  *pstVcLoopSet;

    pstVcLoopSet = (VCVOICE_LOOP_REQ_STRU *)pstOsaMsg;

    if (VCVOICE_LOOP_START == pstVcLoopSet->enMode)
    {
        CODEC_PcmSetPcmFrameLength(CODEC_FRAME_LENGTH_NB);
        VOICE_McSetVoiceState(VOICE_MC_MODEM0, VOICE_MC_STATE_RUNNING);

        /* ����ģʽ���£���Ϊֻ������״̬ΪRUNNING̬��Ϊ��BUTTʱ��������Ӧ����Ϣ */
        VOICE_McSetModemNetMode(VOICE_MC_MODEM0, CODEC_NET_MODE_W);

        uwRet = VOICE_McSetLoopEnable();
    }
    else
    {
        uwRet = VOICE_McSetLoopDisable();

        /* ����ģʽ����ΪMODE_BUTT */
        VOICE_McSetModemNetMode(VOICE_MC_MODEM0, CODEC_NET_MODE_BUTT);

        VOICE_McSetVoiceState(VOICE_MC_MODEM0, VOICE_MC_STATE_IDLE);
    }

    /* �ظ�NASִ�н�� */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_LOOP_CNF, pstVcLoopSet->uwSenderPid, uwRet);

    return uwRet;

}
VOS_UINT32 VOICE_McSetLoopEnable(VOS_VOID)
{
    VOS_UINT16                      uhwIsModeMaster;
    DRV_SIO_INT_MASK_ENUM_UINT32    enMask;
    VOS_UINT32                      uwRet;

    enMask  = DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER;
    enMask |= DRV_SIO_INT_MASK_RX_LEFT_FIFO_OVER;
    enMask |= DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER;
    enMask |= DRV_SIO_INT_MASK_TX_LEFT_FIFO_UNDER;

    /* ��ȡSIO����ģʽ���� */
    UCOM_NV_Read( en_NV_Item_SIO_Voice_Master,
                 &uhwIsModeMaster,
                  sizeof(uhwIsModeMaster));

    uwRet  = DRV_SIO_Voice_Open(enMask, DRV_SIO_SAMPLING_8K, uhwIsModeMaster, VOICE_SioIsr, 0);

    uwRet += VOICE_PcmMicInStartLoopDMA(VOICE_MC_DMAC_CHN_MIC, VOICE_PcmVcLoopMicIsr);

    uwRet += VOICE_PcmSpkOutStartLoopDMA(VOICE_MC_DMAC_CHN_SPK, VOICE_PcmVcLoopSpkIsr);

    if (UCOM_RET_SUCC == uwRet)
    {
        return UCOM_RET_SUCC;
    }
    else
    {
        return UCOM_RET_FAIL;
    }
}
VOS_UINT32 VOICE_McSetLoopDisable(VOS_VOID)
{
    DRV_SIO_Voice_Close();

    DRV_DMA_Stop(VOICE_MC_DMAC_CHN_MIC);
    DRV_DMA_Stop(VOICE_MC_DMAC_CHN_SPK);

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

