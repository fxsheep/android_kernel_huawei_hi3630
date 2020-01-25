

/*****************************************************************************
   1 ͷ�ļ�����
*****************************************************************************/
#include "med_aec_hf_af.h"
#include "ucom_comm.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
   2 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
   3 ����ʵ��
*****************************************************************************/


VOS_VOID MED_AEC_HF_AfInit(
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                MED_AEC_HF_AF_STRU               *pstHfAf)
{
    UCOM_MemSet((VOS_VOID*)pstHfAf, 0, sizeof(MED_AEC_HF_AF_STRU));

    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        pstHfAf->shwFftLen              = (MED_AEC_MAX_FFT_LEN / 2);
        pstHfAf->enFftNumIndex          = MED_FFT_NUM_320;
        pstHfAf->enIfftNumIndex         = MED_IFFT_NUM_320;
        pstHfAf->stCoefUpdt.shwStep     = 19661;                                /* AF�̶�������0.6 ->19661��Q15 */
        pstHfAf->stCoefUpdt.shwPowTh    = 671;                                  /* ��һ���в��ź�ʱ���������ޣ�2*10^-6 -> 671, Q25*10 */
        pstHfAf->stCoefUpdt.shwSegmLen  = 10;                                   /* ÿ10֡�������������������ݿ� */
    }
    else
    {
        pstHfAf->shwFftLen              = MED_AEC_MAX_FFT_LEN;
        pstHfAf->enFftNumIndex          = MED_FFT_NUM_640;
        pstHfAf->enIfftNumIndex         = MED_IFFT_NUM_640;
        pstHfAf->stCoefUpdt.shwStep     = 16384;                                /* AF�̶�������0.5 ->16384��Q15 */
        pstHfAf->stCoefUpdt.shwPowTh    = 503;                                  /* ��һ���в��ź�ʱ���������ޣ�1.5*10^-6 -> 504, Q25*10 */
        pstHfAf->stCoefUpdt.shwSegmLen  = 20;                                   /* ÿ20֡�������������������ݿ� */
    }

    pstHfAf->stCoefUpdt.shwAlphFac      = 3277;                                 /* Alpha�˲���ϵ����0.1 -> 3277, Q15 */
    pstHfAf->stCoefUpdt.shwMaxWIdx      = 0;                                    /* �����������ݿ�����*/
    pstHfAf->stCoefUpdt.swFrmCount      = 0;                                    /* ֡������*/
}
VOS_VOID MED_AEC_HF_AfMain (
                MED_AEC_HF_AF_STRU            *pstHfAf,
                VOS_INT16                     *pshwNear,
                VOS_INT16                     *pshwFar,
                VOS_INT16                     *pshwErr)
{
    VOS_INT16               shwFrameLen;
    VOS_INT16               shwIdxI;
    VOS_INT16               ashwEcho[MED_AEC_MAX_FFT_LEN >> 1];
    VOS_INT16              *pshwErrFreq;

    /* �ڴ����� */
    pshwErrFreq     = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)pstHfAf->shwFftLen * sizeof(VOS_INT16));

    shwFrameLen                         = (pstHfAf->shwFftLen >> 1);

    /* ����Զ��ʱ������ */
    CODEC_OpVecCpy(pstHfAf->ashwFar,
                   pstHfAf->ashwFar + shwFrameLen,
                   shwFrameLen);
    CODEC_OpVecCpy(pstHfAf->ashwFar + shwFrameLen,
                   pshwFar,
                   shwFrameLen);

    /* ���½���ʱ������ */
    CODEC_OpVecCpy(pstHfAf->ashwNear,
                   pstHfAf->ashwNear + shwFrameLen,
                   shwFrameLen);
    CODEC_OpVecCpy(pstHfAf->ashwNear + shwFrameLen,
                   pshwNear,
                   shwFrameLen);

    /* ����Զ��Ƶ������ */
    for (shwIdxI = MED_AEC_HF_AF_M - 1; shwIdxI > 0; shwIdxI--)
    {
        CODEC_OpVecCpy(pstHfAf->ashwFarFreq[shwIdxI],
                       pstHfAf->ashwFarFreq[shwIdxI - 1],
                       pstHfAf->shwFftLen);
    }
    MED_FFT_Fft(pstHfAf->enFftNumIndex, pstHfAf->ashwFar, &pstHfAf->ashwFarFreq[0][0]);

    /* ����Ӧ�˲���Ƶ���˲� */
    MED_AEC_HF_AfFilt(pstHfAf->ashwFarFreq,
                      pstHfAf->stCoefUpdt.aswWeight,
                      pstHfAf->shwFftLen,
                      pstHfAf->enIfftNumIndex,
                      ashwEcho);

    /* �в��źż��� */
    MED_AEC_HF_AfErrorCalc(ashwEcho,
                           pshwNear,
                           pstHfAf->shwFftLen,
                           pstHfAf->enFftNumIndex,
                           pshwErr,
                           pshwErrFreq);

    /* ����Ӧ�˲���ϵ������ */
    MED_AEC_HF_AfCoefUpdt(&pstHfAf->stCoefUpdt,
                          pshwErrFreq,
                          pstHfAf->ashwFarFreq,
                          pstHfAf->shwFftLen,
                          pstHfAf->enFftNumIndex,
                          pstHfAf->enIfftNumIndex);

    /* �ڴ��ͷ� */
    UCOM_MemFree(pshwErrFreq);
}


VOS_VOID MED_AEC_HF_AfFilt(
                VOS_INT16  ashwFarFreq[][MED_AEC_MAX_FFT_LEN],
                VOS_INT32  aswWeight[][MED_AEC_MAX_FFT_LEN],
                VOS_INT16  shwFftLen,
                VOS_INT16  enIfftNumIndex,
                VOS_INT16 *pshwEcho)
{
    VOS_INT16               shwIdxI;
    VOS_INT16               shwIdxJ;
    VOS_INT16               shwFrameLen = shwFftLen >> 1;
    VOS_INT32               swTemp;
    VOS_INT16              *pshwTemp;
    VOS_INT32              *pswEchoFreq;

    /* �ڴ����� */
    pswEchoFreq     = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT32));
    pshwTemp        = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT16));

    /* �˲� */
    for (shwIdxJ = 0; shwIdxJ < shwFftLen; shwIdxJ++)
    {
        pswEchoFreq[shwIdxJ]            = 0;
    }
    for (shwIdxI = 0; shwIdxI < MED_AEC_HF_AF_M; shwIdxI++)
    {
        swTemp         = CODEC_OpL_mpy_32_16(aswWeight[shwIdxI][0],
                                             ashwFarFreq[shwIdxI][0]);
        pswEchoFreq[0] = CODEC_OpL_add(swTemp, pswEchoFreq[0]);                 /* ֱ������ */

        swTemp         = CODEC_OpL_mpy_32_16(aswWeight[shwIdxI][1],
                                             ashwFarFreq[shwIdxI][1]);
        pswEchoFreq[1] = CODEC_OpL_add(swTemp, pswEchoFreq[1]);                 /* Ƶ�����Q13 */
    }

    for (shwIdxI = 2; shwIdxI < shwFftLen; shwIdxI = shwIdxI + 2)
    {
        for (shwIdxJ = 0; shwIdxJ < MED_AEC_HF_AF_M; shwIdxJ++)
        {
            swTemp  = CODEC_OpL_sub(CODEC_OpL_mpy_32_16(aswWeight[shwIdxJ][shwIdxI],
                                                        ashwFarFreq[shwIdxJ][shwIdxI]),
                                    CODEC_OpL_mpy_32_16(aswWeight[shwIdxJ][shwIdxI + 1],
                                                        ashwFarFreq[shwIdxJ][shwIdxI + 1]));
            pswEchoFreq[shwIdxI]     = CODEC_OpL_add(swTemp,
                                                     pswEchoFreq[shwIdxI]);     /* ʵ�� */

            swTemp  = CODEC_OpL_add(CODEC_OpL_mpy_32_16(aswWeight[shwIdxJ][shwIdxI + 1],
                                                        ashwFarFreq[shwIdxJ][shwIdxI]),
                                    CODEC_OpL_mpy_32_16(aswWeight[shwIdxJ][shwIdxI],
                                                        ashwFarFreq[shwIdxJ][shwIdxI + 1]));
            pswEchoFreq[shwIdxI + 1] = CODEC_OpL_add(swTemp,
                                                     pswEchoFreq[shwIdxI + 1]); /* �鲿 */
        }
    }

    CODEC_OpVecShr_r32(pswEchoFreq,
                       (VOS_INT32)shwFftLen,
                       MED_AEC_HF_AF_W_QN_FILT,
                       pswEchoFreq);                                            /* Ƶ�����Q0 */

    /* ���ʹ��� */
    for (shwIdxI = 0; shwIdxI < shwFftLen; shwIdxI++)
    {
        pshwTemp[shwIdxI] = CODEC_OpSaturate(pswEchoFreq[shwIdxI]);
    }

    /* ��FFT */
    MED_FFT_Ifft(enIfftNumIndex, pshwTemp, pshwTemp);

    CODEC_OpVecCpy(pshwEcho, pshwTemp + shwFrameLen, shwFrameLen);

    /* �ڴ��ͷ� */
    UCOM_MemFree(pswEchoFreq);
    UCOM_MemFree(pshwTemp);
}


