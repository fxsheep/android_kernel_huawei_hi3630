

/*****************************************************************************
   1 ͷ�ļ�����
*****************************************************************************/
#include "med_aec_af.h"
#include "ucom_comm.h"
#include "om.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#ifdef _MED_C89_
/*****************************************************************************
   2 ȫ�ֱ�������
*****************************************************************************/
UCOM_ALIGN(4)
VOS_INT16                       g_ashwMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];
UCOM_ALIGN(4)
VOS_INT16                       g_ashwMedAecTmp2Len640[MED_AEC_MAX_FFT_LEN];

VOS_INT32                       g_aswMedAecTmp1Len320[CODEC_FRAME_LENGTH_WB];
VOS_INT32                       g_aswMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];

/*****************************************************************************
   3 ����ʵ��
*****************************************************************************/


VOS_VOID MED_AEC_AfInit(
                MED_AEC_NV_STRU                *pstPara,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                MED_AEC_AF_STRU                *pstAf)
{
    VOS_INT16   shwAfFilterLen;

    UCOM_MemSet((VOS_VOID*)pstAf, 0, sizeof(MED_AEC_AF_STRU));

    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        pstAf->enFftLen          = MED_AEC_MAX_FFT_LEN / 2;
        pstAf->enFftNumIndex     = MED_FFT_NUM_320;
        pstAf->enIfftNumIndex    = MED_IFFT_NUM_320;

        /* 8k������ÿms����8�㼴����3λ */
        shwAfFilterLen           = shl(pstPara->shwAfTailLen, MED_PP_SHIFT_BY_3);
    }
    else
    {
        pstAf->enFftLen          = MED_AEC_MAX_FFT_LEN;
        pstAf->enFftNumIndex     = MED_FFT_NUM_640;
        pstAf->enIfftNumIndex    = MED_IFFT_NUM_640;

        /* 16k������ÿms����16�㼴����4λ */
        shwAfFilterLen           = shl(pstPara->shwAfTailLen, MED_PP_SHIFT_BY_4);
    }

    pstAf->stConfig.enAfEnable   = pstPara->enAecEnable;
    pstAf->stConfig.shwTailLen   = pstPara->shwAfTailLen;
    pstAf->shwSampleRate         = (VOS_INT16)enSampleRate;
    pstAf->shwFrameLen           = (VOS_INT16)MED_PP_GetFrameLength();

    pstAf->shwM = (shwAfFilterLen + (pstAf->shwFrameLen -1)) / (pstAf->enFftLen/2);
    pstAf->shwFarPowerAlpha = MED_AEC_AF_XPOW_THR / pstAf->shwM;

    pstAf->swFarPow              = 1;
    pstAf->swEchoPow             = 1;
    pstAf->stCompLeakState.swPyy = MED_AEC_AF_POWX_DITH;
    pstAf->stCompLeakState.swPey = MED_AEC_AF_POWX_DITH;

}


VOS_VOID MED_AEC_AfMain (
                MED_AEC_AF_STRU               *pstAfObj,
                VOS_INT16                     *pshwNear,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag,
                VOS_INT16                     *pshwErr)

