

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "med_eanr.h"
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
MED_EANR_STRU                            g_astMedEanrObj[MED_EANR_MAX_OBJ_NUM];     /* ANRģ��ȫ�ֿ���ʵ�� */
MED_OBJ_INFO                             g_stMedEanrObjInfo;                      /* ANR��Ϣ */

/*�������Сֵ���ұ�*/
MED_INT16 PriSNR_Min_Tab[51] =
{
    128, 102, 81, 64, 51, 40, 32, 26, 20, 16,
    13 , 10 , 8 , 6 , 5 , 4 , 3 , 3 , 2 , 2 ,
    1  , 1  , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
    1  , 1  , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
    1  , 1  , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
    1
};

/*������Сֵ���ұ�*/
MED_INT16 PriSNRSqrt_Min_Tab[51] =
{
    32767, 29205, 26029, 23198, 20675, 18427, 16423, 14637, 13045, 11627,
    10362, 9235 , 8231 , 7336 , 6538 , 5827 , 5193 , 4629 , 4125 , 3677 ,
    3277 , 2920 , 2603 , 2320 , 2068 , 1843 , 1642 , 1464 , 1305 , 1163 ,
    1036 , 924  , 823  , 734  , 654  , 583  , 519  , 463  , 413  , 368  ,
    328  , 292  , 260  , 232  , 207  , 184  , 164  , 146  , 130  , 116  ,
    104
};

/*���������Globalƽ����ϵ��*/
MED_INT16 MED_EANR_SNR_WIN_GLB[31] =
{
    20  , 78  , 173 , 300 , 455 , 632 , 824 , 1024, 1224, 1416,
    1593, 1748, 1875, 1970, 2028, 2048, 2028, 1970, 1875, 1748,
    1593, 1416, 1224, 1024, 824 , 632 , 455 , 300 , 173 , 78  ,
    20

};

/*�Ӵ���������*/
MED_INT16 MED_EANR_SUBBAND_FL[40] =
{
    0  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,20 ,24 ,
    29 ,34 ,40 ,47 ,54 ,60 ,67 ,75 ,84 ,93 ,102,115,
    128,141,153,168,183,200,219,237
};

/*�Ӵ���������*/
MED_INT16 MED_EANR_SUBBAND_FH[40] =
{
    1  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,19 ,23 ,28 ,
    33 ,39 ,46 ,53 ,59 ,66 ,74 ,83 ,92 ,101,114,127,
    140,152,167,182,199,218,236,255
};


/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_VOID* MED_EANR_1MIC_Create( VOS_VOID )
{
    /* ��������˽���ʵ��*/
    return (MED_PP_Create(
                    MED_EANR_GetObjInfoPtr(),
                    MED_EANR_GetObjPtr(),
                    MED_EANR_MAX_OBJ_NUM,
                    sizeof(MED_EANR_STRU)));
}
VOS_UINT32 MED_EANR_1MIC_SetPara(
                VOS_VOID                          *pInstance,
                VOS_INT16                         *pshwPara,
                VOS_INT16                          shwParaLen,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32  enSampleRate)
{
    VOS_UINT32           uwRet;
    MED_INT16            shwTmp;
    MED_INT16            shwI;

    MED_EANR_STRU       *pstObj = (MED_EANR_STRU *)pInstance;                    /* ��ʱANRָ�� */;

    /* �ж�����Ƿ�Ϸ� */
    uwRet         = MED_PP_CheckPtrValid(MED_EANR_GetObjInfoPtr(), pstObj);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (   (shwParaLen  < CODEC_NV_EANR_1MIC_LEN)
            || (pshwPara[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ANRʵ��״̬���� */
            UCOM_MemSet(pstObj, 0, sizeof(MED_EANR_STRU));

            pstObj->enIsUsed = CODEC_OBJ_USED;

            pstObj->shwEnable           = pshwPara[0];                          /*EANRʹ�ܿ��ƣ�0Ϊ�رգ�1Ϊ��*/
            pstObj->shwReduceDB         = pshwPara[1];                          /*��ΧΪ-50~0*/
            pstObj->shwZeroFlagTh       = pshwPara[2];                          /*ȫ0֡�о�����*/

            shwTmp                      = CODEC_OpNegate(pstObj->shwReduceDB);

            switch (enSampleRate)
            {
                case CODEC_SAMPLE_RATE_MODE_8000:
                {
                    pstObj->stCommon.shwSubBandNum   = 32;
                    pstObj->stCommon.shwOpertBandNum = 33;                               /*խ�������Ӵ���*/

                    break;
                }

                case CODEC_SAMPLE_RATE_MODE_16000:
                {
                    pstObj->stCommon.shwSubBandNum   = 40;
                    pstObj->stCommon.shwOpertBandNum = 41;                               /*��������Ӵ���*/

                    break;
                }
                default:
                {
                    return UCOM_RET_ERR_PARA;
                }
            }

            pstObj->stCommon.swIdx                   = 0;
            pstObj->stCommon.shwZfNum                = 1;
            pstObj->stCommon.shwIdxSwitch            = 0;

            pstObj->stSpeechProb.shwSNRMin           = PriSNR_Min_Tab[shwTmp];

            pstObj->stAbsenceProb.shwMeanPriSNR      = 0;
            pstObj->stAbsenceProb.shwPriSNR          = 0;

            for(shwI=0; shwI<MED_EANR_MAXBINNUM; shwI++)
            {
                pstObj->stGain.ashwPriSNRMid[shwI]   = 128;
            }

            pstObj->stGain.shwSNRSqrtMin             = PriSNRSqrt_Min_Tab[shwTmp];
        }
    }

    return uwRet;

}



VOS_UINT32  MED_EANR_1MIC_GetPara(
                VOS_VOID               *pInstance,
                VOS_INT16              *pshwPara,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32                uwRet;
    MED_EANR_STRU            *pstObj = (MED_EANR_STRU *)pInstance;

    /* �ж�����Ƿ�Ϸ� */
    uwRet         = MED_PP_CheckPtrValid(MED_EANR_GetObjInfoPtr(), pstObj);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (shwParaLen < MED_EANR_1MIC_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡNV��pshwPara */
            pshwPara[0] = pstObj->shwEnable;
            pshwPara[1] = pstObj->shwReduceDB;
            pshwPara[2] = pstObj->shwZeroFlagTh;
        }
    }

    return uwRet;

}


VOS_UINT32  MED_EANR_1MIC_Destroy(VOS_VOID  **ppInstance)
{
    VOS_UINT32              uwRet;
    MED_EANR_STRU          *pstObj;
    pstObj               = (MED_EANR_STRU *)(*ppInstance);

    /* �ж�����Ƿ�Ϸ� */
    uwRet                = MED_PP_CheckPtrValid(MED_EANR_GetObjInfoPtr(), pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
       UCOM_MemSet(pstObj, 0, sizeof(MED_EANR_STRU));
       pstObj->enIsUsed  = CODEC_OBJ_UNUSED;                                      /* ʵ������־��Ϊfalse */
       *ppInstance       = MED_NULL;
    }

    return uwRet;

}

