

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "voice_diagnose.h"
#include "codec_op_lib.h"
#include "med_drv_timer_hifi.h"
#include "ucom_mem_dyn.h"
#include "voice_debug.h"
#include "ucom_nv.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_DIAGNOSE_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
VOICE_DIAG_FRAMENUM_STRU        g_stDiagFrameNum;
VOS_UINT32                      g_uwDiagDLSilenceNum;                            /* �����������������Ĵ��� */
VOICE_DIAG_NV_STRU              g_stVoiceDiagCfg;
VOS_UINT16                      g_uhwVoiceDiagReportFalg[VOICE_DIAG_MODEM_NUM_BUTT][VOICE_ERR_CAUSE_BUTT];
VOICE_DIAG_CHANNEL_QUALITY_STRU g_stChannelQuality;                              /* ������ϱ����ŵ�������Ϣ�������������ϼ�� */

VOICE_DIAG_DMA_ISR_STRU         g_stDiagDmaIsr;
VOS_UINT32                      g_auwSuspendBeginTs[VOICE_DIAG_MODEM_NUM_BUTT]; /* �����л���ʼ�����������ͣ��ʱ�� */

NV_ID_ERR_LOG_CTRL_INFO_STRU    g_stSysErrLogCfg;
/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


VOS_VOID VOICE_DiagInit(VOICE_DIAG_NV_STRU *pstVoiceDiagCfg)
{
    VOICE_DIAG_NV_STRU       *pstDiagCfg = VOICE_DiagGetCfgPtr();

    /* ��ȡϵͳErrLog NV���Ʋ��� */
    UCOM_NV_Read(en_NV_Item_ErrLogCtrlInfo,
                 &g_stSysErrLogCfg,
                 sizeof(g_stSysErrLogCfg));

    /* ��ȡ�������ģ���Nv���� */
    UCOM_MemSet(pstDiagCfg, 0, sizeof(VOICE_DIAG_NV_STRU));
    UCOM_MemCpy(pstDiagCfg, pstVoiceDiagCfg, sizeof(VOICE_DIAG_NV_STRU));

    /* ���֡ͳ����Ϣ */
    VOICE_DiagReset();

    /* �����м����Ƿ��ϱ���Ϣ�ı�־����Ϊ0 */
    UCOM_MemSet(g_uhwVoiceDiagReportFalg,
                0,
                ((VOS_UINT32)VOICE_DIAG_MODEM_NUM_BUTT * VOICE_ERR_CAUSE_BUTT * sizeof(VOS_UINT16)));

    /* ���DMAͳ����Ϣ */
    UCOM_MemSet(&g_stDiagDmaIsr, 0, sizeof(VOICE_DIAG_DMA_ISR_STRU));

    /* ��ʱ������ */
    UCOM_MemSet(g_auwSuspendBeginTs, 0, VOICE_DIAG_MODEM_NUM_BUTT * sizeof(VOS_UINT32));

}


VOS_VOID VOICE_DiagGsmGoodFrmNum( VOICE_DIAG_RX_DATA_STRU *pstRxData )
{
    VOICE_DIAG_FRAMENUM_STRU *pstFrameNum = VOICE_DiagGetFrameNumPtr();
    CODEC_ENUM_UINT16         enCodecType;

    enCodecType = pstRxData->enCodecType;

    if((CODEC_FR   == enCodecType)
      ||(CODEC_EFR == enCodecType)
      ||(CODEC_HR  == enCodecType))
    {
        /* SID֡ */
        if(pstRxData->enSid != CODEC_SID_SPEECH)
        {
            if(CODEC_HR  == enCodecType)
            {
                pstFrameNum->uhwGoodFrameNum += VOICE_DIAG_HR_SID_NUM;
            }
            else
            {
                pstFrameNum->uhwGoodFrameNum += VOICE_DIAG_FR_EFR_SID_NUM;
            }
        }
        /* GOOD֡ */
        else if(CODEC_BFI_NO == pstRxData->enBfi)
        {
            pstFrameNum->uhwGoodFrameNum++;
        }
        else
        {
        }
    }
    else
    {
        if((CODEC_AMR_TYPE_RX_SID_FIRST == pstRxData->enAmrFrameType)
         ||(CODEC_AMR_TYPE_RX_SID_UPDATE == pstRxData->enAmrFrameType)
         ||(CODEC_AMR_TYPE_RX_SID_BAD == pstRxData->enAmrFrameType))
        {
            pstFrameNum->uhwGoodFrameNum += VOICE_DIAG_AMR_SID_NUM;
        }
        else if(CODEC_AMR_TYPE_RX_SPEECH_GOOD == pstRxData->enAmrFrameType)
        {
            pstFrameNum->uhwGoodFrameNum++;
        }
        else
        {
        }
    }
}