{
    VOS_INT32                           swCnt;
    VOS_INT32                           swCurrDiffVar;

    VOS_INT16                          *pashwTmp1;
    VOS_INT16                           shwUpdateFore;
    VOS_INT16                           shwUpdateBack;

    /* ���PASS��ʹ�ܣ���AEC�����д���ֱͨ�ź� */
    if (   (MED_AEC_DTD_FLAG_PASS == enDtdFlag)
        || (CODEC_SWITCH_OFF == pstAfObj->stConfig.enAfEnable))
    {
        CODEC_OpVecCpy(pshwErr, pshwNear, pstAfObj->shwFrameLen);
        return;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_AfMain);

    CODEC_OpVecCpy(&pstAfObj->ashwInput[0],
                  pshwNear,
                  pstAfObj->shwFrameLen);

    pstAfObj->swCancelCount = CODEC_OpL_add(pstAfObj->swCancelCount, 1);

    pashwTmp1 = MED_AEC_GetshwVecTmp640Ptr1();

    /* ����Զ��Ƶ������ */
    for(swCnt = (VOS_INT32)pstAfObj->shwM; swCnt > 0; swCnt--)
    {
        CODEC_OpVecCpy(pstAfObj->ashwFarFreq[swCnt],
            pstAfObj->ashwFarFreq[swCnt-1],
            pstAfObj->enFftLen);
    }

    /* ����Զ��ʱ������ */
    CODEC_OpVecCpy(pstAfObj->ashwLastFar,
                 pstAfObj->ashwLastFar + pstAfObj->shwFrameLen,
                 pstAfObj->shwFrameLen);
    CODEC_OpVecCpy(pstAfObj->ashwLastFar + pstAfObj->shwFrameLen,
                 pstAfObj->ashwFar,
                 pstAfObj->shwFrameLen);

    MED_FFT_Fft(pstAfObj->enFftNumIndex, pstAfObj->ashwLastFar, &pstAfObj->ashwFarFreq[0][0]);

    /* ǰ�˲�: ������һ���˲�ϵ�������˲���� */
    MED_AEC_AfFilt(pstAfObj->ashwFarFreq ,
                   pstAfObj->aswForeWeight,
                   pstAfObj->ashwInput,
                   pstAfObj->shwM,
                   pstAfObj->enFftLen,
                   pstAfObj->enIfftNumIndex,
                   pstAfObj->shwFrameLen,
                   pstAfObj->ashwForeEcho,
                   pstAfObj->ashwForeErr,
                   &pstAfObj->swForeErrPow);

    /* ��������Ӧ�� */
    MED_AEC_AfUpdateProp(pstAfObj->aswWeight,
                         pstAfObj->shwM,
                         pstAfObj->enFftLen,
                         pstAfObj->ashwProp);

    /* ���º��˲���ϵ�� */
    MED_AEC_AfUpdateWeight(pstAfObj);

    /* ���˲� */
    MED_AEC_AfFilt(pstAfObj->ashwFarFreq ,
                   pstAfObj->aswWeight,
                   pstAfObj->ashwInput,
                   pstAfObj->shwM,
                   pstAfObj->enFftLen,
                   pstAfObj->enIfftNumIndex,
                   pstAfObj->shwFrameLen,
                   pstAfObj->ashwEcho,
                   pstAfObj->ashwErr,
                   &pstAfObj->swErrPow);

    /* �Ƚ�ǰ���˲���������˲���ϵ�������Ƿ���ʣ�������������Լ���������ο�ֵ */
    CODEC_OpVvSub(pstAfObj->ashwForeEcho,
                pstAfObj->ashwEcho,
                pstAfObj->shwFrameLen,
                pashwTmp1);
    swCurrDiffVar = MED_AEC_AF_DIV_DITH + MED_AEC_AfFrmPower(pashwTmp1,
                                            pashwTmp1,
                                            pstAfObj->shwFrameLen,
                                            MED_PP_SHIFT_BY_6);                 /* ����ǰ���˲�������Ĳ�� */
    MED_AEC_AfCmpTwoFilter(&pstAfObj->stKeeper,
                           swCurrDiffVar,
                           pstAfObj->swForeErrPow ,
                           pstAfObj->swErrPow,
                          &shwUpdateFore,
                          &shwUpdateBack);
    MED_AEC_AfUpdateTwoFilter(pstAfObj, shwUpdateFore, shwUpdateBack);

    /* ����Ӧ�˲���� */
    CODEC_OpVvSub(pstAfObj->ashwInput,
                pstAfObj->ashwForeEcho,
                pstAfObj->shwFrameLen,
                pshwErr);

    /* �˲�����λ�о� */
    pstAfObj->swFarPow      = MED_AEC_AfFrmPower(pstAfObj->ashwFar,
                                                 pstAfObj->ashwFar,
                                                 pstAfObj->shwFrameLen,
                                                 MED_PP_SHIFT_BY_6);            /* ����Զ���ź����� */
    pstAfObj->swEchoPow     = MED_AEC_AfFrmPower(pstAfObj->ashwEcho,
                                                 pstAfObj->ashwEcho,
                                                 pstAfObj->shwFrameLen,
                                                 MED_PP_SHIFT_BY_6);            /* ���˲����ز������ź�����أ������� */
    pstAfObj->swBEchoErrPow = MED_AEC_AfFrmPower(pstAfObj->ashwErr,
                                                 pstAfObj->ashwEcho,
                                                 pstAfObj->shwFrameLen,
                                                 MED_PP_SHIFT_BY_6);            /* ���˲��������źźͺ��˲����в��źŻ���� */
    pstAfObj->swNearPow     = MED_AEC_AfFrmPower(pshwNear,
                                                 pshwNear,
                                                 pstAfObj->shwFrameLen,
                                                 MED_PP_SHIFT_BY_6);            /* ���������ź����� */

    /* �˲�����λ�о� */
    MED_AEC_AfResetJudge(pstAfObj, pshwErr);

    /* ����Ƶ���źź͹����� */
    CODEC_OpVecSet(pashwTmp1, MED_AEC_MAX_FFT_LEN, 0);
    CODEC_OpVecCpy(&pashwTmp1[pstAfObj->shwFrameLen],
                 pstAfObj->ashwErr,
                 pstAfObj->shwFrameLen);

    MED_FFT_Fft(pstAfObj->enFftNumIndex, pashwTmp1, pstAfObj->ashwErrFreq);

    CODEC_OpVecSet(pashwTmp1, MED_AEC_MAX_FFT_LEN, 0);
    CODEC_OpVecCpy(pashwTmp1 + pstAfObj->shwFrameLen,
                 pstAfObj->ashwEcho,
                 pstAfObj->shwFrameLen);
    MED_FFT_Fft(pstAfObj->enFftNumIndex, pashwTmp1, pstAfObj->ashwEchoFreq);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_AfMain);

}
VOS_VOID MED_AEC_AfMainUpdate(
                MED_AEC_AF_STRU               *pstAfObj,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag)
{
    VOS_INT32               swTmp;

    /* ���PASS��ʹ�ܣ���AEC�����д���ֱͨ�ź� */
    if (   (MED_AEC_DTD_FLAG_PASS == enDtdFlag)
        || (0 == pstAfObj->stConfig.enAfEnable))
    {
        return;
    }

    /* ������ź�������ӵ����������Է�ֹ���г���ʱ�����쳣��*/
    swTmp = CODEC_OpL_shr_r(CODEC_OpL_mult0(pstAfObj->enFftLen, MED_AEC_AF_100),
                                        MED_PP_SHIFT_BY_6);
    pstAfObj->swErrPow = CODEC_OpL_max(pstAfObj->swErrPow, swTmp);

    /* ����Ƶ���źź͹����� */
    MED_AEC_AfPowerSpectrum(pstAfObj->ashwErrFreq,
                            pstAfObj->enFftLen,
                            pstAfObj->aswErrPsd);
    MED_AEC_AfPowerSpectrum(pstAfObj->ashwEchoFreq,
                            pstAfObj->enFftLen,
                            pstAfObj->aswEchoPsd);

    /* Զ���ź�ƽ�������� */
    MED_AEC_AfCompSmoothPower(&pstAfObj->ashwFarFreq[0][0],
                              &pstAfObj->aswFarSmoothPsd[0],
                              pstAfObj->shwFarPowerAlpha,
                              pstAfObj->enFftLen);

    /* ����й¶���� */
    MED_AEC_AfCompLeak(pstAfObj);

    /* ��������ѧϰ�� */
    MED_AEC_AfCompRER(pstAfObj->swFarPow,
                      pstAfObj->swErrPow,
                      pstAfObj->swEchoPow ,
                      pstAfObj->shwLeakEstimate,
                      pstAfObj->swBEchoErrPow,
                      &pstAfObj->stStepState.shwRER);

    /* ��������Ӧ�˲������� */
    MED_AEC_AfCompStep(pstAfObj);

}
VOS_VOID MED_AEC_AfFilt(
                VOS_INT16  ashwFarFreq[][MED_AEC_AF_FILT_WB],
                VOS_INT32  aswWeight[][MED_AEC_AF_FILT_WB],
                VOS_INT16 *pshwInput,
                VOS_INT16  shwM,
                VOS_INT16  enFftLen,
                VOS_INT16  shwIfftNumIndex,
                VOS_INT16  shwFrameLen,
                VOS_INT16 *pshwEcho,
                VOS_INT16 *pshwErr,
                VOS_INT32 *pswErrPow)
{
    VOS_INT16                 *pashwTmp1;

    pashwTmp1                  = MED_AEC_GetshwVecTmp640Ptr1();

    MED_AEC_AfSpectralMulAccum(ashwFarFreq,
                               aswWeight,
                               shwM,
                               enFftLen,
                               pashwTmp1);

    MED_FFT_Ifft(shwIfftNumIndex, pashwTmp1, pashwTmp1);

    /* ȡ����ǰʱ��֡���˲������ */
    CODEC_OpVecCpy(pshwEcho,
                 pashwTmp1 + shwFrameLen,
                 shwFrameLen);

    CODEC_OpVvSub(pshwInput,
                pshwEcho,
                shwFrameLen,
                pshwErr);

    *pswErrPow   = MED_AEC_AfFrmPower(pshwErr,
                                     pshwErr,
                                     shwFrameLen,
                                     MED_PP_SHIFT_BY_6);

}
 VOS_VOID MED_AEC_AfSpectralMulAccum(
                VOS_INT16   ashwFarFreq[][MED_AEC_AF_FILT_WB],
                VOS_INT32   aswFilterCoeff[][MED_AEC_AF_FILT_WB],
                VOS_INT16   shwM,
                VOS_INT16   enFftLen,
                VOS_INT16  *pshwEchoFreqOut)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           swCntJ;
    VOS_INT32                           swTemp1;
    VOS_INT32                          *pswEchoFreq;

    pswEchoFreq                  = MED_AEC_GetswVecTmp640Ptr1();

    CODEC_OpVecSet((VOS_INT16*)pswEchoFreq, (VOS_INT32)(enFftLen*2), 0);

    for (swCntJ = 0; swCntJ < shwM; swCntJ++)
    {
        swTemp1        = CODEC_OpL_mpy_32_16(aswFilterCoeff[swCntJ][0],
                                           ashwFarFreq[swCntJ][0]);
        pswEchoFreq[0]= CODEC_OpL_add(swTemp1, pswEchoFreq[0]);                 /* ֱ������ */

        swTemp1        = CODEC_OpL_mpy_32_16(aswFilterCoeff[swCntJ][1],
                                           ashwFarFreq[swCntJ][1]);
        pswEchoFreq[1]= CODEC_OpL_add(swTemp1, pswEchoFreq[1]);
    }

    for (swCntI = 2; swCntI < (enFftLen - 1); swCntI = swCntI + 2)
    {
        for (swCntJ = 0; swCntJ < shwM; swCntJ++)
        {
            swTemp1 = CODEC_OpL_sub(CODEC_OpL_mpy_32_16(aswFilterCoeff[swCntJ][swCntI],
                                                    ashwFarFreq[swCntJ][swCntI]),
                                  CODEC_OpL_mpy_32_16(aswFilterCoeff[swCntJ][swCntI + 1],
                                                    ashwFarFreq[swCntJ][swCntI + 1]));
            pswEchoFreq[swCntI]= CODEC_OpL_add(swTemp1, pswEchoFreq[swCntI]); /* ʵ�� */

            swTemp1 = CODEC_OpL_add(CODEC_OpL_mpy_32_16(aswFilterCoeff[swCntJ][swCntI],
                                                    ashwFarFreq[swCntJ][swCntI + 1]),
                                  CODEC_OpL_mpy_32_16(aswFilterCoeff[swCntJ][swCntI + 1],
                                                    ashwFarFreq[swCntJ][swCntI]));
            pswEchoFreq[swCntI + 1]= CODEC_OpL_add(swTemp1,pswEchoFreq[swCntI + 1]);/* �鲿 */
        }
    }

    CODEC_OpVecShr_r32(pswEchoFreq, (VOS_INT32)enFftLen, MED_AEC_AF_W_QN_FILT, pswEchoFreq);

    /* ���ʹ��� */
    for (swCntI = 0; swCntI < enFftLen; swCntI++)
    {
        pshwEchoFreqOut[swCntI] = CODEC_OpSaturate(pswEchoFreq[swCntI]);
    }
}


