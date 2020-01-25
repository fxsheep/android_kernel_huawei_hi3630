

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "med_mbdrc.h"
#include  "med_pp_comm.h"
#include  "om.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
MED_MBDRC_STRU                          g_stMedMbdrcObj[MED_MBDRC_MAX_OBJ_NUM]; /* MBDRCģ��ȫ�ֿ���ʵ�� */
MED_OBJ_INFO                            g_stMedMbdrcObjInfo;                    /* MBDRC��Ϣ */

/* ����48kʱƵ�任��ϵ�� */
VOS_INT16 g_ashwMedMbdrcNlpWinMb[1024] =
{
       20,   177,   491,   958,  1573,  2331,  3224,  4244,  5381,  6624,  7961,  9379, 10864, 12403, 13980, 15580, 17187, 18787, 20364, 21903, 23388, 24806, 26143, 27386, 28523, 29543, 30436, 31194, 31809, 32276, 32590, 32747,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32747, 32590, 32276, 31809, 31194, 30436, 29543, 28523, 27386, 26143, 24806, 23388, 21903, 20364, 18787, 17187, 15580, 13980, 12403, 10864,  9379,  7961,  6624,  5381,  4244,  3224,  2331,  1573,   958,   491,   177,    20,
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0
};

VOS_INT16                        g_ashwMedMbdrcTmp1Len640[MED_MBDRC_MAX_FFT_LEN];

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