VOS_VOID VOICE_DiagCalcFrameNum(VOICE_DIAG_RX_DATA_STRU *pstRxData)
{
    VOICE_DIAG_FRAMENUM_STRU *pstFrameNum = VOICE_DiagGetFrameNumPtr();

    switch(pstRxData->enNetMode)
    {
        case CODEC_NET_MODE_G:
        {
            VOICE_DiagGsmGoodFrmNum(pstRxData);

            break;
        }
        case CODEC_NET_MODE_W:
        case CODEC_NET_MODE_TD:
        case CODEC_NET_MODE_L:
        {
            if(CODEC_AMR_FQI_QUALITY_GOOD == pstRxData->enQualityIdx)
            {
                pstFrameNum->uhwGoodFrameNum++;
            }

            break;
        }
        default:
        {
            OM_LogError(VOICE_DiagnoseGoodFrameNum_UnkownMode);
        }

    }

    pstFrameNum->uhwTotalFrameNum += 1;

}


VOS_VOID VOICE_DiagReset( VOS_VOID )
{
    VOICE_DIAG_FRAMENUM_STRU *pstFrameNum = VOICE_DiagGetFrameNumPtr();

    pstFrameNum->uhwGoodFrameNum = 0;
    pstFrameNum->uhwTotalFrameNum = 0;

    /* �������������������Ĵ�����Ϊ0 */
    VOICE_DiagSetDLSilenceNum(0);

    /* ���������Ϣ������Ϣ��� */
    UCOM_MemSet(VOICE_DiagGetChannelQualityPtr(),
                0,
                sizeof(VOICE_DIAG_CHANNEL_QUALITY_STRU));

}


VOS_VOID VOICE_DiagLineIn(
                VOICE_DIAG_MODEM_NUM_ENUM_UINT16 enActiveModemNo,
                VOICE_DIAG_RX_DATA_STRU         *pstRxData)
{
    VOICE_DIAG_FRAMENUM_STRU               *pstFrameNum   = VOICE_DiagGetFrameNumPtr();
    VOICE_DIAG_NV_STRU                     *pstDiagCfg    = VOICE_DiagGetCfgPtr();
    VOICE_DIAG_CHANNEL_QUALITY_STRU        *pstDiagChanQa = VOICE_DiagGetChannelQualityPtr();
    VOS_INT32                               shwDiagEnable;
    VOS_INT16                               shwDiagTime;                        /* ͳ��ʱ�䣬��λ(s) */
    VOS_INT16                               shwGoodFrmPercentThd;               /* Q15 (0.8*32767) */
    HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU  stOneWayNoSound;

    UCOM_MemSet(&stOneWayNoSound, 0, sizeof(stOneWayNoSound));

    /* ��ȡNV���� */
    shwDiagEnable        = pstDiagCfg->shwEnable;
    shwDiagTime          = pstDiagCfg->stLineInPara.shwDiagTime;
    shwGoodFrmPercentThd = pstDiagCfg->stLineInPara.shwGoodFrmPercentThd;

    /* LineIn���������ϼ�ⲻʹ�ܣ�ֱ�ӷ��� */
    if(!shwDiagEnable)
    {
        return;
    }

    /* ���ǵ�ǰ�����ź�������Ӱ�� */
    if(0 == pstDiagCfg->stChanQaPara.shwIsIgnoreChanQa)
    {
        /* ��ǰ�ŵ������ܲ����⣬ֱ�ӷ��� */
        if(pstDiagChanQa->uhwIsBadCell)
        {
            return;
        }
    }

    /* ͳ��Good֡ */
    VOICE_DiagCalcFrameNum(pstRxData);

    /* ��ͳ��ʱ�䲻�����������ֱ�ӷ��� */
    if(pstFrameNum->uhwTotalFrameNum < (shwDiagTime * VOICE_DIAG_FRAME_NUM_PER_SECOND))
    {
        return;
    }

    /* GOOD֡�ı��ص������� */
    if(pstFrameNum->uhwGoodFrameNum < CODEC_OpMult((VOS_INT16)pstFrameNum->uhwTotalFrameNum,
                                                    shwGoodFrmPercentThd))
    {
        VOICE_DiagAddDLSilenceNum();
    }
    else
    {
        VOICE_DiagSetDLSilenceNum(0);
    }

    /* ������֡����ֵ����Ϊ0 */
    UCOM_MemSet(VOICE_DiagGetFrameNumPtr(), 0, sizeof(VOICE_DIAG_FRAMENUM_STRU));

    /* ������3�ζ�Ϊsilence�����ϱ������ */
    if(VOICE_DiagGetDLSilenceNum() >= VOICE_DIAG_MAX_SILENCE_NUM)
    {
        /* ��¼������ͨʱ��HIFI�ṹ�� */
        stOneWayNoSound.uhwDevMode       = pstRxData->enDevMode;
        stOneWayNoSound.uhwActiveModem   = enActiveModemNo;
        stOneWayNoSound.uhwNetMode       = pstRxData->enNetMode;
        stOneWayNoSound.uhwIsBadCell     = pstDiagChanQa->uhwIsBadCell;
        stOneWayNoSound.uhwCodecType     = pstRxData->enCodecType;
        stOneWayNoSound.uhwEncoderInited = pstRxData->usIsEncInited;
        stOneWayNoSound.uhwDecoderInited = pstRxData->usIsDecInited;
        stOneWayNoSound.uhwCheckPoint    = VOICE_DIAG_REPORT_LINEIN;
        UCOM_MemCpy(stOneWayNoSound.uwData,
                    pstDiagChanQa->auwChannelQuality,
                    VOICE_DIAG_CHANNEL_QUALITY_LEN * sizeof(VOS_UINT32));

        /* �ϱ������Ϊ��ͨ */
        VOICE_DiagMsgReport(VOICE_DIAG_REPORT_LINEIN,
                            enActiveModemNo,
                            &stOneWayNoSound);
    }

}
VOS_VOID VOICE_DiagMsgReport(
                VOICE_DIAG_ERR_CAUSE_ENUM_UINT16         uhwErrCause,
                VOS_UINT16                               uhwModemNo,
                HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU  *pstOneWayNoSound)

