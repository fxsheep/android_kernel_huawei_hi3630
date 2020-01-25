
#ifndef __MED_ANR_2MIC_FD_H__
#define __MED_ANR_2MIC_FD_H__
/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "med_anr_2mic_comm.h"
#include "codec_com_codec.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define MED_2MIC_MIN_BIN_ENRG           (1)                                     /* �����Ӵ��������� */

#define MED_2MIC_NUM_CHAN_8k            (32)                                    /* 8K�����ʵ�Ƶ���Ӵ��� */
#define MED_2MIC_NUM_CHAN_16k           (40)                                    /* 16K�����ʵ�Ƶ���Ӵ��� */
#define MED_2MIC_SP_LO_BAND_8k          (5)                                     /* ����Ƶ������Ƶ��(8k) */
#define MED_2MIC_SP_HI_BAND_8k          (30)                                    /* ����Ƶ������Ƶ��(8k) */
#define MED_2MIC_SP_LO_BAND_16k         (5)                                     /* ����Ƶ������Ƶ��(16k) */
#define MED_2MIC_SP_HI_BAND_16k         (37)                                    /* ����Ƶ������Ƶ��(16k) */
#define MED_2MIC_Mid_BAND_8k            (23)                                    /* ��Ƶλ��Ƶ��(8k) */
#define MED_2MIC_Mid_BAND_16k           (32)                                    /* ��Ƶλ��Ƶ��(16k) */

#define MED_2MIC_COMP_COEF1             (20577)                                 /* Ƶ��У��ϵ�� */
#define MED_2MIC_COMP_COEF2             (29066)                                 /* Ƶ��У��ϵ�� */
#define MED_2MIC_COMP_COEF3             (32613)                                 /* Ƶ��У��ϵ�� */
#define MED_2MIC_COMP_FREQ_LOW          (9)                                     /* Ƶ��У��������Ƶ�� */
#define MED_2MIC_COMP_FREQ_HIGH         (21)                                    /* Ƶ��У��������Ƶ�� */
#define MED_2MIC_COMP_FREQ_THD1         (17514)                                 /* �����Ӵ��ж���ֵ */
#define MED_2MIC_COMP_FREQ_THD2         (8231)                                  /* ������ź�Ƶ�����Ƴ̶��ж���ֵ */

#define MED_2MIC_SNPSD_MAX              (2048000000)                            /* ���������׵����ֵ */
#define MED_2MIC_SNPSD_STATE_LEN        (50)                                    /* ������С�������ٶ� */
#define MED_2MIC_SNPSD_PROB_ALPHA       (6553)                                  /* ����������ƽ��ϵ�� */
#define MED_2MIC_SNPSD_PROB_SUB_ALPHA   (26214)                                 /* 32767- MED_2MIC_SNPSD_PROB_ALPHA */
#define MED_2MIC_SNPSD_PROB_THD         (24576)                                 /* ���������б���ֵ */
#define MED_2MIC_SNPSD_ALPHA            (31129)                                 /* ƽ�����������׵�ƽ��ϵ�� */
#define MED_2MIC_SNPSD_SUB_ALPHA        (1638)                                  /* 32767 - MED_2MIC_SNPSD_ALPHA */

#define MED_2MIC_ABSEN_TRANS_RATIO_LOW  (8192)                                  /* �������˲ʱ�仯�ʵ���ֵ */
#define MED_2MIC_ABSEN_TRANS_RATIO_HIGH (24576)                                 /* �������˲ʱ�仯�ʸ���ֵ */
#define MED_2MIC_ABSEN_LNS_RATIO_THD    (12616)                                 /* ���ݷ�ƽ�����б�����������ֵ  */
#define MED_2MIC_ABSEN_POST_SNR_ALPHA   (18842)                                 /* ���������ƽ��ϵ�� */
#define MED_2MIC_ABSEN_POST_SNR_THLD    (4096)                                  /* �ж����������ڵĺ����������ֵ*/
#define MED_2MIC_ABSEN_POST_SNR_SUB_ALPHA (14746)                               /* ���������ƽ��ϵ��������������ֵ֮��*/

