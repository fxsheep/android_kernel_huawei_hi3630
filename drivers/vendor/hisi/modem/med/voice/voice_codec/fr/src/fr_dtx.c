/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : fr_dtx.c
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��8��15��
  ����޸�   :
  ��������   : FR DTX����ģ�麯��ʵ��
  �����б�   :
              FR_DtxDecSidLarAndXmaxc
              FR_DtxDecSidOtherPrms
              FR_DtxControlRx
              FR_DtxControlTx
              FR_DtxRxControlState
              FR_DtxRxInit
              FR_DtxTxInit
              FR_DtxEncSidPrm
              FR_DtxGenSilence
              FR_DtxInterpolateCnVec
              FR_DtxPseudoNoise
              FR_DtxQxmax
              FR_DtxUpdateSidMem
  �޸���ʷ   :
  1.��    ��   : 2011��8��15��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "fr_dtx.h"
#include "fr_short_term.h"
#include "fr_lpc.h"
#include "fr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/*DTX֡����������ѯ��*/
const UWord16 g_auhwFrFrmType[FR_FRAME_TYPE_NUM] =
{
    FR_FRAME_TYPE_GOOD_SPEECH,                          /*BFI=0,SID=0*/
    FR_FRAME_TYPE_INVALID_SID,                          /*BFI=0,SID=1*/
    FR_FRAME_TYPE_VALID_SID,                            /*BFI=0,SID=2*/
    FR_FRAME_TYPE_INVALID_SID,                          /*BFI=0,SID=3*/
    FR_FRAME_TYPE_UNUSABLE,                             /*BFI=1,SID=0*/
    FR_FRAME_TYPE_INVALID_SID,                          /*BFI=1,SID=1*/
    FR_FRAME_TYPE_INVALID_SID,                          /*BFI=1,SID=2*/
    FR_FRAME_TYPE_INVALID_SID                           /*BFI=1,SID=3*/
};

/*��һ֡����֡���Ͳ�ѯ��*/
const UWord16 g_auhwFrPrevSp[5] =
{
    0,
    FR_RX_FIRST_SID_UPDATE,
    (FR_RX_FIRST_SID_UPDATE | FR_RX_INVALID_SID_FRAME),
    FR_RX_SP_FLAG,
    FR_RX_SP_FLAG
};

/*��һ֡Ϊ������֡���Ͳ�ѯ��*/
const UWord16 g_auhwFrPrevNotSp[10] =
{
    0,
    0,
    FR_RX_CONT_SID_UPDATE,
    FR_RX_CONT_SID_UPDATE,
    (FR_RX_CONT_SID_UPDATE | FR_RX_INVALID_SID_FRAME),
    (FR_RX_CONT_SID_UPDATE | FR_RX_INVALID_SID_FRAME),
    (FR_RX_SP_FLAG | FR_RX_FIRST_SP_FLAG),
    (FR_RX_SP_FLAG | FR_RX_FIRST_SP_FLAG | FR_RX_PREV_DTX_MUTING),
    FR_RX_CNI_BFI,
    FR_RX_CNI_BFI
};

/*��������ʸ���ڲ����ӱ�*/
const Word16 g_ashwFrFrateFactor[FR_DTX_CN_INT_PERIOD*2] =
{
    0x0555, 0x7aab,
    0x0aaa, 0x7556,
    0x1000, 0x7000,
    0x1555, 0x6aab,
    0x1aaa, 0x6556,
    0x2000, 0x6000,
    0x2555, 0x5aab,
    0x2aaa, 0x5556,
    0x3000, 0x5000,
    0x3555, 0x4aab,
    0x3aaa, 0x4556,
    0x4000, 0x4000,
    0x4555, 0x3aab,
    0x4aaa, 0x3556,
    0x5000, 0x3000,
    0x5555, 0x2aab,
    0x5aaa, 0x2556,
    0x6000, 0x2000,
    0x6555, 0x1aab,
    0x6aaa, 0x1556,
    0x7000, 0x1000,
    0x7555, 0x0aab,
    0x7aaa, 0x0556,
    0x7fff, 0x0001
};

/* ����֡����LARs��RPE Pulses */
const Word16 g_ashwFrFrateSidPara[21] =
{
    42,39,21,10,9,4,3,2,                /* ����֡����LARs[8] */
    3,4,3,4,4,3,3,3,3,4,4,3,3           /* ����֡����RPE Pulses[13] */
};