/*************************************************************************************************
��������: MED_EANR_FastSmooth_32
��������: 32bitƽ���˲�����
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_FastSmooth_32(MED_INT32 *pswIn, MED_INT32 *pswOut, MED_INT16 shwLen)
{
    MED_INT16 shwI ;

    for (shwI = 1; shwI < (shwLen - 1); shwI++)
    {
        pswOut[shwI] = CODEC_OpL_add(CODEC_OpL_add(pswIn[shwI - 1], pswIn[shwI + 1]),
                                   (pswIn[shwI] << 1));

        pswOut[shwI] = CODEC_OpL_shr_r(pswOut[shwI], 2);
    }

    pswOut[0]   = CODEC_OpL_add((pswIn[0] << 1), pswIn[1]);
    pswOut[0]   = CODEC_OpL_shr_r(pswOut[0], 2);

    pswOut[shwLen - 1] = CODEC_OpL_add((pswIn[shwLen - 1] << 1), pswIn[shwLen - 2]);
    pswOut[shwLen - 1] = CODEC_OpL_shr_r(pswOut[shwLen - 1], 2);
}

/*************************************************************************************************
��������: MED_EANR_FastSmooth_16
��������: 16bitƽ���˲�����
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_FastSmooth_16(MED_INT16 *pshwIn, MED_INT16 *pshwOut, MED_INT16 shwLen)
{
    MED_INT16 shwI; 

    for (shwI = 1; shwI < (shwLen-1); shwI++)
    {
        pshwOut[shwI] = CODEC_OpAdd(CODEC_OpAdd(pshwIn[shwI - 1] >> 2, pshwIn[shwI + 1] >> 2),
                                  (pshwIn[shwI] >> 1));
    }

    pshwOut[0]          = CODEC_OpAdd(pshwIn[0] >> 1, pshwIn[1] >> 2);
    pshwOut[shwLen - 1] = CODEC_OpAdd(pshwIn[shwLen - 1] >> 1, pshwIn[shwLen - 2] >> 2);

}

/*************************************************************************************************
��������: MED_EANR_FastSmooth_Shl2
��������: 16bitƽ���˲���,����Ŵ�2bit��
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_FastSmooth_Shl2(MED_INT16 *pshwIn, MED_INT16 *pshwOut, MED_INT16 shwLen)
{
    MED_INT16 shwI ;

    for(shwI = 1; shwI < (shwLen - 1); shwI++)
    {
        pshwOut[shwI]   = CODEC_OpAdd(CODEC_OpAdd(pshwIn[shwI - 1], pshwIn[shwI + 1]),
                                   (MED_INT16)(pshwIn[shwI] << 1));
    }

    pshwOut[0]          = CODEC_OpAdd((MED_INT16)(pshwIn[0] << 1), pshwIn[1]);
    pshwOut[shwLen - 1] = CODEC_OpAdd((MED_INT16)(pshwIn[shwLen - 1] << 1), pshwIn[shwLen - 2]);

}

/*************************************************************************************************
��������: MED_EANR_ConvSmooth
��������: 16bit���ƽ���˲�����
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_ConvSmooth(MED_INT16 *ashwWin,
                         MED_INT16 *pshwIn,
                         MED_INT16 *pshwOut,
                         MED_INT16 shwBinNum)
{
    MED_INT16 ashwtemp[100];
    MED_INT16 shwTmp;
    MED_INT16 shwI;
    MED_INT16 shwJ;

    CODEC_OpVecSet(ashwtemp, shwBinNum + 31, 0);
    CODEC_OpVecCpy(ashwtemp+15, pshwIn, shwBinNum);

    for (shwI = 0; shwI < shwBinNum; shwI++)
    {
        shwTmp     = 0;

        for ( shwJ = 0; shwJ < 31; shwJ++)
        {
            shwTmp = CODEC_OpAdd(shwTmp, CODEC_OpMult(ashwtemp[shwI + shwJ], ashwWin[shwJ]));
        }
        pshwOut[shwI]  = shwTmp;
    }

}


VOS_UINT32  MED_EANR_1MIC_Main(
                VOS_VOID                    *pInstance,
                VOS_INT16                   *pshwFrmFreq,
                VOS_INT16                   *pshwFrmFreqRef,
                VOS_INT16                    shwNormShift)
{
    VOS_INT16              shwI;

    MED_EANR_STRU         *pstAnrObj = (MED_EANR_STRU *)pInstance;

    /* ʹ���ж� */
    if (pstAnrObj->shwEnable == 0)
    {
        return   UCOM_RET_SUCC;
    }

    pstAnrObj->stCommon.shwNormShift = shwNormShift;

    pstAnrObj->stCommon.swIdx++;

    /*ʱƵ��ƽ��*/
    MED_EANR_TFSmooth_FIX(&pstAnrObj->stCommon, &pstAnrObj->stTFSmooth, pshwFrmFreqRef);

    /*����S��Сֵ*/
    MED_EANR_MinTrack_FIX(&pstAnrObj->stCommon, pstAnrObj->stTFSmooth.aswPowSpctSmth);

    /*�����ź������Ƚ������������ĳ�ʼ�ж�*/
    MED_EANR_SpeechDec_FIX(&pstAnrObj->stCommon,
                           &pstAnrObj->stSpeechDec,
                           pstAnrObj->stTFSmooth.aswPowSpctSmth,
                           pstAnrObj->stTFSmooth.aswPowSpct);

    /*�����жϽ�����źŽ���ʱ��Ƶ��ƽ������������Сֵ����*/
    MED_EANR_CondiSmooth_FIX(&pstAnrObj->stCommon,
                             pstAnrObj->stTFSmooth.aswPowSpctSmth,
                             pstAnrObj->stTFSmooth.aswPowSpct,
                             pstAnrObj->stSpeechDec.ashwSpechIdx);


    /*�����жϽ�������������ڸ���*/
    MED_EANR_SpeechProb_FIX(&pstAnrObj->stCommon,
                            &pstAnrObj->stSpeechProb,
                            pstAnrObj->stGain.ashwPriSNRMid,
                            pstAnrObj->stTFSmooth.aswPowSpctSmth,
                            pstAnrObj->stTFSmooth.aswPowSpct);


    /*��һ��ʱ�䴰�ڲ�����Сֵ,ÿMED_EANR_UPDATELEN֡����һ�Σ�
      �洢�����й�����shwBINNUM*MED_EANR_DATAWINNUM������*/
    pstAnrObj->stCommon.shwIdxSwitch++;
    MED_EANR_MinTrackUpdate_FIX(&pstAnrObj->stCommon,
                                &pstAnrObj ->stMinTrackUpdate,
                                pstAnrObj->stTFSmooth.aswPowSpctSmth);

    if (MED_EANR_UPDATELEN == pstAnrObj->stCommon.shwIdxSwitch)
    {
        pstAnrObj->stCommon.shwIdxSwitch = 0;
    }

    /*���������׼���*/
    MED_EANR_NoiseSpect_FIX(&pstAnrObj->stCommon,
                            &pstAnrObj->stNoiseSpect,
                             pstAnrObj->stSpeechProb.ashwSpchProb,
                             pstAnrObj->stTFSmooth.aswPowSpct);

    /*��������������*/

    /*��������ȳ�ʱƽ����Ƶ���ڲ�ͬ������Χ�ڵ�ƽ��*/
    for (shwI = 0; shwI < pstAnrObj->stCommon.shwOpertBandNum; shwI++)
    {
        pstAnrObj->stSpeechProb.ashwPriSNR[shwI] = CODEC_OpMin(MED_EANR_PRISNRMAXLIMIT,
                                                          pstAnrObj->stSpeechProb.ashwPriSNR[shwI]);
    }

    MED_EANR_SNRSmooth_FIX(&pstAnrObj->stCommon,
                           &pstAnrObj->stSnrSmooth,
                           pstAnrObj->stSpeechProb.ashwPriSNR);


    /*�������ڸ��ʼ���*/
    MED_EANR_PresenceProb_FIX(&pstAnrObj->stCommon,
                              &pstAnrObj->stPresenceProb,
                              pstAnrObj->stNoiseSpect.aswNoisePowLong,
                              pstAnrObj->stSnrSmooth.ashwSNRSmthLcl,
                              pstAnrObj->stSnrSmooth.ashwSNRSmthGlb);


    /*���������ڸ���*/
    MED_EANR_AbsenceProb_FIX(&pstAnrObj->stCommon,
                             &pstAnrObj->stAbsenceProb,
                             pstAnrObj->stPresenceProb.ashwProbLcl,
                             pstAnrObj->stPresenceProb.ashwProbGlb);

    /*�������*/
    MED_EANR_Gain_FIX(&pstAnrObj->stCommon,
                      &pstAnrObj->stGain,
                      pstAnrObj->stSpeechProb.shwSNRMin,
                      pstAnrObj->stTFSmooth.aswPowSpct,
                      pstAnrObj->stAbsenceProb.ashwAbsenProb);

    for (shwI = 0; shwI < pstAnrObj->stCommon.shwOpertBandNum; shwI++)
    {
        pstAnrObj->stGain.ashwPriSNRMid[shwI] = CODEC_OpMin(MED_EANR_PRISNRMAXLIMIT,
                                                       pstAnrObj->stGain.ashwPriSNRMid[shwI]);
    }

    /*����ʩ��*/
    MED_EANR_DATA_FIX(&pstAnrObj->stCommon, pstAnrObj->stGain.ashwGain, pshwFrmFreq);

    return UCOM_RET_SUCC;

}

