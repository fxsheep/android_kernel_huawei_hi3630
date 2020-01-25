
/*****************************************************************************
   1 ͷ�ļ�����
*****************************************************************************/
#include "med_aec_dtd.h"
#include "med_aec_nlp.h"
#include "med_anr_2mic_fd.h"
#include "om.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif
/*lint -e(813)*/
/*****************************************************************************
   2 ȫ�ֱ�������
*****************************************************************************/
/* SER�����������ڸ���Pʱ�Ӵ���ֵ��Ȩֵ 16bit Q12 */
VOS_INT16 g_ashwDtdBandSERThdModulTab[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB] =
{
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047,
    2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047
};

/* �������ڸ���P����ƽ��ϵ�� */
VOS_INT16 g_ashwSpBandProbAlphaCoeff[MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_LEN] =
{
    3640, 7280, 10920, 7280, 3640
};

/*****************************************************************************
   3 ����ʵ��
*****************************************************************************/


VOS_VOID MED_AEC_DtdInit(
                MED_AEC_NV_STRU                *pstAecParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                MED_AEC_DTD_STRU               *pstDtd)
{
    VOS_INT16        shwCnt;
    VOS_INT16        shwStFlrThd = MED_AEC_DTD_ST_FLR_THD;

    UCOM_MemSet((VOS_INT16*)pstDtd, 0, sizeof(MED_AEC_DTD_STRU));

    pstDtd->shwFrameLen              = MED_PP_GetFrameLength();
    pstDtd->shwMutePowerThd          = pstAecParam->shwDtdMutePowerThd;

    /* �����ʶ�Ӧ������ʼ�� �������Ժͷ�����DTD */
    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        pstDtd->stDtdLine.enFftLen             = MED_AEC_MAX_FFT_LEN / 2;
        pstDtd->stDtdLine.shwMeanAlpha         = MED_AEC_DTD_MEAN_ALPHA_NB;
        pstDtd->stDtdNline.enFftLen            = MED_AEC_MAX_FFT_LEN / 2;
        pstDtd->stDtdNline.shwFreqBinLen       = MED_AEC_DTD_NLINE_FREQ_BIN_LEN_NB;
        pstDtd->stDtdNline.shwBandLen          = MED_AEC_DTD_NLINE_PSD_BAND_LEN_NB;
    }
    else
    {
        pstDtd->stDtdLine.enFftLen             = MED_AEC_MAX_FFT_LEN;
        pstDtd->stDtdLine.shwMeanAlpha         = MED_AEC_DTD_MEAN_ALPHA_WB;
        pstDtd->stDtdNline.enFftLen            = MED_AEC_MAX_FFT_LEN;
        pstDtd->stDtdNline.shwFreqBinLen       = MED_AEC_DTD_NLINE_FREQ_BIN_LEN_WB;
        pstDtd->stDtdNline.shwBandLen          = MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB;
    }

    /* ����DTD�ṹ���ʼ�� */
    pstDtd->stDtdLine.shwFrameLen     = MED_PP_GetFrameLength();
    pstDtd->stDtdLine.shwStModiThd    = pstAecParam->shwDtdStModiThd;
    pstDtd->stDtdLine.shwDtModiThd    = pstAecParam->shwDtdDtModiThd;
    pstDtd->stDtdLine.shwStFlrThd     = shwStFlrThd;

    /* ������DTD�ṹ���ʼ�� */
    pstDtd->stDtdNline.shwSpProbMean        = MED_AEC_DTD_NLINE_SP_BAND_PROB_INIT_VALUE;
    pstDtd->stDtdNline.enNearFarRatioEnable = pstAecParam->enNearFarRatioEnable;
    pstDtd->stDtdNline.shwNearFarRatioActiveFrmNum = pstAecParam->shwNearFarRatioActiveFrmNum;
    pstDtd->stDtdNline.shwNearFarRatioGain  = pstAecParam->shwNearFarRatioGain;
    pstDtd->stDtdNline.shwSpSerThd          = pstAecParam->shwSpSerThd;
    pstDtd->stDtdNline.shwEchoSerThd        = pstAecParam->shwEchoSerThd;
    pstDtd->stDtdNline.shwBandPsdMuteThd    = pstAecParam->shwBandPsdMuteThd;
    pstDtd->stDtdNline.shwSpThdInit         = pstAecParam->shwSpThdInit;
    pstDtd->stDtdNline.shwSpThd             = pstAecParam->shwSpThdInit;
    pstDtd->stDtdNline.shwSpThdMax          = pstAecParam->shwSpThdMax;
    pstDtd->stDtdNline.shwSpThdMin          = pstAecParam->shwSpThdMin;
    pstDtd->stDtdNline.swSumPsdThd          = CODEC_OpL_mult0(pstAecParam->shwSumPsdThd,
                                                            MED_AEC_DTD_NLINE_SUM_PSD_THD_BASE); /* 32bit Q8 */
    for (shwCnt = 0; shwCnt < pstDtd->stDtdNline.shwBandLen; shwCnt++)
    {
        pstDtd->stDtdNline.ashwSpBandProb[shwCnt] = MED_AEC_DTD_NLINE_SP_BAND_PROB_INIT_VALUE;
    }

    /* ������ǿ���Ԫ�س�ʼ�� ����˫��ʱʹ�� */
    pstDtd->enEstFlag                = pstAecParam->enEstFlag;
    pstDtd->shwEstPowerThd           = pstAecParam->shwEstPowerThd;
    pstDtd->shwEstAmplThd            = pstAecParam->shwEstAmplThd;
    pstDtd->shwEstHangOver           = MED_AEC_DTD_EST_HANG_OVER_LEN;

    /* ��˫���б�����ʼ��Ϊbypass */
    pstDtd->enDtdFlag                = MED_AEC_DTD_FLAG_PASS;

}


VOS_VOID MED_AEC_DtdMuteDetect(
                MED_AEC_DTD_STRU              *pstDtd,
                VOS_INT16                     *pshwFar)
{
    VOS_INT16                     shwMeanFar  = 0;
    VOS_INT16                     shwMaxFar   = 0;
    VOS_INT32                     swPowerThd  = 0;
    VOS_INT32                     swCurrPower = 0;
    VOS_INT32                     swAddPara   = 0;
    VOS_INT16                     shwFrameLen = 0;
    VOS_INT16                     ashwFarTmp[CODEC_PCM_MAX_FRAME_LENGTH];
    VOS_INT32                     swFarSum = 0;
    VOS_INT32                     swCnt    = 0;

    MED_AEC_DTD_FLAG_ENUM_INT16   enDtdFlag;

    shwFrameLen = pstDtd->shwFrameLen;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_DtdMuteDetect);

    /* ����˫����־��ʼ��Ϊ���� */
    enDtdFlag             = MED_AEC_DTD_FLAG_ST;

    /* ����ƽ�����ź����� */
    CODEC_OpVecAbs(pshwFar, shwFrameLen, ashwFarTmp);

    shwMaxFar = CODEC_OpVecMax(ashwFarTmp, shwFrameLen, 0);

    for (swCnt = 0; swCnt < shwFrameLen; swCnt++)
    {
        swFarSum = CODEC_OpL_add(swFarSum, ashwFarTmp[swCnt]);
    }

    shwMeanFar  = (VOS_INT16)(swFarSum / shwFrameLen);

    swCurrPower = CODEC_OpL_mult0(shwMeanFar, shwMeanFar);

    swCurrPower = CODEC_OpL_mpy_32_16(swCurrPower, MED_AEC_DTD_MUTE_ALPHA);
    swAddPara   = CODEC_OpL_mpy_32_16(pstDtd->swPowerLast, ((CODEC_OP_INT16_MAX - MED_AEC_DTD_MUTE_ALPHA) + 1));
    swCurrPower = CODEC_OpL_add(swCurrPower, swAddPara);

    pstDtd->swPowerLast = swCurrPower;

    /* ���������ж��Ƿ��л��� */
    if( (swCurrPower < (VOS_INT32)(pstDtd->shwMutePowerThd)) )
    {
        pstDtd->shwMuteHandover = CODEC_OpSub(pstDtd->shwMuteHandover, 1);

        if(pstDtd->shwMuteHandover <= 0)
        {
            enDtdFlag               = MED_AEC_DTD_FLAG_PASS;
            pstDtd->shwMuteHandover = 0;
        }
    }
    else
    {
        enDtdFlag               = MED_AEC_DTD_FLAG_ST;
        pstDtd->shwMuteHandover = CODEC_OpAdd(pstDtd->shwMuteHandover, 1);

        if(pstDtd->shwMuteHandover> MED_AEC_DTD_MUTE_HO_THD)
        {
            pstDtd->shwMuteHandover = MED_AEC_DTD_MUTE_HO_THD;
        }
    }

    /* ��ǿ����ʹ��ʱ�����Ƿ�ǿ���ж�ΪST��־ */
    if ( CODEC_SWITCH_ON == pstDtd->enEstFlag)
    {
        swPowerThd = CODEC_OpL_mult0(pstDtd->shwEstPowerThd, MED_AEC_DTD_EST_POWER_BASE);

        if ((swCurrPower > swPowerThd)
         || (shwMaxFar   > pstDtd->shwEstAmplThd))
        {
            pstDtd->shwFarLargeFlag = 1;
            pstDtd->shwEstHangOver  = 0;
        }
        else
        {
            pstDtd->shwEstHangOver = CODEC_OpAdd(pstDtd->shwEstHangOver, 1);

            if (pstDtd->shwEstHangOver > MED_AEC_DTD_EST_HANG_OVER_LEN)
            {
                pstDtd->shwFarLargeFlag = 0;
            }
            else
            {
                pstDtd->shwFarLargeFlag = 1;
            }
        }
    }

    pstDtd->enDtdFlag = enDtdFlag;

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_DtdMuteDetect);

}
#ifndef _MED_C89_

