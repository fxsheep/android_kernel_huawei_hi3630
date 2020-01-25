

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "voice_pcm.h"
#include "voice_proc.h"
#include "med_fft.h"
#include "med_vad.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _MED_C89_
/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
MED_VAD_OBJ_STRU    g_astMedVadObjPool[MED_VAD_MAX_OBJ_NUM];                    /* VADʵ����Դ�� */
MED_OBJ_INFO        g_stMedVadObjInfo;                                          /* ��ص���Ϣ */

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_VOID* MED_VAD_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_VAD_GetObjInfoPtr(),
                    MED_VAD_GetObjPtr(),
                    MED_VAD_MAX_OBJ_NUM,
                    sizeof(MED_VAD_OBJ_STRU)));
}


VOS_UINT32 MED_VAD_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_VAD_OBJ_STRU       *pstObj;
    pstObj      = (MED_VAD_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_VAD_GetObjInfoPtr(), pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_VAD_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_VAD_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    MED_VAD_OBJ_STRU         *pstObj    = (MED_VAD_OBJ_STRU*)pstInstance;       /* ��ʱVADָ�� */
    MED_VAD_NV_STRU          *pstNv     = &(pstObj->stNv);                      /* NV�� */
    VOS_UINT32                uwRet;                                            /* ����ֵ */

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_VAD_GetObjInfoPtr(), pstObj);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (shwParaLen < MED_VAD_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ����˲�ϵ������Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0]    = pstNv->shwAlpha;                                  /* �ж�����ϵ�� */
            pshwParam[1]    = pstNv->shwArtiSpeechThd;                          /* �˹������о����� */
            pshwParam[2]    = pstNv->shwChopNumThd;                             /* �˹������У�Ƶ������������� */
        }
    }

    return uwRet;

}