VOS_VOID MED_AEC_AfUpdateProp(
                VOS_INT32  aswW[][MED_AEC_AF_FILT_WB],
                VOS_INT16  shwM,
                VOS_INT16  shwWeightLen,
                VOS_INT16 *pshwProp)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           swCntJ;
    VOS_INT16                           ashwRms[MED_AEC_AF_M_MAX] = {0};
    VOS_INT16                           ashwTmp[MED_AEC_AF_M_MAX] = {0};
    VOS_INT32                           swWeightSum;
    VOS_INT16                           shwMaxSum;
    VOS_INT32                           swPropSum;
    VOS_INT16                           shwWeightQ;

    for (swCntI = 0; swCntI < shwM; swCntI++)
    {
        swWeightSum     = 1;
        for (swCntJ = 0; swCntJ < shwWeightLen; swCntJ++)
        {
            shwWeightQ = (VOS_INT16)CODEC_OpL_shr(aswW[swCntI][swCntJ], MED_AEC_AF_W_QN_PROP);
            swWeightSum = CODEC_OpL_add(swWeightSum, CODEC_OpL_mult0(shwWeightQ, shwWeightQ));
        }
        ashwRms[swCntI]= CODEC_OpSqrt(swWeightSum);
    }

    shwMaxSum = 1;
    for (swCntI = 0; swCntI < shwM; swCntI++)
    {
        shwMaxSum = CODEC_OpMax(ashwRms[swCntI], shwMaxSum);
    }

    swPropSum     = 1;

    for (swCntI = 0; swCntI < shwM; swCntI++)
    {
        ashwTmp[swCntI] = CODEC_OpAdd(ashwRms[swCntI],
                                     CODEC_OpMult_r(shwMaxSum,MED_AEC_AF_0_POINT_1_Q15));
        swPropSum        = CODEC_OpL_add(swPropSum, CODEC_OpL_deposit_l(ashwTmp[swCntI]));
    }

    for (swCntI = 0; swCntI < shwM; swCntI++)
    {
        pshwProp[swCntI]
            = CODEC_OpSaturate(CODEC_OpL_mult0(ashwTmp[swCntI], MED_AEC_AF_0_POINT_99_Q15) / swPropSum);
    }

}


