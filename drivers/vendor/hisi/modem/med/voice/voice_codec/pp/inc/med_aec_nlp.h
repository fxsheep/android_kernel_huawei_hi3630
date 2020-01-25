
#ifndef _MED_AEC_NLP_H_
#define _MED_AEC_NLP_H_

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "med_aec_comm.h"
#include "med_anr_2mic_comm.h"
#include "med_aec_dtd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* ����ȫ�ֱ��� */
#define MED_AEC_GetNlpWinNb()              (&g_ashwMedAecNlpWinNb[0])           /* խ����NLPʱƵ�任��ϵ�� */
#define MED_AEC_GetNlpWinWb()              (&g_ashwMedAecNlpWinWb[0])           /* �����NLPʱƵ�任��ϵ�� */
#define MED_AEC_GetNlpGainModulTabNB()     (&g_ashwMedAecNlpGainModulTabNB[0])  /* խ����NLP�Ӵ����Ƹ�Ƶ���������ϵ�� */
#define MED_AEC_Get2MicNlpGainModulTabNB() (&g_ashwMed2MicAecNlpGainModulTabNB[0])/* խ����2MIC NLP�Ӵ����Ƹ�Ƶ���������ϵ�� */
#define MED_AEC_GetNlpGainModulTabWB()     (&g_ashwMedAecNlpGainModulTabWB[0])  /* �����NLP�Ӵ����Ƹ�Ƶ���������ϵ�� */
#define MED_AEC_Get2MicNlpGainModulTabWB() (&g_ashwMed2MicAecNlpGainModulTabWB[0])/* �����2MIC NLP�Ӵ����Ƹ�Ƶ���������ϵ�� */
#define MED_AEC_GetNlpBinTansTab()         (&g_ashwMedAecNlpDtd2NlpBinTransTab[0])/* NLP�Ӵ�����Ƶ��ӳ��� DTD��FFT������NLP��ͬ���� */

/* ������ */
#define MED_AEC_1MIC_NLP_NB_FRM_LEN     (80)                                    /* խ��֡�� */
#define MED_AEC_1MIC_NLP_WB_FRM_LEN     (160)                                   /* ���֡�� */
#define MED_AEC_2MIC_NLP_NB_FRM_LEN     (160)                                   /* խ��֡�� */
#define MED_AEC_2MIC_NLP_WB_FRM_LEN     (320)                                   /* ���֡�� */
#define MED_AEC_NLP_NB_OVERLAP_LEN      (176)                                   /* խ���������� */
#define MED_AEC_NLP_WB_OVERLAP_LEN      (352)                                   /* ����������� */
#define MED_AEC_NLP_MAX_FFT_LEN         (512)                                   /* NLP���FFT���� */
#define MED_AEC_2MIC_NLP_MAX_FFT_LEN    (640)                                   /* 2MIC_NLP���FFT���� */
#define MED_AEC_NLP_CNG_EST_LEN         (60)                                    /* ����������ֵ���峤�� */
#define MED_AEC_NLP_GAMMA_MAX           (32767)                                 /* Gamma���ֵ, ������ȫ˫��״̬ */
#define MED_AEC_NLP_GAMMA_FIRST         (16384)                                 /* Gamma��ʼֵ */
#define MED_AEC_NLP_GAMMA_MIN           (327)                                   /* Gamma��Сֵ, ������ȫ����״̬ */
#define MED_AEC_NLP_GRAD_NUM            (20)                                    /* NLP����������󳤶� */
#define MED_AEC_NLP_SUB_FRAME_NUM       (2)                                     /* NLP��֡���� */

/* ���������������� */
#define MED_AEC_NLP_SEED1               (100)                                   /* �������1 */
#define MED_AEC_NLP_SEED2               (101)                                   /* �������2 */
#define MED_AEC_NLP_SEED3               (102)                                   /* �������3 */
#define MED_AEC_NLP_RNDOM_NUM           (3)                                     /* �����״̬���� */
#define MED_AEC_NLP_15200_Q0            (15200)                                 /* 15200 Q0  */

/* ͬ�෨�������в��� */
#define MED_AEC_NLP_RAND_A1             (170)                                   /* ��չ����a1 */
#define MED_AEC_NLP_RAND_A2             (171)                                   /* ��չ����a2 */
#define MED_AEC_NLP_RAND_A3             (172)                                   /* ��չ����a3 */
#define MED_AEC_NLP_RAND_M1             (30269)                                 /* ѭ��ģm1 */
#define MED_AEC_NLP_RAND_M2             (30307)                                 /* ѭ��ģm2 */
#define MED_AEC_NLP_RAND_M3             (30323)                                 /* ѭ��ģm3 */

/* ͬ�������������ϵ�� */
#define MED_AEC_NLP_RAND_ALPHA          (10825)                                 /* �������ϵ��1, Q15 */
#define MED_AEC_NLP_RAND_BETA           (10812)                                 /* �������ϵ��2, Q15 */
#define MED_AEC_NLP_RAND_GAMMA          (10806)                                 /* �������ϵ��3, Q15 */

/* Wiener�˲� */
#define MED_AEC_MAX_WIENER_DEC          (10)                                    /* NLP Wiener�˲�����ʱ�̡�����˫����Wiener�˲����ÿ�ʼ����ʱ�̣���Χ0 �� 1000����λ10ms��Ĭ��19 */
#define MED_AEC_MAX_WIENER_OFF          (19)                                    /* NLP Wiener�˲�ֹͣʱ�̣�����˫����Wiener�˲�����ֹͣʱ�̣���Χ���ڵ���Wiener�˲�����ʱ�̣���λ10ms��Ĭ��38 */
#define MED_AEC_NLP_WIENER_LVL          (1)                                     /* Wiener�˲����� */
#define MED_AEC_NLP_WIENER_MIN_COEF     (5898)                                  /* Wiener�˲�ϵ������0.18, -15dB */
#define MED_AEC_NLP_PSD_SMOOTH          (9380)                                  /* 0.3 ������ϵ����������,ֵԽС,����Խ�� */
#define MED_AEC_NLP_SUPP_EN_THD         (10000)                                 /* ��Ƶ��һ�����Ƶ�������ֵ */
#define MED_NLP_NLP_CNG_BIN_SHIFT       (10)
#define MED_AEC_NLP_RES_POW_THD         ((VOS_INT32)2000000000)                 /* Res��Ƶ������ֵ */

/* ������������ */
#define MED_AEC_NLP_BGN_MEAN_POW_MAX    ((VOS_INT32)2000*2000)
#define MED_AEC_NLP_BGN_AMP_ALPHA            (6553)                             /* ƽ��ϵ��0.2     Q15 */
#define MED_AEC_NLP_BGN_ONE_SUB_AMP_ALPHA    (26215)                            /* ƽ��ϵ��0.8     Q15 */
#define MED_AEC_NLP_BGN_FREQ_ALPHA           (655)                              /* ƽ��ϵ��0.02    Q15 */
#define MED_AEC_NLP_BGN_ONE_SUB_FREQ_ALPHA   (32113)                            /* ƽ��ϵ��0.98    Q15 */
#define MED_AEC_NLP_BGN_MIN_SMOOTH_ALPHA     (33)                               /* ƽ��ϵ��0.0001  Q15 */
#define MED_AEC_NLP_BGN_SMOOTH_COFE1         (22938)                            /* ƽ��ϵ��0.7     Q15 */
#define MED_AEC_NLP_BGN_ONE_SUB_SMOOTH_COFE1 (9830)                             /* ƽ��ϵ��0.3     Q15 */
#define MED_AEC_NLP_BGN_INIT_VALUE           (0)                                /* ����������ֵ��ʼֵ */
#define MED_AEC_NLP_MODIFIED_ALPHA           (1)                                /* ����������������ϵ����ʼֵ */
/* ���� */
#define MED_NLP_NLP_CNG_INSERT_COFE          (19660)                            /* ��������������ϵ�� */
#define MED_NLP_NLP_CNG_SHIFT                (2)                                /* ˫Mic���һ�����ƣ��������������Ӧ�������ƺ�ķ��ȡ�
                                                                                   ͨ������6-10������˲����Ƶ���źż�С4�� */

/* ����DTD�Ӵ����ʽ��л������� */
#define MED_AEC_NLP_CAL_SP_PROB_BAND_LEN_NB  (32)                               /* ����DTD�Ӵ����ʽ���խ���������� ��ֹ�Ӵ� */
#define MED_AEC_NLP_CAL_SP_PROB_BAND_LEN_WB  (40)                               /* ����DTD�Ӵ����ʽ��п���������� ��ֹ�Ӵ� */
#define MED_AEC_NLP_LOW_BIN_INDEX            (5)                                /* ����DTD�Ӵ����ʽ��л������� ��Ƶ����������ʼ���� */
#define MED_AEC_NLP_HIGIH_BIN_INDEX          (11)                               /* ����DTD�Ӵ����ʽ��л������� ��Ƶ����������ֹ���� */

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/
/* ǰ���� */
extern VOS_INT16 g_ashwMedAecNlpWinNb[];
extern VOS_INT16 g_ashwMedAecNlpWinWb[];

/*****************************************************************************
  5 STRUCT����
*****************************************************************************/
/*****************************************************************************
 ʵ������  : MED_AEC_NLP_CONFIG_STRU
 ��������  : ����NLP���õĽṹ��
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enNlpEnable;
    VOS_INT16                           shwDt2StLvl;                            /* DT -> ST ���л��ٶȼ��� */
    VOS_INT16                           shwSt2DtLvl;                            /* ST -> DT ���л��ٶȼ��� */
    VOS_INT16                           shwCngInsertLvl;                        /* ������������ */
    VOS_INT16                           shwNoiseFloorValue;                     /* ��������ֵ */
    VOS_INT16                           shwMaxSuppress;                         /* ��Ƶ��������������� */
    VOS_INT16                           shwNonlinearThd;                        /* ��Ƶ�������������� */
    VOS_INT16                           shwNlpBandProbSupFastAlpha;             /* NLP: ���˲�ϵ�� */
    VOS_INT16                           shwNlpBandProbSupSlowAlpha;             /* NLP: ���˲�ϵ�� */
    VOS_INT16                           shwReserve;                             /* ���� */
}MED_AEC_NLP_CONFIG_STRU;

/*****************************************************************************
 ʵ������  : MED_AEC_NLP_SWITCH_STRU
 ��������  : ����NLP gamma�л�״̬�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwGammaPre;
    VOS_INT16                           ashwDt2StGradsTab[MED_AEC_NLP_GRAD_NUM];/* Dt2Stʵ��ʹ�õ�handover���߱� ��󼶱�20 */
    VOS_INT16                           shwDt2StGradsLen;                       /* Dt2Stʵ��ʹ�õ����߱��� */
    VOS_INT16                           ashwSt2DtGradsTab[MED_AEC_NLP_GRAD_NUM];/* St2Dtʵ��ʹ�õ�handover���߱� ��󼶱�20 */
    VOS_INT16                           shwSt2DtGradsLen;                       /* St2Dtʵ��ʹ�õ����߱��� */
    VOS_INT16                           shwDt2StCnt;                            /* DT -> ST ���� */
    VOS_INT16                           shwSt2DtCnt;                            /* ST -> DT ���� */
    VOS_INT16                           shwReserve;
}MED_AEC_NLP_SWITCH_STRU;

/*****************************************************************************
 ʵ������  : MED_AEC_NLP_RANDOM_STRU
 ��������  : ����������в���״̬�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwSeed[MED_AEC_NLP_RNDOM_NUM];        /* ������� */
    VOS_INT16                           ashwState[MED_AEC_NLP_RNDOM_NUM];       /* �������״̬ */
} MED_AEC_NLP_RANDOM_STRU;

/*****************************************************************************
 ʵ������  : MED_AEC_NLP_RANDOM_STRU
 ��������  : ����������������״̬�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT32                           aswSme[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];/* Mic�Ͳв������ */
    VOS_INT32                           aswSmm[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];/* Mic�źŹ����� */
    VOS_INT32                           aswSyy[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];/* ���������� */
    VOS_INT32                           aswSee[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];/* �в���� */
    VOS_INT16                           shwWienerFulEn;                         /* ��ȫʹ�ܽ׶� */
    VOS_INT16                           shwWienerPrtEn;                         /* ����ʹ�ܽ׶� */
    VOS_INT16                           shwDtCnt;                               /* ˫������ */
    VOS_INT16                           shwMaxSuppress;
    VOS_INT16                           shwPowThd;                               /* ST\DT���ɽ׶�����������ֵ */
    VOS_INT16                           shwNonlinearThd;
}MED_AEC_NLP_RES_STRU;

/*****************************************************************************
 ʵ������  : MED_AEC_NLP_RANDOM_STRU
 ��������  : ����������������״̬�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwModifiedALpha;                       /* ����������������ϵ�� */
    VOS_INT16                           shwBgnBinLen;                           /* ��������Ƶ�׳��� */
    VOS_INT32                           aswBgnBin[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)]; /* ��������Ƶ�� */
    VOS_INT32                           swBgnValue;                             /* �����������ȹ���ֵ */
    VOS_INT16                           ashwSubErrFrmLast[CODEC_FRAME_LENGTH_WB/2];/* ��һ����֡ */
    VOS_INT32                           swSmoothedPower;                        /* ƽ���������� */
    VOS_INT32                           swMinNoisePower;                        /* ��С�������� */
    VOS_INT32                           swPowerBuf[MED_AEC_NLP_CNG_EST_LEN];    /* ������������60*20ms */
}MED_AEC_NLP_BGN_STRU;

/*****************************************************************************
 ʵ������  : MED_AEC_NLP_BAND_SUPRESS_STRU
 ��������  : �����Ӵ��������ڸ��ʲв����ƵĽṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwNlpFreqBinGain[1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)];  /* Ƶ������ */
    VOS_INT16                           shwReserve;                             /* ���� */
    VOS_INT32                           swPsdBinLo2Hi;                          /* ��Ƶ�в����� */
    VOS_INT16                           shwNlpBandProbSupFastAlpha;             /* NLP: ���˲�ϵ�� */
    VOS_INT16                           shwNlpBandProbSupSlowAlpha;             /* NLP: ���˲�ϵ�� */
}MED_AEC_NLP_BAND_SUPRESS_STRU;

/*****************************************************************************
 ʵ������  : MED_AEC_NLP_RANDOM_STRU
 ��������  : ����NLP״̬�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwFftLen;                              /* NLP��FFT���� */
    VOS_INT16                           shwFrameLen;                            /* NLP 1MIC:֡��10ms;2MIC:֡��20ms */
    MED_AEC_NLP_CONFIG_STRU             stConfig;                               /* NLP������ */
    MED_AEC_NLP_SWITCH_STRU             stSwCtrl;                               /* gamma��������״̬ */
    MED_AEC_NLP_RANDOM_STRU             stRandSn;                               /* �����������״̬ */
    MED_AEC_NLP_RES_STRU                stRes;                                  /* ������������״̬ */
    MED_AEC_NLP_BGN_STRU                stBgnStru;                              /* ������������״̬ */
    MED_AEC_NLP_BAND_SUPRESS_STRU       stBandSupress;                          /* �Ӵ��������ڸ��ʲв�����״̬ */
}MED_AEC_NLP_STRU;

/*****************************************************************************
 ʵ������  : MED_AEC_NLP_RANDOM_STRU
 ��������  : ����ʱƵת�����ص��ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwOverlapLen;
    VOS_INT16                           shwFrmLen;
    VOS_INT16                           ashwOverlap[MED_AEC_NLP_WB_OVERLAP_LEN*2];
}MED_AEC_OVERLAP_STRU;

/*****************************************************************************
 ʵ������  : MED_AEC_NLP_RANDOM_STRU
 ��������  : ����ʱƵת��״̬�ṹ��
*****************************************************************************/
typedef struct
{
    MED_AEC_OVERLAP_STRU                stErrPreStatus;                         /* �в��ź�ǰ����״̬ */
    MED_AEC_OVERLAP_STRU                stMicPreStatus;                         /* ��˷��ź�ǰ����״̬ */
    MED_AEC_OVERLAP_STRU                stErrPostStatus;                        /* �в��źź���״̬ */
    VOS_INT16                           shwFftLen;                              /* NLP ��ǰFFT IFFT�ĳ��� */
    VOS_INT16                           shwReserve;
    VOS_INT16                          *pshwTrapeWin;                           /* ���δ�ϵ��ָ�� */
    MED_FFT_NUM_ENUM_INT16              enFftNumIndex;                          /* FFT��������ö�� */
    MED_FFT_NUM_ENUM_INT16              enIfftNumIndex;                         /* IFFT��������ö�� */
} MED_AEC_TRANS_STRU;


/*****************************************************************************
  6 UNION����
*****************************************************************************/

/*****************************************************************************
  7 OTHERS����
*****************************************************************************/

/*****************************************************************************
  8 ��������
*****************************************************************************/

