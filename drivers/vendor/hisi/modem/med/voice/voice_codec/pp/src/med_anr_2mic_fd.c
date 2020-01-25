

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "med_anr_2mic_comm.h"
#include "med_anr_2mic_fd.h"
#include "om.h"
#include "med_aec_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* 2MICƵ����˲��ṹ�� */
MED_2MIC_POSTFILTER_STRU                g_stAnr2MicFdProcess;

/* ����ź�Ƶ�ʲ���ϵ�� */
const VOS_INT16 g_ashwAnr2MicCompCoef[321]     =
    {8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192,
     20577, 20577, 20577, 20577, 20577, 20577, 20577, 20577, 20577,
     29066, 29066, 29066, 29066, 29066, 29066, 29066, 29066, 29066,
     29066, 29066, 29066, 29066, 29066, 32613, 32613, 32613, 32613,
     32613, 32613, 32613, 32613, 32613, 32613, 32613, 32613, 32613,
     32613, 32613, 20577, 20577, 20577, 20577, 20577, 20577, 20577,
     20577, 20577, 20577, 20577, 20577, 20577};

/* 2Mic�Ӵ�Ƶ������ */
const VOS_INT16 g_ashwMedAnr2MicChTbl[MED_2MIC_NUM_CHAN_16k][2] =
{
    {1,     2},
    {3,     4},
    {5,     6},
    {7,     8},
    {9,    11},
    {12,   14},
    {15,   17},
    {18,   20},
    {21,   23},
    {24,   26},
    {27,   29},
    {30,   32},
    {33,   35},
    {36,   38},
    {39,   41},
    {42,   44},
    {45,   49},
    {50,   54},
    {55,   59},
    {60,   64},
    {65,   69},
    {70,   74},
    {75,   79},
    {80,   86},
    {87,   93},
    {94,  101},
    {102, 110},
    {111, 119},
    {120, 128},
    {129, 158},
    {159, 159},
    {160, 161},
    {162, 176},
    {177, 191},
    {192, 210},
    {211, 229},
    {230, 250},
    {251, 273},
    {274, 296},
    {297, 321}
};

/* �������������ڸ��ʺ�����������˹�����֮�ȵ�ƽ��ϵ�� */
const VOS_INT16 g_ashwAnr2MicPsdRatioFreqAlpha[5] =
    {3277, 6553, 13107, 6553, 3277};

/* ����OM_LSA����ʱ���Ӵ���ƽ��ϵ�� */
const VOS_INT16 g_ashwAnr2MicBandAlpha[3]         =
    {6553, 19660, 6553};

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_UINT32 MED_ANR_2MIC_FdInit(
                MED_2MIC_NV_STRU                 *pst2MICParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate,
                VOS_INT16                         shwFftLen)
{
    MED_2MIC_POSTFILTER_STRU            *pstPostFilter       = MED_2MIC_GetFdProcessPtr();
    MED_2MIC_COMPFREQAP_STRU            *pstCompFreqAp       = &(pstPostFilter->stCompFreqAp);
    MED_2MIC_SNPSDMIC_STRU              *pstSnPsdMc          = &(pstPostFilter->stSnPsdMc);
    MED_2MIC_SNPSDMIC_STRU              *pstSnPsdRc          = &(pstPostFilter->stSnPsdRc);
    MED_2MIC_PROBSPABSEN_STRU           *pstProbSpAbsen      = &(pstPostFilter->stProbSpAbsen);
    MED_2MIC_MODFYPROBSPABSEN_STRU      *pstModfyProbSpAbsen = &(pstPostFilter->stModfyProbSpAbsen);
    MED_2MIC_PROBSPPRESEN_STRU          *pstProbSpPresen     = &(pstPostFilter->stProbSpPresen);
    MED_2MIC_LSAGAIN_STRU               *pstLsaGain          = &(pstPostFilter->stLsaGain);
    VOS_INT16                            shwBandLen;
    VOS_INT16                            shwCntI;

    /* �쳣��μ�� */
    if (   (MED_NULL == pst2MICParam)
        || (enSampleRate >= CODEC_SAMPLE_RATE_MODE_BUTT)
        || (shwFftLen <= 0))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* ��2MIC�ṹ���ȳ�ʼ��Ϊ0 */
    UCOM_MemSet(pstPostFilter, 0, sizeof(MED_2MIC_POSTFILTER_STRU));

    /* �����׳��� */
    pstPostFilter->shwFreqBinLen                  = (shwFftLen/2) + 1;

    /* ��һ֡���źŹ����׺�����������*/
    pstPostFilter->enFirstFrameInitFlag = CODEC_SWITCH_OFF;

    /* ����ƽ��������ʱ*/
    pstPostFilter->shwTimeAlpha = pst2MICParam->shwTimeAlpha;

    /* Զ���ź����ޣ���������������FD���ο��ź����� */
    pstPostFilter->shwFarMaxThd = pst2MICParam->shwFarMaxThd2;

    /* խ��ʱ�������׻���Ϊ32���Ӵ��������Ϊ40���Ӵ� */
    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        /* խ��ʱ�������׻���Ϊ32���Ӵ� */
        pstPostFilter->shwBandLen                 = MED_2MIC_NUM_CHAN_8k;

        /* ����Ƶ�׵ĵʹ��͸ߴ� */
        pstProbSpAbsen->shwSpBandLow              = MED_2MIC_SP_LO_BAND_8k;
        pstProbSpAbsen->shwSpBandHigh             = MED_2MIC_SP_HI_BAND_8k;

        pstProbSpPresen->shwTmosExitBandLow       = MED_2MIC_TMOS_EXIT_LOWBIN_8k;
        pstProbSpPresen->shwTmosExitBandHi        = MED_2MIC_TMOS_EXIT_HIGHBIN_8k;

        pstModfyProbSpAbsen->shwDifIndex          = MED_2MIC_Mid_BAND_8k;
    }
    else
    {
        /* ���ʱ�������׻���Ϊ40���Ӵ� */
        pstPostFilter->shwBandLen                 = MED_2MIC_NUM_CHAN_16k;

        /* ����Ƶ�׵ĵʹ��͸ߴ� */
        pstProbSpAbsen->shwSpBandLow              = MED_2MIC_SP_LO_BAND_16k;
        pstProbSpAbsen->shwSpBandHigh             = MED_2MIC_SP_HI_BAND_16k;

        pstProbSpPresen->shwTmosExitBandLow       = MED_2MIC_TMOS_EXIT_LOWBIN_16k;
        pstProbSpPresen->shwTmosExitBandHi        = MED_2MIC_TMOS_EXIT_HIGHBIN_16k;

        pstModfyProbSpAbsen->shwDifIndex          = MED_2MIC_Mid_BAND_16k;
    }
    shwBandLen = pstPostFilter->shwBandLen;

    /* Ƶ�첹��ģ���ʼ�� */
    pstCompFreqAp->pshwCompCoef                   = (VOS_INT16 *)MED_2MIC_GetCompCoefPtr();
    pstCompFreqAp->shwSampleRate                  = (VOS_INT16)enSampleRate;

    /* ��ʼ��������˹����� */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pstSnPsdMc->aswMinPsd[shwCntI] = MED_2MIC_SNPSD_MAX;
        pstSnPsdMc->aswTmpPsd[shwCntI] = MED_2MIC_SNPSD_MAX;

        pstSnPsdRc->aswMinPsd[shwCntI] = MED_2MIC_SNPSD_MAX;
        pstSnPsdRc->aswTmpPsd[shwCntI] = MED_2MIC_SNPSD_MAX;
    }

    /* ȫ���������ڸ�����ֵ */
    pstProbSpAbsen->shwSumProbThld = pst2MICParam->shwSumProbThld;


    /* �������������ڸ���ģ���ʼ�� */
    pstModfyProbSpAbsen->shwPowerRatioLowLast     = MED_2MIC_MODFY_ABSEN_RATIO_INIT;
    pstModfyProbSpAbsen->shwPowerRatioHighLast    = MED_2MIC_MODFY_ABSEN_RATIO_INIT;
    pstModfyProbSpAbsen->shwFarMuteCnt            = MED_2MIC_MODFY_ABSEN_FAR_MUTE_INIT;

    pstModfyProbSpAbsen->shwFarMuteCntThd         = pst2MICParam->shwFarMuteCntThd;

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pstModfyProbSpAbsen->ashwPsdRatioLast[shwCntI] = MED_2MIC_MODFY_ABSEN_RATIO_INIT;
    }

    pstModfyProbSpAbsen->pshwPsdRatioFreqAlpha    = (VOS_INT16 *)MED_2MIC_GetPsdRatioFreqAlphaPtr();
    pstModfyProbSpAbsen->shwFreqSmoothLen         = MED_2MIC_MODFY_FREQ_SMOOTH_LEN;

    /* �����������ڸ���ģ���ʼ�� */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pstProbSpPresen->aswPostSNR[shwCntI] = MED_2MIC_SPPRENSEN_POSTSNR_INIT;
    }

    pstProbSpPresen->shwSpLowTh = pst2MICParam->shwSpLowTh;
    pstProbSpPresen->shwNsLowTh = pst2MICParam->shwNsLowTh;
    pstProbSpPresen->shwNsHighTh = pst2MICParam->shwNsHighTh;

    /* ��������ģ���ʼ�� */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pstPostFilter->ashwLsaGain[shwCntI] = CODEC_OP_INT16_MAX;
        pstPostFilter->ashwOmLsaGain[shwCntI] = CODEC_OP_INT16_MAX;
    }

    pstLsaGain->shwBandLow                        = MED_2MIC_LSA_LOW_BAND;
    pstLsaGain->shwGainMin                        = pst2MICParam->shwGainMin;
    pstLsaGain->pshwBandAlpha                     = (VOS_INT16 *)MED_2MIC_GetBandAlphaPtr();
    pstLsaGain->shwSmoothLen                      = MED_2MIC_LSAGAIN_FREQ_SMOOTH_LEN;

    return UCOM_RET_SUCC;
}

VOS_UINT32 MED_ANR_2MIC_FdProcess(
                CODEC_SWITCH_ENUM_UINT16           enEnable,
                MED_2MIC_DATAFLOW_MC_STRU         *pstDataFlowMc,
                MED_2MIC_DATAFLOW_RC_STRU         *pstDataFlowRc,
                VOS_INT16                          shwMaxFar)
{
    VOS_INT32                 *pswPsdBinMc        = MED_NULL;
    VOS_INT32                 *pswPsdBinRc        = MED_NULL;
    VOS_INT32                 *pswPsdBinOriRc     = MED_NULL;

    VOS_INT32                  aswBandMc[MED_2MIC_NUM_CHAN_16k];
    VOS_INT32                  aswBandRc[MED_2MIC_NUM_CHAN_16k];

    MED_2MIC_POSTFILTER_STRU  *pstPostFilter      =  MED_2MIC_GetFdProcessPtr();
    VOS_INT16                  shwFreqBinLen      =  pstPostFilter->shwFreqBinLen;
    MED_2MIC_FREQ_STRU        *pstMc              = &(pstDataFlowMc->stMc);
    MED_2MIC_FREQ_STRU        *pstRc              = &(pstDataFlowRc->stRc);
    MED_2MIC_FREQ_STRU        *pstOriRc           = &(pstDataFlowRc->stOriRc);

    MED_2MIC_COMPFREQAP_STRU  *pstCompFreqAp      = &(pstPostFilter->stCompFreqAp);
    VOS_INT16                  shwBandLen         =  pstPostFilter->shwBandLen;

    VOS_INT16                  shwTimeAlpha       =  pstPostFilter->shwTimeAlpha;
    VOS_INT32                 *pswBandSY          =  pstPostFilter->aswBandSY;
    VOS_INT32                 *pswBandSU          =  pstPostFilter->aswBandSU;
    VOS_INT32                 *pswBandMY          =  pstPostFilter->aswBandMY;
    VOS_INT32                 *pswBandMU          =  pstPostFilter->aswBandMU;
    MED_2MIC_SNPSDMIC_STRU    *pstSnPsdMc         = &(pstPostFilter->stSnPsdMc);
    MED_2MIC_SNPSDMIC_STRU    *pstSnPsdRc         = &(pstPostFilter->stSnPsdRc);

    VOS_INT16                  ashwEstiProb[MED_2MIC_NUM_CHAN_16k];
    MED_2MIC_PROBSPABSEN_STRU *pstProbSpAbsen     = &(pstPostFilter->stProbSpAbsen);
    VOS_INT16                 *pshwProbSpAbsen    =  pstPostFilter->ashwProbSpAbsen;

    MED_2MIC_MODFYPROBSPABSEN_STRU      *pstModfyProbSpAbsen
                                                  = &(pstPostFilter->stModfyProbSpAbsen);

    VOS_INT16                 *pshwLsaGain        =  pstPostFilter->ashwLsaGain;
    MED_2MIC_PROBSPPRESEN_STRU          *pstProbSpPresen
                                                  = &(pstPostFilter->stProbSpPresen);
    VOS_INT16                 *pshwProbSpPresen   =  pstPostFilter->ashwProbSpPresen;
    VOS_INT16                  ashwSNRFactor[MED_2MIC_NUM_CHAN_16k];

    VOS_INT32                 *pswPrioriSNR       =  pstProbSpPresen->aswPrioriSNR;
    MED_2MIC_LSAGAIN_STRU     *pstLsaGain         = &(pstPostFilter->stLsaGain);
    VOS_INT16                 *pshwOmLsaGain      =  pstPostFilter->ashwOmLsaGain;

    VOS_INT16                 *pshwFreqBinGain    = MED_NULL;

    /* �쳣��μ�� */
    if (   (MED_NULL == pstDataFlowMc)
        || (MED_NULL == pstDataFlowRc))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* ���ANR��ʹ�� */
    if (CODEC_SWITCH_OFF == enEnable)
    {
        return UCOM_RET_SUCC;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_ANR_2MIC_FdProcess);

    /* �ڴ����� */
    pswPsdBinMc     = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBinRc     = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pswPsdBinOriRc  = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));
    pshwFreqBinGain = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT16));

    if (shwMaxFar > pstPostFilter->shwFarMaxThd)
    {
        /* ����л���������Mic�ź���0�����⽫����ѧ������ */
        CODEC_OpVecSet(
                    pstRc->ashwFreq,
                    shwFreqBinLen,
                    0);
        CODEC_OpVecSet(
                    pstOriRc->ashwFreq,
                    shwFreqBinLen,
                    0);
    }

    /* ����ÿ�������� */
    MED_2MIC_CalcPsdBin(pstMc, shwFreqBinLen, pswPsdBinMc);
    MED_2MIC_CalcPsdBin(pstRc, shwFreqBinLen, pswPsdBinRc);
    MED_2MIC_CalcPsdBin(pstOriRc, shwFreqBinLen, pswPsdBinOriRc);

    /* �Ը�����е����������׽������� */
    MED_2MIC_CompFreqAp(pswPsdBinMc,
                        pswPsdBinOriRc,
                        shwFreqBinLen,
                        pstCompFreqAp,
                        pswPsdBinRc);

    /* Ƶ���Ӵ��ϲ�������ÿ���Ӵ��Ĺ����� */
    MED_2MIC_CalcBandPsd(pswPsdBinMc, shwBandLen, aswBandMc);
    MED_2MIC_CalcBandPsd(pswPsdBinRc, shwBandLen, aswBandRc);

    /* ��һ֡��ʼ�� */
    if (CODEC_SWITCH_OFF == pstPostFilter->enFirstFrameInitFlag)
    {
        MED_2MIC_FreqProcessFirstFrame(aswBandMc, aswBandRc, pstPostFilter);
        pstPostFilter->enFirstFrameInitFlag = CODEC_SWITCH_ON;
    }

    /* ����������˸��Ӵ���ƽ�������� */
    MED_2MIC_CalcSmoothPsd(aswBandMc, shwBandLen, shwTimeAlpha, pswBandSY);
    MED_2MIC_CalcSmoothPsd(aswBandRc, shwBandLen, shwTimeAlpha, pswBandSU);

    /* ����������˸��Ӵ������������� */
    MED_2MIC_CalcSnPsd(pswBandSY,
                       aswBandMc,
                       shwBandLen,
                       pswBandMY,
                       pstSnPsdMc);
    MED_2MIC_CalcSnPsd(pswBandSU,
                       aswBandRc,
                       shwBandLen,
                       pswBandMU,
                       pstSnPsdRc);

    /* ���Ƹ��Ӵ��������ڸ���*/
    MED_2MIC_EstiProbSpPresen(pswBandSY,
                              pswBandSU,
                              pswBandMY,
                              pswBandMU,
                              shwBandLen,
                              ashwEstiProb);

    /* ��������˸��Ӵ������������ڸ���    */
    MED_2MIC_CalcProbSpAbsen(ashwEstiProb,
                             aswBandMc,
                             pswBandMY,
                             shwBandLen,
                             pstProbSpAbsen,
                             pshwProbSpAbsen);

    /* ��������˸��Ӵ������������ڸ��� */
    MED_2MIC_ModifyProbSpAbsen(pswBandSY,
                               pswBandSU,
                               shwBandLen,
                               aswBandMc,
                               aswBandRc,
                               pshwProbSpAbsen,
                               pstModfyProbSpAbsen);

    /* ��������˸��Ӵ����������ڸ��� */
    MED_2MIC_CalcProbSpPresen(aswBandMc,
                              pshwProbSpAbsen,
                              pshwLsaGain,
                              shwBandLen,
                              pstProbSpPresen,
                              pshwProbSpPresen,
                              ashwSNRFactor);

    /* ��������˸��Ӵ������� */
    MED_2MIC_CalcLsaGain(pswPrioriSNR,
                         ashwSNRFactor,
                         pshwProbSpPresen,
                         pstLsaGain,
                         shwBandLen,
                         pshwOmLsaGain,
                         pshwLsaGain);

    /* ���Ӵ����䵽����Ƶ�״� */
    MED_2MIC_ExtendBand2FreqBinGain(aswBandMc,
                                    pshwOmLsaGain,
                                    shwFreqBinLen,
                                    shwBandLen,
                                    pshwFreqBinGain,
                                    pstProbSpPresen,
                                    pshwProbSpPresen);

    /* ������˸�Ƶ��ʩ���������������� */
    MED_2MIC_FilterFrm(shwFreqBinLen,
                       pshwFreqBinGain,
                       pstMc);

    /* �ڴ��ͷ� */
    UCOM_MemFree(pswPsdBinMc);
    UCOM_MemFree(pswPsdBinRc);
    UCOM_MemFree(pswPsdBinOriRc);
    UCOM_MemFree(pshwFreqBinGain);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_ANR_2MIC_FdProcess);

    return UCOM_RET_SUCC;
}
#ifndef _MED_C89_

