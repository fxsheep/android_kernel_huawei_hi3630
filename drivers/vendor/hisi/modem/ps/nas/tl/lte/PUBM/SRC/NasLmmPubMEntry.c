

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasLmmPubMInclude.h"
#include    "NasLmmPubMTimer.h"
#include    "NasLmmTest.h"
#include    "NasCommBuffer.h"
#include    "NasEmmAttDetInclude.h"
#include    "NasEmmSmsInterface.h"
#include    "NasMmlCtx.h"
#include    "omerrorlog.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASMMPUBMENTRY_C
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
  3 Declare the local function
*****************************************************************************/
static  VOS_UINT32  NAS_LMM_PreProcRrcRelInd ( MsgBlock *        pMsg );
static  VOS_UINT32  NAS_LMM_PreProcMsgEsmEstReq     ( MsgBlock *        pMsg );
static  VOS_UINT32  NAS_LMM_PreProcIntraConn2IdleReq( MsgBlock *        pMsg );
static  VOS_UINT32  NAS_EMM_PreProcMsgEsmBearStatusReq( MsgBlock *     pMsg );
static  VOS_UINT32  NAS_EMM_PreProcMsgRrcErrInd( MsgBlock *            pMsg );
static VOS_UINT32  NAS_LMM_PreProcRrcRelCnf(    MsgBlock *          pMsg );
#if (FEATURE_PTM == FEATURE_ON)
VOS_UINT32 NAS_LMM_PreProcMsgEsmSaveErrlogInd(MsgBlock *     pMsg);
VOS_VOID  LNAS_InitErrLogGloInfo(LNAS_ERRLOG_GLO_INFO_STRU *pstErrlogGloInfo);
#endif



#ifdef PS_ITT_PC_TEST
extern  VOS_UINT32 PS_NVIM_Init();
#endif


NAS_LMM_COMM_ACT_STRU               g_astMmPreProcMap[] =
{
    /* MM_MSG_TYPE_ENUM_UINT32          NAS_LMM_COMM_ACTION_FUN */

    /*========== APP_MM_MSG ==========*/
    {ID_MMC_LMM_START_REQ,                         WUEPS_PID_MMC,              NAS_LMM_PreProcAppStartReq},
    {ID_MMC_LMM_STOP_REQ,                          WUEPS_PID_MMC,              NAS_LMM_PreProcAppStopReq},
    {ID_MMC_LMM_DETACH_REQ,                        WUEPS_PID_MMC,              NAS_LMM_PreProcAppDetReq},
    {ID_MMC_LMM_ATTACH_REQ,                        WUEPS_PID_MMC,              NAS_LMM_PreProcMsgAppMmAttachReq},

    /*========== SM_MM_MSG  ==========*/
    {ID_EMM_ESM_BEARER_STATUS_REQ,                 PS_PID_ESM,              NAS_EMM_PreProcMsgEsmBearStatusReq},
    /*leili modify for isr begin*/
    {ID_EMM_ESM_BEARER_MODIFY_REQ,                 PS_PID_ESM,              NAS_EMM_PreProcMsgEsmBearModifyReq},
    /*leili modify for isr end*/
    {ID_EMM_ESM_EST_REQ,                           PS_PID_ESM,              NAS_LMM_PreProcMsgEsmEstReq},

    {ID_EMM_ESM_CLR_ESM_PROC_RES_NOTIFY,           PS_PID_ESM,              NAS_LMM_PreProcMsgEsmClrEsmPrcResNotify},
    /*========== RRC_MM_MSG ==========*/
    {ID_LRRC_LMM_ERR_IND,                          PS_PID_ERRC,             NAS_EMM_PreProcMsgRrcErrInd},
    {ID_LRRC_LMM_REL_IND,                          PS_PID_ERRC,             NAS_LMM_PreProcRrcRelInd},
    {ID_LRRC_LMM_REL_CNF,                          PS_PID_ERRC,             NAS_LMM_PreProcRrcRelCnf},
    {ID_LRRC_LMM_SYS_CFG_CNF,                      PS_PID_ERRC,             NAS_LMM_PreProcRrcSysCfgCnf},
    {ID_LRRC_LMM_UTRAN_MODE_CNF,                   PS_PID_ERRC,             NAS_LMM_PreProcRrcUtranModeCnf},
    {ID_LRRC_LMM_DATA_CNF,                         PS_PID_ERRC,             NAS_LMM_PreProcRrcDataCnf},

    {ID_LRRC_LMM_SUSPEND_REL_CNF,                  PS_PID_ERRC,             NAS_EMM_PreProcMsgRrcSuspendRelCnf},

    {ID_LRRC_LMM_SUSPEND_INFO_CHANGE_IND,          PS_PID_ERRC,             NAS_EMM_PreProcMsgRrcSuspendInfoChangeInd },
    {ID_LRRC_LMM_PAGING_IND,                       PS_PID_ERRC,             NAS_EMM_PreProcMsgRrcPagingInd},

    /*========== MM_INTRA_MSG =========*/
    {ID_NAS_LMM_INTRA_EMMC_EMM_SYS_INFO_IND,        PS_PID_MM,              NAS_LMM_PreProcIntraSystemInfoInd},
    {ID_NAS_LMM_INTRA_EMMC_EMM_COVERAGE_LOST_IND,   PS_PID_MM,              NAS_LMM_PreProcIntraCoverageLostInd},
    {ID_NAS_LMM_INTRA_CONN2IDLE_REQ,                PS_PID_MM,              NAS_LMM_PreProcIntraConn2IdleReq},
    {ID_NAS_LMM_INTRA_TAU_REQ,                      PS_PID_MM,              NAS_LMM_PreProcIntraTauReq},


    /*========== MM_USIM_CNF ==========*/
    {PS_USIM_GET_FILE_RSP,                         WUEPS_PID_USIM,          NAS_LMM_MsNullSsNullReadingUsimMsgUsimCnf},
    {ID_MMC_LMM_USIM_STATUS_REQ,                   WUEPS_PID_MMC,           NAS_LMM_PreProcMsgUsimStatusInd},
    {NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_LMM_TIMER_WAIT_USIM_CNF),
                                                   VOS_PID_TIMER,           NAS_LMM_MsNullSsNullReadingUsimTimerExp},
    {PS_USIM_SET_FILE_RSP,                         WUEPS_PID_USIM,          NAS_LMM_PreProcUsimSetCnf},

    /*========== TIMER EXP ==========*/
    {NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_EMM_PTL_T3416),
                                                    VOS_PID_TIMER,          NAS_EMM_PreProcMsgT3416Exp},
    {NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_EMM_PTL_SWITCH_OFF_TIMER),
                                                    VOS_PID_TIMER,          NAS_EMM_PreProcMsgSwithOffTimerExp},

    {NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_EMM_STATE_DEL_FORB_TA_PROID),
                                                    VOS_PID_TIMER,          NAS_EMM_PreProcMsgDelForbTaTimerExpire},
    {NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_EMM_STATE_WAIT_SYSCFG_CNF_TIMER),
                                                    VOS_PID_TIMER,          NAS_EMM_PreProcMsgWaitSyscfgCnfTimerExpire},
    /*leili modify for isr begin*/
    {NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_EMM_PTL_T3412),
                                                    VOS_PID_TIMER,          NAS_EMM_PreProcMsgT3412Exp},
    {NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_EMM_PTL_T3423),
                                                    VOS_PID_TIMER,          NAS_EMM_PreProcMsgT3423Exp},
    /*leili modify for isr end*/
	{NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_EMM_PTL_T3402),
                                                    VOS_PID_TIMER,          NAS_EMM_PreProcMsgT3402Exp},

    {NAS_LMM_TIMER_EXP_MSG_ET(TI_NAS_EMM_PTL_CSFB_DELAY), VOS_PID_TIMER,    NAS_EMM_PreProcMsgCsfbDelayTimerExp},

    /*========== CN MSG ==========*/
    {ID_LRRC_LMM_DATA_IND,                         PS_PID_ERRC,             NAS_LMM_PreProcrRrcDataInd},

    /*========== GMM =============*/
    {ID_GMM_LMM_RESEL_SECU_INFO_REQ,               WUEPS_PID_GMM,               NAS_LMM_PreProcReselSecuInfoReq},
    {ID_GMM_LMM_HO_SECU_INFO_REQ,                  WUEPS_PID_GMM,               NAS_LMM_PreProcHoSecuInfoReq},
    {ID_GMM_LMM_INFO_CHANGE_NOTIFY_REQ,            WUEPS_PID_GMM,               NAS_LMM_PreProcInfoChangeNotifyReq},

    /*========== MMC =============*/
    {ID_MMC_LMM_REL_REQ,                           WUEPS_PID_MMC,           NAS_LMM_PreProcMmcRelReq},
    {ID_MMC_LMM_SYS_CFG_REQ,                       WUEPS_PID_MMC,           NAS_LMM_PreProcMmcSysCfgReq},
    {ID_MMC_LMM_UTRAN_MODE_REQ,                    WUEPS_PID_MMC,           NAS_LMM_PreProcMmcUtranModeReq},
    {ID_MMC_LMM_DISABLE_LTE_NOTIFY,                WUEPS_PID_MMC,           NAS_LMM_PreProcMmcDisableReq},
    {ID_MMC_LMM_ENABLE_LTE_NOTIFY,                 WUEPS_PID_MMC,           NAS_LMM_PreProcMmcEnableReq},

    {ID_MMC_LMM_UE_OOC_STATUS_NOTIFY,              WUEPS_PID_MMC,           NAS_LMM_PreProcMmcOocStateReq},

    {ID_MMC_LMM_IMS_VOICE_CAP_CHANGE_NOTIFY,       WUEPS_PID_MMC,           NAS_LMM_PreProcMmcImsVoiceCapChangeNotify},

    {ID_MMC_LMM_VOICE_DOMAIN_CHANGE_IND,           WUEPS_PID_MMC,           NAS_LMM_PreProcMmcImsVoiceDomainChangeInd},
    {ID_MMC_LMM_CS_CONN_STATUS_NOTIFY,              WUEPS_PID_MMC,           NAS_LMM_PreProcMmcCsConnStatusNotify},
    /*========== SUSPEND =========*/
    {ID_MMC_LMM_SUSPEND_REQ,                       WUEPS_PID_MMC,           NAS_EMM_PreProcMsgMmcSuspendReq},
    {ID_LRRC_LMM_SUSPEND_IND,                      PS_PID_ERRC,             NAS_EMM_PreProcMsgRrcSuspendInd},
    {ID_MMC_LMM_ACTION_RESULT_REQ,                 WUEPS_PID_MMC,           NAS_EMM_PreProcMsgMmcActionRstReq},

    /*========== ERABM ==============*/
    {ID_EMM_ERABM_DRB_SETUP_IND,                   PS_PID_RABM,             NAS_EMM_PreProcMsgRabmDrbSetupInd},

    /*========== SMS ==============*/
    {ID_SMS_LMM_EST_REQ,                           WUEPS_PID_SMS,           NAS_EMM_PreProcMsgSmsEstReq},
    {ID_SMS_LMM_DATA_REQ,                          WUEPS_PID_SMS,           NAS_EMM_PreProcMsgSmsDataReq},

    /*========== MM ==============*/
    {ID_MM_LMM_CSFB_SERVICE_START_NOTIFY,          WUEPS_PID_MM,            NAS_EMM_PreProcMsgMmCsfbSerStartNotify},
    {ID_MM_LMM_CSFB_SERVICE_ABORT_NOTIFY,          WUEPS_PID_MM,            NAS_EMM_PreProcMsgMmCsfbSerAbortNotify},

    {ID_MM_LMM_HO_SECU_INFO_REQ,                   WUEPS_PID_MM,            NAS_LMM_PreProcMsgMmHoSecuInfoReq},

    {ID_MM_LMM_SRVCC_STATUS_NOTIFY,                WUEPS_PID_MM,           NAS_LMM_PreProcMsgSrvccNofify},

    /*========== GMM ==============*/
    {ID_GMM_LMM_TIMER_STATE_NOTIFY,          WUEPS_PID_GMM,            NAS_EMM_PreProcMsgGmmTimerStateNotifyMsg},

    {ID_OM_EMM_INFO_REPORT_REQ,                  MSP_PID_DIAG_APP_AGENT,  NAS_EMM_OmInfoIndMsgProc },

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    /*========== OM ==============*/
    #if (FEATURE_PTM == FEATURE_ON)
    {ID_OM_FTM_CTRL_IND,                           ACPU_PID_OM,             NAS_LMM_RevOmFtmCtrlMsg},
    {ID_OM_LNAS_ERR_LOG_CTRL_IND,                  ACPU_PID_OM,             NAS_LMM_RevOmErrlogCtrlMsg},
    {ID_OM_LNAS_ERR_LOG_REPORT_REQ,                ACPU_PID_OM,             NAS_LMM_RevOmReadErrlogReq},
    {ID_EMM_ESM_SAVE_ERRLOG_IND,                   PS_PID_ESM,              NAS_LMM_PreProcMsgEsmSaveErrlogInd},
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog end   */
    {ID_MMC_LMM_OTHER_MODEM_INFO_NOTIFY,          WUEPS_PID_MMC,           NAS_LMM_RevMmcModemInfoReq},
};

static VOS_UINT32   g_astMmPreProcMapNum
        = sizeof(g_astMmPreProcMap)/sizeof(NAS_LMM_COMM_ACT_STRU);
VOS_UINT32 g_ulSendMsgCounter = 0;
NAS_LMM_USIM_FILE_TYPE_ENUM_UINT32 g_enReadUsimFileType;
/*****************************************************************************
  3 Function
*****************************************************************************/


/*****************************************************************************
 Function Name  : MM_FidInit
 Discription    : mm��FID��ʼ������
 Input          :
 Output         : None
 Return         : None
 History:
      1.  ��³�� 41410  yyyy-mm-dd  Draft Enact
*****************************************************************************/
VOS_UINT32 MM_FidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    VOS_UINT32                              ulRet = 0;

    (VOS_VOID)ulRet;
    (VOS_VOID)ip;
    #if (VOS_OS_VER == VOS_WIN32)/*��PC�����£��Ż����PID��ʼ�����������廷���£���GU MM�����ʼ��*/
    switch( ip )
    {
        case    VOS_IP_LOAD_CONFIG:

                ulRet = VOS_RegisterPIDInfo(PS_PID_MM,
                                            NAS_MM_PidInit,
                                            NAS_MM_PidMsgEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }

                ulRet = VOS_RegisterPIDInfo(WUEPS_PID_MMC,
                                            NAS_MMC_PidInit,
                                            NAS_MmcStub_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }

                ulRet = VOS_RegisterPIDInfo(WUEPS_PID_GMM,
                                            NAS_GMM_PidInit,
                                            NAS_GmmStub_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }

                ulRet = VOS_RegisterPIDInfo(WUEPS_PID_MM,
                                            NAS_GUMM_PidInit,
                                            NAS_GUMmStub_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }

                ulRet = VOS_RegisterPIDInfo(WUEPS_PID_SMS,
                                            NAS_SMS_PidInit,
                                            NAS_SmsStub_TaskEntry);
                if( VOS_OK != ulRet )
                {
                    return VOS_ERR;
                }
                ulRet = VOS_RegisterMsgTaskPrio(PS_FID_MM, VOS_PRIORITY_M4);
                if( PS_SUCC != ulRet )
                {
                    return ulRet;
                }
                break;

        case    VOS_IP_FARMALLOC:
        case    VOS_IP_INITIAL:
        case    VOS_IP_ENROLLMENT:
        case    VOS_IP_LOAD_DATA:
        case    VOS_IP_FETCH_DATA:
        case    VOS_IP_STARTUP:
        case    VOS_IP_RIVAL:
        case    VOS_IP_KICKOFF:
        case    VOS_IP_STANDBY:
        case    VOS_IP_BROADCAST_STATE:
        case    VOS_IP_RESTART:
        case    VOS_IP_BUTT:
                break;

        default:
                break;
    }
    #endif

    return VOS_OK;
}

/*****************************************************************************
 Function Name  : EMM_PidInit
 Discription    : emm��PID��ʼ������
 Input          :
 Output         : None
 Return         : None
 History:
      1.  ��³�� 41410  yyyy-mm-dd  Draft Enact
*****************************************************************************/
VOS_UINT32 NAS_MM_PidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{

    switch( ip )
    {
        case    VOS_IP_LOAD_CONFIG:
                {
                    /*NAS_EMMC_FsmReg();*/
                    NAS_EMM_FsmReg();
                    NAS_LMM_SECU_FsmReg();

                    /*���� NVIM ׮�ļ�����ȡ�ļ�����*/
                    #ifdef PS_ITT_PC_TEST
                        PS_NVIM_Init();
                    #endif
                }
                break;

        case    VOS_IP_FARMALLOC:
                break;
        case    VOS_IP_INITIAL:
                {
                    NAS_LMM_PUBM_LOG_NORM(
                    "===============================================");
                    NAS_LMM_PUBM_LOG_NORM(
                    "NAS_LMM                            START INIT...");
                    /*PUBM,MMC,EMMģ���ʼ��,EMM��ʼ��������MMC֮ǰ*/
                    NAS_LMM_PUBM_Init_FidInit();
                }
                break;
        case    VOS_IP_ENROLLMENT:
        case    VOS_IP_LOAD_DATA:
        case    VOS_IP_FETCH_DATA:
        case    VOS_IP_STARTUP:
        case    VOS_IP_RIVAL:
        case    VOS_IP_KICKOFF:
        case    VOS_IP_STANDBY:
        case    VOS_IP_BROADCAST_STATE:
        case    VOS_IP_RESTART:
        case    VOS_IP_BUTT:
                break;

        default:
                break;
    }

    return VOS_OK;
}


/*****************************************************************************
 Function Name  :
 Discription    : NAS_LMM ע���dopra����Ϣ��������
 Input          :
 Output         : None
 Return         : None
 History:
      1.  Name+ID  yyyy-mm-dd  Draft Enact
*****************************************************************************/
void    NAS_MM_PidMsgEntry( MsgBlock *pMsg)
{
    VOS_UINT32                          ulRst;
    /*�����ϢΪ��*/
    if(NAS_LMM_NULL_PTR == pMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_MM_PidMsgEntry: BUF_MSG is NULL_PTR");
        return;
    }

    /* ���� ���� NAS_LMM_Entry   */
    ulRst = NAS_LMM_Entry(pMsg);
/*    NAS_LMM_PUBM_LOG1_INFO("NAS_MM_PidMsgEntry: ulRst = ", ulRst);*/
    (VOS_VOID)(ulRst);

    return;
}



/*****************************************************************************
 Function Name  : NAS_LMM_Entry
 Discription    : NAS_LMM����ں���,����MMCģ���EMMģ��
 Input          :
 Output         : None
 Return         : ��ɸ���Ϣ�����̴���
                  ����Ϣ��Ҫ����
                  ��������Ϣ
 History:
      1.  Name+ID  yyyy-mm-dd  Draft Enact
*****************************************************************************/
VOS_UINT32 NAS_LMM_Entry(MsgBlock *pMsg)
{
    VOS_UINT32                          ulRst;
    NAS_EMM_EVENT_TYPE_ENUM_UINT32      ulEt;

    /*����ǿտ���Ϣ��Ᵽ����������Ϣ������*/
    ulRst = NAS_EMM_SecuUnProtect(pMsg);
    if(NAS_EMM_NOT_GO_INTO_EMM_FSM == ulRst)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_Entry:UnProt Err OR SMC Finish.");
        return ulRst;
    }

    /* ���ڴ������յ�����Ϣ֮ǰ���Ȼ�ȡEventType */
    ulRst = NAS_LMM_GetEventType(pMsg, &ulEt);
    if(NAS_EMM_FAIL == ulRst)
    {
        NAS_LMM_PUBM_LOG_WARN("NAS_LMM_Entry: WARN, Get Event Type ERR !!");
        return  ulRst;
    }
    /* ������Ϣ����,���ڸ�λ���� */
    NAS_EMM_SaveRecvMsgList((VOS_VOID *)pMsg);

    /* ����MMC��EMM״̬������ */
    ulRst = NAS_LMM_MsgHandle(pMsg, ulEt);

    /* Ȼ��MM�ڲ���Ϣ���� */
    ulRst = NAS_LMM_IntraMsgProcess();

    /*��ʼbuff�Ĵ���*/
    ulRst = NAS_LMM_BufMsgProcess();


    /* ���EMMģ���ά����Ϣ�����仯������������ϱ�EMM��Ϣ��ȫ�ֱ�����
    ͬʱ�����µ���Ϣ�ϱ� */
    if (OM_EMM_REPORT_INFO_OPEN == g_NasEmmOmInfoIndFlag )
    {
        if (NAS_EMM_FAIL == NAS_LMM_CompareEmmInfo())
        {
            /* ͨ��DIAG�ϱ� */
            #if (VOS_WIN32 != VOS_OS_VER)
            (VOS_VOID)DIAG_ReportCommand(DIAG_CMD_EMM_APP_EMM_INFO_IND, sizeof(OM_EMM_STATE_INFO_STRU), (VOS_VOID*)&g_stEmmOmInfo);
            #endif
        }
    }

    /*EMMģ����Ҫ�ϱ�·�����Ϣ�б�Ļ����������ϱ�*/
    NAS_LMM_DtJudgeifNeedRpt();
    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_LMM_OmInfoIndProc();
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog end   */

    return  ulRst;
}



