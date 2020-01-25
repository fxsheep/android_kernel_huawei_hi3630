



/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_op_etsi.h"
#include "codec_op_lib.h"
#include "codec_op_netsi.h"
#include "codec_com_codec.h"
#include "med_pp_comm.h"

#ifndef __MED_EANR_H__
#define __MED_EANR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define MED_EANR_GetObjPtr()               (&g_astMedEanrObj[0])                   /* ��ȡEANRȫ�ֱ��� */
#define MED_EANR_GetObjInfoPtr()           (&g_stMedEanrObjInfo)                  /* ��ȡEANRȫ�ֶ�����Ϣ */

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define MED_EANR_MAX_OBJ_NUM                    (2)                             /*EANRʵ�����*/
#define MED_EANR_1MIC_PARAM_LEN                 (3)                             /*EANR��NV��Ч��������*/
#define MED_EANR_MAXWINLEN                      (1024)                          /*STFT�Ӵ����ȣ��ɸ���FFT��С���б仯*/
#define MED_EANR_MAXBINNUM                      (41)                            /*�����Ӵ�����խ��33���41*/
#define MED_EANR_DATAWINLEN                     (328)                           /*���¹�������Сֵʱ��Ҫ����Ĺ��������ݳ���(8*41)*/
#define MED_EANR_DATAWINNUM                     (8)                             /*���¹�������Сֵʱ��Ҫ����Ĺ�����֡��*/
#define MED_EANR_UPDATELEN                      (21)                            /*���¹�������Сֵʱ���¹��������ݵ�ʱ����*/
#define MED_EANR_SNRMAPCOEFA                    (32603)                         /*(CODEC_OpSub(32767, 0.005))*/
#define MED_EANR_SNRMAPCOEFB                    (32685)                         /*(CODEC_OpAdd(0.005>>1, CODEC_OpSub(32767, 0.005)))*/
#define MED_EANR_ALPHA_COEF                     (27852)                         /*=27852(0.85->27852 Q15)����Ӧ������ƽ��ϵ��*/
#define MED_EANR_SPEECHDEC_THRESHOLDA           (31277)                         /*=31277 (4.6*Bmin = 7.636->31277   Q12)����Ӧ��������ֵ*/
#define MED_EANR_SPEECHDEC_THRESHOLDB           (22710)                         /*=22710 (1.67*Bmin = 2.7722->22710 Q13)����Ӧ��������ֵ*/
#define MED_EANR_SPEECHDEC_MIN_COMPENS          (27197)                         /*= 27197 (1.66->27197 Q14)����Сֵ����*/
#define MED_EANR_SPEECHPROB_STAT                (2)                             /*�����ȶ��Բ����������ȶ�����������ѡȡӰ�첻��*/
#define MED_EANR_SPEECHPROB_SNR_SMTH_COEF       (31130)                         /*=31130(0.95->31130 Q15)����������ȵ�ƽ��ϵ��*/
#define MED_EANR_SPEECHPROB_THRESHOLDC          (24576)                         /*=24576(3.0->24576 Q13)���������㹫ʽ�б����ľ���ֵ*/
#define MED_EANR_NOISESPECT_ALPHA_COEF          (27853)                         /*=27853(0.85->27853 Q15)�����������е�ƽ��ϵ��*/
#define MED_EANR_NOISESPECT_ALPHA_COEF_L        (32440)                         /*=32440(0.99->32440 Q15)��������ʱƽ��ϵ��*/
#define MED_EANR_APSSMOOTH_ASNR_COEF            (22937)                         /*=22937(0.7->22937 Q15)����������ȵĳ�ʱƽ��*/
#define MED_EANR_PRO_BIN_IDXA                   (13)                            /*��ʼ�Ӵ�����*/
#define MED_EANR_PRO_BIN_IDXB                   (31)                            /*��ֹ�Ӵ�����*/
#define MED_EANR_PROABSENCE_BIN_IDXC            (2)                             /*��������Ⱦ�ֵ������ʼ�Ӵ�����*/
#define MED_EANR_PROABSENCE_SNR_H_LIMIT         (640)                           /*=640(10->640 Q6)����������Ⱦ�ֵ����ֵ���޷�����*/
#define MED_EANR_PROABSENCE_SNR_L_LIMIT         (0)                             /*=0����������Ⱦ�ֵ����ֵ���޷�����*/
#define MED_EANR_PROABSENCE_ABSENCE_PRO_MAX     (32702)                         /*=32702(0.998->32702 Q15)�����������ڸ��ʵļ�ֵ*/
#define MED_EANR_PSTSNRMAX                      (522240)                        /*������������ֵ����(255->522240 Q11)*/
#define MED_EANR_MAX16                          (32767)                         /*MED_INT16���ܱ�ʾ������������ദ������ʾ1(Q15)*/
#define MED_EANR_MAX11                          (2047)                          /*12bit���ܱ�ʾ���ֵ�����㻯�����г���*/
#define MED_EANR_Q11                            (2048)                          /*2^11,���㻯�����г���*/
#define MED_EANR_Q7                             (128)                           /*2^7, ���㻯�����г���*/
#define MED_EANR_Q13                            (8192)                          /*2^13,���㻯�����г���*/
#define MED_EANR_Q14                            (16384)                         /*2^14,���㻯�����г���*/
#define MED_EANR_SMTHSTARTFRM                   (14)                            /*��Сֵ������ʼ����֡���*/
#define MED_EANR_SNRFACT_MAX                    (32650)                         /*32650�������������15.9424->32650 Q11, EXP(15.9424)<<8 = 2^31 */
#define MED_EANR_NOISESPECT_PROBTHD             (32750)                         /*������ƽ��ʱ�������ڸ������ޣ�����������������ƽ����*/
#define MED_EANR_NOISESPECT_NOISECOMPENS        (24060)                         /*���������������ӣ�1.4685->24060 Q14*/
#define MED_EANR_PRESENCEPROB_TH1               (13)                            /*0.1->13(Q7),Local������������޵ͽ磬С�ڴ�����Local������������Ϊ��Сֵ*/
#define MED_EANR_PRESENCEPROB_TH2               (40)                            /*0.3162->40(Q7),Local������������޸߽磬���ڴ�����Local������������Ϊ���ֵ*/
#define MED_EANR_PRESENCEPROB_FRMTH             (120)                           /*������������ʱ��ʱ��֡����*/
#define MED_EANR_PRESENCEPROB_POWTH             (20480)                         /*2.5->20480(Q13)������������ʱ�Ĺ�������*/
#define MED_EANR_PROB_MIN                       (164)                           /*0.005->164(Q15)��������������Сֵ*/
#define MED_EANR_ABSENCEPROB_SNRTH              (-640)                          /*������������ʱ��������ޣ�-10 -> -640(Q6)*/
#define MED_EANR_ABSENCEPROB_SNRPENALTYA        (320)                           /*������������ʱ����Ȳ�������,5 ->320(Q6)*/
#define MED_EANR_ABSENCEPROB_SNRPENALTYB        (640)                           /*������������ʱ����Ȳ�������,10->640(Q6)*/
#define MED_EANR_ABSENCEPROB_DIVISOR            (5)                             /*������������ʱ�ĳ���*/
#define MED_EANR_GAIN_ABSENCEPROBTH             (29491)                         /*��������ʱ�����������ޣ�  0.9->29491(Q15)*/
#define MED_EANR_GAIN_SNRFACTORTH               (10240)                         /*��������ʱ������������ޣ�5  ->10240(Q11)*/
#define MED_EANR_GAIN_SMOOTHCOEF                (6554)                          /*��������ʱ������ƽ��ϵ����0.2->6554(Q15)*/
#define MED_EANR_GAIN_UPLIMIT                   (4095)                          /*�����Ͻ磬1->4095(Q12)*/
#define MED_EANR_PRISNRMAXLIMIT                 (32640)                         /*����������Ͻ磬255->32640(Q7)*/
#define MED_EANR_MINPOWERTHD                    (16384)                         /*��С������ֵ*/
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
 ʵ������  : MED_EANR_COMM_STRU
 ��������  : ����EANR���нṹ��
*****************************************************************************/
typedef struct
{
    MED_INT32 swIdx;                                                            /*ʱ��֡����ָʾ*/
    MED_INT16 shwZfNum;                                                         /*ȫ��֡����ָʾ*/
    MED_INT16 shwIdxSwitch;                                                     /*��֡������Сֵ�洢���ݸ���ָʾ*/
    MED_INT16 shwNormShift;                                                     /*ʱƵ����������bit��*/
    MED_INT16 shwRev;
    MED_INT32 aswNoisePow[MED_EANR_MAXBINNUM];                                  /*����������*/
    MED_INT32 aswNoisePowRud[MED_EANR_MAXBINNUM];                               /*δ����������������*/
    MED_INT32 aswPowSpctSmth[MED_EANR_MAXBINNUM];                               /*ʱƵƽ��������*/
    MED_INT32 aswPowSpctCond[MED_EANR_MAXBINNUM];                               /*���������о�ָʾ��ƽ����Ĺ����׹���*/
    MED_INT32 aswPowSpctMin[MED_EANR_MAXBINNUM];                                /*ʱ�䴰��ƽ����������Сֵ*/
    MED_INT32 aswPowSpctMinC[MED_EANR_MAXBINNUM];                               /*ʱ�䴰������ƽ����������Сֵ*/
    MED_INT32 aswPowSpctMinCurt[MED_EANR_MAXBINNUM];                            /*��ǰƽ����������Сֵ*/
    MED_INT32 aswPowSpctMinCurtC[MED_EANR_MAXBINNUM];                           /*��ǰ����ƽ����������Сֵ*/
    MED_INT16 ashwSNRSmth[MED_EANR_MAXBINNUM];                                  /*ƽ���������*/
    MED_INT16 ashwPresenIndSmth[MED_EANR_MAXBINNUM];                            /*ƽ������������ָʾ*/
    MED_INT16 shwSubBandNum;                                                    /*�Ӵ���Ŀ*/
    MED_INT16 shwOpertBandNum;                                                  /*������Ӵ���*/
}MED_EANR_COMM_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_TFSMOOTH_STRU
 ��������  : ����MED_EANR_TFSmooth_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT32 aswPowSpctSmth[MED_EANR_MAXBINNUM];                               /*ʱƵƽ��������*/
    MED_INT32 aswPowSpct[MED_EANR_MAXBINNUM];                                   /*ԭʼ������*/
}MED_EANR_TFSMOOTH_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_SPEECHDEC_STRU
 ��������  : ����MED_EANR_SpeechDec_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT16 ashwSpechIdx[MED_EANR_MAXBINNUM];                                 /*��ʼ������(1)������(0)ָʾ*/
}MED_EANR_SPEECHDEC_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_SPEECHPROB_STRU
 ��������  : ����MED_EANR_SpeechProb_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT16 shwSNRMin;                                                        /*�����������Сֵ*/
    MED_INT16 ashwPriSNR[MED_EANR_MAXBINNUM];                                   /*���������*/
    MED_INT16 ashwSpchProb[MED_EANR_MAXBINNUM];                                 /*����������������*/
}MED_EANR_SPEECHPROB_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_MINTRACKUPDATE_STRU
 ��������  : ����MED_EANR_MinTrackUpdate_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT32 aswPowStck[MED_EANR_DATAWINLEN];                                  /*��ʱ�洢����,���ڴ洢������*/
    MED_INT32 aswCondPowStck[MED_EANR_DATAWINLEN];                              /*��ʱ�洢����,���ڴ洢����������*/
}MED_EANR_MINTRACKUPDATE_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_NOISESPECT_STRU
 ��������  : ����MED_EANR_NoiseSpect_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT32 aswNoisePowLong[MED_EANR_MAXBINNUM];                              /*��ʱƽ��������������*/
}MED_EANR_NOISESPECT_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_SNRSMOOTH_STRU
 ��������  : ����MED_EANR_SNRSmooth_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT16 ashwSNRSmthGlb[MED_EANR_MAXBINNUM];                               /*��������ȵ�Globalƽ�����*/
    MED_INT16 ashwSNRSmthLcl[MED_EANR_MAXBINNUM];                               /*��������ȵ�Localƽ�����*/
}MED_EANR_SNRSMOOTH_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_PRESENCEPROB_STRU
 ��������  : ����MED_EANR_PresenceProb_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT16 ashwProbLcl[MED_EANR_MAXBINNUM];                                  /*Local��������*/
    MED_INT16 ashwProbGlb[MED_EANR_MAXBINNUM];                                  /*Global��������*/
}MED_EANR_PRESENCEPROB_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_ABSENCEPROB_STRU
 ��������  : ����MED_EANR_AbsenceProb_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT16 shwMeanPriSNR;                                                    /*��������ȵľ�ֵ*/
    MED_INT16 shwPriSNR;                                                        /*��������Ⱦ�ֵ����ֵ���޷�ֵ*/
    MED_INT16 ashwAbsenProb[MED_EANR_MAXBINNUM];                                /*��������*/
}MED_EANR_ABSENCEPROB_STRU;

/*****************************************************************************
 ʵ������  : MED_EANR_GAIN_STRU
 ��������  : ����MED_EANR_Gain_FIX�����ṹ��
*****************************************************************************/
typedef struct
{
    MED_INT16 ashwPriSNRMid[MED_EANR_MAXBINNUM];                                /*������������ȹ����м����*/
    MED_INT16 shwSNRSqrtMin;                                                    /*��С����*/
    MED_INT16 ashwGain[MED_EANR_MAXBINNUM];                                     /*����*/
}MED_EANR_GAIN_STRU;


typedef struct
{
    MED_OBJ_HEADER
    MED_EANR_COMM_STRU                           stCommon;                      /*���ýṹ��*/
    MED_EANR_TFSMOOTH_STRU                       stTFSmooth;                    /*ʱƵ��ƽ���ṹ��*/
    MED_EANR_SPEECHDEC_STRU                      stSpeechDec;                   /*�������нṹ��*/
    MED_EANR_SPEECHPROB_STRU                     stSpeechProb;                  /*���Ե��������ڸ��ʼ���ṹ��*/
    MED_EANR_MINTRACKUPDATE_STRU                 stMinTrackUpdate;              /*��������Сֵ�������½ṹ��*/
    MED_EANR_NOISESPECT_STRU                     stNoiseSpect;                  /*�������ʼ���ṹ��*/
    MED_EANR_SNRSMOOTH_STRU                      stSnrSmooth;                   /*��������ȼ���ṹ��*/
    MED_EANR_PRESENCEPROB_STRU                   stPresenceProb;                /*�������ڸ��ʼ���ṹ��*/
    MED_EANR_ABSENCEPROB_STRU                    stAbsenceProb;                 /*���������ڸ��ʼ���ṹ��*/
    MED_EANR_GAIN_STRU                           stGain;                        /*�������ṹ��*/
    MED_INT16                                    shwEnable;                                                        /*EANRʹ�ܿ��ƣ�0Ϊ�رգ�1Ϊ��*/
    MED_INT16                                    shwReduceDB;
    MED_INT16                                    shwZeroFlagTh;
    MED_INT16                                    shwReserve;
}MED_EANR_STRU;


