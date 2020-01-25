

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_vec.h"
#include "codec_com_codec.h"
#include "amrwb_interface.h"
#include "amrwb_convert.h"
#include "WttfCodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e958 -e760  */
#include "amrwb_cod_main.h"
#include "amrwb_dec_main.h"
#include "amrwb_main.h"
#include "amrwb_dtx.h"
#include "amrwb_bits.h"
/*lint +e958 +e760  */

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
Coder_State                            *g_pstAmrwbEncObj = VOS_NULL;            /*AMRWB������״ָ̬��*/
Decoder_State                          *g_pstAmrwbDecObj = VOS_NULL;            /*AMRWB������״ָ̬��*/

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : AMRWB_Enc_Init
 ��������  : AMRWB�����ʼ������
 �������  : enDtxMode, ʹ��DTX����ģʽ, 1Ϊʹ��, 0Ϊ����
             pfCodecCallBack, ֡���ͻص�����
 �������  : ��
 �� �� ֵ  : �ɹ�����CODEC_RET_OK
             ʧ�ܷ���CODEC_RET_ERR
 ���ú���  : Init_coder()
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��15��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AMRWB_Enc_Init(
                CODEC_DTX_ENUM_UINT16       enDtxMode,
                VOS_VOID                   *pfCodecCallBack)
{
    Coder_State                **ppstEncObj;

    if (enDtxMode >= CODEC_DTX_BUTT)
    {
        return CODEC_RET_ERR;
    }

    /*��ȡȫ�ֱ���AMRWB����״̬�ṹ��ָ���ָ��*/
    ppstEncObj     = AMRWB_GetPtrOfEncObjPtr();

    /* ����AMRWB�����������ʼ������ */
    Init_coder((void **)ppstEncObj);

    /* ��ʼ��CODEC�ص������������VAD����ֱ�����֡���͸�RLC��*/
    (*ppstEncObj)->pfCallBack = (VOS_INT16 *)pfCodecCallBack;

    /* ����AMR_WB����������״̬ */
    Reset_tx_state();

    return CODEC_RET_OK;
}

/*****************************************************************************
 �� �� ��  : AMRWB_Dec_Init
 ��������  : AMRWB�����ʼ������
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ɹ�����CODEC_RET_OK
             ʧ�ܷ���CODEC_RET_ERR
 ���ú���  : AmrwbClrAllocatedObjCnt()
             Init_decoder()
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��15��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AMRWB_Dec_Init()
{
    VOS_UINT16                  *puhwSidFirstFlag;
    Decoder_State              **ppstDecObj;

    /*��ȡȫ�ֱ���AMRWBһ��ͨ���������Ƿ��յ���SID_FIRST֡��־,����ʼ��Ϊ0*/
    puhwSidFirstFlag  = AMRWB_GetSidFirstFlagPtr();
    *puhwSidFirstFlag = 0;

    /*��ȡȫ�ֱ���AMRWB����״̬�ṹ��ָ���ָ��*/
    ppstDecObj     = AMRWB_GetPtrOfDecObjPtr();

    /* ����AMRWB�����������ʼ������ */
    Init_decoder((void **)ppstDecObj);

    /* ����AMR_WB����������״̬ */
    Reset_rx_state();

    return CODEC_RET_OK;
}

