

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/

#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "med_gain.h"
#include "med_pp_comm.h"
#include "med_avc.h"

//#include "ucom_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* AVCʵ����Դ�� */
MED_AVC_OBJ_STRU    g_astMedAvcObjPool[MED_AVC_MAX_OBJ_NUM];

/* �����ּ����, Q15 */
VOS_INT16           g_ashwMedAvcStepLag[MED_AVC_NOISE_STEPS_NUM] = {16423, 16423, 16423, 16423};

/* ������ձ�, Q11 */
VOS_INT16           g_ashwMedAvcGainTab[MED_AVC_NOISE_STEPS_NUM] = {16268, 8153, 4086, 2048};

/* ��ص���Ϣ */
MED_OBJ_INFO        g_stMedAvcObjInfo;

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_VOID* MED_AVC_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_AVC_GetObjInfoPtr,
                    MED_AVC_GetObjPtr,
                    MED_AVC_MAX_OBJ_NUM,
                    sizeof(MED_AVC_OBJ_STRU)));
}


VOS_UINT32 MED_AVC_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_AVC_OBJ_STRU       *pstObj;
    pstObj      = (MED_AVC_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AVC_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_AVC_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_AVC_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32                uwRet;
    MED_AVC_OBJ_STRU         *pstAvc    = (MED_AVC_OBJ_STRU*)pstInstance;       /* ��ʱAVCָ�� */
    MED_AVC_NV_STRU          *pstNv     = &(pstAvc->stNv);                      /* NV��ָ�� */

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AVC_GetObjInfoPtr, pstAvc);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_AVC_PARAM_LEN)
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ����˲�ϵ������Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0]    = (VOS_INT16)(pstNv->enEnable);                     /* ʹ���� */
            pshwParam[1]    = pstNv->shwGainMaxInc;                             /* ������� */
            pshwParam[2]    = pstNv->shwGainMaxDec;                             /* ����½� */
            pshwParam[3]    = pstNv->shwSensitivityOffset;                      /* ������˷�������ƫ�� */
            pshwParam[4]    = pstNv->shwAntiSatMaxLevel;                        /* �����ʹ���������� */
            CODEC_OpVecCpy(&pshwParam[5], pstNv->ashwNoiseStep, MED_AVC_NOISE_STEPS_NUM);
        }
    }

    return uwRet;

}
VOS_UINT32 MED_AVC_SetPara (
                VOS_VOID                        *pstInstance,
                VOS_INT16                       *pshwParam,
                VOS_INT16                        shwParaLen,
                MED_PP_MIC_NUM_ENUM_UINT16       enPpMicNum)
{
    VOS_UINT32           uwRet;
    MED_AVC_OBJ_STRU    *pstAvc         = (MED_AVC_OBJ_STRU*)pstInstance;       /* ��ʱAVCָ�� */
    MED_AVC_NV_STRU     *pstNv          = &(pstAvc->stNv);                      /* NV��ָ�� */
    VOS_INT32            swCnt;                                                 /* ������ */

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_AVC_GetObjInfoPtr, pstAvc);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_AVC_PARAM_LEN)
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ֵʹ��������о�ϵ�����˹�����������ޣ�����ƽ��ϵ�� */
            pstNv->enEnable                    = (VOS_UINT16)pshwParam[0];      /* ʹ���� */
            pstNv->shwGainMaxInc               = pshwParam[1];                  /* �������,Q14 */
            pstNv->shwGainMaxDec               = pshwParam[2];                  /* ����½�,Q14 */
            pstNv->shwSensitivityOffset        = pshwParam[3];                  /* ����½� */
            pstNv->shwAntiSatMaxLevel          = pshwParam[4];                  /* �����ʹ���������� */
            CODEC_OpVecCpy(pstNv->ashwNoiseStep, &pshwParam[5], MED_AVC_NOISE_STEPS_NUM);

            for (swCnt = 0; swCnt < MED_AVC_NOISE_STEPS_NUM; swCnt++)
            {
                pstAvc->aswMedAvcStep[swCnt]    = CODEC_OpL_mult0(pshwParam[5 + swCnt],
                                                                pshwParam[5 + swCnt]);  /* �����ּ���NV�� */
            }

            /* ��ȡ֡����Ϣ */
            pstAvc->shwFrmLen                   = MED_PP_GetFrameLength();

            /* ����֡����ȷ����Ա������ֵ */
            /* խ�� */
            if(MED_PP_MIC_NUM_1 == enPpMicNum)
            {
                if (CODEC_FRAME_LENGTH_NB == pstAvc->shwFrmLen)
                {
                    pstAvc->pshwWeightTab           = &g_ashwMedAvcWeightTab8k[0];          /* ������Ȩ�� */
                    pstAvc->enFreqLen               = MED_AVC_FFT_LEN_8K;                   /* FFT�ĳ��� */
                }
                /* ��� */
                else
                {
                    pstAvc->pshwWeightTab           = &g_ashwMedAvcWeightTab16k[0];         /* ������Ȩ�� */
                    pstAvc->enFreqLen               = MED_AVC_FFT_LEN_16K;                  /* FFT�ĳ��� */
                }
            }
            else
            {
                if (CODEC_FRAME_LENGTH_NB == pstAvc->shwFrmLen)
                {
                    pstAvc->pshwWeightTab           = &g_ashwMedAvcWeightTab8k[0];          /* ������Ȩ�� */
                    pstAvc->enFreqLen               = MED_AVC_FFT_LEN_2MIC_8K;                   /* FFT�ĳ��� */
                }
                /* ��� */
                else
                {
                    pstAvc->pshwWeightTab           = &g_ashwMedAvcWeightTab16k[0];         /* ������Ȩ�� */
                    pstAvc->enFreqLen               = MED_AVC_FFT_LEN_2MIC_16K;                  /* FFT�ĳ��� */
                }
            }



            CODEC_OpVecSet((VOS_INT16*)(&(pstAvc->aenVadFlag[0])),
                         MED_AVC_BUF_LEN,
                         CODEC_VAD_FLAG_SPEECH);                                     /* ��ʼ��ʱ����ʷ��ϢȫΪ���� */
            UCOM_MemSet(pstAvc->aswFrmLvl, 0, MED_AVC_BUF_LEN * sizeof(VOS_INT32));     /* ��ʼ��ʱ����ʷ��ϢȫΪ0 */
            pstAvc->shwLastGain                 = MED_AVC_GAIN_INITIAL;                 /* ��ʼ��Gain��ֵ */
            pstAvc->shwLastStep                 = MED_AVC_NOISE_STEPS_NUM;              /* ���������� */
            pstAvc->swNoiseLvl                  = MED_AVC_NOISE_LVL_INITIAL;            /* ���Ƶ��������� */
            pstAvc->enVadFlag                   = CODEC_VAD_FLAG_NOISE;
        }
    }

    return uwRet;

}
VOS_VOID MED_AVC_UpdateTx(
                MED_AVC_OBJ_STRU       *pstAvc,
                VOS_INT16              *pshwTxFreq,
                VOS_INT16               enVadFlag)
{

    VOS_INT16       swCnt               = 0;                                    /* ������ */
    VOS_INT32       swTmp;                                                      /* ��ʱ���� */
    VOS_INT32       swFrmLvl            = 0;                                    /* ֡������Q0 */

    /* ����VAD��־ */
    pstAvc->enVadFlag                   = enVadFlag;

    /* ���㵱ǰ֡������ˮƽ */
    for (swCnt = 1; swCnt < (pstAvc->enFreqLen >> 1); swCnt++)
    {
        swTmp       = CODEC_OpL_add(CODEC_OpL_mult0(pshwTxFreq[2 * swCnt],
                                                pshwTxFreq[2 * swCnt]),
                                  CODEC_OpL_mult0(pshwTxFreq[(2 * swCnt) + 1],
                                                pshwTxFreq[(2 * swCnt) + 1]));  /* ����֡������������ֱ�� */
        swTmp       = CODEC_OpL_mpy_32_16(swTmp, pstAvc->pshwWeightTab[swCnt - 1]);/* pshwWeightTab: Q13�����: Q-2 */
        swFrmLvl    = CODEC_OpL_add(swFrmLvl,
                                  CODEC_OpL_shl(swTmp, MED_PP_SHIFT_BY_2));      /* ֡������ˮƽ��Q0 */
    }
    swFrmLvl        = CODEC_OpL_shl(swFrmLvl, 1);                                 /* �Գƣ�������Ҫ*2 */

    /* ����ǰ֡������ˮƽ�浽AVC�ṹ���� */
    pstAvc->swTxFrmLvl = swFrmLvl;
}