/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : FR_DtxTxInit
 ��������  : DTX���б����ʼ��
 �������  : FR_ENC_DTX_STATE_STRU * pstEncDtxState  --DTX����ṹ��ָ��
 �������  : FR_ENC_DTX_STATE_STRU * pstEncDtxState  --DTX����ṹ��ָ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxTxInit(FR_ENC_DTX_STATE_STRU * pstEncDtxState)
{
    if (VOS_NULL == pstEncDtxState)
    {
        return;
    }

    /*��ʼ��hangoverʱ��*/
    pstEncDtxState->shwHangover = FR_DTX_HANGOVER;

    /*��ʼ��������һ��SID����֡ʱ��*/
    pstEncDtxState->shwNelapsed = FR_DTX_INIT_VALUE_NELAPSED;    /*FR_DTX_INIT_VALUE_NELAPSED*/

    /*��ʼ�������DTX��������*/
    pstEncDtxState->shwCtrlWord = (FR_TX_SP_FLAG | FR_TX_VAD_FLAG);

    /*��ʼ�������PN��������*/
    pstEncDtxState->swPnSeed    = FR_DTX_PN_INITIAL_SEED;
}


/*****************************************************************************
 �� �� ��  : FR_DtxRxInit
 ��������  : DTX���п��Ƴ�ʼ��
 �������  : FR_DEC_DTX_STATE_STRU * pstDecDtxState  --DTX����ṹ��ָ��
 �������  : FR_DEC_DTX_STATE_STRU * pstDecDtxState  --DTX����ṹ��ָ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxRxInit(FR_DEC_DTX_STATE_STRU * pstDecDtxState)
{
    if (VOS_NULL == pstDecDtxState)
    {
        return;
    }

    /* suppose infinitely long speech period before start */

    /*��ʼ��hangoverʱ��*/
    pstDecDtxState->shwHangoverPeriod  = FR_DTX_HANGOVER;

    /*��ʼ��������һ��SID֡ʱ��*/
    pstDecDtxState->shwNelapsed        = FR_DTX_INIT_VALUE_NELAPSED;

    /*��ʼ�������DTX������*/
    pstDecDtxState->shwCtrlWord        = FR_TX_SP_FLAG;

    /*��ʼ��PN������������*/
    pstDecDtxState->swPnSeed           = FR_DTX_PN_INITIAL_SEED;

    /*��ʼ������������������״̬*/
    pstDecDtxState->shwCniState        = FR_DTX_CN_INT_PERIOD - 1;

    /*��ʼ����ʧ��SID֡���ۼ�ֵ*/
    pstDecDtxState->shwSidLostFrameNum = 0;

    /*��ʼ����һ֡BFI��־*/
    pstDecDtxState->enPrevBfi          = FR_BFI_STATUS_NOT;

    /*��ʼ��״̬��*/
    pstDecDtxState->shwStateCnt        = 0;
}


/*****************************************************************************
 �� �� ��  : FR_DtxControlTx
 ��������  : FR����DTX����
 �������  : FR_VAD_STATUS_ENUM_UINT16  enVadFlag       --VAD����־
             FR_ENC_DTX_STATE_STRU *pstEncDtxState  --DTX�������ṹ��
 �������  : FR_ENC_DTX_STATE_STRU *pstEncDtxState  --DTX�������ṹ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxControlTx(
                 FR_VAD_STATUS_ENUM_UINT16  enVadFlag,
                 FR_ENC_DTX_STATE_STRU     *pstEncDtxState)
{
    Word16                                  shwTmp;

    /* �ۼ����Ӿ�����һ��SID����֡��֡��shwNelapsed */
    pstEncDtxState->shwNelapsed         = add(pstEncDtxState->shwNelapsed, 1);

    /* ���VAD��־Ϊ1��ֱ�ӷ��� */
    if (FR_VAD_STATUS_YES == enVadFlag)
    {
        pstEncDtxState->shwHangover     = FR_DTX_HANGOVER;
        pstEncDtxState->shwCtrlWord     = (FR_TX_SP_FLAG | FR_TX_VAD_FLAG);
        return;
    }

    /* �����ӳ��ڵ���0,��ʾ�����ӳ���,��hangover��������Ҫ����SID */
    if (0 == pstEncDtxState->shwHangover)
    {
        /* �����ӳ��ڵ���0,��ʾ�����ӳ���,��hangover��������Ҫ����SID */
        pstEncDtxState->shwNelapsed     = 0;

        if ((pstEncDtxState->shwCtrlWord & FR_TX_HANGOVER_ACTIVE) != 0)
        {
            pstEncDtxState->shwCtrlWord = FR_TX_PREV_HANGOVER_ACTIVE
                                          | FR_TX_SID_UPDTE;
            pstEncDtxState->swPnSeed    = FR_DTX_PN_INITIAL_SEED;
        }
        else
        {
            pstEncDtxState->shwCtrlWord = FR_TX_SID_UPDTE;
        }
    }
    else
    {
        pstEncDtxState->shwHangover     = sub(pstEncDtxState->shwHangover, 1);

        shwTmp = add(pstEncDtxState->shwNelapsed, pstEncDtxState->shwHangover);

        if (shwTmp < FR_DTX_ELAPSED_THRESHOLD)
        {
            /* ʹ����һ֡SID֡����  */
            pstEncDtxState->shwCtrlWord = FR_TX_USE_OLD_SID;
        }
        else
        {
            if ((pstEncDtxState->shwCtrlWord & FR_TX_HANGOVER_ACTIVE) != 0)
            {
                pstEncDtxState->shwCtrlWord = (FR_TX_PREV_HANGOVER_ACTIVE
                                            | FR_TX_HANGOVER_ACTIVE
                                            | FR_TX_SP_FLAG);
            }
            else
            {
                pstEncDtxState->shwCtrlWord = (FR_TX_HANGOVER_ACTIVE
                                            | FR_TX_SP_FLAG);
            }
        }
    }
}

