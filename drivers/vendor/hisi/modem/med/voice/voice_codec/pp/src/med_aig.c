

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "med_aig.h"
#include "med_pp_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
MED_AIG_OBJ_STRU    g_astMedAigObjPool[MED_AIG_MAX_OBJ_NUM];                    /* AGCʵ����Դ�� */
MED_OBJ_INFO        g_stMedAigObjInfo;                                          /* AGC��Ϣ */



/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_VOID* MED_AIG_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_AIG_GetObjInfoPtr,
                    MED_AIG_GetObjPtr,
                    MED_AIG_MAX_OBJ_NUM,
                    sizeof(MED_AIG_OBJ_STRU)));
}


VOS_UINT32 MED_AIG_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_AIG_OBJ_STRU       *pstObj;

    pstObj      = (MED_AIG_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AIG_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_AIG_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_AIG_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32              uwRet;
    MED_AIG_OBJ_STRU       *pstObj    = (MED_AIG_OBJ_STRU *)pstInstance;
    MED_AIG_NV_STRU        *pstNv     = &(pstObj->stNv);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AIG_GetObjInfoPtr, pstObj);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_AIG_PARAM_LEN)
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡNV��Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            CODEC_OpVecCpy(pshwParam, (VOS_INT16 *)pstNv, MED_AIG_PARAM_LEN);
        }
    }

    return uwRet;


}


VOS_UINT32 MED_AIG_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen,
                VOS_INT32               swSampleRate)
{
    VOS_UINT32              uwRet;
    MED_AIG_OBJ_STRU       *pstObj    = (MED_AIG_OBJ_STRU *)pstInstance;
    MED_AIG_NV_STRU        *pstNv     = &(pstObj->stNv);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AIG_GetObjInfoPtr, pstObj);

    /* ��κϷ������Զ�ָ������λ�ý���������д����� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻��������ε�ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_AIG_PARAM_LEN)
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡNV��Ϣ */
            CODEC_OpVecCpy((VOS_INT16 *)pstNv, pshwParam, MED_AIG_PARAM_LEN);

            if (MED_SAMPLE_8K == swSampleRate)
            {
                pstObj->shwFrmLen = MED_FRM_LEN_8K;
            }
            else if (MED_SAMPLE_16K == swSampleRate)
            {
                pstObj->shwFrmLen = MED_FRM_LEN_16K;
            }
            else
            {
                pstObj->shwFrmLen = MED_FRM_LEN_48K;
            }

            pstObj->shwGainMaxRefThd    = 10*256;
            pstObj->shwGainMax          = 10*256;
            pstObj->swdBfs0             = MED_AIG_RX_0_DBFS;

            pstObj->shwMaxIn            = pstNv->shwMaxIn;
            pstObj->shwSpeechCnt        = pstNv->shwSpeechCntInit;

            pstObj->shwLastPowerIndBfs  = pstNv->shwLastPowerIndBfsInit;
            pstObj->shwLastGain         = pstNv->shwLastGainInit;
            pstObj->shwLastPowerOutdBfs = pstNv->shwLastPowerOutdBfsInit;
            pstObj->shwMakeupGain       = pstNv->shwMakeupGain;

        }
    }

    return uwRet;

}


