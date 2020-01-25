/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : med_com_vec_op.c
  �� �� ��   : ����
  ��    ��   : ��
  ��������   : 2011��8��20��
  ����޸�   :
  ��������   : ���������������㺯����׼Cʵ��
  �����б�   :CODEC_OpVcAnd
              CODEC_OpVcMac
              CODEC_OpVcMultQ15Add
              CODEC_OpVcMultR
              CODEC_OpVcMultScale
              CODEC_OpVecCpy
              CODEC_OpVecMax
              CODEC_OpVecMaxAbs
              CODEC_OpVecMin
              CODEC_OpVecScaleMac
              CODEC_OpVecSet
              CODEC_OpVecShl
              CODEC_OpVecShr
              CODEC_OpVecSum
              CODEC_OpVvAdd
              CODEC_OpVvAdd32
              CODEC_OpVvFormWithCoef
              CODEC_OpVvFormWithDimi
              CODEC_OpVvFormWithQuar
              CODEC_OpVvMac
              CODEC_OpVvMsu
              CODEC_OpVvSub

  �޸���ʷ   :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_vec.h"
#include "codec_typedefine.h"
#include "codec_op_netsi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/* ��׼C������ʹ�� */
#ifdef _MED_C89_
/*****************************************************************************
 �� �� ��  : CODEC_OpVcAnd
 ��������  : Y[i] = X[i]&Const
 �������  : Word16 *pshwX    - ����
             Word32 swLen     - ��������
             Word16 shwConst  - ����
             Word16 *pshwY    - ���������������
 �������  : Word16 *pshwY    - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVcAnd(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16 *pshwY)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwY[i] = pshwX[i] & shwConst;
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVcMac
 ��������  : Sum = L_mac(sum, X[i], Const) = Sum + (X[i]*Const)<<1
 �������  : Word16 *pshwX    - ����
             Word32 swLen     - ��������
             Word16 shwConst  - ����
             Word32 swSum     - �ۼƺͳ�ʼֵ
 �������  : ��
 �� �� ֵ  : Word32           - �ۼӺ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word32 CODEC_OpVcMac(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word32  swSum)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_mac(swSum, pshwX[i], shwConst);
    }

    return swSum;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVcMultQ15Add
 ��������  : Z[i] = add(X[i], mult(Y[i],const)) = X[i] + (Y[i]*Const)>>15
 �������  : Word16 *pshwX      - �����ӵ�����
             Word16 *pshwY      - �볣����˵�����
             Word32  swLen      - ��������
             Word16  shwConst   - ����
             Word16 *pshwZ      - ������������
 �������  : Word16 *pshwZ      - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVcMultQ15Add(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                const Word16  shwConst,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(pshwX[i], mult(pshwY[i], shwConst));
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVcMultScale
 ��������  : Y[i] = L_mult(X[i], const) >> ScaleBIt�� ScaleBIt>=0
 �������  : Word16 *pshwX         - ��������
             Word32  swLen         - ��������
             Word16  shwConst      - ����
             Word16  shwScaleBit   - ����bit��
             Word16 *pshwY         - ������������
 �������  : Word16 *pshwY         - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVcMultScale(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16        shwScaleBit,
                Word16       *pshwY)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwY[i] = saturate(L_shr(L_mult(pshwX[i], shwConst), shwScaleBit));
    }
}

/*****************************************************************************
�� �� ��  : CODEC_OpVcMult
��������  : Z[i] = mult(X[i], k)
�������  : Word16 *pshwX      - ����������
            Word32  swLen      - ��������
            Word16  shwConst   - ��������
            Word16 *pshwZ      - ������������
�������  : Word16 *pshwZ      - ������
�� �� ֵ  : void
���ú���  :
��������  :

�޸���ʷ      :
1.��    ��   : 2011��8��20��
��    ��   : ��
�޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVcMult(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = mult(pshwX[i], shwConst);
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVcMultR
 ��������  : Z[i] = round(mult(X[i], k))
 �������  : Word16 *pshwX      - ����������
             Word32  swLen      - ��������
             Word16  shwConst   - ��������
             Word16 *pshwZ      - ������������
 �������  : Word16 *pshwZ      - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVcMultR(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = mult_r(pshwX[i], shwConst);
    }
}


VOS_VOID CODEC_OpVcSub(
                const Word16           *pshwX,
                const Word32            swLen,
                const Word16            shwY,
                Word16                 *pshwZ)
{
    Word32      swCnt = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pshwZ[swCnt] = sub(pshwX[swCnt], shwY);
    }
}


VOS_VOID CODEC_OpVecAbs(
                      Word16           *pshwX,
                      Word32            swLen,
                      Word16           *pshwY)
{
    Word32      swCnt;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pshwY[swCnt] = abs_s(pshwX[swCnt]);
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecCpy
 ��������  : Des[i] =  Src[i]���������ƣ�Src��Des�����ڴ���ظ�
 �������  : Word16 *pshwDes    - Ŀ������
             Word16 *pshwSrc    - Դ����
             Word32 swLen       - ��������
 �������  : Word16 *pshwDes    - ���ƽ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVecCpy(
                Word16       *pshwDes,
                const Word16 *pshwSrc,
                const Word32  swLen)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwDes[i] = pshwSrc[i];
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecMax
 ��������  : Ѱ������Ԫ�����ֵ
 �������  : Word16 *pshwX       - ��������
             Word32 swLen        - ��������
             Word16 *pswMaxPos   - �������ֵλ�õĵ�Ԫ
 �������  : Word16 *pswMaxPos   - ���ֵλ��
 �� �� ֵ  : ���ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 CODEC_OpVecMax(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16       *pswMaxPos)
{
    Word32 i        = 0;
    Word16 shwMax   = 0;
    Word16 shwMaxId = 0;

    shwMax = pshwX[0];

    for (i = 1; i < swLen; i++)
    {
        if (shwMax <= pshwX[i])
        {
            shwMax   = pshwX[i];
            shwMaxId = (Word16)i;
        }
    }

    if (pswMaxPos != 0)
    {
        *pswMaxPos = shwMaxId;
    }

    return shwMax;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecMaxAbs
 ��������  : Y= max(abs(X[i]))
 �������  : Word16 *pshwX       - ��������
             Word32 swLen        - ��������
             Word16 *pswMaxPos   - ����������ֵԪ������λ��
 �������  : Word16  *pswMaxPos  - ������ֵԪ������λ��
 �� �� ֵ  : Word16 ��������������ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 CODEC_OpVecMaxAbs(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16       *pswMaxPos)
{
    Word32 i        = 0;
    Word16 shwMax   = 0;
    Word16 shwMaxId = 0;
    Word16 shwTemp  = 0;

    shwMax = abs_s(pshwX[0]);

    for (i = 1; i < swLen; i++)
    {
        shwTemp = abs_s(pshwX[i]);

        if (shwMax <= shwTemp)
        {
            shwMax   = shwTemp;
            shwMaxId = (Word16)i;
        }
    }

    if (pswMaxPos != 0)
    {
        *pswMaxPos = shwMaxId;
    }

    return shwMax;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecMin
 ��������  : Ѱ������Ԫ����Сֵ
 �������  : Word16 *pshwX        - ��������
             Word32 swLen         - ��������
             Word16 *pswMinPos    - ������Сֵλ�õĵ�Ԫ
 �������  : Word16 *pswMinPos    - ��Сֵλ��
 �� �� ֵ  : Word16 ��Сֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 CODEC_OpVecMin(
                const Word16 *pshwX,
                const Word32 swLen,
                Word16      *pswMinPos)
{
    Word32 i        = 0;
    Word16 shwMin   = 0;
    Word16 shwMinId = 0;

    shwMin = pshwX[0];

    for (i = 1; i < swLen; i++)
    {
        if (shwMin >= pshwX[i])
        {
            shwMin   = pshwX[i];
            shwMinId = (Word16)i;
        }
    }

    if (pswMinPos != 0)
    {
        *pswMinPos = shwMinId;
    }

    return shwMin;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecSet
 ��������  : ������ֵ, Des[i] = Value
 �������  : Word16 *pshwDes     - ����ֵ����
             Word32 swLen        - ��������
             Word16 pshwValue    - ��ֵĿ��ֵ
 �������  : Word16 *pshwDes     - ��ֵ���
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVecSet(
                Word16       *pshwDes,
                const Word32  swLen,
                const Word16  shwValue)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwDes[i] = shwValue;
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecShl
 ��������  : Y[i] = shl(X[i], Bits)
 �������  : Word16 *pshwX     - ����λ����
             Word32 swLen      - ��������
             Word16 shwBits    - ��λ��
             Word16 *pshwY     - ������������
 �������  : Word16 *pshwY     - ��λ���
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVecShl(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16        shwBits,
                Word16       *pshwY)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwY[i] = shl(pshwX[i], shwBits);
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecShr
 ��������  : Y[i] = shr(X[i], Bits)
 �������  : Word16 *pshwX      - ����λ����
             Word32 swLen       - ��������
             Word16 shwBits     - ��λ��
             Word16 *pshwY      - ������������
 �������  : Word16 *pshwY      - ��λ���
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVecShr(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16        shwBits,
                Word16       *pshwY)
{
    Word32 i = 0;

    if (shwBits< 0)
    {
        return;
    }

    for (i = 0; i < swLen; i++)
    {
        pshwY[i] = shr(pshwX[i], shwBits);
    }
}


VOS_VOID CODEC_OpVecShr_r(
                Word16 *pshwX,
                Word32  swLen,
                Word16  shwBits,
                Word16 *pshwY)
{
    Word32      swCnt = 0;

    if (shwBits< 0)
    {
        return;
    }

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pshwY[swCnt] = shr_r(pshwX[swCnt], shwBits);
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecSum
 ��������  : ��������Ԫ���ۼӺ�
 �������  : Word16 *pshwX   - ����
             Word32 swLen    - ��������
 �������  : ��
 �� �� ֵ  : Word16 �ۼӺ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 CODEC_OpVecSum(
                const Word16 *pshwX,
                const Word32  swLen)
{
    Word32 i      = 0;
    Word16 shwSum = 0;

    for (i = 0; i < swLen; i++)
    {
        shwSum = add(shwSum, pshwX[i]);
    }

    return shwSum;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVecScaleMac
 ��������  : temp = shr(X[i], bits) �� Bits>=0
             sum = L_mac(sum, temp, temp)
 �������  : Word16 *pshwX    - ����
             Word32 swLen     - ��������
             Word32 swSum     - �ۼӺͳ�ʼֵ
             Word16 shwBits   - ��������λ��
 �������  : ��
 �� �� ֵ  : Word32  �ۼӽ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word32 CODEC_OpVecScaleMac(
                const Word16 *pshwX,
                const Word32  swLen,
                Word32        swSum,
                Word16        shwBits)
{
    Word32 i       = 0;
    Word16 shwTemp = 0;

    for (i = 0; i < swLen; i++)
    {
        shwTemp = shr(pshwX[i], shwBits);

        swSum   = L_mac(swSum, shwTemp, shwTemp);
    }

    return swSum;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvAdd
 ��������  : Z[i] = add(X[i], Y[i])
 �������  : Word16 *pshwX    - ����������
             Word16 *pshwY    - ��������
             Word32 swLen     - ��������
             Word16 *pshwZ    - ������������
 �������  : Word16 *pshwZ    - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVvAdd(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(pshwX[i], pshwY[i]);
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvAdd32
 ��������  : Z[i] = L_add(X[i], Y[i])
 �������  : Word32 *pshwX    - ����������
             Word32 *pshwY    - ��������
             Word32 swLen     - ��������
             Word32 *pshwZ    - ������������
 �������  : Word32 *pshwZ    - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVvAdd32(
                const Word32 *pswX,
                const Word32 *pswY,
                const Word32  swLen,
                Word32       *pswZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pswZ[i] = L_add(pswX[i], pswY[i]);
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvSub
 ��������  : Z[i] = sub(X[i], Y[i])
 �������  : Word16 *pshwX    - ����������
             Word16 *pshwY    - ��������
             Word32 swLen     - ��������
             Word16 *pshwZ    - ������������
 �������  : Word16 *pshwZ    - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVvSub(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = sub(pshwX[i], pshwY[i]);
    }
}


void CODEC_OpVvMult(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = mult(pshwX[i], pshwY[i]);
    }
}


void CODEC_OpVvMultR(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = mult_r(pshwX[i], pshwY[i]);
    }
}


VOS_VOID CODEC_OpVvMultRD(
                Word16       *pshwX,
                Word16       *pshwY,
                Word32        swLen,
                Word16       *pshwZ)
{
    Word32      swCnt     = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pshwZ[swCnt] = mult_r(pshwX[swCnt], pshwY[-swCnt]);
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvMac
 ��������  : Y = sum(X[i]*Y[i])+sum
 �������  : Word16 *pshwX    - ��������X
             Word16 *pshwY    - ��������Y
             Word32 swLen     - ��������
             Word32 swSum     - �ۼӳ�ʼֵ
 �������  : ��
 �� �� ֵ  : Word32 �ۼӺ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word32 CODEC_OpVvMac(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_mac(swSum, pshwX[i], pshwY[i]);
    }

    return swSum;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvMacD
 ��������  : Y = sum(X[i]*Y[-i])+sum
 �������  : Word16 *pshwX    - ��������X
             Word16 *pshwY    - ��������Y
             Word32 swLen     - ��������
             Word32 swSum     - �ۼӳ�ʼֵ
 �������  : ��
 �� �� ֵ  : Word32 �ۼӺ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word32 CODEC_OpVvMacD(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_mac(swSum, pshwX[i], pshwY[-i]);
    }

    return swSum;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvMsu
 ��������  :Y = sum - sum(X[i]*Y[i])
 �������  : const Word16 *pshwX    - 16bit����������
             const Word16 *pshwY    - 16bit����������
             Word32 swLen           - ������������
             Word32 swSum           - �ۼӳ�ʼ��ֵ
 �������  : ��
 �� �� ֵ  : Word32 ����������ۼӳ�ʼ��ֵ����Ľ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word32 CODEC_OpVvMsu(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i      = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_msu(swSum , pshwX[i], pshwY[i]);
    }

    return swSum;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvMsuD
 ��������  :Y = sum - sum(X[i]*Y[-i])
 �������  : const Word16 *pshwX    - 16bit����������
             const Word16 *pshwY    - 16bit����������
             Word32 swLen           - ������������
             Word32 swSum           - �ۼӳ�ʼ��ֵ
 �������  : ��
 �� �� ֵ  : Word32 ����������ۼӳ�ʼ��ֵ����Ľ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
Word32 CODEC_OpVvMsuD(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i      = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_msu(swSum , pshwX[i], pshwY[-i]);
    }

    return swSum;

}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvFormWithCoef
 ��������  : Z[i] = add(mult(X[i], a), mult(Y[i], b))
 �������  : Word16 *pshwX     - ����1
             Word16 *pshwY     - ����2
             Word32  swLen     - ��������
             Word16  shwA      - ����1�ļ�Ȩϵ��
             Word16  shwB      - ����2�ļ�Ȩϵ��
             Word16 *pshwZ     - ������������
 �������  : Word16 *pshwZ     - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVvFormWithCoef(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                const Word16  shwA,
                const Word16  shwB,
                Word16 *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(mult(pshwX[i], shwA), mult(pshwY[i], shwB));
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvFormWithDimi
 ��������  : Z[i] = add(X[i]>>1, Y[i]>>1)
 �������  : Word16 *pshwX    - ����1
             Word16 *pshwY    - ����2
             Word32  swLen    - ��������
             Word16 *pshwZ    - ������������
 �������  : Word16 *pshwZ    - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVvFormWithDimi(
                Word16 *pshwX,
                Word16 *pshwY,
                Word32  swLen,
                Word16 *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(shr(pshwX[i], 1), shr(pshwY[i], 1));
    }
}

/*****************************************************************************
 �� �� ��  : CODEC_OpVvFormWithQuar
 ��������  : Z[i] = add(X[i]>>2, sub(Y[i],Y[i]>>2))
 �������  : Word16 *pshwX   - ��Ȩϵ��Ϊ0.25������
             Word16 *pshwY   - ��Ȩϵ��Ϊ0.75������
             Word32  swLen   - ��������
             Word16 *pshwZ   - ������������
 �������  : Word16 *pshwZ  - ������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��8��20��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
void CODEC_OpVvFormWithQuar(
                Word16 *pshwX,
                Word16 *pshwY,
                Word32  swLen,
                Word16 *pshwZ)
{
    Word32 i        = 0;

    Word16 shwTempX = 0;
    Word16 shwTempY = 0;

    for (i = 0; i < swLen; i++)
    {
        shwTempX = shr(pshwX[i], 2);

        shwTempY = sub(pshwY[i], shr(pshwY[i], 2));

        pshwZ[i] = add(shwTempX, shwTempY);
    }
}


VOS_VOID CODEC_OpVecShr_r32(
                Word32        *pswX,
                Word32         swLen,
                Word16         shwBits,
                Word32        *pswY)
{
    Word32      swCnt = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pswY[swCnt] = L_shr_r(pswX[swCnt], shwBits);
    }
}

#endif /* _MED_C89_ */

