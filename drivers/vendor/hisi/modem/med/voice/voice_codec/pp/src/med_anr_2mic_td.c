

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "med_anr_2mic_td.h"
#include  "om.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
MED_2MIC_TDPROCESS_STRU         g_stTdProcess;

/* �˲���ϵ������ */
const VOS_INT16 g_ashwFilterNum[4] = {8729,24252,24252,8729};
const VOS_INT16 g_ashwFilterDen[4] = {32767,16892,18901,-2597};

const VOS_INT16 g_ashwDelayCoef[MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH] =                                           \
                  { -141,   308,   -623,   1134,  -1959, 3388, -6471, 20748,    \
                   20748, -6471,  3388,  -1959,   1134, -623,  308,   -141};


/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_UINT32 MED_ANR_2MIC_TdInit(
                MED_2MIC_NV_STRU                    *pst2MICParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32      enSampleRate)

{
    MED_2MIC_TDPROCESS_STRU     *pstTdProcess;
    MED_2MIC_CALIB_STRU         *pstCalib;
    MED_2MIC_SPACESPLIT_STRU    *pstSpaceSplit;
    MED_2MIC_ANC_STRU           *pstAnc;
    CODEC_SWITCH_ENUM_UINT16     enCalibEnable;
    VOS_INT16                    shwRcDelay;
    VOS_INT16                    shwVadRatioThld;
    VOS_INT16                    shwFarMaxThd;
    VOS_UINT32                   uwCalibRet;
    VOS_UINT32                   uwSpaceRet;
    VOS_UINT32                   uwAncRet;

    /* �쳣��μ�� */
    if (   (VOS_NULL == pst2MICParam)
        || (CODEC_SAMPLE_RATE_MODE_BUTT <= enSampleRate))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* ��ȡ��Ӧ�ṹ��ָ�� */
    pstTdProcess    = MED_2MIC_GetTdProcessPtr();
    pstCalib        = MED_2MIC_GetTdCalibPtr();
    pstSpaceSplit   = MED_2MIC_GetTdSpaceSplitPtr();
    pstAnc          = MED_2MIC_GetTdAncPtr();

    /* ��ȡ��ӦNV�� */
    enCalibEnable   = pst2MICParam->enCalibEnable;
    shwRcDelay      = pst2MICParam->shwRcDelay;
    shwVadRatioThld = pst2MICParam->shwVadRatioThld;
    shwFarMaxThd    = pst2MICParam->shwFarMaxThd1;

    /* ��ʼ��TD���̿��ƽṹ�� */
    UCOM_MemSet(pstTdProcess, 0, sizeof(MED_2MIC_TDPROCESS_STRU));

    /* ���ò����� */
    MED_2MIC_SetTdSampleRate(enSampleRate);

    /* Calibģ���ʼ�� */
    uwCalibRet  = MED_ANR_2MIC_TdCalibInit(enCalibEnable, pstCalib);

    /* SpaceSplitģ���ʼ�� */
    uwSpaceRet  = MED_ANR_2MIC_TdSpaceSplitInit(shwRcDelay, pstSpaceSplit);

    /* Ancģ���ʼ�� */
    uwAncRet    = MED_ANR_2MIC_TdAncInit(shwVadRatioThld, shwFarMaxThd, pstAnc);

    /* ���س�ʼ����� */
    if (   (UCOM_RET_SUCC == uwCalibRet)
        && (UCOM_RET_SUCC == uwSpaceRet)
        && (UCOM_RET_SUCC == uwAncRet))
    {
        return UCOM_RET_SUCC;
    }
    else
    {
        return UCOM_RET_FAIL;
    }

}
VOS_UINT32 MED_ANR_2MIC_TdCalibInit(
                CODEC_SWITCH_ENUM_UINT16       enEnable,
                MED_2MIC_CALIB_STRU         *pstCalib)
{
    pstCalib->enCalibEnable         = enEnable;                                 /* У׼ģ��ʹ�ܱ�־ */
    pstCalib->shwLastGain           = MED_2MIC_MAX_Q14;                         /* ��һ֡��У׼���� */

    return UCOM_RET_SUCC;
}


VOS_UINT32 MED_ANR_2MIC_TdSpaceSplitInit(
                VOS_INT16                           shwRcDelay,
                MED_2MIC_SPACESPLIT_STRU            *pstSpaceSplit)
{
    MED_2MIC_SPACESPLIT_MC_STRU         *pstSpaceMc;
    MED_2MIC_SPACESPLIT_RC_STRU         *pstSpaceRc;
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32      enSampleRate;

    /* ��ȡ������ */
    enSampleRate    = MED_2MIC_GetTdSampleRate();

    /* �ռ������Mic��ʼ�� */
    pstSpaceMc      = &(pstSpaceSplit->stMainNormMode);

    /* ����������ӳٳ��� */
    pstSpaceMc->shwDelayValue   = CODEC_OpExtract_l(CODEC_OpL_mult0((VOS_INT16)enSampleRate / (VOS_INT16)CODEC_SAMPLE_RATE_MODE_8000,
                                                                MED_2MIC_SPACESPLIT_MC_DELAY));

    /* �ռ���븨Mic��ʼ�� */
    pstSpaceRc      = &(pstSpaceSplit->stRefMode);

    pstSpaceRc->shwDelayValue   = shwRcDelay;
    pstSpaceRc->shwNeGain       = MED_2MIC_SPACESPLIT_MC_NE_GAIN;

    return UCOM_RET_SUCC;

}


VOS_UINT32 MED_ANR_2MIC_TdAncInit(
                VOS_INT16                           shwVadRatioThld,
                VOS_INT16                           shwFarMaxThd,
                MED_2MIC_ANC_STRU                  *pstAnc)
{
    MED_2MIC_VAD_STRU                   *pstAncVad;
    MED_2MIC_VAD_WB_STRU                *pstAncVadWb;
    MED_2MIC_LAF_STRU                   *pstAncLaf;
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32     enSampleRate;

    /* �����ʻ�ȡ */
    enSampleRate    = MED_2MIC_GetTdSampleRate();

    /* VAD��ʼ�� */
    pstAncVad                   = &(pstAnc->stVad);
    pstAncVadWb                 = &(pstAnc->stVad.stVadWb);

    pstAncVad->shwVadRatioThd   = shwVadRatioThld;
    pstAncVad->shwPsPnRatio     = MED_2MIC_ANC_VAD_PS_PN_RATIO_INIT;
    pstAncVadWb->shwFilterOrder = MED_2MIC_ANC_VAD_FILTER_ORDER;

    pstAncVadWb->pshwFilterNum = (VOS_INT16 *)MED_2MIC_GetTdFilterNumPtr();
    pstAncVadWb->pshwFilterDen = (VOS_INT16 *)MED_2MIC_GetTdFilterDenPtr();

    /* LAF��ʼ�� */
    pstAncLaf                   = &(pstAnc->stLaf);

    /* �ɲ�����ȷ�������� */
    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        pstAncLaf->shwN         = MED_2MIC_LAF_FILTER_ORDER_NB;
        pstAncLaf->swInShortThd = MED_2MIC_LAF_WEIGHT_SHORT_POWER_THD_NB;
    }
    else
    {
        pstAncLaf->shwN         = MED_2MIC_LAF_FILTER_ORDER_WB;
        pstAncLaf->swInShortThd = MED_2MIC_LAF_WEIGHT_SHORT_POWER_THD_WB;
    }

    pstAncLaf->swInShortPower   = 0;
    pstAncLaf->shwAlpha         = MED_2MIC_LAF_WEIGHT_SHORT_POWER_ALPHA;

    pstAncLaf->shwStep          = MED_2MIC_LAF_STEP_INIT;
    pstAncLaf->swRefPowerThd    = MED_2MIC_LAF_STEP_REF_POWER_THD;
    pstAncLaf->shwFarMaxThd     = shwFarMaxThd;

    /* ANR��ʼ�� */
    pstAnc->shwMainDelayQ       = CODEC_OpShl(pstAncLaf->shwN, MED_PP_SHIFT_BY_7);

    return UCOM_RET_SUCC;

}
VOS_UINT32 MED_ANR_2MIC_TdProcess(
                CODEC_SWITCH_ENUM_UINT16         enEnable,
                CODEC_VAD_FLAG_ENUM_INT16        enVadFlag,
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut,
                VOS_INT16                       *pshwRcCalib)
{
    MED_2MIC_CALIB_STRU                     *pstCalib;
    MED_2MIC_SPACESPLIT_STRU                *pstSpaceSplit;
    MED_2MIC_ANC_STRU                       *pstAnc;
    VOS_INT16                               *pshwMcCalib;
    VOS_INT16                               *pshwMcSpace;
    VOS_INT16                               *pshwRcSpace;
    VOS_INT16                                shwFrameLen = MED_PP_GetFrameLength();
    VOS_UINT32                               uwCalibRet;
    VOS_UINT32                               uwSpaceRet;
    VOS_UINT32                               uwAncRet = UCOM_RET_SUCC;

    /* ���ANR��ʹ�� */
    if (CODEC_SWITCH_OFF == enEnable)
    {
        CODEC_OpVecCpy(pshwMcOut, pshwMcIn, shwFrameLen);
        CODEC_OpVecCpy(pshwRcOut, pshwRcIn, shwFrameLen);
        CODEC_OpVecCpy(pshwRcCalib, pshwRcIn, shwFrameLen);

        return UCOM_RET_ERR_PARA;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_ANR_2MIC_TdProcess);

    /* �����ڴ�ռ� */
    pshwMcCalib = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));
    pshwMcSpace = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));
    pshwRcSpace = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    /* ��ȡ��ģ��ָ�� */
    pstCalib            = MED_2MIC_GetTdCalibPtr();
    pstSpaceSplit       = MED_2MIC_GetTdSpaceSplitPtr();
    pstAnc              = MED_2MIC_GetTdAncPtr();

    /* ����У׼ */
    uwCalibRet = MED_ANR_2MIC_TdCalib(      enVadFlag,
                                            pstCalib,
                                            pshwMcIn,
                                            pshwRcIn,
                                            pshwMcCalib,
                                            pshwRcCalib);

    /* �ռ���� */
    uwSpaceRet = MED_ANR_2MIC_TdSpaceSplit( pstSpaceSplit,
                                            pshwMcCalib,
                                            pshwRcCalib,
                                            pshwMcSpace,
                                            pshwRcSpace);

    /* ��������źŽ�������Ӧ�������� */
    uwAncRet = MED_ANR_2MIC_TdAncNormalMode(pstAnc,
                                            pshwMcSpace,
                                            pshwRcSpace,
                                            pshwRcCalib,
                                            pshwMcOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_ANR_2MIC_TdProcess);

    if (   (UCOM_RET_SUCC == uwCalibRet)
        && (UCOM_RET_SUCC == uwSpaceRet)
        && (UCOM_RET_SUCC == uwAncRet))
    {
        /* ���TD��������ź� */
        UCOM_MemCpy(pshwRcOut, pshwRcSpace, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

        /* �ͷŷ���Ŀռ� */
        UCOM_MemFree(pshwMcCalib);
        UCOM_MemFree(pshwMcSpace);
        UCOM_MemFree(pshwRcSpace);

        return UCOM_RET_SUCC;
    }
    else
    {
        /* �ͷŷ���Ŀռ� */
        UCOM_MemFree(pshwMcCalib);
        UCOM_MemFree(pshwMcSpace);
        UCOM_MemFree(pshwRcSpace);

        return UCOM_RET_FAIL;
    }

}
VOS_UINT32 MED_ANR_2MIC_TdCalib(
                CODEC_VAD_FLAG_ENUM_INT16     enVadFlag,
                MED_2MIC_CALIB_STRU             *pstCalib,
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut)
{
    VOS_INT16                       shwLastGain;
    VOS_INT16                       shwFrmLen;
    VOS_INT32                       swMainPower;
    VOS_INT16                       shwIndex;
    VOS_INT32                       swRefpower;
    VOS_INT16                       shwCurrentGain;
    VOS_INT16                       shwMaxGainThld;
    VOS_INT16                       shwMinGainThld;
    VOS_INT16                       shwRestrictGain;
    VOS_INT32                       swTemp;
    VOS_INT16                       shwTemp;
    VOS_INT16                       shwSubTemp;
    VOS_INT16                       shwSmoothGain;
    VOS_INT32                       swRcOut;

    /* ��ȡ֡�� */
    shwFrmLen   = MED_PP_GetFrameLength();

    /* ��ȡ��һ֡У׼���� */
    shwLastGain = pstCalib->shwLastGain;


    /* ��У׼ģ�鲻ʹ�ܣ����� */
    if (CODEC_SWITCH_OFF == pstCalib->enCalibEnable)
    {
        /* ������ź�У׼�󱣳ֲ��� */
        UCOM_MemCpy(pshwMcOut, pshwMcIn, (VOS_UINT16)shwFrmLen * sizeof(VOS_INT16));

        UCOM_MemCpy(pshwRcOut, pshwRcIn, (VOS_UINT16)shwFrmLen * sizeof(VOS_INT16));

        return UCOM_RET_SUCC;
    }

    swMainPower = 0;

    /* ����������ź����� */
    for (shwIndex = 0; shwIndex < shwFrmLen; shwIndex++)
    {
        swTemp              = CODEC_OpL_mult0(pshwMcIn[shwIndex], pshwMcIn[shwIndex]);
        swMainPower         = CODEC_OpL_add(swTemp, swMainPower);
    }

    swMainPower = CODEC_OpL_max(swMainPower, MED_2MIC_MIN_POWER_VALUE);
#ifdef _MED_MAX_CYCLE_
    if (1)
#else
    /* ����У׼����:������źŲ�Ϊ��·�����Ҳ�Ϊ����ʱ */
    if ((MED_2MIC_CALIB_FLOOR_MIN_POWER < swMainPower)
        && (CODEC_VAD_FLAG_NOISE == enVadFlag))
#endif

    {
            /* ���㸨����ź����� */
            swRefpower = 0;

            for (shwIndex = 0; shwIndex < shwFrmLen; shwIndex++)
            {
                swTemp      = CODEC_OpL_mult0(pshwRcIn[shwIndex], pshwRcIn[shwIndex]);
                swRefpower  = CODEC_OpL_add(swTemp, swRefpower);
            }

            swRefpower      = CODEC_OpL_max(swRefpower, MED_2MIC_MIN_POWER_VALUE);

            /* ��������У׼���沢���ƴ�С */
            shwCurrentGain  = CODEC_OpSqrt(CODEC_OpNormDiv_32(swMainPower , swRefpower, MED_PP_SHIFT_BY_28));
            shwMaxGainThld  = CODEC_OpAdd(shwLastGain, MED_2MIC_CALIB_GAIN_DELTA_MAX);
            shwSubTemp      = CODEC_OpSub(shwLastGain, MED_2MIC_CALIB_GAIN_DELTA_MAX);
            shwMinGainThld  = CODEC_OpMax(shwSubTemp, 0);
            shwRestrictGain = CODEC_OpMax(shwMinGainThld, CODEC_OpMin(shwCurrentGain, shwMaxGainThld));

            /* ��У׼�������ƽ�������ƴ�С */
            shwTemp          = CODEC_OpMult(CODEC_OpSub(MED_2MIC_CALIB_GAIN_MAX, MED_2MIC_CALIB_ALPHA), shwRestrictGain);
            shwSmoothGain   = CODEC_OpAdd(CODEC_OpMult(MED_2MIC_CALIB_ALPHA, shwLastGain), shwTemp);
            shwSmoothGain   = CODEC_OpMax(MED_2MIC_CALIB_GAIN_MIN, CODEC_OpMin(shwSmoothGain, MED_2MIC_CALIB_GAIN_MAX));

            /* У׼���������� */
            pstCalib->shwLastGain= shwSmoothGain;

    }

    /* ������ź�У׼���ֲ��䣬�������ɸ����У׼������в��� */
    UCOM_MemCpy(pshwMcOut, pshwMcIn, (VOS_UINT16)shwFrmLen * sizeof(VOS_INT16));

    for (shwIndex = 0; shwIndex < shwFrmLen; shwIndex++)
    {
        /* �Ը�����ź�У׼ */
        swRcOut = CODEC_OpL_shl( CODEC_OpL_mult(pshwRcIn[shwIndex], pstCalib->shwLastGain), 1);
        pshwRcOut[shwIndex] = CODEC_OpRound(swRcOut);
    }

    return UCOM_RET_SUCC;

}


VOS_UINT32 MED_ANR_2MIC_TdSpaceSplit(
                MED_2MIC_SPACESPLIT_STRU        *pstSpaceSplit,
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut)
{
    MED_2MIC_SPACESPLIT_MC_STRU         *pstMainNormMode;
    MED_2MIC_SPACESPLIT_RC_STRU         *pstRefMode;
    VOS_INT16                           *pshwMainState;
    VOS_INT16                            shwDelayValue;
    VOS_UINT32                           uwRet = UCOM_RET_SUCC;

    /* �쳣��μ�� */
    if ((VOS_NULL_PTR == pstSpaceSplit)
     || (VOS_NULL_PTR == pshwMcIn)
     || (VOS_NULL_PTR == pshwRcIn)
     || (VOS_NULL_PTR == pshwMcOut)
     || (VOS_NULL_PTR == pshwRcOut))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* ��ȡ����˷���ṹ����Ϣ */
    pstMainNormMode     = &(pstSpaceSplit->stMainNormMode);
    shwDelayValue       = pstMainNormMode->shwDelayValue;
    pshwMainState       = pstMainNormMode->ashwMainState;

    /* ��ȡ����˷���ṹ����Ϣ */
    pstRefMode          = &(pstSpaceSplit->stRefMode);

    /* ������źſռ����:�����ӳ�͸������ */
    uwRet += MED_ANR_2MIC_TdSpaceIntegerDelay(pshwMcIn,
                                             shwDelayValue,
                                             pshwMainState,
                                             pshwMcOut);

    /* ������źſռ����:�����ӳٿ���󷽷� */
    uwRet += MED_ANR_2MIC_TdBlockMatrix(pstRefMode,
                                       pshwMcIn,
                                       pshwRcIn,
                                       pshwRcOut);

    return uwRet;

}
VOS_UINT32 MED_ANR_2MIC_TdSpaceIntegerDelay(
                VOS_INT16                       *pshwIn,
                VOS_INT16                        shwDelay,
                VOS_INT16                       *pshwState,
                VOS_INT16                       *pshwOut)
{
    VOS_INT16                       shwDataLen;
    VOS_INT16                       shwLen;

    /* ��ȡ֡�� */
    shwDataLen = MED_PP_GetFrameLength();

    /* �ȽϵͰ�λ���ж�shwDelayValue�Ƿ�ΪС�� */
    if (0 != (shwDelay & 0xFF))
    {
        return UCOM_RET_ERR_STATE;
    }

    /* �����ӳ� */
    shwDelay    = CODEC_OpShr(shwDelay, MED_PP_SHIFT_BY_8);

    /* ������һ֡��������ݣ�����Ϊ�ӳٵ���  */
    UCOM_MemCpy(pshwOut, pshwState, (VOS_UINT16)shwDelay * sizeof(VOS_INT16));

    /* ������֡���ݣ�����Ϊ֡����ȥ���ӳٵ�����ʣ�೤�� */
    shwLen      = CODEC_OpSub(shwDataLen, shwDelay);
    UCOM_MemCpy((pshwOut + shwDelay), pshwIn, (VOS_UINT16)shwLen * sizeof(VOS_INT16));

    /* ����״̬��Ϣ��������һ֡�ӳٴ��� */
    UCOM_MemCpy(pshwState, (pshwIn + shwLen), (VOS_UINT16)shwDelay * sizeof(VOS_INT16));

    return UCOM_RET_SUCC;

}