/*************************************************************************************************
��������: MED_EANR_TFSmooth_FIX
��������: �����źŵĹ����ף�����ʱƵ���϶Թ����׽���ʱ��֡���Ƶ����ƽ����
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_TFSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                           MED_EANR_TFSMOOTH_STRU *stTFSmooth,
                           MED_INT16 *shwDataBuf)
{
    MED_INT16   shwI;
    MED_INT16   shwJ;
    MED_INT16   shwTmpSub;
    MED_INT16   shwTmpNorm;                                                     /*�����׹�һ�������м����*/
    MED_INT16   shwBinHi;                                                       /*�Ӵ�����Ƶ������*/
    MED_INT16   shwBinLow;                                                      /*�Ӵ�����Ƶ������*/
    MED_INT32   swTmpPowS;                                                      /*ʱƵƽ�������׼����м����*/
    MED_INT32   aswPowSpec[MED_EANR_MAXBINNUM];                                 /*Ƶ���ƽ����Ĺ�����*/
    MED_INT32   aswTmp[MED_EANR_MAXBINNUM];
    MED_INT32   swCntI;

    shwTmpNorm   = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 5);                   /*�Թ�������ʱ��֡��ƽ����������λ����ȥ��NormShift��Ŵ�5bit*/

    /*�����׼��㣬��β����Ƶ���������ⵥ������*/
    aswTmp[0]    = CODEC_OpL_mult0(shwDataBuf[0], shwDataBuf[0] );
    aswTmp[0]    = CODEC_OpL_add(aswTmp[0], CODEC_OpL_mult0(shwDataBuf[2], shwDataBuf[2]));
    aswTmp[0]    = CODEC_OpL_add(aswTmp[0], CODEC_OpL_mult0(shwDataBuf[3], shwDataBuf[3]));

    stTFSmooth->aswPowSpct[0]    = aswTmp[0] >> 1;

    stTFSmooth->aswPowSpct[stCommon->shwSubBandNum]
                                 = CODEC_OpL_mult0(shwDataBuf[1], shwDataBuf[1]);

    /*�����׼��㣬�м�Ƶ��ļ���*/
    for (shwI = 1; shwI < stCommon->shwSubBandNum; shwI++)
    {
        aswTmp[shwI]     = 0;

        shwBinHi         = MED_EANR_SUBBAND_FH[shwI];
        shwBinLow        = MED_EANR_SUBBAND_FL[shwI];

        for (shwJ = shwBinLow; shwJ <= shwBinHi; shwJ++)
        {
            aswTmp[shwI] = CODEC_OpL_add(aswTmp[shwI],
                                       CODEC_OpL_mult0(shwDataBuf[shwJ * 2], shwDataBuf[shwJ * 2]));

            aswTmp[shwI] = CODEC_OpL_add(aswTmp[shwI],
                                       CODEC_OpL_mult0(shwDataBuf[(shwJ * 2) + 1], shwDataBuf[(shwJ * 2) + 1]));
        }

        stTFSmooth->aswPowSpct[shwI]
                         = aswTmp[shwI] / CODEC_OpSub(CODEC_OpAdd(shwBinHi, 1), shwBinLow);
    }

    /*Ƶ����ƽ��*/
    MED_EANR_FastSmooth_32(stTFSmooth->aswPowSpct, aswPowSpec, stCommon->shwOpertBandNum);

    /*ʱ��֡��ƽ��*/
    /*NormShift��һ����,aswPowSpec(Q5)*/
    CODEC_OpVecShr_r32(aswPowSpec, stCommon->shwOpertBandNum, shwTmpNorm,aswPowSpec);

    if (stCommon->swIdx == stCommon->shwZfNum)
    {
        /*��һ����ȫ��֮֡ǰֱ�ӽ���ǰ�����׸�ֵ����Ӧ�����ױ���������������*/
        CODEC_OpVecShr_r32(aswPowSpec,
                         stCommon->shwOpertBandNum,
                         0,
                         stCommon->aswPowSpctCond);

        CODEC_OpVecShr_r32(aswPowSpec,
                         stCommon->shwOpertBandNum,
                         0,
                         stTFSmooth->aswPowSpctSmth);

        CODEC_OpVecShr_r32(stTFSmooth->aswPowSpct,
                         stCommon->shwOpertBandNum,
                         (MED_INT16)(stCommon->shwNormShift << 1),
                         stCommon->aswPowSpctSmth);

        shwTmpSub = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 6);

        for (swCntI = 0; swCntI < stCommon->shwOpertBandNum; swCntI++)
        {
            stCommon->aswNoisePow[swCntI]    = L_shr_r(stTFSmooth->aswPowSpct[swCntI], shwTmpSub);
            stCommon->aswNoisePowRud[swCntI] = L_shr_r(stTFSmooth->aswPowSpct[swCntI], shwTmpSub);
        }

    }
    else
    {
        /*��Ƶ����ʱ��֡��ƽ��,stTFSmooth->aswPowSpctSmth(Q5)*/
        for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
        {
            swTmpPowS = CODEC_OpL_sub(stTFSmooth->aswPowSpctSmth[shwI],aswPowSpec[shwI] );

            swTmpPowS = CODEC_OpL_mpy_32_16_r(swTmpPowS,MED_EANR_ALPHA_COEF);

            stTFSmooth->aswPowSpctSmth[shwI]
                      = CODEC_OpL_add(swTmpPowS, aswPowSpec[shwI] );
        }
    }
}

/*************************************************************************************************
��������: MED_EANR_MinTrack_FIX
��������: ����ʱƵ��ƽ������źŹ����׵�����Ƶ���ϵ���Сֵ��
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_MinTrack_FIX(MED_EANR_COMM_STRU *stCommon, MED_INT32 *aswPowSpctSmth)
{
    MED_INT16 shwI;

    if (stCommon->swIdx < (MED_EANR_SMTHSTARTFRM + stCommon->shwZfNum))
    {
        /* �����13����ȫ��֡��������Сֵ���� */
        UCOM_MemCpy(stCommon->aswPowSpctMin ,
                    aswPowSpctSmth,
                    (VOS_UINT16)(stCommon->shwOpertBandNum) * sizeof(MED_INT32));

        UCOM_MemCpy(stCommon->aswPowSpctMinCurt,
                    aswPowSpctSmth,
                    (VOS_UINT16)(stCommon->shwOpertBandNum) * sizeof(MED_INT32));
    }
    else
    {
        for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
        {
            stCommon->aswPowSpctMin[shwI]     = CODEC_OpL_min(aswPowSpctSmth[shwI],
                                                            stCommon->aswPowSpctMin[shwI]);

            stCommon->aswPowSpctMinCurt[shwI] = CODEC_OpL_min(aswPowSpctSmth[shwI],
                                                            stCommon->aswPowSpctMinCurt[shwI] );
        }
    }
}

/*************************************************************************************************
��������: MED_EANR_SpeechDec_FIX
��������: ������������������о���
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_SpeechDec_FIX(MED_EANR_COMM_STRU *stCommon,
                            MED_EANR_SPEECHDEC_STRU *stSpeechDec,
                            MED_INT32*aswPowSpctSmth,
                            MED_INT32 *aswPowSpct)
{
    MED_INT16 shwI;
    MED_INT32 swPowTh;
    MED_INT32 swPowSmthTh;
    MED_INT32 aswPowSpctTh;
    MED_INT32 aswPowSpctSmthTh;

    MED_INT16 shwTmpNorm;

    shwTmpNorm      = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift<<1), 5);   /*NormShift������λbit��*/

    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        stSpeechDec->ashwSpechIdx[shwI]     = 0;                                /*����ָʾ��0��ʾ������1��ʾ����,Ĭ������*/

        swPowTh     = CODEC_OpL_mpy_32_16_r(stCommon->aswPowSpctMin[shwI] << 3,
                                          MED_EANR_SPEECHDEC_THRESHOLDA);       /*���޼���*/
        swPowSmthTh = CODEC_OpL_mpy_32_16_r(stCommon->aswPowSpctMin[shwI],
                                          MED_EANR_SPEECHDEC_THRESHOLDB);       /*���޼���*/

        aswPowSpctTh     = CODEC_OpL_shl(swPowTh, shwTmpNorm);
        aswPowSpctSmthTh = CODEC_OpL_shl(swPowSmthTh, 2);

        if( (aswPowSpct[shwI] < aswPowSpctTh)
         && (aswPowSpctSmth[shwI] < aswPowSpctSmthTh))
        {
            stSpeechDec->ashwSpechIdx[shwI] = 1;                                /*�����о�*/
        }
    }
}