/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern MED_EANR_STRU    g_astMedEanrObj[MED_EANR_MAX_OBJ_NUM];                  /* ANRģ��ȫ�ֿ���ʵ�� */
extern MED_OBJ_INFO     g_stMedEanrObjInfo;                                     /* ANR��Ϣ */
extern MED_INT16        MED_EANR_SUBBAND_FL[40];                                /*�Ӵ���������*/
extern MED_INT16        MED_EANR_SUBBAND_FH[40];                                /*�Ӵ���������*/
extern MED_INT16        MED_EANR_SNR_WIN_GLB[31];                               /*���������Globalƽ����ϵ��*/


/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_VOID* MED_EANR_1MIC_Create( VOS_VOID );
extern VOS_UINT32  MED_EANR_1MIC_Destroy(VOS_VOID  **ppInstance);
extern VOS_UINT32  MED_EANR_1MIC_GetPara(
                       VOS_VOID               *pInstance,
                       VOS_INT16              *pshwPara,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32  MED_EANR_1MIC_Main(
                       VOS_VOID                    *pInstance,
                       VOS_INT16                   *pshwFrmFreq,
                       VOS_INT16                   *pshwFrmFreqRef,
                       VOS_INT16                    shwNormShift);
extern VOS_UINT32 MED_EANR_1MIC_SetPara(
                       VOS_VOID                          *pInstance,
                       VOS_INT16                         *pshwPara,
                       VOS_INT16                          shwParaLen,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32  enSampleRate);
extern void MED_EANR_AbsenceProb_FIX(MED_EANR_COMM_STRU *stCommon,
                                     MED_EANR_ABSENCEPROB_STRU *stAbsenceProb,
                                     MED_INT16 *ashwProbLcl,
                                     MED_INT16 *ashwProbGlb);
extern void MED_EANR_CondiSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                                     MED_INT32 *aswPowSpctSmth,
                                     MED_INT32 *aswPowSpct,
                                     MED_INT16 *ashwSpechIdx);
