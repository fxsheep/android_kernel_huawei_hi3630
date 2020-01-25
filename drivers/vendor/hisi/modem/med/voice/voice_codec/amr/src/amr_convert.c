

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_op_vec.h"

#include "amr_interface.h"
#include "amr_convert.h"
#include "ucom_comm.h"

#include "voice_pcm.h"

/*
#include "med_com_vec_op.h"
*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/* AMR 4.75kbit/s ģʽ����˳���Ӧ��ϵ�����Э��TS26.101��Annex B */
const VOS_UINT16 g_auhwAmr475BitOrder[95] =
{
    0,      1,       2,      3,      4,      5,      6,      7,     8,      9,
    10,     11,     12,     13,     14,     15,     23,     24,     25,     26,
    27,     28,     48,     49,     61,     62,     82,     83,     47,     46,
    45,     44,     81,     80,     79,     78,     17,     18,     20,     22,
    77,     76,     75,     74,     29,     30,     43,     42,     41,     40,
    38,     39,     16,     19,     21,     50,     51,     59,     60,     63,
    64,     72,     73,     84,     85,     93,     94,     32,     33,     35,
    36,     53,     54,     56,     57,     66,     67,     69,     70,     87,
    88,     90,     91,     34,     55,     68,     89,     37,     58,     71,
    92,     31,     52,     65,     86
};

/* AMR 5.15kbit/s ģʽ����˳���Ӧ��ϵ�����Э��TS26.101��Annex B */
const VOS_UINT16 g_auhwAmr515BitOrder[103] =
{
     7,      6,      5,      4,      3,      2,      1,      0,     15,     14,
    13,     12,     11,     10,      9,      8,     23,     24,     25,     26,
    27,     46,     65,     84,     45,     44,     43,     64,     63,     62,
    83,     82,     81,    102,    101,    100,     42,     61,     80,     99,
    28,     47,     66,     85,     18,     41,     60,     79,     98,     29,
    48,     67,     17,     20,     22,     40,     59,     78,     97,     21,
    30,     49,     68,     86,     19,     16,     87,     39,     38,     58,
    57,     77,     35,     54,     73,     92,     76,     96,     95,     36,
    55,     74,     93,     32,     51,     33,     52,     70,     71,     89,
    90,     31,     50,     69,     88,     37,     56,     75,     94,     34,
    53,     72,     91
};

/* AMR 5.9kbit/s ģʽ����˳���Ӧ��ϵ�����Э��TS26.101��Annex B */
const VOS_UINT16 g_auhwAmr59BitOrder[118] =
{
    0,     1,      4,      5,      3,      6,      7,      2,     13,     15,
    8,     9,     11,     12,     14,     10,     16,     28,     74,     29,
    75,    27,    73,     26,     72,     30,     76,     51,     97,     50,
    71,    96,    117,    31,     77,     52,     98,     49,     70,     95,
    116,   53,    99,     32,     78,     33,     79,     48,     69,     94,
    115,   47,    68,     93,    114,     46,     67,     92,    113,     19,
    21,    23,    22,     18,     17,     20,     24,    111,     43,     89,
    110,   64,    65,     44,     90,     25,     45,     66,     91,    112,
    54,    100,   40,     61,     86,    107,     39,     60,     85,    106,
    36,    57,    82,    103,     35,     56,     81,    102,     34,     55,
    80,    101,   42,     63,     88,    109,     41,     62,     87,    108,
    38,    59,    84,    105,     37,     58,     83,    104
};

/* AMR 6.7kbit/s ģʽ����˳���Ӧ��ϵ�����Э��TS26.101��Annex B */
const VOS_UINT16 g_auhwAmr67BitOrder[134] =
{
    0,      1,      4,      3,      5,      6,     13,      7,      2,      8,
    9,      11,     15,     12,     14,     10,     28,     82,     29,     83,
    27,     81,     26,     80,     30,     84,     16,     55,    109,     56,
    110,    31,     85,     57,    111,     48,     73,    102,    127,     32,
    86,     51,     76,    105,    130,     52,     77,    106,    131,     58,
    112,    33,     87,     19,     23,     53,     78,    107,    132,     21,
    22,     18,     17,     20,     24,     25,     50,     75,    104,    129,
    47,     72,    101,    126,     54,     79,    108,    133,     46,     71,
    100,   125,    128,    103,     74,     49,     45,     70,     99,    124,
    42,     67,     96,    121,     39,     64,     93,    118,     38,     63,
    92,    117,     35,     60,     89,    114,     34,     59,     88,    113,
    44,     69,     98,    123,     43,     68,     97,    122,     41,     66,
    95,    120,     40,     65,     94,    119,     37,     62,     91,    116,
    36,     61,     90,    115
};

/* AMR 7.4kbit/s ģʽ����˳���Ӧ��ϵ�����Э��TS26.101��Annex B */
const VOS_UINT16 g_auhwAmr74BitOrder[148] =
{
    0,      1,      2,      3,      4,      5,      6,      7,      8,      9,
    10,     11,     12,     13,     14,     15,     16,     26,     87,     27,
    88,     28,     89,     29,     90,     30,     91,     51,     80,    112,
    141,    52,     81,    113,    142,     54,     83,    115,    144,     55,
    84,    116,    145,     58,    119,     59,    120,     21,     22,     23,
    17,     18,     19,     31,     60,     92,    121,     56,     85,    117,
    146,    20,     24,     25,     50,     79,    111,    140,     57,     86,
    118,   147,     49,     78,    110,    139,     48,     77,     53,     82,
    114,   143,    109,    138,     47,     76,    108,    137,     32,     33,
    61,     62,     93,     94,    122,    123,     41,     42,     43,     44,
    45,     46,     70,     71,     72,     73,     74,     75,    102,    103,
    104,   105,    106,    107,    131,    132,    133,    134,    135,    136,
    34,     63,     95,    124,     35,     64,     96,    125,     36,     65,
    97,    126,     37,     66,     98,    127,     38,     67,     99,    128,
    39,     68,    100,    129,     40,     69,    101,    130
};