VOS_VOID MED_AEC_AfUpdateWeight(
                MED_AEC_AF_STRU         *pstAf)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           swCntJ;
    VOS_INT32                           swTmp1;
    VOS_INT32                           swTmp2;
    VOS_INT32                           swWeightMax;
    VOS_INT32                           swWeightTmp;
    VOS_INT16                           shwWeightMaxPower;
    VOS_INT16                           shwFftLen;
    VOS_INT16                          *pashwWtmp;
    VOS_INT16                           shwInMax     = 0;
    VOS_INT16                           shwNormShift = 0;
    VOS_INT16                          *pswMaxPos    = MED_NULL;

    shwFftLen  = pstAf->enFftLen;
    pashwWtmp = MED_AEC_GetshwVecTmp640Ptr2();

    /* ��������˲���ϵ�� */
    for (swCntI = pstAf->shwM - 1; swCntI >= 0; swCntI--)
    {
        swTmp1 = CODEC_OpL_mult0(pstAf->ashwFarFreq[swCntI + 1][0], pstAf->ashwErrFreq[0]);
        swTmp2 = CODEC_OpL_mpy_32_16(pstAf->stStepState.aswStep[0],
                                   pstAf->ashwProp[swCntI]);

        pstAf->aswWeight[swCntI][0]
               = CODEC_OpL_add(pstAf->aswWeight[swCntI][0],
                             CODEC_OpFloatTo32(CODEC_OpFloatAMul32U(swTmp1, swTmp2)));

        swTmp1 = CODEC_OpL_mult0(pstAf->ashwFarFreq[swCntI + 1][1], pstAf->ashwErrFreq[1]);

        swTmp2 = CODEC_OpL_mpy_32_16(pstAf->stStepState.aswStep[shwFftLen >> 1],
                                   pstAf->ashwProp[swCntI]);

        pstAf->aswWeight[swCntI][1]
               = CODEC_OpL_add(pstAf->aswWeight[swCntI][1],
                             CODEC_OpFloatTo32(CODEC_OpFloatAMul32U(swTmp1,  swTmp2)));

        for (swCntJ = 1; swCntJ <= ((shwFftLen >> 1) - 1); swCntJ++)
        {

            swTmp1 = CODEC_OpL_add(CODEC_OpL_mult0(pstAf->ashwFarFreq[swCntI + 1][2 * swCntJ],
                                               pstAf->ashwErrFreq[2 * swCntJ]),
                                 CODEC_OpL_mult0(pstAf->ashwFarFreq[swCntI + 1][(2 * swCntJ) + 1],
                                               pstAf->ashwErrFreq[(2 * swCntJ) + 1]));
            swTmp2 = CODEC_OpL_mpy_32_16(pstAf->stStepState.aswStep[swCntJ],
                                       pstAf->ashwProp[swCntI]);

            pstAf->aswWeight[swCntI][2 * swCntJ]
                   = CODEC_OpL_add(pstAf->aswWeight[swCntI][2 * swCntJ],
                                 CODEC_OpFloatTo32(CODEC_OpFloatAMul32U(swTmp1, swTmp2)));

            swTmp1 = CODEC_OpL_sub(CODEC_OpL_mult0(pstAf->ashwFarFreq[swCntI + 1][2 * swCntJ],
                                               pstAf->ashwErrFreq[(2 * swCntJ) + 1]),
                                 CODEC_OpL_mult0(pstAf->ashwFarFreq[swCntI + 1][(2 * swCntJ) + 1],
                                               pstAf->ashwErrFreq[2 * swCntJ]));
            swTmp2 = CODEC_OpL_mpy_32_16(pstAf->stStepState.aswStep[swCntJ],
                                       pstAf->ashwProp[swCntI]);

            pstAf->aswWeight[swCntI][(2 * swCntJ) + 1]
                   = CODEC_OpL_add(pstAf->aswWeight[swCntI][(2 * swCntJ) + 1],
                                 CODEC_OpFloatTo32(CODEC_OpFloatAMul32U(swTmp1, swTmp2)));

        }
    }

    /* ȥ��ѭ���������(��ϸԭ��μ�AUMDF�㷨) */
    for (swCntI = 0; swCntI < pstAf->shwM; swCntI++)
    {
        if (   (0 == swCntI)
            || (CODEC_OpMod32(pstAf->swCancelCount, pstAf->shwM - 1) == (swCntI - 1)))
        {

            swWeightMax = 2;

            for (swCntJ = 0; swCntJ < pstAf->enFftLen; swCntJ++)
            {
                swWeightTmp = L_abs(pstAf->aswWeight[swCntI][swCntJ]);

                if (swWeightTmp > swWeightMax)
                {
                    swWeightMax = swWeightTmp;
                }
            }

            shwWeightMaxPower = 31 - CODEC_OpNorm_l(swWeightMax);

            if (shwWeightMaxPower > MED_AEC_AF_WEIGHT_POWER_THD)
            {
                for (swCntJ = 0; swCntJ < pstAf->enFftLen; swCntJ++)
                {
                    pashwWtmp[swCntJ] = CODEC_OpSaturate(CODEC_OpL_shr_r(pstAf->aswWeight[swCntI][swCntJ], shwWeightMaxPower - MED_AEC_AF_WEIGHT_POWER_THD));
                }
            }
            else
            {
                for (swCntJ = 0; swCntJ < pstAf->enFftLen; swCntJ++)
                {
                    pashwWtmp[swCntJ] = CODEC_OpSaturate(pstAf->aswWeight[swCntI][swCntJ]);
                }
            }

            MED_FFT_Ifft(pstAf->enIfftNumIndex, pashwWtmp, pashwWtmp);

            CODEC_OpVecSet(pashwWtmp + pstAf->shwFrameLen, pstAf->shwFrameLen, 0);

            /* ��һ��*/
            shwInMax       = CODEC_OpVecMaxAbs(pashwWtmp,
                                               pstAf->enFftLen,
                                               pswMaxPos);

            shwNormShift   = CODEC_OpNorm_s(shwInMax);

            CODEC_OpVecShl(pashwWtmp,
                           pstAf->enFftLen,
                           shwNormShift,
                           pashwWtmp);

            MED_FFT_Fft(pstAf->enFftNumIndex, pashwWtmp, pashwWtmp);

            /* ����һ��*/
            CODEC_OpVecShr_r(pashwWtmp,
                             pstAf->enFftLen,
                             shwNormShift,
                             pashwWtmp);

            if (shwWeightMaxPower > MED_AEC_AF_WEIGHT_POWER_THD)
            {
                for (swCntJ = 0; swCntJ < pstAf->enFftLen; swCntJ++)
                {
                    pstAf->aswWeight[swCntI][swCntJ] = (VOS_INT32)pashwWtmp[swCntJ];
                    pstAf->aswWeight[swCntI][swCntJ] = CODEC_OpL_shl(pstAf->aswWeight[swCntI][swCntJ], shwWeightMaxPower - MED_AEC_AF_WEIGHT_POWER_THD);
                }
            }
        }
    }
}


