

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "med_pp_comm.h"
#include "med_filt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
MED_PREEMP_OBJ_STRU    g_astMedPreempObjPool[MED_PREEMP_MAX_OBJ_NUM];           /* Ԥ�����˲���ʵ����Դ�� */
MED_DEEMP_OBJ_STRU     g_astMedDeempObjPool[MED_DEEMP_MAX_OBJ_NUM];             /* ȥ�����˲���ʵ����Դ�� */
MED_HPF_OBJ_STRU       g_astMedHpfObjPool[MED_HPF_MAX_OBJ_NUM];                 /* ��ͨ�˲���ʵ����Դ�� */
MED_COMP_OBJ_STRU      g_astMedCompObjPool[MED_COMP_MAX_OBJ_NUM];               /* Ƶ�첹���˲���ʵ����Դ�� */
MED_OBJ_INFO           g_stMedPreempObjInfo;                                    /* ��ص���Ϣ */
MED_OBJ_INFO           g_stMedDeempObjInfo;                                     /* ��ص���Ϣ */
MED_OBJ_INFO           g_stMedHpfObjInfo;                                       /* ��ص���Ϣ */
MED_OBJ_INFO           g_stMedCompObjInfo;                                      /* ��ص���Ϣ */

/* ��ʱȫ������, ���˲���ʱʹ�� */
VOS_INT32              g_aswMedFiltTmpLen320[CODEC_FRAME_LENGTH_WB];

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
#ifndef _MED_C89_

VOS_VOID MED_FILT_Fir (
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwCoef,
                VOS_INT16               shwInLen,
                VOS_INT32               swFirLen,
                VOS_INT16               shwGain,
                VOS_INT16              *pshwOut)
{
    VOS_INT32       swCnt;                                                      /* ���� */
    fir_bk(pshwOut,  pshwIn, pshwCoef, shwInLen, swFirLen);
    for (swCnt = 0; swCnt < shwInLen; swCnt++)
    {
        pshwOut[swCnt] = CODEC_OpShl(pshwOut[swCnt], shwGain);
    }
}
#else
VOS_VOID MED_FILT_Fir (
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwCoef,
                VOS_INT16               shwInLen,
                VOS_INT32               swFirLen,
                VOS_INT16               shwGain,
                VOS_INT16              *pshwOut)
{
    VOS_INT32       swCnt;                                                      /* ���� */
    VOS_INT32       swTemp;                                                     /* ��ʱ���� */

    for (swCnt = 0; swCnt < shwInLen; swCnt++)
    {
        swTemp = CODEC_OpVvMacD(&pshwIn[swCnt], &pshwCoef[swFirLen - 1], swFirLen, 0);  /* Q16 */

        /* ���ȵ��� */
        pshwOut[swCnt] = CODEC_OpSaturate(CODEC_OpL_shl(swTemp, CODEC_OpSub(shwGain, MED_PP_SHIFT_BY_16)));

    }
}
#endif

#if 0

VOS_VOID MED_FILT_Iir32 (
                MED_IIR32_PARA_STRU    *pstFiltPara,
                VOS_INT16              *pshwIn,
                VOS_INT16               shwInLen,
                VOS_INT16              *pshwOut)
{
    VOS_INT32       swSum;                                                      /* �����ͣ�32bit Q15 */
    VOS_INT32       swCntI;                                                     /* ������ */
    VOS_INT32       swCntJ;                                                     /* ������ */
    VOS_INT32       swIirLen            = pstFiltPara->swIirLen;
    VOS_INT16      *pshwNum             = pstFiltPara->pshwNum;
    VOS_INT16      *pshwDen             = pstFiltPara->pshwDen;
    VOS_INT32      *pswOutFeedBack      = pstFiltPara->pswOutFeedBack;

    /* ��Ҫ����32λ��ʷ��Ϣ */
    for (swCntI = swIirLen - 1; swCntI < (shwInLen + (swIirLen - 1)); swCntI++)
    {
        swSum = CODEC_OpL_mult0(pshwNum[0], pshwIn[swCntI]);
        for (swCntJ = 1; swCntJ < swIirLen; swCntJ++)
        {
            swSum = CODEC_OpL_add(swSum,  CODEC_OpL_mult0(pshwIn[swCntI - swCntJ],
                                                      pshwNum[swCntJ]));
            swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutFeedBack[swCntI - swCntJ],
                                                          pshwDen[swCntJ]));
        }
        pswOutFeedBack[swCntI] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);
    }

    for (swCntI = 0; swCntI < shwInLen; swCntI++)
    {
        pshwOut[swCntI] = CODEC_OpSaturate(CODEC_OpL_shr(pswOutFeedBack[swCntI + (swIirLen - 1)],
                                                     MED_FILT_NORM_MAX));
    }
}


VOS_VOID MED_FILT_Iir (
                MED_IIR_PARA_STRU      *pstFiltPara,
                VOS_INT16              *pshwIn,
                VOS_INT16               shwInLen,
                VOS_INT16              *pshwOut)
{
    VOS_INT16       shwSum;                                                     /* �����ͣ�16bit Q0 */
    VOS_INT32       swCntI;                                                     /* ������ */
    VOS_INT32       swCntJ;                                                     /* ������ */
    VOS_INT32       swIirLen            = pstFiltPara->swIirLen;
    VOS_INT16      *pshwNum             = pstFiltPara->pshwNum;
    VOS_INT16      *pshwDen             = pstFiltPara->pshwDen;

    for (swCntI = swIirLen - 1; swCntI < (shwInLen + (swIirLen - 1)); swCntI++)
    {
        shwSum = CODEC_OpMult(pshwNum[0], pshwIn[swCntI]);

        for (swCntJ = 1; swCntJ < swIirLen; swCntJ++)
        {
            shwSum = CODEC_OpAdd(shwSum,  CODEC_OpMult( pshwIn[swCntI - swCntJ],
                                                    pshwNum[swCntJ]));
            shwSum = CODEC_OpSub(shwSum,  CODEC_OpMult( pshwOut[swCntI - swCntJ],
                                                    pshwDen[swCntJ]));
        }
        pshwOut[swCntI] = CODEC_OpShr_r(shwSum, pshwDen[0]);
    }
}
#endif
VOS_VOID* MED_PREEMP_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_PREEMP_GetObjInfoPtr,
                    MED_PREEMP_GetObjPtr,
                    MED_PREEMP_MAX_OBJ_NUM,
                    sizeof(MED_PREEMP_OBJ_STRU)));
}


