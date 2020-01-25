

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_com_codec.h"

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
/*****************************************************************************
 �� �� ��  : CODEC_ComCodecBits2Prm
 ��������  : ����Ӧ������ת��Ϊ����
 �������  : VOS_INT16 *pshwBitStream      --������ָ��
             VOS_INT16 shwNumOfBits        --��ת�����ظ���
 �������  : VOS_INT16 *pshwPrmValue       --ת����õĲ���ָ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��6��9��
    ��    ��   : ��
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID CODEC_ComCodecBits2Prm(VOS_INT16 *pshwBitStream, VOS_INT16 shwNumOfBits, VOS_INT16 *pshwPrmValue)
{
    VOS_INT16   i;
    VOS_INT16   shwValue = 0;
    VOS_INT16   shwBit;

    /* ��shwNumOfBits�����ط���˫�ֽ����ĵ�shwNumOfBitsλ����λΪ0 */
    for (i = 0; i < shwNumOfBits; i++)
    {
        shwBit  = *pshwBitStream++;
        shwBit  = shwBit & BIT0_MASK;
        shwBit  = shl(shwBit, i);
        shwValue= add(shwValue, shwBit);
    }

    *pshwPrmValue = shwValue;
}
VOS_VOID CODEC_ComCodecPrm2Bits(VOS_INT16 shwValue, VOS_INT16 shwNumOfBits, VOS_INT16 *pshwBitStream)
{
    VOS_INT16   shwCnt;
    VOS_INT16   shwBit;

    VOS_INT16  *pshwStream;

    pshwStream = pshwBitStream;

    /*��shwValue��shwNumOfBits���������η���pshwBitStream��*/
    for (shwCnt = 0; shwCnt < shwNumOfBits; shwCnt++)
    {
        /*��ȡ������i����ֵ*/
        shwBit = shwValue & BIT0_MASK;

        if (0 == shwBit)
        {
            *(pshwStream++) = 0;
        }
        else
        {
            *(pshwStream++) = 1;
        }

        /*����1λ��ȡ��֮����*/
        shwValue = shr(shwValue, 1);
    }

}
VOS_VOID CODEC_ComCodecBits2Bytes(
                VOS_INT16 *pshwBits,
                VOS_INT16  shwBitsNum,
                VOS_INT8  *pscBytes)
{
    VOS_INT16    i;
    VOS_INT16    shwBit;
    VOS_INT16    shwBitCnt;

    VOS_INT8    *pscByteTemp;

    pscByteTemp = pscBytes;
    shwBitCnt   = 0;

    do
    {
        *pscByteTemp = 0;
        for (i = 0; (i < CODEC_BYTE_BITS_NUM) && (shwBitCnt < shwBitsNum); i++)
        {
            shwBit       = *(pshwBits + shwBitCnt);
            shwBit       = shwBit & BIT0_MASK;
            shwBit       = shl(shwBit, (7-i));                                  /*7��ʾ�Ӹ߱�����ͱ���������ñ�����*/
            *pscByteTemp = *pscByteTemp + (VOS_INT8)shwBit;
            shwBitCnt++;
        }
        pscByteTemp++;

    }while(shwBitCnt < shwBitsNum);

}
VOS_VOID CODEC_ComCodecBytes2Bits(
                VOS_INT8                *pscBytes,
                VOS_INT16                shwBitsNum,
                VOS_INT16               *pshwBits)
{
    VOS_INT16       i;
    VOS_INT16       shwBitCnt;
    VOS_INT16      *pshwBitsTemp;

    VOS_INT8        scByte;
    VOS_INT8        scByteMask;

    pshwBitsTemp = pshwBits;
    shwBitCnt    = 0;

    do
    {
        scByte     = *(pscBytes++);

        for (i = CODEC_BYTE_BITS_NUM; (i > 0) && (shwBitCnt < shwBitsNum); i--)
        {
            scByteMask                                = scByte >> (i-1);
            *(pshwBitsTemp + (CODEC_BYTE_BITS_NUM - i)) = scByteMask & BIT0_MASK;

            shwBitCnt++;
        }

        pshwBitsTemp += CODEC_BYTE_BITS_NUM;

    }while(shwBitCnt < shwBitsNum);

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