VOS_VOID MED_AEC_AfCmpTwoFilter(
                MED_AEC_AF_KEEPER_STRU  *pstKeeper,
                VOS_INT32                swCurrDiffVar,
                VOS_INT32                swForeErrPow ,
                VOS_INT32                swErrPow,
                VOS_INT16               *pshwUpdateFore,
                VOS_INT16               *pshwUpdateBack)
{
    VOS_INT16               shwUpdateFore = 0;
    VOS_INT16               shwUpdateBack = 0;
    CODEC_OP_FLOAT_STRU       stFloatDbf;
    CODEC_OP_FLOAT_STRU       stFloatVar1Smooth;
    CODEC_OP_FLOAT_STRU       stFloatVar2Smooth;
    CODEC_OP_FLOAT_STRU       stFloatVarBacktrack;
    CODEC_OP_FLOAT_STRU       stFloatVar1Update;
    CODEC_OP_FLOAT_STRU       stFloatVar2Update;
    VOS_INT16               shwUpdateForeground;
    VOS_INT16               shwResetBackground;

    /* ������ʱ�䴰���������������˲�������ź�������ľ�ֵDavg�ͷ���Dvar */
    pstKeeper->swDiffAvg1
        = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstKeeper->swDiffAvg1, MED_AEC_AF_DAVG1_COEF1),
                      CODEC_OpL_mpy_32_16((swForeErrPow - swErrPow), MED_AEC_AF_DAVG1_COEF2));
    pstKeeper->swDiffAvg2
        = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstKeeper->swDiffAvg2, MED_AEC_AF_DAVG2_COEF1),
                      CODEC_OpL_mpy_32_16((swForeErrPow - swErrPow), MED_AEC_AF_DAVG2_COEF2));

    stFloatVar1Smooth.shwM    = MED_AEC_AF_DVAR1_COEF1_M;
    stFloatVar1Smooth.shwE    = MED_AEC_AF_DVAR1_COEF1_E;
    stFloatVar2Smooth.shwM    = MED_AEC_AF_DVAR2_COEF1_M;
    stFloatVar2Smooth.shwE    = MED_AEC_AF_DVAR2_COEF1_E;

    stFloatVar1Update.shwM    = MED_AEC_AF_VAR1_THD_M;                          /* 0.5, ˫�˲���ѡ����ֵ1 */
    stFloatVar1Update.shwE    = MED_AEC_AF_VAR1_THD_E;
    stFloatVar2Update.shwM    = MED_AEC_AF_VAR2_THD_M;                          /* 0.25, ˫�˲���ѡ����ֵ2 */
    stFloatVar2Update.shwE    = MED_AEC_AF_VAR2_THD_E;
    stFloatVarBacktrack.shwM  = MED_AEC_AF_BACKTRACK_THD_M;                     /* 4, ˫�˲���ѡ����ֵ3 */
    stFloatVarBacktrack.shwE  = MED_AEC_AF_BACKTRACK_THD_E;

    stFloatDbf = CODEC_OpToFloat(swCurrDiffVar);

    pstKeeper->stFloatDiffVar1
        = CODEC_OpFloatAdd(CODEC_OpFloatMul(stFloatVar1Smooth, pstKeeper->stFloatDiffVar1),
                         CODEC_OpFloatMul(CODEC_OpToFloat(CODEC_OpL_mpy_32_16(swForeErrPow,
                         MED_AEC_AF_DVAR1_COEF2)), stFloatDbf));
    pstKeeper->stFloatDiffVar2
        = CODEC_OpFloatAdd(CODEC_OpFloatMul(stFloatVar2Smooth, pstKeeper->stFloatDiffVar2),
                         CODEC_OpFloatMul(CODEC_OpToFloat(CODEC_OpL_mpy_32_16(swForeErrPow,
                         MED_AEC_AF_DVAR2_COEF2)), stFloatDbf));

    /* ��������˲������������ܺá��������˲���ϵ�����±����˲��� */
    /* �˴�double talk��⣬ Sff�����˲�������ź�������SeeΪ���˲�������ź����� */
    shwUpdateForeground = VOS_FALSE;
    if  (CODEC_OpFloatLT(CODEC_OpFloatMul(CODEC_OpToFloat(swForeErrPow), stFloatDbf),
                        CODEC_OpFloatMul(CODEC_OpToFloat(swForeErrPow - swErrPow),
                                       CODEC_OpToFloat(CODEC_OpL_abs(swForeErrPow-swErrPow)))))
    {
        shwUpdateForeground = VOS_TRUE;                                                /* ST, DbfΪ�����˲�����źŲ�ķ��� */
    }
    else if (CODEC_OpFloatLT(CODEC_OpFloatMul(stFloatVar1Update,pstKeeper->stFloatDiffVar1),
                            CODEC_OpFloatMul(CODEC_OpToFloat(pstKeeper->swDiffAvg1),
                            CODEC_OpToFloat(CODEC_OpL_abs(pstKeeper->swDiffAvg1)))))
    {
        shwUpdateForeground = VOS_TRUE;
    }
    else if (CODEC_OpFloatLT(CODEC_OpFloatMul(stFloatVar2Update,pstKeeper->stFloatDiffVar2),
                            CODEC_OpFloatMul(CODEC_OpToFloat(pstKeeper->swDiffAvg2),
                            CODEC_OpToFloat(CODEC_OpL_abs(pstKeeper->swDiffAvg2)))))
    {
        shwUpdateForeground = VOS_TRUE;
    }
    else
    {

    }

    /* ST���൱��single talk״̬ */
    if (1 == shwUpdateForeground)
    {
        pstKeeper->swDiffAvg1           = 0;                                    /* �ź�������ľ�ֵ1 */
        pstKeeper->swDiffAvg2           = 0;                                    /* �ź�������ľ�ֵ2 */
        pstKeeper->stFloatDiffVar1.shwM = 0;                                    /* �ź�������ľ�ֵ1 */
        pstKeeper->stFloatDiffVar1.shwE = 0;
        pstKeeper->stFloatDiffVar2.shwM = 0;                                    /*  �ź�������ľ�ֵ2 */
        pstKeeper->stFloatDiffVar2.shwE = 0;
        shwUpdateFore                   = 1;
    }
    /* DT���൱��double talk״̬, ������˲����Ƿ�ɢ�������ɢ����reset���˲��� */
    else
    {
        /* ��������˲������������ܲ���ñ����˲�������reset���˲��� */
        shwResetBackground = VOS_FALSE;

        if (CODEC_OpFloatLT(CODEC_OpFloatAmult(stFloatVarBacktrack,
                           CODEC_OpFloatAmult(CODEC_OpToFloat(swForeErrPow),stFloatDbf)),
                           CODEC_OpFloatAmult(CODEC_OpToFloat(swErrPow - swForeErrPow),
                           CODEC_OpToFloat(CODEC_OpL_abs(swForeErrPow - swErrPow)))))
        {
            shwResetBackground = VOS_TRUE;
        }

        if (CODEC_OpFloatLT(CODEC_OpFloatAmult(stFloatVarBacktrack, pstKeeper->stFloatDiffVar1),
                           CODEC_OpFloatAmult(CODEC_OpToFloat(CODEC_OpL_negate(pstKeeper->swDiffAvg1)),
                           CODEC_OpToFloat(CODEC_OpL_abs(pstKeeper->swDiffAvg1)))))
        {
            shwResetBackground = VOS_TRUE;
        }

        if (CODEC_OpFloatLT(CODEC_OpFloatAmult(stFloatVarBacktrack , pstKeeper->stFloatDiffVar2),
                           CODEC_OpFloatAmult(CODEC_OpToFloat(CODEC_OpL_negate(pstKeeper->swDiffAvg2)),
                           CODEC_OpToFloat(CODEC_OpL_abs(pstKeeper->swDiffAvg2)))))
        {
            shwResetBackground = VOS_TRUE;
        }

        if (1 == shwResetBackground)
        {
            pstKeeper->swDiffAvg1           = 0;                                /* �ź�������ľ�ֵ1 */
            pstKeeper->swDiffAvg2           = 0;                                /* �ź�������ľ�ֵ2 */
            pstKeeper->stFloatDiffVar1.shwM = 0;                                /* �ź�������ľ�ֵ1 */
            pstKeeper->stFloatDiffVar1.shwE = 0;
            pstKeeper->stFloatDiffVar2.shwM = 0;                                /*  �ź�������ľ�ֵ2 */
            pstKeeper->stFloatDiffVar2.shwE = 0;
            shwUpdateBack                   = 1;
        }
    }

    *pshwUpdateBack = shwUpdateBack;
    *pshwUpdateFore = shwUpdateFore;

}


VOS_VOID MED_AEC_AfUpdateTwoFilter(
                MED_AEC_AF_STRU *pstMedAecAf,
                VOS_INT16        shwUpdateFore,
                VOS_INT16        shwUpdateBack)
{
    if (VOS_TRUE == shwUpdateFore)
    {
        /* ���˲�����ǰǰ�˲����� */
        UCOM_MemCpy(pstMedAecAf->aswForeWeight,
                    pstMedAecAf->aswWeight,
                    sizeof(pstMedAecAf->aswWeight));                            /* �ú��˲���ϵ������ǰ�˲���ϵ�� */
        UCOM_MemCpy(pstMedAecAf->ashwForeEcho,
                    pstMedAecAf->ashwEcho,
                    sizeof(pstMedAecAf->ashwEcho));                             /* ���˲��������ΪAF��� */
    }
    else
    {
        if (VOS_TRUE == shwUpdateBack)                                          /* ϵ�����º��˲�Ч���ܲ� */
        {
            UCOM_MemCpy(pstMedAecAf->aswWeight,
                        pstMedAecAf->aswForeWeight,
                        sizeof(pstMedAecAf->aswWeight));                        /* ��ǰ�˲���ϵ�����º��˲���ϵ�� */
            UCOM_MemCpy(pstMedAecAf->ashwEcho,
                    pstMedAecAf->ashwForeEcho,
                    sizeof(pstMedAecAf->ashwEcho));                             /* ǰ�˲����������Ϊ��������ο�ֵ */
            UCOM_MemCpy(pstMedAecAf->ashwErr,
                    pstMedAecAf->ashwForeErr,
                    sizeof(pstMedAecAf->ashwErr));                              /* ǰ�˲��в���Ϊ�����ο� */
            pstMedAecAf->swErrPow  = pstMedAecAf->swForeErrPow ;                /* ǰ�˲��в�������Ϊ�����ο� */
        }

        /* ����ϵ������ Ĭ��ʹ��ǰ�˲����st.y��ʹ�ø���ϵ������˲������Ϊ��������ο�ֵ */
        /* ������ */
    }
}


