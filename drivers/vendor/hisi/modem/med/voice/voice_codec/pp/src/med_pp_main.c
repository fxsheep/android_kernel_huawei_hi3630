

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "OmCodecInterface.h"
#include "med_pp_main.h"
#include "om.h"
#include "voice_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_MED_PP_MAIN_C


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
MED_PP_STRU                            g_stMedPpObj;                            /* PPȫ�ֿ���ʵ�� */
AUDIO_PP_STRU                          g_stAudioPpObj;                          /* ��ƵPPȫ�ֿ���ʵ�� */

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_INT16 MED_PP_Init()
 {
    VOS_INT16                           shwFlag;
    MED_PP_STRU                        *pstPP = MED_PP_GetObjPtr();

    /* ����Ԥ�����˲�ʵ�� */
    pstPP->pstPreEmpMc   = MED_PREEMP_Create();
    pstPP->pstPreEmpRc   = MED_PREEMP_Create();
    pstPP->pstPreEmpFar  = MED_PREEMP_Create();

    /* ����ȥ�����˲�ʵ�� */
    pstPP->pstDeEmpMc    = MED_DEEMP_Create();

    /* �������и�ͨ�˲�ʵ�� */
    pstPP->pstHpfTxMc     = MED_HPF_Create();
    pstPP->pstHpfTxRc     = MED_HPF_Create();
    pstPP->pstHpfAecMc    = MED_HPF_Create();
    pstPP->pstHpfAecRc    = MED_HPF_Create();

    /* �������и�ͨ�˲�ʵ�� */
    pstPP->pstHpfRx      = MED_HPF_Create();

    /* ���������豸����ֵʵ�� */
    pstPP->pstDevGainTx  = MED_DEVGAIN_Create();

    /* ���������豸����ֵʵ�� */
    pstPP->pstDevGainRx  = MED_DEVGAIN_Create();

    /* �������в����˲���ʵ�� */
    pstPP->pstCompTx     = MED_COMP_Create();

    /* �������в����˲���ʵ�� */
    pstPP->pstCompRx     = MED_COMP_Create();

    /* ����AECʵ�� */
    pstPP->pstAec        = MED_AEC_Create();

    /* ����NSʵ�� ���� */
    pstPP->pstAnrTx      = MED_EANR_1MIC_Create();
    pstPP->pstAnr2Mic    = MED_ANR_2MIC_Create();

    /* ����ANRʵ�� ���� */
    pstPP->pstAnrRx      = MED_EANR_1MIC_Create();

    /* ��������AGCʵ�� */
    pstPP->pstAgcTx      = MED_AGC_Create();

    /* ����MBDRCʵ�� */
    pstPP->pstMbdrc      = MED_MBDRC_Create();

    /* ��������AIGʵ�� */
    pstPP->pstAigTx      = MED_AIG_Create();

    /* ��������AIGʵ�� */
    pstPP->pstAigRx      = MED_AIG_Create();

    /* ��������AGCʵ�� */
    pstPP->pstAgcRx      = MED_AGC_Create();

    /* ����AVCʵ�� */
    pstPP->pstAvc        = MED_AVC_Create();

    /* ��������VADʵ�� */
    pstPP->pstVadTx      = MED_VAD_Create();

    /* ��������VADʵ�� */
    pstPP->pstVadRx      = MED_VAD_Create();

    /* ������������ʵ�� */
    pstPP->pstSTGain     = MED_SIDETONE_Create();

    shwFlag =
        (  (MED_NULL == pstPP->pstPreEmpMc)
        || (MED_NULL == pstPP->pstPreEmpRc)
        || (MED_NULL == pstPP->pstPreEmpFar)
        || (MED_NULL == pstPP->pstDeEmpMc)
        || (MED_NULL == pstPP->pstHpfTxMc)
        || (MED_NULL == pstPP->pstHpfTxRc)
        || (MED_NULL == pstPP->pstHpfAecMc)
        || (MED_NULL == pstPP->pstHpfAecRc)
        || (MED_NULL == pstPP->pstHpfRx)
        || (MED_NULL == pstPP->pstDevGainTx)
        || (MED_NULL == pstPP->pstDevGainRx)
        || (MED_NULL == pstPP->pstCompTx)
        || (MED_NULL == pstPP->pstCompRx)
        || (MED_NULL == pstPP->pstAec)
        || (MED_NULL == pstPP->pstAnrTx)
        || (MED_NULL == pstPP->pstAnr2Mic)
        || (MED_NULL == pstPP->pstAnrRx)
        || (MED_NULL == pstPP->pstAgcTx)
        || (MED_NULL == pstPP->pstAgcRx)
        || (MED_NULL == pstPP->pstMbdrc)
        || (MED_NULL == pstPP->pstAigTx)
        || (MED_NULL == pstPP->pstAigRx)
        || (MED_NULL == pstPP->pstAvc)
        || (MED_NULL == pstPP->pstVadTx)
        || (MED_NULL == pstPP->pstVadRx)
        || (MED_NULL == pstPP->pstSTGain) );

    return shwFlag;

}