/*****************************************************************************
 �� �� ��  : FR_DtxControlRx
 ��������  : FR����DTX���ƺͶ�ʧ֡�ָ���������֡��־����DTX�����֣����趨��Ӧ֡���ͽ������
 �������  : FR_TAF_STATUS_ENUM_UINT16  enTaf           --TAF֡��־
             FR_BFI_STATUS_ENUM_UINT16  enBfi           --BFI֡��־
             FR_SID_STATUS_ENUM_UINT16  enSidFlag       --SID֡��־
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --����DTX���ƽṹ��״̬
 �������  : Word16                    *pshwState       --DTX״̬��
             FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi      --��һ֡BFI��־
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --����DTX���ƽṹ��״̬
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxControlRx(
                 FR_TAF_STATUS_ENUM_UINT16                  enTaf,
                 FR_BFI_STATUS_ENUM_UINT16                  enBfi,
                 FR_SID_STATUS_ENUM_UINT16                  enSidFlag,
                 Word16                                    *pshwState,
                 FR_BFI_STATUS_ENUM_UINT16                 *penPrevBfi,
                 FR_DEC_DTX_STATE_STRU                     *pstDecDtxState)
{
    UWord16                             uhwFrameType;
    Word16                             *pshwCtrlWord;
    UWord16                             uhwIndex;

    /* ����֡��־����DTX���нṹ�����������״ֵ̬shwCtrlWord */
    pshwCtrlWord      = (Word16 *)&(pstDecDtxState->shwCtrlWord);

    uhwIndex          = (UWord16)(enBfi << 2) + enSidFlag;
    if (uhwIndex >= FR_FRAME_TYPE_NUM)
    {
        return;
    }

    uhwFrameType      = g_auhwFrFrmType[uhwIndex];

    /* �ȸ���pshwCtrlWord */
    if ((*pshwCtrlWord & FR_RX_SP_FLAG) != 0)
    {
        *pshwCtrlWord = (Word16)g_auhwFrPrevSp[uhwFrameType];
    }
    else
    {
        uhwIndex      = (0 == *pshwCtrlWord) ? 0 : 1;
        uhwIndex     += (UWord16)(uhwFrameType << 1);
        *pshwCtrlWord = (Word16)g_auhwFrPrevNotSp[uhwIndex];
    }

    /* ��������������������״̬ */
    FR_DtxUpdateCniState(enTaf, pshwCtrlWord, uhwFrameType, pstDecDtxState);

    /* ����SID֡״̬��¼���� */
    FR_DtxUpdateSidRxState(pshwCtrlWord, pstDecDtxState);

    /* ���¶�ʧ֡����״̬�� */
    FR_DtxRxControlState(*pshwCtrlWord, enBfi, penPrevBfi, pshwState);
}

/*****************************************************************************
 �� �� ��  : FR_DtxUpdateCniState
 ��������  : ��������������������״̬
 �������  : FR_TAF_STATUS_ENUM_UINT16  enTaf       --TAF֡��־
             Word16                    *pshwCtrlWord   -- ������
             UWord16                    uhwFrameType-- ֡����
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --����DTX���ƽṹ��״̬
 �������  : Word16                    *pshwState       --DTX״̬��
             FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi      --��һ֡BFI��־
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --����DTX���ƽṹ��״̬
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxUpdateCniState(
                 FR_TAF_STATUS_ENUM_UINT16                  enTaf,
                 Word16                                    *pshwCtrlWord,
                 UWord16                                    uhwFrameType,
                 FR_DEC_DTX_STATE_STRU                     *pstDecDtxState)
{
    /* ����֡���� */
    if ((*pshwCtrlWord & FR_RX_SP_FLAG) != 0)
    {
        pstDecDtxState->shwSidLostFrameNum      = 0;
        pstDecDtxState->shwCniState             = FR_DTX_CN_INT_PERIOD - 1;
    }
    else
    {
        /* ��һ����ЧSID֡���� */
        if ((*pshwCtrlWord & FR_RX_FIRST_SID_UPDATE) != 0)
        {
            pstDecDtxState->shwSidLostFrameNum  = 0;
            pstDecDtxState->shwCniState         = FR_DTX_CN_INT_PERIOD - 1;
        }

        /* �ǵ�һ��SID֡���� */
        if ((*pshwCtrlWord & FR_RX_CONT_SID_UPDATE) != 0)
        {
            pstDecDtxState->shwSidLostFrameNum  = 0;

            if (FR_FRAME_TYPE_VALID_SID == uhwFrameType)
            {
                pstDecDtxState->shwCniState     = 0;
            }
            else
            {
                if ((FR_FRAME_TYPE_INVALID_SID == uhwFrameType)
                 && (pstDecDtxState->shwCniState < (FR_DTX_CN_INT_PERIOD - 1)))
                {
                    pstDecDtxState->shwCniState = (pstDecDtxState->shwCniState + 1);
                }
            }
         }

        /* ����BFIΪ1��SID֡���� */
        if ((*pshwCtrlWord & FR_RX_CNI_BFI) != 0)
        {

            if (pstDecDtxState->shwCniState < (FR_DTX_CN_INT_PERIOD - 1))
            {
                pstDecDtxState->shwCniState = (pstDecDtxState->shwCniState + 1);
            }

            /* ����ڽ�����������ʱ����TAF��־1ʱ����ǰ֡��Ϊ��ʧSID֡����*/
            if (FR_TAF_STATUS_YES == enTaf)
            {
                *pshwCtrlWord = *pshwCtrlWord | FR_RX_LOST_SID_FRAME;

                pstDecDtxState->shwSidLostFrameNum = add(pstDecDtxState->shwSidLostFrameNum, 1);
            }
            else
            {
                *pshwCtrlWord = *pshwCtrlWord | FR_RX_NO_TRANSMISSION;

            }

            /* ��ʧ��SID֡���ۼƱ���shwSidLostFrameNum ���� 1 */
            if (pstDecDtxState->shwSidLostFrameNum > 1)
            {
                *pshwCtrlWord = *pshwCtrlWord | FR_RX_DTX_MUTING;
            }
        }
    }
}

/*****************************************************************************
 �� �� ��  : FR_DtxUpdateSidRxState
 ��������  : ����SID֡״̬��¼����
 �������  : Word16                    *pshwCtrlWord   -- ������
             FR_DEC_DTX_STATE_STRU *pstDecDtxState  --����DTX���ƽṹ��״̬
 �������  : FR_DEC_DTX_STATE_STRU *pstDecDtxState  --����DTX���ƽṹ��״̬
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxUpdateSidRxState(
                Word16                 *shwCtrlWord,
                FR_DEC_DTX_STATE_STRU  *pstDecDtxState)
{
    /* ������һ��SID֡ʱ��shwNelapsed��1 */
    pstDecDtxState->shwNelapsed = add(pstDecDtxState->shwNelapsed , 1);

    if ((*shwCtrlWord & FR_RX_SP_FLAG) != 0)
    {
        pstDecDtxState->shwHangoverPeriod     = FR_DTX_HANGOVER;
    }
    else
    {
        /* ������һ��SID֡ʱ��shwNelapsed������ֵ */
        if (pstDecDtxState->shwNelapsed > FR_DTX_ELAPSED_THRESHOLD)
        {
            *shwCtrlWord                     |= FR_RX_UPD_SID_QUANT_MEM;
            pstDecDtxState->shwNelapsed       = 0;
            pstDecDtxState->shwHangoverPeriod = 0;
            pstDecDtxState->swPnSeed          = FR_DTX_PN_INITIAL_SEED;
        }
        else if (0 == pstDecDtxState->shwHangoverPeriod) /* hangoverʱ��Ϊ0 */
        {
            pstDecDtxState->shwNelapsed       = 0;
        }
        else
        {
            pstDecDtxState->shwHangoverPeriod = (pstDecDtxState->shwHangoverPeriod - 1);
        }
    }
}

/*****************************************************************************
 �� �� ��  : FR_DtxRxControlState
 ��������  : ��ʧ֡����״̬������
 �������  : Word16                     shwCtrlWord    --DTX״̬��������
             FR_BFI_STATUS_ENUM_UINT16  enBfi          --��ǰBFI֡��־
 �������  : FR_BFI_STATUS_ENUM_UINT16 *penPrevBfi     --��һ֡BFI֡��־
             Word16                    *pshwState      --��ʧ֡����״̬��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��9��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxRxControlState(
                 Word16                                     shwCtrlWord,
                 FR_BFI_STATUS_ENUM_UINT16                  enBfi,
                 FR_BFI_STATUS_ENUM_UINT16                 *penPrevBfi,
                 Word16                                    *pshwState)
{
    if (enBfi != CODEC_BFI_NO)
    {
        *pshwState = (*pshwState + 1); /* BFI֡��ʧ֡����״̬��pshwState��1 */
    }
    else if (6 == *pshwState)
    {
        *pshwState = 5;
    }
    else
    {
        *pshwState = 0;
    }

    /* ��ʧ֡����״̬��pshwState����6����Ϊ6 */
    if (*pshwState > 6)
    {
        *pshwState = 6;
    }

    /* ���յ�һ������֡ */
    if ((shwCtrlWord & FR_RX_FIRST_SP_FLAG) != 0)
    {
        *pshwState  = 5;
        *penPrevBfi = CODEC_BFI_NO;
    }
    else if ((shwCtrlWord & FR_RX_PREV_DTX_MUTING) != 0) /* ǰһ֡Ϊ����֡ */
    {
        *pshwState  = 5;
        *penPrevBfi = 1;
    }
    else
    {
        /* suspend pc lint */
    }
}