VOS_VOID MED_AEC_AfPowerSpectrum(
                VOS_INT16 *pshwInFreq,
                VOS_INT16  shwLen,
                VOS_INT32 *pswInPsd)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           shwTmp1;
    VOS_INT32                           shwTmp2;

    pswInPsd[0]           = CODEC_OpL_mult0(pshwInFreq[0], pshwInFreq[0]);
    pswInPsd[shwLen >> 1] = CODEC_OpL_mult0(pshwInFreq[1], pshwInFreq[1]);

    for (swCntI = 1; swCntI < (shwLen >> 1) ; swCntI++)
    {
        shwTmp1 = CODEC_OpL_mult0(pshwInFreq[2 * swCntI], pshwInFreq[2 * swCntI]);
        shwTmp2 = CODEC_OpL_mult0(pshwInFreq[(2 * swCntI) + 1],
                                pshwInFreq[(2 * swCntI) + 1]);
        pswInPsd[swCntI]
                = CODEC_OpL_add(shwTmp1, shwTmp2);
    }
}


VOS_VOID MED_AEC_AfCompSmoothPower(
                VOS_INT16 *pshwX,
                VOS_INT32 *paswPower,
                VOS_INT16  shwAlpha,
                VOS_INT16  shwLen)
{
    VOS_INT32                               swCnt;
    VOS_INT16                               shwTmp;
    VOS_INT32                              *paswXf;

    paswXf     = MED_AEC_GetswVecTmp640Ptr1();

    shwTmp     = CODEC_OpSub(CODEC_OP_INT16_MAX, shwAlpha);

    MED_AEC_AfPowerSpectrum(&pshwX[0], shwLen, paswXf);

    /* ����Զ���źŵ�ƽ��������power,����ֻ�õ�ǰ֡���Ĺ����ս���ƽ����*/
    for (swCnt = 0; swCnt< ((shwLen / 2) + 1); swCnt++)
    {
        paswPower[swCnt] = CODEC_OpL_add(CODEC_OpL_mpy_32_16(paswPower[swCnt], shwTmp),
                                        CODEC_OpL_mpy_32_16(paswXf[swCnt], shwAlpha));
        paswPower[swCnt] = CODEC_OpL_add(paswPower[swCnt], 1);
    }
}
VOS_VOID MED_AEC_AfStateReset(MED_AEC_AF_STRU  *pstAf)
{
    VOS_INT16                        shwSampleRate;
    VOS_INT16                        shwAfTailLen;
    VOS_INT16                        shwAfFilterLen;

    shwSampleRate                = pstAf->shwSampleRate;
    shwAfTailLen                 = pstAf->stConfig.shwTailLen;

    UCOM_MemSet((VOS_VOID*)pstAf, 0, sizeof(MED_AEC_AF_STRU));

    if (CODEC_SAMPLE_RATE_MODE_8000 == shwSampleRate)
    {
        pstAf->enFftLen          = MED_AEC_MAX_FFT_LEN / 2;
        pstAf->enFftNumIndex     = MED_FFT_NUM_320;
        pstAf->enIfftNumIndex    = MED_IFFT_NUM_320;

        /* 8k������ÿms����8�㼴����3λ */
        shwAfFilterLen           = shl(shwAfTailLen, 3);
    }
    else
    {
        pstAf->enFftLen          = MED_AEC_MAX_FFT_LEN;
        pstAf->enFftNumIndex     = MED_FFT_NUM_640;
        pstAf->enIfftNumIndex    = MED_IFFT_NUM_640;

        /* 16k������ÿms����16�㼴����4λ */
        shwAfFilterLen           = shl(shwAfTailLen, 4);
    }

    pstAf->stConfig.enAfEnable   = CODEC_SWITCH_ON;
    pstAf->stConfig.shwTailLen   = shwAfTailLen;
    pstAf->shwSampleRate         = shwSampleRate;
    pstAf->shwFrameLen           = MED_PP_GetFrameLength();

    pstAf->shwM = (shwAfFilterLen + (pstAf->shwFrameLen -1)) / (pstAf->enFftLen/2);

    pstAf->shwFarPowerAlpha = MED_AEC_AF_XPOW_THR / pstAf->shwM;

    pstAf->swFarPow              = 1;
    pstAf->swEchoPow             = 1;
    pstAf->stCompLeakState.swPyy = MED_AEC_AF_POWX_DITH;
    pstAf->stCompLeakState.swPey = MED_AEC_AF_POWX_DITH;

}


VOS_VOID MED_AEC_AfResetJudge(MED_AEC_AF_STRU *pstAf, VOS_INT16 *pshwOut)
{
    VOS_INT32               swThd;
    VOS_INT16               shwFlag;
    VOS_INT16               shwTmp1;
    VOS_INT16               shwTmp2;
    VOS_INT16               shwTmp3;

    swThd   = pstAf->enFftLen * MED_AEC_AF_RESET_THR;                           /* �޷����û������㣬�ɱ�֤����� */

    shwTmp1 = (CODEC_OpL_shr(pstAf->swForeErrPow, MED_PP_SHIFT_BY_10)  < swThd);
    shwTmp2 = (CODEC_OpL_shr(pstAf->swEchoPow, MED_PP_SHIFT_BY_10)  < swThd);
    shwTmp3 = (CODEC_OpL_shr(pstAf->swFarPow, MED_PP_SHIFT_BY_10)  < swThd);
    shwFlag = (shwTmp1 && (shwTmp2 && shwTmp3));

    /* �ж��˲���״̬�Ƿ�ܲ� */
    if (   (pstAf->swEchoPow < 0)
        || (pstAf->swFarPow  < 0)
        || (pstAf->swErrPow  < 0)
        || (VOS_FALSE == shwFlag))
    {
        pstAf->shwScrewedUp  = CODEC_OpAdd(pstAf->shwScrewedUp, MED_AEC_AF_RESET_NUM);
        CODEC_OpVecSet(pshwOut, pstAf->shwFrameLen, 0);
    }
    /* �ж��Ƿ��˲����Ӵ���� */
    else if (CODEC_OpL_shr(pstAf->swForeErrPow, MED_PP_SHIFT_BY_2)
           > CODEC_OpL_add(pstAf->swNearPow,
                         CODEC_OpL_shr(CODEC_OpL_mult0(pstAf->enFftLen, MED_AEC_AF_10000),
                         MED_PP_SHIFT_BY_6)))
    {
        pstAf->shwScrewedUp = CODEC_OpAdd(pstAf->shwScrewedUp, 1);
    }
    /* �˲���״̬���� */
    else
    {
        pstAf->shwScrewedUp = 0;
    }

    /* AF���� ע�⣺AF�����ڵ����д����쳣���������λ�ٽ���� */
    /*
    if (pstAf->shwScrewedUp >= MED_AEC_AF_RESET_NUM)
    {
        MED_AEC_AfStateReset(pstAf);
    }
    */

    return;
}


