
#ifndef __MED_AIG_H__
#define __MED_AIG_H__


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "med_pp_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* ������Ϣ */
#define MED_AIG_MAX_OBJ_NUM             (2)                                     /* AIG���ʵ����Ŀ */
#define MED_AIG_PARAM_LEN               (21)                                    /* ��Ч����������� */
#define MED_AIG_RX_0_DBFS               (32767)
#define MED_AIG_ALPHA_NORM              (128)
#define MED_AIG_SLOP_NORM               (128)
#define MED_AIG_GAIN_30DB_NORM          (7706)

/* ��ȡȫ�ֱ��� */
#define MED_AIG_GetObjPtr               (&g_astMedAigObjPool[0])                /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_AIG_GetObjInfoPtr           (&g_stMedAigObjInfo)                    /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  6 STRUCT����
*****************************************************************************/
/*****************************************************************************
ʵ������  : MED_AIG_NV_STRU
��������  : AIG NV��ά���ṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* ������ ʹ���� */
    VOS_INT16                           shwDnCompThr;                       /* ��ѹ�������ޣ����ڴ�����Ϊ��ѹ����������λΪdBfs */
    VOS_INT16                           shwUpCompThr;                       /* ��ѹ�������ޣ�С�ڴ�����Ϊ��ѹ����������λΪdBfs */
    VOS_INT16                           shwDnExpThr;                        /* ����չ�����ޣ�С�ڴ�����Ϊ����չ��������λΪdBfs */
    VOS_INT16                           shwDnCompSlop;                      /* ��ѹ������������б�� */
    VOS_INT16                           shwUpCompSlop;                      /* ��ѹ������������б�� */
    VOS_INT16                           shwDnExpSlop;                       /* ����չ����������б�� */
    VOS_INT16                           shwMindB;                           /* ��СdBֵ */
    VOS_INT16                           shwGainAlphaattack;
    VOS_INT16                           shwGainAlpharelease;                /* ����alpha�˲�ϵ�� */
    VOS_INT16                           shwAlphaThd;                        /* alpha����ֵ */
    VOS_INT16                           shwLastPowerIndBfsInit;             /*��һ֡���빦�ʳ�ʼֵ */
    VOS_INT16                           shwLastPowerOutdBfsInit;            /*��һ֡������ʳ�ʼֵ */
    VOS_INT16                           shwGainMindB;                       /* ������СdBֵ */
    VOS_INT16                           shwLastGainInit;                    /*��һ֡�����ʼֵ */
    VOS_INT16                           shwMaxIn;                           /* �������ֵ */
    VOS_INT16                           shwExpectMax;                       /* �������ֵ */
    VOS_INT16                           shwMaxGainDelta;                    /* �������Deltaϵ�� */
    VOS_INT16                           shwSpeechCntInit;
    VOS_INT16                           shwVADholdlenth;
    VOS_INT16                           shwMakeupGain;
    VOS_INT16                           shwReserved;
}MED_AIG_NV_STRU;

/*****************************************************************************
ʵ������  : MED_AIG_STRU
��������  : AIG��ά���ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_AIG_NV_STRU                     stNv;                                   /* NV��ṹ�� */
    VOS_INT16                           shwFrmLen;                              /* ֡�ĳ��� */
    VOS_INT16                           shwGainMaxRefThd;                       /* �������ο���ֵ */
    VOS_INT16                           shwGainMax;                             /* �������ֵ */
    VOS_INT16                           shwMaxIn;                               /* �������ֵ */
    VOS_INT16                           shwSpeechCnt;
    VOS_INT16                           shwLastPowerIndBfs;                     /* ��һ֡���빦��dBֵ */
    VOS_INT16                           shwLastGain;                            /* ��һ֡���� */
    VOS_INT16                           shwLastGaindB;                          /* ��һ֡���� */
    VOS_INT16                           shwLastPowerOutdBfs;                    /* ��һ֡�������dBֵ */
    VOS_INT16                           swdBfs0;                                /* DRC��RMS�������ߺ�����ź��޷����޵�0dBfs */
    VOS_INT16                           shwMakeupGain;
}MED_AIG_OBJ_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern MED_AIG_OBJ_STRU    g_astMedAigObjPool[MED_AIG_MAX_OBJ_NUM];             /* AIGʵ����Դ�� */
extern MED_OBJ_INFO        g_stMedAigObjInfo;                                   /* AIG��Ϣ */

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_VOID* MED_AIG_Create(MED_VOID);
extern VOS_UINT32 MED_AIG_Destroy(MED_VOID **ppstObj);
extern VOS_UINT32 MED_AIG_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_AIG_SetPara (
                MED_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen,
                VOS_INT32               swSampleRate);
extern VOS_UINT32 MED_AIG_Main(
                 MED_AIG_OBJ_STRU    *pstAig,
                 VOS_INT16           *pshwIn,
                 VOS_INT16           *pshwRef,
                 VOS_INT16           *pshwOut,
                 VOS_INT16            shwVadFlag);
extern VOS_VOID MED_AIG_PowerCal(
                VOS_INT16               *pshwIn,
                VOS_INT16               *pshwPowerRMS,
                VOS_INT16               *pshwNormShift,
                VOS_INT16                shwFrameLen);
extern VOS_VOID MED_AIG_GainCal(
                MED_AIG_OBJ_STRU        *pstAig,
                VOS_INT16                shwPowerRMS,
                VOS_INT16                shwNormShift,
                VOS_INT16                shwPowerRMSRef,
                VOS_INT16                shwNormShiftRef,
                VOS_INT16               *pshwGainAIG,
                VOS_INT16               *pshwAlpha);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* med_agc.h */