VOS_UINT32 MED_ANR_2MIC_TdBlockMatrix(
                MED_2MIC_SPACESPLIT_RC_STRU     *pstRefMode,
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                       *pshwRcOut)
{
    VOS_INT16                   *pshwRefState;
    VOS_INT16                   *pshwMainState;
    VOS_INT16                    shwDelayValue;
    VOS_INT16                    shwTemp;
    VOS_INT16                    shwNeGain;
    VOS_INT16                    shwFrameLen;
    VOS_INT16                   *pshwMcDelay;
    VOS_INT16                   *pshwRcDelay;
    VOS_INT16                    shwCnt;
    VOS_UINT32                   uwRet;

    /* �����ڴ� */
    pshwMcDelay = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));
    pshwRcDelay = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    /* ��ȡ����֡֡���� */
    shwFrameLen     = MED_PP_GetFrameLength();

    /* ��ȡ��ʷ��Ϣ */
    pshwRefState    = pstRefMode->ashwRefState;
    pshwMainState   = pstRefMode->ashwMainState;

    /* ��ȡ������ź��ӳ���ֵ(֧�ַ���)*/
    shwDelayValue   = pstRefMode->shwDelayValue;

    /* ��������źŽ����ӳ�(֧�ַ���) */
    uwRet = MED_ANR_2MIC_TdSpaceFractionDelay(pshwMcIn,
                                              pshwRcIn,
                                              shwDelayValue,
                                              pshwMainState,
                                              pshwRefState,
                                              pshwMcDelay,
                                              pshwRcDelay);

    /* ������źŵ��︨��˵Ĺ̶����� */
    shwNeGain       = pstRefMode->shwNeGain;

    /* ����������е������ź� */
    for (shwCnt = 0; shwCnt < shwFrameLen; shwCnt++)
    {
        shwTemp        = CODEC_OpRound(CODEC_OpL_mult(pshwMcDelay[shwCnt], shwNeGain));

        pshwRcOut[shwCnt] = CODEC_OpSub(pshwRcDelay[shwCnt],shwTemp);
    }

    /* �����ڴ��ͷ� */
    UCOM_MemFree(pshwMcDelay);
    UCOM_MemFree(pshwRcDelay);

    return uwRet;

}

#ifndef _MED_C89_