VOS_UINT32  NAS_LMM_MsgHandle(           MsgBlock *          pMsg,
                                        VOS_UINT32          ulEventType)
{
    VOS_UINT32                          ulRst;
    VOS_UINT32                          ulProcessFlag;

    /* MM��Ԥ���� */
    ulRst = NAS_LMM_MsgPreProcess(pMsg);

    /* ��״̬���д������ָ���Ϣ��Ҫ���棬�򻺴� */
    if( (NAS_LMM_STORE_HIGH_PRIO_MSG == ulRst) ||
        (NAS_LMM_STORE_MID_PRIO_MSG  == ulRst) ||
        (NAS_LMM_STORE_LOW_PRIO_MSG  == ulRst))
    {

        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_MsgHandle: PreProcess Store Msg !!");
        ulProcessFlag               = ulRst;
        ulRst = NAS_LMM_StoreMsgToQue(NAS_LMM_PARALLEL_FSM_EMM,
                                        ulProcessFlag,
                                        ulEventType,
                                        (VOS_VOID *)pMsg);
    }

    /*�������Ϣ��Ԥ����ģ�鴦����,���ٽ���EMMģ�鴦��*/
    if ( NAS_LMM_MSG_DISCARD == ulRst)
    {
        ulRst = NAS_EMM_MsgProcessInFsm(pMsg, ulEventType );

        /* ��״̬���д������ָ���Ϣ��Ҫ���棬�򻺴� */
        if( (NAS_LMM_STORE_HIGH_PRIO_MSG == ulRst) ||
            (NAS_LMM_STORE_MID_PRIO_MSG  == ulRst) ||
            (NAS_LMM_STORE_LOW_PRIO_MSG  == ulRst))
        {
            ulProcessFlag               = ulRst;
            ulRst = NAS_LMM_StoreMsgToQue(NAS_LMM_PARALLEL_FSM_EMM,
                                            ulProcessFlag,
                                            ulEventType,
                                            (VOS_VOID *)pMsg);
        }
    }

    /*�������Ϣ��EMMģ�鴦����,���ٽ���MMCģ�鴦��*/
    if( NAS_LMM_MSG_DISCARD == ulRst)
    {
        ulRst = NAS_EMMC_MsgDistr(pMsg);

        #if 0

        /* ��״̬���д������ָ���Ϣ��Ҫ���棬�򻺴� */
        if( (NAS_LMM_STORE_HIGH_PRIO_MSG == ulRst) ||
            (NAS_LMM_STORE_MID_PRIO_MSG  == ulRst) ||
            (NAS_LMM_STORE_LOW_PRIO_MSG  == ulRst))
        {
            ulProcessFlag               = ulRst;
            ulRst = NAS_LMM_StoreMsgToQue(NAS_LMM_PARALLEL_FSM_MMC,
                                            ulProcessFlag,
                                            ulEventType,
                                            (VOS_VOID *)pMsg);
        }
        #endif
    }

    /* ��ӡ��ʾ��ǰ�յ�����Ϣû�б�EMM����
    */
    if ( NAS_LMM_MSG_DISCARD == ulRst)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_MsgHandle: Current Receive Message Is Discarded !!");
    }
    return ulRst;
}





/*lint -e960*/
/*lint -e961*/
VOS_UINT32  NAS_LMM_GetEventType(        MsgBlock                        *pMsg,
                                        NAS_EMM_EVENT_TYPE_ENUM_UINT32  *pulEmmEt )
{
    REL_TIMER_MSG                      *pTimerMsg;
    NAS_LMM_PID_MSG_STRU                *pMmPidMsg;
    LRRC_LMM_DATA_IND_STRU               *pRrcMmDataIndMsg;
    VOS_UINT32                          ulMmCnMsgType;


    pMmPidMsg                           = (NAS_LMM_PID_MSG_STRU *)pMsg;
    pRrcMmDataIndMsg                    = (LRRC_LMM_DATA_IND_STRU *)pMsg;


    /* ������ģ�鲻��MM */
    if( PS_PID_MM != pMmPidMsg->ulReceiverPid)
    {

        /* �����ǳ�ʱ��Ϣ����ӡRCV_PID, PS_PID_MM, ����ϢID */
        if( VOS_PID_TIMER != pMmPidMsg->ulSenderPid)
        {
            NAS_LMM_PUBM_LOG3_ERR("NAS_LMM_GetEventType, Rcv_pid err!! Rcv_pid =, PS_PID_MM =, MsgId = ",
                                            pMsg->ulReceiverPid,
                                            PS_PID_MM,
                                            pMmPidMsg->ulMsgId);
        }
        else
        {   /* ���򣬴�ӡRCV_PID, PS_PID_MM,  */
            NAS_LMM_PUBM_LOG2_ERR("NAS_LMM_GetEventType, TIMER MSG Rcv_pid err!! Rcv_pid =, PS_PID_MM =",
                                            pMsg->ulReceiverPid,
                                            PS_PID_MM);
        }

        return  NAS_EMM_FAIL;
    }

    /*�����ǰMM��MMC����״̬����״̬*/
    NAS_LMM_PUBM_LOG_NORM("\n");
    NAS_LMM_PUBM_LOG_NORM("======NAS_EMM Current State:======");
    NAS_LMM_PrintFsmState(NAS_LMM_PARALLEL_FSM_EMM);
    /*NAS_LMM_PrintFsmState(NAS_LMM_PARALLEL_FSM_MMC);*/

    /* ��ӡ��ϢID */
    NAS_LMM_PUBM_PrintRevMsg((const PS_MSG_HEADER_STRU *)pMmPidMsg,
                             NAS_COMM_GET_MM_PRINT_BUF());



    /*��ӡ��Ϣ����*/
    NAS_LMM_PUBM_LOG_NORM("====================IN MSG CONTENT is : ============");
    NAS_COMM_PrintArray(             NAS_COMM_GET_MM_PRINT_BUF(),
                                     (VOS_UINT8 *)pMmPidMsg,
                                     (pMmPidMsg->ulLength + NAS_EMM_LEN_VOS_MSG_HEADER));
    NAS_LMM_PUBM_LOG_NORM("====================================================");

    /* ���ǳ�ʱ��Ϣ */
    if( VOS_PID_TIMER == pMmPidMsg->ulSenderPid)
    {

        pTimerMsg                       = (REL_TIMER_MSG *)pMsg;
        (*pulEmmEt)                     = NAS_LMM_TIMER_EXP_MSG_ET(pTimerMsg->ulName);

    }
    else
    {
        /* �����յ���Ϣ, pMmPidMsg�е�BYTE4 ��Ϊ0 */
        *pulEmmEt = pMmPidMsg->ulMsgId  & NAS_LMM_MSGID_DPID_POS;

        /* ����RRC������ ����CN��Ϣ����Ϣ����pMmPidMsg�е�BYTE4��ΪCNMT: CN Msg Type */
        if(ID_LRRC_LMM_DATA_IND == pMmPidMsg->ulMsgId)
        {

            /* ��ȡCN ��MSG_TYPE */
            ulMmCnMsgType   = pRrcMmDataIndMsg->stNasMsg.aucNasMsg[NAS_LMM_CN_MSG_MT_POS];

            /* ��pMmPidMsg�е�BYTE4��ΪCNMT: CN Msg Type */
            ulMmCnMsgType   = ulMmCnMsgType<<NAS_LMM_MOVEMENT_24_BITS;
            (*pulEmmEt)     = (*pulEmmEt)| ulMmCnMsgType;

            /* ���� ID_LRRC_LMM_DATA_IND ��Ϣ����ӡ CN��Ϣ���� */
            NAS_LMM_PrintCnMsgAndData(&(pRrcMmDataIndMsg->stNasMsg));
/*            NAS_LMM_PrintCnMsg(pRrcMmDataIndMsg);*/
        }

        /*������ڲ��տ���Ϣ��αװΪ�����Ŀտ���Ϣ*/
        if(ID_NAS_LMM_INTRA_DATA_IND == pMmPidMsg->ulMsgId)
        {
            *pulEmmEt = ID_LRRC_LMM_DATA_IND & NAS_LMM_MSGID_DPID_POS;

            /* ��ȡCN ��MSG_TYPE */
            ulMmCnMsgType   = pRrcMmDataIndMsg->stNasMsg.aucNasMsg[NAS_LMM_INTRA_DATA_IND_MT_POS];

            /* ��pMmPidMsg�е�BYTE4��ΪCNMT: CN Msg Type */
            ulMmCnMsgType   = ulMmCnMsgType<<NAS_LMM_MOVEMENT_24_BITS;
            (*pulEmmEt)     = (*pulEmmEt)| ulMmCnMsgType;
        }

    }
    /*����Ҫ��Ϣ��������Ϣ�ٿ�һ��*/
/*    NAS_LMM_PUBM_LOG_INFO("\n");*/

    /*
    NAS_LMM_PUBM_LOG_INFO(".......................................................................................................................................................");
    NAS_LMM_PrintFsmState(NAS_LMM_PARALLEL_FSM_EMM);
    NAS_LMM_PrintFsmState(NAS_LMM_PARALLEL_FSM_MMC);
    NAS_LMM_PUBM_LOG_INFO(".......................................................................................................................................................");
    */
    return  NAS_EMM_SUCC;
}



VOS_VOID    NAS_LMM_SendRabmRrcConRelInd(VOS_VOID)
{
    EMM_ERABM_RRC_CON_REL_IND_STRU      *pRabmRrcConRelIndMsg    = NAS_EMM_NULL_PTR;

    NAS_LMM_PUBM_LOG_NORM( "NAS_LMM_SendRabmRrcConRelInd is entered.");

    /* ����DOPRA��Ϣ */
    pRabmRrcConRelIndMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ERABM_RRC_CON_REL_IND_STRU));

    if(NAS_EMM_NULL_PTR == pRabmRrcConRelIndMsg)
    {
        NAS_LMM_PUBM_LOG_ERR( "NAS_LMM_SendRabmRrcConRelInd: MSG ALLOC ERR !!");
        return;
    }

    /* ��дEMM_RABM_RRC_CON_REL_IND ��DOPRA��Ϣͷ */
    EMM_COMP_ERABM_MSG_HEADER(pRabmRrcConRelIndMsg, sizeof(EMM_ERABM_RRC_CON_REL_IND_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    /* ��дEMM_RABM_RRC_CON_REL_IND ����ϢID��ʶ */
    pRabmRrcConRelIndMsg->ulMsgId           = ID_EMM_ERABM_RRC_CON_REL_IND;

    /* ������Ϣ(Send the msg of) EMM_RABM_RRC_CON_REL_IND */
    NAS_LMM_SEND_MSG(                       pRabmRrcConRelIndMsg);

    return;

}


VOS_VOID  NAS_EMM_TcSendRelInd(VOS_VOID)
{
    EMM_ETC_REL_IND_STRU                *pEmmTcRelIndMsg = VOS_NULL_PTR;

    pEmmTcRelIndMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(EMM_ETC_REL_IND_STRU));


    /*�ж�����������ʧ�ܴ�ӡ�����˳�*/
    if (NAS_EMM_NULL_PTR == pEmmTcRelIndMsg)
    {
        /*��ӡ����*/
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_TcSendRelInd: MSG ALLOC ERR!");
        return;

    }

    /*����ID_EMM_ETC_REL_IND��Ϣ*/
    /*�����Ϣͷ*/
    NAS_EMM_COMP_AD_TC_MSG_HEADER(pEmmTcRelIndMsg, sizeof(EMM_ETC_REL_IND_STRU) -
                                        EMM_LEN_VOS_MSG_HEADER);

    /*�����ϢID*/
    pEmmTcRelIndMsg->ulMsgId         = ID_EMM_ETC_REL_IND;


    /*��TC����ID_EMM_ETC_REL_IND��Ϣ*/
    NAS_LMM_SEND_MSG(                   pEmmTcRelIndMsg);

    /* ��ӡ������Ϣ */
    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_TcSendRelInd:NORM:Send ID_EMM_ETC_REL_IND!");
}
static VOS_UINT32  NAS_LMM_PreProcIntraConn2IdleReq( MsgBlock *          pMsg )
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcIntraConn2IdleReq is enter!");

    ( VOS_VOID )pMsg;

    /*֪ͨ RABM:RRC_CONN_REL_IND*/
    NAS_LMM_SendRabmRrcConRelInd();

    /*��鵱ǰ״̬������T3412*/
    if(EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
    {
        NAS_LMM_StartInactivePtlTimer(TI_NAS_EMM_PTL_T3412);

        /*leili modify for isr begin*/
        /*֪ͨGUģT3412ֹͣ*/
        NAS_EMM_SendTimerStateNotify(TI_NAS_EMM_PTL_T3412, LMM_MMC_TIMER_START);
        /*leili modify for isr end*/
    }

    /*ֹͣT3416��ɾ��RAND��RES*/
    if(EMM_MS_DEREG == NAS_LMM_GetEmmCurFsmMS())
    {
        NAS_LMM_StopPtlTimer(            TI_NAS_EMM_PTL_T3416);
        NAS_EMM_SecuClearRandRes();
    }

    /*���氲ȫ�����ĺ�PSLOC(GUTI,LVR TAI,EU)��Ϣ*/
    /*NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_UPDATE);*/
    /* lihong00150010 emergency tau&service begin */
    /*NAS_LMM_WritePsLoc(NAS_NV_ITEM_UPDATE);*/
	/* lihong00150010 emergency tau&service end */

    NAS_EMM_ClrAllUlDataReqBufferMsg();

    return NAS_LMM_MSG_HANDLED;
}
static VOS_UINT32  NAS_LMM_PreProcRrcRelInd(    MsgBlock *          pMsg )
{
    VOS_UINT32                          ulRet = NAS_LMM_MSG_DISCARD;
    LRRC_LMM_REL_IND_STRU              *pLrrcRelInd;

    pLrrcRelInd = (LRRC_LMM_REL_IND_STRU*)pMsg;
    /* ��LRRC�ͷ�ʱ�ر�CIPHERʱ��ת�Ƶ�LMM�ͷ�ʱ�ر�:��������������ȼ��ͣ���LMM��ʹ��CIPHER��ʱ��
        ������ռ��LRRC��CIPHERʱ�ӹرգ�����LMM DATA ABORT���� */
    #if (VOS_OS_VER != VOS_WIN32)
    /*�͹��ķ���Ϊ��ʡ���ģ���LTEģLNAS�յ�LRRC�ͷŵ�ʱ��������ӿڹص� cipher ʱ���Լ�ͶƱ�Ƿ��pll��
     Э��ջֻ������һ���ӿڣ��汾����֧�ɵ����ڴ˽ӿ��ڲ����з�װ*/
    (VOS_VOID)BSP_CIPHER_Disable();
    #endif
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    /* ����ͷ�ԭ��ֵ��CSFB�����ȼ������¼��MML��GU NASʹ�ã��˱�ʶ��GU������� */
    /*lint -e40*/
    if(LRRC_LNAS_REL_CAUSE_CSFB_HIGH_PRIOR == pLrrcRelInd->enRelCause)
    /*lint +e40*/
    {
        NAS_MML_SetRelCauseCsfbHighPrioFlg(VOS_TRUE);
    }
    else
    {
        NAS_MML_SetRelCauseCsfbHighPrioFlg(VOS_FALSE);
    }


    /* ��LMM�ǹ�������У����Ѿ�����Ҫ��ERABM֪ͨһ�£�*/
    if(   (NAS_LMM_CUR_LTE_SUSPEND == NAS_EMM_GetCurLteState())
       || (EMM_MS_SUSPEND == NAS_LMM_GetEmmCurFsmMS())
       || (EMM_MS_RESUME  == NAS_LMM_GetEmmCurFsmMS()))
    {

        if (NAS_EMM_GetConnState()!= NAS_EMM_CONN_IDLE)
        {
            /* �л���CCO ���������У��������ɺ󣬻������˹����У�����IDLE̬ */
            NAS_EMM_CommProcConn2Ilde();
        }

        /*NAS_EMM_MrrcChangeRrcStatusToIdle();
        NAS_LMM_SendRabmRrcConRelInd();*/

        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcRrcRelInd: SUSPENDING or SUSPENDED ");
        return NAS_LMM_MSG_DISCARD;
    }

    /* ����ͷ�ԭ��ֵΪ HO_FAIL������Ҫ����״̬������*/
    if(LRRC_LNAS_REL_CAUSE_GU2L_HO_ERR == pLrrcRelInd->enRelCause)
    {
        ulRet = NAS_LMM_MSG_DISCARD;
    }
    else
    {
        /*��IDLE̬���յ�RRC_REL_IND,ֱ�Ӷ���
          ��LMM�ǻָ������У����ʱ״̬ΪCONNECT������״̬������ */
        if(NAS_EMM_CONN_IDLE == NAS_EMM_GetConnState())
        {
            ulRet = NAS_LMM_MSG_HANDLED;
        }
    }


    NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_PreProcRrcRelInd: ulRet = ", ulRet);
    return(ulRet);
}
/*****************************************************************************
 Function Name   : NAS_LMM_PreProcRrcRelInd
 Description     : RRC_REL_CNF��Ϣ��Ԥ��������
 Input           : MsgBlock
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.leixiantiao      2015-5-5  Draft Enact

*****************************************************************************/
static VOS_UINT32  NAS_LMM_PreProcRrcRelCnf(    MsgBlock *          pMsg )
{
    /* ��LRRC�ͷ�ʱ�ر�CIPHERʱ��ת�Ƶ�LMM�ͷ�ʱ�ر�:��������������ȼ��ͣ���LMM��ʹ��CIPHER��ʱ��
        ������ռ��LRRC��CIPHERʱ�ӹرգ�����LMM DATA ABORT���� */
    #if (VOS_OS_VER != VOS_WIN32)
    /*�͹��ķ���Ϊ��ʡ���ģ���LTEģLNAS�յ�LRRC�ͷŵ�ʱ��������ӿڹص� cipher ʱ���Լ�ͶƱ�Ƿ��pll��
     Э��ջֻ������һ���ӿڣ��汾����֧�ɵ����ڴ˽ӿ��ڲ����з�װ*/
    (VOS_VOID)BSP_CIPHER_Disable();
    #endif

    return NAS_LMM_MSG_DISCARD;
}

VOS_UINT32  NAS_LMM_PreProcMmcRelReq( MsgBlock *          pMsg )
{
    VOS_UINT32                          ulCurEmmStat;
    VOS_UINT32                          ulMainStat;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMmcRelReq Enter.");

    (VOS_VOID)pMsg;
    if (NAS_EMM_NULL_PTR == pMsg)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMmcRelReq: Input para is null");
        return NAS_LMM_MSG_HANDLED;
    }

    ulMainStat = NAS_EMM_CUR_MAIN_STAT;

    if (NAS_EMM_CONN_IDLE == NAS_EMM_GetConnState())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: Idle");
        NAS_EMM_SendMmcStatusInd(       MMC_LMM_STATUS_TYPE_CONN_STATE,
                                        MMC_LMM_CONN_IDLE);

        return NAS_LMM_MSG_HANDLED;
    }

    if (EMM_MS_RRC_CONN_REL_INIT == ulMainStat)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq:current state is EMM_MS_RRC_CONN_REL_INIT!");

        if(NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState())
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: conn state is not releasing");
            NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);

            if(NAS_EMM_TAU_COMPLETE_VALID == NAS_EMM_TAU_GetEmmTauCompleteFlag())
            {
               NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq,TAU COMPLETE NEEDED DELAY");

               /* �ӳ�500ms����Ϊ��ʹ��TAU COMPLETE��Ϣ�ܹ��������ͳ�ȥ */
               (VOS_VOID)TLPS_TaskDelay(500);
            }

            /*������ʱ��TI_NAS_EMM_MRRC_WAIT_RRC_CNF*/
            NAS_LMM_StartStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);

            /*����RRC_MM_REL_REQ*/
            NAS_EMM_SndRrcRelReq(NAS_LMM_NOT_BARRED);

            /* ��RRC���ͷź�����״̬��Ϊ�ͷŹ����� */
            NAS_EMM_SetConnState(NAS_EMM_CONN_RELEASING);

        }
        return NAS_LMM_MSG_HANDLED;
    }

    NAS_LMM_IfEmmHasBeenPushedThenPop();

    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT,
                                                NAS_EMM_CUR_SUB_STAT);

    switch(ulCurEmmStat)
    {
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT, EMM_SS_ATTACH_WAIT_ESM_PDN_RSP):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT, EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT, EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT, EMM_SS_ATTACH_WAIT_RRC_DATA_CNF):
                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: REG_INIT");
                NAS_EMM_ProcDeregMmcRelReq();
                break;

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_DEREG_INIT,EMM_SS_DETACH_WAIT_CN_DETACH_CNF):
                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: DEREG_INIT is discard");
                NAS_EMM_ProcDetachMmcRelReq();
                break;
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE):
                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: REG+NORMAL_SERVICE");
                NAS_EMM_DisableLteCommonProc();
                break;

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_TAU_INIT, EMM_SS_TAU_WAIT_CN_TAU_CNF):
                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: TauInit+WaitCnTauCnf");
                NAS_EMM_ProcRegMmcRelReqWhileTauInit();
                break;

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF):
                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: SerInit+WaitCnSerCnf");
                NAS_EMM_ProcRegMmcRelReqWhileSerInit();
                break;

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF):
                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF");
                NAS_EMM_ProcRegMmcRelReqWhileImsiDetach();
                break;
                /*�Ķ�����: �µ��ⳡ���ԣ���绰CSFB HO��Wʧ�ܣ����л���Gģ����ҵ�񣬴�ʱ��Ҫ������
                ����: ��W���˵�L, rrc��nas����resume���̣���resume�Ĺ����У� MMC�·���·�ͷ�����
                ��·�ͷųɹ��� Ȼ���߹������̣� ���Gģ����ҵ�� ������NAS����
                EMM_MS_RESUME + EMM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND״̬��ʱ���յ�MMC��rel req��
                ����·�ͷ����󷢸�RRC����*/
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RESUME, EMM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND):
                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMmcRelReq: EMM_MS_RESUME");
                NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
                break;
        default:
                /*
                EMM_MS_DEREG+EMM_SS_DEREG_WAIT_MRRC_REL_CNF
                EMM_MS_REG+EMM_SS_REG_WAIT_MRRC_REL_CNF
                EMM_MS_TAU_INIT+EMM_SS_TAU_WAIT_RRC_REL_IND
                EMM_MS_SER_INIT+EMM_SS_SER_WAIT_RRC_REL_IND
                EMM_MS_REG_INIT+EMM_SS_ATTACH_WAIT_RRC_REL_IND
                EMM_MS_DEREG_INIT+EMM_SS_DETACH_WAIT_RRC_REL_IND
                */
                /*����*/
                break;

    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_EMM_IsSuspended( VOS_VOID )
{
    if( ((EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())&&(EMM_SS_REG_NO_CELL_AVAILABLE ==NAS_LMM_GetEmmCurFsmSS()))
       ||((EMM_MS_DEREG == NAS_LMM_GetEmmCurFsmMS())&&(EMM_SS_DEREG_NO_CELL_AVAILABLE ==NAS_LMM_GetEmmCurFsmSS()))
       ||((EMM_MS_DEREG == NAS_LMM_GetEmmCurFsmMS())&&(EMM_SS_DEREG_NO_IMSI ==NAS_LMM_GetEmmCurFsmSS())))
    {
        if(NAS_LMM_CUR_LTE_SUSPEND == NAS_EMM_GetCurLteState())
        {
            return NAS_EMM_IS_SUSPEND;
        }
    }

    return NAS_EMM_NOT_SUSPEND;
}


