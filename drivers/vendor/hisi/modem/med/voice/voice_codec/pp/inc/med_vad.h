

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "med_pp_comm.h"
#include "codec_op_float.h"

#ifndef __MED_VAD_H__
#define __MED_VAD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* ������Ϣ */
#define MED_VAD_MAX_OBJ_NUM             (2)                                     /* VAD���ʵ����Ŀ */
#define MED_VAD_PARAM_LEN               (3)                                     /* VAD������ĸ��� */
#define MED_VAD_FRQ_BAND_MAX            (256)                                   /* ���Ƶ���� */
#define MED_VAD_STARTING_FRM_LEN        (5)                                     /* ���޿�ʼ����Ӧ���µ���ʼ֡�� */
#define MED_VAD_ARTI_VOICE_DETECT_AMP_THD (30)                                  /* �˹������о�ʹ�ܵķ���������� */
#define MED_VAD_ARTI_SPCH_FREQ_BAND_MIN (9)                                     /* ��СƵ�� */
#define MED_VAD_ARTI_SPCH_FREQ_BAND_DIFF (5)                                    /* Ƶ���� */
#define MED_VAD_SMOOTH_FRM_AMP_ALPHA    (22937)                                 /* ��Ȩϵ����0.7 Q15 */
#define MED_VAD_CURR_FRM_ALPHA          (9830)                                  /* ��Ȩϵ����0.3 Q15 */
#define MED_VAD_HALFFFT_8K              (128)                                   /* FFT���ȵ�һ�룬��ֵ�ϵ��� CODEC_OpShr(stSV.shwFftLen, 1); */
#define MED_VAD_HALFFFT_SHIFT_8K        (7)                                     /* FFT����һ���ö����Ʊ�ʾ����ռ��λ�� floor(log2(double(stSV.shwHalfFftLen))); */
#define MED_VAD_MAX_FREQ_ENG_8K         ((VOS_INT32)16777215)                   /* ���Ƶ���������ޣ���ֵ�ϵ��� CODEC_OpNormDiv_32(1, stSV.shwHalfFftLen, 31); */
#define MED_VAD_MAX_STD_DEVI_THD_8K     (4096)                                  /* ���Ƶ���������������ޣ���ֵ�ϵ��� CODEC_OpSqrt(stSV.swMaxFreqEngThd); */
#define MED_VAD_FREQ_ENG_NORM_PARA_8K   (7)                                     /* Ƶ�������������һ����������ֵ�ϵ��� CODEC_OpNorm_l(stSV.swMaxFreqEngThd); */
#define MED_VAD_STD_DEVI_NORM_PARA_8K   (18)                                    /* Ƶ�������������һ����������ֵ�ϵ��� CODEC_OpNorm_l(stSV.swMaxStandardDeviThd); */
#define MED_VAD_HALFFFT_16K             (256)                                   /* FFT���ȵ�һ�룬��ֵ�ϵ��� CODEC_OpShr(stSV.shwFftLen, 1); */
#define MED_VAD_HALFFFT_SHIFT_16K       (8)                                     /* FFT����һ���ö����Ʊ�ʾ����ռ��λ�� floor(log2(double(stSV.shwHalfFftLen))); */
#define MED_VAD_MAX_FREQ_ENG_16K        ((VOS_INT32)8388607)                    /* ���Ƶ���������ޣ���ֵ�ϵ��� CODEC_OpNormDiv_32(1, stSV.shwHalfFftLen, 31); */
#define MED_VAD_MAX_STD_DEVI_THD_16K    (2896)                                  /* ���Ƶ���������������ޣ���ֵ�ϵ��� CODEC_OpSqrt(stSV.swMaxFreqEngThd); */
#define MED_VAD_FREQ_ENG_NORM_PARA_16K  (8)                                     /* Ƶ�������������һ����������ֵ�ϵ��� CODEC_OpNorm_l(stSV.swMaxFreqEngThd); */
#define MED_VAD_STD_DEVI_NORM_PARA_16K  (19)                                    /* Ƶ�������������һ����������ֵ�ϵ��� CODEC_OpNorm_l(stSV.swMaxStandardDeviThd); */
#define MED_VAD_PSEUDOFLOAT_ONE_M       (16384)                                 /* α������ 1 ��m���� */
#define MED_VAD_PSEUDOFLOAT_ONE_E       (-14)                                   /* α������ 1 ��e���� */
#define MED_VAD_1_Q9                    (512)                                   /* 1 �� Q9����ʾ */
#define MED_VAD_1_Q19                   ((VOS_INT32)524288)                     /* 1 �� Q19����ʾ */
#define MED_VAD_0P7_Q15                 (22938)                                 /* 0.7 �� Q9����ʾ */
#define MED_VAD_0P3_Q15                 (9830)                                  /* 0.3 �� Q9����ʾ */
#define MIN_SMOOTH_ALPHA                (1)                                     /* ƽ��ϵ����Q15 */
#define MED_VAD_SMOOTH_AMP_THD          (25)                                    /* ƽ���������� */
#define MED_VAD_SMOOTH_AMP_UPDATE_THD   (40)                                    /* ƽ������ƽ������ */
#define MED_VAD_AVE_AMP_CURR_FRM_THD    (40)                                    /* ƽ���������� */

