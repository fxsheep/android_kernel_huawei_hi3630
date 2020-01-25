
#ifndef __MED_FILT_H__
#define __MED_FILT_H__


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "codec_com_codec.h"
#include "med_pp_comm.h"
#include "CodecNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* ������Ϣ */
#define MED_FILT_NORM_MAX               (15)                                    /* С����16bitʱ��֤��Խ���Q */
#define MED_PREEMP_MAX_OBJ_NUM          (3)                                     /* Ԥ�����˲���ģ�����ʵ����Ŀ */
#define MED_PREEMP_PARAM_LEN            (2)                                     /* Ԥ�����˲�����Ч����������� */
#define MED_PREEMP_LEN                  (2)                                     /* Ԥ�����˲�����FIR�˲����� */
#define MED_DEEMP_MAX_OBJ_NUM           (1)                                     /* ȥ�����˲���ģ�����ʵ����Ŀ */
#define MED_DEEMP_PARAM_LEN             (2)                                     /* ȥ�����˲�����Ч����������� */
#define MED_DEEMP_LEN                   (2)                                     /* ȥ�����˲����˲����� */
#define MED_HPF_MAX_OBJ_NUM             (5)                                     /* ��ͨ�˲���ģ�����ʵ����Ŀ */
#define MED_HPF_LEN                     (4)                                     /* ��ͨ�˲������� */
#define MED_HPF_PARAM_LEN               ((2 * (MED_HPF_LEN)) + 1)              /* ��ͨ�˲�����Ч����������� */
#define MED_HPF_PARAM_VEC_BASE          (1)                                     /* ��ͨ�˲���������ʼλ�� */
#define MED_HPF_HISTORY_BUFF_LEN        ((MED_HPF_LEN) - 1)                     /* ��ͨ�˲�������ʷ���泤�� */
#define MED_COMP_MAX_OBJ_NUM            (6)                                     /* Ƶ�첹���˲���ģ�����ʵ����Ŀ */
#define MED_COMP_FILT_LEN_MAX           (65)                                    /* Ƶ�첹���˲������������ */
#define MED_COMP_PRE_LEN_MAX            ((2*(MED_COMP_FILT_LEN_MAX)) - 1)      /* Ƶ�첹���˲���������һ֡�źŵ���������� */
#define MED_COMP_PARAM_VEC_BASE         (3)                                     /* Ƶ�첹���˲�����������ʼλ�� */
#define MED_COMP_CHANNEL_MAX            (2)                                      /* ���CHANNEL�� */
#define MED_COMP_CHANNEL_MONO           (1)                                      /* ���������� */
#define MED_COMP_CHANNEL_STEREO         (2)                                      /* ���������� */

/* PCM�����˲����˲�ϵ�����ֵ����ΪNV��ĳ��������޵� */
#define AUDIO_PCM_COMP_COEF_LEN_MAX      (128)

/* ��ȡȫ�ֱ��� */
#define MED_PREEMP_GetObjPtr            (&g_astMedPreempObjPool[0])             /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_PREEMP_GetObjInfoPtr        (&g_stMedPreempObjInfo)                 /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */
#define MED_DEEMP_GetObjPtr             (&g_astMedDeempObjPool[0])              /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_DEEMP_GetObjInfoPtr         (&g_stMedDeempObjInfo)                  /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */
#define MED_HPF_GetObjPtr               (&g_astMedHpfObjPool[0])                /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_HPF_GetObjInfoPtr           (&g_stMedHpfObjInfo)                    /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */
#define MED_COMP_GetObjPtr              (&g_astMedCompObjPool[0])               /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_COMP_GetObjInfoPtr          (&g_stMedCompObjInfo)                   /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */

/* ��ʱȫ������, ���˲���ʱʹ�� */
extern VOS_INT32                        g_aswMedFiltTmpLen320[CODEC_FRAME_LENGTH_WB];

#define MED_FILT_GetswVecTmp320Ptr()    (&g_aswMedFiltTmpLen320[0])             /* ��ʱȫ������ָ�� ����320 INT32 */


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
 ʵ������  : MED_IIR32_PARA_STRU
 ��������  : �߾���IIR�˲���ά���ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT32                          *pswOutFeedBack;                         /* ��������źŻ���ָ�� */
    VOS_INT16                          *pshwDen;                                /* �˲���ϵ��ָ�� */
    VOS_INT16                          *pshwNum;                                /* �˲���ϵ��ָ�� */
    VOS_INT32                           swIirLen;                               /* IIR�˲����ĳ��� */
}MED_IIR32_PARA_STRU;