VOS_UINT32 MED_PREEMP_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_PREEMP_OBJ_STRU    *pstObj;
    pstObj      = (MED_PREEMP_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_PREEMP_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_PREEMP_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_PREEMP_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32           uwRet;
    MED_PREEMP_OBJ_STRU *pstPreemp = (MED_PREEMP_OBJ_STRU *)pstInstance;
    MED_PREEMP_NV_STRU  *pstNv     = &(pstPreemp->stNv);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_PREEMP_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_PREEMP_PARAM_LEN)
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ����˲�ϵ������Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0]    = (VOS_INT16)pstNv->enEnable;                       /* ʹ���� */
            pshwParam[1]    = pstNv->shwCoef;
        }
    }

    return uwRet;

}
VOS_UINT32 MED_PREEMP_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32           uwRet;
    MED_PREEMP_OBJ_STRU *pstPreemp = (MED_PREEMP_OBJ_STRU *)pstInstance;
    MED_PREEMP_NV_STRU  *pstNv     = &(pstPreemp->stNv);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_PREEMP_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý������������ò��� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_PREEMP_PARAM_LEN)
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ֵʹ���������롢��������ƫ���֡�� */
            pstNv->enEnable                   = (VOS_UINT16)pshwParam[0];
            pstNv->shwCoef                    = pshwParam[1];
            pstPreemp->shwFrmLen              = MED_PP_GetFrameLength();

            /* �����źŵ����һ���ʼ�� */
            pstPreemp->shwPreSignal           = 0;
        }
    }

    return uwRet;

}
VOS_UINT32 MED_PREEMP_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{
    VOS_INT32   swCnt;
    VOS_INT16   ashwInTmp[CODEC_FRAME_LENGTH_WB] = {0};

    /* ��ʱPREEMP�����ָ�� */
    MED_PREEMP_OBJ_STRU *pstPreemp = (MED_PREEMP_OBJ_STRU*)pstInstance;
    MED_PREEMP_NV_STRU  *pstNv     = &(pstPreemp->stNv);


    /* �ж�PREEMPģ���Ƿ�ʹ�� */
    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        /* PREEMP��ʹ�� */
        CODEC_OpVecCpy(pshwOut, pshwIn, pstPreemp->shwFrmLen);
        return UCOM_RET_SUCC;
    }

    /* ��ֹpshwIn��pshwOutָ���ڴ��ص������˲����� */
    CODEC_OpVecCpy(&(ashwInTmp[0]), pshwIn, pstPreemp->shwFrmLen);

    /* FIR �˲� */
    pshwOut[0] = CODEC_OpAdd(ashwInTmp[0], CODEC_OpMult(pstNv->shwCoef, pstPreemp->shwPreSignal));

    for (swCnt = 1; swCnt < pstPreemp->shwFrmLen; swCnt++)
    {
        pshwOut[swCnt] = CODEC_OpAdd(ashwInTmp[swCnt], CODEC_OpMult(pstNv->shwCoef, ashwInTmp[swCnt-1]));
    }

    /* ���������źŵ����һ���� */
    pstPreemp->shwPreSignal = ashwInTmp[pstPreemp->shwFrmLen - 1];

    return UCOM_RET_SUCC;
}


VOS_VOID* MED_DEEMP_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_DEEMP_GetObjInfoPtr,
                    MED_DEEMP_GetObjPtr,
                    MED_DEEMP_MAX_OBJ_NUM,
                    sizeof(MED_DEEMP_OBJ_STRU)));
}


VOS_UINT32 MED_DEEMP_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_DEEMP_OBJ_STRU     *pstObj;
    pstObj      = (MED_DEEMP_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_DEEMP_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_DEEMP_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_DEEMP_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32           uwRet;
    MED_DEEMP_OBJ_STRU  *pstDeemp  = (MED_DEEMP_OBJ_STRU *)pstInstance;
    MED_DEEMP_NV_STRU   *pstNv     = &(pstDeemp->stNv);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_DEEMP_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (shwParaLen < MED_DEEMP_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ����˲�ϵ����������ȵ���Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0]    = (VOS_INT16)pstNv->enEnable;
            pshwParam[1]    = pstNv->shwCoef;                                   /* ȥ����ϵ�� */
        }
    }

    return uwRet;

}
VOS_UINT32 MED_DEEMP_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32           uwRet;
    MED_DEEMP_OBJ_STRU  *pstDeemp        = (MED_DEEMP_OBJ_STRU *)pstInstance;
    MED_DEEMP_NV_STRU   *pstNv           = &(pstDeemp->stNv);
    VOS_INT16            shwFrameLength  = MED_PP_GetFrameLength();
    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_DEEMP_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý������������ò��� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ��㹻���ж�Ԥ���ص�ʹ�����Ƿ�Ϸ�������Ϸ�����ô��ȥ����ģ�鸳��ͬ����ʹ����*/
        if (   (shwParaLen < MED_DEEMP_PARAM_LEN)
            || ((MED_PREEMP_GetObjPtr->stNv).enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ֵ�˲�ϵ����֡����ʹ���� */
            pstNv->enEnable                  = (VOS_UINT16)pshwParam[0];
            pstNv->shwCoef                   = pshwParam[1];
            pstDeemp->shwFrmLen              = shwFrameLength;

            /* �����źŵ����һ���ʼ�� */
            pstDeemp->shwPreOut              = 0;
        }
    }

    return uwRet;

}
VOS_UINT32 MED_DEEMP_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{
    MED_DEEMP_OBJ_STRU *pstDeemp = (MED_DEEMP_OBJ_STRU*)pstInstance;            /* ��ʱDEEMPָ�� */
    MED_DEEMP_NV_STRU  *pstNv    = &(pstDeemp->stNv);
    VOS_INT32           swCnt;

    VOS_INT16           ashwInTmp[CODEC_FRAME_LENGTH_WB] = {0};

    /* �ж�DEEMPģ���Ƿ�ʹ�� */
    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        /* DEEMP��ʹ�� */
        CODEC_OpVecCpy(pshwOut, pshwIn, pstDeemp->shwFrmLen);
        return UCOM_RET_SUCC;
    }

    /* ��ֹpshwIn��pshwOutָ���ڴ��ص������˲����� */
    CODEC_OpVecCpy(&(ashwInTmp[0]), pshwIn, pstDeemp->shwFrmLen);

    pshwOut[0] = CODEC_OpSub(ashwInTmp[0], CODEC_OpMult(pstNv->shwCoef, pstDeemp->shwPreOut));

    for (swCnt = 1; swCnt < pstDeemp->shwFrmLen; swCnt++)
    {
        pshwOut[swCnt] = CODEC_OpSub(ashwInTmp[swCnt], CODEC_OpMult(pstNv->shwCoef, pshwOut[swCnt-1]));
    }

    /* ��������źŵ����һ���� */
    pstDeemp->shwPreOut = pshwOut[pstDeemp->shwFrmLen-1];

    return UCOM_RET_SUCC;
}
VOS_VOID* MED_HPF_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_HPF_GetObjInfoPtr,
                    MED_HPF_GetObjPtr,
                    MED_HPF_MAX_OBJ_NUM,
                    sizeof(MED_HPF_OBJ_STRU)));
}