{
    VOS_UINT16                    *puhwDiagReportFlag = VOICE_DiagGetReportFlag(uhwModemNo);
    OM_ERR_LOG_REPORT_CNF_STRU    *pstOmErrLogCnf;
    OM_ERR_LOG_DIAG_STRU          *pstDiagErrLog;
    VOS_UINT32                     uwDiagErrLogLen;
    VOS_UINT32                     uwOmErrLogCnfSize;

    /* ϵͳ��ErrLog���ܹر� */
    if(!g_stSysErrLogCfg.ucAlmStatus)
    {
        return;
    }

    /* ���������澯��ʹ�� */
    if(!(g_stSysErrLogCfg.aucReportBitMap[0] & ERR_LOG_ALARM_REPORT_ENABLE)
       || (!(g_stSysErrLogCfg.aucReportBitMap[1] & ERR_LOG_VOICE_ALARM_ENABLE)))
    {
        return;
    }

    /* �ü���û���ϱ�����ͨ��Ϣ */
    if(!puhwDiagReportFlag[uhwErrCause])
    {
        /* Ϊ�����ϱ����ݽṹ�����ڴ� */
        uwDiagErrLogLen = sizeof(OM_ERR_LOG_DIAG_STRU) + sizeof(HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU);
        pstDiagErrLog = (OM_ERR_LOG_DIAG_STRU*)UCOM_MemAlloc(uwDiagErrLogLen);

        /* Ϊ�����ϱ���Ϣ�����ڴ� */
        uwOmErrLogCnfSize = uwDiagErrLogLen + sizeof(OM_ERR_LOG_REPORT_CNF_STRU);
        pstOmErrLogCnf = (OM_ERR_LOG_REPORT_CNF_STRU*)UCOM_MemAlloc(uwOmErrLogCnfSize);


        /* �������ϱ����ݽṹ */
        pstDiagErrLog->stOmErrLogHeader.ulMsgModuleId = OM_ERR_LOG_MOUDLE_ID_HIFI;
        pstDiagErrLog->stOmErrLogHeader.usModemId = uhwModemNo;
        pstDiagErrLog->stOmErrLogHeader.usAlmId = HIFI_ERR_LOG_ONE_WAY_NO_SOUND;
        pstDiagErrLog->stOmErrLogHeader.usAlmLevel = HIFI_ERR_LOG_ALARM_CRITICAL;
        pstDiagErrLog->stOmErrLogHeader.usAlmType = HIFI_ERR_LOG_ALARM_COMMUNICATION;
        pstDiagErrLog->stOmErrLogHeader.usAlmLowSlice = DRV_TIMER_ReadSysTimeStamp();
        pstDiagErrLog->stOmErrLogHeader.ulAlmLength = sizeof(HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU);
        UCOM_MemCpy(pstDiagErrLog->auhwContent,
                    pstOneWayNoSound,
                    sizeof(HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU));

        /* �����ϱ���Ϣ */
        pstOmErrLogCnf->ulMsgName   = ID_OM_FAULT_ERR_LOG_IND;
        pstOmErrLogCnf->ulMsgType   = OM_ERR_LOG_MSG_FAULT_REPORT;
        pstOmErrLogCnf->ulRptlen    = uwDiagErrLogLen;
        UCOM_MemCpy(pstOmErrLogCnf->aucContent, pstDiagErrLog, uwDiagErrLogLen);

    #if(FEATURE_ON == FEATURE_PTM)
        /* ��OM����ID_OM_AUDIO_ERR_LOG_IND��Ϣ */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        ACPU_PID_OM,
                        pstOmErrLogCnf,
                        uwOmErrLogCnfSize);
    #endif

        /* �ϱ���ͨ��Ϣ */
        OM_LogError2(VOICE_DiagnoseMsgReport_OneWayNoSound,
                     uhwErrCause,
                     uhwModemNo);

        puhwDiagReportFlag[uhwErrCause] = VOICE_DIAG_TRUE;

        /* �ͷ��ڴ� */
        UCOM_MemFree(pstDiagErrLog);
        UCOM_MemFree(pstOmErrLogCnf);
    }

}



