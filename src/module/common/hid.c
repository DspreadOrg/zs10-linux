/**********************************************************************
* Copyright (C)    Dspread
* The name of the file：hid.c
* Summary of content：HID-related APIs
*
***********************************************************************/
//Header file
#include "var.h"

//Macro definitions
#define HID_LOG_MODE
#ifdef HID_LOG_MODE
#define HID_LOG(msg, ...) LOG("[HID]"msg"", ##__VA_ARGS__)
#else
#define HID_LOG(msg, ...)
#endif

#define HID_TASK_STACK_SIZE (3 * 1024)
#define HID_TASK_EVENT_NUM (10)
#define HID_EVENT_WAIT_TIME_MS (10)

#define HID_READ_ONE_PACK_SIZE (64)
#define HID_READ_BUF_SIZE (4 * 1024 + 188)

#define HID_PACK_HEAD 0x02
#define HID_PACK_RESP_RET_CODE_LEN (4)
#define DEV_SN "TEST00000000001"
#define HW_VER "HW_V1.0.0.0"
#define SBI_VER "0001"
#define OS_VER "DH_V1.0.0.0"
#define APP_VER "DH_V1.0.0.0"


#define PIC_FILE_NAME_SAVE "res/save.jpg"
#define PIC_FILE_NAME_CONNECT_APP "res/connect_app.jpg"
#define PIC_FILE_NAME_TRANS_SUC "res/trans_suc.jpg"
#define PIC_FILE_NAME_TRANS_FAIL "res/trans_fail.jpg"
#define PIC_FILE_NAME_TRANS_SUC_AD "res/trans_suc_ad.jpg"
#define PIC_FILE_NAME_TRANS_QR "res/trans_qr.jpg"


#define HID_DOWNLOAD_FILE_MALLOC_MAX_SIZE (135 * 1024)
#define CURRENCY_SYMBOL "₹"

typedef enum{
	HID_CMO_TYPE_LCD = 0x01,
	HID_CMO_TYPE_BEEPER,
	HID_CMD_TYPE_LED,
	HID_CMD_TYPE_CONFIG,
}EM_HID_CMD_TYPE;

typedef enum{
	HID_LCD_OPS_CODE_CLEAR = 0x00,
	HID_LCD_OPS_CODE_SHOW_AMT_QR_BASE64 = 0x01,//amout+qrcode(base64)
	HID_LCD_OPS_CODE_SHOW_AMT_TR_SUCC = 0x02,//amout transaction was successful
	HID_LCD_OPS_CODE_SHOW_AMT_TR_FAIL = 0x03,//amout transaction was failed
	HID_LCD_OPS_CODE_SHOW_UNCON_PIC = 0x04,//shows unconnected pictures
	HID_LCD_OPS_CODE_SHOW_AMT_QR_STR = 0x11,//amout+qrcode(string)
	HID_LCD_OPS_CODE_SET_BL = 0x20,//Backlight Setting(1-10)
	HID_LCD_OPS_CODE_DISP_ADS = 0x21,//plays ads
}EM_HID_LCD_OPS_CODE;//lcd ops code

typedef enum{
	HID_CON_OPS_CODE_READ_DEV_INFO = 0x00,
	HID_CON_OPS_CODE_FIRM_OTA,
	HID_CON_OPS_CODE_DOWNLOAD_FILE,
	HID_CON_OPS_CODE_DOWNLOAD_PIC_ZIP,
}EM_HID_CON_OPS_CODE;//lcd config code



//Constant

//Local variables
//Global variables
static T_Task s_tHIDTaskID = 0;
static char *s_pcRead = NULL;
//Function implementation
extern int HIDTaskEvent(EM_YMI_EVENT_ID eID, void *pvName, void *pvData, uint wDataLen);


static int ReadDevInfo(char *pcOutData, int *piOutDataLen)
{
	uint wLen, wOff = 0;
	char szBuff[64] = "";
	if(pcOutData == NULL || piOutDataLen == NULL)
	{
		return RET_FAIL;
	}
	*piOutDataLen = 0;
	wLen = 0;
	// YMI_SysGetDevInfo(DEV_INFO_SN, &wLen, szBuff);
	if(wLen == 0)//for test
	{
		// wLen = strlen(SysInfoGet()->szDeviceSN);
		// memcpy(szBuff, SysInfoGet()->szDeviceSN, wLen);
		wLen = strlen(DEV_SN);
		memcpy(szBuff, DEV_SN, wLen);
	}
	memcpy(pcOutData + wOff, szBuff, wLen);
	wOff += wLen;
	pcOutData[wOff] = 0x00;
	wOff ++;
	wLen = 0;
	memset(szBuff, 0x00, sizeof(szBuff));
	wLen = strlen(HW_VER);
	memcpy(pcOutData + wOff, HW_VER, wLen);
	wOff += wLen;
	pcOutData[wOff] = 0x00;
	wOff ++;
	wLen = strlen(SBI_VER);
	memcpy(pcOutData + wOff, SBI_VER, wLen);
	wOff += wLen;
	pcOutData[wOff] = 0x00;
	wOff ++;
	wLen = strlen(OS_VER);
	memcpy(pcOutData + wOff, OS_VER, wLen);
	wOff += wLen;
	pcOutData[wOff] = 0x00;
	wOff ++;
	wLen = strlen(APP_VER);
	memcpy(pcOutData + wOff, APP_VER, wLen);
	wOff += wLen;
	pcOutData[wOff] = 0x00;
	wOff ++;
	*piOutDataLen = wOff;
	return RET_SUCC;
}

static int DownLoadFile(int iCode, bool bFirstPack, char *pcInData, int iInDataLen)
{
	static ulong s_wPackLen = 0, s_wPackDataSaveOff = 0, s_wTotalSaveOff = 0;
	static ulong s_wOffSet = 0, s_wDataLen = 0, s_wTotalLen = 0;
	static char *s_pcSave = NULL;
	static ulong s_wSaveOff = 0;
	static char *s_pszSaveFile = NULL;
	int iOff = 0;
	ulong iRet;
	if(bFirstPack)
	{
		s_wPackLen = 0;
		s_wOffSet = 0;
		s_wDataLen = 0;
		s_wTotalLen = 0;
		s_wPackDataSaveOff = 0;
		s_wTotalSaveOff = 0;

		s_wPackLen = (pcInData[iOff] << 8) | pcInData[iOff + 1];
		iOff += 2;
		s_wOffSet = (pcInData[iOff] << 24) | (pcInData[iOff + 1] << 16) | (pcInData[iOff + 2] << 8) | pcInData[iOff + 3];
		iOff += 4;
		s_wDataLen = (pcInData[iOff] << 24) | (pcInData[iOff + 1] << 16) | (pcInData[iOff + 2] << 8) | pcInData[iOff + 3];
		iOff += 4;
		s_wTotalLen = (pcInData[iOff] << 24) | (pcInData[iOff + 1] << 16) | (pcInData[iOff + 2] << 8) | pcInData[iOff + 3];
		iOff += 4;
		if(s_wDataLen + 12 != s_wPackLen)
		{
			HID_LOG("[ERR]pack len err datalen=%d, packlen=%d", s_wDataLen, s_wPackLen);
			return RET_FAIL;
		}
		if(s_wOffSet == 0)
		{//first data pack
			s_wSaveOff = 0;
			YMI_FsGetDiskSpace(EM_FS_GET_SPACE_UNUSED, &s_wSaveOff);
			if(s_wTotalLen >= s_wSaveOff / 2)
			{
				HID_LOG("[ERR]fs no space(%d) to save file=%d", s_wSaveOff, s_wTotalLen);
				return RET_FAIL;
			}
			s_pszSaveFile = OTA_FILE_NAME;
			// if(memcmp(pcInData + iOff, RES_FILE_HEAD_CODE, strlen(RES_FILE_HEAD_CODE)) == 0)
			HID_LOG("download code=%d===================>", iCode);
			if(iCode == HID_CON_OPS_CODE_DOWNLOAD_FILE)
			{
				s_pszSaveFile = RES_FILE_NAME;
			}
			else if(iCode == HID_CON_OPS_CODE_DOWNLOAD_PIC_ZIP)
			{
				s_pszSaveFile = PIC_ZIP_FILE_NAME;
			}
			s_wSaveOff = 0;
			if(s_pcSave)
			{
				FREE(s_pcSave);
				s_pcSave = NULL;
			}
			iRet = s_wTotalLen > HID_DOWNLOAD_FILE_MALLOC_MAX_SIZE ? HID_DOWNLOAD_FILE_MALLOC_MAX_SIZE : s_wTotalLen;
			s_pcSave = MALLOC(iRet, char);
			if(s_pcSave == NULL)
			{
				HID_LOG("[ERR]save data malloc:%d", iRet);
				return RET_FAIL;
			}
			if(iCode == HID_CON_OPS_CODE_FIRM_OTA)
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_OTA_START, s_pszSaveFile, NULL, 0);
			}
			else if(iCode == HID_CON_OPS_CODE_DOWNLOAD_FILE)
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_RES_START, s_pszSaveFile, NULL, 0);
			}
			else
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_PIC_ZIP_START, s_pszSaveFile, NULL, 0);
			}
			if (iRet != RET_SUCC)
			{
				return RET_FAIL;
			}
		}
		// HID_LOG("pack len=%d, offset=%ld, datalen=%d, totallen=%d, save add=%04x", 
		// 	s_wPackLen, s_wOffSet, s_wDataLen, s_wTotalLen, s_pcSave);
	}
	if(s_pcSave == NULL)
	{
		HID_LOG("[ERR]not malloc save buff");
		return RET_FAIL;
	}
	if (s_wTotalLen > HID_DOWNLOAD_FILE_MALLOC_MAX_SIZE && 
		HID_DOWNLOAD_FILE_MALLOC_MAX_SIZE - s_wSaveOff < iInDataLen - iOff)
	{
		iRet = HID_DOWNLOAD_FILE_MALLOC_MAX_SIZE - s_wSaveOff;
		memcpy(s_pcSave + s_wSaveOff, pcInData + iOff, iRet);
		iOff += iRet;
		s_wSaveOff += iRet;
		s_wPackDataSaveOff += iRet;
		s_wTotalSaveOff += iRet;
		if (strcmp(s_pszSaveFile, OTA_FILE_NAME) == 0)
		{
			iRet = DevEventSent(DEV_EVENT_ID_SAVE_OTA, s_pszSaveFile, s_pcSave, s_wSaveOff);
		}
		else if (strcmp(s_pszSaveFile, RES_FILE_NAME) == 0)
		{
			iRet = DevEventSent(DEV_EVENT_ID_SAVE_RES, s_pszSaveFile, s_pcSave, s_wSaveOff);
		}
		else
		{
			iRet = DevEventSent(DEV_EVENT_ID_SAVE_PIC_ZIP, s_pszSaveFile, s_pcSave, s_wSaveOff);
		}
		s_pcSave = NULL;
		if(iRet != RET_SUCC)
		{
			return RET_FAIL;
		}
		iRet = s_wTotalLen - s_wTotalSaveOff;
		if(iRet > HID_DOWNLOAD_FILE_MALLOC_MAX_SIZE)
		{
			iRet = HID_DOWNLOAD_FILE_MALLOC_MAX_SIZE;
		}
		s_pcSave = MALLOC(iRet, char);
		if(s_pcSave == NULL)
		{
			if (strcmp(s_pszSaveFile, OTA_FILE_NAME) == 0)
			{
				DevEventSent(DEV_EVENT_ID_SAVE_OTA_FAILS, s_pszSaveFile, NULL, 0);
			}
			else if (strcmp(s_pszSaveFile, RES_FILE_NAME) == 0)
			{
				DevEventSent(DEV_EVENT_ID_SAVE_RES_FAILS, s_pszSaveFile, NULL, 0);
			}
			else
			{
				DevEventSent(DEV_EVENT_ID_SAVE_PIC_ZIP_FAILS, s_pszSaveFile, NULL, 0);
			}
			HID_LOG("[ERR]save data malloc:%d", iRet);
			return RET_FAIL;
		}
		s_wSaveOff = 0;
	}
	iRet = iInDataLen - iOff;
	memcpy(s_pcSave + s_wSaveOff, pcInData + iOff, iRet);
	iOff += iRet;
	s_wSaveOff += iRet;
	s_wPackDataSaveOff += iRet;
	s_wTotalSaveOff += iRet;
	if(s_wPackDataSaveOff == s_wDataLen + 1)//one pack over
	{
		s_wPackDataSaveOff = 0;
		s_wSaveOff -= 1;
		if(s_wOffSet + s_wDataLen == s_wTotalLen)
		{
			YMI_HIDClearRecvBuffer();
			if (strcmp(s_pszSaveFile, OTA_FILE_NAME) == 0)
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_OTA, s_pszSaveFile, s_pcSave, s_wSaveOff);
			}
			else if (strcmp(s_pszSaveFile, RES_FILE_NAME) == 0)
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_RES, s_pszSaveFile, s_pcSave, s_wSaveOff);
			}
			else 
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_PIC_ZIP, s_pszSaveFile, s_pcSave, s_wSaveOff);
			}
			s_wSaveOff = 0;
			s_wTotalSaveOff = 0;
			s_pcSave = NULL;
			if(iRet != RET_SUCC)
			{
				return RET_FAIL;
			}
			if (strcmp(s_pszSaveFile, OTA_FILE_NAME) == 0)
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_OTA_DONE, s_pszSaveFile, NULL, s_wTotalLen);
			}
			else if (strcmp(s_pszSaveFile, RES_FILE_NAME) == 0)
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_RES_DONE, s_pszSaveFile, NULL, s_wTotalLen);
			}
			else
			{
				iRet = DevEventSent(DEV_EVENT_ID_SAVE_PIC_ZIP_DONE, s_pszSaveFile, NULL, s_wTotalLen);
			}
			s_pszSaveFile = NULL;
			if(iRet != RET_SUCC)
			{
				return RET_FAIL;
			}
		}
		return RET_SUCC;
	}
	else if(s_wPackDataSaveOff > s_wDataLen + 1)
	{
		HID_LOG("[ERR]save over data len %d>%d", s_wPackDataSaveOff, s_wDataLen + 1);
		FREE(s_pcSave);
		s_wPackDataSaveOff = 0;
		s_wSaveOff = 0;
		s_wTotalSaveOff = 0;
		if (strcmp(s_pszSaveFile, OTA_FILE_NAME) == 0)
		{
			DevEventSent(DEV_EVENT_ID_SAVE_OTA_FAILS, s_pszSaveFile, NULL, 0);
		}
		else if (strcmp(s_pszSaveFile, RES_FILE_NAME) == 0)
		{
			DevEventSent(DEV_EVENT_ID_SAVE_RES_FAILS, s_pszSaveFile, NULL, 0);
		}
		else
		{
			DevEventSent(DEV_EVENT_ID_SAVE_PIC_ZIP_FAILS, s_pszSaveFile, NULL, 0);
		}
		s_pszSaveFile = NULL;
		return RET_FAIL;
	}
	return RET_OTHER;
}
static int LCDStopAdv(void)
{
	
	if(DevEventSent(DEV_EVENT_ID_STOP_ADS, NULL, NULL, 0) != RET_SUCC)
	{
		return RET_FAIL;
	}
	return RET_SUCC;
}
static int UidAnaly(void *pvInVal, void *pvOutVal)
{
	if (!pvInVal || !pvOutVal || !strlen(pvInVal))
	{
		return -1;
	}
	
	char *pcEnd = NULL, *pcStart = NULL;
	pcStart = strstr(pvInVal, "pa=");
	if (!pcStart)
	{
		pcStart = strstr(pvInVal, "PA=");
	}
	if (pcStart == NULL)
	{
		return -1;
	}

	pcStart += 3;
	pcEnd = strchr(pcStart, '&');
	if (!pcEnd)
	{
		pcEnd = pcStart + strlen(pcStart);
	}
	memcpy(pvOutVal, pcStart, pcEnd - pcStart);

	return 0;
}
static int LCDShow(int iCode, char *pcInData, int iInDataLen)
{
	int iOff = 0, iPackLen;
	int iLen = 0;
	char szQrcode[512] = {0};
	ST_DevPlayAds *pstPlay = NULL;
	ST_PicDispConf stPicUp = {
		.iX = 0,
		.iY = 0,
		.szFilePath = "",
		.wWidth = 800,
		.wHeight = 1280,
	};
	ST_TextDispConf stText = {
        .szText = "",
        .iX = 0,
        .iY = 410,
        .wBgColor = 0xE39B2C,
        .wFontColor = 0x000000,
        .wFontSize = 88,
    };
	ST_QRCodeDispConf stQRCode = {
        .szQRCode = "",
        .iX = 500,
        .iY = 20,
        .wSize = 240,
    };
	// ST_VideoDispConf stVideo = {
    //     .szFilePath = "", 
    //     .iX = 0,
    //     .iY = 0,
    //     .wWidth = 800,
    //     .wHeight = 600,
    //     .wTimes = VIDEO_PLAY_ON_LOOP,
    // };

	if(pcInData == NULL || iInDataLen == 0)
	{
		return RET_FAIL;
	}
	LCDStopAdv();
	if(iCode == HID_LCD_OPS_CODE_CLEAR)
	{
		sprintf(stPicUp.szFilePath, "%s", PIC_FILE_NAME_POWER_ON);
		LVScreenDisp(&stPicUp, NULL, NULL, NULL, NULL);
		return RET_SUCC;
	}

	iPackLen = (pcInData[iOff] << 8) | pcInData[iOff + 1];
	iOff += 2;
	if (iPackLen + iOff != iInDataLen - 1)
	{
		HID_LOG("[ERR]pack len err datalen=%d, packlen=%d", iInDataLen, iPackLen);
		return RET_FAIL;
	}
	if(iCode == HID_LCD_OPS_CODE_DISP_ADS)
	{
		pstPlay = MALLOC(1, ST_DevPlayAds);
		if(pstPlay == NULL)
		{
			HID_LOG("[ERR]malloc play ads=%d", sizeof(ST_DevPlayAds));
			return RET_FAIL;
		}
		memset(pstPlay, 0x00, sizeof(ST_DevPlayAds));
		pstPlay->iIndex = pcInData[iOff];
		iOff ++;
		pstPlay->iTimeMS = (pcInData[iOff] << 8) | pcInData[iOff + 1];
		iOff += 2;
		pstPlay->cTransMode = pcInData[iOff];
		iOff ++;
		pstPlay->cDispArea = pcInData[iOff];
		HID_LOG("play index=%d, time=%d", pstPlay->iIndex, pstPlay->iTimeMS);
		if(DevEventSent(DEV_EVENT_ID_PLAY_ADS, NULL, pstPlay, sizeof(ST_DevPlayAds)) != RET_SUCC)
		{
			return RET_FAIL;
		}
		return RET_SUCC;
	}
	iLen = (pcInData[iOff] << 8) | pcInData[iOff + 1];
	iOff += 2;
	if(strstr(pcInData + iOff, CURRENCY_SYMBOL) == NULL)
	{
		memcpy(stText.szText, CURRENCY_SYMBOL, sizeof(CURRENCY_SYMBOL));
		memcpy(stText.szText + sizeof(CURRENCY_SYMBOL) - 1, pcInData + iOff, iLen);
		HID_LOG("text=%s", stText.szText);
	}
	else
	{
		memcpy(stText.szText, pcInData + iOff, iLen);
	}
	iOff += iLen;
	if (iCode == HID_LCD_OPS_CODE_SHOW_AMT_TR_SUCC)
	{
		sprintf(stPicUp.szFilePath, "%s", PIC_FILE_NAME_TRANS_SUC);
	}
	else if (iCode == HID_LCD_OPS_CODE_SHOW_AMT_TR_FAIL)
	{
		stText.iY = 650;
		stText.wBgColor = 0xFFFFFF;
		sprintf(stPicUp.szFilePath, "%s", PIC_FILE_NAME_TRANS_FAIL);
	}
	if (iCode == HID_LCD_OPS_CODE_SHOW_AMT_TR_SUCC || iCode == HID_LCD_OPS_CODE_SHOW_AMT_TR_FAIL)
	{
		LVScreenDisp(&stPicUp, NULL, &stText, NULL, NULL);
		return RET_SUCC;
	}
	iLen = (pcInData[iOff] << 8) | pcInData[iOff + 1];
	iOff += 2;
	memcpy(szQrcode, pcInData + iOff, iLen);
	iOff += iLen;
	if((iCode == HID_LCD_OPS_CODE_SHOW_AMT_QR_STR) || (iCode == HID_LCD_OPS_CODE_SHOW_AMT_QR_BASE64))
	{
		ST_TextDispConf stTextLabel = {
			.szText = "",
			.iX = 0,
			.iY = 745,
			.wBgColor = 0xFFFFFF,
			.wFontColor = 0x525252,
			.wFontSize = 30,
		};
		sprintf(stQRCode.szQRCode, "%s", szQrcode);
		if (iCode == HID_LCD_OPS_CODE_SHOW_AMT_QR_BASE64)
		{
			char szBa64[256] = {0};
			int iLen;
			mbedtls_base64_decode(szBa64, sizeof(szBa64), &iLen, szQrcode, strlen(szQrcode));
			memcpy(stQRCode.szQRCode, szBa64, iLen);
			stQRCode.szQRCode[iLen] = 0;
		}
		memcpy(stTextLabel.szText, "UPI ID: ", strlen("UPI ID: "));
		UidAnaly(stQRCode.szQRCode, stTextLabel.szText + strlen(stTextLabel.szText));
		stText.iY = 130;
		stText.wBgColor = 0xE39B2C;
		stQRCode.iX = 185;
		stQRCode.iY = 300;
		stQRCode.wSize = 430;
		sprintf(stPicUp.szFilePath, "%s", PIC_FILE_NAME_TRANS_QR);
		LVScreenDisp(&stPicUp, &stQRCode, &stText, NULL, &stTextLabel);
	}

	return RET_SUCC;
}

static int CmdProce(EM_HID_CMD_TYPE eType, int iCode, char *pcInData, int iInDataLen, char *pcOutData, int *piOutDataLen)
{
	int iLen = 0;
	int iRet = RET_SUCC;
	static bool bFirstPack = TRUE;
	
	if(pcInData == NULL || iInDataLen == 0)
	{
		return RET_FAIL;
	}
	switch (eType)
	{
	case HID_CMO_TYPE_LCD:
			switch (iCode)
			{
			case HID_LCD_OPS_CODE_CLEAR:
			case HID_LCD_OPS_CODE_SHOW_AMT_QR_BASE64:
			case HID_LCD_OPS_CODE_SHOW_AMT_TR_SUCC:
			case HID_LCD_OPS_CODE_SHOW_AMT_TR_FAIL:
			case HID_LCD_OPS_CODE_SHOW_AMT_QR_STR:
			case HID_LCD_OPS_CODE_DISP_ADS:
				iRet = LCDShow(iCode, pcInData, iInDataLen);
				break;
			default:
				iRet = RET_FAIL;
				break;
			}
		break;
	case HID_CMD_TYPE_CONFIG:
			switch (iCode)
			{
			case HID_CON_OPS_CODE_READ_DEV_INFO:
				HID_LOG("hid ops read dev info");
				iRet = ReadDevInfo(pcOutData + HID_PACK_RESP_RET_CODE_LEN, &iLen);
				break;
			case HID_CON_OPS_CODE_DOWNLOAD_FILE:
			case HID_CON_OPS_CODE_DOWNLOAD_PIC_ZIP:
			case HID_CON_OPS_CODE_FIRM_OTA:
				// HID_LOG("hid ops download file");
				//sub,each package contain 4K
				iRet = DownLoadFile(iCode, bFirstPack, pcInData, iInDataLen);
				if(iRet == RET_OTHER)
				{
					bFirstPack = FALSE;
					return iRet;
				}
				break;
			default:
				iRet = RET_FAIL;
				break;
			}
		break;
	default:
		break;
	}

	bFirstPack = TRUE;
	memset(pcOutData, 0x00, HID_PACK_RESP_RET_CODE_LEN);
	pcOutData[HID_PACK_RESP_RET_CODE_LEN - 1] = (iRet == RET_SUCC ? 0x00 : 0x01);
	*piOutDataLen = iLen + HID_PACK_RESP_RET_CODE_LEN;

	return iRet;
}


static int PackResProce(int iSeq, int iType, int iCode, char *pcInData, int iDataLen, char *pcOutData, int *piOutDataLen)
{
	int iOff = 0, i;
	char cSum = 0x00;
	if(pcOutData == NULL || piOutDataLen == NULL)
	{
		return RET_FAIL;
	}
	pcOutData[iOff] = HID_PACK_HEAD;
	iOff += 1;
	pcOutData[iOff] = iSeq & 0xFF;
	iOff += 1;
	pcOutData[iOff] = iType & 0xFF;
	iOff += 1;
	pcOutData[iOff] = iCode & 0xFF;
	iOff += 1;
	pcOutData[iOff] = (iDataLen >> 8) & 0xFF;
	pcOutData[iOff + 1] = iDataLen & 0xFF;
	iOff += 2;
	if(iDataLen > 0)
	{
		memcpy(pcOutData + iOff, pcInData, iDataLen);
		iOff += iDataLen;
	}
	for(i = 0; i < iOff; i ++)
	{
		cSum ^= pcOutData[i];
	}
	pcOutData[iOff] = cSum;
	iOff += 1;
	*piOutDataLen = iOff;
	return RET_SUCC;
}
static int CheckPackSum(char *pcPack, int iPackLen)
{
	int i;
	char cSum = 0x00;
	for(i = 0; i < iPackLen - 1; i ++)
	{
		cSum ^= pcPack[i];
	}
	if(cSum != pcPack[iPackLen - 1])
	{
		HID_LOG("[ERR]hid pack check sum %02x != %02x", cSum, pcPack[iPackLen - 1]);
		return RET_FAIL;
	}
	return RET_SUCC;

}
static int HIDDataProce(char *pcInData, int iInDataLen, char *pcOutData, int *piOutDataLen)
{
	int iOff = 0, iLen = 0;
	static int iSeq, iCmdType, iCode;
	int iRet = RET_SUCC;
	char szBuff[128] = "";
	static bool bDataInOnePack = TRUE;

	if(pcInData == NULL || iInDataLen == 0)
	{
		return RET_FAIL;
	}
	if(bDataInOnePack)
	{
		// if(CheckPackSum(pcInData + iOff, iPackLen) != RET_SUCC)
		// {
		// 	return RET_FAIL;
		// }
		if(pcInData[iOff] != HID_PACK_HEAD)
		{
			HID_LOG("[ERR]hid pack head=%02x", pcInData[iOff]);
			return RET_FAIL;
		}
		iOff += 1;
		iSeq = pcInData[iOff];
		iOff += 1;
		iCmdType = pcInData[iOff];
		iOff += 1;
		iCode = pcInData[iOff];
		iOff += 1;
	}
	iRet = CmdProce(iCmdType, iCode, pcInData + iOff, iInDataLen - iOff, szBuff, &iLen);
	if(iRet == RET_OTHER)
	{
		bDataInOnePack = FALSE;
	}
	else
	{
		bDataInOnePack = TRUE;
	}
	
	if(iRet == RET_SUCC)
	{
		return PackResProce(iSeq, iCmdType, iCode, szBuff, iLen, pcOutData, piOutDataLen);
	}
	return iRet;
}


void HIDTask(void *pvParam)
{
	ST_Event stEvent;
	bool bRun = TRUE;
	int iRet, iReadLen = 0, iWriteLen;
	char szWriteBuff[128] = "";

	while (bRun)
	{
		memset(s_pcRead, 0x00, HID_READ_BUF_SIZE);
		iReadLen = 0;
		iRet = YMI_HIDRead(HID_READ_BUF_SIZE, s_pcRead, 20, &iReadLen);
		if(iRet == YMI_OK && iReadLen)
		{
			// HID_LOG("hid read len=%d", iReadLen);
			if(iReadLen <= 64)
			{
				// LOG_HEX("read data", s_pcRead, iReadLen);
			}
			iRet = HIDDataProce(s_pcRead, iReadLen, szWriteBuff, &iWriteLen);
			// HID_LOG("hid data proce ret=%d, writelen=%d", iRet, iWriteLen);
			if(iRet == RET_SUCC && iWriteLen)
			{
				iReadLen = 0;
				iRet = YMI_HIDWrite(iWriteLen, szWriteBuff, 20, &iReadLen);
				if(iRet == RET_SUCC && iReadLen == iWriteLen)
				{
					// HID_LOG("hid write len=%d", iWriteLen);
					if (iReadLen <= 64)
					{
						// LOG_HEX("write data", szWriteBuff, iReadLen);
					}
				}
				else
				{
					HID_LOG("[ERR]hid write err ret=%d, %d!=%d", iRet, iReadLen, iWriteLen);
				}
			}
			else if(iRet == RET_QUIT)
			{
				//pack loss reprocess
				HID_LOG("reprocess");
				iRet = HIDDataProce(s_pcRead, iReadLen, szWriteBuff, &iWriteLen);
				if(iRet == RET_SUCC && iWriteLen)
				{
					iReadLen = 0;
					iRet = YMI_HIDWrite(iWriteLen, szWriteBuff, 20, &iReadLen);
					if(iRet == RET_SUCC && iReadLen == iWriteLen)
					{
						HID_LOG("hid write len=%d", iWriteLen);
						if (iReadLen <= 64)
						{
							// LOG_HEX("write data", szWriteBuff, iReadLen);
						}
					}
					else
					{
						HID_LOG("[ERR]hid write err ret=%d, %d!=%d", iRet, iReadLen, iWriteLen);
					}
				}
			}
			else if(iRet == RET_FAIL)
			{
				HID_LOG("[ERR]hid pack proce fails");
				YMI_HIDClearRecvBuffer();
			}
		}
		memset(&stEvent, 0x00, sizeof(ST_Event));
		if(YMI_EventWait(&stEvent, HID_EVENT_WAIT_TIME_MS) == RET_SUCC)
		{
			switch (stEvent.wId)
			{
			case TASK_MANAGE_EVENT_ID:
				if(stEvent.wParam1 == TASK_OPT_EXIT)
				{
					bRun = FALSE;
				}
				break;
			default:
				break;
			}
		}
	}
	FREE(s_pcRead);
	YMI_HIDClearRecvBuffer();
	YMI_HIDClose();
	HID_LOG("============>out hid task");
	YMI_TaskDelete(s_tHIDTaskID);
}


int HIDTaskInit(void)
{
	int iRet;
	iRet = YMI_HIDOpen();
	if(iRet != YMI_OK)
	{
		HID_LOG("[ERR]hid open ret=%d", iRet);
		return RET_FAIL;
	}
	YMI_HIDClearRecvBuffer();
	s_pcRead = MALLOC(HID_READ_BUF_SIZE, char);
	if(s_pcRead == NULL)
	{
		HID_LOG("[ERR]hid read malloc err", iRet);
		return RET_FAIL;
	}

	YMI_FsDelete(PIC_FILE_NAME_SAVE);
	iRet = YMI_TaskCreat(&s_tHIDTaskID, "hid_task", HID_TASK_STACK_SIZE, HIDTask, NULL, EM_TASK_PRIORITY_NORMAL, HID_TASK_EVENT_NUM);
	if(iRet != YMI_OK)
	{
		HID_LOG("[ERR]hid task creat ret=%d", iRet);
		return RET_FAIL;
	}
	return RET_SUCC;
}

