

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/

#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "med_agc.h"
#include "med_pp_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
MED_AGC_OBJ_STRU    g_astMedAgcObjPool[MED_AGC_MAX_OBJ_NUM];                    /* AGCʵ����Դ�� */
MED_OBJ_INFO        g_stMedAgcObjInfo;                                          /* AGC��Ϣ */

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


VOS_VOID* MED_AGC_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_AGC_GetObjInfoPtr,
                    MED_AGC_GetObjPtr,
                    MED_AGC_MAX_OBJ_NUM,
                    sizeof(MED_AGC_OBJ_STRU)));
}


VOS_UINT32 MED_AGC_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_AGC_OBJ_STRU       *pstObj;
    pstObj      = (MED_AGC_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AGC_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_AGC_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_AGC_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32              uwRet;
    MED_AGC_OBJ_STRU       *pstObj    = (MED_AGC_OBJ_STRU *)pstInstance;
    MED_AGC_NV_STRU        *pstNv     = &(pstObj->stNv);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AGC_GetObjInfoPtr, pstObj);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_AGC_PARAM_LEN)
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ���������롢�����������롢ƫ��������������ʹ�������Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0] = (VOS_INT16)(pstNv->enEnable);
            pshwParam[1] = pstNv->shwMaxIn;
            pshwParam[2] = pstNv->shwMaxOut;
            pshwParam[3] = pstNv->shwFlrNoise;
            pshwParam[4] = pstNv->shwOffset;
            pshwParam[5] = (VOS_INT16)pstNv->enClearNoiseEn;
        }
    }

    return uwRet;

}