VOS_VOID MED_AEC_HF_AfErrorCalc(
                VOS_INT16 *pshwEcho,
                VOS_INT16 *pshwNear,
                VOS_INT16  shwFftLen,
                VOS_INT16  enFftNumIndex,
                VOS_INT16 *pshwErr,
                VOS_INT16 *pshwErrFreq)
{
    VOS_INT16              shwFrameLen  = shwFftLen >> 1;
    VOS_INT16             *pshwTemp;

    /* �ڴ����� */
    pshwTemp        = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT16));

    CODEC_OpVecSet(pshwTemp, shwFftLen, 0);

    /* ʱ��в���� */
    CODEC_OpVvSub(pshwNear, pshwEcho, shwFrameLen, pshwErr);

    /* Ƶ��в���� */
    CODEC_OpVecCpy(pshwTemp + shwFrameLen, pshwErr, shwFrameLen);

    MED_FFT_Fft(enFftNumIndex, pshwTemp, pshwErrFreq);

    /* �ڴ��ͷ� */
    UCOM_MemFree(pshwTemp);

}


VOS_VOID MED_AEC_HF_AfCoefUpdt(
                MED_AEC_HF_COEFUPDT_STRU *pstCoefUpdt,
                VOS_INT16 *pshwErrFreq,
                VOS_INT16  ashwFarFreq[][MED_AEC_MAX_FFT_LEN],
                VOS_INT16  shwFftLen,
                VOS_INT16  enFftNumIndex,
                VOS_INT16  enIfftNumIndex)
{
    VOS_INT32                swThTmp;
    VOS_INT16                shwIdxI;
    VOS_INT16                shwIdxJ;
    VOS_INT16                shwTemp;
    VOS_INT16                shwThMultM;
    VOS_INT16                shwNormShift;
    VOS_INT32               *pswTemp;
    VOS_INT32               *pswAbsErr;
    VOS_INT32               *pswNormErr;

    /* �ڴ����� */
    pswTemp           = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT32));
    pswAbsErr         = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT32));
    pswNormErr        = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT32));

    /* ���ñ������ݿ����������� */
    shwThMultM        = CODEC_OpSaturate(CODEC_OpL_mult(pstCoefUpdt->shwPowTh,
                                         (MED_AEC_HF_AF_M << MED_PP_SHIFT_BY_3))); /* Q29*10 */
    /*��1�����һ��Ƶ�㵥������*/
    for (shwIdxI = 0; shwIdxI < 2; shwIdxI++)
    {
        /*Զ���ź�PSDƽ����Q0*/
        pswTemp[shwIdxI]    = CODEC_OpL_mult0(ashwFarFreq[0][shwIdxI],
                                              ashwFarFreq[0][shwIdxI]);
        pswTemp[shwIdxI]    = CODEC_OpL_sub(pswTemp[shwIdxI],
                                            pstCoefUpdt->aswFarPow[shwIdxI]);
        pswTemp[shwIdxI]    = CODEC_OpL_mpy_32_16_r(pswTemp[shwIdxI],
                                                    pstCoefUpdt->shwAlphFac);
        pstCoefUpdt->aswFarPow[shwIdxI] = CODEC_OpL_add(pstCoefUpdt->aswFarPow[shwIdxI],
                                                        pswTemp[shwIdxI]);

        /*Ƶ��в��źŵ�ģ��Q9*/
        pswAbsErr[shwIdxI]  = CODEC_OpL_shl(CODEC_OpAbs_s(pshwErrFreq[shwIdxI]),
                                            MED_AEC_HF_AF_ERRABS_QN);

        /*ģ�����ޣ�Q29*10 - Q15 - Q5 * 10 = Q9*/
        swThTmp             = CODEC_OpL_mpy_32_16_r(pstCoefUpdt->aswFarPow[shwIdxI],
                                                    shwThMultM);

        /*Ƶ��в��źŹ�һ����Q30*10 */
        if (pswAbsErr[shwIdxI] > swThTmp)
        {
            pswNormErr[shwIdxI]
                = CODEC_OpL_mult(pstCoefUpdt->shwPowTh,
                                 pshwErrFreq[shwIdxI]);                         /* Q26*10 */
            pswNormErr[shwIdxI]
                = CODEC_OpNormDiv_32(CODEC_OpL_abs(pswNormErr[shwIdxI]),
                                     CODEC_OpL_max(pswAbsErr[shwIdxI], 1),
                                     MED_AEC_HF_AF_BIGNORMERR_QN);              /* Q30*10 */
            if (0 > pshwErrFreq[shwIdxI])
            {
               pswNormErr[shwIdxI] = CODEC_OpL_negate(pswNormErr[shwIdxI]);
            }
        }
        else
        {
            pswNormErr[shwIdxI]
                = CODEC_OpL_mult0(pshwErrFreq[shwIdxI],
                                  (MED_AEC_HF_AF_Q5 / MED_AEC_HF_AF_M));        /* Q5 */
            pswNormErr[shwIdxI]
                = CODEC_OpNormDiv_32(CODEC_OpL_abs(pswNormErr[shwIdxI]),
                                     CODEC_OpL_max(pstCoefUpdt->aswFarPow[shwIdxI], 1),
                                     MED_AEC_HF_AF_SMALLNORMERR_QN);            /* Q30*10 */
            if (0 > pshwErrFreq[shwIdxI])
            {
               pswNormErr[shwIdxI] = CODEC_OpL_negate(pswNormErr[shwIdxI]);
            }
        }
        /*���������㣬Q30*10 */
        for (shwIdxJ = 0; shwIdxJ < MED_AEC_HF_AF_M; shwIdxJ++)
        {
            pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI]
                = CODEC_OpL_mult0(ashwFarFreq[shwIdxJ][shwIdxI],
                                  CODEC_OpSaturate(pswNormErr[shwIdxI]));
            pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI]
            = CODEC_OpL_mpy_32_16_r(pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI],
                                    pstCoefUpdt->shwStep);
        }
    }


    /*����Ƶ��ͳһ����*/
    for(shwIdxI = 2; shwIdxI < shwFftLen; shwIdxI = shwIdxI + 2)
    {
        shwTemp          = (shwIdxI >> 1) + 1;
        /*Զ���ź�PSDƽ����Q0*/
        pswTemp[shwTemp] = CODEC_OpL_mult0(ashwFarFreq[0][shwIdxI],
                                           ashwFarFreq[0][shwIdxI]);
        pswTemp[shwTemp] = CODEC_OpL_add(pswTemp[shwTemp],
                                         CODEC_OpL_mult0(ashwFarFreq[0][shwIdxI + 1],
                                                         ashwFarFreq[0][shwIdxI + 1]));
        pswTemp[shwTemp] = CODEC_OpL_sub(pswTemp[shwTemp],
                                         pstCoefUpdt->aswFarPow[shwTemp]);
        pswTemp[shwTemp] = CODEC_OpL_mpy_32_16_r(pswTemp[shwTemp],
                                                 pstCoefUpdt->shwAlphFac);
        pstCoefUpdt->aswFarPow[shwTemp] = CODEC_OpL_add(pstCoefUpdt->aswFarPow[shwTemp],
                                                        pswTemp[shwTemp]);      /* Q0 */

        /*Ƶ��в��źŵ�ģ��Q9*/
        pswTemp[shwTemp]    = CODEC_OpL_mult0(pshwErrFreq[shwIdxI],
                                              pshwErrFreq[shwIdxI]);
        pswTemp[shwTemp]    = CODEC_OpL_add(pswTemp[shwTemp],
                                            CODEC_OpL_mult0(pshwErrFreq[shwIdxI + 1],
                                                            pshwErrFreq[shwIdxI + 1]));

        shwNormShift = CODEC_OpNorm_l(pswTemp[shwTemp]) - 1;                    /* ����ǰ��һ����2^30 */
        shwNormShift = CODEC_OpMax((shwNormShift >> 1), 0);
        pswTemp[shwTemp]    = CODEC_OpL_shl(pswTemp[shwTemp], CODEC_OpShl(shwNormShift, 1));
        pswAbsErr[shwTemp]  = CODEC_OpL_shl((VOS_INT32)CODEC_OpSqrt(pswTemp[shwTemp]),
                                            MED_AEC_HF_AF_ERRABS_QN);
        pswAbsErr[shwTemp]  = CODEC_OpL_shr(pswAbsErr[shwTemp], shwNormShift);  /* ȥ��һ�� */

        /*ģ�����ޣ�Q29*10 - Q15 - Q5 * 10 = Q9 */
        swThTmp             = CODEC_OpL_mpy_32_16_r(pstCoefUpdt->aswFarPow[shwTemp],
                                                    shwThMultM);                /* Q9 */

        /*Ƶ��в��źŹ�һ����Q30*10 */
        if (pswAbsErr[shwTemp] > swThTmp)
        {
            pswNormErr[shwIdxI]     = CODEC_OpL_mult(pstCoefUpdt->shwPowTh,
                                                     pshwErrFreq[shwIdxI]);     /* Q26*10 */
            pswNormErr[shwIdxI]     = CODEC_OpNormDiv_32(CODEC_OpL_abs(pswNormErr[shwIdxI]),
                                                         CODEC_OpL_max(pswAbsErr[shwTemp], 1),
                                                         MED_AEC_HF_AF_BIGNORMERR_QN); /* Q30*10 */
            if (0 > pshwErrFreq[shwIdxI])
            {
               pswNormErr[shwIdxI]  = CODEC_OpL_negate(pswNormErr[shwIdxI]);
            }

            pswNormErr[shwIdxI + 1] = CODEC_OpL_mult(pstCoefUpdt->shwPowTh,
                                                     pshwErrFreq[shwIdxI + 1]);  /* Q26*10 */
            pswNormErr[shwIdxI + 1] = CODEC_OpNormDiv_32(CODEC_OpL_abs(pswNormErr[shwIdxI + 1]),
                                                         CODEC_OpL_max(pswAbsErr[shwTemp], 1),
                                                         MED_AEC_HF_AF_BIGNORMERR_QN); /* Q30*10 */
            if (0 > pshwErrFreq[shwIdxI + 1])
            {
               pswNormErr[shwIdxI + 1]  = CODEC_OpL_negate(pswNormErr[shwIdxI + 1]);
            }
        }
        else
        {
            pswNormErr[shwIdxI]
                = CODEC_OpL_mult0(pshwErrFreq[shwIdxI],
                                  (VOS_INT16)(MED_AEC_HF_AF_Q5 / MED_AEC_HF_AF_M)); /* Q5 */
            pswNormErr[shwIdxI]
                = CODEC_OpNormDiv_32(CODEC_OpL_abs(pswNormErr[shwIdxI]),
                                     CODEC_OpL_max(pstCoefUpdt->aswFarPow[shwTemp], 1),
                                     MED_AEC_HF_AF_SMALLNORMERR_QN);

            pswNormErr[shwIdxI + 1]
                = CODEC_OpL_mult0(pshwErrFreq[shwIdxI + 1],
                                  (VOS_INT16)(MED_AEC_HF_AF_Q5 / MED_AEC_HF_AF_M)); /* Q5 */
            pswNormErr[shwIdxI + 1]
                = CODEC_OpNormDiv_32(CODEC_OpL_abs(pswNormErr[shwIdxI + 1]),
                                     CODEC_OpL_max(pstCoefUpdt->aswFarPow[shwTemp], 1),
                                     MED_AEC_HF_AF_SMALLNORMERR_QN); /* Q30*10 */

            if (0 > pshwErrFreq[shwIdxI + 1])
            {
               pswNormErr[shwIdxI + 1]  = CODEC_OpL_negate(pswNormErr[shwIdxI + 1]);
            }
            if (0 > pshwErrFreq[shwIdxI])
            {
               pswNormErr[shwIdxI]      = CODEC_OpL_negate(pswNormErr[shwIdxI]);
            }
        }

        /*���������㣬Q30*10 */
        for (shwIdxJ = 0; shwIdxJ < MED_AEC_HF_AF_M; shwIdxJ++)
        {
            pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI]
                = CODEC_OpL_mult0(ashwFarFreq[shwIdxJ][shwIdxI],
                                  CODEC_OpSaturate(pswNormErr[shwIdxI]));
            pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI]
                = CODEC_OpL_add(pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI],
                                CODEC_OpL_mult0(ashwFarFreq[shwIdxJ][shwIdxI + 1],
                                                CODEC_OpSaturate(pswNormErr[shwIdxI + 1])));
            pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI]
                = CODEC_OpL_mpy_32_16_r(pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI],
                                        pstCoefUpdt->shwStep);
            pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI + 1]
                = CODEC_OpL_mult0(ashwFarFreq[shwIdxJ][shwIdxI],
                                  CODEC_OpSaturate(pswNormErr[shwIdxI + 1]));
            pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI + 1]
                = CODEC_OpL_sub(pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI + 1],
                                CODEC_OpL_mult0(ashwFarFreq[shwIdxJ][shwIdxI + 1],
                                                CODEC_OpSaturate(pswNormErr[shwIdxI])));
            pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI + 1]
                = CODEC_OpL_mpy_32_16_r(pstCoefUpdt->aswGrad[shwIdxJ][shwIdxI + 1],
                                        pstCoefUpdt->shwStep);
        }
    }

    /* �˲���ϵ��ʱƵ���� */
    MED_AEC_HF_AfCoefTDMang(pstCoefUpdt->aswGrad,
                            shwFftLen,
                            enIfftNumIndex,
                            enFftNumIndex,
                            pstCoefUpdt->aswWeight);

    /* ����λ�ö�λ */
    MED_AEC_HF_AfEchoLocat(pstCoefUpdt, shwFftLen);

    /* �ڴ��ͷ� */
    UCOM_MemFree(pswTemp);
    UCOM_MemFree(pswAbsErr);
    UCOM_MemFree(pswNormErr);
}
VOS_VOID MED_AEC_HF_AfCoefTDMang(
                    VOS_INT32   aswGrad[][MED_AEC_MAX_FFT_LEN],
                    VOS_INT16   shwFftLen,
                    VOS_INT16   enIfftNumIndex,
                    VOS_INT16   enFftNumIndex,
                    VOS_INT32   aswWeight[][MED_AEC_MAX_FFT_LEN])
{
    VOS_INT16                   shwIdxI;
    VOS_INT16                   shwIdxJ;
    VOS_INT32                   swTmp;
    VOS_INT32                   swMaxTmp;
    VOS_INT16                   shwNormShift;
    VOS_INT16                   shwWeightMaxPower;
    VOS_INT16                   shwMaxTmp;
    VOS_INT32                  *pswTemp;
    VOS_INT16                   swMaxPos;
    VOS_INT16                  *pshwFftTmp;

    /* �ڴ����� */
    pshwFftTmp        = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT16));
    pswTemp           = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT32));

    for (shwIdxI = 0; shwIdxI < MED_AEC_HF_AF_M; shwIdxI++)
    {
        /* IFFT��һ�� */
        swMaxTmp            = L_abs(aswGrad[shwIdxI][0]);
        for (shwIdxJ = 1; shwIdxJ < shwFftLen; shwIdxJ++)
        {
            swTmp           = L_abs(aswGrad[shwIdxI][shwIdxJ]);
            if (swMaxTmp < swTmp)
            {
                swMaxTmp    = swTmp;
            }
        }

        shwWeightMaxPower   = 31 - CODEC_OpNorm_l(swMaxTmp);

        if (shwWeightMaxPower > MED_AEC_HF_AF_WEIGPOWER_THD)
        {
            for (shwIdxJ = 0; shwIdxJ < shwFftLen; shwIdxJ++)
            {
                pshwFftTmp[shwIdxJ]
                    = CODEC_OpSaturate(CODEC_OpL_shr_r(aswGrad[shwIdxI][shwIdxJ],
                                       shwWeightMaxPower - MED_AEC_HF_AF_WEIGPOWER_THD));
            }
        }
        else
        {
            for (shwIdxJ = 0; shwIdxJ < shwFftLen; shwIdxJ++)
            {
                pshwFftTmp[shwIdxJ] = CODEC_OpSaturate(aswGrad[shwIdxI][shwIdxJ]);
            }
        }
        /* ��FFT */
        MED_FFT_Ifft(enIfftNumIndex, pshwFftTmp, pshwFftTmp);

        /* ��벿������ */
        CODEC_OpVecSet(pshwFftTmp + (shwFftLen >> 1), (shwFftLen >> 1), 0);

        /* FFT��һ��*/
        shwMaxTmp    = CODEC_OpVecMaxAbs(pshwFftTmp, shwFftLen, &swMaxPos);

        shwNormShift = CODEC_OpNorm_s(shwMaxTmp);

        CODEC_OpVecShl(pshwFftTmp, shwFftLen, shwNormShift, pshwFftTmp);

        /* FFT */
        MED_FFT_Fft(enFftNumIndex, pshwFftTmp, pshwFftTmp);

        /* ��FFT��һ����Q25+Q3*/
        CODEC_OpVecShr_r(pshwFftTmp, shwFftLen, shwNormShift, pshwFftTmp);

        if (shwWeightMaxPower > MED_AEC_HF_AF_WEIGPOWER_THD)
        {
            for (shwIdxJ = 0; shwIdxJ < shwFftLen; shwIdxJ++)
            {
                aswGrad[shwIdxI][shwIdxJ]
                    = CODEC_OpL_shl((VOS_INT32)pshwFftTmp[shwIdxJ],
                                    ((shwWeightMaxPower - MED_AEC_HF_AF_WEIGPOWER_THD) + MED_PP_SHIFT_BY_3));
                aswWeight[shwIdxI][shwIdxJ]
                    = CODEC_OpL_add(aswGrad[shwIdxI][shwIdxJ],
                                    aswWeight[shwIdxI][shwIdxJ]);
            }
        }
        else
        {
            for (shwIdxJ = 0; shwIdxJ < shwFftLen; shwIdxJ++)
            {
                aswWeight[shwIdxI][shwIdxJ]
                    = CODEC_OpL_add(((VOS_INT32)pshwFftTmp[shwIdxJ]) << MED_PP_SHIFT_BY_3,
                                     aswWeight[shwIdxI][shwIdxJ]);

            }
        }
    }

    /* �ڴ��ͷ� */
    UCOM_MemFree(pshwFftTmp);
    UCOM_MemFree(pswTemp);
}