MMC_LMM_ATTACH_RSLT_ENUM_UINT32  NAS_LMM_SupendResumeAttachRslt( MsgBlock  *pMsg )
{
    MMC_LMM_ATTACH_RSLT_ENUM_UINT32     ulAttachRslt = MMC_LMM_ATT_RSLT_BUTT;
    MMC_LMM_ATTACH_REQ_STRU            *pstMmcAttReq = VOS_NULL_PTR;

    pstMmcAttReq            = (MMC_LMM_ATTACH_REQ_STRU*)pMsg;
    /* ATTACH����ΪIMSI */
    if(MMC_LMM_ATT_REQ_TYPE_CS_ONLY == pstMmcAttReq->ulAttachType)
    {
        if(NAS_EMM_NO == NAS_EMM_IsCsPsUeMode())
        {
            return MMC_LMM_ATT_RSLT_FAILURE;
        }

        if((NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
            || (NAS_LMM_REG_DOMAIN_CS == NAS_LMM_GetEmmInfoRegDomain()))
        {
             return MMC_LMM_ATT_RSLT_SUCCESS;
        }

        return MMC_LMM_ATT_RSLT_FAILURE;
    }

    /* ATTACH����ΪEPS */
    else if(MMC_LMM_ATT_REQ_TYPE_PS_ONLY == pstMmcAttReq->ulAttachType)
    {
        if((NAS_LMM_REG_DOMAIN_NULL == NAS_LMM_GetEmmInfoRegDomain())
            || (NAS_LMM_REG_DOMAIN_CS == NAS_LMM_GetEmmInfoRegDomain()))
        {
            return  MMC_LMM_ATT_RSLT_FAILURE;
        }

        return MMC_LMM_ATT_RSLT_SUCCESS;

    }

    /* ATTACH����ΪCOMBINED */
    else if(MMC_LMM_ATT_REQ_TYPE_CS_PS== pstMmcAttReq->ulAttachType)
    {
        if(NAS_EMM_NO == NAS_EMM_IsCsPsUeMode())
        {
            return MMC_LMM_ATT_RSLT_FAILURE;
        }

        if(NAS_LMM_REG_DOMAIN_CS_PS == NAS_LMM_GetEmmInfoRegDomain())
        {
            return MMC_LMM_ATT_RSLT_SUCCESS;
        }

        return  MMC_LMM_ATT_RSLT_FAILURE;

    }
    else
    {
        /*dayin*/
    }
    return ulAttachRslt;
}


VOS_UINT32  NAS_LMM_PreProcMsgAppMmAttachReq( MsgBlock *          pMsg )
{

    VOS_UINT32                          ulRet = NAS_LMM_MSG_DISCARD;
    VOS_UINT32                          ulCurEmmStat;
    MMC_LMM_ATTACH_REQ_STRU             *pstMmcAttachReq = VOS_NULL_PTR;
    VOS_UINT32                          ulAttachRst = MMC_LMM_ATT_RSLT_SUCCESS;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMsgAppMmAttachReq: enter.");

    pstMmcAttachReq                        = (VOS_VOID*)pMsg;
    /* ����ǰLTE������������Ϣ */

    pstMmcAttachReq = (MMC_LMM_ATTACH_REQ_STRU *)pMsg;

    /*NULL״̬��ֱ�Ӷ���*/
    if  (EMM_MS_NULL == NAS_LMM_GetEmmCurFsmMS())
    {

        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcMsgAppMmAttachReq, LTE CUR NULL,discard it.");

        return NAS_LMM_MSG_HANDLED;
    }
    if(NAS_EMM_IS_SUSPEND == NAS_EMM_IsSuspended())
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_LMM_PreProcMsgAppMmAttachReq: LTE CUR SUSPEND");
        return NAS_LMM_MSG_HANDLED;

    }


    /*�����û�����ATTACH����������*/
    NAS_EMM_GLO_SetAttReqType(pstMmcAttachReq->ulAttachType);
    /*�����û�����ATTACH��ԭ��ֵ*/
    NAS_LMM_SetEmmInfoMmcAttachReason(pstMmcAttachReq->ulAttachReason);

    if (MMC_LMM_ATT_REQ_TYPE_PS_ONLY == pstMmcAttachReq->ulAttachType)
    {
        /* ��������PS�Զ�ATTACH*/
        NAS_MML_SetPsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);
    }
    else if (MMC_LMM_ATT_REQ_TYPE_CS_ONLY == pstMmcAttachReq->ulAttachType)
    {
        /* ��������CS�Զ�ATTACH*/
         NAS_MML_SetCsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);
    }
    else if(MMC_LMM_ATT_REQ_TYPE_CS_PS == pstMmcAttachReq->ulAttachType)
    {
        /* ��������PS�Զ�ATTACH*/
        NAS_MML_SetPsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);

        /* ��������CS�Զ�ATTACH*/
        NAS_MML_SetCsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);
    }
    else
    {

    }



    if( (EMM_MS_SUSPEND == NAS_LMM_GetEmmCurFsmMS())
        ||(EMM_MS_RESUME == NAS_LMM_GetEmmCurFsmMS()))
    {
        NAS_EMM_ATTACH_LOG_NORM("NAS_LMM_PreProcMsgAppMmAttachReq: LTE CUR SUSPEND ING OR RESUME");
        /*����APP����*/
        NAS_EMM_SaveAppMsgPara(pstMmcAttachReq->ulMsgId,pstMmcAttachReq->ulOpId);
        ulAttachRst = NAS_LMM_SupendResumeAttachRslt(pMsg);
        NAS_EMM_MmcSendAttCnf(ulAttachRst);
        return NAS_LMM_MSG_HANDLED;

    }

    /*��ȡEMM��ǰ״̬*/
    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT,
                                            NAS_EMM_CUR_SUB_STAT);

    switch(ulCurEmmStat)
    {
        /*���������л�����·�ͷŹ����У������ȼ�����*/
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RRC_CONN_REL_INIT,EMM_SS_RRC_CONN_WAIT_REL_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RRC_CONN_EST_INIT,EMM_SS_RRC_CONN_WAIT_EST_CNF):
                ulRet = NAS_LMM_STORE_HIGH_PRIO_MSG;
                break;

        /*��AUTH_INIT+WAIT_CN_AUTH_CNF�����У������ȼ����� */
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_AUTH_INIT, EMM_SS_AUTH_WAIT_CN_AUTH):

                ulRet = NAS_LMM_STORE_LOW_PRIO_MSG;
                break;
        default:
                ulRet = NAS_LMM_MSG_DISCARD;
                break;
    }
    return(ulRet);
}


static VOS_UINT32  NAS_LMM_PreProcMsgEsmEstReq( MsgBlock *          pMsg )
{
    (VOS_VOID)                            pMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMsgEsmEstReq Enter.");

    /*��svlte��ĳЩ�����£���Ҫ����modem�е���modem����ʱ��modem detach ps������Ϊps not allow
      ��modem attach��ps�� ͬʱapp�·���ndis_req�ִ�������modem���¼���Ӷ�����˫ps attach��
      Ϊ�˱����������������modem��ps not allow��������յ�ndis_req�������¼��ֱ�ӻظ�����ʧ��*/
    if((NAS_EMM_NO == NAS_MML_GetCsOnlyDataServiceSupportFlg())
        && (NAS_EMM_AUTO_ATTACH_NOT_ALLOW == NAS_MML_GetPsAttachAllowFlg()))
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMsgEsmEstReq: have enabled ps_not_allow, don't est process");
        NAS_EMM_EsmSendEstCnf(EMM_ESM_ATTACH_RESULT_FAIL);
        return NAS_LMM_MSG_HANDLED;
    }
    NAS_MML_SetPsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);

    return NAS_LMM_MSG_DISCARD;
}

/*****************************************************************************
 Function Name   : NAS_LMM_PreProcMsgEsmClrEsmPrcResNotify
 Description     : ��EMM_ESM_CLR_ESM_PROC_RES_NOTIFY��Ϣ�Ĵ���
 Input           : pMsg
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2012-11-01  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_LMM_PreProcMsgEsmClrEsmPrcResNotify
(
    MsgBlock                           *pMsg
)
{
    EMM_ESM_CLR_ESM_PROC_RES_NOTIFY_STRU   *pstEsmClrEsmProcResNotify   = NAS_EMM_NULL_PTR;

    pstEsmClrEsmProcResNotify = (EMM_ESM_CLR_ESM_PROC_RES_NOTIFY_STRU*)pMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMsgEsmClrEsmPrcNotify Enter.");
	/* lihong00150010 emergency tau&service begin */
    /* ����OPIDɾ�������ESM��Ϣ */
    NAS_EMM_SER_DeleteEsmMsg(pstEsmClrEsmProcResNotify->ulOpId);
	/* lihong00150010 emergency tau&service end */
    /* ����ǽ������ͣ������״̬Ϊû�н���PDN�����������ATTACH,TAU,SERVICE
       ����ԭ��ֵ���������Ľ���PDN����������Ϣ */
    if (VOS_TRUE == pstEsmClrEsmProcResNotify->ulIsEmcType)
    {
        NAS_LMM_SetEmmInfoIsEmerPndEsting(VOS_FALSE);

        if ((EMM_ATTACH_CAUSE_ESM_ATTACH_FOR_INIT_EMC_BERER == NAS_EMM_GLO_AD_GetAttCau())
            || (EMM_ATTACH_CAUSE_ESM_EMC_ATTACH == NAS_EMM_GLO_AD_GetAttCau()))
        {
            NAS_EMM_GLO_AD_GetAttCau() = EMM_ATTACH_CAUSE_OTHER;
        }
		/* lihong00150010 emergency tau&service begin */
        if (NAS_EMM_TAU_START_CAUSE_ESM_EMC_PDN_REQ == NAS_EMM_TAU_GetEmmTAUStartCause())
        {
            NAS_EMM_TAU_SaveEmmTAUStartCause(NAS_EMM_TAU_START_CAUSE_OTHERS);
        }

        if ((NAS_EMM_SER_START_CAUSE_ESM_DATA_REQ_EMC == NAS_EMM_SER_GetEmmSERStartCause())
            || (NAS_EMM_SER_START_CAUSE_RABM_REEST_EMC == NAS_EMM_SER_GetEmmSERStartCause()))
        {
            NAS_EMM_SER_SaveEmmSERStartCause(NAS_EMM_SER_START_CAUSE_BUTT);
        }
		/* lihong00150010 emergency tau&service end */
        NAS_EMM_ClearEmcEsmMsg();
    }

    return NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_EMM_PreProcMsgSwithOffTimerExp
 Description     : �ػ���ʱ����ʱԤ����
 Input           : pMsg
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.sunbing49683      2010-1-29  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_PreProcMsgSwithOffTimerExp( MsgBlock *        pMsg )/*lint -e818*/
{
    VOS_UINT32                          ulRet;

    (VOS_VOID)pMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_PreProcMsgSwithOffTimerExp enter!");

    NAS_LMM_IfEmmHasBeenPushedThenPop();

    ulRet = NAS_EMM_MsNullSsWaitSwitchOffProcMsgRrcRelInd();

    return ulRet;
}/*lint  +e818*/


VOS_UINT32 NAS_EMM_PreProcMsgDelForbTaTimerExpire( MsgBlock *pMsg )
{
    NAS_LMM_NETWORK_INFO_STRU           *pstNetInfo;
    VOS_UINT32                          ulNum;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_PreProcMsgDelForbTaTimerExpire is entered.");

    (void)pMsg;

    /* ��μ�� */
    if ( NAS_EMM_NULL_PTR == pMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_PreProcMsgDelForbTaTimerExpire:input ptr null!");
        return  NAS_LMM_ERR_CODE_PTR_NULL;
    }


    /* ɾ����ֹTA�б� */
    pstNetInfo = NAS_LMM_GetEmmInfoNetInfoAddr();

    ulNum                               = pstNetInfo->stForbTaForRoam.ulTaNum;

    /*delete list of "forbidden tracking areas for roaming"*/
    pstNetInfo->bitOpFoibTaForRoam = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_ClearTaList(&pstNetInfo->stForbTaForRoam);

    /*delete list of "forbidden tracking areas for regional provision of service"*/
    pstNetInfo->bitOpFoibTaForRpos = NAS_EMM_BIT_NO_SLCT;
    NAS_LMM_ClearTaList(&pstNetInfo->stForbTaForRpos);

    if (ulNum != pstNetInfo->stForbTaForRoam.ulTaNum)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_PreProcMsgDelForbTaTimerExpire:Forb TA FOR Roaming is change");
        NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);
    }

    return NAS_LMM_MSG_HANDLED;
}

VOS_UINT32  NAS_EMM_PreProcMsgWaitSyscfgCnfTimerExpire( MsgBlock *pMsg )
{
    (void)pMsg;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_PreProcMsgWaitSyscfgCnfTimerExpire is entered.");

    NAS_LMM_SetEmmInfoSyscfgCtrl(NAS_LMM_SYSCFG_FLAG_INVALID);

    return NAS_LMM_MSG_HANDLED;
}


VOS_UINT32  NAS_LMM_PreProcrRrcDataInd( MsgBlock *          pMsg )
{
    LRRC_LMM_DATA_IND_STRU             *pRrcMmDataIndMsg    = VOS_NULL_PTR;
    VOS_UINT8                           ucMsgType           = NAS_EMM_NULL;
    VOS_UINT8                           ucPD                = NAS_EMM_NULL;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcrRrcDataInd Enter.");

    pRrcMmDataIndMsg = (LRRC_LMM_DATA_IND_STRU *)pMsg;
    ucMsgType = pRrcMmDataIndMsg->stNasMsg.aucNasMsg[NAS_LMM_CN_MSG_MT_POS];

    /*ȡ��PD*/
    ucPD = pRrcMmDataIndMsg->stNasMsg.aucNasMsg[NAS_EMM_NULL] & EMM_CN_MSG_PDMASK;

    /* �����AuthInit״̬���յ�ESM��Ϣ�� ��ֹAUTH���̣���ջ */
    if  ((EMM_CN_MSG_PD_ESM == ucPD)
        && (EMM_MS_AUTH_INIT == NAS_LMM_GetEmmCurFsmMS()))
    {
        NAS_EMM_AbortAuthProcedure();
    }

    /*�����AuthInit״̬���յ�CN_DETACH_REQ��Ϣ����ֹAUTH���̣���ջ*/
    if((NAS_EMM_CN_MT_DETACH_REQ_MT == ucMsgType)
        && (EMM_MS_AUTH_INIT == NAS_LMM_GetEmmCurFsmMS()))
    {
        NAS_EMM_AbortAuthProcedure();
    }

    return NAS_LMM_MSG_DISCARD;

}
static VOS_UINT32  NAS_EMM_PreProcMsgEsmBearStatusReq( MsgBlock *          pMsg )
{
    VOS_UINT32              ulRet       = NAS_LMM_MSG_DISCARD;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_PreProcMsgEsmBearStatusReq Enter");

    ulRet                               = NAS_EMM_MsAnyStateSsAnySateEsmBearStatusReq((VOS_VOID*)pMsg);

    return(ulRet);

}


static VOS_UINT32  NAS_EMM_PreProcMsgRrcErrInd( MsgBlock *          pMsg  )
{
    VOS_UINT32              ulRet       = NAS_LMM_MSG_DISCARD;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_PreProcMsgRrcErrInd Enter");


    vos_printf("\r\n NAS_EMM_PreProcMsgRrcErrInd: receive LRRC_LMM_ERR_IND! \r\n");

    /* ��ջ,Ŀ����:ά��ջ�Ĺ�������
    */
    NAS_LMM_IfEmmHasBeenPushedThenPop();

    /* �����ǰ��״̬��NULL,��Ҫ���ݵ�ǰ����״̬����صĴ���
    */
    if(EMM_MS_NULL                      ==  NAS_EMM_CUR_MAIN_STAT)
    {


        ulRet                           = NAS_EMM_MsNullSsAnyStateMsgRrcErrInd(ID_LRRC_LMM_ERR_IND,
                                                                              (VOS_VOID*)pMsg);
    }
    else
    {
        ulRet                           = NAS_EMM_MsNotNullSsAnyStateMsgRrcErrInd(ID_LRRC_LMM_ERR_IND,
                                                                                   (VOS_VOID*)pMsg);
    }
    return(                              ulRet);

}


VOS_UINT32  NAS_EMM_PreProcMsgT3416Exp( MsgBlock *          pMsg )/*lint  -e818*/
{
    (VOS_VOID)pMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_PreProcMsgT3416Exp enter!");

    NAS_EMM_SecuClearRandRes();

    return NAS_LMM_MSG_HANDLED;
}/*lint +e818*/


VOS_UINT32  NAS_EMM_PreProcMsgT3402Exp( MsgBlock *          pMsg )
{
    (VOS_VOID)pMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_PreProcMsgT3402Exp enter!");

    NAS_EMM_TAU_SaveEmmTAUAttemptCnt(NAS_EMM_TAU_ATTEMPT_CNT_ZERO);
    NAS_EMM_AttResetAttAttempCounter();

    return NAS_LMM_MSG_DISCARD;
}


VOS_UINT32  NAS_EMM_PreProcMsgCsfbDelayTimerExp( MsgBlock *          pMsg )
{
    (VOS_VOID)pMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_PreProcMsgCsfbDelayTimerExp enter!");

    /* ��MMC�ϱ�SERVICEʧ�ܴ�������ȥGU */
    NAS_EMM_MmcSendSerResultIndOtherType(MMC_LMM_SERVICE_RSLT_FAILURE);

    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    #if (FEATURE_PTM == FEATURE_ON)
    if (VOS_TRUE == NAS_EMM_SER_IsCsfbProcedure())
    {
        NAS_EMM_ExtServiceErrRecord(EMM_OM_ERRLOG_CN_CAUSE_NULL, EMM_OM_LMM_CSFB_FAIL_CAUSE_DELAY_TIMER_EXP);
    }
    #endif

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_LMM_MsgPreProcess(       MsgBlock *          pMsg)
{
    VOS_UINT32                          ulRet               = NAS_LMM_MSG_DISCARD;
    NAS_LMM_COMM_ACTION_FUN              pActFun             = NAS_LMM_NULL_PTR;
    VOS_UINT32                          ulMsgId;
    PS_MSG_HEADER_STRU                 *pHeader     = VOS_NULL_PTR;


    /* ��ȡ��Ϣͷָ��*/
    pHeader = (PS_MSG_HEADER_STRU *) pMsg;

    /*OMԤ������Ϣ*/
    ulRet = NAS_LMM_OmMaintain(pMsg);
    if(NAS_LMM_MSG_HANDLED == ulRet)
    {
        return  ulRet;
    }

    /*����Ԥ������Ϣ*/
    NAS_LMM_GetNameFromMsg(ulMsgId, pMsg);   /* ��ȡ��ϢID */
    pActFun = NAS_LMM_CommFindFun(       (NAS_LMM_COMM_ACT_STRU *)g_astMmPreProcMap,
                                        g_astMmPreProcMapNum,
                                        ulMsgId,
                                        pHeader->ulSenderPid);
    if (NAS_LMM_NULL_PTR != pActFun)
    {   /* �������ҵ��ˣ���������ִ�� */
        ulRet = (*pActFun)(pMsg);
    }

    return  ulRet;
}
VOS_UINT32 NAS_LMM_PreProcAppStartReq(   MsgBlock  * pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulCurEmmStat;
    VOS_UINT32                          ulMsgId;

    /* ��ȡ��ϢID */
    NAS_LMM_GetNameFromMsg(ulMsgId, pMsg);

    ulRet                               = NAS_LMM_MSG_DISCARD;
    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT,
                                            NAS_EMM_CUR_SUB_STAT);

    switch(ulCurEmmStat)
    {   /* EMM ״̬ */

                /* �ȴ����� */
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_APP_START_REQ):
                ulRet = NAS_EMM_MsNullSsWaitAppStartReqMsgAppStartReq(ulMsgId, (VOS_VOID *)pMsg);
                break;
/* V7R1
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAITING_USIM_READY):
                ulRet = NAS_EMM_MsNullSsWaitingUsimReadyMsgAppStartReq(ulMsgId, (VOS_VOID *)pMsg);
                break;
*/
                /* ���������� */

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL,EMM_SS_NULL_WAIT_READING_USIM):

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_MMC_START_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_RRC_START_CNF):
                /* ���������ж���������Ϣ */
                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcAppStartReq: Power oning..., discard it.");
                break;

                /* �ػ������� */
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_SWITCH_OFF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_MMC_STOP_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_RRC_STOP_CNF):
                /* �ػ������У���APP��Ӧ�������� */
                ulRet = NAS_EMM_WhenStopingThenMsgAppStartReq(ulMsgId, (VOS_VOID *)pMsg);
                break;

                /* ����״̬: ������ɺ�δ�յ��ػ�������Ϣǰ */
        default :
                ulRet = NAS_EMM_MsNotNullSsAnyStateMsgAppStartReq(ulMsgId, (VOS_VOID *)pMsg);
                break;
    }

    return  ulRet;
}
VOS_VOID NAS_LMM_IfEmmHasBeenPushedThenPop( VOS_VOID )
{
    NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_IfEmmHasBeenPushedThenPop: NAS_EMM_CUR_MAIN_STAT = .",
                            NAS_EMM_CUR_MAIN_STAT);

    switch(NAS_EMM_CUR_MAIN_STAT)
    {

        case    EMM_MS_RRC_CONN_EST_INIT:

                /* ֹͣ ��ʱ��*/
                NAS_LMM_StopStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_CONN_CNF);

                /* ��ջ */
                NAS_EMM_FSM_PopState();
                break;

        case    EMM_MS_RRC_CONN_REL_INIT:
                if(NAS_EMM_CONN_RELEASING != NAS_EMM_GetConnState())
                {
                    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_T3440);
                    NAS_LMM_StopPtlTimer(TI_NAS_EMM_PTL_REATTACH_DELAY);

                    NAS_EMM_FSM_PopAllState();
                }
                else
                {
                    /* ֹͣ ��ʱ��*/
                    NAS_LMM_StopStateTimer(TI_NAS_EMM_MRRC_WAIT_RRC_REL_CNF);
                    NAS_EMM_FSM_PopAllState();
                    NAS_EMM_CommProcConn2Ilde();
                }
                break;

        case    EMM_MS_AUTH_INIT:
                /*��ֹAUTH����*/
                NAS_EMM_AbortAuthProcedure();
                break;

        default:
                NAS_LMM_PUBM_LOG_INFO("NAS_LMM_IfEmmHasBeenPushedThenPop: No Push.");
                break;
    }

    return;
}


VOS_VOID NAS_LMM_FreeDynMem( VOS_VOID )
{

    /* �ͷ�MM���״̬���Ļ�����Ϣ���� */
    NAS_LMM_BufMsgQueFree();

    /* �ͷ���ģ�黺�� */
    NAS_LMM_SubModFreeDyn();
}


VOS_VOID  NAS_LMM_SubModFreeDyn( VOS_VOID )
{
    /* �ͷ�Attachģ�黺�� */
    NAS_EMM_FreeAttDyn();

    /* �ͷ�Detachģ�黺�� */
    NAS_EMM_FreeDetDyn();

    /* �ͷ�TAUģ�黺�� */
    NAS_EMM_FreeTauDyn();

    /* �ͷ�Gutiģ�黺�� */
    NAS_EMM_FreeGutiDyn();

    /* �ͷ�SERVICEģ�黺�� */
    NAS_EMM_FreeSerDyn();

    /* �ͷ�IDENģ�黺�� */
    NAS_EMM_FreeIdenDyn();

    /* �ͷ�SECUģ�黺�� */
    NAS_EMM_FreeSecuDyn();

    /* �ͷ�Mrrcģ�黺�� */
    NAS_EMM_FreeMrrcDyn();

    return;
}



VOS_UINT32  NAS_LMM_PreProcAppStopReq(   MsgBlock  * pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulCurEmmStat;
    VOS_UINT32                          ulMsgId;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcAppStopReq: enter.");

    /* ��ȡ��ϢID */
    NAS_LMM_GetNameFromMsg(ulMsgId, pMsg);

    /* ���������״̬: ������ʼ��״̬��������ʼ��״̬����Ȩ������״̬��
       ��ֹͣ��ǰ�Ķ�ʱ����״̬��ջ��*/
    NAS_LMM_IfEmmHasBeenPushedThenPop();

    /* �ͷſ��ܴ��ڵĶ�̬�ڴ� */
    NAS_LMM_FreeDynMem();

    NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_NO);

    NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_VALID);

    /* lihong00150010 csfb begin */

    NAS_EMM_ClearEmergencyList();

    /* lihong00150010 csfb end */

    ulRet                                 = NAS_LMM_MSG_DISCARD;
    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT,
                                            NAS_EMM_CUR_SUB_STAT);

    switch(ulCurEmmStat)
    {   /* EMM ״̬ */

                /* δ������ֱ�Ӹ�APP�ظ��ػ��ɹ� */
        /*case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAITING_USIM_READY):*/
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_READING_USIM):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_APP_START_REQ):
                ulRet = NAS_EMM_MsNullSsAnyReadUsimStateMsgAppStopReq(ulMsgId, (VOS_VOID *)pMsg);
                break;

                /* �ػ�������, ���������� */
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_MMC_STOP_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_RRC_STOP_CNF):

        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_SWITCH_OFF):

                break;

                /* ��ע��ɹ�, ���CN����ע����Ϣ, ���ȸ�CN���͹ػ�DETACH��
                   �ٸ����ģ��SM/MMC/RRC���ػ���Ϣ���ٸ�APP�ظ��ػ��ɹ�  */
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT, EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT, EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT, EMM_SS_ATTACH_WAIT_RRC_DATA_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_NORMAL_SERVICE):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_WAIT_ACCESS_GRANT_IND):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_TAU_INIT, EMM_SS_TAU_WAIT_CN_TAU_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_SER_INIT, EMM_SS_SER_WAIT_CN_SER_CNF):
                ulRet = NAS_EMM_MsRegSsAnyMsgAppStopReq(ulMsgId, (VOS_VOID *)pMsg);
                break;

        default:

                /* ����DETACH: �ȸ����ģ��SM/MMC/RRC���ػ���Ϣ���ٸ�APP�ظ��ػ��ɹ� */
                ulRet = NAS_EMM_MsNotNullNotRegMsgAppStopReq(ulMsgId, (VOS_VOID *)pMsg);
                break;

    }

    return  ulRet;
}
VOS_VOID NAS_LMM_SetAttachAllowFlgByDetachType(
                               MMC_LMM_MO_DETACH_TYPE_ENUM_UINT32 ulDetachType)
{
    if (MMC_LMM_MO_DET_PS_ONLY == ulDetachType)
    {
        /* ���ò�����PS�Զ�ATTACH*/
        NAS_MML_SetPsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_NOT_ALLOW);
    }
    else if (MMC_LMM_MO_DET_CS_ONLY == ulDetachType)
    {
        /* ���ò�����CS�Զ�ATTACH*/
        NAS_MML_SetCsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_NOT_ALLOW);
    }
    else
    {
        /* ���ò�����PS�Զ�ATTACH*/
        NAS_MML_SetPsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_NOT_ALLOW);

        /* ���ò�����CS�Զ�ATTACH*/
        NAS_MML_SetCsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_NOT_ALLOW);
    }
}

/*lint -e818*/
/*lint -e830*/

VOS_VOID NAS_LMM_LteCurrentSuspendProcAppDetReq
(
    const MMC_LMM_DETACH_REQ_STRU            *pstAppDetReq
)
{
    if(EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
    {
        /* ���ֻ��DETACH CS����ôֻ�޸�ע����سɹ����� */
        if(MMC_LMM_MO_DET_CS_ONLY == pstAppDetReq->ulDetachType)
        {
            NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
            return;
        }

        /* DETACH PS��ת��DEREG̬�����ע����(ת״̬��Ȼ����)��Ҳ�����Ǵ��ڽ���ע����޿�״̬ */
        if (NAS_LMM_SIM_STATUS_UNAVAILABLE == NAS_LMM_GetSimState())
        {
            NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                   EMM_SS_DEREG_NO_IMSI,
                                   TI_NAS_EMM_STATE_NO_TIMER);
        }
        else
        {
            /* �п� */
            NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                   EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                   TI_NAS_EMM_STATE_NO_TIMER);
        }

    }

   return;

}
VOS_UINT32  NAS_LMM_MsSuspendOrResumePreProcAppDetReq
(
    const MMC_LMM_DETACH_REQ_STRU            *pstAppDetReq
)
{
    VOS_UINT32                          ulRet = NAS_LMM_MSG_DISCARD;

    /* �ȹ���END�ͽ�ҵ�ESM��ERABM�ظ���������״̬�������յ���Ϊ�������յ�ֱ�ӻسɹ� */
    if((EMM_SS_SUSPEND_WAIT_END == NAS_LMM_GetEmmCurFsmSS())
        ||(EMM_SS_RESUME_RRCRSM_WAIT_OTHER_RESUME == NAS_LMM_GetEmmCurFsmSS()))
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_MsSuspendOrResumePreProcAppDetReq: SUSPEND_WAIT_END or RRCRSM_WAIT_OTHER_RESUME.");
        NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
        return NAS_LMM_MSG_HANDLED;
    }

    /* ���ǡ���ǵ�MMC����ظ�״̬�������ȼ����浽��һ״̬���� */
    if(EMM_SS_SUSPEND_RRCORI_WAIT_MMC_SUSPEND == NAS_LMM_GetEmmCurFsmSS())
    {
        return NAS_LMM_STORE_HIGH_PRIO_MSG;
    }

    /* ����״̬���ݵ�ǰUE��ע�����DETACH���ͣ��ж��Ƿ����ֱ�ӻسɹ� */
    switch(NAS_LMM_GetEmmInfoRegDomain())
    {
                /* ע����NULLֱ�ӻسɹ� */
        case    NAS_LMM_REG_DOMAIN_NULL:

                NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
                ulRet = NAS_LMM_MSG_HANDLED;

                break;

                /* ע����ΪCSֱ�ӻسɹ��� ��PS DETACH֮�������ע���� */
        case    NAS_LMM_REG_DOMAIN_CS:

                if(MMC_LMM_MO_DET_PS_ONLY != pstAppDetReq->ulDetachType)
                {
                    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_NULL);
                }

                NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
                ulRet = NAS_LMM_MSG_HANDLED;

                break;

                /* ע����ΪPS��CS DETACHֱ�ӻسɹ��������״̬������ */
        case    NAS_LMM_REG_DOMAIN_PS:

                if(MMC_LMM_MO_DET_CS_ONLY == pstAppDetReq->ulDetachType)
                {
                    NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
                    ulRet = NAS_LMM_MSG_HANDLED;
                }

                break;

                /* ע����ΪCS_PS���յ�CS DETACH����CSFB�����⣬ȫ��ֱ���ڴ˻ظ�MMC*/
        case    NAS_LMM_REG_DOMAIN_CS_PS:

                /* ���۹����ǽ�ҹ����У�ֻҪ����CSFB���̾��ڴ�ֱ�ӻظ�CS DETACH */
                if ((LRRC_LMM_SUS_CAUSE_CSFB != NAS_EMM_GetSuspendCause())
                    &&(MMC_LMM_MO_DET_CS_ONLY == pstAppDetReq->ulDetachType))
                {
                    NAS_LMM_SetEmmInfoRegDomain(NAS_LMM_REG_DOMAIN_PS);
                    NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
                    ulRet = NAS_LMM_MSG_HANDLED;
                }

                break;

        default:
                break;
    }

    return ulRet;

}
/*lint +e830*/
/*lint +e818*/

VOS_UINT32  NAS_LMM_IsSvlteOrLcNeedLocalDetach(VOS_VOID)
{
    if (((NAS_EMM_YES == NAS_MML_GetSvlteSupportFlag())||(NAS_EMM_YES == NAS_MML_GetLcEnableFlg()))
        &&(NAS_LMM_LOCAL_DETACH_FLAG_VALID == NAS_MML_GetPsLocalDetachFlag()))
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_IsSvlteOrLcNeedLocalDetach: Need local detach.");
        return  NAS_EMM_YES;
    }

    return NAS_EMM_NO;
}


VOS_VOID  NAS_LMM_MsRegSsAnyLocalDetStateTrans(VOS_VOID)
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_MsRegSsAnyLocalDetStateTrans is entered.");

/* sunjitan delete protect for ESM������ĿΪ0�ı���DETACHʹ�� */

    switch(NAS_LMM_GetEmmCurFsmSS())
    {
        case    EMM_SS_REG_LIMITED_SERVICE:
                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_LIMITED_SERVICE,
                                       TI_NAS_EMM_STATE_NO_TIMER);
                break;

        case    EMM_SS_REG_PLMN_SEARCH:
                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_PLMN_SEARCH,
                                       TI_NAS_EMM_STATE_NO_TIMER);
                break;

        case    EMM_SS_REG_WAIT_ACCESS_GRANT_IND:
                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_ATTACH_NEEDED,
                                       TI_NAS_EMM_STATE_NO_TIMER);
                break;

        case    EMM_SS_REG_NO_CELL_AVAILABLE:
                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                       TI_NAS_EMM_STATE_NO_TIMER);
                break;

        default:
                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_NORMAL_SERVICE,
                                       TI_NAS_EMM_STATE_NO_TIMER);
                break;

    }

    return;
}
VOS_UINT32  NAS_LMM_SvlteOrLcPsLocalDetachProc
(
    const MMC_LMM_DETACH_REQ_STRU            *pstAppDetReq
)
{
    VOS_UINT32                          ulRet = NAS_LMM_MSG_DISCARD;
    MMC_LMM_TAU_RSLT_ENUM_UINT32        ulTauRslt;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_SvlteOrLcPsLocalDetachProc is entered.");

    /* NULL״̬�����ߵ����� */

    /* �������ѹջ״̬�ȳ�ջ��Ȼ���ճ�ջ���״̬������Ӧ���� */
    NAS_LMM_IfEmmHasBeenPushedThenPop();

    switch(NAS_LMM_GetEmmCurFsmMS())
    {
        /* ��ǰ�Ѿ�����δע��̬��ֱ�ӻسɹ� */
        case    EMM_MS_DEREG:
                NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
                NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);

                /* ����DETACH�ͷ���Դ:��̬�ڴ桢����ֵ */
                NAS_LMM_DeregReleaseResource();

                /* �����ʶ */
                NAS_MML_SetPsLocalDetachFlag(NAS_LMM_LOCAL_DETACH_FLAG_INVALID);
                ulRet = NAS_LMM_MSG_HANDLED;
                break;

        /* ��ǰ����ע������У�ֱ�ӱ���DETACH */
        case    EMM_MS_REG_INIT:

                /*��MMC����LMM_MMC_ATTACH_CNF��LMM_MMC_ATTACH_IND��Ϣ*/
                NAS_EMM_AppSendAttOtherType(MMC_LMM_ATT_RSLT_MO_DETACH_FAILURE);

                /* ֪ͨESMִ���˱���DETACH������MMC��DETACH�ɹ� */
                NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);
                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_NORMAL_SERVICE,
                                       TI_NAS_EMM_STATE_NO_TIMER);

                NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
                NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);

                /* ����DETACH�ͷ���Դ:��̬�ڴ桢����ֵ */
                NAS_LMM_DeregReleaseResource();
                NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

                /* �����ʶ */
                NAS_MML_SetPsLocalDetachFlag(NAS_LMM_LOCAL_DETACH_FLAG_INVALID);
                ulRet = NAS_LMM_MSG_HANDLED;
                break;

        /* ��ǰ����ע��̬��ֱ�ӱ���DETACH */
        case    EMM_MS_REG:

                /* ���3411�����У���Ҫ��MMC��TAU���������MMC�˳�����״̬�� */
                if (NAS_LMM_TIMER_RUNNING == NAS_LMM_IsPtlTimerRunning(TI_NAS_EMM_PTL_T3411))
                {
                    NAS_EMM_SetTauTypeNoProcedure();

                    ulTauRslt = MMC_LMM_TAU_RSLT_MO_DETACH_FAILURE;
                    NAS_EMM_MmcSendTauActionResultIndOthertype((VOS_VOID*)&ulTauRslt);
                }

                /* ֪ͨESMִ���˱���DETACH������MMC��DETACH�ɹ� */
                NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);
                NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
                NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);

                /* ���ݲ�ͬ��ע��״̬ת����Ӧ��δע��״̬ */
                NAS_LMM_MsRegSsAnyLocalDetStateTrans();

                /* ����DETACH�ͷ���Դ:��̬�ڴ桢����ֵ */
                NAS_LMM_DeregReleaseResource();
                NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

                /* �����ʶ */
                NAS_MML_SetPsLocalDetachFlag(NAS_LMM_LOCAL_DETACH_FLAG_INVALID);
                ulRet = NAS_LMM_MSG_HANDLED;
                break;

        /* ��ǰ����TAU��SER�����У�����״̬����������ǰ���ܴ��������ҵ������ */
        case    EMM_MS_TAU_INIT:
        case    EMM_MS_SER_INIT:
                ulRet = NAS_LMM_MSG_DISCARD;
                break;

        /* Svlte��CL��ģ�����У�������״̬һ�㲻�����յ�������յ��Ͱ���ͨ����µ�DETACH������
           ���������ʹ��״̬����Ҳֻ���Ǳ���DETACH����ʶ���Ծ��ڴ˴�������������ͷ���·��
           ����״̬ѹջ�����յ���ϵͳ�����е��ⲿ��Ϣ���󶪵� */
        case    EMM_MS_SUSPEND:
        case    EMM_MS_RESUME:
                ulRet = NAS_LMM_MsSuspendOrResumePreProcAppDetReq(pstAppDetReq);

                /* �����ʶ���ͷ���·(һ��Ҳ������) */
                NAS_MML_SetPsLocalDetachFlag(NAS_LMM_LOCAL_DETACH_FLAG_INVALID);
                break;

        default:
                /* ����״̬ͳһת��DEREG_NORMAL */
                NAS_EMM_EsmSendStatResult(EMM_ESM_ATTACH_STATUS_DETACHED);
                NAS_EMM_AdStateConvert(EMM_MS_DEREG,
                                       EMM_SS_DEREG_NORMAL_SERVICE,
                                       TI_NAS_EMM_STATE_NO_TIMER);
                NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
                NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);

                /* ����DETACH�ͷ���Դ:��̬�ڴ桢����ֵ */
                NAS_LMM_DeregReleaseResource();
                NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

                /* �����ʶ */
                NAS_MML_SetPsLocalDetachFlag(NAS_LMM_LOCAL_DETACH_FLAG_INVALID);
                ulRet = NAS_LMM_MSG_HANDLED;
                break;
    }

    return ulRet;
}



VOS_UINT32 NAS_LMM_ProcDetReqWithReseaonNon3gppAttach
(
    const MMC_LMM_DETACH_REQ_STRU            *pstAppDetReq
)
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ProcDetReqWithReseaonNon3gppAttach: enter.");

    /* CL��ģ��Ա���DETACH�Ĵ�����Svlte��CL����������ѡ����������ͬ */
    return NAS_LMM_SvlteOrLcPsLocalDetachProc(pstAppDetReq);

}

VOS_UINT32  NAS_LMM_ProcAppDetReq
(
    const MMC_LMM_DETACH_REQ_STRU            *pstAppDetReq
)
{
    VOS_UINT32                          ulRet = NAS_LMM_MSG_DISCARD;
    VOS_UINT32                          ulCurEmmStat;

    /*��ȡEMM��ǰ״̬*/
    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT, NAS_EMM_CUR_SUB_STAT);

    switch(ulCurEmmStat)
    {
        /*ֱ�ӻظ�APPȥע��ɹ�,״̬��ת��*/
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_DEREG, EMM_SS_DEREG_NORMAL_SERVICE):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_DEREG, EMM_SS_DEREG_NO_IMSI):

                NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
                NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
                ulRet = NAS_LMM_MSG_HANDLED;
                break;

        /*����APPȥע��������Ϣ*/
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_DEREG_INIT, EMM_SS_DETACH_WAIT_CN_DETACH_CNF):

                NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcAppDetReq: ESM or RABM ERROR, discard it.");
                NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
                ulRet = NAS_LMM_MSG_HANDLED;
                break;

        /*���������л�����·�ͷŹ����У������ȼ�����*/
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RRC_CONN_REL_INIT,EMM_SS_RRC_CONN_WAIT_REL_CNF):
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RRC_CONN_EST_INIT,EMM_SS_RRC_CONN_WAIT_EST_CNF):

                NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
                ulRet = NAS_LMM_STORE_HIGH_PRIO_MSG;
                break;

        /*��AUTH_INIT+WAIT_CN_AUTH_CNF�����У������ȼ����� */
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_AUTH_INIT, EMM_SS_AUTH_WAIT_CN_AUTH):

                NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
                ulRet = NAS_LMM_STORE_LOW_PRIO_MSG;
                break;

        default:
                ulRet = NAS_LMM_MSG_DISCARD;
                break;
    }
    return ulRet;
}
VOS_UINT32  NAS_LMM_PreProcAppDetReq(MsgBlock  * pMsg)
{
    VOS_UINT32                          ulRet = NAS_LMM_MSG_DISCARD;
    MMC_LMM_DETACH_REQ_STRU             *pstAppDetReq = VOS_NULL_PTR;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcAppDetReq: enter.");

    pstAppDetReq                        = (VOS_VOID*)pMsg;

    /* DETACH���ʹ��󣬶��� */
    if(MMC_LMM_MO_DETACH_TYPE_BUTT <= pstAppDetReq->ulDetachType)
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcAppDetReq: DetachType ERROR!");
        return NAS_LMM_MSG_HANDLED;
    }

    /* ����DETACH���Ͷ���Ӧ�������ò�����ע���ʶ */
    NAS_LMM_SetAttachAllowFlgByDetachType(pstAppDetReq->ulDetachType);

    /*����DETACH����������*/
    NAS_EMM_GLO_SetDetTypeMo(pstAppDetReq->ulDetachType);

    /* NULL״̬ʱֱ�ӻظ��ɹ� */
    if (EMM_MS_NULL == NAS_LMM_GetEmmCurFsmMS())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcAppDetReq: LTE CUR NULL.");

        /*����APP������ֻ���ڸ���֧�б��棬��ֹAPP DETACH���APP ATTACH����֮ǰ��¼�Ĳ�����ʧ */
        NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
        NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
        return NAS_LMM_MSG_HANDLED;
    }

    /* ��ģ״̬ʱֱ�ӻظ��ɹ� */
    if (NAS_EMM_IS_SUSPEND == NAS_EMM_IsSuspended())
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcAppDetReq: LTE CUR SUSPEND.");
        NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
        NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
        NAS_LMM_LteCurrentSuspendProcAppDetReq(pstAppDetReq);
        return NAS_LMM_MSG_HANDLED;
    }

    /* �û�����DETACH����ΪIMSI��UEģʽΪPS����ֱ�ӻظ��ɹ� */
    /* modify by jiqiang 2014.03.20 pclint 960 begin */
    if ((NAS_EMM_NO == NAS_EMM_IsCsPsUeMode()) && (MMC_LMM_MO_DET_CS_ONLY == pstAppDetReq->ulDetachType))
    /* modify by jiqiang 2014.03.20 pclint 960 end */
    {
        NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
        NAS_EMM_AppSendDetCnf(MMC_LMM_DETACH_RSLT_SUCCESS);
        return NAS_LMM_MSG_HANDLED;
    }

    /* CL��ģ����£��ر�LTEʱMMC�����·�DETACH����ԭ��ΪNON_3GPP_ATTACH��
       ��ʱLMMֻ��ִ�б���DETACH�����ܷ��Ϳտ���Ϣ */
    if((MMC_LMM_MO_DET_CS_ONLY != pstAppDetReq->ulDetachType)
        &&(MMC_LMM_DETACH_RESEAON_NON_3GPP_ATTACH == pstAppDetReq->ulDetachReason))
    {
        return NAS_LMM_ProcDetReqWithReseaonNon3gppAttach(pstAppDetReq);
    }

    /* ��ǰ֧��SVLTE���ߵ�ǰ��̬ΪC+L���ҵ�ǰΪPS ONLY����(�߲�ֻ���·�PS��DETACH����Ϊ�ӿ�Լ��)��
       ��GU��ʶֻ�ܽ��б���DETACHʱ�������ǵ�ǰ�Ƿ��ܷ���տ���Ϣ����������DETACH */
    /*lint -e960*/
    if ((MMC_LMM_MO_DET_PS_ONLY == pstAppDetReq->ulDetachType)
        &&(NAS_EMM_YES == NAS_LMM_IsSvlteOrLcNeedLocalDetach()))
    /*lint +e960*/
    {
        /* ִ�б���DETACH�Ĵ��� */
        return NAS_LMM_SvlteOrLcPsLocalDetachProc(pstAppDetReq);
    }

    /* ������ҹ������յ��û�����DETACH���������⴦�� */
    if((EMM_MS_SUSPEND == NAS_LMM_GetEmmCurFsmMS())
        ||(EMM_MS_RESUME == NAS_LMM_GetEmmCurFsmMS()))
    {
        NAS_EMM_SaveAppMsgPara(pstAppDetReq->ulMsgId, pstAppDetReq->ulOpId);
        return NAS_LMM_MsSuspendOrResumePreProcAppDetReq(pstAppDetReq);
    }
    ulRet = NAS_LMM_ProcAppDetReq(pstAppDetReq);
    return ulRet;

}

/*****************************************************************************
 Function Name  : NAS_LMM_IntraMsgProcess
 Discription    : MM PID �ڲ���Ϣ����
 Input          :
 Output         : None
 Return         : None
 History:
      1.  Name+ID  yyyy-mm-dd  Draft Enact
*****************************************************************************/
VOS_UINT32  NAS_LMM_IntraMsgProcess(VOS_VOID)
{
    VOS_UINT32                          ulLoop;
    NAS_EMM_EVENT_TYPE_ENUM_UINT32      ulEventType;
    VOS_UINT32                          ulRet               = NAS_LMM_SUCC;
    VOS_UINT8                          *pucMsg              = NAS_LMM_NULL_PTR;


    /* ѭ�������������� */
    for ( ulLoop = 0; ulLoop < (NAS_LMM_INTRA_MSG_MAX_NUM - 1); ulLoop++ )
    {
        /* ��ȡ������һ����Ϣ */
        pucMsg = NAS_LMM_GetIntraMsgFromQueue();
        if (NAS_LMM_NULL_PTR == pucMsg)
        {
            break;
        }

        /* ��ȡ�¼�����*/
        ulRet = NAS_LMM_GetEventType((MsgBlock *)pucMsg, &ulEventType);
        if(NAS_EMM_SUCC != ulRet)
        {
            NAS_LMM_PUBM_LOG_ERR("NAS_LMM_IntraMsgProcess, ERR !!");
            return ulRet;
        }

        /* ���ڲ���Ϣ����״̬������ */
        ulRet = NAS_LMM_MsgHandle((MsgBlock *)pucMsg, ulEventType);
    }

    return ulRet;
}



VOS_UINT32 NAS_LMM_BufMsgProcess(VOS_VOID)
{

    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulProcBufferMsgFlag;
    VOS_UINT32                          ulProcBufferMsgCount = 0;

    NAS_LMM_ERROR_CODE_ENUM_UINT32       enEmmBufMsgProcRet;

    ulRet                               = NAS_LMM_SUCC;
    ulProcBufferMsgFlag                 = NAS_LMM_CONTINUE_TO_PROC_BUF_MSG;

    /* �ֱ���MMC��EMM����������״̬����buff ��Ϣ */
    while ( NAS_LMM_CONTINUE_TO_PROC_BUF_MSG==ulProcBufferMsgFlag)
    {
        ulProcBufferMsgCount++;
        /* �����ǰ������Ϣ�������ж���NAS_EMM_BUFFER_MSG_COUNT����Ϣ��
        ** �����Ϣ���в�����ѭ��
        */
        if(NAS_EMM_BUFFER_MSG_COUNT<ulProcBufferMsgCount)
        {
            NAS_LMM_ClearBufMsgQue( NAS_LMM_PARALLEL_FSM_EMM , NAS_LMM_STORE_HIGH_PRIO_MSG );
            NAS_LMM_ClearBufMsgQue( NAS_LMM_PARALLEL_FSM_EMM , NAS_LMM_STORE_LOW_PRIO_MSG );

            NAS_LMM_PUBM_LOG_WARN("NAS_LMM_BufMsgProcess,Buffer Msg >NAS_EMM_BUFFER_MSG_COUNT");
            break;
        }

        /* EMM������Ϣ���� */
        enEmmBufMsgProcRet = NAS_LMM_EmmBufMsgProcess();

        if(NAS_LMM_NOT_TRAVEL_BUFFER_MSG == enEmmBufMsgProcRet)
        {   /* ������״̬����������������Ϣ��������ѭ�� */
            ulProcBufferMsgFlag         = NAS_LMM_STOP_PROCING_BUF_MSG;
        }
    }

    /* ���Ҫ֧��MMC�Ļ��洦������Ҫ�ڴ����Ӵ���
    */

    return ulRet;
}



VOS_UINT32  NAS_LMM_EmmBufMsgProcess(    VOS_VOID)
{
    VOS_UINT32                          ulRsltOutMsgFromQue = NAS_LMM_NOT_TRAVEL_BUFFER_MSG;
    VOS_UINT32                          ulEmmStableOrNot;
    NAS_LMM_BUFF_MSG_STRU				stBufMsg;
    VOS_UINT32                          ulRslt;

    /* ��ʱ������
    */
    stBufMsg.pBuffMsg                   = VOS_NULL_PTR;
    stBufMsg.ulEvtType                  = 0;

    /* ��ǰEMM�Ƿ��������̬ */
    ulEmmStableOrNot = NAS_EMM_JudgeStableState();

    if (NAS_LMM_STATE_IS_STABLE == ulEmmStableOrNot)
    {
        /*����ǰEMM������̬�����ȴ��������ȼ�������Ϣ */
        ulRsltOutMsgFromQue = NAS_LMM_OutMsgFromQue(NAS_LMM_PARALLEL_FSM_EMM,
                                        NAS_LMM_STORE_HIGH_PRIO_MSG,
                                        &stBufMsg);

        if (VOS_NULL_PTR == stBufMsg.pBuffMsg)
        {
            /*����ǰEMM������̬�����������ȼ�������Ϣ */
            ulRsltOutMsgFromQue = NAS_LMM_OutMsgFromQue(NAS_LMM_PARALLEL_FSM_EMM,
                                        NAS_LMM_STORE_LOW_PRIO_MSG,
                                        &stBufMsg);
        }

    }
    else if (NAS_LMM_STATE_IS_UNSTABLE == ulEmmStableOrNot)
    {
        /*����ǰEMMû�н�����̬�����������ȼ�������Ϣ */
        ulRsltOutMsgFromQue = NAS_LMM_OutMsgFromQue(NAS_LMM_PARALLEL_FSM_EMM,
                                        NAS_LMM_STORE_HIGH_PRIO_MSG,
                                        &stBufMsg);
    }
    else
    {
        /*������*/
    }

    if (VOS_NULL_PTR != stBufMsg.pBuffMsg)
    {
        /* ��ӡ��Ϣ���� */
        NAS_LMM_PUBM_LOG1_NORM( "NAS_LMM_EmmBufMsgProcess:ulEvtType = ",
                            stBufMsg.ulEvtType );

        /* ����״̬������ */
		(VOS_VOID)NAS_LMM_MsgHandle((MsgBlock *)stBufMsg.pBuffMsg, stBufMsg.ulEvtType);

        /* �ͷ���������Ϣ�����Buffer */
        ulRslt = NAS_COMM_FreeBuffItem(NAS_COMM_BUFF_TYPE_EMM,stBufMsg.pBuffMsg);

        if (NAS_COMM_BUFF_SUCCESS != ulRslt)
        {
            NAS_LMM_PUBM_LOG_WARN("NAS_LMM_EmmBufMsgProcess, Memory Free is not succ");
        }

        /*�ٴ����ɸøմ�����buff��Ϣ���ܲ������ڲ���Ϣ*/
        (VOS_VOID)NAS_LMM_IntraMsgProcess();
    }
    return ulRsltOutMsgFromQue;
}
VOS_UINT32  NAS_LMM_MmcBufMsgProcess(VOS_VOID)
{
    /* STUB FUNCTION */

    return NAS_LMM_NOT_TRAVEL_BUFFER_MSG;
}
VOS_UINT32  NAS_EMM_JudgeStableState(   VOS_VOID)
{
    if(EMM_MS_DEREG == NAS_LMM_GetEmmCurFsmMS())
    {
        return  NAS_LMM_STATE_IS_STABLE;
    }
    /* ��״̬ΪREGʱ����״̬����EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF��
       EMM_SS_REG_IMSI_DETACH_WAIT_RRC_REL_IND֮�⣬������̬ */
    else if(EMM_MS_REG == NAS_LMM_GetEmmCurFsmMS())
    {
        if (EMM_SS_REG_IMSI_DETACH_WATI_CN_DETACH_CNF == NAS_LMM_GetEmmCurFsmSS())
        {
            return  NAS_LMM_STATE_IS_UNSTABLE;
        }
        /* ���NORMAL_SERVICE��UPDATE��UPDATE_MM̬��LRRC REL��С��פ��ǰ�յ�ESM�������ؽ����������ڿտڷ���ʧ�ܣ�
           ����detach,�������ATTACH����
           ����:��������״̬�£��ҵ�ǰConn StateΪRelease�����У��յ�ESM�������ؽ��������ȸ����ȼ����棬
           �ȵ��յ�LRRCϵͳ��Ϣ����(�յ�ϵͳ��Ϣ��CONN State��ת��idle̬)*/
        if((NAS_EMM_CONN_RELEASING == NAS_EMM_GetConnState())
            /*&&(VOS_TRUE == NAS_LMM_GetEmmInfoIsEmerPndEsting())*/
            &&((EMM_SS_REG_NORMAL_SERVICE == NAS_LMM_GetEmmCurFsmSS())
                ||(EMM_SS_REG_ATTEMPTING_TO_UPDATE == NAS_LMM_GetEmmCurFsmSS())
                ||(EMM_SS_REG_ATTEMPTING_TO_UPDATE_MM == NAS_LMM_GetEmmCurFsmSS())))
         {
            return  NAS_LMM_STATE_IS_TRANSIENT;
         }
        return  NAS_LMM_STATE_IS_STABLE;
    }
    else if ((EMM_MS_RRC_CONN_EST_INIT == NAS_LMM_GetEmmCurFsmMS())
            ||(EMM_MS_RRC_CONN_REL_INIT == NAS_LMM_GetEmmCurFsmMS())
            ||((EMM_MS_SUSPEND == NAS_LMM_GetEmmCurFsmMS())
                &&(EMM_SS_SUSPEND_RRCORI_WAIT_MMC_SUSPEND == NAS_LMM_GetEmmCurFsmSS())))
    {
        return NAS_LMM_STATE_IS_TRANSIENT;
    }
    else
    {
        return  NAS_LMM_STATE_IS_UNSTABLE;
    }
}
VOS_VOID  NAS_LMM_PUBM_Init_FidInit( VOS_VOID )
{

    NAS_LMM_FSM_STATE_STRU              stEmmDstState;

    NAS_LMM_PUBM_LOG_INFO(
    "NAS_LMM_PUBM_Init_FidInit          START INIT...");

    /*��� PUBM �ڴ�Ϊȫ 0*/
    NAS_LMM_MEM_SET(&(g_stMmMainContext), 0, sizeof(NAS_LMM_MAIN_CONTEXT_STRU));

    /* g_stMmMainContext.stNasMmFsm MM״̬�� ��ʼ�� */
    NAS_LMM_MainContextInit();

    /* g_stMmMainContext.stNasMmAuxFsm ����״̬�� ��ʼ�� */
    NAS_LMM_AuxFsmInit();

    /* g_stMmMainContext.astMmTimerSta ��ʱ�� ��ʼ�� */
    NAS_LMM_TimerInit();

    /* g_stMmMainContext.pstMmIntraMsgQueue �ڲ���Ϣ���� ��ʼ��  */
    NAS_LMM_IntraMsgQueueInit();

    /* ��ʼ��ȫ�ֱ���g_stSMsgSysInfCounter,���ڼ�¼��Ե״̬���յ�ϵͳ��Ϣ�Ĵ��� */
    NAS_LMM_MEM_SET(&(g_stMsgSysInfCnt), 0, sizeof(NAS_LMM_SYSINFO_COUNT_STRU));

    #ifdef PS_ITT_PC_TEST_NAS_ST
    /* ��ʼ�� g_ulMmcSerRegFlag */
    NAS_Emm_Get_Mmc_Srv_Reg_Flag()= NAS_LMM_MMC_SER_REG_STAT_CLOSE_FLAG;
    #else
    NAS_Emm_Get_Mmc_Srv_Reg_Flag()= NAS_LMM_MMC_SER_REG_STAT_OPEN_FLAG;
    #endif

    /* ��״̬����ΪNULL */
    stEmmDstState.enFsmId               = NAS_LMM_PARALLEL_FSM_EMM;
    stEmmDstState.enMainState           = EMM_MS_NULL;
    stEmmDstState.enStaTId              = TI_NAS_EMM_STATE_NO_TIMER;


    stEmmDstState.enSubState            = EMM_SS_NULL_WAIT_APP_START_REQ;

    NAS_LMM_StaTransProc(stEmmDstState);

    /* MM������ʼ�� */
    NAS_LMM_DebugInit();

    /* xiongxianghui00253310 modify for ftmerrlog begin */
    #if (FEATURE_PTM == FEATURE_ON)
    NAS_LMM_FtmInfoInit();
    /*ȫ����Ϣ����pid��ʼ����ʱ��ִ�У�ִ��һ�Σ����治���ظ�*/
    LNAS_InitErrLogGloInfo(NAS_EMM_GetErrlogGloInfoAddr());
    #endif
    /* xiongxianghui00253310 modify for ftmerrlog end   */

    #if 0
    /*��ʼ��UE����PS���Զ�ATTACH*/
    NAS_MML_SetPsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);

    /*��ʼ��UE����CS���Զ�ATTACH*/
    NAS_MML_SetCsAttachAllowFlg(NAS_EMM_AUTO_ATTACH_ALLOW);
    #endif
    return;
}



VOS_VOID  NAS_LMM_PUBM_Init( VOS_VOID )
{
    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PUBM_Init                  START INIT...");

    /*��� PUBM �ڴ�Ϊȫ 0*/
    NAS_LMM_MEM_SET(&(g_stMmMainContext), 0, sizeof(NAS_LMM_MAIN_CONTEXT_STRU));

    /* g_stMmMainContext.stNasMmFsm MM״̬�� ��ʼ�� */
    NAS_LMM_MainContextInit();

    /* g_stMmMainContext.stNasMmAuxFsm ����״̬�� ��ʼ�� */
    NAS_LMM_AuxFsmInit();

    /* g_stMmMainContext.astMmTimerSta ��ʱ�� ��ʼ�� */
    NAS_LMM_TimerInit();

    /* g_stMmMainContext.pstMmIntraMsgQueue �ڲ���Ϣ���� ��ʼ�� */
    NAS_LMM_IntraMsgQueueInit();

    return;
}
VOS_VOID    NAS_LMM_MainContextInit( VOS_VOID )
{
    /* g_stMmMainContext.stNasMmFsm--EMM״̬�� ��ʼ��*/
    NAS_EMM_FsmInit();

    /* g_stMmMainContext.stNasMmFsm--MMC״̬�� ��ʼ��*/
    /*NAS_EMMC_FsmInit();*/

    /* g_stMmMainContext.stNasMmFsm--SECU״̬�� ��ʼ��*/
    NAS_Secu_FsmInit();

    /* ��ʼ��EMM������ */
    NAS_COMM_InitBuff(NAS_COMM_BUFF_TYPE_EMM);

    return;
}
VOS_VOID    NAS_LMM_AuxFsmInit( VOS_VOID)
{
    NAS_LMM_AUXILIARY_FSM_STRU          *pMmAuxFsm;


    NAS_LMM_PUBM_LOG_NORM(
    "NAS_LMM_AuxFsmInit                 START INIT...");
    /* ��ǰ״̬��ʼ�� */
    pMmAuxFsm = NAS_LMM_GetMmAuxFsmAddr();

    /*RRC����״̬*/
    pMmAuxFsm->ucRrcConnState           = NAS_EMM_CONN_IDLE;

    /*NAS APPע��״̬*/
    pMmAuxFsm->enNasAppRegState         = NAS_APP_RS_NOT_REG_AND_NOT_SRCH_OPERATOR;

    /*UPDATE STATUS*/
    pMmAuxFsm->ucEmmUpStat              = EMM_US_UPDATED_EU1;

    /* LTE״̬����Ϊ���� */
    NAS_EMM_GetCurLteState()            = NAS_LMM_CUR_LTE_SUSPEND;

    /* ���� RegisterState,ServiceState */
    NAS_LMM_GetNasMmcRegisterState()    = MMC_LMM_REGISTER_STATUS_BUTT;

    NAS_LMM_GetNasMmcServiceState()     = MMC_LMM_SERVICE_STATUS_TYPE_BUTT;

    NAS_LMM_GetNasMmcCSServiceState()     = MMC_LMM_SERVICE_STATUS_TYPE_BUTT;

    NAS_LMM_GetNasRejCause12Flag()      = NAS_LMM_REJ_CAUSE_NOT_12;
    return;
}


VOS_VOID    NAS_LMM_TimerInit(           VOS_VOID)
{
    /*��ʼ������״̬��ʱ��*/
    NAS_LMM_InitAllStateTimer();

    /*��ʼ������Э�鶨ʱ��*/
    NAS_LMM_InitAllPtlTimer();

    return;
}


VOS_VOID  NAS_LMM_IntraMsgQueueInit(   VOS_VOID)
{
    NAS_LMM_INTRA_MSG_QUEUE_STRU        *pstMmIntraMsgQueue;

    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_IntraMsgQueueInit: ENTER.");

    pstMmIntraMsgQueue = NAS_LMM_GetMmIntrMsgQueueAddr();

    pstMmIntraMsgQueue->usHeader        =NAS_LMM_INTRA_MSG_HEAD_INIT;
    pstMmIntraMsgQueue->usTail          =NAS_LMM_INTRA_MSG_TAIL_INIT;


    return;
}



VOS_UINT32  NAS_LMM_PreProcIntraSystemInfoInd(MsgBlock  * pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulCurEmmStat;
    EMMC_EMM_SYS_INFO_IND_STRU          *pstsysinfo;

    pstsysinfo = (EMMC_EMM_SYS_INFO_IND_STRU *)pMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcIntraSystemInfoInd: enter.");
    /*ulRet                               = NAS_LMM_MSG_DISCARD;*/


    /* ���RRC����״̬�����ͷŹ����У�����ΪIDLE */
    if(NAS_EMM_CONN_RELEASING == NAS_EMM_GetConnState())
    {
        NAS_EMM_SetConnState(NAS_EMM_CONN_IDLE);
    }


    /*��ȡEMM��ǰ״̬*/
    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT,
                                            NAS_EMM_CUR_SUB_STAT);

    if((EMMC_EMM_CELL_STATUS_ANYCELL == pstsysinfo->ulCellStatus)
     &&(NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_DEREG, EMM_SS_DEREG_NO_IMSI) == ulCurEmmStat))
     {
         NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PreProcIntraSystemInfoInd:DEREG NO IMSI anycell");
         return NAS_LMM_MSG_DISCARD;
    }

    switch(ulCurEmmStat)
    {
        /* ����������յ�ϵͳ��Ϣ����������*/
        #if 0
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RRC_CONN_REL_INIT, EMM_SS_RRC_CONN_WAIT_REL_CNF):
                ulRet = NAS_LMM_STORE_LOW_PRIO_MSG;
                NAS_LMM_PUBM_LOG_NORM("Store SYS INFO AT EMM_MS_RRC_CONN_REL_INIT AND EMM_SS_RRC_CONN_WAIT_REL_CNF.");

                /*ͳ�Ƴ��ָ���*/
                NAS_LMM_CountSysInfo(EMM_SS_RRC_CONN_WAIT_REL_CNF);
                break;
        #endif
        case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RESUME, EMM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND):
                ulRet = NAS_EMM_PreProcSysWhenResumingReverse(pstsysinfo);
                break;

        default:
                ulRet = NAS_LMM_MSG_DISCARD;
                break;
    }

    return ulRet;

}
VOS_UINT32  NAS_LMM_MsNullSsNullReadingUsimTimerExp (MsgBlock  * pMsg)
{
    NAS_LMM_FSM_STATE_STRU               EmmState;
    VOS_UINT32                          ulSendResult;

    NAS_LMM_UEID_STRU                    *pstUeId;


    (VOS_VOID)                          (pMsg);

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_MsNullSsNullReadingUsimTimerExp is entered");

    /*�����ȡ����GUTI��Ч���ǰ�ȫ��������Ч����ɾ�����غ�NV�е�������*/
    pstUeId                             = NAS_LMM_GetEmmInfoUeidAddr();

    if((NAS_EMM_BIT_NO_SLCT == pstUeId->bitOpGuti)
        &&(EMM_CUR_SECU_NOT_EXIST!= NAS_LMM_GetSecuCurFsmCS()))
    {
        NAS_EMM_ClearCurSecuCntxt();
        NAS_LMM_WriteEpsSecuContext(NAS_NV_ITEM_DELETE);
    }

    /* Read NV Files*/
    NAS_LMM_ProcessNvData();

    /* ��MMC����MMC_EMM_START_REQ(��������)��Ϣ */
    ulSendResult = NAS_EMM_PLMN_SendMmcStartReq(EMMC_EMM_START_REQ_CAUSE_NORMAL_START);
    if (NAS_EMM_SEND_MMC_ERR == ulSendResult)
    {
        /* ��ӡ������ERROR_LEVEL */
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_PLMN_SendMmcStartReq : Send IntraMsg Failure!!!");
        return NAS_LMM_MSG_HANDLED;
    }

    /* ����TI_NAS_EMM_WAIT_MMC_START_CNF_TIMER */
    NAS_LMM_StartStateTimer(TI_NAS_EMM_WAIT_MMC_START_CNF_TIMER);

    /* ��״̬ת����MS_NULL + SS_WAIT_MMC_START_CNF״̬ */
    EmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
    EmmState.enMainState                = EMM_MS_NULL;
    EmmState.enSubState                 = EMM_SS_NULL_WAIT_MMC_START_CNF;
    EmmState.enStaTId                   = TI_NAS_EMM_WAIT_MMC_START_CNF_TIMER;
    NAS_LMM_StaTransProc(EmmState);

    return NAS_LMM_MSG_HANDLED;
}