VOS_VOID MED_AVC_EstBgNoise(MED_AVC_OBJ_STRU *pstAvc)
{
    VOS_INT16           shwIndex;                                               /* �����������е��±� */
    MED_AVC_NV_STRU    *pstNv          = &(pstAvc->stNv);                       /* NV��ָ�� */


    /* ��˷�������ƫ�� */
    pstAvc->swTxFrmLvl = CODEC_OpL_shl( CODEC_OpL_mpy_32_16((pstAvc->swTxFrmLvl),
                                                         pstNv->shwSensitivityOffset),
                                      (MED_PP_SHIFT_BY_3));

    /* ���浱ǰTx֡���� */
    UCOM_MemCpy(&(pstAvc->aswFrmLvl[0]),
                &(pstAvc->aswFrmLvl[1]),
                (MED_AVC_BUF_LEN - 1) * sizeof(VOS_INT32));
    pstAvc->aswFrmLvl[MED_AVC_BUF_LEN - 1] = pstAvc->swTxFrmLvl;

    /* ���浱ǰTx֡��VAD��־ */
    UCOM_MemCpy(&(pstAvc->aenVadFlag[0]),
                &(pstAvc->aenVadFlag[1]),
                (MED_AVC_BUF_LEN - 1) * sizeof(CODEC_VAD_FLAG_ENUM_INT16));
    pstAvc->aenVadFlag[MED_AVC_BUF_LEN - 1] = pstAvc->enVadFlag;

    if (CODEC_OpVecSum((VOS_INT16*)(pstAvc->aenVadFlag), MED_AVC_BUF_LEN) < MED_AVC_NOISE_UPDATE_MAX_VOICE_ACTIVE)
    {
        /* ������������ */
        shwIndex           = CODEC_OpShr(MED_AVC_BUF_LEN,1);
        pstAvc->swNoiseLvl = CODEC_OpL_add (CODEC_OpL_mpy_32_16(pstAvc->swNoiseLvl,
                                                            MED_AVC_NOISE_UPDATE_ALPHA),
                                          CODEC_OpL_mpy_32_16(pstAvc->aswFrmLvl[shwIndex - 1],
                                                           (CODEC_OP_INT16_MAX - MED_AVC_NOISE_UPDATE_ALPHA)));
    }
}


VOS_VOID MED_AVC_GainRxVol(
                MED_AVC_OBJ_STRU       *pstAvc,
                VOS_INT16              *pshwRxFrm,
                VOS_INT16               shwRxDevLinearGain)                     /* Q12 */
{
    MED_AVC_NV_STRU *pstNv              = &(pstAvc->stNv);                      /* NV��ָ�� */
    VOS_INT32       swCnt;                                                      /* ������ */
    VOS_INT16       shwStep;                                                    /* Ŀ�����漶�� */
    VOS_INT16       shwLastStep         = pstAvc->shwLastStep;                  /* ��ʷ���漶�� */
    VOS_INT16       shwLastGain         = pstAvc->shwLastGain;                  /* ��ʷ���� */
    VOS_INT32       swStepUpdateThd;                                            /* Ŀ�����漶��������� */
    VOS_INT16       shwTemp;                                                    /* ��ʱ���� */
    VOS_INT32       swTemp;                                                     /* ��ʱ���� */
    VOS_INT16       shwRxMax;                                                   /* �����ź�֡�еķ������ֵ */
    VOS_INT16       shwRealMax;                                                 /* �����ź�֡�еķ������ֵ������ֵ */
    VOS_INT16       shwGain;                                                    /* AVC������ */
    VOS_INT16       shwPermitGain;                                              /* PERMIT GAIN, Q11 */

    /* ����Ŀ�����漶�� */
    shwStep             = MED_AVC_NOISE_STEPS_NUM - 1;
    for (swCnt = 0; swCnt < MED_AVC_NOISE_STEPS_NUM; swCnt++)
    {
       if (pstAvc->swNoiseLvl > pstAvc->aswMedAvcStep[swCnt])
       {
           shwStep      = (VOS_INT16)swCnt;
           break;
       }
    }

    /* ���������� */
    if ((shwStep - 1) == shwLastStep)
    {
       swStepUpdateThd = CODEC_OpL_mpy_32_16(pstAvc->aswMedAvcStep[shwLastStep],
                                           MED_AVC_GetNoiseStepLag(shwLastStep));
       if (pstAvc->swNoiseLvl  > swStepUpdateThd)
       {
           shwStep      = shwLastStep;
       }
    }
    pstAvc->shwLastStep = shwStep;

    /* ��������,shwGain, 16bit Q11 */
    shwGain     = CODEC_OpAdd(CODEC_OpMult(MED_AVC_GAIN_UPDATE_ALPHA, shwLastGain),
                            CODEC_OpMult(CODEC_OP_INT16_MAX - MED_AVC_GAIN_UPDATE_ALPHA,
                                       MED_AVC_GetGainFromTab(shwStep)));

    /* �������������޷� */
    shwTemp     = CODEC_OpMult_shr(shwLastGain,
                                 pstNv->shwGainMaxInc,
                                 MED_PP_SHIFT_BY_14);                          /* Q11 */
    shwGain     = CODEC_OpMin(shwGain, shwTemp);

    /* �����������С�޷� */
    shwTemp     = CODEC_OpMult_shr(shwLastGain,
                                 pstNv->shwGainMaxDec,
                                 MED_PP_SHIFT_BY_14);                          /* Q11 */
    shwGain     = CODEC_OpMax(shwGain, shwTemp);

    /* ȡRx���е����ֵ */
    shwRxMax    = CODEC_OpVecMax(pshwRxFrm, (VOS_INT32)(pstAvc->shwFrmLen), 0);

    /* ������ƥ������С��1ʱ,����������ƥ�������Ч�� */
    shwRxDevLinearGain= CODEC_OpMax(MED_AVC_RX_DEV_GAIN_MIN, shwRxDevLinearGain);

    /* ���濹���� */
    shwRealMax  =  CODEC_OpMult_shr(CODEC_OpMult_shr(shwRxMax, shwGain, MED_PP_SHIFT_BY_11),
                                  shwRxDevLinearGain,
                                  MED_PP_SHIFT_BY_12);
    if  (shwRealMax > pstNv->shwAntiSatMaxLevel)
    {
        swTemp  = CODEC_OpL_mult0(shwRxMax, shwRxDevLinearGain);                  /* swTemp: Q12 */
        shwPermitGain = CODEC_OpSaturate((CODEC_OpNormDiv_32((VOS_INT32)(pstNv->shwAntiSatMaxLevel),
                                                     swTemp,
                                                     MED_PP_SHIFT_BY_11
                                                     + MED_PP_SHIFT_BY_12)));   /* shwPermitGain: 16bit Q11 */
        shwGain = CODEC_OpAdd(CODEC_OpMult(MED_AVC_ANTI_SAT_ALPHA, shwGain) ,
                            CODEC_OpMult((CODEC_OP_INT16_MAX - MED_AVC_ANTI_SAT_ALPHA),
                                       shwPermitGain));
    }

    /* AVC ����˥��ԭ�е����� */
    shwGain     = CODEC_OpMax(MED_AVC_GAIN_MIN, shwGain);                         /* 16bit Q11 */
    pstAvc->shwLastGain = shwGain;

    /* ʩ������ */
    CODEC_OpVcMultScale(pshwRxFrm,
                      (signed)MED_PP_GetFrameLength(),
                      shwGain,
                      MED_PP_SHIFT_BY_12,
                      pshwRxFrm);
}


VOS_UINT32 MED_AVC_Main(
                VOS_VOID                *pstMedAvcObj,
                VOS_INT16               *pshwRxFrm,
                VOS_INT16                shwRxDevGain)
{
    MED_AVC_NV_STRU     *pstNv;                                                 /* NV��ָ�� */
    VOS_INT16            shwRxDevLinearGain;                                    /* �������棬Q12 */
    MED_AVC_OBJ_STRU    *pstMedAvc;

    pstMedAvc = (MED_AVC_OBJ_STRU*)pstMedAvcObj;
    pstNv     = &(pstMedAvc->stNv);

    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        /* AVC��ʹ�� */
        return UCOM_RET_SUCC;
    }

    /* ����ˮƽ���� */
    MED_AVC_EstBgNoise(pstMedAvc);

    /* ������� */
    shwRxDevLinearGain = CODEC_OpShr_r(MED_AVC_Db2Linear(shwRxDevGain), 1);       /* MED_AVC_Db2Linear�����ΪQ13�������Ƶ�Q12 */
    MED_AVC_GainRxVol(pstMedAvc, pshwRxFrm, shwRxDevLinearGain);

    return UCOM_RET_SUCC;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

