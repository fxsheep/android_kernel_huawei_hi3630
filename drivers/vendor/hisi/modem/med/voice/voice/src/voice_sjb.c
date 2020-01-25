
#include "voice_sjb.h"
#include "om_log.h"
#include "voice_debug.h"

/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_SJB_C


/*****************************************************************************
   2 ȫ�ֱ�������
******************************************************************************/
/* JitterBuffer�����п��ƿ鼰�洢���� */
VOICE_SJB_RX_MANAGER_STRU                g_stRxSjbManager;
VOICE_SJB_TX_MANAGER_STRU                g_stTxSjbManager;
VOS_VOID VOICE_SJB_InitTx(VOICE_SJB_TX_MANAGER_STRU *pstSjbManager)
{
    VOICE_JB_PKT_TIME_ENUM_UINT32 enPktTime;

    enPktTime = pstSjbManager->enPktTime;

    /* JB���ƿ��ʼ�� */
    UCOM_MemSet(pstSjbManager, 0, sizeof(VOICE_SJB_TX_MANAGER_STRU));

    /* ���Init֮ǰ��PktTime����Чֵ����������������Ϊ20ms */
    if((enPktTime < VOICE_JB_PKT_TIME_BUTT)
      && ((enPktTime % VOICE_JB_PKT_TIME_20MS) == 0) && (enPktTime > 0))
    {
        pstSjbManager->enPktTime = enPktTime;
    }
    else
    {
        pstSjbManager->enPktTime = VOICE_JB_PKT_TIME_20MS;
    }

    /* ��ʼ����� */
    OM_LogInfo(VOICE_SJB_InitTxSucc);

    return;
}
VOS_UINT32 VOICE_SJB_SetPktTimeTx(
                VOICE_SJB_TX_MANAGER_STRU *pstSjbManager,
                VOICE_JB_PKT_TIME_ENUM_UINT32 enPktTime)
{
    /* ��������õ�PktTime����Ч�ģ������á����򱣳ֲ��䲢����ʧ�ܡ� */
    if((enPktTime < VOICE_JB_PKT_TIME_BUTT) && ((enPktTime % VOICE_JB_PKT_TIME_20MS) == 0))
    {
        pstSjbManager->enPktTime = enPktTime;
        OM_LogInfo1(VOICE_SJB_SetPktTimeTxSucc, enPktTime);
        return UCOM_RET_SUCC;
    }
    else
    {
        OM_LogWarning1(VOICE_SJB_SetPktTimeTxFail, enPktTime);
        return UCOM_RET_FAIL;
    }
}
VOS_UINT32 VOICE_SJB_StorePacketTx(VOICE_SJB_TX_MANAGER_STRU *pstSjbManager, VOICE_IMSA_TX_DATA_IND_STRU *pstPkt)
{
    VOICE_SJB_PKT_TX_STRU *pstPktTemp;
    VOS_UINT32 uwRet;

    /* �������������ɾ��β�� */
    if(pstSjbManager->stPara.uwCurPktNum >= VOICE_SJB_TX_DEPTH)
    {
        VOICE_SJB_RemoveTailTx( pstSjbManager );
        pstSjbManager->stPara.uwLostByStore++;
    }

    /* �ҵ���λ��д���֡����Ϊ���в�������������ÿ�μ����֡���Ƕ��� */
    pstPktTemp = VOICE_SJB_FindEmptyPtrTx( pstSjbManager );

    if(VOS_NULL == pstPktTemp)
    {
        pstSjbManager->stPara.uwLostByStore++;
        OM_LogError(VOICE_SJB_StorePacketTxFail);
        return UCOM_RET_FAIL;
    }
    UCOM_MemCpy(pstPktTemp, pstPkt, sizeof(VOICE_IMSA_TX_DATA_IND_STRU));

     /* �����ǰ֡������֡�� */
    uwRet = VOICE_SJB_InsertPktTx(pstSjbManager, pstPktTemp, VOS_NULL, pstSjbManager->stBuff.pstHead);

    return uwRet;
}
VOS_UINT32 VOICE_SJB_RemoveTailTx(VOICE_SJB_TX_MANAGER_STRU  *pstSjbManager)
{
    /* β�Ͳ�Ϊ�� */
    if(VOS_NULL != pstSjbManager->stBuff.pstTail)
    {
        pstSjbManager->stBuff.pstTail->enIsNotEmpty = VOICE_SJB_ITEM_STATUS_EMPTY;
        pstSjbManager->stBuff.pstTail = pstSjbManager->stBuff.pstTail->pstPrev;

        /* β�͵���һ֡��Ϊ�գ�����Nextָ����Ϊ�� */
        if(VOS_NULL != pstSjbManager->stBuff.pstTail)
        {
            pstSjbManager->stBuff.pstTail->pstNext= VOS_NULL;
        }

        /* β�͵���һ֡�ǿյģ����ʱ�����ͷҲҪ��Ϊ��*/
        else
        {
            pstSjbManager->stBuff.pstHead = VOS_NULL;
        }
        pstSjbManager->stPara.uwCurPktNum--;
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_SJB_InsertPktTx(
                VOICE_SJB_TX_MANAGER_STRU               *pstSjbManager,
                VOICE_SJB_PKT_TX_STRU                   *pstPktToStore,
                VOICE_SJB_PKT_TX_STRU                   *pstPktPrev,
                VOICE_SJB_PKT_TX_STRU                   *pstPktNext)
{
    pstPktToStore->pstNext      = pstPktNext;
    pstPktToStore->pstPrev      = pstPktPrev;
    pstPktToStore->enIsNotEmpty = VOICE_SJB_ITEM_STATUS_NOTEMPTY;

    /* �嵽���� */
    if(VOS_NULL == pstPktPrev)
    {
        pstSjbManager->stBuff.pstHead = pstPktToStore;
    }
    else
    {
        pstPktPrev->pstNext = pstPktToStore;
    }

    /* �嵽��β */
    if(VOS_NULL == pstPktNext)
    {
        pstSjbManager->stBuff.pstTail = pstPktToStore;
    }
    else
    {
        pstPktNext->pstPrev = pstPktToStore;
    }
    pstSjbManager->stPara.uwRcvPktNum++;
    pstSjbManager->stPara.uwCurPktNum++;

    /* �ϱ���ǰ��Ⱥ���֡�� */
    OM_LogInfo2(VOICE_SJB_InsertPktTxSucc,
                pstSjbManager->stPara.uwCurPktNum,
                pstSjbManager->stPara.uwRcvPktNum);

    return UCOM_RET_SUCC;
}

VOICE_SJB_PKT_TX_STRU* VOICE_SJB_FindEmptyPtrTx(VOICE_SJB_TX_MANAGER_STRU *pstSjbManager)
{
    VOS_UINT32 uhwCycle;

    VOICE_SJB_PKT_TX_STRU* pstPktTemp = &pstSjbManager->stBuff.stPkts[0];

    for(uhwCycle = 0; uhwCycle < VOICE_SJB_TX_DEPTH; uhwCycle++)
    {
        if(VOICE_SJB_ITEM_STATUS_EMPTY == pstPktTemp->enIsNotEmpty)
        {
            return pstPktTemp;
        }
        else
        {
            pstPktTemp++;
        }
    }

    /* �������������λ��ȫ�����գ��򷵻ؿ�ָ�� */
    return VOS_NULL;
}
VOS_UINT32 VOICE_SJB_GetPacketTx(VOICE_SJB_TX_MANAGER_STRU *pstSjbManager, VOICE_IMSA_TX_DATA_IND_STRU *pstPkt)
{
    VOS_UINT32 uwRet;

    /* �������ǿյģ�ȡ����������Ҫ����TS */
    if (0 == pstSjbManager->stPara.uwCurPktNum)
    {
        OM_LogWarning(VOICE_SJB_GetPacketTxFail);
        return UCOM_RET_FAIL;
    }
    /* ���鲻�� */
    else
    {
        /* ȡ���� */
        UCOM_MemCpy(pstPkt, pstSjbManager->stBuff.pstTail, sizeof(VOICE_IMSA_TX_DATA_IND_STRU));

        /* ɾ��β�� */
        uwRet = VOICE_SJB_RemoveTailTx(pstSjbManager);
        return uwRet;
    }
}


VOS_VOID VOICE_SJB_InitRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager)
{
    /* JB���ƿ��ʼ�� */
    UCOM_MemSet(pstSjbManager, 0, sizeof(VOICE_SJB_RX_MANAGER_STRU));

    /* 20ms��Ӧ��TS����Ϊ320 */
    pstSjbManager->stState.uwTSof20ms = VOICE_SJB_20MSTS_WB;

    /* ��ʼ����� */
    OM_LogInfo(VOICE_SJB_InitRxSucc);

    return;
}
VOS_UINT32 VOICE_SJB_StorePacketRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager, IMSA_VOICE_RX_DATA_IND_STRU *pstPkt)
{
    VOS_UINT32 uwCurrentTS = pstPkt->ulTS;
    VOS_UINT32 uwCycle;
    VOS_UINT32 uwRet;
    VOS_UINT32 uwCpuSr;
    VOICE_SJB_PKT_RX_STRU *pstPktTemp;
    VOICE_SJB_PKT_RX_STRU *pstPktItr;

    /* ���жϻ��� */
    uwCpuSr = VOS_SplIMP();

    /* ʱ��С��LastTS��˵���ð�̫�ɣ��������˴���Ҫ����ʱ����ת */
    if(VOICE_SJB_TSCompare(uwCurrentTS, pstSjbManager->stPara.uwLastTS))
    {
        pstSjbManager->stPara.uwLostByStore++;
        pstSjbManager->stPara.uwTSErrCnt++;
        OM_LogWarning2(VOICE_SJB_StorePacketRx_TsTooOld, uwCurrentTS, pstSjbManager->stPara.uwLastTS);

        /*  ��ʱ������������������࣬����Ϊ���쳣����ʼ��JB  */
        if(pstSjbManager->stPara.uwTSErrCnt > VOICE_SJB_TSErrCntMax)
        {
            VOICE_SJB_InitRx(pstSjbManager);
            OM_LogWarning(VOICE_SJB_StorePacketRx_InitJB);
        }

        /* �ͷŻ��� */
        VOS_Splx(uwCpuSr);
        return UCOM_RET_FAIL;
    }
    else
    {
        /* ��������������������������0 */
        pstSjbManager->stPara.uwTSErrCnt = 0;
    }

    /* �����������������һ�� */
    if(pstSjbManager->stPara.uwCurPktNum >= VOICE_SJB_RX_DEPTH)
    {
        /* ��β���ϣ�������β */
        if(VOICE_SJB_TSCompare(pstSjbManager->stBuff.pstTail->stPktData.ulTS, uwCurrentTS))
        {
            VOICE_SJB_RemoveTailRx(pstSjbManager);
            pstSjbManager->stPara.uwLostByStore++;
        }
        else
        {
            /* �ð����ϣ��������� */
            pstSjbManager->stPara.uwLostByStore++;
            OM_LogInfo1(VOICE_SJB_StorePacketRx_TsTooOld, pstSjbManager->stPara.uwLostByStore);

            /* �ͷŻ��� */
            VOS_Splx(uwCpuSr);
            return UCOM_RET_FAIL;
        }
    }

    /* ���������ҵ�һ����λ��Ȼ�󽫸�֡д�� */
    pstPktTemp = VOICE_SJB_FindEmptyPtrRx(pstSjbManager);
    if(VOS_NULL == pstPktTemp)
    {
        pstSjbManager->stPara.uwLostByStore++;
        OM_LogInfo1(VOICE_SJB_StorePacketRx_NoSpace, pstSjbManager->stPara.uwLostByStore);

        /* �������ά�������������߽��÷�֧�����˸÷�֧��˵��JB�����Ѿ����ˣ����³�ʼ�� */
        VOICE_SJB_InitRx(pstSjbManager);

        /* �ͷŻ��� */
        VOS_Splx(uwCpuSr);
        return UCOM_RET_FAIL;
    }
    UCOM_MemCpy(pstPktTemp, pstPkt, sizeof( IMSA_VOICE_RX_DATA_IND_STRU));

    /* ���¸�֡ǰ���prev��nextָ�룬������֡�� */
    pstPktItr = pstSjbManager->stBuff.pstHead;
    for(uwCycle = 0; uwCycle < pstSjbManager->stPara.uwCurPktNum; uwCycle++)
    {
        /* ���ʱ���ظ���˵�����ظ��İ���Ҫ���� */
        if(pstPktItr->stPktData.ulTS == uwCurrentTS)
        {
            pstSjbManager->stPara.uwLostByStore++;
            OM_LogInfo1(VOICE_SJB_StorePacketRx_SamePkt, pstSjbManager->stPara.uwLostByStore);

            /* �ͷŻ��� */
            VOS_Splx(uwCpuSr);
            return UCOM_RET_FAIL;
        }

        /* �ҵ���һ��TS�ȵ�ǰ֡С�ģ����ǵ�ǰ֡��Next */
        if(VOICE_SJB_TSCompare(pstPktItr->stPktData.ulTS, uwCurrentTS))
        {
             uwRet = VOICE_SJB_InsertPktRx(pstSjbManager, pstPktTemp,  pstPktItr->pstPrev, pstPktItr);

             /* �ͷŻ��� */
             VOS_Splx(uwCpuSr);
             return uwRet;
        }
        pstPktItr = pstPktItr->pstNext;
    }

    /* ���������а��������ڵ�Ҫ����Ҫ�嵽��β���档�����ǰ��֡��Ϊ0����pstTailӦΪNULL */
    uwRet = VOICE_SJB_InsertPktRx(pstSjbManager, pstPktTemp, pstSjbManager->stBuff.pstTail, VOS_NULL);

    /* �ͷŻ��� */
    VOS_Splx(uwCpuSr);
    return uwRet;

}
VOS_UINT32 VOICE_SJB_RemoveTailRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager)
{
    /* �������滹��β�� */
    if(VOS_NULL != pstSjbManager->stBuff.pstTail)
    {
        pstSjbManager->stBuff.pstTail->enIsNotEmpty = VOICE_SJB_ITEM_STATUS_EMPTY;
        pstSjbManager->stBuff.pstTail = pstSjbManager->stBuff.pstTail->pstPrev;

        /* β�͵���һ֡��Ϊ��,������һ֡��Nextָ���ÿ� */
        if(VOS_NULL != pstSjbManager->stBuff.pstTail)
        {
            pstSjbManager->stBuff.pstTail->pstNext= VOS_NULL;
        }

        /* β�͵���һ֡�ǿյģ��������ͷҲ�ÿ� */
        else
        {
            pstSjbManager->stBuff.pstHead = VOS_NULL;
        }
        pstSjbManager->stPara.uwCurPktNum--;
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_SJB_InsertPktRx(
                VOICE_SJB_RX_MANAGER_STRU                   *pstSjbManager,
                VOICE_SJB_PKT_RX_STRU                       *pstPktToStore,
                VOICE_SJB_PKT_RX_STRU                       *pstPktPrev,
                VOICE_SJB_PKT_RX_STRU                       *pstPktNext)
{
    pstPktToStore->pstNext      = pstPktNext;
    pstPktToStore->pstPrev      = pstPktPrev;
    pstPktToStore->enIsNotEmpty = VOICE_SJB_ITEM_STATUS_NOTEMPTY;

    /* �嵽���� */
    if(VOS_NULL == pstPktPrev)
    {
        pstSjbManager->stBuff.pstHead = pstPktToStore;
    }
    else
    {
        pstPktPrev->pstNext = pstPktToStore;
    }

    /* �嵽��β */
    if(VOS_NULL == pstPktNext)
    {
        pstSjbManager->stBuff.pstTail = pstPktToStore;
    }
    else
    {
        pstPktNext->pstPrev = pstPktToStore;
    }
    pstSjbManager->stPara.uwRcvPktNum++;
    pstSjbManager->stPara.uwCurPktNum++;

    /* �������ݰ���CodecType����uwTSof20ms��ֵ��WB��G711��320��������160 */
    if((pstPktToStore->stPktData.usCodecType == CODEC_AMRWB)
        || (pstPktToStore->stPktData.usCodecType == CODEC_G711))
    {
        pstSjbManager->stState.uwTSof20ms = VOICE_SJB_20MSTS_WB;
    }
    else
    {
        pstSjbManager->stState.uwTSof20ms = VOICE_SJB_20MSTS_NB;
    }

    /* �ϱ���ǰ��Ⱥ���֡�� */
    OM_LogInfo2(VOICE_SJB_InsertPktRxSucc,
                pstSjbManager->stPara.uwCurPktNum,
                pstSjbManager->stPara.uwRcvPktNum);

    return UCOM_RET_SUCC;
}



VOICE_SJB_PKT_RX_STRU* VOICE_SJB_FindEmptyPtrRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager)
{
    VOS_UINT32 uwCycle;
    VOICE_SJB_PKT_RX_STRU *pstPktTemp = &pstSjbManager->stBuff.stPkts[0];

    for(uwCycle = 0; uwCycle < VOICE_SJB_RX_DEPTH; uwCycle++)
    {
        if(VOICE_SJB_ITEM_STATUS_EMPTY == pstPktTemp->enIsNotEmpty)
        {
            return pstPktTemp;
        }
        else
        {
            pstPktTemp++;
        }
    }

    return VOS_NULL;
}



VOS_UINT32 VOICE_SJB_GetPacketRx(VOICE_SJB_RX_MANAGER_STRU *pstSjbManager, IMSA_VOICE_RX_DATA_IND_STRU *pstPkt)
{
    VOS_UINT32                   uwRet;
    VOS_UINT32                   uwTSDiff;
    VOS_UINT32                   uwCpuSr;
    VOICE_JB_HOOK_STRU           stJBHook;

    /* ���жϻ��� */
    uwCpuSr = VOS_SplIMP();

    /* ��ʼ����־Ϊfalse�����ж���û����� */
    if(VOICE_SJB_INIT_STATUS_NOT == pstSjbManager->stState.enAlreadyInit)
    {
        if(pstSjbManager->stPara.uwCurPktNum < VOICE_SJB_INIT_DEPTH)
        {
            OM_LogInfo(VOICE_SJB_GetPacketRx_UnInit);

            /* �ͷŻ��� */
            VOS_Splx(uwCpuSr);
            return UCOM_RET_FAIL;
        }
        else
        {
            /* ����ʼ����ȫ��־��Ϊtrue����LastTS��Ϊ��β֡TS��20ms���Ա�֤��һ֡�ɹ�ȡ�� */
            pstSjbManager->stState.enAlreadyInit = VOICE_SJB_INIT_STATUS_DONE;
            pstSjbManager->stPara.uwLastTS       = pstSjbManager->stBuff.pstTail->stPktData.ulTS - pstSjbManager->stState.uwTSof20ms;
        }
    }

    /* �������ǿյģ�ȡ������,Ҫ����LastTS */
    if(0 == pstSjbManager->stPara.uwCurPktNum)
    {
        pstSjbManager->stPara.uwLostByGet++;
        pstSjbManager->stPara.uwLastTS += pstSjbManager->stState.uwTSof20ms;
        OM_LogWarning1(VOICE_SJB_GetPacketRx_NoPkt, pstSjbManager->stPara.uwLostByGet);
        uwRet = UCOM_RET_FAIL;
    }
    else
    {
        uwTSDiff = VOICE_SJB_TSCompare( pstSjbManager->stPara.uwLastTS, pstSjbManager->stBuff.pstTail->stPktData.ulTS );

        /* ���β֡ʱ����LastTS������40���룬����Ҫȡ��֡������ʧ��,����LastTs */
        if(uwTSDiff > (2 * (pstSjbManager->stState.uwTSof20ms)))
        {
            pstSjbManager->stPara.uwLostByGet++;
            pstSjbManager->stPara.uwGetErrCnt++;
            pstSjbManager->stPara.uwLastTS += pstSjbManager->stState.uwTSof20ms;

            OM_LogInfo3(VOICE_SJB_GetPacketRx_NoWantedPkt,
                        pstSjbManager->stPara.uwLostByGet,
                        pstSjbManager->stPara.uwLastTS,
                        pstSjbManager->stBuff.pstTail->stPktData.ulTS);

            /* ����ȡ��ʧ�ܹ��࣬����Ϊ���쳣����ʼ��JB */
            if(pstSjbManager->stPara.uwGetErrCnt > VOICE_SJB_TSErrCntMax)
            {
                VOICE_SJB_InitRx(pstSjbManager);
                OM_LogWarning(VOICE_SJB_GetPacketRx_InitJB);

                /* �ͷŻ��� */
                VOS_Splx(uwCpuSr);
                return UCOM_RET_FAIL;
            }

            uwRet = UCOM_RET_FAIL;
        }
        /* �ɹ�ȡ��β֡������LastTS */
        else
        {
            pstSjbManager->stPara.uwGetErrCnt = 0;
            UCOM_MemCpy(pstPkt, pstSjbManager->stBuff.pstTail, sizeof(IMSA_VOICE_RX_DATA_IND_STRU));
            pstSjbManager->stPara.uwLastTS = pstSjbManager->stBuff.pstTail->stPktData.ulTS;
            uwRet = VOICE_SJB_RemoveTailRx(pstSjbManager);
        }
    }

    /*��д�ϱ���JB����*/
    UCOM_MemSet(&stJBHook.stData, 0, sizeof(HME_JB_HIFI_STATICS));
    stJBHook.stData.uiBufLen       = pstSjbManager->stPara.uwCurPktNum * VOICE_JB_PKT_TIME_20MS;
    stJBHook.stData.uiBufMaxSize   = VOICE_SJB_RX_DEPTH;
    stJBHook.stData.uiSeq          = pstPkt->usSN;
    stJBHook.stData.uiTimeStamp    = pstPkt->ulTS;
    stJBHook.stData.uiSSRC         = pstPkt->ulSSRC;
    stJBHook.stData.uiPacketRecv   = pstSjbManager->stPara.uwRcvPktNum;
    stJBHook.stData.uiPacketLoss   = pstSjbManager->stPara.uwLostByGet;
    stJBHook.stData.uiJBPacketLoss = pstSjbManager->stPara.uwLostByStore;

    /* ��JB�Ĳ�����ȡ�ϱ���OM */
    VOICE_DbgSendHook(VOICE_HOOK_JB_PARA,
                      &stJBHook,
                      sizeof(HME_JB_HIFI_STATICS));

    /* �ͷŻ��� */
    VOS_Splx(uwCpuSr);
    return uwRet;
}


VOS_UINT32 VOICE_SJB_GetPktNumTx(VOICE_SJB_TX_MANAGER_STRU* pstSjbManager)
{
    return pstSjbManager->stPara.uwRcvPktNum;
}


VOS_UINT32 VOICE_SJB_TSCompare(VOS_UINT32 uwSrc, VOS_UINT32 uwDest)
{
    VOS_UINT32 uwRet;

    if(uwDest > uwSrc)
    {
        uwRet = uwDest - uwSrc;
        return uwRet;
    }
    else
    {
        /* �˴���ʱ�귭ת����� */
        if((uwDest + (VOICE_SJB_UINT32_MAX - uwSrc)) < VOICE_SJB_MaxTSDiff)
        {
            uwRet = uwDest + (VOICE_SJB_UINT32_MAX - uwSrc) + 1;
            return uwRet;
        }
        else
        {
            return 0;
        }
    }
}

