
/*****************************************************************************
   1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_op_float.h"
#include "codec_op_etsi.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
   2 ȫ�ֱ�������
*****************************************************************************/
const CODEC_OP_FLOAT_STRU                 CODEC_OP_FLOAT_ZERO  = {0, 0};            /* α����0 */
const CODEC_OP_FLOAT_STRU                 CODEC_OP_FLOAT_ONE   = {16384,-14};       /* α����1 */
const CODEC_OP_FLOAT_STRU                 CODEC_OP_FLOAT_MAX16 = {16384,1};         /* α����32768 */

/*****************************************************************************
   3.1  α������㺯��
*****************************************************************************/


 CODEC_OP_FLOAT_STRU CODEC_OpFloatSqrt(CODEC_OP_FLOAT_STRU stX)
{
    CODEC_OP_FLOAT_STRU                   stRet = {0, 0};
    VOS_INT32                           swM   = 0;

    if (stX.shwM < 0)
    {
        return stRet;
    }

    swM         = CODEC_OpShl(stX.shwM, CODEC_OP_Q14);
    stRet.shwE  = stX.shwE - CODEC_OP_Q14;

    //if ((stRet.shwE&1) != 0)
    if (1 == CODEC_OpMod32(((VOS_INT32)stRet.shwE), 2))
    {
        stRet.shwE -= 1;
        swM = CODEC_OpL_shl(swM, 1);
    }

    /* �������� */
    stRet.shwE = CODEC_OpShr(stRet.shwE, 1);
    stRet.shwM = (VOS_INT16)CODEC_OpSqrt(swM);

    return stRet;

}
VOS_INT16 CODEC_OpFloatLT(CODEC_OP_FLOAT_STRU stA, CODEC_OP_FLOAT_STRU stB)
{
    VOS_INT16                           shwExp              = 0;

    /* ��0���⴦�� */
    if (0 == stA.shwM)
    {
        return (stB.shwM > 0);
    }

    if (0 == stB.shwM)
    {
        return (stA.shwM < 0);
    }

    /* ��0���� */
    if (stA.shwE > stB.shwE)
    {
        shwExp   = CODEC_OpMin((stA.shwE - stB.shwE) + 1, CODEC_OP_Q15);
        return (CODEC_OpShr_r(stA.shwM, 1) < CODEC_OpShr_r(stB.shwM, shwExp));
    }
    else
    {
        shwExp   = CODEC_OpMin((stB.shwE - stA.shwE) + 1, CODEC_OP_Q15);
        return (CODEC_OpShr_r(stB.shwM, 1) > CODEC_OpShr_r(stA.shwM, shwExp));
    }

}


CODEC_OP_FLOAT_STRU CODEC_OpFloatAdd(
                CODEC_OP_FLOAT_STRU stA,
                CODEC_OP_FLOAT_STRU stB)
{
    CODEC_OP_FLOAT_STRU                   stRet               = {0, 0};
    VOS_INT16                           shwExp              = 0;

    /* ��0���⴦�� */
    if (0 == stA.shwM)
    {
        return stB;
    }

    if (0 == stB.shwM)
    {
        return stA;
    }

    if (stA.shwE > stB.shwE)
    {
        shwExp     = CODEC_OpMin((stA.shwE - stB.shwE) + 1, CODEC_OP_Q15);
        stRet.shwM = CODEC_OpAdd(CODEC_OpShr_r(stA.shwM, 1), CODEC_OpShr_r(stB.shwM, shwExp));
        stRet.shwE = stA.shwE + 1;
    }
    else
    {
        shwExp     = CODEC_OpMin((stB.shwE - stA.shwE) + 1, CODEC_OP_Q15);
        stRet.shwM = CODEC_OpAdd(CODEC_OpShr_r(stB.shwM, 1), CODEC_OpShr_r(stA.shwM, shwExp));
        stRet.shwE = stB.shwE + 1;
    }

    /* ��һ���� */
    if (CODEC_OpAbs_s(stRet.shwM) < CODEC_OP_INT16_HALF)
    {
        stRet.shwM = CODEC_OpShl(stRet.shwM, 1);
        stRet.shwE -= 1;
    }

    return stRet;

}


CODEC_OP_FLOAT_STRU CODEC_OpFloatSub(
                CODEC_OP_FLOAT_STRU stA,
                CODEC_OP_FLOAT_STRU stB)
{
    CODEC_OP_FLOAT_STRU                   stRet  = {0, 0};
    VOS_INT16                           shwExp = 0;

    /* ��0���⴦�� */
    if (0 == stA.shwM)
    {
        stRet.shwE = stB.shwE;
        stRet.shwM = CODEC_OpNegate(stB.shwM);
        return stRet;
    }

    if (0 == stB.shwM)
    {
        return stA;
    }

    if (stA.shwE > stB.shwE)
    {
        shwExp     = CODEC_OpMin((stA.shwE - stB.shwE) + 1, CODEC_OP_Q15);
        stRet.shwM = CODEC_OpShr_r(stA.shwM, 1) - CODEC_OpShr_r(stB.shwM, shwExp);
        stRet.shwE = stA.shwE + 1;
    }
    else
    {
        shwExp     = CODEC_OpMin((stB.shwE - stA.shwE) + 1, CODEC_OP_Q15);
        stRet.shwM = CODEC_OpShr_r(stA.shwM, shwExp) - CODEC_OpShr_r(stB.shwM, 1);
        stRet.shwE = stB.shwE + 1;
    }

    /* ��һ���� */
    if (CODEC_OpAbs_s(stRet.shwM) < CODEC_OP_INT16_HALF)
    {
        stRet.shwM = CODEC_OpShl(stRet.shwM, 1);
        stRet.shwE -= 1;
    }

    return stRet;
}



#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif
