#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"

#define L_CODE    40
#define NB_TRACK  5
#define NB_PULSE  10
#define STEP      5

/* local functions */

void EFR_cor_h_x (
    Word16 h[],    /* (i)  : impulse response of weighted synthesis filter */
    Word16 x[],    /* (i)  : target                                        */
    Word16 dn[]    /* (o)  : correlation between target and h[]            */
);

void EFR_set_sign (
    Word16 dn[],      /* (i/o)  : correlation between target and h[]       */
    Word16 cn[],      /* (i)  : residual after long term prediction        */
    Word16 sign[],    /* (o)  : sign of d[n]                               */
    Word16 pos_max[], /* (o)  : position of maximum of dn[]                */
    Word16 ipos[]     /* (o)  : starting position for each pulse           */
);

void EFR_cor_h (
    Word16 h[],         /* (i)  : impulse response of weighted synthesis
                                  filter */
    Word16 sign[],      /* (i)  : sign of d[n]                             */
    Word16 rr[][L_CODE] /* (o)  : matrix of autocorrelation                */
);
void EFR_search_10i40 (
    Word16 dn[],         /* (i) : correlation between target and h[]       */
    Word16 rr[][L_CODE], /* (i) : matrix of autocorrelation                */
    Word16 ipos[],       /* (i) : starting position for each pulse         */
    Word16 pos_max[],    /* (i) : position of maximum of dn[]              */
    Word16 codvec[]      /* (o) : algebraic codebook vector                */
);
void EFR_build_code (
    Word16 codvec[], /* (i)  : algebraic codebook vector                   */
    Word16 sign[],   /* (i)  : sign of dn[]                                */
    Word16 cod[],    /* (o)  : algebraic (fixed) codebook excitation       */
    Word16 h[],      /* (i)  : impulse response of weighted synthesis filter*/
    Word16 y[],      /* (o)  : filtered fixed codebook excitation           */
    Word16 indx[]    /* (o)  : index of 10 pulses (position+sign+ampl)*10   */
);

void EFR_q_p (
    Word16 *ind,   /* Pulse position                                        */
    Word16 n       /* Pulse number                                          */
);

/*************************************************************************
 *
 *  FUNCTION:  code_10i40_35bits()
 *
 *  PURPOSE:  Searches a 35 bit algebraic codebook containing 10 pulses
 *            in a frame of 40 samples.
 *
 *  DESCRIPTION:
 *    The code contains 10 nonzero pulses: i0...i9.
 *    All pulses can have two possible amplitudes: +1 or -1.
 *    The 40 positions in a subframe are divided into 5 tracks of
 *    interleaved positions. Each track contains two pulses.
 *    The pulses can have the following possible positions:
 *
 *       i0, i5 :  0, 5, 10, 15, 20, 25, 30, 35.
 *       i1, i6 :  1, 6, 11, 16, 21, 26, 31, 36.
 *       i2, i7 :  2, 7, 12, 17, 22, 27, 32, 37.
 *       i3, i8 :  3, 8, 13, 18, 23, 28, 33, 38.
 *       i4, i9 :  4, 9, 14, 19, 24, 29, 34, 39.
 *
 *    Each pair of pulses require 1 bit for their signs and 6 bits for their
 *    positions (3 bits + 3 bits). This results in a 35 bit codebook.
 *    The function determines the optimal pulse signs and positions, builds
 *    the codevector, and computes the filtered codevector.
 *
 *************************************************************************/

void EFR_Code_10i40_35bits (
    Word16 x[],   /* (i)   : target vector                                 */
    Word16 cn[],  /* (i)   : residual after long term prediction           */
    Word16 h[],   /* (i)   : impulse response of weighted synthesis filter
                             h[-L_subfr..-1] must be set to zero           */
    Word16 cod[], /* (o)   : algebraic (fixed) codebook excitation         */
    Word16 y[],   /* (o)   : filtered fixed codebook excitation            */
    Word16 indx[] /* (o)   : index of 10 pulses (sign + position)          */
)
{
    Word16 ipos[NB_PULSE], pos_max[NB_TRACK], codvec[NB_PULSE];
    Word16 dn[L_CODE], sign[L_CODE];
    Word16 rr[L_CODE][L_CODE], i;

    EFR_cor_h_x (h, x, dn);
    EFR_set_sign (dn, cn, sign, pos_max, ipos);
    EFR_cor_h (h, sign, rr);
    EFR_search_10i40 (dn, rr, ipos, pos_max, codvec);
    EFR_build_code (codvec, sign, cod, h, y, indx);
    for (i = 0; i < 10; i++)
    {
        EFR_q_p (&indx[i], i);
    }
    return;
}

/*************************************************************************
 *
 *  FUNCTION:  cor_h_x()
 *
 *  PURPOSE:  Computes correlation between target signal "x[]" and
 *            impulse response"h[]".
 *
 *  DESCRIPTION:
 *    The correlation is given by:
 *       d[n] = sum_{i=n}^{L-1} x[i] h[i-n]      n=0,...,L-1
 *
 *    d[n] is normalized such that the sum of 5 maxima of d[n] corresponding
 *    to each position track does not saturate.
 *
 *************************************************************************/
void EFR_cor_h_x (
    Word16 h[],     /* (i)   : impulse response of weighted synthesis filter */
    Word16 x[],     /* (i)   : target                                        */
    Word16 dn[]     /* (o)   : correlation between target and h[]            */
)
{
    Word16 i, j, k;
    Word32 s, y32[L_CODE], max, tot;

    /* first keep the result on 32 bits and find absolute maximum */

    tot = 5;

    for (k = 0; k < NB_TRACK; k++)
    {
        max = 0;
        for (i = k; i < L_CODE; i += STEP)
        {
            s = CODEC_OpVvMac(x + i, h, L_CODE - i, 0);
            y32[i] = s;

            s = L_abs (s);

            if (L_sub (s, max) > (Word32) 0L)
                max = s;
        }
        tot = L_add (tot, L_shr (max, 1));
    }

    j = sub (norm_l (tot), 2);                   /* multiply tot by 4 */

    for (i = 0; i < L_CODE; i++)
    {
        dn[i] = round (L_shl (y32[i], j));
    }
}

/*************************************************************************
 *
 *  FUNCTION  set_sign()
 *
 *  PURPOSE: Builds sign[] vector according to "dn[]" and "cn[]", and modifies
 *           dn[] to include the sign information (dn[i]=sign[i]*dn[i]).
 *           Also finds the position of maximum of correlation in each track
 *           and the starting position for each pulse.
 *
 *************************************************************************/

void EFR_set_sign (
    Word16 dn[],      /* (i/o): correlation between target and h[]         */
    Word16 cn[],      /* (i)  : residual after long term prediction        */
    Word16 sign[],    /* (o)  : sign of d[n]                               */
    Word16 pos_max[], /* (o)  : position of maximum correlation            */
    Word16 ipos[]     /* (o)  : starting position for each pulse           */
)
{
    Word16 i, j;
    Word16 val, cor, k_cn, k_dn, max, max_of_all, pos;
    Word16 en[L_CODE];                  /* correlation vector */
    Word32 s;

    /* calculate energy for normalization of cn[] and dn[] */

    s = CODEC_OpVvMac(cn, cn, L_CODE, 256);
    s = EFR_Inv_sqrt (s);
    k_cn = extract_h (L_shl (s, 5));

    s = CODEC_OpVvMac(dn, dn, L_CODE, 256);
    s = EFR_Inv_sqrt (s);
    k_dn = extract_h (L_shl (s, 5));

    for (i = 0; i < L_CODE; i++)
    {
        val = dn[i];
        cor = round (L_shl (L_mac (L_mult (k_cn, cn[i]), k_dn, val), 10));


        if (cor >= 0)
        {
            sign[i] = 32767;                     /* sign = +1 */
        }
        else
        {
            sign[i] = -32767;                    /* sign = -1 */
            cor = negate (cor);
            val = negate (val);
        }
        /* modify dn[] according to the fixed sign */
        dn[i] = val;
        en[i] = cor;
    }

    max_of_all = -1;
    for (i = 0; i < NB_TRACK; i++)
    {
        max = -1;

        for (j = i; j < L_CODE; j += STEP)
        {
            cor = en[j];
            val = sub (cor, max);

            if (val > 0)
            {
                max = cor;
                pos = j;
            }
        }
        /* store maximum correlation position */
        pos_max[i] = pos;
        val = sub (max, max_of_all);

        if (val > 0)
        {
            max_of_all = max;
            /* starting position for i0 */
            ipos[0] = i;
        }
    }

    /*----------------------------------------------------------------*
     *     Set starting position of each pulse.                       *
     *----------------------------------------------------------------*/

    pos = ipos[0];
    ipos[5] = pos;

    for (i = 1; i < NB_TRACK; i++)
    {
        pos = add (pos, 1);
        if (sub (pos, NB_TRACK) >= 0)
        {
            pos = 0;
        }
        ipos[i] = pos;
        ipos[i + 5] = pos;
    }
}

void EFR_q_p (
    Word16 *ind,        /* Pulse position */
    Word16 n            /* Pulse number   */
)
{
    static const Word16 gray[8] = {0, 1, 3, 2, 6, 4, 5, 7};
    Word16 tmp;

    tmp = *ind;


    if (sub (n, 5) < 0)
    {
        tmp = (tmp & 0x8) | gray[tmp & 0x7];

    }
    else
    {
        tmp = gray[tmp & 0x7];
    }

    *ind = tmp;
}

/*************************************************************************
 *
 *  FUNCTION:  cor_h()
 *
 *  PURPOSE:  Computes correlations of h[] needed for the codebook search;
 *            and includes the sign information into the correlations.
 *
 *  DESCRIPTION: The correlations are given by
 *         rr[i][j] = sum_{n=i}^{L-1} h[n-i] h[n-j];   i>=j; i,j=0,...,L-1
 *
 *  and the sign information is included by
 *         rr[i][j] = rr[i][j]*sign[i]*sign[j]
 *
 *************************************************************************/

void EFR_cor_h (
    Word16 h[],         /* (i) : impulse response of weighted synthesis
                                 filter                                  */
    Word16 sign[],      /* (i) : sign of d[n]                            */
    Word16 rr[][L_CODE] /* (o) : matrix of autocorrelation               */
)
{
    Word32 i, j, k;
    Word16 dec, h2[L_CODE];
    Word32 s;

    /* Scaling for maximum precision */

    s = CODEC_OpVvMac(h, h, L_CODE, 2);
    j = sub (extract_h (s), 32767);

    if (j == 0)
    {
        CODEC_OpVecShr(h, L_CODE, 1, h2);
    }
    else
    {
        s = L_shr (s, 1);
        k = extract_h (L_shl (EFR_Inv_sqrt (s), 7));
        k = mult (k, 32440);                     /* k = 0.99*k */

        for (i = 0; i < L_CODE; i++)
        {
            h2[i] = round (L_shl (L_mult (h[i], k), 9));

        }
    }

    /* build matrix rr[] */
    s = 0;
    i = L_CODE - 1;
    for (k = 0; k < L_CODE; k++, i--)
    {
        s = L_mac (s, h2[k], h2[k]);
        rr[i][i] = round (s);
    }

    for (dec = 1; dec < L_CODE; dec++)
    {
        s = 0;
        j = L_CODE - 1;
        i = sub (j, dec);
        for (k = 0; k < (L_CODE - dec); k++, i--, j--)
        {
            s = L_mac (s, h2[k], h2[k + dec]);
            rr[j][i] = mult (round (s), mult (sign[i], sign[j]));

            rr[i][j] = rr[j][i];
        }
    }
}

/*************************************************************************
 *
 *  FUNCTION  search_10i40()
 *
 *  PURPOSE: Search the best codevector; determine positions of the 10 pulses
 *           in the 40-sample frame.
 *
 *************************************************************************/

