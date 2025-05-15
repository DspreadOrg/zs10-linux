/**********************************************************************
* Copyright (C)    Dspread
* File name: mqtt.c
* Summary: MQTT related APIs
*
***********************************************************************/
//Header file
#include "var.h"

//Macro definitions
#define MMQTT_LOG_MODE
#ifdef MMQTT_LOG_MODE
#define MQTT_LOG(msg, ...) LOG("[MQTT]"msg"", ##__VA_ARGS__)
#else
#define MMQTT_LOG_MODE(msg, ...)
#endif

#define HDD_MQTT_TASK_STACK_SIZE (35 * 1024)
#define MQTT_TASK_EVENT_CNT 10
//Constant

//Local variables
static T_Task s_tHDDMQTTTaskID = 0;
//Global variables
extern int g_iNetOnline;

//Function implementation


void UserMessageProcess(char *pszTopic, uchar *pszRecvData, uint wRecvDataLen)
{
    MQTT_LOG("%s get msg=%s,%d", __func__, pszRecvData, wRecvDataLen);
}

void http_test(void)
{
    char send[64] = "{sn:QR939200000058}";
    char recv[128] = "";
    char file[64] = "EFS:/20230102153409104110079.jpg";
    int ret = 0;
	// EM_SOCKET_TYPE type = SOCKET_TCP_CLIENT; 
	EM_SOCKET_TYPE type = SOCKET_WIFI_TCP_CLIENT; 
    ret = YMI_HttpCommu(type, "http://dspread/device/findDeviceInfo", NULL, 
        (uchar *)send, strlen(send), (uchar *)recv, sizeof(recv));
    MQTT_LOG("http commut ret=%d", ret);

    YMI_FsDelete(file);
    ret = YMI_HttpDownLoadFile(type, 
        "http://dspread/bigScreen/1/20230102/image/20230102153409104110079.jpg", 0, file);
    MQTT_LOG("down load file ret=%d", ret);
    ulong size = -1;
    YMI_FsFileSize(file, &size);
    MQTT_LOG("file size=%d", size);
	if(size > 0)
	{
		// uint16_t *pic = NULL;
		// pic = MALLOC(size, uint16_t);
		// if(pic != NULL)
		// {
		// 	int fd = YMI_FsOpen(file, "r");
		// 	if(fd)
		// 	{
		// 		ret = YMI_FsRead(fd, (char *)pic, size);
		// 		MQTT_LOG("fs read ret=%d", ret);
		// 		YMI_FsClose(fd);
		// 	}
		// 	YMI_DispSetBackLight(1);
		// 	YMI_DispBlockUpdate(0, 0, 800, 920, pic);
		// 	while (1)
		// 	{
		// 		SLEEP_S(1);
		// 	}
		// 	FREE(pic);
		// }

		// ret = YMI_DispSetBackLight(1);
        MQTT_LOG("YMI_DispSetBackLight ret=%d", ret);
        // ret = YMI_DispJPG(file, 0, 100, 800, 920);
        MQTT_LOG("YMI_DispJPG ret=%d", ret);
        while (1)
        {
            SLEEP_S(1);
        }
    }
	
    YMI_FsDelete(file);



}
int HDDMQTTTaskInit(void)
{
	int iRet;
	ST_MQTTConf *pstConf = NULL;
	
	while (!g_iNetOnline)
	{
		SLEEP_S(1);
	}
	


	pstConf = MALLOC(1, ST_MQTTConf);
    if(pstConf == NULL)
    {
        MQTT_LOG("[ERR]malloc MQTT_INFO_T");
        return RET_FAIL;
    }
    memset(pstConf, 0x00, sizeof(ST_MQTTConf));
    //default
    pstConf->iPort = 443;
    sprintf(pstConf->szUrl, "%s", "iot-as-mqtt.cn-shanghai.aliyuncs.com");
    pstConf->eSocketType = SOCKET_TCP_CLIENT;
    pstConf->iConnectTimeOut = 5000;
    pstConf->eConType = MQTT_CON_TYPE_TLS;
    pstConf->iHeartBeatIntervalTime = 300 * 1000; 
    pstConf->iHeartBeatLostMaxTime = 1; 

    sprintf(pstConf->szDeviceName, "%s", "QR939200000058");
    sprintf(pstConf->szDeviceSecret, "%s","f88723d1f719d3aa3404933e06b45c8e");
    sprintf(pstConf->szProductKey, "%s", "gwtlJeUFTcF");
    // sprintf(pstConf->astPubList[0].szPubTopic, "/%s/%s/user/report", pstConf->szProductKey, pstConf->szDeviceName);
    // pstConf->astPubList[0].cb_PubMsgPack = ReportStatusPubMsgPack;
    // sprintf(pstConf->astPubList[1].szPubTopic, "/ota/device/inform/%s/%s", pstConf->szProductKey, pstConf->szDeviceName);
    // pstConf->astPubList[1].cb_PubMsgPack = PubTermInfo;
    // pstConf->iPubNum = 2;

    sprintf(pstConf->astSubList[0].szSubTopic, "/%s/%s/user/message", pstConf->szProductKey, pstConf->szDeviceName);
    pstConf->astSubList[0].cb_SubRecv = UserMessageProcess;
    sprintf(pstConf->astSubList[1].szSubTopic, "/ota/device/upgrade/%s/%s", pstConf->szProductKey, pstConf->szDeviceName);
    pstConf->astSubList[1].bIsOta = true;
    // sprintf(pstConf->atSubInfo[2].acSubTopic, "/sys/%s/%s/rrpc/request/+", pstConf->acProductKey, pstConf->acDeviceName);
    // pstConf->atSubInfo[2].fSubRecvHandle = QlcyMessageProcess;
    // pstConf->atSubInfo[2].pvParam = pstConf->atSubInfo[2].acSubTopic;
    // sprintf(pstConf->atSubInfo[3].acSubTopic, "/sys/%s/%s/thing/service/#", pstConf->acProductKey, pstConf->acDeviceName);
    // pstConf->atSubInfo[3].fSubRecvHandle = QlcyMessageProcess;
    // pstConf->atSubInfo[3].pvParam = pstConf->atSubInfo[3].acSubTopic;
    pstConf->iSubNum = 2;
	
	iRet = YMI_TaskCreat(&s_tHDDMQTTTaskID, "hdd mqtt", HDD_MQTT_TASK_STACK_SIZE, YMI_MQTTProce, pstConf, EM_TASK_PRIORITY_NORMAL, MQTT_TASK_EVENT_CNT);
    if(iRet != YMI_OK)
    {
        FREE(pstConf);
		return RET_FAIL;
    }
	// http_test();



	return RET_SUCC;
}


//end of file