/* AMR 7.95kbit/s ģʽ����˳���Ӧ��ϵ�����Э��TS26.101��Annex B */
const VOS_UINT16 g_auhwAmr795BitOrder[159] =
{
     8,     7,       6,      5,      4,      3,      2,     14,     16,      9,
    10,     12,     13,     15,     11,     17,     20,     22,     24,     23,
    19,     18,     21,     56,     88,    122,    154,     57,     89,    123,
    155,    58,     90,    124,    156,     52,     84,    118,    150,     53,
    85,    119,    151,     27,     93,     28,     94,     29,     95,     30,
    96,     31,     97,     61,    127,     62,    128,     63,    129,     59,
    91,    125,    157,     32,     98,     64,    130,      1,      0,     25,
    26,     33,     99,     34,    100,     65,    131,     66,    132,     54,
    86,    120,    152,     60,     92,    126,    158,     55,     87,    121,
    153,   117,    116,    115,     46,     78,    112,    144,     43,     75,
    109,   141,     40,     72,    106,    138,     36,     68,    102,    134,
    114,   149,    148,    147,    146,     83,     82,     81,     80,     51,
    50,     49,     48,     47,     45,     44,     42,     39,     35,     79,
    77,     76,     74,     71,     67,    113,    111,    110,    108,    105,
    101,   145,    143,    142,    140,    137,    133,     41,     73,    107,
    139,    37,     69,    103,    135,     38,     70,    104,    136
};

/* AMR 10.2kbit/s ģʽ����˳���Ӧ��ϵ�����Э��TS26.101��Annex B */
const VOS_UINT16 g_auhwAmr102BitOrder[204] =
{
     7,      6,      5,      4,      3,      2,      1,      0,     16,     15,
    14,     13,     12,     11,     10,      9,      8,     26,     27,     28,
    29,     30,     31,    115,    116,    117,    118,    119,    120,     72,
    73,    161,    162,     65,     68,     69,    108,    111,    112,    154,
    157,   158,    197,    200,    201,     32,     33,    121,    122,     74,
    75,    163,    164,     66,    109,    155,    198,     19,     23,     21,
    22,     18,     17,     20,     24,     25,     37,     36,     35,     34,
    80,     79,     78,     77,    126,    125,    124,    123,    169,    168,
    167,   166,     70,     67,     71,    113,    110,    114,    159,    156,
    160,   202,    199,    203,     76,    165,     81,     82,     92,     91,
    93,     83,     95,     85,     84,     94,    101,    102,     96,    104,
    86,    103,     87,     97,    127,    128,    138,    137,    139,    129,
    141,   131,    130,    140,    147,    148,    142,    150,    132,    149,
    133,   143,    170,    171,    181,    180,    182,    172,    184,    174,
    173,   183,    190,    191,    185,    193,    175,    192,    176,    186,
    38,     39,     49,     48,     50,     40,     52,     42,     41,     51,
    58,     59,     53,     61,     43,     60,     44,     54,    194,    179,
    189,   196,    177,    195,    178,    187,    188,    151,    136,    146,
    153,   134,    152,    135,    144,    145,    105,     90,    100,    107,
    88,    106,     89,     98,     99,     62,     47,     57,     64,     45,
    63,     46,     55,     56
};

/* AMR 12.2kbit/s ģʽ����˳���Ӧ��ϵ�����Э��TS26.101��Annex B */
const VOS_UINT16 g_auhwAmr122BitOrder[244] =
{
    0,       1,      2,      3,      4,      5,      6,      7,      8,      9,
    10,     11,     12,     13,     14,     23,     15,     16,     17,     18,
    19,     20,     21,     22,     24,     25,     26,     27,     28,     38,
    141,    39,     142,     40,    143,     41,    144,     42,    145,     43,
    146,    44,     147,     45,    148,     46,    149,     47,     97,    150,
    200,    48,      98,    151,    201,     49,     99,    152,    202,     86,
    136,   189,     239,     87,    137,    190,    240,     88,    138,    191,
    241,    91,     194,     92,    195,     93,    196,     94,    197,     95,
    198,    29,      30,     31,     32,     33,     34,     35,     50,    100,
    153,   203,      89,    139,    192,    242,     51,    101,    154,    204,
    55,    105,     158,    208,     90,    140,    193,    243,     59,    109,
    162,   212,      63,    113,    166,    216,     67,    117,    170,    220,
    36,     37,      54,     53,     52,     58,     57,     56,     62,     61,
    60,     66,      65,     64,     70,     69,     68,    104,    103,    102,
    108,   107,      106,    112,    111,    110,    116,    115,    114,    120,
    119,   118,      157,    156,    155,    161,    160,    159,    165,    164,
    163,   169,      168,    167,    173,    172,    171,    207,    206,    205,
    211,   210,      209,    215,    214,    213,    219,    218,    217,    223,
    222,   221,       73,     72,     71,     76,     75,     74,     79,     78,
    77,     82,       81,     80,     85,     84,     83,    123,    122,    121,
    126,   125,      124,    129,    128,    127,    132,    131,    130,    135,
    134,   133,      176,    175,    174,    179,    178,    177,    182,    181,
    180,   185,      184,    183,    188,    187,    186,    226,    225,    224,
    229,   228,      227,    232,    231,    230,    235,    234,    233,    238,
    237,   236,       96,    199
};

/* ֡���ͱ��������ݶ��ձ�AMR��������ģʽ��A��B��C������Ӧ�ĳ��ȣ���bitΪ��λ*/
const VOS_UINT16 g_ashwAmrFrmBitsLen[8][3] =
{
    { 42, 53, 0 },
    { 49, 54, 0 },
    { 55, 63, 0 },
    { 58, 76, 0 },
    { 61, 87, 0 },
    { 75, 84, 0 },
    { 65, 99, 40 },
    { 81, 103, 60 }
};