VOS_UINT32 MED_ANR_2MIC_TdSpaceFractionDelay(
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                        shwDelayValue,
                VOS_INT16                       *pshwMainState,
                VOS_INT16                       *pshwRefState,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut)
{
    VOS_INT16   shwMcStateLen;
    VOS_INT16   shwRcStateLen;
    VOS_INT16   shwFrameLen;
    VOS_INT16   shwIntegerDelayValue;
    VOS_INT16   shwCntM;
    VOS_INT32   swTemp;
    VOS_INT16  *pshwNewMicIn;

    /* �����ڴ� */
    pshwNewMicIn = (VOS_INT16 *)UCOM_MemAlloc((MED_2MIC_SPACESPLIT_MAX_DELAY + CODEC_FRAME_LENGTH_WB) \
                                               * sizeof(VOS_INT16));

    /* ��ȡ��ǰ����֡֡���� */
    shwFrameLen      = MED_PP_GetFrameLength();

    /* �����˲����ĳ�ʼֵ */
    shwMcStateLen   = MED_2MIC_SPACESPLIT_MAX_DELAY;
    shwMcStateLen   = CODEC_OpShl(shwMcStateLen, MED_PP_SHIFT_BY_8);
    shwRcStateLen   = MED_2MIC_SPACESPLIT_MAX_DELAY;
    shwRcStateLen   = CODEC_OpShl(shwRcStateLen, MED_PP_SHIFT_BY_8);

    if ((shwDelayValue > shwMcStateLen)
        || (shwDelayValue > shwRcStateLen))
    {
        /* �ͷ��ڴ� */
        UCOM_MemFree(pshwNewMicIn);

        return UCOM_RET_ERR_PARA;
    }

    /* ȡС��λ��ֵ�����ж� */
    if (0 == (shwDelayValue & 0xFF))
    {
        /* ���ӳ�Ϊ���������������˽��������ӳ� */
        MED_ANR_2MIC_TdSpaceIntegerDelay(pshwMcIn, shwDelayValue, pshwMainState, pshwMcOut);

        UCOM_MemCpy(pshwRcOut, pshwRcIn, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));
    }

    /* ���С��������0.5 (128���㷽��: 0.5 * 2^8) */
    else if (128 == (shwDelayValue & 0xFF))
    {
        /* ��ǰ�źź���ʷ�ĵ���ϳ��µ��ź�֡ */
        UCOM_MemSet(pshwNewMicIn,
                    0,
                    (MED_2MIC_SPACESPLIT_MAX_DELAY + CODEC_FRAME_LENGTH_WB)*sizeof(VOS_INT16));

        UCOM_MemCpy(pshwNewMicIn,
                    pshwMainState,
                    (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1) * sizeof(VOS_INT16));

        UCOM_MemCpy(pshwNewMicIn + (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1),
                    pshwMcIn,
                    (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

        fir_bk(pshwMcOut,
               pshwNewMicIn,
               g_ashwDelayCoef,
               shwFrameLen,
               MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH);
        UCOM_MemCpy(pshwMainState,
                    pshwNewMicIn + shwFrameLen,
                    (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1) * sizeof(VOS_INT16));
        shwIntegerDelayValue = CODEC_OpSub(1920, shwDelayValue);
        MED_ANR_2MIC_TdSpaceIntegerDelay(pshwRcIn, shwIntegerDelayValue, pshwRefState, pshwRcOut);
    }
    else
    {
        UCOM_MemFree(pshwNewMicIn);
        return UCOM_RET_ERR_STATE;
    }
    UCOM_MemFree(pshwNewMicIn);
    return UCOM_RET_SUCC;
}
#else
VOS_UINT32 MED_ANR_2MIC_TdSpaceFractionDelay(
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                        shwDelayValue,
                VOS_INT16                       *pshwMainState,
                VOS_INT16                       *pshwRefState,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut)
{
    VOS_INT16   shwMcStateLen;
    VOS_INT16   shwRcStateLen;
    VOS_INT16   shwFrameLen;
    VOS_INT16   shwIntegerDelayValue;
    VOS_INT16   shwCntM;
    VOS_INT32   swTemp;
    VOS_INT16  *pshwNewMicIn;
    pshwNewMicIn = (VOS_INT16 *)UCOM_MemAlloc((MED_2MIC_SPACESPLIT_MAX_DELAY + CODEC_FRAME_LENGTH_WB) \
                                               * sizeof(VOS_INT16));
    shwFrameLen      = MED_PP_GetFrameLength();
    shwMcStateLen   = MED_2MIC_SPACESPLIT_MAX_DELAY;
    shwMcStateLen   = CODEC_OpShl(shwMcStateLen, MED_PP_SHIFT_BY_8);
    shwRcStateLen   = MED_2MIC_SPACESPLIT_MAX_DELAY;
    shwRcStateLen   = CODEC_OpShl(shwRcStateLen, MED_PP_SHIFT_BY_8);
    if ((shwDelayValue > shwMcStateLen)
        || (shwDelayValue > shwRcStateLen))
    {
        UCOM_MemFree(pshwNewMicIn);
        return UCOM_RET_ERR_PARA;
    }
    if (0 == (shwDelayValue & 0xFF))
    {
        MED_ANR_2MIC_TdSpaceIntegerDelay(pshwMcIn, shwDelayValue, pshwMainState, pshwMcOut);
        UCOM_MemCpy(pshwRcOut, pshwRcIn, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));
    }
    else if (128 == (shwDelayValue & 0xFF))
    {
        UCOM_MemSet(pshwNewMicIn,
                    0,
                    (MED_2MIC_SPACESPLIT_MAX_DELAY + CODEC_FRAME_LENGTH_WB)*sizeof(VOS_INT16));
        UCOM_MemCpy(pshwNewMicIn,
                    pshwMainState,
                    (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1) * sizeof(VOS_INT16));
        UCOM_MemCpy(pshwNewMicIn + (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1),
                    pshwMcIn,
                    (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));
        /* �����ӳ��˲� FIR (�˲������Ϊ�ӳٵ���Ϊ7.5) */
        for (shwCntM = 0; shwCntM < shwFrameLen; shwCntM++)
        {
            swTemp = 0;

            /* �˲� */
            swTemp = CODEC_OpVvMac(pshwNewMicIn + shwCntM, g_ashwDelayCoef, MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH, 0);

            pshwMcOut[shwCntM] = CODEC_OpRound(swTemp);
        }

        /* �����������ʷ��Ϣ����֡���(�˲�������-1)�㣬������һ֡FIR�˲� */
        UCOM_MemCpy(pshwMainState,
                    pshwNewMicIn + shwFrameLen,
                    (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1) * sizeof(VOS_INT16));

        /* �Ը���˽��в��� ���ڵ��������7.5���ӳٶ����(1920����7.5 * 2^8�������) */
        shwIntegerDelayValue = CODEC_OpSub(1920, shwDelayValue);

        MED_ANR_2MIC_TdSpaceIntegerDelay(pshwRcIn, shwIntegerDelayValue, pshwRefState, pshwRcOut);

    }

    else
    {
        /* �ͷ��ڴ� */
        UCOM_MemFree(pshwNewMicIn);

        return UCOM_RET_ERR_STATE;
    }

    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwNewMicIn);

    return UCOM_RET_SUCC;

}
#endif

VOS_UINT32 MED_ANR_2MIC_TdAncNormalMode(
                MED_2MIC_ANC_STRU                       *pstAnc,
                VOS_INT16                               *pshwMcIn,
                VOS_INT16                               *pshwRcIn,
                VOS_INT16                               *pshwCalib,
                VOS_INT16                               *pshwMcOut)
{
    MED_2MIC_VAD_STRU               *pstVad;
    MED_2MIC_LAF_STRU               *pstLaf;
    VOS_INT16                       *pshwMainDelayState;
    VOS_INT16                        shwMainDelayQ;
    VOS_INT16                       *pshwDelayedMc;
    CODEC_VAD_FLAG_ENUM_INT16     enVadFlag;
    VOS_UINT32                       uwSpaceRet;
    VOS_UINT32                       uwLafRet;

    /* �����ڴ�ռ� */
    pshwDelayedMc = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    /* ��ȡ״̬ */
    pstVad                  = &(pstAnc->stVad);
    pstLaf                  = &(pstAnc->stLaf);
    shwMainDelayQ           = pstAnc->shwMainDelayQ;
    pshwMainDelayState      = pstAnc->ashwMainDelayState;

    /* ����ӳٵ�������ź� */
    uwSpaceRet  = MED_ANR_2MIC_TdSpaceIntegerDelay(pshwMcIn, shwMainDelayQ, pshwMainDelayState, pshwDelayedMc);

    /* VAD��� */
    enVadFlag   = MED_ANR_2MIC_TdNoiseVadDetect(pstVad, pshwMcIn, pshwCalib);

    /* LAF */
    uwLafRet    = MED_ANR_2MIC_TdLaf(pstLaf, pshwDelayedMc, pshwRcIn, enVadFlag, pshwMcOut);

    if (  (UCOM_RET_SUCC == uwSpaceRet)
        &&(UCOM_RET_SUCC == uwLafRet))
    {
        /* �ͷŷ�����ڴ� */
        UCOM_MemFree(pshwDelayedMc);

        return UCOM_RET_SUCC;
    }
    else
    {
        /* �ͷŷ�����ڴ� */
        UCOM_MemFree(pshwDelayedMc);

        return UCOM_RET_FAIL;
    }

}
CODEC_VAD_FLAG_ENUM_INT16 MED_ANR_2MIC_TdNoiseVadDetect(
                MED_2MIC_VAD_STRU                       *pstVad,
                VOS_INT16                               *pshwMcIn,
                VOS_INT16                               *pshwRcIn)
{
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32     enSampleRate;
    MED_2MIC_VAD_WB_STRU               *pstVadWb;
    VOS_INT16                           shwVadAlpha;
    VOS_INT16                           shwVadRatioThd;
    VOS_INT16                           shwPsPnRatioLast;
    VOS_INT16                           shwFrameLen;
    VOS_INT16                          *pshwMcXState;
    VOS_INT16                          *pshwMcYState;
    VOS_INT16                          *pshwRcXState;
    VOS_INT16                          *pshwRcYState;
    VOS_INT32                           swPsPower;
    VOS_INT32                           swPnPower;
    VOS_INT16                           shwTemp;
    VOS_INT16                           shwPsPnRatio;

    CODEC_VAD_FLAG_ENUM_INT16        enVadFlag;

    /* ��ȡ��ǰ������ */
    enSampleRate        = MED_2MIC_GetTdSampleRate();

    /* ��ȡ��ǰ֡�� */
    shwFrameLen         = MED_PP_GetFrameLength();

    /* ���ò��� */
    shwVadAlpha         = MED_2MIC_ANR_VAD_ALPHA;
    pstVadWb            = &(pstVad->stVadWb);
    shwVadRatioThd      = pstVad->shwVadRatioThd;
    shwPsPnRatioLast    = pstVad->shwPsPnRatio;

    /* �ֱ�����������ƽ������ */
    if (CODEC_SAMPLE_RATE_MODE_16000 == enSampleRate)
    {
        pshwMcXState      = pstVadWb->ashwMcXState;
        pshwMcYState      = pstVadWb->ashwMcYState;
        pshwRcXState      = pstVadWb->ashwRcXState;
        pshwRcYState      = pstVadWb->ashwRcYState;

        /* ������������źŵĵ�Ƶ������ */
        MED_ANR_2MIC_TdIIRLowPass(pstVadWb, pshwMcIn, pshwMcXState, pshwMcYState, &swPsPower);

        MED_ANR_2MIC_TdIIRLowPass(pstVadWb, pshwRcIn, pshwRcXState, pshwRcYState, &swPnPower);
    }
    else
    {
        /* ����������źŵ�ƽ������ */
        swPsPower = MED_ANR_2MIC_TdCalcMeanPower(pshwMcIn, shwFrameLen);

        /* ���㸨����źŵ�ƽ������ */
        swPnPower = MED_ANR_2MIC_TdCalcMeanPower(pshwRcIn, shwFrameLen);
    }

    /* ��������������ֵ  (����Ҫ��ϸ�) */
    shwTemp         = CODEC_OpRound(CODEC_OpNormDiv_32(swPsPower, swPnPower, MED_PP_SHIFT_BY_28));
    shwTemp         = CODEC_OpMult(CODEC_OpSub(CODEC_OP_INT16_MAX, shwVadAlpha), shwTemp);
    shwPsPnRatio    = CODEC_OpAdd(CODEC_OpMult(shwVadAlpha, shwPsPnRatioLast), shwTemp);

    /* ����������������Ȼ��VAD��־ */
    if (shwPsPnRatio > shwVadRatioThd)
    {
        enVadFlag   = CODEC_VAD_FLAG_SPEECH;
    }
    else
    {
        enVadFlag   = CODEC_VAD_FLAG_NOISE;
    }

    /* ���湦���ױ�ֵ�������´μ��� */
    pstVad->shwPsPnRatio  = shwPsPnRatio;

    return enVadFlag;

}


