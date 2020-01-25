/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
****************************************************************************
*
*      File             : q_plsf_3.c
*      Purpose          : Quantization of LSF parameters with 1st order MA
*                         prediction and split by 3 vector quantization
*                         (split-VQ)
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "q_plsf.h"

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "lsp_lsf.h"
#include "reorder.h"
#include "lsfwt.h"
#include "cnst.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
#include "q_plsf_3_tab.c"        /* Codebooks of LSF prediction residual */
#define PAST_RQ_INIT_SIZE 8
/*
*****************************************************************************
*                         LOCAL PROGRAM CODE
*****************************************************************************
*/
/* Quantization of a 4 dimensional subvector */

static Word16
Vq_subvec4(             /* o: quantization index,            Q0  */
    Word16 * lsf_r1,    /* i: 1st LSF residual vector,       Q15 */
    Word16 * dico,      /* i: quantization codebook,         Q15 */
    Word16 * wf1,       /* i: 1st LSF weighting factors,     Q13 */
    Word16 dico_size)   /* i: size of quantization codebook, Q0  */
{
    Word16 i, index = 0;
    Word16 *p_dico, temp;
    Word32 dist_min, dist;

    dist_min = MAX_32;
    p_dico = dico;

    for (i = 0; i < dico_size; i++)
    {
        temp = sub (lsf_r1[0], *p_dico++);
        temp = mult (wf1[0], temp);
        dist = L_mult (temp, temp);

        temp = sub (lsf_r1[1], *p_dico++);
        temp = mult (wf1[1], temp);
        dist = L_mac (dist, temp, temp);

        temp = sub (lsf_r1[2], *p_dico++);
        temp = mult (wf1[2], temp);
        dist = L_mac (dist, temp, temp);

        temp = sub (lsf_r1[3], *p_dico++);
        temp = mult (wf1[3], temp);
        dist = L_mac (dist, temp, temp);

        if ( dist < dist_min )
        {
            dist_min = dist;
            index = i;
        }
    }

    /* Reading the selected vector */

    p_dico = &dico[shl (index, 2)];
    lsf_r1[0] = *p_dico++;
    lsf_r1[1] = *p_dico++;
    lsf_r1[2] = *p_dico++;
    lsf_r1[3] = *p_dico++;

    return index;

}

/* Quantization of a 3 dimensional subvector */

static Word16
Vq_subvec3(             /* o: quantization index,            Q0  */
    Word16 * lsf_r1,    /* i: 1st LSF residual vector,       Q15 */
    Word16 * dico,      /* i: quantization codebook,         Q15 */
    Word16 * wf1,       /* i: 1st LSF weighting factors,     Q13 */
    Word16 dico_size,   /* i: size of quantization codebook, Q0  */
    Flag use_half)      /* i: use every second entry in codebook */
{
    Word16 i, index = 0;
    Word16 *p_dico, temp;
    Word32 dist_min, dist;

    dist_min = MAX_32;
    p_dico = dico;

    if (use_half == 0) {
       for (i = 0; i < dico_size; i++)
       {
          temp = sub(lsf_r1[0], *p_dico++);
          temp = mult(wf1[0], temp);
          dist = L_mult(temp, temp);

          temp = sub(lsf_r1[1], *p_dico++);
          temp = mult(wf1[1], temp);
          dist = L_mac(dist, temp, temp);

          temp = sub(lsf_r1[2], *p_dico++);
          temp = mult(wf1[2], temp);
          dist = L_mac(dist, temp, temp);

          if ( dist < dist_min ) {
             dist_min = dist;
             index = i;
          }
       }
       p_dico = &dico[add(index, add(index, index))];
    }
    else
    {
       for (i = 0; i < dico_size; i++)
       {
          temp = sub(lsf_r1[0], *p_dico++);
          temp = mult(wf1[0], temp);
          dist = L_mult(temp, temp);

          temp = sub(lsf_r1[1], *p_dico++);
          temp = mult(wf1[1], temp);
          dist = L_mac(dist, temp, temp);

          temp = sub(lsf_r1[2], *p_dico++);
          temp = mult(wf1[2], temp);
          dist = L_mac(dist, temp, temp);

          if ( dist < dist_min )
          {
             dist_min = dist;
             index = i;
          }
          p_dico = p_dico + 3; add(0,0);
       }
       p_dico = &dico[shl(add(index, add(index, index)),1)];
    }

    /* Reading the selected vector */
    lsf_r1[0] = *p_dico++;
    lsf_r1[1] = *p_dico++;
    lsf_r1[2] = *p_dico++;

    return index;
}

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/

/***********************************************************************
 *
 * routine:   Q_plsf_3()
 *
 * Quantization of LSF parameters with 1st order MA prediction and
 * split by 3 vector quantization (split-VQ)
 *
 ***********************************************************************/