/*****************************************************************************
 ʵ������  : MED_IIR_PARA_STRU
 ��������  : IIR�˲���ά���ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                          *pshwDen;                                /* �˲���ϵ��ָ�� */
    VOS_INT16                          *pshwNum;                                /* �˲���ϵ��ָ�� */
    VOS_INT32                           swIirLen;                               /* IIR�˲����ĳ��� */
}MED_IIR_PARA_STRU;

/*****************************************************************************
 ʵ������  : MED_PREEMP_NV_OBJ_STRU
 ��������  : Ԥ����NV��ά���ṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* ������ ʹ�ܱ�־ */
    VOS_INT16                           shwCoef;                                /* ������ Ԥ���ص�ϵ�� */
} MED_PREEMP_NV_STRU;


/*****************************************************************************
 ʵ������  : MED_PREEMP_OBJ_STRU
 ��������  : Ԥ����ά���ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_PREEMP_NV_STRU                  stNv;                                   /* NV�� */
    VOS_INT16                           shwFrmLen;                              /* ֡�ĳ��� */
    VOS_INT16                           shwPreSignal;                           /* �����źŵ����һ�� */
} MED_PREEMP_OBJ_STRU;

/*****************************************************************************
 ʵ������  : MED_DEEMP_NV_OBJ_STRU
 ��������  : Ԥ����NV��ά���ṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* ������ ʹ�ܱ�־ */
    VOS_INT16                           shwCoef;                                /* ������ Ԥ���ص�ϵ�� */
} MED_DEEMP_NV_STRU;

/*****************************************************************************
 ʵ������  : MED_DEEMP_OBJ_STRU
 ��������  : ȥ����ά���ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_DEEMP_NV_STRU                   stNv;                                   /* NV�� */
    VOS_INT16                           shwFrmLen;                              /* ֡�ĳ��� */
    VOS_INT16                           shwPreOut;                              /* ����źŵ����һ�� */
} MED_DEEMP_OBJ_STRU;

/*****************************************************************************
 ʵ������  : MED_HPF_NV_OBJ_STRU
 ��������  : ��ͨ�˲���NV��ά���ṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* ������ ʹ�ܱ�־ */
    VOS_INT16                           ashwDen[MED_HPF_LEN];                   /* �˲���ϵ�� */
    VOS_INT16                           ashwNum[MED_HPF_LEN];                   /* �˲���ϵ�� */
    VOS_INT16                           shwReserved;
} MED_HPF_NV_STRU;

/*****************************************************************************
 ʵ������  : MED_HPF_OBJ_STRU
 ��������  : ��ͨ�˲��ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_HPF_NV_STRU                     stNv;
    VOS_INT16                           shwFrmLen;                              /* ֡�ĳ��� */
    VOS_INT16                           ashwPreIn[MED_HPF_HISTORY_BUFF_LEN];    /* ������һ֡�˲�ǰ�ź� */
    VOS_INT32                           aswPreOut[MED_HPF_HISTORY_BUFF_LEN];    /* ������һ֡�˲�ǰ�ź� */
    MED_IIR32_PARA_STRU                 stFiltPara;                             /* �����˲��Ľṹ�� */
} MED_HPF_OBJ_STRU;

/*****************************************************************************
 ʵ������  : MED_COMP_NV_STRU
 ��������  : Ƶ�첹��NV��ά���ṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* ������ ʹ�ܱ�־ */
    VOS_INT16                           shwCompGain;                            /* ������ ʩ�����棨���ƣ� */
    VOS_INT16                           shwCoefLen;                             /* ������ �˲����ĳ��� */
    VOS_INT16                           ashwCompCoef[(2 * MED_COMP_FILT_LEN_MAX) - 1];    /* ������ �˲�����ϵ�� */
} MED_COMP_NV_STRU;

/*****************************************************************************
 ʵ������  : MED_COMP_OBJ_STRU
 ��������  : Ƶ�첹���ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_COMP_NV_STRU                    stNv;                                   /* NV�� */
    VOS_INT16                           shwFrmLen;                              /* ֡�ĳ��� */
    VOS_INT16                           ashwPreIn[MED_COMP_PRE_LEN_MAX];        /* �������һ֡�ź� */
    VOS_INT16                           shwReserved2;
} MED_COMP_OBJ_STRU;