/* �������Ӵ����������ڸ��� */
#define MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA    (16384)                       /* ������ƽ��ϵ�� */
#define MED_2MIC_MODFY_ABSEN_PSD_RATIO_ALPHA      (16384)                       /* ������֮��ƽ��ϵ�� */
#define MED_2MIC_MODFY_ABSEN_POWER_RATIO_LOW      (3072)                        /* ���������� */
#define MED_2MIC_MODFY_ABSEN_RATIO_INIT           (1024)                        /* �����ȳ�ʼֵ */
#define MED_2MIC_MODFY_ABSEN_POWER_RATIO_HIGH     (2048)                        /* ���������� */
#define MED_2MIC_MODFY_ABSEN_PSD_RATIO_LOW        (3072)                        /* ������֮������ */
#define MED_2MIC_MODFY_ABSEN_PSD_RATIO_HIGH       (3072)                        /* ������֮������ */
#define MED_2MIC_MODFY_FREQ_SMOOTH_LEN            (5)                           /* ƽ������ */
#define MED_2MIC_SPABSEN_Q13                      (8192)                        /* 2^13 */
#define MED_2MIC_MODFY_ABSEN_FAR_MUTE_INIT        (3200)                        /* ��Զ���ź�֡������ʼֵ */

/* ������Ӵ��������ڸ��� */
#define MED_2MIC_SPPRENSEN_POSTSNR_MIN            (3)                           /* �����������Сֵ */
#define MED_2MIC_SPPRENSEN_PRIORSNR_MIN           (3)                           /* �����������Сֵ */
#define MED_2MIC_SPPRENSEN_POSTSNR_INIT           (256)                         /* ��������ȳ�ʼֵ */
#define MED_2MIC_SPPRENSEN_PRIORSNR_ALPHA         (30146)                       /* ���������ƽ��ϵ�� */
#define MED_2MIC_SPPRENSEN_PRIORSNR_SUB_ALPHA     (2621)                        /* 1 - ���������ƽ��ϵ�� */
#define MED_2MIC_SPPRENSEN_NSPSD_ALPHA            (27852)                       /* ����������ƽ��ϵ�� */
#define MED_2MIC_SPPRENSEN_NSPSD_SUB_ALPHA        (4915)                        /* 1 - ����������ƽ��ϵ�� */
#define MED_2MIC_SPPRENSEN_NSPSD_BETA             (32767)                       /* ����������ƽ��ϵ�� */
#define MED_2MIC_SPPRENSEN_PROBZEROTHD            (29490)                       /* ���������б���ֵ */
#define MED_2MIC_CALCPROBSPPRESEN_SNRFACTOR_MAX   (32072)                       /* ������������� */
#define MED_2MIC_SPPRENSEN_Q8                     (256)                         /* 2^8 */


#define MED_2MIC_LSAGAIN_MIN_ALPHA      (9830)                                  /* ��С����ƽ��ϵ�� */
#define MED_2MIC_LSAGAIN_MIN_BETA       (24576)                                 /* ��С����ƽ��ϵ�� */
#define MED_2MIC_LSAGAIN_TIME_ALPHA     (13107)                                 /* ֡��ƽ��ϵ�� */
#define MED_2MIC_LSAGAIN_SMOOTH_THD     (9830)                                  /* ƽ��������ж���ֵ */
#define MED_2MIC_LSA_LOW_BAND           (5)                                     /* ��Ƶ����ʼλ�� */
#define MED_2MIC_LSA_DELTA              (2)                                     /* ƽ������ */
#define MED_2MIC_LSA_MEAN_COEF          (6553)                                  /* ��ƽ��(����5�൱�ڳ���2)*/
#define MED_2MIC_LSAGAIN_FREQ_SMOOTH_LEN (3)
#define MED_2MIC_LSAGAIN_Q8             (256)                                   /* 2^8 */

#define MED_2MIC_TMOS_SILENCETh         (50)                                    /* �رս��뵥Ԫǰ��Ĭ֡������ */
#define MED_2MIC_TMOS_CLOSEANR_LEN      (150)                                   /* �رս��뵥Ԫ֡�� */
#define MED_2MIC_TMOS_EXIT_LOWBIN_8k    (4)                                     /* �˳��رս���ʱͳ����������խ����Ƶ������ */
#define MED_2MIC_TMOS_EXIT_HIGHBIN_8k   (16)                                    /* �˳��رս���ʱͳ����������խ����Ƶ������ */
#define MED_2MIC_TMOS_EXIT_LOWBIN_16k   (8)                                     /* �˳��رս���ʱͳ���������������Ƶ������ */
#define MED_2MIC_TMOS_EXIT_HIGHBIN_16k  (32)                                    /* �˳��رս���ʱͳ���������������Ƶ������ */
/*****************************************************************************
 ������    : MED_2MIC_GetFdProcessPtr
 ��������  : ��ȡFDģ��ṹ��ָ��
*****************************************************************************/
#define MED_2MIC_GetFdProcessPtr()                (&g_stAnr2MicFdProcess)

