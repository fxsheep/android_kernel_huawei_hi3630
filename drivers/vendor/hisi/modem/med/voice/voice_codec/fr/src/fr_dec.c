/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header: /home/kbs/jutta/src/gsm/gsm-1.0/src/RCS/gsm_decode.c,v 1.1 1992/10/28 00:15:50 jutta Exp $ */

#include "fr_etsi_op.h"
#include "fr_dec.h"
#include "fr_rpe.h"
#include "fr_long_term.h"
#include "fr_short_term.h"
#include "ucom_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
 �� �� ��  : FR_DecInit
 ��������  : FR ���н����ʼ��
 �������  : FR_DECODE_STATE_STRU *pstMedFrDecObj  --FR����ṹ��ָ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��5��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_DecInit(FR_DECODE_STATE_STRU *pstMedFrDecObj)
{
    UCOM_MemSet(pstMedFrDecObj, 0, sizeof(FR_DECODE_STATE_STRU));

    pstMedFrDecObj->stRpeLtpState.nrp = 40;

    /* ����DTX����ģ���ʼ�� */
    FR_DtxRxInit(&(pstMedFrDecObj->stDecDtxState));
}

/*
 *  4.3 FIXED POINT IMPLEMENTATION OF THE RPE-LTP DECODER
 */

static void Postprocessing(
            Word16                     *pshwMsr,
            Word16                     *s)
{
    Word32       k;
    Word16       msr = *pshwMsr;
    Word16       tmp;

    for (k = 160; k--; s++) {
        tmp = GSM_MULT_R( msr, 28180 );
        msr = GSM_ADD(*s, tmp);        /* Deemphasis         */
#ifdef USE_ROUNDING_INSTEAD_OF_TRUNCATION
        *s = GSM_ADD(msr, msr);           /* Upscaling by 2 */
        *s = GSM_ADD(*s, 4) & 0xFFF8;     /* Truncation */
#else
        *s  = (Word16)(GSM_ADD(msr, msr) & 0xFFF8); /* Upscaling & Truncation */
#endif
    }

    *pshwMsr = msr;
}