/*****************************************************************************
 �� �� ��  : FR_DtxEncSidPrm
 ��������  : FR���������������ƣ���SID֡��������
 �������  : FR_ENC_DTX_STATE_STRU *pstEncDtxState  --����DTX���ƽṹ��
 �������  : Word16                    *pshwLarc        --����ϵ�����������
             Word16                    *pshwXmaxc       --�ӿ�������ֵ
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxEncSidPrm(
                 Word16                                    *pshwLarc,
                 Word16                                    *pshwXmaxc,
                 FR_ENC_DTX_STATE_STRU                 *pstEncDtxState)
{
    Word32                              swCnt1, swCnt2;
    Word32                              swAcc;
    Word16                              shwXmaxc;

    /* ʹ����һ֡SID֡���� */
    if (pstEncDtxState->shwCtrlWord & FR_TX_USE_OLD_SID)
    {
        CODEC_OpVecCpy(pshwLarc, pstEncDtxState->ashwSidLar, FR_DTX_NUM_OF_LAR_COEF);
        CODEC_OpVecCpy(pshwXmaxc, pstEncDtxState->ashwSidXmax, FR_DTX_NUM_OF_BLOCK_COEF);
    }
    else
    {
        /* ���㵱ǰSID�Ĳ���--���������SID_LAR[8]: LARcHist[4][8] ÿ��4��Ԫ�����ֵ */
        for (swCnt1 = 0; swCnt1 < 8; swCnt1++)
        {
            swAcc = 2;

            for(swCnt2 = 0; swCnt2 < 4; swCnt2++)
            {
                swAcc = L_add(swAcc, pstEncDtxState->ashwLarcHist[swCnt2][swCnt1]);
            }

            pshwLarc[swCnt1] = saturate(L_shr(swAcc, 2));
        }

        /* �Է���ϵ�����������pshwLARc�������� */
        Quantization_and_coding(pshwLarc);

        CODEC_OpVecCpy(pstEncDtxState->ashwSidLar, pshwLarc, 8);

        /* ���㵱ǰSID�Ĳ���--ashwSidXmax[4] */

        swAcc = 8;

        for(swCnt1 = 0; swCnt1 < 4; swCnt1++)
        {
            for(swCnt2 = 0; swCnt2 < 4; swCnt2++)
            {
                swAcc = L_add(swAcc, pstEncDtxState->ashwXmaxHist[swCnt2][swCnt1]);
            }
        }

        /* ���ӿ�������ֵpshwXmaxc[4]�������� */
        shwXmaxc = FR_DtxQxmax(saturate(L_shr(swAcc, 4)));

        CODEC_OpVecSet(&pshwXmaxc[0], 4, shwXmaxc);

        CODEC_OpVecCpy(pstEncDtxState->ashwSidXmax, pshwXmaxc, 4);
    }
}

/*****************************************************************************
 �� �� ��  : FR_DtxQxmax
 ��������  : ���ӿ�������ֵ������������
 �������  : Word16 shwXmax        --����ǰ�ӿ�������ֵ
 �������  : ��
 �� �� ֵ  : Word16                --�������ӿ�������ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��9��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 FR_DtxQxmax(Word16 shwXmax)
{
    Word16                              shwTemp, shwExp;
    Word16                              shwXmaxc;

    /* ȡshwXmax��7λ */
    shwTemp = (Word16)(shwXmax & 0xfe00);

    if (!shwTemp)
    {
        shwExp = 0;
    }
    else
    {
        shwExp = 6 - norm_s(shwTemp);
    }

    /* �������ӿ�������ֵshwXmaxc */
    shwTemp = shwExp + 5;
    shwXmaxc = add(shr(shwXmax, shwTemp), shl(shwExp, 3));

    return shwXmaxc;
}

