

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_com_codec.h"

#ifndef __HR_INTERFACE_H__
#define __HR_INTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define HR_CODED_SERIAL_LENGTH          (18)                                    /*HR������������,18��������36��Byte*/
#define HR_CODED_BITS_LENGTH            (112)                                   /*HR���������������ȡ���,18��������ȡ��112����*/
#define HR_HOMING_WHOLE_FRAME           (18)                                    /*HR����ȫ֡HOMING����������*/
#define HR_HOMING_FIRST_SUBFRAME        (9)                                     /*HR���е�һ��֡HOMING����������*/


#define HR_GetDtxFlagPtr()              (&giDTXon)                              /*��ȡȫ�ֱ���HR��DTXģʽ����ָ��*/

#define HR_GetOldResetPtr()             (&g_shwHrOldResetFlag)                  /*��ȡȫ�ֱ���HR��һ֡reset��־ָ��*/

#define HR_GetUnvoiceParsNumPtr()       (g_ashwHrUnvoicedParmBitsTable)         /*��ȡȫ�ֱ���HR��Unvoiceģʽ�¸�������ռ��������ָ��*/

#define HR_GetVoiceParsNumPtr()         (g_ashwHrVoicedParmBitsTable)           /*��ȡȫ�ֱ���HR��Voiceģʽ�¸�������ռ��������ָ��*/



/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/


/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  6 STRUCT����
*****************************************************************************/
/*****************************************************************************
 ʵ������  : HR_ENCODE_SERIAL_STRU
 ��������  : HR���������������нṹ�壬��20����������VadFlag��SpFlag
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwR0;                                  /*��֡����*/
    VOS_INT16                           ashwVqIndeces[3];                       /*LPC ʸ�������뱾*/
    VOS_INT16                           shwSoftInterp;                          /*�Ƿ������ֵ,1��0*/
    VOS_INT16                           shwVoicing;                             /*�Ƿ�ΪVoiceģʽ,0Ϊ��,1��2��3Ϊ��*/
    VOS_INT16                           ashwCodeWords[12];                      /*�����뱾���*/
    CODEC_VAD_ENUM_UINT16           enVadFlag;                              /*������VAD��־,*/
    CODEC_SP_ENUM_UINT16            enSpFlag;                               /*������SP��־,*/
}HR_ENCODE_SERIAL_STRU;

/*****************************************************************************
 ʵ������  : HR_DECODE_SERIAL_STRUCT
 ��������  : HR�����������������нṹ�壬��22������
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwR0;                                  /*��֡����*/
    VOS_INT16                           ashwVqIndeces[3];                       /*LPC ʸ�������뱾*/
    VOS_INT16                           shwSoftInterp;                          /*�Ƿ������ֵ,1��0*/
    VOS_INT16                           shwVoicing;                             /*�Ƿ�ΪVoiceģʽ,0Ϊ��,1��2��3Ϊ��*/
    VOS_INT16                           ashwCodeWords[12];                      /*�����뱾���*/
    CODEC_BFI_ENUM_UINT16           enBfiFlag;                              /* HR/FR/EFRʹ��, Bad Frame Indication */
    CODEC_BFI_ENUM_UINT16           enHrUfiFlag;                            /* HRʹ��, Unreliable Frame Indication */
    CODEC_SID_ENUM_UINT16           enSidFlag;                              /* HR/FR/EFRʹ��, Silence Descriptor flag */
    CODEC_TAF_ENUM_UINT16           enTafFlag;                              /* HR/FR/EFRʹ��, Time Alignment flag  */
}HR_DECODE_SERIAL_STRU;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_VOID HR_DlFrameConvert(VOS_INT16 *pshwBits, VOS_INT16 *pshwEncodedSerial);
extern VOS_VOID HR_UlFrameConvert(VOS_INT16 *pshwEncodedSerial, VOS_INT16 *pshwBits);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hr_interface.h */
