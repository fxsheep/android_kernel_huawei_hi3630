


#ifndef __MED_MBDRC_H__
#define __MED_MBDRC_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "med_pp_comm.h"
#include "med_eanr.h"
#include "med_aec_nlp.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define MED_MBDRC_RX_NB_FREQLEN         (256)
#define MED_MBDRC_RX_WB_FREQLEN         (512)
#define MED_MBDRC_RX_MU_FREQLEN         (1024)

#define MED_MBDRC_RX_LIMIT_THR          (24572)
#define MED_MBDRC_RX_LIMIT_MAKEUP_GAIN  (40)
#define MED_MBDRC_RX_0DBFS              (32767)
#define MED_MBDRC_RX_SUBBAND_NUM        (5)
#define MED_MBDRC_RX_ALPHA_NORM         (128)
#define MED_MBDRC_RX_SLOP_NORM          (128)
#define MED_MBDRC_RX_GAIN_30DB_NORM     (7706)
#define MED_MBDRC_RX_IFFT_NUM_IND       (3)
#define MED_MBDRC_RX_FFT_NUM_IND        (1)
#define MED_MBDRC_RX_NB_FRMLEN          (160)
#define MED_MBDRC_RX_NB_BANDWIDTH       (129)
#define MED_MBDRC_RX_MU_BANDWIDTH       (513)
#define MED_BGN_INIT_VALUE              (0)

#define MED_MBDRC_MAX_OBJ_NUM           (3)                                     /* MBDRC���ʵ����Ŀ */
#define MED_MBDRC_PARAM_LEN             (126)
#define MED_MBDRC_MAX_FFT_LEN           (1024)

#define MED_MBDRC_FRM_LEN_8K            (80)
#define MED_MBDRC_FFT_LEN_8K            (256)
#define MED_MBDRC_FRM_LEN_16K           (160)
#define MED_MBDRC_FFT_LEN_16K           (512)
#define MED_MBDRC_FRM_LEN_48K           (960)
#define MED_MBDRC_FFT_LEN_48K           (1024)

#define MED_MBDRC_NB_OVERLAP_LEN          (176)                                 /* խ���������� */
#define MED_MBDRC_WB_OVERLAP_LEN          (352)                                 /* ����ص������� */
#define MED_MBDRC_MB_OVERLAP_LEN          (32)                                  /* ��Ƶ�ص������� */
#define MED_MBDRC_NLP_WB_POST_OVERLAP_LEN (192)                                 /* ������ص������� */

#define MED_MBDRC_ANR_VOICE_SUB_FRAME_NUM (2)                                   /* NLP��֡���� */
#define MED_MBDRC_ANR_AUDIO_SUB_FRAME_NUM (1)                                   /* NLP��֡���� */

/* ����ȫ�ֱ��� */
#define MED_MBDRC_GetObjPtr()           (&g_stMedMbdrcObj[0])                   /* ��ȡMBDRCȫ�ֱ��� */
#define MED_MBDRC_GetObjInfoPtr()       (&g_stMedMbdrcObjInfo)                  /* ��ȡMBDRCȫ�ֶ�����Ϣ */

#define MED_MBDRC_GetNlpWinNb()         (&g_ashwMedAecNlpWinNb[0])              /* խ����ʱƵ�任��ϵ�� */
#define MED_MBDRC_GetNlpWinWb()         (&g_ashwMedAecNlpWinWb[0])              /* �����ʱƵ�任��ϵ�� */
#define MED_MBDRC_GetNlpWinMb()         (&g_ashwMedMbdrcNlpWinMb[0])            /* �����ʱƵ�任��ϵ�� */

/* ��ʱȫ�����飬��MBDRC��ģ��ʹ�� */
extern  VOS_INT16                        g_ashwMedMbdrcTmp1Len640[MED_MBDRC_MAX_FFT_LEN];

#define MED_MBDRC_GetshwVecTmp640Ptr1()  (&g_ashwMedMbdrcTmp1Len640[0])         /* ��ʱȫ������ָ�� ����640 INT16 */



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
 ʵ������  : MED_MBDRC_NV_STRU
 ��������  : MBDRC_NV�ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                   shwEnable;                                      /* MBDRCʹ���ź� */
    VOS_INT16                   shwSubbandNum;                                  /* �Ӵ��� */
    VOS_INT16                   ashwSubbandMaxGain[MED_MBDRC_RX_SUBBAND_NUM];   /* �Ӵ�������� */
    VOS_INT16                   ashwFreqBoundLow[MED_MBDRC_RX_SUBBAND_NUM];     /* Ƶ���±߽� */
    VOS_INT16                   ashwFreqBoundHigh[MED_MBDRC_RX_SUBBAND_NUM];    /* Ƶ���ϱ߽� */
    VOS_INT16                   ashwDnCompThr[MED_MBDRC_RX_SUBBAND_NUM];        /* ���Ӵ���ѹ�������ޣ����ڴ�����Ϊ��ѹ����������λΪdBfs */
    VOS_INT16                   ashwDnCompSlop[MED_MBDRC_RX_SUBBAND_NUM];       /* ���Ӵ���ѹ������������б�� */
    VOS_INT16                   ashwUpCompThr[MED_MBDRC_RX_SUBBAND_NUM];        /* ���Ӵ���ѹ�������ޣ�С�ڴ�����Ϊ��ѹ����������λΪdBfs */
    VOS_INT16                   ashwUpCompSlop[MED_MBDRC_RX_SUBBAND_NUM];       /* ���Ӵ���ѹ������������б�� */
    VOS_INT16                   ashwDnExpThr[MED_MBDRC_RX_SUBBAND_NUM];         /* ���Ӵ�����չ�����ޣ�С�ڴ�����Ϊ����չ��������λΪdBfs */
    VOS_INT16                   ashwDnExpSlop[MED_MBDRC_RX_SUBBAND_NUM];        /* ���Ӵ�����չ����������б�� */
    VOS_INT16                   ashwDnCompAlphaUp[MED_MBDRC_RX_SUBBAND_NUM];    /* ���Ӵ���ѹ����RMS�������źŹ��������ǰһ֡���ʱ��alpha�˲�ϵ�� */
    VOS_INT16                   ashwDnCompAlphaDn[MED_MBDRC_RX_SUBBAND_NUM];    /* ���Ӵ���ѹ����RMS�������źŹ��������ǰһ֡��Сʱ��alpha�˲�ϵ�� */
    VOS_INT16                   ashwLinearAlphaUp[MED_MBDRC_RX_SUBBAND_NUM];    /* ���Ӵ�������RMS�������źŹ��������ǰһ֡���ʱ��alpha�˲�ϵ�� */
    VOS_INT16                   ashwLinearAlphaDn[MED_MBDRC_RX_SUBBAND_NUM];    /* ���Ӵ�������RMS�������źŹ��������ǰһ֡��Сʱ��alpha�˲�ϵ�� */
    VOS_INT16                   ashwUpCompAlphaUp[MED_MBDRC_RX_SUBBAND_NUM];    /* ���Ӵ���ѹ����RMS�������źŹ��������ǰһ֡���ʱ��alpha�˲�ϵ�� */
    VOS_INT16                   ashwUpCompAlphaDn[MED_MBDRC_RX_SUBBAND_NUM];    /* ���Ӵ���ѹ����RMS�������źŹ��������ǰһ֡��Сʱ��alpha�˲�ϵ�� */
    VOS_INT16                   ashwDnExpAlphaUp[MED_MBDRC_RX_SUBBAND_NUM];     /* ���Ӵ�����չ��RMS�������źŹ��������ǰһ֡���ʱ��alpha�˲�ϵ�� */
    VOS_INT16                   ashwDnExpAlphaDn[MED_MBDRC_RX_SUBBAND_NUM];     /* ���Ӵ�����չ��RMS�������źŹ��������ǰһ֡��Сʱ��alpha�˲�ϵ�� */
    VOS_INT16                   ashwDnExpMinGain[MED_MBDRC_RX_SUBBAND_NUM];     /* ���Ӵ�����չ����С���� */
    VOS_INT16                   ashwAlphaUpDnThr[MED_MBDRC_RX_SUBBAND_NUM];     /* ���Ӵ�ALPHAUP��ALPHADN��ѡȡ���޲��� */
    VOS_INT16                   ashwMinPowerRMS[MED_MBDRC_RX_SUBBAND_NUM];      /* ��СRMS */
    VOS_INT16                   ashwLastPowerRMSInit[MED_MBDRC_RX_SUBBAND_NUM]; /* ��һ֡����RMS��ʼֵ */
    VOS_INT16                   ashwLastGain[MED_MBDRC_RX_SUBBAND_NUM];         /* ��һ֡���� */
    VOS_INT16                   shwDRCMindB;                                    /* DRC��СdBֵ */
    VOS_INT16                   ashwLastPowerRMSdBinInit[MED_MBDRC_RX_SUBBAND_NUM]; /* ��һ֡����RMS��ʼֵ */
    VOS_INT16                   ashwGainMakeUp[MED_MBDRC_RX_SUBBAND_NUM];       /* �������� */
    VOS_INT16                   shwFirLen;                                      /* Fir���� */
    VOS_INT16                   shwSpeechCntInit;
    VOS_INT16                   shwVADholdLenth;
} MED_MBDRC_NV_STRU;

/*****************************************************************************
 ʵ������  : MED_MBDRC_SUBAND_STRU
 ��������  : MBDRC_SUBAND�ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                        ashwMinPowerRMS[MED_MBDRC_RX_SUBBAND_NUM]; /* ��СRMS */
    VOS_INT16                        ashwLastPowerRMS[MED_MBDRC_RX_SUBBAND_NUM];/* ��һ֡RMS */
    VOS_INT16                        ashwLastGain[MED_MBDRC_RX_SUBBAND_NUM];    /* ��һ֡���� */
    VOS_INT16                        ashwLastPowerRMSdBin[MED_MBDRC_RX_SUBBAND_NUM];
    VOS_INT16                        shwLimitThr;                               /* �ź��޷����ޣ���λΪdBfs */
    VOS_INT16                        shwLimitMakeUpGain;                        /* ���Ӵ�����RMS�������� */
    VOS_INT16                        shwFrameLenth;
    VOS_INT16                        shwFreqLenth;                              /* Ƶ�׳���  */
    VOS_INT16                        shwSpeechCnt;
    VOS_INT16                        shwReserve;
    VOS_INT32                        swdBfs0;                                   /* DRC��RMS�������ߺ�����ź��޷����޵�0dBfs  */
} MED_MBDRC_SUBAND_STRU;

/*****************************************************************************
 ʵ������  : MED_MBDRC_AIG_STRU
 ��������  : MBDRC_AIG�ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwLastGain;                            /* ��һ֡���� */
    VOS_INT16                           shwFreqLenth;                           /* Ƶ�׳���  */
    VOS_INT32                           swdBfs0;                                /* DRC��RMS�������ߺ�����ź��޷����޵�0dBfs */

} MED_MBDRC_AIG_STRU;


/*****************************************************************************
 ʵ������  : MED_FFT_COM_CFG_STRU
 ��������  : ����FFT_COM_CFG�ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwFftLen;                              /* FFT���� */
    VOS_INT16                           shwSubFrmLen;                           /* �Ӵ�֡�� */
} MED_FFT_COM_CFG_STRU;

/*****************************************************************************
 ʵ������  : MED_FRM_PRE_STATUS_STRU
 ��������  : ����FRM_PRE_STATUS�ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwOverlapLen;                          /* �ص����鳤�� */
    VOS_INT16                           shwReserve;
    VOS_INT16                           ashwOverlap[MED_MBDRC_WB_OVERLAP_LEN]; /* �ص�����  */
} MED_ERR_PRE_STATUS_STRU;

/*****************************************************************************
 ʵ������  : MED_FRM_POST_STATUS_STRU
 ��������  : ����FRM_POST_STATUS�ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwOverlapLen;                          /* �ص����鳤�� */
    VOS_INT16                           shwReserve;
    VOS_INT16                           ashwOverlap[MED_MBDRC_WB_OVERLAP_LEN];  /* �ص����� */
} MED_ERR_POST_STATUS_STRU;

/*****************************************************************************
 ʵ������  : MED_FFT_TRANSFORM_STRU
 ��������  : ����FFT_TRANSFORM�ṹ��
*****************************************************************************/
typedef struct
{
    MED_ERR_PRE_STATUS_STRU             stErrPreStatus;                         /* �в�ǰ����״̬ */
    MED_ERR_POST_STATUS_STRU            stErrPostStatus;                        /* �в����״̬ */
} MED_FFT_TRANSFORM_STRU;

