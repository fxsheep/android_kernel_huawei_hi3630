

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/

#include "codec_op_vec.h"
#include "med_pp_comm.h"
#include "med_gain.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
MED_SIDETONE_OBJ_STRU    g_astMedSideToneObjPool[MED_SIDETONE_MAX_OBJ_NUM];     /* ���������˲���ʵ����Դ�� */
MED_DEVGAIN_OBJ_STRU     g_astMedDevGainObjPool[MED_DEVGAIN_MAX_OBJ_NUM];       /* �豸����ʵ����Դ�� */
MED_OBJ_INFO             g_stMedSideToneObjInfo;                             /* ��ص���Ϣ */
MED_OBJ_INFO             g_stMedDevGainObjInfo;                                 /* ��ص���Ϣ */

/* dB���������ӳ���, Q13 */
VOS_INT16                g_ashwGainTab[93] =
{
     0,      1,      1,      1,      1,      1,      2,
     2,      2,      2,      3,      3,      3,      4,
     4,      5,      5,      6,      7,      7,      8,
     9,      10,     12,     13,     15,     16,     18,
     21,     23,     26,     29,     33,     37,     41,
     46,     52,     58,     65,     73,     82,     92,
     103,    116,    130,    146,    163,    183,    206,
     231,    259,    291,    326,    366,    411,    461,
     517,    580,    651,    730,    819,    919,    1031,
     1157,   1298,   1457,   1635,   1834,   2058,   2309,
     2591,   2907,   3261,   3659,   4106,   4607,   5169,
     5799,   6507,   7301,   8192,   9192,   10313,  11572,
     12983,  14568,  16345,  18340,  20577,  23088,  25905,
     29066,  32613
 };

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_VOID* MED_SIDETONE_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_SIDETONE_GetObjInfoPtr,
                    MED_SIDETONE_GetObjPtr,
                    MED_SIDETONE_MAX_OBJ_NUM,
                    sizeof(MED_SIDETONE_OBJ_STRU)));
}


VOS_UINT32 MED_SIDETONE_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_SIDETONE_OBJ_STRU  *pstObj;
    pstObj      = (MED_SIDETONE_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_SIDETONE_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_SIDETONE_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_SIDETONE_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32             uwRet;
    VOS_INT32              swCnt;
    MED_SIDETONE_OBJ_STRU *pstSideTone = (MED_SIDETONE_OBJ_STRU *)pstInstance;

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_SIDETONE_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻��,�������ֹ���� */
        if (shwParaLen < MED_SIDETONE_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡ�˲�ϵ������Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            for (swCnt = MED_GAIN_MIN; swCnt <= MED_GAIN_MAX; swCnt++)
            {
                if (MED_GAIN_Db2Linear(swCnt) == pstSideTone->shwSetGain)
                {
                    break;
                }
            }

            pshwParam[0]    = (VOS_INT16)swCnt;                                        /* ʩ������ */
        }
    }

    return uwRet;

}
VOS_UINT32 MED_SIDETONE_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32             uwRet;
    MED_SIDETONE_OBJ_STRU *pstSideTone = (MED_SIDETONE_OBJ_STRU *)pstInstance;

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_SIDETONE_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý������������ò��� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻�����������ֹ���� */
        if (shwParaLen < MED_SIDETONE_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ashwFrmDat��Ա����ȫ����Ϊ0 */
            CODEC_OpVecSet(&(pstSideTone->ashwFrmDat[0]), (VOS_INT32)(pstSideTone->shwFrmLen), 0);

            /* dB����ת�������� */
            /* 1.�ж�����Ƿ�Ϸ� */
            if ((pshwParam[0] > MED_GAIN_MAX)||(pshwParam[0] < MED_GAIN_MIN))
            {
                return UCOM_RET_ERR_PARA;
            }

            /* 2.���㲢������������ */
            pstSideTone->shwSetGain  = MED_GAIN_Db2Linear(pshwParam[0]);
            pstSideTone->shwCurGain  = MED_GAIN_Db2Linear(pshwParam[0]);

            /* ��ȡ֡����Ϣ */
            pstSideTone->shwFrmLen= MED_PP_GetFrameLength();

        }
    }

    return uwRet;

}


VOS_UINT32 MED_SIDESTONE_UpdateTx (
                VOS_VOID               *pstSideToneInstance,
                VOS_INT16              *pshwTxIn)
{
    MED_SIDETONE_OBJ_STRU *pstSideTone = (MED_SIDETONE_OBJ_STRU*)pstSideToneInstance;/* ��ʱSIDETONEָ�� */

    if((NULL == pshwTxIn) || (NULL == pstSideToneInstance))
    {
        return UCOM_RET_FAIL;
    }

    /* ��TX֡���Ƶ�����������ӽṹ���� */
    CODEC_OpVecCpy(  &(pstSideTone->ashwFrmDat[0]),
                    pshwTxIn,
                    pstSideTone->shwFrmLen);
    return UCOM_RET_SUCC;
}