/* ģʽ����˳���Ӧ��ϵ�ۺ�table��ΪAMR��������ģʽ�±���˳���Ӧ��ϵ���ۺϱ� */
const VOS_UINT16 *g_apAmrBitsOrderTab[8] =
{
    g_auhwAmr475BitOrder,
    g_auhwAmr515BitOrder,
    g_auhwAmr59BitOrder,
    g_auhwAmr67BitOrder,
    g_auhwAmr74BitOrder,
    g_auhwAmr795BitOrder,
    g_auhwAmr102BitOrder,
    g_auhwAmr122BitOrder
};

/* AMR��������ģʽ��Ӧ�����ݳ��ȣ���bitΪ��λ */
const VOS_UINT16 g_ashwAmrBitsOrderTabLen[8] =
{
    95,                                                                         /* ��Ӧ����4.75kbit/s�������س��� */
    103,                                                                        /* ��Ӧ����5.15kbit/s�������س��� */
    118,                                                                        /* ��Ӧ����5.9kbit/s�������س��� */
    134,                                                                        /* ��Ӧ����6.7kbit/s�������س��� */
    148,                                                                        /* ��Ӧ����7.4kbit/s�������س��� */
    159,                                                                        /* ��Ӧ����7.95kbit/s�������س��� */
    204,                                                                        /* ��Ӧ����10.2kbit/s�������س��� */
    244                                                                         /* ��Ӧ����12.2kbit/s�������س��� */
};

/* ָʾһ��ͨ���������Ƿ��յ���SID_FIRST֡ */
VOS_UINT16 g_uhwAmrSidFirstFlag = 0;
/* AMR,ABC��������ת����
*/
MED_UINT16 g_auwAmrMap[AMR_FRAMETYPE_NUM][AMR_SUB_FRM_BUTT]
    = {{42,53,0},
       {49,54,0},
       {55,63,0},
       {58,76,0},
       {61,87,0},
       {75,84,0},
       {65,99,40},
       {81,103,60},
       {39,0,0},
       {43,0,0},
       {38,0,0},
       {37,0,0},
       {0,0,0},
       {0,0,0},
       {0,0,0},
       {0,0,0}};

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

VOS_UINT32 AMR_UmtsUlConvertSpeech(
                VOS_UINT16                 uhwMode,
                VOS_INT16                 *pshwEncSerialBef,
                VOICE_WPHY_AMR_SERIAL_STRU  *pstFrameAfterConv)
{
    VOS_UINT16              uhwALen;
    VOS_UINT16              uhwBLen;
    VOS_UINT16              uhwCLen;

    VOS_INT16              *pshwEncSerialAfter;
    const VOS_UINT16       *puhwBitsOrder;

    /* ����ģʽ����ȷ */
    if (uhwMode > CODEC_AMR_FRM_TYPE_122K)
    {
        return CODEC_RET_ERR;
    }

    uhwALen = pstFrameAfterConv->auhwLen[0];
    uhwBLen = pstFrameAfterConv->auhwLen[1];
    uhwCLen = pstFrameAfterConv->auhwLen[2];

    pshwEncSerialAfter = (VOS_INT16 *)pstFrameAfterConv->auwData;

    /*��ȡ��Ӧģʽ����˳���*/
    puhwBitsOrder      = AMR_GetBitsOrderTabPtr(uhwMode);

    /* ת��A��֡ */
    AMR_UmtsUlConvertSubFrame(uhwALen,
                              puhwBitsOrder,
                              pshwEncSerialBef,
                              pshwEncSerialAfter);

    puhwBitsOrder += uhwALen;
    pshwEncSerialAfter  += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwALen, 31), 5)); /*����5λ��ʾ32���ض��룬��2��˱�ʾpshwSubFrameΪ16��������ָ��*/

    /* ת��B��֡ */
    AMR_UmtsUlConvertSubFrame(uhwBLen,
                              puhwBitsOrder,
                              pshwEncSerialBef,
                              pshwEncSerialAfter);

    puhwBitsOrder += uhwBLen;
    pshwEncSerialAfter  += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwBLen, 31), 5)); /*����5λ��ʾ32���ض��룬��2��˱�ʾpshwSubFrameΪ16��������ָ��*/

    /* ת��C��֡ */
    AMR_UmtsUlConvertSubFrame(uhwCLen,
                              puhwBitsOrder,
                              pshwEncSerialBef,
                              pshwEncSerialAfter);

    return CODEC_RET_OK;
}
VOS_VOID AMR_UmtsUlConvertSubFrame(
                VOS_UINT16          uhwLen,
                const VOS_UINT16   *puhwBitsOrder,
                VOS_INT16          *pshwEncSerialBef,
                VOS_INT16          *pshwEncSerialAfter)
{
    VOS_UINT16  i;
    VOS_INT16   ashwBits[AMR_UMTS_MAX_SUB_FRAME_SIZE];                          /*12.2kbpsģʽ��B�����ĳ������,Ϊ103 */

    /*���ת������Ϊ0����������*/
    if (0 == uhwLen)
    {
        return;
    }

    /*�Ƚ���BIT����*/
    for(i = 0; i < uhwLen ; i++)
    {
        ashwBits[i] = *(pshwEncSerialBef + puhwBitsOrder[i]);
    }

    CODEC_ComCodecBits2Bytes(ashwBits,
                           (VOS_INT16)uhwLen,
                           (VOS_INT8 *)pshwEncSerialAfter);
}
VOS_VOID AMR_UlConvertSidFirst(
                VOS_UINT16                  uhwMode,
                AMR_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter)
{
    /*��SID_FIRST֡ȫ0����*/
    pstSidEncSerialAfter->ashwSidData1[0]   = 0;
    pstSidEncSerialAfter->ashwSidData1[1]   = 0;
    pstSidEncSerialAfter->shwSidData2       = 0;
    pstSidEncSerialAfter->shwSti            = CODEC_AMR_STI_SID_FIRST;
    pstSidEncSerialAfter->shwModeIndication = uhwMode;

}
VOS_VOID AMR_UlConvertSidUpdate(
                VOS_UINT16                  uhwMode,
                VOS_INT16                  *pshwEncSerial,
                AMR_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter)
{
    /*ת��35����SID֡*/
    CODEC_ComCodecBits2Bytes(pshwEncSerial,
                           AMR_SID_UPDATE_LENGTH,
                           (VOS_INT8 *)pstSidEncSerialAfter);

    /* 1bit STI��־ */
    pstSidEncSerialAfter->shwSti = CODEC_AMR_STI_SID_UPDATE;

    /* ����ģʽָʾ */
    pstSidEncSerialAfter->shwModeIndication = uhwMode;
}
VOS_UINT32  AMR_UmtsUlConvert(
                AMR_ENCODED_SERIAL_STRU  *pstFrameBefConv,
                VOICE_WPHY_AMR_SERIAL_STRU *pstFrameAfterConv)
{

    VOS_UINT16                          uhwMode = 0;
    VOS_INT16                          *pshwEncSerialBef;
    VOS_INT16                          *pshwEncSerialAfter;
    CODEC_RET_ENUM_UINT32           enRetResult;

    if ((VOS_NULL == pstFrameBefConv) || (VOS_NULL == pstFrameAfterConv))
    {
        return CODEC_RET_ERR;
    }

    enRetResult = CODEC_RET_OK;

    /*�Ը�ʽת����������������ݽṹ������*/
    UCOM_MemSet(pstFrameAfterConv, 0, sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /*����ת����AMR������֡������־ΪΪ��֡*/
    pstFrameAfterConv->enQualityIdx = CODEC_AMR_FQI_QUALITY_GOOD;

    /*����������л�ȡAMR����ģʽ��ת��ǰ����ָ��*/
    uhwMode            = (VOS_UINT16)pstFrameBefConv->enAmrMode;
    pshwEncSerialBef   = pstFrameBefConv->ashwEncSerial;

    /*��ȡת��������ָ��*/
    pshwEncSerialAfter = (VOS_INT16 *)pstFrameAfterConv->auwData;

    /*�жϱ����������֡����*/
    switch (pstFrameBefConv->uhwFrameType)
    {
        case CODEC_AMR_TYPE_TX_SPEECH_GOOD:
        {
            if (uhwMode > CODEC_AMR_MODE_122)
            {
                return CODEC_RET_ERR;
            }

            /*д��SPEECH_GOOD֡��ABC���������ĳ���*/
            pstFrameAfterConv->auhwLen[0] = AMR_GetNumOfFrmBits(uhwMode, AMR_SUB_FRM_A);
            pstFrameAfterConv->auhwLen[1] = AMR_GetNumOfFrmBits(uhwMode, AMR_SUB_FRM_B);
            pstFrameAfterConv->auhwLen[2] = AMR_GetNumOfFrmBits(uhwMode, AMR_SUB_FRM_C);

            /*���������֡���б���ת��*/
            enRetResult = AMR_UmtsUlConvertSpeech(uhwMode,
                                                  pshwEncSerialBef,
                                                  pstFrameAfterConv);

            break;
        }

        case CODEC_AMR_TYPE_TX_SID_FIRST:
        {
            if (uhwMode > CODEC_AMR_MODE_122)
            {
                return CODEC_RET_ERR;
            }

            /*д��SID_FIRST֡��ABC���������ĳ���*/
            pstFrameAfterConv->auhwLen[0] = AMR_SID_A_LENGTH;
            pstFrameAfterConv->auhwLen[1] = AMR_SID_B_LENGTH;
            pstFrameAfterConv->auhwLen[2] = AMR_SID_C_LENGTH;

            /*��SID_FIRST֡���б���ת��*/
            AMR_UlConvertSidFirst(uhwMode, (AMR_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);
            break;
        }

        case CODEC_AMR_TYPE_TX_SID_UPDATE:
        {
            if (uhwMode > CODEC_AMR_MODE_122)
            {
                return CODEC_RET_ERR;
            }

            /*д��SID_UPDATE֡��ABC���������ĳ���*/
            pstFrameAfterConv->auhwLen[0] = AMR_SID_A_LENGTH;
            pstFrameAfterConv->auhwLen[1] = AMR_SID_B_LENGTH;
            pstFrameAfterConv->auhwLen[2] = AMR_SID_C_LENGTH;

            /*��SID_UPDATE֡���б���ת��*/
            AMR_UlConvertSidUpdate(uhwMode,
                                   pshwEncSerialBef,
                                   (AMR_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);

            break;
        }

        default: /*����NO_DATA�����*/
        {
            break;
        }
    }

    return enRetResult;
}


VOS_VOID AMR_UmtsDlConvertSpeech(
                VOS_UINT16                  uhwMode,
                VOICE_WPHY_AMR_SERIAL_STRU   *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU    *pstDecSerialAfter)
{
    VOS_UINT16              uhwALen;
    VOS_UINT16              uhwBLen;
    VOS_UINT16              uhwCLen;

    VOS_INT16                   *pshwSerialBef;
    VOS_INT16                   *pshwSerialAfter;

    const VOS_UINT16            *puhwBitsOrder;

    pshwSerialBef   = (VOS_INT16 *)pstDecSerialBef->auwData;
    pshwSerialAfter = pstDecSerialAfter->ashwEncSerial;

    /*��ȡȫ�ֱ�����ͬ����ģʽ���������ָ��*/
    puhwBitsOrder = AMR_GetBitsOrderTabPtr(uhwMode);

    /*����֡����Ϊ�õ�����֡*/
    pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SPEECH_GOOD;

    /* ���AMR����ģʽ */
    pstDecSerialAfter->enAmrMode    = uhwMode;

    /*��ȡABC��������*/
    uhwALen = pstDecSerialBef->auhwLen[0];
    uhwBLen = pstDecSerialBef->auhwLen[1];
    uhwCLen = pstDecSerialBef->auhwLen[2];

    /* ת����֡A */
    AMR_UmtsDlConvertSubFrame(uhwALen,
                              puhwBitsOrder,
                              pshwSerialBef,
                              pshwSerialAfter);

    puhwBitsOrder   += uhwALen;
    //pshwSerialAfter += pstDecSerialBef->uhwALen;
    pshwSerialBef   += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwALen, 31), 5)); /*����5λ��ʾ32���ض��룬��2��˱�ʾpshwSubFrameΪ16��������ָ��*/

    /* ת����֡B */
    AMR_UmtsDlConvertSubFrame(uhwBLen,
                              puhwBitsOrder,
                              pshwSerialBef,
                              pshwSerialAfter);

    puhwBitsOrder   += uhwBLen;
    //pshwSerialAfter += pstDecSerialBef->uhwBLen;
    pshwSerialBef   += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwBLen, 31), 5)); /*����5λ��ʾ32���ض��룬��2��˱�ʾpshwSubFrameΪ16��������ָ��*/

    /* ת����֡C */
    AMR_UmtsDlConvertSubFrame(uhwCLen,
                              puhwBitsOrder,
                              pshwSerialBef,
                              pshwSerialAfter);

}
VOS_VOID AMR_UmtsDlConvertSubFrame(
                VOS_UINT16         uhwLen,
                const VOS_UINT16  *puhwBitsOrder,
                VOS_INT16         *pshwSerialBef,
                VOS_INT16         *pshwSerialAfter)
{
    VOS_UINT16    i;
    VOS_UINT16    uhwIndex;
    VOS_INT16     ashwBits[AMR_UMTS_MAX_SUB_FRAME_SIZE];                        /* 12.2kbpsģʽ��B�����ĳ������,Ϊ103 */

    /*���ת�������Ƿ�Ϊ0��Ϊ0ʱ��������*/
    if (0 == uhwLen)
    {
        return;
    }

    /*����������ת���ɷǽ��ձ�����*/
    CODEC_ComCodecBytes2Bits((VOS_INT8 *)pshwSerialBef,
                           (VOS_INT16)uhwLen,
                           ashwBits);

    /*���ݱ��������Խ��յķǽ��ձ��������з�����*/
    for (i = 0; i < uhwLen ; i++)
    {
        uhwIndex                  = puhwBitsOrder[i];
        pshwSerialAfter[uhwIndex] = ashwBits[i];
    }

}