#define _1_2    (Word16)(32768L/2)
#define _1_4    (Word16)(32768L/4)
#define _1_8    (Word16)(32768L/8)
#define _1_16   (Word16)(32768L/16)
#define _1_32   (Word16)(32768L/32)
#define _1_64   (Word16)(32768L/64)
#define _1_128  (Word16)(32768L/128)

void EFR_search_10i40 (
    Word16 dn[],         /* (i) : correlation between target and h[]        */
    Word16 rr[][L_CODE], /* (i) : matrix of autocorrelation                 */
    Word16 ipos[],       /* (i) : starting position for each pulse          */
    Word16 pos_max[],    /* (i) : position of maximum of dn[]               */
    Word16 codvec[]      /* (o) : algebraic codebook vector                 */
)
{
    Word32 i0, i1, i2, i3, i4, i5, i6, i7, i8, i9;
    Word16 i, j, k, pos, ia, ib;
    Word16 psk, ps, ps0, ps1, ps2, sq, sq2;
    Word16 alpk, alp, alp_16;
    Word16 rrv[L_CODE];
    Word32 s, alp0, alp1, alp2;

    /* fix i0 on maximum of correlation position */

    i0 = pos_max[ipos[0]];

    /*------------------------------------------------------------------*
     * i1 loop:                                                         *
     *------------------------------------------------------------------*/

    /* Default value */
    psk = -1;
    alpk = 1;
    for (i = 0; i < NB_PULSE; i++)
    {
        codvec[i] = i;
    }

    for (i = 1; i < NB_TRACK; i++)
    {
        i1 = pos_max[ipos[1]];
        ps0 = add (dn[i0], dn[i1]);
        alp0 = L_mult (rr[i0][i0], _1_16);
        alp0 = L_mac (alp0, rr[i1][i1], _1_16);
        alp0 = L_mac (alp0, rr[i0][i1], _1_8);

        /*----------------------------------------------------------------*
         * i2 and i3 loop:                                                *
         *----------------------------------------------------------------*/

        /* initialize 4 indices for next loop. */
        /* initialize "rr[i3][i3]" pointer */
        /* initialize "rr[i0][i3]" pointer */
        /* initialize "rr[i1][i3]" pointer */
        /* initialize "rrv[i3]" pointer    */

        for (i3 = ipos[3]; i3 < L_CODE; i3 += STEP)
        {
            s = L_mult (rr[i3][i3], _1_8);       /* index incr= STEP+L_CODE */
            s = L_mac (s, rr[i0][i3], _1_4);     /* index increment = STEP  */
            s = L_mac (s, rr[i1][i3], _1_4);     /* index increment = STEP  */
            rrv[i3] = round (s);
        }

        /* Default value */
        sq = -1;
        alp = 1;
        ps = 0;
        ia = ipos[2];
        ib = ipos[3];

        /* initialize 4 indices for i2 loop. */
        /* initialize "dn[i2]" pointer     */
        /* initialize "rr[i2][i2]" pointer */
        /* initialize "rr[i0][i2]" pointer */
        /* initialize "rr[i1][i2]" pointer */

        for (i2 = ipos[2]; i2 < L_CODE; i2 += STEP)
        {
            /* index increment = STEP  */
            ps1 = add (ps0, dn[i2]);

            /* index incr= STEP+L_CODE */
            alp1 = L_mac (alp0, rr[i2][i2], _1_16);
            /* index increment = STEP  */
            alp1 = L_mac (alp1, rr[i0][i2], _1_8);
            /* index increment = STEP  */
            alp1 = L_mac (alp1, rr[i1][i2], _1_8);

            /* initialize 3 indices for i3 inner loop */
            /* initialize "dn[i3]" pointer     */
            /* initialize "rrv[i3]" pointer    */
            /* initialize "rr[i2][i3]" pointer */

            for (i3 = ipos[3]; i3 < L_CODE; i3 += STEP)
            {
                /* index increment = STEP */
                ps2 = add (ps1, dn[i3]);

                /* index increment = STEP */
                alp2 = L_mac (alp1, rrv[i3], _1_2);
                /* index increment = STEP */
                alp2 = L_mac (alp2, rr[i2][i3], _1_8);

                sq2 = mult (ps2, ps2);

                alp_16 = round (alp2);

                s = L_msu (L_mult (alp, sq2), sq, alp_16);


                if (s > 0)
                {
                    sq = sq2;
                    ps = ps2;
                    alp = alp_16;
                    ia = i2;
                    ib = i3;
                }
            }
        }
        i2 = ia;
        i3 = ib;

        /*----------------------------------------------------------------*
         * i4 and i5 loop:                                                *
         *----------------------------------------------------------------*/

        ps0 = ps;
        alp0 = L_mult (alp, _1_2);

        /* initialize 6 indices for next loop (see i2-i3 loop) */


        for (i5 = ipos[5]; i5 < L_CODE; i5 += STEP)
        {
            s = L_mult (rr[i5][i5], _1_8);
            s = L_mac (s, rr[i0][i5], _1_4);
            s = L_mac (s, rr[i1][i5], _1_4);
            s = L_mac (s, rr[i2][i5], _1_4);
            s = L_mac (s, rr[i3][i5], _1_4);
            rrv[i5] = round (s);
        }

        /* Default value */
        sq = -1;
        alp = 1;
        ps = 0;
        ia = ipos[4];
        ib = ipos[5];

        /* initialize 6 indices for i4 loop (see i2-i3 loop) */


        for (i4 = ipos[4]; i4 < L_CODE; i4 += STEP)
        {
            ps1 = add (ps0, dn[i4]);

            alp1 = L_mac (alp0, rr[i4][i4], _1_32);
            alp1 = L_mac (alp1, rr[i0][i4], _1_16);
            alp1 = L_mac (alp1, rr[i1][i4], _1_16);
            alp1 = L_mac (alp1, rr[i2][i4], _1_16);
            alp1 = L_mac (alp1, rr[i3][i4], _1_16);

            /* initialize 3 indices for i5 inner loop (see i2-i3 loop) */


            for (i5 = ipos[5]; i5 < L_CODE; i5 += STEP)
            {
                ps2 = add (ps1, dn[i5]);

                alp2 = L_mac (alp1, rrv[i5], _1_4);
                alp2 = L_mac (alp2, rr[i4][i5], _1_16);

                sq2 = mult (ps2, ps2);

                alp_16 = round (alp2);

                s = L_msu (L_mult (alp, sq2), sq, alp_16);


                if (s > 0)
                {
                    sq = sq2;
                    ps = ps2;
                    alp = alp_16;
                    ia = i4;
                    ib = i5;
                }
            }
        }
        i4 = ia;
        i5 = ib;

        /*----------------------------------------------------------------*
         * i6 and i7 loop:                                                *
         *----------------------------------------------------------------*/

        ps0 = ps;
        alp0 = L_mult (alp, _1_2);

        /* initialize 8 indices for next loop (see i2-i3 loop) */



        for (i7 = ipos[7]; i7 < L_CODE; i7 += STEP)
        {
            s = L_mult (rr[i7][i7], _1_16);
            s = L_mac (s, rr[i0][i7], _1_8);
            s = L_mac (s, rr[i1][i7], _1_8);
            s = L_mac (s, rr[i2][i7], _1_8);
            s = L_mac (s, rr[i3][i7], _1_8);
            s = L_mac (s, rr[i4][i7], _1_8);
            s = L_mac (s, rr[i5][i7], _1_8);
            rrv[i7] = round (s);
        }

        /* Default value */
        sq = -1;
        alp = 1;
        ps = 0;
        ia = ipos[6];
        ib = ipos[7];

        /* initialize 8 indices for i6 loop (see i2-i3 loop) */



        for (i6 = ipos[6]; i6 < L_CODE; i6 += STEP)
        {
            ps1 = add (ps0, dn[i6]);

            alp1 = L_mac (alp0, rr[i6][i6], _1_64);
            alp1 = L_mac (alp1, rr[i0][i6], _1_32);
            alp1 = L_mac (alp1, rr[i1][i6], _1_32);
            alp1 = L_mac (alp1, rr[i2][i6], _1_32);
            alp1 = L_mac (alp1, rr[i3][i6], _1_32);
            alp1 = L_mac (alp1, rr[i4][i6], _1_32);
            alp1 = L_mac (alp1, rr[i5][i6], _1_32);

            /* initialize 3 indices for i7 inner loop (see i2-i3 loop) */


            for (i7 = ipos[7]; i7 < L_CODE; i7 += STEP)
            {
                ps2 = add (ps1, dn[i7]);

                alp2 = L_mac (alp1, rrv[i7], _1_4);
                alp2 = L_mac (alp2, rr[i6][i7], _1_32);

                sq2 = mult (ps2, ps2);

                alp_16 = round (alp2);

                s = L_msu (L_mult (alp, sq2), sq, alp_16);


                if (s > 0)
                {
                    sq = sq2;
                    ps = ps2;
                    alp = alp_16;
                    ia = i6;
                    ib = i7;
                }
            }
        }
        i6 = ia;
        i7 = ib;

        /*----------------------------------------------------------------*
         * i8 and i9 loop:                                                *
         *----------------------------------------------------------------*/

        ps0 = ps;
        alp0 = L_mult (alp, _1_2);

        /* initialize 10 indices for next loop (see i2-i3 loop) */



        for (i9 = ipos[9]; i9 < L_CODE; i9 += STEP)
        {
            s = L_mult (rr[i9][i9], _1_16);
            s = L_mac (s, rr[i0][i9], _1_8);
            s = L_mac (s, rr[i1][i9], _1_8);
            s = L_mac (s, rr[i2][i9], _1_8);
            s = L_mac (s, rr[i3][i9], _1_8);
            s = L_mac (s, rr[i4][i9], _1_8);
            s = L_mac (s, rr[i5][i9], _1_8);
            s = L_mac (s, rr[i6][i9], _1_8);
            s = L_mac (s, rr[i7][i9], _1_8);
            rrv[i9] = round (s);
        }

        /* Default value */
        sq = -1;
        alp = 1;
        ps = 0;
        ia = ipos[8];
        ib = ipos[9];

        /* initialize 10 indices for i8 loop (see i2-i3 loop) */



        for (i8 = ipos[8]; i8 < L_CODE; i8 += STEP)
        {
            ps1 = add (ps0, dn[i8]);

            alp1 = L_mac (alp0, rr[i8][i8], _1_128);
            alp1 = L_mac (alp1, rr[i0][i8], _1_64);
            alp1 = L_mac (alp1, rr[i1][i8], _1_64);
            alp1 = L_mac (alp1, rr[i2][i8], _1_64);
            alp1 = L_mac (alp1, rr[i3][i8], _1_64);
            alp1 = L_mac (alp1, rr[i4][i8], _1_64);
            alp1 = L_mac (alp1, rr[i5][i8], _1_64);
            alp1 = L_mac (alp1, rr[i6][i8], _1_64);
            alp1 = L_mac (alp1, rr[i7][i8], _1_64);

            /* initialize 3 indices for i9 inner loop (see i2-i3 loop) */


            for (i9 = ipos[9]; i9 < L_CODE; i9 += STEP)
            {
                ps2 = add (ps1, dn[i9]);

                alp2 = L_mac (alp1, rrv[i9], _1_8);
                alp2 = L_mac (alp2, rr[i8][i9], _1_64);

                sq2 = mult (ps2, ps2);

                alp_16 = round (alp2);

                s = L_msu (L_mult (alp, sq2), sq, alp_16);


                if (s > 0)
                {
                    sq = sq2;
                    ps = ps2;
                    alp = alp_16;
                    ia = i8;
                    ib = i9;
                }
            }
        }

        /*----------------------------------------------------------------*
         * memorise codevector if this one is better than the last one.   *
         *----------------------------------------------------------------*/

        s = L_msu (L_mult (alpk, sq), psk, alp);


        if (s > 0)
        {
            psk = sq;
            alpk = alp;
            codvec[0] = i0;
            codvec[1] = i1;
            codvec[2] = i2;
            codvec[3] = i3;
            codvec[4] = i4;
            codvec[5] = i5;
            codvec[6] = i6;
            codvec[7] = i7;
            codvec[8] = ia;
            codvec[9] = ib;
        }
        /*----------------------------------------------------------------*
         * Cyclic permutation of i1,i2,i3,i4,i5,i6,i7,i8 and i9.          *
         *----------------------------------------------------------------*/

        pos = ipos[1];
        for (j = 1, k = 2; k < NB_PULSE; j++, k++)
        {
            ipos[j] = ipos[k];
        }

        ipos[NB_PULSE - 1] = pos;
}
}