extern VOS_VOID MED_AecNlpWienerCofe(
                       VOS_INT32          *paswSme,
                       VOS_INT32          *paswSmm,
                       VOS_INT32          *paswSyy,
                       VOS_INT16           shwPow,
                       VOS_INT16           ashwMaxH,
                       VOS_INT16           ashwMinH,
                       VOS_INT16           shwFftLen,
                       VOS_INT16          *pshwWienerCoef);
extern VOS_VOID MED_AEC_1MIC_NlpMain(
                       MED_AEC_NLP_STRU              *pstNlp,
                       MED_AEC_DTD_NLINE_STRU        *pstDtdNline,
                       VOS_INT16                     *pshwErr,
                       MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag,
                       VOS_INT16                     *pshwMcBin,
                       VOS_INT16                     *pshwMcErrBinNorm,
                       VOS_INT16                      shwNormShift);
extern VOS_VOID MED_AEC_NlpBandProbSuppress(
                MED_AEC_DTD_NLINE_STRU         *pstDtdNline,
                MED_AEC_NLP_BAND_SUPRESS_STRU  *pstBandSupress,
                VOS_INT16                      *pshwErrBinNorm,
                VOS_INT16                       shwNormShift,
                VOS_INT16                       shwFftLen);
extern VOS_VOID MED_AEC_2MIC_Fft(
                       MED_2MIC_FREQ_STRU      *pstFreqOut,
                       MED_2MIC_COMM_STRU      *pstComCfg,
                       VOS_INT16               *pshwTimeIn,
                       CODEC_SWITCH_ENUM_UINT16   enNormEnable);
extern VOS_VOID MED_AEC_2MIC_Freq2Time(
                       MED_2MIC_DATAFLOW_MC_STRU         *pstFreqIn,
                       MED_2MIC_COMM_STRU                *pstComCfg,
                       VOS_INT16                         *pshwTimeOut);
extern VOS_VOID  MED_AEC_2MIC_NlpMain(
                       MED_2MIC_DATAFLOW_MC_STRU        *pstDataFlowMc,
                       MED_2MIC_DATAFLOW_RC_STRU        *pstDataFlowRc,
                       MED_AEC_NLP_STRU                 *pstNlp,
                       MED_AEC_DTD_NLINE_STRU           *pstDtdNline,
                       VOS_INT16                        *pshwMcErr,
                       MED_AEC_DTD_FLAG_ENUM_INT16       enDtdFlag);
extern VOS_VOID MED_AEC_2MIC_NlpRc(
                       VOS_INT16              *pshwRcErrBinNorm,
                       VOS_INT16              *pshwRcsBinNorm,
                       VOS_INT16              *pshwWienerCoef,
                       VOS_INT16               shwFftLen );
extern VOS_VOID MED_AEC_2MIC_Time2Freq(
                       MED_2MIC_DATAFLOW_MC_STRU        *pstFreqMc,
                       MED_2MIC_DATAFLOW_RC_STRU        *pstFreqRc,
                       MED_2MIC_COMM_STRU                stComCfg,
                       VOS_INT16                        *pshwMcTd,
                       VOS_INT16                        *pshwRcTd,
                       VOS_INT16                        *pshwMcIn,
                       VOS_INT16                        *pshwRcCalib);
extern VOS_VOID MED_AEC_Freq2Time(
                       MED_AEC_TRANS_STRU          *pstTrans,
                       VOS_INT16                   *pshwMcErrBinNorm,
                       VOS_INT16                    shwNormShift,
                       VOS_INT16                   *pshwErrOut);
extern VOS_VOID MED_AEC_NlpBgnEst (
                       MED_AEC_NLP_BGN_STRU *pstBgnStru,
                       VOS_INT16            *pshwFrm,
                       VOS_INT16            *pshwBin,
                       VOS_INT16             shwFrmLen,
                       VOS_INT16             shwNoiseFloorValue);
extern VOS_VOID MED_AEC_NlpCngWhiteGen(
                       MED_AEC_NLP_RANDOM_STRU     *pstRandomState,
                       VOS_INT16                    shwGenLen,
                       VOS_INT16                   *pshwWhiteSn);
extern VOS_VOID MED_AEC_NlpCtrl(
                       MED_AEC_NLP_SWITCH_STRU     *pstSwCtrl,
                       VOS_INT16                   *pshwdErr,
                       MED_AEC_DTD_FLAG_ENUM_INT16  enDtd,
                       VOS_INT16                   *pshwGamma);
