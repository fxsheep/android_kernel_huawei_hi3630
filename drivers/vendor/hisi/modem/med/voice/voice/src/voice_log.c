

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "ucom_comm.h"
#include "ucom_nv.h"
#include "om_log.h"
#include "voice_log.h"
#include "voice_mc.h"
#include "voice_proc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _MED_ERRORLOG
/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_LOG_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

MSG_CODEC_ERRLOG_STRU                     g_stVoiceErrLogMsg;                       /* �ϱ� OM �� ErrorLog ��Ϣ�ṹ */
VOICE_ERRLOG_STATUS_STRU                  g_stVoiceErrLogStatus;                    /* ErrorLog ״̬ */

/* NV����� */
CODEC_ERRLOG_CONFIG_STRU                  g_stVoiceErrLogPara;

/* �¼�Լ������ */
VOS_UINT16 g_auhwVoiceErrLogEvntRules[][sizeof(VOICE_ERRLOG_EVENT_RULE_STRU)/sizeof(VOS_UINT16)] =
{
    /* �����ϸ����¼������ö��˳���屾�����Ԫ�� */

    /*          ͳ���¼�����         |         �¼�����ģʽ          |           �¼�Ĭ������          */

    /* CODEC_ERRLOG_EVENTS_DMA_MIC_RESET */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_DMA_SPK_RESET */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_WCDMA_DEC_INT_LOST */
    { CODEC_ERRLOG_EVENT_TYPE_LOST,   CODEC_ERRLOG_EVENT_MODE_WCDMA,  VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_WCDMA_BBP_DATA_NONE */
    { CODEC_ERRLOG_EVENT_TYPE_REPEAT, CODEC_ERRLOG_EVENT_MODE_WCDMA,  VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_WCDMA_BBP_DATA_BAD */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_WCDMA,  VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_WCDMA_DECIPHER_LOST */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_WCDMA,  VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_GSM_BFI_FRAME */
    { CODEC_ERRLOG_EVENT_TYPE_REPEAT, CODEC_ERRLOG_EVENT_MODE_GSM,  ( 2 * VOICE_ERRLOG_EVENT_DEFAULT_THD ) },

    /* CODEC_ERRLOG_EVENTS_CODEC_SID_FRAME */
    { CODEC_ERRLOG_EVENT_TYPE_REPEAT, CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_PCVOICE_INTERPOLATE */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_PCVOICE_RINGBUF_FULL */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },
};



/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


VOS_VOID VOICE_ErrLogResetEventCnts( VOICE_ERRLOG_EVENT_INFO_STRU *pstEvent )
{
    pstEvent->uhwCnt     =  0;
    pstEvent->shwLastCyc =  VOICE_ERRLOG_LAST_CYCLE_INI_VAL;
}


VOS_UINT16 VOICE_ErrLogGetEventThd( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    VOS_UINT16                              uhwThd;
    VOS_UINT16                             *puhwNvTab;
    VOICE_ERRLOG_EVENT_RULE_STRU             *pstEvntsRule;

    /* ��ֹ����Խ�� */
    if( enEvent >= ( (sizeof( CODEC_ERRLOG_CONFIG_STRU ) / sizeof( VOS_UINT16 ) ) - VOICE_ERRLOG_CONFIG_HEAD_LEN ) )
    {
        /* ������Ϣ��� */
        OM_LogError(VOICE_ErrLogEventNumBeyondNvItems);

        return VOICE_ERRLOG_EVENT_DEFAULT_THD;
    }

    puhwNvTab       = (VOS_UINT16*)&g_stVoiceErrLogPara;
    pstEvntsRule    = ( VOICE_ERRLOG_EVENT_RULE_STRU* )g_auhwVoiceErrLogEvntRules[enEvent];

    /* ��ȡ��Ӧλ�õ�Nv�� */
    uhwThd = puhwNvTab[enEvent + VOICE_ERRLOG_CONFIG_HEAD_LEN];

    /* ���Nv����ֵ�Ϸ��� */
    if( uhwThd > VOICE_ERRLOG_EVENT_MAXIMUM_THD )
    {
        /* ������Ϣ��� */
        OM_LogError(VOICE_ErrLogInvalidNvItemValue);

        uhwThd = pstEvntsRule->uhwThd;
    }

    return uhwThd;
}
VOS_VOID VOICE_ErrLogInit( CODEC_ERRLOG_CONFIG_STRU *pstErrlogCfg )
{
    VOS_UINT16                          uhwEvent;
    VOICE_ERRLOG_EVENT_INFO_STRU         *pstEvntsInfo;
    VOICE_ERRLOG_EVENT_RULE_STRU         *pstEvntsRule;

    UCOM_MemCpy(&g_stVoiceErrLogPara,
                pstErrlogCfg,
                sizeof(CODEC_ERRLOG_CONFIG_STRU));

    /* ErrorLog����״̬�ṹ���ʼ�� */
    g_stVoiceErrLogStatus.uhwEnable = g_stVoiceErrLogPara.uhwEnable;
    pstEvntsInfo = g_stVoiceErrLogStatus.astEventsInfo;

    /* �ж�Nv���Ƿ��� */
    if (g_stVoiceErrLogStatus.uhwEnable >= CODEC_SWITCH_BUTT)
    {
        /* ������Ϣ��� */
        OM_LogError(VOICE_ErrLogNvItemsBroken);

        g_stVoiceErrLogStatus.uhwEnable = CODEC_SWITCH_OFF;
    }

    /* ��ErrorLogδʹ��,���˳� */
    if (CODEC_SWITCH_OFF == g_stVoiceErrLogStatus.uhwEnable)
    {
        /* ������Ϣ��� */
        OM_LogError(VOICE_ErrLogNvDisableErrLog);

        return;
    }

    /* ��ʼ���¼����� */
    for (uhwEvent = 0; uhwEvent < CODEC_ERRLOG_EVENTS_ENUM_BUTT; uhwEvent++)
    {
        /* ��ʼ������ֵ���� */
        VOICE_ErrLogResetEventCnts( &pstEvntsInfo[uhwEvent] );

        /* ��ʼ���¼�Լ�� */
        pstEvntsRule = ( VOICE_ERRLOG_EVENT_RULE_STRU* )g_auhwVoiceErrLogEvntRules[uhwEvent];

        pstEvntsInfo[uhwEvent].stRules.enMode = pstEvntsRule->enMode;
        pstEvntsInfo[uhwEvent].stRules.enType = pstEvntsRule->enType;

        /* ��ʼ��������ֵ */
        pstEvntsInfo[uhwEvent].stRules.uhwThd = VOICE_ErrLogGetEventThd( uhwEvent );

        /* ��ʼ���ϱ����� */
        pstEvntsInfo[uhwEvent].uhwRprtd = 0;
    }

    /* ���ü����� */
    g_stVoiceErrLogStatus.shwCycleCnt = 0;

    return;
}
VOS_VOID VOICE_ErrLogRecord( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    /* �¼������¼�ṹ���� */
    VOICE_ERRLOG_EVENT_INFO_STRU       *pstEvntsInfo;
    VOICE_MC_STATE_ENUM_UINT16          enState;

    /* ��ȡǰ̨����״̬ */
    enState = VOICE_McGetForeGroundVoiceState();

    pstEvntsInfo = g_stVoiceErrLogStatus.astEventsInfo;

    /* �¼���Ų�����,ErrorLog����δ����,���������RUNNING̬ */
    if ((VOICE_MC_STATE_RUNNING != enState)
        || (CODEC_SWITCH_OFF == g_stVoiceErrLogStatus.uhwEnable)
        || (enEvent >= CODEC_ERRLOG_EVENTS_ENUM_BUTT))
    {
        return;
    }

    /* �����¼����ͽ��д����¼ */
    switch(pstEvntsInfo[enEvent].stRules.enType)
    {
        /* ��������ͳ���Լ���ʧ�¼�ͳ�Ƶ��¼� */
        case CODEC_ERRLOG_EVENT_TYPE_TOTAL:
        case CODEC_ERRLOG_EVENT_TYPE_LOST:
        {
            /* ��¼���� */
            pstEvntsInfo[enEvent].uhwCnt++;

            /* ��¼������ʱ�� */
            pstEvntsInfo[enEvent].shwLastCyc = g_stVoiceErrLogStatus.shwCycleCnt;
        }
        break;

        /* ���������¼���ͳ�� */
        case CODEC_ERRLOG_EVENT_TYPE_REPEAT:
        {
            /* ��¼�ظ����� */
            if (1 == (g_stVoiceErrLogStatus.shwCycleCnt - pstEvntsInfo[enEvent].shwLastCyc))
            {
                pstEvntsInfo[enEvent].uhwCnt++;

                /* ��¼������ʱ�� */
                pstEvntsInfo[enEvent].shwLastCyc = g_stVoiceErrLogStatus.shwCycleCnt;
            }
            else
            {
                /* �������¼�����С������ʱ,�������¼��������� */
                if (pstEvntsInfo[enEvent].uhwCnt < pstEvntsInfo[enEvent].stRules.uhwThd)
                {
                    pstEvntsInfo[enEvent].uhwCnt = 0;

                    /* ��¼������ʱ�� */
                    pstEvntsInfo[enEvent].shwLastCyc = g_stVoiceErrLogStatus.shwCycleCnt;
                }
            }
        }
        break;

        default:
        {
            /* Ĭ�ϼ�¼���� */
            pstEvntsInfo[enEvent].uhwCnt++;

            /* ��¼������ʱ�� */
            pstEvntsInfo[enEvent].shwLastCyc = g_stVoiceErrLogStatus.shwCycleCnt;
        }
        break;
    }

}


VOS_VOID  VOICE_ErrLogUpdateEnv( VOS_VOID )
{
    CODEC_ERRLOG_ENV_STRU              *pstErrEnv;          /* VOICE ������л��� */
    VOS_UINT16                          uhwCodecType;       /* ���������� */
    VOS_UINT16                          uhwAmrMode;         /* AMR����ģʽ */
    VOS_UINT16                          uhwNetMode;         /* ����ģʽ */
    VOS_UINT16                          uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOICE_PROC_CODEC_STRU              *pstCodec         = VOICE_ProcGetCodecPtr();

    pstErrEnv       = &g_stVoiceErrLogMsg.stEventRpt.stErrEnv;
    uhwCodecType    = pstCodec->enCodecType;
    uhwAmrMode      = pstCodec->stEncInObj.enAmrMode;

    /* ��ǰ����ǰ̨��������»���״̬ */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwActiveModemNo)
    {
        return ;
    }
    else
    {
        uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    }

    /* ����������״̬  */
    switch(uhwCodecType)
    {
        /* AMR */
        case CODEC_AMR:
        {
            if (   (CODEC_NET_MODE_W == uhwNetMode)
                || (CODEC_NET_MODE_TD == uhwNetMode) )
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_WCDMA_AMR_475
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
            }
            else if (CODEC_NET_MODE_G == uhwNetMode)
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_GSM_AMR_475
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
            }
            else
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_ENUM_BUTT;
            }
        }
        break;

        /* AMR2 */
        case CODEC_AMR2:
        {
            pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_WCDMA_AMR2_475
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
        }
        break;

        /* AMRWB */
        case CODEC_AMRWB:
        {
            if (CODEC_NET_MODE_W == uhwNetMode)
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_WCDMA_AMRWB_660
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
            }
            else if (CODEC_NET_MODE_G == uhwNetMode)
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_GSM_AMRWB_660
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
            }
            else
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_ENUM_BUTT;
            }
        }
        break;

        /* EFR, FR, HR */
        case CODEC_EFR:
        case CODEC_FR:
        case CODEC_HR:
        {
            pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_GSM_EFR
                                    + ( uhwCodecType - CODEC_EFR );
        }
        break;

        default:
        {
            pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_ENUM_BUTT;
        }
        break;

    }

    /* ��������������� */

    pstErrEnv->stInout.uhwTxIn  = g_stVoiceMcObjs.stInout.usTxIn;
    pstErrEnv->stInout.uhwRxOut = g_stVoiceMcObjs.stInout.usRxOut;
}
VOS_VOID  VOICE_ErrLogPackLogHead(
                MNTN_HEAD_INFO_STRU *pstMsgMntnHead,
                CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    /* ��¼����� */
    pstMsgMntnHead->ulErrNo = enEvent + VOICE_ERRLOG_ERRNO_BASE;

    /* д����Ϣ����, ���*2��ת����byte��λ(ZSP����) */
    pstMsgMntnHead->ulInfoLen = sizeof( MSG_CODEC_ERRLOG_STRU )
                                 - sizeof( MSG_CODEC_OM_ERRLOG_HEAD_STRU);

    /* ��ARM�������� */
    pstMsgMntnHead->ulSliceTime = 0;
    pstMsgMntnHead->ulBoardtime = 0;
}
VOS_VOID  VOICE_ErrLogReport( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    VOICE_ERRLOG_EVENT_INFO_STRU         *pstEvntsInfo;
    MNTN_HEAD_INFO_STRU                *pstHeadInfo;
    MSG_CODEC_ERRLOG_STRU                *pstErrorLog;

    pstHeadInfo  = (MNTN_HEAD_INFO_STRU *)VOICE_ERRLOG_GetRecordHeadPtr();
    pstEvntsInfo = (VOICE_ERRLOG_EVENT_INFO_STRU *)VOICE_ERRLOG_GetEventsInfoPtr();
    pstErrorLog  = (MSG_CODEC_ERRLOG_STRU *)VOICE_ERRLOG_GetErrLogMsgPtr();

    UCOM_MemSet(pstErrorLog, 0, sizeof(MSG_CODEC_ERRLOG_STRU));

    /* ���»���״̬  */
    VOICE_ErrLogUpdateEnv();

    /* ��¼�����¼��������� */
    switch (pstEvntsInfo[enEvent].stRules.enType)
    {
        /* ��������ͳ���Լ������¼�ͳ�Ƶ��¼� */
        case CODEC_ERRLOG_EVENT_TYPE_TOTAL:
        case CODEC_ERRLOG_EVENT_TYPE_REPEAT:
        {
            /* ��¼���� */
            pstErrorLog->stEventRpt.uhwErrCnt = pstEvntsInfo[enEvent].uhwCnt;
        }
        break;

        /* ���ڶ�ʧ��Ŀ����ͳ�Ƶ��¼� */
        case CODEC_ERRLOG_EVENT_TYPE_LOST:
        {
            /* ��¼��ʧ���� */
            pstErrorLog->stEventRpt.uhwErrCnt =  VOICE_ERRLOG_CHECK_PERIOD
                                                     - pstEvntsInfo[enEvent].uhwCnt;
        }
        break;

        default:
        {
            /* Ĭ�ϼ�¼���� */
            pstErrorLog->stEventRpt.uhwErrCnt = pstEvntsInfo[enEvent].uhwCnt;
        }
        break;
    }

    /* ��¼�����¼����� */
    pstErrorLog->stEventRpt.enErrType = pstEvntsInfo[enEvent].stRules.enType;

    /* д OM ��Ϣͷ */
    pstErrorLog->stOmHead.usTransPrimId = MSG_VOICE_OM_ERRLOG_PID;

    /*  дErrorLog ͷ */
    VOICE_ErrLogPackLogHead(pstHeadInfo, enEvent);

    /* �ϱ� ErrorLog ��Ϣ */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                         UCOM_PID_PS_OM,
                         (VOS_VOID *)pstErrorLog,
                         sizeof(MSG_CODEC_ERRLOG_STRU));
}
VOS_UINT32 VOICE_ErrLogCheckEvent( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    VOICE_ERRLOG_EVENT_INFO_STRU         *pstEvntsInfo;
    VOS_UINT32                            uwResult;
    VOS_UINT16                            uhwNetMode;
    VOS_UINT16                            uhwActiveModemNo = VOICE_McGetForeGroundNum();

    pstEvntsInfo    = g_stVoiceErrLogStatus.astEventsInfo;
    uwResult        = CODEC_SWITCH_OFF;

    /* ��ǰ����ǰ̨,�����ϱ� */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwActiveModemNo)
    {
        return uwResult;
    }
    else
    {
        uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    }

    /* ���л������ */
    if (CODEC_ERRLOG_EVENT_MODE_ALL != pstEvntsInfo[enEvent].stRules.enMode)
    {
        if (uhwNetMode != pstEvntsInfo[enEvent].stRules.enMode)
        {
            return uwResult;
        }
    }

    /* �����¼����ͼ�� */
    switch (pstEvntsInfo[enEvent].stRules.enType)
    {
        /* ��������ͳ���Լ������¼�ͳ�Ƶ��¼� */
        case CODEC_ERRLOG_EVENT_TYPE_TOTAL:
        case CODEC_ERRLOG_EVENT_TYPE_REPEAT:
        {
            /* ����¼����� */
            if (pstEvntsInfo[enEvent].uhwCnt >= pstEvntsInfo[enEvent].stRules.uhwThd)
            {
                uwResult = CODEC_SWITCH_ON;
            }
        }
        break;

        /* ���ڶ�ʧ��Ŀ����ͳ�Ƶ��¼� */
        case CODEC_ERRLOG_EVENT_TYPE_LOST:
        {
             /* ��鶪ʧ��ֵ */
            if (pstEvntsInfo[enEvent].uhwCnt
                < (VOICE_ERRLOG_CHECK_PERIOD - pstEvntsInfo[enEvent].stRules.uhwThd))
            {
                uwResult = CODEC_SWITCH_ON;
            }
        }
        break;

        default:
        {
            /* Ĭ�ϲ��ϱ� */
        }
        break;
    }

    return uwResult;
}