/*************************************************************************************************
��������: MED_EANR_CondiSmooth_FIX
��������: �����о�ָʾ�����µ�ʱƵ��ƽ������Сֵ������
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/

void MED_EANR_CondiSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                              MED_INT32 *aswPowSpctSmth,
                              MED_INT32 *aswPowSpct,
                              MED_INT16 *ashwSpechIdx)
{
    MED_INT16     shwI;
    MED_INT16     shwTmpNorm;
    MED_INT32     swTmp;
    MED_INT32     aswTmp[MED_EANR_MAXBINNUM+2] = {0};
    MED_INT32     aswTmp1[MED_EANR_MAXBINNUM] = {0};
    MED_INT32     aswTmp2[MED_EANR_MAXBINNUM] = {0};
    MED_INT32     swMinPowerThrdTemp = 0;

    shwTmpNorm    = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 5);                  /*NormShift������λbit��*/

    /*������ָʾ����Ƶ����ƽ������*/
    MED_EANR_FastSmooth_Shl2(ashwSpechIdx,
                             stCommon->ashwPresenIndSmth,
                             stCommon->shwOpertBandNum);

    /*Ϊ����forѭ�������������źŹ��������˸���һ��0*/
    aswTmp[0]                             = 0;
    aswTmp[stCommon->shwOpertBandNum + 1] = 0;

    for (shwI = 0; shwI< stCommon->shwOpertBandNum;shwI++)
    {
        aswTmp[shwI+1]                    = 0;

        if(1 == ashwSpechIdx[shwI])
        {
            aswTmp[shwI + 1]              = aswPowSpct[shwI];
        }
    }

    /*�����о������µ�ʱƵ��Ƶ���ƽ��*/
    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        if (1 == ashwSpechIdx[shwI])
        {
            /*������ʱƵ���ƽ��*/
            aswTmp2[shwI] = CODEC_OpL_shl(aswTmp[shwI + 1], 1);

            aswTmp2[shwI] = CODEC_OpL_add(aswTmp2[shwI], aswTmp[shwI + 2]);

            aswTmp2[shwI] = CODEC_OpNormDiv_32(CODEC_OpL_add(aswTmp2[shwI], aswTmp[shwI]),
                                             stCommon->ashwPresenIndSmth[shwI],
                                             0);

            aswTmp1[shwI] = CODEC_OpL_shr_r(aswTmp2[shwI], shwTmpNorm);
        }
        else
        {
            /*������ʱ��ƽ��*/
            aswTmp1[shwI] =  stCommon->aswPowSpctCond[shwI];
        }
    }

    swMinPowerThrdTemp = CODEC_OpL_shl(MED_EANR_MINPOWERTHD, (shwTmpNorm+2));

    /*ʱ��֡��ƽ��*/
    if (stCommon->swIdx < (MED_EANR_SMTHSTARTFRM + stCommon->shwZfNum))
    {
        /*�����13����ȫ��֡������ƽ������Сֵ����*/
        UCOM_MemCpy(stCommon->aswPowSpctCond ,
                    aswPowSpctSmth ,
                    (MED_UINT16)(stCommon->shwOpertBandNum) * sizeof(MED_INT32));

        for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
        {

            stCommon->aswPowSpctMinC[shwI]     = CODEC_OpL_min(stCommon->aswPowSpctCond[shwI], swMinPowerThrdTemp);
            stCommon->aswPowSpctMinCurtC[shwI] = CODEC_OpL_min(stCommon->aswPowSpctCond[shwI], swMinPowerThrdTemp);
        }
    }
    else
    {
        for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
        {
            /*ʱ��֡��ƽ��*/
            swTmp  = CODEC_OpL_sub(stCommon->aswPowSpctCond[shwI], aswTmp1[shwI]);

            swTmp  = CODEC_OpL_mpy_32_16_r(swTmp, MED_EANR_ALPHA_COEF);

            stCommon->aswPowSpctCond[shwI]     = CODEC_OpL_add(swTmp, aswTmp1[shwI]);

            /*ƽ������Сֵ����*/
            stCommon->aswPowSpctMinC[shwI]     = CODEC_OpL_min(stCommon->aswPowSpctMinC[shwI] ,
                                                             stCommon->aswPowSpctCond[shwI]);

            stCommon->aswPowSpctMinCurtC[shwI] = CODEC_OpL_min(stCommon->aswPowSpctMinCurtC[shwI],
                                                             stCommon->aswPowSpctCond[shwI]);
        }
    }
}

/*************************************************************************************************
��������: MED_EANR_SpeechProb_FIX
��������: ���������,����������Լ��������ʼ��㡣
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_SpeechProb_FIX(MED_EANR_COMM_STRU       *stCommon,
                                        MED_EANR_SPEECHPROB_STRU *stSpeechProb,
                                        MED_INT16                *ashwPriSNRMid,
                                        MED_INT32                *aswPowSpctSmth,
                                        MED_INT32                *aswPowSpct)

{
    MED_INT16   shwFilterCoeff;
    MED_INT16   shwI;
    MED_INT16   shwTmpShift;
    MED_INT32   aswPstSNR[MED_EANR_MAXBINNUM]    = {0};
    MED_INT16   ashwSNRFact[MED_EANR_MAXBINNUM]  = {0};
    MED_INT32   aswPresenInd[MED_EANR_MAXBINNUM] = {0};
    MED_INT32   swThreshold;
    MED_INT16   ashwLackProb;
    MED_INT16   ashwTmp;
    MED_INT32   swTmp;
    MED_INT32   swTmp1;
    MED_INT16   ashwTmp2[MED_EANR_MAXBINNUM]     = {0};

    shwFilterCoeff = CODEC_OpSub(MED_EANR_MAX11, MED_EANR_SPEECHPROB_SNR_SMTH_COEF >> 4);
    shwTmpShift    = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 6);                 /*��������λ�������ƺ���aswPowSpct�Ŵ���ͬ����*/

    /*�������������ashwPriSNR��������������swPstSNR�����¸��ʼ���ı�Ҫ����shwSNRFact*/
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        /*������������aswPstSNR(Q11)*/
        swTmp             = CODEC_OpL_shl(stCommon->aswNoisePow[shwI], shwTmpShift);

        aswPstSNR[shwI]   = CODEC_OpNormDiv_32(aswPowSpct[shwI],
                                             CODEC_OpL_max(1, swTmp),
                                             11);                               /*aswPstSNR(Q11)*/

        /*�������������ashwPriSNR(Q7)*/
        stSpeechProb->ashwPriSNR[shwI]
                          = CODEC_OpMult_r(ashwPriSNRMid[shwI], MED_EANR_SPEECHPROB_SNR_SMTH_COEF);

        if (aswPowSpct[shwI] > CODEC_OpL_shl(stCommon->aswNoisePow[shwI], shwTmpShift))
        {
            /*���aswPstSNR(Q11) > 2048,����ashwPriSNR(Q7)*/
            swTmp         = CODEC_OpL_sub(aswPstSNR[shwI], MED_EANR_Q11);
            swTmp         = CODEC_OpL_min(swTmp, MED_EANR_PSTSNRMAX);

            ashwTmp       = CODEC_OpSaturate(CODEC_OpL_mpy_32_16_r(swTmp, shwFilterCoeff));

            stSpeechProb->ashwPriSNR[shwI] = CODEC_OpAdd(stSpeechProb->ashwPriSNR[shwI],
                                                       ashwTmp);
        }

        stSpeechProb->ashwPriSNR[shwI]     = CODEC_OpMax(stSpeechProb->ashwPriSNR[shwI],
                                                       stSpeechProb->shwSNRMin);/*��shwSNRMin������Сֵ����*/

        /*���¸��ʼ���ı�Ҫ����shwSNRFact(Q11)*/
        swTmp             = ((VOS_INT32)stSpeechProb->ashwPriSNR[shwI]) << 15;
        ashwSNRFact[shwI] = CODEC_OpSaturate(swTmp / CODEC_OpAdd(MED_EANR_Q7, stSpeechProb->ashwPriSNR[shwI]));

        ashwTmp2[shwI] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16_r(aswPstSNR[shwI], ashwSNRFact[shwI]));

        ashwSNRFact[shwI] = CODEC_OpMin(ashwTmp2[shwI], MED_EANR_SNRFACT_MAX);                    /* ���ֵ����15.9424->32650(Q11)*/
    }

/* �˴��������㷨���������벻ͬ����ע�͵�����Ϊ�÷�֧ʼ�ղ��������޷���lint */
/*
    if (2 > MED_EANR_SPEECHPROB_STAT)
    {
        for(shwI = 0; shwI <stCommon->shwOpertBandNum ; shwI++)
        {
            swTmp              = CODEC_OpL_shl(stCommon->aswPowSpctMin[shwI],
                                             CODEC_OpAdd(shwTmpShift, 1));

            swTmp              = CODEC_OpL_mpy_32_16_r(swTmp, MED_EANR_SPEECHDEC_MIN_COMPENS);

            aswPresenInd[shwI] = CODEC_OpNormDiv_32(aswPowSpct[shwI],
                                                  CODEC_OpL_max(1, swTmp),
                                                  12);
        }
    }
    else
    {
*/
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        swTmp              = CODEC_OpL_shl(stCommon->aswPowSpctMinC[shwI],
                                             CODEC_OpAdd(shwTmpShift, 1));

        swTmp              = CODEC_OpL_mpy_32_16_r(swTmp, MED_EANR_SPEECHDEC_MIN_COMPENS);

        aswPresenInd[shwI] = CODEC_OpNormDiv_32(aswPowSpct[shwI],
                                                  CODEC_OpL_max(1,swTmp),
                                                  12);                          /*aswPresenInd(Q13)*/
    }
