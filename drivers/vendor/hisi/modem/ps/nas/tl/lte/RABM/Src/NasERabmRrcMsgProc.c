




/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "EmmRabmInterface.h"
#include  "NasERabmRrcMsgProc.h"
#include  "NasERabmAppMsgProc.h"
#include  "NasERabmEsmMsgProc.h"
#include  "CdsErabmInterface.h"
#include  "NasERabmCdsMsgProc.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASRABMRRCMSGPROC_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


/*****************************************************************************
  3 Function
*****************************************************************************/
#if 0
#ifdef PS_ITT_PC_TEST_NAS
    VOS_VOID  APPITF_FreeAllofRabmBuf( VOS_VOID )
    {
        return ;
    }

    VOS_VOID APPITF_SendDataofRabmBuf( VOS_VOID )
    {
        return ;
    }
#endif
#endif


/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_ERABM_RrcMsgDistr( VOS_VOID *pRcvMsg )
{
    PS_MSG_HEADER_STRU            *pRrcMsg;

    pRrcMsg = (PS_MSG_HEADER_STRU*)pRcvMsg;

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RrcMsgDistr is entered.");

    /*根据消息名，调用相应的消息处理函数*/
    switch (pRrcMsg->ulMsgName)
    {
        /*如果收到的是RRC_RABM_RAB_IND消息，调用NAS_ERABM_RcvRabmRrcRabInd函数*/
        case ID_LRRC_LRABM_RAB_IND:
            NAS_ERABM_RcvRabmRrcRabInd((LRRC_LRABM_RAB_IND_STRU *) pRcvMsg);
            break;

        /*如果收到的是RRC_RABM_STATUS_IND消息，调用NAS_ERABM_RcvRabmRrcStatusInd函数*/
        case ID_LRRC_LRABM_STATUS_IND:
            NAS_ERABM_RcvRabmRrcStatusInd((LRRC_LRABM_STATUS_IND_STRU *) pRcvMsg);
            break;

        default:
            NAS_ERABM_WARN_LOG("NAS_ERABM_RrcMsgDistr:WARNING:RRC->RABM Message name non-existent!");
            break;
    }
}



VOS_VOID NAS_ERABM_RcvRbSetupSucc( VOS_UINT32 ulEpsbId, VOS_UINT32 ulRbId )
{
    /*APP_DATA_STATUS_CALLBACK_FUNC AppDataStatusFun;*/

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRbSetupSucc is entered.");

    /*把RBID记录在EPS承载激活表中*/
    NAS_ERABM_SetEpsbRbIdInfo(ulEpsbId,ulRbId);

    /*记录RB的连接状态*/
    NAS_ERABM_SetRbStateInfo(ulEpsbId, NAS_ERABM_RB_CONNECTED);

    /*通知APP启动数传，调用APP提供的函数*/
    /*AppDataStatusFun(NAS_ERABM_GetEpsbIdInfo(ulEpsbId),NAS_ERABM_APP_DATA_STATUS_START,NAS_ERABM_APP_DATA_START_RBSETUP_SUCC);*/
}

VOS_VOID NAS_ERABM_RcvRbReconfig( VOS_UINT32 ulEpsbId, VOS_UINT32 ulRbId )
{
    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRbReconfig is entered.");

    /* 只有承载处于激活状态才修改关联关系 */
    if (NAS_ERABM_EPSB_ACTIVE == NAS_ERABM_GetEpsbStateInfo(ulEpsbId))
    {
        /*把RBID记录在EPS承载激活表中*/
        NAS_ERABM_SetEpsbRbIdInfo(ulEpsbId,ulRbId);

        /* 2016-02-18, 防止DRB已经建立，但是RRC没有通知ERABM SetUp而是直接通知ReConfig的情况出现，此处增加保护来记录该EPSBID已处于连接状态 */
        NAS_ERABM_SetRbStateInfo(ulEpsbId, NAS_ERABM_RB_CONNECTED);

        NAS_ERABM_LOG2("NAS_ERABM_RcvRbReconfig, RAB change:", ulEpsbId, ulRbId);
    }
}


VOS_VOID  NAS_ERABM_RcvRbSetupFail( VOS_UINT32 ulEpsbId )
{
    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRbSetupFail is entered.");

    /*把RBID置为无效ID*/
    NAS_ERABM_SetEpsbRbIdInfo(ulEpsbId, NAS_ERABM_ILL_RB_ID);

    /*记录RB的非连接状态*/
    NAS_ERABM_SetRbStateInfo(ulEpsbId, NAS_ERABM_RB_DISCONNECTED);
}

/*****************************************************************************
 Function Name   : NAS_ERABM_RcvRbRelease
 Description     : 处理rrc释放rb消息
 Input           : VOS_UINT32 ulEpsbId
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing49683              2009-4-27  Draft Enact
    2.lihong00150010            2009-12-24  Modify

*****************************************************************************/
VOS_VOID NAS_ERABM_RcvRbRelease( VOS_UINT32 ulEpsbId )
{
    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRbRelease is entered.");

    /*把RBID置为无效ID*/
    NAS_ERABM_SetEpsbRbIdInfo(ulEpsbId, NAS_ERABM_ILL_RB_ID);

    /*记录RB的非连接状态*/
    NAS_ERABM_SetRbStateInfo(ulEpsbId, NAS_ERABM_RB_DISCONNECTED);
}

#if 0
VOS_VOID  NAS_ERABM_RbSetupReqProc( VOS_UINT32 ulRbNum )
{
    VOS_UINT8                      ucEpsbActFlag = NAS_ERABM_EPSB_ACTIVE;   /*默认所有请求建立RB所对应的EPS承载为激活态*/
    VOS_UINT32                     ulEpsbNum     = 0;                      /*记录对应EPS承载激活的数量*/
    VOS_UINT32                     ulEpsbId      = NAS_ERABM_ILL_EPSB_ID;
    VOS_UINT32                     ulLoop        = 0;
    LRRC_LRABM_RAB_RSP_STRU          stRabmRabRsp  = {0};

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RbSetupReqProc is entered.");

    /*扫描承载激活临时表*/
    for (ulLoop = 0; ulLoop < ulRbNum; ulLoop++)
    {
        ulEpsbId = NAS_ERABM_GetRbSetupEpsbIdInfo(ulLoop);
        if (NAS_ERABM_EPSB_ACTIVE == NAS_ERABM_GetEpsbStateInfo(ulEpsbId))
        {
            stRabmRabRsp.aulRabId[ulEpsbNum] = ulEpsbId;
            ulEpsbNum++;
        }
        else
        {
            ucEpsbActFlag = NAS_ERABM_EPSB_INACTIVE;
            break;
        }
    }

    /*如果要求建立的RB对应的所有EPS承载都激活*/
    if (NAS_ERABM_EPSB_ACTIVE == ucEpsbActFlag)
    {
        /*向RRC回复RRC_RABM_RAB_RSP消息*/
        NAS_ERABM_SndRabmRrcRabRsp(stRabmRabRsp.aulRabId, ulEpsbNum);
    }
    else
    {
        /*判断是否有等待EPS承载激活标识*/
        if (NAS_ERABM_NOT_WAIT_EPSB_ACT_MSG == NAS_ERABM_GetWaitEpsbActSign())
        {
            /*启动定时器*/
            NAS_ERABM_TimerStart(NAS_ERABM_WAIT_EPSB_ACTT_LENGTH, NAS_ERABM_WAIT_EPSB_ACT_TIMER);

            /*设置等待EPS承载激活标识*/
            NAS_ERABM_SetWaitEpsBActSign(NAS_ERABM_WAIT_EPSB_ACT_MSG);
        }
    }
}
#endif
VOS_VOID  NAS_ERABM_InformEsmBearerStatus( VOS_UINT8 uclOpid )
{
    VOS_UINT32                          ulRbNumWithRb  = NAS_ERABM_NULL;
    VOS_UINT32                          ulActEpsbNum   = NAS_ERABM_NULL;
    VOS_UINT32                          aulEpsIdWithRb[NAS_ERABM_MAX_EPSB_NUM] = {0};
    VOS_UINT32                          ulEpsbId         = NAS_ERABM_NULL;

    for (ulEpsbId = NAS_ERABM_MIN_EPSB_ID; ulEpsbId<= NAS_ERABM_MAX_EPSB_ID; ulEpsbId++)
    {
        NAS_ERABM_LOG2("NAS_ERABM_InformEsmBearerStatus", NAS_ERABM_GetEpsbStateInfo(ulEpsbId), NAS_ERABM_GetRbStateInfo(ulEpsbId));

        if (NAS_ERABM_EPSB_ACTIVE == NAS_ERABM_GetEpsbStateInfo(ulEpsbId))
        {
            ulActEpsbNum ++;
            if(NAS_ERABM_RB_DISCONNECTED != NAS_ERABM_GetRbStateInfo(ulEpsbId))
            {
                aulEpsIdWithRb[ulRbNumWithRb] = ulEpsbId;
                ulRbNumWithRb++;
            }
        }
    }

    NAS_ERABM_LOG1("NAS_ERABM_InformEsmBearerStatus", ulActEpsbNum);

    /*只有在存在EPS承载的场景下，才需要通知ESM*/
    if(ulActEpsbNum > 0)
    {
        NAS_ERABM_SndRabmEsmBearerStatusReq(aulEpsIdWithRb,ulRbNumWithRb,uclOpid);
    }
}
VOS_VOID  NAS_ERABM_RcvRabmRrcRabInd(const LRRC_LRABM_RAB_IND_STRU *pRcvMsg )
{
    VOS_UINT32                          ulRbSetupReqNum  = 0;             /*记录请求建立的RB数量*/
    VOS_UINT32                          ulLoop           = 0;
    VOS_UINT32                          ulRabcnt         = pRcvMsg->ulRabCnt;
    PS_BOOL_ENUM_UINT8                  enIsRecSetupSucc = PS_FALSE;
    PS_BOOL_ENUM_UINT8                  enIsRecSetupFail = PS_FALSE;
    PS_BOOL_ENUM_UINT8                  enIsRecRelease   = PS_FALSE;
    VOS_UINT32                          aulRabId[NAS_ERABM_MAX_EPSB_NUM] = {0};
    /*APP_DATA_STATUS_CALLBACK_FUNC     AppDataStatusFun = APP_DATA_STATUS_FUNC;*/

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmRrcRabInd is entered.");

    /*根据RB的操作类型进行相应处理*/
    for (ulLoop = 0; ulLoop < ulRabcnt; ulLoop++)
    {
        switch ( pRcvMsg->astRabInfo[ulLoop].enRabChangeType)
        {
            /*收到请求建立RB消息，LTE系统中不会收到RRC_NAS_RAB_SETUP_REQUEST*/
            case LRRC_LNAS_RAB_SETUP_REQUEST:
                /*把要求建立的RB的消息保存在临时表中*/
                NAS_ERABM_SetRbSetupRbIdInfo(ulRbSetupReqNum, pRcvMsg->astRabInfo[ulLoop].ulRbId);
                NAS_ERABM_SetRbSetupEpsbIdInfo(ulRbSetupReqNum, pRcvMsg->astRabInfo[ulLoop].ulRabId);
                ulRbSetupReqNum++;
                NAS_ERABM_SetRbNumInfo(ulRbSetupReqNum);
                break;

            /*收到RB建立成功消息*/
            case LRRC_LNAS_RAB_SETUP_SUCCESS:
                NAS_ERABM_RcvRbSetupSucc( pRcvMsg->astRabInfo[ulLoop].ulRabId, pRcvMsg->astRabInfo[ulLoop].ulRbId );
                enIsRecSetupSucc = PS_TRUE;
                break;

            /*收到RB建立失败消息*/
            case LRRC_LNAS_RAB_SETUP_FAILURE:
                NAS_ERABM_RcvRbSetupFail( pRcvMsg->astRabInfo[ulLoop].ulRabId );
                enIsRecSetupFail = PS_TRUE;
                break;

            /*收到RB释放消息*/
            case LRRC_LNAS_RAB_RELEASE:
                NAS_ERABM_RcvRbRelease(pRcvMsg->astRabInfo[ulLoop].ulRabId);
                enIsRecRelease = PS_TRUE;

                /*通知APP停止数传,调用APP提供的函数*/
                /*AppDataStatusFun(pRcvMsg->astRabInfo[ulLoop].ulRabId,NAS_ERABM_APP_DATA_STATUS_STOP,0xFF);*/
                break;
            /*收到DRB重配消息*/
            case LRRC_LNAS_RAB_RECONFIG:
                NAS_ERABM_RcvRbReconfig(pRcvMsg->astRabInfo[ulLoop].ulRabId, pRcvMsg->astRabInfo[ulLoop].ulRbId);
                break;
            default:
                /*打印错误消息*/
                NAS_ERABM_ERR_LOG("NAS_ERABM_RcvRabmRrcRabInd: the message is error!");
                break;
         }
    }

    /* 根据收到的请求建立的RB，原封不动地形成RRC_RABM_RAB_RSP消息 */
    if (ulRbSetupReqNum > 0)
    {
        for (ulLoop = 0; ulLoop < NAS_ERABM_GetRbNumInfo(); ulLoop++)
        {
            aulRabId[ulLoop] = NAS_ERABM_GetRbSetupEpsbIdInfo(ulLoop);
        }

        /*向RRC回复RRC_RABM_RAB_RSP消息*/
        NAS_ERABM_SndRabmRrcRabRsp(aulRabId, NAS_ERABM_GetRbNumInfo());

        return ;
    }

    /* RRC_RABM_RAB_IND消息结果处理 */
    NAS_ERABM_RrcRabIndResultProc(enIsRecSetupSucc, enIsRecSetupFail, enIsRecRelease);
}