VOS_VOID VOICE_ErrLogCycle( VOS_INT16 shwPeriod )
{
    VOS_UINT16                          uhwEvent;
    VOICE_ERRLOG_EVENT_INFO_STRU       *pstEvntsInfo;
    VOICE_MC_STATE_ENUM_UINT16          enState;

    /* ��ȡǰ̨����״̬ */
    enState = VOICE_McGetForeGroundVoiceState();

    pstEvntsInfo    = g_stVoiceErrLogStatus.astEventsInfo;

    /* �����������RUNNING̬����ErrorLogδʹ�� */
    if ((VOICE_MC_STATE_RUNNING != enState)
        || (CODEC_SWITCH_OFF == g_stVoiceErrLogStatus.uhwEnable))
    {
        return;
    }

    /* �������� */
    g_stVoiceErrLogStatus.shwCycleCnt++;

    /* ����Ƿ�������һ���� */
    if (0 == (g_stVoiceErrLogStatus.shwCycleCnt % shwPeriod))
    {
        for (uhwEvent = 0; uhwEvent < CODEC_ERRLOG_EVENTS_ENUM_BUTT; uhwEvent++)
        {
            /* ����Ƿ�����ϱ����� */
            if (VOICE_ErrLogCheckEvent(uhwEvent) != CODEC_SWITCH_OFF)
            {
                /* ����ϱ����� */
                if (pstEvntsInfo[uhwEvent].uhwRprtd < VOICE_ERRLOG_EVENT_REPORT_MAX)
                {
                    /* �ϱ���Ϣ */
                    VOICE_ErrLogReport(uhwEvent);

                    /* ������Ϣ��� */
                    OM_LogInfo(VOICE_ErrLogReportEvent);

                    /* �ϱ��������� */
                    pstEvntsInfo[uhwEvent].uhwRprtd++;
                }
            }

            /* ���¼����������� */
            VOICE_ErrLogResetEventCnts(&pstEvntsInfo[uhwEvent]);
        }

        /* ������������,��ֹ��� */
        g_stVoiceErrLogStatus.shwCycleCnt = 0;

    }
}
VOS_VOID  VOICE_ErrLogResetStatus( VOS_VOID )
{
    VOS_UINT16                       uhwEvent;
    VOICE_ERRLOG_EVENT_INFO_STRU      *pstEvntsInfo;

    pstEvntsInfo    = g_stVoiceErrLogStatus.astEventsInfo;

    /* δʹ��ErrorLog */
    if (CODEC_SWITCH_OFF == g_stVoiceErrLogStatus.uhwEnable)
    {
        return;
    }

    for (uhwEvent = 0; uhwEvent < CODEC_ERRLOG_EVENTS_ENUM_BUTT; uhwEvent++)
    {
        /* ���¼����������� */
        VOICE_ErrLogResetEventCnts(&pstEvntsInfo[uhwEvent]);
    }

    /* ������������ */
    g_stVoiceErrLogStatus.shwCycleCnt = 0;

}

#endif /* _MED_ERRORLOG */



VOS_VOID VOICE_ErrLogCheckDecStatus( VOS_VOID *pstObj )
{
    CODEC_DEC_IN_PARA_STRU  *pstDecObj;
    VOS_UINT16               uhwNetMode;         /* ����ģʽ */
    VOS_UINT16               uhwActiveModemNo = VOICE_McGetForeGroundNum();

    /* ��ǰ����ǰ̨�����������״̬ */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwActiveModemNo)
    {
        return ;
    }
    else
    {
        uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    }

    pstDecObj = (CODEC_DEC_IN_PARA_STRU *)pstObj;

    /* BFI ֡ */
    if (CODEC_BFI_YES == pstDecObj->enBfiFlag)
    {
        /* ErrorLog ��¼�¼� */
        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_GSM_BFI_FRAME);
    }

    /* GSM �е� SID֡ */
    if ((CODEC_NET_MODE_G == uhwNetMode)
       && (pstDecObj->enSidFlag != CODEC_SID_SPEECH))
    {
        /* ErrorLog ��¼�¼� */
        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_CODEC_SID_FRAME);
    }

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