/* ������� */
#define MED_VAD_FRM_NUM_1P5_SECOND      (75)                                    /* 1.5��֡�� */
#define MED_VAD_BUFF_LEN                (256)                                   /* ���泤�� */

/* ��ȡȫ�ֱ��� */
#define MED_VAD_GetObjPtr()             (&g_astMedVadObjPool[0])                /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_VAD_GetObjInfoPtr()         (&g_stMedVadObjInfo)                    /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
 ʵ������  : MED_VAD_FFT_LEN_ENUM
 ��������  : FFT����ö�ٶ���
*****************************************************************************/
enum MED_VAD_FFT_LEN_ENUM
{
    MED_VAD_FFT_LEN_8K                  = 256,                                  /* 8k������ */
    MED_VAD_FFT_LEN_16K                 = 512,                                  /* 16k������ */
    MED_VAD_FFT_LEN_BUTT
};
typedef VOS_INT16 MED_VAD_FFT_LEN_ENUM_INT16;

/*****************************************************************************
 ʵ������  : MED_VAD_PREVFRM_LEN_ENUM
 ��������  : �Ӵ�ʱ��һ֡ĩβ�ĳ���ö�ٶ��壨�Ӵ��ã�
*****************************************************************************/
enum MED_VAD_PREVFRM_LEN_ENUM
{
    /* 8k������ */
    MED_VAD_PREVFRM_LEN_8K              = MED_VAD_FFT_LEN_8K - CODEC_FRAME_LENGTH_NB,
    /* 16k������ */
    MED_VAD_PREVFRM_LEN_16K             = MED_VAD_FFT_LEN_16K - CODEC_FRAME_LENGTH_WB,
    MED_VAD_PREVFRM_LEN_BUTT
};
typedef VOS_INT16 MED_VAD_PREVFRM_LEN_ENUM_INT16;


/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  6 STRUCT����
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwAlpha;                               /* ������ �����о�ϵ�� */
    VOS_INT16                           shwArtiSpeechThd;                       /* ������ �˹������������ */
    VOS_INT16                           shwChopNumThd;                          /* ������ �˹������У�Ƶ������������� */
    VOS_INT16                           shwReserved;
}MED_VAD_NV_STRU;