/*
    }
*/
    /*�������������벻���ڵ��������ʣ��������������ڸ��ʸ�������*/
    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        ashwLackProb                      = MED_EANR_MAX16;
        stSpeechProb->ashwSpchProb[shwI]  = 0;

        swThreshold = CODEC_OpL_mpy_32_16_r(stCommon->aswPowSpctMinC[shwI] << 2,
                                          MED_EANR_SPEECHDEC_THRESHOLDB);

        /*�������ڣ�ashwSpchProb=0����֮ashwSpchProb=1������ȷ�����������о���ʽ�������*/
        if ((aswPresenInd[shwI]      < MED_EANR_SPEECHPROB_THRESHOLDC)
            && (aswPowSpctSmth[shwI] < swThreshold)
            && (aswPresenInd[shwI]   > MED_EANR_Q13))
        {
           ashwLackProb             = CODEC_OpSub(MED_EANR_SPEECHPROB_THRESHOLDC,
                                                (MED_INT16)aswPresenInd[shwI]); /*���������ڸ���*/

           swTmp1  = CODEC_OpL_Exp(ashwSNRFact[shwI], 11, 8);                     /*ashwSNRFact(Q11),���Q8*/

           swTmp1  = CODEC_OpL_mpy_32_16_r(swTmp1, CODEC_OpSub(MED_EANR_Q14, ashwLackProb));

           swTmp   = CODEC_OpL_mult(ashwLackProb,
                                  CODEC_OpAdd(MED_EANR_Q7, stSpeechProb->ashwPriSNR[shwI]));

           swTmp   = CODEC_OpL_add(swTmp1 , swTmp >> 15);

           stSpeechProb->ashwSpchProb[shwI]
                   = CODEC_OpSaturate(CODEC_OpNormDiv_32(swTmp1 , swTmp, 15));  /*�������ڸ���(Q15)*/
        }

        if((aswPresenInd[shwI]   >= MED_EANR_SPEECHPROB_THRESHOLDC)
         ||(aswPowSpctSmth[shwI] >= swThreshold))
        {
            stSpeechProb->ashwSpchProb[shwI] = MED_EANR_MAX16;
        }
    }
}

/*************************************************************************************************
��������: MED_EANR_MinTrackUpdate_FIX
��������: ���³���ΪMED_EANR_DATAWINLEN��ʱ�䴰�ڵĹ��������ݲ���������䴰�ڸ�Ƶ���ϵ���Сֵ��
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/

void MED_EANR_MinTrackUpdate_FIX(MED_EANR_COMM_STRU *stCommon,
                                 MED_EANR_MINTRACKUPDATE_STRU *stMinTrackUpdate,
                                 MED_INT32 *aswPowSpctSmth)
{

    MED_INT16   shwI;
    MED_INT16   shwJ;
    MED_INT32   swTemp1;
    MED_INT32   swTemp2;

    if (stCommon->shwIdxSwitch == MED_EANR_UPDATELEN)
    {
        if(stCommon->swIdx == ((MED_EANR_UPDATELEN - 1) + stCommon->shwZfNum))
        {
            for(shwI = 0; shwI < MED_EANR_DATAWINNUM; shwI++)
            {
                UCOM_MemCpy(stMinTrackUpdate->aswPowStck + (stCommon->shwOpertBandNum * shwI),
                            aswPowSpctSmth,
                            (MED_UINT16)(stCommon->shwOpertBandNum) * sizeof(MED_INT32));

                UCOM_MemCpy(stMinTrackUpdate->aswCondPowStck + (stCommon->shwOpertBandNum * shwI),
                            stCommon->aswPowSpctCond,
                            (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));
            }
        }
        else
        {
            UCOM_MemCpy(stMinTrackUpdate->aswPowStck,
                        stMinTrackUpdate->aswPowStck + stCommon->shwOpertBandNum,
                        (MED_UINT16)stCommon->shwOpertBandNum * (MED_EANR_DATAWINNUM - 1) * sizeof(MED_INT32));

            UCOM_MemCpy(stMinTrackUpdate->aswPowStck + (stCommon->shwOpertBandNum * (MED_EANR_DATAWINNUM - 1)),
                        stCommon->aswPowSpctMinCurt,
                        (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

            UCOM_MemCpy(stMinTrackUpdate->aswCondPowStck,
                        stMinTrackUpdate->aswCondPowStck + stCommon->shwOpertBandNum,
                        (MED_UINT16)stCommon->shwOpertBandNum * (MED_EANR_DATAWINNUM - 1) * sizeof(MED_INT32));

            UCOM_MemCpy(stMinTrackUpdate->aswCondPowStck + (stCommon->shwOpertBandNum * (MED_EANR_DATAWINNUM - 1)),
                        stCommon->aswPowSpctMinCurtC,
                        (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

            UCOM_MemCpy(stCommon->aswPowSpctMinCurt,
                        aswPowSpctSmth,
                        (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

            UCOM_MemCpy(stCommon->aswPowSpctMinCurtC,
                        stCommon->aswPowSpctCond,
                        (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

            for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
            {
                swTemp1  = stMinTrackUpdate->aswPowStck[shwI];

                swTemp2  = stMinTrackUpdate->aswCondPowStck[shwI];

                for(shwJ = stCommon->shwOpertBandNum;
                    shwJ < (MED_EANR_DATAWINNUM * stCommon->shwOpertBandNum);
                    shwJ+=(stCommon->shwOpertBandNum))
                {
                    swTemp1 = CODEC_OpL_min(swTemp1,
                                          stMinTrackUpdate->aswPowStck[shwI + shwJ]);

                    swTemp2 = CODEC_OpL_min(swTemp2,
                                          stMinTrackUpdate->aswCondPowStck[shwI + shwJ]);
                }

                stCommon->aswPowSpctMin[shwI]  = swTemp1;

                stCommon->aswPowSpctMinC[shwI] = swTemp2;
            }
        }
    }
}

/*************************************************************************************************
��������: MED_EANR_NoiseSpect_FIX
��������: ���������׵ļ��㡣
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/
void MED_EANR_NoiseSpect_FIX(MED_EANR_COMM_STRU *stCommon,
                             MED_EANR_NOISESPECT_STRU *stNoiseSpect,
                             MED_INT16 *ashwSpchProb,
                             MED_INT32 *aswPowSpct)
{
    MED_INT16 shwI;
    MED_INT16 shwFilterCoef;
    MED_INT32 swTmp;
    MED_INT16 ashwPresenProb[MED_EANR_MAXBINNUM];
    MED_INT16 ashwPresenProbL[MED_EANR_MAXBINNUM];
    MED_INT32 aswNormPowSpetrm[MED_EANR_MAXBINNUM] = {0};

    /*����������ƽ�� */
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        shwFilterCoef          = CODEC_OpSub(MED_EANR_MAX16, MED_EANR_NOISESPECT_ALPHA_COEF);

        aswNormPowSpetrm[shwI] = CODEC_OpL_shr_r(aswPowSpct[shwI],
                                               CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 6));

        ashwPresenProb[shwI]   = CODEC_OpAdd(MED_EANR_NOISESPECT_ALPHA_COEF,
                                           CODEC_OpMult_r(shwFilterCoef, ashwSpchProb[shwI]));

        if(MED_EANR_NOISESPECT_PROBTHD < ashwPresenProb[shwI])
        {
            /*�����������ʷǳ��ߵĳ�����Ϊ������ǰһ֡��ͬ*/
            stCommon->aswNoisePowRud[shwI] = stCommon->aswNoisePowRud[shwI];
        }
        else
        {
            swTmp                          = CODEC_OpL_sub(stCommon->aswNoisePowRud[shwI],
                                                         aswNormPowSpetrm[shwI]);

            swTmp                          = CODEC_OpL_mpy_32_16_r(swTmp, ashwPresenProb[shwI]);

            stCommon->aswNoisePowRud[shwI] = CODEC_OpL_add(swTmp, aswNormPowSpetrm[shwI]);
        }

        stCommon->aswNoisePow[shwI]        = stCommon->aswNoisePowRud[shwI] << 1;
    }

    /*���������г�ʱƽ��*/
    if(stCommon->swIdx < (MED_EANR_SMTHSTARTFRM + stCommon->shwZfNum))
    {
        /*�����13����ȫ��֡������ƽ������*/
        UCOM_MemCpy(stNoiseSpect->aswNoisePowLong,
                    stCommon->aswNoisePowRud,
                    (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));
    }
    else
    {
        shwFilterCoef  = CODEC_OpSub(MED_EANR_MAX16, MED_EANR_NOISESPECT_ALPHA_COEF_L);

        for(shwI=0; shwI < stCommon->shwOpertBandNum; shwI++)
        {
            ashwPresenProbL[shwI]
                = CODEC_OpAdd(MED_EANR_NOISESPECT_ALPHA_COEF_L,
                            CODEC_OpMult_r(shwFilterCoef, ashwSpchProb[shwI]));

            if(MED_EANR_NOISESPECT_PROBTHD < ashwPresenProbL[shwI])
            {
                /*�����������ʷǳ��ߵĳ�����Ϊ������ǰһ֡��ͬ*/
                stNoiseSpect->aswNoisePowLong[shwI] = stNoiseSpect->aswNoisePowLong[shwI];
            }
            else
            {
                swTmp  = CODEC_OpL_sub(stNoiseSpect->aswNoisePowLong[shwI], aswNormPowSpetrm[shwI]);

                swTmp  = CODEC_OpL_mpy_32_16_r(swTmp, ashwPresenProbL[shwI]);

                stNoiseSpect->aswNoisePowLong[shwI]
                       = CODEC_OpL_add(swTmp, aswNormPowSpetrm[shwI]);
            }
        }
    }

    /*������������������������ �˴��������㷨���������벻ͬ��������Ȼ��������LINT������Ҫע�� */