VOS_UINT32 MED_PpSetPara(
                VOICE_NV_STRU                       *pstNv,
                MED_PP_MIC_NUM_ENUM_UINT16         enPpMicNum )
{
    MED_PP_STRU                        *pstPP           = MED_PP_GetObjPtr();
    VOS_UINT32                          uwRet           = UCOM_RET_SUCC;
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate    = MED_PP_GetSampleRate();


    /* ��μ�� */
    if (VOS_NULL == pstNv)
    {
        return UCOM_RET_NUL_PTR;
    }

    pstPP->enMicNum   = enPpMicNum;

    pstPP->uhwFrameId = 0;

    /* Ԥ�����˲��������� */
    uwRet += MED_PREEMP_SetPara(  pstPP->pstPreEmpMc,
                                  pstNv->ashwPreEmp,
                                  CODEC_NV_PREEMP_LEN);
    uwRet += MED_PREEMP_SetPara(  pstPP->pstPreEmpRc,
                                  pstNv->ashwPreEmp,
                                  CODEC_NV_PREEMP_LEN);
    uwRet += MED_PREEMP_SetPara(  pstPP->pstPreEmpFar,
                                  pstNv->ashwPreEmp,
                                  CODEC_NV_PREEMP_LEN);

    /* ȥ�����˲��������� */
    uwRet += MED_DEEMP_SetPara(   pstPP->pstDeEmpMc,
                                  pstNv->ashwDeEmp,
                                  CODEC_NV_DEEMP_LEN);

    /* ���и�ͨ�˲��������� */
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfTxMc,
                                  pstNv->ashwHpfTx,
                                  CODEC_NV_HPF_TX_LEN);
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfTxRc,
                                  pstNv->ashwHpfTx,
                                  CODEC_NV_HPF_TX_LEN);
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfAecMc,
                                  pstNv->ashwHpfTx,
                                  CODEC_NV_HPF_TX_LEN);
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfAecRc,
                                  pstNv->ashwHpfTx,
                                  CODEC_NV_HPF_TX_LEN);

    /* ���и�ͨ�˲��������� */
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfRx,
                                  pstNv->ashwHpfRx,
                                  CODEC_NV_HPF_RX_LEN);

    /* ���������豸ƥ������ֵ */
    uwRet += MED_DEVGAIN_SetPara( pstPP->pstDevGainTx,
                                  &(pstNv->ashwDevGain[0]),
                                  1);

    /* ���������豸ƥ������ֵ */
    uwRet += MED_DEVGAIN_SetPara( pstPP->pstDevGainRx,
                                  &(pstNv->ashwDevGain[1]),
                                  1);

    /* �������в����˲���ϵ�� */
    uwRet += MED_COMP_SetPara((VOS_UINT32)enSampleRate,
                                          pstPP->pstCompRx,
                                          pstNv->ashwCompRx,
                                          CODEC_NV_COMP_RX_LEN);

    /* �������в����˲���ϵ�� */
    uwRet += MED_COMP_SetPara((VOS_UINT32)enSampleRate,
                                          pstPP->pstCompTx,
                                          pstNv->ashwCompTx,
                                          CODEC_NV_COMP_TX_LEN);

    /* ����AEC���� */
    uwRet += MED_AEC_SetPara(     pstPP->pstAec,
                                  pstNv->ashwAec,
                                  CODEC_NV_AEC_LEN,
                                  enSampleRate,
                                  enPpMicNum);

    /* ����ANR���� */
    uwRet += MED_EANR_1MIC_SetPara( pstPP->pstAnrTx,
                                   pstNv->ashwAnr1MicTx,
                                   CODEC_NV_EANR_1MIC_LEN,
                                   enSampleRate);

    uwRet += MED_EANR_1MIC_SetPara( pstPP->pstAnrRx,
                                   pstNv->ashwAnr1MicRx,
                                   CODEC_NV_EANR_1MIC_LEN,
                                   enSampleRate);

    uwRet += MED_ANR_2MIC_SetPara( pstPP->pstAnr2Mic,
                                   pstNv->ashwAnr2Mic,
                                   CODEC_NV_ANR_2MIC_LEN,
                                   enSampleRate);

    /* ��������AGC����*/
    uwRet += MED_AGC_SetPara(     pstPP->pstAgcTx,
                                  pstNv->ashwAgcTx,
                                  CODEC_NV_AGC_TX_LEN);

    /* ��������AGC����*/
    uwRet += MED_AGC_SetPara(     pstPP->pstAgcRx,
                                  pstNv->ashwAgcRx,
                                  CODEC_NV_AGC_RX_LEN);
    /* ����MBDRC����*/
    uwRet += MED_MBDRC_SetPara(   pstPP->pstMbdrc,
                                  pstNv->ashwMbdrc,
                                  CODEC_NV_MBDRC_LEN,
                                  enSampleRate);

    /* ��������AIG����*/
    uwRet += MED_AIG_SetPara (    pstPP->pstAigTx,
                                  pstNv->ashwAigTx,
                                  CODEC_NV_AIG_TX_LEN,
                                  enSampleRate);

    /* ��������AIG����*/
    uwRet += MED_AIG_SetPara (    pstPP->pstAigRx,
                                  pstNv->ashwAigRx,
                                  CODEC_NV_AIG_RX_LEN,
                                  enSampleRate);


    /* ����AVC����*/
    uwRet += MED_AVC_SetPara(     pstPP->pstAvc,
                                  pstNv->ashwAvc,
                                  CODEC_NV_AVC_LEN,
                                  enPpMicNum);

    /* ��������VAD����*/
    uwRet += MED_VAD_SetPara(     pstPP->pstVadTx,
                                  pstNv->ashwVadTx,
                                  CODEC_NV_VAD_TX_LEN);

    /* ��������VAD����*/
    uwRet += MED_VAD_SetPara(     pstPP->pstVadRx,
                                  pstNv->ashwVadRx,
                                  CODEC_NV_VAD_RX_LEN);

    /* ���²����������*/
    uwRet += MED_SIDETONE_SetPara(pstPP->pstSTGain,
                                  pstNv->ashwSTGain,
                                  CODEC_NV_SIDEGAIN_LEN);

    return uwRet;

}


