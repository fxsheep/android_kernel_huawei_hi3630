/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : fr_dtx.h
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��8��15��
  ����޸�   :
  ��������   : fr_dtx.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2011��8��15��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#ifndef __FR_DTX_H__
#define __FR_DTX_H__

#include "codec_op_etsi.h"
#include "fr_vad.h"
#include "CodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define FR_FRAME_TYPE_NUM                   (8)
#define FR_DTX_NUM_OF_LAR_COEF              (8)                                 /*����ϵ����������Ȳ�������*/
#define FR_DTX_NUM_OF_XMC                   (13)                                /*RPE-pluse����xMc����������*/
#define FR_DTX_NUM_OF_BLOCK_COEF            (4)                                 /*�ӿ����*/
#define FR_DTX_NUM_OF_FRAME_RECORD          (4)                                 /*DTX���Ʊ������ʷ֡��*/

#define FR_DTX_INIT_VALUE_NELAPSED          (0x7fff)                            /*�ṹ��״̬����������һ��SID����֡ʱ����ʼ��ֵ*/
#define FR_DTX_PN_INITIAL_SEED              (0x70816958L)                       /*��֪������������ֵ*/

#define FR_DTX_HANGOVER                     (4)                                 /*hangover����*/
#define FR_DTX_ELAPSED_THRESHOLD            (27)                                /*DTX�о�ƽ����ֵ*/
#define FR_DTX_CN_INT_PERIOD                (24)                                /*���������ڲ����ڳ���*/
#define FR_DTX_COMPRESS_SEVEN_TO_FIVE       (23405)                             /*5/7��Q15��ʾ*/
#define FR_DTX_MUTE_THRESH                  (7)                                 /*����֡����֡����ֵ*/

#define FR_DTX_SET_VALUE_SID_LARC           (2)                                 /*SID֡����ϵ������������趨ֵ*/
#define FR_DTX_SET_VALUE_SID_XMAX           (8)                                 /*SID֡�ӿ�������ֵ�趨ֵ*/
#define FR_DTX_SET_VALUE_SID_CN_NC_ODD      (40)                                /*SID��֪����LTP�ӳٲ���Nc������ֵ*/
#define FR_DTX_SET_VALUE_SID_CN_NC_EVEN     (120)                               /*SID��֪����LTP�ӳٲ���Ncż����ֵ*/
#define FR_DTX_SET_VALUE_SID_SILENCE_NC     (40)                                /*����֡LTP�ӳٲ����趨ֵ*/
#define FR_DTX_SET_VALUE_SID_SILENCE_MC     (1)                                 /*����֡RPE-gridλ�ò����趨ֵ*/

#define FR_DTX_STATE_MACHINE_HIGH_VALUE     (6)                                 /*DTX���п���״̬����ֵ*/
#define FR_DTX_STATE_MACHINE_HIGH_SUB_VALUE (5)                                 /*DTX���п���״̬���θ�ֵ*/

#define FR_DTX_MASK_FRAME_RECORD_INDEX      (0x0003)                            /*2����֡����ֵ����*/

/* DTX���п����ֳ�����ʶ */
#define FR_TX_SP_FLAG                       (0x0001)
#define FR_TX_VAD_FLAG                      (0x0002)
#define FR_TX_HANGOVER_ACTIVE               (0x0004)
#define FR_TX_PREV_HANGOVER_ACTIVE          (0x0008)
#define FR_TX_SID_UPDTE                     (0x0010)
#define FR_TX_USE_OLD_SID                   (0x0020)

/* DTX���п����ֳ�����ʶ */
#define FR_RX_SP_FLAG                       (0x0001)
#define FR_RX_UPD_SID_QUANT_MEM             (0x0002)
#define FR_RX_FIRST_SID_UPDATE              (0x0004)
#define FR_RX_CONT_SID_UPDATE               (0x0008)
#define FR_RX_LOST_SID_FRAME                (0x0010)
#define FR_RX_INVALID_SID_FRAME             (0x0020)
#define FR_RX_NO_TRANSMISSION               (0x0040)
#define FR_RX_DTX_MUTING                    (0x0080)
#define FR_RX_PREV_DTX_MUTING               (0x0100)
#define FR_RX_CNI_BFI                       (0x0200)
#define FR_RX_FIRST_SP_FLAG                 (0x0400)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
 ʵ������  : FR_DTX_ENUM
 ��������  : DTXȡֵ����
*****************************************************************************/
enum FR_CODEC_DTX_ENUM
{
    FR_DTX_DISABLE                      = CODEC_DTX_DISABLE,                /* DTX��ʹ�� */
    FR_DTX_ENABLE                       = CODEC_DTX_ENABLE,                 /* DTXʹ��   */
    FR_DTX_BUTT
};
typedef UWord16 FR_DTX_ENUM_UINT16;

/*****************************************************************************
 ö �� ��  : FR_BFI_STATUS_ENUM
 ö��˵��  : BFI֡��־״̬
*****************************************************************************/
enum FR_BFI_STATUS_ENUM
{
    FR_BFI_STATUS_NOT                   = CODEC_BFI_NO,                     /*��BFI֡*/
    FR_BFI_STATUS_YES                   = CODEC_BFI_YES,                    /*  BFI֡*/
    FR_BFI_STATUS_BUTT
};
typedef UWord16 FR_BFI_STATUS_ENUM_UINT16;

/*****************************************************************************
 ö �� ��  : FR_TAF_STATUS_ENUM
 ö��˵��  : TAF֡��־״̬
 *****************************************************************************/
enum FR_TAF_STATUS_ENUM
{
    FR_TAF_STATUS_NOT                   = CODEC_TAF_NONE,                   /*��TAF֡*/
    FR_TAF_STATUS_YES                   = CODEC_TAF_ALIGNED,                /*  TAF֡*/
    FR_TAF_STATUS_BUTT
};
typedef UWord16 FR_TAF_STATUS_ENUM_UINT16;

/*****************************************************************************
 ö �� ��  : FR_SID_STATUS_ENUM
 ö��˵��  : SID֡��־״̬
 *****************************************************************************/
enum FR_SID_STATUS_ENUM
{
    FR_SID_STATUS_NOT                   = 0,                                    /*��Ч����֡*/
    FR_SID_STATUS_INVALID,                                                      /*��ЧSID֡*/
    FR_SID_STATUS_YES,                                                          /*��ЧSID֡*/
    FR_SID_STATUS_BUTT
};
typedef UWord16 FR_SID_STATUS_ENUM_UINT16;


/*****************************************************************************
 ö �� ��  : FR_HOMING_STATUS_ENUM
 ö��˵��  : Homing֡��־״̬
 *****************************************************************************/
enum FR_HOMING_STATUS_ENUM
{
    FR_HOMING_STATUS_NOT                = 0,                                    /*��Homing֡*/
    FR_HOMING_STATUS_YES,                                                       /*Homing֡*/
    FR_HOMING_STATUS_BUTT
};
typedef UWord16 FR_HOMING_STATUS_ENUM_UINT16;


/*****************************************************************************
 ö �� ��  : FR_FRAME_TYPE_ENUM
 ö��˵��  : ֡���ͷ���
 *****************************************************************************/
enum FR_FRAME_TYPE_ENUM
{
    FR_FRAME_TYPE_VALID_SID             = 1,                                    /*��ЧSID֡*/
    FR_FRAME_TYPE_INVALID_SID           = 2,                                    /*��ЧSID֡*/
    FR_FRAME_TYPE_GOOD_SPEECH           = 3,                                    /*��Ч����֡*/
    FR_FRAME_TYPE_UNUSABLE              = 4,                                    /*������֡*/
    FR_FRAME_TYPE_BUTT
};
typedef UWord16 FR_FRAME_TYPE_ENUM_UINT16;

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
ʵ������: FR_ENC_DTX_STATE_STRU
��������: FR���д���DTX���ƺ�����������������ṹ��
************************************************************************/
typedef struct
{
    Word16                              shwCtrlWord;                            /*�����DTX��������*/
    Word16                              shwNelapsed;                            /*������һ��SID����֡ʱ��*/
    Word32                              swPnSeed;                               /*�����PN��������*/
    Word16                              shwHangover;                            /*hangoverʱ��(VAD=0,SP=1)*/
    Word16                              shwHistIndx;
    Word16                              ashwLarcHist[FR_DTX_NUM_OF_BLOCK_COEF][FR_DTX_NUM_OF_LAR_COEF];   /*ǰ4֡LARC*/
    Word16                              ashwXmaxHist[FR_DTX_NUM_OF_BLOCK_COEF][FR_DTX_NUM_OF_BLOCK_COEF]; /*ǰ4֡Xmax*/
    Word16                              ashwSidLar[FR_DTX_NUM_OF_LAR_COEF];     /*��һ��SID֡LAR*/
    Word16                              ashwSidXmax[FR_DTX_NUM_OF_BLOCK_COEF];  /*��һ��SID֡Xmax*/
} FR_ENC_DTX_STATE_STRU;

/************************************************************************
ʵ������: FR_DEC_DTX_STATE_STRU
��������: FR���д���DTX���ơ�����������������ʧ֡�ָ�����ṹ��
************************************************************************/
typedef struct
{
    Word16                              shwCtrlWord;                            /*�����DTX������*/
    Word16                              shwCniState;                            /*����������������״̬*/
    Word16                              shwNelapsed;                            /*������һ��SID֡ʱ��*/
    Word16                              shwSidLostFrameNum;                     /*��ʧ��SID֡���ۼ�*/
    Word16                              shwHangoverPeriod;                      /*hangoverʱ��*/

    /*��ʧ֡����״̬��*/
    FR_BFI_STATUS_ENUM_UINT16           enPrevBfi;                              /*��һ֡BFI��־*/
    Word16                              shwStateCnt;
    Word16                              shwMuteCount;                           /*�ѹ��쾲��֡��*/
    Word32                              swPnSeed;                               /*PN������������*/

    /*������������*/
    Word16                              ashwLarCnOld[FR_DTX_NUM_OF_LAR_COEF];   /*������������LAR����*/
    Word16                              ashwLarCnNew[FR_DTX_NUM_OF_LAR_COEF];   /*��ǰ��������LAR����*/
    Word16                              ashwXmaxCnOld[FR_DTX_NUM_OF_BLOCK_COEF];/*������������Xmax����*/
    Word16                              ashwXmaxCnNew[FR_DTX_NUM_OF_BLOCK_COEF];/*��ǰ��������Xmax����*/
} FR_DEC_DTX_STATE_STRU;

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

extern void FR_DtxDecSidLarAndXmaxc(
                Word16                    *pshwLarc,
                Word16                    *pshwLarppNew,
                Word16                    *pshwLarppOld,
                Word16                    *pshwXmaxc,
                FR_BFI_STATUS_ENUM_UINT16  enBfi,
                FR_DEC_DTX_STATE_STRU     *pstDecDtxState);
extern void FR_DtxDecSidOtherPrms(
                Word16                     pshwNc[],
                Word16                     pshwBc[],
                Word16                     pshwMc[],
                Word16                     pshwXmc[],
                Word32                    *pswSeed);
extern void FR_DtxControlRx (
                FR_TAF_STATUS_ENUM_UINT16  enTaf,
                FR_BFI_STATUS_ENUM_UINT16  enBfi,
                FR_SID_STATUS_ENUM_UINT16  enSidFlag,
                Word16                    *pshwState,
                FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi,
                FR_DEC_DTX_STATE_STRU     *pstDecDtxState);
extern void FR_DtxUpdateCniState(
                FR_TAF_STATUS_ENUM_UINT16                   enTaf,
                Word16                                     *pshwCtrlWord,
                UWord16                                     uhwFrameType,
                FR_DEC_DTX_STATE_STRU                      *pstDecDtxState);

extern void FR_DtxControlTx(
                        FR_VAD_STATUS_ENUM_UINT16  enVadFlag,
                        FR_ENC_DTX_STATE_STRU     *pstEncDtxState);
extern void FR_DtxRxControlState(
                Word16                     shwCtrlWord,
                FR_BFI_STATUS_ENUM_UINT16  enBfi,
                FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi,
                Word16                    *pshwState);
extern void FR_DtxRxInit(FR_DEC_DTX_STATE_STRU * pstDecDtxState);
extern void FR_DtxTxInit(FR_ENC_DTX_STATE_STRU * pstEncDtxState);
extern void FR_DtxEncSidPrm(
                Word16                 *pshwLARc,
                Word16                 *pshwXmaxc,
                FR_ENC_DTX_STATE_STRU  *pstEncDtxState);
extern void FR_DtxGenSilence(
                Word16                  pshwLARpp[],
                Word16                  pshwNc[],
                Word16                  pshwBc[],
                Word16                  pshwMc[],
                Word16                  pshwXmaxc[],
                Word16                  pshwXmc[]);
extern void FR_DtxInterpolateCnVec(Word16 pshwOldCn[],
                Word16                  pshwNewCn[],
                Word16                  pshwInterpCn[],
                Word16                  pshwRxDtxState,
                Word16                  shwLen);
extern Word16 FR_DtxPseudoNoise(Word32 *pswShiftReg, Word16 shwNumBits);
extern Word16 FR_DtxQxmax(Word16 shwXmax);
extern void FR_DtxUpdateSidMem(
                Word16 *pshwLaruq,
                Word16                 *pshwXmaxuq,
                FR_ENC_DTX_STATE_STRU  *pstEncDtxState);
extern void FR_DtxUpdateSidRxState(
                Word16                 *shwCtrlWord,
                FR_DEC_DTX_STATE_STRU  *pstDecDtxState);


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of fr_dtx.h */