VOS_UINT32 MED_VAD_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    MED_VAD_OBJ_STRU    *pstVad         = (MED_VAD_OBJ_STRU*)pstInstance;       /* ��ʱVADָ�� */
    MED_VAD_NV_STRU     *pstNv          = &(pstVad->stNv);                      /* NV�� */
    VOS_UINT32           uwRet;                                                 /* ����ֵ */

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_VAD_GetObjInfoPtr(), pstVad);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (shwParaLen < MED_VAD_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��Ϊ��Ҫ��ֵΪ0�����Ƚ϶࣬��Ҫ�Ƚ������ṹ���������� */
            UCOM_MemSet(pstVad, 0, sizeof(MED_VAD_OBJ_STRU));

            pstVad->enIsUsed                   = CODEC_OBJ_USED;

            /* ��ֵ�����о�ϵ�����˹�����������ޣ�����ƽ��ϵ�� */
            pstNv->shwAlpha                    = pshwParam[0];                          /* �ж�����ϵ�� */
            pstNv->shwArtiSpeechThd            = pshwParam[1];                          /* �˹������о����� */
            pstNv->shwChopNumThd               = pshwParam[2];                          /* �˹������У�Ƶ������������� */

            /* ��ȡ֡����Ϣ */
            pstVad->shwFrmLen                   = MED_PP_GetFrameLength();

            /* ����֡����ȷ����Ա������ֵ */
            /* խ�� */
            if (CODEC_FRAME_LENGTH_NB == pstVad->shwFrmLen)
            {
                pstVad->enFftLen                = MED_VAD_FFT_LEN_8K;                   /* FFT�ĳ��� */
                pstVad->enPrevFrmLen            = MED_VAD_PREVFRM_LEN_8K;               /* ��һ֡�����źŵĳ��� */
                pstVad->shwHalfFftLen           = MED_VAD_HALFFFT_8K;                   /* FFT���ȵ�һ�룬��ֵ�ϵ��� CODEC_OpShr(stSV.shwFftLen, 1); */
                pstVad->shwHalfFftShift         = MED_VAD_HALFFFT_SHIFT_8K;             /* FFT����һ���ö����Ʊ�ʾ����ռ��λ�� floor(log2(double(stSV.shwHalfFftLen))); */
                pstVad->swMaxFreqEngThd         = MED_VAD_MAX_FREQ_ENG_8K;              /* ���Ƶ���������ޣ���ֵ�ϵ��� CODEC_OpNormDiv_32(1, stSV.shwHalfFftLen, 31); */
                pstVad->swMaxStandardDeviThd    = MED_VAD_MAX_STD_DEVI_THD_8K;          /* ���Ƶ���������������ޣ���ֵ�ϵ��� CODEC_OpSqrt(stSV.swMaxFreqEngThd); */
                pstVad->shwFreqEngNormPara      = MED_VAD_FREQ_ENG_NORM_PARA_8K;        /* Ƶ�������������һ����������ֵ�ϵ��� CODEC_OpNorm_l(stSV.swMaxFreqEngThd); */
                pstVad->shwStdDeviNormPara      = MED_VAD_STD_DEVI_NORM_PARA_8K;        /* Ƶ�������������һ����������ֵ�ϵ��� CODEC_OpNorm_l(stSV.swMaxStandardDeviThd); */
                pstVad->pshwHammingWindow       = &(g_ashwMedVadHammingWin8k[0]);       /* ָ��8k(256��)������ */
            }

            /* ��� */
            else
            {
                pstVad->enFftLen                = MED_VAD_FFT_LEN_16K;                  /* FFT�ĳ��� */
                pstVad->enPrevFrmLen            = MED_VAD_PREVFRM_LEN_16K;              /* ��һ֡�����źŵĳ��� */
                pstVad->shwHalfFftLen           = MED_VAD_HALFFFT_16K;                  /* FFT���ȵ�һ�룬��ֵ�ϵ��� CODEC_OpShr(stSV.shwFftLen, 1); */
                pstVad->shwHalfFftShift         = MED_VAD_HALFFFT_SHIFT_16K;            /* FFT����һ���ö����Ʊ�ʾ����ռ��λ�� floor(log2(double(stSV.shwHalfFftLen))); */
                pstVad->swMaxFreqEngThd         = MED_VAD_MAX_FREQ_ENG_16K;             /* ���Ƶ���������ޣ���ֵ�ϵ��� CODEC_OpNormDiv_32(1, stSV.shwHalfFftLen, 31); */
                pstVad->swMaxStandardDeviThd    = MED_VAD_MAX_STD_DEVI_THD_16K;         /* ���Ƶ���������������ޣ���ֵ�ϵ��� CODEC_OpSqrt(stSV.swMaxFreqEngThd); */
                pstVad->shwFreqEngNormPara      = MED_VAD_FREQ_ENG_NORM_PARA_16K;       /* Ƶ�������������һ����������ֵ�ϵ��� CODEC_OpNorm_l(stSV.swMaxFreqEngThd); */
                pstVad->shwStdDeviNormPara      = MED_VAD_STD_DEVI_NORM_PARA_16K;       /* Ƶ�������������һ����������ֵ�ϵ��� CODEC_OpNorm_l(stSV.swMaxStandardDeviThd); */
                pstVad->pshwHammingWindow       = &(g_ashwMedVadHammingWin16k[0]);      /* ָ��16k(512��)������ */
            }

            /* ������Ա������ʼ�� */
            pstVad->shwFrmCount                 = 1;                                    /* ǰ5֡������ʼ�� */
            pstVad->shwModifiedAlpha            = CODEC_OP_INT16_MAX;                     /* ƽ������ */
        }
    }

    return uwRet;

}