/* ��ȡ��������ָ�� */
#define MED_2MIC_GetCompCoefPtr()                 (g_ashwAnr2MicCompCoef)
#define MED_2MIC_GetPsdRatioFreqAlphaPtr()        (g_ashwAnr2MicPsdRatioFreqAlpha)
#define MED_2MIC_GetBandAlphaPtr()                (g_ashwAnr2MicBandAlpha)
#define MED_2MIC_GetAnr2MicChTblPtr()             (g_ashwMedAnr2MicChTbl)

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
 ʵ������  : MED_2MIC_COMPFREQAP_STRU
 ��������  : ��������������ײ����ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                          *pshwCompCoef;                           /* ����ź�Ƶ�ʲ���ϵ�� */
    VOS_INT16                           shwSampleRate;                          /* ������ */
    VOS_INT16                           shwReserve;
} MED_2MIC_COMPFREQAP_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_PROBSPABSEN_STRU
 ��������  : �������������ڸ��ʽṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwProbSpPresen[MED_2MIC_NUM_CHAN_16k];  /* ���Ӵ����������ڸ��� */
    VOS_INT16                           shwSumProbThld;                           /* ȫ���������ڸ�����ֵ */
    VOS_INT16                           shwSpBandLow;                             /* �������ڵ���ʼ�Ӵ� */
    VOS_INT16                           shwSpBandHigh;                            /* �������ڵĽ�ֹ�Ӵ� */
    VOS_INT16                           shwReserve;
} MED_2MIC_PROBSPABSEN_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_MODFYPROBSPABSEN_STRU
 ��������  : �������������ڸ��ʽṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                            shwPowerRatioLowLast;                   /* ��һ֡��Ƶ�������� */
    VOS_INT16                            shwPowerRatioHighLast;                  /* ��һ֡��Ƶ�������� */
    VOS_INT16                            ashwPsdRatioLast[MED_2MIC_NUM_CHAN_16k];/* ��һ֡���Ӵ�������֮�� */
    VOS_INT16                           *pshwPsdRatioFreqAlpha;                  /* ������֮��ƽ��ϵ�� */
    VOS_INT16                            shwFreqSmoothLen;                       /* ƽ������ */
    VOS_INT16                            shwDifIndex;                            /* ��Ƶλ�� */
    VOS_INT16                            shwFarMuteCnt;
    VOS_INT16                            shwFarMuteCntThd;
} MED_2MIC_MODFYPROBSPABSEN_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_PROBSPPRESEN_STRU
 ��������  : �����������ڸ��ʽṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT32                           aswPostSNR[MED_2MIC_NUM_CHAN_16k];      /* ���Ӵ���������� */
    VOS_INT32                           aswPrioriSNR[MED_2MIC_NUM_CHAN_16k];    /* ���Ӵ���������� */
    VOS_INT32                           aswNsPsd[MED_2MIC_NUM_CHAN_16k];        /* ���Ӵ����������� */
    VOS_INT16                           shwNsIndex;
    VOS_INT16                           shwAllPassNum;
    VOS_INT16                           shwNsLowTh;                             /* �����о������� */
    VOS_INT16                           shwNsHighTh;                            /* �����о������� */
    VOS_INT16                           shwSpLowTh;                             /* �������ڸ��ʵ����� */
    VOS_INT16                           shwReserve;
    VOS_INT16                           shwTmosExitBandLow;                     /* Tmos�����Ƴ�ͳ������ʼ�Ӵ� */
    VOS_INT16                           shwTmosExitBandHi;                      /* Tmos�����Ƴ�ͳ������ֹ�Ӵ� */
} MED_2MIC_PROBSPPRESEN_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_SNPSDMIC_STRU
 ��������  : ���������ƽ�����������׽ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT32                           aswMinPsd[MED_2MIC_NUM_CHAN_16k];       /* ���Ӵ���С������ */
    VOS_INT32                           aswTmpPsd[MED_2MIC_NUM_CHAN_16k];       /* ���Ӵ������� */
    VOS_INT16                           shwFrameNum;                            /* ����֡�� */
    VOS_INT16                           shwReserve;
    VOS_INT16                           ashwProbSpPresen[MED_2MIC_NUM_CHAN_16k];/* �������ڸ��� */
} MED_2MIC_SNPSDMIC_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_LSAGAIN_STRU
 ��������  : ������Ӵ�����Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                            shwBandLow;                             /* ���Ӵ�λ�� */
    VOS_INT16                            shwGainMin;                             /* ��С���� */
    VOS_INT16                           *pshwBandAlpha;                       /* �Ӵ���ƽ��ϵ�� */
    VOS_INT16                            shwSmoothLen;                           /* ƽ������ */
    VOS_INT16                            shwReserve;
} MED_2MIC_LSAGAIN_STRU;

