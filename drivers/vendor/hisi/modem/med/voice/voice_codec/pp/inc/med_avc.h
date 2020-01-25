

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "med_pp_comm.h"

#ifndef __MED_AVC_H__
#define __MED_AVC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* ������Ϣ */
#define MED_AVC_MAX_OBJ_NUM             (1)                                     /* AVC���ʵ����Ŀ */
#define MED_AVC_PARAM_LEN               (9)                                     /* AVC��Ч�Ĳ������� */
#define MED_AVC_BUF_LEN                 (6)                                     /* �����VAD��Ϣ���� */
#define MED_AVC_NOISE_UPDATE_MAX_VOICE_ACTIVE (1)                               /* ��������ʱ���������������״̬�� */
#define MED_AVC_NOISE_UPDATE_ALPHA      (31456)                                 /* ��������ƽ��ϵ�� */
#define MED_AVC_NOISE_STEPS_NUM         (4)                                     /* ���������� */
#define MED_AVC_GAIN_UPDATE_ALPHA       (31456)                                 /* ����ƽ��ϵ�� */
#define MED_AVC_ANTI_SAT_ALPHA          (2949)                                  /* �����ʹ���ƽ��ϵ�� */
#define MED_AVC_GAIN_INITIAL            (2048)                                  /* �����ʼ����ֵ */
#define MED_AVC_NOISE_LVL_INITIAL       (100)                                   /* ���Ƶ��������� */
#define MED_AVC_RX_DEV_GAIN_MIN         (4096)                                  /* �����豸������Сֵ */
#define MED_AVC_GAIN_MIN                (2048)                                  /* ������Сֵ */

/* ��ȡȫ�ֱ��� */
#define MED_AVC_GetNoiseStepLag(shwIdx) (g_ashwMedAvcStepLag[shwIdx])           /* ���������ּ����� */
#define MED_AVC_GetGainFromTab(shwIdx)  (g_ashwMedAvcGainTab[shwIdx])           /* ������������������ձ��е�ֵ */
#define MED_AVC_GetObjPtr               (&g_astMedAvcObjPool[0])                /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_AVC_GetObjInfoPtr           (&g_stMedAvcObjInfo)                    /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */

/* ��������ת���� */
#define MED_AVC_Db2Linear(shwIdx)       MED_GAIN_Db2Linear(shwIdx)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/*****************************************************************************
 ʵ������  : MED_AVC_FFT_LEN_ENUM
 ��������  : AVC FFT����ö�ٶ���
*****************************************************************************/
enum MED_AVC_FFT_LEN_ENUM
{
    MED_AVC_FFT_LEN_8K                  = 256,
    MED_AVC_FFT_LEN_2MIC_8K             = 320,
    MED_AVC_FFT_LEN_16K                 = 512,
    MED_AVC_FFT_LEN_2MIC_16K            = 640,
    MED_AVC_FFT_LEN_BUTT
};
typedef VOS_UINT16 MED_AVC_FFT_LEN_ENUM_UINT16;

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
ʵ������  : MED_AVC_NV_STRU
��������  : AVC NV��ά���ṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16              enEnable;                               /* ������ ʹ�ܿ��� */
    VOS_INT16                           shwGainMaxInc;                          /* ������ ������� Q14 */
    VOS_INT16                           shwGainMaxDec;                          /* ������ ����½� Q14 */
    VOS_INT16                           shwSensitivityOffset;                   /* ������ ������˷�������ƫ�� */
    VOS_INT16                           shwAntiSatMaxLevel;                     /* ������ �����ʹ���������� */
    VOS_INT16                           ashwNoiseStep[MED_AVC_NOISE_STEPS_NUM]; /* ������ ���������� */
    VOS_INT16                           shwReserved;
}MED_AVC_NV_STRU;

/*****************************************************************************
ʵ������  : MED_AVC_STRU
��������  : AVCά���ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_AVC_NV_STRU                     stNv;                                   /* NV�� */

    /* ����������� */
    VOS_INT32                           swNoiseLvl;                             /* ��ǰ�������� */
    VOS_INT16                          *pshwWeightTab;                          /* ������Ȩ��ָ�� */
    VOS_INT16                           shwFrmLen;                              /* ֡�� */
    VOS_INT16                           shwReserved2;

    /* �����ּ�,Q0 */
    VOS_INT32                           aswMedAvcStep[MED_AVC_NOISE_STEPS_NUM];

    /* ��ʷ��Ϣ����ά�� */
    CODEC_VAD_FLAG_ENUM_INT16        aenVadFlag[MED_AVC_BUF_LEN];            /* VAD��ʷ��Ϣ�Ļ��� */
    VOS_INT32                           aswFrmLvl[MED_AVC_BUF_LEN];             /* ��˷�ƫ���������֡�����Ļ��� */

    /* ��һ֡��Ϣά��*/
    VOS_INT16                           shwLastStep;                            /* Ŀ�����漶�� */
    VOS_INT16                           shwLastGain;                            /* ���棬Q11 */

    /* ������Ϣά�� */
    VOS_INT32                           swTxFrmLvl;                             /* ����֡�źŵ����� */
    CODEC_VAD_FLAG_ENUM_INT16        enVadFlag;                              /* ����֡��VAD��Ϣ */
    MED_AVC_FFT_LEN_ENUM_UINT16         enFreqLen;                              /* ����֡Ƶ�򳤶� */
} MED_AVC_OBJ_STRU;


/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern VOS_INT16 g_ashwMedAvcWeightTab8k[];
extern VOS_INT16 g_ashwMedAvcWeightTab16k[];

/*****************************************************************************
  10 ��������
*****************************************************************************/

extern VOS_VOID* MED_AVC_Create(VOS_VOID);
extern VOS_UINT32 MED_AVC_Destroy(VOS_VOID **ppstObj);
extern VOS_VOID MED_AVC_EstBgNoise(MED_AVC_OBJ_STRU *pstAvc);
extern VOS_VOID MED_AVC_GainRxVol(
                       MED_AVC_OBJ_STRU       *pstAvc,
                       VOS_INT16              *pshwRxFrm,
                       VOS_INT16               shwRxDevGain);
extern VOS_UINT32 MED_AVC_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_AVC_Main(
                       VOS_VOID                *pstMedAvcObj,
                       VOS_INT16               *pshwRxFrm,
                       VOS_INT16                shwRxDevGain);
extern VOS_UINT32 MED_AVC_SetPara (
                       VOS_VOID                         *pstInstance,
                       VOS_INT16                        *pshwParam,
                       VOS_INT16                        shwParaLen,
                       MED_PP_MIC_NUM_ENUM_UINT16       enPpMicNum);
extern VOS_VOID MED_AVC_UpdateTx(
                       MED_AVC_OBJ_STRU       *pstAvc,
                       VOS_INT16              *pshwTxFreq,
                       VOS_INT16               enVadFlag);











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_avc.h */