VOS_VOID MED_VAD_UpdateThd(
                MED_VAD_OBJ_STRU       *pstVad,
                CODEC_OP_FLOAT_STRU      *pstFloatCurrTarget)
{
    /* ��ʼ����ʱ���� */
    CODEC_OP_FLOAT_STRU   stFloatTemp1;
    CODEC_OP_FLOAT_STRU   stFloatTemp2;
    VOS_INT16           shwTemp;
    VOS_INT32           swTemp;
    VOS_INT16           shwTmpAlpha;                                            /* ����ƽ��ϵ��������� */
    VOS_INT32           shwCnt;                                                 /* ������ */
    VOS_INT16           shwOptAlpha;                                            /* ����ƽ��ϵ�� */
    VOS_INT16           shwLastSmoothAlpha;                                     /* ����ƽ��ϵ�� */
    CODEC_OP_FLOAT_STRU   stFloatLastSmoothAlpha;                                 /* ����ƽ��ϵ�� (α����)*/

    /* ��������ƽ��ϵ�� */
    stFloatTemp1    = CODEC_OpFloatAdd(pstVad->stFloatMinTarget, CODEC_OP_FLOAT_ONE);
    stFloatTemp1    = CODEC_OpFloatDivU(pstVad->stFloatSmoothedTarget, stFloatTemp1);
    stFloatTemp1.shwE = CODEC_OpAdd(stFloatTemp1.shwE, MED_PP_SHIFT_BY_9);
    shwTemp        = CODEC_OpFloatTo16(stFloatTemp1);                             /* 16bit Q9 */
    shwTemp        = CODEC_OpSub(shwTemp, MED_VAD_1_Q9);                          /* 16bit Q9 */
    swTemp         = CODEC_OpL_add(CODEC_OpL_mult(shwTemp, shwTemp), MED_VAD_1_Q19);/* 32bit Q19 */
    shwOptAlpha    = CODEC_OpSaturate(CODEC_OpNormDiv_32(MED_VAD_1_Q19,
                                                     swTemp,
                                                     MED_PP_SHIFT_BY_15));     /* 16bit Q15 */

    /* ��������ƽ��ϵ�� */
    stFloatTemp1    = CODEC_OpFloatAdd(*pstFloatCurrTarget, CODEC_OP_FLOAT_ONE);
    stFloatTemp1    = CODEC_OpFloatDivU(pstVad->stFloatSmoothedTarget, stFloatTemp1);
    stFloatTemp1.shwE = CODEC_OpAdd(stFloatTemp1.shwE, MED_PP_SHIFT_BY_9);
    shwTemp        = CODEC_OpFloatTo16(stFloatTemp1);                             /* 16bit Q9 */
    shwTemp        = CODEC_OpSub(shwTemp, MED_VAD_1_Q9);                          /* 16bit Q9 */
    swTemp         = CODEC_OpL_add(CODEC_OpL_mult(shwTemp, shwTemp), MED_VAD_1_Q19);/* 32bit Q19 */
    shwTmpAlpha    = CODEC_OpSaturate(CODEC_OpNormDiv_32(MED_VAD_1_Q19,
                                                     swTemp,
                                                     MED_PP_SHIFT_BY_15));     /* 16bit Q15 */
    shwTmpAlpha    = CODEC_OpMax(shwTmpAlpha, MED_VAD_0P7_Q15);
    pstVad->shwModifiedAlpha = CODEC_OpAdd(CODEC_OpMult_r(MED_VAD_0P7_Q15,
                                                      pstVad->shwModifiedAlpha),
                                         CODEC_OpMult_r(MED_VAD_0P3_Q15,
                                                      shwTmpAlpha));

    /* ��������ƽ��ϵ�� */
    shwLastSmoothAlpha = CODEC_OpMult_r(shwOptAlpha, pstVad->shwModifiedAlpha);
    shwLastSmoothAlpha = CODEC_OpMax(MIN_SMOOTH_ALPHA, shwLastSmoothAlpha);

    stFloatLastSmoothAlpha = CODEC_OpToFloat((VOS_INT32)shwLastSmoothAlpha);
    stFloatLastSmoothAlpha.shwE = CODEC_OpSub(stFloatLastSmoothAlpha.shwE,
                                            MED_PP_SHIFT_BY_15);
    stFloatTemp1           = CODEC_OpFloatMul(pstVad->stFloatSmoothedTarget, stFloatLastSmoothAlpha);
    stFloatTemp2           = CODEC_OpFloatMul(*pstFloatCurrTarget,
                                            CODEC_OpFloatSub(CODEC_OP_FLOAT_ONE ,stFloatLastSmoothAlpha));
    pstVad->stFloatSmoothedTarget = CODEC_OpFloatAdd(stFloatTemp1, stFloatTemp2);


    /* ������ƽ��ϵ���������ζ��еĵ�ǰֵ */
    UCOM_MemCpy(&(pstVad->astFloatTargetBuf[pstVad->shwRingCnt]),
                &(pstVad->stFloatSmoothedTarget),
                sizeof(CODEC_OP_FLOAT_STRU));


    /* ����Ŀ����Сֵ�����û���BUFFER */
    pstVad->shwRingCnt      = pstVad->shwRingCnt + 1;
    if (pstVad->shwRingCnt >= MED_VAD_FRM_NUM_1P5_SECOND)
    {
        pstVad->shwRingCnt  = 0;
    }

    /* ������Сֵ */
    UCOM_MemCpy(&(pstVad->stFloatMinTarget),
                &(pstVad->astFloatTargetBuf[0]),
                sizeof(CODEC_OP_FLOAT_STRU));

    for (shwCnt = 1; shwCnt < MED_VAD_FRM_NUM_1P5_SECOND; shwCnt++)
    {
        if (CODEC_OpFloatLT(pstVad->astFloatTargetBuf[shwCnt], pstVad->stFloatMinTarget))
        {
            UCOM_MemCpy(&(pstVad->stFloatMinTarget),
                        &(pstVad->astFloatTargetBuf[shwCnt]),
                        sizeof(CODEC_OP_FLOAT_STRU));
        }
    }
}