VOS_VOID AMR_UmtsDlConvertSid(
                AMR_ENCODED_SID_FRAME_STRU  *pstSidEncSerial,
                AMR_ENCODED_SERIAL_STRU     *pstDecSerialAfter)
{
    VOS_UINT16   uhwSidFirstFlag               = 0;
    VOS_UINT16  *puhwSidFirstFlag              = VOS_NULL;

    /*��ȡȫ�ֱ���AMRһ��ͨ���������Ƿ��յ���SID_FIRST֡��־,����ʼ��Ϊ0*/
    puhwSidFirstFlag  = AMR_GetSidFirstFlagPtr();
    uhwSidFirstFlag   = *puhwSidFirstFlag;

    pstDecSerialAfter->enAmrMode = CODEC_AMR_MODE_MRDTX;

    /*STI_SID_Update֡��������*/
    if (CODEC_AMR_STI_SID_UPDATE == pstSidEncSerial->shwSti)
    {
        /* �������ͨ����δ����SID_FIRST,��ѱ�֡��NO_DATA���� */
        if (uhwSidFirstFlag != 0)
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SID_UPDATE;
        }
        else
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SID_BAD;
            return;
        }

        /*ת��35����SID֡*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pstSidEncSerial,
                                AMR_SID_UPDATE_LENGTH,
                                pstDecSerialAfter->ashwEncSerial);
    }
    else  /* STI_SID_FIRST֡ */
    {
        pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SID_FIRST;
        *puhwSidFirstFlag               = 1;
    }
}
VOS_VOID AMR_UmtsDlConvertGoodFrame(
                VOICE_WPHY_AMR_SERIAL_STRU  *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
{
    VOS_UINT16                  uhwMode    = 0;
    VOS_UINT16                  uhwALength = 0;

    VOS_INT16                  *pshwEncSerial;

    /*�����������л�ȡA�����ĳ��Ⱥ�����ָ��*/
    uhwALength    = pstDecSerialBef->auhwLen[0];
    pshwEncSerial = (VOS_INT16 *)pstDecSerialBef->auwData;


    if (AMR_SID_A_LENGTH == uhwALength)
    {
        /*SID֡��������*/
        AMR_UmtsDlConvertSid((AMR_ENCODED_SID_FRAME_STRU *)pshwEncSerial,
                             pstDecSerialAfter);
    }
    else
    {
        /*����A�������Ȳ����������ģʽ*/
        for (uhwMode = 0; uhwMode <= CODEC_AMR_MODE_122; uhwMode++)
        {
            if (uhwALength == AMR_GetNumOfFrmBits(uhwMode, 0))
            {
                break;
            }
        }

        /*��δ�鵽��Ӧ����ģʽ������ΪNO DATA֡*/
        if (uhwMode > CODEC_AMR_MODE_122)
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_NO_DATA;


            pstDecSerialAfter->enAmrMode    = CODEC_AMR_MODE_MRDTX;
        }
        else /* ������AMR����֡ */
        {
            *AMR_GetSidFirstFlagPtr() = 0;
            AMR_UmtsDlConvertSpeech(uhwMode, pstDecSerialBef, pstDecSerialAfter);
        }
    }

}
VOS_VOID AMR_UmtsDlConvertBadFrame(
                VOICE_WPHY_AMR_SERIAL_STRU   *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU    *pstDecSerialAfter)
{
    VOS_UINT16               uhwAmrLength;

    /* �����������л�ȡA�����ĳ��� */
    uhwAmrLength  = pstDecSerialBef->auhwLen[0];

    if (uhwAmrLength >= AMR_MR475_A_LENGTH)
    {
        /*speech��֡����*/
        pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SPEECH_BAD;
    }
    else if (AMR_SID_A_LENGTH == uhwAmrLength)
    {
        pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SID_BAD;

        /* ����ģʽָʾ */
        pstDecSerialAfter->enAmrMode    = CODEC_AMR_MODE_475;
    }
    else
    {
        /*NO DATA֡����*/
        pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_NO_DATA;

        /*����ģʽָʾ*/
        pstDecSerialAfter->enAmrMode    = CODEC_AMR_MODE_475;
    }
}
VOS_UINT32  AMR_UmtsDlConvert(
                VOICE_WPHY_AMR_SERIAL_STRU  *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
{
    CODEC_AMR_FQI_QUALITY_ENUM_UINT16    enAmrQualityIndication;

    if (( VOS_NULL == pstDecSerialAfter ) || ( VOS_NULL == pstDecSerialBef ))
    {
        return CODEC_RET_ERR;
    }

    enAmrQualityIndication = pstDecSerialBef->enQualityIdx;

    /* �Ա���ת��������������������ṹ��������� */
    UCOM_MemSet(pstDecSerialAfter, 0, sizeof(AMR_ENCODED_SERIAL_STRU));

    if (CODEC_AMR_FQI_QUALITY_GOOD == enAmrQualityIndication)
    {
        AMR_UmtsDlConvertGoodFrame(pstDecSerialBef, pstDecSerialAfter);
    }
    else
    {
        AMR_UmtsDlConvertBadFrame(pstDecSerialBef, pstDecSerialAfter);
    }

    return CODEC_RET_OK;
}
VOS_UINT32 AMR_GsmUlConvert(
                AMR_ENCODED_SERIAL_STRU *pstFrameBefConv,
                VOS_INT16               *pshwEncSerialAfter)
{
    VOS_UINT16                 i;
    VOS_UINT16                 uhwMode;
    VOS_UINT16                 uhwLen;
    VOS_UINT16                 uhwIndex;
    const VOS_UINT16          *puhwBitOrderTable;


    VOS_INT16                  ahswEncSerialSorted[AMR_MAX_SERIAL_SIZE];
    VOS_INT16                 *pshwEncSerial;

    /*��������ǿռ��*/
    if(( VOS_NULL == pstFrameBefConv ) ||( VOS_NULL == pshwEncSerialAfter ))
    {
        return CODEC_RET_ERR;
    }

    /*�Ա������ź����ahswEncSerialSorted[]��������*/
    CODEC_OpVecSet(ahswEncSerialSorted, AMR_MAX_SERIAL_SIZE, 0);

    /*������������������������*/
    CODEC_OpVecSet(pshwEncSerialAfter, AMR_NUM_WORDS_OF_ENC_SERIAL, 0);

    /*��ȡ����ģʽ�ͱ���ת��ǰ����ָ��*/
    uhwMode         = (VOS_UINT16)pstFrameBefConv->enAmrMode;
    pshwEncSerial   = pstFrameBefConv->ashwEncSerial;

    /* ���ݱ���������������б������Ŵ��� */
    if (CODEC_AMR_TYPE_TX_SPEECH_GOOD == pstFrameBefConv->uhwFrameType)
    {
        /*��ȡ��Ӧģʽ���������ͱ��س���*/
        puhwBitOrderTable = AMR_GetBitsOrderTabPtr(uhwMode);
        uhwLen            = AMR_GetBitsLenOfMode(uhwMode);

        /*���ݱ���˳���Ա��������б�������*/
        for (i = 0; i < uhwLen; i++)
        {
            uhwIndex               = puhwBitOrderTable[i];
            ahswEncSerialSorted[i] = pshwEncSerial[uhwIndex];
        }

        /*�����ź������ѹ��Ϊ���ձ�����*/
        CODEC_ComCodecBits2Bytes(ahswEncSerialSorted,
                               (VOS_INT16)uhwLen,
                               (VOS_INT8 *)pshwEncSerialAfter);
    }
    else if (CODEC_AMR_TYPE_TX_SID_UPDATE == pstFrameBefConv->uhwFrameType)
    {
        /*��ȡ35����SID_UPDATE֡����*/
        AMR_UlConvertSidUpdate(uhwMode,
                               pshwEncSerial,
                               (AMR_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);
    }
    else
    {
        /* ����SID_FIRST��NO_DATA��������������pAmrFrame�����㴦�� */
        /* ����֮ǰ�Ѿ�Ԥ��ȫ�����㣬�˴������ٴν��� */
    }

    return CODEC_RET_OK;
}
VOS_UINT32 AMR_GsmDlConvert(
                VOS_INT16                         *pshwAmrFrame,
                CODEC_AMR_TYPE_RX_ENUM_UINT16  enRxType,
                CODEC_AMR_MODE_ENUM_UINT16     enMode,
                VOS_INT16                         *pshwDecSerial)
{
    VOS_UINT16           i;
    VOS_UINT16           uhwIndex;
    VOS_UINT16           uhwBitsLength;

    VOS_INT16            ashwEncSerial[AMR_MAX_SERIAL_SIZE];

    const VOS_UINT16    *puhwBitsOrder;

    /* ��������ǿռ�� */
    if (( VOS_NULL == pshwAmrFrame ) || ( VOS_NULL == pshwDecSerial ))
    {
        return CODEC_RET_ERR;
    }

    /*�Էǽ���������������н�������*/
    CODEC_OpVecSet(ashwEncSerial, AMR_MAX_SERIAL_SIZE, 0);

    /*�Ա���ת������������������ݽ�������*/
    CODEC_OpVecSet(pshwDecSerial, AMR_MAX_SERIAL_SIZE, 0);

    /* �����ⲿ�����RX_FrameType�ж��Ƿ�������֡���Ƿ�����֡ */
    if (   (CODEC_AMR_TYPE_RX_SPEECH_GOOD == enRxType)
        || (CODEC_AMR_TYPE_RX_SPEECH_DEGRADED == enRxType))
    {
        /*��ȡȫ�ֱ�����ͬ����ģʽ���������ָ��ͱ��س���*/
        puhwBitsOrder = AMR_GetBitsOrderTabPtr(enMode);
        uhwBitsLength = AMR_GetBitsLenOfMode(enMode);

        /*������������ת�ɷǽ������������*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pshwAmrFrame,
                               (VOS_INT16)uhwBitsLength,
                               ashwEncSerial);

        /*�Ըñ��������б��ط����ţ������pDecodeSpeech*/
        for (i = 0; i < uhwBitsLength; i++)
        {
            uhwIndex                = puhwBitsOrder[i];
            pshwDecSerial[uhwIndex] = ashwEncSerial[i];
        }
    }
    else if (CODEC_AMR_TYPE_RX_SID_UPDATE == enRxType)
    {
        /*������������ת�ɷǽ������������*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pshwAmrFrame,
                                AMR_SID_UPDATE_LENGTH,
                                ashwEncSerial);

        /*��ȡ35���ص��������ݣ�������з����ţ������pDecodeSpeech*/
        CODEC_OpVecCpy(pshwDecSerial, ashwEncSerial, AMR_SID_UPDATE_LENGTH);
    }
    else
    {
        /* �����������������ʹ��֡���������ݣ���˲������� */
    }

    return CODEC_RET_OK;
}


VOS_UINT32 AMR_ImsDlConvert(IMS_DL_DATA_OBJ_STRU *pstSrc,AMR_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
{
    VOICE_WPHY_AMR_SERIAL_STRU stImsDlData;
    VOS_UINT8                 *puchData     = NULL;
    VOS_UINT8                  uchRateMode  = 0;
    VOS_UINT8                  uchSti       = 0;
    VOS_UINT8                  uchBitLeft   = 0;
    VOS_UINT8                  uchTemp      = 0;

    /* ���������ʱ���� */
    UCOM_MemSet(&stImsDlData,0,sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /* ErrorFlag��⡢FrameType��⣬���ڵ�ǰFrameType��Χ�ڣ����nodata֡ */
    if ((IMSA_VOICE_ERROR == pstSrc->usErrorFlag) || (AMR_FRAMETYPE_NUM <= pstSrc->usFrameType))
    {
        /* ����nodata֡ */
        /* ABC�������� */
        stImsDlData.auhwLen[AMR_SUB_FRM_A] = 0;
        stImsDlData.auhwLen[AMR_SUB_FRM_B] = 0;
        stImsDlData.auhwLen[AMR_SUB_FRM_C] = 0;
        stImsDlData.enQualityIdx = pstSrc->usQualityIdx;
    }
    else
    {
        /*****************���hifi��amr֡�ṹ*****************/
        /* ���֡ͷ */
        /* ABC�������� */
        stImsDlData.auhwLen[AMR_SUB_FRM_A] = g_auwAmrMap[pstSrc->usFrameType][AMR_SUB_FRM_A];
        stImsDlData.auhwLen[AMR_SUB_FRM_B] = g_auwAmrMap[pstSrc->usFrameType][AMR_SUB_FRM_B];
        stImsDlData.auhwLen[AMR_SUB_FRM_C] = g_auwAmrMap[pstSrc->usFrameType][AMR_SUB_FRM_C];
        stImsDlData.enQualityIdx = pstSrc->usQualityIdx;

        /* ��������� */
        AMR_FilledImsDlData((VOS_UINT8 *)pstSrc->ausData,&stImsDlData);

        /* ��SID���д��� */
        if(AMR_SID_A_LENGTH == stImsDlData.auhwLen[AMR_SUB_FRM_A])
        {
            puchData    = (MED_UINT8*)&stImsDlData.auwData[0];
            uchRateMode = (puchData[4]&0x0e)<<3;      /*ModeIndication:3*/
            uchSti      = (puchData[4]&0x10)>>1;      /*Sti:1*/
            uchBitLeft  = (puchData[4]&0xe0)>>4;      /*SidData2:3*/
            uchTemp     = (uchRateMode|uchSti)|(uchBitLeft);
            puchData[4] = uchTemp;
        }
    }

    /*���������е���������ת��Ϊ�ǽ����������ݣ����������ű���з�����*/
    AMR_UmtsDlConvert(&stImsDlData,
                       pstDecSerialAfter);

    return CODEC_RET_OK;
}
VOS_VOID AMR_ImsUlConvert(VOS_UINT16 uhwAMRType,VOS_UINT16 uhwMode,VOS_INT16* pstEncSerial)
{
    IMS_UL_DATA_OBJ_STRU stUlDataObj;
    VOS_UINT16           uhwFrameType;
    VOS_UINT8           *puchData = NULL;

    UCOM_MemSet(&stUlDataObj,0,sizeof(IMS_UL_DATA_OBJ_STRU));

    puchData = (MED_UINT8*)stUlDataObj.ausData;

    /*****************�����Ϣ*****************/
    /*****************���IMSA��amr֡�ṹ*****************/
    /* ��������� */
    if(CODEC_AMR_TYPE_TX_NO_DATA != uhwAMRType)
    {
        AMR_FilledImsUlData(&puchData[0],(VOICE_WPHY_AMR_SERIAL_STRU*)pstEncSerial);
    }

    /* ��ȡ֡���� */
    switch(uhwAMRType)
    {
        /* SID֡ */
        case CODEC_AMR_TYPE_TX_SID_FIRST:
        {
            uhwFrameType = AMR_FRAMETYPE_SID;
            puchData[4]  = (puchData[4]&0xe0)|((MED_UINT8)(uhwMode<<1));
        }break;
        case CODEC_AMR_TYPE_TX_SID_UPDATE:
        {
            uhwFrameType = AMR_FRAMETYPE_SID;
            puchData[4]  = (puchData[4]&0xe0)|(0x10|((MED_UINT8)(uhwMode<<1)));

        }break;
        /* NOData֡ */
        case CODEC_AMR_TYPE_TX_NO_DATA:
        {
            uhwFrameType = AMR_FRAMETYPE_NODATA;
        }break;
        /* speech֡ */
        default:
        {
            uhwFrameType = (MED_UINT8)(uhwMode);
        }break;
    }

    stUlDataObj.usStatus    = uhwAMRType;   /* ����֡����SID֡����֡���ǻ�֡ */
    stUlDataObj.usFrameType = uhwFrameType; /* ֡���� */

    /* ����Encode Buffer���ݣ��ó�����IMS���ݽṹ */
    UCOM_MemSet(pstEncSerial,0,sizeof(VOS_UINT16)*VOICE_CODED_FRAME_LENGTH);
    UCOM_MemCpy(&pstEncSerial[0],&stUlDataObj,sizeof(IMS_UL_DATA_OBJ_STRU));

}


VOS_VOID AMR_FilledImsDlData( VOS_UINT8 *puchData,VOICE_WPHY_AMR_SERIAL_STRU *pstDes)
{
    MED_UINT8*  puchSrc;
    MED_UINT32* puwADes;
    MED_UINT32* puwBDes;
    MED_UINT32* puwCDes;

    /* A B C�������� */
    MED_UINT32 uwALen = pstDes->auhwLen[AMR_SUB_FRM_A];
    MED_UINT32 uwBLen = pstDes->auhwLen[AMR_SUB_FRM_B];
    MED_UINT32 uwCLen = pstDes->auhwLen[AMR_SUB_FRM_C];

    MED_UINT32 uwCurBit = 0;   /* ��ǰbit���� */

    puchSrc = puchData; /* ȥ��֡ͷ��������� */

    /* ������Ҫ����A B C������ʼ��ַ,32bit���� */
    puwADes = &(pstDes->auwData[0]);
    puwBDes = ((0==(uwALen%32))?(puwADes + (uwALen/32)):(puwADes + (uwALen/32) + 1));
    puwCDes = ((0==(uwBLen%32))?(puwBDes + (uwBLen/32)):(puwBDes + (uwBLen/32) + 1));

    /* ���A���� */
    AMR_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwADes,&uwCurBit,uwALen);

    /* ���B���� */
    AMR_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwBDes,&uwCurBit,uwBLen);

    /* ���C���� */
    AMR_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwCDes,&uwCurBit,uwCLen);

}
VOS_VOID AMR_FilledImsUlData( VOS_UINT8   *puchImsaData,
                                    VOICE_WPHY_AMR_SERIAL_STRU* pstHifiData)
{
    VOS_UINT8*  puchDes;
    VOS_UINT32* puwASrc;
    VOS_UINT32* puwBSrc;
    VOS_UINT32* puwCSrc;

    /* A B C�������� */
    VOS_UINT32 uwALen = (VOS_UINT32)pstHifiData->auhwLen[AMR_SUB_FRM_A];
    VOS_UINT32 uwBLen = (VOS_UINT32)pstHifiData->auhwLen[AMR_SUB_FRM_B];
    VOS_UINT32 uwCLen = (VOS_UINT32)pstHifiData->auhwLen[AMR_SUB_FRM_C];

    VOS_UINT32 uwCurBit = 0;   /* ��ǰbit���� */

    /* ������Ҫ����A B C������ʼ��ַ,32bit���� */
    puwASrc = &(pstHifiData->auwData[0]);
    puwBSrc = ((0 == (uwALen%32))?(puwASrc + (uwALen/32)):(puwASrc + (uwALen/32) + 1));
    puwCSrc = ((0 == (uwBLen%32))?(puwBSrc + (uwBLen/32)):(puwBSrc + (uwBLen/32) + 1));

    /* ����Ŀ�ĵ�ַ */
    puchDes = puchImsaData;

    /* ���A���� */
    AMR_ImsUlBitFilled((VOS_UINT8*)puwASrc,puchDes,&uwCurBit,uwALen);

    /* ���B���� */
    AMR_ImsUlBitFilled((VOS_UINT8*)puwBSrc,puchDes,&uwCurBit,uwBLen);

    /* ���C���� */
    AMR_ImsUlBitFilled((VOS_UINT8*)puwCSrc,puchDes,&uwCurBit,uwCLen);
}
VOS_VOID AMR_ImsUlBitFilled(VOS_UINT8* puchSrc,VOS_UINT8* puchDes,
                                 VOS_UINT32* puwCurBit,VOS_UINT32 uwLen)
{
    VOS_UINT32 i = 0;
    VOS_UINT8  uchVal = 0;
    VOS_UINT8  auwBitShift[8]
    ={UCOM_BIT7,UCOM_BIT6,UCOM_BIT5,UCOM_BIT4,UCOM_BIT3,UCOM_BIT2,UCOM_BIT1,UCOM_BIT0};

    for(i = 0; i < uwLen; i++)
    {
        uchVal     = (puchSrc[i/8]>>auwBitShift[i%8])&0x01;
        uchVal     = (VOS_UINT8)(uchVal<<auwBitShift[(*puwCurBit)%8]);
        puchDes[(*puwCurBit)/8]  = (uchVal)|(puchDes[(*puwCurBit)/8]);
        (*puwCurBit) += 1;
    }
}


VOS_VOID AMR_ImsDlBitFilled(VOS_UINT8* puchSrc,VOS_UINT8* puchDes,
                                 VOS_UINT32* puwCurBit,VOS_UINT32 uwLen)
{
    VOS_UINT32 i = 0;
    VOS_UINT8  uchVal = 0;
    VOS_UINT8  auwBitShift[8]
    ={UCOM_BIT7,UCOM_BIT6,UCOM_BIT5,UCOM_BIT4,UCOM_BIT3,UCOM_BIT2,UCOM_BIT1,UCOM_BIT0};

    for(i = 0; i < uwLen; i++)
    {
        uchVal     = (puchSrc[(*puwCurBit)/8]>>auwBitShift[(*puwCurBit)%8])&0x01;
        uchVal     = (VOS_UINT8)(uchVal<<auwBitShift[i%8]);
        puchDes[i/8]  = (uchVal)|(puchDes[i/8]);
        (*puwCurBit) += 1;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


