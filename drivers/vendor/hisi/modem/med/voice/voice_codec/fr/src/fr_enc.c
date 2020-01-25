/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#include "fr_enc.h"
#include "fr_lpc.h"
#include "fr_preproce.h"
#include "fr_rpe.h"
#include "fr_short_term.h"
#include "fr_long_term.h"
#include "fr_interface.h"
#include "ucom_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
 �� �� ��  : FR_EncInit
 ��������  : FR ���б����ʼ��
 �������  : FR_ENCODE_STATE_STRU *pstMedFrEncObj  --FR���б������ṹ��
             FR_DTX_ENUM_UINT16 enDtxMode  --DTXģʽ��־
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : MED_FR_VadReset��MED_FR_DtxTxReset
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��5��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_EncInit(
                FR_ENCODE_STATE_STRU                       *pstMedFrEncObj,
                FR_DTX_ENUM_UINT16                          enDtxMode)
{
    UCOM_MemSet(pstMedFrEncObj, 0, sizeof(FR_ENCODE_STATE_STRU));

    pstMedFrEncObj->stRpeLtpState.nrp   = 40;

    pstMedFrEncObj->enDtxFlag           = enDtxMode;

    /* VADģ���ʼ�� */
    FR_VadInit(&(pstMedFrEncObj->stVadState));

    /* DTX���б����ʼ�� */
    FR_DtxTxInit(&(pstMedFrEncObj->stEncDtxState));
}

/*
 *  4.2 FIXED POINT IMPLEMENTATION OF THE RPE-LTP CODER
 */