extern void MED_EANR_ConvSmooth(MED_INT16 *ashwWin,
                                MED_INT16 *pshwIn,
                                MED_INT16 *pshwOut,
                                MED_INT16 shwBinNum);
extern void MED_EANR_DATA_FIX(MED_EANR_COMM_STRU *stCommon,
                              MED_INT16 *ashwGain,
                              MED_INT16 *shwDataBuf);
extern void MED_EANR_FastSmooth_16(MED_INT16 *pshwIn, MED_INT16 *pshwOut, MED_INT16 shwLen);
extern void MED_EANR_FastSmooth_32(MED_INT32 *pswIn, MED_INT32 *pswOut, MED_INT16 shwLen);
extern void MED_EANR_FastSmooth_Shl2(MED_INT16 *pshwIn, MED_INT16 *pshwOut, MED_INT16 shwLen);
extern void MED_EANR_Gain_FIX(MED_EANR_COMM_STRU *stCommon,
                              MED_EANR_GAIN_STRU *stGain,
                              MED_INT16 shwSNRMin,
                              MED_INT32 *aswPowSpct,
                              MED_INT16 *ashwAbsenProb);
extern void MED_EANR_MinTrackUpdate_FIX(MED_EANR_COMM_STRU *stCommon,
                                        MED_EANR_MINTRACKUPDATE_STRU *stMinTrackUpdate,
                                        MED_INT32 *aswPowSpctSmth);
extern void MED_EANR_MinTrack_FIX(MED_EANR_COMM_STRU *stCommon, MED_INT32 *aswPowSpctSmth);
extern void MED_EANR_NoiseSpect_FIX(MED_EANR_COMM_STRU *stCommon,
                                    MED_EANR_NOISESPECT_STRU *stNoiseSpect,
                                    MED_INT16 *ashwSpchProb,
                                    MED_INT32 *aswPowSpct);
extern void MED_EANR_PresenceProb_FIX(MED_EANR_COMM_STRU *stCommon,
                                      MED_EANR_PRESENCEPROB_STRU *stPresenceProb,
                                      MED_INT32 *aswNoisePowLong,
                                      MED_INT16 *ashwSNRSmthLcl,
                                      MED_INT16 *ashwSNRSmthGlb);
extern void MED_EANR_SNRSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                                   MED_EANR_SNRSMOOTH_STRU *stSnrSmooth,
                                   MED_INT16 *ashwPriSNR);
extern void MED_EANR_SpeechDec_FIX(MED_EANR_COMM_STRU *stCommon,
                                   MED_EANR_SPEECHDEC_STRU *stSpeechDec,
                                   MED_INT32*aswPowSpctSmth,
                                   MED_INT32 *aswPowSpct);
extern void MED_EANR_SpeechProb_FIX(MED_EANR_COMM_STRU *stCommon,
                                    MED_EANR_SPEECHPROB_STRU *stSpeechProb,
                                    MED_INT16 *ashwPriSNRMid,
                                    MED_INT32 *aswPowSpctSmth,
                                    MED_INT32 *aswPowSpct);
extern void MED_EANR_TFSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                                  MED_EANR_TFSMOOTH_STRU *stTFSmooth,
                                  MED_INT16 *shwDataBuf);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_eanr.h */