void Q_plsf_3(
    Q_plsfState *st,    /* i/o: state struct                             */
    enum Mode mode,     /* i  : coder mode                               */
    Word16 *lsp1,       /* i  : 1st LSP vector                      Q15  */
    Word16 *lsp1_q,     /* o  : quantized 1st LSP vector            Q15  */
    Word16 *indice,     /* o  : quantization indices of 3 vectors   Q0   */
    Word16 *pred_init_i /* o  : init index for MA prediction in DTX mode */
)
{
    Word16 i, j;
    Word16 lsf1[M], wf1[M], lsf_p[M], lsf_r1[M];
    Word16 lsf1_q[M];

    Word32 L_pred_init_err;
    Word32 L_min_pred_init_err;
    Word16 temp_r1[M];
    Word16 temp_p[M];

    /* convert LSFs to normalize frequency domain 0..16384 */

    Lsp_lsf(lsp1, lsf1, M);

    /* compute LSF weighting factors (Q13) */

    Lsf_wt(lsf1, wf1);

    /* Compute predicted LSF and prediction error */
    if ( mode != MRDTX )
    {
       for (i = 0; i < M; i++)
       {
          lsf_p[i] = add(mean_lsf[i],
                         mult(st->past_rq[i],
                              pred_fac[i]));
          lsf_r1[i] = sub(lsf1[i], lsf_p[i]);
      }
    }
    else
    {
       /* DTX mode, search the init vector that yields */
       /* lowest prediction resuidual energy           */
       *pred_init_i = 0;
       L_min_pred_init_err = 0x7fffffff; /* 2^31 - 1 */
       for (j = 0; j < PAST_RQ_INIT_SIZE; j++)
       {
          CODEC_OpVvAdd(&mean_lsf[0], &past_rq_init[j*M+0], M, &temp_p[0]);

          CODEC_OpVvSub(&lsf1[0], &temp_p[0], M, &temp_r1[0]);

          L_pred_init_err = CODEC_OpVvMac(&temp_r1[0],
                                        &temp_r1[0],
                                        M,
                                        0);

          if ( L_pred_init_err < L_min_pred_init_err )
          {
             L_min_pred_init_err = L_pred_init_err;

             CODEC_OpVecCpy(lsf_r1, temp_r1, M);

             CODEC_OpVecCpy(lsf_p, temp_p, M);

             /* Set zerom */
             CODEC_OpVecCpy(st->past_rq, &past_rq_init[j*M], M);

             *pred_init_i = j;
          } /* endif */
       } /* next j */
    } /* endif MRDTX */

    /*---- Split-VQ of prediction error ----*/
    if ( MR475 == mode || MR515 == mode)
    {   /* MR475, MR515 */

      indice[0] = Vq_subvec3(&lsf_r1[0], dico1_lsf, &wf1[0], DICO1_SIZE, 0);

      indice[1] = Vq_subvec3(&lsf_r1[3], dico2_lsf, &wf1[3], DICO2_SIZE/2, 1);

      indice[2] = Vq_subvec4(&lsf_r1[6], mr515_3_lsf, &wf1[6], MR515_3_SIZE);

    }
    else if ( MR795 == mode)
    {   /* MR795 */

      indice[0] = Vq_subvec3(&lsf_r1[0], mr795_1_lsf, &wf1[0], MR795_1_SIZE, 0);

      indice[1] = Vq_subvec3(&lsf_r1[3], dico2_lsf, &wf1[3], DICO2_SIZE, 0);

      indice[2] = Vq_subvec4(&lsf_r1[6], dico3_lsf, &wf1[6], DICO3_SIZE);

    }
    else
    {   /* MR59, MR67, MR74, MR102 , MRDTX */

      indice[0] = Vq_subvec3(&lsf_r1[0], dico1_lsf, &wf1[0], DICO1_SIZE, 0);

      indice[1] = Vq_subvec3(&lsf_r1[3], dico2_lsf, &wf1[3], DICO2_SIZE, 0);

      indice[2] = Vq_subvec4(&lsf_r1[6], dico3_lsf, &wf1[6], DICO3_SIZE);

    }

    /* Compute quantized LSFs and update the past quantized residual */

    CODEC_OpVvAdd(&lsf_r1[0], &lsf_p[0], M, &lsf1_q[0]);

    CODEC_OpVecCpy(&st->past_rq[0],
                 &lsf_r1[0],
                 M);

    /* verification that LSFs has mimimum distance of LSF_GAP Hz */

    Reorder_lsf(lsf1_q, LSF_GAP, M);

    /*  convert LSFs to the cosine domain */

    Lsf_lsp(lsf1_q, lsp1_q, M);
}
