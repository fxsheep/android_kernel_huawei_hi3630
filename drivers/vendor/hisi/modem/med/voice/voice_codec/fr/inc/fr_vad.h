/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : fr_vad.h
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��5��3��
  ����޸�   :
  ��������   : fr_vad.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#ifndef __FR_VAD_H__
#define __FR_VAD_H__

#include "codec_op_etsi.h"
#include "fr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define FR_VAD_MAX_VALUE_16BITS         (32767)                                 /*16�����з��������ֵ*/
#define FR_VAD_MAX_VALUE_15BITS         (16384)                                 /*15�����з��������ֵ*/
#define FR_VAD_MAX_VALUE_13BITS         (4095)                                  /*13�����з��������ֵ*/

#define FR_VAD_NUM_OF_RVAD              (9)                                     /*����Ӧ�˲�ϵ������*/
#define FR_VAD_NUM_OF_SACF              (27)                                    /*�����ϵ������*/
#define FR_VAD_NUM_OF_SAV0              (36)                                    /*�����ϵ����ֵ����*/

#define FR_VAD_INIT_VALUE_RVAD0         (24576)                                 /*��0������Ӧ�˲�ϵ����ʼ��ֵ*/
#define FR_VAD_INIT_VALUE_RVAD1         (-16384)                                /*��1������Ӧ�˲�ϵ����ʼ��ֵ*/
#define FR_VAD_INIT_VALUE_RVAD2         (4096)                                  /*��2������Ӧ�˲�ϵ����ʼ��ֵ*/
#define FR_VAD_INIT_VALUE_NORM_RVAD     (7)                                     /*rvad�������ӳ�ʼ��ֵ*/
#define FR_VAD_INIT_VALUE_THVAD_E       (17)                                    /*VAD�ж�����Ӧ��ֵָ����ʼ��ֵ*/
#define FR_VAD_INIT_VALUE_THVAD_M       (31250)                                 /*VAD�ж�����Ӧ��ֵβ����ʼ��ֵ*/
#define FR_VAD_INIT_VALUE_HANG_COUNT    (-1)                                    /*hangover֡���ۼƳ�ʼ��ֵ*/
#define FR_VAD_INIT_VALUE_OLD_LAG       (40)                                    /*LTP���ڳ�ʼ��ֵ*/
#define FR_VAD_INIT_VALUE_TONE          (0)                                     /*TONE����ʼ��ֵ*/

#define FR_VAD_SET_VALUE_ACF0_E         (32)                                    /*�ź�����ָ���趨ֵ*/
#define FR_VAD_SET_VALUE_SCAL           (10)                                    /*���������趨ֵ*/
#define FR_VAD_SET_VALUE_COEF0          (16384)                                 /*LPC�����˲�ϵ��0�趨ֵ*/
#define FR_VAD_SET_VALUE_SAV0           (4095)                                  /*�����ϵ����ֵ0�趨ֵ*/
#define FR_VAD_SET_VALUE_DM             (32768)                                 /*����ϵ���趨ֵ*/
#define FR_VAD_SET_VALUE_DM_DIFF        (3277)                                  /*����ϵ������趨ֵ*/
#define FR_VAD_SET_VALUE_ADAPT_COUNT    (9)                                     /*����Ӧ֡���ۼ��趨ֵ*/
#define FR_VAD_SET_VALUE_HANG_COUNT     (5)                                     /*hangover֡���ۼ��趨ֵ*/
#define FR_VAD_SET_VALUE_BURST_COUNT    (3)                                     /*����Burst֡���ۼ��趨ֵ*/

#define FR_VAD_THRESH_E_PTH             (19)                                    /*�����ϵ��ACF[0]�����ֵָ������*/
#define FR_VAD_THRESH_M_PTH             (18750)                                 /*�����ϵ��ACF[0]�����ֵβ������*/
#define FR_VAD_THRESH_E_PLEV            (17)                                    /*VAD�о�������ֵָ������*/
#define FR_VAD_THRESH_M_PLEV            (25000)                                 /*VAD�о�������ֵβ������*/
#define FR_VAD_THRESH_E_MARGIN          (27)                                    /*�ź�������Ե��ֵָ������*/
#define FR_VAD_THRESH_M_MARGIN          (19531)                                 /*�ź�������Ե��ֵβ������*/

#define FR_AR_LEN                       (9)                                     /*�����ϵ������*/

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/*****************************************************************************
 ʵ������  : FR_COMPARE_ENUM
 ��������  : �����Ƚϴ�Сȡֵ����
*****************************************************************************/
enum FR_COMPARE_ENUM
{
    FR_COMPARE_BIG                      = 0,
    FR_COMPARE_SMALL                    = 1,
    FR_COMPARE_BUTT
};
typedef UWord16 FR_COMPARE_ENUM_UINT16;

/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  6 STRUCT����
*****************************************************************************/