/*************************************************************************
 *
 *  FUNCTION:  build_code()
 *
 *  PURPOSE: Builds the codeword, the filtered codeword and index of the
 *           codevector, based on the signs and positions of 10 pulses.
 *
 *************************************************************************/

void EFR_build_code (
    Word16 codvec[], /* (i)  : position of pulses                           */
    Word16 sign[],   /* (i)  : sign of d[n]                                 */
    Word16 cod[],    /* (o)  : innovative code vector                       */
    Word16 h[],      /* (i)  : impulse response of weighted synthesis filter*/
    Word16 y[],      /* (o)  : filtered innovative code                     */
    Word16 indx[]    /* (o)  : index of 10 pulses (sign+position)           */
)
{
    Word16 i, j, k, track, index, _sign[NB_PULSE];
    Word16 *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;
    Word32 s;

    CODEC_OpVecSet(cod, L_CODE, 0);
    CODEC_OpVecSet(indx, NB_TRACK, -1);

    for (k = 0; k < NB_PULSE; k++)
    {
        /* read pulse position */
        i = codvec[k];
        /* read sign           */
        j = sign[i];

        index = mult (i, 6554);                  /* index = pos/5       */
        /* track = pos%5 */
        track = sub (i, extract_l (L_shr (L_mult (index, 5), 1)));

        if (j > 0)
        {
            cod[i] = add (cod[i], 4096);
            _sign[k] = 8192;

        }
        else
        {
            cod[i] = sub (cod[i], 4096);
            _sign[k] = -8192;
            index = add (index, 8);
        }


        if (indx[track] < 0)
        {
            indx[track] = index;
        }
        else
        {

            if (((index ^ indx[track]) & 8) == 0)
            {
                /* sign of 1st pulse == sign of 2nd pulse */


                if (sub (indx[track], index) <= 0)
                {
                    indx[track + 5] = index;
                }
                else
                {
                    indx[track + 5] = indx[track];

                    indx[track] = index;
                }
            }
            else
            {
                /* sign of 1st pulse != sign of 2nd pulse */


                if (sub ((indx[track] & 7), (index & 7)) <= 0)
                {
                    indx[track + 5] = indx[track];

                    indx[track] = index;
                }
                else
                {
                    indx[track + 5] = index;
                }
            }
        }
    }

    p0 = h - codvec[0];
    p1 = h - codvec[1];
    p2 = h - codvec[2];
    p3 = h - codvec[3];
    p4 = h - codvec[4];
    p5 = h - codvec[5];
    p6 = h - codvec[6];
    p7 = h - codvec[7];
    p8 = h - codvec[8];
    p9 = h - codvec[9];

    for (i = 0; i < L_CODE; i++)
    {
        s = 0;
        s = L_mac (s, *p0++, _sign[0]);
        s = L_mac (s, *p1++, _sign[1]);
        s = L_mac (s, *p2++, _sign[2]);
        s = L_mac (s, *p3++, _sign[3]);
        s = L_mac (s, *p4++, _sign[4]);
        s = L_mac (s, *p5++, _sign[5]);
        s = L_mac (s, *p6++, _sign[6]);
        s = L_mac (s, *p7++, _sign[7]);
        s = L_mac (s, *p8++, _sign[8]);
        s = L_mac (s, *p9++, _sign[9]);
        y[i] = round (s);
    }
}

