

#ifndef _MED_AEC_DTD_H_
#define _MED_AEC_DTD_H_

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "med_aec_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* ������������DTD�궨�� */
#define MED_AEC_DTD_MEAN_ALPHA_NB       (205)                                   /* խ����ʼMeanAlpha */
#define MED_AEC_DTD_MEAN_ALPHA_WB       (102)                                   /* �����ʼMeanAlpha */
#define MED_AEC_DTD_MUTE_ALPHA          (24576)                                 /* �������ƽ��ϵ�� */
#define MED_AEC_DTD_MUTE_HO_THD         (30)                                    /* ����������ֵ */
#define MED_AEC_DTD_ST_FLR_THD          (16)                                    /* ������ֵ�����ھ���С���ź�, ���ڴ�ֱֵ����Ϊ���� */
#define MED_AEC_DTD_SPEC_ALPHA          (9830)                                  /* ������ƽ��ϵ�� */
#define MED_AEC_DTD_CORR_THD            (31130)                                 /* ��˫�����ϵ����ֵ */
#define MED_AEC_DTD_BETA_MIN            (328)                                   /* beta ��Сֵ */
#define MED_AEC_DTD_BETA_MAX            (32767)                                 /* beta ���ֵ */

/* ������DTD�б�궨�� */
#define MED_AEC_DTD_NLINE_FREQ_BIN_LEN_NB            (161)                      /* խ��Ƶ�׸��� */
#define MED_AEC_DTD_NLINE_FREQ_BIN_LEN_WB            (321)                      /* ���Ƶ�׸��� */
#define MED_AEC_DTD_NLINE_PSD_BAND_LEN_NB            (32)                       /* խ���Ӵ����� */
#define MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB            (40)                       /* ����Ӵ����� */
#define MED_AEC_DTD_NLINE_DT_CONTINUOUS_LEN          (6)                        /* ˫��hangover����֡����ֵ 6bit Q0 6 */
#define MED_AEC_DTD_NLINE_DT_HANGOVER_LEN            (6)                        /* ˫��hangover֡�� 16bit Q0 6 */
#define MED_AEC_DTD_NLINE_SP_BAND_PROB_INIT_VALUE    (16384)                    /* �Ӵ������������ڸ��ʳ�ʼֵ 16bit Q15 0.5 */
#define MED_AEC_DTD_NLINE_CAL_SP_PROB_BAND_LEN       (15)                       /* �����Ӵ������������ڸ���ʹ���Ӵ��� 16bit Q0 31 */
#define MED_AEC_DTD_NLINE_PSD_BAND_SMOOTH_ALPHA      (26214)                    /* �����Ӵ�������ƽ��ϵ�� 16bit Q15 0.8 */
#define MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_LEN    (5)                        /* �����Ӵ������������ڸ��ʴ���ƽ������ */
#define MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_INDEX  (19)                       /* �����Ӵ������������ڸ��ʴ���ƽ����ʼ�Ӵ����� */
#define MED_AEC_DTD_NLINE_SP_BAND_PROB_SMOOTH_ALPHA  (32767)                    /* �����Ӵ������������ڸ���ƽ��ϵ�� 16bit Q15 1 */
#define MED_AEC_DTD_NLINE_SP_PROB_MEAN_SMOOTH_ALPHA  (3276)                     /* �����Ӵ������������ڸ���ƽ��ֵƽ��ϵ�� 16bit Q15 0.7*/
#define MED_AEC_DTD_NLINE_SP_SER_THD                 (32767)                    /* �������������Żر� 16bit Q8 8 */
#define MED_AEC_DTD_NLINE_ECHO_SER_THD               (8192)                     /* ���������������Żر� 16bit Q8 2 */
#define MED_AEC_DTD_NLINE_BAND_PSD_MUTE_THD          (25600)                    /* �Ӵ������׾���֡�ж���ֵ 32bit Q8 100*/
#define MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_SMALL     (82)                       /* ���������ж�����������ֵС���� 16bit Q15 0.0025 */
#define MED_AEC_DTD_NLINE_SP_PROB_THD_STEP_LARGE     (655)                      /* ���������ж�����������ֵ�󲽳� 16bit Q15 0.02*/
#define MED_AEC_DTD_NLINE_SUM_PSD_ALPHA              (3277)                     /* ���������ж���������ƽ��ϵ�� 16bit Q15 0.1 */
#define MED_AEC_DTD_NLINE_SUM_PSD_THD_BASE           (100)                      /* ���������ж�������ֵNV������ʵ�������� 32bit Q8 100 */


#define MED_AEC_DtdNlineGetSerModulTabPtr()          (&g_ashwDtdBandSERThdModulTab[0])
#define MED_AEC_DtdNlineGetProbAlphaCoeffPtr()       (&g_ashwSpBandProbAlphaCoeff[0])

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  5 STRUCT����
*****************************************************************************/
/* DTD״̬ �����жϽṹ�� */
typedef struct
{
    VOS_INT16                           shwFrameLen;                            /* ֡�� */
    MED_AEC_AF_FFT_LEN_ENUM_INT16       enFftLen;                               /* FFT���� */
    VOS_INT16                           shwMeanAlpha;                           /* ��ֵ����ƽ��ϵ�� */
    VOS_INT16                           shwStModiThd;                           /* ������������ */
    VOS_INT16                           shwDtModiThd;                           /* ˫���������� */
    VOS_INT16                           shwStFlrThd;                            /* ���������������� */
    VOS_INT32                           aswPdy[CODEC_FRAME_LENGTH_WB * 2];  /* ���˺Ͳв��źŵ�ƽ���������� */
    VOS_INT32                           aswPdd[CODEC_FRAME_LENGTH_WB * 2];  /* ����ƽ���Թ����� */
}MED_AEC_DTD_LINE_STRU;

/* DTD״̬ �������жϽṹ�� */
typedef struct
{
    MED_AEC_AF_FFT_LEN_ENUM_INT16       enFftLen;                               /* FFT���� */
    VOS_INT16                           shwFreqBinLen;                          /* Ƶ�׸��� */
    VOS_INT16                           shwBandLen;                             /* �Ӵ����� */
    VOS_INT16                           shwReserved;
    VOS_INT32                           aswPsdBandNear[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB]; /* �����ź��Ӵ������� */
    VOS_INT32                           aswPsdBandFar[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB];  /* Զ���ź��Ӵ������� */
    VOS_INT32                           aswPsdBandEcho[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB]; /* ���ƻ����ź��Ӵ������� */
    VOS_INT16                           ashwSpBandProb[MED_AEC_DTD_NLINE_PSD_BAND_LEN_WB]; /* �����Ӵ����ڸ��� */
    VOS_INT16                           shwSpProbMean;                          /* �����Ӵ����ڸ���ƽ��ֵ */
    CODEC_SWITCH_ENUM_UINT16            enNearFarRatioEnable;                   /* ������Զ�˹������ܶȱ�ֵʹ�� */
    VOS_INT16                           shwNearFarRatioActiveFrmNum;            /* DTD�������ݲο�֡��*/
    VOS_INT16                           shwNearFarRatioActiveFrmCnt;
    VOS_INT16                           shwNearFarRatioGain;                    /* Զ�˹������ܶȵ����� */
    VOS_INT16                           shwSpSerThd;                            /* �������ڸ���Ϊ1��SER���� */
    VOS_INT16                           shwEchoSerThd;                          /* �������ڸ���Ϊ0��SER���� */
    VOS_INT16                           shwBandPsdMuteThd;                      /* ���˹������ܶ���Ϊ���������� */
    VOS_INT16                           shwSpThdInit;                           /* �������������б���ֵ��ʼֵ */
    VOS_INT16                           shwSpThd;                               /* �������������б���ֵ */
    VOS_INT16                           shwSpThdMax;                            /* �������������б���ֵ���ֵ */
    VOS_INT16                           shwSpThdMin;                            /* �������������б���ֵ��Сֵ */
    VOS_INT32                           swSumPsdThd;                            /* �������������б�������ֵ */
    VOS_INT32                           swSumPsd;                               /* �������������б����� */

}MED_AEC_DTD_NLINE_STRU;

/* DTD״̬ �ܽṹ�� */
typedef struct
{
    MED_AEC_DTD_FLAG_ENUM_INT16         enDtdFlag;                              /*��˫����־ */
    VOS_INT16                           shwFrameLen;                            /*֡�� */
    VOS_INT16                           shwDtConLen;                            /*����˫��֡�� */
    VOS_INT16                           shwDtHangLen;                           /*˫��hangover֡�� */
    VOS_INT16                           shwMutePowerThd;                        /*���������������*/
    VOS_INT16                           shwMuteHandover;                        /*��������л������� */
    VOS_INT32                           swPowerLast;                            /*��һ֡���� */
    MED_AEC_DTD_LINE_STRU               stDtdLine;                              /*����DTD�б�ṹ�� */
    MED_AEC_DTD_NLINE_STRU              stDtdNline;                             /*������DTD�б�ṹ�� */
    CODEC_SWITCH_ENUM_UINT16            enEstFlag;                              /*DTD��ǿ����(EST)ʹ�ܿ���*/
    VOS_INT16                           shwEstPowerThd;                         /*DTD��ǿ����(EST)������ֵ*/
    VOS_INT16                           shwEstAmplThd;                          /*DTD��ǿ����(EST)������ֵ*/
    VOS_INT16                           shwEstHangOver;                         /*DTD��ǿ����(EST)hangover֡��*/
    VOS_INT16                           shwFarLargeFlag;                        /*DTD��ǿ����(EST)�����������־*/
    VOS_INT16                           shwReserved;
}MED_AEC_DTD_STRU;


/*****************************************************************************
  6 UNION����
*****************************************************************************/

/*****************************************************************************
  7 OTHERS����
*****************************************************************************/

/*****************************************************************************
  8 ��������
*****************************************************************************/
extern MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdLine(
                MED_AEC_DTD_LINE_STRU         *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagPre,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwError,
                VOS_INT16                     *pshwNear);
extern MED_AEC_DTD_FLAG_ENUM_INT16 MED_AEC_DtdNline(
                MED_AEC_DTD_NLINE_STRU        *pstDtd,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlagLine,
                VOS_INT16                     *pshwErrFreq,
                VOS_INT16                     *pshwEchoFreq,
                VOS_INT16                     *pshwFarFreq);
extern VOS_VOID MED_AEC_DtdSpProb(
                MED_AEC_DTD_NLINE_STRU            *pstDtd,
                VOS_INT32                         *pswPsdBandNear,
                VOS_INT32                         *pswPsdBandFar,
                VOS_INT32                         *pswPsdBandEcho,
                VOS_INT16                         *pshwSpBandProb);
extern VOS_VOID MED_AEC_Dtd(
                        MED_AEC_DTD_STRU              *pstDtd,
                        VOS_INT16                     *pshwErrFreq,
                        VOS_INT16                     *pshwEchoFreq,
                        VOS_INT16                     *pshwFarFreq,
                        VOS_INT16                     *pshwError,
                        VOS_INT16                     *pshwNear);
extern VOS_VOID MED_AEC_DtdInit(
                       MED_AEC_NV_STRU                *pstAecParam,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                       MED_AEC_DTD_STRU               *pstDtd);
extern VOS_VOID MED_AEC_DtdMuteDetect(
                       MED_AEC_DTD_STRU              *pstDtd,
                       VOS_INT16                     *pshwFar);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_dtd.h*/