/*
    if(MED_EANR_SPEECHPROB_STAT <= 2)
    {
*/
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        stCommon->aswNoisePow[shwI] = CODEC_OpL_mpy_32_16_r(stCommon->aswNoisePow[shwI],
                                                            MED_EANR_NOISESPECT_NOISECOMPENS);
    }
/*
    }
*/
}

/*************************************************************************************************
��������: MED_EANR_SNRSmooth_FIX
��������: ���������ƽ�����ȶ�ÿ���Ӵ���ʱ��ƽ�����ٷֱ����Ӵ����Local��Globalƽ��
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/

void MED_EANR_SNRSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                            MED_EANR_SNRSMOOTH_STRU *stSnrSmooth,
                            MED_INT16 *ashwPriSNR)
{
    MED_INT16 ashwTmpSNR[MED_EANR_MAXBINNUM];
    MED_INT16 ashwTmp[MED_EANR_MAXBINNUM];

    CODEC_OpVvSub(stCommon->ashwSNRSmth,
                ashwPriSNR,
                stCommon->shwOpertBandNum,
                ashwTmp);

    /*�������������ʱ��ƽ��ֵ��16bit Q7*/
    CODEC_OpVcMultQ15Add(ashwPriSNR,
                       ashwTmp,
                       stCommon->shwOpertBandNum,
                       MED_EANR_APSSMOOTH_ASNR_COEF,
                       stCommon->ashwSNRSmth);

    /*��������������Ӵ����Localƽ��ֵ��16bit Q7*/
    MED_EANR_FastSmooth_16(stCommon->ashwSNRSmth,
                        stSnrSmooth->ashwSNRSmthLcl,
                        stCommon->shwOpertBandNum);

    CODEC_OpVecCpy(ashwTmpSNR, stCommon->ashwSNRSmth, stCommon->shwOpertBandNum);

    /*��������������Ӵ����Globalƽ��ֵ��16bit Q7*/
    MED_EANR_ConvSmooth(MED_EANR_SNR_WIN_GLB,
                        ashwTmpSNR,
                        stSnrSmooth->ashwSNRSmthGlb,
                        stCommon->shwOpertBandNum);

}

/*************************************************************************************************
��������: MED_EANR_PresenceProb_FIX
��������: ����Local��Global�������ڸ���
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/

void MED_EANR_PresenceProb_FIX(MED_EANR_COMM_STRU *stCommon,
                               MED_EANR_PRESENCEPROB_STRU *stPresenceProb,
                               MED_INT32 *aswNoisePowLong,
                               MED_INT16 *ashwSNRSmthLcl,
                               MED_INT16 *ashwSNRSmthGlb)
{
    MED_INT16 shwI;
    MED_INT16 ashwLclPriSNR[MED_EANR_MAXBINNUM] = {0};
    MED_INT16 ashwGlbPriSNR[MED_EANR_MAXBINNUM] = {0};
    MED_INT16 shwMeanProb;
    MED_INT16 ashwTmp[MED_EANR_MAXBINNUM]       = {0};
    MED_INT32 swTmp;

    /*��Local��Global�����������ƽ������ֵ(Q7)ת���ɶ���ֵ��16bit Q10*/
    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        ashwLclPriSNR[shwI] = CODEC_OpLog10(7, (MED_INT32)(CODEC_OpMax(ashwSNRSmthLcl[shwI], 1)));

        ashwGlbPriSNR[shwI] = CODEC_OpLog10(7, (MED_INT32)(CODEC_OpMax(ashwSNRSmthGlb[shwI], 1)));
    }

    /*����Local��Global�����������ƽ������ֵ�����������Ŀ�����*/
    /*��Local�����������0.1~0.3162��ʱ��Local�����������¼��㣬16bit Q15*/
    CODEC_OpVcMult(ashwLclPriSNR, stCommon->shwOpertBandNum, MED_EANR_SNRMAPCOEFA, ashwTmp);

    CODEC_OpVecShl(ashwTmp, stCommon->shwOpertBandNum, 5, ashwTmp);

    CODEC_OpVcSub(ashwTmp, stCommon->shwOpertBandNum, CODEC_OpNegate(MED_EANR_SNRMAPCOEFB), ashwTmp);

    CODEC_OpVecShl(ashwTmp, stCommon->shwOpertBandNum, 1, stPresenceProb->ashwProbLcl);

    /*����Global�������ʣ�ͬLocal��������*/
    CODEC_OpVcMult(ashwGlbPriSNR, stCommon->shwOpertBandNum, MED_EANR_SNRMAPCOEFA, ashwTmp);

    CODEC_OpVecShl(ashwTmp, stCommon->shwOpertBandNum, 5, ashwTmp);

    CODEC_OpVcSub(ashwTmp, stCommon->shwOpertBandNum, CODEC_OpNegate(MED_EANR_SNRMAPCOEFB), ashwTmp);

    CODEC_OpVecShl(ashwTmp, stCommon->shwOpertBandNum, 1, stPresenceProb->ashwProbGlb);

    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        /*��Local���������С��MED_EANR_PRESENCEPROB_TH1(0.1->13(Q7))��
        Local����������Ϊ�趨����Сֵ(0.005->164(Q15))*/
        if (ashwSNRSmthLcl[shwI] <= MED_EANR_PRESENCEPROB_TH1)
        {
            stPresenceProb->ashwProbLcl[shwI] = MED_EANR_PROB_MIN;
        }

        /*��Local��������ȴ���MED_EANR_PRESENCEPROB_TH2(0.3162->40(Q7))��
        Local����������Ϊ�趨�����ֵMED_EANR_MAX16(1.0->32767(Q15))*/
        if (ashwSNRSmthLcl[shwI] >= MED_EANR_PRESENCEPROB_TH2)
        {
            stPresenceProb->ashwProbLcl[shwI] = MED_EANR_MAX16;
        }

        /*����Global�������ʣ�ͬLocal��������*/
        if (ashwSNRSmthGlb[shwI] <= MED_EANR_PRESENCEPROB_TH1)
        {
            stPresenceProb->ashwProbGlb[shwI] = MED_EANR_PROB_MIN;
        }

        if (ashwSNRSmthGlb[shwI] >= MED_EANR_PRESENCEPROB_TH2)
        {
            stPresenceProb->ashwProbGlb[shwI] = MED_EANR_MAX16;
        }
    }

    /*�����Ӵ�1~Common->shwOpertBandNum-1��ƽ��Local�������ʣ�16bit Q15*/
    shwMeanProb = CODEC_OpVecMeanAbs(stPresenceProb->ashwProbLcl + 1, stCommon->shwOpertBandNum - 2);

    /*��ƽ��Local��������С��MED_EANR_Q13(0.25->8192 Q15)ʱ���Ӵ�
    MED_EANR_PRO_BIN_IDXA~MED_EANR_PRO_BIN_IDXB��Local����������Ϊ�趨����Сֵ*/
    if(shwMeanProb < MED_EANR_Q13)
    {
        CODEC_OpVecSet(stPresenceProb->ashwProbLcl + MED_EANR_PRO_BIN_IDXA,
                     MED_EANR_PRO_BIN_IDXB - MED_EANR_PRO_BIN_IDXA,
                     MED_EANR_PROB_MIN);
    }

    /*��ƽ��Local��������С��MED_EANR_Q14(0.5->16384 Q15)���ҵ�ǰ����
    MED_EANR_PRESENCEPROB_FRMTH(120)֡ʱ���Ӵ�4~Common->shwOpertBandNum-2ִ�����д���*/
    if ((shwMeanProb < MED_EANR_Q14) && (stCommon->swIdx >= MED_EANR_PRESENCEPROB_FRMTH))
    {
        for(shwI = 4; shwI < (stCommon->shwOpertBandNum - 2); shwI++)
        {
            /*����ǰ�Ӵ��������ʴ���ǰ���Ӵ���������֮�͵�MED_EANR_PRESENCEPROB_POWTH(2.5->20480 Q13)
            ��ʱ����ǰ��ǰ���Ӵ���Local����������Ϊ�趨����Сֵ*/
            swTmp = CODEC_OpL_add(aswNoisePowLong[shwI+1], aswNoisePowLong[shwI-1]);

            swTmp = CODEC_OpL_mpy_32_16(swTmp, MED_EANR_PRESENCEPROB_POWTH);

            if ((aswNoisePowLong[shwI] >> 2) > swTmp)
            {
                stPresenceProb->ashwProbLcl[shwI-1] = MED_EANR_PROB_MIN;

                stPresenceProb->ashwProbLcl[shwI]   = MED_EANR_PROB_MIN;

                stPresenceProb->ashwProbLcl[shwI+1] = MED_EANR_PROB_MIN;
            }
        }
    }
}