VOS_UINT32 MED_PpGetPara( VOICE_NV_STRU *pstNv)
{
    MED_PP_STRU                        *pstObjs     = MED_PP_GetObjPtr();
    VOS_UINT32                          swRet       = 0;

    /* ע: ��������������ʹ��PP��NV����� */

    /* ��Ԥ�����˲���д��pstNv��ָ�ڴ��С���Ϊ������˹���Ԥ�����˲�NV�������ڴ�ֻ��get�����*/
    swRet += MED_PREEMP_GetPara( pstObjs->pstPreEmpMc,
                                 pstNv->ashwPreEmp,
                                 CODEC_NV_PREEMP_LEN);

    /* ��ȥ�����˲���д��pstNv��ָ�ڴ��� */
    swRet += MED_DEEMP_GetPara(  pstObjs->pstDeEmpMc,
                                 pstNv->ashwDeEmp,
                                 CODEC_NV_DEEMP_LEN);

    /* �����и�ͨ�˲�������д��pstNv��ָ�ڴ��� */
    swRet += MED_HPF_GetPara(    pstObjs->pstHpfTxMc,
                                 pstNv->ashwHpfTx,
                                 CODEC_NV_HPF_TX_LEN);

    /* �����и�ͨ�˲�������д��pstNv��ָ�ڴ���*/
    swRet += MED_HPF_GetPara(    pstObjs->pstHpfRx,
                                 pstNv->ashwHpfRx,
                                 CODEC_NV_HPF_RX_LEN);

    /* �������豸����ֵ��д��pstNv��ָ�ڴ��� */
    swRet += MED_DEVGAIN_GetPara(pstObjs->pstDevGainTx,
                                 &(pstNv->ashwDevGain[0]),
                                 1);

    /* �������豸����ֵ��д��pstNv��ָ�ڴ��� */
    swRet += MED_DEVGAIN_GetPara(pstObjs->pstDevGainRx,
                                 &(pstNv->ashwDevGain[1]),
                                 1);

    /* ����������ֵ��д��pstNv��ָ�ڴ��� */
    swRet += MED_SIDETONE_GetPara(pstObjs->pstSTGain,
                                 pstNv->ashwSTGain,
                                 CODEC_NV_SIDEGAIN_LEN);

    /* �����в����˲���ϵ����д��pstNv��ָ�ڴ��� */
    swRet += MED_COMP_GetPara(   pstObjs->pstCompTx,
                                 pstNv->ashwCompTx,
                                 CODEC_NV_COMP_TX_LEN);

    /* �����в����˲���ϵ����д��pstNv��ָ�ڴ��� */
    swRet += MED_COMP_GetPara(   pstObjs->pstCompRx,
                                 pstNv->ashwCompRx,
                                 CODEC_NV_COMP_RX_LEN);

    /* ��AEC������д��pstNv��ָ�ڴ��� */
    swRet +=  MED_AEC_GetPara(   pstObjs->pstAec,
                                 pstNv->ashwAec,
                                 CODEC_NV_AEC_LEN);

    /* ��ANR������д��pstNv��ָ�ڴ��� */
    swRet +=  MED_EANR_1MIC_GetPara(   pstObjs->pstAnrTx,
                                      pstNv->ashwAnr1MicTx,
                                      CODEC_NV_EANR_1MIC_LEN);

    swRet +=  MED_EANR_1MIC_GetPara(   pstObjs->pstAnrRx,
                                      pstNv->ashwAnr1MicRx,
                                      CODEC_NV_EANR_1MIC_LEN);


    swRet +=  MED_ANR_2MIC_GetPara(   pstObjs->pstAnr2Mic,
                                      pstNv->ashwAnr2Mic,
                                      CODEC_NV_ANR_2MIC_LEN);

    /* ������AGC������д��pstNv��ָ�ڴ���*/
    swRet +=  MED_AGC_GetPara(   pstObjs->pstAgcTx,
                                 pstNv->ashwAgcTx,
                                 CODEC_NV_AGC_TX_LEN);

    /* ��MBDRC������д��pstNv��ָ�ڴ���*/
    swRet +=  MED_MBDRC_GetPara( pstObjs->pstMbdrc,
                                 pstNv->ashwMbdrc,
                                 CODEC_NV_MBDRC_LEN);

    /* ������AIG������д��pstNv��ָ�ڴ���*/
    swRet +=  MED_AIG_GetPara( pstObjs->pstAigTx,
                               pstNv->ashwAigTx,
                               CODEC_NV_AIG_TX_LEN);

    /* ������AIG������д��pstNv��ָ�ڴ���*/
    swRet +=  MED_AIG_GetPara( pstObjs->pstAigRx,
                               pstNv->ashwAigRx,
                               CODEC_NV_AIG_RX_LEN);

    /* ������AGC������д��pstNv��ָ�ڴ���*/
    swRet +=  MED_AGC_GetPara(   pstObjs->pstAgcRx,
                                 pstNv->ashwAgcRx,
                                 CODEC_NV_AGC_RX_LEN);

    /* ��AVC������д��pstNv��ָ�ڴ���*/
    swRet +=  MED_AVC_GetPara(   pstObjs->pstAvc,
                                 pstNv->ashwAvc,
                                 CODEC_NV_AVC_LEN);

    /* ������VAD������д��pstNv��ָ�ڴ���*/
    swRet +=  MED_VAD_GetPara(   pstObjs->pstVadTx,
                                 pstNv->ashwVadTx,
                                 CODEC_NV_VAD_TX_LEN);

    /* ������VAD������д��pstNv��ָ�ڴ���*/
    swRet +=  MED_VAD_GetPara(   pstObjs->pstVadRx,
                                 pstNv->ashwVadRx,
                                 CODEC_NV_VAD_RX_LEN);

    return swRet;

}


VOS_UINT32 MED_TxPp(
                VOS_INT16                         *pshwMcIn,
                VOS_INT16                         *pshwRcIn,
                VOS_INT16                         *pshwLineOut,
                VOS_INT16                         *pshwEcRef)
{
    MED_PP_STRU        *pstObjs     = MED_PP_GetObjPtr();
    VOS_UINT32          uwRet;

    /* ����Զ������ */
    MED_AEC_UpdateRx(pstObjs->pstAec, pshwEcRef);

    /* ���ݵ�˫���ʹ�ܱ�־�ֱ����*/
    if (MED_PP_MIC_NUM_1 == pstObjs->enMicNum)
    {
        /*  �������������ǰ���� */
        uwRet = MED_PpTx_1Mic(pshwMcIn, pshwLineOut);

    }
    else
    {
        /* ˫�����������ǰ���� */
        uwRet = MED_PpTx_2Mic(pshwMcIn, pshwRcIn, pshwLineOut);
    }

    /* �����˲���ϵ�� */
    MED_AEC_UpdateTx();

    return uwRet;

}
VOS_UINT32 MED_PpTx_1Mic(
                VOS_INT16                         *pshwMicIn,
                VOS_INT16                         *pshwLineOut)
{
    MED_PP_STRU                        *pstObjs     = MED_PP_GetObjPtr();
    CODEC_VAD_FLAG_ENUM_INT16           enVadFlag;
    VOS_UINT32                          uwRet = 0;
    VOS_INT16                           shwFrameLength =  MED_PP_GetFrameLength();

    /* ����ж� */
    if (   (MED_NULL == pshwMicIn)
        || (MED_NULL == pshwLineOut))
    {
        return UCOM_RET_NUL_PTR;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_TxPp_1MIC);

    /* 1. VAD */
    uwRet += MED_VAD_Main(pstObjs->pstVadTx, pshwMicIn, &enVadFlag);

    /* 2. Tx ƥ������: shwMicIn -> shwMicIn */
    uwRet += MED_DEVGAIN_Main(pstObjs->pstDevGainTx, pshwMicIn, pshwMicIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 3. Tx ��ͨ�˲� */
    uwRet += MED_HPF_Main(pstObjs->pstHpfTxMc, pshwMicIn, pshwMicIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 4. ���²��� */
    uwRet += MED_SIDESTONE_UpdateTx(pstObjs->pstSTGain, pshwMicIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    /* 5. �����ź�Ԥ�����˲� */
    uwRet += MED_PREEMP_Main(pstObjs->pstPreEmpMc, pshwMicIn, pshwMicIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    /* ��ȡAEC������ */
    VOICE_DbgSendHook(VOICE_HOOK_AECIN_PCM, pshwMicIn, (VOS_UINT16)shwFrameLength);

    /* 6. EC��NS���ϴ��� */
    uwRet += MED_1MIC_AECANR_Main(pshwMicIn,
                                  pshwLineOut,
                                  enVadFlag,
                                  pstObjs->pstPreEmpFar,
                                  pstObjs->pstHpfAecMc,
                                  pstObjs->pstAvc);

    /* 7. AGC���� */
    uwRet += MED_AGC_Main(pstObjs->pstAgcTx, pshwLineOut, enVadFlag, pshwLineOut);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AIG_Main_TX);

    /* 8. AIG���� */
    uwRet += MED_AIG_Main(pstObjs->pstAigTx, pshwLineOut, pshwLineOut, pshwLineOut, enVadFlag);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AIG_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX);

    /* 9 ȥ���� */
    uwRet += MED_DEEMP_Main(pstObjs->pstDeEmpMc, pshwLineOut, pshwLineOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_COMP_Main_TX);

    /* 10. Tx �����˲� */
    uwRet += MED_COMP_Main(pstObjs->pstCompTx, pshwLineOut, pshwLineOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_COMP_Main_TX);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_TxPp_1MIC);

    if (uwRet > 0)
    {
        return UCOM_RET_ERR_STATE;
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 MED_PpTx_2Mic(
                VOS_INT16                         *pshwMcIn,
                VOS_INT16                         *pshwRcIn,
                VOS_INT16                         *pshwLineOut)
{
    MED_PP_STRU                        *pstObjs     = MED_PP_GetObjPtr();
    CODEC_VAD_FLAG_ENUM_INT16           enVadFlag;
    VOS_UINT32                          uwRet       = 0;
    VOS_INT16                           shwFrameLength =  MED_PP_GetFrameLength();

    /* ����ж� */
    if (   (MED_NULL == pshwMcIn)
        || (MED_NULL == pshwRcIn)
        || (MED_NULL == pshwLineOut))
    {
        return UCOM_RET_NUL_PTR;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_TxPp_2MIC);

    /* 1. VAD */
    uwRet += MED_VAD_Main(pstObjs->pstVadTx, pshwMcIn, &enVadFlag);

    /* 2. Tx �������ƥ������: shwMcIn -> shwMcIn��shwRcIn -> shwRcIn */
    uwRet += MED_DEVGAIN_Main(pstObjs->pstDevGainTx, pshwMcIn, pshwMcIn);
    uwRet += MED_DEVGAIN_Main(pstObjs->pstDevGainTx, pshwRcIn, pshwRcIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 3. Tx ������˸�ͨ�˲� */
    uwRet += MED_HPF_Main(pstObjs->pstHpfTxMc, pshwMcIn, pshwMcIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    uwRet += MED_HPF_Main(pstObjs->pstHpfTxRc, pshwRcIn, pshwRcIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 4. ���²��� */
    uwRet += MED_SIDESTONE_UpdateTx(pstObjs->pstSTGain, pshwMcIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    /* 5. ��������ź�Ԥ�����˲� */
    uwRet += MED_PREEMP_Main(pstObjs->pstPreEmpMc, pshwMcIn, pshwMcIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    uwRet += MED_PREEMP_Main(pstObjs->pstPreEmpRc, pshwRcIn, pshwRcIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    /* ��ȡAEC������ */
    VOICE_DbgSendHook(VOICE_HOOK_AECIN_PCM, pshwMcIn, (VOS_UINT16)shwFrameLength);

    /* 6. EC��NS���ϴ��� */
    uwRet += MED_2MIC_AECANR_Main(pshwMcIn,
                                  pshwRcIn,
                                  pshwLineOut,
                                  enVadFlag,
                                  pstObjs->pstPreEmpFar,
                                  pstObjs->pstHpfAecMc,
                                  pstObjs->pstHpfAecRc,
                                  pstObjs->pstAvc);

    /* 7. AGC���� */
    uwRet += MED_AGC_Main(pstObjs->pstAgcTx, pshwLineOut, enVadFlag, pshwLineOut);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AIG_Main_TX);

    /* 8. AIG���� */
    uwRet += MED_AIG_Main(pstObjs->pstAigTx, pshwLineOut, pshwLineOut, pshwLineOut, enVadFlag);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AIG_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX);

    /* 9 ȥ���� */
    uwRet += MED_DEEMP_Main(pstObjs->pstDeEmpMc, pshwLineOut, pshwLineOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_COMP_Main_TX);

    /* 10. Tx �����˲� */
    uwRet += MED_COMP_Main(pstObjs->pstCompTx, pshwLineOut, pshwLineOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_COMP_Main_TX);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_TxPp_2MIC);

    if (uwRet > 0)
    {
        return UCOM_RET_ERR_STATE;
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 MED_RxPp( VOS_INT16                         *pshwLineIn)
{
    MED_PP_STRU                        *pstObjs        = MED_PP_GetObjPtr();
    MED_EANR_STRU                      *pstMedAnr;
    CODEC_VAD_FLAG_ENUM_INT16           enVadFlag;
    VOS_UINT32                          uwRet          = 0;
    VOS_INT16                          *pshwAigRef;
    VOS_INT16                           shwDeviceGain;
    VOS_INT16                           shwFrameLen    = MED_PP_GetFrameLength();

    /* ����ж� */
    if (   (MED_NULL == pshwLineIn)
        || (MED_NULL == pstObjs))
    {
        return UCOM_RET_NUL_PTR;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_RxPp);

    pstMedAnr = (MED_EANR_STRU *)pstObjs->pstAnrRx;

    /* ֡���ۼ� */
    pstObjs->uhwFrameId = pstObjs->uhwFrameId + 1;

    /* �����ڴ� */
    pshwAigRef = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

    /* 2. VAD */
    uwRet += MED_VAD_Main(pstObjs->pstVadRx, pshwLineIn, &enVadFlag);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_RX);

    /* 3. ��ͨ�˲��� */
    uwRet += MED_HPF_Main(pstObjs->pstHpfRx, pshwLineIn, pshwLineIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_RX);

    /* AIG ���ݻ��� */
    CODEC_OpVecCpy(pshwAigRef, pshwLineIn, shwFrameLen);

    /* 4. ����ANR��MBDRC */
    uwRet += MED_ANR_MBDRC_Rx_Main(pstObjs->pstAnrRx,
                                   pstObjs->pstMbdrc,
                                   pshwLineIn,
                                   pshwLineIn,
                                   enVadFlag,
                                   (VOS_UINT16)(pstMedAnr->shwEnable));

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AIG_Main_RX);

    /* 5. AIG */
    uwRet += MED_AIG_Main(pstObjs->pstAigRx, pshwLineIn, pshwAigRef, pshwLineIn, enVadFlag);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AIG_Main_RX);

    /* 6. AGCǰ���е��������� */
    uwRet += MED_AGC_PreClearNoise(pstObjs->pstAgcRx, pshwLineIn, pshwLineIn);

    /* 7. AGC */
    uwRet += MED_AGC_Main(pstObjs->pstAgcRx, pshwLineIn, enVadFlag, pshwLineIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_COMP_Main_RX);

    /* 8. Rx �����˲� */
    uwRet += MED_COMP_Main(pstObjs->pstCompRx, pshwLineIn, pshwLineIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_COMP_Main_RX);

    /* 9. AVC */
    (VOS_VOID)MED_DEVGAIN_GetPara(pstObjs->pstDevGainRx, &shwDeviceGain, 1);
    uwRet += MED_AVC_Main(pstObjs->pstAvc, pshwLineIn, shwDeviceGain);

    /* 10. ��������, ������һ֡���������� */
    uwRet += MED_SIDETONE_Main(pstObjs->pstSTGain, pshwLineIn, pshwLineIn);

    /* 11. ƥ������, shwSpkOut -> shwSpkOut */
    uwRet += MED_DEVGAIN_Main(pstObjs->pstDevGainRx, pshwLineIn, pshwLineIn);

    /* �ͷ��ڴ� */
    UCOM_MemFree(pshwAigRef);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_RxPp);

    if (uwRet > 0)
    {
        return UCOM_RET_ERR_STATE;
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 MED_PpDestroy(VOS_VOID)
{
    MED_PP_STRU              *pstObjs     = MED_PP_GetObjPtr();
    VOS_UINT32                uwRet = 0;

    /* �ͷ���������mic�ź�Ԥ�����˲�ʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_PREEMP_Destroy(&(pstObjs->pstPreEmpMc));
    uwRet += MED_PREEMP_Destroy(&(pstObjs->pstPreEmpRc));

    /* �ͷ�����far_end�ź�Ԥ�����˲�ʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_PREEMP_Destroy(&(pstObjs->pstPreEmpFar));

    /* �ͷ�����mic�ź�ȥ�����˲�ʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_DEEMP_Destroy(&(pstObjs->pstDeEmpMc));

    /* �ͷ����и�ͨ�˲�ʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfTxMc));
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfTxRc));
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfAecMc));
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfAecRc));

    /* �ͷ����и�ͨ�˲�ʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfRx));

    /* �ͷ���������ֵʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_DEVGAIN_Destroy(&(pstObjs->pstDevGainTx));

    /* �ͷ���������ֵʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_DEVGAIN_Destroy(&(pstObjs->pstDevGainRx));

    /* �ͷŲ�������ʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_SIDETONE_Destroy(&(pstObjs->pstSTGain));

    /* �ͷ����в����˲���ʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompTx));

    /* �ͷ����в����˲���ʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompRx));

    /* �ͷ�AECʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_AEC_Destroy(&(pstObjs->pstAec));

    /* �ͷ�ANRʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_EANR_1MIC_Destroy(&(pstObjs->pstAnrTx));
    uwRet += MED_EANR_1MIC_Destroy(&(pstObjs->pstAnrRx));
    uwRet += MED_ANR_2MIC_Destroy(&(pstObjs->pstAnr2Mic));

    /* �ͷ�����AGCʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_AGC_Destroy(&(pstObjs->pstAgcTx));

    /* �ͷ�����AGCʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_AGC_Destroy(&(pstObjs->pstAgcRx));

    /* �ͷ�MBDRCʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_MBDRC_Destroy(&(pstObjs->pstMbdrc));

    /* �ͷ�����AIGʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_AIG_Destroy(&(pstObjs->pstAigTx));

    /* �ͷ�����AIGʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_AIG_Destroy(&(pstObjs->pstAigRx));

    /* �ͷ�AVCʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_AVC_Destroy(&(pstObjs->pstAvc));

    /* �ͷ�����VADʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_VAD_Destroy(&(pstObjs->pstVadTx));

    /* �ͷ�����VADʵ�壬������g_stMedCtrlProcObjs */
    uwRet += MED_VAD_Destroy(&(pstObjs->pstVadRx));

    return uwRet;

}


VOS_INT16 AUDIO_PP_Init()
 {
    VOS_INT16                           shwFlag;
    AUDIO_PP_STRU                      *pstPP = AUDIO_PP_GetObjPtr();

    /* �������������������˲���ʵ�� */
    pstPP->pstCompTxL    = MED_COMP_Create();

    /* �������������������˲���ʵ�� */
    pstPP->pstCompTxR     = MED_COMP_Create();

    /* �������������������˲���ʵ�� */
    pstPP->pstCompRxL     = MED_COMP_Create();

    /* �������������������˲���ʵ�� */
    pstPP->pstCompRxR     = MED_COMP_Create();

    /* ����������MBDRCʵ�� */
    pstPP->pstMbdrcL      = MED_MBDRC_Create();

    /* ����������MBDRCʵ�� */
    pstPP->pstMbdrcR      = MED_MBDRC_Create();

    shwFlag =
        (  (VOS_NULL == pstPP->pstCompTxL)
        || (VOS_NULL == pstPP->pstCompTxR)
        || (VOS_NULL == pstPP->pstCompRxL)
        || (VOS_NULL == pstPP->pstCompRxR)
        || (VOS_NULL == pstPP->pstMbdrcL)
        || (VOS_NULL == pstPP->pstMbdrcR));

    return shwFlag;

}

VOS_UINT32 AUDIO_PpSetPara(
                AUDIO_NV_STRU             *pstNv,
                VOS_UINT32                 uwSampleRate)
{
    VOS_UINT32                uwRet      = UCOM_RET_SUCC;
    AUDIO_PP_STRU            *pstPp      = AUDIO_PP_GetObjPtr();

    /* ��μ�� */
    if (VOS_NULL == pstNv)
    {
        return UCOM_RET_NUL_PTR;
    }

    /* �������������������˲���ϵ�� */
    uwRet += MED_COMP_SetPara(  uwSampleRate,
                                pstPp->pstCompTxL,
                                pstNv->ashwCompTx,
                                AUDIO_NV_COMP_TX_LEN);

    /* �������������������˲���ϵ�� */
    uwRet += MED_COMP_SetPara(  uwSampleRate,
                                pstPp->pstCompTxR,
                                pstNv->ashwCompTx,
                                AUDIO_NV_COMP_TX_LEN);

    /* �������������������˲���ϵ�� */
    uwRet += MED_COMP_SetPara(  uwSampleRate,
                                pstPp->pstCompRxL,
                                pstNv->ashwCompRx,
                                AUDIO_NV_COMP_RX_LEN);

    /* �������������������˲���ϵ�� */
    uwRet += MED_COMP_SetPara(  uwSampleRate,
                                pstPp->pstCompRxR,
                                pstNv->ashwCompRx,
                                AUDIO_NV_COMP_RX_LEN);

    /* ������Ƶ������MBDRCϵ�� */
    uwRet += MED_MBDRC_SetPara( pstPp->pstMbdrcL,
                                pstNv->ashwMbdrc,
                                AUDIO_NV_MBDRC_LEN,
                                (VOS_INT32)uwSampleRate);

    /* ������Ƶ������MBDRCϵ�� */
    uwRet += MED_MBDRC_SetPara( pstPp->pstMbdrcR,
                                pstNv->ashwMbdrc,
                                AUDIO_NV_MBDRC_LEN,
                                (VOS_INT32)uwSampleRate);

    return uwRet;

}
VOS_UINT32 AUDIO_PpGetPara(AUDIO_NV_STRU   *pstNv)
{
    AUDIO_PP_STRU            *pstPp     = AUDIO_PP_GetObjPtr();
    VOS_UINT32                uwRet     = UCOM_RET_SUCC;

    /* �����������������˲���ϵ����д��pstNv��ָ�ڴ��� */
    uwRet += MED_COMP_GetPara(   pstPp->pstCompTxL,
                                 pstNv->ashwCompTx,
                                 AUDIO_NV_COMP_TX_LEN);

    /* �����������������˲���ϵ����д��pstNv��ָ�ڴ��� */
    uwRet += MED_COMP_GetPara(   pstPp->pstCompTxR,
                                 pstNv->ashwCompTx,
                                 AUDIO_NV_COMP_TX_LEN);

    /* �����������������˲���ϵ����д��pstNv��ָ�ڴ��� */
    uwRet += MED_COMP_GetPara(   pstPp->pstCompRxL,
                                 pstNv->ashwCompRx,
                                 AUDIO_NV_COMP_RX_LEN);

    /* �����������������˲���ϵ����д��pstNv��ָ�ڴ��� */
    uwRet += MED_COMP_GetPara(   pstPp->pstCompRxR,
                                 pstNv->ashwCompRx,
                                 AUDIO_NV_COMP_RX_LEN);

    /* ��������MBDRC������д��pstNv��ָ�ڴ���*/
    uwRet += MED_MBDRC_GetPara(  pstPp->pstMbdrcL,
                                 pstNv->ashwMbdrc,
                                 AUDIO_NV_MBDRC_LEN);

    /* ��������MBDRC������д��pstNv��ָ�ڴ���*/
    uwRet += MED_MBDRC_GetPara(  pstPp->pstMbdrcR,
                                 pstNv->ashwMbdrc,
                                 AUDIO_NV_MBDRC_LEN);

    return uwRet;

}


VOS_UINT32 AUDIO_ProcPpTx(
                VOS_INT16                         *pshwMicInL,
                VOS_INT16                         *pshwMicInR,
                VOS_INT16                         *pshwLineOutL,
                VOS_INT16                         *pshwLineOutR,
                AUDIO_PP_CHANNEL_ENUM_UINT16       uhwChannel)
{
    AUDIO_PP_STRU                      *pstObjs     = AUDIO_PP_GetObjPtr();
    VOS_UINT32                          uwRet       = UCOM_RET_SUCC;

    /* ����ж� */
    if (  ((VOS_NULL == pshwMicInL) && (VOS_NULL == pshwMicInR))
        ||((VOS_NULL == pshwLineOutL) && (VOS_NULL == pshwLineOutR))
        ||(VOS_NULL == pstObjs))
    {
        return UCOM_RET_NUL_PTR;
    }

    switch (uhwChannel)
    {
        /* Tx �����������˲� */
        case AUDIO_PP_CHANNEL_LEFT  :
        {
            uwRet = MED_COMP_Main(pstObjs->pstCompTxL, pshwMicInL, pshwLineOutL);
            break;
        }

        /* Tx �����������˲� */
        case AUDIO_PP_CHANNEL_RIGHT :
        {
            uwRet = MED_COMP_Main(pstObjs->pstCompTxR, pshwMicInR, pshwLineOutR);
            break;
        }

        /* Tx ˫���������˲� */
        case AUDIO_PP_CHANNEL_ALL   :
        {
            uwRet  = MED_COMP_Main(pstObjs->pstCompTxL, pshwMicInL, pshwLineOutL);
            uwRet += MED_COMP_Main(pstObjs->pstCompTxR, pshwMicInR, pshwLineOutR);
            break;
        }

        default                     :
        {
            return UCOM_RET_ERR_STATE;
        }
    }

    return uwRet;
}


VOS_UINT32 AUDIO_ProcPpRx(
                VOS_INT16                         *pshwLineInL,
                VOS_INT16                         *pshwLineInR,
                VOS_INT16                         *pshwLineOutL,
                VOS_INT16                         *pshwLineOutR,
                AUDIO_PP_CHANNEL_ENUM_UINT16       uhwChannel)
{
    AUDIO_PP_STRU                      *pstObjs     = AUDIO_PP_GetObjPtr();
    VOS_UINT32                          uwRet       = UCOM_RET_SUCC;

    /* ����ж� */
    if (  ((VOS_NULL == pshwLineInL) && (VOS_NULL == pshwLineInR))
        ||((VOS_NULL == pshwLineOutL) && (VOS_NULL == pshwLineOutR))
        ||(VOS_NULL == pstObjs))
    {
        return UCOM_RET_NUL_PTR;
    }

    switch (uhwChannel)
    {
        /* Rx �����������˲���MBDRC����*/
        case AUDIO_PP_CHANNEL_LEFT :
        {
            uwRet += MED_COMP_Main(pstObjs->pstCompRxL, pshwLineInL, pshwLineOutL);

            uwRet += MED_ANR_MBDRC_Rx_Main(VOS_NULL,
                                           pstObjs->pstMbdrcL,
                                           pshwLineOutL,
                                           pshwLineOutL,
                                           CODEC_VAD_SPEECH,
                                           CODEC_SWITCH_OFF);
            break;
        }

        /* Rx �����������˲���MBDRC����*/
        case AUDIO_PP_CHANNEL_RIGHT :
        {
            uwRet += MED_COMP_Main(pstObjs->pstCompRxR, pshwLineInR, pshwLineOutR);

            uwRet += MED_ANR_MBDRC_Rx_Main(VOS_NULL,
                                           pstObjs->pstMbdrcR,
                                           pshwLineOutR,
                                           pshwLineOutR,
                                           CODEC_VAD_SPEECH,
                                           CODEC_SWITCH_OFF);
            break;
        }

        /* Rx ˫���������˲���MBDRC����*/
        case AUDIO_PP_CHANNEL_ALL :
        {
            uwRet += MED_COMP_Main(pstObjs->pstCompRxL, pshwLineInL, pshwLineOutL);

            uwRet += MED_ANR_MBDRC_Rx_Main(VOS_NULL,
                                           pstObjs->pstMbdrcL,
                                           pshwLineOutL,
                                           pshwLineOutL,
                                           CODEC_VAD_SPEECH,
                                           CODEC_SWITCH_OFF);

            uwRet += MED_COMP_Main(pstObjs->pstCompRxR, pshwLineInR, pshwLineOutR);

            uwRet += MED_ANR_MBDRC_Rx_Main(VOS_NULL,
                                           pstObjs->pstMbdrcR,
                                           pshwLineOutR,
                                           pshwLineOutR,
                                           CODEC_VAD_SPEECH,
                                           CODEC_SWITCH_OFF);
            break;
        }

        default :
        {
            return UCOM_RET_ERR_STATE;
        }
    }
    if (uwRet > 0)
        {
            return UCOM_RET_ERR_STATE;
        }

    return UCOM_RET_SUCC;
}

VOS_UINT32 AUDIO_PpDestroy(VOS_VOID)
{
    AUDIO_PP_STRU              *pstObjs     = AUDIO_PP_GetObjPtr();
    VOS_UINT32                  uwRet       = UCOM_RET_SUCC;

    /* �ͷ����������������˲���ʵ�� */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompTxL));

    /* �ͷ����������������˲���ʵ�� */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompTxR));

    /* �ͷ����������������˲���ʵ�� */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompRxL));

    /* �ͷ����������������˲���ʵ�� */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompRxR));

    /* �ͷ�������MBDRCʵ�� */
    uwRet += MED_MBDRC_Destroy(&(pstObjs->pstMbdrcL));

    /* �ͷ�������MBDRCʵ�� */
    uwRet += MED_MBDRC_Destroy(&(pstObjs->pstMbdrcR));

    return uwRet;

}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

