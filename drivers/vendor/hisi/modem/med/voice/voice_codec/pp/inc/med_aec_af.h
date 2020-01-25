

#ifndef _MED_AEC_AF_H_
#define _MED_AEC_AF_H_

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
/* AF �궨��*/
#define MED_AEC_AF_FILT_NB              (320)                                   /* խ��MDF�˲��鳤�� */
#define MED_AEC_AF_FILT_WB              (640)                                   /* ���MDF�˲��鳤�� */
#define MED_AEC_AF_M_MAX                (3)                                     /* MDF�˲�������, ���֧��44ms�˲����� */
#define MED_AEC_AF_POWX_DITH            (10)                                    /* Զ���ź�����ƽ���Ŷ� */
#define MED_AEC_AF_XPOW_THR             (11469)                                 /* Զ���źŹ�����ƽ������ */
#define MED_AEC_AF_RESET_THR            ((VOS_INT32)1000000)                    /* AF��λ�о���ֵ */
#define MED_AEC_AF_RESET_NUM            (50)                                    /* AF��λ�о�������ֵ */
#define MED_AEC_AF_WEIGHT_POWER_THD     (8)

/* �˲���Ȩֵ���� */
#define MED_AEC_AF_W_QN_FILT            (12)                                    /* Filter������W ԭʼ���꾫�� */
#define MED_AEC_AF_W_QN_PROP            (18)                                    /* UpdateProp������W ԭʼ���꾫�� */


/* ����й©�ʵ�ϵ�� */
#define MED_AEC_AF_LEAK_BETA0           (1311)                                  /* (2.0*frame_size)/sampling_rate */
#define MED_AEC_AF_LEAK_BETA_MAX        (328)                                   /* (0.5*frame_size)/sampling_rate */
#define MED_AEC_AF_LEAK_SMOOTH_ALPHA    (655)                                   /* ����в���ز����ƾ�ֵ��ƽ��ϵ�� */
#define MED_AEC_AF_LEAK_MIN_LEAK        (164)                                   /* й©������Сֵ */
#define MED_AEC_AF_LEAK_THR             (983)                                   /* й©������ֵ */

/* �Ա������˲�����������ʱ�䴰����Ӧϵ�� */
#define MED_AEC_AF_DAVG1_COEF1          (19661)                                 /* Davg1�˲�ϵ��1��0.6 Q15 */
#define MED_AEC_AF_DAVG1_COEF2          (13107)                                 /* Davg1�˲�ϵ��2��0.4 Q15 */
#define MED_AEC_AF_DAVG2_COEF1          (27853)                                 /* Davg2�˲�ϵ��1��0.85 Q15 */
#define MED_AEC_AF_DAVG2_COEF2          (4915)                                  /* Davg2�˲�ϵ��1��0.15 Q15 */
#define MED_AEC_AF_DVAR1_COEF1_M        (11796)                                 /* Dvar1�˲�ϵ��1��0.36 Q15 */
#define MED_AEC_AF_DVAR1_COEF1_E        (-15)
#define MED_AEC_AF_DVAR1_COEF2          (5243)                                  /* Dvar1�˲�ϵ��2��0.16 Q15 */
#define MED_AEC_AF_DVAR2_COEF1_M        (23675)                                 /* Dvar2�˲�ϵ��1��0.7225 Q15 */
#define MED_AEC_AF_DVAR2_COEF1_E        (-15)
#define MED_AEC_AF_DVAR2_COEF2          (737)                                   /* Dvar2�˲�ϵ��1��0.0225 Q15 */
#define MED_AEC_AF_VAR1_THD_M           (16384)                                 /* ˫�˲���ѡ����ֵ1 0.5 Q15 */
#define MED_AEC_AF_VAR1_THD_E           (-15)
#define MED_AEC_AF_VAR2_THD_M           (16384)                                 /* ˫�˲���ѡ����ֵ2 0.25 Q15 */
#define MED_AEC_AF_VAR2_THD_E           (-16)
#define MED_AEC_AF_BACKTRACK_THD_M      (16384)                                 /* ˫�˲���ѡ����ֵ3 4 Q15 */
#define MED_AEC_AF_BACKTRACK_THD_E      (-12)
#define MED_AEC_AF_DBF_FLOOR            (10)                                    /* ��������Ŷ� */