/*****************************************************************************
 �� �� ��  : FR_DtxUpdateSidMem
 ��������  : DTX�ṹ��Sid���ɲ�������ϵ������Ⱥ��ӿ�������ֵ�ڴ����
 �������  : INT16 *LARuq                  --��ǰ֡δ��������ϵ�����������
             INT16 *xmaxuq                 --��ǰ֡δ�����ӿ�������ֵ
             FR_ENC_DTX_STATE_STRU *pstEncDtxState   --DTX�������ṹ��
 �������  : FR_ENC_DTX_STATE_STRU *pstEncDtxState   --DTX�������ṹ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��9��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxUpdateSidMem(
                Word16                                     *pshwLaruq,
                Word16                                     *pshwXmaxuq,
                FR_ENC_DTX_STATE_STRU                      *pstEncDtxState)
{
    Word16                              shwIndex;

    /* ����ʱ��������indexΪ�ṹ��״̬��������֡����ֵashwHistIndx */
    shwIndex = pstEncDtxState->shwHistIndx;

    CODEC_OpVecCpy(&(pstEncDtxState->ashwLarcHist[shwIndex][0]), pshwLaruq, 8);
    CODEC_OpVecCpy(&(pstEncDtxState->ashwXmaxHist[shwIndex][0]), pshwXmaxuq, 4);

    shwIndex = (shwIndex + 1) & 0x0003;

    /* ���½ṹ��״̬��������֡����ֵashwHistIndx */
    pstEncDtxState->shwHistIndx = shwIndex;
}

/*****************************************************************************
 �� �� ��  : FR_DtxDecSidLarAndXmaxc
 ��������  : FR���в���������������ϵ����������Ⱥ��ӿ�������ֵ
 �������  : Word16                    *pshwLarc       --LARc����
             Word16                    *pshwLarppOld   --ǰһ֡����LAR����
             Word16                    *pshwXmaxc      --�ӿ�������ֵ
             FR_BFI_STATUS_ENUM_UINT16  enBfi          --BFI��־
             FR_DEC_DTX_STATE_STRU *pstDecDtxState --����DTX���ƽṹ��״̬
 �������  : Word16                    *pshwLarppNew   --��ǰ֡����LAR����
             Word16                    *pshwXmaxc      --�ӿ�������ֵ
             FR_DEC_DTX_STATE_STRU *pstDecDtxState --����DTX���ƽṹ��״̬
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxDecSidLarAndXmaxc(
                 Word16                                    *pshwLarc,
                 Word16                                    *pshwLarppNew,
                 Word16                                    *pshwLarppOld,
                 Word16                                    *pshwXmaxc,
                 FR_BFI_STATUS_ENUM_UINT16                  enBfi,
                 FR_DEC_DTX_STATE_STRU                     *pstDecDtxState)
{
    Word32                              swCnt;
    Word16                              shwMuteFlag = 0;
    Word16                              shwCtrlWord = pstDecDtxState->shwCtrlWord;

    /* ��BFI֡���� */
    if (FR_BFI_STATUS_NOT == enBfi)
    {
        /* ��LARc���������������pshwLarppNew */
        Decoding_of_the_coded_Log_Area_Ratios(pshwLarc, pshwLarppNew);

        /* ���յ�һ����ЧSID֡ */
        if (shwCtrlWord & FR_RX_FIRST_SID_UPDATE)
        {
            CODEC_OpVecCpy(pstDecDtxState->ashwLarCnOld, pshwLarppNew, 8);
            CODEC_OpVecCpy(pstDecDtxState->ashwXmaxCnOld, pshwXmaxc, 4);
        }
        else
        {
            /* SID update֡, ���������������� */
            CODEC_OpVecCpy(pstDecDtxState->ashwLarCnOld, pstDecDtxState->ashwLarCnNew, 8);
            CODEC_OpVecCpy(pstDecDtxState->ashwXmaxCnOld, pstDecDtxState->ashwXmaxCnNew, 4);
        }

        CODEC_OpVecCpy(pstDecDtxState->ashwLarCnNew, pshwLarppNew, 8);
        CODEC_OpVecCpy(pstDecDtxState->ashwXmaxCnNew, pshwXmaxc, 4);
    }
    else  /* BFI֡���� */
    {
        if (shwCtrlWord & FR_RX_NO_TRANSMISSION) /* ��������������֡ */
        {
            /* �ڲ����ǰ֡����LAR����pshwLarppNew[8] */
            FR_DtxInterpolateCnVec(pstDecDtxState->ashwLarCnOld,
                                   pstDecDtxState->ashwLarCnNew,
                                   pshwLarppNew,
                                   pstDecDtxState->shwCniState,
                                   8);

            /* �ڲ����ǰ֡�ӿ�������ֵpshwXmaxc[4] */
            FR_DtxInterpolateCnVec(pstDecDtxState->ashwXmaxCnOld,
                                   pstDecDtxState->ashwXmaxCnNew,
                                   pshwXmaxc,
                                   pstDecDtxState->shwCniState,
                                   4);
        }
        else
        {
            /* ��һ����ʧ������֡��SID֡���� */
            if (((shwCtrlWord & FR_RX_SP_FLAG)
              && (1 == pstDecDtxState->shwStateCnt))
              || (shwCtrlWord & FR_RX_DTX_MUTING))
            {
                /* �ظ���һ�κõ�LARs��Xmax */
                CODEC_OpVecCpy(pshwLarppNew, pstDecDtxState->ashwLarCnNew, 8);
                CODEC_OpVecCpy(pshwXmaxc, pstDecDtxState->ashwXmaxCnNew, 4);
            }
            else
            {
                /* �ǵ�һ����ʧ����֡���þ�����־Ϊ1 */
                shwMuteFlag = 1;
            }
        }
    }

    if (1 == shwMuteFlag)
    {
        /* �������Ļ�֡���ظ���һ�κõ�LARs����4�ݼ�Xmaxֱ��Ϊ0 */
        CODEC_OpVecCpy(pshwLarppNew, pstDecDtxState->ashwLarCnNew, 8);

        for (swCnt = 0; swCnt < 4; swCnt++)
        {
            pshwXmaxc[swCnt] = pstDecDtxState->ashwXmaxCnNew[swCnt] - 4;
            if (pshwXmaxc[swCnt] < 0)
            {
                pshwXmaxc[swCnt] = 0;
            }
        }

        CODEC_OpVecCpy(pstDecDtxState->ashwXmaxCnNew, pshwXmaxc, 4);

        /* �ۼ�muting֡�� */
        pstDecDtxState->shwMuteCount = add(pstDecDtxState->shwMuteCount, 1);
    }
    else
    {
        /* muting֡���ۼƹ�0 */
        pstDecDtxState->shwMuteCount = 0;
    }
}

