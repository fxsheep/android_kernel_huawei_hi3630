
#ifndef __MED_ANR_2MIC_INTERFACE_H__
#define __MED_ANR_2MIC_INTERFACE_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"
#include "med_anr_2mic_td.h"
#include "med_anr_2mic_fd.h"
#include "med_anr_2mic_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ����ȫ�ֱ��� */
#define MED_ANR_2MIC_GetObjPtr()        (&g_stMed2MicObj)                       /* ��ȡ2MICȫ�ֱ���*/
#define MED_ANR_2MIC_GetObjInfoPtr()    (&g_stMed2MicObjInfo)                   /* ��ȡ2MICȫ�ֶ�����Ϣ*/
#define MED_ANR_2MIC_GetWinNb()         (g_ashwMedAnr2MicWinNb)                 /* խ��ʱƵ�任��ϵ�� */
#define MED_ANR_2MIC_GetWinWb()         (g_ashwMedAnr2MicWinWb)                 /* ���ʱƵ�任��ϵ�� */

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define MED_ANR_2MIC_MAX_OBJ_NUM             (1)                                /* ǰ������˫���ANR�ĸ���*/
#define MED_ANR_2MIC_PARAM_LEN               (13)                                /* ANR_2MIC��NV����� */

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
 ʵ������  : MED_ANR_2MIC_STRU
 ��������  : 2MIC�ڲ�״̬�ṹ
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* 2MICʹ�ܿ��� */
    MED_2MIC_NV_STRU                    st2MicNv;                               /* 2MIC��Nv�� */
    MED_2MIC_TDPROCESS_STRU            *pstTdProcess;                           /* 2MICʱ����ģ��*/
    MED_2MIC_COMM_STRU                  stComCfg;                               /* 2MIC����ģ��*/
    MED_2MIC_DATAFLOW_MC_STRU           stDataFlowMc;                           /* 2MIC������ź��������ṹ��*/
    MED_2MIC_DATAFLOW_RC_STRU           stDataFlowRc;                           /* 2MIC������ź��������ṹ��*/
    VOS_INT16                           shwReserve;
    MED_2MIC_POSTFILTER_STRU           *pstPostFilter;                          /* 2MICƵ����˲�ģ��ṹ��*/
} MED_ANR_2MIC_STRU;

/****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern MED_ANR_2MIC_STRU   g_stMed2MicObj;                                             /* ˫��˽���ģ��ȫ�ֿ���ʵ�� */
extern MED_OBJ_INFO        g_stMed2MicObjInfo;
extern VOS_INT16           g_ashwMedAnr2MicWinNb[160];
extern VOS_INT16           g_ashwMedAnr2MicWinWb[320];
/*****************************************************************************
  10 ��������
*****************************************************************************/

extern VOS_VOID* MED_ANR_2MIC_Create( VOS_VOID );
extern VOS_UINT32 MED_ANR_2MIC_Destroy( VOS_VOID **ppInstance );
extern VOS_UINT32 MED_ANR_2MIC_GetPara(
                       VOS_VOID                *pInstance,
                       VOS_INT16               *pshwPara,
                       VOS_INT16                shwParaLen);
extern VOS_UINT32  MED_ANR_2MIC_SetPara(
                       VOS_VOID                         *pInstance,
                       VOS_INT16                        *pshwPara,
                       VOS_INT16                         shwParaLen,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate);










#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_anr_2mic_interface.h */