VOS_VOID    NAS_LMM_EmmAndMmcGlobInfoClear( VOS_VOID )
{
    /* EMM g_stEmmInfo ��� */
    NAS_LMM_MEM_SET(                     NAS_LMM_GetEmmInfoAddr(),
                                        0,
                                        sizeof(NAS_LMM_PUB_INFO_STRU));

    /*������ȫ�ֱ�������*/
    NAS_EMM_GET_SEND_COUNTER() = NAS_NVIM_SEND_COUNTER_NONE;

    /*������ʼ����ȫ�ļ�����Ϊ0 */
    NAS_EMM_GET_USIM_SECU_FILE_LEN() = 0 ;

    NAS_EMM_GET_USIM_FILE_TYPE() = NAS_LMM_USIM_OPTIONAL_FILE;

    return;
}
VOS_VOID    NAS_LMM_EmmSubModGlobInit(VOS_VOID )
{
    NAS_EMM_AttachInit();
    NAS_EMM_DetachInit();
    NAS_EMM_TAU_Init();
    NAS_EMM_SER_Init();
    NAS_EMM_GutiInit();
    NAS_EMM_IdenInit();
    NAS_EMM_MrrcInit();
    NAS_EMM_SecuInit();

    return;
}


VOS_VOID  NAS_LMM_EmmSubModGlobClear(VOS_VOID)
{
    NAS_EMM_AttachInit();
    NAS_EMM_DetachInit();
    /*NAS_EMM_TauClearResouce();*/
    /*NAS_EMM_SER_RelResource();*/
    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_SER_ClearResource();
    NAS_EMM_GutiInit();
    NAS_EMM_IdenInit();
    NAS_EMM_SecuInit();
    NAS_EMM_MrrcInit();

    return;
}

VOS_VOID NAS_LMM_SetUsimInvalidDetachType( VOS_VOID )
{
    VOS_UINT32                          ulCurEmmStat;

    /* ����DETACH REQ��Ϣ�е�DETACH��������ͳһԭ��ģ���Ҫ���ǵ��������û������
       DETACH���ͣ�UEģʽ��ע���򣬰ο�ʱ�������û������DETACH���ͣ��������ʹ��
       ͳһ�ı���ԭ������Ҫ�ڱ���ǰ�ȸ�һ�����ʵ�ֵ��Ϊ�û������DETACH���ͣ�
       Ŀǰ����дԭ��:�����ATTACH�����У������ATTACH������д�û������DETACH
       ���ͣ��������ATTACH�����У�����ΪEPS/IMSI */

    /* ����Ժ�ο�����DETACH���̵ĳ������ӣ�����Ҫ�������ӵĳ����������д�û�
       �����DETACH����*/

    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT,
                                            NAS_EMM_CUR_SUB_STAT);


    if ((ulCurEmmStat != NAS_LMM_PUB_COMP_EMMSTATE(    EMM_MS_REG_INIT,
                                                        EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF))
        && (ulCurEmmStat != NAS_LMM_PUB_COMP_EMMSTATE(  EMM_MS_REG_INIT,
                                                        EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF))
        && (ulCurEmmStat != NAS_LMM_PUB_COMP_EMMSTATE(  EMM_MS_REG_INIT,
                                                        EMM_SS_ATTACH_WAIT_RRC_DATA_CNF)))
    {
        NAS_EMM_GLO_SetDetTypeMo(MMC_LMM_MO_DET_CS_PS);

        return ;
    }

    /* �����ATTACH�����еİο��������ATTACH������дDETACH���� */
    if (MMC_LMM_ATT_TYPE_COMBINED_EPS_IMSI == NAS_EMM_GLO_GetCnAttReqType())
    {
        NAS_EMM_GLO_SetDetTypeMo(MMC_LMM_MO_DET_CS_PS);
    }
    else
    {
        NAS_EMM_GLO_SetDetTypeMo(MMC_LMM_MO_DET_PS_ONLY);
    }
}
VOS_UINT32  NAS_LMM_PreProcMsgUsimStatusInd( MsgBlock  *pMsg )
{
    VOS_UINT32                          ulRslt         = NAS_LMM_MSG_DISCARD;
    VOS_UINT32                          ulCurEmmStat;
    MMC_LMM_USIM_STATUS_REQ_STRU       *pstUsimStatus  = NAS_LMM_NULL_PTR;

    pstUsimStatus                     = (MMC_LMM_USIM_STATUS_REQ_STRU*)pMsg;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMsgUsimStatusInd is entered");

    /* �����״̬ѹջ���ȵ���״̬ */
    /*NAS_LMM_IfEmmHasBeenPushedThenPop();*/

    ulCurEmmStat = NAS_LMM_PUB_COMP_EMMSTATE(NAS_EMM_CUR_MAIN_STAT,
                                            NAS_EMM_CUR_SUB_STAT);

    NAS_LMM_SetEmmInfoRejCause2Flag(NAS_EMM_REJ_NO);

    /*����ܾ�#18��ʶ*/
    NAS_EMMC_SetRejCause18Flag( NAS_EMM_REJ_NO);

    /*����ܾ�#18�б�*/
    NAS_LMM_MEM_SET(NAS_EMMC_GetRej18PlmnListAddr(), 0, sizeof(NAS_MM_PLMN_LIST_STRU));

    /* �������Ч */
    if (MMC_LMM_USIM_VALID == pstUsimStatus->ulUsimState)
    {
        /* �Ȳ忨�������޲�֧�� */

    }
    else /* �������Ч */
    {
        /*����USIM����Ч*/
        NAS_LMM_GetSimState() =  NAS_LMM_SIM_STATUS_UNAVAILABLE;

         NAS_LMM_SetPsSimValidity(NAS_LMM_SIM_VALID);

        switch(ulCurEmmStat)
        {
            /* �ڿ������߹ػ������У�MMC�����·���״̬���棬������ */
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_APP_START_REQ):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_READING_USIM):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_MMC_START_CNF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_RRC_START_CNF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_SWITCH_OFF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_MMC_STOP_CNF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_NULL, EMM_SS_NULL_WAIT_RRC_STOP_CNF):

            /* GU->L����ѡ���ض�������л���GUģ�����MMC���յ�Lģ�Ľ�һ���GUģ�Ļ��˽��ǰ�������·���״̬���棻
               L->GU����ѡ���ض�������л���Lģ�����MMC���յ�GUģ�Ľ�һ���Lģ�Ļ��˽��ǰ�������·���״̬����
               �������״̬������ */
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_SUSPEND,EMM_SS_SUSPEND_WAIT_END):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RESUME,EMM_SS_RESUME_RRCRSM_WAIT_OTHER_RESUME):
			/* lihong00150010 emergency tau&service begin */
            /* MMC�����յ�LMM�Ĺ���ظ��󣬲��·���״̬���棬������ */
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_SUSPEND,EMM_SS_SUSPEND_MMCORI_WAIT_OTHER_SUSPEND):
                    ulRslt = NAS_LMM_MSG_HANDLED;
                    break;
			/* lihong00150010 emergency tau&service end */
            /* �Ѿ�������Ϊ�޿�״̬��DETACH���������쳣������ɺ󣬻ᴦ���ο����� */
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_DEREG_INIT,EMM_SS_DETACH_WAIT_CN_DETACH_CNF):
					/* lihong00150010 emergency tau&service begin */
                    /* ���ðο���ʶ */
                    NAS_EMM_GLO_AD_GetUsimPullOutFlag() = VOS_TRUE;
					/* lihong00150010 emergency tau&service end */
                    ulRslt = NAS_LMM_MSG_HANDLED;
                    break;

            /* ֱ�ӻظ�MMC_LMM_USIM_STATUS_CNF */
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_DEREG, EMM_SS_DEREG_NO_IMSI):
                    NAS_EMM_SendMmcUsimStatusCnf();
                    ulRslt = NAS_LMM_MSG_HANDLED;
                    break;

            /*ע��״̬�±�bar������bar���ͽ��в�ͬ����*/
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG, EMM_SS_REG_WAIT_ACCESS_GRANT_IND):
                    ulRslt = NAS_LMM_MsRegSsWaitAccessGrantIndMsgUsimStausInd();
                    break;

            /*ע�������,�����������ӽ��в�ͬ����*/
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_PDN_RSP):
                    ulRslt = NAS_LMM_MsRegInitSsAttachWaitEsmPdnRspMsgUsimStausInd();
                    break;

            /* �Ѿ�������Ϊ�޿�״̬�������ڲ���·�ͷ���Ϣ���յ��ڲ���·�ͷ���Ϣ���ٴ����ο����� */


            /*תDEREG_WAIT_MRRC_REL_CNF�������ڲ���·�ͷ���Ϣ*/


            /*���������л�����·�ͷŹ����У������ȼ�����*/
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RRC_CONN_REL_INIT,EMM_SS_RRC_CONN_WAIT_REL_CNF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RRC_CONN_EST_INIT,EMM_SS_RRC_CONN_WAIT_EST_CNF):
                    ulRslt = NAS_LMM_STORE_HIGH_PRIO_MSG;
                    break;

            /*��״̬�ȴ������ͷ��У�״̬ת����DEREG�ȴ������ͷţ�����REL_REQ��Ϣ*/
            /*case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_RRC_REL_IND):*/
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_AUTH_INIT,EMM_SS_AUTH_WAIT_CN_AUTH):
                    ulRslt = NAS_LMM_MsAnyStateSsWaitRrcRelIndMsgUsimStausInd();
                    break;

            /*ע��״̬�£��������෢��DETACH REQ��Ϣ*/
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_CN_ATTACH_CNF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_ESM_BEARER_CNF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG_INIT,EMM_SS_ATTACH_WAIT_RRC_DATA_CNF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_NORMAL_SERVICE):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_REG,EMM_SS_REG_ATTEMPTING_TO_UPDATE):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_TAU_INIT,EMM_SS_TAU_WAIT_CN_TAU_CNF):
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_SER_INIT,EMM_SS_SER_WAIT_CN_SER_CNF):
                    NAS_LMM_SetUsimInvalidDetachType();
					/* lihong00150010 emergency tau&service begin */
                    /* ���ðο���ʶ */
                    NAS_EMM_GLO_AD_GetUsimPullOutFlag() = VOS_TRUE;
					/* lihong00150010 emergency tau&service end */
                    NAS_EMM_SendDetachReqMo();
                    ulRslt = NAS_LMM_MSG_HANDLED;
                    break;

            /* �����������ӽ��в�ͬ���� */
            case    NAS_LMM_PUB_COMP_EMMSTATE(EMM_MS_RESUME,EMM_SS_RESUME_RRCORI_WAIT_SYS_INFO_IND):
                    ulRslt = NAS_LMM_MsResumeSsResumeRrcOriWaitSysInfoIndMsgUsimStausInd();
                    break;

            /* ����״̬(����EMM_MS_RESUME+EMM_SS_RESUME_MMCORI_WAIT_SYS_INFO_IND):
                �����ͷ���Դ��תDEREG+NOIMSI����MMC�ظ�MMC_LMM_USIM_STATUS_CNF;
               MMC�ȷ��˿�״̬���棬���յ�LMM�Ĺ�����Ϣ�������쳣�ͻ����
               EMM_MS_SUSPEND+EMM_SS_SUSPEND_RRCORI_WAIT_OTHER_SUSPEND״̬���յ���״̬����
               */
            default :

                    /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
                    NAS_EMM_PUB_SendEsmStatusInd(       EMM_ESM_ATTACH_STATUS_DETACHED);
                    /* ֪ͨAPP_DETACH_IND(APP_MM_DETACH_ENTITY_ME)*/
                    NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);
                    #if (FEATURE_PTM == FEATURE_ON)
                    NAS_EMM_LocalDetachErrRecord(EMM_ERR_LOG_LOCAL_DETACH_TYPE_OTHER);
                    #endif
                    NAS_EMM_ClearAppMsgPara();
                    NAS_EMM_ProcLocalNoUsim();
                    ulRslt = NAS_LMM_MSG_HANDLED;
                    break;
        }
    }

    return ulRslt;
}



VOS_UINT32 NAS_LMM_PreProcUsimSetCnf(   MsgBlock  * pMsg)
{
    PS_USIM_SET_FILE_CNF_STRU          *pUsimSetCnf;

    (void)(pMsg);

    pUsimSetCnf = (PS_USIM_SET_FILE_CNF_STRU *)pMsg;

    NAS_LMM_PUBM_LOG2_INFO("NAS_LMM_PreProcUsimSetCnf: usEfId = , ulResult = ",
                                        pUsimSetCnf->usEfId,
                                        pUsimSetCnf->ulResult);

    return  NAS_LMM_MSG_HANDLED;
}



VOS_VOID  NAS_LMM_SetImsiUnavail( VOS_VOID )
{
    NAS_LMM_UEID_STRU                    *pstUeId;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_SetImsiUnavail IS ENTERED ");
    pstUeId                             = NAS_LMM_GetEmmInfoUeidAddr();

    /*����µ�IMSI*/
    pstUeId->bitOpImsi                  = NAS_EMM_BIT_NO_SLCT;

    NAS_LMM_MEM_SET( NAS_LMM_GetEmmInfoUeidImsiAddr(), 0, NAS_MM_MAX_UEID_BUF_SIZE);

    return;
}


/*****************************************************************************
 Function Name   : NAS_PUB_GetSleepInfo
 Description     : ����NAS״̬�ж��ʲ�����˯��
 Input           : None
 Output          : None
 Return          : VOS_UINT32
                   0 ������
                   1 ����
 History         :
    1.sunbing49683      2009-8-17  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_PUB_GetSleepInfo( VOS_VOID)
{
#if 0
    /*EMM����ע��̬�����߷�ע��̬������MMC������������̬������˯��*/
    if(((EMM_MS_REG == NAS_EMM_CUR_MAIN_STAT)||(EMM_MS_DEREG == NAS_EMM_CUR_MAIN_STAT))
      &&( ( MMC_MS_ON_PLMN == NAS_LMM_GetMmcCurFsmMS())
      ||(MMC_MS_WAIT_FOR_PLMN== NAS_LMM_GetMmcCurFsmMS())
      ||(MMC_MS_NO_IMSI== NAS_LMM_GetMmcCurFsmMS())
      ||(MMC_SS_WAIT_FOR_ACTION_RESULT == NAS_LMM_GetMmcCurFsmSS())) )
    {
       return 1;
    }
    else
    {
       return 0;
    }
#endif
    return 0;
}
VOS_VOID  NAS_LMM_SendStubSoftUsimCnf( VOS_UINT32 ulReadUsimRslt )
{
    PS_USIM_GET_FILE_CNF_STRU     *pstUsimCnf = NAS_LMM_NULL_PTR;

    NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_SendStubSoftUsimCnf entered.ulReadUsimRslt:",ulReadUsimRslt);

    /*������Ϣ�ڴ�*/
    pstUsimCnf = (VOS_VOID *)NAS_LMM_GetSendIntraMsgBuffAddr(sizeof(PS_USIM_GET_FILE_CNF_STRU));

    /*�ж�����������ʧ�ܴ�ӡ�����˳�*/
    if (NAS_EMM_NULL_PTR == pstUsimCnf)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_SendStubSoftUsimCnf: MSG ALLOC ERR!");
        return;

    }

    /* ��дVOS��Ϣͷ: dopra��Ϣ���������д*/
    pstUsimCnf->ulSenderCpuId     = VOS_LOCAL_CPUID;
    pstUsimCnf->ulSenderPid       = WUEPS_PID_USIM;
    pstUsimCnf->ulReceiverCpuId   = VOS_LOCAL_CPUID;
    pstUsimCnf->ulReceiverPid     = PS_PID_MM;

    /* ���DOPRA��ϢID*/
    pstUsimCnf->ulMsgName = PS_USIM_GET_FILE_RSP;

    /* ����ļ�ID */
    pstUsimCnf->usEfId = NAS_USIM_FILE_ID_BUTT;

    /* ����ȡ��� */
    pstUsimCnf->ulResult = ulReadUsimRslt;

    /* ��д dopra��Ϣ����*/
    pstUsimCnf->ulLength = sizeof(PS_USIM_GET_FILE_CNF_STRU)
                                    - NAS_LMM_VOS_HEADER_LEN;

    NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_SendStubSoftUsimCnf entered.pstUsimCnf->ulResult:",pstUsimCnf->ulResult);

    /*�����ڲ���Ϣ�����*/
    NAS_EMM_SEND_INTRA_MSG(          pstUsimCnf);

}


VOS_UINT32  NAS_LMM_ReadUsimEmmInfoFile( VOS_VOID )
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulReturn;
    VOS_UINT32                     ulUsimSerEpsSecu;



    /*��ȡNV�е�IMSI*/
    ulRslt = NAS_LMM_MatchNvImsi();
    if (NAS_EMM_SUCC != ulRslt)
    {   /* IMSI ����ͬ */
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadUsimEmmInfoFile: IMSI DIFF !!");

        /*ɾ��NV�е�EPS SECURITY CONTEXT*/
        NAS_LMM_WriteNvEpsSecurityContext(NAS_NV_ITEM_DELETE);

        /*ɾ��NV�е�PS LOC*/
        NAS_LMM_WriteNvPsLoc(NAS_NV_ITEM_DELETE);


        NAS_LMM_UpdateNvImsi(NAS_LMM_GetEmmInfoUeidImsiAddr());
    }
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /* �ж�SerTable�Ƿ�֧�ְ�ȫ������ */
    /* ulRslt = NAS_LMM_GetUsimOptionFileStatus(NAS_LMM_SER_TABLE_EMMINFO_BIT);*/
    ulUsimSerEpsSecu = NAS_EMM_IsServiceAvailable(USIM_SVR_EPS_INFO);
    /*PC REPLAY MODIFY BY LEILI END*/
    if(PS_USIM_SERVICE_AVAILIABLE == ulUsimSerEpsSecu)

    {/* ӲUSIM�����£�����֧�ְ�ȫ������ */

        /* ӲUSIM֧�ְ�ȫ������ */
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_ReadUsimEmmInfoFile:Read Hard USIM secu Para");

        /*��ȡӲUSIM�е�EPS SECURITY CONTEXT*/
        NAS_LMM_ReadSimEpsSecContext();

        /*��ȡӲUSIM�е�PS LOC*/
        NAS_LMM_ReadSimPsLoc();

        ulReturn = NAS_LMM_ERR_CODE_HARD_USIM;
    }
    else
    {
        /* ��USIM������ӲUSIM��֧�ְ�ȫ�����ģ����ȡNV�еİ�ȫ������ */
        NAS_LMM_PUBM_LOG1_INFO(
            "NAS_LMM_ReadUsimEmmInfoFile: SOFT USIM BRANCH: ulUsimSerEpsSecu, ulUsimSerEpsLoc",
            ulUsimSerEpsSecu);

        /*��ȡNV�е�EPS SECURITY CONTEXT*/
        NAS_LMM_ReadNvEpsSecContext();

        /*��ȡNV�е�PS LOC*/
        NAS_LMM_ReadNvPsLoc();

        ulReturn = NAS_LMM_SUCC;
    }

    return ulReturn;
}
/* lihong00150010 emergency tau&service begin */
#if 0

VOS_VOID  NAS_LMM_ProcUsimStatusIndWhenUsimInvalid( VOS_VOID )
{
    NAS_LMM_FSM_STATE_STRU                  stEmmState;

    /* ���ÿ���Ч */
    NAS_LMM_GetSimState() =  NAS_LMM_SIM_STATUS_UNAVAILABLE;

    /* ת��UsimStatusInd��MMC */
    /*NAS_EMMC_SendIntraUsimStatusInd(USIMM_CARD_STATUS_ABSENT);*/

    /* ͣ���ж�ʱ�� */
    NAS_LMM_StopAllPtlTimer();
    NAS_LMM_StopAllStateTimer();

    /* �ͷŸ���ģ�����Դ */
    NAS_LMM_EmmSubModGlobClear();

    /* ת״̬ΪNoImsi */
    stEmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
    stEmmState.enMainState                = EMM_MS_DEREG;
    stEmmState.enSubState                 = EMM_SS_DEREG_NO_IMSI;
    stEmmState.enStaTId                   = TI_NAS_EMM_STATE_NO_TIMER;
    NAS_LMM_StaTransProc(stEmmState);

    return;
}
#endif
/* lihong00150010 emergency tau&service end */

VOS_UINT32  NAS_LMM_MsNullSsWaitUsimReadyMsgUsimStatusInd(VOS_VOID)
{
    NAS_LMM_FSM_STATE_STRU               stEmmState;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_MsNullSsWaitUsimReadyMsgUsimStatusInd is entered");

    /*ֹͣ��ʱ��*/
    NAS_LMM_StopStateTimer(TI_NAS_LMM_TIMER_WAIT_USIM_READY_START);

    /*ת״̬EMM_MS_NULL��EMM_SS_NULL_WAIT_APP_START_REQ*/
    stEmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
    stEmmState.enMainState                = EMM_MS_NULL;
    stEmmState.enSubState                 = EMM_SS_NULL_WAIT_APP_START_REQ;
    stEmmState.enStaTId                   = TI_NAS_EMM_STATE_NO_TIMER;
    NAS_LMM_StaTransProc(stEmmState);

    /* �п�ʱ,����Ƿ���Ҫ�Զ�����*/
    /* V7R1�����Զ������� */
    /*NAS_EMM_CheckAutoStartFeature();*/

    return NAS_LMM_MSG_HANDLED;

}
VOS_VOID    NAS_LMM_PUBM_ClearResource(VOS_VOID)
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PUBM_ClearResource: enter.");

    /*�ͷ�stNasMmFsm �е�״̬ѹջ*/
    NAS_LMM_FSM_ClearStackResource();

    /*�ͷ�stNasMmFsm ��3��״̬������Ϣ����*/
    NAS_LMM_BufMsgQueFree();

    /*�ͷ��ڲ���Ϣ���в���������
    ��ΪDETACH��ɺ���ʱ��֪ͨMMC ACTION_RESULT��������
    ����MMC�ղ�����Ϣ��
    ����NAS_EMM_MsDrgInitSsWtCnDetCnfMsgIntraConnectFailInd*/
    /*NAS_LMM_IntraMsgQueueInit();*/

    return;
}
VOS_VOID    NAS_LMM_DeregReleaseResource(VOS_VOID)
{

    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_DeregReleaseResource: enter.");

    /* �رյ�ǰEMM�ĳ�Del Forb Ta Proid֮���״̬��ʱ��, Del Forb Ta Proidֻ���ڹػ�ʱֹͣ*/
    NAS_LMM_StopAllStateTimerExceptDelForbTaProidTimer();

    /* ֹͣ����EMMЭ�鶨ʱ��*/
    NAS_LMM_StopAllEmmPtlTimer();

    /*PUBM����*/
    NAS_LMM_PUBM_ClearResource();

    /*EMM����ģ������*/
    NAS_EMM_PUBU_ClearResource();
    NAS_EMM_SECU_ClearResource();

    NAS_EMM_Attach_ClearResourse();
    NAS_EMM_Detach_ClearResourse();

    NAS_EMM_TAU_ClearResouce();
    NAS_EMM_SER_ClearResource();

    /*GUTI/IDEN/MRRC/PLMNģ�������Ҫ�ͷŵ�����,��������Ҫ��
    ������ģ��ĺ����������������*/

    return;
}
/*****************************************************************************
 Function Name   : NAS_LMM_ImsiDetachReleaseResource
 Description     : IMSI DETACH֮��������Դ
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1. lihong00150010   2011-09-28  Draft Enact

*****************************************************************************/
VOS_VOID NAS_LMM_ImsiDetachReleaseResource( VOS_VOID )
{

    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_ImsiDetachReleaseResource: enter.");

    NAS_EMM_Detach_ClearResourse();

    return;
}

VOS_UINT32  NAS_LMM_MsRegSsWaitAccessGrantIndMsgUsimStausInd(VOS_VOID)
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_MsRegSsWaitAccessGrantIndMsgUsimStausInd is enter.");

    if(NAS_EMM_SUCC != NAS_EMM_JudgeBarType(NAS_EMM_BAR_TYPE_MO_SIGNAL))
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_LMM_MsRegSsWaitAccessGrantIndMsgUsimStausInd: MO-SIGNALLING is not bar");

        NAS_LMM_SetUsimInvalidDetachType();

        /* lihong00150010 emergency begin */
        /* ���ðο���ʶ */
        NAS_EMM_GLO_AD_GetUsimPullOutFlag() = VOS_TRUE;
        /* lihong00150010 emergency end */

        NAS_EMM_SendDetachReqMo();

        return  NAS_LMM_MSG_HANDLED;
    }

    /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
    NAS_EMM_PUB_SendEsmStatusInd(       EMM_ESM_ATTACH_STATUS_DETACHED);

    /* ֪ͨAPP_DETACH_IND(APP_MM_DETACH_ENTITY_ME)*/
    NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

    #if (FEATURE_PTM == FEATURE_ON)
    NAS_EMM_LocalDetachErrRecord(EMM_ERR_LOG_LOCAL_DETACH_TYPE_OTHER);
    #endif
    NAS_EMM_ClearAppMsgPara();

    NAS_EMM_ProcLocalNoUsim();

    return NAS_LMM_MSG_HANDLED;
}
VOS_UINT32  NAS_LMM_MsRegInitSsAttachWaitEsmPdnRspMsgUsimStausInd( VOS_VOID )
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_MsRegInitSsAttachWaitEsmPdnRspMsgUsimStausInd is enter.");

    /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
    NAS_EMM_PUB_SendEsmStatusInd(       EMM_ESM_ATTACH_STATUS_DETACHED);

    /* ֪ͨAPP_DETACH_IND(APP_MM_DETACH_ENTITY_ME)*/
    NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

    #if (FEATURE_PTM == FEATURE_ON)
    NAS_EMM_LocalDetachErrRecord(EMM_ERR_LOG_LOCAL_DETACH_TYPE_OTHER);
    #endif
    NAS_EMM_ClearAppMsgPara();

    NAS_EMM_ProcLocalNoUsim();

    if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {

        /* �·�RelReq */
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    return NAS_LMM_MSG_HANDLED;

}
/*****************************************************************************
 Function Name   : NAS_LMM_MsResumeSsResumeRrcOriWaitSysInfoIndMsgUsimStausInd
 Description     : ��״̬RESUME+RESUME_WAIT_SYS_INFO_IND���յ�USIM STATUS IND��Ϣ(����Ч)
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-7-2  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_LMM_MsResumeSsResumeRrcOriWaitSysInfoIndMsgUsimStausInd( VOS_VOID )
{
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_MsResumeSsResumeRrcOriWaitSysInfoIndMsgUsimStausInd is enter.");

    /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
    NAS_EMM_PUB_SendEsmStatusInd(       EMM_ESM_ATTACH_STATUS_DETACHED);

    /* ֪ͨAPP_DETACH_IND(APP_MM_DETACH_ENTITY_ME)*/
    NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

    #if (FEATURE_PTM == FEATURE_ON)
    NAS_EMM_LocalDetachErrRecord(EMM_ERR_LOG_LOCAL_DETACH_TYPE_OTHER);
    #endif
    NAS_EMM_ClearAppMsgPara();

    NAS_EMM_ProcLocalNoUsim();

    if (NAS_EMM_CONN_IDLE != NAS_EMM_GetConnState())
    {
        /* �·�RelReq */
        NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);
    }

    return NAS_LMM_MSG_HANDLED;

}
VOS_VOID  NAS_EMM_ProcLocalNoUsim( VOS_VOID )
{
    NAS_LMM_FSM_STATE_STRU               stEmmState;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_ProcLocalNoUsim is enter.");


    /*�޸�״̬��������״̬DEREG��״̬DEREG_NORMAL_SERVICE*/
    stEmmState.enFsmId                    = NAS_LMM_PARALLEL_FSM_EMM;
    stEmmState.enMainState                = EMM_MS_DEREG;
    stEmmState.enSubState                 = EMM_SS_DEREG_NO_IMSI;
    stEmmState.enStaTId                   = TI_NAS_EMM_STATE_NO_TIMER;
    NAS_LMM_StaTransProc(stEmmState);

    /* ����DETACH�ͷ���Դ:��̬�ڴ桢����ֵ */
    NAS_LMM_DeregReleaseResource();

    /* lihong00150010 emergency begin */
    /* ����ο���ʶ */
    NAS_EMM_GLO_AD_GetUsimPullOutFlag() = VOS_FALSE;
    /* lihong00150010 emergency end */

    /* ��MMC�ظ�LMM_MMC_USIM_STATUS_CNF */
    NAS_EMM_SendMmcUsimStatusCnf();

    return;
}



VOS_UINT32  NAS_LMM_MsAnyStateSsWaitRrcRelIndMsgUsimStausInd(  )
{
    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_MsAnyStateSsWaitRrcRelIndMsgUsimStausInd is enter.");

    /*��ESM����ID_EMM_ESM_STATUS_IND��Ϣ*/
    NAS_EMM_PUB_SendEsmStatusInd(       EMM_ESM_ATTACH_STATUS_DETACHED);

    /* ֪ͨAPP_DETACH_IND(APP_MM_DETACH_ENTITY_ME)*/
    NAS_EMM_MmcSendDetIndLocal(MMC_LMM_L_LOCAL_DETACH_OTHERS);

    #if (FEATURE_PTM == FEATURE_ON)
    NAS_EMM_LocalDetachErrRecord(EMM_ERR_LOG_LOCAL_DETACH_TYPE_OTHER);
    #endif
    NAS_EMM_ClearAppMsgPara();

    NAS_EMM_ProcLocalNoUsim();

    /* �·�RelReq */
    NAS_EMM_RelReq(NAS_LMM_NOT_BARRED);

    return NAS_LMM_MSG_HANDLED;

}
VOS_VOID  NAS_LMM_SaveCurRatToPreRat(VOS_VOID )
{
   NAS_LMM_GetPreGsmPrio()      = NAS_LMM_GetCurGsmPrio();
   NAS_LMM_GetPreWcdmaPrio()    = NAS_LMM_GetCurWcdmaPrio();
   NAS_LMM_GetPreLtePrio()      = NAS_LMM_GetCurLtePrio();
   /*NAS_LMM_GetPreButtPrio()     = NAS_LMM_GetCurButtPrio();*/
}
/*****************************************************************************
 Function Name   : NAS_LMM_PreProcInfoChangeNotifyReq
 Description     : ����GMM������GMM_LMM_INFO_CHANGE_NOTIFY_REQ��Ϣ
 Input           : pstMsg------------------------��Ϣָ��
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2011-08-27  Draft Enact

*****************************************************************************/
VOS_UINT32 NAS_LMM_PreProcInfoChangeNotifyReq
(
    MsgBlock                           *pstMsg
)
{
    GMM_LMM_INFO_CHANGE_NOTIFY_REQ_STRU *pstGmmInfoChange = NAS_EMM_NULL_PTR;

    NAS_LMM_PUBM_LOG_NORM("NAS_LMM_PreProcInfoChangeNotifyReq is entered.");

    pstGmmInfoChange = (GMM_LMM_INFO_CHANGE_NOTIFY_REQ_STRU*)(VOS_VOID *)pstMsg;

    /* ��μ�� */
    if ( NAS_EMM_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_PreProcInfoChangeNotifyReq:input ptr null!");
        return  NAS_LMM_MSG_HANDLED;
    }

    /* �洢Gģ����״̬ */
    if (NAS_EMM_BIT_SLCT == pstGmmInfoChange->bitOpGprsMmState)
    {
        NAS_LMM_SetEmmInfoGConnState(pstGmmInfoChange->enGprsMmState);
    }

    /* �洢Uģ����״̬ */
    if (NAS_EMM_BIT_SLCT == pstGmmInfoChange->bitOpPacketMmState)
    {
        NAS_LMM_SetEmmInfoUConnState(pstGmmInfoChange->enPacketMmState);
    }

    /* �洢PS��״̬ */
    if (NAS_EMM_BIT_SLCT == pstGmmInfoChange->bitOpGprsSuspensionState)
    {
        NAS_LMM_SetEmmInfoPsState(pstGmmInfoChange->enGprsSuspensionState);
    }

    return NAS_LMM_MSG_HANDLED;
}
/*lint +e826*/
/*****************************************************************************
 Function Name   : NAS_LMM_SendRrcSysCfgReq
 Description     : ��RRC����RRC_MM_SYS_CFG_REQ��Ϣ
 Input           : None
 Output          : None
 Return          : NAS_LMM_SUCC  -- ���ͳɹ�
                     -- ����ʧ��

 History         :
    1.ZHANGCAIXIA 0079470      2011-9-10  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_LMM_SendRrcSysCfgReq( MsgBlock  *pMsg )
{
    LRRC_LNAS_SYS_CFG_REQ_STRU         *pstRrcSysCfgReqMsg = VOS_NULL_PTR;
    MMC_LMM_SYS_CFG_REQ_STRU           *pstMmcSysCfgReqMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulSendResult;
    VOS_UINT32                          ulMsgLen;

    pstMmcSysCfgReqMsg              = (VOS_VOID*)pMsg;

    /*���㳤�Ȳ�����VOS��Ϣͷ*/
    ulMsgLen     = sizeof(LRRC_LNAS_SYS_CFG_REQ_STRU);

    /*�����ڴ�*/
    pstRrcSysCfgReqMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(ulMsgLen);
    if(NAS_EMM_NULL_PTR == pstRrcSysCfgReqMsg)
    {
        NAS_EMM_PUBU_LOG_WARN("NAS_LMM_SendRrcSysCfgReq: MSG ALLOC ERR!");
        ulSendResult = NAS_EMM_SEND_MMC_ERR;
        return ulSendResult;
    }

    /*������뵽����Ϣ�ռ�*/
    PS_MEM_SET(pstRrcSysCfgReqMsg,0,sizeof(LRRC_LNAS_SYS_CFG_REQ_STRU));

    /*���VOS��Ϣͷ*/
    NAS_EMM_SET_RRC_MSG_HEADER(pstRrcSysCfgReqMsg, ulMsgLen-VOS_MSG_HEAD_LENGTH);

    /* �����ϢID */
    pstRrcSysCfgReqMsg->ulMsgId         = ID_LRRC_LMM_SYS_CFG_REQ;

    /* �����Ϣ�� */
    /* opId,��ʱ��Ϊ�̶�ֵ*/
    pstRrcSysCfgReqMsg->ulOpId          = 0x01;

    /* BAND */
    if (NAS_EMM_BIT_SLCT == pstMmcSysCfgReqMsg->bitOpBand)
    {
        pstRrcSysCfgReqMsg->bitOpBand = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(&(pstRrcSysCfgReqMsg->stBand),
                        &(pstMmcSysCfgReqMsg->stBand),
                        sizeof(NAS_RRC_USER_SET_FREQ_BAND_STRU));

    }
    /*RAT*/
    if (NAS_EMM_BIT_SLCT == pstMmcSysCfgReqMsg->bitOpRat)
    {
        pstRrcSysCfgReqMsg->bitOpRat = NAS_EMM_BIT_SLCT;
        NAS_LMM_MEM_CPY(pstRrcSysCfgReqMsg->aucRatPrioList,
                        pstMmcSysCfgReqMsg->aucRatPrioList,
                        sizeof(RRMM_RAT_PRIO_ENUM_UINT8)*RRMM_RAT_TYPE_BUTT);
    }
    /* ����ID_LRRC_LMM_SYS_CFG_REQ */
    NAS_LMM_SEND_MSG(pstRrcSysCfgReqMsg);

    ulSendResult                        = NAS_EMM_SEND_RRC_OK;

    return ulSendResult;
}



VOS_UINT32  NAS_LMM_PreProcMmcSysCfgReq( MsgBlock  *pMsg )
{

    VOS_UINT32                          ulSendResult;

    MMC_LMM_RESULT_ID_ENUM_UINT32       ulRslt = MMC_LMM_SUCC;

    MMC_LMM_SYS_CFG_REQ_STRU           *pMmcLmmSysCfgReq = VOS_NULL_PTR;

	NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMmcSysCfgReq Enter.");


    pMmcLmmSysCfgReq = (MMC_LMM_SYS_CFG_REQ_STRU *)(VOS_VOID *)pMsg;
    /* ���ڿ��ػ��������յ�SYSCFGEX���� */
    if(EMM_MS_NULL == NAS_EMM_CUR_MAIN_STAT)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_SendRrcSysCfgReq : POWER_ON OR POWER_OFF ING,DISCARD!");
        return NAS_LMM_MSG_HANDLED;
    }

    if (  (NAS_LMM_RAT_PRIO_BUTT <= pMmcLmmSysCfgReq->aucRatPrioList[NAS_LMM_RAT_TYPE_GSM])
        ||(NAS_LMM_RAT_PRIO_BUTT <= pMmcLmmSysCfgReq->aucRatPrioList[NAS_LMM_RAT_TYPE_WCDMA])
        ||(NAS_LMM_RAT_PRIO_BUTT <= pMmcLmmSysCfgReq->aucRatPrioList[NAS_LMM_RAT_TYPE_LTE]) )
    {
        NAS_LMM_EMM_PLMN_LOG_ERR("NAS_LMM_PreProcMmcSysCfgReq: MMC SYSCFG RAT LIST IS VALID ");

        /* ��MMC�ظ�SYS CFG���*/
        ulSendResult = NAS_LMM_SendMmcSysCfgCnf(ulRslt);

        if (NAS_EMM_SEND_RRC_OK != ulSendResult)
        {
            /* ��ӡ������ERROR_LEVEL */
            NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_SendMmcSysCfgCnf : Send Msg Failure!!! ulSendResult=",
                                     ulSendResult);
        }
        return NAS_LMM_MSG_HANDLED;
    }

    /*����ϵͳ���õĽ��뼼����ʹ֮��Ч*/
    NAS_LMM_SaveCurRatToPreRat();

    /* ���浽���� */
    NAS_LMM_GetCurGsmPrio()         = pMmcLmmSysCfgReq->aucRatPrioList[NAS_LMM_RAT_TYPE_GSM];
    NAS_LMM_GetCurWcdmaPrio()       = pMmcLmmSysCfgReq->aucRatPrioList[NAS_LMM_RAT_TYPE_WCDMA];
    NAS_LMM_GetCurLtePrio()         = pMmcLmmSysCfgReq->aucRatPrioList[NAS_LMM_RAT_TYPE_LTE];

    /* ��RRC��ϵͳ����������Ϣ*/
    ulSendResult = NAS_LMM_SendRrcSysCfgReq(pMsg);

    if (NAS_EMM_SEND_RRC_OK != ulSendResult)
    {
        /* ��ӡ������ERROR_LEVEL */
        NAS_LMM_PUBM_LOG1_INFO("NAS_LMM_SendRrcSysCfgReq : Send Msg Failure!!! ulSendResult=",
                                 ulSendResult);

        return NAS_LMM_MSG_HANDLED;

    }
    NAS_LMM_SetEmmInfoSyscfgCtrl(NAS_LMM_SYSCFG_FLAG_VALID);
    NAS_LMM_StartStateTimer(TI_NAS_EMM_STATE_WAIT_SYSCFG_CNF_TIMER);

    return NAS_LMM_MSG_HANDLED;
}

/*****************************************************************************
 Function Name   : NAS_LMM_SendMmcSysCfgCnf
 Description     : ��RRC����RRC_MM_SYS_CFG_CNF��Ϣ
 Input           : None
 Output          : None
 Return          : NAS_LMM_SUCC  -- ���ͳɹ�
                     -- ����ʧ��

 History         :
    1.ZHANGCAIXIA 0079470      2011-9-10  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_LMM_SendMmcSysCfgCnf( MMC_LMM_RESULT_ID_ENUM_UINT32 ulRst )
{
    LMM_MMC_SYS_CFG_CNF_STRU           *pstMmcSysCfgCnfMsg    = NAS_EMM_NULL_PTR;
    VOS_UINT32                          ulSendResult;

    NAS_LMM_PUBM_LOG_INFO( "NAS_LMM_SendMmcSysCfgCnf: enter");

    /* ����MMC�ڲ���Ϣ */
    pstMmcSysCfgCnfMsg = (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(sizeof(LMM_MMC_SYS_CFG_CNF_STRU));

    if(NAS_EMM_NULL_PTR == pstMmcSysCfgCnfMsg)
    {
        NAS_LMM_PUBM_LOG_ERR( "NAS_LMM_SendMmcSysCfgCnf: MSG ALLOC ERR !!");
        ulSendResult = NAS_EMM_SEND_MMC_ERR;
        return ulSendResult;
    }

    NAS_LMM_MEM_SET(pstMmcSysCfgCnfMsg,0,sizeof(LMM_MMC_SYS_CFG_CNF_STRU));

    /* ��дLMM_MMC_SYS_CFG_CNF_STRU ��DOPRA��Ϣͷ */
    EMM_PUBU_COMP_MMC_MSG_HEADER(pstMmcSysCfgCnfMsg,
            sizeof(LMM_MMC_SYS_CFG_CNF_STRU) - EMM_LEN_VOS_MSG_HEADER);

    /* ��дLMM_MMC_SYS_CFG_CNF ����ϢID��ʶ */
    pstMmcSysCfgCnfMsg->ulMsgId           = ID_LMM_MMC_SYS_CFG_CNF;

    /* ��д���*/
    pstMmcSysCfgCnfMsg->ulRst             = ulRst;

    /* ������Ϣ */
    NAS_LMM_SendLmmMmcMsg(                    pstMmcSysCfgCnfMsg);

    ulSendResult                          = NAS_EMM_SEND_RRC_OK;
    return ulSendResult;
}
VOS_UINT32  NAS_LMM_PreProcRrcSysCfgCnf( MsgBlock  *pMsg )
{

    LRRC_LNAS_SYS_CFG_CNF_STRU         *pstRrcLmmSysCfgCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt = NAS_LMM_MSG_DISCARD;
    NAS_UE_CAP_CHG_UPDATE_TYPE_ENUM_UINT32      enResult = NAS_UE_CAP_CHG_BUTT;

    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcRrcSysCfgCnf Enter.");

    pstRrcLmmSysCfgCnf                        = (VOS_VOID*)pMsg;

    (void)pMsg;
    if (NAS_EMM_NULL_PTR == pMsg)
    {
        NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcRrcSysCfgCnf: Input para is null");

        return NAS_LMM_MSG_HANDLED;
    }
    NAS_LMM_SetEmmInfoSyscfgCtrl(NAS_LMM_SYSCFG_FLAG_INVALID);
    NAS_LMM_StopStateTimer(TI_NAS_EMM_STATE_WAIT_SYSCFG_CNF_TIMER);

    enResult = NAS_EMM_RecordUeRadioAccCapChg(pstRrcLmmSysCfgCnf->ucRadioAccCapChgInd);
    /* ����RRC��SUSPEND��� */
    NAS_EMM_GetRrcSysCfgSusType()   = pstRrcLmmSysCfgCnf->enSusNotify;

    /* ���ݲ�ͬ������в�ͬ�Ĵ������������ */
    switch(NAS_EMM_GetRrcSysCfgSusType())
    {
        case    LRRC_LNAS_SUSPEND_LTE_BAND_NOT_SUPPORT:

                NAS_LMM_ProcRrcSysCfgCnfBandNotSupport();

                ulRslt = NAS_LMM_MSG_HANDLED;
                break;

        case    LRRC_LNAS_SUSPEND_LTE_RAT_NOT_ACTIVE:

                NAS_LMM_ProcRrcSysCfgCnfLteNotActive();

                ulRslt = NAS_LMM_MSG_HANDLED;
                break;

        case    LRRC_LNAS_NOT_SUSPEND:
                if(NAS_UE_CAP_CHG_NO_UPDATE == enResult)
                {
                    /* ֱ�Ӹ�MMC�ظ���� */
                    (VOS_VOID)NAS_LMM_SendMmcSysCfgCnf(MMC_LMM_SUCC);
                }
                else
                {
                    ulRslt = NAS_LMM_ProcRrcSysCfgCnfNotSuspend();
                }
                break;

        default:

                /* ������Ϣ*/
                NAS_LMM_PUBM_LOG1_INFO("ERROR  enSusNotify, value = ", pstRrcLmmSysCfgCnf->enSusNotify);
                break;
    }

    return ulRslt;
}


VOS_UINT32  NAS_LMM_PreProcMmcOocStateReq( MsgBlock  *pMsg )
{
    (void)pMsg;
    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_PreProcMmcOocStateReq Enter.");

    /*�о�LTE�Ƿ���ģ������������ģ�л�����������Ҫ�κδ��� */
    if(NAS_LMM_CUR_LTE_ACTIVE == NAS_EMM_GetCurLteState())
    {
        /*��RRC���Ͷ����������*/
        NAS_EMM_SendLrrcOssStateReq();

        /*�ڲ�״̬ת������״̬ */
        switch(NAS_LMM_GetEmmCurFsmMS())
        {
            case EMM_MS_REG:
            NAS_EMM_AdStateConvert( EMM_MS_REG,
                                    EMM_SS_REG_NO_CELL_AVAILABLE,
                                    TI_NAS_EMM_STATE_NO_TIMER);
            break;

            case EMM_MS_DEREG:
            if (EMM_SS_DEREG_NO_IMSI != NAS_LMM_GetEmmCurFsmSS())
            {
                NAS_EMM_AdStateConvert( EMM_MS_DEREG,
                                        EMM_SS_DEREG_NO_CELL_AVAILABLE,
                                        TI_NAS_EMM_STATE_NO_TIMER);
            }
            break;

            default:
            break;
        }
    }

    return NAS_LMM_MSG_HANDLED;
}
VOS_VOID  NAS_EMM_SendLrrcOssStateReq( VOS_VOID )
{
    LRRC_LMM_OOS_STATE_REQ_STRU           *pstLrrcOssStateReqMsg;

    /* ����DOPRA��Ϣ */
    pstLrrcOssStateReqMsg = (VOS_VOID *) NAS_LMM_ALLOC_MSG(sizeof(LRRC_LMM_OOS_STATE_REQ_STRU));
    if (NAS_LMM_NULL_PTR == pstLrrcOssStateReqMsg)
    {
        /* ��ӡ�쳣��ERROR_LEVEL */
        NAS_EMM_PUBU_LOG_NORM("NAS_EMM_SendLrrcOssStateReq: MSG ALLOC ERROR!!!");
        return ;
    }

    /* ��� */
    NAS_LMM_MEM_SET(pstLrrcOssStateReqMsg, 0, sizeof(LRRC_LMM_SUSPEND_REQ_STRU));

    /* ���VOS��Ϣͷ */
    NAS_EMM_SET_RRC_MSG_HEADER((pstLrrcOssStateReqMsg),
            NAS_EMM_GET_MSG_LENGTH_NO_HEADER(LRRC_LMM_OOS_STATE_REQ_STRU));

    /* �����ϢID */
    pstLrrcOssStateReqMsg->enMsgId       = ID_LRRC_LMM_OOS_STATE_REQ;

    /* ����DOPRA��Ϣ */
    NAS_LMM_SEND_MSG(pstLrrcOssStateReqMsg);

    return;
}

VOS_UINT32  NAS_LMM_PreProcRrcDataCnf( MsgBlock  *pMsg )
{
    LRRC_LMM_DATA_CNF_STRU              *pRrcDataCnf;
    VOS_UINT32                           ulIndex = 0;
    NAS_LMM_SEND_MSG_RESULT_FAIL_ACTION_FUN   pFailActFun;
    NAS_LMM_SEND_MSG_RESULT_SUCC_ACTION_FUN   pSuccActFun;
    VOS_UINT32                           ulRslt = VOS_FALSE;
    NAS_EMM_MRRC_MGMT_DATA_STRU         *pEmmMrrcMgmtData = NAS_EMM_NULL_PTR;
    VOS_UINT32                           ulIsDelBuff = VOS_FALSE;


    pRrcDataCnf = (LRRC_LMM_DATA_CNF_STRU *)(VOS_VOID*)pMsg;

    /*�����Ƿ���ֱ�Ӷ��� */
    if (LRRC_LMM_SEND_RSLT_BUTT == pRrcDataCnf->enSendRslt)
    {
        NAS_EMM_PUBU_LOG_NORM("NAS_LMM_PreProcRrcDataCnf: Invalid Parameter");
        return NAS_LMM_MSG_HANDLED;
    }


    /*�鲻������Ϣ��¼,��ֱ�Ӷ��� */
    pEmmMrrcMgmtData = NAS_EMM_FindMsgInDataReqBuffer(pRrcDataCnf->ulOpId);

    if (NAS_EMM_NULL_PTR == pEmmMrrcMgmtData)
    {
        NAS_EMM_PUBU_LOG_WARN("NAS_LMM_PreProcRrcDataCnf(WARN): OpId mismatch!");
        return  NAS_LMM_MSG_HANDLED;
    }

    /* ֹͣ������ʱ��*/
    NAS_LMM_StopStateTimer(TI_NAS_EMM_WAIT_RRC_DATA_CNF);

    ulRslt = VOS_FALSE;


    /*���ݻظ����,��ѯ�������� ,������Ӧ����*/
    for (ulIndex = 0; ulIndex < g_ulSendMsgResultActTblLen; ulIndex++)
    {
        /* ͨ��MSG ID���Ҵ������� */
        if(pEmmMrrcMgmtData->enLastMsgId == gstEmmMrrcSendMsgResultActTbl[ulIndex].enMsgId)
        {
            ulRslt = VOS_TRUE;

            /* ���ͳɹ��Ĵ��� */
            if (LRRC_LMM_SEND_RSLT_SUCCESS == pRrcDataCnf->enSendRslt)
            {
                pSuccActFun = gstEmmMrrcSendMsgResultActTbl[ulIndex].pfSuccActionFun;

                /* �д������� */
                if (VOS_NULL_PTR != pSuccActFun)
                {
                    (VOS_VOID)(pSuccActFun)(pMsg);
                }

                NAS_EMM_ClrDataReqBufferMsg(pRrcDataCnf->ulOpId);

                return NAS_LMM_MSG_HANDLED;

            }
            else
            {
                #if (FEATURE_PTM == FEATURE_ON)
                NAS_EMM_ProcErrlogEstCnfOrDataCnfFail((VOS_VOID*)pRrcDataCnf, EMM_OM_ERRLOG_TYPE_DATA_CNF_FAIL);
                #endif

                /* ����ʧ�ܵĴ��� */
                pFailActFun = gstEmmMrrcSendMsgResultActTbl[ulIndex].pfFailActionFun;

                /* �д������� */
                if (VOS_NULL_PTR != pFailActFun)
                {
                    (VOS_VOID)(pFailActFun)(pMsg,&ulIsDelBuff);

                    /* ��Ҫ������� */
                    if (VOS_TRUE == ulIsDelBuff)
                    {
                        NAS_EMM_ClrDataReqBufferMsg(pRrcDataCnf->ulOpId);
                    }
                    return NAS_LMM_MSG_HANDLED;
                }
                else
                {
                    /* û�д������������������Ϣ */
                    NAS_EMM_ClrDataReqBufferMsg(pRrcDataCnf->ulOpId);
                    return NAS_LMM_MSG_HANDLED;
                }
            }
        }
    }
    /* û���ҵ��������� */
    if (VOS_FALSE == ulRslt)
    {
        NAS_EMM_ClrDataReqBufferMsg(pRrcDataCnf->ulOpId);

    }
    return NAS_LMM_MSG_HANDLED;
}

VOS_UINT32  NAS_LMM_PreProcIntraCoverageLostInd( MsgBlock  *pMsg )
{
    (VOS_VOID)pMsg;

    /* ���RRC����״̬�����ͷŹ����У�����ΪIDLE */
    if(NAS_EMM_CONN_RELEASING == NAS_EMM_GetConnState())
    {
        NAS_EMM_SetConnState(NAS_EMM_CONN_IDLE);
    }

    /* ��״̬������ */
    return NAS_LMM_MSG_DISCARD;
}

/*****************************************************************************
 Function Name   : NAS_LMM_SendMmcSuspendInfoChange
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HANLUFENG 41410      2013-6-19  Draft Enact

*****************************************************************************/
VOS_VOID  NAS_LMM_SendMmcSuspendInfoChangeRatChange(
                LMM_MMC_RAT_CHANGE_TYPE_ENUM_UINT32 ulRatChangeType)
{
    LMM_MMC_SUSPEND_INFO_CHANGE_NOTIFY_STRU           *pstSuspendInfoChangeMsg    = NAS_EMM_NULL_PTR;

    NAS_LMM_PUBM_LOG_INFO( "NAS_LMM_SendMmcSuspendInfoChange: enter");

    /* ����MMC�ڲ���Ϣ */
    pstSuspendInfoChangeMsg =
        (VOS_VOID *)NAS_LMM_GetLmmMmcMsgBuf(
                        sizeof(LMM_MMC_SUSPEND_INFO_CHANGE_NOTIFY_STRU));

    if(NAS_EMM_NULL_PTR == pstSuspendInfoChangeMsg)
    {
        NAS_LMM_PUBM_LOG1_ERR( "NAS_LMM_SendMmcSuspendInfoChange: MSG ALLOC ERR !!",
                                pstSuspendInfoChangeMsg);
        return ;
    }


    NAS_LMM_MEM_SET(pstSuspendInfoChangeMsg,
                    0,
                    sizeof(LMM_MMC_SUSPEND_INFO_CHANGE_NOTIFY_STRU));


    /* ��дLMM_MMC_SYS_CFG_CNF_STRU ��DOPRA��Ϣͷ */
    EMM_PUBU_COMP_MMC_MSG_HEADER(pstSuspendInfoChangeMsg,
            sizeof(LMM_MMC_SUSPEND_INFO_CHANGE_NOTIFY_STRU) - EMM_LEN_VOS_MSG_HEADER);

    /* ��дLMM_MMC_SYS_CFG_CNF ����ϢID��ʶ */
    pstSuspendInfoChangeMsg->ulMsgId           = ID_LMM_MMC_SUSPEND_INFO_CHANGE_NOTIFY;

    /* ��д���*/
    pstSuspendInfoChangeMsg->ulInfoType     = LMM_MMC_SUSPEND_RAT_INFO_CHANGE;
    pstSuspendInfoChangeMsg->ulRatChangeType= ulRatChangeType;

    NAS_LMM_PUBM_LOG1_INFO( "NAS_LMM_SendMmcSuspendInfoChange: ulRatChangeType =",
                            pstSuspendInfoChangeMsg->ulRatChangeType);

    NAS_LMM_PUBM_LOG1_INFO( "NAS_LMM_SendMmcSuspendInfoChange: pulInfoType =",
                            pstSuspendInfoChangeMsg->ulInfoType);
    /* ������Ϣ */
    NAS_LMM_SendLmmMmcMsg(                    pstSuspendInfoChangeMsg);

    return ;
}


/*****************************************************************************
 Function Name   : NAS_EMM_PreProcMsgRrcSuspendInfoChangeInd
 Description     :
 Input           : None
 Output          : None
 Return          : VOS_UINT32

 History         :
    1.HANLUFENG 41410      2013-6-19  Draft Enact

*****************************************************************************/
VOS_UINT32  NAS_EMM_PreProcMsgRrcSuspendInfoChangeInd( MsgBlock  *pstMsg )
{

    LRRC_LMM_SUSPEND_INFO_CHANGE_IND_STRU *pstInfoChange = NAS_EMM_NULL_PTR;

    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_PreProcMsgRrcSuspendInfoChangeInd is entered.");

    pstInfoChange = (LRRC_LMM_SUSPEND_INFO_CHANGE_IND_STRU*)(VOS_VOID *)pstMsg;

    /* ������� */
    if(LRRC_LNAS_SUSPEND_RAT_INFO_CHANGE != pstInfoChange->ulInfoType)
    {
        NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_PreProcMsgRrcSuspendInfoChangeInd,ERROR : ulInfoType = ",
                                pstInfoChange->ulInfoType);
        return  NAS_LMM_MSG_HANDLED;
    }

    NAS_LMM_PUBM_LOG1_NORM("NAS_EMM_PreProcMsgRrcSuspendInfoChangeInd,ERROR : ulRatChangeType = ",
                                pstInfoChange->ulRatChangeType);

    if(LRRC_LNAS_RAT_CHANGE_G2W == pstInfoChange->ulRatChangeType)
    {
        NAS_EMM_SetSuspendDir(LRRC_LMM_SUS_SYS_CHNG_DIR_L2W);
        NAS_LMM_SendMmcSuspendInfoChangeRatChange(LMM_MMC_RAT_CHANGE_G2W);
    }
    else if(LRRC_LNAS_RAT_CHANGE_W2G == pstInfoChange->ulRatChangeType)
    {
        NAS_EMM_SetSuspendDir(LRRC_LMM_SUS_SYS_CHNG_DIR_L2G);
        NAS_LMM_SendMmcSuspendInfoChangeRatChange(LMM_MMC_RAT_CHANGE_W2G);
    }
    else
    {
        NAS_LMM_PUBM_LOG_NORM("NAS_EMM_PreProcMsgRrcSuspendInfoChangeInd,ERROR : ulRatChangeType");
    }

    return  NAS_LMM_MSG_HANDLED;
}
VOS_UINT32 NAS_LMM_RevMmcModemInfoReq(MsgBlock *  pMsg)
{
    MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU *pstMmcRevInfo;
    LMM_LRRC_OTHER_MODEM_INFO_NOTIFY_STRU *pModemInfoMsg;


    NAS_LMM_PUBM_LOG_INFO("NAS_LMM_RevMmcInfoReq!");

    pstMmcRevInfo = (MMC_LMM_OTHER_MODEM_INFO_NOTIFY_STRU *)(VOS_VOID*)pMsg;
    /*������Ϣ�ڴ�*/
    pModemInfoMsg = (VOS_VOID *)NAS_LMM_ALLOC_MSG(sizeof(LMM_LRRC_OTHER_MODEM_INFO_NOTIFY_STRU));

    /*�ж�����������ʧ�ܴ�ӡ�����˳�*/
    if (VOS_NULL_PTR == pModemInfoMsg)
    {
        /*��ӡ����*/
        NAS_LMM_PUBM_LOG_ERR("NAS_LMM_RevMmcModemInfoReq: MSG ALLOC ERR!");
        return NAS_LMM_MSG_DISCARD;
    }
    NAS_LMM_MEM_SET(pModemInfoMsg, 0, sizeof(LMM_LRRC_OTHER_MODEM_INFO_NOTIFY_STRU));

    /*����ID_LMM_LRRC_OTHER_MODEM_INFO_NOTIFY��Ϣ*/
    /*�����Ϣͷ*/
    NAS_EMM_SET_RRC_MSG_HEADER( pModemInfoMsg,
                                (sizeof(LMM_LRRC_OTHER_MODEM_INFO_NOTIFY_STRU)-
                                 NAS_EMM_LEN_VOS_MSG_HEADER));
    /*�����ϢID*/
    pModemInfoMsg->ulMsgId = ID_LRRC_LMM_OTHER_MODEM_INFO_NOTIFY;
    pModemInfoMsg->bitOpCurrCampPlmnId = pstMmcRevInfo->bitOpCurrCampPlmnId;
    pModemInfoMsg->stCurrCampPlmnId.aucPlmnId[0] =
                    pstMmcRevInfo->stCurrCampPlmnId.aucPlmnId[0];
    pModemInfoMsg->stCurrCampPlmnId.aucPlmnId[1] =
                    pstMmcRevInfo->stCurrCampPlmnId.aucPlmnId[1];
    pModemInfoMsg->stCurrCampPlmnId.aucPlmnId[2] =
                    pstMmcRevInfo->stCurrCampPlmnId.aucPlmnId[2];

    /*��RRCģ�鷢��״̬�仯��Ϣ*/
    NAS_LMM_SEND_MSG(pModemInfoMsg);

    return NAS_LMM_MSG_HANDLED;
}

/*lint +e960*/

VOS_UINT32 NAS_EMM_PreProcMsgRrcPagingInd(MsgBlock  *pstMsg)
{
    NAS_LMM_MAIN_STATE_ENUM_UINT16       enMainState;
    LRRC_LMM_PAGING_IND_STRU             *pMsgRrcPagingInd = NAS_EMM_NULL_PTR;
    NAS_LMM_PUBM_LOG_NORM("NAS_EMM_PreProcMsgRrcPagingInd is entered.");

    /* ��μ�� */
    if ( NAS_EMM_NULL_PTR == pstMsg)
    {
        NAS_LMM_PUBM_LOG_ERR("NAS_EMM_PreProcMsgRrcPagingInd:input ptr null!");
        return  NAS_LMM_MSG_HANDLED;
    }
    enMainState = NAS_LMM_GetEmmCurFsmMS();
    pMsgRrcPagingInd = (LRRC_LMM_PAGING_IND_STRU *)(VOS_VOID *)pstMsg;

    /* TAU��SER�������յ�CS PAGINGʱ�����ȼ����棬��TAU��SER���̽�������,����״̬��ԭ��������״̬������ */
    if(LRRC_LNAS_PAGING_TYPE1 == pMsgRrcPagingInd->enPagingType)
    {
        if((EMM_MS_TAU_INIT == enMainState) || (EMM_MS_SER_INIT == enMainState) || (EMM_MS_RRC_CONN_EST_INIT == enMainState))
        {
            NAS_LMM_PUBM_LOG_NORM("NAS_EMM_PreProcMsgRrcPagingInd LOW_PRIO_MSG.");
            return NAS_LMM_STORE_LOW_PRIO_MSG;
        }
    }
    return NAS_LMM_MSG_DISCARD;
}

#if (FEATURE_PTM == FEATURE_ON)
NAS_LMM_ERRLOG_ACT_STRU  gstEmmErrlogActTbl[]=
{
    {EMM_MS_REG_INIT,               NAS_EMM_AttachErrRecord},
    {EMM_MS_TAU_INIT,               NAS_EMM_TAUErrRecord},
    {EMM_MS_SER_INIT,               NAS_EMM_NorServiceErrRecord}
};
VOS_UINT32        g_ulEmmErrlogActTblLen = sizeof(gstEmmErrlogActTbl)
                                                / sizeof(NAS_LMM_ERRLOG_ACT_STRU);

VOS_VOID NAS_EMM_ProcErrlogEstCnfOrDataCnfFail(
                 VOS_VOID*                         pstEmmProcessFail,
                 EMM_OM_ERRLOG_TYPE_ENUM_UINT16    enErrType)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT16                          usCurState;
    NAS_LMM_ERRLOG_ACT_FUN              pActFun;

    NAS_LMM_PUBM_LOG_INFO("NAS_EMM_ProcErrlogEstCnfOrDataCnfFail!!");

    usCurState = NAS_EMM_CUR_MAIN_STAT;
    if((EMM_MS_REG_INIT != usCurState)
      && (EMM_MS_TAU_INIT != usCurState)
      && (EMM_MS_SER_INIT != usCurState))
    {
        return;
    }

    /*���ݵ�ǰ��״̬ȥ�Ҷ�Ӧ��errlog��������*/
    for(ulLoop = 0; ulLoop < g_ulEmmErrlogActTblLen; ulLoop++)
    {
        if(gstEmmErrlogActTbl[ulLoop].enCurState == usCurState)
        {
            pActFun = gstEmmErrlogActTbl[ulLoop].pfErrlogFun;
            if(VOS_NULL_PTR != pActFun)
            {
                (VOS_VOID)pActFun(pstEmmProcessFail, enErrType);
            }
            break;
        }
    }

    return;
}
#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of ��ϵͳ+ģ��+�ļ���.c */