static inline VOS_VOID MED_AEC_DtdUnit(
                VOS_INT32    shwFftLen,
                VOS_INT16   *pshwErrFreq,
                VOS_INT16   *pshwEchoFreq,
                VOS_INT16   *pshwNearFreq,
                VOS_INT32   *pswPdy,
                VOS_INT32   *pswPdd,
                VOS_INT32   *pswPdySum,
                VOS_INT32   *pswPddSum)
{
    VOS_INT32   swCntI;
    VOS_INT16   shwSpecAlpha  = MED_AEC_DTD_SPEC_ALPHA;
    VOS_INT16   shwSmoothCoef = MED_AEC_NLP_BGN_SMOOTH_COFE1;

    ae_p24x2s   aepEr, aepEc, aepNe, aepSa, aepSc;
    ae_q56s     aeqSum1, aeqSum2, aeqTmp1, aeqTmp2;

    /* 4�ֽڶ��뱣�� */
    if (((((VOS_INT32)&pshwErrFreq[0]) & 0x03) != 0)
      || ((((VOS_INT32)&pshwEchoFreq[0]) & 0x03) != 0)
      || ((((VOS_INT32)&pshwNearFreq[0]) & 0x03) != 0))
    {
        return;
    }

    aepSa = AE_CVTP24A16(shwSpecAlpha);
    aepSc = AE_CVTP24A16(shwSmoothCoef);

    aeqSum1 = *((ae_q32s *)pswPdySum);
    aeqSum2 = *((ae_q32s *)pswPddSum);

    for (swCntI = 0; swCntI < shwFftLen>>1; swCntI++)
    {
        aepEr = *((ae_p16x2s *)&pshwErrFreq[2*swCntI]);
        aepEc = *((ae_p16x2s *)&pshwEchoFreq[2*swCntI]);

        aepEr = AE_ABSSP24S(aepEr);
        aepEc = AE_ABSSP24S(aepEc);
        aepNe = AE_ADDSP24S(aepEr, aepEc);

        *((ae_p16x2s *)&pshwNearFreq[2*swCntI]) = aepNe;

        aeqTmp1 = *((ae_q32s *)&pswPdy[2*swCntI]);
        aeqTmp1 = AE_MULFQ32SP16S_L(aeqTmp1, aepSa);
        aeqTmp2 = AE_MULP24S_HH(aepEc, aepNe);
        aeqTmp2 = AE_MULFQ32SP16S_L(aeqTmp2, aepSc);
        aeqTmp2 = AE_ADDSQ56S(aeqTmp1, aeqTmp2);
        aeqSum1 = AE_ADDSQ56S(aeqTmp2, aeqSum1);
        aeqTmp2 = AE_ROUNDSQ32ASYM(aeqTmp2);
        pswPdy[2*swCntI] = AE_TRUNCA32Q48(aeqTmp2);

        aeqTmp1 = *((ae_q32s *)&pswPdy[2*swCntI+1]);
        aeqTmp1 = AE_MULFQ32SP16S_L(aeqTmp1, aepSa);
        aeqTmp2 = AE_MULP24S_LL(aepEc, aepNe);
        aeqTmp2 = AE_MULFQ32SP16S_L(aeqTmp2, aepSc);
        aeqTmp2 = AE_ADDSQ56S(aeqTmp1, aeqTmp2);
        aeqSum1 = AE_ADDSQ56S(aeqTmp2, aeqSum1);
        aeqTmp2 = AE_ROUNDSQ32ASYM(aeqTmp2);
        pswPdy[2*swCntI+1] = AE_TRUNCA32Q48(aeqTmp2);

        aeqTmp1 = *((ae_q32s *)&pswPdd[2*swCntI]);
        aeqTmp1 = AE_MULFQ32SP16S_L(aeqTmp1, aepSa);
        aeqTmp2 = AE_MULP24S_HH(aepNe, aepNe);
        aeqTmp2 = AE_MULFQ32SP16S_L(aeqTmp2, aepSc);
        aeqTmp2 = AE_ADDSQ56S(aeqTmp1, aeqTmp2);
        aeqSum2 = AE_ADDSQ56S(aeqTmp2, aeqSum2);
        aeqTmp2 = AE_ROUNDSQ32ASYM(aeqTmp2);
        pswPdd[2*swCntI] = AE_TRUNCA32Q48(aeqTmp2);

        aeqTmp1 = *((ae_q32s *)&pswPdd[2*swCntI+1]);
        aeqTmp1 = AE_MULFQ32SP16S_L(aeqTmp1, aepSa);
        aeqTmp2 = AE_MULP24S_LL(aepNe, aepNe);
        aeqTmp2 = AE_MULFQ32SP16S_L(aeqTmp2, aepSc);
        aeqTmp2 = AE_ADDSQ56S(aeqTmp1, aeqTmp2);
        aeqSum2 = AE_ADDSQ56S(aeqTmp2, aeqSum2);
        aeqTmp2 = AE_ROUNDSQ32ASYM(aeqTmp2);
        pswPdd[2*swCntI+1] = AE_TRUNCA32Q48(aeqTmp2);

    }

    (*pswPdySum) = AE_TRUNCA32Q48(aeqSum1);
    (*pswPddSum) = AE_TRUNCA32Q48(aeqSum2);

}
MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdLine(
                MED_AEC_DTD_LINE_STRU         *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagPre,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwError,
                VOS_INT16                     *pshwNear)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           swValue;
    VOS_INT16                           shwTmp;
    VOS_INT16                           shwCorr;
    VOS_INT32                           swPdySum = 0;
    VOS_INT32                           swPddSum = 0;
    VOS_INT32                           swSumPara1;
    VOS_INT32                           swSumPara2;
    VOS_INT16                           shwTempMultPara;

    VOS_INT16                          *pshwNearFreq;
    VOS_INT16                          *pshwTmp;
    VOS_INT32                           swTmpSum = 0;
    VOS_INT32                           swAvgAbsErr;
    VOS_INT16                           shwBeta;
    VOS_INT32                           swAvgAbsNear;

    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlag;

    shwTmp       = MED_AEC_NLP_BGN_SMOOTH_COFE1;
    pshwNearFreq = MED_AEC_GetshwVecTmp640Ptr1();

    /* pass̬ */
    if (MED_AEC_DTD_FLAG_PASS == enDtdFlagPre)
    {
        return enDtdFlagPre;
    }

    MED_AEC_DtdUnit((VOS_INT32)pstDtd->enFftLen,
                    pshwErrFreq,
                    pshwEchoFreq,
                    pshwNearFreq,
                    pstDtd->aswPdy,
                    pstDtd->aswPdd,
                    &swPdySum,
                    &swPddSum);

    /* �����һ������� */
    swPdySum = CODEC_OpL_max(swPdySum, 0);
    swPddSum = CODEC_OpL_max(swPddSum, 1);
    shwCorr  = CODEC_OpSqrt(CODEC_OpNormDiv_32(swPdySum, swPddSum, MED_PP_SHIFT_BY_30));

    /* ����С�ź���������Ч��������������Ϊ��������ֵ�ʵ����͡�*/
    pshwTmp = pshwNearFreq;
    CODEC_OpVecAbs(pshwError, pstDtd->shwFrameLen, pshwTmp);

    for (swCntI = 0; swCntI < pstDtd->shwFrameLen; swCntI++)
    {
        swTmpSum = CODEC_OpL_add((VOS_INT32)pshwTmp[swCntI], swTmpSum);
    }

    swAvgAbsErr = CODEC_OpL_mpy_32_16(swTmpSum, pstDtd->shwMeanAlpha);

    if (swAvgAbsErr < pstDtd->shwStModiThd)
    {
        shwBeta = CODEC_OpSaturate(CODEC_OpNormDiv_32(swAvgAbsErr,
                                                  pstDtd->shwStModiThd,
                                                  MED_PP_SHIFT_BY_15));
        shwBeta = CODEC_OpMax(shwBeta, MED_AEC_DTD_BETA_MIN);
    }
    else
    {
        shwBeta = MED_AEC_DTD_BETA_MAX;
    }

    /* ��˫���ж� */
    shwTmp =    (shwCorr >= CODEC_OpMult(MED_AEC_DTD_CORR_THD, shwBeta))
             || (swAvgAbsErr < pstDtd->shwStFlrThd);
    if (1== shwTmp)
    {
        enDtdFlag = MED_AEC_DTD_FLAG_ST;
    }
    else
    {
        enDtdFlag = MED_AEC_DTD_FLAG_DT;
    }

    /* ������ʱ�����������Ϊ˫��������Ϊ�������������ȴ��ڵ�������ֵ��AF��в��źŷ��ȴ���MIC�źŵ�һ�� */
    if (MED_AEC_DTD_FLAG_ST == enDtdFlag)
    {
        CODEC_OpVecAbs(pshwNear, pstDtd->shwFrameLen, pshwTmp);
        swTmpSum = 0;

        for (swCntI = 0; swCntI < pstDtd->shwFrameLen; swCntI++)
        {
            swTmpSum = CODEC_OpL_add((VOS_INT32)pshwTmp[swCntI], swTmpSum);
        }

        swAvgAbsNear = CODEC_OpL_mpy_32_16(swTmpSum, pstDtd->shwMeanAlpha);
        swValue = CODEC_OpMult(pstDtd->shwDtModiThd, (VOS_INT16)swAvgAbsNear);

        if (   (swAvgAbsErr > pstDtd->shwStModiThd)
            && (swAvgAbsErr > swValue))
        {
            enDtdFlag = MED_AEC_DTD_FLAG_DT;
        }
    }

    return enDtdFlag;

}