VOS_UINT32 MED_SIDETONE_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwRxIn,
                VOS_INT16              *pshwOut)
{
    VOS_INT16              shwTmp;

    /* ��ʱSIDETONEָ�� */
    MED_SIDETONE_OBJ_STRU *pstSideTone = (MED_SIDETONE_OBJ_STRU*)pstInstance;
    VOS_INT32              swEnerSum;

    /* ���㵱ǰ֡������ */
    swEnerSum = 0;

    swEnerSum = CODEC_OpVvMac(pshwRxIn, pshwRxIn, pstSideTone->shwFrmLen,swEnerSum);
    swEnerSum = CODEC_OpL_shr(swEnerSum, 1);

    shwTmp    = MED_GAIN_MIN;

    /* ���ݵ�ǰ֡������С���Ʋ������� */
    if (swEnerSum > MED_SIDETONE_RX_LOUD)                                       /* ����-30dBm0,��ر�sidetone */
    {

        pstSideTone->shwCurGain    = MED_GAIN_Db2Linear(shwTmp);
        pstSideTone->swSTMuteCount = 0;
    }
    else
    /* �����ʱ����sidetone�ر���(sideToneΪ��С����)����Ӽ������ж��Ƿ������ֵ */
    {
        if (MED_GAIN_Db2Linear(shwTmp) == pstSideTone->shwCurGain)
        {
            pstSideTone->swSTMuteCount = pstSideTone->swSTMuteCount + 1;
            if(pstSideTone->swSTMuteCount > MED_PP_AGC_RX_DELAY)
            {
                pstSideTone->shwCurGain    = pstSideTone->shwSetGain;
                pstSideTone->swSTMuteCount = 0;
            }
        }
    }

    /* �������棬��ʵ��Out'(i) = Gain * SideSignal(i) */
    CODEC_OpVcMultScale(&(pstSideTone->ashwFrmDat[0]),
                      (VOS_INT32)(pstSideTone->shwFrmLen),
                      pstSideTone->shwCurGain,
                      MED_PP_SHIFT_BY_14,
                      &(pstSideTone->ashwFrmDat[0]));

    /* �������ӣ���ʵ��Out(i) = RxIn(i) + Out'(i) */
    CODEC_OpVvAdd(pshwRxIn,
                &(pstSideTone->ashwFrmDat[0]),
                (VOS_INT32)(pstSideTone->shwFrmLen),
                pshwOut);

    return UCOM_RET_SUCC;
}
VOS_VOID* MED_DEVGAIN_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_DEVGAIN_GetObjInfoPtr,
                    MED_DEVGAIN_GetObjPtr,
                    MED_DEVGAIN_MAX_OBJ_NUM,
                    sizeof(MED_DEVGAIN_OBJ_STRU)));
}


VOS_UINT32 MED_DEVGAIN_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32               uwRet;
    MED_DEVGAIN_OBJ_STRU    *pstObj;
    pstObj      = (MED_DEVGAIN_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_DEVGAIN_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_DEVGAIN_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_DEVGAIN_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32             uwRet;
    VOS_INT32              swCnt;
    MED_DEVGAIN_OBJ_STRU  *pstDevGain = (MED_DEVGAIN_OBJ_STRU *)pstInstance;

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_DEVGAIN_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻�����������ֹ���� */
        if (shwParaLen < MED_DEVGAIN_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡ�������� */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            for (swCnt = MED_GAIN_MIN; swCnt <= MED_GAIN_MAX; swCnt++)
            {
                if (MED_GAIN_Db2Linear(swCnt) == pstDevGain->shwGain)
                {
                    break;
                }
            }

            pshwParam[0]    = (VOS_INT16)swCnt;                                            /* ʩ������ */
        }
    }

    return uwRet;

}
VOS_UINT32 MED_DEVGAIN_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32             uwRet;
    MED_DEVGAIN_OBJ_STRU  *pstDevGain = (MED_DEVGAIN_OBJ_STRU *)pstInstance;

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_DEVGAIN_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý������������ò��� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻�����������ֹ���� */
        if (shwParaLen < MED_DEVGAIN_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* dB����ת�������� */
            /* �ж�����Ƿ�Ϸ� */
            if ((pshwParam[0] > MED_GAIN_MAX)||(pshwParam[0] < MED_GAIN_MIN))
            {
                return UCOM_RET_ERR_PARA;
            }

            /* ���㲢������������ */
            pstDevGain->shwGain  = MED_GAIN_Db2Linear(pshwParam[0]);

            /* ��ȡ֡����Ϣ */
            pstDevGain->shwFrmLen= MED_PP_GetFrameLength();

        }
    }

    return uwRet;

}
VOS_UINT32 MED_DEVGAIN_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{
    /* ��ʱDEVGAINָ�� */
    MED_DEVGAIN_OBJ_STRU *pstDevGain = (MED_DEVGAIN_OBJ_STRU*)pstInstance;

    /* �豸�������㣬��ʵ��Out(i) = Gain * In(i) */
    CODEC_OpVcMultScale(  pshwIn,
                        (VOS_INT32)(pstDevGain->shwFrmLen),
                        pstDevGain->shwGain,
                        MED_PP_SHIFT_BY_14,
                        pshwOut);

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