VOS_VOID MED_2MIC_CalcPsdBin(
                MED_2MIC_FREQ_STRU      *pstFreq,
                VOS_INT16                shwFreqBinLen,
                VOS_INT32               *pswPsdBin)
{
    VOS_INT16  shwCntK;
    VOS_INT16  shwNormQ8;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1, aeqS2;

    VOS_INT16 *pshwFreq     = pstFreq->ashwFreq;
    VOS_INT16  shwNormShift = pstFreq->shwNormShift;

    /* �����׼���  */
    UCOM_MemSet(pswPsdBin, 0, (VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));

    /* �����һ�������׺����һ�������� */
    pswPsdBin[0] = CODEC_OpL_mult(pshwFreq[0], pshwFreq[0]);
    pswPsdBin[shwFreqBinLen - 1] = CODEC_OpL_mult(pshwFreq[1], pshwFreq[1]);

    /* �������������� */
    for (shwCntK = 1; shwCntK < (shwFreqBinLen - 1)>>1; shwCntK++)
    {
        aepX1 = *((ae_p16x2s *)&pshwFreq[4 * shwCntK - 2]);
        aepX2 = *((ae_p16x2s *)&pshwFreq[4 * shwCntK - 2]);

        aepY1 = *((ae_p16x2s *)&pshwFreq[4 * shwCntK]);
        aepY2 = *((ae_p16x2s *)&pshwFreq[4 * shwCntK]);

        aeqS1 = AE_MULZAAFP24S_HH_LL(aepX1, aepX2);
        aeqS2 = AE_MULZAAFP24S_HH_LL(aepY1, aepY2);

        /* ���͵�48bit */
        aeqS1 = AE_SATQ48S(aeqS1);
        aeqS2 = AE_SATQ48S(aeqS2);

        /* aeqS1[bit9 - bit47] -> pswPsdBin */
        pswPsdBin[2 * shwCntK - 1] = AE_TRUNCA32Q48(aeqS1);
        pswPsdBin[2 * shwCntK] = AE_TRUNCA32Q48(aeqS2);
    }

    aepX1 = *((ae_p16x2s *)&pshwFreq[2*(shwFreqBinLen - 2)]);
    aepX2 = *((ae_p16x2s *)&pshwFreq[2*(shwFreqBinLen - 2)]);

    aeqS1 = AE_MULZAAFP24S_HH_LL(aepX1, aepX2);
    aeqS1 = AE_SATQ48S(aeqS1);

    pswPsdBin[shwFreqBinLen - 2] = AE_TRUNCA32Q48(aeqS1);

    /* �������׹�һ����Q8 */
    shwNormQ8 = (2 * shwNormShift) - MED_PP_SHIFT_BY_7;
    /*CODEC_OpVecShr_r32(pswPsdBin,
                   shwFreqBinLen,
                   shwNormQ8,
                   pswPsdBin);*/
    CODEC_OpVecShr32(pswPsdBin,
        shwFreqBinLen,
        shwNormQ8,
        pswPsdBin);
}
#else

VOS_VOID MED_2MIC_CalcPsdBin(
                MED_2MIC_FREQ_STRU      *pstFreq,
                VOS_INT16                shwFreqBinLen,
                VOS_INT32               *pswPsdBin)
{
    VOS_INT16  shwCntK;
    VOS_INT16  shwNormQ8;

    VOS_INT16 *pshwFreq     = pstFreq->ashwFreq;
    VOS_INT16  shwNormShift = pstFreq->shwNormShift;

    /* �����׼���  */
    UCOM_MemSet(pswPsdBin, 0, (VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT32));

    /* �����һ�������׺����һ�������� */
    pswPsdBin[0] = CODEC_OpL_mult(pshwFreq[0], pshwFreq[0]);
    pswPsdBin[shwFreqBinLen - 1] = CODEC_OpL_mult(pshwFreq[1], pshwFreq[1]);

    /* �������������� */
    for (shwCntK = 1; shwCntK < (shwFreqBinLen - 1); shwCntK++)
    {
        pswPsdBin[shwCntK] = CODEC_OpL_mac(pswPsdBin[shwCntK],
                                   pshwFreq[2 * shwCntK],
                                   pshwFreq[2 * shwCntK]);
        pswPsdBin[shwCntK] = CODEC_OpL_mac(pswPsdBin[shwCntK],
                                   pshwFreq[(2 * shwCntK) + 1],
                                   pshwFreq[(2 * shwCntK) + 1]);
    }

    /* �������׹�һ����Q8 */
    shwNormQ8 = (VOS_INT16)(2 * shwNormShift) - MED_PP_SHIFT_BY_7;
    /*CODEC_OpVecShr_r32(pswPsdBin,
                   shwFreqBinLen,
                   shwNormQ8,
                   pswPsdBin);*/
    CODEC_OpVecShr32(pswPsdBin,
        shwFreqBinLen,
        shwNormQ8,
        pswPsdBin);
}
#endif

VOS_VOID MED_2MIC_CompFreqAp(
                VOS_INT32                         *pswPsdBinMc,
                VOS_INT32                         *pswPsdBinOriRc,
                VOS_INT16                          shwFreqBinLen,
                MED_2MIC_COMPFREQAP_STRU          *pstCompFreqAp,
                VOS_INT32                         *pswPsdBinRc)
{
    VOS_INT16  shwCntI;
    VOS_INT32  swTemp1;
    VOS_INT32  swTemp2;
    VOS_INT16  shwTemp;

    VOS_INT16  shwSampleRate = pstCompFreqAp->shwSampleRate;
    VOS_INT16 *pshwCompCoef  = pstCompFreqAp->pshwCompCoef;

    /* ��ƵУ�� */
    for (shwCntI = (MED_2MIC_COMP_FREQ_LOW - 1); shwCntI < MED_2MIC_COMP_FREQ_HIGH; shwCntI++)
    {
        /* Ϊ�˺����ж��Ƿ���Ҫ���� */
        swTemp1 = CODEC_OpL_mpy_32_16(pswPsdBinOriRc[shwCntI],
                                    MED_2MIC_COMP_FREQ_THD1);
        swTemp1 = CODEC_OpL_shl(swTemp1, MED_PP_SHIFT_BY_2);
        swTemp2 = CODEC_OpL_mpy_32_16(pswPsdBinOriRc[shwCntI],
                                    MED_2MIC_COMP_FREQ_THD2);

        /* �Ա����Ƶ��ĸ������������������ */
        if ( (pswPsdBinMc[shwCntI] < swTemp1)
          && (pswPsdBinRc[shwCntI] < swTemp2) )
        {
            pswPsdBinRc[shwCntI] = pswPsdBinOriRc[shwCntI];
        }
     }

    /* ��ƵƵ�첹�� */
    if (CODEC_SAMPLE_RATE_MODE_16000 == shwSampleRate)
    {
        for (shwCntI = 0; shwCntI < shwFreqBinLen; shwCntI++)
        {
            shwTemp = CODEC_OpMult(pshwCompCoef[shwCntI],
                                 pshwCompCoef[shwCntI]);
            pswPsdBinRc[shwCntI] = CODEC_OpL_mpy_32_16(pswPsdBinRc[shwCntI],
                                                     shwTemp);
            pswPsdBinRc[shwCntI] = CODEC_OpL_shl(pswPsdBinRc[shwCntI],
                                               MED_PP_SHIFT_BY_4);
        }
    }
}