VOS_UINT32 MED_ANR_2MIC_TdIIRLowPass(
                MED_2MIC_VAD_WB_STRU                    *pstVadWb,
                VOS_INT16                               *pshwMcIn,
                VOS_INT16                               *pshwMcXState,
                VOS_INT16                               *pshwMcYState,
                VOS_INT32                               *pswPowerMean)
{
    VOS_INT16                       shwFilterOrder;
    VOS_INT16                      *pshwFilterNum;
    VOS_INT16                      *pshwFilterDen;
    VOS_INT16                      *pshwTmpMicX;
    VOS_INT16                      *pshwTmpMicY;
    VOS_INT16                      *pshwTmp;
    VOS_INT16                       shwFrameLength;
    VOS_INT16                       shwCntK;
    VOS_INT16                       shwSwSum;

    /* �����ڴ�ռ� */
    pshwTmpMicX     = (VOS_INT16 *)UCOM_MemAlloc((MED_2MIC_ANC_VAD_FILTER_ORDER + CODEC_FRAME_LENGTH_WB) \
                                                 * sizeof(VOS_INT16));
    pshwTmpMicY     = (VOS_INT16 *)UCOM_MemAlloc((MED_2MIC_ANC_VAD_FILTER_ORDER + CODEC_FRAME_LENGTH_WB) \
                                                 * sizeof(VOS_INT16));

    /* ������ȡ */
    shwFilterOrder  = pstVadWb->shwFilterOrder;
    pshwFilterNum   = pstVadWb->pshwFilterNum;
    pshwFilterDen   = pstVadWb->pshwFilterDen;
    shwFrameLength  = MED_PP_GetFrameLength();

    /* ������ʱ�ռ� */
    UCOM_MemCpy(pshwTmpMicX, pshwMcXState, MED_2MIC_ANC_VAD_FILTER_ORDER * sizeof(VOS_INT16));
    UCOM_MemCpy((pshwTmpMicX + MED_2MIC_ANC_VAD_FILTER_ORDER), pshwMcIn, CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    UCOM_MemCpy(pshwTmpMicY, pshwMcYState, MED_2MIC_ANC_VAD_FILTER_ORDER * sizeof(VOS_INT16));
    UCOM_MemSet((pshwTmpMicY + MED_2MIC_ANC_VAD_FILTER_ORDER), 0, CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    for (shwCntK = 0; shwCntK < shwFrameLength; shwCntK++)
    {
        shwSwSum = CODEC_OpMult(pshwFilterNum[0], pshwTmpMicX[shwCntK + shwFilterOrder]);

        shwSwSum = CODEC_OpAdd(shwSwSum, CODEC_OpMult(pshwFilterNum[1], pshwTmpMicX[(shwCntK + shwFilterOrder) - 1]));
        shwSwSum = CODEC_OpSub(shwSwSum, CODEC_OpMult(pshwFilterDen[1], pshwTmpMicY[(shwCntK + shwFilterOrder) - 1]));
        shwSwSum = CODEC_OpAdd(shwSwSum, CODEC_OpMult(pshwFilterNum[2], pshwTmpMicX[(shwCntK + shwFilterOrder) - 2]));
        shwSwSum = CODEC_OpSub(shwSwSum, CODEC_OpMult(pshwFilterDen[2], pshwTmpMicY[(shwCntK + shwFilterOrder) - 2]));
        shwSwSum = CODEC_OpAdd(shwSwSum, CODEC_OpMult(pshwFilterNum[3], pshwTmpMicX[(shwCntK + shwFilterOrder) - 3]));
        shwSwSum = CODEC_OpSub(shwSwSum, CODEC_OpMult(pshwFilterDen[3], pshwTmpMicY[(shwCntK + shwFilterOrder) - 3]));

        pshwTmpMicY[shwCntK + shwFilterOrder] = shwSwSum;
    }

    /* ���㵱ǰ֡�źŵ�ƽ������ */
    pshwTmp = pshwTmpMicY + shwFilterOrder;

    *pswPowerMean = MED_ANR_2MIC_TdCalcMeanPower(pshwTmp, shwFrameLength);

    /* �����˲���״̬ */
    UCOM_MemCpy(pshwMcXState,
                pshwMcIn + (shwFrameLength - shwFilterOrder),
                ((VOS_UINT16)shwFilterOrder - 1) * sizeof(VOS_INT16));

    UCOM_MemCpy(pshwMcYState,
                pshwTmp + (shwFrameLength - shwFilterOrder),
                ((VOS_UINT16)shwFilterOrder - 1) * sizeof(VOS_INT16));

    /* �ͷŷ�����ڴ� */
    UCOM_MemFree(pshwTmpMicX);
    UCOM_MemFree(pshwTmpMicY);

    return UCOM_RET_SUCC;

}




VOS_INT32 MED_ANR_2MIC_TdCalcMeanPower(
                VOS_INT16                       *pshwMicIn,
                VOS_INT16                        shwFrameLength)
{
    VOS_INT16                       shwCnt;
    VOS_INT32                       swTemp;
    VOS_INT32                       swPowerMean;

    /* ��ʼ�� */
    swPowerMean = 0;

    /* ����ƽ���� */
    for (shwCnt = 0; shwCnt < shwFrameLength; shwCnt++)
    {
        swTemp        = CODEC_OpL_mult0(pshwMicIn[shwCnt], pshwMicIn[shwCnt]);
        swPowerMean   = CODEC_OpL_add(swTemp, swPowerMean);
    }

    /* ����ƽ��ֵ */
    swPowerMean       = CODEC_OpNormDiv_32(swPowerMean, (VOS_INT32)(shwFrameLength), 0);
    swPowerMean       = CODEC_OpL_max(swPowerMean, MED_2MIC_MIN_POWER_VALUE);

    return swPowerMean;

}
VOS_UINT32 MED_ANR_2MIC_TdLaf(
                    MED_2MIC_LAF_STRU                       *pstLaf,
                    VOS_INT16                               *pshwMcIn,
                    VOS_INT16                               *pshwRcIn,
                    VOS_INT16                                shwVadFlag,
                    VOS_INT16                               *pshwMcOut)
{
    VOS_INT16               *pshwWeight;
    VOS_INT16               *pshwRcBuf;
    VOS_INT16               *pshwRefShortBuf = MED_NULL;
    VOS_INT16                shwN;
    VOS_INT16                shwStep;
    VOS_INT16                shwUpdateLen;
    VOS_INT32                swInShortPower;
    VOS_INT32                swInShortThd;
    VOS_INT16                shwAlpha;
    VOS_INT32                swMaxRefPower;
    VOS_INT32                swRefShortPower;
    VOS_INT16                shwFrameLen;
    VOS_INT16                shwIndex;
    VOS_INT16                shwCntI;
    VOS_INT16                shwCntK;
    VOS_INT16                shwTmpError = 0;
    VOS_INT16                shwTemp1;
    VOS_INT32                swTemp2;
    VOS_INT32                swTmp3;
    VOS_INT16                shwTmp4;
    VOS_INT32                swTempDiv;
    VOS_INT16                shwTmpIndex;
    VOS_INT32                swTmpPower;
    VOS_INT32               *pswRefPower;

    /* �ڴ���� */
    pswRefPower     = (VOS_INT32 *)UCOM_MemAlloc((2*CODEC_FRAME_LENGTH_WB) * sizeof(VOS_INT32));
    UCOM_MemSet(pswRefPower, 0, (2*CODEC_FRAME_LENGTH_WB) * sizeof(VOS_INT32));

    /* ��ȡ֡�� */
    shwFrameLen     = MED_PP_GetFrameLength();

    pshwWeight      = pstLaf->ashwWeight;                                       /* �˲���ϵ�� */
    pshwRcBuf       = pstLaf->ashwRcBuf;                                        /* ��һ֡������ź� */
    shwN            = pstLaf->shwN;                                             /* �˲������� */
    shwStep         = pstLaf->shwStep;                                          /* �˲���ѧϰ���� */
    swInShortPower  = pstLaf->swInShortPower;                                   /* ��һ֡��ʱ���� */
    shwAlpha        = pstLaf->shwAlpha;                                         /* ƽ��ϵ�� */
    swInShortThd    = pstLaf->swInShortThd;                                     /* ��ʱ������ֵ */

    /* ���渨����źŵ�2֡���ݣ����ں������LAF�˲�(�����������) */
    UCOM_MemCpy(pshwRcBuf, pshwRcBuf + shwFrameLen, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));
    UCOM_MemCpy(pshwRcBuf + shwFrameLen, pshwRcIn, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

    /* ѧϰ�������� */
    swMaxRefPower   = pstLaf->swRefPowerThd;

    /*���㸨����źŵ�ƽ������*/
    swRefShortPower = MED_ANR_2MIC_TdCalcMeanPower(pshwRcIn, shwFrameLen);

    /* ���������Խ�󣬸���Խ�� */
    if (swRefShortPower <= swMaxRefPower)
    {
        shwTemp1 = CODEC_OpRound(CODEC_OpNormDiv_32(swRefShortPower,
                                swMaxRefPower, MED_PP_SHIFT_BY_31));
        shwStep = CODEC_OpMult(shwStep, shwTemp1);
    }

    /* �����ʱƽ������ */
    swTmpPower  = MED_ANR_2MIC_TdCalcMeanPower(pshwMcIn, shwFrameLen);

    /* ����˶�ʱ��������ƽ�� */
    swTemp2 = CODEC_OpL_mpy_32_16(swInShortPower, CODEC_OpSub(CODEC_OP_INT16_MAX, shwAlpha));
    swInShortPower = CODEC_OpL_add(swTemp2, CODEC_OpL_mpy_32_16(swTmpPower, shwAlpha));

    /* ���渨����źŵ�2֡���ݵ�����(��������ڴ�����) */
    for (shwCntK = 0; shwCntK < (2 * shwFrameLen); shwCntK++)
    {
        pswRefPower[shwCntK] = CODEC_OpL_mult0(pshwRcBuf[shwCntK] , pshwRcBuf[shwCntK]);
    }

    shwUpdateLen = (VOS_INT16)MED_2MIC_LAF_WEIGHT_UPDATE_LEN;

    pshwRefShortBuf = pshwRcBuf;

    for (shwIndex = 0; shwIndex < (shwFrameLen / shwUpdateLen); shwIndex++)
    {
        for (shwCntI = 0; shwCntI < shwUpdateLen; shwCntI++)
        {
            shwTmpIndex = (VOS_INT16)(shwIndex * shwUpdateLen) + shwCntI;
            pshwRefShortBuf = (((pshwRcBuf + shwTmpIndex) + shwFrameLen) - shwN) + 1;

            if (pstLaf->shwMaxFar < pstLaf->shwFarMaxThd )
            {
                /* LAF�˲� */
                shwTemp1 = CODEC_OpRound(CODEC_OpVvMacD(pshwWeight, (pshwRefShortBuf+ shwN) -1, (VOS_INT32)shwN, 0));
            }
            else
            {
                shwTemp1 = 0;

            }

            shwTmpError = CODEC_OpSub(pshwMcIn[shwTmpIndex], shwTemp1);
            pshwMcOut[shwTmpIndex] = shwTmpError;
        }

#ifndef _MED_MAX_CYCLE_
        if ((0 == shwVadFlag) && (swInShortPower > swInShortThd))
#else
        if(1)
#endif
        {
            /* LAF�˲�ϵ������ */
            swTmpPower = 0;

            for (shwCntK = 0; shwCntK < shwN; shwCntK++)
            {
                shwCntI    = ((VOS_INT16)(shwIndex * shwUpdateLen) + (shwFrameLen - shwN))+ shwCntK;

                swTmpPower = CODEC_OpL_add(swTmpPower, pswRefPower[shwCntI]);
            }

            swTmpPower = CODEC_OpL_max(swTmpPower , MED_2MIC_MIN_POWER_VALUE);

            swTmp3     = CODEC_OpL_abs((VOS_INT32)shwTmpError);

            swTempDiv  = CODEC_OpNormDiv_32(swTmp3,
                                       swTmpPower,
                                       (VOS_INT16)MED_PP_SHIFT_BY_30);

            if (shwTmpError < 0)
            {
                swTempDiv = CODEC_OpL_negate(swTempDiv);
            }

            for (shwCntI = 0; shwCntI < shwN; shwCntI++)
            {

                swTmp3      = CODEC_OpL_mpy_32_16(swTempDiv , pshwRefShortBuf[(shwN - shwCntI) - 1]);
                shwTmp4     = CODEC_OpSaturate(swTmp3);
                swTmp3      = CODEC_OpL_mult(shwTmp4, shwStep);
                shwTmp4     = CODEC_OpRound(swTmp3);
                pshwWeight[shwCntI] = CODEC_OpAdd(pshwWeight[shwCntI], shwTmp4);
            }

         }
    }

    /* ����״̬ */
    pstLaf->swInShortPower = swInShortPower;

    /* �ڴ��ͷ� */
    UCOM_MemFree(pswRefPower);

    return UCOM_RET_SUCC;

}
#if 0

VOS_INT16 MED_ANR_2MIC_FirConvolute(
                VOS_INT16               *pshwWeight,
                VOS_INT16               *pshwRcIn,
                VOS_INT16                shwLen )
{
    VOS_INT32               swSum = 0;
    VOS_INT16               shwCntI;
    VOS_INT16               shwOut;

    /* �˲� */
    swSum = CODEC_OpVvMacD(pshwWeight, pshwRcIn+shwLen-1, (VOS_INT32)shwLen, 0);
    /*
    for (shwCntI = 0; shwCntI < shwLen; shwCntI++)
    {
       swSum = CODEC_OpL_add(CODEC_OpL_mult(pshwWeight[shwCntI],
                                        pshwRcIn[(shwLen - shwCntI) - 1]), swSum);

    }
    */

    shwOut = CODEC_OpRound(swSum);

    shwOut  = CODEC_OpRound(CODEC_OpVvMacD(pshwWeight, pshwRcIn+shwLen-1, (VOS_INT32)shwLen, 0));

    return shwOut;

}
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

