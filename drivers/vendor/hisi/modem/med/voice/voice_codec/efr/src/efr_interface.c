

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_com_codec.h"
#include "efr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "efr_codec.h"
#include "efr_e_homing.h"
#include "efr_d_homing.h"
#include "efr_sig_proc.h"
#include "efr_cnst.h"
#include "efr_dtx.h"

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
VOS_INT16 g_shwEfrOldResetFlag;                                                 /*��ʶ��һ��EFR�������Ƿ����������*/

VOS_INT16 efr_synth_buf[CODEC_FRAME_LENGTH_NB + EFR_M];                       /*EFR�����������л���*/

VOS_INT16 efr_dtx_mode;                                                         /*EFR�������DTX���ܴ򿪱�־*/

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


VOS_UINT32 EFR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode)
{
    /*��ȡȫ�ֱ���DTX��־ָ��*/
    VOS_INT16 *pshwDtxFlag      = EFR_GetDtxFlagPtr();

    /*��DTX��־*/
    *pshwDtxFlag      = (VOS_INT16)enDtxMode;

    /*��ʼ������ģ��*/
    EFR_reset_enc();

    return CODEC_RET_OK;
}
VOS_UINT32 EFR_Dec_Init(VOS_VOID)
{
    /*��ȡȫ�ֱ���FR��һ֡reset��־ָ��*/
    VOS_INT16 *pshwOldResetFlag = EFR_GetOldResetPtr();

    /*��reset��־*/
    *pshwOldResetFlag = 1;

    /*��ʼ������ģ��*/
    EFR_reset_dec();

    return CODEC_RET_OK;
}
VOS_UINT32 EFR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara)
{
    VOS_INT16     shwResetFlag;
    VOS_INT16     ashwParam[ERF_CODED_PRMS_LENGTH];                             /*������������������,��57������*/
    VOS_INT16     ashwSyn[CODEC_FRAME_LENGTH_NB];                           /*Debug��Ϣ������� */
    VOS_INT16     ashwSerial[EFR_CODED_BITS_LENGTH];                            /*��������������ȡ����������еı���������244������*/

    VOS_INT16    *pshwNewSpeech;
    VOS_INT16    *pshwDtxFlag;
    VOS_INT16    *pshwTxDtxCtrl;

    /*���ñ������ڲ�ȫ�ֱ��������б����������л���ָ��*/
    pshwNewSpeech          = EFR_GetNewSpeechPtr();

    /*���ñ������ڲ�ȫ�ֱ�����DTXģʽ��־ָ��*/
    pshwDtxFlag            = EFR_GetDtxFlagPtr();

    /*���ñ������ڲ�ȫ�ֱ���������DTX������*/
    pshwTxDtxCtrl          = &EFR_GetTxDtxCtrl();

    /*ÿ֡����DTX����*/
    *pshwDtxFlag = (VOS_INT16)(pstEncInPara->enDtxMode);

    /*������ǰPCM���ݿ���������ڲ�ȫ�ֱ�����ַ��*/

    /* ����EFR����EFR_encoder_homing_frame_test��������Homing֡��� */
    shwResetFlag = EFR_encoder_homing_frame_test (pstEncInPara->pshwEncSpeech);

    /*ȡPCM���ݸ�13Bit��Ч*/
    CODEC_OpVcAnd(pstEncInPara->pshwEncSpeech,
                CODEC_FRAME_LENGTH_NB,
                (VOS_INT16)CODEC_PCM_MASK,
                pshwNewSpeech);

    /* ����EFR����EFR_Pre_Process���б���Ԥ���������˲������� */
    EFR_Pre_Process(pshwNewSpeech, CODEC_FRAME_LENGTH_NB);

    /* ����EFR����EFR_Coder_12k2���б��룬���57������������Debug��Ϣ */
    EFR_Coder_12k2(&ashwParam[0], &ashwSyn[0]);

    /* ���������Ϊ������������������������*/
    if ((*pshwTxDtxCtrl & EFR_TX_SP_FLAG) == 0)
    {
        EFR_CN_encoding(&ashwParam[0], *pshwTxDtxCtrl);
    }

    /*����EFR����EFR_Prm2bits_12k2����57��������ȡΪ244����������еı�����*/
    EFR_Prm2bits_12k2(&ashwParam[0], &ashwSerial[0]);

    /*���������Ϊ������������Ҫ����ȡ���ĳЩ���ؽ��ж��⴦��S*/
    if ((*pshwTxDtxCtrl & EFR_TX_SP_FLAG) == 0)
    {
        EFR_sid_codeword_encoding(&ashwSerial[0]);
    }

    /*��244������������еı�������ʽת��Ϊ�������е�������ʽ*/
    CODEC_ComCodecBits2Bytes(&ashwSerial[0],
                           EFR_CODED_BITS_LENGTH,
                           (VOS_INT8 *)&(pstEncOutPara->pshwEncSerial[0]));

    /* ���ݱ����������״̬����ȡVadFlag��SpFlag��־ֵ */
    pstEncOutPara->enVadFlag = CODEC_VAD_SILENCE;
    pstEncOutPara->enSpFlag  = CODEC_SP_SID;

    if ((*pshwTxDtxCtrl & EFR_TX_VAD_FLAG) != 0)
    {
        pstEncOutPara->enVadFlag = CODEC_VAD_SPEECH;
    }
    if ((*pshwTxDtxCtrl & EFR_TX_SP_FLAG) != 0)
    {
        pstEncOutPara->enSpFlag = CODEC_SP_SPEECH;
    }

    /*������Homing���Ϊ�棬�����ñ����� */
    if (shwResetFlag != 0)
    {
        EFR_reset_enc ();
    }

    return CODEC_RET_OK;
}