VOS_UINT32 MED_AIG_Main(
                 MED_AIG_OBJ_STRU    *pstAig,
                 VOS_INT16           *pshwIn,
                 VOS_INT16           *pshwRef,
                 VOS_INT16           *pshwOut,
                 VOS_INT16            shwVadFlag)
{
    VOS_INT16                         shwCnt;
    VOS_INT16                         shwFrameLen;
    VOS_INT16                         shwSpeechCnt;

    VOS_INT16                         shwPowerRMS;
    VOS_INT16                         shwPowerRMSRef;
    VOS_INT16                         shwNormShift;
    VOS_INT16                         shwNormShiftRef;

    VOS_INT16                         shwGainAIG;
    VOS_INT16                         shwGainAIGtemp;
    VOS_INT16                         shwAlpha;
    VOS_INT16                         shwGainLast;

    VOS_INT32                         swGaintemp1;
    VOS_INT32                         swGaintemp2;
    VOS_INT32                         swGaintemp3;

    MED_AIG_NV_STRU                  *pstNv;

    /* ������ȡ */
    pstNv       = &(pstAig->stNv);
    shwFrameLen = pstAig->shwFrmLen;


    /* VAD hangover���̴��� */
    shwSpeechCnt = pstAig->shwSpeechCnt;

    if(shwVadFlag == 1)
    {
        shwSpeechCnt = pstNv->shwVADholdlenth;
    }
    else
    {
        shwSpeechCnt = shwSpeechCnt - 1;
        shwSpeechCnt = CODEC_OpMax(0, shwSpeechCnt);
    }

    pstAig->shwSpeechCnt = shwSpeechCnt;

    if ((CODEC_SWITCH_OFF == pstNv->enEnable)
     || (pstAig->shwSpeechCnt <= 0))
    {
        CODEC_OpVecCpy(pshwOut, pshwIn, shwFrameLen);

        return UCOM_RET_SUCC;
    }

    pstAig->shwMaxIn = CODEC_OpVecMaxAbs(pshwIn, shwFrameLen, MED_NULL);

    MED_AIG_PowerCal(pshwIn, &shwPowerRMS, &shwNormShift, shwFrameLen);

    MED_AIG_PowerCal(pshwRef, &shwPowerRMSRef, &shwNormShiftRef, shwFrameLen);

    MED_AIG_GainCal(pstAig,
                    shwPowerRMS,
                    shwNormShift-3,
                    shwPowerRMSRef,
                    shwNormShiftRef-3,
                    &shwGainAIG,
                    &shwAlpha);

    shwGainLast = pstAig->shwLastGain;

    for (shwCnt = 0; shwCnt < shwFrameLen; shwCnt++)
    {
        swGaintemp1     = CODEC_OpL_mult0(shwAlpha,shwGainAIG);
        swGaintemp2     = CODEC_OpL_mult0(CODEC_OpSub(MED_AIG_ALPHA_NORM,shwAlpha), shwGainLast);
        swGaintemp3     = CODEC_OpL_add(swGaintemp1,swGaintemp2);
        swGaintemp3     = CODEC_OpL_shl(swGaintemp3, -7);
        shwGainAIGtemp  = CODEC_OpSaturate(swGaintemp3);
        pshwOut[shwCnt] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(CODEC_OpL_shl(shwGainAIGtemp,5),pshwIn[shwCnt]));
        shwGainLast     = shwGainAIGtemp;
    }

    pstAig->shwLastGain = shwGainLast;

    return UCOM_RET_SUCC;

}