/*************************************************************************************************
��������: MED_EANR_AbsenceProb_FIX
��������: �������������ڸ���
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/

void MED_EANR_AbsenceProb_FIX(MED_EANR_COMM_STRU *stCommon,
                              MED_EANR_ABSENCEPROB_STRU *stAbsenceProb,
                              MED_INT16 *ashwProbLcl,
                              MED_INT16 *ashwProbGlb)
{
    MED_INT16 shwMeanSNR;
    MED_INT16 shwSNR;
    MED_INT16 shwI;
    MED_INT16 shwPresenProb;
    MED_INT16 shwTmp;
    MED_INT16 ashwTmp[MED_EANR_MAXBINNUM];

    /*��һ֡��Ƶ����������Ⱦ�ֵ��16bit Q7*/
    shwMeanSNR  = stAbsenceProb->shwMeanPriSNR;

    /*���㱾֡Ƶ����������Ⱦ�ֵ���Ӵ�MED_EANR_PROABSENCE_BIN_IDXC-1
    ~Common->shwOpertBandNum-MED_EANR_PROABSENCE_BIN_IDXC+1��16bit Q7*/
    stAbsenceProb->shwMeanPriSNR
                = CODEC_OpVecMeanAbs(stCommon->ashwSNRSmth + (MED_EANR_PROABSENCE_BIN_IDXC - 1),
                                   stCommon->shwOpertBandNum - (MED_EANR_PROABSENCE_BIN_IDXC - 1));

    /*���㱾֡����һ֡Ƶ����������Ⱦ�ֵ֮�16bit Q7*/
    shwMeanSNR  = CODEC_OpSub(stAbsenceProb->shwMeanPriSNR, shwMeanSNR);

    /*���㱾֡Ƶ����������Ⱦ�ֵ�Ķ���ֵ16bit Q6(Q11+Q10-Q15); 10->20480(Q11)*/
    shwSNR      = CODEC_OpMult(20480,
                             CODEC_OpLog10(7, (MED_INT32)(CODEC_OpMax(stAbsenceProb->shwMeanPriSNR, 1))));

    /*�����֡Ƶ����������Ⱦ�ֵС�ڵ���MED_EANR_ABSENCEPROB_SNRTH(-10dB->-640 Q6)��
    ��ƽ������������Ϊ�趨����СֵMED_EANR_PROB_MIN(0.005->164 Q15)*/
    if (shwSNR <= MED_EANR_ABSENCEPROB_SNRTH)
    {
        shwPresenProb = MED_EANR_PROB_MIN;
    }
    /*�����֡����һ֡Ƶ����������Ⱦ�ֵ֮����ڵ���0,��ƽ������������Ϊ�趨��
    ���ֵMED_EANR_MAX16(1.0->32767 Q15)ͬʱ���㱾֡Ƶ����������Ⱦ�ֵ����ֵ���޷�ֵ
    (MED_EANR_PROABSENCE_SNR_L_LIMIT��MED_EANR_PROABSENCE_SNR_H_LIMIT֮��)*/
    else if(shwMeanSNR >= 0)
    {
        stAbsenceProb->shwPriSNR = CODEC_OpMin(CODEC_OpMax(shwSNR, MED_EANR_PROABSENCE_SNR_L_LIMIT),
                                             MED_EANR_PROABSENCE_SNR_H_LIMIT);
        shwPresenProb = MED_EANR_MAX16;
    }
    /*�����֡Ƶ����������Ⱦ�ֵ���ڵ����޷�ֵ��MED_EANR_ABSENCEPROB_SNRPENALTYA
    (5->320 Q6)��ƽ������������Ϊ�趨�����ֵ*/
    else if(shwSNR >= (stAbsenceProb->shwPriSNR - MED_EANR_ABSENCEPROB_SNRPENALTYA))
    {
        shwPresenProb = MED_EANR_MAX16;
    }
    /*�����֡Ƶ����������Ⱦ�ֵС�ڵ����޷�ֵ��MED_EANR_ABSENCEPROB_SNRPENALTYB
    (10dB->640 Q6)��ƽ������������Ϊ�趨����Сֵ*/
    else if(shwSNR <= (stAbsenceProb->shwPriSNR - MED_EANR_ABSENCEPROB_SNRPENALTYB))
    {
        shwPresenProb = MED_EANR_PROB_MIN;
    }
    /*�������������������ƽ�������������¼��㣬16bit Q15*/
    else
    {
        shwTmp        = CODEC_OpAdd(CODEC_OpSub(shwSNR, stAbsenceProb->shwPriSNR),
                                  MED_EANR_ABSENCEPROB_SNRPENALTYB);

        shwTmp        = CODEC_OpMult((MED_INT16)((shwTmp << 6) / MED_EANR_ABSENCEPROB_DIVISOR),
                                   CODEC_OpSub(MED_EANR_MAX16, MED_EANR_PROB_MIN));

        shwTmp        = CODEC_OpAdd(MED_EANR_PROB_MIN, CODEC_OpShl(shwTmp, 3));

        shwPresenProb = shwTmp;

    }

    /*������Ӵ����������ڸ���(1-P_GLB[i]*P_LCL[i]*P_frame)����������
    MED_EANR_PROABSENCE_ABSENCE_PRO_MAX��16bit Q15*/
    CODEC_OpVvMult(ashwProbLcl, ashwProbLcl, stCommon->shwOpertBandNum, ashwTmp);

    CODEC_OpVcMult(ashwTmp, stCommon->shwOpertBandNum, shwPresenProb, ashwTmp);

    for(shwI=0;shwI<stCommon->shwOpertBandNum;shwI++)
    {
        stAbsenceProb->ashwAbsenProb[shwI] = CODEC_OpSub(MED_EANR_MAX16, ashwTmp[shwI]);

        stAbsenceProb->ashwAbsenProb[shwI] = CODEC_OpMin(stAbsenceProb->ashwAbsenProb[shwI],
                                                       MED_EANR_PROABSENCE_ABSENCE_PRO_MAX);
    }

}

/*************************************************************************************************
��������: MED_EANR_Gain_FIX
��������: ��������
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/

void MED_EANR_Gain_FIX(MED_EANR_COMM_STRU *stCommon,
                       MED_EANR_GAIN_STRU *stGain,
                       MED_INT16 shwSNRMin,
                       MED_INT32 *aswPowSpct,
                       MED_INT16 *ashwAbsenProb)
{
    MED_INT16 shwI;
    MED_INT16 shwTmp;
    MED_INT32 aswPstSNR[MED_EANR_MAXBINNUM]      = {0};
    MED_INT16 ashwPriSNR[MED_EANR_MAXBINNUM]     = {0};
    MED_INT16 ashwPresenProb[MED_EANR_MAXBINNUM] = {0};
    MED_INT16 ashwPresenGain[MED_EANR_MAXBINNUM] = {0};
    MED_INT16 ashwAbsenGain[MED_EANR_MAXBINNUM]  = {0};
    MED_INT16 ashwSNRFact[MED_EANR_MAXBINNUM]    = {0};
    MED_INT32 aswNoisePowSpt[MED_EANR_MAXBINNUM] = {0};
    MED_INT32 swTmp1;
    MED_INT32 aswTmp2[MED_EANR_MAXBINNUM]        = {0};
    MED_INT32 aswTmp3[MED_EANR_MAXBINNUM]        = {0};
    MED_INT16 ashwTmp4[MED_EANR_MAXBINNUM];
    MED_INT32 swTmp5;

    /*�����������С��1ʱ�����¸�����������ȣ�16bit Q7*/
    CODEC_OpVcMult(stGain->ashwPriSNRMid,
                 stCommon->shwOpertBandNum,
                 MED_EANR_SPEECHPROB_SNR_SMTH_COEF,
                 ashwPriSNR);

    CODEC_OpVecShr32(stCommon->aswNoisePow,
                   stCommon->shwOpertBandNum,
                   CODEC_OpSub(6, (MED_INT16)(stCommon->shwNormShift << 1)),
                   aswTmp2);

    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        /*�����������ȣ�32bit Q11*/
        aswPstSNR[shwI] = CODEC_OpNormDiv_32(aswPowSpct[shwI],
                                           CODEC_OpL_max(1, aswTmp2[shwI]),
                                           11);

        aswPstSNR[shwI] = CODEC_OpL_max(aswPstSNR[shwI], 1);

        /*����������ȴ��ڵ���MED_EANR_Q11(1->2048 Q11)ʱ,�������������,16bit Q7*/
        if (aswPstSNR[shwI] >= MED_EANR_Q11)
        {
            swTmp5           = CODEC_OpL_sub(aswPstSNR[shwI], MED_EANR_Q11);
            swTmp1           = CODEC_OpL_min(swTmp5,
                                           (MED_INT32)MED_EANR_PSTSNRMAX);

            shwTmp           = CODEC_OpSub(MED_EANR_MAX16,
                                         MED_EANR_SPEECHPROB_SNR_SMTH_COEF);

            swTmp1           = CODEC_OpL_mpy_32_16_r(swTmp1, shwTmp >> 4);

            ashwPriSNR[shwI] = CODEC_OpAdd(ashwPriSNR[shwI], CODEC_OpSaturate(swTmp1));
        }

        /*�����������������shwSNRMin*/
        ashwPriSNR[shwI]     = CODEC_OpMax(ashwPriSNR[shwI], shwSNRMin);

        /*�����������棬16bit Q15*/
        swTmp5               = ((VOS_INT32)ashwPriSNR[shwI]) << 15;
        ashwPresenGain[shwI] = CODEC_OpSaturate(swTmp5 / CODEC_OpAdd(MED_EANR_Q7, ashwPriSNR[shwI]));

        /*����������ޣ�16bit Q11*/
        swTmp1               = CODEC_OpL_mpy_32_16_r(aswPstSNR[shwI], ashwPresenGain[shwI]);

        shwTmp               = CODEC_OpSaturate(swTmp1);
        ashwSNRFact[shwI]    = CODEC_OpMax(shwTmp, 1);

        ashwPresenProb[shwI] = 0;

        /*����������С��MED_EANR_GAIN_ABSENCEPROBTH(0.9->29491 Q15)ʱ
          ���¼����������ʣ�������������Ϊ0*/
        if(ashwAbsenProb[shwI] < MED_EANR_GAIN_ABSENCEPROBTH)
        {
            aswTmp2[shwI]        = CODEC_OpL_Exp(ashwSNRFact[shwI], 11, 8);

            shwTmp               = CODEC_OpSub(MED_EANR_MAX16, ashwAbsenProb[shwI]);

            aswTmp2[shwI]        = CODEC_OpL_mpy_32_16_r(aswTmp2[shwI],shwTmp);

            shwTmp               = CODEC_OpAdd(MED_EANR_Q7, ashwPriSNR[shwI]);

            swTmp1               = CODEC_OpL_mult(ashwAbsenProb[shwI], shwTmp) >> 15;

            ashwPresenProb[shwI] = CODEC_OpSaturate(CODEC_OpNormDiv_32(aswTmp2[shwI],
                                                    CODEC_OpL_add(aswTmp2[shwI], swTmp1),
                                                    15));
        }

        /*����������С��MED_EANR_GAIN_SNRFACTORTH(5->10240 Q11)ʱ��
          ���¼����������棬16bit Q11*/
        ashwPresenGain[shwI]     = ashwPresenGain[shwI] >> 4;

        if(ashwSNRFact[shwI] <= MED_EANR_GAIN_SNRFACTORTH)
        {
            swTmp1 = CODEC_OpL_Integral(ashwSNRFact[shwI]);

            swTmp1 = CODEC_OpL_Exp(CODEC_OpRound(swTmp1), 12, 15);

            swTmp1 = CODEC_OpL_mpy_32_16(swTmp1, ashwPresenGain[shwI]);

            ashwPresenGain[shwI] = CODEC_OpSaturate(swTmp1);
        }

    }

    UCOM_MemCpy(aswNoisePowSpt,
                stCommon->aswNoisePow,
                (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

    /*���Ӵ�1~Common->shwSubBandNum��ȡ��ǰ�Ӵ���ǰ���Ӵ�����СֵΪ��ǰ�Ӵ����������ʣ�32bit Q6*/
    for(shwI = 1; shwI < stCommon->shwSubBandNum; shwI++)
    {
        aswNoisePowSpt[shwI] = CODEC_OpL_min( CODEC_OpL_min( stCommon->aswNoisePow[shwI-1] ,
                                                         stCommon->aswNoisePow[shwI+1]),
                                            stCommon->aswNoisePow[shwI]);
    }

    CODEC_OpVecShr32(stCommon->aswPowSpctSmth,
                   stCommon->shwOpertBandNum,
                   -2,
                   aswTmp2);

    CODEC_OpVecShr32(aswPowSpct,
                   stCommon->shwOpertBandNum,
                   (MED_INT16)(stCommon->shwNormShift << 1),
                   aswTmp3);

    CODEC_OpVvAdd32( aswTmp2,
                   aswTmp3,
                   stCommon->shwOpertBandNum,
                   stCommon->aswPowSpctSmth);

    /*�����������棬16bit Q11��ͬʱ�����źŹ��ʵ�ƽ��ֵ��32bit Q0*/
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        stCommon->aswPowSpctSmth[shwI] = CODEC_OpL_mpy_32_16(stCommon->aswPowSpctSmth[shwI],
                                                           MED_EANR_GAIN_SMOOTHCOEF);

        swTmp1                         = CODEC_OpNormDiv_32(aswNoisePowSpt[shwI],
                                                          stCommon->aswPowSpctSmth[shwI] + 1,
                                                          16);

        ashwAbsenGain[shwI]            = CODEC_OpMult(stGain->shwSNRSqrtMin,
                                                    CODEC_OpSqrt(swTmp1));

        ashwAbsenGain[shwI]            = CODEC_OpMax(ashwAbsenGain[shwI], 1);
    }

    CODEC_OpVecShr32(aswPstSNR, stCommon->shwOpertBandNum, -4, aswTmp2);

    CODEC_OpVvMult(ashwPresenGain, ashwPresenGain, stCommon->shwOpertBandNum, ashwTmp4);

    /*�������棬16bit Q12�����޽ص�MED_EANR_GAIN_UPLIMIT(1->4095, Q12)��
    ͬʱ������������ȼ�����м������16bit Q7*/
    for(shwI = 0;shwI < stCommon->shwOpertBandNum; shwI++)
    {
        shwTmp = ashwPresenGain[shwI] / CODEC_OpMax(ashwAbsenGain[shwI],1);

        swTmp1 = CODEC_OpL_Power(shwTmp, 0, ashwPresenProb[shwI], 15, 1);

        swTmp1 = CODEC_OpL_mult0(ashwAbsenGain[shwI], CODEC_OpSaturate(swTmp1));

        shwTmp = CODEC_OpSaturate(swTmp1);

        stGain->ashwGain[shwI]      = CODEC_OpMin(shwTmp, MED_EANR_GAIN_UPLIMIT);

        stGain->ashwPriSNRMid[shwI] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(aswTmp2[shwI],
                                                                       ashwTmp4[shwI]));
    }
}

/*************************************************************************************************
��������: MED_EANR_DATA_FIX
��������: Ƶ�����ݳ�������
�޸���ʷ: 1. 2013��7��27�գ������ɺ���
**************************************************************************************************/

void MED_EANR_DATA_FIX(MED_EANR_COMM_STRU *stCommon,
                       MED_INT16 *ashwGain,
                       MED_INT16 *shwDataBuf)
{
    MED_INT16 shwI;
    MED_INT16 shwJ;

    for (shwI = 1; shwI < stCommon->shwSubBandNum; shwI++)
    {
        /*ÿ���Ӵ��ĸ���Ƶ��ͬ���Ը��Ӵ������棬16bit Q0*/
        for (shwJ = MED_EANR_SUBBAND_FL[shwI];
             shwJ <= MED_EANR_SUBBAND_FH[shwI];
             shwJ++)
        {
            shwDataBuf[shwJ * 2]
                  = CODEC_OpSaturate(CODEC_OpL_mult0(shwDataBuf[shwJ * 2], ashwGain[shwI]) >> 12);

            shwDataBuf[(shwJ * 2) + 1]
                  = CODEC_OpSaturate(CODEC_OpL_mult0(shwDataBuf[(shwJ * 2)+ 1], ashwGain[shwI]) >> 12);
        }
    }

    /*��ȥֱ���͵�Ƶ�ź�*/
    shwDataBuf[0] = 0;
    shwDataBuf[1] = 0;
    shwDataBuf[2] = 0;
    shwDataBuf[3] = 0;
    shwDataBuf[4] = 0;
    shwDataBuf[5] = 0;
    shwDataBuf[6] = 0;
    shwDataBuf[7] = 0;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
