/*-----------------------------------------------------------------------*
 *                         PIT_SHRP.C                                    *
 *-----------------------------------------------------------------------*
 * Performs Pitch sharpening routine                                     *
 *-----------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"

void Pit_shrp(
     Word16 * x,                           /* in/out: impulse response (or algebraic code) */
     Word16 pit_lag,                       /* input : pitch lag                            */
     Word16 sharp,                         /* input : pitch sharpening factor (Q15)        */
     Word16 L_subfr                        /* input : subframe size                        */
)
{
    Word32 i, L_tmp;

    for (i = pit_lag; i < L_subfr; i++)
    {
        L_tmp = L_deposit_h(x[i]);
        L_tmp = L_mac(L_tmp, x[i - pit_lag], sharp);
        x[i] = round(L_tmp);

    }

    return;
}
