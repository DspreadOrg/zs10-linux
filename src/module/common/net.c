/**********************************************************************
* Copyright (C)    Dspread
* File name: net.c
* Summary: Communication related APIs
*
***********************************************************************/
//Header file
#include "var.h"
#include "apn.h"

//Macro definitions
#define NET_LOG_MODE
#ifdef NET_LOG_MODE
#define NET_LOG(msg, ...) LOG("[NET]"msg"", ##__VA_ARGS__)
#else
#define NET_LOG(msg, ...)
#endif

#define NET_TASK_STACK_SIZE (5 * 1024)
#define NET_TASK_EVENT_NUM (5)
#define NET_TASK_EVENT_WAIT_TIMEOUT_MS (100)

#define LTE_REG_WAIT_SEM_TIME_MS (1000)
#define LTE_REG_WAIT_TIME_MS (60 * 1000)
#define LTE_REG_WAIT_CNT (LTE_REG_WAIT_TIME_MS / LTE_REG_WAIT_SEM_TIME_MS)
#define LTE_MONITOR_WAIT_EVENT_TIME_MS 100
#define LTE_GET_CSQ_PLAY_AUDIO_TIME_MS (60 * 1000)
#define LTE_GET_CSQ_WATT_TIEM_MS 100
#define LTE_GET_CSQ_PLAY_AUDIO_CNT (LTE_GET_CSQ_PLAY_AUDIO_TIME_MS / LTE_GET_CSQ_WATT_TIEM_MS)

#define DEV_LTE_REGISTER_MAX_TIME 1
#define LTE_PROFILE_INDEX 1
#define DEV_LET_IP_VER_IPV4 1
#define DEV_LET_IP_VER_IPV6 2
#define DEV_LET_IP_VER_IPV4V6 3

#define WIFI_CONFIG_TIMEOUT_S (10 * 60) //AP configuration waiting time of 10 minutes
#define WIFI_MONITOR_OFFLINE_WAIT_TIME_MS (100)
#define WIFI_MONITOR_ONLINE_WAIT_TIME_MS (10 * 1000)
#define WIFI_MONITOR_DIS_PLAY_TIME_MS (30 * 1000)  
#define WIFI_MONITOR_WATI_PLAY_CNT (WIFI_MONITOR_DIS_PLAY_TIME_MS / WIFI_MONITOR_OFFLINE_WAIT_TIME_MS)

#define DATA_CALL_PROFILE_IDX 1

//Constant

//Local variables
static T_Task s_tNetTaskID;
static T_Mutex s_tLETStatusMutex = NULL;
static T_Mutex s_tWIFIStatusMutex = NULL;

//Global variables
int g_iNetOnline = 0;

//Function implementation
static const StructAPNinfo dApn = {NULL, "3gnet", NULL, NULL};
static QL_ERROR_CODE_E start_data_call(void)
{
    int ret = QL_SUCCESS;
    char IMSI[32] = {0};
    StructAPNinfo const * pAPNinfo = NULL;

    for (int i = 0; i < 5; i++)
    {
        YMI_WlGetInfo(EM_WLM_INFO_IMSI, IMSI, sizeof(IMSI));
        if (strlen(IMSI) != 0)
        {
            break;
        }
        SLEEP_S(1);
    }
    
    pAPNinfo=GetApnInfo(IMSI);
   	if (pAPNinfo==NULL)
   	{
   		NET_LOG("%s_%d ===APN not found,use default\n", __func__, __LINE__);
   		pAPNinfo=&dApn;
   	}

    ret = YMI_PppDial(DATA_CALL_PROFILE_IDX, (char *)pAPNinfo->apn, (char *)pAPNinfo->user, (char *)pAPNinfo->password);
	if(ret!=0)
    {
        NET_LOG("%s_%d ===Dial failed,ret=%d\n", __func__, __LINE__, ret);
        ret = RET_FAIL;
    }

exit:
	return ret;
}

static void LTEStatusMonitor(void)
{
	int iRet, iCnt = 0, iSQ = 0;
	char szBuff[128] = "";
	EM_WLM_SIM_STATUS eSimStatus;
	uint wPppStatus;

	YMI_MutexLock(s_tLETStatusMutex, WAIT_FOREVER);
	do
	{
		//check sim
		iCnt = 0;
		while (1)
		{
			// if(SystemInfoGetCurComm() != COMMU_TYPE_LTE)
			// {
			// 	NET_LOG("====>out lte monitor");
			// 	goto exit;
			// }
			iRet = YMI_WlCheckSIM(&eSimStatus);
			NET_LOG("check sim status ret=%d, status=%d", iRet, eSimStatus);
			if(iRet == RET_SUCC && eSimStatus == EM_WLM_SIM_STATUS_READY)
			{
				NET_LOG("sim card ready");
				break;
			}
			SLEEP_S(1);
			iCnt ++;
			if(iCnt % 30 == 10)
			{
				iCnt = 0;
				NET_LOG("check sim card cnt = %d", iCnt);
				// AudioPlayInterrupt(AudioQingcrsimk, FILE_TEXT);
			}
		}
		//get reg status
        iCnt = 0;
		while (1)
		{
			// if(SystemInfoGetCurComm() != COMMU_TYPE_LTE)
			// {
			// 	NET_LOG("====>out lte monitor");
			// 	goto exit;
			// }
			iRet = YMI_WlResidentStatus();
			iCnt ++;
			if(iRet == RET_SUCC)
			{
				NET_LOG("resident status ok");
				break;
			}
			else if(iCnt % LTE_REG_WAIT_CNT == 0)
			{
				if(iCnt == LTE_REG_WAIT_CNT)
				{
					NET_LOG("[ERR]wait reg out time reset");
					YMI_WlModemReset();
				}
				else
				{
					// AudioPlayInterrupt(AudioWanlljsbqjcwl, FILE_TEXT);
				}
			}
		}
		//csq
		iCnt = 0;
		iSQ = 0;
		while (1)
		{
			// if(SystemInfoGetCurComm() != COMMU_TYPE_LTE)
			// {
			// 	NET_LOG("====>out lte monitor");
			// 	goto exit;
			// }
			iRet = YMI_WlModemGetSQ(&iSQ);
			if(iSQ < 12 || iRet != RET_SUCC)
			{
				iCnt ++;
				if(iCnt >= LTE_GET_CSQ_PLAY_AUDIO_CNT)
				{
					iCnt = 0;
					// AudioPlayInterrupt(AudioDangq4Gwlxhr, FILE_TEXT);
				}
			}
			else if(iSQ > 15)
			{
				break;
			}
			SLEEP_MS(LTE_GET_CSQ_WATT_TIEM_MS);
		}
		//ppp
		iCnt = 0;
		while (1)
		{
			// if(SystemInfoGetCurComm() != COMMU_TYPE_LTE)
			// {
			// 	NET_LOG("====>out lte monitor");
			// 	goto exit;
			// }
            iRet = start_data_call();
			if(iRet == RET_SUCC)
			{
				break;
			}
			if(iCnt == 2)
			{
				// AudioPlayInterrupt(AudioWanlljsbqjcwl, FILE_TEXT);
			}
			else if(iCnt >= 3)
			{
				NET_LOG("[ERR]wait ppp out time reset");
				YMI_WlModemReset();
				iCnt = 0;
			}
			SLEEP_S(60);
			iCnt ++;
		}
		//check ppp state
		wPppStatus = 0;
		iCnt = 0;
		while (1)
		{
			// if(SystemInfoGetCurComm() != COMMU_TYPE_LTE)
			// {
			// 	NET_LOG("====>out lte monitor");
			// 	goto exit;
			// }
			iRet = YMI_PppCheck((uint *)&wPppStatus, LTE_PROFILE_INDEX);
			if(iRet == RET_SUCC && wPppStatus)
			{
                g_iNetOnline = 1;
                if(!g_iNetOnline)
                // if(DevInfoGetNetState() != EM_DEV_NET_STATE_ONLINE)
				{
					// iRet = YMI_PppGetNetAddr(szBuff, LTE_PROFILE_INDEX);
					if(iRet == RET_SUCC)
					{
						// DevInfoSetIpAdrr(szBuff);
						// DevInfoSetNetState(EM_DEV_NET_STATE_ONLINE);
						// DevNixieTubeNetOnline();
						// AudioPlayInterrupt(AudioWanglljcg, FILE_TEXT);
                        NET_LOG("====>lte online");
					}
				}
                iCnt = 0;
			}
			else
			{
				if(iCnt == 2)
				{
					// DevInfoSetNetState(EM_DEV_NET_STATE_OFFLINE);
                    g_iNetOnline = 0;
					NET_LOG("[ERR]check ppp status=%d, ret=%d", wPppStatus, iRet);
					break;
				}
				iCnt ++;
			}
			SLEEP_MS(500);
		}
	} while (1);
exit:
	YMI_PppHangup(LTE_PROFILE_INDEX);
	YMI_MutexUnLock(s_tLETStatusMutex);
}
// void WIFIInfoSet(char *pacSSID, char *pacPWD)
// {
//     SystemInfoSetWIFISSID(pacSSID);
//     SystemInfoSetWIFIPWD(pacPWD);
// }

static void NetTask(void *pvParam)
{
    // EM_COMMU_TYPE eCurCommuType = COMMU_TYPE_UNKNOWN;
    ST_Event stEvent;
    
    // CheckWifiInit();
    while (1)
    {
        memset(&stEvent, 0x00, sizeof(ST_Event));
        if(YMI_EventWait(&stEvent, NET_TASK_EVENT_WAIT_TIMEOUT_MS) == YMI_OK)
		{
			if(stEvent.wId == TASK_MANAGE_EVENT_ID && stEvent.wParam1 == TASK_OPT_EXIT)
			{
				break;
			}
		}
        LTEStatusMonitor();
        // WIFIStatusMonitor();

        // if(eCurCommuType != SystemInfoGetCurComm())
        // {
        //     DevInfoSetNetState(EM_DEV_NET_STATE_OFFLINE);
        //     if(eCurCommuType != COMMU_TYPE_UNKNOWN)
        //     {
        //         MQTTExit();
        //         DevMqttReConn();
        //         MQTTWaitStopRuning();
        //         if(eCurCommuType == COMMU_TYPE_WIFI)
        //         {
        //             Ext_wifi_Off();
        //         }
        //     }
        //     NET_LOG("net changing %d to %d", eCurCommuType, SystemInfoGetCurComm());
        //     eCurCommuType = SystemInfoGetCurComm();
        //     if(DevInfoGetTiming() == DEV_TIMING_POWER_ON)
        //     {
        //         ql_rtos_semaphore_wait(s_tDevNetStartMonitorSem, POWER_ON_NET_MONITOR_MAX_TIME_MS);
        //     }
        //     if (eCurCommuType == COMMU_TYPE_LTE)
        //     {
        //         LTEStatusMonitor();
        //     }
        //     else if(eCurCommuType == COMMU_TYPE_WIFI)
        //     {
        //         Ext_wifi_Open();
        //         WIFIStatusMonitor();
        //     }
        // }
        // else
        // {
        //     if(eCurCommuType == COMMU_TYPE_WIFI)
        //     {//WiFi need to switch the connection mode
        //         if (DevInfoGetWIFISetType() != WIFI_SET_TYPE_UNKONW)
        //         {
        //             WIFIStatusMonitor();
        //         }
        //     }
        // }
    }//end of while
    YMI_TaskDelete(s_tNetTaskID);
}

int NetTaskInit(void)
{
	int iRet;

    iRet = YMI_MutexCreat(&s_tLETStatusMutex);
    if(iRet != YMI_OK)
	{
	    NET_LOG("[ERR]let mutex creat ret=%d", iRet);
		return RET_FAIL;
	}
    iRet = YMI_MutexCreat(&s_tWIFIStatusMutex);
    if(iRet != YMI_OK)
	{
	    NET_LOG("[ERR]wifi mutex creat ret=%d", iRet);
		return RET_FAIL;
	}

	iRet = YMI_TaskCreat(&s_tNetTaskID, "net_task", NET_TASK_STACK_SIZE, NetTask, NULL, EM_TASK_PRIORITY_BELOW_HIGH, NET_TASK_EVENT_NUM);
	if(iRet != YMI_OK)
	{
		NET_LOG("[ERR]task creat ret=%d", iRet);
		return RET_FAIL;
	}
	return RET_SUCC;
}


//end of file