Word16 CODEC_OpVecMeanAbs(Word16 *pshwArr, Word32 swArrLen)
{
    Word32          swSumVal = 0;                                               /* Q0 */
    Word32          swCnt;
    Word16          shwAverEng;
    Word16          ashwTempArr[320];                                           /* ���֧�ֵ�swArrLenΪ320���ɸ�����Ҫ�޸� */

    CODEC_OpVecAbs(pshwArr, swArrLen, ashwTempArr);

    for (swCnt = 0; swCnt < swArrLen; swCnt++)
    {
        swSumVal = L_add(swSumVal, (Word32)(ashwTempArr[swCnt]));
    }

    /* ��ƽ��ֵ */
    shwAverEng = saturate(swSumVal / swArrLen);

    return shwAverEng;
}
Word32 CODEC_OpVecMax32(Word32 *pswArr, Word32 swLen)
{
    Word32          swCnt;
    Word32          swMax = pswArr[0];

    for (swCnt = 1; swCnt < swLen; swCnt++)
    {
        if (swMax < pswArr[swCnt])
        {
            swMax = pswArr[swCnt];
        }
    }

    return swMax;
}


Word32 CODEC_OpVecMin32(Word32 *pswArr, Word32 swLen)
{
    Word32          swCnt;
    Word32          swMin = pswArr[0];

    for (swCnt = 1; swCnt < swLen; swCnt++)
    {
        if (swMin > pswArr[swCnt])
        {
            swMin = pswArr[swCnt];
        }
    }

    return swMin;
}


Word32 CODEC_OpVecMean32(Word32 *pswArr, Word32 swArrLen)
{
    Word32          swSumVal = 0;                                               /* Q0 */
    Word32          swCnt;
    Word32          swAverEng;

    for (swCnt = 0; swCnt < swArrLen; swCnt++)
    {
        swSumVal = L_add(swSumVal, pswArr[swCnt]);
    }

    /* ��ƽ��ֵ */
    if (swSumVal >= 0)
    {
        swAverEng = swSumVal / swArrLen;
    }
    else
    {
        swAverEng = L_negate(swSumVal) / swArrLen;
        swAverEng = L_negate(swAverEng);
    }

    return swAverEng;
}


VOS_VOID CODEC_OpVecShr32(
                Word32 *pswX,
                Word32  swLen,
                Word16  shwBits,
                Word32 *pswY)
{
    Word32      swCnt = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pswY[swCnt] = L_shr(pswX[swCnt], shwBits);
    }
}


Word32 CODEC_OpVecSum32(Word32 *pswArray, Word32 swLen)
{
    Word32          swSum = 0;
    Word32          swCnt;

    for (swCnt=0; swCnt < swLen; swCnt++)
    {
        swSum = L_add(swSum, pswArray[swCnt]);
    }

    return swSum;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