VOS_VOID VOICE_DiagAlarmReport(
                VOS_UINT16                               uhwModemNo,
                VOICE_DIAG_ERR_CAUSE_ENUM_UINT16         uhwAlarmCause,
                VOS_VOID                                *pstReportData,
                VOS_UINT32                               uwDataSize,
                HIFI_ERR_LOG_ALARM_ID_ENUM_UINT16        uhwAlarmId)

{
    OM_ERR_LOG_REPORT_CNF_STRU    *pstOmErrLogCnf;
    OM_ERR_LOG_DIAG_STRU          *pstDiagErrLog;
    VOS_UINT16                    *puhwDiagReportFlag = VOICE_DiagGetReportFlag(uhwModemNo);
    VOS_UINT32                     uwDiagErrLogSize;
    VOS_UINT32                     uwOmErrLogCnfSize;
    VOICE_DIAG_NV_STRU            *pstDiagCfg = VOICE_DiagGetCfgPtr();

    /* ϵͳ��ErrLog���ܹر� */
    if(!g_stSysErrLogCfg.ucAlmStatus)
    {
        OM_LogInfo1(VOICE_DiagnoseMsgReport_VoiceAlarm, 1);
        return;
    }

    /* ���������澯��ʹ�� */
    if(!(g_stSysErrLogCfg.aucReportBitMap[0] & ERR_LOG_ALARM_REPORT_ENABLE)
       || (!(g_stSysErrLogCfg.aucReportBitMap[1] & ERR_LOG_VOICE_ALARM_ENABLE)))
    {
        OM_LogInfo1(VOICE_DiagnoseMsgReport_VoiceAlarm, 2);
        return;
    }

    /* ������澯�ϱ����ܲ�ʹ�� */
    if(!pstDiagCfg->shwEnable)
    {
        OM_LogInfo1(VOICE_DiagnoseMsgReport_VoiceAlarm, 3);
        return;
    }

    /* ��ͨ�绰���Ѿ��ϱ���ͬ��澯 */
    if(puhwDiagReportFlag[uhwAlarmCause])
    {
        return;
    }

    /* ���ݸ澯ԭ������Ӧ���ϱ����� */
    uwDiagErrLogSize = sizeof(OM_ERR_LOG_DIAG_STRU) + uwDataSize;

    /* Ϊ�����ϱ����ݽṹ�����ڴ� */
    pstDiagErrLog = (OM_ERR_LOG_DIAG_STRU*)UCOM_MemAlloc(uwDiagErrLogSize);

    /* ���澯�ϱ�����ͷ���� */
    pstDiagErrLog->stOmErrLogHeader.ulMsgModuleId = OM_ERR_LOG_MOUDLE_ID_HIFI;
    pstDiagErrLog->stOmErrLogHeader.usModemId = uhwModemNo;
    pstDiagErrLog->stOmErrLogHeader.usAlmId = uhwAlarmId;
    pstDiagErrLog->stOmErrLogHeader.usAlmLevel = HIFI_ERR_LOG_ALARM_MAJOR;
    pstDiagErrLog->stOmErrLogHeader.usAlmType = HIFI_ERR_LOG_ALARM_COMMUNICATION;
    pstDiagErrLog->stOmErrLogHeader.usAlmLowSlice = DRV_TIMER_ReadSysTimeStamp();
    pstDiagErrLog->stOmErrLogHeader.ulAlmLength = uwDataSize;
    UCOM_MemCpy(pstDiagErrLog->auhwContent, pstReportData, uwDataSize);

    /* Ϊ�����ϱ���Ϣ�����ڴ� */
    uwOmErrLogCnfSize = uwDiagErrLogSize + sizeof(OM_ERR_LOG_REPORT_CNF_STRU);
    pstOmErrLogCnf = (OM_ERR_LOG_REPORT_CNF_STRU*)UCOM_MemAlloc(uwOmErrLogCnfSize);

    /* �����ϱ���Ϣ */
    pstOmErrLogCnf->ulMsgName   = ID_OM_ALARM_ERR_LOG_IND;
    pstOmErrLogCnf->ulMsgType   = OM_ERR_LOG_MSG_ALARM_REPORT;
    pstOmErrLogCnf->ulRptlen    = uwDiagErrLogSize;
    UCOM_MemCpy(pstOmErrLogCnf->aucContent, pstDiagErrLog, uwDiagErrLogSize);

#if(FEATURE_ON == FEATURE_PTM)
    /* ��OM����ID_OM_AUDIO_ERR_LOG_IND��Ϣ */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    ACPU_PID_OM,
                    pstOmErrLogCnf,
                    uwOmErrLogCnfSize);
