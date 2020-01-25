/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : VOICE_proc.c
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��5��10��
  ����޸�   :
  ��������   : ���ļ���Ҫ�������������̿�������ϸ������ʵ��
  �����б�   :
              VOICE_ProcCfgGsmDlCodec
              VOICE_ProcCfgGsmUlCodec
              VOICE_ProcCfgUmtsDlCodec
              VOICE_ProcCfgUmtsUlCodec
              VOICE_ProcDecode
              VOICE_ProcDestroy
              VOICE_ProcEncode
              VOICE_ProcFrameTypeToRlc
              VOICE_ProcGetSampleRate
              VOICE_ProcInit
              VOICE_ProcInitCodec
              VOICE_ProcCheckUpdate
              VOICE_ProcOutputFrameType
              VOICE_ProcReadNv
              VOICE_ProcRxPp
              VOICE_ProcSetDevMode
              VOICE_ProcSetNv
              VOICE_ProcTxPp
              VOICE_ProcUpdate
              VOICE_ProcUpdateNv
  �޸���ʷ   :
  1.��    ��   : 2011��5��10��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
   1 ͷ�ļ�����
******************************************************************************/
#include "OmCodecInterface.h"
#include "om.h"

#include "ucom_comm.h"
#include "ucom_nv.h"

#include "codec_op_vec.h"

#include "voice_proc.h"
#include "voice_amr_mode.h"
#include "voice_pcm.h"
#include "voice_mc.h"
#include "voice_debug.h"
#include "med_gain.h"
#include "ucom_pcm.h"
#include "mlib_interface.h"
#include "voice_diagnose.h"
#include "om_cpuview.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                OM_FILE_ID_VOICE_PROC_C

/*****************************************************************************
   2 ȫ�ֱ�������
******************************************************************************/
/* ���������ṹ��ȫ�ֱ�������ȫ�ֱ�����Ҫ������Hifi��TCM�� */
UCOM_ALIGN(4)
UCOM_SEC_TCMBSS
VOICE_PROC_CODEC_DATA_STRU           g_stVoiceCodecData;

/* PROCģ�����ȫ�ֱ��� */
UCOM_ALIGN(4)
UCOM_SEC_TCMBSS
VOICE_PROC_OBJS_STRU                 g_stVoiceProcObjs = {0};

VOICE_DEC_SERIAL_STRU               *g_pstDecSerialWrite = &g_stVoiceCodecData.astDecSerial[0];

/* ������������ʵ�ӳ��� */
VOS_UINT32  g_auwCodecToSampleRateTbl[CODEC_BUTT][2]
    = {{CODEC_AMR,      CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_EFR,      CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_FR,       CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_HR,       CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_AMR2,     CODEC_SAMPLE_RATE_MODE_8000},
       {CODEC_AMRWB,    CODEC_SAMPLE_RATE_MODE_16000},
       {CODEC_G711,     CODEC_SAMPLE_RATE_MODE_16000}};

/*****************************************************************************
   3 �ⲿ��������
******************************************************************************/

/*****************************************************************************
   4 ����ʵ��
******************************************************************************/

VOS_UINT32 VOICE_ProcInit(VOS_VOID)
{
    VOS_INT16                     *pshwEncSerial = VOICE_ProcGetEncBufPtr();
    VOICE_DEC_SERIAL_STRU         *pstDecSerial  = VOICE_ProcGetDecBufPtr();
    VOS_INT16                     *pshwLineOut   = VOICE_ProcGetLineOutBufPtr();

    /* ��ʼ����������buff */
    UCOM_MemSet(pshwEncSerial,
                0,
                (VOICE_CODED_FRAME_WITH_OBJ_LEN  * sizeof(VOS_UINT16)));

    /* ��ʼ����������buff */
    UCOM_MemSet(pstDecSerial,
                0,
                VOICE_CODED_DECODE_BUF_FRAME_NUM * sizeof(VOICE_DEC_SERIAL_STRU));

    /* ��ʼ������ǰPCM����buff */
    UCOM_MemSet(pshwLineOut,
                0,
                ((VOS_UINT32)(VOS_INT32)CODEC_PCM_MAX_FRAME_LENGTH * sizeof(VOS_UINT16)));

    /* ��ѧ�����ʼ�� */
    MLIB_PathInit(MLIB_PATH_CS_VOICE_CALL_MICIN);
    MLIB_PathInit(MLIB_PATH_CS_VOICE_CALL_SPKOUT);

    return UCOM_RET_SUCC;

}