VOS_VOID MED_AEC_AfCompLeak(MED_AEC_AF_STRU *pstAf)
{
    VOS_INT16                          *pshwErrAbs;
    VOS_INT16                          *pshwEchoAbs;
    VOS_INT16                          *pshwEhTmp;
    VOS_INT16                          *pshwYhTmp;
    VOS_INT32                           swCntI;
    VOS_INT16                           shwTmp1;
    VOS_INT16                           shwTmp2;
    VOS_INT16                           shwCoef1;
    VOS_INT32                           swPyyTmp;
    VOS_INT32                           swPeyTmp;
    VOS_INT32                           swMin;
    VOS_INT16                           shwAlpha;
    VOS_INT32                           swTmp1;
    VOS_INT32                           swTmp2;
    VOS_INT32                           swTmp3;

    pshwErrAbs  = MED_AEC_GetshwVecTmp640Ptr1();
    pshwEchoAbs = MED_AEC_GetshwVecTmp640Ptr1() + CODEC_FRAME_LENGTH_WB;

    pshwEhTmp   = MED_AEC_GetshwVecTmp640Ptr2();
    pshwYhTmp   = MED_AEC_GetshwVecTmp640Ptr2() + CODEC_FRAME_LENGTH_WB;

    CODEC_OpVecAbs(pstAf->ashwErr, pstAf->shwFrameLen, pshwErrAbs);
    CODEC_OpVecAbs(pstAf->ashwEcho, pstAf->shwFrameLen, pshwEchoAbs);
    shwCoef1 = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_AEC_AF_LEAK_SMOOTH_ALPHA);

    /* ����echo�Ͳв�ĳ�ʱ������ */
    for (swCntI = 0; swCntI< pstAf->shwFrameLen; swCntI++)
    {
        shwTmp1 = CODEC_OpMult(shwCoef1, pstAf->stCompLeakState.shwAvgErrAbs);
        shwTmp2 = CODEC_OpMult(MED_AEC_AF_LEAK_SMOOTH_ALPHA, pshwErrAbs[swCntI]);
        pstAf->stCompLeakState.shwAvgErrAbs = CODEC_OpAdd(shwTmp1, shwTmp2);

        shwTmp1 = CODEC_OpMult(shwCoef1, pstAf->stCompLeakState.shwAvgEchoAbs);
        shwTmp2 = CODEC_OpMult(MED_AEC_AF_LEAK_SMOOTH_ALPHA, pshwEchoAbs[swCntI]);
        pstAf->stCompLeakState.shwAvgEchoAbs = CODEC_OpAdd(shwTmp1, shwTmp2);
    }

    /* ��������źź��˲�������źŵ����ֵ������ */
    CODEC_OpVcSub(pshwErrAbs,
                pstAf->shwFrameLen,
                pstAf->stCompLeakState.shwAvgErrAbs,
                pshwEhTmp);

    CODEC_OpVcSub(pshwEchoAbs,
                pstAf->shwFrameLen,
                pstAf->stCompLeakState.shwAvgEchoAbs,
                pshwYhTmp);

    /* ���㵱ǰ֡�Ĳ���������˲�����������ֵPey���˲�������������ֵPyy */
    swPyyTmp = 1;
    swPeyTmp = 1;

    swPyyTmp = CODEC_OpL_add(swPyyTmp, MED_AEC_AfFrmPower(pshwYhTmp,
                                                         pshwYhTmp,
                                                         pstAf->shwFrameLen,
                                                         MED_PP_SHIFT_BY_4));
    swPeyTmp = CODEC_OpL_add(swPeyTmp, MED_AEC_AfFrmPower(pshwEhTmp,
                                                         pshwYhTmp,
                                                         pstAf->shwFrameLen,
                                                         MED_PP_SHIFT_BY_4));

     /* ����ƽ�����ֵ */
    swTmp2   = CODEC_OpL_mpy_32_16(pstAf->swEchoPow, MED_AEC_AF_LEAK_BETA0);
    swTmp3   = CODEC_OpL_mpy_32_16(pstAf->swErrPow, MED_AEC_AF_LEAK_BETA_MAX);
    swMin    = CODEC_OpL_min(swTmp2, swTmp3);
    shwAlpha = CODEC_OpSaturate(CODEC_OpNormDiv_32(swMin,
                                                 pstAf->swErrPow,
                                                 MED_PP_SHIFT_BY_15));


    /* �������ϵ�� */
    swTmp2 = CODEC_OpL_mpy_32_16(pstAf->stCompLeakState.swPyy,
                               CODEC_OpSub(CODEC_OP_INT16_MAX, shwAlpha));
    swTmp1 = CODEC_OpL_add(swTmp2,
                         CODEC_OpL_mpy_32_16(swPyyTmp, shwAlpha));
    pstAf->stCompLeakState.swPyy = CODEC_OpL_max(swTmp1, 1);
    swTmp1 = CODEC_OpL_mpy_32_16(pstAf->stCompLeakState.swPey,
                               CODEC_OpSub(CODEC_OP_INT16_MAX, shwAlpha));
    swTmp1 = CODEC_OpL_add(swTmp1, CODEC_OpL_mpy_32_16(swPeyTmp, shwAlpha));
    swTmp2 = CODEC_OpL_mpy_32_16(pstAf->stCompLeakState.swPyy,
                               MED_AEC_AF_LEAK_MIN_LEAK);
    pstAf->stCompLeakState.swPey = CODEC_OpL_max(swTmp1, swTmp2);
    pstAf->stCompLeakState.swPey
           = CODEC_OpL_min(pstAf->stCompLeakState.swPey,
                         pstAf->stCompLeakState.swPyy);

    /* ����й©�� */
    pstAf->shwLeakEstimate
           = CODEC_OpSaturate( CODEC_OpNormDiv_32(pstAf->stCompLeakState.swPey,
                                               pstAf->stCompLeakState.swPyy,
                                               MED_PP_SHIFT_BY_15));

}


