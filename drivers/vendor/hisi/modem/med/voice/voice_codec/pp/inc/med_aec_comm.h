
#ifndef _MED_AEC_COMM_H_
#define _MED_AEC_COMM_H_

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_op_float.h"
#include "codec_op_etsi.h"
#include "codec_op_netsi.h"
#include "codec_op_lib.h"
#include "med_fft.h"
#include "codec_com_codec.h"
#include "ucom_comm.h"
#include "med_pp_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/* FFT IFFT�ص����� */
typedef VOS_VOID (*MED_AEC_FFT_CALLBACK)(VOS_INT16 *, VOS_INT16 *);

#define MED_AEC_MAX_FFT_LEN             (640)                                   /* ���FFT���� */
#define MED_AEC_HF_AF_M                 (2)

/* ��ʱȫ�����飬��AEC��ģ��ʹ�� */
extern VOS_INT16                        g_ashwMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];
extern VOS_INT16                        g_ashwMedAecTmp2Len640[MED_AEC_MAX_FFT_LEN];
extern VOS_INT32                        g_aswMedAecTmp1Len320[CODEC_FRAME_LENGTH_WB];
extern VOS_INT32                        g_aswMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];

/* AEC �����궨��*/
#define MED_AEC_GetshwVecTmp640Ptr1()   (&g_ashwMedAecTmp1Len640[0])            /* ��ʱȫ������ָ�� ����640 INT16 */
#define MED_AEC_GetshwVecTmp640Ptr2()   (&g_ashwMedAecTmp2Len640[0])            /* ��ʱȫ������ָ�� ����640 INT16 */
#define MED_AEC_GetswVecTmp320Ptr1()    (&g_aswMedAecTmp1Len320[0])             /* ��ʱȫ������ָ�� ����320 INT32 */
#define MED_AEC_GetswVecTmp640Ptr1()    (&g_aswMedAecTmp1Len640[0])             /* ��ʱȫ������ָ�� ����640 INT32 */

#define MED_AEC_OFFSET_THD              (60)                                    /* �ӳٲ�����ֵ */
#define MED_AEC_MAX_OFFSET              (960)                                   /* ��󲹳����ȣ���λ������ Q0*/
#define MED_AEC_MAX_TAIL_LEN            (960)                                   /* ���β�˳��ȣ���λms�����֧��60msβ���ӳ� Q0*/

#define MED_AEC_MAX_DTD_ST_MODI_THD     (5000)                                  /* DTD���������������ֵ Q0*/
#define MED_AEC_MAX_DTD_DT_MODI_THD     (30000)                                 /* DTD˫�������������ֵQ15*/
#define MED_AEC_MAX_DTD_POWER_THD       (30000)                                 /* DTD�����������ֵ Q0*/
#define MED_AEC_MAX_NLP_DT2ST_THD       (20)                                    /* NLP˫���л������������ֵ Q0*/
#define MED_AEC_MAX_NLP_ST2DT_THD       (20)                                    /* NLP�����л�˫���������ֵ Q0*/
#define MED_AEC_MAX_NLP_CNG_THD         (2000)                                  /* NLP�������������������ֵ Q0*/
#define MED_AEC_MAX_NLP_NOISE_FLOOR_THD (256)                                   /* NLP���������������ֵ Q0*/
#define MED_AEC_MAX_NLP_MAX_SUPPS_LVL   (32767)                                 /* NLP��Ƶ���Ƽ������ֵ Q15*/
#define MED_AEC_MAX_NLP_NON_LINEAR_THD  (5000)                                  /* NLP��Ƶ�����������������ֵ Q0*/
#define MED_AEC_MAX_DTD_EST_POWER_THD   (30000)                                 /* ��ǿ����ʹ��ʱ������ֵ���ֵ Q0*/
#define MED_AEC_MAX_DTD_EST_AMPL_THD    (30000)                                 /* ��ǿ����ʹ��ʱ������ֵ���ֵ Q0*/
#define MED_AEC_DTD_EST_POWER_BASE      (1000)                                  /* ��ǿ����ʹ��ʱ������ֵ��λֵ Q0*/
#define MED_AEC_DTD_EST_HANG_OVER_LEN   (4)                                     /* ��ǿ����ʹ��ʱhangover֡���� Q0*/

#define MED_AEC_MAX_DTD_NLINE_NEAR_FAR_RATIO_GAIN   (32765)                     /* ������DTDԶ�˹������ܶȵ��������ֵ Q11 */
#define MED_AEC_MAX_DTD_NLINE_SP_SER_THD            (32765)                     /* ������DTD�������ڸ���Ϊ1��SER�������ֵ Q11 */
#define MED_AEC_MAX_DTD_NLINE_ECHO_SER_THD          (32765)                     /* ������DTD�������ڸ���Ϊ0��SER�������ֵ Q11 */
#define MED_AEC_MAX_DTD_NLINE_BAND_PSD_MUTE_THD     (32765)                     /* ������DTD���˹������ܶ���Ϊ�������������ֵ Q0 */

#define MED_AEC_MAX_DTD_NLINE_SP_THD_INIT           (32000)                     /* ������DTD˫���ж��������ڸ�����ֵ�����ֵ Q15 */
#define MED_AEC_MAX_DTD_NLINE_SP_THD_MAX            (32000)                     /* ������DTD˫���ж��������ڸ�����ֵ���ֵ�����ֵ Q15 */
#define MED_AEC_MAX_DTD_NLINE_SP_THD_MIN            (32000)                     /* ������DTD˫���ж��������ڸ�����ֵ��Сֵ�����ֵ Q15 */
#define MED_AEC_MAX_DTD_NLINE_SUM_PSD_THD           (32000)                     /* ������DTD˫���ж�ʣ��������ֵ���ֵ�����ֵ Q15 */

#define MED_AEC_MAX_NLP_OVERDRIVE_FAR_CNT           (30000)                     /* Զ���źż������ֵ  Q0 */
#define MED_AEC_MAX_NLP_OVERDRIVE_FAR_THD           (30000)                     /* Զ���źż����������ֵ  Q0 */
#define MED_AEC_MAX_NLP_OVERDRIVE_MAX               (32000)                     /* OVERDRIVE���ֵ  Q0 */
#define MED_AEC_MAX_NLP_STSUPPRESS_ALPH             (32000)                     /* OVERDRIVE���ֵ  Q0 */
#define MED_AEC_MAX_NLP_STSUPPRESS_POWTHD           (10000)                     /* OVERDRIVE���ֵ  Q0 */
#define MED_AEC_MAX_NLP_SMOOTH_GAIN_DOD             (32767)                     /* �������ӵ���ָ�������ֵ*/
#define MED_AEC_MAX_NLP_BANDSORT_IDX                (60)                        /*  NLP: �������������� */
/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/* AF FFT���� */
enum MED_AEC_AF_FFT_LEN_ENUM
{
    MED_AEC_AF_FFT_LEN_NB = 320,                                                /* խ��FFT���� */
    MED_AEC_AF_FFT_LEN_WB = 640,                                                /* ���FFT���� */
    MED_AEC_AF_FFT_LEN_BUTT
};
typedef VOS_INT16  MED_AEC_AF_FFT_LEN_ENUM_INT16;

/* NLP FFT���� */
enum MED_AEC_NLP_FFT_LEN_ENUM
{
    MED_1MIC_AEC_NLP_FFT_LEN_NB = 256,                                          /* 1MICխ��FFT���� */
    MED_1MIC_AEC_NLP_FFT_LEN_WB = 512,                                          /* 1MIC���FFT���� */
    MED_2MIC_AEC_NLP_FFT_LEN_NB = 320,                                          /* 2MICխ��FFT���� */
    MED_2MIC_AEC_NLP_FFT_LEN_WB = 640,                                          /* 2MIC���FFT���� */
    MED_AEC_NLP_FFT_LEN_BUTT
};
typedef VOS_INT16  MED_AEC_NLP_FFT_LEN_ENUM_INT16;

/* DTD����־ */
enum MED_AEC_DTD_FLAG_ENUM
{
    MED_AEC_DTD_FLAG_ST,                                                        /* ���� */
    MED_AEC_DTD_FLAG_DT,                                                        /* ˫�� */
    MED_AEC_DTD_FLAG_PASS,                                                      /* ȫͨ */
    MED_AEC_DTD_FLAG_BUTT
};
typedef VOS_INT16 MED_AEC_DTD_FLAG_ENUM_INT16;

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  5 STRUCT����
*****************************************************************************/

/* AEC������ʼ���ӿ� */
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enAecEnable;                            /*AECʹ�ܿ��� */
    VOS_INT16                           shwAfTailLen;                           /*����β�˳���, ��λ������*/
    VOS_INT16                           shwOffsetLen;                           /*�̶��ӳ�ƫ�Ƴ���, ��λ����*/
    VOS_INT16                           shwDtdMutePowerThd;                     /*��ΪPASS��������ֵ*/
    VOS_INT16                           shwDtdStModiThd;                        /*����������ֵ*/
    VOS_INT16                           shwDtdDtModiThd;                        /*˫��������ֵ*/
    VOS_INT16                           shwNlpDt2StLvl;                         /*˫���е�������*/
    VOS_INT16                           shwNlpSt2DtLvl;                         /*������˫������*/
    VOS_INT16                           shwNlpCngInsertLvl;                     /*����������������*/
    VOS_INT16                           shwNlpNoiseFloorThd;                    /*����������ֵ*/
    VOS_INT16                           shwNlpMaxSuppressLvl;                   /*��Ƶ������Ƽ���*/
    VOS_INT16                           shwNlpNonlinearThd;                     /*��Ƶ��������������*/
    CODEC_SWITCH_ENUM_UINT16            enEstFlag;                              /*DTD��ǿ����(EST)ʹ�ܿ���*/
    VOS_INT16                           shwEstPowerThd;                         /*DTD��ǿ����(EST)������ֵ*/
    VOS_INT16                           shwEstAmplThd;                          /*DTD��ǿ����(EST)������ֵ*/
    CODEC_SWITCH_ENUM_UINT16            enNearFarRatioEnable;                   /* ������DTD: ������Զ�˹������ܶȱ�ֵʹ�� */
    VOS_INT16                           shwNearFarRatioActiveFrmNum;            /* ������DTD: ��ʼ��˫������Զ���ź�����֡��  */
    VOS_INT16                           shwNearFarRatioGain;                    /* ������DTD: Զ�˹������ܶȵ����� */
    VOS_INT16                           shwSpSerThd;                            /* ������DTD: �������ڸ���Ϊ1��SER���� */
    VOS_INT16                           shwEchoSerThd;                          /* ������DTD: �������ڸ���Ϊ0��SER���� */
    VOS_INT16                           shwBandPsdMuteThd;                      /* ������DTD: ���˹������ܶ���Ϊ���������� */
    VOS_INT16                           shwSpThdInit;                           /* ������DTD: �������������б���ֵ��ʼֵ */
    VOS_INT16                           shwSpThdMax;                            /* ������DTD: �������������б���ֵ���ֵ */
    VOS_INT16                           shwSpThdMin;                            /* ������DTD: �������������б���ֵ��Сֵ */
    VOS_INT16                           shwSumPsdThd;                           /* ������DTD: �������������б���ֵ */
    VOS_INT16                           shwReserve1;                            /* ����1 */
    VOS_INT16                           shwReserve2;                            /* ����2 */
    VOS_INT16                           shwReserve3;                            /* ����3 */
    VOS_INT16                           shwNlpBandProbSupFastAlpha;             /* NLP: ���˲�ϵ�� */
    VOS_INT16                           shwNlpBandProbSupSlowAlpha;             /* NLP: ���˲�ϵ�� */
    CODEC_SWITCH_ENUM_UINT16            enEAecEnable;                           /* AEC: ��ǿ����AECʹ�ܿ��� */
    VOS_INT16                           shwNlpRefCnt;                           /* NLP: Զ���źż��� */
    VOS_INT16                           shwNlpRefAmp1;                          /* NLP: Զ���ź���������1���������Ƶ�һ������ */
    VOS_INT16                           shwNlpExOverdrive;                      /* NLP: ��ǿ��Overdrive */
    VOS_INT16                           shwReserve4;                            /* ����4 */
    VOS_INT16                           shwNlpResdPowAlph;                      /* NLP: �в��ź������˲�ϵ�� */
    VOS_INT16                           shwNlpResdPowThd;                       /* NLP: �в��ź��������� */
    VOS_INT16                           shwNlpSmoothGainDod;                    /* NLP: �������ӵ���ָ��"���С��0�������ʵ�ʵ���ָ����������ָ������ָ��" */
    VOS_INT16                           shwNlpBandSortIdx;                      /* NLP: խ������ĸ� λ��MED_AEC_HF_ECHO_BIN_RANGE*(3/4)or(1/2)-1 */
    VOS_INT16                           shwNlpBandSortIdxLow;                   /* NLP: խ������ĵ� λ��MED_AEC_HF_ECHO_BIN_RANGE*(1/2)or(3/10)-1*/

} MED_AEC_NV_STRU;
/*****************************************************************************
  6 UNION����
*****************************************************************************/

/*****************************************************************************
  7 OTHERS����
*****************************************************************************/

/*****************************************************************************
  8 ��������
*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

