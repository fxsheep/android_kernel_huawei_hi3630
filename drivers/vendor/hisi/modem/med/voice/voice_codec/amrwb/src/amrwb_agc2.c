/*------------------------------------------------------------------------*
 *                         AGC2.C                                         *
 *------------------------------------------------------------------------*
 * Performs adaptive gain control                                         *
 *------------------------------------------------------------------------*/
#ifdef _MED_C89_

#include <stdio.h>
#include <stdlib.h>

#endif /* _MED_C89_ */

#include "amrwb_cnst.h"
#include "amrwb_acelp.h"
#include "amrwb_math_op.h"

void AMRWB_agc2(
     Word16 * sig_in,                      /* (i)     : postfilter input signal  */
     Word16 * sig_out,                     /* (i/o)   : postfilter output signal */
     Word16 l_trm                          /* (i)     : subframe size            */
)
{

    Word16 exp;
    Word16 gain_in, gain_out, g0;
    Word32 s, i;

    Word16 temp;

    /* calculate gain_out with exponent */

    temp = shr(sig_out[0], 2);
    s = L_mult(temp, temp);
    for (i = 1; i < l_trm; i++)
    {
        temp = shr(sig_out[i], 2);
        s = L_mac(s, temp, temp);
    }

    if (s == 0)
    {
        return;
    }
    exp = sub(norm_l(s), 1);
    gain_out = round(L_shl(s, exp));

    /* calculate gain_in with exponent */

    temp = shr(sig_in[0], 2);
    s = L_mult(temp, temp);
    for (i = 1; i < l_trm; i++)
    {
        temp = shr(sig_in[i], 2);
        s = L_mac(s, temp, temp);
    }

    if (s == 0)
    {
        g0 = 0;
    } else
    {
        i = norm_l(s);
        gain_in = round(L_shl(s, i));
        exp = sub(exp, i);

        /*---------------------------------------------------*
         *  g0 = sqrt(gain_in/gain_out);                     *
         *---------------------------------------------------*/

        s = L_deposit_l(div_s(gain_out, gain_in));
        s = L_shl(s, 7);                   /* s = gain_out / gain_in */
        s = L_shr(s, exp);                 /* add exponent */

        s = Isqrt(s);
        g0 = round(L_shl(s, 9));
    }
    /* sig_out(n) = gain(n) sig_out(n) */

    for (i = 0; i < l_trm; i++)
    {
        sig_out[i] = extract_h(L_shl(L_mult(sig_out[i], g0), 2));
    }

    return;
}
