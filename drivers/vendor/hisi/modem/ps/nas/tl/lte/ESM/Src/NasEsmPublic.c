


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEsmPublic.h"
#include    "NasEsmInclude.h"
#include    "NasCommBuffer.h"
#include    "NasCommPrint.h"
#include    "PsMd5.h"
#include    "NasIpInterface.h"
#include    "IpComm.h"


/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASESMPUBULIC_C
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
VOS_UINT32                              g_ulPcoFlag     = NAS_ESM_PCO_ON;
VOS_UINT32                              g_ulMd5Flag     = NAS_ESM_FUN_MD5_CTRL_ON;
VOS_UINT32                              g_ulDelSdfWithQosFlag = LPS_NV_GET_DEL_SDF_WITH_QOS_BIT;

VOS_UINT8                               g_aucChallengeValue[NAS_ESM_AUTH_CHALLENGE_VALUE_LEN] = {
                                                                0x36,0x50,0x4e,0x20,0x77,0x47,0x19,0x93,
                                                                0xa0,0x24,0x13,0x34,0xb3,0x39,0xe3,0x65};

VOS_UINT8                               g_aucChallegeRspValue[NAS_ESM_AUTH_RESPONSE_VALUE_LEN] = {
                                                                0x57,0x14,0x72,0x31,0x04,0xa3,0x19,0x69,
                                                                0xb5,0x24,0x45,0x87,0x64,0x29,0xc4,0xb6};


NAS_ESM_APP_ERROR_MAPPING_STRU  g_astEsmAppErrorMappingTbl[] =
{
    {APP_SUCCESS                                        ,NAS_ESM_CAUSE_SUCCESS                                    },
    {APP_ERR_SM_NW_PROT_ERR_UNSPECIFIED                 ,NAS_ESM_CAUSE_ERROR                                      },

    {APP_ERR_SM_NW_OPERATOR_DETERMINED_BARRING                  ,NAS_ESM_CAUSE_OPERATOR_DETERMINE_BARRING                 },
    {APP_ERR_SM_NW_INSUFFICIENT_RESOURCES                       ,NAS_ESM_CAUSE_INSUFFICIENT_RESOURCES                     },
    {APP_ERR_SM_NW_MISSING_OR_UKNOWN_APN                        ,NAS_ESM_CAUSE_UNKNOWN_OR_MISSING_APN                     },
    {APP_ERR_SM_NW_UNKNOWN_PDN_TYPE                             ,NAS_ESM_CAUSE_UNKNOWN_PDN_TYPE                           },
    {APP_ERR_SM_NW_USER_AUTH_FAIL                               ,NAS_ESM_CAUSE_USER_AUTH_FAIL                             },
    {APP_ERR_SM_NW_REQ_REJ_BY_SGW_OR_PGW                        ,NAS_ESM_CAUSE_REQ_REJ_BY_SGW_OR_PGW                      },
    {APP_ERR_SM_NW_REQ_REJ_UNSPECITY                            ,NAS_ESM_CAUSE_REQ_REJ_UNSPECITY                          },
    {APP_ERR_SM_NW_SERVICE_OPTION_NOT_SUPPORT                   ,NAS_ESM_CAUSE_SERVICE_OPTION_NOT_SUPPORT                 },
    {APP_ERR_SM_NW_REQ_SERVICE_NOT_SUBSCRIBE                    ,NAS_ESM_CAUSE_REQ_SERVICE_NOT_SUBSCRIBED                 },
    {APP_ERR_SM_NW_SERVICE_OPTION_TEMP_OUT_ORDER                ,NAS_ESM_CAUSE_SERVICE_OPTION_TEMP_OUT_ORDER              },
    {APP_ERR_SM_NW_PTI_ALREADY_IN_USE                           ,NAS_ESM_CAUSE_PTI_ALREADY_IN_USED                        },
    {APP_ERR_SM_NW_REGULAR_DEACT                                ,NAS_ESM_CAUSE_REGULAR_DEACTIVATION                       },
    {APP_ERR_SM_NW_EPS_QOS_NOT_ACCEPT                           ,NAS_ESM_CAUSE_EPS_QOS_NOT_ACCEPT                         },
    {APP_ERR_SM_NW_NET_FAIL                                     ,NAS_ESM_CAUSE_NETWORK_FAILURE                            },
    {APP_ERR_SM_NW_PROT_ERR_UNSPECIFIED                         ,NAS_ESM_CAUSE_FEATURE_NOT_SUPPORTED                      },
    {APP_ERR_SM_NW_SEMANTIC_ERR_IN_TFT                          ,NAS_ESM_CAUSE_SEMANTIC_ERR_IN_TFT                        },
    {APP_ERR_SM_NW_SYNTACTIC_ERR_IN_TFT                         ,NAS_ESM_CAUSE_SYNTACTIC_ERR_IN_TFT                       },
    {APP_ERR_SM_NW_INVALID_EPS_BERER_IDENTITY                   ,NAS_ESM_CAUSE_INVALID_EPS_BEARER_IDENTITY                },
    {APP_ERR_SM_NW_SEMANTIC_ERR_IN_PACKET_FILTER                ,NAS_ESM_CAUSE_SEMANTIC_ERR_IN_PACKET_FILTER              },
    {APP_ERR_SM_NW_SYNCTACTIC_ERR_IN_PACKET_FILTER              ,NAS_ESM_CAUSE_SYNCTACTIC_ERR_IN_PACKET_FILTER            },
    {APP_ERR_SM_NW_BEARER_WITHOUT_TFT_ACT                       ,NAS_ESM_CAUSE_BEARER_WITHOUT_TFT_ACT                     },
    {APP_ERR_SM_NW_PTI_MISMATICH                                ,NAS_ESM_CAUSE_PTI_MISMATCH                               },
    {APP_ERR_SM_NW_LAST_PDN_DISCONN_NOT_ALLOWED                 ,NAS_ESM_CAUSE_LAST_PDN_DISCONN_NOT_ALLOWED               },
    {APP_ERR_SM_NW_PDN_TPYE_IPV4_ONLY_ALLOWED                   ,NAS_ESM_CAUSE_PDNTYPE_IPV4_ONLY_ALLOWED                  },
    {APP_ERR_SM_NW_PDN_TPYE_IPV6_ONLY_ALLOWED                   ,NAS_ESM_CAUSE_PDNTYPE_IPV6_ONLY_ALLOWED                  },
    {APP_ERR_SM_NW_SINGLE_ADDR_BERERS_ONLY_ALLOWED              ,NAS_ESM_CAUSE_SINGLE_ADDR_BEARER_ONLY_ALLOWED            },
    {APP_ERR_SM_NW_ESM_INFO_NOT_RECEIVED                        ,NAS_ESM_CAUSE_ESM_INFORMATION_NOT_RECEIVED               },
    {APP_ERR_SM_NW_PDN_CONN_NOT_EXIST                           ,NAS_ESM_CAUSE_PDN_CONNECTION_DOES_NOT_EXIST              },
    {APP_ERR_SM_NW_MULTI_PDN_CONN_FOR_ONE_APN_NOT_ALLOWED       ,NAS_ESM_CAUSE_SAME_APN_MULTI_PDN_CONNECTION_NOT_ALLOWED  },
    {APP_ERR_SM_NW_COLLISION_WITH_NW_INTIAL_REQEST              ,NAS_ESM_CAUSE_COLLISION_WITH_NETWORK_INITIATED_REQUEST   },
    {APP_ERR_SM_NW_UNSUPPORTED_QCI_VALUE                        ,NAS_ESM_CAUSE_UNSUPPORTED_QCI_VALUE                      },
    {APP_ERR_SM_NW_INVALID_PTI_VALUE                            ,NAS_ESM_CAUSE_INVALID_PTI_VALUE                          },
    {APP_ERR_SM_NW_SYNCTACTIC_INCORRECT_MSG                     ,NAS_ESM_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE             },
    {APP_ERR_SM_NW_INVALID_MANDATORY_INFOR                      ,NAS_ESM_CAUSE_INVALID_MANDATORY_INFORMATION              },
    {APP_ERR_SM_NW_MSG_TYPE_NON_EXIST                           ,NAS_ESM_CAUSE_MESSAGE_TYPE_NON_EXIST_OR_NOT_IMPLEMENTED  },
    {APP_ERR_SM_NW_MSG_TYPE_NOT_COMPATIBLE_WITH_PROT            ,NAS_ESM_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STA  },
    {APP_ERR_SM_NW_INFOR_ELEM_NON_EXIST                         ,NAS_ESM_CAUSE_INFO_ELEMENT_NON_EXIST_OR_NOT_IMPLEMENTED  },
    {APP_ERR_SM_NW_CONDITIONAL_IE_ERROR                         ,NAS_ESM_CAUSE_CONDITIONAL_IE_ERROR                       },
    {APP_ERR_SM_NW_MSG_NOT_COMPATIBLE_WITH_PROT                 ,NAS_ESM_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROT_STA       },
    {APP_ERR_SM_NW_PROT_ERR_UNSPECIFIED                         ,NAS_ESM_CAUSE_PROTOCOL_ERROR                             },

    {APP_ERR_SM_NW_APN_RESTRICTION_INCOMPATIBLE_WITH_ACT_EPS_BEARER , NAS_ESM_CAUSE_APN_RESTRICTION_VAL_INCOMPATIBLE_WITH_ACT_BEARER}

};


NAS_COMM_PRINT_LIST_STRU g_astEsmAppMsgIdArray[] =
{
    {   ID_APP_ESM_SET_TFT_REQ               ,
            "MSG:  ID_APP_ESM_SET_TFT_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_SET_TFT_CNF     ,
            "MSG:  ID_APP_ESM_SET_TFT_CNF                 ",
        NAS_ESM_PrintEsmAppSetResult},
    {   ID_APP_ESM_SET_QOS_REQ               ,
            "MSG:  ID_APP_ESM_SET_QOS_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_SET_QOS_CNF               ,
            "MSG:  ID_APP_ESM_SET_QOS_CNF                           ",
        NAS_ESM_PrintEsmAppSetResult},
    {   ID_APP_ESM_SET_EPS_QOS_REQ             ,
            "MSG:  ID_APP_ESM_SET_EPS_QOS_REQ                         ",
        NAS_ESM_PrintAppEsmSetEpsQosReqMsg},
    {   ID_APP_ESM_SET_EPS_QOS_CNF               ,
            "MSG:  ID_APP_ESM_SET_EPS_QOS_CNF                           ",
        NAS_ESM_PrintEsmAppSetResult},
    {   ID_APP_ESM_SET_APN_REQ     ,
            "MSG:  ID_APP_ESM_SET_APN_REQ                 ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_SET_APN_CNF               ,
            "MSG:  ID_APP_ESM_SET_APN_CNF                           ",
        NAS_ESM_PrintEsmAppSetResult},
    {   ID_APP_ESM_SET_PCO_REQ               ,
            "MSG:  ID_APP_ESM_SET_PCO_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_SET_PCO_CNF             ,
            "MSG:  ID_APP_ESM_SET_PCO_CNF                         ",
        NAS_ESM_PrintEsmAppSetResult},
    {   ID_APP_ESM_SET_PDN_TYPE_REQ               ,
            "MSG:  ID_APP_ESM_SET_PDN_TYPE_REQ                           ",
        NAS_ESM_PrintAppEsmSetPdnTypeReqMsg},
    {   ID_APP_ESM_SET_PDN_TYPE_CNF     ,
            "MSG:  ID_APP_ESM_SET_PDN_TYPE_CNF                 ",
        NAS_ESM_PrintEsmAppSetResult},
    {   ID_APP_ESM_SET_BEARER_TYPE_REQ               ,
            "MSG:  ID_APP_ESM_SET_BEARER_TYPE_REQ                           ",
        NAS_ESM_PrintAppEsmSetBearerTypeReqMsg},
    {   ID_APP_ESM_SET_BEARER_TYPE_CNF               ,
            "MSG:  ID_APP_ESM_SET_BEARER_TYPE_CNF                           ",
        NAS_ESM_PrintEsmAppSetResult},
    {   ID_APP_ESM_SET_PDP_MANAGER_TYPE_REQ             ,
            "MSG:  ID_APP_ESM_SET_PDP_MANAGER_TYPE_REQ                         ",
        NAS_ESM_PrintAppEsmSetManageTypeReqMsg},
    {   ID_APP_ESM_SET_PDP_MANAGER_TYPE_CNF               ,
            "MSG:  ID_APP_ESM_SET_PDP_MANAGER_TYPE_CNF                           ",
        NAS_ESM_PrintEsmAppSetManageTypeResult},
    {   ID_APP_ESM_SET_GW_AUTH_REQ,
            "MSG:  ID_APP_ESM_SET_GW_AUTH_REQ                           ",
        NAS_ESM_PrintAppEsmSetGwAuthReqMsg},
    {   ID_APP_ESM_SET_GW_AUTH_CNF               ,
        "MSG:  ID_APP_ESM_SET_GW_AUTH_CNF                           ",
        NAS_ESM_PrintEsmAppSetResult},

    {   ID_APP_ESM_SET_CGDCONT_REQ,
            "MSG:  ID_APP_ESM_SET_CGDCONT_REQ                           ",
        NAS_ESM_PrintAppEsmSetCgdontReqMsg},
    {   ID_APP_ESM_SET_CGDCONT_CNF               ,
        "MSG:  ID_APP_ESM_SET_CGDCONT_CNF                           ",
        NAS_ESM_PrintEsmAppCgdontResult},

    {   ID_APP_ESM_IPV6_INFO_NOTIFY               ,
        "MSG:  ID_APP_ESM_IPV6_INFO_NOTIFY                           ",
        NAS_ESM_PrintEsmAppIpv6InfoNotify},
    {   ID_APP_ESM_PROCEDURE_ABORT_NOTIFY     ,
            "MSG:  ID_APP_ESM_PROCEDURE_ABORT_NOTIFY                 ",
        NAS_ESM_PrintEsmAppProcedureAbortNotify},
    {   ID_APP_ESM_INQ_TFT_REQ     ,
            "MSG:  ID_APP_ESM_INQ_TFT_REQ                 ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_TFT_CNF               ,
            "MSG:  ID_APP_ESM_INQ_TFT_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_QOS_REQ               ,
            "MSG:  ID_APP_ESM_INQ_QOS_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_QOS_CNF             ,
            "MSG:  ID_APP_ESM_INQ_QOS_CNF                         ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_EPS_QOS_REQ               ,
            "MSG:  ID_APP_ESM_INQ_EPS_QOS_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_EPS_QOS_CNF     ,
            "MSG:  ID_APP_ESM_INQ_EPS_QOS_CNF                 ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_APN_REQ               ,
            "MSG:  ID_APP_ESM_INQ_APN_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_APN_CNF               ,
            "MSG:  ID_APP_ESM_INQ_APN_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_PCO_REQ             ,
            "MSG:  ID_APP_ESM_INQ_PCO_REQ                         ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_PCO_CNF               ,
            "MSG:  ID_APP_ESM_INQ_PCO_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_PDN_TYPE_REQ     ,
            "MSG:  ID_APP_ESM_INQ_PDN_TYPE_REQ                 ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_PDN_TYPE_CNF               ,
            "MSG:  ID_APP_ESM_INQ_PDN_TYPE_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_BEARER_TYPE_REQ               ,
            "MSG:  ID_APP_ESM_INQ_BEARER_TYPE_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_BEARER_TYPE_CNF             ,
            "MSG:  ID_APP_ESM_INQ_BEARER_TYPE_CNF                         ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_PDP_MANAGER_TYPE_REQ               ,
        "MSG:  ID_APP_ESM_INQ_PDP_MANAGER_TYPE_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_PDP_MANAGER_TYPE_CNF     ,
        "MSG:  ID_APP_ESM_INQ_PDP_MANAGER_TYPE_CNF                 ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_BEAER_QOS_REQ               ,
        "MSG:  ID_APP_ESM_INQ_BEAER_QOS_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_BEAER_QOS_CNF               ,
        "MSG:  ID_APP_ESM_INQ_BEAER_QOS_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_BEAER_QOS_IND             ,
        "MSG:  ID_APP_ESM_INQ_BEAER_QOS_IND                         ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_DYNAMIC_PDP_CONT_REQ               ,
        "MSG:  ID_APP_ESM_INQ_DYNAMIC_PDP_CONT_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_DYNAMIC_PDP_CONT_CNF     ,
        "MSG:  ID_APP_ESM_INQ_DYNAMIC_PDP_CONT_CNF                 ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_DYNAMIC_EPS_QOS_REQ               ,
        "MSG:  ID_APP_ESM_INQ_DYNAMIC_EPS_QOS_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_DYNAMIC_EPS_QOS_CNF               ,
        "MSG:  ID_APP_ESM_INQ_DYNAMIC_EPS_QOS_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_DYNAMIC_TFT_REQ             ,
        "MSG:  ID_APP_ESM_INQ_DYNAMIC_TFT_REQ                         ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_DYNAMIC_TFT_CNF               ,
            "MSG:  ID_APP_ESM_INQ_DYNAMIC_TFT_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_GW_AUTH_REQ             ,
        "MSG:  ID_APP_ESM_INQ_GW_AUTH_REQ                         ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_INQ_GW_AUTH_CNF               ,
            "MSG:  ID_APP_ESM_INQ_GW_AUTH_CNF                           ",
        VOS_NULL_PTR},
    {   ID_OM_ESM_INFO_REPORT_REQ               ,
            "MSG:  ID_OM_ESM_INFO_REPORT_REQ                           ",
        VOS_NULL_PTR},
    {   ID_OM_ESM_INFO_REPORT_CNF,
        "MSG:  ID_APP_OM_INFO_REPORT_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_SETUP_REQ     ,
            "MSG:  ID_APP_ESM_PDP_SETUP_REQ                 ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_SETUP_CNF               ,
            "MSG:  ID_APP_ESM_PDP_SETUP_CNF                           ",
        VOS_NULL_PTR},

    {   ID_APP_ESM_NDISCONN_REQ,
            "MSG:  ID_APP_ESM_NDISCONN_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_NDISCONN_CNF               ,
        "MSG:  ID_APP_ESM_NDISCONN_CNF                           ",
        VOS_NULL_PTR},

    {   ID_APP_ESM_PDP_SETUP_IND               ,
            "MSG:  ID_APP_ESM_PDP_SETUP_IND                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_MODIFY_REQ             ,
            "MSG:  ID_APP_ESM_PDP_MODIFY_REQ                         ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_MODIFY_CNF               ,
            "MSG:  ID_APP_ESM_PDP_MODIFY_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_MODIFY_IND     ,
            "MSG:  ID_APP_ESM_PDP_MODIFY_IND                 ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_RELEASE_REQ               ,
            "MSG:  ID_APP_ESM_PDP_RELEASE_REQ                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_RELEASE_CNF               ,
            "MSG:  ID_APP_ESM_PDP_RELEASE_CNF                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_RELEASE_IND             ,
            "MSG:  ID_APP_ESM_PDP_RELEASE_IND                         ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_MANAGER_IND               ,
            "MSG:  ID_APP_ESM_PDP_MANAGER_IND                           ",
        VOS_NULL_PTR},
    {   ID_APP_ESM_PDP_MANAGER_RSP             ,
            "MSG:  ID_APP_ESM_PDP_MANAGER_RSP                         ",
        VOS_NULL_PTR},
};

NAS_COMM_PRINT_MSG_LIST_STRU g_astEsmStateTimerArray[] =
{
    {   TI_NAS_ESM_T3482               ,
            "STATE TI:  TI_NAS_ESM_T3482                           "},
    {   TI_NAS_ESM_T3492     ,
            "STATE TI:  TI_NAS_ESM_T3492                 "},
    {   TI_NAS_ESM_T3480               ,
            "STATE TI:  TI_NAS_ESM_T3480                           "},
    {   TI_NAS_ESM_T3481               ,
            "STATE TI:  TI_NAS_ESM_T3481                           "},
    {   TI_NAS_ESM_WAIT_APP_CNF             ,
            "STATE TI:  TI_NAS_ESM_WAIT_APP_CNF                         "},
    {   TI_NAS_ESM_WAIT_PTI_INVALID               ,
            "STATE TI:  TI_NAS_ESM_WAIT_PTI_INVALID                           "},
    {   TI_NAS_ESM_ATTACH_BEARER_REEST               ,
            "STATE TI:  TI_NAS_ESM_ATTACH_BEARER_REEST                           "},
    {   TI_NAS_ESM_REL_NON_EMC_BEARER,
            "STATE TI:  TI_NAS_ESM_REL_NON_EMC_BEARER               "}
};

/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
 Function Name   : NAS_ESM_IsGbrBearer
 Description     : �ж��Ƿ���GBR����
 Input           : ucQci--------QCI
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-1-13  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_ESM_IsGbrBearer( VOS_UINT8 ucQci )
{
    if ((ucQci >= NAS_ESM_QCI_TYPE_QCI1_GBR) && (ucQci <= NAS_ESM_QCI_TYPE_QCI4_GBR))
    {
        return PS_TRUE;
    }

    return PS_FALSE;
}

/*****************************************************************************
 Function Name   : NAS_ESM_GetBearCntxtType
 Description     : ��ȡ��������
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-8-31  Draft Enact

*****************************************************************************/
NAS_ESM_BEARER_TYPE_ENUM_UINT8  NAS_ESM_GetBearCntxtType( VOS_UINT32 ulEpsbId )
{
    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_GetBearCntxtType:Unsigned or Reserved Eps Bearer ID!");
        return NAS_ESM_BEARER_TYPE_BUTT;
    }

    return (NAS_ESM_Entity())->astEpsbCntxtInfo[(ulEpsbId - NAS_ESM_MIN_EPSB_ID)].enBearerCntxtType;
}

/*****************************************************************************
 Function Name   : NAS_ESM_SetBearCntxtType
 Description     : ���ó�������
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-8-31  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_SetBearCntxtType
(
    VOS_UINT32                          ulEpsbId,
    NAS_ESM_BEARER_TYPE_ENUM_UINT8      enDestType
)
{
    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_SetBearCntxtType:Unsigned or Reserved Eps Bearer ID!");
        return ;
    }

    (NAS_ESM_Entity())->astEpsbCntxtInfo[(ulEpsbId - NAS_ESM_MIN_EPSB_ID)].enBearerCntxtType = enDestType;
}


/*****************************************************************************
 Function Name   : NAS_ESM_GetBearCntxtState
 Input           : ��ȡ����״̬
 Output          : None
 Return          : NAS_ESM_BEARER_STATE_ENUM_UINT8

 History         :
    1.lihong00150010      2009-8-28  Draft Enact

*****************************************************************************/
NAS_ESM_BEARER_STATE_ENUM_UINT8 NAS_ESM_GetBearCntxtState(VOS_UINT32 ulEpsbId)
{
    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_GetBearCntxtState:Unsigned or Reserved Eps Bearer ID!");
        return NAS_ESM_BEARER_STATE_INACTIVE;
    }

    return (NAS_ESM_Entity())->astEpsbCntxtInfo[(ulEpsbId - NAS_ESM_MIN_EPSB_ID)].enBearerCntxtState;
}

/*****************************************************************************
 Function Name   : NAS_ESM_SetBearCntxtState
 Input           : ���ó���״̬
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-8-28  Draft Enact

*****************************************************************************/
/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_ESM_SetBearCntxtState
(
    VOS_UINT32                          ulEpsbId,
    NAS_ESM_BEARER_STATE_ENUM_UINT8     enDestState
)
{
    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_SetBearCntxtState:Unsigned or Reserved Eps Bearer ID!");
        return ;
    }

    NAS_ESM_NORM_LOG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    NAS_ESM_LOG1("SM Bearer Context state change----EpsbId : ", ulEpsbId);
    NAS_ESM_PRINT_BEARCNTXTSTA(NAS_ESM_GetBearCntxtState(ulEpsbId));
    NAS_ESM_NORM_LOG("==>>");
    (NAS_ESM_Entity())->astEpsbCntxtInfo[(ulEpsbId - NAS_ESM_MIN_EPSB_ID)].enBearerCntxtState = enDestState;
    NAS_ESM_PRINT_BEARCNTXTSTA(enDestState);
    NAS_ESM_NORM_LOG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
 }

/*****************************************************************************
 Function Name   : NAS_ESM_SetBearCntxtLinkCid
 Description     : ���ó��ع�����CID
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-8-31  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_SetBearCntxtLinkCid
(
    VOS_UINT32                          ulEpsbId,
    VOS_UINT32                          ulCid
)
{
    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_SetBearCntxtLinkCid:Unsigned or Reserved Eps Bearer ID!");
        return ;
    }

    (NAS_ESM_Entity())->astEpsbCntxtInfo[(ulEpsbId - NAS_ESM_MIN_EPSB_ID)].ulBitCId |= NAS_ESM_BIT_0 << ulCid;

    (NAS_ESM_Entity())->astEpsbCntxtInfo[(ulEpsbId - NAS_ESM_MIN_EPSB_ID)].bitOpCId = NAS_ESM_OP_TRUE;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ClearBearCntxtLinkCid
 Description     : ������ع���CID
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-8-31  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_ClearBearCntxtLinkCid
(
    VOS_UINT32                          ulEpsbId,
    VOS_UINT32                          ulCid
)
{
    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_ClearBearCntxtLinkCid:Unsigned or Reserved Eps Bearer ID!");
        return ;
    }

    ((NAS_ESM_Entity())->astEpsbCntxtInfo[(ulEpsbId - NAS_ESM_MIN_EPSB_ID)].ulBitCId &= (~((VOS_UINT32)(NAS_ESM_BIT_0 << ulCid))));
}

/*****************************************************************************
 Function Name   : NAS_ESM_UpdateBearCntextLinkCid
 Description     : ���³��ع���CID
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-05-31  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_UpdateBearCntextLinkCid
(
    const NAS_ESM_NW_MSG_STRU          *pstDecodedNwMsg,
    VOS_UINT32                          ulEpsbId,
    VOS_UINT32                          ulCid
)
{
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo  = VOS_NULL_PTR;

    /* ���˴��޸Ĳ���TFT��Ϣ�����߳�������Ϊȱʡ���أ���������³��صĹ���CID��Ϣ */
    if ((NAS_ESM_OP_TRUE != pstDecodedNwMsg->bitOpTadInfo)
        || (NAS_ESM_BEARER_TYPE_DEDICATED != NAS_ESM_GetBearCntxtType(ulEpsbId)))
    {
        return ;
    }

    pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCid);

    /* ��TAD��ΪADD_FILTER��REPLACE_FILTER(��ΪADD��REPLACE FILTER���п�������һ��SDF)��
       �򽫳������SDF���������TAD��ΪDELETE_FILTER���Ҵ�SDF������packet filter����ɾ����
       �򽫳������SDFȥ������ */
    switch(pstDecodedNwMsg->stTadInfo.enTftOpType)
    {
        case NAS_ESM_TFT_OP_TYPE_ADD_FILTER:
        case NAS_ESM_TFT_OP_TYPE_REPLACE_FILTER:
            NAS_ESM_SetBearCntxtLinkCid(ulEpsbId, ulCid);
            break;
        case NAS_ESM_TFT_OP_TYPE_DELETE_FILTER:
            if (NAS_ESM_NULL == pstSdfCntxtInfo->ulSdfPfNum)
            {
                NAS_ESM_ClearBearCntxtLinkCid(ulEpsbId, ulCid);
            }
            break;
        default:
            break;
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_GetEpsbCntxtInfoAddr
 Description     : ��ȡ������Ϣ
 Input           : None
 Output          : None
 Return          : NAS_ESM_EPSB_CNTXT_INFO_STRU *

 History         :
    1.lihong00150010      2009-9-1  Draft Enact

*****************************************************************************/
NAS_ESM_EPSB_CNTXT_INFO_STRU * NAS_ESM_GetEpsbCntxtInfoAddr( VOS_UINT32 ulEpsbId )
{
    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_GetEpsbCntxtInfoAddr:Unsigned or Reserved Eps Bearer ID!");
        return &((NAS_ESM_Entity())->astEpsbCntxtInfo[(NAS_ESM_MAX_EPSB_ID - NAS_ESM_MIN_EPSB_ID)]);
    }

    return &((NAS_ESM_Entity())->astEpsbCntxtInfo[(ulEpsbId - NAS_ESM_MIN_EPSB_ID)]);
}
VOS_VOID NAS_ESM_AssignOpId(VOS_UINT32         *pulOpId)
{
    VOS_UINT32                         ulOpId = NAS_ESM_NULL;

    /*��ȡ��ǰ���OpId*/
    ulOpId = NAS_ESM_GetCurMaxOpIdValue();

    /*�����δ�����ֵ��ֱ�Ӽ�1*/
    if( ulOpId < NAS_ESM_MAX_OPID_VALUE )
    {
        (*pulOpId) = ulOpId + 1;
    }
    else/*����Ѿ������ֵ���ӳ�ֵ��ʼ����*/
    {
        (*pulOpId) = NAS_ESM_MIN_OPID_VALUE;
    }

    NAS_ESM_SetCurMaxOpIdValue(*pulOpId);
}
VOS_VOID NAS_ESM_AssignErabmSessionId(VOS_UINT8         *pucOpId)
{
    VOS_UINT8                         ucOpId = NAS_ESM_NULL;

    /*��ȡ��ǰ���OpId*/
    ucOpId = NAS_ESM_GetErabmSessionIdValue();

    /*�����δ�����ֵ��ֱ�Ӽ�1*/
    if( ucOpId < NAS_ESM_PTI_MAX_VALUE )
    {
        (*pucOpId) = ucOpId + 1;
    }
    else/*����Ѿ������ֵ���ӳ�ֵ��ʼ����*/
    {
        (*pucOpId) = NAS_ESM_MIN_OPID_VALUE + 1;
    }
    NAS_ESM_SetErabmSessionIdValue(*pucOpId);
}
VOS_VOID NAS_ESM_AssignPTI(VOS_UINT32         *pulPti)
{
    VOS_UINT8                          ucPti = NAS_ESM_NULL;

    /*��ȡ��ǰ���PTI*/
    ucPti = NAS_ESM_GetCurMaxPTIValue();

    /*�����δ�����ֵ��ֱ�Ӽ�1*/
    if( ucPti < NAS_ESM_PTI_MAX_VALUE)
    {
        (*pulPti) = (VOS_UINT32)(ucPti + 1);
    }
    else/*����Ѿ������ֵ���ӳ�ֵ��ʼ����*/
    {
        (*pulPti) = NAS_ESM_PTI_MIN_VALUE;
    }

    NAS_ESM_SetCurMaxPTIValue((VOS_UINT8)(*pulPti));
}
VOS_VOID  NAS_ESM_AssignGwIpId( VOS_UINT8   *pucGwAuthId)
{
    VOS_UINT8                          ucGwAuthId = NAS_ESM_NULL;

    /*��ȡ��ǰ���PTI*/
    ucGwAuthId = NAS_ESM_GetCurMaxGwAuthIdVaule();

    /*�����δ�����ֵ��ֱ�Ӽ�1*/
    if( ucGwAuthId < NAS_ESM_GWAUTHID_MAX_VALUE)
    {
        *pucGwAuthId = ucGwAuthId + 1;
    }
    else/*����Ѿ������ֵ���ӳ�ֵ��ʼ����*/
    {
        *pucGwAuthId = NAS_ESM_GWAUTHID_MIN_VALUE;
    }

    NAS_ESM_SetCurMaxGwAuthIdVaule(*pucGwAuthId);
}
VOS_UINT32  NAS_ESM_Random()
{
    return PS_RAND(NAS_ESM_MAX_UINT32);
}


VOS_VOID  NAS_ESM_GetGwAuthChallengeValue( VOS_UINT8 *pucValue )
{
    VOS_UINT32                          ulRandTmp   = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt       = NAS_ESM_NULL;
    VOS_UINT32                          ulOffSet    = NAS_ESM_NULL;
    VOS_UINT32                          ulLoopTimes = NAS_ESM_NULL;
    VOS_UINT8                           aucChallengeValue[NAS_ESM_AUTH_CHALLENGE_VALUE_LEN] = {NAS_ESM_NULL};

    if (NAS_ESM_GetMd5Flag() == NAS_ESM_FUN_MD5_CTRL_ON)
    {
        ulLoopTimes = (sizeof(aucChallengeValue) / sizeof(ulRandTmp));
        for (ulCnt = NAS_ESM_NULL; ulCnt < ulLoopTimes; ulCnt++)
        {
            ulRandTmp = NAS_ESM_Random();

            aucChallengeValue[ulOffSet++] = (VOS_UINT8)((ulRandTmp & NAS_ESM_FOURTH_BYTE)
                                                            >> NAS_ESM_MOVEMENT_24_BITS);
            aucChallengeValue[ulOffSet++] = (VOS_UINT8)((ulRandTmp & NAS_ESM_THIRD_BYTE)
                                                            >> NAS_ESM_MOVEMENT_16_BITS);
            aucChallengeValue[ulOffSet++] = (VOS_UINT8)((ulRandTmp & NAS_ESM_HIGH_BYTE)
                                                            >> NAS_ESM_MOVEMENT_8_BITS);
            aucChallengeValue[ulOffSet++] = (VOS_UINT8)(ulRandTmp & NAS_ESM_LOW_BYTE);
        }

        NAS_ESM_MEM_CPY(    pucValue,
                            aucChallengeValue,
                            NAS_ESM_AUTH_CHALLENGE_VALUE_LEN);
    }
    else
    {
        NAS_ESM_MEM_CPY(    pucValue,
                            NAS_ESM_GetChallegeValue(),
                            NAS_ESM_AUTH_CHALLENGE_VALUE_LEN);
    }

    return;
}


VOS_VOID  NAS_ESM_GetGwAuthRpsValue
(
    const VOS_UINT8                    *pucPwd,
    VOS_UINT8                           ucPwdLen,
    VOS_UINT8                           ucId,
    const VOS_UINT8                    *pucChlgVal,
    VOS_UINT8                          *pucRspVal
)
{
    VOS_CHAR                            acMd5Input[NAS_ESM_MAX_MD5_INPUT_PARA_LEN]   = {NAS_ESM_NULL};
    VOS_UCHAR                       	aucRspValue[NAS_ESM_AUTH_RESPONSE_VALUE_LEN] = {NAS_ESM_NULL};
    VOS_UCHAR                           ucLen = NAS_ESM_NULL;

    if (NAS_ESM_GetMd5Flag() == NAS_ESM_FUN_MD5_CTRL_ON)
    {
        /* The Response Value is the one-way hash calculated over a stream of
           octets consisting of the Identifier, followed by (concatenated
           with) the "secret", followed by (concatenated with) the Challenge
           Value. */
        acMd5Input[0] = (VOS_CHAR)ucId;

        NAS_ESM_MEM_CPY(    &acMd5Input[1],
                            pucPwd,
                            ucPwdLen);

        NAS_ESM_MEM_CPY(    &acMd5Input[1 + ucPwdLen],
                            pucChlgVal,
                            NAS_ESM_AUTH_CHALLENGE_VALUE_LEN);

        ucLen = 1 + ucPwdLen + NAS_ESM_AUTH_CHALLENGE_VALUE_LEN;

        /* ����MD5�㷨����challege respose value */
        PS_MD5String(acMd5Input, aucRspValue, ucLen);

        NAS_ESM_MEM_CPY(    pucRspVal,
                            aucRspValue,
                            NAS_ESM_AUTH_RESPONSE_VALUE_LEN);
    }
    else
    {
        NAS_ESM_MEM_CPY(    pucRspVal,
                            NAS_ESM_GetChallegeRspValue(),
                            NAS_ESM_AUTH_RESPONSE_VALUE_LEN);
    }

    return;
}

/*****************************************************************************
 Function Name   : NAS_ESM_QueryPfInTft
 Description     : ��TFT��Ϣ�в���ָ����PF
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-8-28  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_ESM_QueryPfInTft
(
    VOS_UINT8                           ucPacketFilterID,
    const APP_ESM_TFT_INFO_STRU        *pstCntxtTftInfo,
    VOS_UINT32                          ulPfNum,
    VOS_UINT32                         *pulIndex
)
{
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;
    const NAS_ESM_CONTEXT_TFT_STRU     *pstTmpTFT       = VOS_NULL_PTR;

    /* ����TFT�е�����PF��Ϣ������ָ����PF ID */
    for (ulCnt = NAS_ESM_NULL; ulCnt < ulPfNum; ulCnt++)
    {
        pstTmpTFT = &(pstCntxtTftInfo[ulCnt]);
        if (ucPacketFilterID == pstTmpTFT->ucPacketFilterId)
        {
            *pulIndex = ulCnt;

            return NAS_ESM_SUCCESS;
        }
    }

    return NAS_ESM_FAILURE;
}

/*****************************************************************************
 Function Name  : NAS_ESM_QueryEpsbCntxtTblByCid
 Discription    : ����CID��ѯEpsbCntxtTbl�����õ���Ӧ���غ�
 Input          : VOS_UINT32          ulCId
                  VOS_UINT32         *pulEpsId
 Output         : VOS_UINT32         *pulEpsId
 Return         : VOS_UINT32(���û�в鵽����SM_FAILURE,���򷵻�SM_SUCCESS)
 History:
      1.ף��ǿ      2009-02-10  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_QueryEpsbCntxtTblByCid(VOS_UINT32  ulCId,
                                       VOS_UINT32  *pulEpsbId)
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    NAS_ESM_EPSB_CNTXT_INFO_STRU        *pstEpsbCntxtInfo   = VOS_NULL_PTR;

    /*���� StateInfoTbl*/
    for( ulCnt = NAS_ESM_MIN_EPSB_ID; ulCnt <= NAS_ESM_MAX_EPSB_ID; ulCnt++ )
    {
        /*����EpsIdȡ��������Ϣ*/
        pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulCnt);

        /*�˳����Ѿ�����CID*/
        if( NAS_ESM_OP_TRUE == pstEpsbCntxtInfo->bitOpCId )
        {
            /*�˳�����Ϣ�е�CID����ָ��CID*/
            if( NAS_ESM_OP_TRUE ==((pstEpsbCntxtInfo->ulBitCId >> ulCId )&NAS_ESM_BIT_0))
            {
                /*��ʱ�Ѿ�����EPSbID,���ش˳���ID*/
                (*pulEpsbId) = ulCnt;
                return NAS_ESM_SUCCESS;
            }
        }
    }

    /*���û���ҵ�������SM_FAILURE*/
    return NAS_ESM_FAILURE;
}

/*****************************************************************************
 Function Name  : NAS_ESM_QueryEpsbCntxtTblByEpsbId
 Discription    : ����EpsbCntxtTbl������û�й���CID��������򷵻ظ�CID��
 Input          : VOS_UINT32          ulEpsbId
 Output         : VOS_UINT32         *ulCId
 Return         : VOS_UINT32(���û�в鵽����SM_FAILURE,���򷵻�SM_SUCCESS)
 History:
      1.ף��ǿ      2009-02-10  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_QueryEpsbCntxtTblByEpsbId(VOS_UINT32  ulEpsbId,
                                       VOS_UINT32  *pulCId)
{
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo = VOS_NULL_PTR;
    VOS_UINT32                          ulCnt            = 0;
    VOS_UINT32                          ulTmpCid         = 0;

    /*��ȡ��ӦEpsbCntxtTbl����ַ*/
    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);
    ulTmpCid = pstEpsbCntxtInfo->ulBitCId;

    /*���EpsbCntxtTbl���У�û�й�����CID���򷵻ش�����Ϣ*/
    if(NAS_ESM_OP_FALSE == pstEpsbCntxtInfo->bitOpCId)
    {
        return NAS_ESM_FAILURE;
    }

    for(ulCnt = NAS_ESM_MIN_CID; ulCnt <= NAS_ESM_MAX_CID; ulCnt++)
    {
        if(NAS_ESM_OP_TRUE ==((ulTmpCid >> ulCnt )& NAS_ESM_BIT_0))
        {
            /*��ʱ�ȷ��ع�����ֵ��С��CID*/
            (*pulCId) = ulCnt;
            return NAS_ESM_SUCCESS;
        }
    }

    /*���û�й���CID������SM_FAILURE*/
    return NAS_ESM_FAILURE;
}

/*****************************************************************************
 Function Name   : NAS_ESM_QueryFirstSdfIdByEpsbId
 Description     : ���ݳ��غ�,���ҵ�һ������������SDF��
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.sunbing49683      2009-4-16  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_QueryFirstSdfIdByEpsbId
(
    VOS_UINT32                          ulEpsbId,
    VOS_UINT32                         *pulCid
)
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo     = VOS_NULL_PTR;

    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_QueryFirstSdfIdByEpsbId:Unsigned or Reserved Eps Bearer ID!");
        return NAS_ESM_FAILURE;
    }

    for(ulCnt = NAS_ESM_MIN_CID; ulCnt <= NAS_ESM_MAX_CID; ulCnt++)
    {
        pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCnt);
        if(ulEpsbId == pstSdfCntxtInfo->ulEpsbId)
        {
            /*��ʱ�ȷ��ع�����ֵ��С��CID*/
            (*pulCid) = ulCnt;
            return NAS_ESM_SUCCESS;
        }
    }

    return NAS_ESM_FAILURE;
}

/*****************************************************************************
 Function Name   : NAS_ESM_QuerySdfIdByEpsbIdAndPfId
 Description     : ���ݳ��غź�PF�ţ�����SDF��
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.sunbing49683      2009-4-16  Draft Enact
    2.lihong00150010    2009-7-14   Modify

*****************************************************************************/
VOS_UINT32 NAS_ESM_QuerySdfIdByEpsbIdAndPfId
(
    VOS_UINT8                           ucEpsbId,
    VOS_UINT8                           ucPacketFilterId,
    VOS_UINT32                         *pulCid
)
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt2              = NAS_ESM_NULL;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo     = VOS_NULL_PTR;

    /* ���ݳ��غź�PF�ţ�����SDF�� */
    for(ulCnt = NAS_ESM_MIN_CID; ulCnt <= NAS_ESM_MAX_CID; ulCnt++)
    {
        pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCnt);

        /* �жϳ��غ��Ƿ���ͬ */
        if(ucEpsbId == pstSdfCntxtInfo->ulEpsbId)
        {
            for(ulCnt2 = 0; ulCnt2 < pstSdfCntxtInfo->ulSdfPfNum; ulCnt2++)
            {
                /* �ж�PF���Ƿ���ͬ */
                if(ucPacketFilterId == pstSdfCntxtInfo->astSdfPfInfo[ulCnt2].ucNwPacketFilterId)
                {
                    *pulCid = ulCnt;
                    return NAS_ESM_SUCCESS;
                }
            }
        }
    }

    return NAS_ESM_FAILURE;
}
NAS_ESM_QUERY_CID_RSLT_ENUM_UINT8  NAS_ESM_GetQuerySdfIdResult
(
     VOS_UINT32                         ulFindNum,
     VOS_UINT32                         ulPacketFilterNum,
     PS_BOOL_ENUM_UINT8                 enIsCidEqual
)
{
    /* ȫ���ҵ������ */
    if(ulFindNum == ulPacketFilterNum)
    {
        if(PS_TRUE == enIsCidEqual)
        {
            return NAS_ESM_QUERY_CID_RSLT_ALL_FOUND_EQUAL_CID;
        }
        else
        {
            return NAS_ESM_QUERY_CID_RSLT_ALL_FOUND_UNEQUAL_CID;
        }
    }

    /* ȫ��û�ҵ������ */
    if(NAS_ESM_NULL == ulFindNum)
    {
        return NAS_ESM_QUERY_CID_RSLT_ALL_UNFOUND;
    }

    /* �����ҵ������ */
    if(PS_TRUE == enIsCidEqual)
    {
        return NAS_ESM_QUERY_CID_RSLT_PART_FOUND_EQUAL_CID;
    }
    else
    {
        return NAS_ESM_QUERY_CID_RSLT_PART_FOUND_UNEQUAL_CID;
    }
}
NAS_ESM_QUERY_CID_RSLT_ENUM_UINT8  NAS_ESM_QuerySdfId
(
    const NAS_ESM_NW_MSG_STRU          *pstEsmNwMsgIE,
    VOS_UINT32                         *pulCid
)
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    VOS_UINT32                          ulFindNum           = NAS_ESM_NULL;
    VOS_UINT32                          ulCid               = NAS_ESM_ILL_CID;
    VOS_UINT32                          ulLastCid           = NAS_ESM_ILL_CID;
    VOS_UINT8                           ucPacketFilter      = NAS_ESM_NULL;
    PS_BOOL_ENUM_UINT8                  enIsCidEqual        = PS_TRUE;
    VOS_UINT32                          ulRslt              = NAS_ESM_NULL;

    /* �ж��Ƿ�Ϊȱʡ����*/
    if ((PS_TRUE == NAS_ESM_IsDefaultEpsBearerType(NAS_ESM_GetBearCntxtType(pstEsmNwMsgIE->ucEpsbId)))
      ||(NAS_ESM_OP_TRUE != pstEsmNwMsgIE->bitOpTadInfo))
    {
        /* ���ݳ��غ�,���ҵ�һ������������SDF�� */
        ulRslt = NAS_ESM_QueryFirstSdfIdByEpsbId(pstEsmNwMsgIE->ucEpsbId, pulCid);
        if(NAS_ESM_SUCCESS == ulRslt)
        {
            return NAS_ESM_QUERY_CID_RSLT_ALL_FOUND_EQUAL_CID;
        }
        else
        {
            return NAS_ESM_QUERY_CID_RSLT_ALL_UNFOUND;
        }
    }

    for(ulCnt = 0; ulCnt < pstEsmNwMsgIE->stTadInfo.ucSdfPfNum; ulCnt++)
    {
        ucPacketFilter = pstEsmNwMsgIE->stTadInfo.astSdfPfInfo[ulCnt].ucNwPacketFilterId;

        /* ���ݳ��غź͵�ǰPF��������CID */
        ulRslt = NAS_ESM_QuerySdfIdByEpsbIdAndPfId( pstEsmNwMsgIE->ucEpsbId,
                                                    ucPacketFilter,
                                                    &ulCid);

        if(NAS_ESM_SUCCESS == ulRslt)
        {
            ulFindNum++;
            if(NAS_ESM_ILL_CID == ulLastCid)
            {
                /* ���浱ǰ�ҵ���CID */
                ulLastCid = ulCid;
                *pulCid = ulCid;
            }
            else
            {
                /* ���ǰ�������ҵ���CID��ͬ,������enIsCidEqualΪPS_FALSE */
                if(ulLastCid != ulCid)
                {
                    enIsCidEqual = PS_FALSE;
                }
            }
        }
    }

    return NAS_ESM_GetQuerySdfIdResult( ulFindNum,
                                        pstEsmNwMsgIE->stTadInfo.ucSdfPfNum,
                                        enIsCidEqual);

}
VOS_UINT32  NAS_ESM_GetLinkCidByActiveCid
(
    VOS_UINT32                          ulCid
)

{
    VOS_UINT32                          ulEpsbId         = NAS_ESM_NULL;
    VOS_UINT32                          ulLinkCid        = NAS_ESM_ILL_CID;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo = VOS_NULL_PTR;

    /* ��Cid��Ӧ�ĳ��ص�EpsbId */
    if (NAS_ESM_FAILURE == NAS_ESM_QueryEpsbCntxtTblByCid(ulCid,&ulEpsbId))
    {
        return NAS_ESM_ILL_CID;
    }

    if (PS_TRUE == NAS_ESM_IsDefaultEpsBearerType(NAS_ESM_GetBearCntxtType(ulEpsbId)))
    {
        return NAS_ESM_ILL_CID;
    }

    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

    /* ���ҹ������ض�Ӧ��Cid*/
    if (NAS_ESM_FAILURE == NAS_ESM_QueryEpsbCntxtTblByEpsbId(pstEpsbCntxtInfo->ulLinkedEpsbId,
                                                             &ulLinkCid))
    {
        return NAS_ESM_ILL_CID;
    }

    return ulLinkCid;
}



/*****************************************************************************
 Function Name  : NAS_ESM_GetStateTblIndexByCid
 Discription    : ����CID������һ��StateInfoTbl���ڵ�;
 Input          : VOS_UINT32        ulCidRecv
 Output         : VOS_UINT32        *ulStateId
 Return         : VOS_UINT32(���û�в鵽����SM_FAILURE,���򷵻�SM_SUCCESS)
 History:
      1.ף��ǿ      2009-02-5  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_GetStateTblIndexByCid(VOS_UINT32 ulCidRecv,VOS_UINT32 *ulStateId)
{
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStatTblnfo   = VOS_NULL_PTR;

    /*���� StateInfoTbl*/
    for(ulCnt = 0; ulCnt < NAS_ESM_MAX_STATETBL_NUM; ulCnt++)
    {
        pstStatTblnfo = NAS_ESM_GetStateTblAddr(ulCnt);

        /*���CID�Ѿ���STATE����*/
        if(ulCidRecv == pstStatTblnfo->ulCid)
        {
            return NAS_ESM_FAILURE;
        }
    }

    /*���䵽ulStateId*/
    for(ulCnt = 0; ulCnt < NAS_ESM_MAX_STATETBL_NUM; ulCnt++)
    {
        pstStatTblnfo = NAS_ESM_GetStateTblAddr(ulCnt);

        /*���STATE_INFO_TBL���ж�ӦCIDΪ��Чֵ*/
        if(NAS_ESM_ILL_CID == pstStatTblnfo->ulCid)
        {
            /*ulStateId����ɹ�*/
            (*ulStateId) = ulCnt;
            pstStatTblnfo->ulCid = ulCidRecv;

            NAS_ESM_NORM_LOG("*********************");
            NAS_ESM_LOG1("GET StateTblIndex : ", *ulStateId);
            NAS_ESM_NORM_LOG("*********************");

            return NAS_ESM_SUCCESS;
        }
    }

    /*����Ҳ������е�ulStateId*/
    return NAS_ESM_FAILURE;

}

/*****************************************************************************
 Function Name  : NAS_ESM_QueryStateTblIndexByCid
 Discription    : ����CID����ѯStateInfoTbl��;
 Input          : VOS_UINT32 ulCidRecv
 Output         : VOS_UINT32         *pulEpsId
 Return         : VOS_UINT32(���û�в鵽����SM_FAILURE,���򷵻�SM_SUCCESS)

 History:
      1.ף��ǿ      2009-02-5  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_QueryStateTblIndexByCid(VOS_UINT32 ulCidRecv,VOS_UINT32 *ulStateId)
{
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStatTblnfo   = VOS_NULL_PTR;

    /*���� StateInfoTbl*/
    for(ulCnt = 0; ulCnt < NAS_ESM_MAX_STATETBL_NUM; ulCnt++)
    {
        pstStatTblnfo = NAS_ESM_GetStateTblAddr(ulCnt);

        /*���CID�Ѿ���STATE_INFO_TBL����*/
        if(ulCidRecv == pstStatTblnfo->ulCid)
        {
            /*����StateInfoTbl������*/
            (*ulStateId) = ulCnt;
            return NAS_ESM_SUCCESS;
        }
    }

    return NAS_ESM_FAILURE;

}

/*****************************************************************************
 Function Name  : NAS_ESM_QueryStateTblIndexByPti
 Discription    : ����ucPti������һ��StateId;
 Input          : VOS_UINT8         ucPti
 Output         : VOS_UINT32         *pulEpsId
 Return         : VOS_UINT32(���û�в鵽����SM_FAILURE,���򷵻�SM_SUCCESS)

 History:
      1.ף��ǿ      2009-02-10  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_QueryStateTblIndexByPti(VOS_UINT8 ucPti,VOS_UINT32 *ulStateId)
{
    VOS_UINT8                           ucCurrPti           = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStatTblnfo       = VOS_NULL_PTR;

    /*���� StateInfoTbl*/
    for(ulCnt = 0; ulCnt < NAS_ESM_MAX_STATETBL_NUM; ulCnt++)
    {
        pstStatTblnfo = NAS_ESM_GetStateTblAddr(ulCnt);
        ucCurrPti = pstStatTblnfo->stNwMsgRecord.ucPti;

        /*�����STATE�����ҵ���ӦPTIֵ*/
        if(ucPti == ucCurrPti)
        {
            /*���ض�ӦState��������*/
            (*ulStateId) = ulCnt;
            return NAS_ESM_SUCCESS;
        }
    }

    /*û�ж�Ӧ�Ľڵ�*/
    return NAS_ESM_FAILURE;

}

/*****************************************************************************
 Function Name  : NAS_ESM_QueryStateTblIndexByEpsbId
 Discription    : ���ݳ��غ�EpsbId,��ѯStateTbl��������StateTbl��������
 Input          : VOS_UINT32        ulEpsbId
 Output         : VOS_UINT32       *ulStateId
 Return         : VOS_UINT32(���û�в鵽����SM_FAILURE,���򷵻�SM_SUCCESS)

 History:
      1.ף��ǿ      2009-02-10  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_QueryStateTblIndexByEpsbId(VOS_UINT32 ulEpsbId,VOS_UINT32 *ulStateId)
{
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStatTblnfo   = VOS_NULL_PTR;

    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_QueryStateTblIndexByEpsbId:Unsigned or Reserved Eps Bearer ID!");
        return NAS_ESM_FAILURE;
    }

    for(ulCnt = 0; ulCnt < NAS_ESM_MAX_STATETBL_NUM; ulCnt++)
    {
        pstStatTblnfo = NAS_ESM_GetStateTblAddr(ulCnt);

        /*�����STATE�����ж�ӦEpsbId*/
        if(ulEpsbId == pstStatTblnfo->ulEpsbId)
        {
            /*����StateTbl����*/
            (*ulStateId) = ulCnt;

            return NAS_ESM_SUCCESS;
        }
    }

    return NAS_ESM_FAILURE;
}


/*****************************************************************************
 Function Name   : NAS_ESM_GetBuffItemAddr
 Description     : ��ȡ�����¼��ַ
 Input           : ulIndex------------------------��ʱ��������
 Output          : None
 Return          : VOS_VOID*

 History         :
    1.lihong00150010      2010-4-23  Draft Enact

*****************************************************************************/
VOS_VOID* NAS_ESM_GetBuffItemAddr
(
    NAS_ESM_BUFF_ITEM_TYPE_ENUM_UINT8   enEsmBuffType,
    VOS_UINT32                          ulIndex
)
{
    NAS_ESM_BUFF_MANAGE_INFO_STRU      *pstEsmBuffManInfo   = NAS_ESM_GetEsmBufferManageInfoAddr();
    VOS_VOID                           *pBuffItem           = VOS_NULL_PTR;

    if (enEsmBuffType == NAS_ESM_BUFF_ITEM_TYPE_PTI)
    {
        pBuffItem = pstEsmBuffManInfo->paPtiBuffer[ulIndex];
    }
    else
    {
        pBuffItem = pstEsmBuffManInfo->paAttBuffer[ulIndex];
    }

    return pBuffItem;
}

/*****************************************************************************
 Function Name   : NAS_ESM_GetPtiBuffItemTimer
 Description     : ��ȡESM�����¼������ʱ��
 Input           : ulIndex------------------------��ʱ��������
 Output          : None
 Return          : NAS_ESM_TIMER_STRU*

 History         :
    1.lihong00150010      2010-1-29     Draft Enact
    2.lihong00150010      2010-04-23    Modify

*****************************************************************************/
NAS_ESM_TIMER_STRU* NAS_ESM_GetPtiBuffItemTimer
(
    VOS_UINT32                          ulIndex
)
{
    NAS_ESM_PTI_BUFF_ITEM_STRU         *pstEsmPtiBuffItem = VOS_NULL_PTR;

    /* ��ȡ�����¼ */
    pstEsmPtiBuffItem = (NAS_ESM_PTI_BUFF_ITEM_STRU*)
                                NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulIndex);
    if (pstEsmPtiBuffItem == VOS_NULL_PTR)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_GetPtiBuffItemTimer:WARN:Get buffer item failed!");
        return VOS_NULL_PTR;
    }

    return &pstEsmPtiBuffItem->stTimerInfo;
}

/*****************************************************************************
 Function Name   : NAS_ESM_GetTimerLen
 Description     : ��ȡ��ʱ��ʱ��
 Input           : enTimerType------------------��ʱ������
                   ulIndex----------------------��ʱ��������
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_ESM_GetTimerLen
(
    NAS_ESM_TIMER_PARA_ENUM_UINT8       enTimerType
)
{
    VOS_UINT32                  ulTimerLen   = NAS_ESM_NULL;

    /*���ݶ�ʱ����ͬ���ͣ���ʱ��ʱ����ͬ*/
    switch( enTimerType )
    {
        case TI_NAS_ESM_T3482:
            ulTimerLen = TI_NAS_ESM_T3482_TIMER_LEN;
            break;
        case TI_NAS_ESM_T3492:
            ulTimerLen = TI_NAS_ESM_T3492_TIMER_LEN;
            break;
        case TI_NAS_ESM_T3480:
            ulTimerLen = TI_NAS_ESM_T3480_TIMER_LEN;
            break;
        case TI_NAS_ESM_T3481:
            ulTimerLen = TI_NAS_ESM_T3481_TIMER_LEN;
            break;
        case TI_NAS_ESM_WAIT_APP_CNF:
            ulTimerLen = TI_NAS_ESM_WAIT_APP_CNF_TIMER_LEN;
            break;
        case TI_NAS_ESM_WAIT_PTI_INVALID:
            ulTimerLen = TI_NAS_ESM_WAIT_PTI_INVALID_TIMER_LEN;
            break;
        case TI_NAS_ESM_ATTACH_BEARER_REEST:
            ulTimerLen = NAS_ESM_GetAttachBearerReestTimerLen();
            break;
        case TI_NAS_ESM_REL_NON_EMC_BEARER:
            ulTimerLen = TI_NAS_ESM_REL_NON_EMC_BEARER_TIMER_LEN;
            break;
        default :
            break;
    }

    return ulTimerLen;
}

/*****************************************************************************
 Function Name   : NAS_ESM_GetTimer
 Description     : ��ȡ��ʱ��
 Input           : enTimerType------------------��ʱ������
                   ulIndex----------------------��ʱ��������
 Output          : None
 Return          : NAS_ESM_TIMER_STRU*

 History         :
    1.lihong00150010      2010-1-29  Draft Enact

*****************************************************************************/
NAS_ESM_TIMER_STRU*  NAS_ESM_GetTimer
(
    VOS_UINT32                          ulIndex,
    NAS_ESM_TIMER_PARA_ENUM_UINT8       enTimerType
)
{
    NAS_ESM_TIMER_STRU         *pstTimerInfo = VOS_NULL_PTR;

    /*���ݶ�ʱ����ͬ���ͣ���ȡ��ʱ��*/
    switch( enTimerType )
    {
        case TI_NAS_ESM_T3482:
        case TI_NAS_ESM_T3492:
        case TI_NAS_ESM_T3480:
        case TI_NAS_ESM_T3481:
        case TI_NAS_ESM_WAIT_APP_CNF:
            pstTimerInfo = NAS_ESM_GetStateTblTimer(ulIndex);
            break;
        case TI_NAS_ESM_WAIT_PTI_INVALID:
            pstTimerInfo = NAS_ESM_GetPtiBuffItemTimer(ulIndex);
            break;
        case TI_NAS_ESM_ATTACH_BEARER_REEST:
            pstTimerInfo = &(NAS_ESM_Entity()->stAttachBearerReestInfo.stTimerInfo);
            break;
        case TI_NAS_ESM_REL_NON_EMC_BEARER:
            pstTimerInfo = &(NAS_ESM_Entity()->stDeferRelNonEmcTimerInfo);
            break;
        default :
            break;
    }

    return pstTimerInfo;
}

/*****************************************************************************
 Function Name   : NAS_ESM_PrintTimeStartInfo
 Description     : ��ӡ��ʱ��������Ϣ
 Input           : enTimerType------------------��ʱ������
                   ulIndex----------------------��ʱ��������
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-1-29  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_PrintTimeStartInfo
(
    VOS_UINT32                          ulIndex,
    NAS_ESM_TIMER_PARA_ENUM_UINT8       enTimerType
)
{
    /*���ݶ�ʱ����ͬ���ͣ���ӡ��Ӧ��Ϣ*/
    switch( enTimerType )
    {
        case TI_NAS_ESM_T3482:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStartInfo:NORM:SM TimerStart(ulIndex): TI_NAS_ESM_T3482", ulIndex);
            break;
        case TI_NAS_ESM_T3492:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStartInfo:NORM:SM TimerStart(ulIndex): TI_NAS_ESM_T3492", ulIndex);
            break;
        case TI_NAS_ESM_T3480:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStartInfo:NORM:SM TimerStart(ulIndex): TI_NAS_ESM_T3480", ulIndex);
            break;
        case TI_NAS_ESM_T3481:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStartInfo:NORM:SM TimerStart(ulIndex): TI_NAS_ESM_T3481", ulIndex);
            break;
        case TI_NAS_ESM_WAIT_APP_CNF:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStartInfo:NORM:SM TimerStart(ulIndex): TI_NAS_ESM_WAIT_APP_CNF", ulIndex);
            break;
        case TI_NAS_ESM_WAIT_PTI_INVALID:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStartInfo:NORM:SM TimerStart(ulIndex): TI_NAS_ESM_WAIT_PTI_INVALID", ulIndex);
            break;
        case TI_NAS_ESM_ATTACH_BEARER_REEST:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStartInfo:NORM:SM TimerStart(ulIndex): TI_NAS_ESM_ATTACH_BEARER_REEST", ulIndex);
            break;
        case TI_NAS_ESM_REL_NON_EMC_BEARER:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStartInfo:NORM:SM TimerStart(ulIndex): TI_NAS_ESM_REL_NON_EMC_BEARER", ulIndex);
            break;
        default :
            break;
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_PrintTimeStopInfo
 Description     : ��ӡ��ʱ���ر���Ϣ
 Input           : enTimerType------------------��ʱ������
                   ulIndex----------------------��ʱ��������
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-1-29  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_PrintTimeStopInfo
(
    VOS_UINT32                          ulIndex,
    NAS_ESM_TIMER_PARA_ENUM_UINT8       enTimerType
)
{
    /*���ݶ�ʱ����ͬ���ͣ���ӡ��Ӧ��Ϣ*/
    switch(enTimerType)
    {
        case TI_NAS_ESM_T3482:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStopInfo:NORM:SM TimerStop(ulIndex): TI_NAS_ESM_T3482", ulIndex);
            break;
        case TI_NAS_ESM_T3492:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStopInfo:NORM:SM TimerStop(ulIndex): TI_NAS_ESM_T3492", ulIndex);
            break;
        case TI_NAS_ESM_T3480:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStopInfo:NORM:SM TimerStop(ulIndex): TI_NAS_ESM_T3480", ulIndex);
            break;
        case TI_NAS_ESM_T3481:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStopInfo:NORM:SM TimerStop(ulIndex): TI_NAS_ESM_T3481", ulIndex);
            break;
        case TI_NAS_ESM_WAIT_APP_CNF:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStopInfo:NORM:SM TimerStop(ulIndex): TI_NAS_ESM_WAIT_APP_CNF", ulIndex);
            break;
        case TI_NAS_ESM_WAIT_PTI_INVALID:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStopInfo:NORM:SM TimerStop(ulIndex): TI_NAS_ESM_WAIT_PTI_INVALID", ulIndex);
            break;
        case TI_NAS_ESM_ATTACH_BEARER_REEST:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStopInfo:NORM:SM TimerStop(ulIndex): TI_NAS_ESM_ATTACH_BEARER_REEST", ulIndex);
            break;
        case TI_NAS_ESM_REL_NON_EMC_BEARER:
            NAS_ESM_LOG1("NAS_ESM_PrintTimeStopInfo:NORM:SM TimerStop(ulIndex): TI_NAS_ESM_REL_NON_EMC_BEARER", ulIndex);
            break;
        default :
            break;
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_IsTimerNameValid
 Description     : �ж϶�ʱ�����Ƿ�Ϸ�
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_ESM_IsTimerNameValid
(
    VOS_UINT32                          ulIndex,
    NAS_ESM_TIMER_PARA_ENUM_UINT8       enTimerType
)
{
    switch(enTimerType)
    {
        case TI_NAS_ESM_T3482:
        case TI_NAS_ESM_T3492:
        case TI_NAS_ESM_T3480:
        case TI_NAS_ESM_T3481:
        case TI_NAS_ESM_WAIT_APP_CNF:
        {
            if (ulIndex < NAS_ESM_MAX_STATETBL_NUM)
            {
                return PS_TRUE;
            }

            break;
        }
        case TI_NAS_ESM_WAIT_PTI_INVALID:
        {
            if (ulIndex < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM)
            {
                return PS_TRUE;
            }

            break;
        }
        case TI_NAS_ESM_ATTACH_BEARER_REEST:
        {
            return PS_TRUE;
        }
        case TI_NAS_ESM_REL_NON_EMC_BEARER:
        {
            return PS_TRUE;
        }
        default:
            break;
    }

    return PS_FALSE;
}


VOS_VOID NAS_ESM_TimerStart
(
    VOS_UINT32                          ulIndex,
    NAS_ESM_TIMER_PARA_ENUM_UINT8       enTimerType
)
{
    VOS_UINT32                  ulTimerLen   = NAS_ESM_NULL;
    NAS_ESM_TIMER_STRU         *pstTimerInfo = VOS_NULL_PTR;

    /*��ulIndex�Ϸ����ж�*/
    if (PS_FALSE == NAS_ESM_IsTimerNameValid(ulIndex, enTimerType))
    {
        /*��ӡ�쳣��Ϣ*/
        NAS_ESM_WARN_LOG("NAS_ESM_TimerStart: WARN: Input Para(ulIndex) err !");
        return;
    }

    /*������Ϣ��Ӧ�������źͶ�ʱ������,��ȡ������Ķ�ʱ��*/
    pstTimerInfo = NAS_ESM_GetTimer(ulIndex, enTimerType);
    if (pstTimerInfo == VOS_NULL_PTR)
    {
        /*��ӡ�쳣��Ϣ*/
        NAS_ESM_ERR_LOG("NAS_ESM_TimerStart:ERROR: Get Timer failed.");
        return;
    }

    /*�ж϶�ʱ���Ƿ�򿪣��Ѵ���ر�*/
    if(VOS_NULL_PTR != pstTimerInfo->hTimer)
    {
        /*�ر�ʧ�ܣ��򱨾�����*/
        if (VOS_OK != PS_STOP_REL_TIMER(&(pstTimerInfo->hTimer)))
        {
            /*��ӡ�쳣��Ϣ*/
            NAS_ESM_WARN_LOG("NAS_ESM_TimerStart:WARN: stop reltimer error!");
            return;
        }

        /*��ӡ�쳣��Ϣ*/
        NAS_ESM_LOG2("(TimerType) Timer not close!",pstTimerInfo->enPara, ulIndex);
    }

    /*���ݶ�ʱ����ͬ���ͣ���ʱ����Ϣ��ͬ*/
    ulTimerLen = NAS_ESM_GetTimerLen(enTimerType);
    if (ulTimerLen == NAS_ESM_NULL)
    {
        /*��ӡ�쳣��Ϣ*/
        NAS_ESM_ERR_LOG("NAS_ESM_TimerStart:ERROR: start unreasonable reltimer.");
        return;
    }

    /*�趨��ʱ��NAME,enTimerType�趨��ʱ��Para����ʧ���򱨾�����*/
    if (VOS_OK !=\
            PS_START_REL_TIMER(&(pstTimerInfo->hTimer),PS_PID_ESM,\
                                ulTimerLen,ulIndex,\
                                (VOS_UINT32)enTimerType,VOS_RELTIMER_NOLOOP))
    {
          /*��ӡ�쳣��Ϣ*/
          NAS_ESM_WARN_LOG("NAS_ESM_TimerStart:WARN: start reltimer error!");
          return;
    }

    /*���¶�ʱ��������Ϣ�Ͷ�ʱ�����*/
    pstTimerInfo->enPara     = enTimerType;


    /*������ǰ�Ķ�ʱ����Ϣ */
    NAS_ESM_SndOmEsmTimerStatus(NAS_ESM_TIMER_RUNNING, enTimerType, ulTimerLen);


    /* ��ӡ��ʱ��������Ϣ */
    NAS_ESM_PrintTimeStartInfo(ulIndex, enTimerType);
}
VOS_VOID NAS_ESM_TimerStop
(
    VOS_UINT32                          ulIndex,
    NAS_ESM_TIMER_PARA_ENUM_UINT8       enTimerType
)
{
    NAS_ESM_TIMER_STRU        *pstTimerInfo = VOS_NULL_PTR;
    VOS_UINT32                 ulTimerRemainLen;

    /*��ulIndex�Ϸ����ж�*/
    if (PS_FALSE == NAS_ESM_IsTimerNameValid(ulIndex, enTimerType))
    {
        /*��ӡ�쳣��Ϣ*/
        NAS_ESM_WARN_LOG("NAS_ESM_TimerStop: WARN: Input Para(ulIndex) err !");
        return;
    }

    /*������Ϣ��Ӧ�������źͶ�ʱ������,��ȡ������Ķ�ʱ��*/
    pstTimerInfo = NAS_ESM_GetTimer(ulIndex, enTimerType);
    if (pstTimerInfo == VOS_NULL_PTR)
    {
        /*��ӡ�쳣��Ϣ*/
        NAS_ESM_ERR_LOG("NAS_ESM_TimerStop:ERROR:Get Timer failed.");
        return;
    }

    /*��ʱ�����ڴ�״̬����رգ����򣬺���*/
    if(VOS_NULL_PTR != pstTimerInfo->hTimer)
    {
        /* ��ȡ��ǰ��ʱ����ʣ��ʱ�� */
        if (VOS_OK != VOS_GetRelTmRemainTime(&(pstTimerInfo->hTimer), &ulTimerRemainLen ))
        {
            ulTimerRemainLen = 0;
        }

        /*�ر�ʧ�ܣ��򱨾�����*/
        if (VOS_OK != PS_STOP_REL_TIMER(&(pstTimerInfo->hTimer)))
        {
            /*��ӡ�쳣��Ϣ*/
            NAS_ESM_WARN_LOG("NAS_ESM_TimerStop:WARN: stop reltimer error!");
            return;
        }

        /*���¶�ʱ����ʱ����*/
        pstTimerInfo->ucExpireTimes = 0;


        /*������ǰ�Ķ�ʱ����Ϣ */
        NAS_ESM_SndOmEsmTimerStatus(NAS_ESM_TIMER_STOPED, enTimerType, ulTimerRemainLen);

        /* ��ӡ��ʱ���ر���Ϣ */
        NAS_ESM_PrintTimeStopInfo(ulIndex, enTimerType);
    }
}
VOS_UINT32  NAS_ESM_GetTimerMaxExpNum(
                                        NAS_ESM_TIMER_PARA_ENUM_UINT8 enTimerType )
{
    VOS_UINT32                          ulTimerMaxExpNum = 0;

    switch( enTimerType )
    {
        case TI_NAS_ESM_T3482:
             ulTimerMaxExpNum = TI_NAS_ESM_T3482_TIMER_MAX_EXP_NUM;
             break;

        case TI_NAS_ESM_T3492:
             ulTimerMaxExpNum = TI_NAS_ESM_T3492_TIMER_MAX_EXP_NUM;
             break;

        case TI_NAS_ESM_T3480:
             ulTimerMaxExpNum = TI_NAS_ESM_T3480_TIMER_MAX_EXP_NUM;
             break;

        case TI_NAS_ESM_T3481:
             ulTimerMaxExpNum = TI_NAS_ESM_T3481_TIMER_MAX_EXP_NUM;
             break;

        default :
            NAS_ESM_WARN_LOG("NAS_ESM_GetTimerMaxExpNum:enTimerType Invalid.");
            break;
    }

    return ulTimerMaxExpNum;
}


/*****************************************************************************
 Function Name   : NAS_ESM_QueryOverduePtiBuffItem
 Description     : ���ҹ��ڵ�PTI�����¼
 Input           : ucPti----------------------PTI
                   ucEpsbId-------------------���غ�
 Output          : pulIndex-------------------�����¼������
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-2-2      Draft Enact
    2.lihong00150010      2010-04-23    Modify

*****************************************************************************/
VOS_UINT32  NAS_ESM_QueryOverduePtiBuffItem
(
    VOS_UINT8                           ucPti,
    VOS_UINT8                           ucEpsbId,
    VOS_UINT32                         *pulIndex
)
{
    NAS_ESM_PTI_BUFF_ITEM_STRU         *pstPtiBuffItem  = VOS_NULL_PTR;
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;

    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM; ulCnt++)
    {
        /* ��ȡ�����¼ */
        pstPtiBuffItem = (NAS_ESM_PTI_BUFF_ITEM_STRU*)
                                NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulCnt);

        if (pstPtiBuffItem == VOS_NULL_PTR)
        {
            continue;
        }

        /* �������಻�ܶ�ͬһ����ͬʱ�������ֲ�������˼����յ���������ϢЯ����
           ���غ�ΪucEpsbId��PTIΪucPti����PTI�����¼�г��غ���ucEpsbId��ͬ����
           PTIС��ucPti����������Ϊ��PTI�����¼Ϊ������Ч��PTI�����¼*/
        if ((pstPtiBuffItem->ucPti < ucPti)
            && (pstPtiBuffItem->ucEpsbId == ucEpsbId))
        {
            /* �ҵ���Ӧ��PTI�����¼������������ */
            *pulIndex = ulCnt;

            break;
        }
    }

    if (ulCnt < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM)
    {
        return NAS_ESM_SUCCESS;
    }

    return NAS_ESM_FAILURE;
}

/*****************************************************************************
 Function Name   : NAS_ESM_SavePtiBuffItem
 Description     : ����PTI�ͻظ���Ϣ
 Input           : ucPti----------------------PTI
                   ucEpsbId-------------------���غ�
                   ulLength-------------------�ظ���Ϣ����
                   pucSendMsg-----------------�ظ���Ϣ�׵�ַ
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-1-29     Draft Enact
    2.lihong00150010      2010-04-23    Modify

*****************************************************************************/
VOS_UINT32  NAS_ESM_SavePtiBuffItem
(
    VOS_UINT8                           ucPti,
    VOS_UINT8                           ucEpsbId,
    VOS_UINT32                          ulLength,
    const VOS_UINT8                    *pucSendMsg
)
{
    NAS_ESM_BUFF_MANAGE_INFO_STRU      *pstEsmBuffManInfo   = NAS_ESM_GetEsmBufferManageInfoAddr();
    NAS_ESM_PTI_BUFF_ITEM_STRU         *pstEsmPtiBuffItem   = VOS_NULL_PTR;
    VOS_UINT32                          ulBuffItemLen       = NAS_ESM_NULL;
    VOS_UINT32                          ulIndex             = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;

    NAS_ESM_INFO_LOG("NAS_ESM_SavePtiBuffItem is entered!");

    /* ������ڵĻ����¼ */
    if (NAS_ESM_SUCCESS == NAS_ESM_QueryOverduePtiBuffItem(ucPti, ucEpsbId, &ulIndex))
    {
        NAS_ESM_ClearEsmBuffItem(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulIndex);
    }

    /* �ж��Ƿ��ѵ���ϵͳ�ܹ��洢�����¼�ļ��� */
    if (pstEsmBuffManInfo->ucPtiBuffItemNum >= NAS_ESM_MAX_PTI_BUFF_ITEM_NUM)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_SavePtiBuffItem:WARN:PTI Buffer is full!");
        return NAS_ESM_FAILURE;
    }

    /* ���㻺���¼���� */
    ulBuffItemLen = (sizeof(NAS_ESM_PTI_BUFF_ITEM_STRU) - 4) + ulLength;

    /* ��ȡ�����¼�ռ� */
    pstEsmPtiBuffItem = (NAS_ESM_PTI_BUFF_ITEM_STRU *)NAS_COMM_AllocBuffItem(NAS_COMM_BUFF_TYPE_ESM,
                                                                             ulBuffItemLen);
    if (VOS_NULL_PTR == pstEsmPtiBuffItem)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_SavePtiBuffItem:WARN:NAS_COMM_AllocBuffItem is failed!");
        return NAS_ESM_FAILURE;
    }

    /* �������ڴ洢�����¼�׵�ַ��ָ�� */
    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM; ulCnt++)
    {
        if (pstEsmBuffManInfo->paPtiBuffer[ulCnt] == VOS_NULL_PTR)
        {
            break;
        }
    }

    if (ulCnt >= NAS_ESM_MAX_PTI_BUFF_ITEM_NUM)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_SavePtiBuffItem:Get the valuable which stores buffer item addr failed!");
        return NAS_ESM_FAILURE;
    }

    /* ��¼�洢�����¼���׵�ַ */
    pstEsmBuffManInfo->paPtiBuffer[ulCnt] = (VOS_VOID *)pstEsmPtiBuffItem;

    /* ����PTI,EPSBID,OPID��Ϣ */
    pstEsmPtiBuffItem->ucPti = ucPti;
    pstEsmPtiBuffItem->ucEpsbId = ucEpsbId;
    pstEsmPtiBuffItem->ulOpId = NAS_ESM_ILLEGAL_OPID;

    /* ����ظ���Ϣ */
    pstEsmPtiBuffItem->stEsmMsg.ulEsmMsgSize = ulLength;
    if (ulLength != NAS_ESM_NULL)
    {
        NAS_ESM_MEM_CPY(pstEsmPtiBuffItem->stEsmMsg.aucEsmMsg,
                        pucSendMsg,
                        ulLength);
    }

    /* ������ʱ��TI_NAS_ESM_WAIT_PTI_INVALID */
    NAS_ESM_TimerStart(ulCnt, TI_NAS_ESM_WAIT_PTI_INVALID);

    /* ���ӻ����¼���� */
    pstEsmBuffManInfo->ucPtiBuffItemNum++;

    return NAS_ESM_SUCCESS;
}

/*****************************************************************************
 Function Name   : NAS_ESM_QueryPtiBuffItemByEpsbId
 Description     : ͨ�����غŲ���ESM�����¼
 Input           : ucEpsbId-------------------���غ�
 Output          : pulIndex-------------------�����¼������
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-2-2      Draft Enact
    2.lihong00150010      2010-04-23    Modify

*****************************************************************************/
VOS_UINT32  NAS_ESM_QueryPtiBuffItemByEpsbId
(
    VOS_UINT8                           ucEpsbId,
    VOS_UINT32                         *pulIndex
)
{
    NAS_ESM_PTI_BUFF_ITEM_STRU         *pstPtiBuffItem  = VOS_NULL_PTR;
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;

    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM; ulCnt++)
    {
        /* ��ȡ�����¼ */
        pstPtiBuffItem = (NAS_ESM_PTI_BUFF_ITEM_STRU*)
                                NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulCnt);

        if (pstPtiBuffItem == VOS_NULL_PTR)
        {
            continue;
        }

        if (ucEpsbId == pstPtiBuffItem->ucEpsbId)
        {
            /* �ҵ���Ӧ��PTI�����¼������������ */
            *pulIndex = ulCnt;

            break;
        }
    }

    if (ulCnt < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM)
    {
        return NAS_ESM_SUCCESS;
    }

    return NAS_ESM_FAILURE;
}


/*****************************************************************************
 Function Name   : NAS_ESM_QueryPtiBuffItemByPti
 Description     : ͨ��PTI����ESM�����¼
 Input           : ucPti----------------------PTI
 Output          : pulIndex-------------------�����¼������
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-2-2      Draft Enact
    2.lihong00150010      2010-04-23    Modify

*****************************************************************************/
VOS_UINT32  NAS_ESM_QueryPtiBuffItemByPti
(
    VOS_UINT8                           ucPti,
    VOS_UINT32                         *pulIndex
)
{
    NAS_ESM_PTI_BUFF_ITEM_STRU         *pstPtiBuffItem      = VOS_NULL_PTR;
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;

    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM; ulCnt++)
    {
        /* ��ȡ�����¼ */
        pstPtiBuffItem = (NAS_ESM_PTI_BUFF_ITEM_STRU*)
                                NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulCnt);

        if (pstPtiBuffItem == VOS_NULL_PTR)
        {
            continue;
        }

        if (ucPti == pstPtiBuffItem->ucPti)
        {
            /* �ҵ���Ӧ��PTI�����¼������������ */
            *pulIndex = ulCnt;

            break;
        }
    }

    if (ulCnt < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM)
    {
        return NAS_ESM_SUCCESS;
    }

    return NAS_ESM_FAILURE;
}

/*****************************************************************************
 Function Name   : NAS_ESM_ClearEsmBuff
 Description     : ���ESM����
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-2-11  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_ClearEsmBuff( VOS_VOID )
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    NAS_ESM_BUFF_MANAGE_INFO_STRU      *pstEsmBuffManInfo   = NAS_ESM_GetEsmBufferManageInfoAddr();

    /* �ͷ�ATTACHING���͵Ļ����¼ */
    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_ATTACHING_BUFF_ITEM_NUM; ulCnt++)
    {
        if (pstEsmBuffManInfo->paAttBuffer[ulCnt] != VOS_NULL_PTR)
        {
            NAS_ESM_ClearEsmBuffItem(NAS_ESM_BUFF_ITEM_TYPE_ATTACHING, ulCnt);
        }
    }

    /* �ͷ�PTI���͵Ļ����¼ */
    for (ulCnt = NAS_ESM_NULL; ulCnt < NAS_ESM_MAX_PTI_BUFF_ITEM_NUM; ulCnt++)
    {
        if (pstEsmBuffManInfo->paPtiBuffer[ulCnt] != VOS_NULL_PTR)
        {
            NAS_ESM_ClearEsmBuffItem(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulCnt);
        }
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_ClearEsmBuffItem
 Description     : ���ESM�����¼
 Input           : ulIndex-------------------�����¼������
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-2-2  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_ClearEsmBuffItem
(
    NAS_ESM_BUFF_ITEM_TYPE_ENUM_UINT8   enEsmBuffType,
    VOS_UINT32                          ulIndex
)
{
    VOS_VOID                           *pEsmBuffItemAddr    = VOS_NULL_PTR;
    NAS_ESM_BUFF_MANAGE_INFO_STRU      *pstEsmBuffManInfo   = NAS_ESM_GetEsmBufferManageInfoAddr();
    VOS_VOID                          **ppBuffItem          = VOS_NULL_PTR;
    VOS_UINT8                          *pucBuffItemNum      = VOS_NULL_PTR;

    NAS_ESM_INFO_LOG("NAS_ESM_ClearEsmBuffItem is entered!");

    /* ��ȡ�����¼ */
    if (enEsmBuffType == NAS_ESM_BUFF_ITEM_TYPE_PTI)
    {
        pEsmBuffItemAddr = NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulIndex);

        /* ֹͣ��ʱ��TI_NAS_ESM_WAIT_PTI_INVALID */
        NAS_ESM_TimerStop(ulIndex, TI_NAS_ESM_WAIT_PTI_INVALID);

        ppBuffItem      = &pstEsmBuffManInfo->paPtiBuffer[ulIndex];
        pucBuffItemNum  = &pstEsmBuffManInfo->ucPtiBuffItemNum;
    }
    else
    {
        pEsmBuffItemAddr = NAS_ESM_GetBuffItemAddr(NAS_ESM_BUFF_ITEM_TYPE_ATTACHING, ulIndex);

        ppBuffItem      = &pstEsmBuffManInfo->paAttBuffer[ulIndex];
        pucBuffItemNum  = &pstEsmBuffManInfo->ucAttBuffItemNum;
    }

    if (pEsmBuffItemAddr == VOS_NULL_PTR)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_ClearEsmBuffItem:WARN:Get buffer item failed!");
        return ;
    }

    /* ��������¼ */
    (VOS_VOID)NAS_COMM_FreeBuffItem(NAS_COMM_BUFF_TYPE_ESM, pEsmBuffItemAddr);

    *ppBuffItem = VOS_NULL_PTR;

    /* �������¼����1 */
    *pucBuffItemNum = *pucBuffItemNum - 1;
}


VOS_VOID NAS_ESM_ClearEsmBearerResource( VOS_UINT32  ulEpsbId )
{
    VOS_UINT32                          ulTmpBitCid         = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    VOS_UINT32                          ulIndex             = NAS_ESM_NULL;
    VOS_UINT32                          ulStateTblIndex     = NAS_ESM_NULL;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbInfo         = VOS_NULL_PTR;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo     = VOS_NULL_PTR;

    /*��ӡ��Ϣ*/
    NAS_ESM_LOG1("NAS_ESM_ClearEsmBearerResource:NORM:EpsbId = ",ulEpsbId);

    /* ��������Ч���غţ��򷵻�δ����̬ */
    if ((NAS_ESM_MIN_EPSB_ID > ulEpsbId)
        || (NAS_ESM_MAX_EPSB_ID < ulEpsbId))
    {
        NAS_ESM_INFO_LOG("NAS_ESM_ClearEsmBearerResource:Unsigned or Reserved Eps Bearer ID!");
        return;
    }

    /* ��������¼����˳�����صļ�¼ */
    if (NAS_ESM_SUCCESS == NAS_ESM_QueryPtiBuffItemByEpsbId((VOS_UINT8)ulEpsbId,
                                                             &ulIndex))
    {
        NAS_ESM_ClearEsmBuffItem(NAS_ESM_BUFF_ITEM_TYPE_PTI, ulIndex);
    }
    /*�ͷ��������صĶ�̬����Դ */
    if (NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexByEpsbId(ulEpsbId, &ulStateTblIndex))
    {
        NAS_ESM_RelStateTblResource(ulStateTblIndex);
    }

    /*��ȡ������Ϣ��ַ*/
    pstEpsbInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

    /*����CID���ͷ���Ӧ��SDF��Ϣ*/
    if(NAS_ESM_OP_TRUE == pstEpsbInfo->bitOpCId)
    {
        ulTmpBitCid = pstEpsbInfo->ulBitCId;

        for( ulCnt = NAS_ESM_MIN_CID; ulCnt <= NAS_ESM_MAX_CID; ulCnt++ )
        {
            /*������ȡ����CID��¼*/
            if(NAS_ESM_OP_TRUE ==((ulTmpBitCid >> ulCnt)&NAS_ESM_OP_TRUE))
            {
                /* ���SDF������ */
                pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCnt);
                NAS_ESM_MEM_SET(pstSdfCntxtInfo, 0, sizeof(NAS_ESM_SDF_CNTXT_INFO_STRU));

                /* ���CID��21~31֮�䣬��ɾ����̬��Ϣ */
                if ((ulCnt >= NAS_ESM_MIN_NW_CONTROL_CID)
                   && (ulCnt <= NAS_ESM_MAX_CID))
                {
                    NAS_ESM_ClearSdfPara(ulCnt);
                }
            }
        }
    }

    /*���֧��˫APN�����ұ�ȥ����ĳ�����ע����أ������ע����ؼ�¼*/
    if (NAS_ESM_OP_TRUE == NAS_ESM_AttachBearerReestEnable())
    {
        if ((ulEpsbId == NAS_ESM_GetAttachBearerId()) &&
            (PS_TRUE == NAS_ESM_IsDefaultEpsBearerType(pstEpsbInfo->enBearerCntxtType)))
        {
            NAS_ESM_SetAttachBearerId(NAS_ESM_UNASSIGNED_EPSB_ID);
        }
    }

    /*��ʼ�������Ϣ*/
    NAS_ESM_InitEpsbCntxtInfo((VOS_UINT32)ulEpsbId);
}

/*****************************************************************************
 Function Name   : NAS_ESM_RelStateTblResource
 Description     : ����״̬��index�ͷ�״̬����Դ
 Input           : VOS_UINT32  ulStateTblIndex
 Output          : None
 Return          : VOS_VOID

 History         :
    1.sunbing49683      2009-2-11  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_RelStateTblResource(VOS_UINT32  ulStateTblIndex )
{
    NAS_ESM_STATE_INFO_STRU            *pstStateAddr     = VOS_NULL_PTR;

    NAS_ESM_INFO_LOG1("NAS_ESM_RelStateTblResource", ulStateTblIndex);

    /*���ulStateTblIndex��Ч*/
    if(ulStateTblIndex >= NAS_ESM_MAX_STATETBL_NUM)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RelStateTblResource:WARN: StateTblIndex illegal!");
        return ;
    }

    pstStateAddr = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

    /*ֹͣ��ʱ��*/
    NAS_ESM_TimerStop(ulStateTblIndex, pstStateAddr->stTimerInfo.enPara);

    /* �����ڻ�����Ϣ�������ͷŻ�����Ϣ�ռ� */
    if (pstStateAddr->pstDecodedNwMsg != VOS_NULL_PTR)
    {
        NAS_ESM_MEM_FREE(pstStateAddr->pstDecodedNwMsg);
    }

    /* ����������ؽ�����ȡ������֪ͨEMM������ESM�����õ�����Դ��
       FIXME: �˺����п����ڽ���PDN���ӻ�δ�·���EMM֮ǰ�ͱ����е���
              �����ʧ�ܣ���˻ᵼ�¶෢һ��notify
    */
    if (NAS_ESM_BEARER_TYPE_EMERGENCY == pstStateAddr->enBearerCntxtType)
    {
        NAS_ESM_SndEsmEmmClrEsmProcResNtyMsg(pstStateAddr->ulCid, PS_TRUE);
    }

    /*�����������*/
    NAS_ESM_MEM_SET(pstStateAddr, 0, sizeof(NAS_ESM_STATE_INFO_STRU));

    /*�ó�ʼֵ*/
    pstStateAddr->bitOpNwMsgRecord      = NAS_ESM_OP_FALSE;
    pstStateAddr->bitOpAppMsgRecord     = NAS_ESM_OP_FALSE;
    pstStateAddr->ulCid                 = NAS_ESM_ILL_CID;
    pstStateAddr->ulEpsbId              = NAS_ESM_UNASSIGNED_EPSB_ID;
    pstStateAddr->ulLinkedEpsbId        = NAS_ESM_UNASSIGNED_EPSB_ID;
    pstStateAddr->enBearerCntxtState    = NAS_ESM_BEARER_STATE_INACTIVE;
    pstStateAddr->enProcTransTState     = NAS_ESM_BEARER_PROC_TRANS_STATE_INACTIVE;
    pstStateAddr->pstDecodedNwMsg       = VOS_NULL_PTR;
    pstStateAddr->ucIsWaitForUserAnswer = PS_FALSE;
    pstStateAddr->enPdnReqType          = NAS_ESM_PDN_REQ_TYPE_INITIAL;
}

/*****************************************************************************
 Function Name   : NAS_ESM_QueryNWPacketFilterID
 Description     : ����App���õ�packetfilterID���������������packetfilterID
 Input           : ucPacketFilterID   -- APP���õ�packetfilterID
                   ulCID              -- Ҫ���ҵ�CID
 Output          : ucNWPacketFilterID -- ��������packetfilterID
 Return          : NAS_ESM_SUCCESS    -- �ҵ��˶�Ӧ������packetfilterID
                   NAS_ESM_FAILURE    -- û�ҵ��˶�Ӧ������packetfilterID
 History         :
    1.liuwenyu00143951      2009-10-23  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_QueryNWPacketFilterID
(
    VOS_UINT8                           ucPacketFilterID,
    VOS_UINT32                          ulCID,
    VOS_UINT8                          *ucNWPacketFilterID
)
{
    VOS_UINT32                          ulCnt           = NAS_ESM_NULL;
    NAS_ESM_CONTEXT_TFT_STRU           *pstTmpTFTInfo   = VOS_NULL_PTR;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo = VOS_NULL_PTR;

    /* when app set tft, packetfilter id is checked. */
    /* so here,no need to do input check, */

    pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCID);

    /* search mapped network packetfilter */
    for (ulCnt = NAS_ESM_NULL; ulCnt < pstSdfCntxtInfo->ulSdfPfNum; ulCnt++)
    {
        pstTmpTFTInfo = &pstSdfCntxtInfo->astSdfPfInfo[ulCnt];

        if (ucPacketFilterID == pstTmpTFTInfo->ucPacketFilterId)
        {
            *ucNWPacketFilterID = pstTmpTFTInfo->ucNwPacketFilterId;
            return NAS_ESM_SUCCESS;
        }
    }

    return NAS_ESM_FAILURE;
}

#if 0
/*****************************************************************************
 Function Name   : NAS_ESM_ReadPdpManInfoFromNv
 Description     : ��NV�ж�ȡ���ع���ģʽ
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2009-12-1  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_ReadPdpManInfoFromNv( VOS_VOID )
{
    VOS_UINT32                          ulResult        = VOS_NULL;
    NAS_ESM_NV_PDP_MANAGE_INFO_STRU     stNvPdpManInfo;
    NAS_ESM_PDP_MANAGE_INFO_STRU       *pBearMagageInfo = VOS_NULL_PTR;

    NAS_ESM_INFO_LOG("NAS_ESM_ReadPdpManInfoFromNv is entered.");

    /*��ȡ���س��ع�����Ϣ�ṹָ��*/
    pBearMagageInfo = NAS_ESM_GetBearerManageInfoAddr();

    ulResult = LPs_NvimItem_Read((EN_NV_ID_PS_BEARER_MANAGE_TYPE),\
                            (VOS_VOID *)(&stNvPdpManInfo),\
                            sizeof(NAS_ESM_NV_PDP_MANAGE_INFO_STRU));

    /* �ж϶�ȡ��� */
    if (ulResult != ERR_MSP_SUCCESS)
    {
        /*��ӡ�쳣��Ϣ*/
        NAS_ESM_ERR_LOG("NAS_ESM_ReadPdpManInfoFromNv:ERROR: Cannot Read from NV .");
    }
    else
    {
        /* �ж�NV�д洢�ĳ��ع���ģʽ�Ƿ���Ч */
        if ((stNvPdpManInfo.bitOpPdpManInfo == NAS_ESM_OP_TRUE)
         && (stNvPdpManInfo.stPdpManInfo.enMode < NAS_ESM_BEARER_MANAGE_MODE_BUTT)
         && (stNvPdpManInfo.stPdpManInfo.enType < NAS_ESM_BEARER_MANANGE_TYPE_BUTT))
        {
            NAS_ESM_MEM_CPY(pBearMagageInfo,
                            &stNvPdpManInfo.stPdpManInfo,
                            sizeof(NAS_ESM_PDP_MANAGE_INFO_STRU));
        }
        else
        {
            NAS_ESM_INFO_LOG("NAS_ESM_ReadPdpManInfoFromNv:INFO: Pdp manage information is invalid in nv!");
        }
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_WritePdpManInfoToNv
 Description     : �����ع���ģʽд��NV
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2009-12-1  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_ESM_WritePdpManInfoToNv( VOS_VOID )
{
    VOS_UINT32                          ulResult        = VOS_NULL;
    NAS_ESM_NV_PDP_MANAGE_INFO_STRU     stNvPdpManInfo;
    NAS_ESM_PDP_MANAGE_INFO_STRU       *pBearMagageInfo = VOS_NULL_PTR;
    VOS_VOID                           *pData;

    NAS_ESM_INFO_LOG("NAS_ESM_WritePdpManInfoToNv is entered.");

    /*��ȡ���س��ع�����Ϣ�ṹָ��*/
    pBearMagageInfo = NAS_ESM_GetBearerManageInfoAddr();

    NAS_ESM_MEM_SET(&stNvPdpManInfo,
                    NAS_ESM_NULL,
                    sizeof(NAS_ESM_NV_PDP_MANAGE_INFO_STRU));

    /* ��ֵ���ع���ģʽ */
    stNvPdpManInfo.bitOpPdpManInfo = NAS_ESM_OP_TRUE;

    NAS_ESM_MEM_CPY(&stNvPdpManInfo.stPdpManInfo,
                    pBearMagageInfo,
                    sizeof(NAS_ESM_PDP_MANAGE_INFO_STRU));

    pData  = &stNvPdpManInfo;
    ulResult = LPs_NvimItem_Write((EN_NV_ID_PS_BEARER_MANAGE_TYPE),\
                            pData,\
                            sizeof(NAS_ESM_NV_PDP_MANAGE_INFO_STRU));

    /* �ж϶�ȡ��� */
    if(ulResult != ERR_MSP_SUCCESS)
    {
        /*��ӡ�쳣��Ϣ*/
        NAS_ESM_ERR_LOG("NAS_ESM_WritePdpManInfoToNv:ERROR: Cannot Write to NV .");
    }
}
#endif


VOS_UINT32  NAS_ESM_IsTftInfoValid
(
    const APP_ESM_TFT_INFO_STRU               *pstAppTftInfo
)
{
    /* ����TFT�����Ƿ�Ϸ� */
    if (pstAppTftInfo->enDirection >= APP_ESM_TRANSFER_DIRECTION_BUTT)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsTftInfoValid:Invalid TFT Direction!");
        return NAS_ESM_FAILURE;
    }

    /* PFid ����Ч�Լ��*/
    if ((pstAppTftInfo->ucPacketFilterId == NAS_ESM_NULL)
      ||(pstAppTftInfo->ucPacketFilterId > NAS_ESM_MAX_SDF_PF_NUM))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsTftInfoValid:Invalid PacketFilter ID!");
        return NAS_ESM_FAILURE;
    }

    if ((NAS_ESM_OP_TRUE == pstAppTftInfo->bitOpRemotePortRange)
     && ((pstAppTftInfo->usRmtPortHighLimit < pstAppTftInfo->usRmtPortLowLimit)
        ||(NAS_ESM_OP_TRUE == pstAppTftInfo->bitOpSingleRemotePort)))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsTftInfoValid:Remote port error!");
        return NAS_ESM_FAILURE;
    }

    if ((NAS_ESM_OP_TRUE == pstAppTftInfo->bitOpLocalPortRange)
     && ((pstAppTftInfo->usLcPortHighLimit < pstAppTftInfo->usLcPortLowLimit)
        ||(NAS_ESM_OP_TRUE == pstAppTftInfo->bitOpSingleLocalPort)))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsTftInfoValid:Locate port error!");
        return NAS_ESM_FAILURE;
    }

    if ((NAS_ESM_OP_TRUE == pstAppTftInfo->bitOpFlowLabelType)
     && (pstAppTftInfo->ulFlowLabelType > NAS_ESM_MAX_FLOW_LABEL_TYPE))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsTftInfoValid:Flow Label Type error!");
        return NAS_ESM_FAILURE;
    }

    return NAS_ESM_SUCCESS;
}


VOS_UINT32  NAS_ESM_IsSdfQosValid
(
    const APP_ESM_EPS_QOS_INFO_STRU          *pstSdfQosInfo
)
{
     /* ���QCI�ĺϷ��� */
     if ((pstSdfQosInfo->ucQCI < NAS_ESM_QCI_TYPE_QCI1_GBR)
         || (pstSdfQosInfo->ucQCI > NAS_ESM_QCI_TYPE_QCI9_NONGBR))
     {
         NAS_ESM_WARN_LOG("NAS_ESM_IsSdfQosValid:QCI is illegal!");
         return NAS_ESM_FAILURE;
     }

     if (PS_TRUE == NAS_ESM_IsGbrBearer(pstSdfQosInfo->ucQCI))
     {
         if ((pstSdfQosInfo->ulDLGMaxRate > pstSdfQosInfo->ulDLMaxRate)
          || (pstSdfQosInfo->ulULGMaxRate > pstSdfQosInfo->ulULMaxRate))
          {
             NAS_ESM_WARN_LOG("NAS_ESM_IsSdfQosValid:APP->SM ,MBR < GBR!");
             return NAS_ESM_FAILURE;
          }

         if ((NAS_ESM_EPS_QOS_NULL_RATE == pstSdfQosInfo->ulULMaxRate)
          && (NAS_ESM_EPS_QOS_NULL_RATE == pstSdfQosInfo->ulDLMaxRate))
         {
              NAS_ESM_WARN_LOG("NAS_ESM_IsSdfQosValid:APP->SM ,upload and download MBR both zero!");
              return NAS_ESM_FAILURE;
         }
     }

     return NAS_ESM_SUCCESS;
}
VOS_UINT32  NAS_ESM_IsGwAuthInfoValid
(
    const APP_ESM_GW_AUTH_INFO_STRU           *pstGwAuthInfo
)
{
    if (pstGwAuthInfo->ucUserNameLen > APP_ESM_MAX_USER_NAME_LEN)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsGwAuthInfoValid:APP->ESM ,User name len is invalid!");
        return APP_ERR_SM_USER_NAME_TOO_LONG;
    }

    if (pstGwAuthInfo->ucPwdLen > APP_ESM_MAX_PASSWORD_LEN)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsGwAuthInfoValid:APP->ESM ,Password len is invalid!");
        return APP_ERR_SM_USER_PASSWORD_TOO_LONG;
    }

    if (pstGwAuthInfo->ucAccNumLen > APP_ESM_MAX_ACCESS_NUM_LEN)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsGwAuthInfoValid:APP->ESM ,Access Num len is invalid!");
        return APP_ERR_SM_ACCESS_NUM_TOO_LONG;
    }

    if (pstGwAuthInfo->enGwAuthType >= APP_ESM_AUTH_TYPE_BUTT)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsGwAuthInfoValid:APP->ESM ,Auth Type error!");
        return APP_ERR_SM_AUTH_TYPE_ILLEGAL;
    }

    return APP_SUCCESS;
}

VOS_UINT32  NAS_ESM_IsSdfParaOtherInfoValid
(
    const NAS_ESM_SDF_PARA_STRU           *pstSdfPara
)
{
    if (NAS_ESM_OP_TRUE == pstSdfPara->bitOpPdnType)
    {
        if ((pstSdfPara->enPdnType >= APP_ESM_PDN_TYPE_BUTT)
          ||(pstSdfPara->enPdnType == NAS_ESM_NULL))
        {
            NAS_ESM_WARN_LOG("NAS_ESM_IsSdfParaOtherInfoValid:Pdn type error!");
            return NAS_ESM_FAILURE;
        }
    }

    if ((NAS_ESM_OP_TRUE == pstSdfPara->bitIpv4AddrAllocType)
     && (pstSdfPara->enIpv4AddrAllocType >= APP_ESM_IPV4_ADDR_ALLOC_TYPE_BUTT))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsSdfParaOtherInfoValid:IP address allocacte type error!");
        return NAS_ESM_FAILURE;
    }

    if (pstSdfPara->enBearerCntxtType >= NAS_ESM_BEARER_TYPE_BUTT)
    {
        NAS_ESM_WARN_LOG("NAS_ESM_IsSdfParaOtherInfoValid:bearer type error!");
        return NAS_ESM_FAILURE;
    }

    return NAS_ESM_SUCCESS;
}


VOS_UINT32  NAS_ESM_IsSdfParaValid
(
    const NAS_ESM_SDF_PARA_STRU           *pstSdfPara
)
{
    VOS_UINT32                          ulCnt     = NAS_ESM_NULL;
    NAS_ESM_CONTEXT_APN_STRU            stApnInfo = {NAS_ESM_NULL};

    /* ��TFT�ļ��*/
    for (ulCnt = NAS_ESM_NULL; ulCnt < pstSdfPara->ulPfNum; ulCnt++)
    {
        if (NAS_ESM_FAILURE == NAS_ESM_IsTftInfoValid(&pstSdfPara->astCntxtTftInfo[ulCnt]))
        {
            return NAS_ESM_FAILURE;
        }
    }

    /* ��SDF Qos�ļ�� */
    if (NAS_ESM_OP_TRUE == pstSdfPara->bitOpSdfQos)
    {
       if (NAS_ESM_FAILURE == NAS_ESM_IsSdfQosValid(&pstSdfPara->stSdfQosInfo))
       {
            return NAS_ESM_FAILURE;
       }
    }

    /* �����ؼ�Ȩ��Ϣ�ļ�� */
    if (NAS_ESM_OP_TRUE == pstSdfPara->bitOpGwAuthInfo)
    {
        if (APP_SUCCESS != NAS_ESM_IsGwAuthInfoValid(&pstSdfPara->stGwAuthInfo))
        {
            return NAS_ESM_FAILURE;
        }
    }

    /* ��Apn��Ϣ�ļ��*/
    if (NAS_ESM_OP_TRUE == pstSdfPara->bitOpApn)
    {

        if (NAS_ESM_FAILURE == NAS_ESM_ApnTransformaton(&stApnInfo,
                                                        &pstSdfPara->stApnInfo))
        {
            return NAS_ESM_FAILURE;
        }

    }

    if (NAS_ESM_FAILURE == NAS_ESM_IsSdfParaOtherInfoValid(pstSdfPara))
    {
        return NAS_ESM_FAILURE;
    }

    return NAS_ESM_SUCCESS;
}




/*****************************************************************************
 Function Name   : NAS_ESM_GetAppErrorByEsmCause
 Description     : ����ESMԭ��ֵ����ö�Ӧ��App������
 Input           : enEsmCause -- ESMԭ��ֵ
 Output          : None
 Return          : App������

 History         :
    1.liuwenyu00143951      2009-09-22  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_GetAppErrorByEsmCause
(
    NAS_ESM_CAUSE_ENUM_UINT8            enEsmCause
)
{
    VOS_UINT32          ulArrayLength = NAS_ESM_GetEsmAppErrorMapArrayLen();
    VOS_UINT32          ulCnt         = VOS_NULL;
    NAS_ESM_APP_ERROR_MAPPING_STRU   *pstAppErrorEle = NAS_ESM_GetEsmAppErrorMapTbl();

    for (ulCnt = 0; ulCnt < ulArrayLength; ulCnt++ )
    {
        if (pstAppErrorEle[ulCnt].enEsmCause == enEsmCause)
        {
            return pstAppErrorEle[ulCnt].ulAppErrorCode;
        }
    }

    return APP_ERR_SM_NW_PROT_ERR_UNSPECIFIED;
}


VOS_UINT32  NAS_ESM_GetBearerLinkedCidNum
(
    VOS_UINT32                          ulEpsbId
)
{
    VOS_UINT32                          ulCnt       = NAS_ESM_NULL;
    VOS_UINT32                          ulActCidNum = NAS_ESM_NULL;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbInfo = VOS_NULL_PTR;

    pstEpsbInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

    /* ͳ�Ƴ��ض�Ӧ�ļ����Cid��Ŀ*/
    for (ulCnt = NAS_ESM_MIN_CID; ulCnt <= NAS_ESM_MAX_CID; ulCnt++)
    {
        if (NAS_ESM_OP_TRUE == ((pstEpsbInfo->ulBitCId >> ulCnt) & NAS_ESM_BIT_0))
        {
            ulActCidNum++;
        }
    }

    return ulActCidNum;
}
VOS_INT32  NAS_ESM_PrintAppEsmSetBearerTypeReqMsg
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    APP_ESM_SET_BEARER_TYPE_REQ_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_SET_BEARER_TYPE_REQ_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ulCid = %d \r\n",
                       pstRcvMsg->ulCid,
                       &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enSetType = %d \r\n",
                       pstRcvMsg->enSetType,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enBearCidType = %d \r\n",
                       pstRcvMsg->enBearCidType,
                       &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    if (NAS_ESM_OP_TRUE == pstRcvMsg->bitOpLinkdCid)
    {
        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " ulLinkdCid = %d \r\n",
                           pstRcvMsg->ulLinkdCid,
                          &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }


    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_SET_BEARER_TYPE_REQ_STRU****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}




VOS_INT32  NAS_ESM_PrintAppEsmSetManageTypeReqMsg
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                                    ilOutPutLen = 0;
    VOS_UINT16                                   usTotalLen  = usOffset;
    APP_ESM_SET_PDP_MANAGER_TYPE_REQ_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;


    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_SET_PDP_MANAGER_TYPE_REQ_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enSetType = %d \r\n",
                       pstRcvMsg->enSetType,
                       &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enAnsMode = %d \r\n",
                       pstRcvMsg->enAnsMode,
                       &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    if (APP_ESM_ANSWER_MODE_AUTO == pstRcvMsg->enAnsMode)
    {
        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " enAnsType = %d \r\n",
                           pstRcvMsg->enAnsType,
                           &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }


    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_SET_PDP_MANAGER_TYPE_REQ_STRU****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}



VOS_INT32  NAS_ESM_PrintAppEsmSetPdnTypeReqMsg
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    APP_ESM_SET_PDN_TYPE_REQ_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;


    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_SET_PDN_TYPE_REQ_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ulCid = %d \r\n",
                       pstRcvMsg->ulCid,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enSetType = %d \r\n",
                       pstRcvMsg->enSetType,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enPdnType = %d \r\n",
                       pstRcvMsg->enPdnType,
                       &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    if (NAS_ESM_OP_TRUE == pstRcvMsg->bitOpIpv4AddrAllocType)
    {
        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " enIpv4AddrAllocType = %d \r\n",
                           pstRcvMsg->enIpv4AddrAllocType,
                           &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }


    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_SET_PDN_TYPE_REQ_STRU****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}



VOS_INT32  NAS_ESM_PrintAppEsmSetEpsQosReqMsg
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    APP_ESM_SET_EPS_QOS_REQ_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;


    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_SET_EPS_QOS_REQ_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ulCid = %d \r\n",
                       pstRcvMsg->ulCid,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enSetType = %d \r\n",
                       pstRcvMsg->enSetType,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " stSdfQosInfo.ucQCI = %d \r\n",
                       pstRcvMsg->stSdfQosInfo.ucQCI,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " stSdfQosInfo.ulULMaxRate = %d \r\n",
                       pstRcvMsg->stSdfQosInfo.ulULMaxRate,
                       &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " stSdfQosInfo.ulDLMaxRate = %d \r\n",
                       pstRcvMsg->stSdfQosInfo.ulDLMaxRate,
                       &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " stSdfQosInfo.ulULGMaxRate = %d \r\n",
                       pstRcvMsg->stSdfQosInfo.ulULGMaxRate,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " stSdfQosInfo.ulDLGMaxRate = %d \r\n",
                       pstRcvMsg->stSdfQosInfo.ulDLGMaxRate,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_SET_EPS_QOS_REQ_STRU****************",
                    &ilOutPutLen);

   usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  NAS_ESM_PrintAppEsmSetGwAuthReqMsg
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT32                          ulLoop;
    VOS_UINT16                          usTotalLen  = usOffset;
    APP_ESM_SET_GW_AUTH_REQ_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_SET_GW_AUTH_REQ_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ulCid = %d \r\n",
                       pstRcvMsg->ulCid,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enGwAuthType = %d \r\n",
                       pstRcvMsg->stGwAuthInfo.enGwAuthType,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "stGwAuthInfo.ucAccNumLen = %d \r\n",
                       pstRcvMsg->stGwAuthInfo.ucAccNumLen,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    for (ulLoop = 0; ulLoop < pstRcvMsg->stGwAuthInfo.ucAccNumLen; ulLoop++)
    {
        NAS_COMM_nsprintf_2(pcBuff,
                           usTotalLen,
                           " auAccessNum[%d] = %d \r\n",
                           ulLoop,
                           pstRcvMsg->stGwAuthInfo.auAccessNum[ulLoop],
                          &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "stGwAuthInfo.ucUserNameLen = %d \r\n",
                       pstRcvMsg->stGwAuthInfo.ucUserNameLen,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    for (ulLoop = 0; ulLoop < pstRcvMsg->stGwAuthInfo.ucUserNameLen; ulLoop++)
    {
        NAS_COMM_nsprintf_2(pcBuff,
                           usTotalLen,
                           " aucUserName[%d] = %d \r\n",
                           ulLoop,
                           pstRcvMsg->stGwAuthInfo.aucUserName[ulLoop],
                          &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "stGwAuthInfo.ucPwdLen = %d \r\n",
                       pstRcvMsg->stGwAuthInfo.ucPwdLen,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;


    for (ulLoop = 0; ulLoop < pstRcvMsg->stGwAuthInfo.ucPwdLen; ulLoop++)
    {
        NAS_COMM_nsprintf_2(pcBuff,
                           usTotalLen,
                           " aucPwd[%d] = %d \r\n",
                           ulLoop,
                           pstRcvMsg->stGwAuthInfo.aucPwd[ulLoop],
                          &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }

    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_SET_GW_AUTH_REQ_STRU****************",
                    &ilOutPutLen);

   usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  NAS_ESM_PrintAppEsmSetCgdontReqMsg
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    APP_ESM_SET_CGDCONT_REQ_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;

    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_SET_CGDCONT_REQ_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ulCid = %d \r\n",
                       pstRcvMsg->ulCid,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    if (NAS_ESM_OP_TRUE == pstRcvMsg->bitOpApn)
    {
        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " aucApnName = %d \r\n",
                           (VOS_UINT32)pstRcvMsg->stApnInfo.aucApnName,
                           &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " ucApnLen = %d \r\n",
                           pstRcvMsg->stApnInfo.ucApnLen,
                           &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }

    if (NAS_ESM_OP_TRUE == pstRcvMsg->bitOpBearType)
    {
        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " enBearCidType = %d \r\n",
                           pstRcvMsg->enBearCidType,
                           &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;

    }

    if (NAS_ESM_OP_TRUE == pstRcvMsg->bitOpPdnType)
    {
        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " enBearCidType = %d \r\n",
                           pstRcvMsg->enPdnType,
                           &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;

    }

    if (NAS_ESM_OP_TRUE == pstRcvMsg->bitOpIpv4AddrAllocType)
    {
        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           " enIpv4AddrAllocType = %d \r\n",
                           pstRcvMsg->enIpv4AddrAllocType,
                           &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;

    }

    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " enGwAuthType = %d \r\n",
                       pstRcvMsg->enSetType,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_SET_CGDCONT_REQ_STRU****************",
                    &ilOutPutLen);

   usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}




VOS_INT32  NAS_ESM_PrintEsmAppIpv6InfoNotify
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulLoop2;
    APP_ESM_IPV6_INFO_NOTIFY_STRU      *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;


    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_IPV6_INFO_NOTIFY_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ucEpsbId = %d \r\n",
                       pstRcvMsg->ucEpsbId,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ucIpv6PrefixNum = %d \r\n",
                       pstRcvMsg->ucIpv6PrefixNum,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    for (ulLoop = 0; ulLoop < pstRcvMsg->ucIpv6PrefixNum; ulLoop++)
    {
         NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ucPrefixLen = %d \r\n",
                       pstRcvMsg->astIpv6PrefixArray[ulLoop].ucPrefixLen,
                      &ilOutPutLen);

        usTotalLen += (VOS_UINT16)ilOutPutLen;

        for (ulLoop2 = 0; ulLoop2 < pstRcvMsg->astIpv6PrefixArray[ulLoop].ucPrefixLen; ulLoop2++)
        {
            NAS_COMM_nsprintf_2(pcBuff,
                               usTotalLen,
                               " aucPrefix[%d] = %d \r\n",
                               ulLoop2,
                               pstRcvMsg->astIpv6PrefixArray[ulLoop].aucPrefix[ulLoop2],
                              &ilOutPutLen);

            usTotalLen += (VOS_UINT16)ilOutPutLen;
        }
    }

    return (usTotalLen - usOffset) ;
}
VOS_INT32  NAS_ESM_PrintEsmAppProcedureAbortNotify
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    APP_ESM_PROCEDURE_ABORT_NOTIFY_STRU      *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;


    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_PROCEDURE_ABORT_NOTIFY_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ulCid = %d \r\n",
                       pstRcvMsg->ulCid,
                      &ilOutPutLen);

   usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  NAS_ESM_PrintEsmAppSetResult
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    APP_ESM_PARA_SET_CNF_STRU          *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;


    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_PARA_SET_CNF_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ulCid = %d \r\n",
                       pstRcvMsg->ulCid,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "ulSetRslt = %d \r\n",
                       pstRcvMsg->ulSetRslt,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_PARA_SET_CNF_STRU****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}


VOS_INT32  NAS_ESM_PrintEsmAppCgdontResult
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen  = usOffset;
    APP_ESM_SET_CGDCONT_CNF_STRU       *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;


    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_SET_CGDCONT_CNF_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       " ulCid = %d \r\n",
                       pstRcvMsg->ulCid,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                       usTotalLen,
                       "ulSetRslt = %d \r\n",
                       pstRcvMsg->ulRslt,
                      &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_PARA_SET_CNF_STRU****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}





VOS_INT32  NAS_ESM_PrintEsmAppSetManageTypeResult
(
    VOS_CHAR                                *pcBuff,
    VOS_UINT16                               usOffset,
    const PS_MSG_HEADER_STRU                *pstMsg
)
{
    VOS_INT32                               ilOutPutLen = 0;
    VOS_UINT16                              usTotalLen  = usOffset;
    APP_ESM_SET_PDP_MANAGER_TYPE_CNF_STRU  *pstRcvMsg   = VOS_NULL_PTR;

    pstRcvMsg = (VOS_VOID *)pstMsg;


    /* ��ӡ��Ϣ�ṹ���� */
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "\r\n ****************Begin :APP_ESM_SET_PDP_MANAGER_TYPE_CNF_STRU****************\r\n",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ��ÿ������*/
    NAS_COMM_nsprintf_1(pcBuff,
                        usTotalLen,
                        " ulSetRslt = %d \r\n",
                        pstRcvMsg->ulSetRslt,
                       &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ��Ϣ�Ľ�������*/
    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     " ****************End :APP_ESM_SET_PDP_MANAGER_TYPE_CNF_STRU****************",
                    &ilOutPutLen);

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    return (usTotalLen - usOffset) ;
}
VOS_INT32  NAS_ESM_PrintEsmAppMsg
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    const PS_MSG_HEADER_STRU            *pstMsg
)

{
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulEsmAppMsgNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* �����Ϣ���ĳ��� */
    ulEsmAppMsgNum = sizeof(g_astEsmAppMsgIdArray)/sizeof(NAS_COMM_PRINT_LIST_STRU);

    /* ���Ҷ�Ӧ����Ϣ */
    for (ulLoop = 0; ulLoop< ulEsmAppMsgNum ; ulLoop++)
    {
        if (pstMsg->ulMsgName == g_astEsmAppMsgIdArray[ulLoop].ulId)
        {
            break;
        }
    }

    /* ��ӡ��Ӧ����Ϣ */
    if ( ulLoop < ulEsmAppMsgNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astEsmAppMsgIdArray[ulLoop].aucPrintString),
                         &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        if (VOS_NULL_PTR != g_astEsmAppMsgIdArray[ulLoop].pfActionFun)
        {
            ilOutPutLen = g_astEsmAppMsgIdArray[ulLoop].pfActionFun(pcBuff,
                                                                     usTotalLen,
                                                                     pstMsg);
        }

        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        NAS_ESM_LOG1("NAS_ESM_PrintEsmAppMsg, Invalid enMsgId: ",pstMsg->ulMsgName);
    }

    return (usTotalLen - usOffset);

}
VOS_INT32  NAS_ESM_PrintEsmTimer
(
    VOS_CHAR                            *pcBuff,
    VOS_UINT16                           usOffset,
    VOS_UINT32                           ulTimerType
)

{
    VOS_UINT32                          ulEsmTimerNum;
    VOS_INT32                           ilOutPutLen = 0;
    VOS_UINT16                          usTotalLen = usOffset;

    /* �����Ϣ���ĳ��� */
    ulEsmTimerNum = sizeof(g_astEsmStateTimerArray)/sizeof(NAS_COMM_PRINT_MSG_LIST_STRU);

    /* ��ӡ��Ӧ����Ϣ */
    if ( ulTimerType < ulEsmTimerNum )
    {
        NAS_COMM_nsprintf(pcBuff,
                         usTotalLen,
                         (VOS_CHAR *)(g_astEsmStateTimerArray[ulTimerType].aucPrintString),
                         &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;

        NAS_COMM_nsprintf_1(pcBuff,
                           usTotalLen,
                           "[ TICK : %ld ]",
                           PS_GET_TICK(),
                          &ilOutPutLen);
        usTotalLen += (VOS_UINT16)ilOutPutLen;
    }
    else
    {
        NAS_ESM_LOG1("NAS_ESM_PrintEsmTimer, Invalid enMsgId: ",ulTimerType);
    }

    return (usTotalLen - usOffset);

}


VOS_VOID NAS_ESM_PrintEsmRevMsg
(
    const PS_MSG_HEADER_STRU           *pstMsg,
    VOS_CHAR                           *pcBuff
)
{
    VOS_INT32                           ilOutPutLenHead     = 0;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen          = 0;

    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "NAS ESM Receive Message:",
                     &ilOutPutLen);

    if ( 0 == ilOutPutLen )
    {
        NAS_ESM_WARN_LOG("NAS_ESM_PrintEsmRevMsg, Print receive msg header exception.");
        return ;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ����PID����Ϣ */
    switch ( pstMsg->ulSenderPid )
    {
        case PS_PID_RABM:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "RABM-->ESM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintEsmRabmMsg(pcBuff,
                                              usTotalLen,
                                              pstMsg);

            break;

        case PS_PID_MM:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "MM-->ESM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintMmEsmMsg(pcBuff,
                                            usTotalLen,
                                            pstMsg);

            break;

        case PS_PID_OM  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "APP(OM)-->ESM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_ESM_PrintEsmAppMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg);
            break;
        /* xiongxianghui00253310 add ACPU_PID_OM for errlog 2013-11-30 begin */
        #if (FEATURE_PTM == FEATURE_ON)
        case ACPU_PID_OM  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "APP(ACPU_PID_OM)-->ESM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_ESM_PrintEsmAppMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg);
            break;
        #endif
        /* xiongxianghui00253310 add ACPU_PID_OM for errlog 2013-11-30 end   */
        case WUEPS_PID_TAF:
            NAS_COMM_nsprintf(  pcBuff,
                                usTotalLen,
                                "SM-->ESM\t",
                                &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintSmEsmMsg(pcBuff,
                                                usTotalLen,
                                                pstMsg);
            break;
        case VOS_PID_TIMER  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "TIMER-->ESM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_ESM_PrintEsmTimer(pcBuff,
                                            usTotalLen,
                                            ((REL_TIMER_MSG *)(VOS_VOID *)pstMsg)->ulPara);
            break;

        case MSP_PID_DIAG_APP_AGENT:
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "om-->ESM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;
            break;



        default:
            NAS_ESM_LOG2("NAS_PrintRevSmMsg,Invalid Pid, MsgId: ",
                          pstMsg->ulSenderPid,
                          pstMsg->ulMsgName);

            return ;
    }

    if ( 0 == ilOutPutLen )
    {
        NAS_ESM_LOG2("NAS_ESM_PrintEsmRevMsg, print return zero length.SenderPid, MsgId: ",
                      pstMsg->ulSenderPid,
                      pstMsg->ulMsgName);
        return;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_Print(pcBuff, usTotalLen);

    return ;

}
VOS_VOID NAS_ESM_PrintEsmSendMsg
(
    const PS_MSG_HEADER_STRU           *pstMsg,
    VOS_CHAR                           *pcBuff
)
{
    VOS_INT32                           ilOutPutLenHead     = 0;
    VOS_INT32                           ilOutPutLen         = 0;
    VOS_UINT16                          usTotalLen          = 0;

    NAS_COMM_nsprintf(pcBuff,
                     usTotalLen,
                     "NAS ESM Send Message:",
                     &ilOutPutLen);

    if ( 0 == ilOutPutLen )
    {
        NAS_ESM_WARN_LOG("NAS_ESM_PrintEsmSendMsg, Print send msg header exception.");
        return ;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    /* ��ӡ����PID����Ϣ */
    switch ( pstMsg->ulReceiverPid )
    {
        case PS_PID_RABM :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "ESM-->RABM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintEsmRabmMsg(
                                             pcBuff,
                                             usTotalLen,
                                             pstMsg);
            break;

        case PS_PID_MM :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "ESM-->MM\t",
                            &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintMmEsmMsg(
                                            pcBuff,
                                            usTotalLen,
                                            pstMsg);
            break;

        case PS_PID_OM  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "ESM-->APP(OM)\t",
                            &ilOutPutLenHead);
            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_ESM_PrintEsmAppMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg);
            break;
        /* xiongxianghui00253310 add ACPU_PID_OM for errlog 2013-11-30 begin */
        #if (FEATURE_PTM == FEATURE_ON)
        case ACPU_PID_OM  :
            NAS_COMM_nsprintf(pcBuff,
                             usTotalLen,
                             "ESM-->APP(ACPU_PID_OM)\t",
                            &ilOutPutLenHead);
            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_ESM_PrintEsmAppMsg(pcBuff,
                                             usTotalLen,
                                             pstMsg);
            break;
        #endif
        /* xiongxianghui00253310 add ACPU_PID_OM for errlog 2013-11-30 end   */
        case WUEPS_PID_TAF:
            NAS_COMM_nsprintf(  pcBuff,
                                usTotalLen,
                                "ESM-->SM\t",
                                &ilOutPutLenHead);

            usTotalLen += (VOS_UINT16)ilOutPutLenHead;

            ilOutPutLen = NAS_COMM_PrintSmEsmMsg(pcBuff,
                                                usTotalLen,
                                                pstMsg);
            break;
        default:
            NAS_ESM_LOG2("NAS_ESM_PrintEsmSendMsg,Invalid Pid, MsgId: ",
                          pstMsg->ulReceiverPid,
                          pstMsg->ulMsgName);
            return ;
    }

    if ( 0 == ilOutPutLen )
    {
        NAS_ESM_LOG2("NAS_ESM_PrintEsmSendMsg, print return zero length. ReceiverPid, MsgId: ",
                              pstMsg->ulReceiverPid,
                              pstMsg->ulMsgName);
        return;
    }

    usTotalLen += (VOS_UINT16)ilOutPutLen;

    NAS_COMM_Print(pcBuff,usTotalLen);

    return ;

}

VOS_VOID NAS_ESM_PrintEpsQos( const NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbInfo )
{
    if(NAS_ESM_OP_TRUE == pstEpsbInfo->bitOpEpsQos )
    {
        NAS_ESM_LOG1("ucQCI = ",pstEpsbInfo->stEpsQoSInfo.stQosInfo.ucQCI);
        NAS_ESM_LOG1("ucNwQCI = ",pstEpsbInfo->stEpsQoSInfo.ucNwQCI);
        NAS_ESM_LOG1("ulULMaxRate = ",pstEpsbInfo->stEpsQoSInfo.stQosInfo.ulULMaxRate);
        NAS_ESM_LOG1("ulDLMaxRate = ",pstEpsbInfo->stEpsQoSInfo.stQosInfo.ulDLMaxRate);
        NAS_ESM_LOG1("ulULGMaxRate = ",pstEpsbInfo->stEpsQoSInfo.stQosInfo.ulULGMaxRate);
        NAS_ESM_LOG1("ulDLGMaxRate = ",pstEpsbInfo->stEpsQoSInfo.stQosInfo.ulDLGMaxRate);
    }

}



VOS_VOID NAS_ESM_PrintEpsLinkedSdfId( const NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbInfo )
{
    VOS_UINT32                          ulBitCId    = NAS_ESM_NULL;
    VOS_UINT8                           ucLoop      = NAS_ESM_NULL;
    PS_BOOL_ENUM_UINT8                  enIsFirSdf  = PS_TRUE;

    if(NAS_ESM_OP_TRUE != pstEpsbInfo->bitOpCId )
    {
         NAS_ESM_INFO_LOG(" no SDF\r\n");
         return ;
    }

    ulBitCId = pstEpsbInfo->ulBitCId;

    NAS_ESM_INFO_LOG("Eps Bearer Linked CID:");

    for (ucLoop = NAS_ESM_MIN_CID; ucLoop <= NAS_ESM_MAX_CID; ucLoop++)
    {
        if (NAS_ESM_OP_TRUE != ((ulBitCId >> ucLoop) & NAS_ESM_BIT_0))
        {
            continue;
        }

        if (PS_TRUE == enIsFirSdf)
        {
            NAS_ESM_LOG1("",ucLoop);
            enIsFirSdf = PS_FALSE;
        }
        else
        {
            NAS_ESM_LOG1(",",ucLoop);
        }
    }

}

/*****************************************************************************
 Function Name   : NAS_ESM_PrintEpsPdn
 Description     : ��ӡ������Ϣ�е�PDN��Ϣ
 Input           : pstEpsbInfo
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010    2010-01-19  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_PrintEpsPdn( const NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbInfo )
{
    if ((PS_TRUE == NAS_ESM_IsDefaultEpsBearerType(pstEpsbInfo->enBearerCntxtType))
        && (NAS_ESM_OP_TRUE == pstEpsbInfo->bitOpPdnAddr))
    {
        if ( (APP_ESM_PDN_TYPE_IPV4 == pstEpsbInfo->stPdnAddrInfo.ucIpType)
             || (APP_ESM_PDN_TYPE_IPV4_IPV6 == pstEpsbInfo->stPdnAddrInfo.ucIpType))
        {
            NAS_ESM_INFO_LOG("ucIpType = APP_ESM_PDN_TYPE_IPV4");
            NAS_ESM_LOG1("aucIpV4Addr[0] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV4Addr[0]);

            NAS_ESM_LOG1("aucIpV4Addr[1] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV4Addr[1]);
            NAS_ESM_LOG1("aucIpV4Addr[2] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV4Addr[2]);
            NAS_ESM_LOG1("aucIpV4Addr[3] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV4Addr[3]);
        }

        if ( (APP_ESM_PDN_TYPE_IPV6 == pstEpsbInfo->stPdnAddrInfo.ucIpType)
             || (APP_ESM_PDN_TYPE_IPV4_IPV6 == pstEpsbInfo->stPdnAddrInfo.ucIpType))
        {
            NAS_ESM_INFO_LOG("ucIpType = APP_ESM_PDN_TYPE_IPV6");
            NAS_ESM_LOG1("aucIpV6Addr[0] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[0]);

            NAS_ESM_LOG1("aucIpV6Addr[1] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[1]);
            NAS_ESM_LOG1("aucIpV6Addr[2] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[2]);
            NAS_ESM_LOG1("aucIpV6Addr[3] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[3]);

            NAS_ESM_LOG1("aucIpV6Addr[4] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[4]);
            NAS_ESM_LOG1("aucIpV6Addr[5] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[5]);
            NAS_ESM_LOG1("aucIpV6Addr[6] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[6]);

            NAS_ESM_LOG1("aucIpV6Addr[7] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[7]);
            NAS_ESM_LOG1("aucIpV6Addr[8] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[8]);
            NAS_ESM_LOG1("aucIpV6Addr[9] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[9]);

            NAS_ESM_LOG1("aucIpV6Addr[10] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[10]);
            NAS_ESM_LOG1("aucIpV6Addr[11] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[11]);
            NAS_ESM_LOG1("aucIpV6Addr[12] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[12]);

            NAS_ESM_LOG1("aucIpV6Addr[13] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[13]);
            NAS_ESM_LOG1("aucIpV6Addr[14] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[14]);
            NAS_ESM_LOG1("aucIpV6Addr[15] = ",
            pstEpsbInfo->stPdnAddrInfo.aucIpV6Addr[15]);
        }
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_PrintEpsApnAmbr
 Description     : ��ӡ������Ϣ�е�APN-AMBR��Ϣ
 Input           : pstEpsbInfo
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010    2010-01-19  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_PrintEpsApnAmbr( const NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbInfo )
{
    if ((PS_TRUE == NAS_ESM_IsDefaultEpsBearerType(pstEpsbInfo->enBearerCntxtType))
        && (NAS_ESM_OP_TRUE == pstEpsbInfo->bitOpApnAmbr))
    {
        NAS_ESM_INFO_LOG("APN-AMBR Info :\r\n");
        NAS_ESM_LOG1("DLApnAmbr = ",pstEpsbInfo->stApnAmbrInfo.ucDLApnAmbr);
        NAS_ESM_LOG1("DLApnAmbrExt = ",pstEpsbInfo->stApnAmbrInfo.ucDLApnAmbrExt);
        NAS_ESM_LOG1("DLApnAmbrExt2 = ",pstEpsbInfo->stApnAmbrInfo.ucDLApnAmbrExt2);
        NAS_ESM_LOG1("ULApnAmbr = ",pstEpsbInfo->stApnAmbrInfo.ucULApnAmbr);
        NAS_ESM_LOG1("ULApnAmbrExt = ",pstEpsbInfo->stApnAmbrInfo.ucULApnAmbrExt);
        NAS_ESM_LOG1("ULApnAmbrExt2 = ",pstEpsbInfo->stApnAmbrInfo.ucULApnAmbrExt2);
    }
}



VOS_VOID NAS_ESM_PrintEpsbInfo(VOS_UINT32 ulEpsbId)
{
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbInfo         = VOS_NULL_PTR;

    if ((ulEpsbId < NAS_ESM_MIN_EPSB_ID) || (ulEpsbId > NAS_ESM_MAX_EPSB_ID))
    {
        NAS_ESM_INFO_LOG("�����EPSBIDֵ����EPSBIDȡֵ��ΧΪ5..15 \r\n");
        return;
    }

    pstEpsbInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

    NAS_ESM_INFO_LOG("****************Begin:EPS Bearer Information****************");

    NAS_ESM_LOG1("ulEpsbId = ",pstEpsbInfo->ulEpsbId);

    NAS_ESM_LOG1("enBearerCntxtType = ",pstEpsbInfo->enBearerCntxtType);

    if ((pstEpsbInfo->ulLinkedEpsbId >= NAS_ESM_MIN_EPSB_ID)
     || (pstEpsbInfo->ulLinkedEpsbId <= NAS_ESM_MAX_EPSB_ID) )
    {
        NAS_ESM_LOG1("ulLinkedEpsbId = ",pstEpsbInfo->ulLinkedEpsbId);
    }

    /* ��ӡ����QOS */
    NAS_ESM_PrintEpsQos(pstEpsbInfo);

    /* ��ӡ���ذ�����SDF ID */
    NAS_ESM_PrintEpsLinkedSdfId(pstEpsbInfo);

    /* ��ӡPDN��Ϣ */
    NAS_ESM_PrintEpsPdn(pstEpsbInfo);

    /* ��ӡAPN-AMBR��Ϣ */
    NAS_ESM_PrintEpsApnAmbr(pstEpsbInfo);
    NAS_ESM_INFO_LOG("****************End:EPS Bearer Information****************");

}

/*****************************************************************************
 Function Name   : NAS_ESM_GetAllEpsBearerInPdn
 Description     : ��ȡĳPDN�µ����г��غ�
 Input           : ucEpsbId---------------------���غ�
 Output          : pucEpsBearNum----------------������
                   pucEspbId--------------------���غ������׵�ַ
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-05-27   Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_ESM_GetAllEpsBearerInPdn
(
    VOS_UINT8                           ucEpsbId,
    VOS_UINT8                          *pucEpsBearNum,
    VOS_UINT8                          *pucEspbId
)
{
    VOS_UINT8                           ucCnt               = NAS_ESM_NULL;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo    = VOS_NULL_PTR;

    *pucEpsBearNum = NAS_ESM_NULL;

    if (PS_FALSE == NAS_ESM_IsDefaultEpsBearerType(NAS_ESM_GetBearCntxtType(ucEpsbId)))
    {
        NAS_ESM_WARN_LOG("NAS_ESM_GetAllEpsBearerInPdn:Input para ucEpsbId is not default bearer!");

        return NAS_ESM_FAILURE;
    }

    pucEspbId[*pucEpsBearNum] = ucEpsbId;
    (*pucEpsBearNum)++;

    /*ѭ��������Ϣ��������˳��ع�����ר�г���*/
    for( ucCnt = NAS_ESM_MIN_EPSB_ID; ucCnt <= NAS_ESM_MAX_EPSB_ID; ucCnt++ )
    {
        pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ucCnt);

        /*ĳһ���ص�Linked EPSB IDΪ��ǰ����ID*/
        if( (NAS_ESM_BEARER_TYPE_DEDICATED == pstEpsbCntxtInfo->enBearerCntxtType)
            && (NAS_ESM_BEARER_STATE_ACTIVE == pstEpsbCntxtInfo->enBearerCntxtState)\
            && (pstEpsbCntxtInfo->ulLinkedEpsbId == ucEpsbId) )
        {
            pucEspbId[*pucEpsBearNum] = ucCnt;
            (*pucEpsBearNum)++;
        }
    }

    return NAS_ESM_SUCCESS;
}


VOS_UINT32 NAS_ESM_IsExistSamePrecedenceInTft
(
    VOS_UINT32                          ulPfNum,
    const NAS_ESM_CONTEXT_TFT_STRU     *pstTFT
)
{
    VOS_UINT32                          ulCnt1 = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt2 = NAS_ESM_NULL;

    for (ulCnt1 = NAS_ESM_NULL;ulCnt1 < ulPfNum;ulCnt1++)
    {
        for (ulCnt2 = ulCnt1 + 1 ;ulCnt2 < ulPfNum;ulCnt2++)
        {
            if (pstTFT[ulCnt1].ucPrecedence == pstTFT[ulCnt2].ucPrecedence)
            {
                return NAS_ESM_SUCCESS;
            }
        }
    }

    return NAS_ESM_FAILURE;
}


VOS_VOID NAS_ESM_DHCPGetIPv4Mask
(
    const VOS_UINT8                    *pucIpv4Addr,
    VOS_UINT8                          *pucIpv4Mask
)
{
    /*lint -e961*/
    VOS_UINT32                          ulMask = 0xfffffffe;
    /*lint +e961*/
    VOS_UINT32                          ulIpTemp = 0;
    VOS_UINT32                          ulFirstBit;

    IP_GetUint32Data(ulIpTemp, pucIpv4Addr);
    ulFirstBit = ulIpTemp & 0x00000001;

    while (0x0 != ulIpTemp)
    {
        ulMask = ulMask << 1;
        ulIpTemp = ulIpTemp >> 1;

        if (ulFirstBit != (ulIpTemp & 0x00000001))
        {
            break;
        }
    }

    IP_SetUint32Data(pucIpv4Mask, ulMask);
}


VOS_VOID NAS_ESM_DHCPGetIPv4GateWay
(
    const VOS_UINT8                    *pucIpv4Addr,
    const VOS_UINT8                    *pucIpv4Mask,
    VOS_UINT8                          *pucIpv4GateWay
)
{
    VOS_UINT32                          ulMaskTemp = 0;
    VOS_UINT32                          ulIpTemp = 0;
    VOS_UINT32                          ulGateWay = 0;

    IP_GetUint32Data(ulIpTemp, pucIpv4Addr);
    IP_GetUint32Data(ulMaskTemp, pucIpv4Mask);

    ulGateWay = (ulIpTemp & ulMaskTemp) + 1;

    while (ulGateWay == ulIpTemp)
    {
        ulGateWay++;
    }

    IP_SetUint32Data(pucIpv4GateWay, ulGateWay);
}


VOS_VOID NAS_ESM_ClearSdfPara(VOS_UINT32  ulCid)
{
    NAS_ESM_SDF_PARA_STRU              *pstSdfPara       = VOS_NULL_PTR;

    pstSdfPara = NAS_ESM_GetSdfParaAddr(ulCid);

    NAS_ESM_MEM_SET(pstSdfPara, 0, sizeof(NAS_ESM_SDF_PARA_STRU));

    pstSdfPara->ulCId = ulCid;

    return;
}


VOS_VOID NAS_ESM_ClearNwCtrlSdfPara(VOS_VOID)
{
    VOS_UINT32                          ulCnt            = NAS_ESM_NULL;

    /*��ӡ��Ϣ*/
    NAS_ESM_INFO_LOG("NAS_ESM_ClearNwCtrlSdfPara: Clear Nw Control Cid Parameters!");

    /* ����CID 21~31,������о�̬��Ϣ */
    for (ulCnt = NAS_ESM_MIN_NW_CONTROL_CID; ulCnt < NAS_ESM_MAX_CID_NUM; ulCnt++)
    {
        NAS_ESM_ClearSdfPara(ulCnt);
    }

    return;
}

/*****************************************************************************
 Function Name   : NAS_ESM_Ipv4TransformPcoStru
 Description     : ת��PCO�ṹ��ʽ�����������ϱ�APP��PCO�ṹ�ͱ��ش洢PCO�ṹ�Ĳ�ͬ
 Input           : pstContPco  -- ���ش洢��PCO�ṹָ��
 Output          : pstTransPco-----ת����ʽ���PCO�ṹָ��
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-03-29  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_Ipv4TransformPcoStru
(
    const NAS_ESM_CONTEXT_PCO_STRU     *pstContPco,
    NAS_ESM_PCO_TRANSFORM_STRU         *pstTransPco
)
{
    VOS_UINT32                          ulCnt = NAS_ESM_NULL;

    pstTransPco->bitOpBcm = pstContPco->bitOpBcm;
    pstTransPco->enBcm = pstContPco->enBcm;

    /* ת��NBNS�ṹ */
    pstTransPco->ucNbnsNum = pstContPco->ucIpv4NbnsNum;
    for (ulCnt = NAS_ESM_NULL; ulCnt < pstContPco->ucIpv4NbnsNum; ulCnt++)
    {
        pstTransPco->astNbns[ulCnt].ucIpType = NAS_ESM_PDN_IPV4;

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astNbns[ulCnt].aucIpV4Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv4Nbns[ulCnt].aucIpV4Addr[0]), \
                            APP_MAX_IPV4_ADDR_LEN);
    }

    /* ת��DNS�ṹ */
    pstTransPco->ucDnsSerNum = pstContPco->ucIpv4DnsSerNum;
    for (ulCnt = NAS_ESM_NULL; ulCnt < pstContPco->ucIpv4DnsSerNum; ulCnt++)
    {
        pstTransPco->astDnsServer[ulCnt].ucIpType = NAS_ESM_PDN_IPV4;

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astDnsServer[ulCnt].aucIpV4Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv4DnsServer[ulCnt].aucIpV4Addr[0]), \
                            APP_MAX_IPV4_ADDR_LEN);
    }

    /* ת��PCSCF�ṹ */
    pstTransPco->ucPcscfNum = pstContPco->ucIpv4PcscfNum;
    for (ulCnt = NAS_ESM_NULL; ulCnt < pstContPco->ucIpv4PcscfNum; ulCnt++)
    {
        pstTransPco->astPcscf[ulCnt].ucIpType = NAS_ESM_PDN_IPV4;

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astPcscf[ulCnt].aucIpV4Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv4Pcscf[ulCnt].aucIpV4Addr[0]), \
                            APP_MAX_IPV4_ADDR_LEN);
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_Ipv6TransformPcoStru
 Description     : ת��PCO�ṹ��ʽ�����������ϱ�APP��PCO�ṹ�ͱ��ش洢PCO�ṹ�Ĳ�ͬ
 Input           : pstContPco  -- ���ش洢��PCO�ṹָ��
 Output          : pstTransPco-----ת����ʽ���PCO�ṹָ��
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-03-29  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_Ipv6TransformPcoStru
(
    const NAS_ESM_CONTEXT_PCO_STRU     *pstContPco,
    NAS_ESM_PCO_TRANSFORM_STRU         *pstTransPco
)
{
    VOS_UINT32                          ulCnt = NAS_ESM_NULL;

    pstTransPco->bitOpBcm = pstContPco->bitOpBcm;
    pstTransPco->enBcm = pstContPco->enBcm;

    /* ת��NBNS�ṹ */
    pstTransPco->ucNbnsNum = 0;


    /* ת��DNS�ṹ */
    pstTransPco->ucDnsSerNum = pstContPco->ucIpv6DnsSerNum;
    for (ulCnt = NAS_ESM_NULL; ulCnt < pstContPco->ucIpv6DnsSerNum; ulCnt++)
    {
        pstTransPco->astDnsServer[ulCnt].ucIpType = NAS_ESM_PDN_IPV6;

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astDnsServer[ulCnt].aucIpV6Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv6DnsServer[ulCnt].aucIpV6Addr[0]), \
                            APP_MAX_IPV6_ADDR_LEN);
    }

    /* ת��PCSCF�ṹ */
    pstTransPco->ucPcscfNum = pstContPco->ucIpv6PcscfNum;
    for (ulCnt = NAS_ESM_NULL; ulCnt < pstContPco->ucIpv6PcscfNum; ulCnt++)
    {
        pstTransPco->astPcscf[ulCnt].ucIpType = NAS_ESM_PDN_IPV6;

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astPcscf[ulCnt].aucIpV6Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv6Pcscf[ulCnt].aucIpV6Addr[0]), \
                            APP_MAX_IPV6_ADDR_LEN);
    }
}

/*****************************************************************************
 Function Name   : NAS_ESM_Ipv4v6TransformPcoStru
 Description     : ת��PCO�ṹ��ʽ�����������ϱ�APP��PCO�ṹ�ͱ��ش洢PCO�ṹ�Ĳ�ͬ
 Input           : pstContPco  -- ���ش洢��PCO�ṹָ��
 Output          : pstTransPco-----ת����ʽ���PCO�ṹָ��
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-03-29  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_Ipv4v6TransformPcoStru
(
    const NAS_ESM_CONTEXT_PCO_STRU     *pstContPco,
    NAS_ESM_PCO_TRANSFORM_STRU         *pstTransPco
)
{
    VOS_UINT32                          ulCnt = NAS_ESM_NULL;

    pstTransPco->bitOpBcm = pstContPco->bitOpBcm;
    pstTransPco->enBcm = pstContPco->enBcm;


    /* ת��NBNS�ṹ */
    pstTransPco->ucNbnsNum = pstContPco->ucIpv4NbnsNum;
    for (ulCnt = NAS_ESM_NULL; ulCnt < pstContPco->ucIpv4NbnsNum; ulCnt++)
    {
        pstTransPco->astNbns[ulCnt].ucIpType = NAS_ESM_PDN_IPV4;

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astNbns[ulCnt].aucIpV4Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv4Nbns[ulCnt].aucIpV4Addr[0]), \
                            APP_MAX_IPV4_ADDR_LEN);
    }

    /* ת��DNS�ṹ */
    for (ulCnt = 0; ulCnt < pstContPco->ucIpv4DnsSerNum; ulCnt++)
    {
        pstTransPco->astDnsServer[ulCnt].ucIpType = NAS_ESM_PDN_IPV4;

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astDnsServer[ulCnt].aucIpV4Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv4DnsServer[ulCnt].aucIpV4Addr[0]), \
                            APP_MAX_IPV4_ADDR_LEN);
    }

    for (ulCnt = 0; ulCnt < pstContPco->ucIpv6DnsSerNum; ulCnt++)
    {
        if (NAS_ESM_PDN_IPV4 == pstTransPco->astDnsServer[ulCnt].ucIpType)
        {
            pstTransPco->astDnsServer[ulCnt].ucIpType = NAS_ESM_PDN_IPV4_IPV6;
        }
        else
        {
            pstTransPco->astDnsServer[ulCnt].ucIpType = NAS_ESM_PDN_IPV6;
        }

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astDnsServer[ulCnt].aucIpV6Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv6DnsServer[ulCnt].aucIpV6Addr[0]), \
                            APP_MAX_IPV6_ADDR_LEN);
    }
    pstTransPco->ucDnsSerNum = ((pstContPco->ucIpv4DnsSerNum < pstContPco->ucIpv6DnsSerNum)
                                 ? pstContPco->ucIpv6DnsSerNum :  pstContPco->ucIpv4DnsSerNum);

    /* ת��PCSCF�ṹ */
    for (ulCnt = 0; ulCnt < pstContPco->ucIpv4PcscfNum; ulCnt++)
    {
        if(NAS_ESM_MAX_PCSCF_NUM == ulCnt)
        {
            break;
        }
        pstTransPco->astPcscf[ulCnt].ucIpType = NAS_ESM_PDN_IPV4;

        NAS_ESM_MEM_CPY(    pstTransPco->astPcscf[ulCnt].aucIpV4Addr,\
                            pstContPco->astIpv4Pcscf[ulCnt].aucIpV4Addr, \
                            APP_MAX_IPV4_ADDR_LEN);
    }

    for (ulCnt = 0; ulCnt < pstContPco->ucIpv6PcscfNum; ulCnt++)
    {
        if (NAS_ESM_PDN_IPV4 == pstTransPco->astPcscf[ulCnt].ucIpType)
        {
            pstTransPco->astPcscf[ulCnt].ucIpType = NAS_ESM_PDN_IPV4_IPV6;
        }
        else
        {
            pstTransPco->astPcscf[ulCnt].ucIpType = NAS_ESM_PDN_IPV6;
        }

        NAS_ESM_MEM_CPY(    (VOS_VOID*)&(pstTransPco->astPcscf[ulCnt].aucIpV6Addr[0]),\
                            (VOS_VOID*)&(pstContPco->astIpv6Pcscf[ulCnt].aucIpV6Addr[0]), \
                            APP_MAX_IPV6_ADDR_LEN);
    }
    pstTransPco->ucPcscfNum = ((pstContPco->ucIpv4PcscfNum < pstContPco->ucIpv6PcscfNum)
                                 ? pstContPco->ucIpv6PcscfNum :  pstContPco->ucIpv4PcscfNum);
}


/*****************************************************************************
 Function Name   : NAS_ESM_TransformPcoStru
 Description     : ת��PCO�ṹ��ʽ�����������ϱ�APP��PCO�ṹ�ͱ��ش洢PCO�ṹ�Ĳ�ͬ
 Input           : ucIpType -- PDN����
                   pstContPco  -- ���ش洢��PCO�ṹָ��
 Output          : pstTransPco-----ת����ʽ���PCO�ṹָ��
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2011-03-29  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_TransformPcoStru
(
    VOS_UINT8                           ucIpType,
    const NAS_ESM_CONTEXT_PCO_STRU           *pstContPco,
    NAS_ESM_PCO_TRANSFORM_STRU         *pstTransPco
)
{
    if (NAS_ESM_PDN_IPV4 == ucIpType)
    {
        NAS_ESM_Ipv4TransformPcoStru(pstContPco, pstTransPco);
    }
    else if (NAS_ESM_PDN_IPV6 == ucIpType)
    {
        NAS_ESM_Ipv6TransformPcoStru(pstContPco, pstTransPco);

    }
    else
    {
        NAS_ESM_Ipv4v6TransformPcoStru(pstContPco, pstTransPco);
    }
}

VOS_VOID NAS_ESM_ClearCid0StaticInfo(VOS_VOID)
{
    NAS_ESM_SDF_PARA_STRU          *pstBearCntxtPara  = VOS_NULL_PTR;

    /* CID0��PS�ںϺ�Ƚ����⣬��ESMά����������NV�����ݻ���ڲ�һ��, �轫CID0��ʼ��Ϊר������ */
    pstBearCntxtPara = NAS_ESM_GetSdfParaAddr(NAS_ESM_NULL);
    NAS_ESM_MEM_SET(pstBearCntxtPara, 0, sizeof(NAS_ESM_SDF_PARA_STRU));

    pstBearCntxtPara->bitOpLinkdCId         = NAS_ESM_OP_TRUE;
    pstBearCntxtPara->enBearerCntxtType     = NAS_ESM_BEARER_TYPE_DEDICATED;
    pstBearCntxtPara->ulLinkdCId            = 20;
}
VOS_UINT32 NAS_ESM_HasExistedEmergencyPdn(VOS_VOID)
{
    VOS_UINT32 ulEpsId = 0;
    NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsCtxInfo = VOS_NULL_PTR;

    for (ulEpsId = NAS_ESM_MIN_EPSB_ID; ulEpsId <= NAS_ESM_MAX_EPSB_ID; ulEpsId++)
    {
        pstEpsCtxInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsId);

        if ((NAS_ESM_OP_TRUE == pstEpsCtxInfo->bitOpCId) &&
            (NAS_ESM_BEARER_STATE_ACTIVE == pstEpsCtxInfo->enBearerCntxtState) &&
            (NAS_ESM_BEARER_TYPE_EMERGENCY == pstEpsCtxInfo->enBearerCntxtType))
        {
            return PS_TRUE;
        }
    }

    return PS_FALSE;
}


VOS_UINT32 NAS_ESM_HasEstingEmergencyPdn(VOS_VOID)
{
    VOS_UINT32 i = 0;
    NAS_ESM_STATE_INFO_STRU *pstStatTblInfo = VOS_NULL_PTR;

    for(i = 0; i < NAS_ESM_MAX_STATETBL_NUM; i++)
    {
        pstStatTblInfo = NAS_ESM_GetStateTblAddr(i);
        if ((NAS_ESM_ILL_CID != pstStatTblInfo->ulCid) &&
            (NAS_ESM_BEARER_TYPE_EMERGENCY == pstStatTblInfo->enBearerCntxtType))
        {
            return PS_TRUE;
        }
    }

    return PS_FALSE;
}


VOS_UINT32 NAS_ESM_HasEmergencyPdn(VOS_VOID)
{
    if (NAS_ESM_HasExistedEmergencyPdn() || NAS_ESM_HasEstingEmergencyPdn())
    {
        return PS_TRUE;
    }
    else
    {
        return PS_FALSE;
    }
}



VOS_UINT32 NAS_ESM_IsEmmEmcAttachedOrAttaching(VOS_VOID)
{
    if (((0 == NAS_ESM_GetCurPdnNum()) && (PS_TRUE == NAS_ESM_HasEstingEmergencyPdn())) ||
        ((1 == NAS_ESM_GetCurPdnNum()) && (PS_TRUE == NAS_ESM_HasExistedEmergencyPdn())))
    {
        return PS_TRUE;
    }
    else
    {
        return PS_FALSE;
    }
}


VOS_UINT32 NAS_ESM_IsDefaultEpsBearerType(VOS_UINT32 Type)
{
    if ((NAS_ESM_BEARER_TYPE_DEFAULT == Type) || (NAS_ESM_BEARER_TYPE_EMERGENCY == Type))
    {
        return PS_TRUE;
    }
    else
    {
        return PS_FALSE;
    }
}


VOS_UINT32 NAS_ESM_QueryStateTblIndexForEmc(VOS_UINT32 *pulStateIndex)
{
    VOS_UINT32                          ulCnt               = NAS_ESM_NULL;
    NAS_ESM_STATE_INFO_STRU            *pstStatTblnfo       = VOS_NULL_PTR;

    /*���� StateInfoTbl*/
    for(ulCnt = 0; ulCnt < NAS_ESM_MAX_STATETBL_NUM; ulCnt++)
    {
        pstStatTblnfo = NAS_ESM_GetStateTblAddr(ulCnt);
        if (PS_TRUE == pstStatTblnfo->stNwMsgRecord.ucIsEmcPdnReq)
        {
            *pulStateIndex = ulCnt;
            return NAS_ESM_SUCCESS;
        }
    }

    /*û�ж�Ӧ�Ľڵ�*/
    return NAS_ESM_FAILURE;
}



VOS_VOID  NAS_ESM_DeactAllNonEmcPdn(VOS_VOID)
{
    VOS_UINT32 ulCid = 0;
    VOS_UINT32 ulEpsId = 0;
    VOS_UINT32 ulStateTblIndex = 0;
    NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsCtxInfo = VOS_NULL_PTR;
    NAS_ESM_STATE_INFO_STRU      *pstStateTblInfo = VOS_NULL_PTR;
    APP_ESM_PDP_RELEASE_REQ_STRU  stAppPdpRelReq = {0};

    NAS_ESM_INFO_LOG("NAS_ESM_DeactAllNonEmcPdn: enter");

    /*�����̬����������س���*/
    NAS_ESM_ClearStateTable(APP_ERR_SM_DETACHED, PS_TRUE);

    /*�����зǽ���PDN���ӷ���PDN DISCONNECT REQUEST*/
    for (ulCid = 0; ulCid < NAS_ESM_MAX_CID_NUM; ulCid++)
    {
        if (NAS_ESM_SUCCESS == NAS_ESM_QueryEpsbCntxtTblByCid(ulCid, &ulEpsId))
        {
            pstEpsCtxInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsId);

            /*ֻ�м�������Ĭ�����͵ĳ��زŽ����ͷ�*/
            if ((pstEpsCtxInfo->enBearerCntxtState == NAS_ESM_BEARER_STATE_ACTIVE) &&
                (pstEpsCtxInfo->enBearerCntxtType == NAS_ESM_BEARER_TYPE_DEFAULT))
            {
                if (NAS_ESM_SUCCESS == NAS_ESM_GetStateTblIndexByCid(ulCid, &ulStateTblIndex))
                {
                    /*����APP��Ϣ��¼�ı�ʶλ*/
                    pstStateTblInfo = NAS_ESM_GetStateTblAddr(ulStateTblIndex);

                    /*���ó���״̬����𡢹���������Ϣ*/
                    pstStateTblInfo->ulEpsbId           = ulEpsId;
                    pstStateTblInfo->enBearerCntxtState = NAS_ESM_GetBearCntxtState(ulEpsId);
                    pstStateTblInfo->enBearerCntxtType  = NAS_ESM_GetBearCntxtType(ulEpsId);
                    pstStateTblInfo->ulLinkedEpsbId     = pstEpsCtxInfo->ulLinkedEpsbId;

                    /*׼��CID*/
                    stAppPdpRelReq.ulCid = ulCid;
                    /*lint -e961*/
                    NAS_ESM_INFO_LOG2("NAS_ESM_DeactAllNonEmcPdn:", ulCid, ulEpsId);
                    /*lint +e961*/
                    /*����PDN DISCONNECT REQUEST�տ���Ϣ*/
                    NAS_ESM_DefltAttMsgAppSdfRelReq((VOS_VOID*)&stAppPdpRelReq);
                }
            }
        }
    }
}
VOS_VOID  NAS_ESM_SndOmEsmTimerStatus(
    NAS_ESM_TIMER_RUN_STA_ENUM_UINT32        enTimerStatus,
    NAS_ESM_TIMER_PARA_ENUM_UINT8           enTimerId,
    VOS_UINT32                              ulTimerRemainLen
)
{
    NAS_ESM_TIMER_INFO_STRU            *pstMsg = VOS_NULL_PTR;

    pstMsg = (NAS_ESM_TIMER_INFO_STRU*)NAS_ESM_MEM_ALLOC(sizeof(NAS_ESM_TIMER_INFO_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        NAS_ESM_ERR_LOG("NAS_ESM_SndOmEsmTimerStatus: mem alloc fail!.");
        return;
    }

    pstMsg->stMsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->stMsgHeader.ulSenderPid     = PS_PID_ESM;
    pstMsg->stMsgHeader.ulReceiverPid   = PS_PID_ESM;
    pstMsg->stMsgHeader.ulLength        = sizeof(NAS_ESM_TIMER_INFO_STRU) - 20;

    pstMsg->stMsgHeader.ulMsgName       = enTimerId + PS_MSG_ID_ESM_TO_ESM_OM_BASE;
    pstMsg->enTimerStatus               = enTimerStatus;
    pstMsg->enTimerId                   = enTimerId;

    pstMsg->ulTimerRemainLen            = ulTimerRemainLen;

    (VOS_VOID)LTE_MsgHook((VOS_VOID*)pstMsg);

    NAS_ESM_MEM_FREE(pstMsg);

}
/*lint +e961*/
/*lint +e960*/
/*PC REPLAY MODIFY BY LEILI BEGIN*/

/*lint -e426*/
/*lint -e830*/
/*lint -e429*/
VOS_UINT32  NAS_ESM_GetSdfPara
(
    VOS_UINT32                         *pulSdfNum,
    APP_ESM_SDF_PARA_STRU              *pstSdfPara
)
{
    VOS_UINT32      ulRslt;
    #ifndef __PS_WIN32_RECUR__
    ulRslt = APP_GetSdfPara (pulSdfNum, pstSdfPara);

    NAS_LMM_LogSdfPara (pulSdfNum, pstSdfPara ,ulRslt);

    #else
    ulRslt = Stub_APP_GetSdfPara(pulSdfNum, pstSdfPara);
    #endif
    return ulRslt;
}
/*lint +e429*/
/*lint +e830*/
/*lint +e426*/


VOS_UINT32  NAS_ESM_GetPdpManageInfo
(
    APP_ESM_PDP_MANAGE_INFO_STRU  *pstPdpManageInfo
)
{
    VOS_UINT32      ulRslt;
    #ifndef __PS_WIN32_RECUR__
    ulRslt = APP_GetPdpManageInfo (pstPdpManageInfo);

    NAS_LMM_LogPdpManageInfo(pstPdpManageInfo,ulRslt);
    #else
    ulRslt = Stub_APP_GetPdpManageInfo(pstPdpManageInfo);
    #endif
    return ulRslt;
}
/*PC REPLAY MODIFY BY LEILI END*/
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of NasEsmPublic.c */