VOS_UINT32 MED_HPF_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_HPF_OBJ_STRU       *pstObj;
    pstObj       = (MED_HPF_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet        = MED_PP_CheckPtrValid(MED_HPF_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_HPF_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_HPF_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32           uwRet;
    MED_HPF_OBJ_STRU    *pstHpf = (MED_HPF_OBJ_STRU *)pstInstance;
    MED_HPF_NV_STRU     *pstNv  = &(pstHpf->stNv);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_HPF_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_HPF_PARAM_LEN)
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ����˲�ϵ������Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0]    = (VOS_INT16)pstNv->enEnable;                                         /* ʹ���� */

            /* ��ͨ�˲�ϵ����� */
            CODEC_OpVecCpy(  &(pshwParam[MED_HPF_PARAM_VEC_BASE]),
                           &(pstNv->ashwDen[0]),
                            MED_HPF_LEN);
            CODEC_OpVecCpy(  &(pshwParam[MED_HPF_PARAM_VEC_BASE + MED_HPF_LEN]),
                           &(pstNv->ashwNum[0]),
                            MED_HPF_LEN);
        }

    }

    return uwRet;

}




VOS_UINT32 MED_HPF_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32           uwRet;
    MED_HPF_OBJ_STRU    *pstHpf = (MED_HPF_OBJ_STRU *)pstInstance;
    MED_HPF_NV_STRU     *pstNv  = &(pstHpf->stNv);
    VOS_INT16            shwFrameLength  = MED_PP_GetFrameLength();

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_HPF_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý������������ò��� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < MED_PREEMP_PARAM_LEN)
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ʹ���ֵ */
            pstNv->enEnable   = (VOS_UINT16)pshwParam[0];

            /* ��ͨ�˲�ϵ����� */
            CODEC_OpVecCpy(  &(pstNv->ashwDen[0]),
                           &(pshwParam[MED_HPF_PARAM_VEC_BASE]),
                            MED_HPF_LEN);
            CODEC_OpVecCpy(  &(pstNv->ashwNum[0]),
                           &(pshwParam[MED_HPF_PARAM_VEC_BASE + MED_HPF_LEN]),
                            MED_HPF_LEN);

            /* ��ȡ֡����Ϣ */
            pstHpf->shwFrmLen  = shwFrameLength;

            /* ��IIRά���ṹ���ʼ�� */
            pstHpf->stFiltPara.pshwDen        = &(pstNv->ashwDen[0]);
            pstHpf->stFiltPara.pshwNum        = &(pstNv->ashwNum[0]);
            pstHpf->stFiltPara.swIirLen       = MED_HPF_LEN;

            /* ����&����źŵ���ʷ��Ϣ��ʼ�� */
            CODEC_OpVecSet(&(pstHpf->ashwPreIn[0]), MED_HPF_HISTORY_BUFF_LEN, 0);
            UCOM_MemSet(&(pstHpf->aswPreOut[0]),
                        0,
                        sizeof(VOS_INT32) * MED_HPF_HISTORY_BUFF_LEN);
        }

    }

    return uwRet;

}

#ifndef _MED_C89_
VOS_UINT32 MED_HPF_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{
    MED_HPF_OBJ_STRU *pstHpf     = (MED_HPF_OBJ_STRU*)pstInstance;
    MED_HPF_NV_STRU  *pstNv      = &(pstHpf->stNv);

    VOS_INT16         ashwInTmp[CODEC_FRAME_LENGTH_WB] = {0};
    VOS_INT32         *pswOutTemp = MED_FILT_GetswVecTmp320Ptr();
    VOS_INT32         swCntI, swSum;
    VOS_INT32         swIirLen            = pstHpf->stFiltPara.swIirLen;
    VOS_INT16        *pshwNum             = pstHpf->stFiltPara.pshwNum;
    VOS_INT16        *pshwDen             = pstHpf->stFiltPara.pshwDen;

    ae_p24x2s aepNum1, aepNum2, aepDen1, aepDen2, aepX1, aepX2;
    ae_q56s   aeqS1, aeqSum;

    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        CODEC_OpVecCpy(pshwOut, pshwIn, pstHpf->shwFrmLen);
        return UCOM_RET_SUCC;
    }

    CODEC_OpVecCpy(&ashwInTmp[0], pshwIn, pstHpf->shwFrmLen);

    aepNum1 = *((ae_p16x2s *)&pshwNum[0]);
    aepNum2 = *((ae_p16x2s *)&pshwNum[2]);
    aepDen1 = *((ae_p16x2s *)&pshwDen[0]);
    aepDen2 = *((ae_p16x2s *)&pshwDen[2]);
    aepX1   = *((ae_p16x2s *)&pstHpf->ashwPreIn[0]);
    aepX2   = *((ae_p16s *)&pstHpf->ashwPreIn[2]);