/*****************************************************************************
 ʵ������  : MED_2MIC_POSTFILTER_STRU
 ��������  : ����2MICƵ�����Ľṹ��
 *****************************************************************************/
typedef struct
{
    VOS_INT16                           shwBandLen;                             /* �Ӵ����� */
    CODEC_SWITCH_ENUM_UINT16            enFirstFrameInitFlag;                   /* ��һ֡�Ƿ��ʼ����־ */
    VOS_INT16                           shwFreqBinLen;                          /* Ƶ�׳��� */
    VOS_INT16                           shwTimeAlpha;                           /* ����ƽ��������ʱ��ƽ��ϵ�� */
    VOS_INT16                           shwFarMaxThd;                           /* Զ���ź����ޣ���������������FD���ο��ź����� */
    VOS_INT16                           shwReserve;
    VOS_INT32                           aswBandSY[MED_2MIC_NUM_CHAN_16k];       /* ����˸��Ӵ������źŹ����� */
    VOS_INT32                           aswBandSU[MED_2MIC_NUM_CHAN_16k];       /* ����˸��Ӵ������źŹ����� */
    VOS_INT32                           aswBandMY[MED_2MIC_NUM_CHAN_16k];       /* ����˸��Ӵ�ƽ������������ */
    VOS_INT32                           aswBandMU[MED_2MIC_NUM_CHAN_16k];       /* ����˸��Ӵ�ƽ������������ */
    VOS_INT16                           ashwProbSpAbsen[MED_2MIC_NUM_CHAN_16k]; /* ����˸��Ӵ����������ڸ��� */
    VOS_INT16                           ashwProbSpPresen[MED_2MIC_NUM_CHAN_16k];/* ����˸��Ӵ��������ڸ��� */
    VOS_INT16                           ashwLsaGain[MED_2MIC_NUM_CHAN_16k];     /* ����˸��Ӵ����� */
    VOS_INT16                           ashwOmLsaGain[MED_2MIC_NUM_CHAN_16k];   /* ��������Ӵ����� */
    MED_2MIC_COMPFREQAP_STRU            stCompFreqAp;                           /* ��������������������ṹ�� */
    MED_2MIC_SNPSDMIC_STRU              stSnPsdMc;                              /* ��������������׽ṹ�� */
    MED_2MIC_SNPSDMIC_STRU              stSnPsdRc;                              /* ��������������׽ṹ�� */
    MED_2MIC_PROBSPABSEN_STRU           stProbSpAbsen;                          /* �������������ڸ��ʽṹ�� */
    MED_2MIC_MODFYPROBSPABSEN_STRU      stModfyProbSpAbsen;                     /* �������������ڸ��ʽṹ�� */
    MED_2MIC_PROBSPPRESEN_STRU          stProbSpPresen;                         /* �����������ڸ��ʽṹ�� */
    MED_2MIC_LSAGAIN_STRU               stLsaGain;                              /* ������Ӵ�����ṹ�� */
} MED_2MIC_POSTFILTER_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/




/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern const VOS_INT16 g_ashwMedAnr2MicChTbl[MED_2MIC_NUM_CHAN_16k][2];
extern MED_2MIC_POSTFILTER_STRU g_stAnr2MicFdProcess;
/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_VOID MED_2MIC_CalcBandPsd(
                        VOS_INT32               *pswPsdBin,
                        VOS_INT16                shwBandLen,
                        VOS_INT32               *pswBandPsd);
extern VOS_VOID MED_2MIC_CalcLsaGain(
                       VOS_INT32                 *pswPrioriSNR,
                       VOS_INT16                 *pshwSNRFactor,
                       VOS_INT16                 *pshwProbSpPresen,
                       MED_2MIC_LSAGAIN_STRU     *pstLsaGain,
                       VOS_INT16                  shwBandLen,
                       VOS_INT16                 *pshwOmLsaGain,
                       VOS_INT16                 *pshwLsaGain);
extern VOS_VOID MED_2MIC_CalcProbSpAbsen(
                       VOS_INT16                         *pshwProbSpPresen,
                       VOS_INT32                         *pswBandPsd,
                       VOS_INT32                         *pswBandMY,
                       VOS_INT16                          shwBandLen,
                       MED_2MIC_PROBSPABSEN_STRU         *pstProbSpAbsen,
                       VOS_INT16                         *pshwProbSpAbsen);
extern VOS_VOID MED_2MIC_CalcProbSpPresen(
                       VOS_INT32                         *pswBandPsd,
                       VOS_INT16                         *pshwProbSpAbsen,
                       VOS_INT16                         *pshwLsaGain,
                       VOS_INT16                          shwBandLen,
                       MED_2MIC_PROBSPPRESEN_STRU        *pstProbSpPresen,
                       VOS_INT16                         *pshwProbSpPresen,
                       VOS_INT16                         *pshwSNRFactor);
