/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : fr_homing.c
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��5��10��
  ����޸�   :
  ��������   : FR Homing֡���
  �����б�   :
               FR_DecDetectHomingFrame
               FR_EncDetectHomingFrame
  �޸���ʷ   :
  1.��    ��   : 2011��5��10��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "fr_homing.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* ����Homing֡������������Э��3GPP TS 46.010 ��4.1 */
const Word16 g_ashwFrDecHomingStreamMask[FR_NUM_OF_PRMS] =
{
    0x0009,                             /* LARc[1] */
    0x0017,                             /* LARc[2] */
    0x000F,                             /* LARc[3] */
    0x0008,                             /* LARc[4] */
    0x0007,                             /* LARc[5] */
    0x0003,                             /* LARc[6] */
    0x0003,                             /* LARc[7] */
    0x0002,                             /* LARc[8] */

    0x0028,                             /* LTP lag  1 */
    0x0000,                             /* LTP gain 1 */
    0x0000,                             /* RPE grid 1 */
    0x0000,                             /* Block amplitude 1*/
    0x0004,                             /* RPE pulse 1-0 */
    0x0004,                             /* RPE pulse 1-1 */
    0x0004,                             /* RPE pulse 1-2 */
    0x0004,                             /* RPE pulse 1-3 */
    0x0004,                             /* RPE pulse 1-4 */
    0x0004,                             /* RPE pulse 1-5 */
    0x0004,                             /* RPE pulse 1-6 */
    0x0004,                             /* RPE pulse 1-7 */
    0x0004,                             /* RPE pulse 1-8 */
    0x0004,                             /* RPE pulse 1-9 */
    0x0004,                             /* RPE pulse 1-10 */
    0x0004,                             /* RPE pulse 1-11 */
    0x0004,                             /* RPE pulse 1-12 */

    0x0028,                             /* LTP lag  2 */
    0x0000,                             /* LTP gain 2 */
    0x0000,                             /* RPE grid 2 */
    0x0000,                             /* Block amplitude 2 */
    0x0004,                             /* RPE pulse 2-0 */
    0x0004,                             /* RPE pulse 2-1 */
    0x0004,                             /* RPE pulse 2-2 */
    0x0004,                             /* RPE pulse 2-3 */
    0x0004,                             /* RPE pulse 2-4 */
    0x0004,                             /* RPE pulse 2-5 */
    0x0004,                             /* RPE pulse 2-6 */
    0x0004,                             /* RPE pulse 2-7 */
    0x0004,                             /* RPE pulse 2-8 */
    0x0004,                             /* RPE pulse 2-9 */
    0x0004,                             /* RPE pulse 2-10 */
    0x0004,                             /* RPE pulse 2-11 */
    0x0004,                             /* RPE pulse 2-12 */


    0x0028,                             /* LTP lag  3 */
    0x0000,                             /* LTP gain 3 */
    0x0000,                             /* RPE grid 3 */
    0x0000,                             /* Block amplitude 3 */
    0x0004,                             /* RPE pulse 3-0 */
    0x0004,                             /* RPE pulse 3-1 */
    0x0004,                             /* RPE pulse 3-2 */
    0x0004,                             /* RPE pulse 3-3 */
    0x0004,                             /* RPE pulse 3-4 */
    0x0004,                             /* RPE pulse 3-5 */
    0x0004,                             /* RPE pulse 3-6 */
    0x0004,                             /* RPE pulse 3-7 */
    0x0004,                             /* RPE pulse 3-8 */
    0x0004,                             /* RPE pulse 3-9 */
    0x0004,                             /* RPE pulse 3-10 */
    0x0004,                             /* RPE pulse 3-11 */
    0x0004,                             /* RPE pulse 3-12 */


    0x0028,                             /* LTP lag  4 */
    0x0000,                             /* LTP gain 4 */
    0x0000,                             /* RPE grid 4 */
    0x0000,                             /* Block amplitude 4*/
    0x0004,                             /* RPE pulse 4-0 */
    0x0004,                             /* RPE pulse 4-1 */
    0x0004,                             /* RPE pulse 4-2 */
    0x0004,                             /* RPE pulse 4-3 */
    0x0003,                             /* RPE pulse 4-4 */
    0x0004,                             /* RPE pulse 4-5 */
    0x0004,                             /* RPE pulse 4-6 */
    0x0004,                             /* RPE pulse 4-7 */
    0x0004,                             /* RPE pulse 4-8 */
    0x0004,                             /* RPE pulse 4-9 */
    0x0004,                             /* RPE pulse 4-10 */
    0x0004,                             /* RPE pulse 4-11 */
    0x0004,                             /* RPE pulse 4-12 */
};

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : FR_EncDetectHomingFrame
 ��������  : FR����Homing֡���
 �������  : Word16 *pshwSpeechIn    --����160��PCM����֡
 �������  : ��
 �� �� ֵ  : Word16
 ���ú���  :
 ��������  : FR_HOMING_STATUS_ENUM_UINT16  --Homing֡����־

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
FR_HOMING_STATUS_ENUM_UINT16 FR_EncDetectHomingFrame(Word16 *pshwSpeechIn)
{
    FR_HOMING_STATUS_ENUM_UINT16         enHomingStatus;
    Word32                               swCnt;

    enHomingStatus = FR_HOMING_STATUS_YES;

    for (swCnt = 0; swCnt < 160; swCnt++)
    {
        /* �����ǰ���㲻����0x0008����Homing֡����־ΪNOT */
        if (pshwSpeechIn[swCnt] != FR_HOMGING_ENC_PCM_VALUE)
        {
            enHomingStatus = FR_HOMING_STATUS_NOT;
            break;
        }
    }

    return enHomingStatus;
}

/*****************************************************************************
 �� �� ��  : FR_DecDetectHomingFrame
 ��������  : FR����Homing֡���
 �������  : Word16 *pshwParm      --����֡����
             Word16 shwNumOfParms  --����������
 �������  : ��
 �� �� ֵ  : FR_HOMING_STATUS_ENUM_UINT16  --Homing֡����־
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
FR_HOMING_STATUS_ENUM_UINT16 FR_DecDetectHomingFrame(Word16 *pshwParm, Word16 shwNumOfParms)
{
    FR_HOMING_STATUS_ENUM_UINT16         enHomingStatus;
    Word32                               swCnt;

    enHomingStatus = FR_HOMING_STATUS_YES;

    for (swCnt = 0; swCnt < shwNumOfParms; swCnt++)
    {
        /* �����ǰ���㲻����Homing֡��������[i]����Homing֡����־ΪNOT */
        if (pshwParm[swCnt] != g_ashwFrDecHomingStreamMask[swCnt])
        {
            enHomingStatus = FR_HOMING_STATUS_NOT;
            break;
        }
    }

    return enHomingStatus;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