/*****************************************************************************
 �� �� ��  : AMRWB_Encode
 ��������  : AMRWB����
 �������  : pstEncInPara      --  ָ��������������������ṹ���ָ��
 �������  : pstEncOutPara     --  ָ��������������ṹ���ָ��
 �� �� ֵ  : �ɹ�����CODEC_RET_OK
             ʧ�ܷ���CODEC_RET_ERR
 ���ú���  : encoder_homing_frame_test()
             Speech_Encode_Frame()
             AMRWB_UmtsUlConvert()
             AMRWB_GsmUlConvert()
             Speech_Encode_Frame_reset()
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��15��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AMRWB_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara)
{
    VOS_INT16                           *pshwSpeech;
    VOS_INT16                            shwResetFlag;
    VOS_INT16                            shwEncSerialLen;

    CODEC_AMRWB_MODE_ENUM_UINT16    enMode;
    CODEC_RET_ENUM_UINT32           enRetResult;
    CODEC_AMRWB_TYPE_TX_ENUM_UINT16 enTxType    = CODEC_AMRWB_TYPE_TX_SPEECH_GOOD;

    enum AMRWB_TXFrameType          enTxTypeTmp = (enum AMRWB_TXFrameType)enTxType;

    AMRWB_ENCODED_SERIAL_STRU       stSerial;
    Coder_State                    *pstAmrEncObj;

    /* ��Ҫ�ڱ���ǰ���������AMRWB���ʣ���ֹ����Ϣ��ID_GPHY_MED_UPDATE_PARA_CMD��Ϣ���� */
    VOS_UINT16                      enEncInMode;

    enRetResult     = CODEC_RET_OK;

    /*��ȡȫ�ֱ���AMRWB����״̬�ṹ��ָ��*/
    pstAmrEncObj    = AMRWB_GetEncObjPtr();

    pshwSpeech      = pstEncInPara->pshwEncSpeech;

    /* ��AMR���ʺϷ�ʱ������������������Ϣ */
    if ( (WTTFVOICE_AMRWB_CODECMODE_TYPE_ENUM_UINT16)pstEncInPara->enAmrMode < WTTFVOICE_AMRWB_CODECMODE_TYPE_BUTT )
    {
        enEncInMode = pstEncInPara->enAmrMode;
    }
    else
    {
        enEncInMode = WTTFVOICE_AMRWB_CODECMODE_TYPE_660K;
    }

    enMode     = AMRWB_GetRealEncMode(enEncInMode);

    /*�������PCM֡�Ƿ�Ϊhoming֡*/
    shwResetFlag = AMRWB_encoder_homing_frame_test(pshwSpeech);

    /* ���ڿ����16����PCM���������ȡ��14����������Ч */
    CODEC_OpVcAnd(pshwSpeech, (Word32)CODEC_FRAME_LENGTH_WB, CODEC_WB_PCM_MASK, pshwSpeech);

    /*����AMRWB����ӿڱ���һ֡*/
    AMRWB_coder((VOS_INT16 *)&enMode,
                pshwSpeech,
                stSerial.ashwEncSerial,
                &shwEncSerialLen,
                pstAmrEncObj,
                (VOS_INT16)pstEncInPara->enDtxMode,
                &enTxTypeTmp);

    enTxType    = (CODEC_AMRWB_TYPE_TX_ENUM_UINT16)enTxTypeTmp;

    /*��ȡ֡���ͼ�����ģʽ*/
    stSerial.uhwFrameType = enTxType;

    if (enTxType != CODEC_AMRWB_TYPE_TX_NO_DATA)
    {
        stSerial.enAmrMode = enEncInMode;
    }
    else
    {
        stSerial.enAmrMode = CODEC_AMRWB_MODE_MRDTX;
    }

    /*IF1 UMTS������ʽ*/
    if (CODEC_AMR_FORMAT_IF1 == pstEncInPara->enAmrFormat)
    {
        enRetResult = AMRWB_UmtsUlConvert(&stSerial,
                                        (VOICE_WPHY_AMR_SERIAL_STRU *)pstEncOutPara->pshwEncSerial);
    }
    /*GSM������ʽ*/
    else if (CODEC_AMR_FORMAT_BITS == pstEncInPara->enAmrFormat)
    {
        enRetResult = AMRWB_GsmUlConvert(&stSerial,
                                       pstEncOutPara->pshwEncSerial);
    }
    /*IMS������ʽ*/
    else if (CODEC_AMR_FORMAT_IMS == pstEncInPara->enAmrFormat)
    {
        /* �Ƚ���IF1 UMTS������ʽת�� */
        enRetResult = AMRWB_UmtsUlConvert(&stSerial,
                                        (VOICE_WPHY_AMR_SERIAL_STRU *)pstEncOutPara->pshwEncSerial);
        /* �ٽ���IMS������ʽת�� */
        AMRWB_ImsUlConvert(enTxType,enMode,pstEncOutPara->pshwEncSerial);
    }
    /*��֧��*/
    else
    {
        return CODEC_RET_ERR;
    }

    if (shwResetFlag != 0)
    {
        Reset_encoder(pstAmrEncObj, 1);
    }

    pstEncOutPara->enAmrFrameType = enTxType;

    /* ����AMRWB�ı������� */
    pstEncOutPara->enAmrMode = enMode;

    return enRetResult;
}

/*****************************************************************************
 �� �� ��  : AMRWB_Decode
 ��������  : AMRWB����
 �������  : pstDecPara     --ָ������������ṹ���ָ��
 �������  : pshwDecSpeech  --��������,����Ϊ160���ֽڵ�PCM����
 �� �� ֵ  : �ɹ�����CODEC_RET_OK
             ʧ�ܷ���CODEC_RET_ERR
 ���ú���  : AMRWB_UmtsDlConvert()
             decoder_homing_frame_test_first()
             Speech_Decode_Frame()
             decoder_homing_frame_test()
             Speech_Decode_Frame_reset()
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��15��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AMRWB_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech)
{

    VOS_INT16                           shwResetFlag    = 0;
    VOS_INT16                           shwOldResetFlag = 0;
    VOS_INT16                           shwFrameLen;
    AMRWB_ENCODED_SERIAL_STRU           stSerial;

    Decoder_State                      *pstAmrDecObj;

    CODEC_AMRWB_MODE_ENUM_UINT16        enMode;
    CODEC_AMRWB_TYPE_RX_ENUM_UINT16     enRxType;

    CODEC_RET_ENUM_UINT32               enRetResult = CODEC_RET_OK;

    /*���AMRWB����״̬�ṹ��ȫ�ֱ���ָ��*/
    pstAmrDecObj    = AMRWB_GetDecObjPtr();

    shwOldResetFlag = pstAmrDecObj->shwOldResetFlag;

    if (CODEC_AMR_FORMAT_IF1 == pstDecPara->enAmrFormat)
    {
        enRetResult = AMRWB_UmtsDlConvert((VOICE_WPHY_AMR_SERIAL_STRU *)pstDecPara->pshwDecSerial,
                                        &stSerial);

        enRxType = stSerial.uhwFrameType;
        enMode   = stSerial.enAmrMode;
    }
    else if (CODEC_AMR_FORMAT_BITS == pstDecPara->enAmrFormat)
    {
        enRxType    = pstDecPara->enAmrFrameType;
        enMode      = pstDecPara->enAmrMode;

        enRetResult = AMRWB_GsmDlConvert(pstDecPara->pshwDecSerial,
                                       enRxType,
                                       enMode,
                                       stSerial.ashwEncSerial);
    }
    else if (CODEC_AMR_FORMAT_IMS == pstDecPara->enAmrFormat)
    {
        /* ��ims�������ݽ�������,���г�UmtsԼ���ĸ�ʽ */
        enRetResult = AMRWB_ImsDlConvert((IMS_DL_DATA_OBJ_STRU *)pstDecPara->pshwDecSerial,&stSerial);

        enRxType = stSerial.uhwFrameType;
        enMode   = stSerial.enAmrMode;
        pstDecPara->uhwRxTypeForAjb = enRxType;

    }
    else
    {
        return CODEC_RET_ERR;
    }

    /* ������AMRWB��ʼ�����յ����׸�SID_FIRST/SID_UPDATE֡����Ҫ�޸�ΪSID_BAD֡ */
    if (    (CODEC_AMRWB_TYPE_RX_SID_FIRST == enRxType)
         || (CODEC_AMRWB_TYPE_RX_SID_UPDATE== enRxType))
    {
        if (AMRWB_SID_RESET_FLG_NO == pstAmrDecObj->shwRxSidResetFlg)
        {
            enRxType                       = CODEC_AMRWB_TYPE_RX_SID_BAD;
            pstAmrDecObj->shwRxSidResetFlg = AMRWB_SID_RESET_FLG_YES;
        }
    }

    if ((CODEC_AMRWB_TYPE_RX_NO_DATA     == enRxType)
     || (CODEC_AMRWB_TYPE_RX_SPEECH_LOST == enRxType))
    {
        enMode       = pstAmrDecObj->prev_mode;
        shwResetFlag = 0;
    }
    else
    {
        pstAmrDecObj->prev_mode = enMode;
    }

    /*�����һ֡ΪHOMING֡����֡���е�һ��֡HOMING֡���*/
    if ((1 == shwOldResetFlag) && (enMode < CODEC_AMRWB_MODE_MRDTX))
    {
        shwResetFlag = AMRWB_decoder_homing_frame_test_first(
                                        stSerial.ashwEncSerial,
                                        (Word16)enMode);
    }

    /*�����֡����һ֡��ΪHOMING֡��������ֱ֡����ΪPCM HOMING ֡*/
    if ((shwResetFlag != 0) && (shwOldResetFlag != 0))
    {
        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_WB,
                      CODEC_EHF_MASK);
    }
    else
    {
        /* ���ý��������н��� */
        AMRWB_decoder((VOS_INT16)enMode,
                      stSerial.ashwEncSerial,
                      pshwDecSpeech,
                      &shwFrameLen,
                      pstAmrDecObj,
                      (VOS_INT16)enRxType);

        /* ���ڿ����16����PCM���������ȡ��14����������Ч */
        CODEC_OpVcAnd(pshwDecSpeech, (Word32)CODEC_FRAME_LENGTH_WB, CODEC_WB_PCM_MASK, pshwDecSpeech);
    }

    /* �����һ֡��ΪHOMING֡����֡��Ҫ����ȫ֡HOMING֡��� */
    if ((0 == shwOldResetFlag) && (enMode < CODEC_AMRWB_MODE_MRDTX))
    {
        shwResetFlag = AMRWB_decoder_homing_frame_test(stSerial.ashwEncSerial, (VOS_INT16)enMode);
    }

    /* �����ǰ֡ΪHOMING֡����Խ��������г�ʼ��,����������ΪHoming��ʽ */
    if (shwResetFlag != 0)
    {
        Reset_decoder(pstAmrDecObj, 1);

        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_WB,
                      CODEC_EHF_MASK);
    }

    pstAmrDecObj->shwOldResetFlag = shwResetFlag;

    return enRetResult;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

