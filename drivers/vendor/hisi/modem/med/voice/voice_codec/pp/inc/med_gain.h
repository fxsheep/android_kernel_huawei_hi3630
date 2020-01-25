

#ifndef __MED_GAIN_H__
#define __MED_GAIN_H__


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "CodecInterface.h"
#include "codec_com_codec.h"
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
#define MED_SIDETONE_MAX_OBJ_NUM        (1)                                     /* �����������ʵ����Ŀ */
#define MED_SIDETONE_PARAM_LEN          (1)                                     /* ������� */
#define MED_GAIN_MAX                    (12)                                    /* ���������� */
#define MED_GAIN_MIN                    (-80)                                   /* ��С�������� */
#define MED_GAIN_BASE                   (80)                                    /* 0 dB��Ӧ�ĳ˷�����λ�� */
#define MED_DEVGAIN_MAX_OBJ_NUM         (2)                                     /* �豸�������ʵ����Ŀ */
#define MED_DEVGAIN_PARAM_LEN           (1)                                     /* ������� */
#define MED_SIDETONE_RX_LOUD            ((VOS_INT32)166553600)                 /* ���д��ź����ޣ���Ӧ-30dB */
#define MED_PP_AGC_RX_DELAY             (27)                                    /* �Ǵ��ź�ʱ�����ӹر����򿪵�����ӳ�ʱ�䣬 540ms */

/* ��ȡȫ�ֱ��� */
#define MED_SIDETONE_GetObjPtr          (&g_astMedSideToneObjPool[0])           /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_GAIN_Db2Linear(shwIdx)      (g_ashwGainTab[(MED_GAIN_BASE + (shwIdx))]) /* dB����תΪ�������� */
#define MED_DEVGAIN_GetObjPtr           (&g_astMedDevGainObjPool[0])            /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_SIDETONE_GetObjInfoPtr      (&g_stMedSideToneObjInfo)               /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */
#define MED_DEVGAIN_GetObjInfoPtr       (&g_stMedDevGainObjInfo)                /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */

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
 ʵ������  : MED_SIDETONE_STRU
 ��������  : ��������ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    VOS_INT16                           shwSetGain;                             /* ������ �������棬Q15 */
    VOS_INT16                           shwCurGain;                             /* ��ǰ�������棬Q15 */
    VOS_INT16                           shwFrmLen;                              /* ֡�� */
    VOS_INT16                           ashwFrmDat[CODEC_FRAME_LENGTH_WB];  /* TX֡���� */
    VOS_INT16                           shwReserved2;
    VOS_INT32                           swSTMuteCount;

} MED_SIDETONE_OBJ_STRU;

/*****************************************************************************
 ʵ������  : MED_DEVGAIN_STRU
 ��������  : �豸����ά���ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    VOS_INT16                           shwGain;                                /* ������ ���� */
    VOS_INT16                           shwFrmLen;                              /* ֡�ĳ��� */
} MED_DEVGAIN_OBJ_STRU;


/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern VOS_INT16 g_ashwGainTab[93];
extern MED_SIDETONE_OBJ_STRU    g_astMedSideToneObjPool[MED_SIDETONE_MAX_OBJ_NUM];/* ���������˲���ʵ����Դ�� */
extern MED_DEVGAIN_OBJ_STRU     g_astMedDevGainObjPool[MED_DEVGAIN_MAX_OBJ_NUM];

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_VOID* MED_DEVGAIN_Create(VOS_VOID);
extern VOS_UINT32 MED_DEVGAIN_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_DEVGAIN_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_DEVGAIN_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_DEVGAIN_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_SIDESTONE_UpdateTx (
                       VOS_VOID               *pstSideToneInstance,
                       VOS_INT16              *pshwTxIn);
extern VOS_VOID* MED_SIDETONE_Create(VOS_VOID);
extern VOS_UINT32 MED_SIDETONE_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_SIDETONE_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_SIDETONE_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwRxIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_SIDETONE_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_gain.h */