/*
    swSum = CODEC_OpL_shr(CODEC_OpL_mult0(pshwNum[0], ashwInTmp[0]), 1);
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[2], pshwNum[1]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[2], pshwDen[1]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[1], pshwNum[2]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[1], pshwDen[2]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[0], pshwNum[3]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[0], pshwDen[3]));
    pswOutTemp[0] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);
*/
    swSum    = pshwNum[0] * ashwInTmp[0];
    aeqSum   = *((ae_q32s *)&swSum);
    AE_MULAP24S_LL(aeqSum, aepX2, aepNum1);
    AE_MULAAP24S_HL_LH(aeqSum, aepX1, aepNum2);
    aeqSum = AE_SRAAQ56(aeqSum, 1);
    aeqS1 = *((ae_q32s *)&pstHpf->aswPreOut[0]);
    AE_MULSFQ32SP16S_L(aeqSum, aeqS1, aepDen2);
    aeqS1 = *((ae_q32s *)&pstHpf->aswPreOut[1]);
    AE_MULSFQ32SP16S_H(aeqSum, aeqS1, aepDen2);
    aeqS1 = *((ae_q32s *)&pstHpf->aswPreOut[2]);
    AE_MULSFQ32SP16S_L(aeqSum, aeqS1, aepDen1);
    aeqSum = AE_SLLAQ56(aeqSum, 2);
    aeqSum = AE_SATQ48S(aeqSum);
    pswOutTemp[0] = AE_TRUNCA32Q48(aeqSum);

    swSum = CODEC_OpL_shr(CODEC_OpL_mult0(pshwNum[0], ashwInTmp[1]),1);
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[0], pshwNum[1]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[0], pshwDen[1]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[2], pshwNum[2]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[2], pshwDen[2]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[1], pshwNum[3]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[1], pshwDen[3]));
    pswOutTemp[1] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);
    swSum = CODEC_OpL_shr(CODEC_OpL_mult0(pshwNum[0], ashwInTmp[2]),1);
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[1], pshwNum[1]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[1], pshwDen[1]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[0], pshwNum[2]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[0], pshwDen[2]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[2], pshwNum[3]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[2], pshwDen[3]));
    pswOutTemp[2] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);

    for (swCntI = swIirLen - 1; swCntI < pstHpf->shwFrmLen; swCntI++)
    {
/*
        swSum = CODEC_OpL_shr((pshwNum[0] * ashwInTmp[swCntI]),1);
        swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr((ashwInTmp[swCntI - 1] * pshwNum[1]),1));
        swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[swCntI - 1], pshwDen[1]));
        swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr((ashwInTmp[swCntI - 2] * pshwNum[2]),1));
        swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[swCntI - 2], pshwDen[2]));
        swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr((ashwInTmp[swCntI - 3] * pshwNum[3]),1));
        swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[swCntI - 3], pshwDen[3]));
        pswOutTemp[swCntI] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);
 */
        aepX1   = *((ae_p16x2s *)&ashwInTmp[swCntI - 3]);
        aepX2   = *((ae_p16s *)&ashwInTmp[swCntI - 1]);
        swSum    = pshwNum[0] * ashwInTmp[swCntI];
        aeqSum   = *((ae_q32s *)&swSum);
        AE_MULAP24S_LL(aeqSum, aepX2, aepNum1);
        AE_MULAAP24S_HL_LH(aeqSum, aepX1, aepNum2);
        aeqSum = AE_SRAAQ56(aeqSum, 1);
        aeqS1 = *((ae_q32s *)&pswOutTemp[swCntI - 3]);
        AE_MULSFQ32SP16S_L(aeqSum, aeqS1, aepDen2);
        aeqS1 = *((ae_q32s *)&pswOutTemp[swCntI - 2]);
        AE_MULSFQ32SP16S_H(aeqSum, aeqS1, aepDen2);
        aeqS1 = *((ae_q32s *)&pswOutTemp[swCntI - 1]);
        AE_MULSFQ32SP16S_L(aeqSum, aeqS1, aepDen1);
        aeqSum = AE_SLLAQ56(aeqSum, 2);
        aeqSum = AE_SATQ48S(aeqSum);
        pswOutTemp[swCntI] = AE_TRUNCA32Q48(aeqSum);
    }

    CODEC_OpVecCpy(&pstHpf->ashwPreIn[0], &(ashwInTmp[pstHpf->shwFrmLen - MED_HPF_HISTORY_BUFF_LEN]), MED_HPF_HISTORY_BUFF_LEN);
    CODEC_OpVecCpy((VOS_INT16 *)&pstHpf->aswPreOut[0], (VOS_INT16 *)&(pswOutTemp[pstHpf->shwFrmLen - MED_HPF_HISTORY_BUFF_LEN]), MED_HPF_HISTORY_BUFF_LEN*2);

    for (swCntI = 0; swCntI < pstHpf->shwFrmLen; swCntI++)
    {
        pshwOut[swCntI] = CODEC_OpSaturate(CODEC_OpL_shr(pswOutTemp[swCntI], MED_FILT_NORM_MAX-1));
    }

    return UCOM_RET_SUCC;
}
#else