void Gsm_Coder(
    FR_ENCODE_STATE_STRU    * S,

    Word16    * s,    /* [0..159] samples         IN  */

/*
 * The RPE-LTD coder works on a frame by frame basis.  The length of
 * the frame is equal to 160 samples.  Some computations are done
 * once per frame to produce at the output of the coder the
 * LARc[1..8] parameters which are the coded LAR coefficients and
 * also to realize the inverse filtering operation for the entire
 * frame (160 samples of signal d[0..159]).  These parts produce at
 * the output of the coder:
 */

    Word16    * LARc,          /* [0..7] LAR coefficients      OUT */

/*
 * Procedure 4.2.11 to 4.2.18 are to be executed four times per
 * frame.  That means once for each sub-segment RPE-LTP analysis of
 * 40 samples.  These parts produce at the output of the coder:
 */

    Word16    * Nc,            /* [0..3] LTP lag                   OUT */
    Word16    * bc,            /* [0..3] coded LTP gain            OUT */
    Word16    * Mc,            /* [0..3] RPE grid selection        OUT */
    Word16    * xmaxc,         /* [0..3] Coded maximum amplitude   OUT */
    Word16    * xMc,           /* [13*4] normalized RPE samples    OUT */
    FR_VAD_STATUS_ENUM_UINT16  *penVadFlag,    /* VAD����־  OUT */
    Word16    *pshwSpFlag      /* SP�ж���־   OUT */
)
{
    Word32     k;
    Word16    *dp  = S->stRpeLtpState.dp0 + 120;   /* [ -120...-1 ] */
    Word16    *dpp = dp;     /* [ 0...39 ]    */

    static Word16 e [50] = {0};

    Word16    so[160];

    /*VAD�����������*/
    Word16                              shwVadScalauto;
    Word16                              ashwVadNc[4];
    Word32                              aswVadLacf[9];
    FR_VAD_STATUS_ENUM_UINT16           enVadStatus;

    /*DTX�������*/
    Word16                              ashwDtxLaruq[8];
    Word16                              ashwDtxXmaxuq[4];
    Word16                              shwSpFlag;

    Gsm_Preprocess(&(S->stRpeLtpState), s, so);

    Gsm_LPC_Analysis(S->stRpeLtpState.fast, so, LARc, aswVadLacf, &shwVadScalauto, ashwDtxLaruq);

    Gsm_Short_Term_Analysis_Filter(&(S->stRpeLtpState), LARc, so);


    for (k = 0; k <= 3; k++, (xMc += 13))
    {
        Gsm_Long_Term_Predictor(S->stRpeLtpState.fast,
                      &so[k*40], /* d      [0..39] IN  */
                      dp,      /* dp  [-120..-1] IN  */
                      &e[5],    /* e      [0..39] OUT */
                      dpp,      /* dpp    [0..39] OUT */
                      Nc++,
                      bc++);



        ashwVadNc[k] = *(Nc-1);

        Gsm_RPE_Encoding(
                      &e[5],  /* e      ][0..39][ IN/OUT */
                      xmaxc++,
                      Mc++,
                      xMc,
                      &ashwDtxXmaxuq[k]);

        /*
         * Gsm_Update_of_reconstructed_short_time_residual_signal
         *          ( dpp, e + 5, dp );
         */
        CODEC_OpVvAdd(&e[5], &dpp[0], 40, &dp[0]);            /*ע��:p��dpָ����ͬ�ڴ�*/

        dp  += 40;
        dpp += 40;

    }

    CODEC_OpVecCpy(S->stRpeLtpState.dp0,
                 (S->stRpeLtpState.dp0 + 160),
                 120);

    /* ���DTX���ܴ� */
    if (FR_DTX_ENABLE == S->enDtxFlag)
    {
        /* ����VAD��� */
        enVadStatus = FR_VadDetect(aswVadLacf,
                                   shwVadScalauto,
                                   ashwVadNc,
                                   &(S->stVadState));
        *penVadFlag = enVadStatus;

        /* ����DTX��� */
        FR_DtxControlTx(enVadStatus, &(S->stEncDtxState));

        shwSpFlag = (S->stEncDtxState.shwCtrlWord & FR_TX_SP_FLAG);

        if (0 == shwSpFlag)
        {
            /* �Ƚ��ѱ��������0 */
            CODEC_OpVecSet(LARc, FR_LARC_LEN, 0);
            CODEC_OpVecSet(Nc-FR_NC_LEN, FR_NC_LEN, 0);
            CODEC_OpVecSet(bc-FR_BC_LEN, FR_BC_LEN, 0);
            CODEC_OpVecSet(Mc-FR_MC_LEN, FR_MC_LEN, 0);
            CODEC_OpVecSet(xmaxc-FR_XMAXC_LEN, FR_XMAXC_LEN, 0);
            CODEC_OpVecSet(xMc-FR_XMC_LEN, FR_XMC_LEN, 0);

            /* ����SID֡ */
            FR_DtxEncSidPrm(LARc, xmaxc-FR_XMAXC_LEN, &(S->stEncDtxState));
        }

        /* Update SID memory */
        FR_DtxUpdateSidMem(ashwDtxLaruq, ashwDtxXmaxuq, &(S->stEncDtxState));

        *pshwSpFlag  = shwSpFlag;
    }
    else
    {
        *pshwSpFlag  = 1;
        *penVadFlag  = 1;
    }

    return;
}

void gsm_encode(FR_ENCODE_STATE_STRU * s, Word16 * source, Word16 * c)
{
    Word16                              LARc[8], Nc[4], Mc[4], bc[4], xmaxc[4], xmc[13*4];
    Word32                              swCnt    = 0;
    FR_VAD_STATUS_ENUM_UINT16           enVadFlag = FR_VAD_STATUS_NOT;
    Word16                              enSpFlag  = 0;
    Word32                              i, j;

    Gsm_Coder(s, source, LARc, Nc, bc, Mc, xmaxc, xmc, &enVadFlag, &enSpFlag);

    /*��ȡLARc�������*/
    for (swCnt = 0; swCnt < 8; swCnt++)
    {
        c[swCnt] = LARc[swCnt];
    }

    /*��ȡ�������*/
    for (i = 0; i < 4; i++)
    {
        j           = (Word16)(8 + (17 * i));
        c[j]        = Nc[i];
        c[j + 1]    = bc[i];
        c[j + 2]    = Mc[i];
        c[j + 3]    = xmaxc[i];

        for ( swCnt = (j + 4); swCnt < (j + 17) ; swCnt++ )
        {
             c[swCnt]= xmc[swCnt - (12 + (4 * i))];
        }
    }

    /*��ȡvad��־��sp��־*/
    c[0] = c[0] | (Word16)(enVadFlag << 15);
    c[1] = c[1] | (Word16)(enSpFlag  << 15);
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

