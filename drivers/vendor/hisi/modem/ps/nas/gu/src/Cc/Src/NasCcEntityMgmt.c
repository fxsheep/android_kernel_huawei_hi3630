

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "NasCcInclude.h"
#include "NasCcIe.h"
#include "MnCallApi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#define    THIS_FILE_ID        PS_FILE_ID_NASCC_ENTITYMGMT_C
/*****************************************************************************
  2 常量定义
*****************************************************************************/
/* DTMF字符缓存的长度 */
#define  NAS_CC_MAX_DTMF_BUF_LEN            24

/* 保存的补充业务操作的最大个数 */
#define  NAS_CC_MAX_STORED_SS_OPERATION     32

/* PC回放导出全局变量使用 */
#define EVT_NAS_CC_OUTSIDE_RUNNING_CONTEXT_FOR_PC_REPLAY   0xaaaa          /* 为PC工程回放定义的消息 */

/*****************************************************************************
  3 类型定义
*****************************************************************************/
/* DTMF请求记录结构，用于缓存一个DTMF请求 */
typedef struct
{
    NAS_CC_DTMF_REQ_ENUM_U8             enReq;
    VOS_UINT8                           ucKey;
} NAS_CC_DTMF_REQ_RECORD_STRU;

/* CC实体结构 */
typedef struct
{
    VOS_BOOL                            bUsed;
    VOS_BOOL                            bProgressFlag;
    VOS_UINT8                           ucCallId;
    VOS_UINT8                           ucTi;
    NAS_CC_BC_PARAM_IDX_ENUM_U8         enCurrBcIdx;
    NAS_CC_BC_PARAMS_STRU               stBcParams[NAS_CC_MAX_BC_PARAM_NUM];
    NAS_CC_CALL_STATE_ENUM_U8           enCallState;
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enHoldAuxState;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyAuxState;
    NAS_CC_DTMF_STATE_ENUM              enDtmfState;
    VOS_UINT8                           ucDtmfReqCnt;
    NAS_CC_CALL_TYPE_ENUM_U8            enCallType;
    VOS_UINT8                           aucReserve1[2];
    NAS_CC_DTMF_REQ_RECORD_STRU         astDtmfReqBuf[NAS_CC_MAX_DTMF_BUF_LEN];
} NAS_CC_ENTITY_STRU;

/* 补充业务操作存储项，每个存储项存储一个补充业务操作 */
typedef struct
{
    VOS_BOOL                            bUsed;
    NAS_CC_ENTITY_ID_T                  entityId;
    VOS_UINT8                           ucInvokeId;
    NAS_SS_OPERATION_ENUM_U8            enOperation;
    VOS_UINT8                           aucReserve[2];
} NAS_CC_SS_OPERATION_ITEM_STRU;

/* 任意点回放导出全局变量使用*/
typedef struct
{
    NAS_CC_ENTITY_STRU                  pc_g_f_astCcEntities[NAS_CC_MAX_ENTITY_NUM];
    NAS_CC_DTMF_Q_STATE_ENUM            pc_g_f_astDtmfQState[NAS_CC_MAX_ENTITY_NUM];
    NAS_CC_SS_OPERATION_ITEM_STRU       pc_g_f_astCcSsOperations[NAS_CC_MAX_STORED_SS_OPERATION];
} NAS_CC_OUTSIDE_RUNNING_CONTEXT_ST;

/*****************************************************************************
 Structure      : NAS_CC_SDT_MSG_ST
 Description    : PC回放工程，CC相关的全局变量通过以下消息结构发送
 Message origin :
*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                      MsgHeader;                                          /* 消息头                                   *//*_H2ASN_Skip*/
    NAS_CC_OUTSIDE_RUNNING_CONTEXT_ST    stOutsideCtx;
}NAS_CC_SDT_MSG_ST;

/*****************************************************************************
  4 变量定义
*****************************************************************************/
/* CC实体 */
LOCAL NAS_CC_ENTITY_STRU                    f_astCcEntities[NAS_CC_MAX_ENTITY_NUM];
LOCAL NAS_CC_DTMF_Q_STATE_ENUM          f_astDtmfQState[NAS_CC_MAX_ENTITY_NUM];

/* CC保存的当前正在进行的补充业务操作 */
LOCAL NAS_CC_SS_OPERATION_ITEM_STRU         f_astCcSsOperations[NAS_CC_MAX_STORED_SS_OPERATION];

/* CC模块补充业务状态切换时，状态与处理函数对照表 */
LOCAL NAS_CC_SS_SWITCH_PROC_FUNC_MAP_STRU   f_astCcSsSwitchFuncMap[] =
{
    {NAS_CC_SS_SWITCH_IDLE,     NAS_CC_SS_SWITCH_SUCCESS,   NAS_CC_ProcSsSwitchSingleCallRetrieveSucc},
    {NAS_CC_SS_SWITCH_IDLE,     NAS_CC_SS_SWITCH_FAILED,    NAS_CC_ProcSsSwitchSingleCallRetrieveFail},
    {NAS_CC_SS_SWITCH_IDLE,     NAS_CC_SS_SWITCH_TIME_OUT,  NAS_CC_ProcSsSwitchSingleCallRetrieveFail},

    {NAS_CC_SS_SWITCH_IDLE, NAS_CC_SS_SWITCH_LAST_CALL_RELEASED, NAS_CC_ProcSsSwitchSingleCallRetrieveFail},

    {NAS_CC_SS_SWITCH_SUCCESS,  NAS_CC_SS_SWITCH_IDLE,      NAS_CC_ProcSsSwitchSingleCallHoldSucc},
    {NAS_CC_SS_SWITCH_SUCCESS,  NAS_CC_SS_SWITCH_SUCCESS,   NAS_CC_ProcSsSwitchMultiCallSucc},
    {NAS_CC_SS_SWITCH_SUCCESS,  NAS_CC_SS_SWITCH_FAILED,    NAS_CC_ProcSsSwitchMultiCallFail},
    {NAS_CC_SS_SWITCH_SUCCESS,  NAS_CC_SS_SWITCH_TIME_OUT,  NAS_CC_ProcSsSwitchMultiCallFail},

    {NAS_CC_SS_SWITCH_FAILED,   NAS_CC_SS_SWITCH_IDLE,      NAS_CC_ProcSsSwitchSingleCallHoldFail},
    {NAS_CC_SS_SWITCH_FAILED,   NAS_CC_SS_SWITCH_SUCCESS,   NAS_CC_ProcSsSwitchMultiCallFail},
    {NAS_CC_SS_SWITCH_FAILED,   NAS_CC_SS_SWITCH_FAILED,    NAS_CC_ProcSsSwitchMultiCallFail},
    {NAS_CC_SS_SWITCH_FAILED,   NAS_CC_SS_SWITCH_TIME_OUT,  NAS_CC_ProcSsSwitchMultiCallFail},

    {NAS_CC_SS_SWITCH_TIME_OUT, NAS_CC_SS_SWITCH_IDLE,      NAS_CC_ProcSsSwitchSingleCallHoldFail},

    {NAS_CC_SS_SWITCH_LAST_CALL_RELEASED, NAS_CC_SS_SWITCH_IDLE, NAS_CC_ProcSsSwitchSingleCallHoldFail},

    {NAS_CC_SS_SWITCH_TIME_OUT, NAS_CC_SS_SWITCH_SUCCESS,   NAS_CC_ProcSsSwitchMultiCallFail},
    {NAS_CC_SS_SWITCH_TIME_OUT, NAS_CC_SS_SWITCH_FAILED,    NAS_CC_ProcSsSwitchMultiCallFail},
    {NAS_CC_SS_SWITCH_TIME_OUT, NAS_CC_SS_SWITCH_TIME_OUT,  NAS_CC_ProcSsSwitchMultiCallFail},
};

/*****************************************************************************
  5 函数定义
*****************************************************************************/
/*****************************************************************************
 函 数 名  : NAS_CC_InitEntities
 功能描述  : 初始化所有CC实体，在CC模块初始化时必须调用该函数。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_InitEntities()
{
    VOS_UINT32  i;

    for (i=0; i<NAS_CC_MAX_ENTITY_NUM; i++)
    {
        f_astCcEntities[i].bUsed = VOS_FALSE;
    }
}



VOS_UINT32  NAS_CC_CreateEntity(
    NAS_CC_CALL_TYPE_ENUM_U8            enCallType,
    VOS_UINT8                           ucCallId,
    VOS_UINT8                           ucTi,
    NAS_CC_ENTITY_ID_T                  *pEntityId
)
{
    VOS_UINT32  i;

    /* 寻找未使用的CC实体 */
    for (i=0; i<NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (VOS_FALSE == f_astCcEntities[i].bUsed)
        {
            break;
        }
    }

    if (i < NAS_CC_MAX_ENTITY_NUM)
    {
        /* 找到了未使用的CC实体，初始化该实体 */
        *pEntityId = i;

        if (NAS_CC_INVALID_TI_VALUE == ucTi)
        {
            /* 这是一个本端发起的业务，分配新的Ti(当前未被使用的最小的Ti)*/
            for (ucTi=0; ucTi<NAS_CC_MAX_ENTITY_NUM; ucTi++)
            {
                /* 查看该Ti值是否已被使用 */
                for (i=0; i<NAS_CC_MAX_ENTITY_NUM; i++)
                {
                    if ((VOS_TRUE == f_astCcEntities[i].bUsed) &&
                        (ucTi == (f_astCcEntities[i].ucTi & 0x7)))
                    {
                        break;
                    }
                }

                if (NAS_CC_MAX_ENTITY_NUM == i)
                {
                    /* 该Ti值未被使用 */
                    break;
                }
            }

            f_astCcEntities[*pEntityId].ucTi = ucTi;
        }
        else
        {
            /* 这是一个网络发起的业务，回复的消息中Ti的最高位设为"1" */
            f_astCcEntities[*pEntityId].ucTi = ucTi | 0x8;
        }

        f_astCcEntities[*pEntityId].ucCallId = ucCallId;

        f_astCcEntities[*pEntityId].enCurrBcIdx = NAS_CC_BC_PARAM_1;

        f_astCcEntities[*pEntityId].enCallState = NAS_CC_CALL_STATE_U0;
        f_astCcEntities[*pEntityId].enHoldAuxState = NAS_CC_HOLD_AUX_S_IDLE;
        f_astCcEntities[*pEntityId].enMptyAuxState = NAS_CC_MPTY_AUX_S_IDLE;
        f_astCcEntities[*pEntityId].enDtmfState = NAS_CC_DTMF_S_IDLE;

        f_astCcEntities[*pEntityId].ucDtmfReqCnt = 0;

        f_astCcEntities[*pEntityId].bProgressFlag = VOS_FALSE;
        f_astDtmfQState[*pEntityId] = NAS_CC_DTMF_Q_NULL;

        f_astCcEntities[*pEntityId].bUsed = VOS_TRUE;

        f_astCcEntities[*pEntityId].enCallType = enCallType;

        return  VOS_OK;
    }
    else
    {
        NAS_CC_ERR_LOG("NAS_CC_CreateEntity: Can not find free CC entity.");
        return VOS_ERR;
    }
}


VOS_VOID  NAS_CC_DeleteAllEntities(VOS_VOID)
{
    VOS_UINT8                           i;

    for (i = 0; i < NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (VOS_TRUE == f_astCcEntities[i].bUsed)
        {
            NAS_CC_DeleteEntity(i);
        }
    }

    return;
}


VOS_VOID  NAS_CC_DeleteEntity(
    NAS_CC_ENTITY_ID_T                  entityId
)
{

    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_DeleteEntity: entityId wrong.");
        return;
    }

    NAS_CC_StopAllTimer(entityId);
    NAS_CC_DetachUserConn(entityId);
    NAS_CC_LocalAbortDtmf(entityId);
    NAS_CC_FlushDTMFBuff(entityId);

    f_astCcEntities[entityId].enCallState = NAS_CC_CALL_STATE_U0;
    f_astCcEntities[entityId].enHoldAuxState = NAS_CC_HOLD_AUX_S_IDLE;
    f_astCcEntities[entityId].enMptyAuxState = NAS_CC_MPTY_AUX_S_IDLE;
    f_astCcEntities[entityId].enDtmfState = NAS_CC_DTMF_S_IDLE;
    f_astDtmfQState[entityId] = NAS_CC_DTMF_Q_NULL;
    f_astCcEntities[entityId].ucDtmfReqCnt = 0;

    f_astCcEntities[entityId].bProgressFlag = VOS_FALSE;

    f_astCcEntities[entityId].bUsed = VOS_FALSE;
    f_astCcEntities[entityId].enCallType = NAS_CC_CALL_TYPE_BUTT;
}


/*****************************************************************************
 函 数 名  : NAS_CC_GetEntityByTi
 功能描述  : 取得与指定Ti值对应的CC实体的ID
 输入参数  : ucTi - Transcation ID
 输出参数  : 无
 返 回 值  : 取得的CC实体ID. 如果没有找到对应实体，则返回CC_INVALID_ENTITY_ID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月22日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
NAS_CC_ENTITY_ID_T  NAS_CC_GetEntityByTi(
    VOS_UINT8                           ucTi
)
{
    VOS_UINT32 i;

    for (i=0; i<NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if ((VOS_TRUE == f_astCcEntities[i].bUsed)
         && (ucTi == f_astCcEntities[i].ucTi))
        {
            /* 找到 */
            return i;
        }
    }

    return NAS_CC_INVALID_ENTITY_ID;
}
VOS_UINT8  NAS_CC_GetEntityTi(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetEntityTi: entityId wrong.");
        return 0;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    return f_astCcEntities[entityId].ucTi;
}



VOS_UINT8  NAS_CC_GetCallId(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetCallId: entityId wrong.");
        return 0;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    return f_astCcEntities[entityId].ucCallId;
}


VOS_VOID  NAS_CC_UpdateCallId(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT8                           ucCallId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_UpdateCallId: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    NAS_CC_NORM_LOG1("CC change state to :", ucCallId);

    f_astCcEntities[entityId].ucCallId= ucCallId;
}



VOS_VOID  NAS_CC_UpdateBcParams(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_BC_PARAM_IDX_ENUM_U8         enIdx,
    const NAS_CC_BC_PARAMS_STRU         *pstParams
)

{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_UpdateBcParams: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    if (enIdx <= NAS_CC_BC_PARAM_2)
    {
        f_astCcEntities[entityId].stBcParams[enIdx].enItc = pstParams->enItc;
    }
    else
    {
        NAS_CC_ERR_LOG("NAS_CC_UpdateBcParams: enIdx wrong.");
    }
}



VOS_VOID  NAS_CC_GetBcParams(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_BC_PARAM_IDX_ENUM_U8         enIdx,
    NAS_CC_BC_PARAMS_STRU               *pstParams
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetBcParams: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    if (enIdx <= NAS_CC_BC_PARAM_2)
    {
        pstParams->enItc = f_astCcEntities[entityId].stBcParams[enIdx].enItc;
    }
    else
    {
        /* Get current BC params */
        pstParams->enItc =
        f_astCcEntities[entityId].stBcParams[f_astCcEntities[entityId].enCurrBcIdx].enItc;
    }
}
VOS_VOID  NAS_CC_BcAlertnate(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_BcAlertnate: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    if (NAS_CC_BC_PARAM_1 == f_astCcEntities[entityId].enCurrBcIdx)
    {
        f_astCcEntities[entityId].enCurrBcIdx = NAS_CC_BC_PARAM_2;
    }
    else
    {
        f_astCcEntities[entityId].enCurrBcIdx = NAS_CC_BC_PARAM_1;
    }
}



NAS_CC_CALL_STATE_ENUM_U8  NAS_CC_GetCallState(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetCallState: entityId wrong.");
        return NAS_CC_CALL_STATE_BUTT;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    return f_astCcEntities[entityId].enCallState;
}

/* Added by n00355355 for 呼叫重建, 2015-9-18, begin */
/*****************************************************************************
 函 数 名  : NAS_CC_GetCallType
 功能描述  : 取得指定CC实体的呼叫类型
 输入参数  : entityId - CC实体的ID
 输出参数  : 无
 返 回 值  : 该实体的呼叫状态.
 调用函数  :
 被调函数  :

 修改历史      :

  1.日    期   : 2015年9月17日
    作    者   : n00355355
    修改内容   : 新生成函数
*****************************************************************************/
NAS_CC_CALL_TYPE_ENUM_U8  NAS_CC_GetCallType(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetCallType: entityId wrong.");
        return NAS_CC_CALL_STATE_BUTT;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    return f_astCcEntities[entityId].enCallType;
}
/* Added by n00355355 for 呼叫重建, 2015-9-18, end */

VOS_VOID  NAS_CC_ChangeCallState(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_CALL_STATE_ENUM_U8           enState
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_ChangeCallState: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    NAS_CC_NORM_LOG1("CC change state to :", enState);

    f_astCcEntities[entityId].enCallState = enState;

    switch(enState)
    {
    case NAS_CC_CALL_STATE_U0:
        if (VOS_TRUE == NAS_CC_IsLastCallEntityID(entityId))
        {
            NAS_CC_ClearAllCacheMsg();
        }

        NAS_CC_DeleteEntity(entityId);
        break;

    default:
        break;
    }
}



NAS_CC_HOLD_AUX_STATE_ENUM_U8 NAS_CC_GetHoldAuxState(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetHoldAuxState: entityId wrong.");
        return NAS_CC_HOLD_AUX_S_BUTT;
    }

    if (VOS_TRUE != f_astCcEntities[entityId].bUsed)
    {
        return NAS_CC_HOLD_AUX_S_BUTT;
    }

    return f_astCcEntities[entityId].enHoldAuxState;
}



VOS_VOID  NAS_CC_ChangeHoldAuxState(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       state
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_ChangeHoldAuxState: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    f_astCcEntities[entityId].enHoldAuxState = state;

    NAS_CC_NORM_LOG1("CC change hold state to :", state);
}



NAS_CC_MPTY_AUX_STATE_ENUM_U8 NAS_CC_GetMptyAuxState(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetMptyAuxState: entityId wrong.");
        return NAS_CC_MPTY_AUX_S_BUTT;
    }

    if (VOS_TRUE != f_astCcEntities[entityId].bUsed)
    {
        return NAS_CC_MPTY_AUX_S_BUTT;
    }

    return f_astCcEntities[entityId].enMptyAuxState;
}



VOS_VOID  NAS_CC_ChangeMptyAuxState(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       state
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_ChangeMptyAuxState: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    f_astCcEntities[entityId].enMptyAuxState = state;

    NAS_CC_NORM_LOG1("CC change MPTY state to :", state);
}
LOCAL VOS_UINT32 NAS_CC_CheckBuildMptyWithCallInMpty(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState
)
{
    VOS_UINT32                          i;

    for (i = 0; i < NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (i == entityId)
        {
            continue;
        }

        /*判断CC实体状态是否允许建立mpty操作*/
        if (((NAS_CC_CALL_STATE_U10 == f_astCcEntities[i].enCallState)
         || (NAS_CC_CALL_STATE_U26 == f_astCcEntities[i].enCallState))
         && (NAS_CC_MPTY_AUX_S_IDLE == f_astCcEntities[i].enMptyAuxState))
        {
            break;
        }
    }

    if (i < NAS_CC_MAX_ENTITY_NUM)
    {
        return VOS_OK;
    }

    NAS_CC_WARN_LOG1("NAS_CC_CheckBuildMptyWithCallInMpty error In Mpty enMptyState :", enMptyState);
    return VOS_ERR;

}
LOCAL VOS_UINT32 NAS_CC_CheckBuildMptyWithNoCallInMpty(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enHoldState,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState
)
{
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enExpHoldState;
    VOS_UINT32                          i;

    if (NAS_CC_HOLD_AUX_S_IDLE == enHoldState)
    {
        enExpHoldState = NAS_CC_HOLD_AUX_S_CALL_HELD;
    }
    else
    {
        enExpHoldState = NAS_CC_HOLD_AUX_S_IDLE;
    }

    for (i = 0; i < NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (i == entityId)
        {
            continue;
        }

        /*判断CC实体状态是否允许建立mpty操作*/
        if (((NAS_CC_CALL_STATE_U10 == f_astCcEntities[i].enCallState)
         || (NAS_CC_CALL_STATE_U26 == f_astCcEntities[i].enCallState))
         && (enExpHoldState == f_astCcEntities[i].enHoldAuxState))
        {
            break;
        }
    }

    if (i < NAS_CC_MAX_ENTITY_NUM)
    {
        return VOS_OK;
    }

    NAS_CC_WARN_LOG1("NAS_CC_CheckBuildMptyWithNoCallInMpty error Idle enMptyState :", enMptyState);
    return VOS_ERR;

}
LOCAL VOS_UINT32 NAS_CC_CheckBuildMptyOperation(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_CALL_STATE_ENUM_U8           enCallState,
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enHoldState,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState
)
{
    VOS_UINT32                          ulResult;

    /* 条件:
       1. 被检查的呼叫在MPTY中: 有另一个不在MPTY中的active呼叫
       2. 被检查的呼叫不在MPTY中: 有另一个active(held)呼叫
       3. 如果被检查的呼叫在进行其他hold或mpty操作，则检查失败
    */
    if ((enCallState != NAS_CC_CALL_STATE_U10)
     && (enCallState != NAS_CC_CALL_STATE_U26))
    {
        NAS_CC_WARN_LOG1("NAS_CC_CheckBuildMptyOperation error callState :", enCallState);
        return VOS_ERR;
    }

    if ((enHoldState != NAS_CC_HOLD_AUX_S_IDLE)
     && (enHoldState != NAS_CC_HOLD_AUX_S_CALL_HELD))
    {
        NAS_CC_WARN_LOG1("NAS_CC_CheckBuildMptyOperation error enHoldState :", enHoldState);
        return VOS_ERR;
    }

    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY == enMptyState)
    {
       ulResult = NAS_CC_CheckBuildMptyWithCallInMpty(entityId, enMptyState);
       return ulResult;
    }
    else if (NAS_CC_MPTY_AUX_S_IDLE == enMptyState)
    {
        ulResult = NAS_CC_CheckBuildMptyWithNoCallInMpty(entityId, enHoldState, enMptyState);
        return ulResult;
    }
    else
    {
        /*当前MPTY状态不正确，返回检查失败*/
        return VOS_ERR;
    }
}
VOS_UINT32 NAS_CC_CheckSsSwitchHoldAllowed(VOS_VOID)
{
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enHoldState;
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enRetrieveState;

    enHoldState     = NAS_CC_GetSsSwitchHoldState();
    enRetrieveState = NAS_CC_GetSsSwitchRetrieveState();

    if (NAS_CC_SS_SWITCH_IDLE == enHoldState)
    {
        if ( (NAS_CC_SS_SWITCH_IDLE == enRetrieveState)
          || (NAS_CC_SS_SWITCH_WAIT_CNF == enRetrieveState) )
        {
            return VOS_OK;
        }
    }

    NAS_CC_WARN_LOG2("NAS_CC_CheckSsSwitchHoldAllowed: Error Switch State :", enHoldState, enRetrieveState);
    return VOS_ERR;
}


VOS_UINT32 NAS_CC_CheckSsSwitchRetrieveAllowed(VOS_VOID)
{
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enHoldState;
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enRetrieveState;

    enHoldState     = NAS_CC_GetSsSwitchHoldState();
    enRetrieveState = NAS_CC_GetSsSwitchRetrieveState();

    if (NAS_CC_SS_SWITCH_IDLE == enRetrieveState)
    {
        if ( (NAS_CC_SS_SWITCH_IDLE == enHoldState)
          || (NAS_CC_SS_SWITCH_WAIT_CNF == enHoldState) )
        {
            return VOS_OK;
        }
    }

    NAS_CC_WARN_LOG2("NAS_CC_CheckSsSwitchRetrieveAllowed: Error Switch State :", enHoldState, enRetrieveState);
    return VOS_ERR;
}



LOCAL VOS_UINT32 NAS_CC_CheckHoldMptyOperation(
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enHoldState,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState
)
{
    /* 条件: 呼叫在MPTY中，且未被保持 */
    if ((NAS_CC_MPTY_AUX_S_CALL_IN_MPTY == enMptyState)
     && (NAS_CC_HOLD_AUX_S_IDLE == enHoldState))
    {
        return NAS_CC_CheckSsSwitchHoldAllowed();
    }

    NAS_CC_WARN_LOG1("NAS_CC_CheckHoldMptyOperation hold mpty error enMptyState :", enMptyState);
    return VOS_ERR;

}
LOCAL VOS_UINT32 NAS_CC_CheckRetrieveMptyOperation(
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enHoldState,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState
)
{
    /* 条件: 呼叫在MPTY中，且被保持 */
    if ((NAS_CC_MPTY_AUX_S_CALL_IN_MPTY == enMptyState)
     && (NAS_CC_HOLD_AUX_S_CALL_HELD == enHoldState))
    {
        return NAS_CC_CheckSsSwitchRetrieveAllowed();
    }

    NAS_CC_WARN_LOG1("NAS_CC_CheckRetrieveMptyOperation retrieve mpty error enMptyState :", enMptyState);
    return VOS_ERR;
}

/*****************************************************************************
 函 数 名  : NAS_CC_CheckSplitMptyOperation
 功能描述  : 检查SPLIT MPTY操作是否可以在当前状态下进行
 输入参数  : enHoldState   - 保持状态
              enMptyState   - MPTY状态
 输出参数  : 无
 返 回 值  : VOS_OK - 检查通过； VOS_ERR - 检查失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
LOCAL VOS_UINT32 NAS_CC_CheckSplitMptyOperation(
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enHoldState,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState
)
{
    /* 条件: 呼叫在MPTY中，且未被保持 */
    if ((NAS_CC_MPTY_AUX_S_CALL_IN_MPTY == enMptyState)
     && (NAS_CC_HOLD_AUX_S_IDLE == enHoldState))
    {
        return VOS_OK;
    }
    else
    {
        NAS_CC_WARN_LOG1("NAS_CC_CheckSplitMptyOperation split mpty error enMptyState :", enMptyState);
        return VOS_ERR;
    }
}

/*****************************************************************************
 函 数 名  : NAS_CC_CheckMptyOperation
 功能描述  : 检查MPTY操作是否可以在当前状态下进行
 输入参数  : entityId      - CC实体的ID
              enSsOperation - 操作的补充业务码
 输出参数  : 无
 返 回 值  : VOS_OK - 检查通过； VOS_ERR - 检查失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 NAS_CC_CheckMptyOperation(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_SS_OPERATION_ENUM_U8            enSsOperation
)
{

    NAS_CC_CALL_STATE_ENUM_U8     enCallState = NAS_CC_GetCallState(entityId);
    NAS_CC_HOLD_AUX_STATE_ENUM_U8 enHoldState = NAS_CC_GetHoldAuxState(entityId);
    NAS_CC_MPTY_AUX_STATE_ENUM_U8 enMptyState = NAS_CC_GetMptyAuxState(entityId);

    switch (enSsOperation)
    {
    case NAS_SS_OPERATION_BUILD_MPTY:
        return NAS_CC_CheckBuildMptyOperation(entityId,
                                          enCallState,
                                          enHoldState,
                                          enMptyState);

    case NAS_SS_OPERATION_HOLD_MPTY:
        return NAS_CC_CheckHoldMptyOperation(enHoldState, enMptyState);

    case NAS_SS_OPERATION_RETRIEVE_MPTY:
        return NAS_CC_CheckRetrieveMptyOperation(enHoldState, enMptyState);

    case NAS_SS_OPERATION_SPLIT_MPTY:
        return NAS_CC_CheckSplitMptyOperation(enHoldState, enMptyState);

    default:
        NAS_CC_NORM_LOG("NAS_CC_CheckMptyOperation: No check for this operation.");
        return VOS_OK;
    }
}



/*****************************************************************************
 函 数 名  : NAS_CC_ConditionalChangeAuxState
 功能描述  : 有条件的改变呼叫的从状态。该函数将改变所有符合条件的CC实体的从状态。
 输入参数  : bCheckHoldState   - 是否检查当前的呼叫保持状态
              bCheckMptyState   - 是否检查当前的MPTY状态
              bChangeHoldState  - 是否改变呼叫保持状态
              bChangeMptyState  - 是否改变MPTY状态
              enExpectHoldState - 期望的呼叫保持状态(如果bCheckHoldState为FALSE, 该参数无意义)
              enExpectMptyState - 期望的MPTY状态(如果bCheckMptyState为FALSE, 该参数无意义)
              enNewHoldState    - 新的呼叫保持状态(如果bChangeHoldState为FALSE, 该参数无意义)
              enNewMptyState    - 新的MPTY状态(如果bChangeMptyState为FALSE, 该参数无意义)
 输出参数  : 无
 返 回 值  : 发生了呼叫从状态改变的CC实体个数
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月22日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
LOCAL VOS_UINT32  NAS_CC_ConditionalChangeAuxState(
    VOS_BOOL                            bCheckHoldState,
    VOS_BOOL                            bCheckMptyState,
    VOS_BOOL                            bChangeHoldState,
    VOS_BOOL                            bChangeMptyState,
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enExpectHoldState,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enExpectMptyState,
    NAS_CC_HOLD_AUX_STATE_ENUM_U8       enNewHoldState,
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enNewMptyState
)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulChanged = 0;
    VOS_BOOL                            bCheckOk;

    for (i=0; i<NAS_CC_MAX_ENTITY_NUM; i++)
    {
        /* 只有正在使用的实体才能改变状态 */
        bCheckOk = f_astCcEntities[i].bUsed;

        /* 只有呼叫状态为Active或MO Modify的实体才能改变从状态 */
        bCheckOk &= ((NAS_CC_CALL_STATE_U10 == f_astCcEntities[i].enCallState) ||
                     (NAS_CC_CALL_STATE_U26 == f_astCcEntities[i].enCallState)) ?
                     VOS_TRUE : VOS_FALSE;

        /* 检查当前的呼叫保持状态是否满足条件 */
        if (bCheckHoldState)
        {
            bCheckOk &= (f_astCcEntities[i].enHoldAuxState == enExpectHoldState) ?
                     VOS_TRUE : VOS_FALSE;
        }

        /* 检查当前的MPTY状态是否满足条件 */
        if (bCheckMptyState)
        {
            bCheckOk &= (f_astCcEntities[i].enMptyAuxState == enExpectMptyState) ?
                     VOS_TRUE : VOS_FALSE;
        }

        /* 如果检查不通过则不修改此实体的状态，接着检查下一个 */
        if (!bCheckOk)
        {
            continue;
        }

        /* 改变呼叫保持状态 */
        if (bChangeHoldState)
        {
            NAS_CC_ChangeHoldAuxState(i, enNewHoldState);
        }

        /* 改变MPTY状态 */
        if (bChangeMptyState)
        {
            if((NAS_CC_MPTY_AUX_S_MPTY_REQ == enNewMptyState)
                && (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY == f_astCcEntities[i].enMptyAuxState))
            {
                continue;
            }
            NAS_CC_ChangeMptyAuxState(i, enNewMptyState);
        }

        ulChanged++;
    }

    return ulChanged;
}
LOCAL VOS_VOID  NAS_CC_HandleMptyEventInvoke(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_SS_OPERATION_ENUM_U8            enSsOperation
)
{
    if (NAS_SS_OPERATION_BUILD_MPTY == enSsOperation)
    {
        NAS_CC_ConditionalChangeAuxState(
                                         VOS_FALSE,
                                         VOS_FALSE,
                                         VOS_FALSE,
                                         VOS_TRUE,
                                         0,
                                         0,
                                         0,
                                         NAS_CC_MPTY_AUX_S_MPTY_REQ
                                         );
    }
    else if (NAS_SS_OPERATION_HOLD_MPTY == enSsOperation)
    {
        NAS_CC_ConditionalChangeAuxState(
                                         VOS_FALSE,
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         0,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                         NAS_CC_HOLD_AUX_S_HOLD_REQ,
                                         0
                                         );

        NAS_CC_SetSsSwitchHoldInfo(entityId, NAS_CC_SS_SWITCH_WAIT_CNF, NAS_CC_CAUSE_NULL);
    }
    else if (NAS_SS_OPERATION_RETRIEVE_MPTY == enSsOperation)
    {
        NAS_CC_ConditionalChangeAuxState(
                                         VOS_FALSE,
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         0,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                         NAS_CC_HOLD_AUX_S_RETRIEVE_REQ,
                                         0
                                         );

        NAS_CC_SetSsSwitchRetrieveInfo(entityId, NAS_CC_SS_SWITCH_WAIT_CNF, NAS_CC_CAUSE_NULL);
    }
    else if (NAS_SS_OPERATION_SPLIT_MPTY == enSsOperation)
    {
        if ((VOS_TRUE == f_astCcEntities[entityId].bUsed) &&
            (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY == f_astCcEntities[entityId].enMptyAuxState))
        {
            f_astCcEntities[entityId].enMptyAuxState = NAS_CC_MPTY_AUX_S_SPLIT_REQ;
        }
        else
        {
            NAS_CC_ERR_LOG("NAS_CC_HandleMptyEventInvoke: Can not split.");
        }
    }
    else
    {
        NAS_CC_WARN_LOG("NAS_CC_HandleMptyEventInvoke: Event ignored.");
    }
}


LOCAL VOS_VOID  NAS_CC_HandleMptyEventSuccess(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_SS_OPERATION_ENUM_U8            enSsOperation
)
{
    if (NAS_SS_OPERATION_BUILD_MPTY == enSsOperation)
    {
        NAS_CC_ConditionalChangeAuxState(
                                         VOS_FALSE,
                                         VOS_FALSE,
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         0,
                                         0,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY
                                         );
    }
    else if (NAS_SS_OPERATION_HOLD_MPTY == enSsOperation)
    {
        /* 记录补充业务切换状态 */
        NAS_CC_SetSsSwitchHoldInfo(entityId, NAS_CC_SS_SWITCH_SUCCESS, NAS_CC_CAUSE_NULL);
    }
    else if (NAS_SS_OPERATION_RETRIEVE_MPTY == enSsOperation)
    {
        /* 记录补充业务切换状态 */
        NAS_CC_SetSsSwitchRetrieveInfo(entityId, NAS_CC_SS_SWITCH_SUCCESS, NAS_CC_CAUSE_NULL);
    }
    else if (NAS_SS_OPERATION_SPLIT_MPTY == enSsOperation)
    {
        if ((VOS_TRUE == f_astCcEntities[entityId].bUsed) &&
            (NAS_CC_MPTY_AUX_S_SPLIT_REQ == f_astCcEntities[entityId].enMptyAuxState))
        {
            f_astCcEntities[entityId].enMptyAuxState = NAS_CC_MPTY_AUX_S_IDLE;
            f_astCcEntities[entityId].enHoldAuxState = NAS_CC_HOLD_AUX_S_IDLE;

            NAS_CC_ConditionalChangeAuxState(
                                             VOS_FALSE,
                                             VOS_TRUE,
                                             VOS_TRUE,
                                             VOS_FALSE,
                                             0,
                                             NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                             NAS_CC_HOLD_AUX_S_CALL_HELD,
                                             0
                                             );
        }
        else
        {
            NAS_CC_ERR_LOG("NAS_CC_HandleMptyEventSuccess: Not spliting MPTY.");
        }
    }
    else
    {
        NAS_CC_WARN_LOG("NAS_CC_HandleMptyEventSuccess: Event ignored.");
    }

}


LOCAL VOS_VOID  NAS_CC_HandleMptyEventFail(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_SS_OPERATION_ENUM_U8            enSsOperation
)
{
    VOS_UINT32                          ulHeldCnt;
    VOS_UINT32                          ulIdleCnt;

    if (NAS_SS_OPERATION_BUILD_MPTY == enSsOperation)
    {
        /* 需要将已在MPTY中的呼叫的MPTY状态改变为in MPTY，将不在MPTY中的呼叫
           的MPTY状态改为IDLE。为满足这个要求，先要区分在MPTY和不在MPTY中的呼叫。
           我们通过查看HELD和IDLE的呼叫各有多少个来实现这个区分。*/
        ulHeldCnt = NAS_CC_ConditionalChangeAuxState(
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         VOS_TRUE,
                                         NAS_CC_HOLD_AUX_S_CALL_HELD,
                                         NAS_CC_MPTY_AUX_S_MPTY_REQ,
                                         0,
                                         NAS_CC_MPTY_AUX_S_IDLE );

        ulIdleCnt = NAS_CC_ConditionalChangeAuxState(
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         VOS_TRUE,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_MPTY_REQ,
                                         0,
                                         NAS_CC_MPTY_AUX_S_IDLE );

        if (ulHeldCnt > 1)
        {
            /* HELD的呼叫数大于1，表明HELD的呼叫在MPTY中 */
            NAS_CC_ConditionalChangeAuxState(
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         VOS_TRUE,
                                         NAS_CC_HOLD_AUX_S_CALL_HELD,
                                         NAS_CC_MPTY_AUX_S_MPTY_REQ,
                                         0,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY);
        }

        if (ulIdleCnt > 1)
        {
            /* IDLE的呼叫数大于1，表明IDLE的呼叫在MPTY中 */
            NAS_CC_ConditionalChangeAuxState(
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         VOS_TRUE,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_MPTY_REQ,
                                         0,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY);
        }
    }
    else if (NAS_SS_OPERATION_HOLD_MPTY == enSsOperation)
    {
        /* 记录补充业务切换状态 */
        NAS_CC_SetSsSwitchHoldInfo(entityId, NAS_CC_SS_SWITCH_FAILED, NAS_CC_CAUSE_NULL);
    }
    else if (NAS_SS_OPERATION_RETRIEVE_MPTY == enSsOperation)
    {
        /* 记录补充业务切换状态 */
        NAS_CC_SetSsSwitchRetrieveInfo(entityId, NAS_CC_SS_SWITCH_FAILED, NAS_CC_CAUSE_NULL);
    }
    else if (NAS_SS_OPERATION_SPLIT_MPTY == enSsOperation)
    {
        if ((VOS_TRUE == f_astCcEntities[entityId].bUsed) &&
            (NAS_CC_MPTY_AUX_S_SPLIT_REQ == f_astCcEntities[entityId].enMptyAuxState))
        {
            f_astCcEntities[entityId].enMptyAuxState = NAS_CC_MPTY_AUX_S_CALL_IN_MPTY;
        }
        else
        {
            NAS_CC_ERR_LOG("NAS_CC_HandleMptyEventFail: Not spliting MPTY.");
        }
    }
    else
    {
        NAS_CC_WARN_LOG("NAS_CC_HandleMptyEventFail: Event ignored.");
    }

}


VOS_VOID  NAS_CC_HandleMptyEvent(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_MPTY_EVT_ENUM                enMptyEvent,
    NAS_SS_OPERATION_ENUM_U8            enSsOperation
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_HandleMptyEvent: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    switch (enMptyEvent)
    {
    case NAS_CC_MPTY_EVT_INVOKE:
        NAS_CC_HandleMptyEventInvoke(entityId, enSsOperation);
        break;

    case NAS_CC_MPTY_EVT_SUCCESS:
        NAS_CC_HandleMptyEventSuccess(entityId, enSsOperation);
        break;

    case NAS_CC_MPTY_EVT_FAIL:
        NAS_CC_HandleMptyEventFail(entityId, enSsOperation);
        break;

    default:
        break;
    }
}



NAS_CC_DTMF_STATE_ENUM  NAS_CC_GetDtmfState(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetDtmfState: entityId wrong.");
        return NAS_CC_DTMF_S_UNKNOWN;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    return f_astCcEntities[entityId].enDtmfState;
}



VOS_VOID  NAS_CC_ChangeDtmfState(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_DTMF_STATE_ENUM              enState
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_ChangeDtmfState: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    f_astCcEntities[entityId].enDtmfState = enState;

    NAS_CC_NORM_LOG1("CC change DTMF state to :", enState);
}

NAS_CC_DTMF_Q_STATE_ENUM  NAS_CC_GetDtmfQState(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetDtmfQState: entityId wrong.");
        return NAS_CC_DTMF_Q_UNKNOWN;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);
    return f_astDtmfQState[entityId];
}



VOS_VOID  NAS_CC_ChangeDtmfQState(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_DTMF_Q_STATE_ENUM            enState
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_ChangeDtmfQState: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);
    f_astDtmfQState[entityId] = enState;
    NAS_CC_INFO_LOG1("CC change DTMF state to :", enState);
}



VOS_VOID  NAS_CC_QueueDtmfReq(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_DTMF_REQ_ENUM_U8             enDtmfReq,
    VOS_UINT8                           ucKey
)
{
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCause;
    NAS_CC_DTMF_Q_STATE_ENUM            enstate;
    NAS_CC_DTMF_REQ_RECORD_STRU        *pstCurrRecord;

    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_QueueDtmfReq: entityId wrong.");
        return;
    }

    enstate = NAS_CC_GetDtmfQState(entityId);
    if (f_astCcEntities[entityId].ucDtmfReqCnt < NAS_CC_MAX_DTMF_BUF_LEN)
    {
        if(enDtmfReq == NAS_CC_DTMF_START_REQ)
        {
            if(enstate != NAS_CC_DTMF_Q_START_REQ)
            {
                pstCurrRecord = &f_astCcEntities[entityId].astDtmfReqBuf[f_astCcEntities[entityId].ucDtmfReqCnt];
                pstCurrRecord->enReq = enDtmfReq;
                pstCurrRecord->ucKey = ucKey;
                f_astCcEntities[entityId].ucDtmfReqCnt++;
                NAS_CC_ChangeDtmfQState(entityId,NAS_CC_DTMF_Q_START_REQ);

            }
            else
            {
                enCause = NAS_CC_CAUSE_128;
                NAS_CC_SendMnccMsg(entityId, MNCC_START_DTMF_REJ, &enCause, sizeof(enCause));
                NAS_CC_ERR_LOG("NAS_CC_QueueDtmfReq: discard DTMFStarREQ.");
            }

        }
        else
        {
            if(enstate != NAS_CC_DTMF_Q_STOP_REQ)
            {
                pstCurrRecord = &f_astCcEntities[entityId].astDtmfReqBuf[f_astCcEntities[entityId].ucDtmfReqCnt];
                pstCurrRecord->enReq = enDtmfReq;
                pstCurrRecord->ucKey = ucKey;
                f_astCcEntities[entityId].ucDtmfReqCnt++;
                NAS_CC_ChangeDtmfQState(entityId,NAS_CC_DTMF_Q_STOP_REQ);

            }
            else
            {
                NAS_CC_SendMnccMsg(entityId, MNCC_STOP_DTMF_CNF, VOS_NULL_PTR, 0);
                NAS_CC_ERR_LOG("NAS_CC_QueueDtmfReq: discard DTMFStopREQ.");

            }

        }
    }
    else
    {
        if(enDtmfReq == NAS_CC_DTMF_START_REQ)
        {
            enCause = NAS_CC_CAUSE_128;
            NAS_CC_SendMnccMsg(entityId, MNCC_START_DTMF_REJ, &enCause, sizeof(enCause));
            NAS_CC_WARN_LOG("NAS_CC_QueueDtmfReq: DTMF buffer is full.");
        }
        else
        {
            NAS_CC_SendMnccMsg(entityId, MNCC_STOP_DTMF_CNF, VOS_NULL_PTR, 0);
            NAS_CC_WARN_LOG("NAS_CC_QueueDtmfReq: DTMF buffer is full.");
        }
    }
}



LOCAL VOS_UINT32 NAS_CC_DequeueDtmfReq(
    NAS_CC_ENTITY_ID_T                  entityId,
    NAS_CC_DTMF_REQ_RECORD_STRU        *pstRecord
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_DequeueDtmfReq: entityId wrong.");
        return VOS_ERR;
    }

    if (f_astCcEntities[entityId].ucDtmfReqCnt > 0)
    {
        PS_MEM_CPY(pstRecord,
                   &f_astCcEntities[entityId].astDtmfReqBuf[0],
                   sizeof(NAS_CC_DTMF_REQ_RECORD_STRU));

        f_astCcEntities[entityId].ucDtmfReqCnt--;

        /* 将缓存中剩余的所有记录前移 */
        if (f_astCcEntities[entityId].ucDtmfReqCnt > 0)
        {
            PS_MEM_CPY(&f_astCcEntities[entityId].astDtmfReqBuf[0],
                       &f_astCcEntities[entityId].astDtmfReqBuf[1],
                       f_astCcEntities[entityId].ucDtmfReqCnt * sizeof(NAS_CC_DTMF_REQ_RECORD_STRU));
        }
        else
        {
            NAS_CC_ChangeDtmfQState(entityId, NAS_CC_DTMF_Q_NULL);
        }

        return VOS_OK;
    }
    else
    {
        NAS_CC_WARN_LOG1("NAS_CC_DequeueDtmfReq ucDtmfReqCnt", f_astCcEntities[entityId].ucDtmfReqCnt);
        return VOS_ERR;
    }

}
VOS_VOID  NAS_CC_SendBufferedDtmfReq(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    NAS_CC_DTMF_REQ_RECORD_STRU             stDtmfReq;


    stDtmfReq.enReq = NAS_CC_DTMF_REQ_BUTT;
    stDtmfReq.ucKey = 0;

    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_SendBufferedDtmfReq: entityId wrong.");
        return;
    }

    if (NAS_CC_DequeueDtmfReq(entityId, &stDtmfReq) == VOS_OK)
    {
        VOS_UINT8  ucTi = NAS_CC_GetEntityTi(entityId);

        if (NAS_CC_DTMF_START_REQ == stDtmfReq.enReq)
        {
            NAS_CC_SendStartDtmf(ucTi, stDtmfReq.ucKey);
            NAS_CC_StartTimer(entityId, TI_NAS_CC_T336, 0);
            NAS_CC_INFO_LOG("NAS_CC_SendBufferedDtmfReq ChangeDtmfState to START_REQ");
            NAS_CC_ChangeDtmfState(entityId, NAS_CC_DTMF_S_START_REQ);
        }
        else
        {
            NAS_CC_SendStopDtmf(ucTi);
            NAS_CC_StartTimer(entityId, TI_NAS_CC_T337, 0);
            NAS_CC_INFO_LOG("NAS_CC_SendBufferedDtmfReq ChangeDtmfState to STOP_REQ");
            NAS_CC_ChangeDtmfState(entityId, NAS_CC_DTMF_S_STOP_REQ);
        }
    }
}


/*****************************************************************************
 函 数 名  : NAS_CC_StoreSsOperation
 功能描述  : 通过invoke类型的facility消息发起补充业务操作时，调用该函数存储该
             补充业务操作，以便收到网络的facility回复时能由invoke ID得到相应的
             补充业务操作码
 输入参数  : entityId      - CC实体的ID
              ucInvokeId    - 补充业务操作的invoke ID
              enSsOperation - 补充业务操作码
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_CC_StoreSsOperation(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT8                           ucInvokeId,
    NAS_SS_OPERATION_ENUM_U8            enSsOperation
)
{
    VOS_UINT32 i;
    VOS_UINT32 ulStorePos = NAS_CC_MAX_STORED_SS_OPERATION;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    for (i=0; i<NAS_CC_MAX_STORED_SS_OPERATION; i++)
    {
        /*
        由于要检查是否有相同的invoke ID被保存了，所以在找到空的存储位置后只是记录
        下来，而不是立刻跳出循环
        */
        if (!f_astCcSsOperations[i].bUsed
         && (NAS_CC_MAX_STORED_SS_OPERATION == ulStorePos))
        {
            ulStorePos = i;
        }

        if (f_astCcSsOperations[i].bUsed
         && (f_astCcSsOperations[i].entityId == entityId)
         && (f_astCcSsOperations[i].ucInvokeId == ucInvokeId))
        {
            /* 对一个invoke ID存储两次是不允许的，在程序正确的情况下是不应出现的 */
            NAS_CC_ERR_LOG("NAS_CC_StoreSsOperation: This invoke ID is in use.");
            return;
        }
    }

    if (ulStorePos != NAS_CC_MAX_STORED_SS_OPERATION)
    {
        f_astCcSsOperations[ulStorePos].entityId = entityId;
        f_astCcSsOperations[ulStorePos].ucInvokeId = ucInvokeId;
        f_astCcSsOperations[ulStorePos].enOperation = enSsOperation;
        f_astCcSsOperations[ulStorePos].bUsed = VOS_TRUE;
    }
    else
    {
        /* 存储空间不够的情况应该是在调试阶段解决的，该函数不应返回失败 */
        NAS_CC_ERR_LOG("NAS_CC_StoreSsOperation: The storage is full.");
    }
}


/*****************************************************************************
 函 数 名  : NAS_CC_RestoreSsOperation
 功能描述  : 取得存储的补充业务操作，并清除该存储项
 输入参数  : entityId      - CC实体的ID
              ucInvokeId    - 补充业务操作的invoke ID
 输出参数  : penSsOperation - 取得的补充业务操作码
 返 回 值  : VOS_OK - 操作成功；VOS_ERR - 没有找到符合条件的存储项，操作失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 NAS_CC_RestoreSsOperation(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT8                           ucInvokeId,
    NAS_SS_OPERATION_ENUM_U8           *penSsOperation
)
{
    VOS_UINT32 i;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    for (i=0; i<NAS_CC_MAX_STORED_SS_OPERATION; i++)
    {
        if (f_astCcSsOperations[i].bUsed
         && (f_astCcSsOperations[i].entityId == entityId)
         && (f_astCcSsOperations[i].ucInvokeId == ucInvokeId))
        {
            *penSsOperation = f_astCcSsOperations[i].enOperation;
            f_astCcSsOperations[i].bUsed = VOS_FALSE;
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


/*****************************************************************************
 函 数 名  : NAS_CC_GetStoredSsOperation
 功能描述  : 取得存储的补充业务操作
 输入参数  : entityId      - CC实体的ID
              ucInvokeId    - 补充业务操作的invoke ID
 输出参数  : penSsOperation - 取得的补充业务操作码
 返 回 值  : VOS_OK - 操作成功；VOS_ERR - 没有找到符合条件的存储项，操作失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 NAS_CC_GetStoredSsOperation(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_UINT8                           ucInvokeId,
    NAS_SS_OPERATION_ENUM_U8            *penSsOperation
)
{
    VOS_UINT32 i;

    NAS_CC_ASSERT(entityId < NAS_CC_MAX_ENTITY_NUM);

    for (i=0; i<NAS_CC_MAX_STORED_SS_OPERATION; i++)
    {
        if (f_astCcSsOperations[i].bUsed
         && (f_astCcSsOperations[i].entityId == entityId)
         && (f_astCcSsOperations[i].ucInvokeId == ucInvokeId))
        {
            *penSsOperation = f_astCcSsOperations[i].enOperation;
            return VOS_OK;
        }
    }

    return VOS_ERR;
}



VOS_VOID  NAS_CC_SetProgressFlag(
    NAS_CC_ENTITY_ID_T                  entityId,
    VOS_BOOL                            bSet
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_SetProgressFlag: entityId wrong.");
        return;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    f_astCcEntities[entityId].bProgressFlag = bSet;
}



VOS_BOOL  NAS_CC_GetProgressFlag(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    if (entityId >= NAS_CC_MAX_ENTITY_NUM)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_GetProgressFlag: entityId wrong.");
        return VOS_FALSE;
    }

    NAS_CC_ASSERT(VOS_TRUE == f_astCcEntities[entityId].bUsed);

    return f_astCcEntities[entityId].bProgressFlag;
}


/*****************************************************************************
 函 数 名  : NAS_CC_IsAllowNewMtCall
 功能描述  : 判定是否能接收新的MT呼叫
 输入参数  : entityId - CC实体的ID
 输出参数  : 无
 返 回 值  : progress标志的状态
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_BOOL  NAS_CC_IsAllowNewMtCall(VOS_VOID)
{
    VOS_UINT8                           i;

    for (i=0; i<NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (VOS_TRUE == f_astCcEntities[i].bUsed)
        {
            if ((NAS_CC_CALL_STATE_U0 != f_astCcEntities[i].enCallState)
              &&(NAS_CC_CALL_STATE_U10 != f_astCcEntities[i].enCallState)
              &&(NAS_CC_CALL_STATE_U26 != f_astCcEntities[i].enCallState)
              &&(NAS_CC_CALL_STATE_U27 != f_astCcEntities[i].enCallState))
            {
                return VOS_FALSE;
            }
        }
    }

    return VOS_TRUE;
}

VOS_VOID NAS_CC_FlushDTMFBuff(
    NAS_CC_ENTITY_ID_T                  entityId
)
{
    NAS_CC_DTMF_REQ_RECORD_STRU         stDtmfReq;
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCause;
    VOS_UINT8                           i;

    /* 初始化 */
    PS_MEM_SET(&stDtmfReq, 0, sizeof(stDtmfReq));

    for (i = 0; i < NAS_CC_MAX_DTMF_BUF_LEN; i++)
    {
        if (NAS_CC_DequeueDtmfReq(entityId, &stDtmfReq) == VOS_OK)
        {
            if (NAS_CC_DTMF_START_REQ == stDtmfReq.enReq)
            {
                enCause = NAS_CC_CAUSE_111;
                NAS_CC_SendMnccMsg(entityId, MNCC_START_DTMF_REJ, &enCause, sizeof(enCause));
            }
            else
            {
                NAS_CC_SendMnccMsg(entityId, MNCC_STOP_DTMF_CNF, VOS_NULL_PTR, 0);
            }
        }
        else
        {
            break;
        }
    }
    NAS_CC_ChangeDtmfQState(entityId, NAS_CC_DTMF_Q_NULL);
}
VOS_UINT32 NAS_CC_AtTest(
    VOS_UINT8                           ucCallId,
    NAS_CC_STATE_INFO_STRU              *pstStateInfo
)
{
    /* 输入参数非法,直接返回出错 */
    if ( (ucCallId > NAS_CC_MAX_ENTITY_NUM)
      || (ucCallId < 1) )
    {
        return VOS_ERR;
    }

    if ( VOS_FALSE == f_astCcEntities[ucCallId - 1].bUsed )
    {
        return VOS_ERR;
    }

    pstStateInfo->ucCallId = f_astCcEntities[ucCallId - 1].ucCallId;
    pstStateInfo->ucTi = f_astCcEntities[ucCallId - 1].ucTi;
    pstStateInfo->enCallState = f_astCcEntities[ucCallId - 1].enCallState;
    pstStateInfo->enHoldAuxState = f_astCcEntities[ucCallId - 1].enHoldAuxState;
    pstStateInfo->enMptyAuxState = f_astCcEntities[ucCallId - 1].enMptyAuxState;

    return VOS_OK;
}
VOS_UINT32 NAS_CC_SndOutsideContextData()
{
    NAS_CC_SDT_MSG_ST                      *pSndMsgCB     = VOS_NULL_PTR;
    NAS_CC_OUTSIDE_RUNNING_CONTEXT_ST      *pstOutsideCtx;

    pSndMsgCB = (NAS_CC_SDT_MSG_ST *)PS_ALLOC_MSG(WUEPS_PID_CC, sizeof(NAS_CC_SDT_MSG_ST));

    if ( VOS_NULL_PTR == pSndMsgCB )
    {
        NAS_CC_ERR_LOG("NAS_CC_SndOutsideContextData:ERROR: Alloc Memory Fail.");
        return VOS_FALSE;
    }

    pstOutsideCtx = &pSndMsgCB->stOutsideCtx;

    PS_MEM_CPY(pstOutsideCtx->pc_g_f_astCcEntities, f_astCcEntities,NAS_CC_MAX_ENTITY_NUM * sizeof(NAS_CC_ENTITY_STRU));
    PS_MEM_CPY(pstOutsideCtx->pc_g_f_astDtmfQState, f_astDtmfQState,NAS_CC_MAX_ENTITY_NUM * sizeof(NAS_CC_DTMF_Q_STATE_ENUM));
    PS_MEM_CPY(pstOutsideCtx->pc_g_f_astCcSsOperations, f_astCcSsOperations,NAS_CC_MAX_STORED_SS_OPERATION * sizeof(NAS_CC_SS_OPERATION_ITEM_STRU));

    pSndMsgCB->MsgHeader.ulReceiverPid = WUEPS_PID_CC;
    pSndMsgCB->MsgHeader.ulSenderPid   = WUEPS_PID_CC;
    pSndMsgCB->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pSndMsgCB->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;

    pSndMsgCB->MsgHeader.ulLength  = sizeof(NAS_CC_OUTSIDE_RUNNING_CONTEXT_ST) + 4;
    pSndMsgCB->MsgHeader.ulMsgName = EVT_NAS_CC_OUTSIDE_RUNNING_CONTEXT_FOR_PC_REPLAY;

    OM_TraceMsgHook(pSndMsgCB);
    PS_FREE_MSG(WUEPS_PID_CC, pSndMsgCB);

    return VOS_TRUE;
}

#ifdef __PS_WIN32_RECUR__

VOS_UINT32 NAS_CC_RestoreContextData(struct MsgCB * pMsg)
{
    NAS_CC_SDT_MSG_ST                      *pRcvMsgCB;
    NAS_CC_OUTSIDE_RUNNING_CONTEXT_ST      *pstOutsideCtx;

    pRcvMsgCB     = (NAS_CC_SDT_MSG_ST *)pMsg;
    pstOutsideCtx = &pRcvMsgCB->stOutsideCtx;

    if (EVT_NAS_CC_OUTSIDE_RUNNING_CONTEXT_FOR_PC_REPLAY == pRcvMsgCB->MsgHeader.ulMsgName)
    {
        PS_MEM_CPY(f_astCcEntities,pstOutsideCtx->pc_g_f_astCcEntities, NAS_CC_MAX_ENTITY_NUM * sizeof(NAS_CC_ENTITY_STRU));
        PS_MEM_CPY(f_astDtmfQState,pstOutsideCtx->pc_g_f_astDtmfQState,NAS_CC_MAX_ENTITY_NUM * sizeof(NAS_CC_DTMF_Q_STATE_ENUM));
        PS_MEM_CPY(f_astCcSsOperations,pstOutsideCtx->pc_g_f_astCcSsOperations,NAS_CC_MAX_STORED_SS_OPERATION * sizeof(NAS_CC_SS_OPERATION_ITEM_STRU));

        PS_LOG(WUEPS_PID_CC, VOS_NULL, PS_PRINT_INFO,"CC: NAS_CC_RestoreContextData - data is restored.");
        return VOS_TRUE;
    }
    return VOS_FALSE;
}
#endif



VOS_VOID  NAS_CC_ProcAtPrimitive(
    const VOS_VOID                      *pMsg
)
{
    MN_APP_REQ_MSG_STRU                *pMnAppReqMsg;

    pMnAppReqMsg = (MN_APP_REQ_MSG_STRU *)pMsg;

    switch (pMnAppReqMsg->usMsgName)
    {
        case AT_CC_MSG_STATE_QRY_REQ:
            NAS_CC_ProcAtCcStateQry(pMsg);
            break;

        default:
            break;
    }

    return;
}


VOS_VOID  NAS_CC_ProcAtCcStateQry(
    const VOS_VOID                      *pMsg
)
{
    MN_APP_REQ_MSG_STRU                *pMnAppReqMsg;
    VOS_UINT8                           ucCallId;
    AT_CC_STATE_QRY_CNF_MSG_STRU       *pstAtCcStateQryCnfMsg;
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucCallNum;

    /* 申请消息内存 */
    pstAtCcStateQryCnfMsg = (AT_CC_STATE_QRY_CNF_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_CC,
                                             sizeof(AT_CC_STATE_QRY_CNF_MSG_STRU));

    if( VOS_NULL_PTR == pstAtCcStateQryCnfMsg )
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_ProcAtCcStateQry:ERROR: Memory Alloc Error for pMsg");
        return;
    }

    /* 初始化 */
    pMnAppReqMsg = (MN_APP_REQ_MSG_STRU *)pMsg;
    ucCallId     = pMnAppReqMsg->aucContent[0];
    ucCallNum    = 0;

    if (0 == ucCallId)
    {
        /* 查询所有呼叫 */
        for (ucCallId = 1 ; ucCallId <= MN_CALL_MAX_NUM ; ucCallId++ )
        {
            ulRet = NAS_CC_AtTest(ucCallId,&pstAtCcStateQryCnfMsg->astCcStateInfoList[ucCallNum]);
            if ( VOS_OK ==  ulRet)
            {
                pstAtCcStateQryCnfMsg->aucCallExist[ucCallNum] = 1;
                ucCallNum++;
            }
        }
    }
    else
    {
        /* 查询特定call id 信息 */
        ulRet = NAS_CC_AtTest(ucCallId,&pstAtCcStateQryCnfMsg->astCcStateInfoList[0]);
        if ( VOS_OK ==  ulRet)
        {
            pstAtCcStateQryCnfMsg->aucCallExist[0] = 1;
            ucCallNum = 1;
        }
        else
        {
            pstAtCcStateQryCnfMsg->aucCallExist[0] = 0;
        }
    }

    /* 构造发送消息 */
    pstAtCcStateQryCnfMsg->ulReceiverPid          = WUEPS_PID_AT;
    pstAtCcStateQryCnfMsg->ulMsgName              = AT_CC_MSG_STATE_QRY_CNF;
    pstAtCcStateQryCnfMsg->stAtAppCtrl.usClientId = pMnAppReqMsg->clientId;
    pstAtCcStateQryCnfMsg->stAtAppCtrl.ucOpId     = pMnAppReqMsg->opId;
    pstAtCcStateQryCnfMsg->ucCallNum              = ucCallNum;

    /* 发送消息 */
    ulRet = PS_SEND_MSG(WUEPS_PID_CC, pstAtCcStateQryCnfMsg);
    if( VOS_OK != ulRet)
    {
        NAS_ERROR_LOG(WUEPS_PID_CC, "NAS_CC_ProcAtCcStateQry:ERROR: Send Msg Error!");
    }

    return;
}




NAS_CC_ENTITY_ID_T  NAS_CC_GetEntityByCallId(
    VOS_UINT8                           ucCallId
)
{
    NAS_CC_ENTITY_ID_T entityId;

    for (entityId = 0; entityId < NAS_CC_MAX_ENTITY_NUM; entityId++)
    {
        if ((VOS_TRUE == f_astCcEntities[entityId].bUsed)
         && (ucCallId == f_astCcEntities[entityId].ucCallId))
        {
            /* 找到 */
            return entityId;
        }
    }

    return NAS_CC_INVALID_ENTITY_ID;
}
VOS_VOID NAS_CC_ProcSsSwitchSingleCallRetrieveSucc(VOS_VOID)
{
    NAS_CC_ENTITY_ID_T                  ulEntityID;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState;
    NAS_CC_MSG_FACILITY_MT_STRU        *pstMsg          = VOS_NULL_PTR;

    ulEntityID  = NAS_CC_GetSsSwitchRetrieveEntityID();
    enMptyState = NAS_CC_GetMptyAuxState(ulEntityID);

    /* 检查呼叫是否为多方通话，非多方通话直接将回复CALL模块RETRIEVE_CNF并更新状态 */
    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY != enMptyState)
    {
        NAS_CC_SendMnccMsg(ulEntityID, MNCC_RETRIEVE_CNF, VOS_NULL_PTR, VOS_NULL);

        NAS_CC_INFO_LOG("NAS_CC_ProcSsSwitchSingleCallRetrieveSucc: ChangeHoldAuxState to IDLE");
        NAS_CC_ChangeHoldAuxState(ulEntityID, NAS_CC_HOLD_AUX_S_IDLE);

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    /* 呼叫为多方通话，则更新状态，并将缓存的Facility消息发送给CALL模块 */
    if (VOS_TRUE == NAS_CC_GetSsSwitchOpFacility())
    {
        pstMsg = NAS_CC_GetSsSwitchFacility();

        NAS_CC_ConditionalChangeAuxState(VOS_FALSE,
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_IDLE);

        NAS_CC_SendMnccMsg(ulEntityID,
                           MNCC_FACILITY_IND,
                           pstMsg,
                           sizeof(NAS_CC_MSG_FACILITY_MT_STRU));

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    NAS_CC_ERR_LOG("NAS_CC_ProcSsSwitchSingleCallRetrieveSucc: Error AuxState or Switch State!");

    /* 补充业务切换状态处理异常，重置信息 */
    NAS_CC_InitSsSwitchInfo();

    return;
}
VOS_VOID NAS_CC_ProcSsSwitchSingleCallRetrieveFail(VOS_VOID)
{
    NAS_CC_ENTITY_ID_T                  ulEntityID;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState;
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCause;
    NAS_CC_MSG_FACILITY_MT_STRU        *pstMsg          = VOS_NULL_PTR;
    VOS_UINT8                           ucInvokeId;

    ulEntityID  = NAS_CC_GetSsSwitchRetrieveEntityID();
    enCause     = NAS_CC_GetSsSwitchRetrieveCause();
    enMptyState = NAS_CC_GetMptyAuxState(ulEntityID);

    /* 检查呼叫是否为多方通话，非多方通话直接将回复CALL模块RETRIEVE_REJ并更新状态 */
    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY != enMptyState)
    {
        NAS_CC_SendMnccMsg(ulEntityID, MNCC_RETRIEVE_REJ, &enCause, sizeof(enCause));

        NAS_CC_INFO_LOG("NAS_CC_ProcSsSwitchSingleCallRetrieveFail: ChangeHoldAuxState to CALL_HELD");
        NAS_CC_ChangeHoldAuxState(ulEntityID, NAS_CC_HOLD_AUX_S_CALL_HELD);

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    /* 将呼叫实体状态恢复为HELD */
    NAS_CC_ConditionalChangeAuxState(VOS_FALSE,
                                     VOS_TRUE,
                                     VOS_TRUE,
                                     VOS_FALSE,
                                     NAS_CC_HOLD_AUX_S_IDLE,
                                     NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                     NAS_CC_HOLD_AUX_S_CALL_HELD,
                                     NAS_CC_MPTY_AUX_S_IDLE);

    /* 呼叫为多方通话，则更新状态，并将缓存的Facility消息发送给CALL模块 */
    if (VOS_TRUE == NAS_CC_GetSsSwitchOpFacility())
    {
        pstMsg = NAS_CC_GetSsSwitchFacility();

        NAS_CC_SendMnccMsg(ulEntityID,
                           MNCC_FACILITY_IND,
                           pstMsg,
                           sizeof(NAS_CC_MSG_FACILITY_MT_STRU));

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    /* 存在Facility操作超时，此时回复FACILITY_LOCAL_REJ */
    if (VOS_TRUE == NAS_CC_GetSsSwitchOpInvokeId())
    {
        ucInvokeId = NAS_CC_GetSsSwitchInvokeId();

        NAS_CC_SendMnccMsg(ulEntityID,
                           MNCC_FACILITY_LOCAL_REJ,
                           &ucInvokeId,
                           sizeof(ucInvokeId));

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    NAS_CC_ERR_LOG("NAS_CC_ProcSsSwitchSingleCallRetrieveFail: Error AuxState or Switch State!");

    /* 补充业务切换状态处理异常，重置信息 */
    NAS_CC_InitSsSwitchInfo();

    return;
}


VOS_VOID NAS_CC_ProcSsSwitchSingleCallHoldSucc(VOS_VOID)
{
    NAS_CC_ENTITY_ID_T                  ulEntityID;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState;
    NAS_CC_MSG_FACILITY_MT_STRU        *pstMsg          = VOS_NULL_PTR;

    ulEntityID  = NAS_CC_GetSsSwitchHoldEntityID();
    enMptyState = NAS_CC_GetMptyAuxState(ulEntityID);

    /* 检查呼叫是否为多方通话，非多方通话直接将回复CALL模块HOLD_CNF并更新状态 */
    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY != enMptyState)
    {
        NAS_CC_SendMnccMsg(ulEntityID, MNCC_HOLD_CNF, VOS_NULL_PTR, VOS_NULL);

        NAS_CC_INFO_LOG("NAS_CC_ProcSsSwitchSingleCallHoldSucc: ChangeHoldAuxState to CALL_HELD");
        NAS_CC_ChangeHoldAuxState(ulEntityID, NAS_CC_HOLD_AUX_S_CALL_HELD);

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    NAS_CC_ConditionalChangeAuxState(VOS_FALSE,
                                     VOS_TRUE,
                                     VOS_TRUE,
                                     VOS_FALSE,
                                     NAS_CC_HOLD_AUX_S_IDLE,
                                     NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                     NAS_CC_HOLD_AUX_S_CALL_HELD,
                                     NAS_CC_MPTY_AUX_S_IDLE);

    /* 呼叫为多方通话，则更新状态，并将缓存的Facility消息发送给CALL模块 */
    if (VOS_TRUE == NAS_CC_GetSsSwitchOpFacility())
    {
        pstMsg = NAS_CC_GetSsSwitchFacility();

        NAS_CC_SendMnccMsg(ulEntityID,
                           MNCC_FACILITY_IND,
                           pstMsg,
                           sizeof(NAS_CC_MSG_FACILITY_MT_STRU));

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    NAS_CC_ERR_LOG("NAS_CC_ProcSsSwitchSingleCallRetrieveSucc: Error AuxState or Switch State!");

    /* 补充业务切换状态处理异常，重置信息 */
    NAS_CC_InitSsSwitchInfo();
    return;
}
VOS_VOID NAS_CC_ProcSsSwitchSingleCallHoldFail(VOS_VOID)
{
    NAS_CC_ENTITY_ID_T                  ulEntityID;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyState;
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCause;
    NAS_CC_MSG_FACILITY_MT_STRU        *pstMsg          = VOS_NULL_PTR;
    VOS_UINT8                           ucInvokeId;

    ulEntityID  = NAS_CC_GetSsSwitchHoldEntityID();
    enCause     = NAS_CC_GetSsSwitchHoldCause();
    enMptyState = NAS_CC_GetMptyAuxState(ulEntityID);

    /* 检查呼叫是否为多方通话，非多方通话直接将回复CALL模块HOLD_REJ并更新状态 */
    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY != enMptyState)
    {
        NAS_CC_SendMnccMsg(ulEntityID, MNCC_HOLD_REJ, &enCause, sizeof(enCause));

        NAS_CC_INFO_LOG("NAS_CC_ProcSsSwitchSingleCallHoldFail: ChangeHoldAuxState to IDLE");
        NAS_CC_ChangeHoldAuxState(ulEntityID, NAS_CC_HOLD_AUX_S_IDLE);

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    NAS_CC_ConditionalChangeAuxState(VOS_FALSE,
                                     VOS_TRUE,
                                     VOS_TRUE,
                                     VOS_FALSE,
                                     NAS_CC_HOLD_AUX_S_IDLE,
                                     NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                     NAS_CC_HOLD_AUX_S_IDLE,
                                     NAS_CC_MPTY_AUX_S_IDLE);

    /* 呼叫为多方通话，则更新状态，并将缓存的Facility消息发送给CALL模块 */
    if (VOS_TRUE == NAS_CC_GetSsSwitchOpFacility())
    {
        pstMsg = NAS_CC_GetSsSwitchFacility();

        NAS_CC_SendMnccMsg(ulEntityID,
                           MNCC_FACILITY_IND,
                           pstMsg,
                           sizeof(NAS_CC_MSG_FACILITY_MT_STRU));

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    /* 存在Facility操作超时，此时回复FACILITY_LOCAL_REJ */
    if (VOS_TRUE == NAS_CC_GetSsSwitchOpInvokeId())
    {
        ucInvokeId = NAS_CC_GetSsSwitchInvokeId();

        NAS_CC_SendMnccMsg(ulEntityID,
                           MNCC_FACILITY_LOCAL_REJ,
                           &ucInvokeId,
                           sizeof(ucInvokeId));

        /* 补充业务切换状态处理完毕，重置信息 */
        NAS_CC_InitSsSwitchInfo();
        return;
    }

    NAS_CC_ERR_LOG("NAS_CC_ProcSsSwitchSingleCallHoldFail: Error AuxState or Switch State!");

    /* 补充业务切换状态处理异常，重置信息 */
    NAS_CC_InitSsSwitchInfo();

    return;
}
VOS_VOID NAS_CC_ProcSsSwitchMultiCallSucc(VOS_VOID)
{
    NAS_CC_ENTITY_ID_T                  ulHoldEntityID;
    NAS_CC_ENTITY_ID_T                  ulRetrieveEntityID;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enHoldMptyState;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enRetrieveMptyState;
    NAS_CC_MSG_FACILITY_MT_STRU        *pstMsg          = VOS_NULL_PTR;

    ulHoldEntityID      = NAS_CC_GetSsSwitchHoldEntityID();
    enHoldMptyState     = NAS_CC_GetMptyAuxState(ulHoldEntityID);
    ulRetrieveEntityID  = NAS_CC_GetSsSwitchRetrieveEntityID();
    enRetrieveMptyState = NAS_CC_GetMptyAuxState(ulRetrieveEntityID);

    /* 先处理呼叫保持通话 */
    /* 检查呼叫是否为多方通话，非多方通话直接将回复CALL模块HOLD_CNF并更新状态 */
    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY != enHoldMptyState)
    {
        NAS_CC_SendMnccMsg(ulHoldEntityID, MNCC_HOLD_CNF, VOS_NULL_PTR, VOS_NULL);

        NAS_CC_INFO_LOG("NAS_CC_ProcSsSwitchMultiCallSucc: ChangeHoldAuxState to HELD");
        NAS_CC_ChangeHoldAuxState(ulHoldEntityID, NAS_CC_HOLD_AUX_S_CALL_HELD);
    }
    else
    {
        /* 呼叫为多方通话，则更新状态 */
        NAS_CC_ConditionalChangeAuxState(VOS_FALSE,
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                         NAS_CC_HOLD_AUX_S_CALL_HELD,
                                         NAS_CC_MPTY_AUX_S_IDLE);

        /*将缓存的Facility消息发送给CALL模块 */
        if (VOS_TRUE == NAS_CC_GetSsSwitchOpFacility())
        {
            pstMsg = NAS_CC_GetSsSwitchFacility();

            NAS_CC_SendMnccMsg(ulHoldEntityID,
                               MNCC_FACILITY_IND,
                               pstMsg,
                               sizeof(NAS_CC_MSG_FACILITY_MT_STRU));

            /* 清除缓存的Facility消息 */
            NAS_CC_SetSsSwitchOpFacility(VOS_FALSE, VOS_NULL_PTR);

        }
        else
        {
            NAS_CC_ERR_LOG("NAS_CC_ProcSsSwitchMultiCallSucc: Error MPTY State First!");
        }
    }

    /* 再处理呼叫恢复通话 */
    /* 检查呼叫是否为多方通话，非多方通话直接将回复CALL模块RETRIEVE_CNF并更新状态 */
    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY != enRetrieveMptyState)
    {
        NAS_CC_SendMnccMsg(ulRetrieveEntityID, MNCC_RETRIEVE_CNF, VOS_NULL_PTR, VOS_NULL);

        NAS_CC_INFO_LOG("NAS_CC_ProcSsSwitchMultiCallSucc: ChangeHoldAuxState to IDLE");
        NAS_CC_ChangeHoldAuxState(ulRetrieveEntityID, NAS_CC_HOLD_AUX_S_IDLE);
    }
    else
    {
        NAS_CC_ConditionalChangeAuxState(VOS_FALSE,
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_IDLE);

        /* 呼叫为多方通话，则更新状态，并将缓存的Facility消息发送给CALL模块 */
        if (VOS_TRUE == NAS_CC_GetSsSwitchOpFacility())
        {
            pstMsg = NAS_CC_GetSsSwitchFacility();

            NAS_CC_SendMnccMsg(ulRetrieveEntityID,
                               MNCC_FACILITY_IND,
                               pstMsg,
                               sizeof(NAS_CC_MSG_FACILITY_MT_STRU));

            /* 清除缓存的Facility消息 */
            NAS_CC_SetSsSwitchOpFacility(VOS_FALSE, VOS_NULL_PTR);

        }
        else
        {
            NAS_CC_ERR_LOG("NAS_CC_ProcSsSwitchMultiCallSucc: Error MPTY State Second!");
        }
    }

    /* 补充业务多路切换状态处理成功，重置信息 */
    NAS_CC_InitSsSwitchInfo();

    return;
}


VOS_VOID NAS_CC_ProcSsSwitchMultiCallFail(VOS_VOID)
{
    NAS_CC_ENTITY_ID_T                  ulHoldEntityID;
    NAS_CC_ENTITY_ID_T                  ulRetrieveEntityID;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enHoldMptyState;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enRetrieveMptyState;
    NAS_CC_CAUSE_VALUE_ENUM_U32         enHoldCause;
    NAS_CC_CAUSE_VALUE_ENUM_U32         enRetrieveCause;
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enHoldState;
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enRetrieveState;
    NAS_CC_MSG_FACILITY_MT_STRU        *pstMsg          = VOS_NULL_PTR;
    VOS_UINT8                           ucInvokeId;

    ulHoldEntityID      = NAS_CC_GetSsSwitchHoldEntityID();
    enHoldCause         = NAS_CC_GetSsSwitchHoldCause();
    enHoldState         = NAS_CC_GetSsSwitchHoldState();
    enHoldMptyState     = NAS_CC_GetMptyAuxState(ulHoldEntityID);
    ulRetrieveEntityID  = NAS_CC_GetSsSwitchRetrieveEntityID();
    enRetrieveCause     = NAS_CC_GetSsSwitchRetrieveCause();
    enRetrieveState     = NAS_CC_GetSsSwitchRetrieveState();
    enRetrieveMptyState = NAS_CC_GetMptyAuxState(ulRetrieveEntityID);

    /* 先处理呼叫保持通话 */
    /* 检查呼叫是否为多方通话，非多方通话直接将回复CALL模块HOLD_REJ并更新状态 */
    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY != enHoldMptyState)
    {
        /* 如果HOLD操作失败，则回复HOLD_REJ */
        if (NAS_CC_SS_SWITCH_SUCCESS != enHoldState)
        {
            NAS_CC_SendMnccMsg(ulHoldEntityID,
                               MNCC_HOLD_REJ,
                               &enHoldCause,
                               sizeof(enHoldCause));
        }

        NAS_CC_INFO_LOG("NAS_CC_ProcSsSwitchMultiCallFail: ChangeHoldAuxState to IDLE");
        NAS_CC_ChangeHoldAuxState(ulHoldEntityID, NAS_CC_HOLD_AUX_S_IDLE);
    }
    else
    {
        /* 呼叫为多方通话，则更新状态，并将缓存的Facility消息发送给CALL模块 */
        NAS_CC_ConditionalChangeAuxState(VOS_FALSE,
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_IDLE);

        /* 如果Facility操作失败，则发送回复 */
        if (NAS_CC_SS_SWITCH_SUCCESS != enHoldState)
        {
            if (VOS_TRUE == NAS_CC_GetSsSwitchOpFacility())
            {
                pstMsg = NAS_CC_GetSsSwitchFacility();

                NAS_CC_SendMnccMsg(ulHoldEntityID,
                                   MNCC_FACILITY_IND,
                                   pstMsg,
                                   sizeof(NAS_CC_MSG_FACILITY_MT_STRU));
            }

            if (VOS_TRUE == NAS_CC_GetSsSwitchOpInvokeId())
            {
                ucInvokeId = NAS_CC_GetSsSwitchInvokeId();

                NAS_CC_SendMnccMsg(ulHoldEntityID,
                                   MNCC_FACILITY_LOCAL_REJ,
                                   &ucInvokeId,
                                   sizeof(ucInvokeId));

            }
        }

        /* 清除缓存的Facility消息及InvokeId */
        NAS_CC_SetSsSwitchOpFacility(VOS_FALSE, VOS_NULL_PTR);
        NAS_CC_SetSsSwitchInvokeId(VOS_FALSE, NAS_CC_INVALID_INVOKE_ID);

    }

    /* 再处理呼叫恢复通话 */
    /* 检查呼叫是否为多方通话，非多方通话直接将回复CALL模块RETRIEVE_REJ并更新状态 */
    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY != enRetrieveMptyState)
    {
        /* 如果RETRIEVE操作失败，则回复RETRIEVE_REJ */
        if (NAS_CC_SS_SWITCH_SUCCESS != enRetrieveState)
        {
            NAS_CC_SendMnccMsg(ulRetrieveEntityID,
                               MNCC_RETRIEVE_REJ,
                               &enRetrieveCause,
                               sizeof(enRetrieveCause));
        }

        NAS_CC_INFO_LOG("NAS_CC_ProcSsSwitchMultiCallFail: ChangeHoldAuxState to HELD");
        NAS_CC_ChangeHoldAuxState(ulRetrieveEntityID, NAS_CC_HOLD_AUX_S_CALL_HELD);
    }
    else
    {
        /* 呼叫为多方通话，则更新状态，并将缓存的Facility消息发送给CALL模块 */
        NAS_CC_ConditionalChangeAuxState(VOS_FALSE,
                                         VOS_TRUE,
                                         VOS_TRUE,
                                         VOS_FALSE,
                                         NAS_CC_HOLD_AUX_S_IDLE,
                                         NAS_CC_MPTY_AUX_S_CALL_IN_MPTY,
                                         NAS_CC_HOLD_AUX_S_CALL_HELD,
                                         NAS_CC_MPTY_AUX_S_IDLE);

        /* 如果Facility操作失败，则发送回复 */
        if (NAS_CC_SS_SWITCH_SUCCESS != enRetrieveState)
        {
            if (VOS_TRUE == NAS_CC_GetSsSwitchOpFacility())
            {
                pstMsg = NAS_CC_GetSsSwitchFacility();

                NAS_CC_SendMnccMsg(ulRetrieveEntityID,
                                   MNCC_FACILITY_IND,
                                   pstMsg,
                                   sizeof(NAS_CC_MSG_FACILITY_MT_STRU));
            }

            if (VOS_TRUE == NAS_CC_GetSsSwitchOpInvokeId())
            {
                ucInvokeId = NAS_CC_GetSsSwitchInvokeId();

                NAS_CC_SendMnccMsg(ulRetrieveEntityID,
                                   MNCC_FACILITY_LOCAL_REJ,
                                   &ucInvokeId,
                                   sizeof(ucInvokeId));

            }
        }

        /* 清除缓存的Facility消息及InvokeId */
        NAS_CC_SetSsSwitchOpFacility(VOS_FALSE, VOS_NULL_PTR);
        NAS_CC_SetSsSwitchInvokeId(VOS_FALSE, NAS_CC_INVALID_INVOKE_ID);

    }

    /* 补充业务多路切换状态处理成功，重置信息 */
    NAS_CC_InitSsSwitchInfo();

    return;
}
VOS_VOID NAS_CC_ProcSsSwitchMain(VOS_VOID)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulMapNum;
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enHoldState;
    NAS_CC_SS_SWITCH_STATE_ENUM_UINT8   enRetrievetate;
    NAS_CC_FUNC_VOID                    pFunc           = VOS_NULL_PTR;

    ulMapNum        = sizeof(f_astCcSsSwitchFuncMap) / sizeof(NAS_CC_SS_SWITCH_PROC_FUNC_MAP_STRU);
    enHoldState     = NAS_CC_GetSsSwitchHoldState();
    enRetrievetate  = NAS_CC_GetSsSwitchRetrieveState();

    for (ulLoop = 0; ulLoop < ulMapNum; ulLoop++)
    {
        if ( (f_astCcSsSwitchFuncMap[ulLoop].enHoldState == enHoldState)
          && (f_astCcSsSwitchFuncMap[ulLoop].enRetrieveState == enRetrievetate) )
        {
            pFunc = f_astCcSsSwitchFuncMap[ulLoop].pFunc;
        }
    }

    if (VOS_NULL_PTR != pFunc)
    {
        pFunc();
    }

    return;
}


VOS_UINT32 NAS_CC_GetFirstMptyCallEntityID(NAS_CC_ENTITY_ID_T EntityID)
{
    VOS_UINT8                           i;

    for (i=0; i < NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (EntityID == i)
        {
            continue;
        }

        if (VOS_TRUE == f_astCcEntities[i].bUsed)
        {
            if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY == f_astCcEntities[i].enMptyAuxState)
            {
                return i;
            }
        }
    }

    return NAS_CC_INVALID_ENTITY_ID;
}


VOS_UINT32 NAS_CC_IsLastCallEntityID(NAS_CC_ENTITY_ID_T EntityID)
{
    VOS_UINT8                           ucCallNumIndex;

    for (ucCallNumIndex = 0; ucCallNumIndex < NAS_CC_MAX_ENTITY_NUM; ucCallNumIndex++)
    {
        if (EntityID == ucCallNumIndex)
        {
            continue;
        }

        if (VOS_TRUE == f_astCcEntities[ucCallNumIndex].bUsed)
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}



VOS_VOID NAS_CC_ProcSsSwitchCallRelease(NAS_CC_ENTITY_ID_T EntityID)
{
    NAS_CC_ENTITY_ID_T                  OtherMptyEntityID;
    NAS_CC_ENTITY_ID_T                  ulHoldEntityID;
    NAS_CC_ENTITY_ID_T                  ulRetrieveEntityID;
    NAS_CC_MPTY_AUX_STATE_ENUM_U8       enMptyStatus;

    /* 获取呼叫的多方通话状态和HOLD/RETRIEVE状态 */
    enMptyStatus        = NAS_CC_GetMptyAuxState(EntityID);

    ulHoldEntityID      = NAS_CC_GetSsSwitchHoldEntityID();
    ulRetrieveEntityID  = NAS_CC_GetSsSwitchRetrieveEntityID();

    /* 被释放的呼叫实体ID不在切换补充业务关注的呼叫实体ID中，不处理 */
    if ((EntityID != ulHoldEntityID)
     && (EntityID != ulRetrieveEntityID))
    {
        return;
    }

    /* 最后一路呼叫挂断，需要按超时处理给taf call回复等待消息，否则call状态异常 */
    if (VOS_TRUE == NAS_CC_IsLastCallEntityID(EntityID))
    {
        if (EntityID == ulHoldEntityID)
        {
            NAS_CC_SetSsSwitchHoldInfo(EntityID, NAS_CC_SS_SWITCH_LAST_CALL_RELEASED, NAS_CC_CAUSE_NULL);
        }
        else
        {
            NAS_CC_SetSsSwitchRetrieveInfo(EntityID, NAS_CC_SS_SWITCH_LAST_CALL_RELEASED, NAS_CC_CAUSE_NULL);
        }

        NAS_CC_ProcSsSwitchMain();

        return;

    }

    if (NAS_CC_MPTY_AUX_S_CALL_IN_MPTY == enMptyStatus)
    {
        /* 还有其他呼叫在会议中，则将切换补充业务关注的呼叫实体ID更新为会议中未释放的呼叫 */
        OtherMptyEntityID = NAS_CC_GetFirstMptyCallEntityID(EntityID);

        if (NAS_CC_INVALID_ENTITY_ID != OtherMptyEntityID)
        {
            if (EntityID == ulHoldEntityID)
            {
                NAS_CC_SetSsSwitchHoldEntityID(OtherMptyEntityID);
            }
            else
            {
                NAS_CC_SetSsSwitchRetrieveEntityID(OtherMptyEntityID);
            }

            return;

        }

        /* 会议中没有未释放的呼叫，则清除切换补充业务信息的会议信息 */
        NAS_CC_SetSsSwitchOpFacility(VOS_FALSE, VOS_NULL_PTR);
        NAS_CC_SetSsSwitchInvokeId(VOS_FALSE, NAS_CC_INVALID_INVOKE_ID);

    }

    /*
    被释放呼叫是切换补充业务关注的保持操作相关呼叫，则清空补充业务切换信息HOLD部分
    被释放呼叫是切换补充业务关注的恢复操作相关呼叫，则清空补充业务切换信息RETRIEVE部分
    */
    if (EntityID == ulHoldEntityID)
    {
        NAS_CC_SetSsSwitchHoldInfo(NAS_CC_INVALID_ENTITY_ID, NAS_CC_SS_SWITCH_IDLE, NAS_CC_CAUSE_NULL);
    }
    else
    {
        NAS_CC_SetSsSwitchRetrieveInfo(NAS_CC_INVALID_ENTITY_ID, NAS_CC_SS_SWITCH_IDLE, NAS_CC_CAUSE_NULL);
    }

    NAS_CC_ProcSsSwitchMain();

    return;
}

#if (FEATURE_ON == FEATURE_IMS)

VOS_VOID  NAS_CC_CreateCcEntityWithCallEntityInfo(
    VOS_VOID                           *pMsg
)
{
    NAS_CC_BC_PARAMS_STRU               stBcParams;
    NAS_CC_ENTITY_ID_T                  entityId;
    VOS_UINT8                           i;
    VOS_UINT8                           ucCallEntityNum;
    MNCC_SRVCC_CALL_INFO_NOTIFY_STRU   *pstSrvccCallInfoNtf = VOS_NULL_PTR;

    pstSrvccCallInfoNtf = (MNCC_SRVCC_CALL_INFO_NOTIFY_STRU *)pMsg;

    /* 边界检查，避免数组越界 */
    ucCallEntityNum = pstSrvccCallInfoNtf->ucCallNum;
    if (MNCC_MAX_ENTITY_NUM < ucCallEntityNum)
    {
        ucCallEntityNum = MNCC_MAX_ENTITY_NUM;
    }

    /* 按照协议规定，SRVCC成功到GU后,都当做MT CALL,由IMSA配好TI相关信息同步到CALL模块 */
    for (i = 0; i < ucCallEntityNum; i++)
    {
        /* 创建实体成功时，同步呼叫信息 */
        if (VOS_OK == NAS_CC_CreateEntity(NAS_CC_CALL_TYPE_MT_NORMAL_CALL, pstSrvccCallInfoNtf->astEntityStatus[i].ucCallId, pstSrvccCallInfoNtf->astEntityStatus[i].ucTi, &entityId))
        {
            f_astCcEntities[entityId].enCallState    = pstSrvccCallInfoNtf->astEntityStatus[i].enCallState;
            f_astCcEntities[entityId].ucTi           = pstSrvccCallInfoNtf->astEntityStatus[i].ucTi;
            f_astCcEntities[entityId].enHoldAuxState = pstSrvccCallInfoNtf->astEntityStatus[i].enHoldAuxState;
            f_astCcEntities[entityId].enMptyAuxState = pstSrvccCallInfoNtf->astEntityStatus[i].enMptyAuxState;

            /* 更新BC信息 */
            stBcParams.enItc = pstSrvccCallInfoNtf->astEntityStatus[i].stBC1.Octet3.InfoTransCap;
            NAS_CC_UpdateBcParams(entityId, NAS_CC_BC_PARAM_1, &stBcParams);

            if (pstSrvccCallInfoNtf->astEntityStatus[i].stBC2.IsExist)
            {
                stBcParams.enItc = pstSrvccCallInfoNtf->astEntityStatus[i].stBC2.Octet3.InfoTransCap;
                NAS_CC_UpdateBcParams(entityId, NAS_CC_BC_PARAM_2, &stBcParams);
            }
        }
    }
}
#endif


VOS_VOID  NAS_CC_GetEntityTiInfo(
    VOS_UINT8                          *pucTiNum,
    VOS_UINT8                          *pucTi
)
{
    VOS_UINT8                           i;

    *pucTiNum = 0;

    /* 寻找未使用的CC实体 */
    for (i =0; i <NAS_CC_MAX_ENTITY_NUM; i++)
    {
        if (VOS_FALSE == f_astCcEntities[i].bUsed)
        {
            continue;
        }

        pucTi[(*pucTiNum)++] = f_astCcEntities[i].ucTi;
    }
}


#if (FEATURE_ON == FEATURE_PTM)
VOS_VOID NAS_CC_SndAcpuOmErrLogRptCnf(
    VOS_CHAR                           *pbuffer,
    VOS_UINT32                          ulBufUseLen
)
{
    OM_ERR_LOG_REPORT_CNF_STRU         *pstQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulMsgLen;

    /* 上报的消息总长度 */
    ulMsgLen  = ((sizeof(OM_ERR_LOG_REPORT_CNF_STRU) - VOS_MSG_HEAD_LENGTH) - 4) + ulBufUseLen;

    /* 消息空间申请 */
    pstQryCnf = (OM_ERR_LOG_REPORT_CNF_STRU *)PS_ALLOC_MSG(WUEPS_PID_CC, ulMsgLen);
    if (VOS_NULL_PTR == pstQryCnf)
    {
        NAS_CC_ERR_LOG("NAS_CC_SndAcpuOmErrLogRptCnf: alloc msg fail!");
        return;
    }

    pstQryCnf->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstQryCnf->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstQryCnf->ulReceiverPid    = ACPU_PID_OM;
    pstQryCnf->ulSenderPid      = WUEPS_PID_CC;
    pstQryCnf->ulMsgName        = ID_OM_ERR_LOG_REPORT_CNF;
    pstQryCnf->ulMsgType        = OM_ERR_LOG_MSG_ERR_REPORT;
    pstQryCnf->ulMsgSN          = 0;
    pstQryCnf->ulRptlen         = ulBufUseLen;

    /* buffer不为空时，len也是不会为空的 */
    if (VOS_NULL_PTR != pbuffer)
    {
        PS_MEM_CPY(pstQryCnf->aucContent, pbuffer, ulBufUseLen);
    }

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_CC, pstQryCnf))
    {
        NAS_CC_ERR_LOG("NAS_CC_SndAcpuOmErrLogRptCnf: SEND MSG FAIL");
    }

    return;

}
VOS_VOID NAS_CC_RcvAcpuOmErrLogRptReq(
    const VOS_VOID                           *pMsg
)
{
    VOS_CHAR                           *pbuffer   = VOS_NULL_PTR;
    VOS_UINT32                          ulBufUseLen;
    VOS_UINT32                          ulRealLen;
    VOS_UINT32                          ulTotalLen;
    NAS_ERR_LOG_MNTN_EVENT_STRU         stNasErrLogMntnEvent;

    /* 查询一下RING BUFFER中有多少数据，以便分配内存 */
    ulBufUseLen = NAS_CC_GetErrLogRingBufferUseBytes();
    ulTotalLen = ulBufUseLen + sizeof(NAS_ERR_LOG_MNTN_EVENT_STRU);

    pbuffer = (VOS_CHAR *)PS_MEM_ALLOC(WUEPS_PID_CC, ulTotalLen);
    if (VOS_NULL_PTR == pbuffer)
    {
        /* 发送ID_OM_ERR_LOG_REPORT_CNF内容为空的消息给OM */
        NAS_CC_SndAcpuOmErrLogRptCnf(VOS_NULL_PTR, 0);
        return;
    }

    PS_MEM_SET(pbuffer, 0, ulTotalLen);

    /* 获取RING BUFFER的内容 */
    ulRealLen = NAS_CC_GetErrLogRingBufContent(pbuffer, ulBufUseLen);
    if (ulRealLen != ulBufUseLen)
    {
        /* 发送ID_OM_ERR_LOG_REPORT_CNF内容为空的消息给OM */
        NAS_CC_SndAcpuOmErrLogRptCnf(VOS_NULL_PTR, 0);
        PS_MEM_FREE(WUEPS_PID_CC, pbuffer);
        return;
    }

    /* 将缓冲区溢出次数信息追加在RingBuf后面 */
    NAS_COMM_BULID_ERRLOG_HEADER_INFO(&stNasErrLogMntnEvent.stHeader,
                                      VOS_GetModemIDFromPid(WUEPS_PID_CC),
                                      NAS_ERR_LOG_ALM_MNTN,
                                      NAS_GetErrLogAlmLevel(NAS_ERR_LOG_ALM_MNTN),
                                      VOS_GetSlice(),
                                      (sizeof(NAS_ERR_LOG_MNTN_EVENT_STRU) - sizeof(OM_ERR_LOG_HEADER_STRU)));

    stNasErrLogMntnEvent.ulCount = NAS_CC_GetErrlogOverflowCnt();

    PS_MEM_CPY(pbuffer + ulBufUseLen, &stNasErrLogMntnEvent, sizeof(stNasErrLogMntnEvent));

    /* 获取完了后需要将RINGBUFFER清空 */
    NAS_CC_CleanErrLogRingBuf();

    /* 重置溢出计数 */
    NAS_CC_SetErrlogOverflowCnt(0);

    /* 可维可测BUF溢出的勾包 */
    NAS_COM_MntnPutRingbuf(NAS_ERR_LOG_ALM_MNTN,
                           WUEPS_PID_CC,
                           (VOS_UINT8 *)&stNasErrLogMntnEvent,
                           sizeof(stNasErrLogMntnEvent));

    /* 发送ID_OM_ERR_LOG_REPORT_CNF消息给ACPU OM */
    NAS_CC_SndAcpuOmErrLogRptCnf(pbuffer, ulTotalLen);

    PS_MEM_FREE(WUEPS_PID_CC, pbuffer);

    return;
}
VOS_VOID NAS_CC_RcvAcpuOmErrLogCtrlInd(
    const VOS_VOID                           *pMsg
)
{
    OM_ERROR_LOG_CTRL_IND_STRU         *pstRcvMsg = VOS_NULL_PTR;

    pstRcvMsg = (OM_ERROR_LOG_CTRL_IND_STRU*)pMsg;

    /* 更新ERRLOG控制标识 */
    if ((VOS_FALSE == pstRcvMsg->ucAlmStatus)
     || (VOS_TRUE  == pstRcvMsg->ucAlmStatus))
    {
        NAS_CC_SetErrlogCtrlFlag(pstRcvMsg->ucAlmStatus);
    }

    if ((pstRcvMsg->ucAlmLevel >= NAS_ERR_LOG_CTRL_LEVEL_CRITICAL)
     && (pstRcvMsg->ucAlmLevel <= NAS_ERR_LOG_CTRL_LEVEL_WARNING))
    {
        NAS_CC_SetErrlogAlmLevel(pstRcvMsg->ucAlmLevel);
    }

    return;
}
VOS_VOID NAS_CC_RcvAcpuOmMsg(
    const VOS_VOID                     *pMsg
)
{
    MSG_HEADER_STRU                    *pMsgHeader = (MSG_HEADER_STRU *)pMsg;

    switch(pMsgHeader->ulMsgName)
    {
        case ID_OM_ERR_LOG_CTRL_IND:
            NAS_CC_RcvAcpuOmErrLogCtrlInd(pMsg);
            break;

        case ID_OM_ERR_LOG_REPORT_REQ:
            NAS_CC_RcvAcpuOmErrLogRptReq(pMsg);
            break;

        default:
            break;
    }

    return;
}
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