VOS_UINT32 MED_HPF_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{

    /* ��ʱHPFָ�� */
    MED_HPF_OBJ_STRU *pstHpf     = (MED_HPF_OBJ_STRU*)pstInstance;
    MED_HPF_NV_STRU  *pstNv      = &(pstHpf->stNv);

    VOS_INT16         ashwInTmp[CODEC_FRAME_LENGTH_WB] = {0};
    VOS_INT32         *pswOutTemp = MED_FILT_GetswVecTmp320Ptr();
    VOS_INT32         swCntI, swSum;

    VOS_INT32         swIirLen            = pstHpf->stFiltPara.swIirLen;
    VOS_INT16        *pshwNum             = pstHpf->stFiltPara.pshwNum;
    VOS_INT16        *pshwDen             = pstHpf->stFiltPara.pshwDen;

    /* �ж�HPFģ���Ƿ�ʹ�� */
    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        /* HPF��ʹ�� */
        CODEC_OpVecCpy(pshwOut, pshwIn, pstHpf->shwFrmLen);
        return UCOM_RET_SUCC;
    }

    /* ��ֹpshwIn��pshwOutָ���ڴ��ص������˲����� */
    CODEC_OpVecCpy(&ashwInTmp[0], pshwIn, pstHpf->shwFrmLen);

    swSum = CODEC_OpL_shr(CODEC_OpL_mult0(pshwNum[0], ashwInTmp[0]), 1);
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[2], pshwNum[1]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[2], pshwDen[1]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[1], pshwNum[2]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[1], pshwDen[2]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[0], pshwNum[3]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[0], pshwDen[3]));
    pswOutTemp[0] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);

    swSum = CODEC_OpL_shr(CODEC_OpL_mult0(pshwNum[0], ashwInTmp[1]),1);
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[0], pshwNum[1]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[0], pshwDen[1]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[2], pshwNum[2]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[2], pshwDen[2]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[1], pshwNum[3]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[1], pshwDen[3]));
    pswOutTemp[1] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);

    swSum = CODEC_OpL_shr(CODEC_OpL_mult0(pshwNum[0], ashwInTmp[2]),1);
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[1], pshwNum[1]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[1], pshwDen[1]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[0], pshwNum[2]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[0], pshwDen[2]));
    swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(pstHpf->ashwPreIn[2], pshwNum[3]),1));
    swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pstHpf->aswPreOut[2], pshwDen[3]));
    pswOutTemp[2] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);

    for (swCntI = swIirLen - 1; swCntI < pstHpf->shwFrmLen; swCntI++)
    {
        swSum = CODEC_OpL_shr(CODEC_OpL_mult0(pshwNum[0], ashwInTmp[swCntI]),1);

        swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[swCntI - 1], pshwNum[1]),1));
        swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[swCntI - 1], pshwDen[1]));

        swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[swCntI - 2], pshwNum[2]),1));
        swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[swCntI - 2], pshwDen[2]));

        swSum = CODEC_OpL_add(swSum,  CODEC_OpL_shr(CODEC_OpL_mult0(ashwInTmp[swCntI - 3], pshwNum[3]),1));
        swSum = CODEC_OpL_sub(swSum,  CODEC_OpL_mpy_32_16(pswOutTemp[swCntI - 3], pshwDen[3]));

        pswOutTemp[swCntI] = CODEC_OpL_shl(swSum, MED_FILT_NORM_MAX - pshwDen[0]);
    }

    CODEC_OpVecCpy(&pstHpf->ashwPreIn[0], &(ashwInTmp[pstHpf->shwFrmLen - MED_HPF_HISTORY_BUFF_LEN]), MED_HPF_HISTORY_BUFF_LEN);
    CODEC_OpVecCpy((VOS_INT16 *)&pstHpf->aswPreOut[0], (VOS_INT16 *)&(pswOutTemp[pstHpf->shwFrmLen - MED_HPF_HISTORY_BUFF_LEN]), MED_HPF_HISTORY_BUFF_LEN*2);

    for (swCntI = 0; swCntI < pstHpf->shwFrmLen; swCntI++)
    {
        pshwOut[swCntI] = CODEC_OpSaturate(CODEC_OpL_shr(pswOutTemp[swCntI], MED_FILT_NORM_MAX-1));
    }

    return UCOM_RET_SUCC;

}
#endif


VOS_VOID* MED_COMP_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_COMP_GetObjInfoPtr,
                    MED_COMP_GetObjPtr,
                    MED_COMP_MAX_OBJ_NUM,
                    sizeof(MED_COMP_OBJ_STRU)));
}


VOS_UINT32 MED_COMP_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_COMP_OBJ_STRU      *pstObj;
    pstObj      = (MED_COMP_OBJ_STRU *)(*ppstObj);

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_COMP_GetObjInfoPtr, pstObj);

    /* ��κϷ�����ָ������ͷţ�����ָ�븳�� */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_COMP_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_COMP_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32           uwRet;
    MED_COMP_OBJ_STRU   *pstComp = (MED_COMP_OBJ_STRU *)pstInstance;
    MED_COMP_NV_STRU    *pstNv   = &(pstComp->stNv);
    VOS_INT16             shwHalfLen;

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_COMP_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if (UCOM_RET_SUCC == uwRet)
    {
        shwHalfLen = (pstNv->shwCoefLen+1)>>1;

        /* BUFFER�ռ䲻������ṹ���ʹ�����ȷ���������ֹ���� */
        if (    (shwParaLen < (MED_COMP_PARAM_VEC_BASE + shwHalfLen))
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ����˲�ϵ������Ϣ */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0]    = (VOS_INT16)(pstNv->enEnable);                             /* ʹ���� */
            pshwParam[1]    = pstNv->shwCompGain;                                       /* ʩ������(����)*/
            pshwParam[2]    = shwHalfLen;                                        /* �˲�ϵ������ */


            CODEC_OpVecCpy(  &(pshwParam[MED_COMP_PARAM_VEC_BASE]),                       /* �˲�ϵ�� */
                           &(pstNv->ashwCompCoef[0]),
                             shwHalfLen);
        }
    }

    return uwRet;

}



