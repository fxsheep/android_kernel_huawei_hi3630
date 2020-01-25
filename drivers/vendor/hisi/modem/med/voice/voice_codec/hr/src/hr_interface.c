

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "codec_com_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "hr_interface.h"
#include "hr_sp_dec.h"
#include "hr_sp_enc.h"
#include "hr_homing.h"
#include "hr_typedefs.h"

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/*HR��Unvoiceģʽ�¸�������ռ��������*/
const VOS_INT16 g_ashwHrUnvoicedParmBitsTable[18] =
{
    5, 11, 9, 8, 1, 2, 7, 7, 5,
    7, 7, 5, 7, 7, 5, 7, 7, 5
};

/*HR��Voiceģʽ�¸�������ռ��������*/
const VOS_INT16 g_ashwHrVoicedParmBitsTable[18] =
{
    5, 11, 9, 8, 1, 2, 8, 9, 5,
    4, 9, 5, 4, 9, 5, 4, 9, 5
};

/*HR��һ֡reset��־*/
VOS_INT16 g_shwHrOldResetFlag = 1;

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


VOS_UINT32 HR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode)
{
    /*��ȡȫ�ֱ���DTX��־ָ��*/
    VOS_INT32 *pshwDtxFlag      = (VOS_INT32 *)HR_GetDtxFlagPtr();

    /*��DTX��־*/
    *pshwDtxFlag      = (VOS_INT32)enDtxMode;

    /*HR���г�ʼ��*/
    resetEnc();

    return CODEC_RET_OK;
}
VOS_UINT32 HR_Dec_Init(VOS_VOID)
{
    /*��ȡȫ�ֱ���FR��һ֡reset��־ָ��*/
    VOS_INT16 *pshwOldResetFlag = HR_GetOldResetPtr();

    /*��reset��־*/
    *pshwOldResetFlag = 1;

    /*HR���г�ʼ��*/
    resetDec();

    return CODEC_RET_OK;
}
VOS_UINT32 HR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU  *pstEncOutPara)
{
    VOS_INT16                   shwResetFlag;
    VOS_INT16                   ashwEncSpeech[CODEC_FRAME_LENGTH_NB];       /*���������8K�����ʵ�PCM������ÿ֡160������*/
    VOS_INT16                   ashwEncSerial[HR_CODED_BITS_LENGTH];            /*HR������ȡ�ı���������112������*/

    HR_ENCODE_SERIAL_STRU       stParamBuf;                                     /*HR���������20������(��VadFlag��SpFlag)*/

    /*��ȡȫ�ֱ���DTX��־ָ��*/
    VOS_INT32 *pshwDtxFlag = (VOS_INT32 *)HR_GetDtxFlagPtr();

    /*ÿ֡����DTX��־*/
    *pshwDtxFlag = (VOS_INT32)pstEncInPara->enDtxMode;

    /*ȡPCM���ݸ�13Bit��Ч*/
    CODEC_OpVcAnd(pstEncInPara->pshwEncSpeech,
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                &ashwEncSpeech[0]);

    /* ����Homing֡��⣬�������ڱ��α��������Ӧ�� */
    shwResetFlag = (VOS_INT16)encoderHomingFrameTest(&ashwEncSpeech[0]);

    /*����HR�ṩ�Ľӿ�speechEncoder������HR���룬���20������(��VadFlag��SpFlag)*/
    speechEncoder(&ashwEncSpeech[0], (VOS_INT16 *)&stParamBuf);

    /*���ú���HR_UlFrameConvert��������֡��ʽת������18��������ȡ��112�����صı�����*/
    HR_UlFrameConvert((VOS_INT16 *)&stParamBuf, &ashwEncSerial[0]);

    /*���ú���ComCodecBits2DWords��112�������б�����ת��Ϊ���ֽڽ�����������*/
    CODEC_ComCodecBits2Bytes(&ashwEncSerial[0],
                           HR_CODED_BITS_LENGTH,
                           (VOS_INT8 *)pstEncOutPara->pshwEncSerial);

    /*����־VadFlag��SpFlag���*/
    pstEncOutPara->enVadFlag = stParamBuf.enVadFlag;
    pstEncOutPara->enSpFlag  = stParamBuf.enSpFlag;

    /*����⵽��֡ΪHomming֡������Ҫ���ñ�����*/
    if (1 == shwResetFlag)
    {
        /*����HR�ṩ�Ľӿ�resetEnc�����ñ�����*/
        resetEnc();
    }

    return CODEC_RET_OK;
}
VOS_UINT32 HR_Decode(
                CODEC_DEC_IN_PARA_STRU  *pstVocodecDecPara,
                VOS_INT16 *pshwDecSpeech)
{
    VOS_INT16                   shwResetFlag;
    VOS_INT16                   shwOldResetFlag;
    VOS_INT16                   ashwDecSerial[HR_CODED_BITS_LENGTH];

    HR_DECODE_SERIAL_STRU       stParamBuf;

    /*��ȡȫ�ֱ�����һ֡reset��־ָ��*/
    VOS_INT16 *pshwOldResetFlag = HR_GetOldResetPtr();

    /*��ʱ��������һ֡reset��־*/
    shwOldResetFlag = *pshwOldResetFlag;

    /*����������ֽڶ���������е�����ת��Ϊ112���صı�����*/
    CODEC_ComCodecBytes2Bits((VOS_INT8 *)(pstVocodecDecPara->pshwDecSerial),
                           HR_CODED_BITS_LENGTH,
                           &ashwDecSerial[0]);

    /*������112���صı������ϲ�Ϊ18�������Ĳ�����*/
    HR_DlFrameConvert(&ashwDecSerial[0], (VOS_INT16*)(&stParamBuf));

    /*��ȡ�����־*/
    stParamBuf.enBfiFlag    = pstVocodecDecPara->enBfiFlag;
    stParamBuf.enHrUfiFlag  = pstVocodecDecPara->enHrUfiFlag;
    stParamBuf.enSidFlag    = pstVocodecDecPara->enSidFlag;
    stParamBuf.enTafFlag    = pstVocodecDecPara->enTafFlag;

    if (1 == shwOldResetFlag)
    {
        /*��֡resetΪ1ʱ,��֡Homing��������һ��֡*/
        shwResetFlag = (VOS_INT16)decoderHomingFrameTest((VOS_INT16*)(&stParamBuf), HR_HOMING_FIRST_SUBFRAME);
    }
    else
    {
        /*����,����Homing���*/
        shwResetFlag = 0;
    }

    if (shwResetFlag && shwOldResetFlag)
    {
        /* ����֡ΪHoming֡,�ұ�֡Homing��һ��֡���ҲΪ��,��ֱ������������ΪHoming��ʽ */
        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_NB,
                      (VOS_INT16)CODEC_EHF_MASK);
    }
    else
    {
        /*����HR����speechDecoder���н��룬���PCM����*/
        speechDecoder((VOS_INT16*)(&stParamBuf), pshwDecSpeech);
    }

    /* ȡ�����PCM�����ĸ�13Bit��Ч */
    CODEC_OpVcAnd(&pshwDecSpeech[0],
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                &pshwDecSpeech[0]);

    /* ����֡��Homing֡,��֡�����Homingȫ֡��� */
    if (!shwOldResetFlag)
    {
        shwResetFlag = (VOS_INT16)decoderHomingFrameTest((VOS_INT16*)(&stParamBuf), HR_HOMING_WHOLE_FRAME);
    }

    /* ��ȫ֡���һ��֡������֡ΪHoming֡,������������,����������ΪHoming��ʽ */
    if (shwResetFlag)
    {
       resetDec();

       CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_NB,
                      (VOS_INT16)CODEC_EHF_MASK);
    }

    /*���汾֡Homing�����,������֡Ӧ��*/
    *pshwOldResetFlag = shwResetFlag;

    return CODEC_RET_OK;
}
VOS_VOID HR_DlFrameConvert(VOS_INT16 *pshwBits, VOS_INT16 *pshwEncodedSerial)
{
    VOS_INT16 i;

    /*HR������������ȡ���б�ָ��*/
    VOS_INT16 *pshwParsBitsNumTbl;

    /* ��������pshwBitsΪ112�����صı�����,��Voiceģʽ��־Ϊ34��35����(��0��ʼ����)*/
    if ((0 == pshwBits[34])&&(0 == pshwBits[35]))
    {
        /*Unvoicedģʽ,ʹ��Unvoicedģʽ�µı�����ȡ���б�*/
        pshwParsBitsNumTbl = (VOS_INT16 *)HR_GetUnvoiceParsNumPtr();
    }
    else
    {
        /*Voicedģʽ��ʹ��Voicedģʽ�µı�����ȡ���б�*/
        pshwParsBitsNumTbl = (VOS_INT16 *)HR_GetVoiceParsNumPtr();
    }

    /*���ݱ�����ȡ���б���в����ָ�����112�������ָ�Ϊ18������*/
    for (i = 0; i < HR_CODED_SERIAL_LENGTH; i++)
    {
        CODEC_ComCodecBits2Prm(pshwBits,
                             pshwParsBitsNumTbl[i],
                             pshwEncodedSerial++);

        pshwBits += pshwParsBitsNumTbl[i];
    }
}
VOS_VOID HR_UlFrameConvert(VOS_INT16 *pshwEncodedSerial, VOS_INT16 *pshwBits)
{
    VOS_INT16 i;

    /*HR������������ȡ���б�ָ��*/
    VOS_INT16 *pshwParsBitsNumTbl;

    /*��HR���������ṹ����HR����������������18������ */
    HR_ENCODE_SERIAL_STRU *pstSerial = (HR_ENCODE_SERIAL_STRU *)pshwEncodedSerial;

    /*��������ΪUnvoicedģʽ*/
    if (0 == pstSerial->shwVoicing)
    {
        /*ʹ��Unvoicedģʽ�µı�����ȡ���б�*/
        pshwParsBitsNumTbl = (VOS_INT16 *)HR_GetUnvoiceParsNumPtr();
    }
    else
    {
        /*ʹ��Voicedģʽ�µı�����ȡ���б�*/
        pshwParsBitsNumTbl = (VOS_INT16 *)HR_GetVoiceParsNumPtr();
    }

    /*���ݱ�����ȡ���б���б�����ȡ����18����������ȡ����112������*/
    for (i = 0; i < HR_CODED_SERIAL_LENGTH; i++)
    {
        CODEC_ComCodecPrm2Bits(pshwEncodedSerial[i],
                             pshwParsBitsNumTbl[i],
                             pshwBits);

        pshwBits += pshwParsBitsNumTbl[i];
    }
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