VOS_UINT32 EFR_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech)
{
    VOS_INT16   shwResetFlag;
    VOS_INT16   ashwAzDec[EFR_AZ_SIZE];                                         /*������˲�ϵ������44��*/
    VOS_INT16   ashwSerial[EFR_CODED_BITS_LENGTH + 1];                          /*BFI+����������ϲ�ǰ����������еı���������1+244������*/

    VOS_INT16  *pshwSynthBuf;
    VOS_INT16  *pshwSynthSpBuf;

    VOS_INT16  *pshwOldResetFlag;
    VOS_INT16   shwOldResetFlag;

    EFR_DECODE_FRAME_STRU stEfrFrame;                                           /*��������ṹ��*/

    /*��ȡȫ�ֱ���EFR������������ָ��*/
    pshwSynthBuf   = EFR_GetSynthBufPtr();
    pshwSynthSpBuf = pshwSynthBuf + EFR_M;

    /*��ȡȫ�ֱ���EFR��һ֡reset��־ָ��*/
    pshwOldResetFlag = EFR_GetOldResetPtr();
    shwOldResetFlag  = *pshwOldResetFlag;

    /*��BFI��־ֱ�Ӹ���������������Ķ�Ӧλ�ã���Э�������У�δ���޸�*/
    ashwSerial[0] = (VOS_INT16)(pstDecPara->enBfiFlag);

    /*���������е�������ʽת��Ϊ244������������еı�������ʽ*/
    CODEC_ComCodecBytes2Bits((VOS_INT8 *)pstDecPara->pshwDecSerial,
                           EFR_CODED_BITS_LENGTH,
                           &ashwSerial[1]);

    /*����EFR����EFR_Bits2prm_12k2����1+244���صı�����ת��Ϊ1+57�������Ľ��������*/
    EFR_Bits2prm_12k2(&ashwSerial[0], (VOS_INT16 *)&stEfrFrame);

    /*����Э�飬��BFIΪFALSEʱ��������Homing���*/
    if (0 == stEfrFrame.enBfiFlag)
    {
        /*����֡����������(Homingȫ֡���ΪTRUE)����ֻ֡��Ҫ��������Homing��һ��֡���*/
        if (1 == shwOldResetFlag)
        {
            shwResetFlag = EFR_decoder_homing_frame_test(stEfrFrame.ashwSerial,
                                                         EFR_HOMING_FIRST_SUBFRAME);
        }
        else
        {
            /*���򣬲���Homing��⣬Ҳ�������ý�����*/
            shwResetFlag = 0;
        }
    }
    /* ���򣬱�֡Ϊ��֡,������֡��Homing���*/
    else
    {
         shwResetFlag = 0;
    }

    /*����֡ΪHoming֡,�ұ�֡Homing��һ��֡���ҲΪ��,��ֱ������������ΪHoming��ʽ*/
    if ((shwResetFlag != 0) && (shwOldResetFlag != 0))
    {
        CODEC_OpVecSet(pshwSynthSpBuf,
                     CODEC_FRAME_LENGTH_NB,
                     (VOS_INT16)CODEC_EHF_MASK);
    }
    else
    {
        /*����EFR����EFR_Decoder_12k2���н��룬���PCM����*/
        EFR_Decoder_12k2((VOS_INT16 *)&stEfrFrame,
                         &pshwSynthSpBuf[0],
                         &ashwAzDec[0],
                         (VOS_INT16)pstDecPara->enTafFlag,
                         (VOS_INT16)pstDecPara->enSidFlag);

        /*����EFR����EFR_Post_Filter���н�����������˲�����*/
        EFR_Post_Filter(&pshwSynthSpBuf[0], &ashwAzDec[0]);                      /* Post-filter */

        /*��15���ط�Χ��չ��16���ط�Χ��������1bit*/
        CODEC_OpVecShl(pshwSynthSpBuf,
                     CODEC_FRAME_LENGTH_NB,
                     1,
                     pshwSynthSpBuf);

        /*ȡ�����PCM�����ĸ�13Bit��Ч */
        CODEC_OpVcAnd(pshwSynthSpBuf,
                    CODEC_FRAME_LENGTH_NB,
                    (VOS_INT16)CODEC_PCM_MASK,
                    pshwSynthSpBuf);

    }

    CODEC_OpVecCpy(pshwDecSpeech,
                 pshwSynthSpBuf,
                 CODEC_FRAME_LENGTH_NB);

    /* ����֡BFIΪFALSE����֡��Homing֡,��֡�����Homingȫ֡��� */
    if ((0 == stEfrFrame.enBfiFlag) && (0 == shwOldResetFlag))
    {
        shwResetFlag = EFR_decoder_homing_frame_test(stEfrFrame.ashwSerial,
                                                     EFR_HOMING_WHOLE_FRAME);
    }

    /* ��ȫ֡���һ��֡������֡ΪHoming֡,������������,����������ΪHoming��ʽ */
    if (shwResetFlag != 0)
    {
       /*����EFR�ṩ�Ľӿ�EFR_reset_dec�����ý�����*/
        EFR_reset_dec();

        CODEC_OpVecSet(pshwSynthSpBuf,
                     CODEC_FRAME_LENGTH_NB,
                     (VOS_INT16)CODEC_EHF_MASK);
    }

    /*���汾֡Homing�����,������֡Ӧ��*/
    *pshwOldResetFlag = shwResetFlag;

    return CODEC_RET_OK;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

