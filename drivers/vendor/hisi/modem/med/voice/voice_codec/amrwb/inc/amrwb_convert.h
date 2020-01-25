/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : amrwb_convert.h
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��11��23��
  ����޸�   :
  ��������   : amrwb_convert.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2011��11��23��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"


#ifndef __AMRWB_CONVERT_H__
#define __AMRWB_CONVERT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/*6.60kbps����AMR-WBģʽ��A�����ĳ��ȣ���bitΪ��λ */
#define AMRWB_MR660_A_LENGTH            (54)

/*AMR-WB����������ռ��С����λΪMED_INT16 */
#define AMRWB_NUM_WORDS_OF_ENC_SERIAL   (40)

/*AMR-WB ABC�����ܺ���������*/
#define AMRWB_UMTS_MAX_ABC_SUM_LENTH    (477)

/*AMR-WB SID֡A�����ĳ��ȣ���bitΪ��λ*/
#define AMRWB_SID_A_LENGTH              (40)

/*AMR-WB SID֡B�����ĳ��ȣ���bitΪ��λ*/
#define AMRWB_SID_B_LENGTH              (0)

/*AMR-WB SID֡C�����ĳ��ȣ���bitΪ��λ*/
#define AMRWB_SID_C_LENGTH              (0)

/*AMR-WB SID UPDATE֡��������*/
#define AMRWB_SID_UPDATE_LENGTH         (35)

/*��ȡȫ�ֱ���AMR֡���ͱ��������ݶ��ձ�*/
#define AMRWB_GetNumOfFrmBits(uhwMode, uhwSubFrmNum) /*lint -e(661)*/ \
                                         (g_ashwAmrwbFrmBitsLen[uhwMode][uhwSubFrmNum])

/*��ȡȫ�ֱ�����Ӧģʽ����˳���Ӧ��ϵ��*/
#define AMRWB_GetBitsOrderTabPtr(uhwMode)                \
                                         (g_apAmrwbBitsOrderTab[uhwMode])

/*��ȡȫ�ֱ�����Ӧģʽ���س���*/
#define AMRWB_GetBitsLenOfMode(uhwMode)                  \
                                         (g_ashwAmrwbBitsOrderTabLen[uhwMode])

/*ͨ���ⲿ�������ʻ�ȡʵ�ʱ�������*/
#define AMRWB_GetRealEncMode(uhwMode)                  \
                                         (g_ashwAmrwbModeMapTab[uhwMode])

/* ABC����ת����,FrameType���� */
#define AMRWB_FRAMETYPE_NUM     (16)

/* SID��NODATA֡���� */
#define AMRWB_FRAMETYPE_SID (0x09)
#define AMRWB_FRAMETYPE_NODATA (0xf)


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
 ʵ������  : AMRWB_SUB_FRM_ENUM_UINT16
 ��������  : AMR��֡����
*****************************************************************************/
enum AMRWB_SUB_FRM_ENUM
{
    AMRWB_SUB_FRM_A                   = 0,
    AMRWB_SUB_FRM_B                   = 1,
    AMRWB_SUB_FRM_C                   = 2,
    AMRWB_SUB_FRM_ABC                 = 3,
    AMRWB_SUB_FRM_BUTT
};
typedef VOS_UINT16 AMRWB_SUB_FRM_ENUM_UINT16;

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
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/

extern const VOS_UINT16 g_ashwAmrwbModeMapTab[11];

/*****************************************************************************
  10 ��������
*****************************************************************************/
extern VOS_UINT32 AMRWB_GsmDlConvert(
                VOS_INT16                       *pshwAmrFrame,
                CODEC_AMRWB_TYPE_RX_ENUM_UINT16  enRxType,
                CODEC_AMRWB_MODE_ENUM_UINT16     enMode,
                VOS_INT16                       *pshwDecSerial);
extern VOS_UINT32 AMRWB_GsmUlConvert(
                AMRWB_ENCODED_SERIAL_STRU        *pstFrameBefConv,
                VOS_INT16                        *pshwEncSerialAfter);
extern VOS_VOID AMRWB_UlConvertSidFirst(
                VOS_UINT16                    uhwMode,
                AMRWB_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter);
extern VOS_VOID AMRWB_UlConvertSidUpdate(
                VOS_UINT16                        uhwMode,
                VOS_INT16                        *pshwEncSerial,
                AMRWB_ENCODED_SID_FRAME_STRU     *pstSidEncSerialAfter);
extern VOS_UINT32 AMRWB_UmtsDlConvert(
                VOICE_WPHY_AMR_SERIAL_STRU         *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertBadFrame(
                VOICE_WPHY_AMR_SERIAL_STRU         *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertGoodFrame(
                VOICE_WPHY_AMR_SERIAL_STRU         *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertSid(
                AMRWB_ENCODED_SID_FRAME_STRU     *pstSidEncSerial,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertSpeech(
                VOS_UINT16                        uhwMode,
                VOICE_WPHY_AMR_SERIAL_STRU         *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU        *pstDecSerialAfter);
extern VOS_VOID AMRWB_UmtsDlConvertSubFrame(
                VOS_UINT16                        uhwLen,
                const VOS_UINT16                 *puhwBitsOrder,
                VOS_INT16                        *pshwSerialBef,
                VOS_INT16                        *pshwSerialAfter);
extern VOS_UINT32 AMRWB_UmtsUlConvert(
                AMRWB_ENCODED_SERIAL_STRU        *pstFrameBefConv,
                VOICE_WPHY_AMR_SERIAL_STRU         *pstFrameAfterConv);
extern VOS_UINT32 AMRWB_UmtsUlConvertSpeech(
                VOS_UINT16                        uhwMode,
                VOS_INT16                        *pshwEncSerialBef,
                VOICE_WPHY_AMR_SERIAL_STRU         *pstFrameAfterConv);
extern VOS_VOID AMRWB_UmtsUlConvertSubFrame(
                VOS_UINT16                        uhwLen,
                const VOS_UINT16                 *puhwBitsOrder,
                VOS_INT16                        *pshwEncSerialBef,
                VOS_INT16                        *pshwEncSerialAfter);
extern VOS_UINT32 AMRWB_ImsDlConvert(
                IMS_DL_DATA_OBJ_STRU      *pstSrc,
                AMRWB_ENCODED_SERIAL_STRU *pstDecSerialAfter);
extern VOS_VOID AMRWB_ImsUlConvert(
                VOS_UINT16 uhwAMRType,
                VOS_UINT16 uhwMode,
                VOS_INT16* pstEncSerial);
extern VOS_VOID AMRWB_FilledImsDlData(
                VOS_UINT8 *puchData,
                VOICE_WPHY_AMR_SERIAL_STRU *pstDes);
extern VOS_VOID AMRWB_FilledImsUlData(
                VOS_UINT8   *puchImsaData,
                VOICE_WPHY_AMR_SERIAL_STRU* pstHifiData);
extern VOS_VOID AMRWB_ImsUlBitFilled(
                VOS_UINT8* puchSrc,
                VOS_UINT8* puchDes,
                VOS_UINT32* puwCurBit,
                VOS_UINT32 uwLen);
extern VOS_VOID AMRWB_ImsDlBitFilled(
                VOS_UINT8* puchSrc,
                VOS_UINT8* puchDes,
                VOS_UINT32* puwCurBit,
                VOS_UINT32 uwLen);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