/*****************************************************************************
 �� �� ��  : FR_DtxInterpolateCnVec
 ��������  : ��������ʸ���ڲ�
 �������  : Word16 pshwOldCn[]           --�ڲ�ο�ʸ��2
             Word16 pshwNewCn[]           --�ڲ�ο�ʸ��1
             Word16 shwRxDtxState        --���������ڲ�����״̬
             Word16 shwLen                --�ڲ�ʸ������
 �������  : Word16 pshwInterpCn[]        --�ڲ����ʸ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��9��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxInterpolateCnVec(
                Word16                  pshwOldCn[],
                Word16                  pshwNewCn[],
                Word16                  pshwInterpCn[],
                Word16                  shwRxDtxState,
                Word16                  shwLen)
{
    Word32                              swCnt;
    Word16                              shwFrateFactor1, shwFrateFactor2;
    Word16                              shwValue;
    Word32                              swTemp;

    /* �����ڲ�����shwFrateFactor1��shwFrateFactor2 */
    shwValue                 = shl(shwRxDtxState,1);
    if (shwValue >= ((FR_DTX_CN_INT_PERIOD * 2) - 1))
    {
        return;
    }

    shwFrateFactor1          = g_ashwFrFrateFactor[shwValue];
    shwFrateFactor2          = g_ashwFrFrateFactor[shwValue+1];

    /* ������������ʸ���ڲ� */
    for (swCnt = 0; swCnt < shwLen; swCnt++)
    {
        swTemp          = L_mult(shwFrateFactor1, pshwNewCn[swCnt]);
        swTemp          = L_mac(swTemp, shwFrateFactor2, pshwOldCn[swCnt]);
        pshwInterpCn[swCnt] = round(swTemp);
    }
}

/*****************************************************************************
 �� �� ��  : FR_DtxDecSidOtherPrms
 ��������  : FR���в�������������������,������������ϵ����������Ⱥ��ӿ����
             ����ֵ
 �������  : Word16                    *pshwSeed       --��������RPE�����������
 �������  : Word16                     pshwNc[4]      --LTP����
             Word16                     pshwBc[4]      --LTP�ӳٲ���
             Word16                     pshwMc[4]      --RPE-gridλ��
             Word16                     pshwXmc[4*13]  --RPE-pluse����xMc
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxDecSidOtherPrms(
                Word16                  pshwNc[4],
                Word16                  pshwBc[4],
                Word16                  pshwMc[4],
                Word16                  pshwXmc[4*13],
                Word32                 *pswSeed)
{

    Word32                              swCnt1, swCnt2;
    Word16                             *pshwTmpXmc;

    /* ����LTP����bc[4]Ϊ0 */
    for (swCnt1 = 0; swCnt1 < FR_DTX_NUM_OF_BLOCK_COEF; swCnt1++)
    {
        pshwBc[swCnt1] = 0;
    }

    /* ����LTP�ӳٲ���Nc[4]Ϊ�̶����� */
    pshwNc[0] = 40;
    pshwNc[2] = 40;
    pshwNc[1] = 120;
    pshwNc[3] = 120;

    /* ��������RPE-gridλ��Mc[4]��RPE-pluse����xMc[4*13] */

    pshwTmpXmc = pshwXmc;

    for (swCnt1 = 0; swCnt1 < FR_DTX_NUM_OF_BLOCK_COEF; swCnt1++)
    {
        /* RPE grid position Mc[]; ��0-3���������(2bits) */
        pshwMc[swCnt1] = FR_DtxPseudoNoise(pswSeed, 2);

        /* RPE pluse xMc[]; ��0-7���������(3bits) */
        for(swCnt2 = 0; swCnt2 < FR_DTX_NUM_OF_XMC; swCnt2++)
        {
            *pshwTmpXmc = FR_DtxPseudoNoise(pswSeed, 3);

            /* ѹ����Χ�� 0-5 */
            *pshwTmpXmc = mult_r(*pshwTmpXmc, FR_DTX_COMPRESS_SEVEN_TO_FIVE);

            /* ������Χ�� 1-6 */
            *pshwTmpXmc += 1;

            pshwTmpXmc++;
        }
    }
}