/************************************************************************
ʵ������: FR_FLOAT_STRU
��������: �����������ͽṹ�嶨��
************************************************************************/
typedef struct
{
    Word16                              shwExp;                                 /* ����ָ�� */
    Word16                              shwMnts;                                /* ����β�� */
}FR_FLOAT_STRU;

/************************************************************************
ʵ������: FR_VAD_STATE_STRU
��������: FR���д���VAD������ṹ��
************************************************************************/
typedef struct
{
    Word16                              shwNormRvad;                            /*rvad��������*/
    Word16                              shwReserved;
    FR_FLOAT_STRU                       stThvad;                               /*VAD�ж�����Ӧ��ֵ*/
    Word16                              shwPtSacf;                              /*sacf�ӳٵ���*/
    Word16                              shwPtSav0;                              /*sav0�ӳٵ���*/
    Word32                              swLastDm;                               /*�������ֵ*/
    Word16                              shwAdaptCnt;                            /*����Ӧ֡���ۼ�*/
    Word16                              shwBurstCnt;                            /*����Burst֡���ۼ�*/
    Word16                              shwHangCnt;                             /*hangover֡���ۼ�*/
    Word16                              shwOldLagCnt;                           /*�����ۼ�*/
    Word16                              shwVeryOldLagCnt;
    Word16                              shwOldLag;                              /*LTP����*/
    Word16                              shwPtch;                                /*���ڼ���־*/
    Word16                              ashwRvad[9];                            /*����Ӧ�˲�ϵ��*/
    Word32                              aswAcf[27];                             /*�����ϵ��*/
    Word32                              aswSav0[36];                            /*�����ϵ����ֵ*/
} FR_VAD_STATE_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/

extern void FR_VadAdaptByPvadWithFac(FR_FLOAT_STRU stPvad, FR_FLOAT_STRU *pstThvad);
extern void FR_VadAdaptByPvadWithMargin(FR_FLOAT_STRU stPvad, FR_FLOAT_STRU *pstThvad);
extern void FR_VadAdaptThreshold(
                Word16                  shwSpecStat,
                Word16                  shwTone,
                Word16                  shwNormRav1,
                FR_FLOAT_STRU           stAcf0,
                FR_FLOAT_STRU           stPvad,
                Word16                  ashwRav1[],
                FR_VAD_STATE_STRU      *pstVadState);
extern void FR_VadAverAcf(
                const Word32            aswAcf[],
                const Word16            shwScalvad,
                Word32                  aswAv0[],
                Word32                  aswAv1[],
                FR_VAD_STATE_STRU      *pstVadState);
extern FR_COMPARE_ENUM_UINT16 FR_VadCompareFloat(
                FR_FLOAT_STRU           stFloatx,
                FR_FLOAT_STRU           stFloaty);
extern Word16 FR_VadCompareSpectral(
                Word16                  ashwRav1[],
                Word16                  shwNormRav1,
                Word32                  aswAv0[],
                FR_VAD_STATE_STRU      *pstVadState);
extern Word16 FR_VadComputeEnergy(
                const Word32            aswAcf[],
                Word16                  shwScalauto,
                FR_FLOAT_STRU          *pstAcf0,
                FR_FLOAT_STRU          *pstPvad,
                FR_VAD_STATE_STRU      *pstVadState);
extern void FR_VadComputeRav1(
                Word16                  ashwAav1[],
                Word16                  ashwRav1[],
                Word16                 *pshwNormRav1);
extern Word16 FR_VadDecideAdapt(Word16 shwPtch, Word16 shwSpecStat, Word16 shwTone);
extern FR_VAD_STATUS_ENUM_UINT16 FR_VadDeciVad(FR_FLOAT_STRU stPvad, FR_FLOAT_STRU stThvad);
extern FR_VAD_STATUS_ENUM_UINT16 FR_VadDetect(
                Word32                 *pswAcf,
                Word16                  shwScalauto,
                Word16                 *pshwNc,
                FR_VAD_STATE_STRU      *pstVadState);
extern void FR_VadDetectPeriod(FR_VAD_STATE_STRU *pstVadState);
extern FR_VAD_STATUS_ENUM_UINT16 FR_VadHangover(
                FR_VAD_STATUS_ENUM_UINT16                   enVvad,
                Word16                                     *pshwBurstCount,
                Word16                                     *pshwHangCount);
extern void FR_VadInit(FR_VAD_STATE_STRU *pstVadState);
extern void FR_VadPredictValues(
                Word32                  aswAv1[],
                Word16                  ashwRav1[],
                Word16                 *pshwNormrav1);
extern void FR_VadSchurRecursion(Word32 aswAv1[], Word16 ashwVpar[]);
extern void FR_VadUpdatePeriod(
                Word16                 *pshwLags,
                FR_VAD_STATE_STRU      *pstVadState);
extern void FR_VadUpdateStep(Word16 ashwVpar[], Word16 ashwAav1[]);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of fr_vad.h */