/* ���㲽�� */
#define MED_AEC_AF_STEP_COEF            (26214)                                 /* �˲�������ƽ��ϵ�� */
#define MED_AEC_AF_STEP_COEF_N          (6554)                                  /* �˲�������ƽ��ϵ�� */
#define MED_AEC_AF_STEP_QN              (27)                                    /* �˲�����������λ�� */
#define MED_AEC_AF_STEP_TMP_COFE        (8192)                                  /* ��ʱ����ϵ�� 0.25 */

/* ��ֵ */
#define MED_AEC_AF_0_POINT_1_Q15        (3277)                                  /* 0.1   Q15 */
#define MED_AEC_AF_0_POINT_99_Q15       (32440)                                 /* 0.99  Q15 */
#define MED_AEC_AF_10000                (10000)                                 /* 10000 */
#define MED_AEC_AF_1000                 (1000)                                  /* 1000 */
#define MED_AEC_AF_100                  (100)                                   /* 100 */
#define MED_AEC_AF_DIV_DITH             (10)                                    /* �Ŷ� */

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/* �˲����ԱȽ�� */
enum MED_AEC_AF_CMP_ENUM
{
    MED_AEC_AF_CMP_SOSO,                                                        /* �˲����»��� */
    MED_AEC_AF_CMP_OK,                                                          /* �˲����ºܺ� */
    MED_AEC_AF_CMP_BAD,                                                         /* �˲����ºܲ� */
    MED_AEC_AF_CMP_BUTT
};
typedef VOS_UINT16  MED_AEC_AF_CMP_ENUM_UINT16;

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  5 STRUCT����
*****************************************************************************/
 /* �ӳٲ�������ʵ�� */
 typedef struct
 {
     VOS_INT16                          shwOffsetLen;                           /* �̶��ӳ�ƫ�Ƴ���, ��λ������, 0-480 */
     VOS_INT16                          shwReserve;
     VOS_INT16                          ashwOffsetBuf[MED_AEC_MAX_OFFSET + CODEC_FRAME_LENGTH_WB]; /* �ӳٻ��� */
 } MED_AEC_OFFSET_OBJ_STRU;

/* AF���ýṹ�� */
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16              enAfEnable;
    VOS_INT16                           shwTailLen;                             /*����β�˳���, ��λ������*/
}MED_AEC_AF_CONFIG_STRU;

/* ǰ���˲����Ƚ�״̬ */
typedef struct
{
    VOS_INT32                           swDiffAvg1;                             /* ��ֵ1 */
    VOS_INT32                           swDiffAvg2;                             /* ��ֵ2 */
    CODEC_OP_FLOAT_STRU                   stFloatDiffVar1;                        /* ����1 */
    CODEC_OP_FLOAT_STRU                   stFloatDiffVar2;                        /* ����2 */
}MED_AEC_AF_KEEPER_STRU;

/* й©����״̬*/
typedef struct
{
    VOS_INT32                           swPyy;                                  /* y/y���ֵ  */
    VOS_INT32                           swPey;                                  /* e/y���ֵ */
    VOS_INT16                           shwAvgErrAbs;                           /* |e|ƽ��ֵ */
    VOS_INT16                           shwAvgEchoAbs;                          /* |y|ƽ��ֵ */
}MED_AEC_AF_LEAK_STRU;

/* ��������ṹ�� */
typedef struct
{
    VOS_INT32                           aswStep[1+(MED_AEC_MAX_FFT_LEN/2)];  /* ����Ӧ����,32bit Q27 */
    VOS_INT32                           swSumAdapt;                             /* 32bit Q27 */
    VOS_INT16                           shwAdapted;                             /* ���������б�־ */
    VOS_INT16                           shwRER;                                 /* ����ѧϰ�� */
}MED_AEC_AF_STEP_STRU;

/* AF״̬ */
typedef struct
{
    MED_AEC_AF_CONFIG_STRU              stConfig;
    MED_AEC_AF_FFT_LEN_ENUM_INT16       enFftLen;                               /* FFT���� */
    MED_FFT_NUM_ENUM_INT16              enFftNumIndex;                          /* FFT��������ö�� */
    MED_FFT_NUM_ENUM_INT16              enIfftNumIndex;                         /* IFFT��������ö�� */
    VOS_INT16                           shwSampleRate;
    VOS_INT16                           shwFrameLen;
    VOS_INT16                           shwM;                                   /* MDF�ֿ���,����ΪMED_AEC_AF_MAX_BLOCK_NUM*/

    /* ʱƵ�ź� */
    VOS_INT16                           ashwInput[CODEC_FRAME_LENGTH_WB];   /* �����ź� */
    VOS_INT16                           ashwFar[CODEC_FRAME_LENGTH_WB];     /* Զ���ź� */
    VOS_INT16                           ashwLastFar[CODEC_FRAME_LENGTH_WB*2];/* ǰMED_AEC_MAX_FRM_LEN��������Ϊǰһ֡Զ���źţ���MED_AEC_MAX_FRM_LEN��������Ϊ��ǰ֡Զ���ź�  */
    VOS_INT16                           ashwFarFreq[MED_AEC_AF_M_MAX+1][MED_AEC_MAX_FFT_LEN]; /* Զ���ź�Ƶ���ź� */
    VOS_INT16                           ashwForeEcho[CODEC_FRAME_LENGTH_WB];/* ǰ�˲������������ź� */
    VOS_INT16                           ashwEcho[CODEC_FRAME_LENGTH_WB];    /* ���˲������������ź� */
    VOS_INT16                           ashwForeErr[CODEC_FRAME_LENGTH_WB]; /* ǰ�˲����в��ź� */
    VOS_INT16                           ashwErr[CODEC_FRAME_LENGTH_WB];     /* ���˲����в��ź� */
    VOS_INT16                           ashwErrFreq[MED_AEC_MAX_FFT_LEN];    /* ���˲����в�Ƶ���ź� */
    VOS_INT16                           ashwEchoFreq[MED_AEC_MAX_FFT_LEN];   /* ���˲�����������Ƶ���ź� */

    /* ���� */
    VOS_INT32                           swFarPow;                               /* Զ���ź����� */
    VOS_INT32                           swEchoPow;                              /* ���˲������������ź����� */
    VOS_INT32                           swErrPow;                               /* ���˲����в��ź�����*/
    VOS_INT32                           swForeErrPow;                           /* ǰ�˲����в��ź����� */
    VOS_INT32                           swBEchoErrPow;                          /* ���˲��������źźͺ��˲����в��źŻ���ع����� */
    VOS_INT32                           aswFarSmoothPsd[(MED_AEC_MAX_FFT_LEN/2) + 1]; /* Զ���ź�ƽ�������� */
    VOS_INT32                           swNearPow;                              /* �����źŹ����� */
    VOS_INT32                           aswErrPsd[(MED_AEC_MAX_FFT_LEN/2) + 1];/* ��˲в��źŹ����� */
    VOS_INT32                           aswEchoPsd[(MED_AEC_MAX_FFT_LEN/2) + 1];/* ��˻��������źŹ����� */

    /* ǰ���˲��� */
    VOS_INT32                           aswForeWeight[MED_AEC_AF_M_MAX][MED_AEC_MAX_FFT_LEN]; /* ǰ�˲����˲�ϵ��,32bit Q27 */
    VOS_INT32                           aswWeight[MED_AEC_AF_M_MAX][MED_AEC_MAX_FFT_LEN];     /* ���˲����˲�ϵ��,32bit Q27 */

    /* ������ */
    VOS_INT32                           swCancelCount;                          /* ֡������ */

    /* ����Ӧ�� */
    VOS_INT16                           ashwProp[MED_AEC_AF_M_MAX];

    VOS_INT16                           shwScrewedUp;                           /* AF��λ�жϼ����� */

    /* ǰ���˲����Ƚ� */
    MED_AEC_AF_KEEPER_STRU              stKeeper;

    /* й©������ر��� */
    MED_AEC_AF_LEAK_STRU                stCompLeakState;
    VOS_INT16                           shwLeakEstimate;                        /* й©�� */

    VOS_INT16                           shwFarPowerAlpha;                       /* Զ������ƽ��ϵ�� */

    /* ������ر��� */
    MED_AEC_AF_STEP_STRU                stStepState;

} MED_AEC_AF_STRU;


/*****************************************************************************
  6 UNION����
*****************************************************************************/

/*****************************************************************************
  7 OTHERS����
*****************************************************************************/

/*****************************************************************************
  8 ��������
*****************************************************************************/
extern VOS_VOID MED_AEC_AfCmpTwoFilter(
                       MED_AEC_AF_KEEPER_STRU  *pstKeeper,
                       VOS_INT32                swCurrDiffVar,
                       VOS_INT32                swForeErrPow,
                       VOS_INT32                swErrPow,
                       VOS_INT16               *pshwUpdateFore,
                       VOS_INT16               *pshwUpdateBack);
extern VOS_VOID MED_AEC_AfCompLeak(MED_AEC_AF_STRU *pstAf);
extern VOS_VOID MED_AEC_AfCompRER(
                       VOS_INT32  swFarPow,
                       VOS_INT32  swErrPow,
                       VOS_INT32  swEchoPow ,
                       VOS_INT16  shwLeakEstimate,
                       VOS_INT32  swBEchoErrPow,
                       VOS_INT16 *pshwRER);
extern VOS_VOID MED_AEC_AfCompSmoothPower(
                       VOS_INT16 *pshwX,
                       VOS_INT32  *paswPower,
                       VOS_INT16  shwAlpha,
                       VOS_INT16  shwLen);
extern VOS_VOID  MED_AEC_AfCompStep(MED_AEC_AF_STRU *pstAf);
extern VOS_VOID MED_AEC_AfFilt(
                       VOS_INT16  ashwFarFreq[][MED_AEC_AF_FILT_WB],
                       VOS_INT32  aswWeight[][MED_AEC_AF_FILT_WB],
                       VOS_INT16 *pshwInput,
                       VOS_INT16  shwM,
                       VOS_INT16  enFftLen,
                       VOS_INT16  shwIfftNumIndex,
                       VOS_INT16  enFrameLen,
                       VOS_INT16 *pshwEcho,
                       VOS_INT16 *pshwErr,
                       VOS_INT32 *pswErrPow);
extern VOS_INT32 MED_AEC_AfFrmPower(
                       VOS_INT16 *pshwX,
                       VOS_INT16 *pshwY,
                       VOS_INT16  shwFrameSize,
                       VOS_INT16  shwShift);
extern VOS_VOID MED_AEC_AfInit(
                       MED_AEC_NV_STRU                *pstPara,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                       MED_AEC_AF_STRU                *pstAf);
extern VOS_VOID MED_AEC_AfMain (
                       MED_AEC_AF_STRU               *pstAfObj,
                       VOS_INT16                     *pshwNear,
                       MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag,
                       VOS_INT16                     *pshwErr);
extern VOS_VOID MED_AEC_AfMainUpdate(
                       MED_AEC_AF_STRU               *pstAfObj,
                       MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag);
extern VOS_VOID MED_AEC_AfPowerSpectrum(
                       VOS_INT16 *pshwInFreq,
                       VOS_INT16  shwLen,
                       VOS_INT32 *pswInPsd);
extern VOS_VOID MED_AEC_AfResetJudge(MED_AEC_AF_STRU *pstAf, VOS_INT16 *pshwOut);
extern  VOS_VOID MED_AEC_AfSpectralMulAccum(
                       VOS_INT16   ashwFarFreq[][MED_AEC_AF_FILT_WB],
                       VOS_INT32   aswFilterCoeff[][MED_AEC_AF_FILT_WB],
                       VOS_INT16   shwM,
                       VOS_INT16   enFftLen,
                       VOS_INT16  *pshwEchoFreqOut);
extern VOS_VOID MED_AEC_AfStateReset(MED_AEC_AF_STRU  *pstAf);
extern VOS_VOID MED_AEC_AfUpdateProp(
                       VOS_INT32  aswW[][MED_AEC_AF_FILT_WB],
                       VOS_INT16  shwM,
                       VOS_INT16  shwWeightLen,
                       VOS_INT16 *pshwProp);
extern VOS_VOID MED_AEC_AfUpdateTwoFilter(
                       MED_AEC_AF_STRU *pstMedAecAf,
                       VOS_INT16        shwUpdateFore,
                       VOS_INT16        shwUpdateBack);
extern VOS_VOID MED_AEC_AfUpdateWeight(
                       MED_AEC_AF_STRU         *pstAf);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_af.h*/

