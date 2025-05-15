/**********************************************************************
* Copyright (C)    Dspread
* File name: mqtt_aliyun.c
* Abstract: MQTT APIs for DS200 devices
*
***********************************************************************/
#include "var.h"
#include "mqtt_aliyun.h"

#define MQTT_LOG_MODE
#ifdef MQTT_LOG_MODE
#define ALIOT_PRINT(msg, ...) LOG("[MQTT]"msg"", ##__VA_ARGS__)
#else
#define ALIOT_PRINT(msg, ...)
#endif


//message for aliyun,define product key, device name, devcie secret
#define DEV_CLIENT_ID			"00001"

//define secure mode ;  2 : tls connect mode, 3: tcp stright connect mode
#define MQTT_TCP_MODE			"3"
#define MQTT_TLS_MODE			"2"

#define SECURE_MODE			MQTT_TLS_MODE


StructTermInfo TermInfo = {0};
aliot_devc_info_t xr_devc_info = {0};
xr_mqtt_param_t	xr_mqtt_para = {0};


mqt_cal cal_set;
Client ysw1_mqtt_client={0,};
char mqtt_ctrl = 0;
//static int ota_download_status = -100;
//static int ota_download_progress = 0;
// unsigned char mqtt_first_connet = 0;

#define OTA_THREAD_STACK_SIZE		(5 * 1024)
T_Task g_ota_thread = 0;

char firmware_version[50] = "ZS10";
extern char YSW1_messageId[50];
extern char YSW1_notifyUrl[200];

extern char *ali_ca_cert;
static ST_MQTTConf *s_pstConf = NULL;
static T_Task s_tMQTTTaskID = 0;


extern int g_iNetOnline;
//function redefine
void lpm_set(uint8_t lock_type,uint8_t value)
{
	
}


int mqtt_client_close(Client* c)
{
	return 0;
}

void alink_devc_init(void)
{
	
}

void mqtt_para_init(void)
{

}

int tcp_mqtt_client(Client* c)
{
	return 0;
}
int ssl_mqtt_client(Client* client, 
	const char *server_cert, uint32_t server_cert_len, 
	const char *client_cert, uint32_t client_cert_len,
	const char *client_prikey, uint32_t client_prikey_len)
{
	if(s_pstConf == NULL)
	{
		return -1;
	}
	if(ysw1_mqtt_client.isconnected == 1)
	{
		return 0;
	}
	s_pstConf->c_pszServerCert = server_cert;
	s_pstConf->iServerCertLen = server_cert_len;
	s_pstConf->c_pszClientCert = client_cert;
	s_pstConf->iClientCertLen = client_cert_len;
	s_pstConf->c_pszClientPrivkey = client_prikey;
	s_pstConf->iClientPrivkeyLen = client_prikey_len;
	return 0;
}

void aliot_device_info_ex( const char *url,
        const char *product_key,
        const char *device_name,
        const char *device_secret,
        const char *dev_client_id,
        const char *secure_mode
        )
{
	if(s_pstConf == NULL)
	{
		s_pstConf = MALLOC(1, ST_MQTTConf);
    	if(s_pstConf == NULL)
    	{
        	ALIOT_PRINT("[ERR]YMI_SysMalloc MQTT_INFO_T");
        	return ;
    	}
	}
	memset(s_pstConf, 0x00, sizeof(ST_MQTTConf));
    sprintf(s_pstConf->szUrl, "%s", url);
#ifdef AWS_SERVER
	s_pstConf->iPort = 8883;
#else
	s_pstConf->iPort = 443;
#endif
    s_pstConf->eSocketType = (SysInfoGet()->iNetChanlLTE == WIFI_MODE ? SOCKET_WIFI_TCP_CLIENT : SOCKET_TCP_CLIENT);
    s_pstConf->iConnectTimeOut = 5000;
	if(memcmp(secure_mode, MQTT_TLS_MODE, 1) == 0)
	{
		s_pstConf->eConType = MQTT_CON_TYPE_TLS;
	}
	else
	{
		s_pstConf->eConType = MQTT_CON_TYPE_TCP;
	}
    s_pstConf->iHeartBeatIntervalTime = 300 * 1000; 
    s_pstConf->iHeartBeatLostMaxTime = 1; 

    sprintf(s_pstConf->szDeviceName, "%s", device_name);
    sprintf(s_pstConf->szDeviceSecret, "%s",device_secret);
    sprintf(s_pstConf->szProductKey, "%s", product_key);
	sprintf(s_pstConf->szClientId, "%s", dev_client_id);
}

void aliot_device_info(
						const char *product_key,
						const char *device_name,
						const char *device_secret,
						const char *dev_client_id,
						const char *secure_mode)
{
	if(s_pstConf == NULL)
	{
		s_pstConf = MALLOC(1, ST_MQTTConf);
    	if(s_pstConf == NULL)
    	{
        	ALIOT_PRINT("[ERR]YMI_SysMalloc MQTT_INFO_T");
        	return ;
    	}
	}
	memset(s_pstConf, 0x00, sizeof(ST_MQTTConf));
	s_pstConf->iPort = 443;
    sprintf(s_pstConf->szUrl, "%s", "iot-as-mqtt.cn-shanghai.aliyuncs.com");
    s_pstConf->eSocketType = (SysInfoGet()->iNetChanlLTE == WIFI_MODE ? SOCKET_WIFI_TCP_CLIENT : SOCKET_TCP_CLIENT);
    s_pstConf->iConnectTimeOut = 5000;
    if(memcmp(secure_mode, MQTT_TLS_MODE, 1) == 0)
	{
		s_pstConf->eConType = MQTT_CON_TYPE_TLS;
	}
	else
	{
		s_pstConf->eConType = MQTT_CON_TYPE_TCP;
	}
    s_pstConf->iHeartBeatIntervalTime = 300 * 1000; 
    s_pstConf->iHeartBeatLostMaxTime = 1; 

    sprintf(s_pstConf->szDeviceName, "%s", device_name);
    sprintf(s_pstConf->szDeviceSecret, "%s",device_secret);
    sprintf(s_pstConf->szProductKey, "%s", product_key);
	sprintf(s_pstConf->szClientId, "%s", dev_client_id);
}
int set_iot_devc_info(aliot_devc_info_t *devc_info)
{
	if(devc_info == NULL)
	{
		return RET_FAIL;
	}
	sprintf(s_pstConf->szUrl, "%s", devc_info->host_name);
	s_pstConf->iPort = devc_info->port;
	sprintf(s_pstConf->szClientId, "%s", devc_info->client_id);
	return RET_SUCC;
}

void MQTTConnectErr(void *pvParam)
{
	cal_set.mqtt_con = MQTT_CACK;
	cal_set.mqtt_pub = MQTT_CACK;
	cal_set.mqtt_sub = MQTT_CACK;
	ysw1_mqtt_client.isconnected = 1;
	ysw1_mqtt_client.isDiscon = 1;
	tts_play_set_idx(AUD_ID_MQTT_CONNECT_FAIL,0,0);
}
void MQTTConnectSucc(void *pvParam)
{	
	
	cal_set.mqtt_con = MQTT_DICACK;
	cal_set.mqtt_pub = MQTT_DICACK;
	cal_set.mqtt_sub = MQTT_DICACK;
	ysw1_mqtt_client.isconnected = 1;
	ysw1_mqtt_client.isDiscon = 0;
	tts_play_set_idx(AUD_ID_MQTT_CONNECT_SUCESS,0,0);
}

static int32_t MQTTLog(int32_t iCode, char *pszMsg)
{
	ALIOT_PRINT("%s", pszMsg);
	return RET_SUCC;
}