#endif

    /* �ϱ���ͨ��Ϣ */
    OM_LogError1(VOICE_DiagnoseMsgReport_VoiceAlarm, uhwModemNo);

    puhwDiagReportFlag[uhwAlarmCause] = VOICE_DIAG_TRUE;

    /* �ͷ��ڴ� */
    UCOM_MemFree(pstDiagErrLog);
    UCOM_MemFree(pstOmErrLogCnf);

}


VOS_VOID VOICE_DiagSuspend(
                VOS_UINT16              uhwModemNo,
                VOS_UINT16              uhwCurNetMode,
                VOS_UINT16              uhwPreNetMode)
{
    VOS_UINT32                           uwTimeStamp;
    VOS_UINT32                           uwSusPendTime;         /* ��λ:0.1ms */
    HIFI_ERR_EVT_VOICE_SUSPEND_SLOW_STRU stSuspendSlowErr;
    VOICE_DIAG_NV_STRU                  *pstDiagCfg = VOICE_DiagGetCfgPtr();

    /* ��ȡSOC����������ȡ��ǰʱ�� */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* ���������л���ʱ:SetCodecʱ��� - Suspendʱ��� */
    uwSusPendTime = (UCOM_COMM_CycSub(uwTimeStamp, g_auwSuspendBeginTs[uhwModemNo], 0xffffffff)
        * VOICE_DIAG_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq();

    /* �����л���ʱ̫�� */
    if ( uwSusPendTime >= (VOS_UINT16)pstDiagCfg->stLineInPara.ahwReserve[0])
    {
        stSuspendSlowErr.uhwErrorReason = VOICE_SUSPEND_SLOW;
        stSuspendSlowErr.uhwActiveModem = uhwModemNo;
        stSuspendSlowErr.uhwCurNetMode  = uhwCurNetMode;
        stSuspendSlowErr.uhwPreNetMode  = uhwPreNetMode;
        stSuspendSlowErr.uwSuspendTime  = uwSusPendTime;

        /* �ϱ��澯 */
        VOICE_DiagAlarmReport(uhwModemNo,
                              VOICE_SUSPEND_SLOW,
                              &stSuspendSlowErr,
                              sizeof(stSuspendSlowErr),
                              HIFI_ERR_LOG_VOICE_SUSPEND_SLOW);
    }

    return;
}






#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

