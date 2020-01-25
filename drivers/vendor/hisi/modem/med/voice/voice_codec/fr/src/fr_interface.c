

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_com_codec.h"
#include "fr_interface.h"
#include "fr_codec.h"
#include "fr_enc.h"
#include "fr_dec.h"
#include "fr_homing.h"
#include "stdlib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/

/*****************************************************************************
  3 ȫ�ֱ�������
*****************************************************************************/
const VOS_INT16 g_ashwFrBitNo[FR_CODED_SERIAL_LENGTH] =
{
    6, 6, 5, 5, 4, 4, 3, 3,                                                     /*LARcϵ����Ӧ������*/
    7, 2, 2, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,                          /*��һ����֡ϵ����Ӧ������*/
    7, 2, 2, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,                          /*�ڶ�����֡ϵ����Ӧ������*/
    7, 2, 2, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,                          /*��������֡ϵ����Ӧ������*/
    7, 2, 2, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3                           /*���ĸ���֡ϵ����Ӧ������*/
};

FR_ENCODE_STATE_STRU   g_stFrEncodeState;                                       /*FR�������ṹ��ȫ�ֱ���*/

FR_DECODE_STATE_STRU   g_stFrDecodeState;                                       /*FR�������ṹ��ȫ�ֱ���*/

VOS_UINT16 g_uhwFrOldResetFlag;                                                  /*���������ñ�־��¼*/

/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/


VOS_UINT32 FR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode)
{
    /*��ȡȫ�ֱ���FR�������ṹ��ָ��*/
    FR_ENCODE_STATE_STRU *pstFrEncState = FR_GetEncStatePtr();

    /*����MED_FR_EncInit�������б����ʼ��*/
    FR_EncInit(pstFrEncState, enDtxMode);

    return CODEC_RET_OK;
}


VOS_UINT32 FR_Dec_Init(VOS_VOID)
{
    /*��ȡȫ�ֱ���FR�������ṹ��ָ��*/
    FR_DECODE_STATE_STRU *pstFrDecState = FR_GetDecStatePtr();

    /*��ȡȫ�ֱ���FR��һ֡reset��־ָ��*/
    VOS_UINT16 *puhwOldResetFlag        = FR_GetOldResetPtr();

    /*��ȫ�ֱ���FR��һ֡reset��־��ʼ��Ϊ1*/
    *puhwOldResetFlag = 1;

    /*����MED_FR_DecInit�������н����ʼ��*/
    FR_DecInit(pstFrDecState);

    return CODEC_RET_OK;
}