VOS_UINT32 MED_COMP_SetPara (
                VOS_UINT32              uwSampleRate,
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32           uwRet;
    MED_COMP_OBJ_STRU   *pstComp   = (MED_COMP_OBJ_STRU *)pstInstance;
    MED_COMP_NV_STRU    *pstNv     = &(pstComp->stNv);
    VOS_INT16           *pshwCoef;
    VOS_INT32            swCnt;                                                 /* ������ */
    VOS_INT16            shwStride;

    /* �ж�����Ƿ�Ϸ� */
    uwRet       = MED_PP_CheckPtrValid(MED_COMP_GetObjInfoPtr, pstInstance);

    /* ��κϷ������Զ�ָ������λ�ý������������ò��� */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER�ռ䲻������ʹ�����ȷ���򳤶Ȳ�����Ҫ���������ֹ���� */
        if (    (shwParaLen < (pshwParam[2] + MED_COMP_PARAM_VEC_BASE))
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT)
            ||  (pshwParam[2] > MED_COMP_FILT_LEN_MAX))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ��ֵʹ����˲�ϵ������Ϣ */
            pstNv->enEnable     = (VOS_UINT16)pshwParam[0];                                       /* ʹ���� */
            pstNv->shwCompGain  = pshwParam[1];                                         /* ʩ������(����)*/
            pstNv->shwCoefLen   = (VOS_INT16)((2*pshwParam[2]) - 1);                                /* �˲����� */
            CODEC_OpVecSet(&(pstNv->ashwCompCoef[0]), (2 * MED_COMP_FILT_LEN_MAX) - 1, 0);
            CODEC_OpVecCpy(  &(pstNv->ashwCompCoef[0]),                                   /* �˲�ϵ�� */
                           &(pshwParam[3]),
                             pshwParam[2]);

            /* �˲���ϵ���ǶԳƵģ���������ֻ����ǰ�벿��: pshwParam[3], ..., pshwParam[3+x-1] */
            /* ���ԳƵĺ�벿�֣���pstComp->ashwCompCoef[pstComp->shwCoefLen], ... */
            /* ��ӦpshwParam[3+x-2], ..., pshwParam[3] */
            //pshwCoef              = &(pshwParam[MED_COMP_PARAM_VEC_BASE]);
            //CODEC_OpVecCpy(&(pstNv->ashwCompCoef[0]), pshwCoef, MED_COMP_FILT_LEN_MAX);
            pshwCoef              = &(pstNv->ashwCompCoef[0]);
            shwStride             = pshwParam[2] - 1;

            /* �˲����ǶԳƵ� */
            for (swCnt = 1; swCnt <= shwStride; swCnt++)
            {
                pshwCoef[shwStride + swCnt] = pshwCoef[shwStride - swCnt];
            }

            /* ��ȡ֡����Ϣ */
            if (MED_SAMPLE_48K == uwSampleRate)
            {
                pstComp->shwFrmLen  = AUDIO_PP_GetFrameLength();
            }
            else
            {
                pstComp->shwFrmLen  = MED_PP_GetFrameLength();
            }

            /* �����źŵ���ʷ��Ϣ��ʼ�� */
            CODEC_OpVecSet(&(pstComp->ashwPreIn[0]), MED_COMP_PRE_LEN_MAX, 0);
        }
    }

    return uwRet;

}
VOS_UINT32 MED_COMP_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{
    /* ��ʱCOMPָ�� */
    MED_COMP_OBJ_STRU *pstComp;
    MED_COMP_NV_STRU  *pstNv;
    VOS_INT16         *pshwInTemp;

    pstComp = (MED_COMP_OBJ_STRU*)pstInstance;
    pstNv   = &(pstComp->stNv);                                                 /* Ƶ��NV�� */


    /* �ж�COMPģ���Ƿ�ʹ�� */
    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        /* COMP��ʹ�� */
        CODEC_OpVecCpy(pshwOut, pshwIn, pstComp->shwFrmLen);
        return UCOM_RET_SUCC;
    }

    /* �����ڴ� */
    pshwInTemp     = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)(CODEC_FRAME_LENGTH_MU + MED_COMP_PRE_LEN_MAX) * sizeof(VOS_INT16));

    /* ��16bit��������ʷ��Ϣ�͵�ǰ����֡�ϲ� */
    CODEC_OpVecCpy(&pshwInTemp[0], &(pstComp->ashwPreIn[0]), pstNv->shwCoefLen - 1);
    CODEC_OpVecCpy(&pshwInTemp[pstNv->shwCoefLen - 1], pshwIn, pstComp->shwFrmLen);

    /* FIR�˲� */
    MED_FILT_Fir( &pshwInTemp[0],
                  &(pstNv->ashwCompCoef[0]),
                    pstComp->shwFrmLen,
                    pstNv->shwCoefLen,
                    pstNv->shwCompGain,
                    pshwOut);

    /* ���������źŵ����pstNv->shwCoefLen - 1���� */
    CODEC_OpVecCpy( &(pstComp->ashwPreIn[0]),
                    &pshwInTemp[pstComp->shwFrmLen],
                     pstNv->shwCoefLen - 1);
    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwInTemp);

    return UCOM_RET_SUCC;
}

#if 0
VOS_UINT32 MED_AUDIO_COMP_Destroy(VOS_VOID *pstInstance)
{
    PP_AUDIO_PCM_COMP_OBJ_STRU *pstObj  = (PP_AUDIO_PCM_COMP_OBJ_STRU *)(pstInstance);
    VOS_INT16                   shwCnt;

    /* ��κϷ�����ָ��ָ��������ͷţ�Ȼ�󽫶����ͷţ�����ָ�븳�� */
    if (VOS_NULL == pstObj)
    {
        return UCOM_RET_NUL_PTR;
    }
    else
    {

        if (VOS_NULL != pstObj->pshwCoef)
        {
            /* �ͷŸ��ڴ� */
            UCOM_MemFree(pstObj->pshwCoef);
        }

        for (shwCnt = 0;  shwCnt < MED_COMP_CHANNEL_MAX; shwCnt++)
        {
            /* ��ָ��ָ�����ʱ������ڴ�free */
            if (VOS_NULL != pstObj->pashwIn[shwCnt])
            {
                /* �ͷŸ��ڴ� */
                UCOM_MemFree(pstObj->pashwIn[shwCnt]);
            }

            if (VOS_NULL == pstObj->pashwPreInBuff[shwCnt])
            {
                /* �ͷŸ��ڴ� */
                UCOM_MemFree(pstObj->pashwPreInBuff[shwCnt]);
            }
        }

        /* ��������ṹ�壬����ָ�룬״̬�� */
        UCOM_MemSet(pstObj, 0, sizeof(PP_AUDIO_PCM_COMP_OBJ_STRU));
    }

    return UCOM_RET_SUCC;

}


