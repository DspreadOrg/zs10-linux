#ifndef _APP_SYSTEM_H
#define _APP_SYSTEM_H

#ifdef __cplusplus
		extern "C" {
#endif

#include "YMAPI.h"

#define SYSINFO_SSID_LEN_MAX		(32)
#define SYSINFO_PSK_LEN_MAX			(65)

typedef struct sysinfo_wlan_sta_param {
	uint8_t szSsid[SYSINFO_SSID_LEN_MAX];
	uint8_t cSsidLen;
	uint8_t szPsk[SYSINFO_PSK_LEN_MAX];
}ST_SysinfoWlanStaParam;

typedef struct {
	int iBattery;
	int iFlash;
	int iMicrophone;
	int iLed;
	int iSpeaker;
	int iRegisterState;
	int iVolume;
	int iVailed;	   // Parameter loading result: 1 - Success; Other - Failure
	char szFirmwareVersion[50];
	char szDeviceSecret[64];
	char szProductKey[64];
	char szDeviceName[64];
	char szThridDeviceSecret[64];
	char szThridProductKey[64];
	char szThridDeviceName[64];
	char szDeviceSN[64];
	char szServerUrl[64];
	char szHwver[32];
	char szServerMqtt[64];  // mqtt url
	int iUartLog;
	ST_SysinfoWlanStaParam stWlanStaParam;
	char szWifiVer[20];
	char szTopic[64];
	int iBacklightLevel;
	int iDeviceType; // 0: 4G device, 1: WIFI + 4G device
	int iNetChanlLTE;
	int iAirkiss;
	int iButtonLevel;
	int iOta;
	int iFirstBoot;
	char acMd5[32+1];
}ST_SysInfo;

typedef enum {
	PARAM_TYPE_STR,
	PARAM_TYPE_NUM = 4,
}EM_PARAM_TYPE;

typedef struct{
	char *pszItemName;
	unsigned short wSize;
	int iOff;
	uchar byType;
}ST_YMSettingItem;

int SysInfoInit(void);
int SysInfoSave(void);
int SysInfoSaveToNv(void);

int SysParamIniParse(void);
ST_SysInfo* SysInfoGet(void);

#ifdef __cplusplus
		}/*"C" */
#endif
		
#endif