VOS_VOID*    MED_MBDRC_Create(VOS_VOID)
{
    /* ����MBDRCʵ��*/
    return (MED_PP_Create(
                        MED_MBDRC_GetObjInfoPtr(),
                        MED_MBDRC_GetObjPtr(),
                        MED_MBDRC_MAX_OBJ_NUM,
                        sizeof(MED_MBDRC_STRU)));
}
VOS_UINT32    MED_MBDRC_SetPara (
                    VOS_VOID         *pInstance,
                    VOS_INT16        *pshwMBDRCNv,
                    VOS_INT16         shwParaLen,
                    VOS_INT32         swSampleRate  )
{
    VOS_UINT32                          uwRet;
    MED_MBDRC_STRU                     *pstObj           = (MED_MBDRC_STRU*)pInstance; /* ��ʱMBDRCָ�� */
    MED_MBDRC_SUBAND_STRU              *pstSuband        = &(pstObj->stSuband);
    VOS_INT16                          *pshwMinPowerRMS  = pstSuband->ashwMinPowerRMS;
    VOS_INT16                           shwIsUsed;
    VOS_INT16                           shwNum;
    VOS_INT16                           shwFrmLen;
    VOS_INT16                           shwOverlapLen;
    VOS_INT16                           shwFftLen;
    VOS_INT16                           shwSubBandNum;
    VOS_INT16                          *pshwTrapewin     = MED_NULL;
    VOS_INT16                          *pshwPreOverlap   = pstObj->stAnrFft.stTransform.stErrPreStatus.ashwOverlap;
    VOS_INT16                          *pshwPostOverlap  = pstObj->stAnrFft.stTransform.stErrPostStatus.ashwOverlap;

    /* �ж�����Ƿ�Ϸ� */
    uwRet         = MED_PP_CheckPtrValid(MED_MBDRC_GetObjInfoPtr(), pstObj);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (   (shwParaLen  < MED_MBDRC_PARAM_LEN)
            || (pshwMBDRCNv[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ���ݲ����ʶ�FFT�任�����ص��������� */
            if (MED_SAMPLE_8K == swSampleRate)
            {
                shwFrmLen     = MED_MBDRC_FRM_LEN_8K;
                shwFftLen     = MED_MBDRC_FFT_LEN_8K;
                shwOverlapLen = MED_MBDRC_NB_OVERLAP_LEN;
                shwSubBandNum = MED_MBDRC_ANR_VOICE_SUB_FRAME_NUM;
                pshwTrapewin  = MED_MBDRC_GetNlpWinNb();
            }
            else if (MED_SAMPLE_16K == swSampleRate)
            {
                shwFrmLen     = MED_MBDRC_FRM_LEN_16K;
                shwFftLen     = MED_MBDRC_FFT_LEN_16K;
                shwOverlapLen = MED_MBDRC_WB_OVERLAP_LEN;
                shwSubBandNum = MED_MBDRC_ANR_VOICE_SUB_FRAME_NUM;
                pshwTrapewin  = MED_MBDRC_GetNlpWinWb();
            }
            else
            {
                shwFrmLen     = MED_MBDRC_FRM_LEN_48K;
                shwFftLen     = MED_MBDRC_FFT_LEN_48K;
                shwOverlapLen = MED_MBDRC_MB_OVERLAP_LEN;
                shwSubBandNum = MED_MBDRC_ANR_AUDIO_SUB_FRAME_NUM;
                pshwTrapewin  = MED_MBDRC_GetNlpWinMb();
            }

            /* ��ʼ�����������ڲ�״̬���� */
            shwIsUsed             = (VOS_INT16)pstObj->enIsUsed;                /* ���� */
            UCOM_MemSet(pstObj, 0, sizeof(MED_MBDRC_STRU));
            pstObj->enIsUsed      = (VOS_UINT16)shwIsUsed;                      /* �ָ� */

            /* ������ε�pInstance->stNv */
            CODEC_OpVecCpy((VOS_INT16 *)(&pstObj->stNv), pshwMBDRCNv, MED_MBDRC_PARAM_LEN);

            for (shwNum = 0; shwNum < MED_MBDRC_RX_SUBBAND_NUM; shwNum++)
            {
                pshwMinPowerRMS[shwNum]  = 1;
            }

            pstObj->shwSubBandNum               = shwSubBandNum;

            pstObj->stSuband.shwFrameLenth      = shwFrmLen;
            pstObj->stSuband.shwFreqLenth       = shwFftLen;
            pstObj->stSuband.shwLimitThr        = MED_MBDRC_RX_LIMIT_THR;
            pstObj->stSuband.shwLimitMakeUpGain = MED_MBDRC_RX_LIMIT_MAKEUP_GAIN;
            pstObj->stSuband.swdBfs0            = MED_MBDRC_RX_0DBFS;

            pstObj->stSuband.shwSpeechCnt       = pstObj->stNv.shwSpeechCntInit;

            CODEC_OpVecCpy(pstObj->stSuband.ashwLastPowerRMSdBin,
                         pstObj->stNv.ashwLastPowerRMSdBinInit,
                         MED_MBDRC_RX_SUBBAND_NUM);

            CODEC_OpVecCpy(pstObj->stSuband.ashwLastPowerRMS,
                         pstObj->stNv.ashwLastPowerRMSInit,
                         MED_MBDRC_RX_SUBBAND_NUM);

            pstObj->stAnrFft.stComCfg.shwSubFrmLen = shwFrmLen;
            pstObj->stAnrFft.stComCfg.shwFftLen    = shwFftLen;
            pstObj->stAnrFft.pshwTrapewin          = pshwTrapewin;

            /* �ź�ǰ����״̬ */
            pstObj->stAnrFft.stTransform.stErrPreStatus.shwOverlapLen = shwOverlapLen;
            UCOM_MemSet(pshwPreOverlap, 0, (VOS_UINT16)shwOverlapLen* sizeof(VOS_INT16));

            /* �����ص��Ӵ� */
            /* Overlap ���� */
            shwOverlapLen = (pstObj->stAnrFft.stComCfg.shwFftLen)
                            - (pstObj->stAnrFft.stComCfg.shwSubFrmLen);

            /* �źź���״̬ */
            pstObj->stAnrFft.stTransform.stErrPostStatus.shwOverlapLen  = shwOverlapLen;

            UCOM_MemSet(pshwPostOverlap, 0, (VOS_UINT16)shwOverlapLen* sizeof(VOS_INT16));

            uwRet = UCOM_RET_SUCC;

         }
    }

    return uwRet;
}
VOS_UINT32    MED_MBDRC_GetPara(
                    VOS_VOID         *pInstance,
                    VOS_INT16        *pshwPara,
                    VOS_INT16         shwParaLen)
{
    VOS_UINT32                  uwRet;
    MED_MBDRC_STRU             *pstObj = (MED_MBDRC_STRU *)pInstance;
    MED_MBDRC_NV_STRU          *pstNv  = &(pstObj->stNv);                       /* NV��ָ�� */

    /* �ж�����Ƿ�Ϸ� */
    uwRet         = MED_PP_CheckPtrValid(MED_MBDRC_GetObjInfoPtr(), pstObj);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (   (shwParaLen      < MED_MBDRC_PARAM_LEN)
            || (pstNv->shwEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ����pInstance->stNv��pshwPara */
            CODEC_OpVecSet(pshwPara, shwParaLen, 0);
            CODEC_OpVecCpy(pshwPara, (VOS_INT16 *)(&pstObj->stNv), MED_MBDRC_PARAM_LEN);
        }
    }

    return uwRet;
}
VOS_UINT32    MED_ANR_MBDRC_Rx_Main (
                MED_EANR_STRU            *pstMedAnr,
                MED_MBDRC_STRU           *pstMedMBDRC,
                VOS_INT16                *pshwInput,
                VOS_INT16                *pshwOutput,
                VOS_INT16                 shwVadFlag,
                CODEC_SWITCH_ENUM_UINT16  enAnrEnable)
{
    MED_ANR_FFT_STRU                 *pstAnrFft;
    MED_MBDRC_SUBAND_STRU            *pstSuband;

    VOS_INT16                         shwCnt;

    VOS_INT16                         shwNormShift;
    VOS_INT16                         shwSubBandNum;
    VOS_INT16                         shwSubFrameLen;
    VOS_INT16                         shwFrameLen;
    VOS_INT16                         shwFreqLen;
    VOS_INT16                         shwSubFramIndex;

    VOS_INT16                        *pshwMainFrm;
    VOS_INT16                        *pshwMicBin;

    VOS_INT16                         shwZeroFlagTh;
    VOS_INT16                         shwZeroFlag;

    shwSubBandNum  = pstMedMBDRC->shwSubBandNum;

    pstAnrFft      = &(pstMedMBDRC->stAnrFft);
    pstSuband      = &(pstMedMBDRC->stSuband);

    shwSubFrameLen = pstSuband->shwFrameLenth;
    shwFreqLen     = pstSuband->shwFreqLenth;

    shwFrameLen    = (VOS_INT16)(shwSubFrameLen * shwSubBandNum);

    /* ��ANR��MBDRC��ʹ��*/
    if ( (CODEC_SWITCH_OFF == enAnrEnable)
      && (CODEC_SWITCH_OFF == pstMedMBDRC->stNv.shwEnable))
    {
        CODEC_OpVecCpy(pshwOutput, pshwInput, shwFrameLen);
        return UCOM_RET_SUCC;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_ANR_MBDRC_Rx_Main);

    /* ��ANRʹ��*/
    if (CODEC_SWITCH_ON == enAnrEnable)
    {
        /* ��ֵ֡�ж� */
        shwZeroFlagTh = pstMedAnr->shwZeroFlagTh;
        shwZeroFlag   = 1;

        for (shwCnt = 0; shwCnt < shwFrameLen; shwCnt++)
        {
            if ( (pshwInput[shwCnt] >= shwZeroFlagTh)
              || (pshwInput[shwCnt] <= -shwZeroFlagTh))
            {
                shwZeroFlag = 0;
                break;
            }
        }

        if (1 == shwZeroFlag)
        {
            CODEC_OpVecCpy(pshwOutput, pshwInput, shwFrameLen);
            return UCOM_RET_SUCC;
        }
    }

    /* �����ڴ� */
    pshwMainFrm = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwSubFrameLen * sizeof(VOS_INT16));
    pshwMicBin  = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFreqLen * sizeof(VOS_INT16));

    /* MBDRC��ANR ע:20ms = 10ms+10ms ������֡���δ��� */
    for (shwSubFramIndex = 0; shwSubFramIndex < shwSubBandNum; shwSubFramIndex++)
    {

        CODEC_OpVecCpy(pshwMainFrm, &pshwInput[shwSubFramIndex*shwSubFrameLen], shwSubFrameLen);

        /* ʱƵ��ת�� */
        MED_ANR_RX_FFTProcess(pstAnrFft,
                              pshwMainFrm,
                              1,
                              pshwMicBin,
                             &shwNormShift);

        /* ANR������ */
        if (CODEC_SWITCH_ON == enAnrEnable)
        {
            OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_EANR_1MIC_Main_RX);

            MED_EANR_1MIC_Main(pstMedAnr,
                              pshwMicBin,
                              pshwMicBin,
                              shwNormShift);
            OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_EANR_1MIC_Main_RX);
        }

        OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_MBDRC_Main_RX);

        /* MBDRC������ */
        MED_MBDRC_Main(pstMedMBDRC, pshwMicBin, &shwNormShift, shwVadFlag);

        OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_MBDRC_Main_RX);

        /* Ƶʱ��ת�� */
        MED_ANR_RX_IFFTProcess (pstAnrFft, pshwMicBin, pshwMainFrm, shwNormShift);

        /* ʱ����� */
        CODEC_OpVecCpy(&pshwOutput[shwSubFramIndex*shwSubFrameLen], pshwMainFrm, shwSubFrameLen);

    }

    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwMainFrm);
    UCOM_MemFree(pshwMicBin);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_ANR_MBDRC_Rx_Main);

    return UCOM_RET_SUCC;

}
VOS_VOID    MED_ANR_RX_FFTProcess (
                       MED_ANR_FFT_STRU                  *pstAnrFft,
                       VOS_INT16                         *pshwInput,
                       VOS_INT16                          shwNormEn,
                       VOS_INT16                         *pshwSpecBin,
                       VOS_INT16                         *pshwNormShift)
{
    VOS_INT16               *pshwBuf;                                           /* size MED_MBDRC_MAX_FFT_LEN */
    VOS_INT16                shwFrmLen;
    VOS_INT16               *pshwTrapeWin;
    VOS_INT16               *pswMaxPos = MED_NULL;
    VOS_INT16                shwInMax;
    VOS_INT16                shwFftLen;
    VOS_INT16               *pshwOverlap;
    VOS_INT16                shwOverlapLen;

    MED_FFT_NUM_ENUM_INT16   enFftIndex;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_ANR_RX_FFTProcess);

    shwOverlapLen       = pstAnrFft->stTransform.stErrPreStatus.shwOverlapLen;
    pshwOverlap         = pstAnrFft->stTransform.stErrPreStatus.ashwOverlap;
    shwFftLen           = pstAnrFft->stComCfg.shwFftLen;
    shwFrmLen           = pstAnrFft->stComCfg.shwSubFrmLen;
    pshwBuf             = MED_MBDRC_GetshwVecTmp640Ptr1();
    pshwTrapeWin        = pstAnrFft->pshwTrapewin;

    CODEC_OpVecSet(pshwBuf, shwFftLen, 0);

    /* ǰ1֡�� overlap */
    CODEC_OpVecCpy(pshwBuf, pshwOverlap, shwOverlapLen);

    /* �м䲿�� */
    CODEC_OpVecCpy(pshwBuf + shwOverlapLen, pshwInput, shwFrmLen);

    /* β��ȫ0 */
    CODEC_OpVecSet((pshwBuf + shwOverlapLen) + shwFrmLen, shwFftLen-(shwFrmLen + shwOverlapLen), 0);

    /* ȡ��ǰ֡�Ľ�β������Ϊ��1֡��overlap */
    CODEC_OpVecCpy(pshwOverlap, pshwBuf + shwFrmLen, shwOverlapLen);

    if (MED_MBDRC_FFT_LEN_48K == shwFftLen)
    {
        CODEC_OpVvMultR(pshwBuf, pshwTrapeWin, shwOverlapLen,pshwBuf);

        CODEC_OpVvMultR(pshwBuf + (shwFftLen - (2*shwOverlapLen)),
                        pshwTrapeWin + (shwFftLen - (2*shwOverlapLen)),
                        shwOverlapLen,
                        pshwBuf + (shwFftLen - (2*shwOverlapLen)));
    }
    else
    {
        /* �Ӵ� */
        CODEC_OpVvMultR(pshwBuf, pshwTrapeWin, shwFftLen, pshwBuf);
    }

    /* ��һ�� */
    if (CODEC_SWITCH_ON == shwNormEn)
    {
        shwInMax       = CODEC_OpVecMaxAbs(
                         pshwBuf,
                         shwOverlapLen + shwFrmLen,
                         pswMaxPos);

        *pshwNormShift = CODEC_OpNorm_s(shwInMax);

        CODEC_OpVecShl(pshwBuf,
                       shwOverlapLen + shwFrmLen,
                      *pshwNormShift,
                       pshwBuf);
    }
    else
    {
        *pshwNormShift = 0;
    }

    /* ���ݲ���������FFT����ֵ */
    if (MED_FFT_LEN_8K == shwFftLen)
    {
        enFftIndex = MED_FFT_NUM_256;
    }
    else if (MED_FFT_LEN_16K == shwFftLen)
    {
        enFftIndex = MED_FFT_NUM_512;
    }
    else
    {
        enFftIndex = MED_FFT_NUM_1024;
    }

    MED_FFT_Fft(enFftIndex, pshwBuf, pshwSpecBin);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_ANR_RX_FFTProcess);

}


VOS_VOID    MED_MBDRC_Main(
                    MED_MBDRC_STRU          *pstMbdrc,
                    VOS_INT16               *pshwFrmFreq,
                    VOS_INT16               *pshwNormShift,
                    VOS_INT16                shwVadFlag)
{
    VOS_INT16                shwVADholdLenth;
    VOS_INT16                shwSpeechCnt;

    shwVADholdLenth = pstMbdrc->stNv.shwVADholdLenth;
    shwSpeechCnt    = pstMbdrc->stSuband.shwSpeechCnt;

    /* VADʹ��hangover���� VAD����֡��Ϊ0���ֹͣMBDRC���� */
    if (1 == shwVadFlag)
    {
        shwSpeechCnt = shwVADholdLenth;
    }
    else
    {
        shwSpeechCnt = shwSpeechCnt - 1;
        shwSpeechCnt = CODEC_OpMax(shwSpeechCnt, 0);
    }

    pstMbdrc->stSuband.shwSpeechCnt = shwSpeechCnt;

    /* ��MBDRC��ʹ�ܻ�������������ֵΪ0�򷵻� */
    if ((CODEC_SWITCH_OFF == pstMbdrc->stNv.shwEnable)
     || (shwSpeechCnt <= 0))
    {
        return;
    }

    /* �Ӵ����� */
    MED_MBDRC_SubBandProcess(pstMbdrc, pshwFrmFreq, *pshwNormShift);

    *pshwNormShift = *pshwNormShift - 1;

}
VOS_VOID MED_MBDRC_SubBandProcess (
                MED_MBDRC_STRU         *pstMedMBDRC,
                VOS_INT16              *pshwFrmFreq,
                VOS_INT16               shwNormShift)
{
    MED_MBDRC_SUBAND_STRU  *pstSuband;
    VOS_INT16               shwFreqLen;
    VOS_INT16               shwGainLen;
    VOS_INT16               shwNum;
    VOS_INT16               shwNi;
    VOS_INT16               shwBandNum;
    VOS_INT16               shwGainDRC;
    VOS_INT32              *pswGainDRC;

    VOS_INT16              *pshwSubbandNormShift;
    VOS_INT16              *pshwSubbandPowerRMS;
    VOS_INT32              *pswGainFinal;
    VOS_INT32              *pswGainFinalFilter;

    pstSuband           = &(pstMedMBDRC->stSuband);
    shwFreqLen          = pstSuband->shwFreqLenth;

    shwGainLen          = (shwFreqLen>>1) + 1;
    shwBandNum          = pstMedMBDRC->stNv.shwSubbandNum;

    /* �����ڴ� */
    pshwSubbandPowerRMS  = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandNum * sizeof(VOS_INT16));
    pshwSubbandNormShift = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandNum * sizeof(VOS_INT16));
    pswGainDRC           = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandNum * sizeof(VOS_INT32));
    pswGainFinal         = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwGainLen * sizeof(VOS_INT32));
    pswGainFinalFilter   = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwGainLen * sizeof(VOS_INT32));

    /* ������ֵ */
    for (shwNi = 0 ; shwNi < ((shwFreqLen >> 1) + 1); shwNi++)
    {
        pswGainFinal[shwNi]       = 0;
    }

    /*�Ӵ�����RMS����*/
    MED_MBDRC_SubBandPowerCal(pstMedMBDRC,
                              pshwFrmFreq,
                              pshwSubbandPowerRMS,
                              pshwSubbandNormShift);

    /*�Ӵ�DRC����*/
    for (shwNi = 0; shwNi < shwBandNum; shwNi++)
    {
       MED_DRC_GainFilter(pstMedMBDRC,
                          pshwSubbandPowerRMS[shwNi],
                          shwNi,
                          pshwSubbandNormShift[shwNi],
                          shwNormShift,
                          &shwGainDRC);

       pswGainDRC[shwNi] = CODEC_OpL_mult0(shwGainDRC, pstMedMBDRC->stNv.ashwGainMakeUp[shwNi]);
    }

    /* �������Բ�ֵ */
    MED_DRC_Interpolate(&(pstMedMBDRC->stNv), pswGainDRC, pswGainFinal);

    /* Ƶ���źų�������*/
    for (shwNum = 1; shwNum < (shwFreqLen >> 1); shwNum++)
    {
       pshwFrmFreq[shwNum * 2]       = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(pswGainFinal[shwNum], pshwFrmFreq[shwNum * 2]));

       pshwFrmFreq[(shwNum * 2) + 1] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(pswGainFinal[shwNum], pshwFrmFreq[(shwNum * 2) + 1]));
    }

    pshwFrmFreq[0] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(pswGainFinal[0], pshwFrmFreq[0]));

    pshwFrmFreq[1] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(pswGainFinal[shwGainLen - 1], pshwFrmFreq[1]));

    CODEC_OpVecShr(pshwFrmFreq, shwFreqLen, 1, pshwFrmFreq);

    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwSubbandPowerRMS);
    UCOM_MemFree(pshwSubbandNormShift);
    UCOM_MemFree(pswGainDRC);
    UCOM_MemFree(pswGainFinal);
    UCOM_MemFree(pswGainFinalFilter);

}
VOS_VOID MED_MBDRC_SubBandPowerCal (
                                 MED_MBDRC_STRU         *pstMedMBDRC,
                                 VOS_INT16              *pshwFrmFreqIn,
                                 VOS_INT16              *pshwSubbandPowerRMS,
                                 VOS_INT16              *pshwNormShift)
{
    MED_MBDRC_SUBAND_STRU  *pstSuband         = &(pstMedMBDRC->stSuband);
    VOS_INT16              *pshwBandBoundlow  = pstMedMBDRC->stNv.ashwFreqBoundLow;
    VOS_INT16              *pshwBandBoundhigh = pstMedMBDRC->stNv.ashwFreqBoundHigh;
    VOS_INT16              *pshwLastPowerRMS  = pstSuband->ashwLastPowerRMS;
    VOS_INT16               shwFreqLen;
    VOS_INT16               shwNi;
    VOS_INT16               shwBandNum;
    VOS_INT16               shwLenFD;
    VOS_INT16               shwNn;
    VOS_INT16               shwBandLen;
    VOS_INT16               shwInMax;
    VOS_INT16               shwCoutI;

    VOS_INT32              *pswSubbandPower;
    VOS_INT16              *pshwFrmFreq;

    /* ������ȡ */
    shwFreqLen    = pstSuband->shwFreqLenth;
    shwBandNum    = pstMedMBDRC->stNv.shwSubbandNum;

    /* �����ڴ� */
    pshwFrmFreq     = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)(shwFreqLen+2) * sizeof(VOS_INT16));
    pswSubbandPower = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandNum * sizeof(VOS_INT32));

    pshwFrmFreq[shwFreqLen]      = pshwFrmFreqIn[1];
    pshwFrmFreq[shwFreqLen+1]    = 0;

    for(shwNi=0; shwNi < shwFreqLen; shwNi++)
    {
        pshwFrmFreq[shwNi] = pshwFrmFreqIn[shwNi];
    }

    pshwFrmFreq[1]  = 0;

    for (shwNi = 0 ; shwNi < shwBandNum; shwNi++)
    {
        pswSubbandPower[shwNi]       = 0;
        pshwSubbandPowerRMS[shwNi]   = 0;
    }

    for(shwNi=0; shwNi < shwBandNum; shwNi++)
    {
        if (pshwBandBoundlow[shwNi] == pshwBandBoundhigh[shwNi])
        {
            pswSubbandPower[shwNi] = 0;
            pshwSubbandPowerRMS[shwNi] = 0;
            pshwLastPowerRMS[shwNi] = pshwSubbandPowerRMS[shwNi];
        }
        else
        {
            shwNn                = (VOS_INT16)((pshwBandBoundlow[shwNi] - 1) << 1);
            shwBandLen           = (VOS_INT16)(((pshwBandBoundhigh[shwNi] - pshwBandBoundlow[shwNi]) + 1) << 1);

            shwInMax             = CODEC_OpVecMaxAbs(&pshwFrmFreq[shwNn], shwBandLen, MED_NULL);
            pshwNormShift[shwNi] = CODEC_OpNorm_s(shwInMax);

            for(shwCoutI=0; shwCoutI < shwBandLen; shwCoutI++)
            {
                pshwFrmFreq[shwCoutI + shwNn] = CODEC_OpShl(pshwFrmFreq[shwCoutI + shwNn], pshwNormShift[shwNi]-6);
            }

            pswSubbandPower[shwNi] = CODEC_OpVvMac(&pshwFrmFreq[shwNn], &pshwFrmFreq[shwNn], shwBandLen, 0);

            shwLenFD = (VOS_INT16)(((pshwBandBoundhigh[shwNi] - pshwBandBoundlow[shwNi]) + 1) << 1);

            pswSubbandPower[shwNi]     = CODEC_OpL_shl(pswSubbandPower[shwNi],7);
            pshwSubbandPowerRMS[shwNi] = CODEC_OpSqrt(CODEC_OpNormDiv_32(pswSubbandPower[shwNi],shwLenFD,0));
        }

        pshwLastPowerRMS[shwNi] = pshwSubbandPowerRMS[shwNi];
    }

    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwFrmFreq);
    UCOM_MemFree(pswSubbandPower);

}
VOS_VOID  MED_DRC_GainFilter (
                MED_MBDRC_STRU        *pstMedMBDRC,
                VOS_INT16              shwPowerIn,
                VOS_INT16              shwindex,
                VOS_INT16              shwPowerNormShift,
                VOS_INT16              shwNormShift,
                VOS_INT16             *pshwGainLinear)
{
    MED_MBDRC_NV_STRU       *pstNv;
    MED_MBDRC_SUBAND_STRU   *pstSuband;
    VOS_INT16               shwPowerInd;
    VOS_INT16               shwDnCompThr;
    VOS_INT16               shwUpCompThr;
    VOS_INT16               shwDnExpThr;
    VOS_INT16               shwDnCompSlop;
    VOS_INT16               shwUpCompSlop;
    VOS_INT16               shwDnExpSlop;
    VOS_INT16               shwLogshift;
    VOS_INT16               shwPowerInDB;
    VOS_INT16               shwPowerIn0dBfs;
    VOS_INT16               shwPowerIndBfs;
    VOS_INT16               shwPowerIndBfstemp;
    VOS_INT32               swPowerouttemp1;
    VOS_INT32               swPowerouttemp2;
    VOS_INT32               swPowerouttemp3;
    VOS_INT32               swPowerouttemp4;
    VOS_INT16               shwPoweroutdBfs;
    VOS_INT16               shwAlpha;
    VOS_INT16               shwPoweroutmin;
    VOS_INT16               shwGain;
    VOS_INT16               shwPowerouttemp5;
    VOS_INT32               swPowerouttemp6;
    VOS_INT32               swGaintemp1;
    VOS_INT32               swGaintemp2;
    VOS_INT32               swGaintemp3;
    VOS_INT32               swGaintemp4;
    VOS_INT32               swInd;
    VOS_INT32               swTmp;

    pstNv         = &(pstMedMBDRC->stNv);
    pstSuband     = &(pstMedMBDRC->stSuband);

    shwDnCompThr  = pstNv->ashwDnCompThr[shwindex];
    shwUpCompThr  = pstNv->ashwUpCompThr[shwindex];
    shwDnExpThr   = pstNv->ashwDnExpThr[shwindex];
    shwDnCompSlop = pstNv->ashwDnCompSlop[shwindex];
    shwUpCompSlop = pstNv->ashwUpCompSlop[shwindex];
    shwDnExpSlop  = pstNv->ashwDnExpSlop[shwindex];

    /* ���빦��RMSת��Ϊdbֵ */
    swTmp              = (VOS_INT32)shwPowerIn << 1;
    swTmp              = CODEC_OpL_shl(swTmp,5);
    shwLogshift        = (1 + 5 + shwPowerNormShift + shwNormShift) - 6;
    shwPowerInDB       = CODEC_OpLog10(shwLogshift,swTmp);
    shwPowerIn0dBfs    = CODEC_OpLog10(0,pstSuband->swdBfs0);
    shwPowerIndBfs     = CODEC_OpSub(shwPowerInDB,shwPowerIn0dBfs);
    shwPowerIndBfstemp = CODEC_OpShl(shwPowerIndBfs,2);
    shwPowerIndBfs     = CODEC_OpAdd(shwPowerIndBfs,shwPowerIndBfstemp);

    if(shwPowerIndBfs > (pstSuband->ashwLastPowerRMSdBin[shwindex]+ 1024))
    {
        shwPowerInd = 1;
    }
    else
    {
        shwPowerInd = 0;
    }

    pstSuband->ashwLastPowerRMSdBin[shwindex] = shwPowerIndBfs;

    /* �������빦��RMSѡ����DRC���ĸ�������д�����������dBֵ��alpha�˲�ϵ��*/
    if (shwPowerIndBfs >= shwDnCompThr)
    {
        swPowerouttemp1 = CODEC_OpL_mult0(shwDnCompSlop,shwPowerIndBfs);
        swPowerouttemp2 = (CODEC_OpL_mult0(CODEC_OpSub(MED_MBDRC_RX_SLOP_NORM,shwDnCompSlop),shwDnCompThr));
        swPowerouttemp3 = CODEC_OpL_add(swPowerouttemp1,swPowerouttemp2);
        swPowerouttemp3 = CODEC_OpL_shl(swPowerouttemp3,-7);
        shwPoweroutdBfs = CODEC_OpSaturate(swPowerouttemp3);

        shwGain = CODEC_OpSub(shwPoweroutdBfs,shwPowerIndBfs);

        shwAlpha = (1 == shwPowerInd)? (pstNv->ashwDnCompAlphaUp[shwindex]):(pstNv->ashwDnCompAlphaDn[shwindex]);
    }

    else if (shwPowerIndBfs >= shwUpCompThr)
    {
        shwGain = 0;

        shwAlpha = (1 == shwPowerInd)? (pstNv->ashwLinearAlphaUp[shwindex]):(pstNv->ashwLinearAlphaDn[shwindex]);

    }
    else if(shwPowerIndBfs >= shwDnExpThr)
    {
        swPowerouttemp1 = CODEC_OpL_mult0(shwUpCompSlop,shwPowerIndBfs);
        swPowerouttemp2 = CODEC_OpL_mult0(CODEC_OpSub(MED_MBDRC_RX_SLOP_NORM, shwUpCompSlop),shwUpCompThr);
        swPowerouttemp3 = CODEC_OpL_add(swPowerouttemp1, swPowerouttemp2);
        swPowerouttemp3 = CODEC_OpL_shl(swPowerouttemp3, -7);
        shwPoweroutdBfs = CODEC_OpSaturate(swPowerouttemp3);

        shwGain = CODEC_OpSub(shwPoweroutdBfs,shwPowerIndBfs);

        shwAlpha = (1 == shwPowerInd)? (pstNv->ashwUpCompAlphaUp[shwindex]):(pstNv->ashwUpCompAlphaDn[shwindex]);

    }
    else
    {
        swPowerouttemp1 = CODEC_OpL_mult0(shwUpCompSlop,shwDnExpThr);
        swPowerouttemp2 = CODEC_OpL_mult0(CODEC_OpSub(MED_MBDRC_RX_SLOP_NORM,shwUpCompSlop),shwUpCompThr);
        swPowerouttemp3 = CODEC_OpL_add(swPowerouttemp1,swPowerouttemp2);
        swPowerouttemp3 = CODEC_OpL_shl(swPowerouttemp3,-7);
        shwPoweroutmin  = CODEC_OpSub(shwDnExpThr,CODEC_OpSaturate(CODEC_OpNormDiv_32(CODEC_OpL_shl(CODEC_OpL_sub(swPowerouttemp3,pstMedMBDRC->stNv.shwDRCMindB),7),pstMedMBDRC->stNv.ashwDnExpSlop[shwindex],0)));

        if (shwPowerIndBfs >= shwPoweroutmin)
        {
            shwPowerouttemp5= CODEC_OpSub(shwDnExpThr, shwPowerIndBfs);
            swPowerouttemp4 = CODEC_OpL_mult0(shwPowerouttemp5, shwDnExpSlop);
            swPowerouttemp4 = CODEC_OpL_shl(swPowerouttemp4,-7);
            swPowerouttemp6 = CODEC_OpL_sub(swPowerouttemp3,swPowerouttemp4);

            shwPoweroutdBfs = CODEC_OpSaturate(swPowerouttemp6);
        }
        else
        {
            shwPoweroutdBfs = CODEC_OpSaturate(pstNv->shwDRCMindB);
        }

        shwGain = CODEC_OpSub(shwPoweroutdBfs,shwPowerIndBfs);

        /* ��С���汣�� */
        if (shwGain < pstNv->ashwDnExpMinGain[shwindex])
        {
            shwGain = pstNv->ashwDnExpMinGain[shwindex];
        }

        shwAlpha = (1 == shwPowerInd)? (pstNv->ashwDnExpAlphaUp[shwindex]):(pstNv->ashwDnExpAlphaDn[shwindex]);

    }

    /* ����õ�������ֵ����alpha�˲� */
    shwGain     = CODEC_OpMax(shwGain,pstMedMBDRC->stNv.ashwDnExpMinGain[shwindex]); // Alpha [16bit Q7]
    swGaintemp1 = CODEC_OpL_mult0(shwAlpha,shwGain);
    swGaintemp2 = CODEC_OpL_mult0(CODEC_OpSub(MED_MBDRC_RX_ALPHA_NORM,shwAlpha),pstSuband->ashwLastGain[shwindex]);
    swGaintemp3 = CODEC_OpL_add(swGaintemp1,swGaintemp2);
    swGaintemp3 = CODEC_OpL_shl(swGaintemp3,-7);
    shwGain     = CODEC_OpSaturate(swGaintemp3);

    pstSuband->ashwLastGain[shwindex] = shwGain;

    /* �Ѿ���alpha�˲�������dBֵת��������ֵ */
    shwGain     = CODEC_OpSub(shwGain, MED_MBDRC_RX_GAIN_30DB_NORM);
    swGaintemp4 = ((VOS_INT32)shwGain) << 16;
    swInd       = CODEC_OpL_shl(CODEC_OpL_mpy_32_16(swGaintemp4, 1638), 2);
    swInd       = CODEC_OpL_min(-1, swInd);
   *pshwGainLinear = CODEC_OpExtract_h(CODEC_OpExp10(swInd));
}
MED_VOID MED_DRC_Interpolate(
                        MED_MBDRC_NV_STRU      *pstMbdrcNv,
                        VOS_INT32              *pswMakeupGain,
                        VOS_INT32              *pswInterpolateGain)
{

    VOS_INT16                                   shwCntI;
    VOS_INT16                                   shwCntJ;
    VOS_INT16                                   shwNumBand;
    VOS_INT16                                  *pshwLowerFreq;
    VOS_INT16                                  *pshwUpperFreq;
    VOS_INT16                                  *pshwRefFreqIdx;

    VOS_INT32                                   swSlope1;
    VOS_INT16                                   shwSlope2;

    /* ������ȡ */
    shwNumBand         = pstMbdrcNv->shwSubbandNum;
    pshwLowerFreq      = pstMbdrcNv->ashwFreqBoundLow;
    pshwUpperFreq      = pstMbdrcNv->ashwFreqBoundHigh;

    /* �����ڴ� */
    pshwRefFreqIdx = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwNumBand * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwNumBand; shwCntI++)
    {
        pshwRefFreqIdx[shwCntI] = CODEC_OpShr(CODEC_OpAdd(pshwLowerFreq[shwCntI], pshwUpperFreq[shwCntI]), 1);
    }

    for (shwCntI = 0; shwCntI < pshwRefFreqIdx[0]; shwCntI++)
    {
        pswInterpolateGain[shwCntI] = pswMakeupGain[0];
    }

    for (shwCntI = pshwRefFreqIdx[shwNumBand - 1] - 1; shwCntI < pshwUpperFreq[shwNumBand - 1]; shwCntI++)
    {
        pswInterpolateGain[shwCntI] = pswMakeupGain[shwNumBand-1];
    }

    for (shwCntI = 0; shwCntI < (shwNumBand - 1); shwCntI++)
    {
        swSlope1  = CODEC_OpL_sub(pswMakeupGain[shwCntI + 1],pswMakeupGain[shwCntI]);

        shwSlope2 = CODEC_OpSub(pshwRefFreqIdx[shwCntI + 1] - 1,pshwRefFreqIdx[shwCntI] - 1);

        for (shwCntJ = pshwRefFreqIdx[shwCntI] - 1; shwCntJ < pshwRefFreqIdx[shwCntI + 1]; shwCntJ++)
        {
            pswInterpolateGain[shwCntJ] = CODEC_OpL_add(pswMakeupGain[shwCntI],
                                                        CODEC_OpL_mpy_32_16(swSlope1,
                                                                            CODEC_OpDiv_s(CODEC_OpSub(shwCntJ,
                                                                                                      pshwRefFreqIdx[shwCntI]-1),
                                                                                          shwSlope2)));
        }
    }

    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwRefFreqIdx);

}
 VOS_VOID MED_ANR_RX_IFFTProcess (
                        MED_ANR_FFT_STRU                 *pstAnrFft,
                        VOS_INT16                        *pshwErrBin,
                        VOS_INT16                        *pshwErrFrm,
                        VOS_INT16                         shwNormShift)
{
    VOS_INT16                shwFrmLen;
    VOS_INT16                shwCntI;
    VOS_INT16               *pshwOverlap;
    VOS_INT16                shwOverlapLen;
    VOS_INT16                shwFftLen;
    VOS_INT16               *pshwFrm;
    VOS_INT16               *pshwTrapewin;

    MED_FFT_NUM_ENUM_INT16   enIfftIndex;

    shwFrmLen      = pstAnrFft->stComCfg.shwSubFrmLen;
    shwFftLen      = pstAnrFft->stComCfg.shwFftLen;

    pshwOverlap    = pstAnrFft->stTransform.stErrPostStatus.ashwOverlap;
    shwOverlapLen  = pstAnrFft->stTransform.stErrPostStatus.shwOverlapLen;

    pshwTrapewin   = pstAnrFft->pshwTrapewin;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_ANR_RX_IFFTProcess);

    /* ��̬�ڴ����� */
    pshwFrm        = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT16));

    /* ���ݲ���������IFFT����ֵ */
    if (MED_FFT_LEN_8K == shwFftLen)
    {
        enIfftIndex = MED_IFFT_NUM_256;
    }
    else if (MED_FFT_LEN_16K == shwFftLen)
    {
        enIfftIndex = MED_IFFT_NUM_512;
    }
    else
    {
        enIfftIndex = MED_IFFT_NUM_1024;
    }

    /* IFFT�任 */
    MED_FFT_Ifft(enIfftIndex, pshwErrBin, pshwFrm);

    /* ����һ�� */
    for (shwCntI = 0; shwCntI < shwFftLen; shwCntI++)
    {
        pshwFrm[shwCntI] = CODEC_OpShr_r(pshwFrm[shwCntI], shwNormShift);
    }

    if ((MED_FFT_LEN_8K  == shwFftLen)
     || (MED_FFT_LEN_16K == shwFftLen))
     {
        /* �Ӵ� */
        CODEC_OpVvMultR(pshwFrm, pshwTrapewin, shwFftLen, pshwFrm);
     }

    /* �����ص� */
    CODEC_OpVvAdd(pshwFrm, pshwOverlap, shwOverlapLen, pshwFrm);
    CODEC_OpVecCpy(pshwOverlap, (pshwFrm + shwFrmLen), (shwFftLen - shwFrmLen));

    /* �в� */
    CODEC_OpVecCpy(pshwErrFrm, pshwFrm, shwFrmLen);

    /* ��̬�ڴ����� */
    UCOM_MemFree(pshwFrm);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_ANR_RX_IFFTProcess);

}


VOS_UINT32   MED_MBDRC_Destroy(VOS_VOID  **ppInstance)
{
    VOS_UINT32              uwRet;
    MED_MBDRC_STRU         *pstObj;

    pstObj               = (MED_MBDRC_STRU *)(*ppInstance);

    /* �ж�����Ƿ�Ϸ� */
    uwRet                = MED_PP_CheckPtrValid(MED_MBDRC_GetObjInfoPtr(), pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
       UCOM_MemSet(pstObj, 0, sizeof(MED_MBDRC_STRU));

       pstObj->enIsUsed  = CODEC_OBJ_UNUSED;                                      /* ʵ������־��Ϊfalse */

       *ppInstance       = MED_NULL;
    }

    return uwRet;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