VOS_VOID MED_AEC_HF_AfEchoLocat(
                MED_AEC_HF_COEFUPDT_STRU   *pstCoefUpdt,
                VOS_INT16                   shwFftLen)
{
    VOS_INT16                shwIdxI;
    VOS_INT16                shwIdxJ;
    VOS_INT32               *pswTemp                        = MED_NULL;
    VOS_INT32                aswAbsWeigSum[MED_AEC_HF_AF_M] = {0};

    /* �ڴ����� */
    pswTemp     = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT32));

    /* ÿshwSegmLen֡��λһ�� */
    pstCoefUpdt->swFrmCount++;
    if (pstCoefUpdt->swFrmCount == pstCoefUpdt->shwSegmLen)
    {
        pstCoefUpdt->swFrmCount = 0;
        /* ����ÿ���˲���ϵ���ľ���ֵ�ĺ� */
        for (shwIdxI = 0; shwIdxI < MED_AEC_HF_AF_M; shwIdxI++)
        {
            CODEC_OpVecShr_r32( pstCoefUpdt->aswWeight[shwIdxI],
                                (VOS_INT32)shwFftLen,
                                MED_PP_SHIFT_BY_9,
                                pswTemp);
            for (shwIdxJ = 0; shwIdxJ < shwFftLen; shwIdxJ++)
            {
                pswTemp[shwIdxJ]    = CODEC_OpL_abs(pswTemp[shwIdxJ]);
            }

            aswAbsWeigSum[shwIdxI]  = CODEC_OpVecSum32(pswTemp, (VOS_INT32)shwFftLen);
        }
        /* �˲���ϵ���ľ���ֵ�ĺ��������ݿ��Ӧ�Ż�����λ�� */
        for (shwIdxI = 1; shwIdxI < MED_AEC_HF_AF_M; shwIdxI++)
        {
            if (aswAbsWeigSum[shwIdxI] > aswAbsWeigSum[shwIdxI - 1])
            {
                pstCoefUpdt->shwMaxWIdx = shwIdxI;
            }
            else
            {
                pstCoefUpdt->shwMaxWIdx = shwIdxI - 1;
            }
        }
    }

    /* �ڴ��ͷ� */
    UCOM_MemFree(pswTemp);
}

#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif
