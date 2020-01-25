/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : VOICE_pcm.c
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2008��5��28��
  ����޸�   :
  ��������   : DMA���ƺ���
  �����б�   :
              VOICE_MicDmaIsr
              VOICE_PcmAdjustRingBufSize
              VOICE_PcmClrLastSpkBuff
              VOICE_PcmGetWritableSpkBuffPtr
              VOICE_PcmInsertCn
              VOICE_PcmManageRingBuf
              VOICE_PcmMicInStartLoopDMA
              VOICE_PcmSpkOutStartLoopDMA
              VOICE_PcmStartDMAFromDataIn
              VOICE_PcmStartDMAToDataOut
              VOICE_PcmSwitchMicBuff
              VOICE_PcmSwitchSpkBuff
              VOICE_PcmUpdate
              VOICE_PcmInit
              VOICE_PcmUpdateRxRingBuf
              VOICE_PcmUpdateTxRingBuf
              VOICE_SioIsr
              VOICE_SpkDmaIsr
  �޸���ʷ   :
  1.��    ��   : 2008��5��28��
    ��    ��   :  л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
   1 ͷ�ļ�����
******************************************************************************/
#include "om_log.h"
#include "ucom_comm.h"
#include "med_drv_sio.h"
#include "med_drv_dma.h"
#include "med_drv_ipc.h"
#include "med_drv_mb_hifi.h"

#include "codec_typedefine.h"
#include "voice_pcm.h"
#include "voice_mc.h"
#include "voice_proc.h"
#include "voice_log.h"
#include "ucom_share.h"
#include "ucom_nv.h"
#include "ucom_pcm.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif


/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_PCM_C

/*****************************************************************************
   2 ȫ�ֱ�������
******************************************************************************/
VOS_INT16                              *g_psVoicePcmMicIn;                      /*ָ��ǰMic�����PCM����Buffer��ָ��*/
VOS_INT16                              *g_psVoicePcmSpkOut;                     /*ָ��ǰ�����Speaker��PCM����Buffer��ָ��*/
VOS_INT16                              *g_psVoiceSpaDes;                        /*ָ��ǰ����EC REF��Bufferָ��*/
VOS_INT16                               g_shwCodecPcmFrameLength = CODEC_FRAME_LENGTH_NB; /* PCM����֡���� */

/* DMAͨ�����ò����ṹ��ȫ�ֱ��� */
UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astVoicePcmMicInDmaPara[2] = {0};

UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astVoicePcmSpkOutDmaPara[2] = {0};

UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astVoiceEcRefInDmaPara[2] = {0};

/* PCM�����ṹ��ȫ�ֱ�������ȫ�ֱ�����Ҫ������Hifi��TCM�� */
UCOM_SEC_TCMBSS
VOICE_PCM_BUFFER_STRU                   g_stVoicePcmBuff = {0};

VOICE_PCM_PC_VOICE_OBJ_STRU             g_stVoicePcVoiceObj= {0};               /*PC Voiceʵ��ȫ�ֱ��� */

VOS_UINT32                              g_uwVcLoopCnt = 0;                      /* VC LOOP���ܵ�ѭ������(����+����) */
/*****************************************************************************
   3 �ⲿ��������
******************************************************************************/


/*****************************************************************************
   4 ����ʵ��
******************************************************************************/

/*****************************************************************************
 �� �� ��  : VOICE_PcmInit
 ��������  : ��ʼ��PCM�ڴ�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��30��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_VOID VOICE_PcmInit(VOS_VOID)
{
    VOS_VOID                    *pshwPcmMicInTmp  = VOS_NULL;
    VOS_VOID                    *pshwPcmSpkOutTmp = VOS_NULL;
    VOICE_PCM_BUFFER_STRU       *pstPcmBuffTmp  = VOS_NULL;

    /* ��¼RingBuff����ַ,��ʱ��v3r3ʵ�ֲ�һ�� */
    /* CARM_HIFI_DYN_ADDR_SHARE_STRU      *pstShare = UCOM_GetCarmShareAddrStru(); */

    /* ��ȡUncache��ַ */
    pshwPcmMicInTmp     = (VOS_VOID *)UCOM_GetUncachedAddr((VOS_UINT32)(&g_astVoicePcmMicInDmaPara[0]));
    pshwPcmSpkOutTmp    = (VOS_VOID *)UCOM_GetUncachedAddr((VOS_UINT32)(&g_astVoicePcmSpkOutDmaPara[0]));
    pstPcmBuffTmp       = (&g_stVoicePcmBuff);

    /* ��ʼ��ȫ�ֱ���ȫ0���豣֤Uncache */
    UCOM_MemSet(pshwPcmMicInTmp,  0, sizeof(g_astVoicePcmMicInDmaPara));
    UCOM_MemSet(pshwPcmSpkOutTmp, 0, sizeof(g_astVoicePcmSpkOutDmaPara));
    UCOM_MemSet(pstPcmBuffTmp,  0, sizeof(g_stVoicePcmBuff));

    g_psVoicePcmMicIn      = pstPcmBuffTmp->asMicInBuffA;
    g_psVoicePcmSpkOut     = pstPcmBuffTmp->asSpkOutBuffA;

    /* Smart PA��ʼ�� */
    UCOM_PCM_SmartPaPcmInit();

    /*��ʼ��PC Voiceʵ��ȫ�ֱ���*/
    UCOM_MemSet(&g_stVoicePcVoiceObj, 0, sizeof(VOICE_PCM_PC_VOICE_OBJ_STRU));

    /* ��¼RingBuff����ַ,��ʱ��v3r3ʵ�ֲ�һ�� */
    /* VOICE_PcmSaveNvMemAddr(pstShare->uwCarmHifiMBAddr); */


    g_stVoicePcVoiceObj.stTxRingBufferState.uhwAdpBufferSize
        = VOICE_PCM_TX_BUF_SIZE_INIT;

    g_stVoicePcVoiceObj.stRxRingBufferState.uhwAdpBufferSize
        = VOICE_PCM_RX_BUF_SIZE_INIT;

}

/*****************************************************************************
 �� �� ��  : VOICE_PcmUpdate
 ��������  : ���ݲ��������������������
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��30��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_UINT32 VOICE_PcmUpdate(
                VOS_UINT32                      swSampleRate,
                VCVOICE_DEV_MODE_ENUM_UINT16    enDevMode)
{
    DRV_SIO_CFG_STRU               *pstSioStatus  = DRV_SIO_GetVoiceCfgPtr();
    DRV_SIO_SAMPLING_ENUM_UINT32    enSioSampleRate;
    DRV_SIO_INT_MASK_ENUM_UINT32    enSioMask;
    VOS_UINT16                      uhwModeMaster;
    VOS_UINT32                      uwVoiceChanNum= DRV_SIO_GetVoiceMicChn();

    /* ���ò�����ģʽ��һ֡PCM�������� */
    switch(swSampleRate)
    {
        case CODEC_SAMPLE_RATE_MODE_8000:
        {
            CODEC_PcmSetPcmFrameLength(CODEC_FRAME_LENGTH_NB);
            enSioSampleRate = DRV_SIO_SAMPLING_8K;
        }
        break;
        case CODEC_SAMPLE_RATE_MODE_16000:
        {
            CODEC_PcmSetPcmFrameLength(CODEC_FRAME_LENGTH_WB);
            enSioSampleRate = DRV_SIO_SAMPLING_16K;
        }
        break;
        default:
        {
            return UCOM_RET_ERR_PARA;
        }
    }

    /* SIOģʽ */
    if (VCVOICE_DEV_MODE_PCVOICE != enDevMode)
    {
        /* ����SIO���ڴ�״̬,���ȹر� */
        if(DRV_SIO_CLK_STATUS_OPEN == pstSioStatus->enClkStatus)
        {
            DRV_SIO_Voice_Close();
        }

        /* ��ȡSIO����ģʽ���� */
        UCOM_NV_Read(en_NV_Item_SIO_Voice_Master,
                    &uhwModeMaster,
                    sizeof(uhwModeMaster));

        /* ��ȡVoice SIO PCM/I2Sģʽ���� */
        /*
        UCOM_NV_Read(en_NV_Item_SIO_Voice_Pcm_I2S,
                    &uwWorkMode,
                    sizeof(uwWorkMode));
        */
        if (CODEC_SWITCH_ON == uhwModeMaster)
        {
            pstSioStatus->enWorkMode = DRV_SIO_MODE_PCM;
        }
        else
        {
            pstSioStatus->enWorkMode = DRV_SIO_Voice_GetWorkMode(uwVoiceChanNum);
        }

        /* ����Mask, �ֲ�������ֹPC-Lint�澯 */
        enSioMask  = DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER;
        enSioMask |= DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER;

        /* ����DRV_SIO_Open�ӿڣ���SIO�豸 */
        DRV_SIO_Voice_Open(enSioMask,
                           enSioSampleRate,
                           uhwModeMaster,
                           VOICE_SioIsr,
                           0);
    }

    return UCOM_RET_SUCC;
}


VOS_VOID VOICE_PcmSwitchMicBuff(VOS_VOID)
{
    VOS_UINT32              ulDestAddr, ulDestAddrB;   /* MIC�ɼ���DMAͨ����Ŀ�ĵ�ַ */
    VOS_UINT32              uwChannelNum;
    VOICE_PCM_BUFFER_STRU  *pstPcmBuffTmp = (&g_stVoicePcmBuff);

    /*��ȡI2S/PCMģʽ������ͨ������*/
    uwChannelNum    = DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr());

    ulDestAddrB     = (VOS_UINT32)&pstPcmBuffTmp->asMicInBuffB[0];

    /*��ȡͨ��0Ŀ�ĵ�ַ�Ĵ���*/
    ulDestAddr      = UCOM_RegRd(DRV_DMA_GetCxDesAddr(VOICE_MC_DMAC_CHN_MIC));

    /*��Ŀ�ĵ�ַ�Ѿ�ΪBufferB��Χ��(LOOPģʽ)��Ŀ�ĵ�ַΪBufferA�����һ��λ��(��LOOPģʽ)*/
    /*���߼�������BufferA��BufferB��ַ��������BufferA��ǰ*/
    if ( (ulDestAddr >= (ulDestAddrB - sizeof(VOS_UINT32)))
        &&(ulDestAddr < (ulDestAddrB + (((VOS_UINT32)(VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * uwChannelNum)
                            - sizeof(VOS_UINT32)))))
    {
        g_psVoicePcmMicIn = pstPcmBuffTmp->asMicInBuffA;
    }
    else
    {
        g_psVoicePcmMicIn = pstPcmBuffTmp->asMicInBuffB;
    }
}
VOS_INT16* VOICE_PcmGetWritableSpkBuffPtr(VOS_VOID)
{
    VOS_UINT32              ulSrcAddr, ulSrcAddrB;   /* SPK���ŵ�DMAͨ����Դ��ַ */
    VOS_UINT32              uwChannelNum;
    VOICE_PCM_BUFFER_STRU  *pstPcmBuffTmp = (&g_stVoicePcmBuff);

    ulSrcAddrB      = (VOS_UINT32)&pstPcmBuffTmp->asSpkOutBuffB[0];

    /*��ȡI2S/PCMģʽ������ͨ������*/
    uwChannelNum    = DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr());

    /*��ȡͨ��1Դ��ַ�Ĵ���*/
    ulSrcAddr       = UCOM_RegRd(DRV_DMA_GetCxSrcAddr(VOICE_MC_DMAC_CHN_SPK));

    /*��Դ��ַ�Ѿ�ΪBufferB��Χ��(LOOPģʽ)��Դ��ַΪBufferA�����һ��λ��(��LOOPģʽ)*/
    /*���߼�������BufferA��BufferB��ַ��������BufferA��ǰ*/
    if((ulSrcAddr >= (ulSrcAddrB - sizeof(VOS_UINT32)))
        &&(ulSrcAddr < (ulSrcAddrB + (((VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES * uwChannelNum)
                                - sizeof(VOS_UINT32)))))
    {
        return pstPcmBuffTmp->asSpkOutBuffA;
    }
    else
    {
        return pstPcmBuffTmp->asSpkOutBuffB;
    }

}
VOS_VOID VOICE_PcmSwitchSpkBuff(VOS_VOID)
{
    g_psVoicePcmSpkOut = VOICE_PcmGetWritableSpkBuffPtr();
}


VOS_VOID VOICE_PcmClrLastSpkBuff(VOS_VOID)
{
    UCOM_MemSet(VOICE_PcmGetWritableSpkBuffPtr(),
                0,
                (VOS_UINT16)CODEC_PCM_FRAME_LENGTH * sizeof(VOS_UINT16));
}

/*****************************************************************************
 �� �� ��  : VOICE_PcmMicInStartLoopDMA
 ��������  : ���ò�������ʽѭ��DMA����ʼMic�ɼ�PCM���ݰ��˵����ݻ�����
 �������  : ChNum  -  MIC�ɼ���DMAͨ����
 �������  : ��
 �� �� ֵ  : VOICE_RET_OK         -   �ɹ�
             VOICE_RET_ERR_PARAM  -   ��������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��30��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_UINT32 VOICE_PcmMicInStartLoopDMA(VOS_UINT16 usChNum, DRV_DMA_INT_FUNC pfuncIsr)
{
    VOS_UINT32              uwRet;
    VOS_UINT32              uwChannelNum;
    VOS_UINT32              uwDmaCfg;
    VOS_UINT32              uwDestAddrA, uwDestAddrB;   /* MIC�ɼ���DMAͨ����Ŀ�ĵ�ַ */
    VOS_UINT32              uwLliAddrA, uwLliAddrB;     /* MIC�ɼ���DMAͨ����LLI��ַ */
    DRV_DMA_CXCFG_STRU     *pstDmaCfgA     = VOS_NULL;
    DRV_DMA_CXCFG_STRU     *pstDmaCfgB     = VOS_NULL;
    VOICE_PCM_BUFFER_STRU  *pvPcmBuffTmp   = VOS_NULL;
    DRV_SIO_CFG_STRU       *pStSioCfg      = VOS_NULL;

    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(VOICE_PcmMicInStartLoopDMA_DmaChanError);
        return UCOM_RET_ERR_PARA;
    }

    pStSioCfg   = DRV_SIO_GetVoiceCfgPtr();
    uwChannelNum= DRV_SIO_GetSioChannelNum(pStSioCfg);

    /* ���ݵ�ǰSIO����ģʽ������DMA���� */
    if (DRV_SIO_MODE_PCM== pStSioCfg->enWorkMode)
    {
        uwDmaCfg = DRV_DMA_GetVoicePcmSioMemCfg();
    }
    else
    {
        uwDmaCfg = DRV_DMA_GetVoiceI2sSioMemCfg();
    }

    /* ��ȡDMA����ȫ�ֱ�����Uncache���ʵ�ַ */
    pstDmaCfgA   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astVoicePcmMicInDmaPara[0]);
    pstDmaCfgB   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astVoicePcmMicInDmaPara[1]);
    pvPcmBuffTmp = (&g_stVoicePcmBuff);

    /* ��ֹMIC�ɼ���DMA */
    DRV_DMA_Stop(usChNum);

    /*�л�buffer������DestAddr��ֵ*/
    if ( g_psVoicePcmMicIn == pvPcmBuffTmp->asMicInBuffA)
    {
        uwDestAddrA = (VOS_UINT32)&pvPcmBuffTmp->asMicInBuffB[0];
        uwDestAddrB = (VOS_UINT32)&pvPcmBuffTmp->asMicInBuffA[0];
    }
    else
    {
        uwDestAddrA = (VOS_UINT32)&pvPcmBuffTmp->asMicInBuffA[0];
        uwDestAddrB = (VOS_UINT32)&pvPcmBuffTmp->asMicInBuffB[0];
    }

    /*LLI����ĵ�ַ*/
    uwLliAddrA = UCOM_GetUnmappedAddr((VOS_UINT32)&g_astVoicePcmMicInDmaPara[0]);
    uwLliAddrB = UCOM_GetUnmappedAddr((VOS_UINT32)&g_astVoicePcmMicInDmaPara[1]);

    UCOM_MemSet(pstDmaCfgA, 0, sizeof(g_astVoicePcmMicInDmaPara));

    /*����DMA���ò���,����Ϊ�������ӣ�ʹ��MICͨ��LOOP���ã�ÿ���ڵ��ϱ�һ���ж�*/
    pstDmaCfgA->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrB);
    pstDmaCfgA->uhwACount        = (VOS_UINT16)((VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * (VOS_INT32)uwChannelNum);
    pstDmaCfgA->uwSrcAddr        = DRV_SIO_GetRxRegAddr(pStSioCfg);
    pstDmaCfgA->uwDstAddr        = uwDestAddrA;
    pstDmaCfgA->uwConfig         = uwDmaCfg;

    pstDmaCfgB->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrA);
    pstDmaCfgB->uhwACount        = (VOS_UINT16)((VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * (VOS_INT32)uwChannelNum);
    pstDmaCfgB->uwSrcAddr        = DRV_SIO_GetRxRegAddr(pStSioCfg);
    pstDmaCfgB->uwDstAddr        = uwDestAddrB;
    pstDmaCfgB->uwConfig         = uwDmaCfg;

    /*����MIC�ɼ�DMAͨ���������ݰ��ˣ���ע��MIC��DMA�жϴ�����*/
    uwRet = DRV_DMA_StartWithCfg(usChNum,
                                 pstDmaCfgA,
                                 pfuncIsr,//VOICE_MicDmaIsr,
                                 0);

    return uwRet;
}

/*****************************************************************************
 �� �� ��  : VOICE_PcmSpkOutStartLoopDMA
 ��������  : Gģʽ�£����ò�����DMA����������PCM���ݰ��˵������豸����
 �������  : ChNum  -  SPEAKER���ŵ�DMAͨ����
 �������  : ��
 �� �� ֵ  : VOICE_RET_OK         -   �ɹ�
             VOICE_RET_ERR_PARAM  -   ��������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��30��
    ��    ��   : л���� 58441
    �޸�����   : �޸ĺ���ͷע��ģ��

*****************************************************************************/
VOS_UINT32 VOICE_PcmSpkOutStartLoopDMA(VOS_UINT16 usChNum, DRV_DMA_INT_FUNC pfuncIsr)
{
    VOS_UINT32              uwRet;
    VOS_UINT32              uwDmaCfg;
    VOS_UINT32              uwChannelNum;
    VOS_UINT32              uwSrcAddrA, uwSrcAddrB;   /* SPK���ŵ�DMAͨ����Դ��ַ */
    DRV_DMA_CXCFG_STRU     *pstDmaCfgA     = VOS_NULL;
    DRV_DMA_CXCFG_STRU     *pstDmaCfgB     = VOS_NULL;
    VOICE_PCM_BUFFER_STRU  *pvPcmBuffTmp   = VOS_NULL;
    DRV_SIO_CFG_STRU       *pStSioCfg      = VOS_NULL;
    VOS_UINT32              uwLliAddrA;
    VOS_UINT32              uwLliAddrB;

    /* �������Ƿ�Ƿ�,ͨ����0-15 */
    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(VOICE_PcmSpkOutStartLoopDMA_DmaChanError);
        return UCOM_RET_ERR_PARA;
    }

    pStSioCfg   = DRV_SIO_GetVoiceCfgPtr();
    uwChannelNum= DRV_SIO_GetSioChannelNum(pStSioCfg);

    /* ���ݵ�ǰSIO����ģʽ������DMA���� */
    if (DRV_SIO_MODE_PCM== pStSioCfg->enWorkMode)
    {
        uwDmaCfg = DRV_DMA_GetVoicePcmMemSioCfg();
    }
    else
    {
        uwDmaCfg = DRV_DMA_GetVoiceI2sMemSioCfg();
    }


    /* ��ȡDMA����ȫ�ֱ�����Uncache���ʵ�ַ */
    pstDmaCfgA   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astVoicePcmSpkOutDmaPara[0]);
    pstDmaCfgB   = (DRV_DMA_CXCFG_STRU *)UCOM_GetUncachedAddr((VOS_UINT32)&g_astVoicePcmSpkOutDmaPara[1]);
    pvPcmBuffTmp = (&g_stVoicePcmBuff);

    /* ��ֹSPK���ŵ�DMA */
    DRV_DMA_Stop(usChNum);

    /*�л�buffer������DestAddr��ֵ*/
    if (g_psVoicePcmSpkOut == pvPcmBuffTmp->asSpkOutBuffA)
    {
        uwSrcAddrA = (VOS_UINT32)&pvPcmBuffTmp->asSpkOutBuffB[0];
        uwSrcAddrB = (VOS_UINT32)&pvPcmBuffTmp->asSpkOutBuffA[0];
    }
    else
    {
        uwSrcAddrA = (VOS_UINT32)&pvPcmBuffTmp->asSpkOutBuffA[0];
        uwSrcAddrB = (VOS_UINT32)&pvPcmBuffTmp->asSpkOutBuffB[0];
    }

    /*LLI����ĵ�ַ*/
    uwLliAddrA = UCOM_GetUnmappedAddr((VOS_UINT32)&g_astVoicePcmSpkOutDmaPara[0]);
    uwLliAddrB = UCOM_GetUnmappedAddr((VOS_UINT32)&g_astVoicePcmSpkOutDmaPara[1]);

    UCOM_MemSet(pstDmaCfgA, 0, sizeof(g_astVoicePcmSpkOutDmaPara));

    /*����DMA���ò���,����Ϊ�������ӣ�ʹ��SPKͨ��LOOP���ã�ÿ���ڵ��ϱ�һ���ж�*/
    pstDmaCfgA->uwLli     = DRV_DMA_LLI_LINK(uwLliAddrB);
    pstDmaCfgA->uhwACount = (VOS_UINT16)((VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * (VOS_INT32)uwChannelNum);
    pstDmaCfgA->uwSrcAddr = uwSrcAddrA;
    pstDmaCfgA->uwDstAddr = DRV_SIO_GetTxRegAddr(pStSioCfg);
    pstDmaCfgA->uwConfig  = uwDmaCfg;

    pstDmaCfgB->uwLli     = DRV_DMA_LLI_LINK(uwLliAddrA);
    pstDmaCfgB->uhwACount = (VOS_UINT16)((VOS_INT32)CODEC_PCM_FRAME_LENGTH_BYTES * (VOS_INT32)uwChannelNum);
    pstDmaCfgB->uwSrcAddr = uwSrcAddrB;
    pstDmaCfgB->uwDstAddr = DRV_SIO_GetTxRegAddr(pStSioCfg);
    pstDmaCfgB->uwConfig  = uwDmaCfg;

    /*����SPEAKER����DMAͨ�����в��ŵ����е����ݰ���, ��ע��MIC/SPK��DMA�жϴ�����*/
    uwRet = DRV_DMA_StartWithCfg(usChNum,
                                 pstDmaCfgA,
                                 pfuncIsr,
                                 0);

    return uwRet;
}
VOS_UINT32 VOICE_PcmStartDMAToDataOut(VOS_UINT16 usChNum)
{
    OMMED_RING_BUFFER_CONTROL_STRU     *pstRxRingBufferControl;
    VOS_UINT32                          uwRet;
    VOS_UINT32                          uwSrcAddr;                              /* DMAͨ����Դ��ַ��Ŀ�ĵ�ַ */
    VOS_UINT32                          uwDestAddr;
    VOS_INT16                           shwFreeSize;

    uwRet                               = UCOM_RET_SUCC;
    shwFreeSize                         = 0;

    /* �������Ƿ�Ƿ�,ͨ����0-15 */
    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(VOICE_PcmStartDMAToDataOut_DmaChanError);
        return UCOM_RET_ERR_PARA;
    }

    /*��AXI-MEM�ж�ȡ���п�����Ϣ*/
    pstRxRingBufferControl  = (OMMED_RING_BUFFER_CONTROL_STRU *)(CODEC_PC_VOICE_CODEC_OM_ADDR);

    /*���������л���buffer�����屣���ֲ�ƥ�䣬˵�������屻�ȣ���״̬�޷��ָ�������λ����*/
    if ((  DRV_MAILBOX_PROTECT1 != pstRxRingBufferControl->uwProtectWord1)
        ||(DRV_MAILBOX_PROTECT1 != pstRxRingBufferControl->uwProtectWord3)
        ||(DRV_MAILBOX_PROTECT2 != pstRxRingBufferControl->uwProtectWord2)
        ||(DRV_MAILBOX_PROTECT2 != pstRxRingBufferControl->uwProtectWord4))
    {
        /* ��������buffer�����屣�����쳣 */
        OM_LogWarning(VOICE_PcmStartDMAFromDataIn_InvalidMagicNum);

        return UCOM_RET_FAIL;
    }

    if ((0 == pstRxRingBufferControl->uwBufAddr)
    || (0 == pstRxRingBufferControl->uwBufSize))
    {
        OM_LogError(VOICE_PcmStartDMAToDataOut_RingBufUnInit);
        return UCOM_RET_ERR_PARA;
    }

    if (pstRxRingBufferControl->uwReadAddr
    >= pstRxRingBufferControl->uwBufSize)
    {
        OM_LogError(VOICE_PcmStartDMAToDataOut_ReadAddrErr);
        return UCOM_RET_ERR_PARA;
    }

    /* ���㻺�������г���*/
    if (pstRxRingBufferControl->uwReadAddr > pstRxRingBufferControl->uwWriteAddr)
    {
        shwFreeSize  = (VOS_INT16)(pstRxRingBufferControl->uwReadAddr
                                   - pstRxRingBufferControl->uwWriteAddr);
    }
    else if (pstRxRingBufferControl->uwReadAddr < pstRxRingBufferControl->uwWriteAddr)
    {
        shwFreeSize
            = (VOS_INT16)(
              (pstRxRingBufferControl->uwReadAddr
              + pstRxRingBufferControl->uwBufSize)
              - pstRxRingBufferControl->uwWriteAddr);
    }
    else
    {
        shwFreeSize = 0;
    }

    /*ring buffer�Ƿ�����*/
    if ((shwFreeSize <= (VOS_INT16)CODEC_PCM_FRAME_LENGTH_BYTES) && (shwFreeSize != 0))
    {
        OM_LogWarning(VOICE_PcmStartDMAToDataOut_RingBufFull);

        /* ErrorLog ��¼�¼� */
        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_PCVOICE_RINGBUF_FULL);

        /* ��ȻҪ��OM�������� */
        VOICE_PcmTransferRxDataInd();

        return UCOM_RET_SUCC;
    }

    /* ��ֹSPK���ŵ�DMA */
    DRV_DMA_Stop(usChNum);

    /*����uwDestAddr��ֵ */
    uwDestAddr = pstRxRingBufferControl->uwBufAddr
                 + pstRxRingBufferControl->uwWriteAddr;

    /*����uwSrcAddr��ֵ*/
    uwSrcAddr  = (VOS_UINT32)&g_psVoicePcmSpkOut[0];

    /*����SPEAKER����DMAͨ�����в��ŵ����е����ݰ���*/
    uwRet = DRV_DMA_MemCpy(usChNum,
                           (VOS_UCHAR *)uwDestAddr,
                           (VOS_UCHAR *)uwSrcAddr,
                           (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES,
                           VOICE_SpkDmaIsr,
                           0);

    return uwRet;
}
VOS_UINT32 VOICE_PcmStartDMAFromDataIn(VOS_UINT16 usChNum)
{
    VOS_UINT32                          uwRet;
    VOS_INT16                           shwBufProcess;
    VOS_UINT32                          uwSrcAddr;                              /* DMAͨ����Դ��ַ��Ŀ�ĵ�ַ */
    VOS_UINT32                          uwDestAddr;
    OMMED_RING_BUFFER_CONTROL_STRU     *pstTxRingBufferControl;
    VOICE_PCM_RING_BUFFER_STATE_STRU   *pstTxRingBufferState;

    pstTxRingBufferState                = VOICE_PcmGetRxBufStatePtr();
    uwRet                               = UCOM_RET_SUCC;

    /* �������Ƿ�Ƿ�,ͨ����0-15 */
    if (usChNum >= DRV_DMA_MAX_CHANNEL_NUM)
    {
        OM_LogError(VOICE_PcmStartDMAFromDataIn_DmaChanError);
        return UCOM_RET_ERR_PARA;
    }

    /*��AXI-MEM�ж�ȡ���п�����Ϣ*/
    pstTxRingBufferControl  = (OMMED_RING_BUFFER_CONTROL_STRU *)(CODEC_PC_VOICE_OM_CODEC_ADDR);

    /*���������л���buffer�����屣���ֲ�ƥ�䣬˵�������屻�ȣ���״̬�޷��ָ�������λ����*/
    if ((  DRV_MAILBOX_PROTECT1 != pstTxRingBufferControl->uwProtectWord1)
        ||(DRV_MAILBOX_PROTECT1 != pstTxRingBufferControl->uwProtectWord3)
        ||(DRV_MAILBOX_PROTECT2 != pstTxRingBufferControl->uwProtectWord2)
        ||(DRV_MAILBOX_PROTECT2 != pstTxRingBufferControl->uwProtectWord4))
    {
        /* ����buffer�������쳣�������������� */
        VOICE_PcmMicInNoData();

        OM_LogWarning(VOICE_PcmStartDMAFromDataIn_InvalidMagicNum);

        return UCOM_RET_FAIL;
    }

    if ((0 == pstTxRingBufferControl->uwBufAddr)
    || (0 == pstTxRingBufferControl->uwBufSize))
    {
        OM_LogError(VOICE_PcmStartDMAFromDataIn_RingBufUnInit);
        return UCOM_RET_ERR_PARA;
    }

    if (pstTxRingBufferControl->uwWriteAddr
    >= pstTxRingBufferControl->uwBufSize)
    {
        OM_LogError(VOICE_PcmStartDMAFromDataIn_WriteAddrErr);
        return UCOM_RET_ERR_PARA;
    }

    /*����Ӧ�������ݻ��峤��*/
    VOICE_PcmAdjustRingBufSize(
                    pstTxRingBufferControl,
                    pstTxRingBufferState);

    /*���л���������*/
    VOICE_PcmManageRingBuf(
                    pstTxRingBufferState->uhwAdpBufferSize,
                    pstTxRingBufferState->uhwCurrDataSize,
                    &shwBufProcess);

    if (0 != pstTxRingBufferState->uhwCurrDataSize)
    {
        /*��֡��Ŀ���� */
        pstTxRingBufferState->shwInsertFrameCnt
            = pstTxRingBufferState->shwInsertFrameCnt - shwBufProcess;

        if (pstTxRingBufferState->shwInsertFrameCnt < 0)
        {
            pstTxRingBufferState->shwInsertFrameCnt = 0;
        }

        if(0 < shwBufProcess)
        {
            OM_LogWarning(VOICE_PcmStartDMAFromDataIn_DelFrame);
        }

        /*��ȡ��ַ���� */
        pstTxRingBufferControl->uwReadAddr
              += (VOS_UINT32)(VOS_INT32)shwBufProcess
                           * (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;

        if (pstTxRingBufferControl->uwReadAddr >= pstTxRingBufferControl->uwBufSize)
        {
            pstTxRingBufferControl->uwReadAddr
                = pstTxRingBufferControl->uwReadAddr
                  - pstTxRingBufferControl->uwBufSize;
        }

        /* ��ֹMIC���ŵ�DMA */
        DRV_DMA_Stop(usChNum);

        /* ����SrcAddr��ֵ */
        uwSrcAddr  = pstTxRingBufferControl->uwBufAddr
                     + pstTxRingBufferControl->uwReadAddr;

        /*�л�buffer������DestAddr��ֵ*/
        uwDestAddr = (VOS_UINT32)&g_psVoicePcmMicIn[0];

        /*����DMA�����������ݰ���*/
        uwRet = DRV_DMA_MemCpy(usChNum,
                               (VOS_UCHAR *)uwDestAddr,
                               (VOS_UCHAR *)uwSrcAddr,
                               (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES,
                               VOICE_MicDmaIsr,
                               0);

    }
    else
    {
        /* ������������ */
        VOICE_PcmMicInNoData();
    }

    return uwRet;
}



VOS_VOID VOICE_PcmAdjustRingBufSize(
                OMMED_RING_BUFFER_CONTROL_STRU      *pstTxRingBufferControl,
                VOICE_PCM_RING_BUFFER_STATE_STRU    *pstTxRingBufferState)
{
    VOS_UINT32                          uwTxReadAddr;
    VOS_UINT32                          uwTxWriteAddr;
    VOS_UINT16                          uhwTxCurrDataSize;
    VOS_UINT32                          uwTxBufSize;
    VOS_UINT16                          uhwTxAdpBufSize;
    VOS_INT16                           shwTxInsertFrameCnt;
    VOS_UINT16                          uhwTxCntDataSizeIsOne;
    VOS_UINT16                          uhwTxFrameCnt;

    uwTxReadAddr          = pstTxRingBufferControl->uwReadAddr;
    uwTxWriteAddr         = pstTxRingBufferControl->uwWriteAddr;
    uwTxBufSize           = pstTxRingBufferControl->uwBufSize;
    uhwTxAdpBufSize       = pstTxRingBufferState->uhwAdpBufferSize;
    shwTxInsertFrameCnt   = pstTxRingBufferState->shwInsertFrameCnt;
    uhwTxCntDataSizeIsOne = pstTxRingBufferState->uhwCntDataSizeIsOne;
    uhwTxFrameCnt         = pstTxRingBufferState->uhwFrameCnt;


    /* ���㻺�������ݳ���*/
    if (uwTxReadAddr <= uwTxWriteAddr)
    {
        uhwTxCurrDataSize
            = (VOS_UINT16)(uwTxWriteAddr - uwTxReadAddr) / (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;
    }
    else
    {
        uhwTxCurrDataSize
            = (VOS_UINT16)(
              (uwTxWriteAddr + uwTxBufSize) - uwTxReadAddr) / (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;
    }

    if (0 == uhwTxCurrDataSize)
    {
        /* �������ѿգ���֡��������1*/
        uhwTxCurrDataSize = 0;
        shwTxInsertFrameCnt++;

        if ((shwTxInsertFrameCnt > VOICE_PCM_FRAME_INSERT_THD)
        && (uhwTxAdpBufSize < ((uwTxBufSize / (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES) - 1 )))
        {
            pstTxRingBufferState->shwInsertFrameCnt   = 0;
            pstTxRingBufferState->uhwFrameCnt         = 0;
            pstTxRingBufferState->uhwCntDataSizeIsOne = 0;
            pstTxRingBufferState->uhwCurrDataSize     = 0;
            pstTxRingBufferState->uhwAdpBufferSize++;
            OM_LogWarning(VOICE_PcmAdjustRingBufSize_BufSizeInc);

            return;
        }
    }

    /* ���������ݳ���С�ڵ���1����Ӧ��������1*/
    if (uhwTxCurrDataSize <= 1 )
    {
        uhwTxCntDataSizeIsOne++;
    }

    /*�ж��Ƿ�ﵽͳ�ƴ���*/
    if (uhwTxFrameCnt < VOICE_PCM_FRAME_WIN_LEN )
    {
        uhwTxFrameCnt++;
    }
    else
    {
        /*�ﵽͳ�ƴ������жϻ������Ƿ����*/
        if ((0 == shwTxInsertFrameCnt)
        &&(uhwTxCntDataSizeIsOne <= 1))
        {
            /*����Ӧ��������1������1*/
            pstTxRingBufferState->uhwAdpBufferSize--;
            OM_LogWarning(VOICE_PcmAdjustRingBufSize_BufSizeDec);

        }

        pstTxRingBufferState->shwInsertFrameCnt   = 0;
        pstTxRingBufferState->uhwFrameCnt         = 0;
        pstTxRingBufferState->uhwCntDataSizeIsOne = 0;
        pstTxRingBufferState->uhwCurrDataSize     = uhwTxCurrDataSize;
        return;
    }
    pstTxRingBufferState->uhwAdpBufferSize    = uhwTxAdpBufSize;
    pstTxRingBufferState->shwInsertFrameCnt   = shwTxInsertFrameCnt;
    pstTxRingBufferState->uhwCntDataSizeIsOne = uhwTxCntDataSizeIsOne;
    pstTxRingBufferState->uhwFrameCnt         = uhwTxFrameCnt;
    pstTxRingBufferState->uhwCurrDataSize     = uhwTxCurrDataSize;

    return;
}


VOS_VOID VOICE_PcmManageRingBuf(
                VOS_UINT16  uhwAdpBufferSize,
                VOS_UINT16  uhwCurrDataSize,
                VOS_INT16  *pshwBufProcess)
{
    /*������ȡ*/
    *pshwBufProcess = 0;

    /* ���㻺�������ݳ���*/
    if (0 == uhwCurrDataSize)
    {
        *pshwBufProcess = -1;
    }

    /*����״̬*/
    if (uhwCurrDataSize > uhwAdpBufferSize)
    {
        /*���㶪֡����*/
        *pshwBufProcess = (VOS_INT16)uhwCurrDataSize - (VOS_INT16)uhwAdpBufferSize;
    }

    return;
}


VOS_VOID VOICE_PcmUpdateTxRingBuf(VOS_VOID)
{
    VOS_UINT16                          uhwDevMode     = VOICE_McGetDeviceMode();
    OMMED_RING_BUFFER_CONTROL_STRU     *pstTxRingBufferControl;

    /*��AHB�����ȡ���п�����Ϣ*/
    pstTxRingBufferControl  = (OMMED_RING_BUFFER_CONTROL_STRU *)(CODEC_PC_VOICE_OM_CODEC_ADDR);

    /* �������ģʽ��PC VOICE */
    if (VCVOICE_DEV_MODE_PCVOICE == uhwDevMode )
    {
        pstTxRingBufferControl->uwReadAddr
            = pstTxRingBufferControl->uwReadAddr + (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;
        /* �����ַ����buffer��Χ����ͷ��ʼ*/
        if (pstTxRingBufferControl->uwReadAddr >= pstTxRingBufferControl->uwBufSize)
        {
            pstTxRingBufferControl->uwReadAddr = 0;
        }
    }
}
VOS_VOID VOICE_PcmUpdateRxRingBuf(VOS_VOID)
{
    VOS_UINT16                          uhwDevMode   = VOICE_McGetDeviceMode();
    OMMED_RING_BUFFER_CONTROL_STRU     *pstRxRingBufferControl;

    /*��AHB�����ȡ���п�����Ϣ*/
    pstRxRingBufferControl  = (OMMED_RING_BUFFER_CONTROL_STRU *)(CODEC_PC_VOICE_CODEC_OM_ADDR);

    /* �������ģʽ��PC VOICE */
    if (VCVOICE_DEV_MODE_PCVOICE == uhwDevMode)
    {
        pstRxRingBufferControl->uwWriteAddr
            = pstRxRingBufferControl->uwWriteAddr + (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES;

        /* �����ַ����buffer��Χ����ͷ��ʼ*/
        if (pstRxRingBufferControl->uwWriteAddr >= pstRxRingBufferControl->uwBufSize)
        {
            pstRxRingBufferControl->uwWriteAddr = 0;
        }

        /* ֪ͨOM�����������ݰ��� */
        VOICE_PcmTransferRxDataInd();
    }
}


VOS_VOID VOICE_MicDmaIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                VOS_UINT32 uwPara)
{
    VOICE_VOICE_TX_PROC_REQ_STRU    stTxProcReqMsg;
    VOICE_MC_DMA_FLAG_STRU         *pstDmaFlag  = VOICE_McGetDmaFlagPtr();

    /* ��ʼ���ظ���Ϣ�ṹ�� */
    UCOM_MemSet(&stTxProcReqMsg, 0, sizeof(VOICE_VOICE_TX_PROC_REQ_STRU));

    /* ���ж�����ΪTC�ж�,ΪMic��DMA�����жϴ������� */
    if ((DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {
        /* RUNNING̬�½������б�����ƣ������������л���������Ϣ���� */
        if (VOICE_MC_STATE_RUNNING == VOICE_McGetForeGroundVoiceState())
        {
            if (CODEC_SWITCH_ON == pstDmaFlag->usMicInSwEnable)
            {
                /* �л����л����� */
                VOICE_PcmSwitchMicBuff();
            }

            /* PC-VOICEģʽ��, ���»���BUFFER�Ŀ�����Ϣ*/
            VOICE_PcmUpdateTxRingBuf();

            /*  �����ϢID */
            stTxProcReqMsg.uhwMsgId = ID_VOICE_VOICE_ENC_REQ;

            /* ���ͱ�����ϢID_VOICE_VOICE_ENC_REQ��PID_VOICE,��ʼ�������д������� */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            DSP_PID_VOICE,
                            &stTxProcReqMsg,
                            sizeof(stTxProcReqMsg));
        }
        else
        {
            /*��RUNNING̬�¼�¼�쳣����RUNNING̬���յ���������DMA�����ж�*/
            OM_LogError(VOICE_MicDmaIsr_NotRunning);
        }
    }
    /* ���ж�����ΪERROR�ж�,��¼�쳣 */
    else
    {
        /*��¼�쳣������DMA Error�ж�*/
        OM_LogError1(VOICE_MicDmaIsr_ErrInt, enIntType);
    }

    return;
}


VOS_VOID VOICE_SpkDmaIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                VOS_UINT32 uwPara)
{
    VOICE_VOICE_RX_PROC_REQ_STRU    stRxProcReqMsg;

    VOICE_MC_DMA_FLAG_STRU          *pstDmaFlag  = VOICE_McGetDmaFlagPtr();

    /* ��ʼ���ظ���Ϣ�ṹ�� */
    UCOM_MemSet(&stRxProcReqMsg, 0, sizeof(VOICE_VOICE_RX_PROC_REQ_STRU));

    /* ���ж�����ΪTC�ж�,ΪSPK��DMA�����жϴ������� */
    if ((DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {
        /* RUNNING̬�½������п��ƣ������������л���UMTS����ʱ���ͻ���*/
        if (VOICE_MC_STATE_RUNNING == VOICE_McGetForeGroundVoiceState())
        {
            if (CODEC_SWITCH_ON == pstDmaFlag->usSpkOutSwEnable)
            {
                /* �л����л����� */
                VOICE_PcmSwitchSpkBuff();
            }
            else
            {
                /* �����һ֡�������� */
                VOICE_PcmClrLastSpkBuff();
            }

            /*  �����ϢID */
            stRxProcReqMsg.uhwMsgId = ID_VOICE_VOICE_DEC_REQ;

            /* ���ͽ�����ϢID_VOICE_VOICE_DEC_REQ��PID_VOICE,��ʼ�������д������� */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            DSP_PID_VOICE,
                            &stRxProcReqMsg,
                            sizeof(stRxProcReqMsg));

            /* ����ʱ���� */
            VOICE_McChkSpkConflick();

            /* PC-VOICEģʽ��, �������л���buffer������Ϣ */
            VOICE_PcmUpdateRxRingBuf();
        }
        else
        {
            /*��RUNNING̬�¼�¼�쳣����RUNNING̬���յ���������DMA�����ж�*/
            OM_LogError1(VOICE_SpkDmaIsr_NotRunning, enIntType);
        }
    }
    /* ���ж�����ΪERROR�ж�,��¼�쳣 */
    else
    {
        /*��¼�쳣������DMA Error�ж�*/
        OM_LogError1(VOICE_SpkDmaIsr_ErrInt, enIntType);
    }
}


VOS_VOID VOICE_SioIsr(DRV_SIO_INT_MASK_ENUM_UINT32 enIntType, VOS_UINT32 uwPara)
{
    if (DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER
        == (enIntType & DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER))
    {
        /* ��¼�쳣,PCMģʽ�£�ΪPCM����FIFO���� */
        OM_LogError1(VOICE_SioIsr_DownOverFlow, enIntType);
    }
    else if (DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER
             == (enIntType & DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER))
    {
        /* ��¼�쳣,PCMģʽ�£�ΪPCM����FIFO���� */
        OM_LogError1(VOICE_SioIsr_UpOverFlow, enIntType);
    }
    else
    {
        /* ��¼�쳣���������ж� */
        OM_LogError1(VOICE_SioIsr_ErrInt, enIntType);
    }
}
VOS_VOID VOICE_EcRefDmaIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16 enIntType,
                VOS_UINT32 uwPara)
{
    /* ���ж�����ΪTC�ж�,ΪMic��DMA�����жϴ������� */
    if ((DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {
        /* RUNNING̬ */
        if (VOICE_MC_STATE_RUNNING != VOICE_McGetForeGroundVoiceState())
        {
            /*��RUNNING̬�¼�¼�쳣����RUNNING̬���յ���������DMA�����ж�*/
            OM_LogError(VOICE_EcRefDmaIsr_NotRunning);
        }
    }
    /* ���ж�����ΪERROR�ж�,��¼�쳣 */
    else
    {
        /*��¼�쳣������DMA Error�ж�*/
        OM_LogError1(VOICE_EcRefDmaIsr_ErrInt, enIntType);
    }

    return;
}
VOS_VOID VOICE_PcmInsertCn(
                VOS_INT16   *pshwIn,
                VOS_INT16    shwLen,
                VOS_INT16    shwPcmRandRange)
{
    VOS_INT16  shwCnt;
    VOS_INT16  shwData;
    VOS_INT16  shwTemp;

    /* С�������������ȵľ���ֵ��������һ֡���������������ֵ������Ҫ����:
       1) ���ܴ���VOICE_PCM_SIGNAL_RAND_RANGE
       2) Ϊ2^N */
    if (shwPcmRandRange >= VOICE_PCM_SIGNAL_RAND_RANGE_32)
    {
        shwPcmRandRange = VOICE_PCM_SIGNAL_RAND_RANGE_32;
    }
    else
    {
        for(shwTemp = VOICE_PCM_SIGNAL_RAND_RANGE_32 >> 1; shwTemp != 0; shwTemp = shwTemp >> 1)
        {
            if ((shwPcmRandRange & shwTemp) != 0)
            {
                shwPcmRandRange = shwTemp;
                break;
            }
        }
    }

    if (0 == shwPcmRandRange)
    {
        UCOM_MemSet(pshwIn, 0, (VOS_UINT32)((VOS_UINT16)shwLen * sizeof(VOS_INT16)));
    }
    else
    {
        shwData = (VOS_INT16)(shwPcmRandRange * 2) - 1;      /* uhwData ������Ϊ[0111..111]����ʽ */
        for(shwCnt = 0; shwCnt < shwLen; shwCnt++)
        {
            shwTemp        = (rand() & 0x7fff ) & shwData;/* [false alarm]:��ȷ�ϴ˴�ʹ��rand������  */
            pshwIn[shwCnt] = shwTemp - shwPcmRandRange;
        }
    }
}


VOS_VOID VOICE_PcmMicInNoData(VOS_VOID)
{
    VOICE_VOICE_TX_PROC_REQ_STRU stTxProcReqMsg;

    UCOM_MemSet(&stTxProcReqMsg, 0, sizeof(stTxProcReqMsg));

    /* ��ӽ�����֡��С��������� */
    VOICE_PcmInsertCn(g_psVoicePcmMicIn, CODEC_PCM_FRAME_LENGTH, VOICE_PCM_SIGNAL_RAND_RANGE_32);

    OM_LogWarning(VOICE_PcmStartDMAFromDataIn_InsertFrame);

    /* ErrorLog ��¼�¼� */
    VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_PCVOICE_INTERPOLATE);

    /* �л������� */
    VOICE_PcmSwitchMicBuff();

    /*  �����ϢID */
    stTxProcReqMsg.uhwMsgId = ID_VOICE_VOICE_ENC_REQ;

    /* ���ͱ�����ϢID_VOICE_VOICE_ENC_REQ��PID_VOICE,��ʼ������������ */
    UCOM_SendOsaMsg(DSP_PID_VOICE, DSP_PID_VOICE, &stTxProcReqMsg, sizeof(stTxProcReqMsg));
}
VOS_UINT32 VOICE_PcmHybrid2Stereo(VOS_INT16 *pshwI2S, VOS_INT16 *pshwLeft, VOS_INT16 *pshwRight, VOS_INT16 shwPcmLen)
{
    VOS_INT16                   shwCnt      = 0;
    VOS_UINT32                  uwSize;
    DRV_SIO_CFG_STRU           *pstCfg      = VOS_NULL;

    pstCfg  = DRV_SIO_GetVoiceCfgPtr();

    if (DRV_SIO_MODE_I2S ==pstCfg->enWorkMode)
    {
        for (shwCnt = 0; shwCnt < shwPcmLen; shwCnt++)
        {
            /* SIO��������16bit,��������16bit */
            pshwRight[shwCnt]  = (VOS_INT16)pshwI2S[shwCnt*2];
            pshwLeft[shwCnt]   = (VOS_INT16)pshwI2S[(shwCnt * 2) + 1];
        }
    }
    else
    {
        uwSize = (VOS_UINT32)((VOS_UINT16)shwPcmLen*(VOS_UINT16)sizeof(VOS_INT16));

        UCOM_MemCpy(pshwRight, pshwI2S, uwSize);
        UCOM_MemCpy(pshwLeft,  pshwI2S, uwSize);
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_PcmStereo2Hybrid(
                VOS_INT16  *pshwSpkOutTemp,
                VOS_INT16  *pshwSpkOut,
                VOS_INT16   shwPcmLen)
{
    VOS_INT16           shwCnt = 0;
    DRV_SIO_CFG_STRU   *pstCfg = VOS_NULL;

    /* ����ж� */
    if (   (VOS_NULL == pshwSpkOutTemp)
        || (VOS_NULL == pshwSpkOut))
    {
        return UCOM_RET_NUL_PTR;
    }

    pstCfg  = DRV_SIO_GetVoiceCfgPtr();

    if (DRV_SIO_MODE_I2S ==pstCfg->enWorkMode)
    {
        /* ��PP�����Ժ���������ΪI2S˫�������� */
        for (shwCnt = 0; shwCnt < shwPcmLen; shwCnt++)
        {
            pshwSpkOut[2 * shwCnt]       = pshwSpkOutTemp[shwCnt];
            pshwSpkOut[(2 * shwCnt) + 1] = pshwSpkOutTemp[shwCnt];
        }
    }
    else
    {
        UCOM_MemCpy(pshwSpkOut, pshwSpkOutTemp, (VOS_UINT16)shwPcmLen*(VOS_UINT16)sizeof(VOS_INT16));
    }

    return UCOM_RET_SUCC;
}


VOS_VOID VOICE_PcmVcLoopMicIsr(DRV_DMA_INT_TYPE_ENUM_UINT16 uhwPara, VOS_UINT32 uwIntType)
{
    VOS_UINT32                      uwSize;
    VOS_UINT32                      uwChannelNum;

    uwChannelNum    = DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr());

    /* �������ݴ�С */
    uwSize          = (CODEC_FRAME_LENGTH_NB * sizeof(VOS_INT16)) * uwChannelNum;

    /* ���ж�����ΪTC�ж�,ΪMic��DMA�����жϴ������̣����ж�����ΪERROR�ж�,��¼�쳣 */
    if (  (DRV_DMA_INT_TYPE_TC1 == uwIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == uwIntType))
    {
        /* �л����л����� */
        VOICE_PcmSwitchMicBuff();

        /* VOICE PCM SPK OUT��VOICE PCM MIC IN����I2S�����ݣ�����Ҫ����I2S<->PCM��ת�� */
        UCOM_MemCpy((void*)g_psVoicePcmSpkOut,
                    (void*)g_psVoicePcmMicIn,
                    uwSize);

        g_uwVcLoopCnt++;
        OM_LogInfo1(VOICE_PcmVcLoopMic, g_uwVcLoopCnt);
    }
    else
    {
        /*��¼�쳣������DMA Error�ж�*/
        OM_LogError(VOICE_PcmVcLoopMicIsr_IntTypeError);
    }

    return;
}
VOS_VOID VOICE_PcmVcLoopSpkIsr(DRV_DMA_INT_TYPE_ENUM_UINT16 uhwPara, VOS_UINT32 uwIntType)
{
    /* ���ж�����ΪTC�ж�,ΪSPK��DMA�����жϴ������̣����ж�����ΪERROR�ж�,��¼�쳣 */
    if (  (DRV_DMA_INT_TYPE_TC1 == uwIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == uwIntType))
    {
        /* �л����л����� */
        VOICE_PcmSwitchSpkBuff();
        g_uwVcLoopCnt++;
        OM_LogInfo1(VOICE_PcmVcLoopSpk, g_uwVcLoopCnt);
    }
    else
    {
        /*��¼�쳣������DMA Error�ж�*/
        OM_LogError(VOICE_PcmVcLoopSpkIsr_IntTypeError);
    }
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