void Gsm_Decoder(
                FR_DECODE_STATE_STRU                       *pstFrDecState,
                Word16                                     *LARcr,              /* [0..7]       IN  */
                Word16                                     *Ncr,                /* [0..3]       IN  */
                Word16                                     *bcr,                /* [0..3]       IN  */
                Word16                                     *Mcr,                /* [0..3]       IN  */
                Word16                                     *xmaxcr,             /* [0..3]       IN  */
                Word16                                     *xMcr,               /* [0..13*4]        IN  */
                Word16                                     *s,                  /* [0..159]     OUT     */
                FR_TAF_STATUS_ENUM_UINT16                   enTafFlag,
                FR_SID_STATUS_ENUM_UINT16                   enSidFlag,
                FR_BFI_STATUS_ENUM_UINT16                   enBfiFlag)
{
    Word32                              j, k;
    Word16                              erp[40], wt[160];
    Word16                             *drp = pstFrDecState->stRpeLtpState.dp0 + 120;
    Word16                             *LARpp_j;
    Word16                             *LARpp_j_1;
    Word16                              shwRxDtxCtrl;                           /* ���н���DTX������ */
    FR_DEC_DTX_STATE_STRU              *pstDecDtxState;

    pstDecDtxState = &(pstFrDecState->stDecDtxState);

    /* �ô����Э�����Gsm_Short_Term_Synthesis_Filter()�������Ƴ����� */
    LARpp_j   = (Word16 *)pstFrDecState->stRpeLtpState.LARpp[pstFrDecState->stRpeLtpState.j];
    LARpp_j_1 = (Word16 *)pstFrDecState->stRpeLtpState.LARpp[pstFrDecState->stRpeLtpState.j ^= 1];

    /* DTX���д���Ͷ�ʧ֡���� */
    FR_DtxControlRx(enTafFlag,
                    enBfiFlag,
                    enSidFlag,
                    &(pstDecDtxState->shwStateCnt),
                    &(pstDecDtxState->enPrevBfi),
                    pstDecDtxState);

    /* ����LAR������Xmaxc���� */
    FR_DtxDecSidLarAndXmaxc(LARcr, LARpp_j, LARpp_j_1, xmaxcr, enBfiFlag, pstDecDtxState);

    shwRxDtxCtrl = pstDecDtxState->shwCtrlWord;

    /* �����ǰ֡Ϊ������֡ */
    if (0 == (shwRxDtxCtrl & FR_RX_SP_FLAG))
    {
        /* �������������������� */
        FR_DtxDecSidOtherPrms(Ncr, bcr, Mcr, xMcr, &(pstDecDtxState->swPnSeed));
    }

    if (pstDecDtxState->shwMuteCount > FR_DTX_MUTE_THRESH)
    {
        /* ��������֡ */
        FR_DtxGenSilence(LARpp_j, Ncr, bcr, Mcr, xmaxcr, xMcr);
    }

    /* ���һ֡���������ʼ����һ֡���� */
    for (j = 0; j <= 3; j++)
    {

        Gsm_RPE_Decoding(*xmaxcr, *Mcr, xMcr, erp);

        Gsm_Long_Term_Synthesis_Filtering(&(pstFrDecState->stRpeLtpState.nrp),
                                          *Ncr,
                                          *bcr,
                                          erp,
                                          drp);

        for (k = 0; k <= 39; k++)
        {
            wt[(j * 40) + k] = drp[k];
        }

        xmaxcr++;
        bcr++;
        Ncr++;
        Mcr++;
        xMcr += 13;
    }

    Gsm_Short_Term_Synthesis_Filter(&(pstFrDecState->stRpeLtpState),
                                    LARpp_j,
                                    LARpp_j_1,
                                    wt,
                                    s);

    Postprocessing(&(pstFrDecState->stRpeLtpState.msr), s);

    /* ������һ֡BFI��־ */
    pstDecDtxState->enPrevBfi = enBfiFlag;
}

void gsm_decode(FR_DECODE_STATE_STRU * s, Word16 * c, Word16 * target)
{
    Word16    LARc[8], Nc[4], Mc[4], bc[4], xmaxc[4], xmc[13*4];

    Word32    shwCnt = 0;
    Word32    i, j;

    FR_SID_STATUS_ENUM_UINT16   enSidFlag = FR_SID_STATUS_NOT;
    FR_TAF_STATUS_ENUM_UINT16   enTafFlag = FR_TAF_STATUS_NOT;
    FR_BFI_STATUS_ENUM_UINT16   enBfiFlag = FR_BFI_STATUS_NOT;

    /* ��ȡLAR������� */
    for(shwCnt = 0; shwCnt < 8; shwCnt++)
    {
        LARc[shwCnt] = c[shwCnt];
    }

    /* ��ȡ������� */
    for (i = 0; i < 4; i++)
    {
        j = (Word16)(8 + (17 * i));
        Nc[i]    = c[j];
        bc[i]    = c[j + 1];
        Mc[i]    = c[j + 2];
        xmaxc[i] = c[j + 3];

        for ( shwCnt = (j + 4); shwCnt < (j + 17) ; shwCnt++ )
        {
            xmc[shwCnt - (12 + (4 * i))] = c[shwCnt];
        }
    }

    /* ��ȡ����֡��־ */
    enSidFlag = (FR_TAF_STATUS_ENUM_UINT16)c[76];
    enTafFlag = (FR_TAF_STATUS_ENUM_UINT16)c[77];
    enBfiFlag = (FR_TAF_STATUS_ENUM_UINT16)c[78];

    Gsm_Decoder(s, LARc, Nc, bc, Mc, xmaxc, xmc, target, enTafFlag, enSidFlag, enBfiFlag);
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