VOS_UINT32 MED_AGC_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32              uwRet;
    MED_AGC_OBJ_STRU       *pstObj    = (MED_AGC_OBJ_STRU *)pstInstance;
    MED_AGC_NV_STRU        *pstNv     = &(pstObj->stNv);
    VOS_INT16               shwTemp1, shwTemp2;
    VOS_INT32               swTemp3,  swTemp4;

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AGC_GetObjInfoPtr, pstObj);
    /* ��ֹ�������*/
    if(0 == (pshwParam[1] - pshwParam[3]))
    {
        uwRet = UCOM_RET_FAIL;
    }

    /* ��κϷ������Զ�ָ������λ�ý���������д����� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻��������ε�ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_AGC_PARAM_LEN)
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ���������롢�����������롢ƫ��������������ʹ�������Ϣ */
            pstNv->enEnable       = (VOS_UINT16)pshwParam[0];
            pstNv->shwMaxIn       = pshwParam[1];
            pstNv->shwMaxOut      = pshwParam[2];
            pstNv->shwFlrNoise    = pshwParam[3];
            pstNv->shwOffset      = pshwParam[4];
            pstNv->enClearNoiseEn = (VOS_UINT16)pshwParam[5];

            /* ��ʷ������Ϣ��ʼ�� */
            pstObj->shwLastGain = MED_AGC_LASTGAIN_INIT;

            /* ��ȡ֡����Ϣ */
            pstObj->shwFrmLen   = MED_PP_GetFrameLength();

            /* ����ֱ�߲��� */
            /* ����б��LineK = (MaxOut - Offset)/(MaxIn - FlrNoise) */
            shwTemp1        = CODEC_OpSub(pstNv->shwMaxOut, pstNv->shwOffset);
            shwTemp2        = CODEC_OpSub(pstNv->shwMaxIn, pstNv->shwFlrNoise);
            pstObj->shwLineK= CODEC_OpDiv_s(shwTemp1, shwTemp2);                          /* 16bit Q15 */

            /* �������Ʒ���LineB = (MaxIn*Offset - MaxOut*FlrNoise)/(MaxIn - FlrNoise) */
            swTemp3         = CODEC_OpL_mult0(pstNv->shwMaxIn, pstNv->shwOffset);         /* ��ʱ������Q0 */
            swTemp4         = CODEC_OpL_mult0(pstNv->shwMaxOut, pstNv->shwFlrNoise);      /* ��ʱ������Q0 */
            swTemp3         = CODEC_OpL_sub(swTemp3, swTemp4);                            /* ��ʱ������Q0 */
            swTemp4         = CODEC_OpL_deposit_l(CODEC_OpSub(pstNv->shwMaxIn,
                                                pstNv->shwFlrNoise));                   /* ��ʱ������Q0 */

            pstObj->shwLineB= CODEC_OpSaturate(swTemp3 / swTemp4);

        }
    }

    return uwRet;

}
VOS_UINT32 MED_AGC_Main(
                VOS_VOID               *pstAgcInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16               shwVadFlag,
                VOS_INT16              *pshwOut)
{
    /* �ֲ�������ʼ�� */
    MED_AGC_OBJ_STRU   *pstAgc          = (MED_AGC_OBJ_STRU*)pstAgcInstance;
    MED_AGC_NV_STRU    *pstNv           = &(pstAgc->stNv);                      /* NV�� */
    VOS_INT16           shwLastGain     = pstAgc->shwLastGain;                  /* ��һ֡��AGC���� */
    VOS_INT16           shwFrmLen       = pstAgc->shwFrmLen;                    /* ֡�� */
    VOS_INT16           shwMaxOut       = pstNv->shwMaxOut;                     /* ������ */
    VOS_INT16           shwAverEng;                                             /* ƽ������ */
    VOS_INT16           shwIncStep;                                             /* �������� */
    VOS_INT16           shwDecStep;                                             /* �½����� */
    VOS_INT16           shwGain;                                                /* ��ǰ AGC ���� */
    VOS_INT32           swCnt;                                                  /* �ڲ�ѭ�������� */
    VOS_INT32           swFrmOutQ11;                                            /* ��ʱ���� 32bit,Q11 */
    VOS_INT16           shwGainMax;                                             /* ��ǰ AGC ������������ֵ */
    VOS_INT16           shwGainMin;                                             /* ��ǰ AGC �����������Сֵ */

    /* �ж�AGCģ���Ƿ��ѱ����� */
    if (CODEC_OBJ_UNUSED == pstAgc->enIsUsed)
    {
        /* δ��ʼ�������ش��� */
        return UCOM_RET_FAIL;
    }

    /* �ж�AGCģ���Ƿ�ʹ�� */
    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        /* AGC��ʹ�� */
        CODEC_OpVecCpy(pshwOut, pshwIn, shwFrmLen);
        return UCOM_RET_SUCC;
    }

    /* ����֡ƽ������ */
    shwAverEng      = CODEC_OpVecMeanAbs(pshwIn, shwFrmLen);
    shwAverEng      = CODEC_OpMax(shwAverEng, MED_AGC_AVERENG_MIN);

    /* �����Լ�������� */
    shwIncStep      = MED_AGC_INC_STEP_NORMAL;                                  /* �������Ӳ��� 16bit Q14 */

    /* ������֡��������̫С */
    if ((CODEC_VAD_FLAG_NOISE == shwVadFlag) || (shwAverEng < pstNv->shwFlrNoise))
    {
        shwGain     = MED_AGC_LASTGAIN_INIT;                                    /* ���� 16bit Q10 */
        shwDecStep  = MED_AGC_DEC_STEP_NORMAL;                                  /* ����˥������ 16bit Q15 */
    }
    /* ����֡����̫�� */
    else if (shwAverEng > pstNv->shwMaxIn)
    {
        shwGain     = CODEC_OpSaturate(CODEC_OpNormDiv_32(CODEC_OpL_deposit_l(shwMaxOut),
                                     CODEC_OpL_deposit_l(shwAverEng), MED_PP_SHIFT_BY_10));
        shwDecStep  = MED_AGC_DEC_STEP_FAST;
    }
    /* ���Ե����� */
    else
    {
        shwGain     = CODEC_OpAdd(CODEC_OpShr_r(pstAgc->shwLineK, MED_PP_SHIFT_BY_5),
                                (VOS_INT16)CODEC_OpNormDiv_32((VOS_INT32)pstAgc->shwLineB, (VOS_INT32)shwAverEng,
                                MED_PP_SHIFT_BY_10));
        shwDecStep  = MED_AGC_DEC_STEP_NORMAL;                                  /* �½�������16bitQ15 */
    }

    /* �������ƣ�����ı仯�������½��������ϣ������������� */
    shwGainMax      = CODEC_OpSaturate(CODEC_OpL_shr_r(CODEC_OpL_mult0(shwLastGain, shwIncStep), MED_PP_SHIFT_BY_14));
    shwGainMin      = CODEC_OpMult_r(shwLastGain, shwDecStep);
    shwGain         = CODEC_OpMin(shwGain, shwGainMax);
    shwGain         = CODEC_OpMax(shwGain, shwGainMin);

    /* ������ʷ���� */
    pstAgc->shwLastGain = shwGain;

    /* ��ǰ����ʩ�� */
    for (swCnt = 0; swCnt < shwFrmLen; swCnt++)
    {
        swFrmOutQ11     = CODEC_OpL_mult(pshwIn[swCnt], shwGain);
        pshwOut[swCnt] = CODEC_OpSaturate(CODEC_OpL_shr_r(swFrmOutQ11, MED_PP_SHIFT_BY_11));
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 MED_AGC_PreClearNoise(
                VOS_VOID               *pstAgcInstance,
                VOS_INT16              *psIn,
                VOS_INT16              *psOut)
{
    VOS_INT32           swPow           = 0;
    MED_AGC_OBJ_STRU   *pstAgc          = (MED_AGC_OBJ_STRU*)pstAgcInstance;
    MED_AGC_NV_STRU    *pstNv           = &(pstAgc->stNv);
    VOS_INT16           shwFrameLength  = MED_PP_GetFrameLength();

    /* �ж�AGCǰ���������Ƿ�ʹ�� */
    if (CODEC_SWITCH_OFF == pstNv->enClearNoiseEn)
    {
        /* �������㲻ʹ�� */
        CODEC_OpVecCpy(psOut, psIn, (VOS_INT32)shwFrameLength);
        return UCOM_RET_SUCC;
    }

    /*���㵱ǰ֡������*/
    swPow = CODEC_OpVvMac(psIn, psIn, (VOS_INT32)shwFrameLength, swPow);

    if (CODEC_FRAME_LENGTH_WB == shwFrameLength)
    {
        swPow = CODEC_OpL_shr(swPow, 2);
    }
    else
    {
        swPow = CODEC_OpL_shr(swPow, 1);
    }

    /*���������Ż�*/
    /*�������С��-66dBm0�������㱾֡*/
    if(swPow < MED_AGC_CLEAR_RX_MUT)
    {
        CODEC_OpVecSet(psOut, (VOS_INT32)shwFrameLength, 0);
    }
    else
    {
        CODEC_OpVecCpy(psOut, psIn, (VOS_INT32)shwFrameLength);
    }

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif





