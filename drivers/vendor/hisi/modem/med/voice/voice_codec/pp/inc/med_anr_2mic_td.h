

#ifndef __MED_2MIC_TD_H__
#define __MED_2MIC_TD_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "med_anr_2mic_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define MED_2MIC_CALIB_FLOOR_MIN_POWER            (1200)                        /* ��·�������� */
#define MED_2MIC_CALIB_GAIN_DELTA_MAX             (3277)                        /* ��������������ֵ int16(0.2 * 2^14) */
#define MED_2MIC_CALIB_GAIN_MAX                   (32767)                       /* У׼������� int16(2 * 2^14) */
#define MED_2MIC_CALIB_GAIN_MIN                   (8192)                        /* У׼��С���� int16(0.5 * 2^14) */
#define MED_2MIC_CALIB_ALPHA                      (32112)                       /* У׼����ƽ��ϵ�� int16(0.98 * 2^15) */

#define MED_2MIC_SPACESPLIT_MAX_DELAY             (15)                          /* ����ӳ� ע��:��ֵ Ӧ���ڵ��� MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH -1 */
#define MED_2MIC_SPACESPLIT_MC_DELAY              (768)                         /* int16(3 * 2^8) */
#define MED_2MIC_SPACESPLIT_MC_NE_GAIN            (13472)                       /* ��������� */
#define MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH       (16)                          /* �����ӳ��˲������� ע��:��ֵ-1 ӦС�ڵ��� MED_2MIC_SPACESPLIT_MAX_DELAY */

#define MED_2MIC_ANC_VAD_PS_PN_RATIO_INIT         (4096)                        /* vad���������˹��ʱȳ�ʼֵ */
#define MED_2MIC_ANC_VAD_FILTER_ORDER             (3)                           /* �˲������� */
#define MED_2MIC_ANR_VAD_ALPHA                    (16384)                       /* ˫���VAD���ƽ��ϵ��,���㷽��0.5 * 32767 */
#define MED_2MIC_ANR_OUTPUT_MAX_LEN               (640)                         /* ANRģ������������֡������Ӧ���ڴ�������п��ٵľֲ����� */

#define MED_2MIC_LAF_FILTER_ORDER_NB              (32)                          /* LAFխ���˲������� */
#define MED_2MIC_LAF_FILTER_ORDER_WB              (64)                          /* LAF����˲������� */
#define MED_2MIC_LAF_WEIGHT_UPDATE_LEN            (8)                           /* LAF�˲���ϵ������Ƶ��(ÿ8�����һ��)*/
#define MED_2MIC_LAF_WEIGHT_SHORT_POWER_THD_NB    (15000)                       /* LAF�˲�ϵ�����¶�ʱ������ֵ NB */
#define MED_2MIC_LAF_WEIGHT_SHORT_POWER_THD_WB    (50000)                       /* LAF�˲�ϵ�����¶�ʱ������ֵ WB */
#define MED_2MIC_LAF_WEIGHT_SHORT_POWER_ALPHA     (32767)                       /* LAF�˲�ϵ�����¶�ʱ��������ƽ��ϵ�� int16(1 * 2^15)*/
#define MED_2MIC_LAF_RC_LEN_MAX                   (640)                         /*  2*(CODEC_SAMPLE_RATE_MODE_16000/50) */


#define MED_2MIC_LAF_STEP_INIT                    (3277)                        /* LAF������ʼֵ int16(0.1 * 2^15) */
#define MED_2MIC_LAF_STEP_REF_POWER_THD           (500000)                      /* LAF��������������ֵ */

#define MED_2MIC_ANC_MC_MAX_DELAY                 (64)                          /* ANC��Mic����ӳ� */
#define MED_2MIC_MAX_Q14                          (16384)                       /* 2^14 */

#define MED_2MIC_MIN_POWER_VALUE                  (100)

#define MED_2MIC_PSD_NORMSHIFT_8                  (256)                         /* 2^8 */

/*****************************************************************************
 ������    : MED_2MIC_GetTdProcessPtr
 ��������  : ��ȡTDģ��ṹ��ָ��
*****************************************************************************/

#define MED_2MIC_GetTdProcessPtr()          (&g_stTdProcess)

/*****************************************************************************
 ������    : MED_2MIC_GetTdCalibPtr
 ��������  : ��ȡУ׼ģ��ṹ��ָ��
*****************************************************************************/

#define MED_2MIC_GetTdCalibPtr()            (&g_stTdProcess.stCalib)

/*****************************************************************************
 ������    : MED_2MIC_GetTdSpaceSplitPtr
 ��������  : ��ȡ�ռ����ģ��ṹ��ָ��
*****************************************************************************/

#define MED_2MIC_GetTdSpaceSplitPtr()       (&g_stTdProcess.stSpaceSplit)

/*****************************************************************************
 ������    : MED_2MIC_GetTdAncPtr
 ��������  : ��ȡ����Ӧ�˲�ģ��ṹ��ָ��
*****************************************************************************/

#define MED_2MIC_GetTdAncPtr()              (&g_stTdProcess.stAnc)

/*****************************************************************************
 ������    : MED_2MIC_GetTdVadPtr
 ��������  : ��ȡVAD���ģ��ṹ��ָ��
*****************************************************************************/

#define MED_2MIC_GetTdVadPtr()              (&g_stTdProcess.stAnc.stVad)

/*****************************************************************************
 ������    : MED_2MIC_SetTdSampleRate
 ��������  : ����VADģ�������
*****************************************************************************/

#define MED_2MIC_SetTdSampleRate(enSmpRate)     (g_stTdProcess.stAnc.stVad.enSampleRate = (enSmpRate))

/*****************************************************************************
 ������    : MED_2MIC_GetTdSampleRate
 ��������  : ��ȡVADģ�������
*****************************************************************************/

#define MED_2MIC_GetTdSampleRate()          (g_stTdProcess.stAnc.stVad.enSampleRate)

/*****************************************************************************
 ������    : MED_2MIC_GetTdFilterNumPtr
 ��������  : ��ȡ�˲���ϵ������ָ��
*****************************************************************************/

#define MED_2MIC_GetTdFilterNumPtr()           (g_ashwFilterNum)

/*****************************************************************************
 ������    : MED_2MIC_GetTdFilterNumPtr
 ��������  : ��ȡ�˲���ϵ������ָ��
*****************************************************************************/

#define MED_2MIC_GetTdFilterDenPtr()           (g_ashwFilterDen)

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
 ʵ������  : MED_2MIC_CALIB_STRU
 ��������  : ����2MICУ׼ģ��Ľṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16          enCalibEnable;                              /* 2MICУ׼ģ��ʹ�ܿ��� */
    VOS_INT16                       shwLastGain;                                /* ��һ֡��У׼����*/
} MED_2MIC_CALIB_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_SPACESPLIT_MC_STRU
 ��������  : ����2MIC�ռ������Mic�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                       ashwMainState[MED_2MIC_SPACESPLIT_MAX_DELAY];/* ��MiC��ʷ��Ϣ */
    VOS_INT16                       shwDelayValue;                               /* ��Mic�ӳٳ��� */
} MED_2MIC_SPACESPLIT_MC_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_SPACESPLIT_RC_STRU
 ��������  : ����2MIC�ռ���븨Mic�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                       ashwMainState[MED_2MIC_SPACESPLIT_MAX_DELAY];   /* ��Mic�ռ�������Mic��ʷ��Ϣ */
    VOS_INT16                       ashwRefState[MED_2MIC_SPACESPLIT_MAX_DELAY];    /* ��Mic�ռ����ĸ�Mic��ʷ��Ϣ */
    VOS_INT16                       shwDelayValue;                                  /* ��Mic�ӳٳ��� */
    VOS_INT16                       shwNeGain;                                      /* �ӳ����� */
} MED_2MIC_SPACESPLIT_RC_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_SPACESPLIT_RC_STRU
 ��������  : ����2MIC�ռ����Ľṹ��
*****************************************************************************/
typedef struct
{
    MED_2MIC_SPACESPLIT_MC_STRU     stMainNormMode;                             /* �ռ������Mic�ṹ�� */
    MED_2MIC_SPACESPLIT_RC_STRU     stRefMode;                                  /* �ռ���븨Mic�ṹ�� */
} MED_2MIC_SPACESPLIT_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_VAD_WB_STRU
 ��������  : ˫Mic ����źŵ�VAD���ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                       shwFilterOrder;                             /* �˲������� */
    VOS_INT16                       shwReserve;
    VOS_INT16                      *pshwFilterNum;                              /* �˲���ϵ�� */
    VOS_INT16                      *pshwFilterDen;                              /* �˲���ϵ�� */
    VOS_INT16                       ashwMcXState[MED_2MIC_ANC_VAD_FILTER_ORDER];        /* ״̬��Ϣ */
    VOS_INT16                       ashwMcYState[MED_2MIC_ANC_VAD_FILTER_ORDER];
    VOS_INT16                       ashwRcXState[MED_2MIC_ANC_VAD_FILTER_ORDER];
    VOS_INT16                       ashwRcYState[MED_2MIC_ANC_VAD_FILTER_ORDER];
} MED_2MIC_VAD_WB_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_VAD_STRU
 ��������  : ˫Mic VAD���ṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate;                               /* ������ */
    VOS_INT16                       shwVadRatioThd;                             /* VAD�ж���ֵ */
    VOS_INT16                       shwPsPnRatio;                               /* �ź��������Ĺ����ױ�ֵ */
    MED_2MIC_VAD_WB_STRU            stVadWb;
} MED_2MIC_VAD_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_LAF_STRU
 ��������  : ˫Mic LAF�˲��ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                       shwN;                                       /* LAF�˲������� */
    VOS_INT16                       ashwWeight[MED_2MIC_LAF_FILTER_ORDER_WB];   /* LAF�˲���ϵ�� */
    VOS_INT16                       shwStep;                                    /* ѧϰ���� */
    VOS_INT16                       ashwRcBuf[2*CODEC_FRAME_LENGTH_WB];         /* �������ʱBuff */
    VOS_INT32                       swInShortPower;                             /* ��ʱ���� */
    VOS_INT16                       shwAlpha;                                   /* ��ʱ����ƽ��ϵ�� */
    VOS_INT16                       shwMaxFar;                                  /* �ο��ź������� */
    VOS_INT16                       shwFarMaxThd;                               /* Զ���ź����ޣ����ڴ����������LAF�˲� */
    VOS_INT16                       shwReserve;                                 /* �ο��ź������� */
    VOS_INT32                       swInShortThd;                               /* �����ж϶�ʱ������ֵ */
    VOS_INT32                       swRefPowerThd;                              /* �����ƽ����������������ֵ */
} MED_2MIC_LAF_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_ANC_STRU
 ��������  : ˫Mic �ռ��˲��ṹ��
*****************************************************************************/
typedef struct
{
    MED_2MIC_VAD_STRU               stVad;                                        /* ˫Mic VAD���ṹ�� */
    MED_2MIC_LAF_STRU               stLaf;                                        /* ˫Mic LAF�˲��ṹ�� */
    VOS_INT16                       shwMainDelayQ;                                /* ������ӳ� */
    VOS_INT16                       shwReserve;
    VOS_INT16                       ashwMainDelayState[MED_2MIC_ANC_MC_MAX_DELAY];/* �������ʷ״̬ */
} MED_2MIC_ANC_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_TDPROCESS_STRU
 ��������  : ����2MIC TD���̿��ƽṹ��
*****************************************************************************/
typedef struct
{
    MED_2MIC_CALIB_STRU             stCalib;                                    /* 2MICУ׼ģ��ṹ�� */
    MED_2MIC_SPACESPLIT_STRU        stSpaceSplit;                               /* 2MIC�ռ����ģ��ṹ��*/
    MED_2MIC_ANC_STRU               stAnc;                                      /* 2MIC����Ӧ�˲�ģ��ṹ��*/
}MED_2MIC_TDPROCESS_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern MED_2MIC_TDPROCESS_STRU  g_stTdProcess;

/*****************************************************************************
  10 ��������
*****************************************************************************/

extern VOS_INT16 MED_ANR_2MIC_FirConvolute(
                       VOS_INT16               *pshwWeight,
                       VOS_INT16               *pshwRcIn,
                       VOS_INT16                shwLen );
extern VOS_UINT32 MED_ANR_2MIC_TdAncInit(
                       VOS_INT16                           shwVadRatioThld,
                       VOS_INT16                           shwFarMaxThd,
                       MED_2MIC_ANC_STRU                  *pstAnc);
extern VOS_UINT32 MED_ANR_2MIC_TdAncNormalMode(
                       MED_2MIC_ANC_STRU                       *pstAnc,
                       VOS_INT16                               *pshwMcIn,
                       VOS_INT16                               *pshwRcIn,
                       VOS_INT16                               *pshwCalib,
                       VOS_INT16                               *pshwMcOut);
extern VOS_UINT32 MED_ANR_2MIC_TdBlockMatrix(
                       MED_2MIC_SPACESPLIT_RC_STRU     *pstRefMode,
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                       *pshwRcOut);
extern VOS_INT32 MED_ANR_2MIC_TdCalcMeanPower(
                       VOS_INT16                       *pshwMicIn,
                       VOS_INT16                        shwFrameLength);
extern VOS_UINT32 MED_ANR_2MIC_TdCalib(
                       CODEC_VAD_FLAG_ENUM_INT16     enVadFlag,
                       MED_2MIC_CALIB_STRU             *pstCalib,
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                       *pshwMcOut,
                       VOS_INT16                       *pshwRcOut);
extern VOS_UINT32 MED_ANR_2MIC_TdCalibInit(
                       CODEC_SWITCH_ENUM_UINT16       enEnable,
                       MED_2MIC_CALIB_STRU         *pstCalib);
extern VOS_UINT32 MED_ANR_2MIC_TdIIRLowPass(
                       MED_2MIC_VAD_WB_STRU                    *pstVadWb,
                       VOS_INT16                               *pshwMcIn,
                       VOS_INT16                               *pshwMcXState,
                       VOS_INT16                               *pshwMcYState,
                       VOS_INT32                               *pswPowerMean);
extern VOS_UINT32 MED_ANR_2MIC_TdInit(
                       MED_2MIC_NV_STRU                    *pst2MICParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32      enSampleRate);
extern VOS_UINT32 MED_ANR_2MIC_TdLaf(
                           MED_2MIC_LAF_STRU                       *pstLaf,
                           VOS_INT16                               *pshwMcIn,
                           VOS_INT16                               *pshwRcIn,
                           VOS_INT16                                shwVadFlag,
                           VOS_INT16                               *pshwMcOut);
extern CODEC_VAD_FLAG_ENUM_INT16 MED_ANR_2MIC_TdNoiseVadDetect(
                       MED_2MIC_VAD_STRU                       *pstVad,
                       VOS_INT16                               *pshwMcIn,
                       VOS_INT16                               *pshwRcIn);
extern VOS_UINT32 MED_ANR_2MIC_TdProcess(
                       CODEC_SWITCH_ENUM_UINT16           enEnable,
                       CODEC_VAD_FLAG_ENUM_INT16     enVadFlag,
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                       *pshwMcOut,
                       VOS_INT16                       *pshwRcOut,
                       VOS_INT16                       *pshwRcCalib);
extern VOS_UINT32 MED_ANR_2MIC_TdSpaceFractionDelay(
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                        shwDelayValue,
                       VOS_INT16                       *pshwMainState,
                       VOS_INT16                       *pshwRefState,
                       VOS_INT16                       *pshwMcOut,
                       VOS_INT16                       *pshwRcOut);
extern VOS_UINT32 MED_ANR_2MIC_TdSpaceIntegerDelay(
                       VOS_INT16                       *pshwIn,
                       VOS_INT16                        shwDelay,
                       VOS_INT16                       *pshwState,
                       VOS_INT16                       *pshwOut);
extern VOS_UINT32 MED_ANR_2MIC_TdSpaceSplit(
                       MED_2MIC_SPACESPLIT_STRU        *pstSpaceSplit,
                       VOS_INT16                       *pshwMcIn,
                       VOS_INT16                       *pshwRcIn,
                       VOS_INT16                       *pshwMcOut,
                       VOS_INT16                       *pshwRcOut);
extern VOS_UINT32 MED_ANR_2MIC_TdSpaceSplitInit(
                       VOS_INT16                           shwRcDelay,
                       MED_2MIC_SPACESPLIT_STRU            *pstSpaceSplit);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_2mic_td.h */