VOS_VOID  NAS_ERABM_RcvRabmRrcStatusMsgProc( VOS_UINT32 ulRabCnt,
                                                             VOS_UINT32 ulStatus,
                                                             const VOS_UINT32 *pulEpsbId)

{
    VOS_UINT32                           ulLoop           = 0;
    VOS_UINT32                           ulAllRbIsConn    = NAS_ERABM_SUCCESS;
    /*APP_DATA_STATUS_CALLBACK_FUNC      AppDataStatusFun;*/

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmRrcStatusMsgProc is entered.");

    if (LRRC_LNAS_RAB_STATUS_STOP == ulStatus)
    {
        for (ulLoop = 0; ulLoop < ulRabCnt; ulLoop++)
        {
            /*更新RB的状态为暂停态*/
            NAS_ERABM_SetRbStateInfo(pulEpsbId[ulLoop], NAS_ERABM_RB_SUSPENDED);

            /*通知APP暂停数传,调用APP提供的函数*/
            /*AppDataStatusFun(pulEpsbId[ulLoop], NAS_ERABM_APP_DATA_STATUS_STOP, NAS_ERABM_APP_DATA_START_RRC_IND);*/
        }
        /* lihong00150010 emergency tau&service begin */
        /*启动保护定时器，超时通知EMM释放RRC链路*/
        NAS_ERABM_TimerStart(   NAS_ERABM_WAIT_DRB_RESUME_LENGTH,
                                NAS_ERABM_WAIT_RB_RESUME_TIMER,
                                NAS_ERABM_WAIT_RB_RESUME_TIMER);
    }   /* lihong00150010 emergency tau&service end */
    else if (LRRC_LNAS_RAB_STATUS_CONTINUE == ulStatus)
    {
        for (ulLoop = 0; ulLoop < ulRabCnt; ulLoop++)
        {
            if ( NAS_ERABM_ILL_RB_ID != NAS_ERABM_GetEpsbRbIdInfo(pulEpsbId[ulLoop]))
            {
                /* 如果当前又承载的DRB由非连接态转至连接态 */
                if (NAS_ERABM_RB_CONNECTED != NAS_ERABM_GetRbStateInfo(pulEpsbId[ulLoop]))
                {
                    ulAllRbIsConn = NAS_ERABM_FAILURE;
                }
                /*更新RB的状态为连接态*/
                NAS_ERABM_SetRbStateInfo(pulEpsbId[ulLoop], NAS_ERABM_RB_CONNECTED);
            }

            /*通知APP恢复数传*/
            /*AppDataStatusFun(pulEpsbId[ulLoop], NAS_ERABM_APP_DATA_STATUS_START, NAS_ERABM_APP_DATA_START_RRC_IND);*/
        }

        NAS_ERABM_TimerStop( NAS_ERABM_WAIT_RB_RESUME_TIMER);

        /* 通知L2发送缓存的数据包 */
        /*APPITF_SendDataofRabmBuf();*/

        /* 只有当有DRB由非连接态转至连接态时才给CDS发二次过滤请求 */
        if (NAS_ERABM_FAILURE == ulAllRbIsConn)
        {
            /* 通知CDS启动二次过滤 */
            NAS_ERABM_SndErabmCdsSndBuffDataInd(CDS_ERABM_SEND_BUFF_DATA_ALLOWED_TYPE_DRB_CONN);

            /* 清除上行数据阻塞标志*/
            NAS_ERABM_SetUpDataPending(EMM_ERABM_NO_UP_DATA_PENDING);
        }

        /* L-GU切换或者CCO失败，回退成功场景下， RRC并不上报LRRC_LRABM_RAB_IND消息，
           而只上报LRRC_LRABM_STATUS_IND消息恢复数传，ERABM通过EMM_ERABM_DRB_SETUP_IND
           消息通知EMM进入信令连接态；目前是收到恢复数传消息都发送EMM_ERABM_DRB_SETUP_IND
           消息，后续可以优化成切换或者CCO回退成功场景才发 */
        NAS_ERABM_SndRabmEmmDrbSetupInd();
    }

    else
    {
        NAS_ERABM_WARN_LOG("NAS_ERABM_RcvRabmRrcStatusMsgProc:WARNING: RRC STATE non-exist!");
    }
}


VOS_VOID  NAS_ERABM_RcvRabmRrcStatusInd(const LRRC_LRABM_STATUS_IND_STRU *pRcvMsg )
{
    VOS_UINT32                          ulRabCnt = 0;
    VOS_UINT32                          ulLoop = 0;
    VOS_UINT32                          aulEpsbId[NAS_ERABM_MAX_EPSB_NUM] = {0};

    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RcvRabmRrcStatusInd is entered.");

    if (NAS_ERABM_L_MODE_STATUS_NORMAL != NAS_ERABM_GetLModeStatus())
    {
        NAS_ERABM_WARN_LOG("NAS_ERABM_RcvRabmRrcStatusInd:Rabm is already suspended!");
        return ;
    }

    ulRabCnt = pRcvMsg->ulRabCnt;

    if (NAS_ERABM_NULL == ulRabCnt)
    {
        for (ulLoop = 0; ulLoop < NAS_ERABM_MAX_EPSB_NUM; ulLoop++)
        {
            aulEpsbId[ulLoop] = ulLoop + NAS_ERABM_MIN_EPSB_ID;
        }

        NAS_ERABM_RcvRabmRrcStatusMsgProc(NAS_ERABM_MAX_EPSB_NUM, pRcvMsg->enStatus, aulEpsbId);
    }
    else
    {
        NAS_ERABM_RcvRabmRrcStatusMsgProc(ulRabCnt, pRcvMsg->enStatus, pRcvMsg->aulRabId);
    }

    /*向RRC回复*/
    NAS_ERABM_SndRabmRrcStatusRsp(LRRC_LNAS_STATUS_SUCC);
}


VOS_VOID NAS_ERABM_SndRabmRrcQosUpdateReq( VOS_VOID )
{
    LRRC_LRABM_QOS_UPDATE_REQ_STRU    *pstQosUpdateReq  = VOS_NULL_PTR;

    /*入口参数检查，每次通知RRC的EPS承载数量为1(目前没有定义任何参数)*/

    /*分配空间和检测是否分配成功*/
    pstQosUpdateReq = (LRRC_LRABM_QOS_UPDATE_REQ_STRU*)NAS_ERABM_ALLOC_MSG(
                                        sizeof(LRRC_LRABM_QOS_UPDATE_REQ_STRU));
    if ( VOS_NULL_PTR == pstQosUpdateReq )
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmRrcQosUpdateReq:ERROR:Alloc msg fail!" );
        return;
    }

    /*清空*/
    NAS_ERABM_MEM_SET(NAS_ERABM_GET_RRC_MSG_ENTITY(pstQosUpdateReq), NAS_ERABM_NULL, NAS_ERABM_GET_MSG_LENGTH(pstQosUpdateReq));

    /*填写消息头*/
    NAS_ERABM_WRITE_RRC_MSG_HEAD(pstQosUpdateReq, ID_LRRC_LRABM_QOS_UPDATE_REQ);

    /* 调用消息发送函数    */
    NAS_ERABM_SND_MSG(pstQosUpdateReq);

}
VOS_VOID NAS_ERABM_SndRabmRrcRabRsp(const VOS_UINT32* pulRabId,
                                                  VOS_UINT32 ulRabCnt)
{
    LRRC_LRABM_RAB_RSP_STRU    *pstRabRsp  = VOS_NULL_PTR;

    /*入口参数检查，每次回复RRC的EPS承载数量最多为NAS_ERABM_MAX_EPSB_NUM*/
    if ((VOS_NULL_PTR == pulRabId)|| (ulRabCnt > NAS_ERABM_MAX_EPSB_NUM))
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmRrcRabRsp:ERROR:Input para error!");
        return;
    }

    /*分配空间和检测是否分配成功*/
    pstRabRsp = (VOS_VOID*)NAS_ERABM_ALLOC_MSG(sizeof(LRRC_LRABM_RAB_RSP_STRU));
    if (VOS_NULL_PTR == pstRabRsp)
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmRrcRabRsp:ERROR:Alloc msg fail!");
        return;
    }

    /*清空*/
    NAS_ERABM_MEM_SET(NAS_ERABM_GET_RRC_MSG_ENTITY(pstRabRsp), NAS_ERABM_NULL, NAS_ERABM_GET_MSG_LENGTH(pstRabRsp));

    /*填写消息头*/
    NAS_ERABM_WRITE_RRC_MSG_HEAD(pstRabRsp,ID_LRRC_LRABM_RAB_RSP);

    /*通知RRC的EPS承载数量和相关信息，填写消息实体*/
    pstRabRsp->ulRabCnt = ulRabCnt;
    NAS_ERABM_MEM_CPY((VOS_UINT8*)pstRabRsp->aulRabId, (VOS_UINT8*)pulRabId,
              (pstRabRsp->ulRabCnt)*(sizeof(VOS_UINT32)/sizeof(VOS_UINT8)));

    /* 调用消息发送函数    */
    NAS_ERABM_SND_MSG(pstRabRsp);

}


VOS_VOID  NAS_ERABM_SndRabmRrcStatusRsp( VOS_UINT32 ulResult )
{
   LRRC_LRABM_STATUS_RSP_STRU    *pstStatusRsp = VOS_NULL_PTR;

   /*入口参数检查*/
    if ((ulResult != LRRC_LNAS_STATUS_SUCC)&& (ulResult != LRRC_LNAS_STATUS_FAIL))
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmRrcStatusRsp:ERROR:Input para error!");
        return;
    }

    /*分配空间和检测是否分配成功*/
    pstStatusRsp = (VOS_VOID*)NAS_ERABM_ALLOC_MSG(sizeof(LRRC_LRABM_STATUS_RSP_STRU));
    if ( VOS_NULL_PTR == pstStatusRsp )
    {
        NAS_ERABM_ERR_LOG("NAS_ERABM_SndRabmRrcRabRsp:ERROR:Alloc msg fail!" );
        return;
    }

    /*清空*/
    NAS_ERABM_MEM_SET(NAS_ERABM_GET_RRC_MSG_ENTITY(pstStatusRsp), 0, NAS_ERABM_GET_MSG_LENGTH(pstStatusRsp));

    /*填写消息头*/
    NAS_ERABM_WRITE_RRC_MSG_HEAD(pstStatusRsp, ID_LRRC_LRABM_STATUS_RSP);

    /*填写消息实体*/
    pstStatusRsp->enResult = ulResult;

    /* 调用消息发送函数    */
    NAS_ERABM_SND_MSG(pstStatusRsp);

}

/*****************************************************************************
 Function Name   : NAS_ERABM_HasDrbWithoutRelatingEpsBear
 Description     : 判断是否存在DRB已建立，但关联的EPS承载尚未激活的情况
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-12-23  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_ERABM_HasDrbWithoutRelatingEpsBear( VOS_VOID )
{
    VOS_UINT32                          ulEpsbId = NAS_ERABM_NULL;

    /* 遍历所有承载，判断是否存在DRB已建立，但EPS承载未激活的情况 */
    for (ulEpsbId = NAS_ERABM_MIN_EPSB_ID; ulEpsbId <= NAS_ERABM_MAX_EPSB_ID; ulEpsbId++)
    {
        if ((NAS_ERABM_ILL_RB_ID != NAS_ERABM_GetEpsbRbIdInfo(ulEpsbId))
            && (NAS_ERABM_EPSB_INACTIVE == NAS_ERABM_GetEpsbStateInfo(ulEpsbId)))
        {
            return NAS_ERABM_SUCCESS;
        }
    }

    return NAS_ERABM_FAILURE;
}

/*****************************************************************************
 Function Name   : NAS_ERABM_IsAllActtiveBearerWithoutDrb
 Description     : 判断是否所有的激活承载关联的DRB都没有建起来，
                   如果没有承载处于激活态，也返回失败。
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-12-23  Draft Enact
    2.sunbing 49683       2010-08-26  Modify
*****************************************************************************/
VOS_UINT32  NAS_ERABM_IsAllActtiveBearerWithoutDrb( VOS_VOID )
{
    VOS_UINT32                          ulEpsbId = NAS_ERABM_NULL;
    VOS_UINT32                          ulActEpsbNum = 0;

    /* 遍历所有承载，判断是否存在承载已激活，但没有对应DRB的情况 */
    for (ulEpsbId = NAS_ERABM_MIN_EPSB_ID; ulEpsbId <= NAS_ERABM_MAX_EPSB_ID; ulEpsbId++)
    {
        /* 承载已激活的场景下 */
        if (NAS_ERABM_EPSB_ACTIVE == NAS_ERABM_GetEpsbStateInfo(ulEpsbId))
        {
            /*EPS承载已激活*/
            ulActEpsbNum ++;

            /*EPS承载有对应的DRB，则返回失败*/
            if(NAS_ERABM_ILL_RB_ID != NAS_ERABM_GetEpsbRbIdInfo(ulEpsbId))
            {
                return NAS_ERABM_FAILURE;
            }
        }
    }

    /*所有的承载都没有对应的DRB*/
    if(ulActEpsbNum > 0)
    {
        return NAS_ERABM_SUCCESS;
    }
    /*不存在激活的承载*/
    else
    {
        return NAS_ERABM_FAILURE;
    }

}

#if 0
/*****************************************************************************
 Function Name   : NAS_ERABM_RrcRabIndResultProc
 Description     : RRC_RABM_RAB_IND消息结果处理
 Input           : enIsRecSetupSucc
                   enIsRecSetupFail
                   enIsRecRelease
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-12-25  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ERABM_RrcRabIndResultProc
(
    PS_BOOL_ENUM_UINT8                  enIsRecSetupSucc,
    PS_BOOL_ENUM_UINT8                  enIsRecSetupFail,
    PS_BOOL_ENUM_UINT8                  enIsRecRelease
)
{
    /*打印进入该函数*/
    NAS_ERABM_INFO_LOG("NAS_ERABM_RrcRabIndResultProc is entered.");

    (VOS_VOID)enIsRecRelease;

    /*判断是否处于SERVICE启动状态，如果是，则发送消息让EMM停止*/
    if (NAS_ERABM_SERVICE_STATE_INIT == NAS_ERABM_GetEmmSrState())
    {
        /*SERVICE流程结束*/
        NAS_ERABM_SetEmmSrState(NAS_ERABM_SERVICE_STATE_TERMIN);

        NAS_ERABM_TimerStop(NAS_ERABM_WAIT_RB_REEST_TIMER);
    }

    /* 通知EMM RB建立，并通知L2发送缓存数据包 */
    if (enIsRecSetupSucc == PS_TRUE)
    {
        /*停止RB建立等待定时器*/
        NAS_ERABM_TimerStop(NAS_ERABM_WAIT_RB_REEST_TIMER);

        NAS_ERABM_SndRabmEmmDrbSetupInd();

        /*APPITF_SendDataofRabmBuf();*/

        /* 通知CDS启动二次过滤 */
        NAS_ERABM_SndErabmCdsSndBuffDataInd(CDS_ERABM_SEND_BUFF_DATA_ALLOWED_TYPE_SERVICE_SUCC);

        /* 清除上行数据阻塞标志*/
        NAS_ERABM_SetUpDataPending(EMM_ERABM_NO_UP_DATA_PENDING);

        /* 环回模式B下，Drb建立成功，停止环回缓存定时器 */
        if (NAS_ERABM_MODE_TYPE_LB == NAS_ERABM_GetMode())
        {
            NAS_ERABM_TimerStop(NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER);
        }

    }

    /* 通知L2释放缓存数据包 */
    if ((enIsRecSetupSucc == PS_FALSE) && (enIsRecSetupFail == PS_TRUE))
    {
        /* 如果在正常模式下 */
        if (NAS_ERABM_MODE_TYPE_NORMAL == NAS_ERABM_GetMode())
        {
            /* 通知L2释放缓存的数据包 */
            /*APPITF_FreeAllofRabmBuf();*/

            /* 通知CDS释放缓存的数据包 */
            NAS_ERABM_SndErabmCdsFreeBuffDataInd();

            /* 清除上行数据阻塞标志*/
            NAS_ERABM_SetUpDataPending(EMM_ERABM_NO_UP_DATA_PENDING);
        }
        /*环回模式下不清理缓存，保持原上行数据Pending状态*/
        else if (NAS_ERABM_TIMER_STATE_STOPED == NAS_ERABM_IsTimerRunning(NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER))
        {
            NAS_ERABM_TimerStart(NAS_ERABM_WAIT_TC_FREE_BUFFER_LENGTH, NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER);
        }
        else
        {
            NAS_ERABM_INFO_LOG("NAS_ERABM_RrcRabIndResultProc:NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER is running");
        }
    }

    /* 判断是否存在DRB已建立，但关联的EPS承载尚未激活的情况*/
    if (NAS_ERABM_SUCCESS == NAS_ERABM_HasDrbWithoutRelatingEpsBear())
    {
        /*停止并重新启动定时器*/
        NAS_ERABM_TimerStop(NAS_ERABM_WAIT_EPSB_ACT_TIMER);

        NAS_ERABM_TimerStart(NAS_ERABM_WAIT_EPSB_ACTT_LENGTH, NAS_ERABM_WAIT_EPSB_ACT_TIMER);

        /*设置等待EPS承载激活标识*/
        NAS_ERABM_SetWaitEpsBActSign(NAS_ERABM_WAIT_EPSB_ACT_MSG);
    }
    /* SERVICE或者Attach过程中，所有DRB没有建立起来，如果给SM发承载状态消息，SM会释放所有承载
        将导致MM本地DETACH之后再ATTACH，因此这种场景下，不能给SM发承载状态信息 */
    else if((enIsRecSetupFail == PS_TRUE)
            && (NAS_ERABM_SUCCESS == NAS_ERABM_IsAllActtiveBearerWithoutDrb()))
    {
        return ;
    }
    else
    {
        /* 向ESM发承载状态消息 */
        NAS_ERABM_InformEsmBearerStatus();
    }
}
#endif

/*****************************************************************************
 Function Name   : NAS_ERABM_RrcRabIndSetupSuccProc
 Description     : RRC_RABM_RAB_IND消息结果处理(setup succ或者setup suc与release组合)
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2012-01-11  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ERABM_RrcRabIndSetupSuccProc( VOS_VOID )
{
    /*SERVICE流程结束*/
    NAS_ERABM_SetEmmSrState(NAS_ERABM_SERVICE_STATE_TERMIN);

    NAS_ERABM_TimerStop(NAS_ERABM_WAIT_RB_REEST_TIMER);

    NAS_ERABM_SndRabmEmmDrbSetupInd();

    /* 通知CDS启动二次过滤 */
    NAS_ERABM_SndErabmCdsSndBuffDataInd(CDS_ERABM_SEND_BUFF_DATA_ALLOWED_TYPE_SERVICE_SUCC);

    /* 清除上行数据阻塞标志*/
    NAS_ERABM_SetUpDataPending(EMM_ERABM_NO_UP_DATA_PENDING);

    /* 环回模式B下，Drb建立成功，停止环回缓存定时器 */
    if (NAS_ERABM_MODE_TYPE_LB == NAS_ERABM_GetMode())
    {
        NAS_ERABM_TimerStop(NAS_ERABM_WAIT_TC_FREE_BUFFER_TIMER);
    }

    /* 如果只是DRB建立成功的场景，判断是否存在DRB已建立，但关联的EPS承载尚未激活
       的情况，如果存在则不发ESM_ERABM_BEARER_STATUS_REQ消息；如果是DRB建立
       和RELEASE组合的场景(service和attach过程的DRB建立成功不可能与release组合，
       因为所有的DRB都会建立成功；只有专有承载激活的DRB建立成功可能与release组合，以及
       切换时DRB建立成功可能与release组合)，也判断是否存在DRB已建立，但关联的EPS承载
       尚未激活的情况，如果存在则不发ESM_ERABM_BEARER_STATUS_REQ消息，等待DRB已建立的
       但EPS承载未建立的承载建立成功后，ERABM收到ESM的ESM_ERABM_ACT_IND消息，再发送
       ESM_ERABM_BEARER_STATUS_REQ消息，通知ESM释放DRB已释放的关联承载 */
    if (NAS_ERABM_SUCCESS == NAS_ERABM_HasDrbWithoutRelatingEpsBear())
    {
        /*停止并重新启动定时器*/
        NAS_ERABM_TimerStop(NAS_ERABM_WAIT_EPSB_ACT_TIMER);
        /* lihong00150010 emergency tau&service begin */
        NAS_ERABM_TimerStart(   NAS_ERABM_WAIT_EPSB_ACTT_LENGTH,
                                NAS_ERABM_WAIT_EPSB_ACT_TIMER,
                                NAS_ERABM_WAIT_EPSB_ACT_TIMER);
        /* lihong00150010 emergency tau&service end */
        /*设置等待EPS承载激活标识*/
        NAS_ERABM_SetWaitEpsBActSign(NAS_ERABM_WAIT_EPSB_ACT_MSG);
    }
    else
    {
        /* 向ESM发承载状态消息,主动发送时，消息的SessionId填为0，不需要匹配  */
        NAS_ERABM_InformEsmBearerStatus(0);
    }
}

/*****************************************************************************
 Function Name   : NAS_ERABM_RrcRabIndSetupFailProc
 Description     : RRC_RABM_RAB_IND消息结果处理(setup fail或者setup fail与release组合)
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2012-01-11  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ERABM_RrcRabIndSetupFailProc
(
    PS_BOOL_ENUM_UINT8                  enIsRecRelease
)
{
    /*判断是否处于SERVICE启动状态，如果是，则发送消息让EMM停止*/
    if (NAS_ERABM_SERVICE_STATE_INIT == NAS_ERABM_GetEmmSrState())
    {
        /*SERVICE流程结束*/
        NAS_ERABM_SetEmmSrState(NAS_ERABM_SERVICE_STATE_TERMIN);

        NAS_ERABM_TimerStop(NAS_ERABM_WAIT_RB_REEST_TIMER);
    }

    /* service和attach过程的DRB建立失败不可能与release组合，因为所有的DRB都会
       建立失败；只有专有承载激活的DRB建立失败可能与release组合，这种场景必须
       发送NAS_ERABM_InformEsmBearerStatus */
    if (PS_TRUE == enIsRecRelease)
    {
        /* 向ESM发承载状态消息,主动发送时，消息的SessionId填为0，不需要匹配  */
        NAS_ERABM_InformEsmBearerStatus(0);
    }

    /* SERVICE和ATTACH过程中的所有DRB建立失败，不发送NAS_ERABM_InformEsmBearerStatus
       消息，只有专有承载激活的DRB建立失败需要发送NAS_ERABM_InformEsmBearerStatus */
    if(NAS_ERABM_SUCCESS != NAS_ERABM_IsAllActtiveBearerWithoutDrb())
    {
        /* 向ESM发承载状态消息,主动发送时，消息的SessionId填为0，不需要匹配  */
        NAS_ERABM_InformEsmBearerStatus(0);
    }
}

/*****************************************************************************
 Function Name   : NAS_ERABM_RrcRabIndResultProc
 Description     : RRC_RABM_RAB_IND消息结果处理
 Input           : enIsRecSetupSucc
                   enIsRecSetupFail
                   enIsRecRelease
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-12-25  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ERABM_RrcRabIndResultProc
(
    PS_BOOL_ENUM_UINT8                  enIsRecSetupSucc,
    PS_BOOL_ENUM_UINT8                  enIsRecSetupFail,
    PS_BOOL_ENUM_UINT8                  enIsRecRelease
)
{
    if (PS_TRUE == enIsRecSetupSucc)
    {
        NAS_ERABM_INFO_LOG("NAS_ERABM_RrcRabIndResultProc:setup succ proc");
        NAS_ERABM_RrcRabIndSetupSuccProc();
        return ;
    }

    if (PS_TRUE == enIsRecSetupFail)
    {
        NAS_ERABM_INFO_LOG("NAS_ERABM_RrcRabIndResultProc:setup fail proc");
        NAS_ERABM_RrcRabIndSetupFailProc(enIsRecRelease);
        return ;
    }

    if (PS_TRUE == enIsRecRelease)
    {
        NAS_ERABM_LOG1("NAS_ERABM_RrcRabIndResultProc:release proc",
                        NAS_ERABM_IsTimerRunning(NAS_ERABM_WAIT_EPSB_ACT_TIMER));

        if (NAS_ERABM_TIMER_STATE_RUNNING != NAS_ERABM_IsTimerRunning(NAS_ERABM_WAIT_EPSB_ACT_TIMER))
        {
            /* 向ESM发承载状态消息,主动发送时，消息的SessionId填为0，不需要匹配  */
            NAS_ERABM_InformEsmBearerStatus(0);
        }

        return ;
    }
}
/* lihong00150010 erabm optimize end */
/*lint +e961*/
/*lint +e960*/
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