VOS_VOID MED_2MIC_CalcBandPsd(
                 VOS_INT32               *pswPsdBin,
                 VOS_INT16                shwBandLen,
                 VOS_INT32               *pswBandPsd)
{
    VOS_INT16 shwCntI;
    VOS_INT16 shwCntJ;

    VOS_INT16 shwChLo;
    VOS_INT16 shwChHi;

    /* �Ӵ������� */
    UCOM_MemSet(pswBandPsd, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT32));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* �Ӵ���Χ */
        shwChLo = g_ashwMedAnr2MicChTbl[shwCntI][0];
        shwChHi = g_ashwMedAnr2MicChTbl[shwCntI][1];

        /* �Ӵ���Χ�ڵĹ�������� */
        for (shwCntJ = (shwChLo - 1); shwCntJ < shwChHi; shwCntJ++)
        {
            pswBandPsd[shwCntI] = CODEC_OpL_add(pswBandPsd[shwCntI],
                                              pswPsdBin[shwCntJ]);
        }

        /* �Ӵ��ڹ�����ȡƽ�� */
        pswBandPsd[shwCntI] /= ((shwChHi - shwChLo) + 1);

        /* �޶���Сֵ */
        pswBandPsd[shwCntI] = CODEC_OpL_max(pswBandPsd[shwCntI],
                                          MED_2MIC_MIN_BIN_ENRG);
    }
}
VOS_VOID MED_2MIC_FreqProcessFirstFrame(
                VOS_INT32                         *pswBandMc,
                VOS_INT32                         *pswBandRc,
                MED_2MIC_POSTFILTER_STRU          *pstPostFilter)
{
    /* �����ƽ���źŹ����� */
    UCOM_MemCpy(pstPostFilter->aswBandSY,
                pswBandMc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

    /* �����ƽ������������ */
    UCOM_MemCpy(pstPostFilter->aswBandMY,
                pswBandMc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

    /* ���������� */
    UCOM_MemCpy(pstPostFilter->stProbSpPresen.aswNsPsd,
                pswBandMc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

     /* ������źŹ����� */
    UCOM_MemCpy(pstPostFilter->aswBandSU,
                pswBandRc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

    /* �����ƽ������������ */
    UCOM_MemCpy(pstPostFilter->aswBandMU,
                pswBandRc,
                MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT32));

}
VOS_VOID MED_2MIC_CalcSmoothPsd(
                VOS_INT32               *pswBandPsd,
                VOS_INT16                shwBandLen,
                VOS_INT16                shwTimeAlpha,
                VOS_INT32               *pswSmoothPsd)
{
    VOS_INT16 shwCntI;
    VOS_INT16 shwSubAlpha;

    shwSubAlpha = CODEC_OP_INT16_MAX - shwTimeAlpha;
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* ֡��ƽ�� */
        pswSmoothPsd[shwCntI] = CODEC_OpLSmooth(pswSmoothPsd[shwCntI],
                                              shwTimeAlpha,
                                              pswBandPsd[shwCntI],
                                              shwSubAlpha);

        /* �������ޣ���ֹ��0 */
        pswSmoothPsd[shwCntI] = CODEC_OpL_max(pswSmoothPsd[shwCntI],
                                           MED_2MIC_MIN_BIN_ENRG);
    }
}


VOS_VOID MED_2MIC_CalcSnPsd(
                VOS_INT32               *pswSmoothPsd,
                VOS_INT32               *pswCurrPsd,
                VOS_INT16                shwBandLen,
                VOS_INT32               *pswSnPsd,
                MED_2MIC_SNPSDMIC_STRU  *pstSnPsdMic)
{
    VOS_INT16 shwCntI;
    VOS_INT32 swPsdThd;
    VOS_INT16 shwTmpAlpha;
    VOS_INT16 ashwSpFlag[MED_2MIC_NUM_CHAN_16k];

    VOS_INT16  shwFrameNum      = pstSnPsdMic->shwFrameNum;
    VOS_INT32 *pswMinPsd        = pstSnPsdMic->aswMinPsd;
    VOS_INT32 *pswTmpPsd        = pstSnPsdMic->aswTmpPsd;
    VOS_INT16 *pshwProbSpPresen = pstSnPsdMic->ashwProbSpPresen;

    /* ���¹��Ƶ���С������ Ŀǰ�㷨�ĸ����ٶ�Ϊ50֡ */
    if (0 != (shwFrameNum % MED_2MIC_SNPSD_STATE_LEN))
    {
        /* ���ٵ�ǰ50֡�ڵ���С������ */
        for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
        {
            pswMinPsd[shwCntI] = CODEC_OpL_min(pswMinPsd[shwCntI],
                                             pswSmoothPsd[shwCntI]);
            pswTmpPsd[shwCntI] = CODEC_OpL_min(pswTmpPsd[shwCntI],
                                             pswSmoothPsd[shwCntI]);
        }
    }
    else
    {
        /* ����50֡��������С������ */
        for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
        {
            pswMinPsd[shwCntI] = CODEC_OpL_min(pswTmpPsd[shwCntI],
                                             pswSmoothPsd[shwCntI]);
            pswTmpPsd[shwCntI] = pswSmoothPsd[shwCntI];
        }
    }

    /* ��¼���ٵ�֡�� */
    pstSnPsdMic->shwFrameNum = CODEC_OpAdd(pstSnPsdMic->shwFrameNum, 1);

    UCOM_MemSet(ashwSpFlag, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    /* ����ÿ���Ӵ����������ڸ��� */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        swPsdThd = CODEC_OpL_mpy_32_16(pswMinPsd[shwCntI], MED_2MIC_SNPSD_PROB_THD);
        swPsdThd = CODEC_OpL_shl(swPsdThd, MED_PP_SHIFT_BY_2);

        /* ����Ƚϴ� */
        if (pswCurrPsd[shwCntI] > swPsdThd)
        {
            ashwSpFlag[shwCntI] = CODEC_OP_INT16_MAX;
        }
    }

    /* �����������ڸ��� */
    CODEC_OpVvFormWithCoef(pshwProbSpPresen,
                         ashwSpFlag,
                         shwBandLen,
                         MED_2MIC_SNPSD_PROB_ALPHA,
                         MED_2MIC_SNPSD_PROB_SUB_ALPHA,
                         pshwProbSpPresen);

    /* ����ƽ������������ */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* MED_2MIC_SNPSD_ALPHA����ƽ�����������׵ĸ����ٶȣ�Խ�����Խ�� */
        shwTmpAlpha = CODEC_OpAdd(MED_2MIC_SNPSD_ALPHA,
                                CODEC_OpMult(MED_2MIC_SNPSD_SUB_ALPHA,
                                           pshwProbSpPresen[shwCntI]));

        /* ʱ��ƽ�� */
        pswSnPsd[shwCntI] = CODEC_OpLSmooth(pswSnPsd[shwCntI],
                                          shwTmpAlpha,
                                          pswCurrPsd[shwCntI],
                                         (CODEC_OP_INT16_MAX - shwTmpAlpha));

        /* ������Сֵ����ֹ�����0 */
        pswSnPsd[shwCntI] = CODEC_OpL_max(pswSnPsd[shwCntI],
                                        MED_2MIC_MIN_BIN_ENRG);
    }
}
VOS_VOID MED_2MIC_EstiProbSpPresen(
                VOS_INT32                         *pswBandSY,
                VOS_INT32                         *pswBandSU,
                VOS_INT32                         *pswBandMY,
                VOS_INT32                         *pswBandMU,
                VOS_INT16                          shwBandLen,
                VOS_INT16                         *pshwProbSpPresen)
{
    VOS_INT16 shwCntK;
    VOS_INT16 ashwTransRatio[MED_2MIC_NUM_CHAN_16k];
    VOS_INT32 swTemp1;
    VOS_INT32 swTemp2;
    VOS_INT32 swPsdMcThd;
    VOS_INT32 swPsdRcThd;

    /* ����������˾ֲ���ƽ���Ժ��������˲ʱ�仯�� */
    UCOM_MemSet(ashwTransRatio, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCntK = 0; shwCntK < shwBandLen; shwCntK++)
    {
        swTemp1 = CODEC_OpL_sub(pswBandSY[shwCntK], pswBandMY[shwCntK]);
        swTemp2 = CODEC_OpL_sub(pswBandSU[shwCntK], pswBandMU[shwCntK]);

        /* ���Ʒ�ĸ����Сֵ����ֹ��0 */
        swTemp2 = CODEC_OpL_max(swTemp2, MED_2MIC_MIN_BIN_ENRG);

        /* �������ʱ����Ⱥܵͣ��������ڸ���Ϊ0�����ҿ��Խ��ͳ����ж������ļ����� */
        if ((swTemp1 <= 0)
         || (swTemp2 < 0))
         {
             ashwTransRatio[shwCntK] = 0;
         }
         else
         {
             ashwTransRatio[shwCntK] = CODEC_OpRound(CODEC_OpNormDiv_32(swTemp1,
                                                                    swTemp2,
                                                                    MED_PP_SHIFT_BY_29));
         }
    }

    /* ���Ƹ��Ӵ��������ڸ��� */
    UCOM_MemSet(pshwProbSpPresen, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCntK = 0; shwCntK < shwBandLen; shwCntK++)
    {
        swPsdMcThd = CODEC_OpL_mpy_32_16(pswBandMY[shwCntK],
                                       MED_2MIC_ABSEN_LNS_RATIO_THD);
        swPsdMcThd = CODEC_OpL_shl(swPsdMcThd,
                                 MED_PP_SHIFT_BY_2);
        swPsdRcThd = CODEC_OpL_mpy_32_16(pswBandMU[shwCntK],
                                       MED_2MIC_ABSEN_LNS_RATIO_THD);
        swPsdRcThd = CODEC_OpL_shl(swPsdRcThd,
                                 MED_PP_SHIFT_BY_2);

        /* ����������С����Ϊ���� */
        if (pswBandSY[shwCntK] <= swPsdMcThd)
        {
            pshwProbSpPresen[shwCntK] = 0;
        }
        /* ����������С����Ϊ���� */
        else if (pswBandSU[shwCntK] <= swPsdRcThd)
        {
            pshwProbSpPresen[shwCntK] = MED_2MIC_SPABSEN_Q13;
        }
        /* �����������˲ʱ�仯�������� */
        else
        {
            if (ashwTransRatio[shwCntK] < MED_2MIC_ABSEN_TRANS_RATIO_LOW)
            {
                pshwProbSpPresen[shwCntK] = 0;
            }
            else if (ashwTransRatio[shwCntK] > MED_2MIC_ABSEN_TRANS_RATIO_HIGH)
            {
                pshwProbSpPresen[shwCntK] = MED_2MIC_SPABSEN_Q13;
            }
            else
            {
                pshwProbSpPresen[shwCntK] = CODEC_OpShr(CODEC_OpSub(ashwTransRatio[shwCntK],
                                                                MED_2MIC_ABSEN_TRANS_RATIO_LOW),
                                                      MED_PP_SHIFT_BY_1);
            }
        }
    }
}
VOS_VOID MED_2MIC_CalcProbSpAbsen(
                VOS_INT16                         *pshwProbSpPresen,
                VOS_INT32                         *pswBandPsd,
                VOS_INT32                         *pswBandMY,
                VOS_INT16                          shwBandLen,
                MED_2MIC_PROBSPABSEN_STRU         *pstProbSpAbsen,
                VOS_INT16                         *pshwProbSpAbsen)
{
    VOS_INT16 shwCntK;
    VOS_INT16 shwSumProbThld;
    VOS_INT16 shwSpBandLow;
    VOS_INT16 shwSpBandHigh;
    VOS_INT16 shwSumProb;
    VOS_INT16 ashwPostSNR[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16 shwTemp1;
    VOS_INT16 shwTemp2;

    /* ȫ�������������ж���ֵ */
    shwSumProbThld = pstProbSpAbsen->shwSumProbThld;

    /* �������ڷ�Χ������Ӵ�������Ӵ� */
    shwSpBandLow  = pstProbSpAbsen->shwSpBandLow;
    shwSpBandHigh = pstProbSpAbsen->shwSpBandHigh;



    /* ����ȫ���������ڸ��� */
    shwSumProb = 0;
    for (shwCntK = (shwSpBandLow - 1); shwCntK < shwSpBandHigh; shwCntK++)
    {
        shwSumProb = CODEC_OpAdd(shwSumProb, pshwProbSpPresen[shwCntK]);
    }

    /* ��ȻshwSumProb����Ϊ[16bit Q15]����������ǲ���Ӱ�������жϡ� */
    shwSumProb =  CODEC_OpSaturate(CODEC_OpNormDiv_32((VOS_INT32)shwSumProb,
                                              (VOS_INT32)((shwSpBandHigh - shwSpBandLow) + 1),
                                              MED_PP_SHIFT_BY_2));

    /* ������Ӵ����������ڸ��� */
    UCOM_MemSet(ashwPostSNR, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    /* ���������Ŀ����Խϴ� */
    if (shwSumProb > shwSumProbThld)
    {
        for (shwCntK = 0; shwCntK < shwBandLen; shwCntK++)
        {
            ashwPostSNR[shwCntK] = CODEC_OpRound(CODEC_OpNormDiv_32(pswBandPsd[shwCntK],
                                                                pswBandMY[shwCntK],
                                               MED_PP_SHIFT_BY_28));
          /* ���������С������������ */
          if (ashwPostSNR[shwCntK] <= MED_2MIC_ABSEN_POST_SNR_THLD)
          {
              pshwProbSpAbsen[shwCntK] = CODEC_OP_INT16_MAX;
          }
          else
          {
              shwTemp1 = CODEC_OpSub(MED_2MIC_ABSEN_POST_SNR_ALPHA,
                                   ashwPostSNR[shwCntK]);

              /* �����������ڸ��ʹ������������ڸ���*/
              pshwProbSpAbsen[shwCntK] = CODEC_OpSub(CODEC_OP_INT16_MAX,
                                                   CODEC_OpShl(pshwProbSpPresen[shwCntK],
                                                             MED_PP_SHIFT_BY_2));
              if (shwTemp1 > 0)
              {
                  /* ���ú�������ȹ������������ڸ��� */
                  shwTemp2 = CODEC_OpSaturate(CODEC_OpNormDiv_32((VOS_INT32)shwTemp1,
                                                         (VOS_INT32)MED_2MIC_ABSEN_POST_SNR_SUB_ALPHA,
                                                         MED_PP_SHIFT_BY_15));
                  pshwProbSpAbsen[shwCntK] = CODEC_OpMax(shwTemp2,
                                                       pshwProbSpAbsen[shwCntK]);
              }
          }
        }
    }
    /* ����֡ */
    else
    {
        for (shwCntK = 0; shwCntK < shwBandLen; shwCntK++)
        {
            pshwProbSpAbsen[shwCntK] = CODEC_OP_INT16_MAX;
        }
    }
}



VOS_VOID MED_2MIC_ModifyProbSpAbsen(
                VOS_INT32                         *pswBandSY,
                VOS_INT32                         *pswBandSU,
                VOS_INT16                          shwBandLen,
                VOS_INT32                         *pswPsdBinMc,
                VOS_INT32                         *pswPsdBinRc,
                VOS_INT16                         *pshwProbSpAbsence,
                MED_2MIC_MODFYPROBSPABSEN_STRU    *pstModifyProb)
{
    VOS_INT16  shwCntI;
    VOS_INT16  shwPowerRatioLowLast;
    VOS_INT16  shwPowerRatioHighLast;
    VOS_INT16 *pshwPsdRatioLast         = pstModifyProb->ashwPsdRatioLast;
    VOS_INT16 *pshwPsdRatioFreqAlpha    = pstModifyProb->pshwPsdRatioFreqAlpha;
    VOS_INT16  shwFreqSmoothLen;
    VOS_INT16  ashwMainToRefPsdRatio[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  ashwTmpMainToRefPsdRatio[(MED_2MIC_NUM_CHAN_16k + MED_2MIC_MODFY_FREQ_SMOOTH_LEN) - 1];
    VOS_INT16  shwHalfSmoothLen;
    VOS_INT16  shwSubAlpha;
    VOS_INT16  shwDifIndex;
    VOS_INT32  swSumMc;
    VOS_INT32  swSumRc;
    VOS_INT16  shwTemp;
    VOS_INT16  shwMainToRefPowerRatioLow;
    VOS_INT16  shwMainToRefPowerRatioHigh;
    VOS_INT16  ashwTmpPsdRatio[MED_2MIC_NUM_CHAN_16k];

    MED_AEC_OBJ_STRU *pstAECObj = MED_AEC_GetObjPtr();


    /* ��ȡ��ʷ��Ϣ */
    shwPowerRatioLowLast  =  pstModifyProb->shwPowerRatioLowLast;
    shwPowerRatioHighLast =  pstModifyProb->shwPowerRatioHighLast;
    pshwPsdRatioLast      =  pstModifyProb->ashwPsdRatioLast;

    pshwPsdRatioFreqAlpha =  pstModifyProb->pshwPsdRatioFreqAlpha;
    shwFreqSmoothLen      =  pstModifyProb->shwFreqSmoothLen;

    /* ��������Ӵ����������ƽ�������ױ��� */
    UCOM_MemSet(ashwMainToRefPsdRatio, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    UCOM_MemSet(ashwTmpPsdRatio, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        ashwMainToRefPsdRatio[shwCntI] = CODEC_OpRound(CODEC_OpNormDiv_32(pswBandSY[shwCntI],
                                                                       pswBandSU[shwCntI],
                                                                       MED_PP_SHIFT_BY_26));
    }

    /* ƽ��ϵ�����ȱ���Ϊ���� */
    UCOM_MemSet(ashwTmpMainToRefPsdRatio, 0,
                ((MED_2MIC_NUM_CHAN_16k + MED_2MIC_MODFY_FREQ_SMOOTH_LEN) - 1) * sizeof(VOS_INT16));

    shwHalfSmoothLen = CODEC_OpShr((shwFreqSmoothLen - 1), MED_PP_SHIFT_BY_1);

    /* ���뿪ͷ�ͽ�β��ΪƵ��ƽ����׼�� */
    for (shwCntI = 0; shwCntI < shwHalfSmoothLen; shwCntI++)
    {
        ashwTmpMainToRefPsdRatio[shwCntI] = ashwMainToRefPsdRatio[0];
        ashwTmpMainToRefPsdRatio[shwCntI + shwBandLen + shwHalfSmoothLen]
                                          = ashwMainToRefPsdRatio[shwBandLen - 1];
    }

    /* �����м�Ԫ�� */
    CODEC_OpVecCpy(&ashwTmpMainToRefPsdRatio[shwCntI], ashwMainToRefPsdRatio, shwBandLen);

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        shwTemp = 0;
        /* �Ӵ���ƽ�� */
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI],
                                                pshwPsdRatioFreqAlpha[0]));
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI + 1],
                                                pshwPsdRatioFreqAlpha[1]));
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI + 2],
                                                pshwPsdRatioFreqAlpha[2]));
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI + 3],
                                                pshwPsdRatioFreqAlpha[3]));
        shwTemp = CODEC_OpAdd(shwTemp, CODEC_OpMult(ashwTmpMainToRefPsdRatio[shwCntI + 4],
                                                pshwPsdRatioFreqAlpha[4]));
        ashwTmpPsdRatio[shwCntI] = shwTemp;

    }

    /* ֡��ƽ�� */
    shwSubAlpha = CODEC_OP_INT16_MAX - MED_2MIC_MODFY_ABSEN_PSD_RATIO_ALPHA;
    CODEC_OpVvFormWithCoef(pshwPsdRatioLast,
                         ashwTmpPsdRatio,
                         shwBandLen,
                         MED_2MIC_MODFY_ABSEN_PSD_RATIO_ALPHA,
                         shwSubAlpha,
                         ashwMainToRefPsdRatio);


    /* �ߵʹ��ֽ��ΪȫƵ�����м�ֵ */
    shwDifIndex = pstModifyProb->shwDifIndex;

    /* �����Ƶ�Ӵ�������������� */
    swSumMc = 0;
    swSumRc = 0;
    for (shwCntI = 0; shwCntI < shwDifIndex; shwCntI++)
    {
        swSumMc = CODEC_OpL_add(swSumMc, pswPsdBinMc[shwCntI]);
        swSumRc = CODEC_OpL_add(swSumRc, pswPsdBinRc[shwCntI]);
    }

    shwTemp = CODEC_OpMult(CODEC_OpRound(CODEC_OpNormDiv_32(swSumMc,
                                                       swSumRc,
                                                       MED_PP_SHIFT_BY_26)),
                         (CODEC_OP_INT16_MAX - MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA));
    shwMainToRefPowerRatioLow = CODEC_OpAdd(CODEC_OpMult(shwPowerRatioLowLast,
                                                     MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA),
                                          shwTemp);

    /* �����Ƶ�Ӵ�������������� */
    swSumMc = 0;
    swSumRc = 0;
    for (shwCntI = shwDifIndex; shwCntI < shwBandLen; shwCntI++)
    {
        swSumMc = CODEC_OpL_add(swSumMc, pswPsdBinMc[shwCntI]);
        swSumRc = CODEC_OpL_add(swSumRc, pswPsdBinRc[shwCntI]);
    }

    shwTemp = CODEC_OpMult(CODEC_OpRound(CODEC_OpNormDiv_32(swSumMc,
                                                       swSumRc,
                                                       MED_PP_SHIFT_BY_26)),
                         (CODEC_OP_INT16_MAX - MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA));
    shwMainToRefPowerRatioHigh = CODEC_OpAdd(CODEC_OpMult(shwPowerRatioHighLast,
                                                       MED_2MIC_MODFY_ABSEN_POWER_RATIO_ALPHA),
                                           shwTemp);

    /* �����������������ڸ��� */
    if (pstAECObj->stDtd.enDtdFlag < MED_AEC_DTD_FLAG_PASS)
    {
        pstModifyProb->shwFarMuteCnt = 0;
    }
    else
    {
        pstModifyProb->shwFarMuteCnt = pstModifyProb->shwFarMuteCnt + 1;
    }

    if (pstModifyProb->shwFarMuteCnt >= pstModifyProb->shwFarMuteCntThd)
    {
        pstModifyProb->shwFarMuteCnt = pstModifyProb->shwFarMuteCntThd;

        /* �ʹ�������������Ƚ�С�����ܴ������� */
        if (shwMainToRefPowerRatioLow < MED_2MIC_MODFY_ABSEN_POWER_RATIO_LOW)
        {
            for (shwCntI = 0; shwCntI < shwDifIndex; shwCntI++)
            {
                /* ��ǰ�Ӵ���������˹����ױ�С������Ϊ���� */
                if (ashwMainToRefPsdRatio[shwCntI] < MED_2MIC_MODFY_ABSEN_PSD_RATIO_LOW)
                {
                    pshwProbSpAbsence[shwCntI] = CODEC_OP_INT16_MAX;
                }
            }
        }

        /* �ߴ�������������Ƚ�С�����ܴ������� */
        if (shwMainToRefPowerRatioHigh < MED_2MIC_MODFY_ABSEN_POWER_RATIO_HIGH)
        {
            for (shwCntI = shwDifIndex; shwCntI < shwBandLen; shwCntI++)
            {
                /* ��ǰ�Ӵ���������˹����ױ�С������Ϊ���� */
                if (ashwMainToRefPsdRatio[shwCntI] < MED_2MIC_MODFY_ABSEN_PSD_RATIO_HIGH)
                {
                    pshwProbSpAbsence[shwCntI] = CODEC_OP_INT16_MAX;
                }
            }
        }
    }

    /* ������ʷ��Ϣ */
    pstModifyProb->shwPowerRatioLowLast  = shwMainToRefPowerRatioLow;
    pstModifyProb->shwPowerRatioHighLast = shwMainToRefPowerRatioHigh;
    UCOM_MemCpy(pstModifyProb->ashwPsdRatioLast, ashwMainToRefPsdRatio, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));
}



VOS_INT16 MED_2MIC_WeightSum(
                VOS_INT16               *pshwIn,
                VOS_INT16               *pshwSmoothAlpha,
                VOS_INT16                shwSmoothLen,
                VOS_INT16                shwCntI)
{
    VOS_INT16 shwCntK;
    VOS_INT16 shwSmoothOut;
    VOS_INT16 shwTemp;

    shwSmoothOut = 0;
    for (shwCntK = 0; shwCntK < shwSmoothLen; shwCntK++)
    {
        shwTemp = CODEC_OpMult(pshwIn[shwCntI + shwCntK ],
                             pshwSmoothAlpha[shwCntK]);
        shwSmoothOut = CODEC_OpAdd(shwSmoothOut,
                                      shwTemp);
    }
    return shwSmoothOut;
}



VOS_VOID MED_2MIC_CalcProbSpPresen(
                VOS_INT32                         *pswBandPsd,
                VOS_INT16                         *pshwProbSpAbsen,
                VOS_INT16                         *pshwLsaGain,
                VOS_INT16                          shwBandLen,
                MED_2MIC_PROBSPPRESEN_STRU        *pstProbSpPresen,
                VOS_INT16                         *pshwProbSpPresen,
                VOS_INT16                         *pshwSNRFactor)
{
    VOS_INT16  shwCntI;
    VOS_INT32 *pswNsPsd       = MED_NULL;
    VOS_INT32 *pswPostSNR     = MED_NULL;
    VOS_INT32 *pswPostSNRLast = MED_NULL;
    VOS_INT32 *pswPrioriSNR   = MED_NULL;
    VOS_INT16  shwTemp1;
    VOS_INT32  swTemp1;
    VOS_INT32  swTemp2;
    VOS_INT32  swExp;
    VOS_INT16  ashwNsPsdAlphaD[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  ashwGainTmp[MED_2MIC_NUM_CHAN_16k];

    /* �ڴ����� */
    pswPostSNRLast = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(VOS_INT32));
    UCOM_MemSet(pswPostSNRLast , 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT32));


    /* ��ȡ��ʷ��Ϣ */
    pswPrioriSNR    = pstProbSpPresen->aswPrioriSNR;
    pswNsPsd        = pstProbSpPresen->aswNsPsd;
    pswPostSNR      = pstProbSpPresen->aswPostSNR;

    UCOM_MemCpy(pswPostSNRLast, pswPostSNR, (VOS_UINT16)shwBandLen * sizeof(VOS_INT32));

    /* ������Ӵ���������� */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        pswPostSNR[shwCntI] =  CODEC_OpNormDiv_32(pswBandPsd[shwCntI],
                                                pswNsPsd[shwCntI],
                                                MED_PP_SHIFT_BY_8);
        /* ������Сֵ����ֹ��0 */
        pswPostSNR[shwCntI] = CODEC_OpL_max(pswPostSNR[shwCntI],
                                          MED_2MIC_SPPRENSEN_POSTSNR_MIN);
    }

    /* ������������Ⱥ���������� */
    UCOM_MemSet(pswPrioriSNR , 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    UCOM_MemSet(pshwSNRFactor, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    CODEC_OpVvMult(pshwLsaGain, pshwLsaGain, shwBandLen, ashwGainTmp);
    CODEC_OpVcMult(ashwGainTmp, shwBandLen, MED_2MIC_SPPRENSEN_PRIORSNR_ALPHA, ashwGainTmp);
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        swTemp1 = CODEC_OpL_mpy_32_16(pswPostSNRLast[shwCntI], ashwGainTmp[shwCntI]);
        swTemp2 = CODEC_OpL_sub(pswPostSNR[shwCntI], MED_2MIC_SPPRENSEN_Q8);
        swTemp2 = CODEC_OpL_max(swTemp2,0);
        swTemp2 = CODEC_OpL_mpy_32_16(swTemp2, MED_2MIC_SPPRENSEN_PRIORSNR_SUB_ALPHA);

        pswPrioriSNR[shwCntI] = CODEC_OpL_add(swTemp1, swTemp2);

        /* ������Сֵ������Ҫ */
        pswPrioriSNR[shwCntI] = CODEC_OpL_max(pswPrioriSNR[shwCntI],
                                            MED_2MIC_SPPRENSEN_PRIORSNR_MIN);

        swTemp1 = CODEC_OpL_add(MED_2MIC_SPPRENSEN_Q8, pswPrioriSNR[shwCntI]);
        swTemp2 = CODEC_OpNormDiv_32(pswPrioriSNR[shwCntI], swTemp1, MED_PP_SHIFT_BY_31);
        shwTemp1 = CODEC_OpRound(swTemp2);

        swTemp2 = CODEC_OpL_mpy_32_16(pswPostSNR[shwCntI], shwTemp1);
        swTemp1 = CODEC_OpL_shl(swTemp2, MED_PP_SHIFT_BY_3);
        pshwSNRFactor[shwCntI] = CODEC_OpSaturate(swTemp1);

        /* �޶���������ӵ����ֵ */
        pshwSNRFactor[shwCntI] = CODEC_OpMin(pshwSNRFactor[shwCntI],
                                           MED_2MIC_CALCPROBSPPRESEN_SNRFACTOR_MAX);
    }

    /* ������Ӵ��������ڸ��� */
    UCOM_MemSet(pshwProbSpPresen, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* ���������ڸ���С���������ڿ����Դ� */
        if (pshwProbSpAbsen[shwCntI] < MED_2MIC_SPPRENSEN_PROBZEROTHD)
        {
            /* ����֡ */
            if ((0 == pshwProbSpAbsen[shwCntI])
             || (MED_2MIC_CALCPROBSPPRESEN_SNRFACTOR_MAX == pshwSNRFactor[shwCntI]))
             {
                 pshwProbSpPresen[shwCntI] = CODEC_OP_INT16_MAX;
             }
             /* ������������Ӽ����������ڸ��� */
             else
             {
                 swTemp1 = CODEC_OpNormDiv_32((VOS_INT32)pshwProbSpAbsen[shwCntI],
                                            (VOS_INT32)(CODEC_OP_INT16_MAX - pshwProbSpAbsen[shwCntI]),
                                            MED_PP_SHIFT_BY_11);
                 swTemp2 = CODEC_OpL_mpy_32_16(CODEC_OpL_shl(CODEC_OpL_add(MED_2MIC_SPPRENSEN_Q8,
                                                                      pswPrioriSNR[shwCntI]),
                                                          MED_PP_SHIFT_BY_4),
                                             (VOS_INT16)swTemp1);

                 swExp = CODEC_OpL_Exp(pshwSNRFactor[shwCntI],
                                     MED_PP_SHIFT_BY_11,
                                     MED_PP_SHIFT_BY_8);
                 swTemp2 = CODEC_OpL_add(swTemp2, swExp);
                 pshwProbSpPresen[shwCntI] = CODEC_OpRound(CODEC_OpNormDiv_32(swExp,
                                                                          swTemp2,
                                                                          MED_PP_SHIFT_BY_31));
             }
        }
    }

    /* �������������� */
    CODEC_OpVecSet(ashwNsPsdAlphaD, shwBandLen, MED_2MIC_SPPRENSEN_NSPSD_ALPHA);
    CODEC_OpVcMultQ15Add(ashwNsPsdAlphaD,
                       pshwProbSpPresen,
                       shwBandLen,
                       MED_2MIC_SPPRENSEN_NSPSD_SUB_ALPHA,
                       ashwNsPsdAlphaD);
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        swTemp1 = CODEC_OpLSmooth(pswNsPsd[shwCntI],
                                ashwNsPsdAlphaD[shwCntI],
                                pswBandPsd[shwCntI],
                                (CODEC_OP_INT16_MAX - ashwNsPsdAlphaD[shwCntI]));
        pswNsPsd[shwCntI] = CODEC_OpL_mpy_32_16(swTemp1,
                                              MED_2MIC_SPPRENSEN_NSPSD_BETA);
    }

    /* �ڴ��ͷ� */
    UCOM_MemFree(pswPostSNRLast);
}
VOS_VOID MED_2MIC_CalcLsaGain(
                VOS_INT32                 *pswPrioriSNR,
                VOS_INT16                 *pshwSNRFactor,
                VOS_INT16                 *pshwProbSpPresen,
                MED_2MIC_LSAGAIN_STRU     *pstLsaGain,
                VOS_INT16                  shwBandLen,
                VOS_INT16                 *pshwOmLsaGain,
                VOS_INT16                 *pshwLsaGain)
{
    VOS_INT16  shwCntI;
    VOS_INT16  shwCntJ;
    VOS_INT16  shwBandHigh;
    VOS_INT16  ashwOmLsaGain[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  shwBandLow;
    VOS_INT16 *pshwBandAlpha = MED_NULL;
    VOS_INT16  shwSmoothLen;
    VOS_INT16  shwGainMin;
    VOS_INT16  ashwExpIntegral[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  shwExp;
    VOS_INT16  ashwSubProbSpPresen[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16  shwPower1;
    VOS_INT16  shwPower2;
    VOS_INT16  shwDelta;
    VOS_INT32  swTemp;
    VOS_INT32  swSumTmp;
    VOS_INT16  shwMultTmp;
    VOS_INT16  shwGainMinTmp;
    VOS_INT16  shwMeanGain;
    VOS_INT16  shwOmLsaGainMin;

    shwBandLow    = pstLsaGain->shwBandLow;
    pshwBandAlpha = pstLsaGain->pshwBandAlpha;
    shwSmoothLen  = pstLsaGain->shwSmoothLen;
    shwGainMin    = pstLsaGain->shwGainMin;

    /* ������� */
    UCOM_MemSet(ashwExpIntegral, 0, MED_2MIC_NUM_CHAN_16k * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* ������� */
        swTemp = CODEC_OpL_Integral(pshwSNRFactor[shwCntI]);
        ashwExpIntegral[shwCntI] = CODEC_OpRound(swTemp);
    }

    /* ���� LSA gain */
    UCOM_MemSet(pshwLsaGain,
                0,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        shwExp = CODEC_OpSaturate(CODEC_OpL_Exp(ashwExpIntegral[shwCntI],
                                            MED_PP_SHIFT_BY_12,
                                            MED_PP_SHIFT_BY_13));
        swTemp = CODEC_OpNormDiv_32(pswPrioriSNR[shwCntI],
                                  (pswPrioriSNR[shwCntI] + MED_2MIC_LSAGAIN_Q8),
                                  MED_PP_SHIFT_BY_31);
        pshwLsaGain[shwCntI] = CODEC_OpRound(CODEC_OpL_mpy_32_16(swTemp, shwExp));
        pshwLsaGain[shwCntI] = CODEC_OpShl(pshwLsaGain[shwCntI], MED_PP_SHIFT_BY_2);
    }

    /* �޶�LSA gain����Сֵ */
    shwMultTmp           = CODEC_OpMult(shwGainMin, MED_2MIC_LSAGAIN_MIN_ALPHA);
    for (shwCntI = 0; shwCntI < shwBandLow; shwCntI++)
    {
        pshwLsaGain[shwCntI] = CODEC_OpMax(pshwLsaGain[shwCntI], shwMultTmp);
    }

    for (shwCntI = shwBandLow; shwCntI < shwBandLen; shwCntI++)
    {
        pshwLsaGain[shwCntI] = CODEC_OpMax(pshwLsaGain[shwCntI], shwGainMin);
    }

    /* ���� OM-LSA gain */
    UCOM_MemSet(ashwOmLsaGain,
                0,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    CODEC_OpVecSet(ashwSubProbSpPresen,
                 shwBandLen,
                 CODEC_OP_INT16_MAX);

    /* 1 - pshwProbSpPresen */
    CODEC_OpVvSub(ashwSubProbSpPresen, pshwProbSpPresen, shwBandLen, ashwSubProbSpPresen);

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        swTemp = CODEC_OpL_Power(pshwLsaGain[shwCntI],
                               MED_PP_SHIFT_BY_15,
                               pshwProbSpPresen[shwCntI],
                               MED_PP_SHIFT_BY_15,
                               MED_PP_SHIFT_BY_31);
        shwPower1 = CODEC_OpRound(swTemp);
        swTemp = CODEC_OpL_Power(shwGainMin,
                               MED_PP_SHIFT_BY_15,
                               ashwSubProbSpPresen[shwCntI],
                               MED_PP_SHIFT_BY_15,
                               MED_PP_SHIFT_BY_15);
        shwPower2 = CODEC_OpSaturate(swTemp);
        ashwOmLsaGain[shwCntI] = CODEC_OpMult(shwPower1, shwPower2);
    }

    /* ƽ��OM-LSA gain */
    MED_2MIC_SmoothLsaGain(pshwOmLsaGain,
                           pshwBandAlpha,
                           shwSmoothLen,
                           shwBandLow,
                           shwBandLen,
                           ashwOmLsaGain);

    /* �޶�ashwOmLsaGain����Сֵ */
    shwDelta = MED_2MIC_LSA_DELTA;
    shwBandHigh = shwBandLen - shwDelta;
    shwGainMinTmp = CODEC_OpShl(CODEC_OpMult(MED_2MIC_LSAGAIN_MIN_BETA,
                                                         shwGainMin),
                                                         MED_PP_SHIFT_BY_3);

    for (shwCntI = shwBandLow; shwCntI < shwBandHigh; shwCntI++)
    {
        swSumTmp = 0;
        shwCntJ = shwCntI - shwDelta;
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ]));
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ + 1]));
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ + 2]));
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ + 3]));
        swSumTmp = CODEC_OpL_add(swSumTmp , (VOS_INT32)(ashwOmLsaGain[shwCntJ + 4]));

        /* ��ƽ������ */
        shwMeanGain = CODEC_OpExtract_l(CODEC_OpL_mpy_32_16(swSumTmp,
                                                       MED_2MIC_LSA_MEAN_COEF));

        shwOmLsaGainMin = CODEC_OpMin(shwMeanGain, shwGainMinTmp);
        ashwOmLsaGain[shwCntI] = CODEC_OpMax(ashwOmLsaGain[shwCntI],
                                           shwOmLsaGainMin);
    }

    /* ������ʷ��Ϣ���Ա���һ֡ʹ�� */
    CODEC_OpVecCpy(pshwOmLsaGain, ashwOmLsaGain, shwBandLen);
}


VOS_VOID MED_2MIC_SmoothLsaGain(
                VOS_INT16               *pshwLsaGainLast,
                VOS_INT16               *pshwBandAlpha,
                VOS_INT16                shwSmoothLen,
                VOS_INT16                shwBandLow,
                VOS_INT16                shwBandLen,
                VOS_INT16               *pshwLsaGain)
{
    VOS_INT16 shwCntI;
    VOS_INT16 ashwLsaGainExtend[MED_2MIC_NUM_CHAN_16k + 2];
    VOS_INT16 ashwLsaGainTmp[MED_2MIC_NUM_CHAN_16k];
    VOS_INT16 shwDelta;
    VOS_INT16 shwTemp1;
    VOS_INT16 shwTemp2;
    VOS_INT16 shwSubAlpha;
    VOS_INT32 swSumTmp;
    VOS_INT16 shwMeanTmp;

    /* �Ӵ���ƽ�� */
    ashwLsaGainExtend[0]              = pshwLsaGain[0];
    ashwLsaGainExtend[shwBandLen + 1] = pshwLsaGain[shwBandLen - 1];
    UCOM_MemCpy(ashwLsaGainExtend + 1,
                pshwLsaGain,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));
    UCOM_MemCpy(ashwLsaGainTmp,
                pshwLsaGain,
                (VOS_UINT16)shwBandLen * sizeof(VOS_INT16));

    shwDelta = MED_2MIC_LSA_DELTA;

    for (shwCntI = shwBandLow; shwCntI < (shwBandLen - shwDelta); shwCntI++)
    {
        /* ���㵱ǰ5���Ӵ���ƽ������ */
        swSumTmp = 0;
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI - 2]));
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI - 1]));
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI]));
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI + 1]));
        swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntI + 2]));
    /*    for (shwCntJ = (shwCntI - shwDelta); shwCntJ < (shwCntI + shwDelta + 1); shwCntJ++)
        {
            swSumTmp = CODEC_OpL_add(swSumTmp, (VOS_INT32)(ashwLsaGainTmp[shwCntJ]));
        }*/

        shwMeanTmp = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(swSumTmp,
                                                      MED_2MIC_LSA_MEAN_COEF));

        /* �Ե�ǰ�Ӵ�����ƽ�� */
        if (shwMeanTmp < MED_2MIC_LSAGAIN_SMOOTH_THD)
        {
            pshwLsaGain[shwCntI] = MED_2MIC_WeightSum(ashwLsaGainExtend,
                                                      pshwBandAlpha,
                                                      shwSmoothLen,
                                                      shwCntI);
        }
    }


    /* ֡��ƽ�� */
    shwSubAlpha = CODEC_OpSub(CODEC_OP_INT16_MAX, MED_2MIC_LSAGAIN_TIME_ALPHA);
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        shwTemp1 = CODEC_OpMult(MED_2MIC_LSAGAIN_TIME_ALPHA, pshwLsaGainLast[shwCntI]);
        shwTemp2 = CODEC_OpMult(shwSubAlpha, pshwLsaGain[shwCntI]);
        pshwLsaGain[shwCntI] = CODEC_OpAdd(shwTemp1, shwTemp2);
    }
}