VOS_UINT32 VOICE_ProcUpdate(VOS_UINT16 uhwDevMode,VOS_UINT16 uhwNetMode)
{
    VOS_UINT32                          uwRet;
    MLIB_PARA_STRU                      stPara;

    UCOM_MemSet(&stPara, 0, sizeof(MLIB_PARA_STRU));

    /* ��˷�ͨ·�������� */
    stPara.uwChnNum     = 2;
    stPara.uwSampleRate = VOICE_ProcGetSampleRate();
    stPara.uwFrameLen   = ( (stPara.uwSampleRate == 16000) ? CODEC_FRAME_LENGTH_WB: CODEC_FRAME_LENGTH_NB);
    stPara.uwResolution = 16;
    stPara.enDevice     = uhwDevMode;
    stPara.enParaSource = MLIB_PARA_SOURCE_NV;
    stPara.uwDataSize   = 0;

    uwRet = MLIB_PathSet(MLIB_PATH_CS_VOICE_CALL_MICIN,
                 &stPara,
                 sizeof(MLIB_PARA_STRU));

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError1(VOICE_SetNvError, uwRet);
    }

    /* ������ͨ·�������� */
    stPara.uwChnNum     = 2;

    uwRet = MLIB_PathSet(MLIB_PATH_CS_VOICE_CALL_SPKOUT,
                 &stPara,
                 sizeof(MLIB_PARA_STRU));

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError1(VOICE_SetNvError, uwRet);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcInitEncoder(VOS_UINT16 usType, VOS_UINT16 usDtxMode)
{
    VOS_UINT32               uwInitRet        = UCOM_RET_SUCC;
    VOICE_PROC_CODEC_STRU   *pstCodec         = VOICE_ProcGetCodecPtr();
    VOS_UINT16               uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16               uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);


    /* �������������ձ���ʼ��ʱ������Ҫ����HOMING֡,�öԶ˽���������ͬʱ���г�ʼ�� */
    CODEC_OpVecSet((pstCodec->stEncInObj).pshwEncSpeech,
                   CODEC_PCM_FRAME_LENGTH,
                   VOICE_PROC_HOMING_MASK);

    switch(usType)
    {
        case CODEC_AMR:
        {
            if(CODEC_NET_MODE_W == uhwNetMode)
            {
                uwInitRet = AMR_Enc_Init(usDtxMode,VOICE_ProcOutputFrameType);
            }
            else
            {
                uwInitRet = AMR_Enc_Init(usDtxMode,MED_NULL);
            }
            OM_LogInfo1(VOICE_ProcInitCodec_AmrEncInit_Done, uwInitRet);
            break;
        }
        case CODEC_EFR:
        {
            uwInitRet = EFR_Enc_Init(usDtxMode);
            OM_LogInfo1(VOICE_ProcInitCodec_EfrEncInit_Done, uwInitRet);
            break;
        }
        case CODEC_FR:
        {
            uwInitRet = FR_Enc_Init(usDtxMode);
            OM_LogInfo1(VOICE_ProcInitCodec_FrEncInit_Done, uwInitRet);
            break;
        }
        case CODEC_HR:
        {
            uwInitRet = HR_Enc_Init(usDtxMode);
            OM_LogInfo1(VOICE_ProcInitCodec_HrEncInit_Done, uwInitRet);
            break;
        }
        case CODEC_AMRWB:
        {
            if (CODEC_NET_MODE_W == uhwNetMode)
            {
                uwInitRet = AMRWB_Enc_Init(usDtxMode, VOICE_ProcOutputFrameType);
            }
            else
            {
                uwInitRet = AMRWB_Enc_Init(usDtxMode, MED_NULL);
            }
            OM_LogInfo1(VOICE_ProcInitCodec_AmrWbEncInit_Done, uwInitRet);
            break;
        }

        default:
        {
            OM_LogError(VOICE_ProcInitCodec_UnkownEncoder);
            break;
        }
    }

    return uwInitRet;
}
VOS_UINT32 VOICE_ProcInitDecoder(VOS_UINT16 usType)
{
    VOS_UINT32                      uwInitRet = UCOM_RET_SUCC;
    VOICE_PROC_CODEC_STRU          *pstCodec  = VOICE_ProcGetCodecPtr();

    /* ����������ж�֡��������0 */
    VOICE_SetRxLostFrameCnt(0);

    /* ����ǰһ֡������Ϊ��ǰһ֡,BFI����ֵ��0,ǰһ֡�ṹ����0 */
    pstCodec->stDecInObj.enGoodFrameType = CODEC_PREFRAME_NONE;
    pstCodec->stDecInObj.uhwFrameBfiCnt  = 0;

    UCOM_MemSet(VOICE_ProcGetPreDecInObj(),
                0,
                sizeof(CODEC_DEC_PREIN_PARA_STRU));

    /* ��ʼ��ǰһ֡�ṹ����AMR֡���ͺ���ΪBFI֡ */
    pstCodec->stPreDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_NO_DATA;
    pstCodec->stPreDecInObj.enBfiFlag      = CODEC_BFI_YES;

    switch(usType)
    {
        case CODEC_AMR:
        {
            uwInitRet = AMR_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_AmrDecInit_Done, uwInitRet);
            break;
        }
        case CODEC_EFR:
        {
            uwInitRet = EFR_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_EfrDecInit_Done, uwInitRet);
            break;
        }
        case CODEC_FR:
        {
            uwInitRet = FR_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_FrDecInit_Done, uwInitRet);
            break;
        }
        case CODEC_HR:
        {
            uwInitRet = HR_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_HrDecInit_Done, uwInitRet);
            break;
        }
        case CODEC_AMRWB:
        {
            uwInitRet = AMRWB_Dec_Init();
            OM_LogInfo1(VOICE_ProcInitCodec_AmrWbDecInit_Done, uwInitRet);
            break;
        }

        default:
        {
            OM_LogError(VOICE_ProcInitCodec_UnkownDecoder);
            break;
        }
    }

    return uwInitRet;
}
VOS_UINT32 VOICE_ProcDestroy(VOICE_PROC_OBJS_STRU *pstProcObj)
{
    MLIB_PathClear(MLIB_PATH_CS_VOICE_CALL_MICIN);
    MLIB_PathClear(MLIB_PATH_CS_VOICE_CALL_SPKOUT);

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcGetSampleRate(VOS_VOID)
{
    VOICE_PROC_CODEC_STRU  *pstCodec = VOICE_ProcGetCodecPtr();
    VOS_UINT32              uwCodecType;

    /* ��ȡ�����������(����32bit,�����������Ч��) */
    if (VOS_NULL == pstCodec)
    {
        /* pstCodecδ����ֵ */
        OM_LogError(VOICE_ProcCodecPtrIsNULLError);
        /* ����Ĭ�ϲ����� */
        return CODEC_SAMPLE_RATE_MODE_8000;

    }
    uwCodecType = pstCodec->enCodecType;

    /* ��������������Ƿ���ȷ */
    if (g_auwCodecToSampleRateTbl[uwCodecType][0] == uwCodecType)
    {
        /* ���ز����� */
        return g_auwCodecToSampleRateTbl[uwCodecType][1];

    }
    else
    {
        /* ���������ö����ӳ������˳��һ�� */
        OM_LogError(VOICE_ProcCodecToSampleRateMapError);

        /* ����Ĭ�ϲ����� */
        return CODEC_SAMPLE_RATE_MODE_8000;
    }

}
VOS_UINT32 VOICE_ProcCfgGsmDlCodec(GPHY_VOICE_RX_DATA_IND_STRU *pstRxDataMsg)
{
    VOICE_PROC_CODEC_STRU      *pstCodec    = VOICE_ProcGetCodecPtr();
    CODEC_DEC_IN_PARA_STRU     *pstDecInObj = &pstCodec->stDecInObj;

    /* ���в����Ϸ��Լ�飬�쳣ʱ���ز������� */
    if (pstRxDataMsg->enCodecType >= CODEC_BUTT)
    {
        return UCOM_RET_ERR_PARA;
    }

    /* AMR������飬����������ΪAMRʱ���У��쳣ʱ���ز������� */
    if (CODEC_AMR == pstRxDataMsg->enCodecType)
    {
        /* AMR */
        if (pstRxDataMsg->enAmrMode >= (VOS_UINT16)CODEC_AMR_RATE_MODE_BUTT)
        {
            return UCOM_RET_ERR_PARA;
        }
    }
    /* AMR-WB������飬����������ΪAMR-WBʱ���У��쳣ʱ���ز������� */
    else if (CODEC_AMRWB == pstRxDataMsg->enCodecType)
    {
        /* AMR-WB */
        if (pstRxDataMsg->enAmrMode > (VOS_UINT16)CODEC_AMRWB_RATE_MODE_1265K)
        {
            return UCOM_RET_ERR_PARA;
        }
    }
    else
    {
        /* ��AMR�¼��BFI/SID/TAF/UFI�Ȳ��� */
        if (   (pstRxDataMsg->enBfi >= CODEC_BFI_BUTT)
            || (pstRxDataMsg->enSid >= CODEC_SID_BUTT)
            || (pstRxDataMsg->enTaf >= CODEC_TAF_BUTT)
            || (pstRxDataMsg->enUfi >= CODEC_BFI_BUTT))
        {
            return UCOM_RET_ERR_PARA;
        }
    }

    /* ���ս����������,����BFI/SID/TAF/UFI/AmrMode/AmrFrameType,����֡��ʽ��־ΪGSM�µ�AMR����֡ */
    pstDecInObj->enBfiFlag      = pstRxDataMsg->enBfi;
    pstDecInObj->enSidFlag      = pstRxDataMsg->enSid;
    pstDecInObj->enTafFlag      = pstRxDataMsg->enTaf;
    pstDecInObj->enHrUfiFlag    = pstRxDataMsg->enUfi;
    pstDecInObj->enAmrMode      = pstRxDataMsg->enAmrMode;
    pstDecInObj->enAmrFrameType = pstRxDataMsg->enAmrFrameType;
    pstDecInObj->enAmrFormat    = CODEC_AMR_FORMAT_BITS; /* ��֡��ʽ��־ΪGSM�µ�AMR����֡ */

    /* ����͵֡��� */
    if (CODEC_FRAME_FACCH_YES == pstRxDataMsg->enIsFrameFacch)
    {
        VOICE_ProcFacchFrameCntAdd();
        OM_LogInfo1(VOICE_ProcGsmRxFacch, VOICE_ProcGetFacchFrameCnt());
    }
    else
    {
        VOICE_ProcFacchFrameCntReset();
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_ProcCfgGsmUlCodec(GPHY_VOICE_UPDATE_PARA_CMD_STRU *pstUpdateMsg, VOS_UINT16 uhwModemNo)
{
    VOICE_PROC_CODEC_STRU       *pstCodec    = VOICE_McGetModemCodecPtr(uhwModemNo);
    CODEC_ENC_IN_PARA_STRU      *pstEncInObj = &pstCodec->stEncInObj;

    /* ���в����Ϸ��Լ�飬�쳣ʱ���ز������� */
    if ((pstUpdateMsg->enCodecType >= CODEC_BUTT)
        || (pstUpdateMsg->enDtxMode >= CODEC_SWITCH_BUTT)
        || (pstUpdateMsg->enAmrNsync >=CODEC_AMR_NSYNC_BUTT))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* AMRNB������飬����������ΪAMRʱ���У��쳣ʱ���ز������� */
    if ((CODEC_AMR == pstUpdateMsg->enCodecType)
        && (pstUpdateMsg->enAmrMode >= (VOS_UINT16)CODEC_AMR_RATE_MODE_BUTT))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* AMR-WB������飬����������ΪAMRʱ���У��쳣ʱ���ز������� */
    if ((CODEC_AMRWB == pstUpdateMsg->enCodecType)
        && (pstUpdateMsg->enAmrMode > (VOS_UINT16)CODEC_AMRWB_RATE_MODE_1265K))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* ��������DTXģʽ��Ҫ��������Ҫͬʱ���³�ʼ�������� */
    if ((VOS_INT16)pstUpdateMsg->enDtxMode != pstEncInObj->enDtxMode)
    {
        pstCodec->usIsEncInited = VOICE_PROC_IS_NO;
        pstEncInObj->enDtxMode  = pstUpdateMsg->enDtxMode;
    }

    /* �������������������������֡��ʽ��־ΪGSM�µ�AMR����֡ */
    if (   (CODEC_AMR == pstUpdateMsg->enCodecType)
        || (CODEC_AMRWB == pstUpdateMsg->enCodecType))
    {
        pstEncInObj->enAmrMode      = pstUpdateMsg->enAmrMode;
        pstEncInObj->enAmrNsync     = pstUpdateMsg->enAmrNsync;
    }

    pstEncInObj->enAmrFormat    = CODEC_AMR_FORMAT_BITS;

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_ProcCfgImsCodec(VOS_UINT16 uhwCodecType,
                                         VOS_UINT16 uhwDtxEnable,
                                         VOS_UINT16 uhwRateMode)
{
    VOICE_PROC_CODEC_STRU       *pstCodec    = VOICE_ProcGetCodecPtr();

    /* �������Ϸ��Լ�� */
    if (uhwCodecType != pstCodec->enCodecType)
    {
        return UCOM_RET_FAIL;
    }

    if(CODEC_G711 != pstCodec->enCodecType)
    {
        /* DTX���ͼ��,��DTX���͸�������Ҫͬʱ���³�ʼ�������� */
        if (pstCodec->stEncInObj.enDtxMode != uhwDtxEnable)
        {
            pstCodec->usIsEncInited         = VOICE_PROC_IS_NO;
            pstCodec->stEncInObj.enDtxMode  = uhwDtxEnable;
        }

        /* ��������ģʽ */
        pstCodec->stEncInObj.enAmrMode      = uhwRateMode;

        /* ������ʽ��־ΪIMS������֡��ʽ */
        pstCodec->stEncInObj.enAmrFormat    = CODEC_AMR_FORMAT_IMS;
        pstCodec->stDecInObj.enAmrFormat    = CODEC_AMR_FORMAT_IMS;
    }

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 �� �� ��  : VOICE_ProcEncode
 ��������  : �������������͵�����Ӧ�ı��������б��봦��
 �������  : psPcm       - ������PCM����
 �������  : psStrm      - �����������
 �� �� ֵ  : VOS_UINT32  -����ֵ����Χ���VOICE_RET_ENUM��������ֵ����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��1��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_UINT32 VOICE_ProcEncode(VOS_INT16 *psPcm, VOS_INT16 *psStrm)
{
    VOICE_PROC_CODEC_STRU          *pstCodec    = VOICE_ProcGetCodecPtr();
    VOS_UINT32                      uwRet       = UCOM_RET_SUCC;
    IMS_UL_DATA_OBJ_STRU           *pstImsUlObj;
    CODEC_ENC_IN_PARA_STRU         *pstEncInPara;
    CODEC_ENC_OUT_PARA_STRU        *pstEncOutPara;

    /* �����Ϸ��Լ�� */
    if ((VOS_NULL == psPcm) || (VOS_NULL == psStrm))
    {
        return UCOM_RET_NUL_PTR;
    }

    pstCodec->stEncInObj.pshwEncSpeech  = psPcm;
    pstCodec->stEncOutObj.pshwEncSerial = psStrm;

    /* ����������ж�֡��������1 */
    VOICE_AddRxLostFrameCnt(1);

    //��������δ��ʼ�����ʼ��������
    if (VOICE_PROC_IS_YES != pstCodec->usIsEncInited)
    {
        if (UCOM_RET_SUCC == VOICE_ProcInitEncoder(pstCodec->enCodecType,
            pstCodec->stEncInObj.enDtxMode))
        {
            pstCodec->usIsEncInited = VOICE_PROC_IS_YES;
        }
        else
        {
            return UCOM_RET_FAIL;
        }
    }

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_ENCODE);

    /* �������������͵�����Ӧ�ı��������б��봦�� */
    switch(pstCodec->enCodecType)
    {
        case CODEC_AMR:
        {
            uwRet = AMR_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_EFR:
        {
            uwRet = EFR_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_FR:
        {
            uwRet = FR_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_HR:
        {
            uwRet = HR_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_AMRWB:
        {
            uwRet = AMRWB_Encode(&(pstCodec->stEncInObj), &(pstCodec->stEncOutObj));
            break;
        }
        case CODEC_G711:
        {
            /* G711����Ҫ����,ֱ���ϴ�PCM���� */
            pstEncInPara  = &(pstCodec->stEncInObj);
            pstEncOutPara = &(pstCodec->stEncOutObj);
            pstImsUlObj   = (IMS_UL_DATA_OBJ_STRU*)pstEncOutPara->pshwEncSerial;
            pstImsUlObj->usStatus    = 0;
            pstImsUlObj->usFrameType = 0;
            UCOM_MemCpy(&(pstImsUlObj->ausData[0]),pstEncInPara->pshwEncSpeech,IMSA_CODEC_MAX_DATA_LEN*sizeof(MED_UINT16));
            break;
        }
        default:
        {
            /* �ڲ���֤������뱾��֧ */
            return UCOM_RET_ERR_PARA;
        }
    }

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_ENCODE);

    return uwRet;
}

/*****************************************************************************
 �� �� ��  : VOICE_ProcDecode
 ��������  : �������������͵�����Ӧ�Ľ��������н��봦��
 �������  : psStrm      - ����������
 �������  : psPcm       - �������PCM����
 �� �� ֵ  : VOS_UINT32  -����ֵ����Χ���VOICE_RET_ENUM��������ֵ����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��1��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_UINT32 VOICE_ProcDecode(VOS_INT16 *psStrm, VOS_INT16 *psPcm)
{
    VOICE_PROC_CODEC_STRU          *pstCodec         = VOICE_ProcGetCodecPtr();
    VOS_UINT32                      uwRet            = UCOM_RET_SUCC;
    VOS_INT16                       shwPosTmp;
    VOS_UINT16                      uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                      uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    IMS_DL_DATA_OBJ_STRU           *pstImsDlObj;
    CODEC_DEC_IN_PARA_STRU         *pstDecInPara;
    /* �����Ϸ��Լ�� */
    if ((VOS_NULL == psPcm) || (VOS_NULL == psStrm))
    {
        return UCOM_RET_NUL_PTR;
    }

    pstCodec->stDecInObj.pshwDecSerial = (VOS_INT16*)psStrm;

    /* ��������δ��ʼ�����ʼ�������� */
    if ((  VOICE_GetRxLostFrameCnt() > VOICE_RX_LOST_FRAME_THD)
        ||(VOICE_PROC_IS_YES != pstCodec->usIsDecInited))
    {
        if (UCOM_RET_SUCC == VOICE_ProcInitDecoder(pstCodec->enCodecType))
        {
            pstCodec->usIsDecInited = VOICE_PROC_IS_YES;
        }
        else
        {
            return UCOM_RET_FAIL;
        }
    }

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_DECODE);

    /* GSM����Ҫ���л�֡�����͵֡���� */
    if(CODEC_NET_MODE_G == uhwNetMode)
    {
        /* BFI���� */
        uwRet = VOICE_ProcBFI(pstCodec);

        /* ��BFI����Ϊ��0����������ֱ�ӷ��� */
        if (UCOM_RET_ERR_STATE == uwRet)
        {
            UCOM_MemSet(psPcm, 0, (VOS_UINT32)((VOS_UINT16)CODEC_PCM_FRAME_LENGTH * sizeof(VOS_INT16)));

            return UCOM_RET_SUCC;
        }

        /* GSM��͵֡���⴦�� */
        uwRet = VOICE_ProcFacch(psPcm);

        /* ����͵֡�������н��룬ֱ�ӷ��سɹ� */
        if(UCOM_RET_ERR_STATE == uwRet)
        {
            return UCOM_RET_SUCC;
        }
    }

    /* �������������͵�����Ӧ�Ľ��������н��봦�� */
    switch(pstCodec->enCodecType)
    {
        case CODEC_AMR:
        {
            uwRet = AMR_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_EFR:
        {
            uwRet = EFR_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_FR:
        {
            uwRet = FR_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_HR:
        {
            uwRet = HR_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_AMRWB:
        {
            uwRet = AMRWB_Decode(&(pstCodec->stDecInObj), psPcm);
            break;
        }
        case CODEC_G711:
        {
            /* G711����Ҫ����,ֱ�����lineIn */
            pstDecInPara = &(pstCodec->stDecInObj);
            pstImsDlObj = (IMS_DL_DATA_OBJ_STRU *)pstDecInPara->pshwDecSerial;

            /* error����bad֡�����0 */
            if ((IMSA_VOICE_ERROR == (pstImsDlObj->usErrorFlag)) || (CODEC_AMR_FQI_QUALITY_BAD == (pstImsDlObj->usQualityIdx)))
            {
                UCOM_MemSet(psPcm,0,CODEC_PCM_MAX_FRAME_LENGTH);
            }
            else
            {
                UCOM_MemCpy(psPcm,
                            &(pstImsDlObj->ausData[0]),
                            (IMSA_CODEC_MAX_DATA_LEN * sizeof(MED_UINT16)));
            }

            break;
        }
        default:
        {
            /* �ڲ���֤������뱾��֧ */
            return UCOM_RET_ERR_PARA;
        }
    } //end for switch

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_DECODE);

    VOICE_SetRxLostFrameCnt(0);

    /* ���汾֡ʱ���źŷ��ȵ�������ֵ */
    VOICE_ProcSetMaxFrameRange(CODEC_OpVecMaxAbs(psPcm, CODEC_PCM_FRAME_LENGTH, &shwPosTmp));

    return uwRet;
}
#if 0
/*****************************************************************************
 �� �� ��  : VOICE_ProcConfigSound
 ��������  : ����SOUND�ϳ���
 �������  : usEnable    - SOUND�ϳ���ʹ��, CODEC_SWITCH_ON/CODEC_SWITCH_OFF
             pstSndCfg   - SOUND�ϳ������ò���, usEnableΪCODEC_SWITCH_OFF���ӱ�����
 �������  : ��
 �� �� ֵ  : VOS_UINT32  -����ֵ����Χ���VOICE_RET_ENUM��������ֵ����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��1��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_UINT32 VOICE_ProcConfigSound(
                VOS_UINT16                                  enEnable,
                VOICE_PROC_SND_CFG_STRU                 *pstSndCfg)
{
    VOS_UINT32                          uwRet      = UCOM_RET_SUCC;
    VOICE_PROC_SOUND_STRU           *pstSound   = &g_stVoiceProcObjs.stSound;

    if ( CODEC_SWITCH_ON == enEnable )
    {
        if ( VOS_NULL == pstSndCfg )
        {
            return UCOM_RET_NUL_PTR;
        }

        if ( VOS_NULL == pstSndCfg->funcSndInd )
        {
            return UCOM_RET_ERR_PARA;
        }

        //����Soundʹ�ܱ�־
        switch (pstSndCfg->enTxRx)
        {
            case VOICE_TXRX_TX:
            {
                pstSound->enTxEnable = CODEC_SWITCH_ON;
                pstSound->enRxEnable = CODEC_SWITCH_OFF;
                break;
            }
            case VOICE_TXRX_RX:
            {
                pstSound->enTxEnable = CODEC_SWITCH_OFF;
                pstSound->enRxEnable = CODEC_SWITCH_ON;
                break;
            }
            case VOICE_TXRX_TXRX:
            {
                pstSound->enTxEnable = CODEC_SWITCH_ON;
                pstSound->enRxEnable = CODEC_SWITCH_ON;
                break;
            }
            default:
            {
                return UCOM_RET_ERR_PARA;
            }
        }

        //��������ֵ
        pstSound->sSndTxVol = pstSndCfg->sTxVolume;
        pstSound->sSndRxVol = pstSndCfg->sRxVolume;

        //���ûص�����
        pstSound->funcSndInd = pstSndCfg->funcSndInd;

        //��λԤֹͣ��־
        pstSound->usPreDisable = CODEC_SWITCH_OFF;

        //��ʼ��Sound�ϳ���
        #if (_FEATURE_VOICE_SOUND == FEATURE_ON)
        uwRet = VoiceCodecSoundInit(pstSndCfg->usSndId, pstSndCfg->usRptCnt);
        #endif

    }
    else if ( CODEC_SWITCH_OFF == enEnable )
    {
        if ( (CODEC_SWITCH_ON == pstSound->enRxEnable)
            || (CODEC_SWITCH_ON == pstSound->enTxEnable) )
        {
            //����Soundʹ�ܱ�־
            pstSound->usPreDisable = CODEC_SWITCH_ON;
        }
    }
    else
    {
        //�����������
        return UCOM_RET_ERR_PARA;
    }

    if (UCOM_RET_SUCC == uwRet)
    {
        OM_LogInfo(VOICE_ProcConfigSound_Ok);
    }

    return uwRet;
}

/*****************************************************************************
 �� �� ��  : VOICE_ProcGenerateSound
 ��������  : �ϳ�SOUND����
 �������  : psSound - SOUND�ϳ��������������
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��7��1��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_VOID VOICE_ProcGenerateSound(VOS_INT16 *psSound)
{
    VOS_UINT32                uwRet    = 0;
    VOICE_PROC_SOUND_STRU *pstSound = &g_stVoiceProcObjs.stSound;

    #if (_FEATURE_VOICE_SOUND == FEATURE_ON)
    uwRet = VoiceCodecSoundPlay(psSound);
    #endif
    if ( (UCOM_RET_SUCC != uwRet) || (CODEC_SWITCH_ON == pstSound->usPreDisable) )
    {
        //���ºϳ���ʹ��
        pstSound->enTxEnable = CODEC_SWITCH_OFF;
        pstSound->enRxEnable = CODEC_SWITCH_OFF;

        //Ԥֹͣ��־��λ
        pstSound->usPreDisable = CODEC_SWITCH_OFF;

        //�ϱ����Ž���
        if ( pstSound->funcSndInd != VOS_NULL )
        {
            pstSound->funcSndInd();
        }

        //�ϱ���־
        OM_LogInfo(VOICE_ProcGenerateSound_SoundEnd);
    }
}
#endif


VOS_INT16* VOICE_ProcGetEcRef( VOS_VOID )
{
    VOS_INT16      *pshwEcRef;
    VOS_UINT32      uwSpaI2SVirAddr;

    /* ȷ��EC REF��ָ�� */
    if (   (VCVOICE_DEV_MODE_HANDFREE == VOICE_McGetDeviceMode())
        && (VOICE_SMART_PA_EN_ENABLE  == VOICE_McGetSmartPaEn()))
    {
        uwSpaI2SVirAddr = UCOM_PCM_SmartPaGetAvailVirtAddr();

        VOICE_PcmHybrid2Stereo((VOS_INT16 *)uwSpaI2SVirAddr,
                               VOICE_PcmGetEcRefLeftChanPtr(),
                               VOICE_PcmGetEcRefRightChanPtr(),
                               CODEC_PCM_FRAME_LENGTH);

        /* �ж�����I2S���ݣ���������Ч������������������Ч */
        if (VOICE_SMART_PA_L_CHAN_VALID == VOICE_McGetSmartPaValidChan())
        {
            pshwEcRef = VOICE_PcmGetEcRefLeftChanPtr();
        }
        else
        {
            pshwEcRef = VOICE_PcmGetEcRefRightChanPtr();
        }

        OM_LogInfo(VOICE_ProcGetEcRef_GetSmartPADataOk);
    }
    else
    {
        /* ��ȡ������ѧ�����Ժ������ */
        pshwEcRef = VOICE_PcmGetSpkOutBufTempPtr();
    }

    return pshwEcRef;

}
VOS_UINT32 VOICE_ProcTxPp(
                VOS_INT16   *pshwMcIn,
                VOS_INT16   *pshwRcIn,
                VOS_INT16   *pshwLineOut)
{
    /* ��ȡ��˫��˱�־ */
    VOS_UINT32                          uwRet;
    VOICE_MC_MODEM_STRU                *pstForeGroundObj = VOICE_McGetForeGroundObj();
    VOS_INT16                          *pshwEcRef;
    MLIB_SOURCE_STRU                    stSourceIn, stSourceOut;
    VOS_UINT32                          uwSampleRate;
    VOS_UINT32                          uwFrameSize;

    /* ����ж� */
    if (   (VOS_NULL == pshwMcIn)
        || (VOS_NULL == pshwRcIn)
        || (VOS_NULL == pshwLineOut)
        || (VOS_NULL == pstForeGroundObj))
    {
        return UCOM_RET_NUL_PTR;
    }

    /* �����������Ϣ��0 */
    UCOM_MemSet(&stSourceIn,  0, sizeof(MLIB_SOURCE_STRU));
    UCOM_MemSet(&stSourceOut, 0, sizeof(MLIB_SOURCE_STRU));

    pshwEcRef       = VOICE_ProcGetEcRef();
    uwSampleRate    = VOICE_ProcGetSampleRate();
    uwFrameSize     = (VOS_UINT32)((VOS_UINT16)(CODEC_PcmGetPcmFrameLength())*sizeof(VOS_INT16));

    /* ��������\������� */
    stSourceIn.uwChnNum = 3;
    MLIB_UtilitySetSource(&stSourceIn,  0, pshwMcIn,  uwFrameSize,  uwSampleRate);
    MLIB_UtilitySetSource(&stSourceIn,  1, pshwRcIn,  uwFrameSize,  uwSampleRate);
    MLIB_UtilitySetSource(&stSourceIn,  2, pshwEcRef, uwFrameSize,  uwSampleRate);

    stSourceOut.uwChnNum = 3;
    MLIB_UtilitySetSource(&stSourceOut, 0, pshwLineOut, uwFrameSize, uwSampleRate);
    MLIB_UtilitySetSource(&stSourceOut, 1, pshwRcIn,  uwFrameSize,  uwSampleRate);
    MLIB_UtilitySetSource(&stSourceOut, 2, pshwEcRef, uwFrameSize,  uwSampleRate);

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_PROC_MICIN);

    /* ������ѧ���� */
    uwRet = MLIB_PathProc(
        MLIB_PATH_CS_VOICE_CALL_MICIN,
        &stSourceOut,
        &stSourceIn);

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_PROC_MICIN);

    return uwRet;

}
VOS_UINT32 VOICE_ProcRxPp(
                VOS_INT16   *pshwLineIn,
                VOS_INT16   *pshwSpkOut)
{
    VOS_UINT32                          uwRet          = 0;
    VOICE_MC_MODEM_STRU                *pstForeGroundObj = VOICE_McGetForeGroundObj();    /* ����ж� */
    MLIB_SOURCE_STRU                    stSourceIn, stSourceOut;
    VOS_UINT32                          uwSampleRate;
    VOS_UINT32                          uwFrameSize;

    if (   (VOS_NULL == pshwSpkOut)
        || (VOS_NULL == pshwLineIn)
        || (VOS_NULL == pstForeGroundObj))
    {
        return UCOM_RET_NUL_PTR;
    }

    /* �����������Ϣ��0 */
    UCOM_MemSet(&stSourceIn,  0, sizeof(MLIB_SOURCE_STRU));
    UCOM_MemSet(&stSourceOut, 0, sizeof(MLIB_SOURCE_STRU));

    uwSampleRate    = VOICE_ProcGetSampleRate();
    uwFrameSize     = (VOS_UINT32)((VOS_UINT16)(CODEC_PcmGetPcmFrameLength())*sizeof(VOS_INT16));

    /* ��������\������� */
    stSourceIn.uwChnNum = 1;
    MLIB_UtilitySetSource(&stSourceIn,  0, pshwLineIn, uwFrameSize,  uwSampleRate);

    stSourceOut.uwChnNum = 1;
    MLIB_UtilitySetSource(&stSourceOut, 0, pshwSpkOut, uwFrameSize, uwSampleRate);

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_PROC_SPKOUT);

    /* ������ѧ���� */
    uwRet = MLIB_PathProc(
        MLIB_PATH_CS_VOICE_CALL_SPKOUT,
        &stSourceOut,
        &stSourceIn);

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_VOICE_PROC_SPKOUT);

    return uwRet;
}


VOS_VOID VOICE_ProcInitDecSerialWrite( VOS_VOID )
{
    VOICE_DEC_SERIAL_STRU       *pstDecBuf       = VOICE_ProcGetDecBufPtr();

    /* ��ս��뻺���е����� */
    UCOM_MemSet(pstDecBuf, 0, VOICE_CODED_DECODE_BUF_FRAME_NUM * sizeof(VOICE_DEC_SERIAL_STRU));

    /* ��ʼ�����뻺��дָ�� */
    VOICE_ProcInitDecSerialWritePtr();
}


VOS_VOID VOICE_ProcSubDecSerialWrite( VOS_UINT16 uhwRxFrmLost)
{
    VOICE_DEC_SERIAL_STRU          *pstDecBuf       = VOICE_ProcGetDecBufPtr();
    VOICE_DEC_SERIAL_STRU          *pstDecSerialWrite;

    /* ����û�ж�֡ */
    if(VOICE_PROC_IS_NO == uhwRxFrmLost)
    {
        /* ʣ�������ƶ���buff�׵�ַ */
        VOICE_ProcSubDecSerialWritePtr();

        pstDecSerialWrite = VOICE_ProcGetDecSerialWritePtr();

        if(pstDecSerialWrite != pstDecBuf)
        {
            UCOM_MemCpy(pstDecBuf,
                        pstDecSerialWrite,
                        sizeof(VOICE_DEC_SERIAL_STRU));
        }

    }
}


VOS_VOID VOICE_ProcAddDecSerialWrite(VOS_VOID *pvMsg, VOS_UINT32 uwLenBytes)
{
    VOICE_DEC_SERIAL_STRU       *pstDecSerialWrite= VOICE_ProcGetDecSerialWritePtr();
    VOICE_DEC_SERIAL_STRU       *pstDecBuf        = VOICE_ProcGetDecBufPtr();

    /* �������ݻ������� */
    if(pstDecSerialWrite >= (pstDecBuf + VOICE_CODED_DECODE_BUF_FRAME_NUM))
    {
        OM_LogWarning(VOICE_ProcAddDecSerialWrite_RemoveExpiredDecSerial);

        /* ��ԭ�е�������ǰPOPһ֡ */
        VOICE_ProcSubDecSerialWrite(VOICE_PROC_IS_NO);

        /* ���»������дָ�� */
        pstDecSerialWrite = VOICE_ProcGetDecSerialWritePtr();
    }

    /* �����н������ݿ�����뻺���� */
    UCOM_MemCpy((VOS_VOID *)pstDecSerialWrite->asDecSerial,
                pvMsg,
                uwLenBytes);

    pstDecSerialWrite->uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* ָ����һ���������ݴ洢��Ԫ */
    VOICE_ProcAddDecSerialWritePtr();
}
VOS_UINT32 VOICE_ProcCfgDlCodec(
                CODEC_NET_MODE_ENUM_UINT16         enNetMode,
                VOS_INT16                         *pshwDecBuf,
                VOS_UINT16                         uhwRxFrmLost,
                VOS_INT16                        **pshwDecData )
{
    VOS_UINT32                    uwRet         = UCOM_RET_SUCC;
    GPHY_VOICE_RX_DATA_IND_STRU  *pstGsmRxData  = VOS_NULL;
    WPHY_VOICE_RX_DATA_IND_STRU  *pstUmtsRxData = VOS_NULL;
    TDPHY_VOICE_RX_DATA_IND_STRU *pstTdRxData   = VOS_NULL;
    IMS_DL_DATA_OBJ_STRU         *pstImsDLData  = VOS_NULL;
    VOICE_PROC_CODEC_STRU        *pstCodec        = VOICE_ProcGetCodecPtr();
    VOICE_DIAG_RX_DATA_STRU       stDiagRxData;
    VOICE_MC_MODEM_NUM_ENUM_UINT16 enActiveModemNo = VOICE_McGetForeGroundNum(); /* ǰ̨ͨ���� */

    UCOM_MemSet(&stDiagRxData, 0, sizeof(VOICE_DIAG_RX_DATA_STRU));

    switch(enNetMode)
    {
        case CODEC_NET_MODE_G:
        {
            pstGsmRxData = (GPHY_VOICE_RX_DATA_IND_STRU *)pshwDecBuf;

            /* ���ж�֡����ΪBFI֡ */
            if(VOICE_PROC_IS_YES == uhwRxFrmLost)
            {
                pstGsmRxData->enBfi = CODEC_BFI_YES;
                pstGsmRxData->enAmrFrameType = CODEC_AMR_TYPE_RX_NO_DATA;
            }

            /* GSM�����ý������ */
            uwRet = VOICE_ProcCfgGsmDlCodec(pstGsmRxData);

            if (UCOM_RET_SUCC != uwRet)
            {
                return uwRet;
            }

            /* ��ȡG�´��������ݵ��׵�ַ */
            *pshwDecData = (VOS_INT16 *)pstGsmRxData->puhwData;

            /* ��������֡���͵������Ϣ�����ڵ�ͨ��� */
            stDiagRxData.enNetMode      = CODEC_NET_MODE_G;
            stDiagRxData.enCodecType    = pstGsmRxData->enCodecType;
            stDiagRxData.enBfi          = pstGsmRxData->enBfi;
            stDiagRxData.enSid          = pstGsmRxData->enSid;
            stDiagRxData.enAmrFrameType = pstGsmRxData->enAmrFrameType;

            break;
        }

        case CODEC_NET_MODE_W:
        {
            pstUmtsRxData = (WPHY_VOICE_RX_DATA_IND_STRU *)pshwDecBuf;

            /* ���ж�֡����ΪBAD֡ */
            if(VOICE_PROC_IS_YES == uhwRxFrmLost)
            {
                pstUmtsRxData->enQualityIdx = CODEC_AMR_FQI_QUALITY_BAD;

                /* ��A/B/C��������Ϊ0 */
                UCOM_MemSet(pstUmtsRxData->auhwLen, 0, (VOS_UINT32)((VOS_UINT16)WPHY_VOICE_AMR_SUBFRAME_NUM * sizeof(VOS_INT16)));
            }

            /* ����VOICE_ProcCfgUmtsDlCodec����UMTS���н���������� */
            uwRet = VOICE_ProcCfgUmtsDlCodec();

            if (uwRet == UCOM_RET_FAIL)
            {
                return UCOM_RET_FAIL;
            }

            /* ��ȡW�´��������ݵ��׵�ַ */
            *pshwDecData = (VOS_INT16 *)&pstUmtsRxData->enQualityIdx;

            /* ��������֡���͵������Ϣ�����ڵ�ͨ��� */
            stDiagRxData.enNetMode      = CODEC_NET_MODE_W;
            stDiagRxData.enQualityIdx   = pstUmtsRxData->enQualityIdx;

            break;
        }

        case CODEC_NET_MODE_TD:
        {
            pstTdRxData = (TDPHY_VOICE_RX_DATA_IND_STRU *)pshwDecBuf;

            /* ���ж�֡����ΪBAD֡ */
            if(VOICE_PROC_IS_YES == uhwRxFrmLost)
            {
                pstTdRxData->enQualityIdx = CODEC_AMR_FQI_QUALITY_BAD;

                /* ��A/B/C��������Ϊ0 */
                UCOM_MemSet(pstTdRxData->auhwLen, 0, (VOS_UINT32)((VOS_UINT16)WPHY_VOICE_AMR_SUBFRAME_NUM * sizeof(VOS_INT16)));
            }

            /* ����VOICE_ProcCfgUmtsDlCodec����TD���н���������� */
            uwRet = VOICE_ProcCfgUmtsDlCodec();

            if (uwRet == UCOM_RET_FAIL)
            {
                return UCOM_RET_FAIL;
            }

            /* ��ȡTD�´��������ݵ��׵�ַ */
            *pshwDecData = (VOS_INT16 *)&pstTdRxData->enQualityIdx;

            /* ��������֡���͵������Ϣ�����ڵ�ͨ��� */
            stDiagRxData.enNetMode      = CODEC_NET_MODE_TD;
            stDiagRxData.enQualityIdx   = pstTdRxData->enQualityIdx;

            break;
        }

        case CODEC_NET_MODE_L:
        {
            *pshwDecData = pshwDecBuf;

            /* ��������֡���͵������Ϣ�����ڵ�ͨ��� */
            pstImsDLData = (IMS_DL_DATA_OBJ_STRU*)pshwDecBuf;
            stDiagRxData.enNetMode      = CODEC_NET_MODE_L;
            stDiagRxData.enQualityIdx   = (CODEC_AMR_FQI_QUALITY_ENUM_UINT16)pstImsDLData->usQualityIdx;

            break;
        }
        default:
        {
            OM_LogError(VOICE_ProcCfgDlCodec_UnkownMode);
            return UCOM_RET_FAIL;
        }
    }


    /* ����֡���ͼ�����������Ƿ�ͨ����� */
    if(pstCodec->enCodecType != CODEC_G711)
    {
        stDiagRxData.enDevMode      = VOICE_McGetDeviceMode();
        stDiagRxData.usIsDecInited  = pstCodec->usIsDecInited;
        stDiagRxData.usIsEncInited  = pstCodec->usIsEncInited;

        VOICE_DiagLineIn(enActiveModemNo, &stDiagRxData);
    }


    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_ProcCfgUmtsDlCodec( VOS_VOID )
{
    VOICE_PROC_CODEC_STRU     *pstCodec   = VOICE_ProcGetCodecPtr();

    /* ������������ͺϷ��� */
    if ( (CODEC_AMR != pstCodec->enCodecType)
        &&(CODEC_AMR2 != pstCodec->enCodecType)
        &&(CODEC_AMRWB != pstCodec->enCodecType))
    {
        OM_LogError(VOICE_CfgUmtsDlCodecError);
        return UCOM_RET_FAIL;
    }

    /* ������ʽ��־ΪUMTS������֡��ʽ */
    pstCodec->stDecInObj.enAmrFormat    = CODEC_AMR_FORMAT_IF1;

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcCfgUmtsUlCodec( VOS_VOID )
{
    VOICE_PROC_CODEC_STRU  *pstCodec = VOICE_ProcGetCodecPtr();

    /* ���������ͼ��,�����������͸�������Ҫͬʱ���³�ʼ�������� */
    if ( (CODEC_AMR != pstCodec->enCodecType)
        &&(CODEC_AMR2 != pstCodec->enCodecType)
        &&(CODEC_AMRWB != pstCodec->enCodecType))
    {
        OM_LogError(VOICE_CfgUmtsUlCodecError);
        return UCOM_RET_FAIL;
    }

    /* DTX���ͼ��,��DTX���͸�������Ҫͬʱ���³�ʼ�������� */
    if (pstCodec->stEncInObj.enDtxMode != (VOS_INT16)VOICE_AmrModeCtrlGetDtx())
    {
        pstCodec->usIsEncInited         = VOICE_PROC_IS_NO;
        pstCodec->stEncInObj.enDtxMode  = VOICE_AmrModeCtrlGetDtx();
    }

    /* ��������ģʽ */
    pstCodec->stEncInObj.enAmrMode      = VOICE_AmrModeCtrlChangeMode();

    /* ������ʽ��־ΪUMTS������֡��ʽ */
    pstCodec->stEncInObj.enAmrFormat    = CODEC_AMR_FORMAT_IF1;

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcFrameTypeToRlc(VOS_UINT16 uhwFrameTypeIdx)
{
    WTTFVOICE_MAC_AMR_DATA_STRU     stAmr;
    VOICE_PROC_CODEC_STRU          *pstCodec    = VOICE_ProcGetCodecPtr();


    UCOM_MemSet(&stAmr, 0, sizeof(stAmr));

    stAmr.enFrameType    = uhwFrameTypeIdx;

    if (CODEC_AMRWB == pstCodec->enCodecType)
    {
        stAmr.enCodecType = WTTFVOICE_AMR_BANDWIDTH_TYPE_WB;

        /* ����ǰ��������Ϊ�����UMTSä����������ͣ����TTF���ⲿ֡������Ҫ�ı� */
        if ((WTTFVOICE_AMRWB_CODECMODE_TYPE_1585K_B == pstCodec->stEncInObj.enAmrMode)
            &&(CODEC_AMRWB_FRM_TYPE_1585K  == uhwFrameTypeIdx))
        {
            stAmr.enFrameType    = WTTFVOICE_AMRWB_FRAME_TYPE_1585K_B;
        }
        else if ((WTTFVOICE_AMRWB_CODECMODE_TYPE_2385K_B == pstCodec->stEncInObj.enAmrMode)
                &&(CODEC_AMRWB_FRM_TYPE_2385K  == uhwFrameTypeIdx))

        {
            stAmr.enFrameType    = WTTFVOICE_AMRWB_FRAME_TYPE_2385K_B;
        }
        else
        {
            /* for pc-lint */
        }
    }
    else
    {
        stAmr.enCodecType = WTTFVOICE_AMR_BANDWIDTH_TYPE_NB;
    }

    stAmr.enMsgName      = ID_VOICE_MAC_AMR_DATA_REQ;
    stAmr.usMuteFlag     = 1;                                                 /*���д��MuteFlag*/

    /* ����ͨ��VOS���ͽӿڣ�������Ϣ */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_WTTF,
                    &stAmr,
                    sizeof(stAmr));

    return UCOM_RET_SUCC;
}



VOS_UINT32 VOICE_ProcOutputFrameType(VOS_UINT16 uhwFrameTypeIdx)
{
    VOS_UINT32              uwRet;

    /* R99�½���֡����֡����ͨ��AHB��ǰ֪ͨMAC����������� */
    uwRet = VOICE_ProcFrameTypeToRlc(uhwFrameTypeIdx);

    /* ����VOICE_McChkMicConflick������UMTS������ʱ���ͻ��� */
    VOICE_McChkMicConflick();

    return uwRet;
}


VOS_UINT32 VOICE_ProcVolume(
                VOS_INT16               shwVolumeDb,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{
    VOS_INT16              shwFrameLength =  CODEC_PcmGetPcmFrameLength();

    /* �ж������ĺϷ��� */
    if ((shwVolumeDb > VOICE_PROC_GAIN_MAX) || (shwVolumeDb < VOICE_PROC_GAIN_MIN))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* �������棬���У�MED_GAIN_Db2Linear(shwVolumeDb)�Ľ��ΪQ13 */
    CODEC_OpVcMultScale(pshwIn,
                      (VOS_INT32)shwFrameLength,
                      MED_GAIN_Db2Linear(shwVolumeDb),
                      MED_PP_SHIFT_BY_14,
                      pshwOut);

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcFacch(VOS_INT16   *psPcm)
{
    VOICE_PROC_CODEC_STRU     *pstCodec = VOICE_ProcGetCodecPtr();
    VOS_INT16                  shwMaxRandRange;

    shwMaxRandRange = VOICE_ProcGetMaxFrameRange();

    /* ͵֡���� */
    /* 1. ������͵֡ʱ����ӽ�����֡��С�����������������ֱ�ӷ��� */
    if (VOICE_ProcGetFacchFrameCnt() > VOICE_PROC_GSM_RX_FACCH_THD)
    {
        VOICE_PcmInsertCn(psPcm, CODEC_PCM_FRAME_LENGTH, shwMaxRandRange);

        return UCOM_RET_ERR_STATE;
    }
    /* 2. ������1-2֡͵֡��������ǰ������ʹ����֡�����ݽ��и��� */
    else if (VOICE_ProcGetFacchFrameCnt() > 0)
    {
        UCOM_MemCpy(&pstCodec->stDecInObj,
                    VOICE_ProcGetPreDecInObj(),
                    sizeof(CODEC_DEC_PREIN_PARA_STRU));

        UCOM_MemCpy(pstCodec->stDecInObj.pshwDecSerial,
                    VOICE_ProcGetPrevDecDataPtr(),
                    GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));

        /* ע: ����G-FR/EFR/HR/AMR/AMR2/AMRWB�������ߵ��˷�֧������ͬʱ��AMR��BFI�ı�־�����޸� */
        pstCodec->stDecInObj.enBfiFlag      = CODEC_BFI_YES;
        pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_NO_DATA;
    }
    /* 3. ����͵֡֡������֡���ݱ������� */
    else
    {
        /* �洢��֡����ǰ������ */
        UCOM_MemCpy(VOICE_ProcGetPreDecInObj(),
                    &pstCodec->stDecInObj,
                    sizeof(CODEC_DEC_PREIN_PARA_STRU));

        UCOM_MemCpy(VOICE_ProcGetPrevDecDataPtr(),
                    pstCodec->stDecInObj.pshwDecSerial,
                    GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_ProcBFI(VOICE_PROC_CODEC_STRU  *pstCodec)
{
    VOS_UINT32  uwRet     = UCOM_RET_SUCC;

    /* ��ΪAMR�������򷵻� */
    if ((CODEC_AMR   == pstCodec->enCodecType)
      ||(CODEC_AMR2  == pstCodec->enCodecType)
      ||(CODEC_AMRWB == pstCodec->enCodecType))
    {
        return uwRet;
    }

    /* ����֡״̬��������һ֡״̬ */
    if ((CODEC_BFI_NO == pstCodec->stDecInObj.enBfiFlag)
      && (0 == VOICE_ProcGetFacchFrameCnt()))
    {
        pstCodec->stDecInObj.enGoodFrameType
        = (pstCodec->stDecInObj.enSidFlag == CODEC_SID_SPEECH)? CODEC_PREFRAME_SPEECH: CODEC_PREFRAME_SID;

        pstCodec->stDecInObj.uhwFrameBfiCnt  = 0;

        /* �洢��֡����ǰ������ */
        UCOM_MemCpy(VOICE_ProcGetPreDecInObj(),
                    &pstCodec->stDecInObj,
                    sizeof(CODEC_DEC_PREIN_PARA_STRU));

        UCOM_MemCpy(VOICE_ProcGetPrevDecDataPtr(),
                    pstCodec->stDecInObj.pshwDecSerial,
                    GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));
    }
    /* ����ǰ֡ΪBFI֡�����ǰһ֡֡���ͽ��д��� */
    else
    {
        /* ����ǰһ֡����Ա�֡��0 */
        if (CODEC_PREFRAME_NONE == pstCodec->stDecInObj.enGoodFrameType)
        {
            return UCOM_RET_ERR_STATE;
        }
        /* ������ǰ��֡BFI֡����SID�����ظ���һ֡ */
        else if (CODEC_PREFRAME_SPEECH == pstCodec->stDecInObj.enGoodFrameType)
        {
            pstCodec->stDecInObj.uhwFrameBfiCnt = pstCodec->stDecInObj.uhwFrameBfiCnt + 1;

            if(pstCodec->stDecInObj.uhwFrameBfiCnt <= VOICE_PROC_BFI_MAX)
            {
                UCOM_MemCpy(&pstCodec->stDecInObj,
                            VOICE_ProcGetPreDecInObj(),
                            sizeof(CODEC_DEC_PREIN_PARA_STRU));

                UCOM_MemCpy(pstCodec->stDecInObj.pshwDecSerial,
                            VOICE_ProcGetPrevDecDataPtr(),
                            GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));

                /* ע: ��Ϊֻ��G-FR/EFR/HR���ߵ��˷�֧������ֻ��BFI�ı�־��ΪYES */
                pstCodec->stDecInObj.enBfiFlag      = CODEC_BFI_YES;
            }
            else
            {
                return UCOM_RET_ERR_STATE;
            }
        }
        else
        {
            //pstDecInObj->enGoodFrameType == CODEC_PREFRAME_SID
            //for lint
        }
    }

    return uwRet;
}
MED_UINT16 VOICE_ProcCheckUpdate( VOS_UINT16 uhwCodecType  )
{
    VOS_INT16                           shwOldFrameLength;
    VOS_INT16                           shwNewFrameLength;
    VOS_UINT16                          ushwRet = VOICE_PROC_IS_NO;

    /* ��ȡ����ǰ��֡���������ж��Ƿ��ǿ�խ���л� */
    shwOldFrameLength = CODEC_PcmGetPcmFrameLength();

    /* ����set_codec֮���֡�� */
    if((CODEC_AMRWB == uhwCodecType) || (CODEC_G711 == uhwCodecType))
    {
        shwNewFrameLength = CODEC_FRAME_LENGTH_WB;
    }
    else
    {
        shwNewFrameLength = CODEC_FRAME_LENGTH_NB;
    }

    /* �������б仯����� */
    if(shwOldFrameLength != shwNewFrameLength)
    {
        ushwRet =  VOICE_PROC_IS_YES;
    }
    else
    {
        ushwRet =  VOICE_PROC_IS_NO;
    }

    return ushwRet;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