/*****************************************************************************
ʵ������  : MED_VAD_STRU
��������  : VADά���ṹ��
*****************************************************************************/
typedef struct
{
    /* ������Ϣ */
    MED_OBJ_HEADER
    MED_VAD_NV_STRU                     stNv;                                   /* NV��ά���ṹ�� */
    MED_VAD_FFT_LEN_ENUM_INT16          enFftLen;                               /* FFT���� */
    MED_VAD_PREVFRM_LEN_ENUM_INT16      enPrevFrmLen;                           /* ��һ֡���ݵ���Ч���� */
    VOS_INT16                           shwFrmLen;                              /* ֡�� */
    VOS_INT16                           shwFrmCount;                            /* ǰ5֡��֡���� */

    /* FFT��� */
    VOS_INT16                          *pshwHammingWindow;                      /* ����ָ�� */
    VOS_INT16                           shwHalfFftLen;                          /* FFT���ȵ�һ�� */
    VOS_INT16                           shwHalfFftShift;                        /* FFT����һ���ö����Ʊ�ʾ����ռ��λ�� */

    /* ������� */
    VOS_INT16                           shwStdDeviNormPara;                     /* Ƶ�������������һ������ */
    VOS_INT16                           shwFreqEngNormPara;                     /* Ƶ�������������һ������ */
    VOS_INT32                           swMaxFreqEngThd;                        /* ���Ƶ���������� */
    VOS_INT32                           swMaxStandardDeviThd;                   /* ���Ƶ���������������� */
    VOS_INT16                           shwSmoothedValue;                       /* ƽ���������ֵ */
    VOS_INT16                           shwModifiedAlpha;                       /* ƽ������ */

    /* ������� */
    VOS_INT16                           shwRingCnt;                             /* ���ζ��У��ڴ洢���ֿ�����Ϣ */
    VOS_INT16                           shwReserved2;
    CODEC_OP_FLOAT_STRU                   stFloatMinTarget;                       /* ����BUFF�е���Сֵ */
    CODEC_OP_FLOAT_STRU                   stFloatSmoothedTarget;                  /* ƽ����ĵ�ǰ�׷������ */
    CODEC_OP_FLOAT_STRU                   stFloatEmbryonicThd;                    /* ��ʷ֡���г�ʼ5֡�ĸ��� */
    CODEC_OP_FLOAT_STRU                   astFloatTargetBuf[MED_VAD_FRM_NUM_1P5_SECOND];/* һ��ʱ����ƽ������׷������BUFF�� */
    VOS_INT16                           ashwFrmDat[MED_VAD_FFT_LEN_16K];        /* ����֡����ʷ֡�Ľ�� */
    VOS_INT32                           aswBuff[MED_VAD_BUFF_LEN];              /* ���� */
} MED_VAD_OBJ_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern VOS_INT16           g_ashwMedVadHammingWin8k[128];                              /* Hamming��8k���� */
extern VOS_INT16           g_ashwMedVadHammingWin16k[256];                             /* Hamming��16k���� */
extern MED_VAD_OBJ_STRU    g_astMedVadObjPool[MED_VAD_MAX_OBJ_NUM];                    /* VADʵ����Դ�� */
extern MED_OBJ_INFO        g_stMedVadObjInfo;                                          /* ��ص���Ϣ */


/*****************************************************************************
  10 ��������
*****************************************************************************/

extern VOS_INT16 MED_VAD_ArtiSpchDetect(
                           MED_VAD_OBJ_STRU   *pstVad,
                           VOS_INT16           shwAveCurrFrm,
                           VOS_INT32          *aswFreqEng);
extern VOS_VOID* MED_VAD_Create(VOS_VOID);
extern VOS_UINT32 MED_VAD_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_VAD_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_VAD_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *penVadFlag);
extern VOS_UINT32 MED_VAD_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_VOID MED_VAD_UpdateThd(
                       MED_VAD_OBJ_STRU       *pstVad,
                       CODEC_OP_FLOAT_STRU      *pstFloatCurrTarget);
extern VOS_VOID MED_VAD_FreqEngCal(
                       MED_VAD_OBJ_STRU         *pstVad,
                       VOS_INT16                *pshwCurrFrm,
                       VOS_INT32                *pswFreqEng);











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_vad.h */