extern VOS_VOID MED_2MIC_CalcPsdBin(
                       MED_2MIC_FREQ_STRU      *pstFreq,
                       VOS_INT16                shwFreqBinLen,
                       VOS_INT32               *pswPsdBin);
extern VOS_VOID MED_2MIC_CalcSmoothPsd(
                       VOS_INT32               *pswBandPsd,
                       VOS_INT16                shwBandLen,
                       VOS_INT16                shwTimeAlpha,
                       VOS_INT32               *pswSmoothPsd);
extern VOS_VOID MED_2MIC_CalcSnPsd(
                       VOS_INT32               *pswSmoothPsd,
                       VOS_INT32               *pswCurrPsd,
                       VOS_INT16                shwBandLen,
                       VOS_INT32               *pswSnPsd,
                       MED_2MIC_SNPSDMIC_STRU  *pstSnPsdMic);
extern VOS_VOID MED_2MIC_CompFreqAp(
                       VOS_INT32                         *pswPsdBinMc,
                       VOS_INT32                         *pswPsdBinOriRc,
                       VOS_INT16                          shwFreqBinLen,
                       MED_2MIC_COMPFREQAP_STRU          *pstCompFreqAp,
                       VOS_INT32                         *pswPsdBinRc);
extern VOS_VOID MED_2MIC_EstiProbSpPresen(
                       VOS_INT32                         *pswBandSY,
                       VOS_INT32                         *pswBandSU,
                       VOS_INT32                         *pswBandMY,
                       VOS_INT32                         *pswBandMU,
                       VOS_INT16                          shwBandLen,
                       VOS_INT16                         *pshwProbSpPresen);
extern VOS_VOID MED_2MIC_ExtendBand2FreqBinGain(
                        VOS_INT32               *pswBandMc,
                        VOS_INT16               *pshwBandGain,
                        VOS_INT16                shwFreqBinLen,
                        VOS_INT16                shwBandLen,
                        VOS_INT16               *pshwFreqBinGain,
                        MED_2MIC_PROBSPPRESEN_STRU *pstProbSpPresen,
                        VOS_INT16                  *pshwProbSpPrese);
extern VOS_VOID MED_2MIC_FilterFrm(
                       VOS_INT16                shwFreqBinLen,
                       VOS_INT16               *pshwFreqBinGain,
                       MED_2MIC_FREQ_STRU      *pstMc);
extern VOS_VOID MED_2MIC_FreqProcessFirstFrame(
                       VOS_INT32                         *pswBandMc,
                       VOS_INT32                         *pswBandRc,
                       MED_2MIC_POSTFILTER_STRU          *pstPostFilter);
extern VOS_VOID MED_2MIC_ModifyProbSpAbsen(
                       VOS_INT32                         *pswBandSY,
                       VOS_INT32                         *pswBandSU,
                       VOS_INT16                          shwBandLen,
                       VOS_INT32                         *pswPsdBinMc,
                       VOS_INT32                         *pswPsdBinRc,
                       VOS_INT16                         *pshwProbSpAbsence,
                       MED_2MIC_MODFYPROBSPABSEN_STRU    *pstModifyProb);
extern VOS_VOID MED_2MIC_SmoothLsaGain(
                       VOS_INT16               *pshwLsaGainLast,
                       VOS_INT16               *pshwBandAlpha,
                       VOS_INT16                shwSmoothLen,
                       VOS_INT16                shwBandLow,
                       VOS_INT16                shwBandLen,
                       VOS_INT16               *pshwLsaGain);
extern VOS_INT16 MED_2MIC_WeightSum(
                       VOS_INT16               *pshwIn,
                       VOS_INT16               *pshwSmoothAlpha,
                       VOS_INT16                shwSmoothLen,
                       VOS_INT16                shwCntI);
extern VOS_UINT32 MED_ANR_2MIC_FdInit(
                       MED_2MIC_NV_STRU                 *pst2MICParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate,
                       VOS_INT16                         shwFftLen);
extern VOS_UINT32 MED_ANR_2MIC_FdProcess(
                       CODEC_SWITCH_ENUM_UINT16           enEnable,
                       MED_2MIC_DATAFLOW_MC_STRU         *pstDataFlowMc,
                       MED_2MIC_DATAFLOW_RC_STRU         *pstDataFlowRc,
                       VOS_INT16                          shwMaxFar);



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_anr_2mic_fd.h */
