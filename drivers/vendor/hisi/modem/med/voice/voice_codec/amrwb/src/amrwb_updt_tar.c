/*-------------------------------------------------------------------*
 *                         UPD_TAR.C                                 *
 *-------------------------------------------------------------------*
 * Update the target vector for codebook search.                     *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"

void Updt_tar(
     Word16 * x,                           /* (i) Q0  : old target (for pitch search)     */
     Word16 * x2,                          /* (o) Q0  : new target (for codebook search)  */
     Word16 * y,                           /* (i) Q0  : filtered adaptive codebook vector */
     Word16 gain,                          /* (i) Q14 : adaptive codebook gain            */
     Word16 L                              /* (i)     : subframe size                     */
)
{
    Word32 i, L_tmp;

    for (i = 0; i < L; i++)
    {
        L_tmp = L_mult(x[i], 16384);
        L_tmp = L_msu(L_tmp, y[i], gain);
        x2[i] = extract_h(L_shl(L_tmp, 1));
    }

    return;
}