#else
MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdLine(
                MED_AEC_DTD_LINE_STRU         *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagPre,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwError,
                VOS_INT16                     *pshwNear)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           swValue;
    VOS_INT16                           shwTmp;
    VOS_INT16                           shwCorr;
    VOS_INT32                           swPdySum = 0;
    VOS_INT32                           swPddSum = 0;
    VOS_INT32                           swSumPara1;
    VOS_INT32                           swSumPara2;
    VOS_INT16                           shwTempMultPara;

    VOS_INT16                          *pshwNearFreq;
    VOS_INT16                          *pshwTmp;
    VOS_INT32                           swTmpSum = 0;
    VOS_INT32                           swAvgAbsErr;
    VOS_INT16                           shwBeta;
    VOS_INT32                           swAvgAbsNear;

    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlag;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_DtdLine);

    shwTmp       = MED_AEC_NLP_BGN_SMOOTH_COFE1;
    pshwNearFreq = MED_AEC_GetshwVecTmp640Ptr1();

    /* pass̬ */
    if (MED_AEC_DTD_FLAG_PASS == enDtdFlagPre)
    {
        return enDtdFlagPre;
    }

    for (swCntI = 0; swCntI < pstDtd->enFftLen; swCntI++)
    {
        pshwNearFreq[swCntI] = CODEC_OpAdd(CODEC_OpAbs_s(pshwErrFreq[swCntI]),
                                          CODEC_OpAbs_s(pshwEchoFreq[swCntI]));
    }

    /* ���˺Ͳв��źŵĻ������ף�֡��ƽ�� */
    for (swCntI = 0; swCntI < pstDtd->enFftLen; swCntI++)
    {
        swSumPara1       = CODEC_OpL_mpy_32_16(pstDtd->aswPdy[swCntI], MED_AEC_DTD_SPEC_ALPHA);
        shwTempMultPara  = CODEC_OpMult_r(shwTmp, CODEC_OpAbs_s(pshwEchoFreq[swCntI]));
        swSumPara2       = CODEC_OpL_mult0(pshwNearFreq[swCntI], shwTempMultPara);
        pstDtd->aswPdy[swCntI] = CODEC_OpL_add(swSumPara1, swSumPara2);
        swPdySum         = CODEC_OpL_add(pstDtd->aswPdy[swCntI], swPdySum);
    }

    /* �����Թ����ף�֡��ƽ�� */
    for (swCntI = 0; swCntI < pstDtd->enFftLen; swCntI++)
    {
        swSumPara1       = CODEC_OpL_mpy_32_16(pstDtd->aswPdd[swCntI], MED_AEC_DTD_SPEC_ALPHA);
        shwTempMultPara  = CODEC_OpMult_r(shwTmp, pshwNearFreq[swCntI]);
        swSumPara2       = CODEC_OpL_abs(CODEC_OpL_mult0(pshwNearFreq[swCntI], shwTempMultPara));
        pstDtd->aswPdd[swCntI] = CODEC_OpL_add(swSumPara1, swSumPara2);
        swPddSum = CODEC_OpL_add(pstDtd->aswPdd[swCntI], swPddSum);
    }

    /* �����һ������� */
    swPdySum = CODEC_OpL_max(swPdySum, 0);
    swPddSum = CODEC_OpL_max(swPddSum, 1);
    shwCorr  = CODEC_OpSqrt(CODEC_OpNormDiv_32(swPdySum, swPddSum, MED_PP_SHIFT_BY_30));

    /* ����С�ź���������Ч��������������Ϊ��������ֵ�ʵ����͡�*/
    pshwTmp = pshwNearFreq;
    CODEC_OpVecAbs(pshwError, pstDtd->shwFrameLen, pshwTmp);

    for (swCntI = 0; swCntI < pstDtd->shwFrameLen; swCntI++)
    {
        swTmpSum = CODEC_OpL_add((VOS_INT32)pshwTmp[swCntI], swTmpSum);
    }

    swAvgAbsErr = CODEC_OpL_mpy_32_16(swTmpSum, pstDtd->shwMeanAlpha);

    if (swAvgAbsErr < pstDtd->shwStModiThd)
    {
        shwBeta = CODEC_OpSaturate(CODEC_OpNormDiv_32(swAvgAbsErr,
                                                  pstDtd->shwStModiThd,
                                                  MED_PP_SHIFT_BY_15));
        shwBeta = CODEC_OpMax(shwBeta, MED_AEC_DTD_BETA_MIN);
    }
    else
    {
        shwBeta = MED_AEC_DTD_BETA_MAX;
    }

    /* ��˫���ж� */
    shwTmp =    (shwCorr >= CODEC_OpMult(MED_AEC_DTD_CORR_THD, shwBeta))
             || (swAvgAbsErr < pstDtd->shwStFlrThd);
    if (1== shwTmp)
    {
        enDtdFlag = MED_AEC_DTD_FLAG_ST;
    }
    else
    {
        enDtdFlag = MED_AEC_DTD_FLAG_DT;
    }

    /* ������ʱ�����������Ϊ˫��������Ϊ�������������ȴ��ڵ�������ֵ��AF��в��źŷ��ȴ���MIC�źŵ�һ�� */
    if (MED_AEC_DTD_FLAG_ST == enDtdFlag)
    {
        CODEC_OpVecAbs(pshwNear, pstDtd->shwFrameLen, pshwTmp);
        swTmpSum = 0;

        for (swCntI = 0; swCntI < pstDtd->shwFrameLen; swCntI++)
        {
            swTmpSum = CODEC_OpL_add((VOS_INT32)pshwTmp[swCntI], swTmpSum);
        }

        swAvgAbsNear = CODEC_OpL_mpy_32_16(swTmpSum, pstDtd->shwMeanAlpha);
        swValue = CODEC_OpMult(pstDtd->shwDtModiThd, (VOS_INT16)swAvgAbsNear);

        if (   (swAvgAbsErr > pstDtd->shwStModiThd)
            && (swAvgAbsErr > swValue))
        {
            enDtdFlag = MED_AEC_DTD_FLAG_DT;
        }
    }

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_DtdLine);

    return enDtdFlag;

}
#endif
MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdNline(
                MED_AEC_DTD_NLINE_STRU        *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagLine,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwFarFreq)
{
    VOS_INT16          shwCnt;
    VOS_INT16          shwFreqBinLen;
    VOS_INT16          shwBandLen;
    VOS_INT16          shwAlpha;
    VOS_INT16          shwSubAlpha;

    VOS_INT16         *pshwNearFreq;
    VOS_INT16         *pshwSpBandProb;

    VOS_INT32          swSpProbSum;
    VOS_INT16          shwSpProbMean;
    VOS_INT16          shwSpProbMeanLast;

    VOS_INT32         *pswPsdBandNearLast;
    VOS_INT32         *pswPsdBandFarLast;
    VOS_INT32         *pswPsdBandEchoLast;
    VOS_INT32         *pswPsdBinNear;
    VOS_INT32         *pswPsdBinFar;
    VOS_INT32         *pswPsdBinEcho;
    VOS_INT32         *pswPsdBandNear;
    VOS_INT32         *pswPsdBandFar;
    VOS_INT32         *pswPsdBandEcho;

    VOS_INT16          shwSpThdMax;
    VOS_INT16          shwSpThdMin;
    VOS_INT16          shwSpThd;

    VOS_INT32          swSumPsd;
    VOS_INT32          swSumPsdLast;
    VOS_INT32          swSumPsdThd;

    MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag;

    shwFreqBinLen = pstDtd->shwFreqBinLen;
    shwBandLen    = pstDtd->shwBandLen;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_DtdNline);

    /* ����֡���� ��Ӧ�жϲ�����ʼ�� */
    if (MED_AEC_DTD_FLAG_PASS == enDtdFlagLine)
    {
        for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
        {
            pstDtd->ashwSpBandProb[shwCnt]  = 0;
            pstDtd->aswPsdBandNear[shwCnt]  = 0;
            pstDtd->aswPsdBandFar[shwCnt]   = 0;
            pstDtd->aswPsdBandEcho[shwCnt]  = 0;
        }

        pstDtd->shwSpProbMean = 0;
        pstDtd->swSumPsd = 0;

        pstDtd->shwSpThd = pstDtd->shwSpThdInit;

        return enDtdFlagLine;
    }

    /* ��̬�ڴ����� */
    pshwNearFreq   = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)pstDtd->enFftLen * sizeof(VOS_INT16));

    pswPsdBinNear  = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBinFar   = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBinEcho  = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBandNear = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT32));
    pswPsdBandFar  = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT32));
    pswPsdBandEcho = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT32));

    pshwSpBandProb = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    /* ��������ź�Ƶ�� */
    CODEC_OpVvAdd(pshwErrFreq,
                pshwEchoFreq,
                pstDtd->enFftLen,
                pshwNearFreq);

    /* ��������źź�Զ���ź��Ӵ������� */
    /* 1. ��������źź�Զ���źŹ����� 32bit Q6 */
    MED_PP_CalcPsdBin(pshwNearFreq, shwFreqBinLen, pswPsdBinNear);
    MED_PP_CalcPsdBin(pshwFarFreq, shwFreqBinLen, pswPsdBinFar);
    MED_PP_CalcPsdBin(pshwEchoFreq, shwFreqBinLen, pswPsdBinEcho);

    /* 2. ������������źź�Զ���ź��Ӵ������� 32bit Q6 */
    MED_PP_CalcBandPsd(pswPsdBinNear, shwBandLen, pswPsdBandNear);
    MED_PP_CalcBandPsd(pswPsdBinFar, shwBandLen, pswPsdBandFar);
    MED_PP_CalcBandPsd(pswPsdBinEcho, shwBandLen, pswPsdBandEcho);

    /* 3. ƽ��������Ӵ�������  32bit Q6 */
    pswPsdBandNearLast = pstDtd->aswPsdBandNear;
    pswPsdBandFarLast  = pstDtd->aswPsdBandFar;
    pswPsdBandEchoLast = pstDtd->aswPsdBandEcho;

    MED_PP_CalcSmoothPsd(pswPsdBandNearLast,
                         shwBandLen,
                         MED_AEC_DTD_NLINE_PSD_BAND_SMOOTH_ALPHA,
                         pswPsdBandNear);

    MED_PP_CalcSmoothPsd(pswPsdBandFarLast,
                         shwBandLen,
                         MED_AEC_DTD_NLINE_PSD_BAND_SMOOTH_ALPHA,
                         pswPsdBandFar);

    MED_PP_CalcSmoothPsd(pswPsdBandEchoLast,
                         shwBandLen,
                         MED_AEC_DTD_NLINE_PSD_BAND_SMOOTH_ALPHA,
                         pswPsdBandEcho);

    /* 4. ���½ṹ���Ӵ������ף�������֡����  32bit Q6 */
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        pstDtd->aswPsdBandNear[shwCnt] = pswPsdBandNear[shwCnt];
        pstDtd->aswPsdBandFar[shwCnt]  = pswPsdBandFar[shwCnt];
        pstDtd->aswPsdBandEcho[shwCnt] = pswPsdBandEcho[shwCnt];
    }

    /* ������Ӵ��������ڸ���  16bit Q15 */
    MED_AEC_DtdSpProb(pstDtd, pswPsdBandNear, pswPsdBandFar, pswPsdBandEcho, pshwSpBandProb);

    /* �����Ӵ��������ڸ���ƽ��ֵ  16bit Q15 */
    swSpProbSum = 0;

    for (shwCnt = 0; shwCnt < MED_AEC_DTD_NLINE_CAL_SP_PROB_BAND_LEN; shwCnt++)
    {
        swSpProbSum = CODEC_OpL_add(swSpProbSum, pshwSpBandProb[shwCnt]);
    }

    shwSpProbMean = CODEC_OpRound(CODEC_OpNormDiv_32(swSpProbSum,
                                                 MED_AEC_DTD_NLINE_CAL_SP_PROB_BAND_LEN,
                                                 16));  /* Q15=Q15-Q0+Q16-Q16 */

    /* ʱ��ƽ���Ӵ��������ڸ���ƽ��ֵ 16bit Q15 */
    shwSpProbMeanLast = pstDtd->shwSpProbMean;

    shwAlpha    = MED_AEC_DTD_NLINE_SP_PROB_MEAN_SMOOTH_ALPHA;
    shwSubAlpha = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_AEC_DTD_NLINE_SP_PROB_MEAN_SMOOTH_ALPHA);

    shwSpProbMean = CODEC_OpAdd(CODEC_OpMult(shwSpProbMeanLast, shwSubAlpha),
                              CODEC_OpMult(shwSpProbMean, shwAlpha));

    pstDtd->shwSpProbMean = shwSpProbMean;


    /* ���㵥˫����־(�����Ӵ��������ڸ���ƽ��ֵ)*/
    /* 1. �����Ӵ��������ڸ���ƽ��ֵ�ж���ֵ 16bit Q15 */
    shwSpThdMax  = pstDtd->shwSpThdMax;
    shwSpThdMin  = pstDtd->shwSpThdMin;
    shwSpThd     = pstDtd->shwSpThd;

    /* 2. �����Ӵ��������ڸ���ƽ��ֵ���㵥˫����־ */
    if (shwSpProbMean < shwSpThd)
    {
        enDtdFlag = MED_AEC_DTD_FLAG_ST;
        shwSpThd  = CODEC_OpAdd(shwSpThd, MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_SMALL); /* ����Ϊ����ʱ��������ֵ��ʹ�����ڵ����б� */
        shwSpThd  = CODEC_OpMin(shwSpThd, shwSpThdMax);
    }
    else
    {
        enDtdFlag = MED_AEC_DTD_FLAG_DT;
        shwSpThd  = CODEC_OpSub(shwSpThd, MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_SMALL); /* ����Ϊ˫��ʱ����С��ֵ��ʹ������˫���б� */
        if (shwSpProbMean > shwSpThdMax)
        {
            shwSpThd = shwSpThd - MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_LARGE;        /* ����Ϊ˫��ʱ������ֵ�ϴ�������˫������ʱ���󲽵�С��ֵ����֤��Ƭ˫�� */
        }
        shwSpThd = CODEC_OpMax(shwSpThd, shwSpThdMin);
    }

    /* 3. �����Ӵ��������ڸ���ƽ��ֵ�ж���ֵ */
    pstDtd->shwSpThd = shwSpThd;

    /* 4. ������˫����־(�����Ӵ�ʩ���Ӵ����ڸ��ʺ�ʣ������)*/
    /* 4.1 ����ʣ������ 32bit Q6 */
    swSumPsd = 0;
    for (shwCnt = 0; shwCnt < MED_AEC_DTD_NLINE_CAL_SP_PROB_BAND_LEN; shwCnt++)
    {
        swSumPsd = CODEC_OpL_add(swSumPsd,
                               CODEC_OpL_mpy_32_16(pswPsdBandNear[shwCnt], pshwSpBandProb[shwCnt])); /* Q6=Q6+Q15-Q15 */
    }

    /* 4.2 ʱ��ƽ��ʣ������ 32bit Q6 */
    swSumPsdLast = pstDtd->swSumPsd;

    shwAlpha    = MED_AEC_DTD_NLINE_SUM_PSD_ALPHA;
    shwSubAlpha = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_AEC_DTD_NLINE_SUM_PSD_ALPHA);

    swSumPsd = CODEC_OpL_add(CODEC_OpL_mpy_32_16(swSumPsdLast, shwSubAlpha),
                           CODEC_OpL_mpy_32_16(swSumPsd, shwAlpha));

    pstDtd->swSumPsd = swSumPsd;


    /* 4.3 ����ʣ��������ֵ���� */
    swSumPsdThd  = pstDtd->swSumPsdThd;

    if (swSumPsd < swSumPsdThd)
    {
        enDtdFlag = MED_AEC_DTD_FLAG_ST;
    }

    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwNearFreq);
    UCOM_MemFree(pswPsdBinNear);
    UCOM_MemFree(pswPsdBinFar);
    UCOM_MemFree(pswPsdBinEcho);
    UCOM_MemFree(pswPsdBandNear);
    UCOM_MemFree(pswPsdBandFar);
    UCOM_MemFree(pswPsdBandEcho);
    UCOM_MemFree(pshwSpBandProb);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_DtdNline);

    return enDtdFlag;

}
VOS_VOID MED_AEC_DtdSpProb(
                MED_AEC_DTD_NLINE_STRU            *pstDtd,
                VOS_INT32                         *pswPsdBandNear,
                VOS_INT32                         *pswPsdBandFar,
                VOS_INT32                         *pswPsdBandEcho,
                VOS_INT16                         *pshwSpBandProb)
{
    VOS_INT16          shwCnt;
    VOS_INT16          shwBandLen;
    VOS_INT16          shwBandLenExt;
    VOS_INT16          shwAlpha;
    VOS_INT16          shwSubAlpha;

    VOS_INT16          shwNearFarRatioActiveFrmCnt;
    VOS_INT16          shwNearFarRatioActiveFrmNum;
    VOS_INT16          shwSpSERThd;
    VOS_INT16          shwEchoSERThd;
    VOS_INT16         *pshwSpBandProbLast;
    VOS_INT16         *pshwSpBandProbTmp;
    VOS_INT32          swBandPsdMuteThd;

    VOS_INT16         *pshwSERModulTab    = MED_AEC_DtdNlineGetSerModulTabPtr();
    VOS_INT16         *pshwProbAlphaCoeff = MED_AEC_DtdNlineGetProbAlphaCoeffPtr();

    VOS_INT16          *pshwPostSER;
    VOS_INT16          *pshwPostNFR;

    /* ��ȡ�ṹ��������� */
    shwBandLen        = pstDtd->shwBandLen;
    shwBandLenExt     = pstDtd->shwBandLen + MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_LEN;

    swBandPsdMuteThd  = CODEC_OpL_shl((VOS_INT32)pstDtd->shwBandPsdMuteThd, MED_PP_SHIFT_BY_6);  /* [16bit Q6] */

    shwNearFarRatioActiveFrmNum = pstDtd->shwNearFarRatioActiveFrmNum;
    shwNearFarRatioActiveFrmCnt = pstDtd->shwNearFarRatioActiveFrmCnt;
    /* ��̬�����ڴ� */
    pshwPostSER       = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    pshwPostNFR       = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    pshwSpBandProbTmp = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwBandLenExt * sizeof(VOS_INT16));

    /* 1. ��������Żر�SER */

    /* ���ݽ���/���ƻ�������SER */
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        pshwPostSER[shwCnt] = CODEC_OpRound(CODEC_OpNormDiv_32(pswPsdBandNear[shwCnt],
                                                           pswPsdBandEcho[shwCnt],
                                                           MED_PP_SHIFT_BY_27));         /* [16bit Q11=Q6-Q6+Q27-Q16] */
    }

    shwNearFarRatioActiveFrmCnt         = CODEC_OpAdd(shwNearFarRatioActiveFrmCnt, 1);
    pstDtd->shwNearFarRatioActiveFrmCnt = shwNearFarRatioActiveFrmCnt;

    /* ��ʹ��Զ���ź����ϼ���SER���ҳ�ʼ��˫��һ��֡��ʱ������SER ashwPostNFR */
    if ((CODEC_SWITCH_ON == pstDtd->enNearFarRatioEnable)
     && (shwNearFarRatioActiveFrmCnt < shwNearFarRatioActiveFrmNum))
    {
        for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
        {
            /* ����NV������Զ���źŹ��ƻ�����С*/
            pswPsdBandFar[shwCnt] = CODEC_OpL_shl(CODEC_OpL_mpy_32_16(pswPsdBandFar[shwCnt],
                                                                  pstDtd->shwNearFarRatioGain),
                                                MED_PP_SHIFT_BY_4);
            /* ��ֹNV������0���³�0 */
            pswPsdBandFar[shwCnt] = CODEC_OpL_max(pswPsdBandFar[shwCnt],
                                                MED_PP_MIN_PSD_VALUE);
            /* �������/Զ���źű�ֵNFR */
            pshwPostNFR[shwCnt] = CODEC_OpRound(CODEC_OpNormDiv_32(pswPsdBandNear[shwCnt],
                                                               pswPsdBandFar[shwCnt],
                                                               MED_PP_SHIFT_BY_27));     /* [16bit Q11=Q6-Q6+Q27-Q16] */

            pshwPostSER[shwCnt] = CODEC_OpMin(pshwPostSER[shwCnt], pshwPostNFR[shwCnt]);
        }
    }

    /* 2. ���ݸ��Ӵ������Żر�SER�����Ӵ��������ڸ��� */
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        /* ������Ӵ������Żر���ֵ [16bit Q11=Q11+Q11-Q15+4]*/
        shwSpSERThd   = CODEC_OpShl(CODEC_OpMult(pshwSERModulTab[shwCnt],
                                             pstDtd->shwSpSerThd),
                                  MED_PP_SHIFT_BY_4);

        shwEchoSERThd = CODEC_OpShl(CODEC_OpMult(pshwSERModulTab[shwCnt],
                                             pstDtd->shwEchoSerThd),
                                  MED_PP_SHIFT_BY_4);

        /* ������ʱ��ֱ���������ڸ���P��0 */
        if (pswPsdBandNear[shwCnt] < swBandPsdMuteThd)
        {
            pshwSpBandProb[shwCnt] = 0;
        }
        else
        {
            if (pshwPostSER[shwCnt] > shwSpSERThd)
            {
                /* SER��ʱ���������ڸ���P��1 */
                pshwSpBandProb[shwCnt] = CODEC_OP_INT16_MAX;   /* 16bit Q15 */
            }
            else if (pshwPostSER[shwCnt] < shwEchoSERThd)
            {
                /* SERСʱ���������ڸ���P��0 */
                pshwSpBandProb[shwCnt] = 0;
            }
            else
            {
                /* SER����ֵ���������ڸ���P��0-1������ӳ�� */
                pshwSpBandProb[shwCnt] = CODEC_OpDiv_s(pshwPostSER[shwCnt]-shwEchoSERThd,  /* [16bit Q15=Q11-Q11+Q15] */
                                                     shwSpSERThd-shwEchoSERThd);
            }
        }
    }

    /* 3. ��С����С�����Ӵ�����������������Ӵ� */
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        /* ����power(x,alpha)��x��0-1��仯��alphaѡ����1�������������ߣ� ����alphaѡ2 */
        pshwSpBandProb[shwCnt] = CODEC_OpMult(pshwSpBandProb[shwCnt],
                                            pshwSpBandProb[shwCnt]); /* 16bit Q15=Q15+Q15-Q15 */
    }

    /* 4. ����ƽ���Ӵ����� */
    UCOM_MemCpy(pshwSpBandProbTmp,
                pshwSpBandProb,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCnt = shwBandLen; shwCnt < shwBandLenExt; shwCnt++)
    {
        pshwSpBandProbTmp[shwCnt] = pshwSpBandProb[shwBandLen-1];
    }

    for (shwCnt = MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_INDEX; shwCnt < shwBandLen; shwCnt++)
    {
        /* 16bit Q15=Q15+Q15-Q15 */
        pshwSpBandProb[shwCnt] = MED_2MIC_WeightSum(pshwSpBandProbTmp,
                                                    pshwProbAlphaCoeff,
                                                    MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_LEN,
                                                    shwCnt-2);
    }

    /* 5. ʱ��ƽ���Ӵ����� */
    pshwSpBandProbLast = pstDtd->ashwSpBandProb;
    shwAlpha    = MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_ALPHA;
    shwSubAlpha = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_ALPHA);
    for (shwCnt = 0; shwCnt < shwBandLen; shwCnt++)
    {
        /* ʱ��ƽ�� */
        pshwSpBandProb[shwCnt] =  CODEC_OpAdd(CODEC_OpMult(pshwSpBandProbLast[shwCnt], shwSubAlpha),
                                            CODEC_OpMult(pshwSpBandProb[shwCnt], shwAlpha));

        pstDtd->ashwSpBandProb[shwCnt] =  pshwSpBandProb[shwCnt];
    }

    /* ��̬�ڴ��ͷ� */
    UCOM_MemFree(pshwPostSER);
    UCOM_MemFree(pshwPostNFR);
    UCOM_MemFree(pshwSpBandProbTmp);

}
VOS_VOID MED_AEC_Dtd(
                MED_AEC_DTD_STRU              *pstDtd,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwFarFreq,
                VOS_INT16                     *pshwError,
                VOS_INT16                     *pshwNear)
{
    VOS_INT16                           shwDtConLen;
    VOS_INT16                           shwDtHangLen;

    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlagPre;
    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlagLine;
    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlagNline;
    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlagFix;

    MED_AEC_DTD_LINE_STRU               *pstDtdLine;
    MED_AEC_DTD_NLINE_STRU              *pstDtdNline;

    enDtdFlagPre = pstDtd->enDtdFlag;
    pstDtdLine   = &(pstDtd->stDtdLine);
    pstDtdNline  = &(pstDtd->stDtdNline);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_Dtd);

    /* 1. ����DTD�ж� */
    enDtdFlagLine = MED_AEC_DtdLine(pstDtdLine,
                                    enDtdFlagPre,
                                    pshwErrFreq,
                                    pshwEchoFreq,
                                    pshwError,
                                    pshwNear);


    /* 2. ������DTD�ж� */
    enDtdFlagNline = MED_AEC_DtdNline(pstDtdNline,
                                      enDtdFlagLine,
                                      pshwErrFreq,
                                      pshwEchoFreq,
                                      pshwFarFreq);

    /* 3. ��������DTD��˫���������� */
    if (MED_AEC_DTD_FLAG_DT == enDtdFlagLine)
    {
        enDtdFlagFix = enDtdFlagNline;
    }
    else
    {
        enDtdFlagFix = enDtdFlagLine;
    }

    /* 4. ˫��hangover����:����˫���������˫��hangover */
    shwDtConLen = pstDtd->shwDtConLen;

    if (enDtdFlagFix == MED_AEC_DTD_FLAG_DT)
    {
        shwDtConLen = shwDtConLen + 1;
    }
    else
    {
        shwDtConLen = 0;
    }

    /* ������˫��֡��������ֵ�������˫��hangover */
    shwDtHangLen = pstDtd->shwDtHangLen;
    if (shwDtConLen > MED_AEC_DTD_NLINE_DT_CONTINUOUS_LEN)
    {
        shwDtHangLen = MED_AEC_DTD_NLINE_DT_HANGOVER_LEN;
    }

    if (enDtdFlagFix == MED_AEC_DTD_FLAG_PASS)
    {
        shwDtHangLen = 0;
    }
    else
    {
        if (shwDtHangLen > 0)
        {
            enDtdFlagFix = MED_AEC_DTD_FLAG_DT;
            shwDtHangLen = shwDtHangLen - 1;
        }
    }


    /* ����˫��hangover�ṹ����� */
    pstDtd->shwDtConLen = shwDtConLen;
    pstDtd->shwDtHangLen = shwDtHangLen;

    /* 5. ��ǿ�������ܴ�ʱ,����Զ��������С��־ǿ���жϵ��� */
    if (CODEC_SWITCH_ON == pstDtd->enEstFlag)
    {
        if (1 == pstDtd->shwFarLargeFlag)
        {
            enDtdFlagFix = MED_AEC_DTD_FLAG_ST;
        }
    }

    /* 6. �����жϱ�־��ֵ */
    pstDtd->enDtdFlag = enDtdFlagFix;

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_Dtd);

}


#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif

