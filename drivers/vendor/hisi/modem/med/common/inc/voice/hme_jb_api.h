/**************************************************************************************************
Copyright (C), 2014-2024, Huawei Tech. Co., Ltd.

File name       : 
Version: 1.00       
Description     : 

Others          :
Function List   :
1: ...
History         :
Date            ver         Modification
    
**************************************************************************************************/

#ifndef _HME_JB_H_
#define _HME_JB_H_


//����ֻ�ṩһ���ӿ��ļ����ӿ��ļ�ʱ�����ݽṹ����

 typedef char           int8;
typedef signed short    int16;  
 typedef signed int     int32;
 typedef unsigned char  uint8;
 typedef unsigned short uint16;
 typedef unsigned int   uint32;
            
 #ifdef __cplusplus
 #define HME_EXTERN_C_BEGIN         extern "C" {
 #define HME_EXTERN_C_END           }
#else
 #define HME_EXTERN_C_BEGIN
 #define HME_EXTERN_C_END
#endif

//#define HME_JB_MEMSIZE      45680              //�����ڴ�������45680 bytes 

#define HME_JB_MEMSIZE      HME_JB_GetMemorySize()              //�����ڴ������� bytes 
#define HME_VERSTR_LENGTH   30


//����ֵ˵��
#define    HME_JB_OK                   0               //���óɹ�        
#define    HME_JB_ERR_POINTER_NULL     1               //��ָ��NULL��ָ��
#define    HME_JB_ERR_JBMODE           2               //�����JB����ģʽ����sJBMode����
#define    HME_JB_ERR_DELAY            3               //�����ʱ�ӳ������ֵ���������ʱ��С�ڳ�ʼʱ��
#define    HME_JB_ERR_FREQUENCY        4               //����Ĳ���Ƶ�ʲ���ulFrequency����     
#define    HME_JB_ERR_REINIT           5               //�ظ�����JB���³�ʼ���ӿ�
#define    HME_JB_SSRC_CHANGE          6               //SSRC�����仯
#define    HME_JB_ERR_UNINIT           7               //JBδ��ʼ��
#define    HME_JB_ERR_ININITING        8               //AJB���ڳ�ʼ���У���ʼ����ɺ���ܴ�JB�л�ȡ����
#define    HME_JB_ERR_PARAM            9               //�����������
#define    HME_JB_ERR                  10              //
#define    HME_JB_RE_INIT              11              //���³�ʼ��


//RTP����ʽ
typedef struct _HME_JB_RTP_INFO
{
    uint16 iCodecType;                 //����������
    uint16 iFrameLength;               //һ֡����(ms)
    uint8* pPayload;                   //RTP�������ݵ�ָ��
    uint32 ulPayloadLength;            //�������ݳ��ȣ���λ�ֽ�
    uint16 usSequenceNumber;           //���кţ�16���أ�RTP�������к�
    uint16 uFreq;                      //������
    uint32 ulTimestamp;                //ʱ����32����
    uint32 ulSSRC;                     //ssrc,Դ������
    uint32 u32Time;                    //ϵͳʱ��
    uint8  ucPayloadType;              //�������ͣ�7���أ��������˸��صĸ�ʽ���ɾ���Ӧ�þ��������
    int8   iRes[7];                    //����ʹ��
    
    //�ڲ�ʹ��
    int32 lNetDelay;       //����ʱ��
    int32 lEstTotalDelay;  //Ŀ����ʱ��
    int32 lRealTotalDelay; //���㷨ģ���м����ʵ����ʱ��
}HME_JB_RTP_INFO;


//����jitter buf����ģʽ
typedef enum _HME_JB_MODE_ENUM
{
   HME_JBMODE_AUTOMATIC = 0,            //�Զ�ģʽ���Զ�����JB��������,�ʺ�WiFi��������
   HME_JBMODE_MANUAL = 1,               //�ֶ�ģʽ,���ʹ��JB��������           
   HME_JBMODE_STATIC   = 2              //��̬JB
}HME_JB_MODE_ENUM;

//����jitter buf����
typedef  struct _HME_JB_PARAMS_STRU
{
    uint16 uiInitJitterBuf;              //��ʼ���(��̬jb��Ч,��̬jbĬ��40ms����̬��������Ϊ100ms)
    uint16 uiMinJitterBuf;               //��СJB���(��̬jb��Ч,ms)
    uint16 uiMaxJitterBuf;               //���JB���,�ⲿ���仺�����(ms)
    int8   iRes[2];                      //4�ֽڶ���
}HME_JB_PARAMS_STRU;


//�������ݽṹ
typedef struct _HME_JB_DEC_STRU
{
    int16   ucPayloadType;               //���������ͻ�PTֵ
    int16   bfi;                         //badָ֡ʾλ 0��good֡ 1��bad֡/���� (��Ϊ1ʱ��ʾҪ���ý�������plc���ܣ�pcPayload��ΪNULL)
    int8    *pcPayload;                  //������������ָ��
    uint32  ulInputLength;               //���������������ȣ����ֽ�Ϊ��λ���Ǵ���֡ʱ������Ϊ0
    int16   *psOutputData;               //�������PCM����ָ��
    uint32  ulOutputLength;              //�������PCM���ݳ��ȣ�������Ϊ��λ
    int8    bPreFrameType;               //���һ֡��֡���ͣ�������֡���Ƿ�����֡(3��ʾ����֡��2��ʾsid,1��ʾnodata,0��ʾ��֡)
    int8    iRes[7];                     //����ʹ��
} HME_JB_DEC_STRU;


//ͳ����Ϣ�ṹ��
typedef struct _HME_JB_STATISTICS_INF
{
    uint16  uiJBOutLossRate;             //5s�ڣ�JB���ڵĶ�����
    uint16  uiTooLateToLossRate;         //5s�ڣ��ӳٶ�����
    uint16  uiNetLossRate;               //5s�ڣ����綪����
    uint16  uiBlockRate;                 //5s�ڣ�������
    uint32  uiRecvFrameCount;            //�ܵ��յ���֡��
    uint32  uiTotalNetLossCount;         //�ܵ����綪֡��
    uint32  uiTotalLossFrameCount;       //�ܵĶ�֡��
    int32   iFrameSpeechCount;           //����֡��
    int32   iFrameSilenceCount;          //����֡��
    int8    iRes[8];                     //����ʹ��
}HME_JB_STATISTICS_INF;


//JB����ṹ��
typedef struct _HME_JB_BUFFER_STATUS
{
   uint16     uBufferLen;            //jb��������ǰ���
   uint16     uFrameCount;           //��ǰ�������е�֡��
   uint16     uMeanBufferDelay;      //ƽ������ʱ��
   uint16     uiMinJitterBuf;        //��СJB���
   uint16     uiMaxJitterBuf;        //��󻺳����
   int8       iRes[6];               //����ʹ��
}HME_JB_BUFFER_STATUS;


typedef struct _HME_JB_LOG_INFO
{
    int16 usLevel;
    int16 usFileNo;
    int16 usLineNo;
    int16 usFreqency;
    int32 uiLogId;//err code
    int32 idate1;
    int32 idate2; 
    int32 idate3;
}HME_JB_LOG_INFO;


typedef struct _HME_JB_HIFI_STATICS
{
    uint32   uiFrameType;            //0/1/2 ����֡�����磬����֡
    uint32   uiBufLen;               //��ǰ�������(ms)
    uint32   uiPacketDelay;          //������ʱ��
    uint32   uiAveBufDelay;          //ƽ������ʱ��
    uint32   uiStartDelay;           //��һ���Ļ���ʱ��
    uint32   uiBufMaxSize;           //���Ļ����С(֡��)

    uint32   uiSeq;                  //�����
    uint32   uiTimeStamp;            //��ʱ��
    uint32   uiSSRC;                 //ssrc
    uint32   iTimeRecv;              //������ʱ��
    int32    iNetDelay;              //��ǰ����ʱ��
    int32    iEstDelay;              //Ŀ��ʱ��
    int32    iTotalDelay;            //������ʱ��
    int32    iRealTotalDelay;        //ʵ����ʱ��
    int32    iAveNetDelay;           //ƽ������ʱ��
    int32    iAveNetJitter;          //ƽ�����綶��

    uint32   uiPacketRecv;           //�ܵ��հ���
    uint32   uiPacketLoss;           //�ܵĶ�����
    uint32   uiJBPacketLoss;         //jb����������
    uint32   uiPlcCount;             //�ܵ�plc����
    uint32   uiTSMCount;             //TSM����
    uint32   uiTSMDateLen;           //TSM�ܵĴ���ʱ��(ms)
    int8     iRes[8];                //����ʹ��
}HME_JB_HIFI_STATICS;


typedef struct _HME_JB_TRACE_INFO
{
    uint16 usTraceId;
    uint16 usDataLen;
    void * pData;  
}HME_JB_TRACE_INFO;


//����ص�����ָ�����Ͷ���,HME_JB_DEC_STRU
typedef int32 (*pDecode)(void *pstJBmodule, void *pstDecPrm);

//ע��ʱ���ȡ����,��ȡϵͳʱ�䵥λ����
typedef int32 (*pSystemTime)(uint32 *piTimeMillisecond);

//ά���ӿڷ���ע��HME_JB_LOG_INFO
//(1)����ע��д��־  iErrCode ��ʾ�����룬pDate��ʾҪ��ӡ����־��Ϣ��iDateLen��ʾҪ��ӡ����־��Ϣ�ĳ���
typedef int32  (*pWriteLog)(void *pLogMessage);

//(2)����ע��дTrace����,pDate��ʾpcm����ָ�룬iDateLen��ʾÿ��д���pcm���ݳ���HME_JB_TRACE_INFO
typedef int32 (*pWriteTrace)(void *pTraceData);


//jbģ���ʼ��ʹ�ýṹ��
typedef struct _HME_JB_CREATE_STRU_
{
    HME_JB_MODE_ENUM           enJBMode;         //jb����ģʽ
    HME_JB_PARAMS_STRU         stJBParams;      //jb������Ϣ
    void                       *pMemAddr;       //�ⲿ����JBģ��ʹ�õ��ڴ��ַ(HME_JB_MEMSIZE��ʾ)
    pSystemTime                pGetTime;        //ϵͳʱ���ȡ����ע��ӿ�
    pWriteLog                  pLog;            //��־�ص�ע��ӿ�
    pWriteTrace                pTrace;          //Trace�ص�ע��ӿ�
    int8                       iRes[8];         //����ʹ��
}HME_JB_CREATE_STRU;


HME_EXTERN_C_BEGIN

int32 HME_JB_GetMemorySize();


//1�������ܺ���
/**************************************************************************************************
Function        : HME_JB_Init
Description     : JBģ��Ĵ����ͳ�ʼ��
Input Parameter : pstJBCreate   JBģ����Ҫʹ�õĹ���ģʽ/JB������Ϣ��������ڴ��ַ���ⲿ�ӿ�ע��
Output Parameter: pstJBmodule  JBģ����
Return Type     : 
Other           : ��
**************************************************************************************************/
int32 HME_JB_Init(void** ppstJBmodule, HME_JB_CREATE_STRU *pstJBCreate);

/**************************************************************************************************
Function        : HME_JB_RegDecode
Description     : ����������������ע���JBģ��
Input Parameter : pstJBmodule                JBģ����
                  pStreamRecv                HME����ʹ�ñ�����������Ʒֱ�Ӵ���NULL
                  pStreamRecvDecode          ����뺯��ָ��
                  uiFrequency                ����Ƶ��(8000/16000/24000/48000)
Output Parameter:
Return Type     : 
Other           : ͨ�����л�codecҲ����ʹ��
**************************************************************************************************/
int32 HME_JB_RegDecode(void* pstJBmodule, void *pStreamRecv,
                      pDecode pStreamRecvDecode, uint32 uiFreqency);
                      
/**************************************************************************************************
Function        : HME_JB_AddPacket
Description     : ��RTP���ݰ����뵽JB�İ������У�������JB���㷨ģ�顣
Input Parameter : pstJBmodule  JBģ����
                  pRtpInf      RTP֡��Ϣ
Output Parameter:
Return Type     : 
Other           : ��
**************************************************************************************************/
int32 HME_JB_AddPacket(void* pstJBmodule, HME_JB_RTP_INFO* pRtpRtcpPacket);

/**************************************************************************************************
Function        : HME_JB_GetPcmData
Description     : ��JBģ���ȡָ�����ȵ�pcm����
Input Parameter : pstJBmodule  JBģ����
                  uinLength    ��Ҫ��ȡ�����ݳ���(��λms,��Ҫ��5ms��������)
Output Parameter: pPcmOut      ���pcm����ָ��
Return Type     : 
Other           : ��
**************************************************************************************************/
int32 HME_JB_GetPcmData(void* pstJBmodule,
                                       uint16             usLength,
                                       void*              pPcmOut,
                                       uint32*            puiOutLen);


//2�����ú���
/**************************************************************************************************
Function        : HME_JB_GetVersion
Description     : ��ȡ�汾��Ϣ
Input Parameter : pcVersion         �汾��Ϣ����ָ��
                  iLen              �汾��Ϣ����
Output Parameter:
Return Type     : 
Other           : ��
**************************************************************************************************/
int32 HME_JB_GetVersion(int8 *pcVersion, int32 StrLen);


/**************************************************************************************************
Function        : HME_JB_GetStatisticsInf
Description     : ��ȡͳ����Ϣ
Input Parameter : pStatisticsInf    jbͳ����Ϣָ��
Output Parameter: pStatisticsInf    jbͳ����Ϣ
Return Type     : 
Other           : ��
**************************************************************************************************/
int32 HME_JB_GetStatisticsInf(void* pstJBmodule, HME_JB_STATISTICS_INF *pStatistics);

/**************************************************************************************************
Function        : HME_JB_GetStatisticsInf
Description     : ��ȡͳ����Ϣ
Input Parameter : pBufferStatus      jb������Ϣָ��
Output Parameter:pBufferStatus       jb������Ϣ 
Return Type     : 
Other           : ��
**************************************************************************************************/
int32 HME_JB_GetBufferStatus(void* pstJBmodule, HME_JB_BUFFER_STATUS *pBufferStatus);


//3��ά���ӿ�
/**************************************************************************************************
Function        : HME_JB_SetMaintainStatus
Description     : ������־��Trace�Ĵ�ӡ״̬
Input Parameter : 
                  iFileWrite        ��־��ӡ״̬��0����ӡ����0��ӡ
                  iTraceWrite       Trace���ݴ�ӡ״̬��0����ӡ����0��ӡ
Output Parameter:
Return Type     : 
Other           : Ĭ�ϲ���ӡ��־��Trace����
**************************************************************************************************/
int32 HME_JB_SetMaintainStatus(int8 iFileWrite,int8 iTraceWrite);


//4�������ӿ�
void HME_JB_ResetStatistics(void *pstJbHandle);

HME_EXTERN_C_END
#endif

/**************************************************************************************************
End of File
**************************************************************************************************/