VOS_VOID MED_AIG_PowerCal(
                VOS_INT16               *pshwIn,
                VOS_INT16               *pshwPowerRMS,
                VOS_INT16               *pshwNormShift,
                VOS_INT16                shwFrameLen)
{
    VOS_INT16                            shwCntI;
    VOS_INT16                            shwInMax;
    VOS_INT16                            shwPowerRMS;
    VOS_INT16                            shwNormShift;
    VOS_INT16                           *pshwInNorm;

    VOS_INT32                            swPowerAIG;

    /* �ڴ���� */
    pshwInNorm = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

    /* ��һ��ֵ���� */
    shwInMax = CODEC_OpVecMaxAbs(pshwIn, shwFrameLen, MED_NULL);

    shwNormShift = CODEC_OpNorm_s(shwInMax);

    /* �����źŹ�һ�� */
    for (shwCntI = 0; shwCntI < shwFrameLen; shwCntI++)
    {
        pshwInNorm[shwCntI] = CODEC_OpShl(pshwIn[shwCntI], shwNormShift - 3);
    }

    /* ƽ���������� */
    swPowerAIG = CODEC_OpVvMac(pshwInNorm, pshwInNorm, shwFrameLen, 0);

    swPowerAIG = CODEC_OpL_shr(swPowerAIG, 1);

    swPowerAIG = CODEC_OpNormDiv_32(swPowerAIG, shwFrameLen, 0);

    /* RMS���� */
    shwPowerRMS = CODEC_OpSqrt(swPowerAIG);

    /* RMS��shwNormShift���*/
    *pshwPowerRMS  = shwPowerRMS;
    *pshwNormShift = shwNormShift;

    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwInNorm);

}
VOS_VOID MED_AIG_GainCal(
                MED_AIG_OBJ_STRU        *pstAig,
                VOS_INT16                shwPowerRMS,
                VOS_INT16                shwNormShift,
                VOS_INT16                shwPowerRMSRef,
                VOS_INT16                shwNormShiftRef,
                VOS_INT16               *pshwGainAIG,
                VOS_INT16               *pshwAlpha)
{
    MED_AIG_NV_STRU                     *pstNv;

    VOS_INT16                            shwDnCompThr;
    VOS_INT16                            shwUpCompThr;
    VOS_INT16                            shwDnExpThr;
    VOS_INT16                            shwDnCompSlop;
    VOS_INT16                            shwUpCompSlop;
    VOS_INT16                            shwDnExpSlop;
    VOS_INT16                            shwLastPowerIndBfs;
    VOS_INT16                            shwAlphaThd;
    VOS_INT16                            shwGainAlphaattack;
    VOS_INT16                            shwLastPowerOutdBfs;
    VOS_INT16                            shwGainAlpharelease;
    VOS_INT16                            shwExpectMax;
    VOS_INT16                            shwGainMindB;
    VOS_INT16                            shwGainMaxRefThd;


    VOS_INT16                            shwLogshift;
    VOS_INT16                            shwPowerInDB;
    VOS_INT16                            shwPowerIn0dBfs;
    VOS_INT16                            shwPowerIndBfs;
    VOS_INT16                            shwPowerIndBfstemp;

    VOS_INT16                            shwLogshiftref;
    VOS_INT16                            shwPowerInDBref;
    VOS_INT16                            shwPowerIndBfsref;
    VOS_INT16                            shwPowerIndBfsreftemp;

    VOS_INT32                            swPowerIn;
    VOS_INT32                            swPowerInref;
    VOS_INT32                            swdBfs0;

    VOS_INT32                            swPowerouttemp1;
    VOS_INT32                            swPowerouttemp2;
    VOS_INT32                            swPowerouttemp3;
    VOS_INT32                            swPowerouttemp4;
    VOS_INT16                            shwPowerouttemp5;
    VOS_INT32                            swPowerouttemp6;

    VOS_INT32                            swMindB;
    VOS_INT32                            swMaxIn;

    VOS_INT16                            shwPoweroutdBfs;
    VOS_INT16                            shwPoweroutmin;
    VOS_INT16                            shwMaxIn;
    VOS_INT16                            shwLogshiftmax;
    VOS_INT16                            shwMaxInDB;
    VOS_INT16                            shwExpectMaxDB;
    VOS_INT16                            shwMaxAmGaindB;
    VOS_INT16                            shwMaxAmGaindBtemp;

    VOS_INT32                            swGaintemp;
    VOS_INT32                            swInd;

    VOS_INT16                            shwGain;
    VOS_INT16                            shwLastGaindB;
    VOS_INT16                            shwMaxGainDelta;
    VOS_INT16                            shwGainMax;
    VOS_INT16                            shwGainMaxRef;
    VOS_INT16                            shwGainLinear;
    VOS_INT16                            shwMakeupGain;

    /* NV������ȡ */
    pstNv               = &(pstAig->stNv);
    shwDnCompThr        = pstNv->shwDnCompThr;
    shwUpCompThr        = pstNv->shwUpCompThr;
    shwDnExpThr         = pstNv->shwDnExpThr;
    shwDnCompSlop       = pstNv->shwDnCompSlop;
    shwUpCompSlop       = pstNv->shwUpCompSlop;
    shwDnExpSlop        = pstNv->shwDnExpSlop;
    swMindB             = (VOS_INT32)(pstNv->shwMindB);
    shwAlphaThd         = pstNv->shwAlphaThd;
    shwGainAlphaattack  = pstNv->shwGainAlphaattack;
    shwGainAlpharelease = pstNv->shwGainAlpharelease;
    shwExpectMax        = pstNv->shwExpectMax;
    shwGainMindB        = pstNv->shwGainMindB;
    shwMaxGainDelta     = pstNv->shwMaxGainDelta;

    /* ��NV������ȡ */
    swdBfs0             = pstAig->swdBfs0;
    shwMaxIn            = pstAig->shwMaxIn;
    shwLastPowerIndBfs  = pstAig->shwLastPowerIndBfs;
    shwLastGaindB       = pstAig->shwLastGaindB;
    shwLastPowerOutdBfs = pstAig->shwLastPowerOutdBfs;
    shwGainMax          = pstAig->shwGainMax;
    shwGainMaxRefThd    = pstAig->shwGainMaxRefThd;
    shwMakeupGain       = pstAig->shwMakeupGain;

    /* ���빦��RMSת����dBֵ */
    swPowerIn             = CODEC_OpL_Comp(0, shwPowerRMS);
    swPowerIn             = CODEC_OpL_shl(swPowerIn, 2);
    shwLogshift           = shwNormShift + 1 + 2 ;
    shwPowerInDB          = CODEC_OpLog10(shwLogshift, swPowerIn);
    shwPowerIn0dBfs       = CODEC_OpLog10(0, swdBfs0);
    shwPowerIndBfs        = CODEC_OpSub(shwPowerInDB, shwPowerIn0dBfs);
    shwPowerIndBfstemp    = CODEC_OpShl(shwPowerIndBfs, 2);
    shwPowerIndBfs        = CODEC_OpAdd(shwPowerIndBfs, shwPowerIndBfstemp);

    /* ����REF����RMSת����dBֵ */
    swPowerInref          = CODEC_OpL_Comp(0, shwPowerRMSRef);
    swPowerInref          = CODEC_OpL_shl(swPowerInref, 2);
    shwLogshiftref        = shwNormShiftRef + 1 + 2 ;
    shwPowerInDBref       = CODEC_OpLog10(shwLogshiftref, swPowerInref);
    shwPowerIn0dBfs       = CODEC_OpLog10(0, swdBfs0);
    shwPowerIndBfsref     = CODEC_OpSub(shwPowerInDBref, shwPowerIn0dBfs);
    shwPowerIndBfsreftemp = CODEC_OpShl(shwPowerIndBfsref, 2);
    shwPowerIndBfsref     = CODEC_OpAdd(shwPowerIndBfsref, shwPowerIndBfsreftemp);

    /* �������빦��RMSѡ����DRC���ĸ�������д�����������dBֵ��alpha�˲�ϵ�� */
    if (shwPowerIndBfs >= shwDnCompThr)
    {
        swPowerouttemp1   = (CODEC_OpL_mult0(shwDnCompSlop, shwPowerIndBfs));
        swPowerouttemp2   = (CODEC_OpL_mult0(CODEC_OpSub(MED_AIG_SLOP_NORM, shwDnCompSlop), shwDnCompThr));
        swPowerouttemp3   = CODEC_OpL_add(swPowerouttemp1,swPowerouttemp2);
        swPowerouttemp3   = CODEC_OpL_shl(swPowerouttemp3,-7);
        shwPoweroutdBfs   = CODEC_OpSaturate(swPowerouttemp3);
        shwGain           = CODEC_OpSub(shwPoweroutdBfs,shwPowerIndBfs);
    }
    else if (shwPowerIndBfs >= shwUpCompThr)
    {
        shwGain = 0;
    }
    else if (shwPowerIndBfs >= shwDnExpThr)
    {
        swPowerouttemp1   = CODEC_OpL_mult0(shwUpCompSlop, shwPowerIndBfs);
        swPowerouttemp2   = CODEC_OpL_mult0(CODEC_OpSub(MED_AIG_SLOP_NORM, shwUpCompSlop), shwUpCompThr);
        swPowerouttemp3   = CODEC_OpL_add(swPowerouttemp1, swPowerouttemp2);
        swPowerouttemp3   = CODEC_OpL_shl(swPowerouttemp3, -7);
        shwPoweroutdBfs   = CODEC_OpSaturate(swPowerouttemp3);

        shwGain = CODEC_OpSub(shwPoweroutdBfs, shwPowerIndBfs);
    }
    else
    {
        swPowerouttemp1   = CODEC_OpL_mult0(shwUpCompSlop, shwDnExpThr);
        swPowerouttemp2   = CODEC_OpL_mult0(CODEC_OpSub(MED_AIG_SLOP_NORM, shwUpCompSlop), shwUpCompThr);
        swPowerouttemp3   = CODEC_OpL_add(swPowerouttemp1, swPowerouttemp2);
        swPowerouttemp3   = CODEC_OpL_shl(swPowerouttemp3, -7);

        shwPoweroutmin    = CODEC_OpSub(shwDnExpThr,
                                      CODEC_OpSaturate(CODEC_OpNormDiv_32(CODEC_OpL_shl(CODEC_OpL_sub(swPowerouttemp3, swMindB),
                                                                                  7),
                                                                      shwDnExpSlop,
                                                                      0)));

        if (shwPowerIndBfs >= shwPoweroutmin)
        {
            shwPowerouttemp5 = CODEC_OpSub(shwDnExpThr, shwPowerIndBfs);
            swPowerouttemp4  = CODEC_OpL_mult0(shwPowerouttemp5, shwDnExpSlop);
            swPowerouttemp4  = CODEC_OpL_shl(swPowerouttemp4, -7);
            swPowerouttemp6  = CODEC_OpL_sub(swPowerouttemp3, swPowerouttemp4);

            shwPoweroutdBfs  = CODEC_OpSaturate(swPowerouttemp6);
        }
        else
        {
            shwPoweroutdBfs = CODEC_OpSaturate(swMindB);
        }

        shwGain = CODEC_OpSub(shwPoweroutdBfs,shwPowerIndBfs);

    }

    /* �����ǰ������ǰһ֡������ͬʱ ǰһ֡������С��������˲� */
    if (((shwPowerIndBfs - shwLastPowerIndBfs) >= shwAlphaThd)
     && (shwLastPowerIndBfs < (-40*256)))
    {
        *pshwAlpha = shwGainAlphaattack;
    }
    else if (((shwDnCompThr - (3*256))  - shwLastPowerOutdBfs) <= (shwPowerIndBfs - shwLastPowerIndBfs))
    {
        /* �����ǰ֡������ǰһ֡���������� ����ǰһ֡��������������ѹ�����޵Ĳ� ������3dB�� ������˲�  */
        *pshwAlpha = shwGainAlphaattack;
    }
    else
    {
        /* �����������˲� */
        *pshwAlpha = shwGainAlpharelease;
    }

    /* �����������ֵ�͵�ǰ֡���ֵ�Ĳ���㵱ǰ��ʩ�ӵ�������� */
    swMaxIn            = CODEC_OpL_Comp(0,shwMaxIn);
    swMaxIn            = CODEC_OpL_shl(swMaxIn,2);
    shwLogshiftmax     = 3;
    shwMaxInDB         = CODEC_OpLog10(shwLogshiftmax, swMaxIn);
    shwExpectMaxDB     = CODEC_OpLog10(0, shwExpectMax);
    shwMaxAmGaindB     = CODEC_OpSub(shwExpectMaxDB, shwMaxInDB);
    shwMaxAmGaindBtemp = CODEC_OpShl(shwMaxAmGaindB, 2);
    shwMaxAmGaindB     = CODEC_OpAdd(shwMaxAmGaindB, shwMaxAmGaindBtemp);

    /* �������ޱ��� */
    shwGain            = CODEC_OpMax(shwGain, shwGainMindB);

    /* �����˲��������迼�� */
    if (((shwPowerIndBfs - shwLastPowerIndBfs) >= shwAlphaThd)
     && (shwLastPowerIndBfs < (-40*256)))
    {

    }
    else
    {
        shwGain = CODEC_OpMax(shwGain, shwLastGaindB - shwMaxGainDelta);
        shwGain = CODEC_OpMin(shwGain, shwLastGaindB + shwMaxGainDelta);
    }

    /* ���治�ܳ������ȼ���õ����������  */
    shwGain = CODEC_OpMin(shwGain, shwMaxAmGaindB);

    /* ������ʷֵ */
    pstAig->shwLastPowerIndBfs  = shwPowerIndBfs;
    pstAig->shwLastGaindB       = shwGain;
    pstAig->shwLastPowerOutdBfs = shwGain + shwPowerIndBfs;

    /* ���治�ܳ������ȼ���õ���������� */
    shwGain       = CODEC_OpMin(shwGainMax, shwGain);

    /* ���治�ܳ���MBDRC����֮ǰ���ź������ı仯���� ���ǲ���Ϊ��ʷֵ���� */
    shwGainMaxRef = CODEC_OpSub( CODEC_OpAdd(shwPowerIndBfsref, shwGainMaxRefThd), shwPowerIndBfs);

    shwGain = CODEC_OpMin(shwGainMaxRef, shwGain);

    /* �Ѿ���alpha�˲�������dBֵת��������ֵ */
    shwGain       = CODEC_OpAdd(shwGain, shwMakeupGain);
    shwGain       = CODEC_OpSub(shwGain, MED_AIG_GAIN_30DB_NORM);                 /* �˴��ԣ�30dB��Ϊ��һ����� */
    swGaintemp   = CODEC_OpL_Comp(shwGain,0);
    swInd         = CODEC_OpL_shl(CODEC_OpL_mpy_32_16(swGaintemp, 1638), 2);
    swInd         = CODEC_OpL_min(-1, swInd);
    shwGainLinear = CODEC_OpExtract_h(CODEC_OpExp10(swInd));

    /* AIG������� */
    *pshwGainAIG  = shwGainLinear;

}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