VOS_UINT32 MED_AUDIO_COMP_GetPara(
                VOS_VOID                   *pstInstance,
                PP_AUDIO_PCM_COMP_NV_STRU  *pstNv)
{
    PP_AUDIO_PCM_COMP_OBJ_STRU *pstComp = (PP_AUDIO_PCM_COMP_OBJ_STRU *)pstInstance;

    if ((VOS_NULL == pstComp) || (VOS_NULL == pstNv))
    {
        return UCOM_RET_NUL_PTR;
    }
    else
    {
        /* ��β��Ϸ� */
        if (    (pstComp->enPcmCompEnable >= CODEC_SWITCH_BUTT)
             || (pstComp->shwM            <= 0)
             || (pstComp->shwM            >  AUDIO_PCM_COMP_COEF_LEN_MAX))
        {
            return UCOM_RET_FAIL;
        }
        else
        {
            /* ��ȡʹ����˲�ϵ������Ϣ */
            UCOM_MemSet(pstNv, 0, sizeof(PP_AUDIO_PCM_COMP_NV_STRU));
            pstNv->enPcmCompEnable  = pstComp->enPcmCompEnable;
            pstNv->shwM             = pstComp->shwM;
            pstNv->shwCompGain      = pstComp->shwCompGain;

            /* �����˲�ϵ����ע pstNv->shwReservedΪ���������Ҫ��ȡ */
            CODEC_OpVecCpy(&(pstNv->ashwCoef[0]),
                           &(pstComp->pshwCoef[0]),
                            (VOS_INT32)pstNv->shwM);
        }
        return UCOM_RET_SUCC;
    }

}

