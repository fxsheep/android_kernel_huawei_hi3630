
#ifndef __MED_AGC_H__
#define __MED_AGC_H__


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
#define MED_AGC_CLEAR_RX_MUT           ((VOS_INT32)40960)                       /* AGC���е���������ֵ -66dB ����ֵ(160*16*16)*/
#define MED_AGC_INC_STEP_NORMAL         (19988)                                 /* ��ͨ���Ӳ�����Q14 */
#define MED_AGC_DEC_STEP_NORMAL         (27255)                                 /* ��ͨ˥��������Q15 */
#define MED_AGC_DEC_STEP_FAST           (22670)                                 /* ����˥��������Q15 */
#define MED_AGC_MAX_OBJ_NUM             (2)                                     /* AGC���ʵ����Ŀ */
#define MED_AGC_PARAM_LEN               (6)                                     /* ��Ч����������� */
#define MED_AGC_LASTGAIN_INIT           (1024)                                  /* ��Ա����LastGain��ʼֵ Q10 */
#define MED_AGC_AVERENG_MIN             (1)                                     /* ֡ƽ��������Сֵ */

/* ��ȡȫ�ֱ��� */
#define MED_AGC_GetObjPtr               (&g_astMedAgcObjPool[0])                /* ��ȡ�ڴ��ȫ�ֱ�����ַ */
#define MED_AGC_GetObjInfoPtr           (&g_stMedAgcObjInfo)                    /* ��ȡ�ڴ����Ϣ��ȫ�ֱ�����ַ */

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
ʵ������  : MED_AGC_NV_STRU
��������  : AGC NV��ά���ṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16              enEnable;                               /* ������ ʹ���� */
    VOS_INT16                           shwOffset;                              /* ������ ƫ�������ñ�����AGC�����������ã�ֻ��MED_AGC_GetPara��ѯ�� */
    VOS_INT16                           shwMaxOut;                              /* ������ ������ */
    VOS_INT16                           shwFlrNoise;                            /* ������ ���� */
    VOS_INT16                           shwMaxIn;                               /* ������ ������� */
    CODEC_SWITCH_ENUM_UINT16              enClearNoiseEn;                         /* ������ ΢С��������ʹ����ñ�������AGC����ʹ�ã�ͨ���ر� */
}MED_AGC_NV_STRU;

/*****************************************************************************
ʵ������  : MED_AGC_STRU
��������  : AGC��ά���ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_AGC_NV_STRU                     stNv;                                   /* NV��ṹ�� */
    VOS_INT16                           shwFrmLen;                              /* ֡�ĳ��� */
    VOS_INT16                           shwLineK;                               /* б��K Q15 */
    VOS_INT16                           shwLineB;                               /* ���Ʒ���B */
    VOS_INT16                           shwLastGain;                            /* ��һ֡������ */
} MED_AGC_OBJ_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern MED_AGC_OBJ_STRU    g_astMedAgcObjPool[MED_AGC_MAX_OBJ_NUM];                    /* AGCʵ����Դ�� */
extern MED_OBJ_INFO        g_stMedAgcObjInfo;                                          /* AGC��Ϣ */

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_VOID* MED_AGC_Create(VOS_VOID);
extern VOS_UINT32 MED_AGC_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_AGC_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_AGC_Main(
                       VOS_VOID               *pstAgcInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16               shwVadFlag,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_AGC_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_AGC_PreClearNoise(
                       VOS_VOID               *pstAgcInstance,
                       VOS_INT16              *psIn,
                       VOS_INT16              *psOut);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* med_agc.h */