VOS_INT16 MED_VAD_ArtiSpchDetect(
                    MED_VAD_OBJ_STRU   *pstVad,
                    VOS_INT16           shwAveCurrFrm,
                    VOS_INT32          *aswFreqEng)
{
    MED_VAD_NV_STRU *pstNv              = &(pstVad->stNv);                      /* NV�� */
    VOS_INT32       swFreqEngDiff;                                              /* ������ */
    VOS_INT16       shwChopNum          = 0;                                    /* ������� */
    VOS_INT32       shwCnt;                                                     /* ������ */
    if (shwAveCurrFrm > MED_VAD_ARTI_VOICE_DETECT_AMP_THD)
    {
        for (   shwCnt = MED_VAD_ARTI_SPCH_FREQ_BAND_MIN;
                shwCnt < (pstVad->shwHalfFftLen - MED_VAD_ARTI_SPCH_FREQ_BAND_DIFF);
                shwCnt++)
        {
            swFreqEngDiff = aswFreqEng[shwCnt] / (CODEC_OpL_max(aswFreqEng[shwCnt + MED_VAD_ARTI_SPCH_FREQ_BAND_DIFF], 1));

            if (swFreqEngDiff > pstNv->shwArtiSpeechThd)
            {
                shwChopNum = shwChopNum + 1;
            }
        }
    }


    return shwChopNum;
}