extern VOS_VOID MED_AEC_NlpInit(
                       MED_AEC_NV_STRU                *pstAecParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                       MED_AEC_NLP_STRU               *pstNlp,
                       MED_PP_MIC_NUM_ENUM_UINT16      enPpMicNum);
extern VOS_VOID MED_AEC_NlpInsertCn (
                       VOS_INT16               *pshwErrBin,
                       VOS_INT16                shwGamma,
                       VOS_INT16               *pshwWhiteNoise,
                       VOS_INT32                swBgnValue,
                       VOS_INT16                shwLen);
extern VOS_VOID MED_AEC_NlpRes(
                       MED_AEC_NLP_RES_STRU *pstRes,
                       VOS_INT16            *pshwMicBin,
                       VOS_INT16            *pshwErrBin,
                       VOS_INT16            *pshwErrBinNorm,
                       VOS_INT16            *pshwWienerCoef,
                       VOS_INT16             shwFftLen,
                       VOS_INT16             shwGamma);
extern VOS_VOID MED_AEC_NlpShapeCng (
                       VOS_INT16       *pshwWhiteNoise,
                       VOS_INT32       *pswNoiseBin,
                       VOS_INT16        shwFftLen);
extern VOS_VOID MED_AEC_NlpSlopSuppress(
                       VOS_INT16       *pshwInput,
                       VOS_INT16        shwFftLen,
                       VOS_INT16        shwMaxSuppress,
                       VOS_INT16       *pshwOutput);
extern VOS_INT16 MED_AEC_NlpSyncCnt(
                       VOS_INT16               shwGamma,
                       VOS_INT16              *pashwGrads,
                       VOS_INT16               shwGradsLen);
extern VOS_VOID MED_AEC_Time2Freq(
                       MED_AEC_TRANS_STRU       *pstTrans,
                       VOS_INT16                *pshwNear,
                       VOS_INT16                *pshwErr,
                       VOS_INT16                *pshwMcBin,
                       VOS_INT16                *pshwMcErrBinNorm,
                       VOS_INT16                *pshwNormShiftErr);
VOS_VOID MED_EAEC_Time2Freq(
                       MED_AEC_TRANS_STRU       *pstTrans,
                       VOS_INT16                *pshwNear,
                       VOS_INT16                *pshwErr,
                       VOS_INT16                *pshwMcBin,
                       VOS_INT16                *pshwMcErrBinNorm,
                       VOS_INT16                *pshwNormShiftNear,
                       VOS_INT16                *pshwNormShiftErr);
extern VOS_VOID MED_AEC_TransInit(
                       MED_AEC_NV_STRU                *pstAecParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                       MED_AEC_TRANS_STRU             *pstTrans);
extern VOS_VOID MED_AEC_TransPostProcess(
                       MED_AEC_OVERLAP_STRU        *pstPostStatus,
                       VOS_INT16                   *pshwErrBin,
                       VOS_INT16                    shwNormShift,
                       VOS_INT16                   *pshwErrFrm,
                       VOS_INT16                    shwFftLen,
                       VOS_INT16                    shwIfftNumIndex,
                       VOS_INT16                   *pshwTrapeWin);
extern VOS_VOID MED_AEC_TransPreProcess(
                       MED_AEC_OVERLAP_STRU        *pstPreStatus,
                       VOS_INT16                   *pshwStream,
                       VOS_INT16                    shwNormEn,
                       VOS_INT16                   *pshwSpecBin,
                       VOS_INT16                   *pshwNormShift,
                       VOS_INT16                    shwFftNumIndex);
extern VOS_VOID MED_AEC_NlpGainModulTab(
                     CODEC_SAMPLE_RATE_MODE_ENUM_INT32  enPpSampleRate,
                     MED_PP_MIC_NUM_ENUM_UINT16         enMicNum,
                     VOS_INT16                         *pshwNlpProbBandLen,
                     VOS_INT16                        **ppshwGainModulTab);

extern VOS_VOID MED_1MIC_ExtendBand2FreqBinGain(
                    VOS_INT16               *pshwBandGain,
                    VOS_INT16                shwFreqBinLen,
                    VOS_INT16                shwBandLen,
                    VOS_INT16               *pshwFreqBinGain);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_nlp.h*/