/*****************************************************************************
 ʵ������  : PP_AUDIO_PCM_COMP_OBJ_STRU
 ��������  : PCM�����˲����ƽṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enPcmCompEnable;                        /* PCM�����˲������Ƿ�� */
    VOS_INT16                           shwCompGain;                            /* ���� */
    VOS_INT16                           shwM;                                   /* �����˲�FIR�˲����ĳ��ȣ���λbyte */
    VOS_INT16                           shwN;                                   /* �����˲�����PCM�����ĳ��ȣ���λbyte */
    VOS_INT16                          *pshwCoef;                               /* �˲�ϵ�����У���NV����� */
    VOS_INT16                          *pashwIn[MED_COMP_CHANNEL_MAX];          /* ����BUFFָ�룬����ΪN+M-1��������ʷ��ϢM-1���͵�ǰ�ĳ���N */
    VOS_INT16                          *pashwPreInBuff[MED_COMP_CHANNEL_MAX];   /* ��ǰ�������ʷ��Ϣ */
    VOS_INT16                          *pashwOut[MED_COMP_CHANNEL_MAX];         /* ���BUFFָ�� */
}PP_AUDIO_PCM_COMP_OBJ_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern MED_PREEMP_OBJ_STRU    g_astMedPreempObjPool[MED_PREEMP_MAX_OBJ_NUM];    /* Ԥ�����˲���ʵ����Դ�� */
extern MED_DEEMP_OBJ_STRU     g_astMedDeempObjPool[MED_DEEMP_MAX_OBJ_NUM];      /* ȥ�����˲���ʵ����Դ�� */
extern MED_HPF_OBJ_STRU       g_astMedHpfObjPool[MED_HPF_MAX_OBJ_NUM];          /* ��ͨ�˲���ʵ����Դ�� */
extern MED_COMP_OBJ_STRU      g_astMedCompObjPool[MED_COMP_MAX_OBJ_NUM];        /* Ƶ�첹���˲���ʵ����Դ�� */

/*****************************************************************************
  10 ��������
*****************************************************************************/

#if 0
extern VOS_UINT32 MED_AUDIO_COMP_CreateAndSetPara (
                       VOS_VOID                   *pstInstance,
                       PP_AUDIO_PCM_COMP_NV_STRU  *pstNv);
extern VOS_VOID MED_AUDIO_COMP_DataDeinterleave(VOS_VOID *pvIn, VOS_INT16 shwInSize, VOS_VOID *pvOut0, VOS_VOID *pvOut1);
extern VOS_VOID MED_AUDIO_COMP_DataInterleave(VOS_VOID *pvIn0, VOS_VOID *pvIn1, VOS_INT16 shwInSize, VOS_VOID *pvOut);
extern VOS_UINT32 MED_AUDIO_COMP_Destroy(VOS_VOID *pstInstance);
extern VOS_UINT32 MED_AUDIO_COMP_GetPara(
                       VOS_VOID                   *pstInstance,
                       PP_AUDIO_PCM_COMP_NV_STRU  *pstNv);
extern VOS_UINT32 MED_AUDIO_COMP_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16               shwInLen,
                       VOS_INT16              *pshwOut,
                       VOS_UINT32              uwChanNum);
#endif
extern VOS_VOID* MED_COMP_Create(VOS_VOID);
extern VOS_UINT32 MED_COMP_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_COMP_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_COMP_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_COMP_SetPara (
                       VOS_UINT32              swSampleRate,
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_VOID* MED_DEEMP_Create(VOS_VOID);
extern VOS_UINT32 MED_DEEMP_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_DEEMP_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_DEEMP_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_DEEMP_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_VOID MED_FILT_Fir (
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwCoef,
                       VOS_INT16               shwInLen,
                       VOS_INT32               swFirLen,
                       VOS_INT16               shwGain,
                       VOS_INT16              *pshwOut);
extern VOS_VOID MED_FILT_Iir (
                       MED_IIR_PARA_STRU      *pstFiltPara,
                       VOS_INT16              *pshwIn,
                       VOS_INT16               shwInLen,
                       VOS_INT16              *pshwOut);
extern VOS_VOID MED_FILT_Iir32 (
                       MED_IIR32_PARA_STRU    *pstFiltPara,
                       VOS_INT16              *pshwIn,
                       VOS_INT16               shwInLen,
                       VOS_INT16              *pshwOut);
extern VOS_VOID* MED_HPF_Create(VOS_VOID);
extern VOS_UINT32 MED_HPF_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_HPF_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_HPF_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_HPF_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_VOID* MED_PREEMP_Create(VOS_VOID);
extern VOS_UINT32 MED_PREEMP_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_PREEMP_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_PREEMP_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_PREEMP_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* med_preemp.h */