/*****************************************************************************
 ʵ������  : MED_ANR_FFT_STRU
 ��������  : ����ANR_FFT�ṹ��
*****************************************************************************/
typedef struct
{
    MED_FFT_COM_CFG_STRU                stComCfg;                               /* FFT���� �ṹ�� */
    MED_FFT_TRANSFORM_STRU              stTransform;                            /* FFTת�� �ṹ��*/
    VOS_INT16                          *pshwTrapewin;
} MED_ANR_FFT_STRU;

/*****************************************************************************
 ʵ������  : MED_MBDRC_STRU
 ��������  : MBDRC�ṹ��
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_MBDRC_NV_STRU                   stNv;                                   /* NV�� */
    MED_MBDRC_SUBAND_STRU               stSuband;                               /* �Ӵ��� */
    MED_ANR_FFT_STRU                    stAnrFft;                               /* FFT�� */
    VOS_INT16                           shwSubBandNum;                          /* �ӿ���� */
    VOS_INT16                           shwReserve;
} MED_MBDRC_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern MED_MBDRC_STRU                    g_stMedMbdrcObj[MED_MBDRC_MAX_OBJ_NUM];/* MBDRCģ��ȫ�ֿ���ʵ�� */
extern MED_OBJ_INFO                      g_stMedMbdrcObjInfo;                   /* MBDRC��Ϣ */

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_UINT32    MED_ANR_MBDRC_Rx_Main (
                       MED_EANR_STRU          *pstMedAnr,
                       MED_MBDRC_STRU         *pstMedMBDRC,
                       VOS_INT16              *pshwInput,
                       VOS_INT16              *pshwOutput,
                       VOS_INT16               shwVadFlag,
                       CODEC_SWITCH_ENUM_UINT16  enAnrEnable);
extern VOS_VOID    MED_ANR_RX_FFTProcess (
                              MED_ANR_FFT_STRU                  *pstAnrFft,
                              VOS_INT16                         *pshwInput,
                              VOS_INT16                          shwNormEn,
                              VOS_INT16                         *pshwSpecBin,
                              VOS_INT16                         *pshwNormShift);
extern  VOS_VOID MED_ANR_RX_IFFTProcess (
                               MED_ANR_FFT_STRU                  *pstAnrFft,
                               VOS_INT16                        *pshwErrBin,
                               VOS_INT16                        *pshwErrFrm,
                               VOS_INT16                         shwNormShift);
extern VOS_VOID  MED_DRC_GainFilter (
                       MED_MBDRC_STRU        *pstMedMBDRC,
                       VOS_INT16              shwPowerIn,
                       VOS_INT16              shwindex,
                       VOS_INT16              shwPowerInd,
                       VOS_INT16              shwNormShift,
                       VOS_INT16             *pshwGainLinear);
extern VOS_VOID*    MED_MBDRC_Create(VOS_VOID);
extern VOS_UINT32   MED_MBDRC_Destroy(VOS_VOID  **ppInstance);
extern VOS_UINT32    MED_MBDRC_GetPara(
                           VOS_VOID         *pInstance,
                           VOS_INT16        *pshwPara,
                           VOS_INT16         shwParaLen);
extern VOS_VOID    MED_MBDRC_Main(
                           MED_MBDRC_STRU          *pstMbdrc,
                           VOS_INT16               *pshwFrmFreq,
                           VOS_INT16               *pshwNormShift,
                           VOS_INT16                shwVadFlag);
extern VOS_UINT32    MED_MBDRC_SetPara (
                           VOS_VOID         *pInstance,
                           VOS_INT16        *psMBDRCNv,
                           VOS_INT16         shwParaLen,
                           VOS_INT32         swSampleRate);
extern VOS_VOID  MED_DRC_Interpolate(
                        MED_MBDRC_NV_STRU      *pstMbdrcNv,
                        VOS_INT32              *pswMakeupGain,
                        VOS_INT32              *pswInterpolateGain);
extern VOS_VOID MED_MBDRC_SubBandPowerCal (
                                        MED_MBDRC_STRU         *pstMedMBDRC,
                                        VOS_INT16              *pshwFrmFreqIn,
                                        VOS_INT16              *pshwSubbandPowerRMS,
                                        VOS_INT16              *pshwPowerInd);
extern VOS_VOID MED_MBDRC_SubBandProcess (
                       MED_MBDRC_STRU         *pstMedMBDRC,
                       VOS_INT16              *pshwFrmFreq,
                       VOS_INT16               shwNormShift);



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_mbdrc.h */
