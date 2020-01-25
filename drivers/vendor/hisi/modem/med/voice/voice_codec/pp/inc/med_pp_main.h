

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_com_codec.h"
#include "med_pp_comm.h"
#include "med_fft.h"
#include "med_avc.h"
#include "med_filt.h"
#include "med_gain.h"
#include "med_aec_main.h"
#include "med_anr_2mic_interface.h"
#include "OmCodecInterface.h"
#include "med_agc.h"
#include "med_vad.h"
#include "med_mbdrc.h"
#include "med_aig.h"

#ifndef __MED_PP_MAIN_H__
#define __MED_PP_MAIN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define MED_PP_GetObjPtr()               (&g_stMedPpObj)             /* ��ȡPPȫ�ֱ��� */
#define MED_PP_GetSampleRate()           (g_stMedPpObj.enSampleRate)   /* ��ȡ��ǰ������ģʽ */
#define MED_PP_SetSampleRate(enSmpRate)  (g_stMedPpObj.enSampleRate = (enSmpRate))   /* ���õ�ǰ������ģʽ */

#define AUDIO_PP_GetObjPtr()             (&g_stAudioPpObj)             /* ��ȡ��ƵPPȫ�ֱ��� */
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
 �ṹ��    : VOICE_NV_STRU
 �ṹ˵��  : NV�����ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwVol[CODEC_NV_VOL_LEN];                /* ���͡�����������dBֵ, -80~12 */
    VOS_INT16                           ashwPreEmp[CODEC_NV_PREEMP_LEN];          /* Ԥ����ʹ�ܡ�Ԥ����ϵ��[15]��ȥ����ϵ��[15] */
    VOS_INT16                           ashwDeEmp[CODEC_NV_DEEMP_LEN];            /* Ԥ����ʹ�ܡ�Ԥ����ϵ��[15]��ȥ����ϵ��[15] */
    VOS_INT16                           ashwHpfTx[CODEC_NV_HPF_TX_LEN];           /* ���и�ͨ�˲�ʹ�ܡ��˲���ϵ��[16] */
    VOS_INT16                           ashwHpfRx[CODEC_NV_HPF_RX_LEN];           /* ���и�ͨ�˲�ʹ�ܡ��˲���ϵ��[16] */
    VOS_INT16                           ashwDevGain[CODEC_NV_DEVGAIN_LEN];        /* �豸�������桢�豸��������*/
    VOS_INT16                           ashwSTGain[CODEC_NV_SIDEGAIN_LEN];        /* �������� */
    VOS_INT16                           ashwCompTx[CODEC_NV_COMP_TX_LEN];         /* ���в����˲�ʹ�ܡ��˲���ϵ��[53] */
    VOS_INT16                           ashwCompRx[CODEC_NV_COMP_RX_LEN];         /* ���в����˲�ʹ�ܡ��˲���ϵ��[53] */
    VOS_INT16                           ashwAec[CODEC_NV_AEC_LEN];                /* AECʹ�ܡ�EC����[20] */
    VOS_INT16                           ashwAnr1MicTx[CODEC_NV_EANR_1MIC_LEN];     /* ANRʹ�ܡ�NS_1MIC����[10] */
    VOS_INT16                           ashwAnr1MicRx[CODEC_NV_EANR_1MIC_LEN];     /* ANRʹ�ܡ�NS_1MIC����[10] */
    VOS_INT16                           ashwAnr2Mic[CODEC_NV_ANR_2MIC_LEN];       /* ANRʹ�ܡ�NS_2MIC����[10] */
    VOS_INT16                           ashwAgcTx[CODEC_NV_AGC_TX_LEN];           /* ����AGCʹ�ܡ�AGC����[6] */
    VOS_INT16                           ashwAgcRx[CODEC_NV_AGC_RX_LEN];           /* ����AGCʹ�ܡ�AGC����[6] */
    VOS_INT16                           ashwMbdrc[CODEC_NV_MBDRC_LEN];            /* MBDRCʹ�ܡ�MBDRC����[224] */
    VOS_INT16                           ashwAigTx[CODEC_NV_AIG_TX_LEN];           /* ����AIGʹ�ܡ�AIG����[41] */
    VOS_INT16                           ashwAigRx[CODEC_NV_AIG_RX_LEN];           /* ����AIGʹ�ܡ�AIG����[41] */
    VOS_INT16                           ashwAvc[CODEC_NV_AVC_LEN];                /* AVCʹ�ܡ�AVC����[10] */
    VOS_INT16                           ashwVadTx[CODEC_NV_VAD_TX_LEN];           /* ����VAD���� */
    VOS_INT16                           ashwVadRx[CODEC_NV_VAD_RX_LEN];           /* ����VAD���� */
    VOS_INT16                           ashwReserve1[CODEC_NV_PP_RESERVE_LEN];    /* PP����Ԥ������1 */
    VOS_INT16                           ashwReserve2[CODEC_NV_PP_RESERVE_LEN];    /* PP����Ԥ������2 */
    VOS_INT16                           ashwReserve3[CODEC_NV_PP_RESERVE_LEN];    /* PP����Ԥ������3 */
    VOS_INT16                           ashwReserve4[CODEC_NV_PP_RESERVE_LEN];    /* PP����Ԥ������4 */
    VOS_INT16                           ashwReserve5[CODEC_NV_PP_RESERVE_LEN];    /* PP����Ԥ������5 */
}VOICE_NV_STRU;

/*****************************************************************************
 �ṹ��    : AUDIO_NV_STRU
 �ṹ˵��  : NV�����ṹ��
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwCompTx[AUDIO_NV_COMP_TX_LEN];       /* ���в����˲�ʹ�ܡ��˲���ϵ��[68] */
    VOS_INT16                           ashwCompRx[AUDIO_NV_COMP_RX_LEN];       /* ���в����˲�ʹ�ܡ��˲���ϵ��[68] */
    VOS_INT16                           ashwMbdrc[AUDIO_NV_MBDRC_LEN];          /* MBDRCʹ�ܡ�MBDRC����[126] */
}AUDIO_NV_STRU;

/*****************************************************************************
ʵ������  : MED_PP_STRU
��������  : �ṹ�������Ϣ
*****************************************************************************/
typedef struct
{
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate;                           /* ������ */
    MED_PP_MIC_NUM_ENUM_UINT16          enMicNum;                               /* Mic��Ŀ */
    VOS_UINT16                          uhwFrameId;                            /*ʱ��֡����ָʾ*/
    VOS_VOID                           *pstDevGainTx;                           /* ����ƥ������ */
    VOS_VOID                           *pstDevGainRx;                           /* ����ƥ������ */
    VOS_VOID                           *pstSTGain;                              /* �������� */
    VOS_VOID                           *pstAec;                                 /* AEC ���� */
    VOS_VOID                           *pstAgcTx;                               /* ����AGC ���� */
    VOS_VOID                           *pstAgcRx;                               /* ����AGC ���� */
    VOS_VOID                           *pstMbdrc;                               /* MBDRC���� */
    VOS_VOID                           *pstAigTx;                               /* ����AIG ���� */
    VOS_VOID                           *pstAigRx;                               /* ����AIG ���� */
    VOS_VOID                           *pstVadTx;                               /* ����VAD���� */
    VOS_VOID                           *pstVadRx;                               /* ����VAD���� */
    VOS_VOID                           *pstAvc;                                 /* AVC ���� */
    VOS_VOID                           *pstAnrTx;                               /* ANR����� ���ж��� */
    VOS_VOID                           *pstAnrRx;                               /* ANR����� ���ж��� */
    VOS_VOID                           *pstAnr2Mic;                             /* ANR˫��� ���� */
    VOS_VOID                           *pstPreEmpMc;                            /* ������ź�Ԥ�����˲���*/
    VOS_VOID                           *pstPreEmpRc;                            /* ������ź�Ԥ�����˲���*/
    VOS_VOID                           *pstPreEmpFar;                           /* Զ���ź�Ԥ�����˲���*/
    VOS_VOID                           *pstDeEmpMc;                             /* �����ź�ȥ�����˲���*/
    VOS_VOID                           *pstHpfTxMc;                             /* ����AFģ��ǰ����˸�ͨ�˲��� */
    VOS_VOID                           *pstHpfTxRc;                             /* ����AFģ��ǰ����˸�ͨ�˲��� */
    VOS_VOID                           *pstHpfAecMc;                            /* ����AFģ�������˸�ͨ�˲��� */
    VOS_VOID                           *pstHpfAecRc;                            /* ����AFģ�����˸�ͨ�˲��� */
    VOS_VOID                           *pstHpfRx;                               /* ���и�ͨ�˲��� */
    VOS_VOID                           *pstCompTx;                              /* ����Ƶ�첹���˲��� */
    VOS_VOID                           *pstCompRx;                              /* ����Ƶ�첹���˲��� */
}MED_PP_STRU;

/*****************************************************************************
ʵ������  : AUDIO_PP_STRU
��������  : �ṹ�������Ϣ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwSampleRate;                           /* ������ */
    VOS_VOID                           *pstMbdrcL;                              /* ������MBDRC���� */
    VOS_VOID                           *pstMbdrcR;                              /* ������MBDRC���� */
    VOS_VOID                           *pstCompTxL;                             /* ����������Ƶ�첹���˲��� */
    VOS_VOID                           *pstCompTxR;                             /* ����������Ƶ�첹���˲��� */
    VOS_VOID                           *pstCompRxL;                             /* ����������Ƶ�첹���˲��� */
    VOS_VOID                           *pstCompRxR;                             /* ����������Ƶ�첹���˲��� */
}AUDIO_PP_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/
extern MED_PP_STRU                            g_stMedPpObj;
extern AUDIO_PP_STRU                          g_stAudioPpObj;

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_UINT32 MED_PpDestroy(VOS_VOID);
extern VOS_UINT32 MED_PpGetPara(VOICE_NV_STRU *pstNv);
extern VOS_UINT32 MED_PpSetPara(
                       VOICE_NV_STRU                     *pstNv,
                       MED_PP_MIC_NUM_ENUM_UINT16         enPpMicNum );
extern VOS_UINT32 MED_PpTx_1Mic(
                       VOS_INT16                         *pshwMicIn,
                       VOS_INT16                         *pshwLineOut);
extern VOS_UINT32 MED_PpTx_2Mic(
                       VOS_INT16                         *pshwMcIn,
                       VOS_INT16                         *pshwRcIn,
                       VOS_INT16                         *pshwLineOut);
extern VOS_INT16 MED_PP_Init(VOS_VOID);
extern VOS_UINT32 MED_RxPp( VOS_INT16               *pshwLineIn);
extern VOS_UINT32 MED_TxPp(
                       VOS_INT16                         *pshwMcIn,
                       VOS_INT16                         *pshwRcIn,
                       VOS_INT16                         *pshwLineOut,
                       VOS_INT16                         *pshwEcRef);
extern VOS_INT16 AUDIO_PP_Init(VOS_VOID);
extern VOS_UINT32 AUDIO_PpSetPara(
                        AUDIO_NV_STRU                     *pstNv,
                        VOS_UINT32                         uwSampleRate);
extern VOS_UINT32 AUDIO_PpGetPara(AUDIO_NV_STRU     *pstNv);
extern VOS_UINT32 AUDIO_ProcPpTx(
                        VOS_INT16                         *pshwMicInL,
                        VOS_INT16                         *pshwMicInR,
                        VOS_INT16                         *pshwLineOutL,
                        VOS_INT16                         *pshwLineOutR,
                        AUDIO_PP_CHANNEL_ENUM_UINT16       uhwChannel);
extern VOS_UINT32 AUDIO_ProcPpRx(
                        VOS_INT16                         *pshwLineInL,
                        VOS_INT16                         *pshwLineInR,
                        VOS_INT16                         *pshwLineOutL,
                        VOS_INT16                         *pshwLineOutR,
                        AUDIO_PP_CHANNEL_ENUM_UINT16       uhwChannel);
extern VOS_UINT32 AUDIO_PpDestroy(VOS_VOID);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_pp_main.h */