VOS_UINT32 FR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara)
{
    VOS_UINT16  uhwResetFlag;
    VOS_INT16   ashwNewSpeech[CODEC_FRAME_LENGTH_NB];                       /*����ԭʼPCM��������,��160��16Bit*/
    VOS_INT16   ashwEncodedSerial[FR_CODED_BITS_LENGTH];                        /*�����������ȡ�����������еı���������,��260������*/
    VOS_INT16   shwCnt;
    VOS_INT16   shwTemp;
    FR_ENCODED_SERIAL_STRU stFrSerial;

    /*��ȡȫ�ֱ���FR�������ṹ��ָ��*/
    FR_ENCODE_STATE_STRU *pstFrEncState = FR_GetEncStatePtr();

    /*����DTX��־*/
    pstFrEncState->enDtxFlag = pstEncInPara->enDtxMode;

    /*����Homing���*/
    uhwResetFlag = FR_EncDetectHomingFrame(pstEncInPara->pshwEncSpeech);

    /* ��ӽ�����֡��С��������� */
    for(shwCnt = 0; shwCnt < CODEC_FRAME_LENGTH_NB; shwCnt++)
    {
        shwTemp             = (rand() & 0x7fff ) & FR_SIGNAL_RAND_RANGE_15; /* [false alarm]:��ȷ�ϴ˴�ʹ��rand������  */
        pstEncInPara->pshwEncSpeech[shwCnt] =  CODEC_OpAdd(pstEncInPara->pshwEncSpeech[shwCnt],
                                                           shwTemp - (FR_SIGNAL_RAND_RANGE_15>>1));
    }

    /*ȡPCM���ݸ�13Bit��Ч*/
    CODEC_OpVcAnd(pstEncInPara->pshwEncSpeech,
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                &ashwNewSpeech[0]);

    /*FR��������76���������*/
    gsm_encode(pstFrEncState, &ashwNewSpeech[0], (VOS_INT16 *)&stFrSerial);

    /*���enVadFlag��enSpFlag�������ֱ����ashwLarc[0]��ashwLarc[1]��16bitλ*/
    pstEncOutPara->enVadFlag = ((stFrSerial.ashwLarc[0] & BIT15_MASK) >> 15) & BIT0_MASK;
    pstEncOutPara->enSpFlag  = ((stFrSerial.ashwLarc[1] & BIT15_MASK) >> 15) & BIT0_MASK;

    /*������ΪHoming֡,���ñ�����*/
    if (1 == uhwResetFlag)
    {
        FR_EncInit(pstFrEncState, pstEncInPara->enDtxMode);
    }

    /*��76������ת��Ϊ260���ǽ��ձ�����*/
    FR_UlFrameConvert((VOS_INT16 *)&stFrSerial, &ashwEncodedSerial[0]);

    /*��260���ǽ��ձ�����ת��Ϊ����������ʽ*/
    CODEC_ComCodecBits2Bytes(&ashwEncodedSerial[0],
                           FR_CODED_BITS_LENGTH,
                           (VOS_INT8 *)(pstEncOutPara->pshwEncSerial));

    return CODEC_RET_OK;
}
VOS_UINT32 FR_Decode(
                CODEC_DEC_IN_PARA_STRU  *pstDecPara,
                VOS_INT16                   *pshwDecSpeech)
{
    VOS_UINT16  uhwResetFlag;
    VOS_UINT16  uhwOldResetFlag;
    VOS_INT16   ashwDecodedSerial[FR_CODED_BITS_LENGTH];                        /*FR����ǰ260��������������еı���������*/

    FR_DECODE_FRAME_STRU   stFrFrame;

    /*��ȡȫ�ֱ���FR�������ṹ��ָ��*/
    FR_DECODE_STATE_STRU *pstFrDecodeState = FR_GetDecStatePtr();

    /*��ȡȫ�ֱ���FR��һ֡reset��־ָ��*/
    VOS_UINT16 *puhwOldResetFlag           = FR_GetOldResetPtr();

    /*��ʱ����FR��һ֡reset��־��ֵ*/
    uhwOldResetFlag                        = *puhwOldResetFlag;

    /*��4�ֽڽ������е�������ʽת��Ϊ260����������еı�������ʽ*/
    CODEC_ComCodecBytes2Bits((VOS_INT8 *)(pstDecPara->pshwDecSerial),
                           FR_CODED_BITS_LENGTH,
                           ashwDecodedSerial);

    /*��260�����صı�����ת���ϲ�Ϊ76�������Ľ��������*/
    FR_DlFrameConvert(&ashwDecodedSerial[0], (VOS_INT16 *)(&stFrFrame));

    /* ����SID/TAF/BFI�����־,���η�������������Ӧλ��*/
    stFrFrame.enSidFlag = pstDecPara->enSidFlag;
    stFrFrame.enTafFlag = pstDecPara->enTafFlag;
    stFrFrame.enBfiFlag = pstDecPara->enBfiFlag;

    /* ����Э��涨������֡����������(Homingȫ֡���ΪTRUE)����ֻ֡��Ҫ��������Homing��һ��֡��� */
    if (1 == uhwOldResetFlag)
    {
        uhwResetFlag = FR_DecDetectHomingFrame((VOS_INT16 *)(&stFrFrame),
                                                FR_HOMING_FIRST_SUBFRAME);
    }
    else
    {
        uhwResetFlag = 0;
    }

    /* ����֡ΪHoming֡,�ұ�֡Homing��һ��֡���ҲΪ��,��ֱ������������ΪHoming��ʽ */
    if (uhwResetFlag && uhwOldResetFlag)
    {
        CODEC_OpVecSet(pshwDecSpeech,
                     CODEC_FRAME_LENGTH_NB,
                     CODEC_EHF_MASK);
    }
    else
    {
        /* ����FR���뺯��gsm_decode���ݽ��н���,����76+3������(����SID/TAF/BFI) */
        gsm_decode(pstFrDecodeState,
                   (VOS_INT16 *)(&stFrFrame),
                   &pshwDecSpeech[0]);
    }

    /* ȡ�����PCM�����ĸ�13Bit��Ч */
    CODEC_OpVcAnd(pshwDecSpeech,
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                pshwDecSpeech);

    /* ����֡��Homing֡,��֡�����Homingȫ֡��� */
    if (0 == uhwOldResetFlag)
    {
        uhwResetFlag = FR_DecDetectHomingFrame((VOS_INT16 *)(&stFrFrame),
                                                FR_HOMING_WHOLE_FRAME);
    }

    /* ����֡ΪHoming֡,������������,����������ΪHoming��ʽ */
    if (uhwResetFlag)
    {
        FR_DecInit(pstFrDecodeState);

        CODEC_OpVecSet(pshwDecSpeech,
                     CODEC_FRAME_LENGTH_NB,
                     CODEC_EHF_MASK);
    }

    /*���汾֡Homing�����,������֡Ӧ��*/
    *puhwOldResetFlag = uhwResetFlag;

    return CODEC_RET_OK;
}



VOS_VOID FR_DlFrameConvert(
                VOS_INT16               *pshwFrFrame,
                VOS_INT16               *pshwEncodedSerial)
{
    VOS_INT32 i;

    /*��ȡFR���������ظ�����*/
    VOS_INT16 *pshwFrParsBitsNumTable = (VOS_INT16 *)FR_GetParsNumPtr();

    /*��260bit�����б�����pFrFrameת��Ϊ76�����������pEncodedSerial*/
    for (i = 0; i < FR_CODED_SERIAL_LENGTH; i++)
    {
        CODEC_ComCodecBits2Prm(pshwFrFrame,
                             pshwFrParsBitsNumTable[i],
                             pshwEncodedSerial++);

        pshwFrFrame += pshwFrParsBitsNumTable[i];
    }
}


VOS_VOID FR_UlFrameConvert(VOS_INT16 *pshwEncodedSerial, VOS_INT16 *pshwFrFrame)
{
    VOS_INT32 i;

    /*��ȡFR���������ظ�����*/
    VOS_INT16 *pshwFrParsBitsNumTable = (VOS_INT16 *)FR_GetParsNumPtr();

    /*��76������ת��Ϊ260���ǽ��ձ�����*/
    for (i = 0; i < FR_CODED_SERIAL_LENGTH; i++)
    {
        CODEC_ComCodecPrm2Bits(pshwEncodedSerial[i],
                             pshwFrParsBitsNumTable[i],
                             pshwFrFrame);

        pshwFrFrame += pshwFrParsBitsNumTable[i];
    }
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