/*****************************************************************************
 �� �� ��  : FR_DtxPseudoNoise
 ��������  : �������������������ɸ����������ĸ�֪��������
 �������  : Word32    *pswShiftReg    --ƫ������
             Word16     shwNumBits     --Ҫ�����ɲ����ı�����
 �������  : Word32    *pswShiftReg    --ƫ������
 �� �� ֵ  : Word16    --�������������ɲ���
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��9��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 FR_DtxPseudoNoise(Word32 *pswShiftReg, Word16 shwNumBits)
{
    Word32                              swCnt;
    Word16                              shwState;
    Word16                              shwNoiseBits = 0;

    for (swCnt = 0; swCnt < shwNumBits; swCnt++)
    {
        /* State n == 31  */
        if (((*pswShiftReg) & 0x00000001L) != 0)
        {
            shwState = 1;
        }
        else
        {
            shwState = 0;
        }

        /* State n == 3  */
        if (((*pswShiftReg) & 0x10000000L) != 0)
        {
            shwState = shwState ^ 1;
        }
        else
        {
            shwState = shwState ^ 0;
        }

        shwNoiseBits = shl(shwNoiseBits, 1);
        shwNoiseBits = shwNoiseBits | (extract_l(*pswShiftReg) & 1);

        *pswShiftReg = L_shr(*pswShiftReg, 1);

        if (shwState & 0x0001)
        {
            *pswShiftReg = (*pswShiftReg) | 0x40000000L;
        }
    }

    return shwNoiseBits;
}

/*****************************************************************************
 �� �� ��  : FR_DtxGenSilence
 ��������  : FR���о���֡��������
 �������  : ����֡��������
 �������  : Word16                    pshwLARpp[8]   --���ָ���LAR����
             Word16                    pshwNc[4]      --���ָ���Nc����
             Word16                    pshwBc[4]      --���ָ���bc����
             Word16                    pshwMc[4]      --���ָ���Mc����
             Word16                    pshwXmaxc[4]   --���ָ���xmaxc����
             Word16                    pshwXmc[4*13]  --���ָ���xMc����
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DtxGenSilence(
                 Word16                 pshwLARpp[],
                 Word16                 pshwNc[],
                 Word16                 pshwBc[],
                 Word16                 pshwMc[],
                 Word16                 pshwXmaxc[],
                 Word16                 pshwXmc[])
{
    Word32                              swCnt;
    Word16                             *pshwTmpXmc;
    Word16                             *pshwSidPara;

    pshwSidPara      = (Word16 *)&g_ashwFrFrateSidPara[0];

    /* ���÷���ϵ�����������LARpp[] */
    CODEC_OpVecCpy(pshwLARpp, pshwSidPara, FR_DTX_NUM_OF_LAR_COEF);

    /* ���� LTP�ӳ�;LTP����;RPE-gridλ��;RPE�ӿ�������ֵ */
    for (swCnt = 0; swCnt < FR_DTX_NUM_OF_BLOCK_COEF; swCnt++)
    {
        pshwNc[swCnt]    = 40;
        pshwBc[swCnt]    = 0;
        pshwMc[swCnt]    = 1;
        pshwXmaxc[swCnt] = 0;
    }

    /* ����xMc[4*13] */
    pshwSidPara += FR_DTX_NUM_OF_LAR_COEF;
    pshwTmpXmc = pshwXmc;

    for (swCnt = 0; swCnt < FR_DTX_NUM_OF_BLOCK_COEF; swCnt++)
    {
        CODEC_OpVecCpy(pshwTmpXmc, pshwSidPara, FR_DTX_NUM_OF_XMC);
        pshwTmpXmc += FR_DTX_NUM_OF_XMC;
    }
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