VOS_VOID MED_AEC_AfCompRER(
                VOS_INT32  swFarPow,
                VOS_INT32  swErrPow,
                VOS_INT32  swEchoPow ,
                VOS_INT16  shwLeakEstimate,
                VOS_INT32  swBEchoErrPow,
                VOS_INT16 *pshwRER)
{
    CODEC_OP_FLOAT_STRU               stFloatBound;
    VOS_INT32                       swTmp;

    swTmp = CODEC_OpL_mpy_32_16(swEchoPow, shwLeakEstimate);
    swTmp = CODEC_OpL_add(CODEC_OpL_shr(swFarPow, MED_PP_SHIFT_BY_13),
                        CODEC_OpL_add(swTmp, CODEC_OpL_shl(swTmp, 1)));

    stFloatBound = CODEC_OpToFloat(swBEchoErrPow);
    stFloatBound = CODEC_OpFloatDivU(CODEC_OpFloatMul(stFloatBound, stFloatBound),
                              CODEC_OpToFloat(CODEC_OpL_add(1, swEchoPow)));

    if (swTmp < CODEC_OpFloatTo32(stFloatBound))
    {
        swTmp = CODEC_OpFloatTo32(stFloatBound);
    }

    if (swTmp > CODEC_OpL_shr(swErrPow, 1))
    {
        swTmp = CODEC_OpL_shr(swErrPow, 1);
    }

    /* �˴���matlab��һ�£���Ҫ���·�����֤ */
    *pshwRER  = CODEC_OpSaturate(CODEC_OpNormDiv_32(swTmp,swErrPow, MED_PP_SHIFT_BY_15));

}
VOS_VOID  MED_AEC_AfCompStep(MED_AEC_AF_STRU *pstAf)
{
    VOS_INT16                       shwTmp1;
    VOS_INT16                       shwTmp2;
    VOS_INT32                       swCntI;
    VOS_INT32                       swMaxValue;
    VOS_INT16                       shwShift;
    VOS_INT32                       swAdaptRate;
    VOS_INT32                       swEchoPsdNormed;
    VOS_INT32                       swErrPsdNormed;
    VOS_INT32                       swTmp1;
    VOS_INT32                       swTmp2;
    VOS_INT32                       swTmp3;
    VOS_INT32                       swNorm;
    CODEC_OP_FLOAT_STRU               stFloatPowerTemp;
    CODEC_OP_FLOAT_STRU               stFloatTmp3;

    shwTmp2 =(pstAf->stStepState.swSumAdapt > CODEC_OpL_shl(pstAf->shwM, 27));
    shwTmp1
        =  ((VOS_FALSE == pstAf->stStepState.shwAdapted)
        && (shwTmp2 && (pstAf->shwLeakEstimate > MED_AEC_AF_LEAK_THR)));

    if (VOS_TRUE == shwTmp1)
    {
        pstAf->stStepState.shwAdapted = VOS_TRUE;
    }

    if (VOS_TRUE == pstAf->stStepState.shwAdapted)
    {
        /* ����Ӧ���� = ���Ų��� */
        for (swCntI = 0; swCntI < ((pstAf->enFftLen >> 1) + 1); swCntI++)
        {
            /* ��aswEchoPsd��aswErrPsd���й�һ��������ֹС�źų˷�������ʧ */
            swTmp1          = CODEC_OpL_add(pstAf->aswErrPsd[swCntI], 1);
            swMaxValue      = CODEC_OpL_max(pstAf->aswEchoPsd[swCntI], swTmp1);
            shwShift        = CODEC_OpNorm_l(swMaxValue);
            swEchoPsdNormed = CODEC_OpL_shl(pstAf->aswEchoPsd[swCntI], shwShift);
            swErrPsdNormed  = CODEC_OpL_shl(CODEC_OpL_add(pstAf->aswErrPsd[swCntI], 1),shwShift);

            swTmp1          = CODEC_OpL_mpy_32_16(swEchoPsdNormed, pstAf->shwLeakEstimate);
            swTmp2          = swErrPsdNormed;
            swTmp3          = CODEC_OpL_shr(swTmp2, MED_PP_SHIFT_BY_1);
            swTmp1          = CODEC_OpL_min(swTmp1, swTmp3);
            swNorm          = CODEC_OpL_add(CODEC_OpL_mpy_32_16(swTmp1, MED_AEC_AF_STEP_COEF),
                                          CODEC_OpL_mpy_32_16(CODEC_OpL_mpy_32_16
                                          (swTmp2,pstAf->stStepState.shwRER),
                                          MED_AEC_AF_STEP_COEF_N));
            swTmp3          = CODEC_OpL_add(pstAf->aswFarSmoothPsd[swCntI],
                                          MED_AEC_AF_DBF_FLOOR);
            stFloatTmp3     = CODEC_OpFloatAmult(CODEC_OpToFloat(swTmp2),
                                               CODEC_OpToFloat(swTmp3));
            stFloatPowerTemp= CODEC_OpFloatDivU(CODEC_OpToFloat(swNorm), stFloatTmp3);
            stFloatPowerTemp.shwE= CODEC_OpAdd(stFloatPowerTemp.shwE, MED_AEC_AF_STEP_QN);
            pstAf->stStepState.aswStep[swCntI] = CODEC_OpFloatTo32(stFloatPowerTemp);
        }
    }
    else
    {
        /* ����Ӧ���� = ��ʱ���� */
        swAdaptRate=0;
        if (pstAf->swFarPow > CODEC_OpL_shr_r(CODEC_OpL_mult0(pstAf->enFftLen, MED_AEC_AF_1000), MED_PP_SHIFT_BY_6))
        {
            swTmp2 = CODEC_OpL_mpy_32_16(pstAf->swFarPow, MED_AEC_AF_STEP_TMP_COFE);
            swTmp3 = CODEC_OpL_mpy_32_16(pstAf->swErrPow, MED_AEC_AF_STEP_TMP_COFE);
            swTmp1 = CODEC_OpL_min(swTmp2, swTmp3);
            swAdaptRate = CODEC_OpNormDiv_32(swTmp1, pstAf->swErrPow, MED_PP_SHIFT_BY_31);
        }

        for (swCntI = 0; swCntI < ((pstAf->enFftLen >> 1) + 1); swCntI++)
        {
            pstAf->stStepState.aswStep[swCntI]
                = CODEC_OpL_shr(swAdaptRate / CODEC_OpL_add(pstAf->aswFarSmoothPsd[swCntI], MED_AEC_AF_DIV_DITH),
                              MED_PP_SHIFT_BY_31 - MED_AEC_AF_STEP_QN);
        }

        pstAf->stStepState.swSumAdapt
                = CODEC_OpL_add(pstAf->stStepState.swSumAdapt,
                              CODEC_OpL_shr_r(swAdaptRate,
                              (MED_PP_SHIFT_BY_31 - MED_AEC_AF_STEP_QN)));
    }

}
VOS_INT32 MED_AEC_AfFrmPower(
                VOS_INT16 *pshwX,
                VOS_INT16 *pshwY,
                VOS_INT16  shwFrameSize,
                VOS_INT16  shwShift)
{
    VOS_INT32                   swSumMulXY = 0;
    VOS_INT32                   swCntI;

    if (0 == shwShift)
    {
        for (swCntI = 0; swCntI < shwFrameSize; swCntI++)
        {
            swSumMulXY = CODEC_OpL_add(swSumMulXY, CODEC_OpL_mult0(pshwX[swCntI],
                                                               pshwY[swCntI]) );
        }
    }
    else
    {
        for (swCntI = 0; swCntI < shwFrameSize; swCntI++)
        {
            swSumMulXY = CODEC_OpL_add(swSumMulXY, CODEC_OpL_shr(
                         CODEC_OpL_mult0(pshwX[swCntI], pshwY[swCntI]), shwShift) );
        }
    }
    return swSumMulXY;
}

#endif

#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif
