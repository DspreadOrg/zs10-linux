#ifndef _DEV_H_
#define _DEV_H_

#include "YMAPI.h"

#define RES_FILE_NAME "res.bin"
#define OTA_FILE_NAME "ota.zip"
#define PIC_ZIP_FILE_NAME "pic_save.zip"
#define PIC_FILE_NAME_POWER_ON "res/power_on.jpg"
#define VIDEO_FILE_NAME_POWER_ON "res/ads.mp4"
#define PIC_FILE_NAME_OTA "res/ota.jpg"

typedef enum{
	PLAY_ADS_TRANS_MODE_UNKNOWN = 0,
	PLAY_ADS_TRANS_MODE_TOP,
	PLAY_ADS_TRANS_MODE_BOTTOM,
	PLAY_ADS_TRANS_MODE_RIGHT,
	PLAY_ADS_TRANS_MODE_LEFT,
}EM_PLAY_ADS_TRANS_MODE;

typedef enum{
	EM_PLAY_ADS_AREA_UNKNOWN = 0,
	EM_PLAY_ADS_AREA_COMPLETE,
	EM_PLAY_ADS_AREA_PARTIAL,
}EM_PLAY_ADS_AREA;

typedef struct dev_play_ads{
	char cActive;
	int iIndex;
	int iTimeMS;
	char cTransMode;
	char cDispArea;
	int iCnt;
}ST_DevPlayAds;

int DevTaskInit(void);
int DevEventSent(EM_YMI_EVENT_ID eID, void *pvName, void *pvData, ulong wDataLen);



#endif