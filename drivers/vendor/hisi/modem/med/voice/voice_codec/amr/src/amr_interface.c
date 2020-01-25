

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_vec.h"
#include "codec_com_codec.h"
#include "amr_interface.h"
#include "amr_convert.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "amr_comm.h"
#include "e_homing.h"
#include "d_homing.h"
#include "sp_enc.h"
#include "sp_dec.h"

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
Speech_Encode_FrameState               *g_pstAmrEncObj = VOS_NULL;                  /*AMR������״ָ̬��*/
Speech_Decode_FrameState               *g_pstAmrDecObj = VOS_NULL;                  /*AMR������״ָ̬��*/

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/


VOS_UINT32 AMR_Enc_Init(
                CODEC_DTX_ENUM_UINT16 enDtxMode,
                VOS_VOID             *pfCodecCallBack)
{
    CODEC_RET_ENUM_UINT32       enEncIntRet;

    Speech_Encode_FrameState   **ppstAmrEncObj;

    /*��ȡȫ�ֱ���AMR����״̬�ṹ��ָ���ָ��*/
    ppstAmrEncObj     = AMR_GetPtrOfEncObjPtr();

    /* ����AMR�����������ʼ������ */
    enEncIntRet = (VOS_UINT32)Speech_Encode_Frame_init(ppstAmrEncObj, enDtxMode, "encoder");

    /* ��ʼ��CODEC�ص������������VAD����ֱ�����֡���͸�RLC��*/
    if((*ppstAmrEncObj)->cod_amr_state != VOS_NULL)
    {
        (*ppstAmrEncObj)->cod_amr_state->pfCallBack = (VOS_INT16*)pfCodecCallBack;
    }

    return enEncIntRet;
}
VOS_UINT32 AMR_Dec_Init(VOS_VOID)
{
    VOS_UINT16                  *puhwSidFirstFlag;

    CODEC_RET_ENUM_UINT32        enDecIntRet;

    Speech_Decode_FrameState   **ppstAmrDecObj;

    /*��ȡȫ�ֱ���AMRһ��ͨ���������Ƿ��յ���SID_FIRST֡��־,����ʼ��Ϊ0*/
    puhwSidFirstFlag  = AMR_GetSidFirstFlagPtr();
    *puhwSidFirstFlag = 0;

    /*��ȡȫ�ֱ���AMR����״̬�ṹ��ָ���ָ��*/
    ppstAmrDecObj     = AMR_GetPtrOfDecObjPtr();

    /* ����AMR�����������ʼ������ */
    enDecIntRet = (VOS_UINT32)Speech_Decode_Frame_init(ppstAmrDecObj, "Decoder");

    return enDecIntRet;
}
VOS_UINT32 AMR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara)
{
    VOS_INT16                           *pshwSpeech;
    VOS_INT16                            shwResetFlag;

    enum Mode                            enMode     = MR475;
    enum Mode                            enUsedMode = MR475;
    enum TXFrameType                     enTxType   = TX_SPEECH_GOOD;

    AMR_ENCODED_SERIAL_STRU              stSerial;
    Speech_Encode_FrameState            *pstAmrEncObj;

    CODEC_RET_ENUM_UINT32            enRetResult = 0;

    enRetResult = CODEC_RET_OK;

    if ((VOS_NULL == pstEncInPara) || (VOS_NULL == pstEncOutPara))
    {
        return CODEC_RET_ERR;
    }

    /*��ȡȫ�ֱ���AMR����״̬�ṹ��ָ��*/
    pstAmrEncObj = AMR_GetEncObjPtr();

    pshwSpeech = pstEncInPara->pshwEncSpeech;
    /* ��AMR���ʺϷ�ʱ������������������Ϣ */
    if ( (enum Mode)pstEncInPara->enAmrMode <= MR122 )
    {
        enMode     = (enum Mode)pstEncInPara->enAmrMode;
    }

    /*�������PCM֡�Ƿ�Ϊhoming֡*/
    shwResetFlag = encoder_homing_frame_test(pshwSpeech);

    /*����AMR����ӿڱ���һ֡*/
    (VOS_VOID)Speech_Encode_Frame(pstAmrEncObj,
                                  enMode,
                                  pshwSpeech,
                                  stSerial.ashwEncSerial,
                                  &enUsedMode,
                                  &enTxType);

    /*��ȡ֡���ͼ�����ģʽ*/
    stSerial.uhwFrameType = (VOS_UINT16)enTxType;

    if (enTxType != TX_NO_DATA)
    {
        stSerial.enAmrMode = (CODEC_AMR_MODE_ENUM_UINT16)enMode;
    }
    else
    {
        stSerial.enAmrMode = CODEC_AMR_MODE_MRDTX;
    }

    /*IF1 UMTS������ʽ*/
    if (CODEC_AMR_FORMAT_IF1 == pstEncInPara->enAmrFormat)
    {
        enRetResult = AMR_UmtsUlConvert(&stSerial,
                                        (VOICE_WPHY_AMR_SERIAL_STRU *)pstEncOutPara->pshwEncSerial);
    }
    /*GSM������ʽ*/
    else if (CODEC_AMR_FORMAT_BITS == pstEncInPara->enAmrFormat)
    {
        enRetResult = AMR_GsmUlConvert(&stSerial,
                                       pstEncOutPara->pshwEncSerial);
    }
    /*IMS������ʽ*/
    else if (CODEC_AMR_FORMAT_IMS == pstEncInPara->enAmrFormat)
    {
        /* �Ƚ���IF1 UMTS������ʽת�� */
        enRetResult = AMR_UmtsUlConvert(&stSerial,
                                        (VOICE_WPHY_AMR_SERIAL_STRU *)pstEncOutPara->pshwEncSerial);
        /* �ٽ���IMS������ʽת�� */
        AMR_ImsUlConvert(enTxType,enMode,pstEncOutPara->pshwEncSerial);
    }
    /*��֧��*/
    else
    {
        return CODEC_RET_ERR;
    }

    if (CODEC_RET_ERR == enRetResult)
    {
        return CODEC_RET_ERR;
    }

    if (shwResetFlag != 0)
    {
        Speech_Encode_Frame_reset(pstAmrEncObj);
    }

    pstEncOutPara->enAmrFrameType = (CODEC_AMR_TYPE_TX_ENUM_UINT16)enTxType;

    /* ����AMR�������� */
    pstEncOutPara->enAmrMode = enMode;

    return CODEC_RET_OK;
}


VOS_UINT32 AMR_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech)
{

    VOS_INT16                            shwResetFlag    = 0;
    VOS_INT16                            shwOldResetFlag = 0;

    AMR_ENCODED_SERIAL_STRU              stSerial;

    Speech_Decode_FrameState            *pstAmrDecObj;

    CODEC_AMR_MODE_ENUM_UINT16           enMode;
    CODEC_AMR_TYPE_RX_ENUM_UINT16        enRxType;

    CODEC_RET_ENUM_UINT32                enRetResult;

    enRetResult     = CODEC_RET_OK;

    /*���AMR����״̬�ṹ��ȫ�ֱ���ָ��*/
    pstAmrDecObj    = AMR_GetDecObjPtr();

    shwOldResetFlag = pstAmrDecObj->shwOldResetFlag;

    if (CODEC_AMR_FORMAT_IF1 == pstDecPara->enAmrFormat)
    {
        enRetResult = AMR_UmtsDlConvert((VOICE_WPHY_AMR_SERIAL_STRU *)pstDecPara->pshwDecSerial,
                                        &stSerial);

        enRxType = stSerial.uhwFrameType;

        enMode   = (VOS_UINT16)stSerial.enAmrMode;
        if (CODEC_AMR_MODE_MRDTX == enMode)
        {
            enMode = pstAmrDecObj->prev_mode;
        }
    }
    else if (CODEC_AMR_FORMAT_BITS == pstDecPara->enAmrFormat)
    {
        enRxType    = pstDecPara->enAmrFrameType;
        enMode      = pstDecPara->enAmrMode;

        enRetResult = AMR_GsmDlConvert(pstDecPara->pshwDecSerial,
                                       enRxType,
                                       enMode,
                                       stSerial.ashwEncSerial);
    }
    else if (CODEC_AMR_FORMAT_IMS == pstDecPara->enAmrFormat)
    {
        /* ��ims�������ݽ�������,���г�UmtsԼ���ĸ�ʽ */
        enRetResult = AMR_ImsDlConvert((IMS_DL_DATA_OBJ_STRU *)pstDecPara->pshwDecSerial,&stSerial);

        enRxType = stSerial.uhwFrameType;

        enMode   = (MED_UINT16)stSerial.enAmrMode;
        if (CODEC_AMR_MODE_MRDTX == enMode)
        {
            enMode = pstAmrDecObj->prev_mode;
        }
        pstDecPara->uhwRxTypeForAjb = enRxType;
    }
    else
    {
        return CODEC_RET_ERR;
    }

    /* ������AMR��ʼ�����յ����׸�SID_FIRST/SID_UPDATE֡����Ҫ�޸�ΪSID_BAD֡�����޸ı�־λ */
    if (    (CODEC_AMR_TYPE_RX_SID_FIRST == enRxType)
         || (CODEC_AMR_TYPE_RX_SID_UPDATE== enRxType))
    {
        if (AMR_SID_RESET_FLG_NO == pstAmrDecObj->shwRxSidResetFlg)
        {
            enRxType                       = CODEC_AMR_TYPE_RX_SID_BAD;
            pstAmrDecObj->shwRxSidResetFlg = AMR_SID_RESET_FLG_YES;
        }
    }

    if (CODEC_AMR_TYPE_RX_NO_DATA == enRxType)
    {
        enMode = pstAmrDecObj->prev_mode;
    }
    else
    {
        pstAmrDecObj->prev_mode = (enum Mode)enMode;
    }

    /*�����һ֡ΪHOMING֡����֡���е�һ��֡HOMING֡���*/
    if ((1 == shwOldResetFlag) && (enMode < CODEC_AMR_MODE_MRDTX))
    {
        shwResetFlag = decoder_homing_frame_test_first(stSerial.ashwEncSerial,
                                                       (enum Mode)enMode);
    }

    /*�����֡����һ֡��ΪHOMING֡��������ֱ֡����ΪPCM HOMING ֡*/
    if ((shwResetFlag != 0) && (shwOldResetFlag != 0))
    {
        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_NB,
                      CODEC_EHF_MASK);
    }
    else
    {
        /* ���ý��������н��� */
        Speech_Decode_Frame(pstAmrDecObj,
                            (enum Mode)enMode,
                            stSerial.ashwEncSerial,
                            (enum RXFrameType)enRxType,
                            pshwDecSpeech);
    }

    /* �����һ֡��ΪHOMING֡����֡��Ҫ����ȫ֡HOMING֡��� */
    if ((0 == shwOldResetFlag) && (enMode < CODEC_AMR_MODE_MRDTX))
    {
        shwResetFlag = decoder_homing_frame_test(stSerial.ashwEncSerial,
                                                 (enum Mode)enMode);
    }

    /* �����ǰ֡ΪHOMING֡����Խ��������г�ʼ��,����������ΪHoming��ʽ */
    if (shwResetFlag != 0)
    {
        Speech_Decode_Frame_reset(pstAmrDecObj);

        CODEC_OpVecSet(&pshwDecSpeech[0],
                      CODEC_FRAME_LENGTH_NB,
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