VOS_VOID MED_2MIC_ExtendBand2FreqBinGain(
                VOS_INT32               *pswBandMc,
                VOS_INT16               *pshwBandGain,
                VOS_INT16                shwFreqBinLen,
                VOS_INT16                shwBandLen,
                VOS_INT16               *pshwFreqBinGain,
                MED_2MIC_PROBSPPRESEN_STRU *pstProbSpPresen,
                VOS_INT16                  *pshwProbSpPrese)
{
    VOS_INT16 shwCntI;
    VOS_INT16 shwCntJ;
    VOS_INT16 shwChLo;
    VOS_INT16 shwChHi;
    VOS_INT32 swNsPow                   = 0;
    VOS_INT16 shwAllPassInd             = 0;
    VOS_INT16 shwSpechProb              = 0;

    VOS_INT16 *pshwMedAnr2MicChTbl  = (VOS_INT16 *)MED_2MIC_GetAnr2MicChTblPtr();
    MED_AEC_OBJ_STRU *pstAECObj     = MED_AEC_GetObjPtr();

    for(shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
       swNsPow = CODEC_OpL_add(swNsPow,
                               CODEC_OpL_shr(pstProbSpPresen->aswNsPsd[shwCntI],
                                             MED_PP_SHIFT_BY_5));
    }

    if (swNsPow <= pstProbSpPresen->shwNsLowTh)
    {
        pstProbSpPresen->shwNsIndex = pstProbSpPresen->shwNsIndex + 1;
        if (pstProbSpPresen->shwNsIndex >= MED_2MIC_TMOS_SILENCETh)
        {
            pstProbSpPresen->shwAllPassNum = MED_2MIC_TMOS_CLOSEANR_LEN;
        }
    }
    else
    {
        pstProbSpPresen->shwNsIndex = 0;
    }

    swNsPow     = 0;
    shwChLo     = pstProbSpPresen->shwTmosExitBandLow;
    shwChHi     = pstProbSpPresen->shwTmosExitBandHi;
    for (shwCntI = shwChLo; shwCntI < shwChHi; shwCntI++)
    {
        swNsPow = CODEC_OpL_add(swNsPow,
                                CODEC_OpL_mpy_32_16(pswBandMc[shwCntI],
                                                    CODEC_OpSub(CODEC_OP_INT16_MAX, pshwProbSpPrese[shwCntI])));
        shwSpechProb = CODEC_OpAdd(shwSpechProb,
                                   CODEC_OpShr(pshwProbSpPrese[shwCntI], MED_PP_SHIFT_BY_5));
    }

    UCOM_MemSet(pshwFreqBinGain,
                0,
                (VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT16));

    pstProbSpPresen->shwAllPassNum = CODEC_OpMax(0, (pstProbSpPresen->shwAllPassNum - 1));

    if (pstProbSpPresen->shwAllPassNum > 0)
    {
        shwAllPassInd = 1;
    }
    if ((CODEC_OpL_shr(swNsPow, MED_PP_SHIFT_BY_5) > pstProbSpPresen->shwNsHighTh)
        && (shwSpechProb < pstProbSpPresen->shwSpLowTh))
    {
        shwAllPassInd = 0;
    }

    if ((0 == shwAllPassInd) || (pstAECObj->stDtd.enDtdFlag == MED_AEC_DTD_FLAG_ST))
    {
        /* ����ÿ���Ӵ������� */
        for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
        {
            /* �Ӵ���Χ */
            shwChLo = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 0);
            shwChHi = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 1);

            for (shwCntJ = (shwChLo - 1); shwCntJ < shwChHi; shwCntJ++)
            {
                pshwFreqBinGain[shwCntJ] = pshwBandGain[shwCntI];
            }
        }
    }
    else
    {
        /* ����ÿ���Ӵ������� */
        for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
        {
            /* �Ӵ���Χ */
            shwChLo = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 0);
            shwChHi = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 1);

            for (shwCntJ = (shwChLo - 1); shwCntJ < shwChHi; shwCntJ++)
            {
                pshwFreqBinGain[shwCntJ] = CODEC_OP_INT16_MAX;
            }
        }
    }
}


VOS_VOID MED_2MIC_FilterFrm(
                VOS_INT16                shwFreqBinLen,
                VOS_INT16               *pshwFreqBinGain,
                MED_2MIC_FREQ_STRU      *pstMc)
{
    VOS_INT16 shwCntI;
    VOS_INT16 *pshwFreqMc = MED_NULL;

    /* �����Ƶ�� */
    pshwFreqMc = pstMc->ashwFreq;

    /* ʩ������ */
    pshwFreqMc[0] = CODEC_OpMult(pshwFreqMc[0], pshwFreqBinGain[0]);
    pshwFreqMc[1] = CODEC_OpMult(pshwFreqMc[1], pshwFreqBinGain[shwFreqBinLen - 1]);

    for (shwCntI = 1; shwCntI < (shwFreqBinLen - 1); shwCntI++)
    {
        pshwFreqMc[2 * shwCntI]       = CODEC_OpMult(pshwFreqMc[2 * shwCntI],
                                                pshwFreqBinGain[shwCntI]);
        pshwFreqMc[(2 * shwCntI) + 1] = CODEC_OpMult(pshwFreqMc[(2 * shwCntI) + 1],
                                                    pshwFreqBinGain[shwCntI]);
    }

}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

