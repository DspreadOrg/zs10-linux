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

#define MQTT_DUAL_TEST

#define ALI_MQTT_URL "iot-as-mqtt.cn-shanghai.aliyuncs.com"
//message for aliyun,define product key, device name, devcie secret
#define DEV_CLIENT_ID			"00001"

//define secure mode ;  2 : tls connect mode, 3: tcp stright connect mode
#define MQTT_TCP_MODE			"3"
#define MQTT_TLS_MODE			"2"
#define SECURE_MODE_A			MQTT_TLS_MODE
#define SECURE_MODE_B			MQTT_TLS_MODE

extern int g_iNetOnline;

char mqtt_ctrlA = 0;
char mqtt_ctrlB = 0;

typedef struct {
	mqt_cal cal_set;
	uint8_t iDisConnect;
	uint8_t mqtt_con_nums;
	uint8_t mqtt_con_rsp_times;
	Client ysw1_mqtt_client;
	aliot_devc_info_t xr_devc_info;
	xr_mqtt_param_t	xr_mqtt_para;
	char * ExitFlag;
	int iMqttType;//0-A,1-B
} reconnectpara;

static reconnectpara s_stParaA = {0};
static reconnectpara s_stParaB = {0};

// unsigned char mqtt_first_connet = 0;

#define OTA_THREAD_STACK_SIZE		(5 * 1024)

extern char firmware_version[50];
extern char YSW1_messageId[50];
extern char YSW1_notifyUrl[200];
const char *ali_ca_cert = \
{
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIGnTCCBYWgAwIBAgIIWtDgQMbPS7swDQYJKoZIhvcNAQELBQAwgbQxCzAJBgNV\r\n" \
	"BAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQHEwpTY290dHNkYWxlMRow\r\n" \
	"GAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjEtMCsGA1UECxMkaHR0cDovL2NlcnRz\r\n" \
	"LmdvZGFkZHkuY29tL3JlcG9zaXRvcnkvMTMwMQYDVQQDEypHbyBEYWRkeSBTZWN1\r\n" \
	"cmUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IC0gRzIwHhcNMjMwNDI4MTE1ODQyWhcN\r\n" \
	"MjQwNDI4MTE1ODQyWjAcMRowGAYDVQQDExFzaXQtdmJkc20uamlvLmNvbTCCASIw\r\n" \
	"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMS4diQpNfFZXQ3dhk1pi4GjPWJq\r\n" \
	"HbYBPWhxKMZnslPdqQbf2I++o3GY+ourDT18EMMtOnMTRnWYsrsB+6S/fQHymEvH\r\n" \
	"EbqxCclGHxWL7jMSeXuL2tMjhr6ee3D9uMerZFJomMAvGgmUqJTzEgELVaPz6Zix\r\n" \
	"UTJbt4d55PWWjoTdAl4UpVJDh4fUP+qxSdFHvTPCw8seZ4s6LGMIWOtu/gcwYdSg\r\n" \
	"5YfZ9sxnaGmkU4xxYY42dZd2z3D+4gQ8j1dATU4o7RkFWeA6bwAFagztqqxASY4U\r\n" \
	"Pg2qmgdmcUkChYqV2uNNoq7uNfJ54n0cKctHl7uyPX4OZUJ03YesiZ+r4qMCAwEA\r\n" \
	"AaOCA0gwggNEMAwGA1UdEwEB/wQCMAAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsG\r\n" \
	"AQUFBwMCMA4GA1UdDwEB/wQEAwIFoDA4BgNVHR8EMTAvMC2gK6AphidodHRwOi8v\r\n" \
	"Y3JsLmdvZGFkZHkuY29tL2dkaWcyczEtNTU3Mi5jcmwwXQYDVR0gBFYwVDBIBgtg\r\n" \
	"hkgBhv1tAQcXATA5MDcGCCsGAQUFBwIBFitodHRwOi8vY2VydGlmaWNhdGVzLmdv\r\n" \
	"ZGFkZHkuY29tL3JlcG9zaXRvcnkvMAgGBmeBDAECATB2BggrBgEFBQcBAQRqMGgw\r\n" \
	"JAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmdvZGFkZHkuY29tLzBABggrBgEFBQcw\r\n" \
	"AoY0aHR0cDovL2NlcnRpZmljYXRlcy5nb2RhZGR5LmNvbS9yZXBvc2l0b3J5L2dk\r\n" \
	"aWcyLmNydDAfBgNVHSMEGDAWgBRAwr0njsw0gzCiM9f7bLPwtCyAzjAzBgNVHREE\r\n" \
	"LDAqghFzaXQtdmJkc20uamlvLmNvbYIVd3d3LnNpdC12YmRzbS5qaW8uY29tMB0G\r\n" \
	"A1UdDgQWBBQ/mcTZFA4GRFT4ddYIT2JKbKdYwDCCAX0GCisGAQQB1nkCBAIEggFt\r\n" \
	"BIIBaQFnAHUA7s3QZNXbGs7FXLedtM0TojKHRny87N7DUUhZRnEftZsAAAGHx7rU\r\n" \
	"HgAABAMARjBEAiBVHgoqaAixM4BFpFKcJWqCfUE65WiOLQ34qzUiugrNpAIgPQCz\r\n" \
	"DZDRDcITzI5Cq0caepEtFP6Be+8F5G6cDbUmlewAdgBIsONr2qZHNA/lagL6nTDr\r\n" \
	"HFIBy1bdLIHZu7+rOdiEcwAAAYfHutTiAAAEAwBHMEUCICDaNFuCZz26t3GG3Nm0\r\n" \
	"8xPLUAKnqwGyPTJEqupTMz9mAiEAhYjmN6O/UeIy6YCmv8tRawfFk1jeAZJJP5jH\r\n" \
	"LASQ/BYAdgDatr9rP7W2Ip+bwrtca+hwkXFsu1GEhTS9pD0wSNf7qwAAAYfHutVD\r\n" \
	"AAAEAwBHMEUCIQCKpQ//qgTE3+IDrX6dC91TWk/5JWHbN4A3BsJ6QtMr6gIgDLlQ\r\n" \
	"TWhTfIPntEbi4I3mGdT3kOisR5+d4gnv+reCEy4wDQYJKoZIhvcNAQELBQADggEB\r\n" \
	"AKvbZwPP+kADVH2e6jn5nz3FvCeE+h8SXgT/liCLvChjTO19MUxtdtXfYg1Ttxd/\r\n" \
	"r7AsDVEwDjljTbW0m6XKp9riorWh8bYEW6MdCwCRuyj6HcFsXgKTLv1REUf234ID\r\n" \
	"4VYNIsq/nOvkfcRRfBxD4EQnm8MGkm8Sk4WDJWhZVdkE7/5/r7fvCsoztZjTyRKM\r\n" \
	"8wwzRjr/h+gGDFH4XlqJNa0I2MgpQJgqeSc64rUOZyhx8BsYlMd6YZXwjK5bd/tS\r\n" \
	"x4/ckMPcZh0WGL9ftxYHImVvmWHJ4pHDueEzR1pr/RXf06uUciYdra3o2Tw20uRh\r\n" \
	"sEvjQzVdabJaOFcRuejpjSE=\r\n"	\
    "-----END CERTIFICATE-----"
};

extern void otaUpdateArrived(char *pszTopic, uchar *pbyRecv, uint wRecvLen);



void otaUpdateArrivedA(char *pszTopic, uchar *pbyRecv, uint wRecvLen)
{
	ALIOT_PRINT("======%s_%d======\n", __func__, __LINE__);
	otaUpdateArrived(pszTopic, pbyRecv, wRecvLen);
}

void otaUpdateArrivedB(char *pszTopic, uchar *pbyRecv, uint wRecvLen)
{
	ALIOT_PRINT("======%s_%d======\n", __func__, __LINE__);
	otaUpdateArrived(pszTopic, pbyRecv, wRecvLen);
}

void _messageArrived(char *pszTopic, uchar *pbyRecv, uint wRecvLen)
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
#endif

	// if(DeviveMessageAnaly(data->message->payload,data->message->payloadlen) == 0)
	// {	
	// 	;
	// }
}

#if 0
void CjsonTest(uchar *pbyRecv)
{
	cJSON *pstCjson = NULL, *pstCjsonSub = NULL;
	char *p = NULL;
	int iNum = 54;

	ALIOT_PRINT("cJSON_CreateObject start!");
	// pstCjson = cJSON_Parse((const char *)pbyRecv);
	if (pstCjson)
	{
		ALIOT_PRINT("cJSON_free start!");
		// pstCjsonSub = cJSON_GetObjectItem(pstCjson, "cjson");
		if ((!pstCjsonSub) || (pstCjsonSub->type != cJSON_String))
		{
			ALIOT_PRINT("\"cjson\" item get fail!");
		}
		else
		{
			ALIOT_PRINT("\"cjson\" item get str = %s!", pstCjsonSub->valuestring);
		}
		// cJSON_Delete(pstCjson);
		pstCjson = NULL;
		ALIOT_PRINT("cJSON_free done!");
	}
	
	ALIOT_PRINT("cJSON_CreateObject start!");
	// pstCjson = cJSON_CreateObject();
	if (pstCjson == NULL)
	{
		ALIOT_PRINT("[ERR]cjson create!");
		return ;
	}
	
	// cJSON_AddItemToObject(pstCjson, "sn", cJSON_CreateString("QR523"));
	// cJSON_AddStringToObject(pstCjson, "test", "xxxxx");
	// cJSON_AddNumberToObject(pstCjson, "num", iNum);

	// p = cJSON_PrintUnformatted(pstCjson);
	if (p != NULL)
	{
		ALIOT_PRINT("json str = %s", p);
		FREE(p);
		p = NULL;
	}
	// cJSON_Delete(pstCjson);
	pstCjson = NULL;
}
#endif

void messageArrivedA(char *pszTopic, uchar *pbyRecv, uint wRecvLen)
{
	ALIOT_PRINT("======%s_%d======\n", __func__, __LINE__);
	_messageArrived(pszTopic, pbyRecv, wRecvLen);
}

void messageArrivedB(char *pszTopic, uchar *pbyRecv, uint wRecvLen)
{
	ALIOT_PRINT("======%s_%d======\n", __func__, __LINE__);
	_messageArrived(pszTopic, pbyRecv, wRecvLen);
}

#if 0
const char * messrec[]=
{
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B01\",\"money\":1.00,\"voiceMsg\":\"" PAY_MSG_LKLSK "1.00" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B02\",\"money\":1.01,\"voiceMsg\":\"" PAY_MSG_LKLDZ  "1.01" PAY_MSG_YUAN "\",\"notifyUrl\":\"\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B03\",\"money\":15.00,\"voiceMsg\":\"" PAY_MSG_ZFBDZ "15.00" PAY_MSG_YUAN "\",\"notifyUrl\":\"\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B04\",\"money\":123.1,\"voiceMsg\":\"" PAY_MSG_WXDZ "123.1" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B05\",\"money\":215.13,\"voiceMsg\":\"" PAY_MSG_YSFDZ "215.13" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B06\",\"money\":10006.87,\"voiceMsg\":\"" PAY_MSG_LKLDZ  "10006.87" PAY_MSG_YUAN "\",\"notifyUrl\":\"\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B07\",\"money\":138,\"voiceMsg\":\"" PAY_MSG_ZFBDZ "138" PAY_MSG_YUAN "\",\"notifyUrl\":\"\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B08\",\"money\":15,\"voiceMsg\":\"" PAY_MSG_WXDZ "15" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
"{\"messageId\":\"F037EE67A71C463386BBADE18CF16B09\",\"money\":10000,\"voiceMsg\":\"" PAY_MSG_YSFDZ "10000" PAY_MSG_YUAN "\",\"notifyUrl\":\"http://180.167.225.202:29090/shanghutong/api/qr/voice/notify\"}",
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
			messageArrivedA(&aa);
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



#define MQTT_THREAD_STACK_SIZE           (1024 * 5)
#define MQTT_DISCONNECT_MAX_TIMES        40 //20
#define MQTT_YIELD_FAILED_MAX_TIMES      50
ql_task_t mqtt_task_ctrl_threadA = NULL;
ql_task_t mqtt_task_ctrl_threadB = NULL;

//uint8_t mqtt_con_nums = 0;
//uint8_t para->mqtt_con_rsp_times = 0;

/* 0: disconnet   1: connected   -1 : error*/
int check_mqtt_server(reconnectpara * para)
{
	int ret = -1;
	char out_stand = 0;

	if (para == NULL)
	{
		return -1;
	}
    
	out_stand = para->ysw1_mqtt_client.ping_outstanding;
	ALIOT_PRINT("%s mqtt_con_rsp_times: %d,out_stand %d\n",__func__ ,para->mqtt_con_rsp_times,out_stand);
	if(out_stand == 1) {
		para->mqtt_con_rsp_times ++;
	}
	else {
		para->mqtt_con_rsp_times = 0;
	}

    
	//if ((mqtt_con_rsp_times > 20) || (mqtt_disconnect_times > 1000*MQTT_DISCONNECT_MAX_TIMES)) // test code
	if (para->mqtt_con_rsp_times > 20) // test code
	{
		ALIOT_PRINT("%s mqtt_con_rsp_times = %d,  mqtt disconnect\n",__func__, para->mqtt_con_rsp_times);
		para->mqtt_con_rsp_times = 0;
		//c->mqtt_con_ping_cnt = 0; // test code
		para->cal_set.mqtt_con = MQTT_CACK;
		para->cal_set.mqtt_sub = MQTT_CACK;
		para->cal_set.mqtt_pub = MQTT_CACK;
		ret = 0;
	}
	else
	{
		ret = 1;
	}

	return ret;
}

#endif
static void mqtt_reconnect(reconnectpara * para )
{
	int iresult=1;
		
	para->mqtt_con_nums ++;
    
	if(para->mqtt_con_nums == 1) {
		// ii = para->mqtt_con_nums*10;
		// while((ii--)&&(*para->ExitFlag))
		// 	ql_rtos_task_sleep_ms(100);
		ALIOT_PRINT("mqtt_reconnect--mqtt_con_nums==1[%d]\n", para->mqtt_con_nums);
	} else if(para->mqtt_con_nums < 7) {
		// iresult = pow(2,para->mqtt_con_nums);
		// ii=iresult*10;
		// while((ii--)&&(*para->ExitFlag))
		// 	ql_rtos_task_sleep_ms(100);
		ALIOT_PRINT("mqtt_reconnect--mqtt_con_nums<2-6>[%d|%d]\n", para->mqtt_con_nums, iresult);
	} else if(para->mqtt_con_nums == 7) {
		para->mqtt_con_nums = 0;
		ALIOT_PRINT("mqtt_reconnect--mqtt_con_nums==7[%d]\n", para->mqtt_con_nums);
	}
}


static ST_MQTTConf *s_pstConfA = NULL;
static ST_MQTTConf *s_pstConfB = NULL;
static T_Task s_tMQTTTaskIDA = 0;
static T_Task s_tMQTTTaskIDB = 0;
static T_Task mqtt_task_ctrl_threadA = 0;
static T_Task mqtt_task_ctrl_threadB = 0;


void aliot_device_info_mulx(
        const char *product_key,
        const char *device_name,
        const char *device_secret,
        const char *dev_client_id,
        const char *secure_mode,
		reconnectpara *pstPara
        )
{
	ST_MQTTConf *pstConf = NULL;

	if(pstPara->iMqttType == 0)
	{
		if(s_pstConfA == NULL)
		{
			s_pstConfA = MALLOC(1, ST_MQTTConf);
			if(s_pstConfA == NULL)
			{
				ALIOT_PRINT("[ERR]YMI_SysMalloc MQTT_INFO_T");
				return ;
			}
			memset(s_pstConfA, 0x00, sizeof(ST_MQTTConf));
		}
		pstConf = s_pstConfA;
	}
	else
	{
		if(s_pstConfB == NULL)
		{
			s_pstConfB = MALLOC(1, ST_MQTTConf);
			if(s_pstConfB == NULL)
			{
				ALIOT_PRINT("[ERR]YMI_SysMalloc MQTT_INFO_T");
				return ;
			}
			memset(s_pstConfB, 0x00, sizeof(ST_MQTTConf));
		}
		pstConf = s_pstConfB;
	}
	
	memset(pstConf, 0x00, sizeof(ST_MQTTConf));
    pstConf->eSocketType = (SysInfoGet()->iNetChanlLTE == WIFI_MODE ? SOCKET_WIFI_TCP_CLIENT : SOCKET_TCP_CLIENT);
    pstConf->iConnectTimeOut = 5000;
	if(memcmp(secure_mode, MQTT_TLS_MODE, 1) == 0)
	{
		pstConf->eConType = MQTT_CON_TYPE_TLS;
	}
	else
	{
		pstConf->eConType = MQTT_CON_TYPE_TCP;
	}
    pstConf->iHeartBeatIntervalTime = 300 * 1000; 
    pstConf->iHeartBeatLostMaxTime = 1; 

    sprintf(pstConf->szDeviceName, "%s", device_name);
    sprintf(pstConf->szDeviceSecret, "%s",device_secret);
    sprintf(pstConf->szProductKey, "%s", product_key);
	sprintf(pstConf->szClientId, "%s", dev_client_id);
}
ST_MQTTConf *SelMqttConf(reconnectpara *pstPara)
{
	ST_MQTTConf *pstConf = NULL;
	if(pstPara != NULL)
	{
		if(pstPara->iMqttType == 0)
		{
			pstConf = s_pstConfA;
		}
		else
		{
			pstConf = s_pstConfB;
		}
	}
	return pstConf;
}

int tcp_mqtt_client_mulx(reconnectpara *pstPara)
{
	ST_MQTTConf *pstConf = SelMqttConf(pstPara);

	if(pstConf == NULL)
	{
		return -1;
	}
	if(strlen(pstPara->xr_mqtt_para.host_name))
	{
		sprintf(pstConf->szUrl, "%s", pstPara->xr_mqtt_para.host_name);
		pstConf->iPort = pstPara->xr_mqtt_para.port;
	}
	pstConf->iConnectTimeOut = pstPara->xr_mqtt_para.command_timeout_ms;

	return 0;
}

int ssl_mqtt_client_mulx(reconnectpara *pstPara, 
	const char *server_cert, uint32_t server_cert_len)
{
	ST_MQTTConf *pstConf = SelMqttConf(pstPara);

	if(pstConf == NULL)
	{
		return -1;
	}
	if(pstPara->ysw1_mqtt_client.isconnected == 1)
	{
		return 0;
	}
	pstConf->c_pszServerCert = server_cert;
	pstConf->iServerCertLen = server_cert_len;
	if(pstConf->c_pszServerCert == NULL || pstConf->iServerCertLen == 0)
	{
		pstConf->wNoSSLVerify = TRUE;
	}

	if(strlen(pstPara->xr_mqtt_para.host_name))
	{
		sprintf(pstConf->szUrl, "%s", pstPara->xr_mqtt_para.host_name);
		pstConf->iPort = pstPara->xr_mqtt_para.port;
	}
	pstConf->iConnectTimeOut = pstPara->xr_mqtt_para.command_timeout_ms;
	return 0;
}

void MQTTConnectAErr(void *pvParam)
{
	ALIOT_PRINT("%s start!", __func__);
	s_stParaA.cal_set.mqtt_con = MQTT_CACK;
	s_stParaA.cal_set.mqtt_pub = MQTT_CACK;
	s_stParaA.cal_set.mqtt_sub = MQTT_CACK;
	s_stParaA.ysw1_mqtt_client.isconnected = 1;
	s_stParaA.ysw1_mqtt_client.isDiscon = 1;
	TermInfo.ServiceOnline &= ~MqttMaskA;
	tts_play_set_idx(AUD_ID_MQTT_CONNECT_FAIL,0,0);
}

void MQTTConnectASucc(void *pvParam)
{
	ALIOT_PRINT("%s start!", __func__);
	s_stParaA.cal_set.mqtt_con = MQTT_DICACK;
	s_stParaA.cal_set.mqtt_pub = MQTT_DICACK;
	s_stParaA.cal_set.mqtt_sub = MQTT_DICACK;
	s_stParaA.ysw1_mqtt_client.isconnected = 1;
	s_stParaA.ysw1_mqtt_client.isDiscon = 0;
	TermInfo.ServiceOnline |= MqttMaskA;
	if ((TermInfo.ServiceOnline&(MqttMaskA|MqttMaskB))==(MqttMaskA|MqttMaskB))
	{
		tts_play_set_idx(AUD_ID_MQTT_CONNECT_SUCESS,0,0);
	}
	ALIOT_PRINT("TermInfo.ServiceOnline = 0x%02x", TermInfo.ServiceOnline);
}

void MQTTConnectBErr(void *pvParam)
{
	ALIOT_PRINT("%s start!", __func__);
	s_stParaB.cal_set.mqtt_con = MQTT_CACK;
	s_stParaB.cal_set.mqtt_pub = MQTT_CACK;
	s_stParaB.cal_set.mqtt_sub = MQTT_CACK;
	s_stParaB.ysw1_mqtt_client.isconnected = 1;
	s_stParaB.ysw1_mqtt_client.isDiscon = 1;
	TermInfo.ServiceOnline &= ~MqttMaskB;
	tts_play_set_idx(AUD_ID_MQTT_CONNECT_FAIL,0,0);
}

void MQTTConnectBSucc(void *pvParam)
{
	ALIOT_PRINT("%s start!", __func__);
	s_stParaB.cal_set.mqtt_con = MQTT_DICACK;
	s_stParaB.cal_set.mqtt_pub = MQTT_DICACK;
	s_stParaB.cal_set.mqtt_sub = MQTT_DICACK;
	s_stParaB.ysw1_mqtt_client.isconnected = 1;
	s_stParaB.ysw1_mqtt_client.isDiscon = 0;
	TermInfo.ServiceOnline |= MqttMaskB;
	if ((TermInfo.ServiceOnline&(MqttMaskA|MqttMaskB))==(MqttMaskA|MqttMaskB))
	{
		tts_play_set_idx(AUD_ID_MQTT_CONNECT_SUCESS,0,0);
	}
	ALIOT_PRINT("TermInfo.ServiceOnline = 0x%02x", TermInfo.ServiceOnline);
}

static int MQTTConnect(reconnectpara *pstPara, MQTTPacket_connectData* pstConData)
{
	ST_MQTTConf *pstConf = SelMqttConf(pstPara);

	if(pstConf == NULL)
	{
		return -1;
	}
	if(pstPara->ysw1_mqtt_client.isconnected)
	{
	  	if(pstPara->cal_set.mqtt_con == MQTT_CACK)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	pstConf->iKeepAlive = pstConData->keepAliveInterval * 3 / 2;
	pstConf->iHeartBeatIntervalTime = pstConData->keepAliveInterval * 1000;
	pstConf->ucCleanSession = pstConData->cleansession;
	if (pstPara->iMqttType == 0)
	{
		pstConf->cb_MQTTConnetErr = MQTTConnectAErr;
		pstConf->cb_MQTTConnetSucc = MQTTConnectASucc;
		pstConf->cb_MQTTLostHeartBeat = MQTTConnectAErr;
		if (strlen(pstConf->szUrl))
			sprintf(pstConf->szUrl, "%s", s_stParaA.xr_mqtt_para.host_name);
		if (pstConf->iPort > 0)
			pstConf->iPort = s_stParaA.xr_mqtt_para.port;
	}
	else
	{
		pstConf->cb_MQTTConnetErr = MQTTConnectBErr;
		pstConf->cb_MQTTConnetSucc = MQTTConnectBSucc;
		pstConf->cb_MQTTLostHeartBeat = MQTTConnectBErr;
		if (strlen(pstConf->szUrl))
			sprintf(pstConf->szUrl, "%s", s_stParaB.xr_mqtt_para.host_name);
		if (pstConf->iPort > 0)
			pstConf->iPort = s_stParaB.xr_mqtt_para.port;
	}

	sprintf(pstConf->szClientId, "%s", pstConData->clientID.cstring);
	sprintf(pstConf->szUserName, "%s", pstConData->username.cstring);
	sprintf(pstConf->szPassWord, "%s", pstConData->password.cstring);
	
	if(strlen(pstConf->szClientId))
	{
		pstConf->isP2P = TRUE;
	}
	return 0;
}

static int MQTTSubscribe(reconnectpara *pstPara, const char* c_pszTopic, enum QoS eQos, CB_MQTT_SUB_RECV cb_MessHandler)
{
	int iSubCnt;
	ST_MQTTConf *pstConf = SelMqttConf(pstPara);

	if(pstConf == NULL)
	{
		return -1;
	}
	if(pstPara->ysw1_mqtt_client.isconnected)
	{
	  	if(pstPara->cal_set.mqtt_con == MQTT_CACK)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	iSubCnt = pstConf->iSubNum;
	if(iSubCnt >= MQTT_SUB_TOPIC_MAX_SIZE)
	{
		return -1;
	}
	sprintf(pstConf->astSubList[iSubCnt].szSubTopic, "%s", c_pszTopic);
	pstConf->astSubList[iSubCnt].cb_SubRecv = cb_MessHandler;
	pstConf->astSubList[iSubCnt].iSubRet = YMI_OK;
	pstConf->iSubNum ++;
	return 0;
}

static int MQTTPublish(reconnectpara *pstPara, const char* c_pszTopic, CB_MQTT_PUB_MSG_PACK cb_MessHandler)
{
	int iPubCnt;
	ST_MQTTConf *pstConf = SelMqttConf(pstPara);

	if(pstConf == NULL)
	{
		return -1;
	}
	if(pstPara->ysw1_mqtt_client.isconnected)
	{
	  	if(pstPara->cal_set.mqtt_con == MQTT_CACK)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	iPubCnt = pstConf->iPubNum;
	if(iPubCnt >= MQTT_PUB_TOPIC_MAX_SIZE)
	{
		return -1;
	}
	sprintf(pstConf->astPubList[iPubCnt].szPubTopic, "%s", c_pszTopic);
	pstConf->astPubList[iPubCnt].cb_PubMsgPack = cb_MessHandler;
	pstConf->astPubList[iPubCnt].iPubRet = YMI_OK;
	pstConf->iPubNum ++;
	return 0;
}

static int MQTTYield(reconnectpara *pstPara, int iTimeOutMS)
{
	ST_MQTTConf *pstConf = SelMqttConf(pstPara);
	int i;

	if(pstConf == NULL)
	{
		return -1;
	}
	if (!g_iNetOnline)
	{
		SLEEP_MS(iTimeOutMS);
		return -1;
	}
	if((pstPara->iMqttType == 0 && s_tMQTTTaskIDA) || 
		(pstPara->iMqttType && s_tMQTTTaskIDB))
	{
		for(i = 0; i < pstConf->iSubNum; i++)
		{
			if(pstConf->astSubList[i].iSubRet != YMI_OK)
			{
				pstPara->cal_set.mqtt_sub = MQTT_CACK;
				ALIOT_PRINT("sub %s fail ret=0x%04x", pstConf->astSubList[i].szSubTopic, -pstConf->astSubList[i].iSubRet);
				break;
			}
		}
		SLEEP_MS(iTimeOutMS);
		return 0;
	}
	int iRet;
	iRet = YMI_TaskCreat(pstPara->iMqttType == 0 ? (&s_tMQTTTaskIDA) : (&s_tMQTTTaskIDB), 
		"mqtt dual", 35 * 1024, YMI_MQTTProce, pstConf, EM_TASK_PRIORITY_NORMAL, 10);
    if(iRet != YMI_OK)
    {
        FREE(pstConf);
		return RET_FAIL;
    }
	return RET_SUCC;
}
#define MAX_MQTT_BUFF_SIZE		0

unsigned char sendbufA[MAX_MQTT_BUFF_SIZE];
unsigned char readbufA[MAX_MQTT_BUFF_SIZE];
unsigned char sendbufB[MAX_MQTT_BUFF_SIZE];
unsigned char readbufB[MAX_MQTT_BUFF_SIZE];

	char topic_sub_messageA[100] = {0};
	char topic_sub_messageA2[100] = {0};
//	char topic_pub_informA[100] = {0};
	char topic_sub_upgradeA[100] = {0};
	char topic_sub_messageB[100] = {0};
//	char topic_pub_informB[100] = {0};
	char topic_sub_upgradeB[100] = {0};

void default_msgArriveA(MessageData* data)
{
	ALIOT_PRINT("======%s_%d======\n", __func__, __LINE__);
	ALIOT_PRINT("topic:%.*s data:%s", data->topicName->lenstring.len, data->topicName->lenstring.data, data->message->payload);
	if(strstr(data->topicName->lenstring.data, topic_sub_upgradeA))
		otaUpdateArrivedA(data->topicName->lenstring.data, data->message->payload, data->message->payloadlen);
	else if(strstr(data->topicName->lenstring.data, topic_sub_messageA))
		messageArrivedA(data->topicName->lenstring.data, data->message->payload, data->message->payloadlen);
}

void default_msgArriveB(MessageData* data)
{
	ALIOT_PRINT("======%s_%d======\n", __func__, __LINE__);
	ALIOT_PRINT("topic:%.*s data:%s", data->topicName->lenstring.len, data->topicName->lenstring.data, data->message->payload);
	// if(strstr(data->topicName->lenstring.data, topic_sub_upgradeB))
	// 	otaUpdateArrivedB(data);
	// else if(strstr(data->topicName->lenstring.data, topic_sub_messageB))
	// 	messageArrivedB(data);
}


void mqtt_work_taskA(void * argv)
{
	int rc = 0;
	MQTTPacket_connectData connectData;// = MQTTPacket_connectData_initializer;
	
	memset(&s_stParaA,0,sizeof(s_stParaA));
	s_stParaA.ExitFlag=&mqtt_ctrlA;
	s_stParaA.ysw1_mqtt_client.isDiscon = 0;
//	s_stParaA.c=&ysw1_mqtt_clientA;

	while (!g_iNetOnline)
	{
		ALIOT_PRINT("wait net online");
		SLEEP_S(10);
	}	
	TermInfo.MqttIsRuning |= MqttMaskA;
	TermInfo.ServiceOnline &= ~MqttMaskA;
	
	ALIOT_PRINT("func %s line %d\n", __func__, __LINE__);
	mqtt_client_close(&s_stParaA.ysw1_mqtt_client);
//	memset(&ysw1_mqtt_clientA,0,sizeof(ysw1_mqtt_clientA));
#ifdef MQTT_DUAL_TEST
	sprintf(SysInfoGet()->szProductKey, "%s", "hkgiXhDumrw");
	sprintf(SysInfoGet()->szDeviceName, "%s", "DspreadTest08");
	sprintf(SysInfoGet()->szDeviceSecret, "%s", "46d83f6f395e3efa065da17068c855a1");
#endif

	//topic must start with '/'  Note:This does not require modifying the server-side topic (server topic is still 'xxx/xxx')
	//eg: 
	//	correct：'/xxx/xxx'
	//	incorrect：'xxx/xxx'
	sprintf(topic_sub_messageA, "/%s/%s/user/message", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
	sprintf(topic_sub_messageA2, "/%s/%s/user/pushMsg", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
	sprintf(topic_sub_upgradeA, "/ota/device/upgrade/%s/%s", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
//	sprintf(topic_sub_inform, "/ota/device/inform/%s/%s", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
	//sprintf(topic_sub_rrpc, "/sys/%s/%s/rrpc/request/+", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
	
	mqtt_ctrlA = 1;
	s_stParaA.cal_set.mqtt_con = MQTT_CACK;
	s_stParaA.cal_set.mqtt_sub = MQTT_CACK;
	s_stParaA.cal_set.mqtt_pub = MQTT_CACK;

//	alink_devc_init();
//	mqtt_para_init();
	aliot_device_info_mulx(SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName, SysInfoGet()->szDeviceSecret, DEV_CLIENT_ID, SECURE_MODE_A,&s_stParaA);
	
	/* connect para */
	connectData.MQTTVersion 		= 4;
	connectData.keepAliveInterval 	= 60;
	connectData.cleansession		= 0;
	connectData.clientID.cstring 	= s_stParaA.xr_devc_info.client_id;
	connectData.username.cstring 	= s_stParaA.xr_devc_info.user_name;
	connectData.password.cstring 	= s_stParaA.xr_devc_info.password;

	/* mqtt para */
	s_stParaA.xr_mqtt_para.command_timeout_ms = 3000; //mqtt timeout 
	s_stParaA.xr_mqtt_para.read_buf = readbufA;
	s_stParaA.xr_mqtt_para.send_buf = sendbufA;
	s_stParaA.xr_mqtt_para.read_buf_size = MAX_MQTT_BUFF_SIZE;
	s_stParaA.xr_mqtt_para.send_buf_size = MAX_MQTT_BUFF_SIZE;
	s_stParaA.xr_mqtt_para.port	= 443;
	strncpy(s_stParaA.xr_mqtt_para.host_name, ALI_MQTT_URL, strlen(ALI_MQTT_URL));
	// ql_rtos_task_sleep_s(3);
//	ysw1_mqtt_client.defaultMessageHandler=otaUpdateArrived;
	
	while(mqtt_ctrlA)
	{
		//ALIOT_PRINT("while(mqtt_ctrl)\n");
		if (s_stParaA.ysw1_mqtt_client.isDiscon)
		{	// Control the reconnection speed
			SLEEP_S(5);
		}

		if(s_stParaA.cal_set.mqtt_con == MQTT_CACK) {
			TermInfo.ServiceOnline &= ~MqttMaskA;
			lpm_set(LPM_LOCK_MQTT,1);
			if(0 == memcmp(SECURE_MODE_A, MQTT_TCP_MODE, 1))
				rc = tcp_mqtt_client_mulx(&s_stParaA);
			if(0 == memcmp(SECURE_MODE_A, MQTT_TLS_MODE, 1))
			{
				ALIOT_PRINT("%s:before ssl_mqtt_client, line %d\n", __func__,__LINE__);
				rc = ssl_mqtt_client_mulx(&s_stParaA,ali_ca_cert, strlen(ali_ca_cert));
				ALIOT_PRINT("%s:after ssl_mqtt_client,line %d\n", __func__,__LINE__);
				ALIOT_PRINT("%s:ssl_mqtt_client : rc = %d \n",__func__, rc);
			}
			s_stParaA.ysw1_mqtt_client.defaultMessageHandler=default_msgArriveA;
	
			if (rc  != 0) {
				mqtt_reconnect(&s_stParaA);
				continue;
			}

			rc = MQTTConnect(&s_stParaA, &connectData);
			if (rc != 0) {
				ALIOT_PRINT("%s:MQTT client connect error! Return error code is %d\n",__func__, rc);
				mqtt_reconnect(&s_stParaA);
				continue;
			}
			ALIOT_PRINT("%s:MQTT Connected\n",__func__);
			s_stParaA.cal_set.mqtt_con = MQTT_DICACK;
		}

		if(s_stParaA.cal_set.mqtt_sub == MQTT_CACK) {
			//subscribe OTA notify start
			rc = MQTTSubscribe(&s_stParaA, topic_sub_upgradeA, QOS1, otaUpdateArrivedA);
			if (rc != 0) 
			{
				ALIOT_PRINT("%s:Return code from MQTT subscribe(OTA) is %d\n",__func__, rc);
				s_stParaA.cal_set.mqtt_con = MQTT_CACK;
				mqtt_reconnect(&s_stParaA);
				continue;
			}
			//subscribe OTA notify end
			rc = MQTTSubscribe(&s_stParaA, topic_sub_messageA, QOS1, messageArrivedA);
			if (rc != 0) {
				ALIOT_PRINT("%s:Return code from MQTT subscribe is %d\n",__func__, rc);
				s_stParaA.cal_set.mqtt_con = MQTT_CACK;
				mqtt_reconnect(&s_stParaA);
				continue;
			}
			rc = MQTTSubscribe(&s_stParaA, topic_sub_messageA2, QOS1, messageArrivedA);
			if (rc != 0) {
				ALIOT_PRINT("%s:Return code from MQTT subscribe is %d\n",__func__, rc);
				s_stParaA.cal_set.mqtt_con = MQTT_CACK;
				mqtt_reconnect(&s_stParaA);
				continue;
			}

			ALIOT_PRINT("%s:MQTT Subscrible is success\n",__func__);
			s_stParaA.cal_set.mqtt_sub = MQTT_DICACK;
		}

		if(s_stParaA.cal_set.mqtt_pub == MQTT_CACK) {
			char topic_pub_inform[100] = {0};
			//update version for OTA start
			// MQTTMessage pub_inform;
			// char *buff;

			// buff=PubTermInfo();
			// if (buff==NULL) continue;
			// pub_inform.qos = QOS1;
			// pub_inform.retained = 0;
			// pub_inform.payload = buff;
			// pub_inform.payloadlen = strlen(buff);
			
			// ALIOT_PRINT("%s:OTA infrom:\n---------------------\n%s\n---------------------\n", __func__,buff);
			sprintf(topic_pub_inform, "/ota/device/inform/%s/%s", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
			rc = MQTTPublish(&s_stParaA, topic_pub_inform, PubTermInfo);
			if (rc != 0) {
				ALIOT_PRINT("%s:Return code from MQTT publish is %d\n",__func__, rc);
				s_stParaA.cal_set.mqtt_con = MQTT_CACK;
				s_stParaA.cal_set.mqtt_sub = MQTT_CACK;
				mqtt_reconnect(&s_stParaA);
				continue;
			}
			ALIOT_PRINT("%s:Inform version for OTA success\n",__func__);
			//update version for OTA end
			

			ALIOT_PRINT("%s:MQTT publish is success\n",__func__);
			s_stParaA.cal_set.mqtt_pub = MQTT_DICACK;
		}
		
		rc = MQTTYield(&s_stParaA, 3000);
		if (rc != 0)
		{
			ALIOT_PRINT("%s:Return code from yield is %d\n",__func__, rc);
			rc = MQTTDisconnect(&s_stParaA.ysw1_mqtt_client);
			if (rc != 0)
			{
				ALIOT_PRINT("%s:Return code from MQTT disconnect is %d\n",__func__,rc);
			}
			else
				ALIOT_PRINT("%s:MQTT disconnect is success\n",__func__);


			s_stParaA.cal_set.mqtt_con = MQTT_CACK;
			s_stParaA.cal_set.mqtt_sub = MQTT_CACK;
			s_stParaA.cal_set.mqtt_pub = MQTT_CACK;
			mqtt_reconnect(&s_stParaA);
			continue;
		}
//		testmess();
//		testota();
//        ql_rtos_task_sleep_ms(200);
		s_stParaA.mqtt_con_nums = 0;
		check_mqtt_server(&s_stParaA.ysw1_mqtt_client);
	}

	ALIOT_PRINT("%s:mqtt_exit line %d\n", __func__,__LINE__);
	
	//MQTTUnsubscribe(&ysw1_mqtt_client, topic_sub_upgrade);
	//MQTTUnsubscribe(&ysw1_mqtt_client, topic_sub_message);

	rc = MQTTDisconnect(&s_stParaA.ysw1_mqtt_client);
	if (rc != 0)
	{
		ALIOT_PRINT("%s:Return code from MQTT disconnect is %d\n",__func__, rc);
	}
	else
		ALIOT_PRINT("%s:MQTT disconnect is success\n",__func__);

	mqtt_client_close(&s_stParaA.ysw1_mqtt_client);
	ALIOT_PRINT("%s:mqtt_work_set end\n",__func__);
	lpm_set(LPM_LOCK_MQTT,1);
	TermInfo.MqttIsRuning &= ~MqttMaskA;
	TermInfo.ServiceOnline &= ~MqttMaskA;
	YMI_MQTTExit(s_tMQTTTaskIDA);
	s_tMQTTTaskIDA = 0;
	FREE(s_pstConfA);
	YMI_TaskDelete(mqtt_task_ctrl_threadA);
}


#if 1

#define IOT_CLIENT_ID			"CTYXY00000000001"  
#define IOT_USER_NAME			"test0001"
#define IOT_PASSWORD			"test0001"
#define IOT_HOST_NAME			"218.66.91.181"
#define IOT_PORT			7075


void mqtt_work_taskB(void * argv)
{
	int rc = 0;
	MQTTPacket_connectData connectData;// = MQTTPacket_connectData_initializer;

	memset(&s_stParaB,0,sizeof(s_stParaB));
	s_stParaB.ExitFlag=&mqtt_ctrlB;
	s_stParaB.iMqttType = 1;
	s_stParaB.ysw1_mqtt_client.isDiscon = 0;
//	s_stParaB.c=&ysw1_mqtt_clientA;
	while (!g_iNetOnline)
	{
		ALIOT_PRINT("wait net online");
		SLEEP_S(10);
	}	
	TermInfo.MqttIsRuning |= MqttMaskB;
	TermInfo.ServiceOnline &= ~MqttMaskB;
	
	ALIOT_PRINT("func %s line %d\n", __func__, __LINE__);
	mqtt_client_close(&s_stParaB.ysw1_mqtt_client);
//	memset(&ysw1_mqtt_clientB,0,sizeof(ysw1_mqtt_clientB));
	
//	sprintf(topic_sub_messageB, "/%s/%s/user/message", PRODUCT_KEY, DEVICE_NAME);
//	sprintf(topic_sub_upgradeB, "/ota/device/upgrade/%s/%s", PRODUCT_KEY, DEVICE_NAME);
	
	mqtt_ctrlB = 1;
	s_stParaB.cal_set.mqtt_con = MQTT_CACK;
	s_stParaB.cal_set.mqtt_sub = MQTT_CACK;
	s_stParaB.cal_set.mqtt_pub = MQTT_CACK;
#ifdef MQTT_DUAL_TEST
	sprintf(SysInfoGet()->szThridProductKey, "%s", "hkgiXhDumrw");
	sprintf(SysInfoGet()->szThridDeviceName, "%s", "DspreadTest09");
	sprintf(SysInfoGet()->szThridDeviceSecret, "%s", "c69a5897237745db343a5cd06f99334c");
#endif
	aliot_device_info_mulx(SysInfoGet()->szThridProductKey, SysInfoGet()->szThridDeviceName, SysInfoGet()->szThridDeviceSecret, DEV_CLIENT_ID, SECURE_MODE_A,&s_stParaB);

	/* connect para */
	connectData.MQTTVersion 		= 4;
	connectData.keepAliveInterval 	= 60;
	connectData.cleansession		= 0;
	// connectData.clientID.cstring 	= IOT_CLIENT_ID;
	// connectData.username.cstring 	= IOT_USER_NAME;
	// connectData.password.cstring 	= IOT_PASSWORD;
	connectData.clientID.cstring 	= s_stParaB.xr_devc_info.client_id;
	connectData.username.cstring 	= s_stParaB.xr_devc_info.user_name;
	connectData.password.cstring 	= s_stParaB.xr_devc_info.password;
	/* mqtt para */
	s_stParaB.xr_mqtt_para.command_timeout_ms = 3000; //mqtt��ʱʱ�� 
	s_stParaB.xr_mqtt_para.read_buf = readbufB;
	s_stParaB.xr_mqtt_para.send_buf = sendbufB;
	s_stParaB.xr_mqtt_para.read_buf_size = MAX_MQTT_BUFF_SIZE;
	s_stParaB.xr_mqtt_para.send_buf_size = MAX_MQTT_BUFF_SIZE;
	// s_stParaB.xr_mqtt_para.port	= IOT_PORT;
	s_stParaB.xr_mqtt_para.port = 443;
	// strncpy(s_stParaB.xr_mqtt_para.host_name, IOT_HOST_NAME, strlen(IOT_HOST_NAME));
	strncpy(s_stParaB.xr_mqtt_para.host_name, ALI_MQTT_URL, strlen(ALI_MQTT_URL));
	// ql_rtos_task_sleep_s(3);
//	ysw1_mqtt_client.defaultMessageHandler=otaUpdateArrived;
	
	while(mqtt_ctrlB)
	{
		//ALIOT_PRINT("while(mqtt_ctrl)\n");
		if (s_stParaB.ysw1_mqtt_client.isDiscon)
		{
			SLEEP_S(5);
		}
		if(s_stParaB.cal_set.mqtt_con == MQTT_CACK) {
		    TermInfo.ServiceOnline &= ~MqttMaskB;
		    lpm_set(LPM_LOCK_MQTT,1);
			if(0 == memcmp(SECURE_MODE_B, MQTT_TCP_MODE, 1))
				rc = tcp_mqtt_client_mulx(&s_stParaB);
			if(0 == memcmp(SECURE_MODE_B, MQTT_TLS_MODE, 1))
			{
				ALIOT_PRINT("%s:before ssl_mqtt_client, line %d\n", __func__,__LINE__);
//				rc = ssl_mqtt_client_mulx(&s_stParaB.ysw1_mqtt_client, &s_stParaB.xr_mqtt_para,ali_ca_cert, strlen(ali_ca_cert);
				rc = ssl_mqtt_client_mulx(&s_stParaB,ali_ca_cert, strlen(ali_ca_cert));
				ALIOT_PRINT("%s:after ssl_mqtt_client,line %d\n", __func__,__LINE__);
				ALIOT_PRINT("%s:ssl_mqtt_client : rc = %d \n",__func__, rc);
			}
			s_stParaB.ysw1_mqtt_client.defaultMessageHandler=default_msgArriveB;
	
			if (rc  != 0) {
				mqtt_reconnect(&s_stParaB);
				continue;
			}

			rc = MQTTConnect(&s_stParaB, &connectData);
			if (rc != 0) {
				ALIOT_PRINT("%s:MQTT client connect error! Return error code is %d\n",__func__, rc);
				mqtt_reconnect(&s_stParaB);
				continue;
			}
			ALIOT_PRINT("%s:MQTT Connected\n",__func__);
			s_stParaB.cal_set.mqtt_con = MQTT_DICACK;
		}

		if(s_stParaB.cal_set.mqtt_pub == MQTT_CACK) {
			char topic_pub_inform[100] = {0};
			//update version for OTA start
			// MQTTMessage pub_inform;
			// char *buff;

			// buff=PubTermInfo();
			// if (buff==NULL) continue;
			// pub_inform.qos = QOS1;
			// pub_inform.retained = 0;
			// pub_inform.payload = buff;
			// pub_inform.payloadlen = strlen(buff);
			
			// ALIOT_PRINT("%s:OTA infrom:\n---------------------\n%s\n---------------------\n", __func__,buff);
			// sprintf(topic_pub_inform, "/ota/device/inform/%s", IOT_CLIENT_ID);
			sprintf(topic_pub_inform, "/ota/device/inform/%s/%s", SysInfoGet()->szThridProductKey, SysInfoGet()->szThridDeviceName);
			// sprintf(topic_sub_messageB, "%s", "vbox/JVB2080/RDPVBGJ10000013/404940968716250/vbPaymentAlert");
			// sprintf(topic_pub_inform, "/ota/device/inform/%s/%s", SysInfoGet()->szProductKey, SysInfoGet()->szDeviceName);
			rc = MQTTPublish(&s_stParaB, topic_pub_inform, PubTermInfo);
			if (rc != 0) {
				ALIOT_PRINT("%s:Return code from MQTT publish is %d\n",__func__, rc);
				s_stParaB.cal_set.mqtt_con = MQTT_CACK;
				s_stParaB.cal_set.mqtt_sub = MQTT_CACK;
				mqtt_reconnect(&s_stParaB);
				continue;
			}
			ALIOT_PRINT("%s:Inform version for OTA success\n",__func__);
			//update version for OTA end
			
			ALIOT_PRINT("%s:MQTT publish is success\n",__func__);
			s_stParaB.cal_set.mqtt_pub = MQTT_DICACK;
		}
		
		rc = MQTTYield(&s_stParaB, 3000);
		if (rc != 0)
		{
			ALIOT_PRINT("%s:Return code from yield is %d\n",__func__, rc);
			rc = MQTTDisconnect(&s_stParaB.ysw1_mqtt_client);
			if (rc != 0)
			{
				ALIOT_PRINT("%s:Return code from MQTT disconnect is %d\n", __func__, rc);
			}
			else
				ALIOT_PRINT("%s:MQTT disconnect is success\n",__func__);


			s_stParaB.cal_set.mqtt_con = MQTT_CACK;
			s_stParaB.cal_set.mqtt_sub = MQTT_CACK;
			s_stParaB.cal_set.mqtt_pub = MQTT_CACK;
			mqtt_reconnect(&s_stParaB);
			continue;
		}
//        ql_rtos_task_sleep_ms(200);
		s_stParaB.mqtt_con_nums = 0;
		check_mqtt_server(&s_stParaB.ysw1_mqtt_client);
	}

	ALIOT_PRINT("%s:mqtt_exit line %d\n", __func__,__LINE__);
	
	//MQTTUnsubscribe(&ysw1_mqtt_client, topic_sub_upgrade);
	//MQTTUnsubscribe(&ysw1_mqtt_client, topic_sub_message);

	rc = MQTTDisconnect(&s_stParaB.ysw1_mqtt_client);
	if (rc != 0)
	{
		ALIOT_PRINT("%s:Return code from MQTT disconnect is %d\n", __func__, rc);
	}
	else
		ALIOT_PRINT("%s:MQTT disconnect is success\n",__func__);

	mqtt_client_close(&s_stParaB.ysw1_mqtt_client);
	ALIOT_PRINT("%s:mqtt_work_set end\n",__func__);
	lpm_set(LPM_LOCK_MQTT,1);
	TermInfo.ServiceOnline &= ~MqttMaskB;
	TermInfo.MqttIsRuning &= ~MqttMaskB;
	YMI_MQTTExit(s_tMQTTTaskIDB);
	FREE(s_pstConfB);
	s_tMQTTTaskIDB = 0;
	YMI_TaskDelete(mqtt_task_ctrl_threadB);
}
#else
void mqtt_work_taskB(void * argv)
{
	int rc = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;

	memset(&s_stParaB,0,sizeof(s_stParaB));
	s_stParaB.ExitFlag=&mqtt_ctrlA;
//	s_stParaB.c=&ysw1_mqtt_clientA;

	TermInfo.MqttIsRuning |= MqttMaskB;
	
	ALIOT_PRINT("func %s line %d\n", __func__, __LINE__);
	mqtt_client_close(&s_stParaB.ysw1_mqtt_client);
//	memset(&ysw1_mqtt_clientB,0,sizeof(ysw1_mqtt_clientB));
	
	sprintf(topic_sub_messageB, "/%s/%s/user/message", PRODUCT_KEY, DEVICE_NAME);
	sprintf(topic_sub_upgradeB, "/ota/device/upgrade/%s/%s", PRODUCT_KEY, DEVICE_NAME);
	
	mqtt_ctrlB = 1;
	s_stParaB.cal_set.mqtt_con = MQTT_CACK;
	s_stParaB.cal_set.mqtt_sub = MQTT_CACK;
	s_stParaB.cal_set.mqtt_pub = MQTT_CACK;

	aliot_device_info_mulx(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, DEV_CLIENT_ID, SECURE_MODE_B,&s_stParaB.xr_devc_info);

	/* connect para */
	connectData.MQTTVersion 		= 4;
	connectData.keepAliveInterval 	= 60;
	connectData.cleansession		= 0;
	connectData.clientID.cstring 	= s_stParaB.xr_devc_info.client_id;
	connectData.username.cstring 	= s_stParaB.xr_devc_info.user_name;
	connectData.password.cstring 	= s_stParaB.xr_devc_info.password;

	/* mqtt para */
	s_stParaB.xr_mqtt_para.command_timeout_ms = 3000; //mqtt��ʱʱ�� 
	s_stParaB.xr_mqtt_para.read_buf = readbufB;
	s_stParaB.xr_mqtt_para.send_buf = sendbufB;
	s_stParaB.xr_mqtt_para.read_buf_size = MAX_MQTT_BUFF_SIZE;
	s_stParaB.xr_mqtt_para.send_buf_size = MAX_MQTT_BUFF_SIZE;
	s_stParaB.xr_mqtt_para.port	= s_stParaB.xr_devc_info.port;
	strncpy(s_stParaB.xr_mqtt_para.host_name, s_stParaB.xr_devc_info.host_name, strlen(s_stParaB.xr_devc_info.host_name));
	ql_rtos_task_sleep_s(3);
//	ysw1_mqtt_client.defaultMessageHandler=otaUpdateArrived;
	
	while(mqtt_ctrlB)
	{
		//ALIOT_PRINT("while(mqtt_ctrl)\n");
		if(s_stParaB.cal_set.mqtt_con == MQTT_CACK) {
			if(0 == memcmp(SECURE_MODE_B, MQTT_TCP_MODE, 1))
				rc = tcp_mqtt_client_mulx(&s_stParaB.ysw1_mqtt_client,&s_stParaB.xr_mqtt_para);
			if(0 == memcmp(SECURE_MODE_B, MQTT_TLS_MODE, 1))
			{
				ALIOT_PRINT("before ssl_mqtt_client, line %d\n", __LINE__);
				rc = ssl_mqtt_client_mulx(&s_stParaB.ysw1_mqtt_client, &s_stParaB.xr_mqtt_para,alink_ca_get(), strlen(alink_ca_get())+1);
				ALIOT_PRINT("after ssl_mqtt_client, line %d\n", __LINE__);
			}
			s_stParaB.ysw1_mqtt_client.defaultMessageHandler=default_msgArriveB;
	
			if (rc  != 0) {
				mqtt_reconnect(&s_stParaB);
				continue;
			}

			rc = MQTTConnect(&s_stParaB.ysw1_mqtt_client, &connectData);
			if (rc != 0) {
				ALIOT_PRINT("MQTT client connect error! Return error code is %d\n", rc);
				continue;
			}
			ALIOT_PRINT("MQTT Connected\n");
			s_stParaB.cal_set.mqtt_con = MQTT_DICACK;
		}

		if(s_stParaB.cal_set.mqtt_sub == MQTT_CACK) {
			//subscribe OTA notify start
			rc = MQTTSubscribe(&s_stParaB.ysw1_mqtt_client, topic_sub_upgradeB, QOS1, otaUpdateArrivedB);
			if (rc != 0) 
			{
				ALIOT_PRINT("Return code from MQTT subscribe(OTA) is %d\n", rc);
				s_stParaB.cal_set.mqtt_con = MQTT_CACK;
				continue;
			}
			//subscribe OTA notify end
			rc = MQTTSubscribe(&s_stParaB.ysw1_mqtt_client, topic_sub_messageB, QOS1, messageArrivedB);
			
			if (rc != 0) {
				ALIOT_PRINT("Return code from MQTT subscribe is %d\n", rc);
				s_stParaB.cal_set.mqtt_con = MQTT_CACK;
				continue;
			}

			ALIOT_PRINT("MQTT Subscrible is success\n");
			s_stParaB.cal_set.mqtt_sub = MQTT_DICACK;
		}

		if(s_stParaB.cal_set.mqtt_pub == MQTT_CACK) {
			char topic_pub_inform[100] = {0};
			//update version for OTA start
			MQTTMessage pub_inform;
			char *buff;

			buff=PubTermInfo();
			if (buff==NULL) continue;
			pub_inform.qos = QOS1;
			pub_inform.retained = 0;
			pub_inform.payload = buff;
			pub_inform.payloadlen = strlen(buff);
			
			ALIOT_PRINT("OTA infrom:\n---------------------\n%s\n---------------------\n", buff);
			sprintf(topic_pub_inform, "/ota/device/inform/%s/%s", PRODUCT_KEY, DEVICE_NAME);
			rc = MQTTPublish(&s_stParaB.ysw1_mqtt_client, topic_pub_inform, &pub_inform);
			if (rc != 0) {
				ALIOT_PRINT("Return code from MQTT publish is %d\n", rc);
				s_stParaB.cal_set.mqtt_con = MQTT_CACK;
				s_stParaB.cal_set.mqtt_sub = MQTT_CACK;
				free(buff);
				continue;
			}
			free(buff);
			ALIOT_PRINT("Inform version for OTA success\n");
			//update version for OTA end
			

			ALIOT_PRINT("MQTT publish is success\n");
			s_stParaB.cal_set.mqtt_pub = MQTT_DICACK;
			
			{
				//tts_play_set(AudioFuwljcg,AudioFuwljcgLen,FixAudioTypeDef);
                tts_play_set_idx(AUD_ID_MQTT_CONNECT_SUCESS,0,0);
			}
				
		}
		
		rc = MQTTYield(&s_stParaB.ysw1_mqtt_client, 3000);
		if (rc != 0)
		{
			ALIOT_PRINT("Return code from yield is %d\n", rc);
			rc = MQTTDisconnect(&s_stParaB.ysw1_mqtt_client);
			if (rc != 0)
				ALIOT_PRINT("Return code from MQTT disconnect is %d\n", rc);
			else
				ALIOT_PRINT("MQTT disconnect is success\n");


			s_stParaB.cal_set.mqtt_con = MQTT_CACK;
			s_stParaB.cal_set.mqtt_sub = MQTT_CACK;
			s_stParaB.cal_set.mqtt_pub = MQTT_CACK;
			continue;
		}
//        ql_rtos_task_sleep_ms(200);
		s_stParaB.mqtt_con_nums = 0;
		check_mqtt_server(&s_stParaB);
	}

	ALIOT_PRINT("mqtt_exit line %d\n", __LINE__);
	
	//MQTTUnsubscribe(&ysw1_mqtt_client, topic_sub_upgrade);
	//MQTTUnsubscribe(&ysw1_mqtt_client, topic_sub_message);

	rc = MQTTDisconnect(&s_stParaB.ysw1_mqtt_client);
	if (rc != 0)
		ALIOT_PRINT("Return code from MQTT disconnect is %d\n", rc);
	else
		ALIOT_PRINT("MQTT disconnect is success\n");

	mqtt_client_close(&s_stParaB.ysw1_mqtt_client);
	ALIOT_PRINT("mqtt_work_set end\n");
	TermInfo.MqttIsRuning &= ~MqttMaskB;
    mqtt_task_ctrl_threadB = NULL;
	ql_rtos_task_delete(NULL);
}
#endif

static int MqttExit_dual(void)
{
	int ii;
	mqtt_ctrlA=0;
	mqtt_ctrlB=0;
	ALIOT_PRINT("<-- MqttExit -->\n");
	for(ii=0;ii<100;ii++)
	{
		if (!TermInfo.MqttIsRuning ) break;
		SLEEP_MS(100);
	}
	return 0;
}

void start_Mqtt_dual_task(void)
{
	if (!(TermInfo.MqttIsRuning&MqttMaskA))
	{
		if (YMI_TaskCreat(&mqtt_task_ctrl_threadA, "mqtt_work_setA", 2 * 1024, mqtt_work_taskA, NULL, EM_TASK_PRIORITY_NORMAL, 5) != RET_SUCC)
		{
			ALIOT_PRINT("thread create mqtt A error\n");
		}
		
	}
	SLEEP_S(2);
	if (!(TermInfo.MqttIsRuning&MqttMaskB))
	{
		if (YMI_TaskCreat(&mqtt_task_ctrl_threadB, "mqtt_work_setB", 2 * 1024, mqtt_work_taskB, NULL, EM_TASK_PRIORITY_NORMAL, 5) != RET_SUCC)
		{
			ALIOT_PRINT("thread create mqtt B error\n");
		}
	}
}

void test_mqtt_dual(void)
{
	start_Mqtt_dual_task();
	SLEEP_S(60);
	ALIOT_PRINT("mqtt dual exit B------------------------>");
	mqtt_ctrlB=0;
	int ii;
	for(ii=0;ii<100;ii++)
	{
		if (!(TermInfo.MqttIsRuning & MqttMaskB))  break;
		SLEEP_MS(100);
	}
	SLEEP_S(60);
	ALIOT_PRINT("mqtt start B------------------------>");
	if (YMI_TaskCreat(&mqtt_task_ctrl_threadB, "mqtt_work_setB", 2 * 1024, mqtt_work_taskB, NULL, EM_TASK_PRIORITY_NORMAL, 5) != RET_SUCC)
	{
		ALIOT_PRINT("thread create mqtt B error\n");
	}
	
}