static int MQTTConnect(Client* pstClient, MQTTPacket_connectData* pstConData)
{
	if(s_pstConf == NULL)
	{
		return -1;
	}
	if(ysw1_mqtt_client.isconnected)
	{
	  	if(cal_set.mqtt_con == MQTT_CACK)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	s_pstConf->iKeepAlive = pstConData->keepAliveInterval * 3 / 2;
	s_pstConf->iHeartBeatIntervalTime = pstConData->keepAliveInterval * 1000;
	s_pstConf->ucCleanSession = pstConData->cleansession;
	s_pstConf->cb_MQTTConnetErr = MQTTConnectErr;
	s_pstConf->cb_MQTTConnetSucc = MQTTConnectSucc;
	s_pstConf->cb_MQTTLostHeartBeat = MQTTConnectErr;
	// s_pstConf->cb_MQTTStateLog = MQTTLog;

	//No calculations are done externally
	sprintf(s_pstConf->szClientId, "%s", pstConData->clientID.cstring);
	sprintf(s_pstConf->szUserName, "%s", pstConData->username.cstring);
	sprintf(s_pstConf->szPassWord, "%s", pstConData->password.cstring);

	if (strlen(s_pstConf->szUrl))
		sprintf(s_pstConf->szUrl, "%s", xr_mqtt_para.host_name);
	if (s_pstConf->iPort > 0)
		s_pstConf->iPort = xr_mqtt_para.port;
	if(strlen(s_pstConf->szClientId))
		s_pstConf->isP2P = TRUE;

	return 0;

}

int MQTTSubscribe(Client* pstClient, const char* c_pszTopic, enum QoS eQos, CB_MQTT_SUB_RECV cb_MessHandler)
{
	int iSubCnt;
	
	if(s_pstConf == NULL)
	{
		return -1;
	}
	if(ysw1_mqtt_client.isconnected)
	{
	  	if(cal_set.mqtt_con == MQTT_CACK)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	iSubCnt = s_pstConf->iSubNum;
	if(iSubCnt >= MQTT_SUB_TOPIC_MAX_SIZE)
	{
		return -1;
	}
	sprintf(s_pstConf->astSubList[iSubCnt].szSubTopic, "%s", c_pszTopic);
	s_pstConf->astSubList[iSubCnt].cb_SubRecv = cb_MessHandler;
	s_pstConf->iSubNum ++;
	return 0;
}

int MQTTPublish(Client* pstClient, const char* c_pszTopic, CB_MQTT_PUB_MSG_PACK cb_MessHandler)
{
	int iPubCnt;
	
	if(s_pstConf == NULL)
	{
		return -1;
	}
	if(ysw1_mqtt_client.isconnected)
	{
	  	if(cal_set.mqtt_con == MQTT_CACK)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	iPubCnt = s_pstConf->iPubNum;
	if(iPubCnt >= MQTT_PUB_TOPIC_MAX_SIZE)
	{
		return -1;
	}
	sprintf(s_pstConf->astPubList[iPubCnt].szPubTopic, "%s", c_pszTopic);
	s_pstConf->astPubList[iPubCnt].cb_PubMsgPack = cb_MessHandler;
	s_pstConf->iPubNum ++;
	return 0;
}

int MQTTDisconnect(Client* pstClient)
{
	return 0;
}
int MQTTYield(Client* pstClient, int iTimeOutMS)
{
	if (!g_iNetOnline)
	{
		SLEEP_MS(iTimeOutMS);
		return -1;
	}
	if(s_tMQTTTaskID)
	{
		SLEEP_MS(iTimeOutMS);
		return RET_SUCC;
	}
	int iRet;
	iRet = YMI_TaskCreat(&s_tMQTTTaskID, "mqtt", 35 * 1024, YMI_MQTTProce, s_pstConf, EM_TASK_PRIORITY_NORMAL, 10);
    if(iRet != YMI_OK)
    {
        FREE(s_pstConf);
		return RET_FAIL;
    }
	return RET_SUCC;
}
void messageArrived(char *pszTopic, uchar *pbyRecv, uint wRecvLen)
{
#if 1
	ALIOT_PRINT("Message arrived on topic : %s: %.*s\n",
				pszTopic,
				wRecvLen,
				(char *)pbyRecv);
#else
	ALIOT_PRINT("Message arrived on topic : %.*s: %d\n",
				data->topicName->lenstring.len,
				data->topicName->lenstring.data,
				data->message->payloadlen);

	if(DeviveMessageAnaly(data->message->payload,data->message->payloadlen) == 0)
	{	
		;
	}
#endif
}

#if 0
const char * messrec[]=
{
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B01\",\"money\":1.00,\"voiceMsg\":\"" PAY_MSG_SK "1.00" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B02\",\"money\":1.01,\"voiceMsg\":\"" PAY_MSG_SK  "1.01" PAY_MSG_YUAN "\",\"notifyUrl\":\"\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B03\",\"money\":15.00,\"voiceMsg\":\"" PAY_MSG_SK "15.00" PAY_MSG_YUAN "\",\"notifyUrl\":\"\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B04\",\"money\":123.1,\"voiceMsg\":\"" PAY_MSG_SK "123.1" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B05\",\"money\":215.13,\"voiceMsg\":\"" PAY_MSG_SK "215.13" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B06\",\"money\":10006.87,\"voiceMsg\":\"" PAY_MSG_SK  "10006.87" PAY_MSG_YUAN "\",\"notifyUrl\":\"\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B07\",\"money\":138,\"voiceMsg\":\"" PAY_MSG_SK "138" PAY_MSG_YUAN "\",\"notifyUrl\":\"\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B08\",\"money\":15,\"voiceMsg\":\"" PAY_MSG_SK "15" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B09\",\"money\":10000,\"voiceMsg\":\"" PAY_MSG_SK "10000" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
};
char * topic="/user/down/zbkby1dy42CDQM6d/8108200700100045";

void testmess(void)
{
	static int set=0;
//	const char * messag="";
	MessageData aa;
	MQTTString str;
	MQTTMessage mess;
	
	ALIOT_PRINT("------------\n%s,line %d,mess %p\n",__func__,__LINE__,messrec[set%3]);
	str.cstring=topic;
	str.lenstring.data=topic;
	str.lenstring.len=strlen(topic);

	mess.payload=(char *)messrec[set%(sizeof(messrec)/sizeof(messrec[0]))];
	mess.payloadlen=strlen(mess.payload);
	
	aa.message=&mess;
	aa.topicName=&str;
	set++;
	if ((set/1)<=(sizeof(messrec)/sizeof(messrec[0])))
//	if (set%20)
		{
			messageArrived(&aa);
		}
}

void testota(void)
{
#if 1
	ota_params *params = 0;
	char *url_buff = 0;
	char flag=0;
	
	if (g_ota_thread!=NULL)
		return;
		
	url_buff = malloc(strlen(url) + 1);
	strcpy(url_buff,url);
	
	params = malloc(sizeof(ota_params));
	memset(params, 0, sizeof(ota_params));
	params->url = url_buff;
	
	if (ql_rtos_task_create(&g_ota_thread,
						OTA_THREAD_STACK_SIZE,
						100,
						"ota_run",
						ota_run,
						params) != OS_OK) {
		ALIOT_PRINT("OTA task create failed\n");
		if (url_buff)
		{
			free(url_buff);
		}
		if (params)
		{
			free(params);
		}
	}
	ALIOT_PRINT("OTA start, wait...\n");
#else
	static int set=0;
//	const char * messag="";
	MessageData aa;
	MQTTString str;
	MQTTMessage mess;
	const char * otatopic="/ota/device/upgrade/48b1NeMrLpRTcDpp/YR9992011L4M000021";
	const char * otamess=
												/ota/device/upgrade/a1QyLNqji87/LDLTE0000010: 
												{"code":"1000","data":{"size":4064984,"sign":"6024c1aab77794880407abd6dbce5e85","version":"1.0.1","url":"https://iotx-ota.oss-cn-shanghai.aliyuncs.com/ota/377754e42556b94217f4b5ed0182738d/ckljbc70k0001268jjf7z3icu.bin?Expires=1614849068&OSSAccessKeyId=LTAI4G1TuWwSirnbAzUHfL3e&Signature=2BHwoYrFfG61QIRH7iFd1aKG6U4%3D","signMethod":"Md5","md5":"6024c1aab77794880407abd6dbce5e85"},"id":1614762668146,"message":"success"}
		
//	"{\"dn\":\"YR9992011L4M000021\",\"id\":\"164118048\",\"method\":\"upgrade\",\"msg\":\"{\\\"softName\\\":\\\"����4G+wifi����\\\",\\\"fileMd5\\\":\\\"C076E3C3580D223438218122ED6FFA3A\\\",\\\"type\\\":\\\"firmware\\\",\\\"version\\\":\\\"1.2_GW2.01_A\\\",\\\"url\\\":\\\"http://192.168.5.236/EC100Y_GW2.99_JH_20201214.bin\\\"}\",\"ts\":1607400656532}";
	ALIOT_PRINT("------------\n%s,line %d,mess %p\n",__func__,__LINE__,otamess);
	str.cstring=otatopic;
	str.lenstring.data=otatopic;
	str.lenstring.len=strlen(otatopic);

	mess.payload=(char *)otamess;
	mess.payloadlen=strlen(mess.payload);
	
	aa.message=&mess;
	aa.topicName=&str;
	set++;
	if (set<5)
		{
			otaUpdateArrived(&aa);
		}
#endif
}

#endif


#define MQTT_THREAD_STACK_SIZE           (1024 * 5)
#define MQTT_DISCONNECT_MAX_TIMES        40 //20
#define MQTT_YIELD_FAILED_MAX_TIMES      50
// ql_task_t mqtt_task_ctrl_thread = NULL;
T_Task mqtt_task_ctrl_thread = 0;
uint8_t mqtt_con_nums = 0;
uint8_t mqtt_con_rsp_times = 0;

/* 0: disconnet   1: connected   -1 : error*/
int check_mqtt_server(Client * c)
{
	// ALIOT_PRINT("check mqtt server");
	SLEEP_MS(100);
	return 1;
}
void mqtt_reconnect(char const * flag)
{
// 	int iresult=1;
// 	int ii;
		
	mqtt_con_nums ++;
    
// 	if(mqtt_con_nums == 1) {
// 		ii = mqtt_con_nums*10;
//         ALIOT_PRINT( "mqtt con_num=%d, reconnect after %dms...\n", mqtt_con_nums,ii * 100 );
// 		while((ii--)&&(*flag))
// 			ql_rtos_task_sleep_ms(100);
// 	} else if(mqtt_con_nums < 7) {
// 		iresult = pow(2,mqtt_con_nums);
// 		ii=iresult*10;
//         ALIOT_PRINT( "mqtt con_num=%d, reconnect after %dms...\n", mqtt_con_nums,ii * 100 );
// 		while((ii--)&&(*flag))
// 			ql_rtos_task_sleep_ms(100);
// 	} else if(mqtt_con_nums == 7) {
// 		mqtt_con_nums = 0;
//         ALIOT_PRINT( "mqtt con_num=%d, reconnect after %dms...\n", mqtt_con_nums,ii * 100 );
// 	}
	if (mqtt_con_nums == 3 || mqtt_con_nums == 5 || mqtt_con_nums == 6) {
		// disp_set_service_connect_error( 1 );
		// disp_sleep_enable( 0 );
		SLEEP_S(1);
		mqtt_con_nums = 0;
//		tts_play_set( AudioFuwulianjieshibai, AudioFuwulianjieshibaiLen, FixAudioTypeDef );
		// tts_play_set_idx(AUD_ID_MQTT_CONNECT_FAIL,0,0);
		// ALIOT_PRINT("mqtt_reconnect send tts play, line %d\n", __LINE__);
	}
	// tts_play_set_idx(AUD_ID_MQTT_CONNECT_FAIL,0,0);
	// ALIOT_PRINT("mqtt_reconnect send tts play, line %d\n", __LINE__);
}

#define MAX_MQTT_BUFF_SIZE		(0)

unsigned char sendbuf[MAX_MQTT_BUFF_SIZE];
unsigned char readbuf[MAX_MQTT_BUFF_SIZE];

char topic_sub_message[100] = { 0 };
char topic_sub_inform[100] = { 0 };
char topic_sub_upgrade[100] = { 0 };


void otaUpdateArrived(char *pszTopic, uchar *pbyRecv, uint wRecvLen)
{
#if 0
	cJSON *root=NULL, *item_data=NULL,
		*item_size, *item_version, *item_url, *item_signMethod, *item_sign;
	uint32_t size = 0;
	char *version_buff = 0, *url_buff = 0, *sign_buff = 0;
	ota_params *params = 0;
	
	ALIOT_PRINT("OTA Message arrived: %.*s: %.*s\n", 
				strlen(pszTopic), pszTopic,
				wRecvLen, (char *)pbyRecv);

	if ((TermInfo.LowBat)&&(TermInfo.Charge==0))
	{
		return; // Low battery and not charging, no upgrade
	}
	
	root = cJSON_Parse((const char *)pbyRecv);
	if (!root)
	{
		ALIOT_PRINT("OTA parse message failed\n");
		goto OTA_ERR;
	}
	item_data = cJSON_GetObjectItem(root, "data");
	if (!item_data)
	{
		ALIOT_PRINT("OTA not found data item\n");
		goto OTA_ERR;
	}
	//size
	item_size = cJSON_GetObjectItem(item_data, "size");
	if (!item_size)
	{
		ALIOT_PRINT("OTA not found size item\n");
		goto OTA_ERR;
	}
	size = item_size->valueint;
//	if (0)//size > PRJCONF_IMG_MAX_SIZE)
//	{
//		ALIOT_PRINT("OTA bin is too large, curr %d, max %d\n", size, PRJCONF_IMG_MAX_SIZE);
//		goto OTA_ERR;
//	}

	//version
	item_version = cJSON_GetObjectItem(item_data, "version");
	if (!item_version)
	{
		ALIOT_PRINT("OTA not found version item\n");
		goto OTA_ERR;
	}
	version_buff = YMI_SysMalloc(strlen(item_version->valuestring) + 1);
	if (!version_buff)
	{
		ALIOT_PRINT("OTA allocate %d bytes for version failed\n", strlen(item_version->valuestring));
		goto OTA_ERR;
	}
	strcpy(version_buff, item_version->valuestring);
	//url
	item_url = cJSON_GetObjectItem(item_data, "url");
	if (!item_url)
	{
		ALIOT_PRINT("OTA not found url item\n");
		goto OTA_ERR;
	}
	url_buff = YMI_SysMalloc(strlen(item_url->valuestring) + 1);
	if (!url_buff)
	{
		ALIOT_PRINT("OTA allocate %d bytes for url failed\n", strlen(item_url->valuestring));
		goto OTA_ERR;
	}
	strcpy(url_buff, item_url->valuestring);

	//signMethod
	item_signMethod = cJSON_GetObjectItem(item_data, "signMethod");
	if (!item_signMethod)
	{
		ALIOT_PRINT("OTA not found signMethod item\n");
		goto OTA_ERR;
	}

	//sign
	item_sign = cJSON_GetObjectItem(item_data, "sign");
	if (!item_sign)
	{
		ALIOT_PRINT("OTA not found sign item\n");
		goto OTA_ERR;
	}
	sign_buff = YMI_SysMalloc(strlen(item_sign->valuestring) + 1);
	if (!sign_buff)
	{
		ALIOT_PRINT("OTA allocate %d bytes for sign failed\n", strlen(item_sign->valuestring));
		goto OTA_ERR;
	}
	strcpy(sign_buff, item_sign->valuestring);
	
	ALIOT_PRINT("OTA new firmware\n");
	ALIOT_PRINT("------------------------------\n");
	ALIOT_PRINT("\tsize: %d\n", size);
	ALIOT_PRINT("\tversion: %s\n", version_buff);
	ALIOT_PRINT("\turl: %s\n", url_buff);
	ALIOT_PRINT("\tsignMethod: %s\n", item_signMethod->valuestring);
	ALIOT_PRINT("\tsign: %s\n", sign_buff);
	ALIOT_PRINT("------------------------------\n");

	if (strcmp(version_buff, firmware_version) == 0)
	{
		ALIOT_PRINT("OTA version is same with current version, skip...\n");
		goto OTA_ERR;
	}

	params = YMI_SysMalloc(sizeof(ota_params));
	memset(params, 0, sizeof(ota_params));
	params->size = size;
	params->version = version_buff;
	params->url = url_buff;
	if (strcmp("Md5", item_signMethod->valuestring) == 0)
	{
		params->verify = OTA_VERIFY_MD5;
		params->verify_value = sign_buff;
	}
	else if (strcmp("SHA256", item_signMethod->valuestring) == 0)
	{
		params->verify = OTA_VERIFY_SHA256;
		params->verify_value = sign_buff;
	}
	else
	{
		params->verify = OTA_VERIFY_NONE;
		params->verify_value = NULL;
		if (sign_buff)
		{
			FREE(sign_buff);
			sign_buff = NULL;
		}
	}
//	params->net_mode = OTA_PROTOCOL_HTTP;

	// if (OtaDown(params) != RET_SUCC) {
	// 	ALIOT_PRINT("OTA task create failed\n");
	// 	if (url_buff)
	// 	{
	// 		FREE(url_buff);
	// 		url_buff = NULL;
	// 	}
	// 	goto OTA_ERR;
	// }
	ALIOT_PRINT("OTA start, wait...\n");
	goto OTA_EXIT;

OTA_ERR:
	if (version_buff)
	{
		FREE(version_buff);
	}
	if (url_buff)
	{
		FREE(url_buff);
	}
	if (sign_buff)
	{
		FREE(sign_buff);
	}
	if (params)
	{
		FREE(params);
	}
OTA_EXIT:
	if (root)
	{
		cJSON_Delete(root);
	}
#endif
}

void default_msgArrive(MessageData* data)
{
	ALIOT_PRINT("Message arrived on topic : %.*s: %d\n",
				data->topicName->lenstring.len,
				data->topicName->lenstring.data,
				data->message->payloadlen);
	if(strstr(data->topicName->lenstring.data, topic_sub_upgrade))
		otaUpdateArrived(data->topicName->lenstring.data, data->message->payload, data->message->payloadlen);
	else if(strstr(data->topicName->lenstring.data, topic_sub_message))
		messageArrived(data->topicName->lenstring.data, data->message->payload, data->message->payloadlen);
}


int ParamsPack(void *pvInBuff, void *pvOutBuff)
{
    int iRet = YMI_ERR;
    ST_MyJson *pstMyJson = NULL;

    ST_JsonPackInfo astPackInfo[] = {
        {"version", MYJSON_STRING, NULL, firmware_version, NULL},
        {NULL, MYJSON_STRING, NULL, NULL, NULL},
    };

    iRet = MyJsonPack(astPackInfo, NULL, &pstMyJson, NULL, 0);
    if (iRet != RET_SUCC)
    {
        return RET_FAIL;
    }
	*(ST_MyJson **)pvOutBuff = pstMyJson;
    return RET_SUCC;
}

int PubTermInfo(char *pszOut)
{
	int iRet;
	ST_MyJson *root = NULL;
	ST_MyJson *params = NULL;
	char szID[4] = "1";


    ST_JsonPackInfo astPackInfo[] = {
        {"id", MYJSON_STRING, NULL, szID, NULL},
        {"params", MYJSON_OBJUECT, NULL, &params, ParamsPack},
        {NULL, MYJSON_STRING, NULL, NULL, NULL},
    };

    iRet = MyJsonPack(astPackInfo, NULL, &root, pszOut, 64);
    if (iRet != RET_SUCC)
    {
        ALIOT_PRINT("json pack err %d,%x", iRet, iRet);
        return RET_FAIL;
    }

	return RET_SUCC;
}

void mqtt_work_set(void * argv)
{
	while (!g_iNetOnline)
	{
		ALIOT_PRINT("wait net online");
		SLEEP_S(10);
	}
	TermInfo.NetStatBak = NET_DEVICE_STATE_CONNECTED;
    TermInfo.MqttIsRuning = 1;
    TermInfo.ServiceOnline = 0;

	int rc = 0;
	MQTTPacket_connectData connectData;// = MQTTPacket_connectData_initializer;
	
	ALIOT_PRINT("func %s line %d\n", __func__, __LINE__);
	mqtt_client_close(&ysw1_mqtt_client);
	memset(&ysw1_mqtt_client,0,sizeof(ysw1_mqtt_client));
	
    sprintf(topic_sub_message, "%s", "vbox/JVB2080B/RDPVBJJ10000010/404940968716250/vbPaymentAlert");   
	sprintf(topic_sub_upgrade, "/ota/device/upgrade/%s/%s", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
	sprintf(topic_sub_inform, "/ota/device/inform/%s/%s", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
	//sprintf(topic_sub_rrpc, "/sys/%s/%s/rrpc/request/+", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
#ifdef AWS_SERVER
	sprintf(topic_sub_message, "%s", "ym_dev");
#endif
	mqtt_ctrl = 1;
	cal_set.mqtt_con = MQTT_CACK;
	cal_set.mqtt_sub = MQTT_CACK;
	cal_set.mqtt_pub = MQTT_CACK;
    // mqtt_first_connet = 0;

	alink_devc_init();
	mqtt_para_init();
	
	#ifdef USE_HTTPS_GET_PARA
	//rc = alink_https_set_device_info(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, DEV_CLIENT_ID);
	rc = alink_https_set_device_info(SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName, SysInfoGet()->szDeviceSecret, DEV_CLIENT_ID);
	if(rc == -1) {
		ALIOT_DEBUG("alink set device para error!\n");
		goto mqtt_exit;
	}
	#else
	//aliot_device_info(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, DEV_CLIENT_ID, SECURE_MODE);
	//if( 0 == memcmp(SysInfoGet()->szServerUrl,"iot-as-mqtt",strlen("iot-as-mqtt"))) 
	if( strlen(SysInfoGet()->szServerMqtt) )
	{
	    // A new version of the instance
	    aliot_device_info_ex(SysInfoGet()->szServerMqtt,SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName, SysInfoGet()->szDeviceSecret, DEV_CLIENT_ID, SECURE_MODE);
	}
	else
	{
	    // Legacy instances
	    aliot_device_info(SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName, SysInfoGet()->szDeviceSecret, DEV_CLIENT_ID, SECURE_MODE);
	}
	#endif
	/* connect para */
	connectData.MQTTVersion 		= 4;
	//The following time settings have a significant impact on standby power consumption, please set the actual situation of MQTT background, and the MQTT library processes heartbeats according to the set real time by default.
	connectData.keepAliveInterval 	= (5*60); //For Alibaba Cloud, the heartbeat occurs once every 5 minutes to reduce standby power consumption
	connectData.cleansession		= 0;
	// connectData.clientID.cstring 	= "TEST_CLIENT_10003";
    connectData.clientID.cstring 	= xr_devc_info.client_id;
	connectData.username.cstring 	= xr_devc_info.user_name;
   	connectData.password.cstring 	= xr_devc_info.password;

	/* mqtt para */
	xr_mqtt_para.command_timeout_ms = 3000; //MQTT timeout period 
	xr_mqtt_para.read_buf = readbuf;
	xr_mqtt_para.send_buf = sendbuf;
	xr_mqtt_para.read_buf_size = MAX_MQTT_BUFF_SIZE;
	xr_mqtt_para.send_buf_size = MAX_MQTT_BUFF_SIZE;
	xr_mqtt_para.port	= 1887;
	// strncpy(xr_mqtt_para.host_name, xr_devc_info.host_name, strlen(xr_devc_info.host_name));
	strncpy(xr_mqtt_para.host_name, "sit-vbdsm.jio.com", sizeof(xr_mqtt_para.host_name) - 1);
	// ql_rtos_task_sleep_s(3);
	// SLEEP_S(3);


	while(mqtt_ctrl)
	{
		//ALIOT_PRINT("while(mqtt_ctrl)\n");
		if (ysw1_mqtt_client.isDiscon)
		{
			SLEEP_S(5);
		}
		if(cal_set.mqtt_con == MQTT_CACK)
		{
		    TermInfo.ServiceOnline = 0;
		    lpm_set(LPM_LOCK_MQTT,1);
		    if ( TermInfo.NetStatBak != NET_DEVICE_STATE_CONNECTED)
		    {
		        // If the service connection fails due to network reasons, the service is logged out
                ALIOT_PRINT( "%s: network disconnect,end task...\n", __func__ );
		        mqtt_ctrl = 0;
		        //ql_rtos_task_sleep_ms(2000);
		        break;
		    }

		    // Reconnection Restrictions
		    mqtt_reconnect( &mqtt_ctrl );

			if(0 == memcmp(SECURE_MODE, MQTT_TCP_MODE, 1))
				rc = tcp_mqtt_client(&ysw1_mqtt_client);
			if(0 == memcmp(SECURE_MODE, MQTT_TLS_MODE, 1))
			{
				ALIOT_PRINT("before ssl_mqtt_client, line %d\n", __LINE__);
#ifdef AWS_SERVER
				rc = ssl_mqtt_client(&ysw1_mqtt_client, aws_ca_crt, strlen(aws_ca_crt), 
					aws_client_crt, strlen(aws_client_crt), aws_client_key_crt, strlen(aws_client_key_crt));
#else
				rc = ssl_mqtt_client(&ysw1_mqtt_client, ali_ca_cert, strlen(ali_ca_cert), NULL, 0, NULL, 0);
#endif
				ALIOT_PRINT("after ssl_mqtt_client, line %d\n", __LINE__);
			}
			ysw1_mqtt_client.defaultMessageHandler=default_msgArrive;

			if ( rc == 0 )
            {
				connectData.username.cstring 	= "328770197068";// "kishor.manik";// userId;//"kishor.manik";//xr_devc_info.user_name;
   				connectData.password.cstring 	= "";//"Jio@2023"; //AuthParamObj.appIdentifierTokenValue;//"Jio@2023";//xr_devc_info.password;
                rc = MQTTConnect( &ysw1_mqtt_client, &connectData );
                if ( rc == 0 )
                {
                    ALIOT_PRINT( "%s: MQTT Connected\n",__func__ );
                    cal_set.mqtt_con = MQTT_DICACK;
                    continue;
                }
                else
                {
                    ALIOT_PRINT( "MQTT client connect error! Return error code is %d\n", rc );
                }
            }
			else
			{
			    ALIOT_PRINT( "MQTT client init error! Return error code is %d\n", rc );
			}

            continue;
		}

		if(cal_set.mqtt_sub == MQTT_CACK) {
			//subscribe OTA notify start
			// ALIOT_PRINT("%s,line %d,sub topic :%s \n",__func__, __LINE__,topic_sub_upgrade);
			// rc = MQTTSubscribe(&ysw1_mqtt_client, topic_sub_upgrade, XR_MQTT_QOS1, otaUpdateArrived);
			// if (rc != 0) 
			// {
			// 	ALIOT_PRINT("Return code from MQTT subscribe(OTA) is %d\n", rc);
			// 	cal_set.mqtt_con = MQTT_CACK;
			// 	continue;
			// }
			// ALIOT_PRINT("%s,line %d,sub topic :%s \n",__func__, __LINE__,topic_sub_message);
			//subscribe OTA notify end
			rc = MQTTSubscribe(&ysw1_mqtt_client, topic_sub_message, QOS1, messageArrived);
			
			if (rc != 0) {
				ALIOT_PRINT("Return code from MQTT subscribe is %d\n", rc);
				cal_set.mqtt_con = MQTT_CACK;
				continue;
			}

			ALIOT_PRINT("%s: sub topic %s success\n",__func__,topic_sub_message);
			cal_set.mqtt_sub = MQTT_DICACK;
		}

        if(cal_set.mqtt_pub == MQTT_CACK) {
			//update version for OTA start
			// char *buff;
			// buff=PubTermInfo();
			// if (buff==NULL) continue;
			// ALIOT_PRINT("OTA infrom:\n---------------------\n%s\n---------------------\n", buff);
			
			// MQTTMessage ota_inform;
			// ota_inform.qos = XR_MQTT_QOS1;
			// ota_inform.retained = 0;
			// ota_inform.payload = buff;
			// ota_inform.payloadlen = strlen(buff);
			
			rc = MQTTPublish(&ysw1_mqtt_client, topic_sub_inform, PubTermInfo);
			if (rc != 0) {
				ALIOT_PRINT("Return code from MQTT publish is %d\n", rc);
				cal_set.mqtt_con = MQTT_CACK;
				cal_set.mqtt_sub = MQTT_CACK;
				// free(buff);
				continue;
			}
			// free(buff);
			ALIOT_PRINT("Inform version for OTA success\n");
			//update version for OTA end

			ALIOT_PRINT("MQTT publish is success\n");
			cal_set.mqtt_pub = MQTT_DICACK;
			TermInfo.ServiceOnline = 1;

            // disp_onoff_request( 1, DISP_HOLDON_MS );
            // ql_rtos_task_sleep_ms( 50 );
            // disp_set_service_connect_error( 0 );
            // disp_sleep_enable( 1 );

            // if (mqtt_first_connet == 0)
            // {
                //tts_play_set(AudioFuwljcg,AudioFuwljcgLen,FixAudioTypeDef);
                // tts_play_set_idx(AUD_ID_MQTT_CONNECT_SUCESS,0,0);
            //     mqtt_first_connet = 1;
            // }
            lpm_set(LPM_LOCK_MQTT,0);
		}	


		rc = MQTTYield(&ysw1_mqtt_client, 3000);
		if (rc != 0)
		{
			ALIOT_PRINT("Return code from yield is %d\n", rc);
			rc = MQTTDisconnect(&ysw1_mqtt_client);
			if (rc != 0)
			{
				ALIOT_PRINT("Return code from MQTT disconnect is %d\n", rc);
			}
			else
			{
				ALIOT_PRINT("MQTT disconnect is success\n");

			}

			cal_set.mqtt_con = MQTT_CACK;
			cal_set.mqtt_sub = MQTT_CACK;
			cal_set.mqtt_pub = MQTT_CACK;

#if 0
			// Reconnect first, and then prompt if the reconnection fails
			disp_set_service_connect_error(1);
			disp_sleep_enable(0);
			ql_rtos_task_sleep_ms(50);

			//timer_offline_notify = ql_rtos_get_systicks_to_ms( ) + 60 * 1000;
            //tts_play_set( AudioFuwulianjieshibai, AudioFuwulianjieshibaiLen, FixAudioTypeDef );
#endif
			// Prevent reconnecting too often in some cases
			// ql_rtos_task_sleep_ms(3000);
			SLEEP_S(3);
			continue;
		}

		mqtt_con_nums = 0;
		if( 0 == check_mqtt_server(&ysw1_mqtt_client) )
		{
#if 0
		    // Reconnect first, and then prompt if the reconnection fails
            disp_set_service_connect_error( 1 );
            disp_sleep_enable( 0 );
            ql_rtos_task_sleep_ms(50);

            //timer_offline_notify = ql_rtos_get_systicks_to_ms( ) + 60 * 1000;
            //tts_play_set( AudioFuwulianjieshibai, AudioFuwulianjieshibaiLen, FixAudioTypeDef );
#endif
		}
//		testmess();
//		testota();
	}
	ALIOT_PRINT("mqtt_exit line %d\n", __LINE__);
	TermInfo.ServiceOnline = 0;
	
	//MQTTUnsubscribe(&ysw1_mqtt_client, topic_sub_upgrade);
	//MQTTUnsubscribe(&ysw1_mqtt_client, topic_sub_message);

	rc = MQTTDisconnect(&ysw1_mqtt_client);
	if(rc != 0)
	{
		ALIOT_PRINT("Return code from MQTT disconnect is %d\n", rc);
	}
	else
	{
		ALIOT_PRINT("MQTT disconnect is success\n");
	}

	mqtt_client_close(&ysw1_mqtt_client);
	ALIOT_PRINT("mqtt_work_set end\n");
	lpm_set(LPM_LOCK_MQTT,1);
	// disp_sleep_enable(0);
	TermInfo.MqttIsRuning = 0;
	// ql_rtos_task_delete(NULL);
	YMI_MQTTExit(s_tMQTTTaskID);
	s_tMQTTTaskID = 0;
	FREE(s_pstConf);
	YMI_TaskDelete(mqtt_task_ctrl_thread);
}

int MqttExit(void)
{
	int ii;
	mqtt_ctrl=0;
	ALIOT_PRINT("%s: wait mqtt exit...\n",__func__);
	for(ii=0;ii<100;ii++)
	{
		if (!TermInfo.MqttIsRuning ) return 0;
		SLEEP_MS(100);
	}
	return -1;
}
void start_Mqtt_task(void)
{
	if (TermInfo.MqttIsRuning) 
    {
        ALIOT_PRINT("%s: mqtt is running or exiting,skip\n",__func__);
        return;
    }
	
	if (YMI_TaskCreat(&mqtt_task_ctrl_thread, "mqtt_work_set", 1024, mqtt_work_set, NULL, EM_TASK_PRIORITY_NORMAL, 5) != RET_SUCC)
	{
		ALIOT_PRINT("thread create error\n");
	}
}