VOS_UINT32 MED_AUDIO_COMP_CreateAndSetPara (
                VOS_VOID                   *pstInstance,
                PP_AUDIO_PCM_COMP_NV_STRU  *pstNv)
{
    PP_AUDIO_PCM_COMP_OBJ_STRU   *pstCfg      =  (PP_AUDIO_PCM_COMP_OBJ_STRU*)pstInstance;

    /* ��κϷ������Զ�ָ������λ�ý����������ȡ���� */
    if ((VOS_NULL == pstNv) || (VOS_NULL == pstInstance))
    {
        return UCOM_RET_NUL_PTR;
    }
    else
    {
        UCOM_MemSet(pstCfg, 0, sizeof(PP_AUDIO_PCM_COMP_OBJ_STRU));

        /* ��ֵʹ����˲������ȡ����� */
        pstCfg->enPcmCompEnable = pstNv->enPcmCompEnable;
        pstCfg->shwM            = pstNv->shwM;
        pstCfg->shwCompGain     = pstNv->shwCompGain;

        /* ����: pstCfg->shwN = 0 */

        /* �жϸղŸ�ֵ��ĺϷ��� */
        if (   (pstCfg->enPcmCompEnable >= CODEC_SWITCH_BUTT)
            || (pstCfg->shwM            >  AUDIO_PCM_COMP_COEF_LEN_MAX)
            || (pstCfg->shwM            <= 0))
        {
            return UCOM_RET_FAIL;
        }

        /* ���ݵõ����˲�ϵ���ĳ��ȣ�������Ӧ���ڴ棬����ֵ��NV��ص����� */
        pstCfg->pshwCoef = (VOS_INT16*)UCOM_MemAlloc((VOS_UINT16)pstCfg->shwM * sizeof(VOS_INT16));
        CODEC_OpVecCpy(pstCfg->pshwCoef, pstNv->ashwCoef, pstNv->shwM);
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 MED_AUDIO_COMP_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16               shwInLen,
                VOS_INT16              *pshwOut,
                VOS_UINT32              uwChanNum)
{
    PP_AUDIO_PCM_COMP_OBJ_STRU *pstComp;
    VOS_INT16                  *pshwXaIn0;
    VOS_INT16                  *pshwXaPreIn0;
    VOS_INT16                  *pshwXaIn1;
    VOS_INT16                  *pshwXaPreIn1;
    VOS_INT16                  *pshwXaOut0;
    VOS_INT16                  *pshwXaOut1;
    VOS_INT16                   shwCurrLen;

    /* ��κϷ����ж� */
    if (   (VOS_NULL == pshwIn)
        || (VOS_NULL == pshwOut)
        || (VOS_NULL == pstInstance)
        || (uwChanNum != MED_COMP_CHANNEL_STEREO))
    {
        return UCOM_RET_FAIL;
    }

    /* ��ʱAUDIO COMPָ�� */
    pstComp = (PP_AUDIO_PCM_COMP_OBJ_STRU*)pstInstance;

    /* �ж�COMPģ���Ƿ�ʹ�� */
    if (CODEC_SWITCH_OFF == pstComp->enPcmCompEnable)
    {
        /* ͸������ */
        CODEC_OpVecCpy(pshwOut, pshwIn, shwInLen);
        return UCOM_RET_SUCC;
    }

    pshwXaIn0    = pstComp->pashwIn[0];
    pshwXaPreIn0 = pstComp->pashwPreInBuff[0];
    pshwXaIn1    = pstComp->pashwIn[1];
    pshwXaPreIn1 = pstComp->pashwPreInBuff[1];
    pshwXaOut0   = pstComp->pashwOut[0];
    pshwXaOut1   = pstComp->pashwOut[1];
    shwCurrLen   = shwInLen / MED_COMP_CHANNEL_STEREO;

    /* ��������PCM�����ĳ��ȣ������û�������ڴ棬�������볤���б䣬���������ڴ���� */
    if (   (shwCurrLen != pstComp->shwN)
        || (VOS_NULL == pshwXaIn0)
        || (VOS_NULL == pshwXaPreIn0)
        || (VOS_NULL == pshwXaIn1)
        || (VOS_NULL == pshwXaPreIn1)
        || (VOS_NULL == pshwXaOut0)
        || (VOS_NULL == pshwXaOut1))
    {

        if (0 != pstComp->shwN)
        {
            /* �ͷŸ��ڴ� */
            UCOM_MemFree(pshwXaIn0);
            UCOM_MemFree(pshwXaPreIn0);
            UCOM_MemFree(pshwXaOut0);
            UCOM_MemFree(pshwXaIn1);
            UCOM_MemFree(pshwXaPreIn1);
            UCOM_MemFree(pshwXaOut1);
        }

        /* ����pstComp->shwN */
        pstComp->shwN   = shwCurrLen;

        /* �����ڴ� */
        pshwXaIn0    = (VOS_INT16*)UCOM_MemAlloc((VOS_UINT16)((pstComp->shwN + pstComp->shwM) - 1) * sizeof(VOS_INT16));
        pshwXaIn1    = (VOS_INT16*)UCOM_MemAlloc((VOS_UINT16)((pstComp->shwN + pstComp->shwM) - 1) * sizeof(VOS_INT16));
        pshwXaPreIn0 = (VOS_INT16*)UCOM_MemAlloc((VOS_UINT16)(pstComp->shwM - 1) * sizeof(VOS_INT16));
        pshwXaPreIn1 = (VOS_INT16*)UCOM_MemAlloc((VOS_UINT16)(pstComp->shwM - 1) * sizeof(VOS_INT16));
        pshwXaOut0   = (VOS_INT16*)UCOM_MemAlloc((VOS_UINT16)(pstComp->shwN) * sizeof(VOS_INT16));
        pshwXaOut1   = (VOS_INT16*)UCOM_MemAlloc((VOS_UINT16)(pstComp->shwN) * sizeof(VOS_INT16));

        /* ��������ڴ��������� */
        UCOM_MemSet(pshwXaIn0,    0, ((VOS_UINT16)(pstComp->shwN + pstComp->shwM) - 1) * sizeof(VOS_INT16));
        UCOM_MemSet(pshwXaPreIn0, 0, (VOS_UINT16)(pstComp->shwM - 1) * sizeof(VOS_INT16));
        UCOM_MemSet(pshwXaIn1,    0, ((VOS_UINT16)(pstComp->shwN + pstComp->shwM) - 1) * sizeof(VOS_INT16));
        UCOM_MemSet(pshwXaPreIn1, 0, (VOS_UINT16)(pstComp->shwM - 1) * sizeof(VOS_INT16));
        UCOM_MemSet(pshwXaOut0,   0, (VOS_UINT16)(pstComp->shwN) * sizeof(VOS_INT16));
        UCOM_MemSet(pshwXaOut1,   0, (VOS_UINT16)(pstComp->shwN) * sizeof(VOS_INT16));

        /* ��������ڴ汣�浽�ṹ��pstComp��ȥ */
        pstComp->pashwIn[0]        = pshwXaIn0;
        pstComp->pashwPreInBuff[0] = pshwXaPreIn0;
        pstComp->pashwIn[1]        = pshwXaIn1;
        pstComp->pashwPreInBuff[1] = pshwXaPreIn1;
        pstComp->pashwOut[0]       = pshwXaOut0;
        pstComp->pashwOut[1]       = pshwXaOut1;

    }

    /* ��16bit��������ʷ��Ϣ�͵�ǰ����֡�ϲ�����ǰ֡������ͨ���⽻֯��ֻ�� */
    CODEC_OpVecCpy(pshwXaIn0, pshwXaPreIn0, pstComp->shwM - 1);
    CODEC_OpVecCpy(pshwXaIn1, pshwXaPreIn1, pstComp->shwM - 1);
    MED_AUDIO_COMP_DataDeinterleave( pshwIn,
                                    pstComp->shwN,
                                   &pshwXaIn0[pstComp->shwM - 1],
                                   &pshwXaIn1[pstComp->shwM - 1]);

#if 1


    /* FIR�˲� */
    MED_FILT_Fir(  pshwXaIn0,
                  pstComp->pshwCoef,
                  pstComp->shwN,
                  pstComp->shwM,
                  pstComp->shwCompGain,
                  pshwXaOut0);

    MED_FILT_Fir(  pshwXaIn1,
                  pstComp->pshwCoef,
                  pstComp->shwN,
                  pstComp->shwM,
                  pstComp->shwCompGain,
                  pshwXaOut1);

    /* ���������źŵ����M-1���� */
    CODEC_OpVecCpy( &(pshwXaPreIn0[0]),
                    &(pshwXaIn0[pstComp->shwN]),
                     pstComp->shwM - 1);

    CODEC_OpVecCpy( &(pshwXaPreIn1[0]),
                    &(pshwXaIn1[pstComp->shwN]),
                     pstComp->shwM - 1);

    /* ������ս�� */
    MED_AUDIO_COMP_DataInterleave( pshwXaOut0,
                                  pshwXaOut1,
                                  pstComp->shwN,
                                  pshwOut);
#else
    MED_AUDIO_COMP_DataInterleave(&pshwXaIn0[pstComp->shwM - 1],
                                  &pshwXaIn1[pstComp->shwM - 1],
                                  pstComp->shwN,
                                  pshwOut);
#endif

    return UCOM_RET_SUCC;
}


VOS_VOID MED_AUDIO_COMP_DataDeinterleave(VOS_VOID *pvIn, VOS_INT16 shwInSize, VOS_VOID *pvOut0, VOS_VOID *pvOut1)
{
    VOS_INT16                           shwLen;                                 /* ���ȣ���λ16bit */
    VOS_INT16                           shwDataCnt;

    VOS_INT16                          *pshwIn              = (VOS_INT16*)pvIn;
    VOS_INT16                          *pshwOut0            = (VOS_INT16*)pvOut0;
    VOS_INT16                          *pshwOut1            = (VOS_INT16*)pvOut1;

    shwLen          = shwInSize;

    for (shwDataCnt = 0; shwDataCnt < shwLen; shwDataCnt++)
    {
        pshwOut0[shwDataCnt] = *pshwIn;
        pshwIn++;

        pshwOut1[shwDataCnt] = *pshwIn;
        pshwIn++;
    }

    return;
}
VOS_VOID MED_AUDIO_COMP_DataInterleave(VOS_VOID *pvIn0, VOS_VOID *pvIn1, VOS_INT16 shwInSize, VOS_VOID *pvOut)
{
    VOS_INT16                           shwLen;                                 /* ���ȣ���λ16bit */
    VOS_INT16                           shwDataCnt;

    VOS_INT16                          *pshwIn0             = (VOS_INT16*)pvIn0;
    VOS_INT16                          *pshwIn1             = (VOS_INT16*)pvIn1;
    VOS_INT16                          *pshwOut             = (VOS_INT16*)pvOut;

    shwLen          = shwInSize;

    for (shwDataCnt = 0; shwDataCnt < shwLen; shwDataCnt++)
    {
        *pshwOut = pshwIn0[shwDataCnt];
        pshwOut++;

        *pshwOut = pshwIn1[shwDataCnt];
        pshwOut++;
    }

    return;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