VOS_VOID MED_VAD_FreqEngCal(
                MED_VAD_OBJ_STRU         *pstVad,
                VOS_INT16                *pshwCurrFrm,
                VOS_INT32                *pswFreqEng)
{
    /* 1. ��ʱ������ֵ */
    VOS_INT16               *pshwFreqDat;                                       /* Ƶ����Ϣ */
    VOS_INT32                shwCnt;                                            /* ������ */

    /* 2. FFT */
    pshwFreqDat     = (VOS_INT16*)(&(pstVad->aswBuff[0]));
    if (CODEC_FRAME_LENGTH_NB == pstVad->shwFrmLen)
    {
        /* խ�� */
        MED_FFT_Fft(MED_FFT_NUM_256 ,pshwCurrFrm, pshwFreqDat);
    }
    else
    {
        /* ��� */
        MED_FFT_Fft(MED_FFT_NUM_512 ,pshwCurrFrm, pshwFreqDat);
    }

    /* 3. �����״����� */
    /* �״�������ռ�õ��ڴ�ռ���״���Ϣռ�õĿռ临�� */
    pswFreqEng[0] = CODEC_OpL_mult0(pshwFreqDat[0], pshwFreqDat[0]);
    for (shwCnt = 1; shwCnt < pstVad->shwHalfFftLen; shwCnt++)
    {
        pswFreqEng[shwCnt] = CODEC_OpL_add(CODEC_OpL_mult0(pshwFreqDat[2 * shwCnt],
                                                       pshwFreqDat[2 * shwCnt]),
                                         CODEC_OpL_mult0(pshwFreqDat[(2 * shwCnt) + 1],
                                                       pshwFreqDat[(2 * shwCnt) + 1])); /* 32bit Q0 */
    }
}
VOS_UINT32 MED_VAD_Main(
                VOS_VOID               *pstVadObj,
                VOS_INT16              *pshwIn,
                VOS_INT16              *penVadFlag)
{
    /* 1. ��ʱ������ֵ */
    MED_VAD_NV_STRU         *pstNv;                                             /* NV�� */
    VOS_INT16               *pshwCurrFrm;                                       /* ����֡����ʷ֡�Ľ�� */
    VOS_INT32               *pswFreqEng;                                        /* Ƶ������ */
    VOS_INT16               *pshwNextFrmHead;                                   /* ��һ֡�Ŀ�ͷ���֣�Ҳ���ǵ�ǰ֡δ�Ӵ�ǰ�Ľ�β���� */
    VOS_INT32                shwCnt;                                            /* ������ */
    VOS_INT16                shwShift1         = 0;                             /* Ƶ�������Ĺ�һ��ϵ�� */
    VOS_INT16                shwShift2         = 0;                             /* �м�ֵaswX�Ĺ�һ��ϵ�� */
    VOS_INT32               *pswX;                                              /* ������ */
    VOS_INT32                swXbar;                                            /* ��һ��֮���Ƶ��������ƽ��ֵ */
    VOS_INT32                swMaxX;                                            /* ��������� */
    VOS_INT32                swMaxFreqEng;                                      /* ���Ƶ������ */
    VOS_INT32                swD               = 0;                             /* �׷��� */
    VOS_INT16                shwAveCurrFrm;                                     /* ��ǰ֡����ֵ��ƽ��ֵ */
    VOS_INT16                shwChopNum;                                        /* ����������� */
    CODEC_OP_FLOAT_STRU        stFloatD;                                          /* �׷���(α����) */
    CODEC_OP_FLOAT_STRU        stFloatTs;                                         /* �׷������� */
    VOS_INT16                shwCheck;                                          /* �߼������� */
    VOS_INT32                swSum;                                             /* �����ͣ���ƽ��ֵ�� */
    MED_VAD_OBJ_STRU        *pstVad;

    pstVad = (MED_VAD_OBJ_STRU*)pstVadObj;
    pstNv  = &(pstVad->stNv);

    /* 2. ����֡���źżӵ���һ֡�źŵ�ĩβ */
    pshwCurrFrm     = &(pstVad->ashwFrmDat[0]);
    CODEC_OpVecCpy(&(pshwCurrFrm[pstVad->enPrevFrmLen]),
                 pshwIn,
                 (VOS_INT32)(pstVad->shwFrmLen));

    /* 3. ��Hamming�� */
    CODEC_OpVvMultR(pshwCurrFrm, pstVad->pshwHammingWindow, pstVad->shwHalfFftLen, pshwCurrFrm);
    CODEC_OpVvMultRD(&(pshwCurrFrm[pstVad->shwHalfFftLen]),
                   &(pstVad->pshwHammingWindow[(pstVad->shwHalfFftLen) - 1]),
                   pstVad->shwHalfFftLen,
                   &(pshwCurrFrm[pstVad->shwHalfFftLen]));

    /* 4. ��ǰ֡���Է��ȵ�ƽ��ֵ */
    shwAveCurrFrm = CODEC_OpVecMeanAbs(pshwIn, pstVad->shwFrmLen);

    /* 5. ���㵱ǰ֡��Ƶ�׺����� */
    pswFreqEng  = &(pstVad->aswBuff[0]);
    MED_VAD_FreqEngCal(pstVad, pshwCurrFrm, pswFreqEng);

    /* 6. �����׷��� */
    /* 6.1 �����Ƶ������ */
    swMaxFreqEng    = CODEC_OpVecMax32(pswFreqEng, (VOS_INT32)(pstVad->shwHalfFftLen));

    /* 6.2 ��Ƶ�������Ĺ�һ��ϵ�� */
    if (swMaxFreqEng > pstVad->swMaxFreqEngThd)
    {
        shwShift1   = CODEC_OpAdd(CODEC_OpSub(pstVad->shwFreqEngNormPara, CODEC_OpNorm_l(swMaxFreqEng)), 1);/* ��һ��ϵ�� */
    }

    /* 6.3 Ƶ��������һ�� */
    CODEC_OpVecShr_r32(pswFreqEng,
                     (VOS_INT32)(pstVad->shwHalfFftLen),
                     shwShift1,
                     pswFreqEng);

    /* 6.4 ���һ��֮���Ƶ��������ƽ��ֵ */
    swSum  = 0;
    for (shwCnt = 0; shwCnt < pstVad->shwHalfFftLen; shwCnt++)
    {
        swSum = CODEC_OpL_add(pswFreqEng[shwCnt], swSum);
    }
    swXbar = CODEC_OpL_shr_r(swSum, pstVad->shwHalfFftShift);

    /* 7. �������м����˹����� */
    shwChopNum = MED_VAD_ArtiSpchDetect(pstVad, shwAveCurrFrm, pswFreqEng);

    /* 6.5 ��������aswX�����������ֵ */
    pswX            = &(pstVad->aswBuff[0]);
    for (shwCnt = 0; shwCnt < pstVad->shwHalfFftLen; shwCnt++)
    {
        pswX[shwCnt]= CODEC_OpL_sub(pswFreqEng[shwCnt], swXbar);
    }
    swMaxX = CODEC_OpVecMax32(pswX, pstVad->shwHalfFftLen);

    /* 6.6 ��Ƶ��������Ľ�һ����һ��ϵ�� */
    if (swMaxX > (pstVad->swMaxStandardDeviThd))
    {
        shwShift2   = CODEC_OpAdd(CODEC_OpSub(pstVad->shwStdDeviNormPara, CODEC_OpNorm_l(swMaxX)), 1);  /* ��һ��ϵ��2 */
    }

    /* 6.7 Ƶ���������һ�� */
    CODEC_OpVecShr_r32(pswX,
                     (VOS_INT32)(pstVad->shwHalfFftLen),
                     shwShift2,
                     pswX);

    /* 6.8 �����׷��� */
    for (shwCnt = 0; shwCnt < pstVad->shwHalfFftLen; shwCnt++)
    {
        swD = CODEC_OpL_add(swD,
                          CODEC_OpL_mult0((VOS_INT16)(pswX[shwCnt]),
                                        (VOS_INT16)(pswX[shwCnt])));
    }
    swD = CODEC_OpL_shr_r(swD, pstVad->shwHalfFftShift);

    /* 6.9 ���׷���ת��Ϊα���� */
    stFloatD      = CODEC_OpToFloat(swD);
    stFloatD.shwE = CODEC_OpAdd(stFloatD.shwE,
                              CODEC_OpAdd(shwShift1, CODEC_OpShl(shwShift2, 1)));

    /* 8. ���޳�ʼ���͸��� */
    if (pstVad->shwFrmCount <= MED_VAD_STARTING_FRM_LEN)
    {
        /* �ò��ִ������ڼ���ǰ5֡�������о����� */

        pstVad->stFloatEmbryonicThd     = CODEC_OpFloatAdd(pstVad->stFloatEmbryonicThd,
                                                    stFloatD);
        pstVad->shwFrmCount             = pstVad->shwFrmCount + 1;
        stFloatTs.shwM                  = CODEC_OP_INT16_MAX;                     /* �о�������Ϊ���ֵ */
        stFloatTs.shwE                  = CODEC_OP_INT16_MAX / 2;
    }
    else if (pstVad->shwFrmCount == (MED_VAD_STARTING_FRM_LEN + 1))
    {
        pstVad->stFloatEmbryonicThd     = CODEC_OpFloatDivU(pstVad->stFloatEmbryonicThd,
                                                          CODEC_OpToFloat(MED_VAD_STARTING_FRM_LEN));
        for (shwCnt = 0; shwCnt < MED_VAD_FRM_NUM_1P5_SECOND; shwCnt++)
        {
            UCOM_MemCpy(&(pstVad->astFloatTargetBuf[shwCnt]),
                        &(pstVad->stFloatEmbryonicThd),
                        sizeof(CODEC_OP_FLOAT_STRU));
        }

        stFloatTs                       = pstVad->stFloatEmbryonicThd;
        pstVad->stFloatSmoothedTarget   = stFloatTs;
        pstVad->shwFrmCount             = pstVad->shwFrmCount + 1;
    }
    else
    {
        MED_VAD_UpdateThd(pstVad, &stFloatD);
        stFloatTs                       = pstVad->stFloatMinTarget;
    }

    /* 9. ����ǰ֡��һ���ֻ���ȫ������Ϊ��һ֡�Ӵ�ʱ�ġ�ǰһ֡�����ݣ�*/
    /* ��Ϊ���� shwFrmLen <= shwFftLen  ��Լ����������ﲻ��Ҫ�����ж� */
    pshwNextFrmHead = &(pstVad->ashwFrmDat[0]);
    CODEC_OpVecCpy(pshwNextFrmHead,
                 &(pshwIn[(2 * pstVad->shwFrmLen) - pstVad->enFftLen]),
                 (VOS_INT32)(pstVad->enPrevFrmLen));

    /* 10. �����о� */
    if (shwAveCurrFrm < MED_VAD_SMOOTH_AMP_UPDATE_THD)
    {
        pstVad->shwSmoothedValue        = CODEC_OpAdd(
                                          CODEC_OpMult(MED_VAD_SMOOTH_FRM_AMP_ALPHA,
                                                     pstVad->shwSmoothedValue),
                                          CODEC_OpMult(MED_VAD_CURR_FRM_ALPHA,
                                                     shwAveCurrFrm));
    }

    shwCheck =    (CODEC_OpFloatLT(CODEC_OpFloatMul(stFloatTs, CODEC_OpToFloat((VOS_INT32)pstNv->shwAlpha)), stFloatD)
                         || (shwChopNum > pstNv->shwChopNumThd))
               && ((pstVad->shwSmoothedValue > MED_VAD_SMOOTH_AMP_THD)
                         || (shwAveCurrFrm > MED_VAD_AVE_AMP_CURR_FRM_THD));

    if (VOS_TRUE == shwCheck)
    {
        *penVadFlag                     = CODEC_VAD_FLAG_SPEECH;             /* ������ */
    }
    else
    {
        *penVadFlag                     = CODEC_VAD_FLAG_NOISE;              /* ������ */
    }

    return UCOM_RET_SUCC;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

