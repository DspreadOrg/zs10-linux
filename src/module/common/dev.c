/**********************************************************************
 * Copyright (C)    Dspread
 * File name: dev.c
 * Content summary: device-related APIs
 *
 ***********************************************************************/
// Header file
#include "var.h"

// Macro definitions
#define DEV_LOG_MODE
#ifdef DEV_LOG_MODE
#define DEV_LOG(msg, ...) LOG("[DEV]" msg "", ##__VA_ARGS__)
#else
#define DEV_LOG(msg, ...)
#endif

#define DEV_TASK_STACK_SIZE (2 * 1024)
#define DEV_TASK_EVENT_NUM (10)
#define DEV_EVENT_WAIT_TIME_MS (50)

#define RES_FILE_HEAD_SIZE (44)
#define RES_DATA_HASH_SIZE (32)
#define RES_DATA_HEAD_SIZE (20)
#define RES_DATA_RES_NAME_SIZE (16)
#define RES_DATA_RES_NAME_EXTENSION_JPG ".jpg"
#define RES_DATA_FILE_DIR1 "res_dir1"
#define RES_DATA_FILE_DIR2 "res_dir2"


#define DEV_INFO_FILE_DIR "dev"
#define DEV_INFO_FILE_NAME "devinfo.txt"
#define DEV_INFO_FILE_PATH ""DEV_INFO_FILE_DIR"/"DEV_INFO_FILE_NAME""
#define DEV_INFO_RES_FILE_MAX_NUM (10)
#define READ_RES_DATA_MAX_SIZE (120 * 1024)
#define DEV_PLAY_ADS_DEFAULT_TIMEOUT (2 * 1000)
// Constant
typedef struct res_info
{
	char cFlag;
	char szName[32];
} ST_RES_INFO;

typedef struct dev_info
{
	char *pszResDir;// Used to distinguish the same file name
	ST_DevPlayAds stPlay;
	uint wResNum;
	ST_RES_INFO astResInfo[DEV_INFO_RES_FILE_MAX_NUM];
} ST_DevInfo;

// Local variables
static T_Task s_tDevTaskID = 0;
static ST_DevInfo s_stDevInfo;

// Global variables
// Function implementation
void ResDataFileRest(void)
{
	YMI_FsDelete(RES_FILE_NAME);
	YMI_FsDeleteDir(RES_DATA_FILE_DIR1);
	YMI_FsDeleteDir(RES_DATA_FILE_DIR2);
	YMI_FsMakeDir(RES_DATA_FILE_DIR1);
	YMI_FsMakeDir(RES_DATA_FILE_DIR2);
}

int DevInfoDataUnpack(char *pszData, uint wDataLen, ST_DevInfo *pstDevInfo)
{
	if (pszData == NULL || pstDevInfo == NULL)
	{
		return RET_FAIL;
	}
	if (wDataLen != sizeof(ST_DevInfo))
	{
		DEV_LOG("[ERR]dev info size %d!=%d", wDataLen, sizeof(ST_DevInfo));
		return RET_OTHER;
	}

	memcpy(pstDevInfo, pszData, wDataLen);
	return RET_SUCC;
}

int DevInfoInit(ST_DevInfo *pstDevInfo)
{
	T_Fd iFd = 0;
    int iRet;
	ulong wFileSize;
	char *pszData = NULL;

	iRet = YMI_FsFileSize(DEV_INFO_FILE_PATH, &wFileSize);
	if (iRet != RET_SUCC || wFileSize == 0)
	{
		DEV_LOG("no dev info file");
		return RET_FAIL;
	}
	pszData = MALLOC(wFileSize, char);
	if (pszData == NULL)
	{
		DEV_LOG("[ERR]malloc %s file size=%d", DEV_INFO_FILE_PATH, wFileSize);
		return RET_FAIL;
	}
	iFd = YMI_FsOpen(DEV_INFO_FILE_PATH, "r");
	if (iFd <= 0)
	{
		DEV_LOG("[ERR]fs open %s", DEV_INFO_FILE_PATH);
		iRet = RET_FAIL;
		goto exit;
	}
	iRet = YMI_FsRead(iFd, pszData, wFileSize);
	if (iRet != wFileSize)
	{
		DEV_LOG("[ERR]fs read %d!=%d", iRet, wFileSize);
		iRet = RET_FAIL;
		goto exit;
	}
	iRet = DevInfoDataUnpack(pszData, wFileSize, pstDevInfo);
	if (iRet == RET_OTHER)
	{
		DEV_LOG("[ERR]dev info not match del old file");
		YMI_FsDelete(DEV_INFO_FILE_PATH);
		ResDataFileRest();
		iRet = RET_FAIL;
	}
exit:
	YMI_FsClose(iFd);
	FREE(pszData);
	return iRet;
}
int DevInfoSave(void *pvDevInfo, uint wSize)
{
	T_Fd iFd = 0;
    int iRet;

	if (pvDevInfo == NULL || wSize == 0)
	{
		return RET_FAIL;
	}
	YMI_FsDelete(DEV_INFO_FILE_PATH);
	iFd = YMI_FsOpen(DEV_INFO_FILE_PATH, "w");
	if (iFd <= 0)
	{
		DEV_LOG("[ERR]fs open %s", DEV_INFO_FILE_PATH);
		return RET_FAIL;
	}
	YMI_FsSeek(iFd, 0, EM_FS_SEEK_STA);
	iRet = YMI_FsWrite(iFd, (char *)pvDevInfo, wSize);
	if (iRet != wSize)
	{
		DEV_LOG("[ERR]fs read %d!=%d", iRet, wSize);
		YMI_FsClose(iFd);
		return RET_FAIL;
	}
	YMI_FsClose(iFd);
	return RET_SUCC;
}

int DevEventSent(EM_YMI_EVENT_ID eID, void *pvName, void *pvData, ulong wDataLen)
{
	int iRet;
	ST_Event stEvent;

	memset(&stEvent, 0x00, sizeof(ST_Event));
	stEvent.wId = eID;
	stEvent.wParam1 = (uint)pvName;
	stEvent.wParam2 = (uint)pvData;
	stEvent.wParam3 = wDataLen;
	iRet = YMI_EventSend(s_tDevTaskID, &stEvent);
	if (iRet != RET_SUCC)
	{
		DEV_LOG("hid event send ret=%d", iRet);
		FREE(pvData);
		return RET_FAIL;
	}
	return RET_SUCC;
}
int DevEventResSave(ST_Event *pstEvent)
{
	T_Fd iFd = 0;
    int iRet;
	char *pszFileName, *pszData;
	uint wDataLen;

	if (pstEvent == NULL)
	{
		return RET_FAIL;
	}

	pszFileName = (char *)pstEvent->wParam1;
	pszData = (char *)pstEvent->wParam2;
	wDataLen = pstEvent->wParam3;

	DEV_LOG("ready to save file=%s, data=%04x, size=%d", pszFileName, pszData, wDataLen);
    if (YMI_FsExist(pszFileName) != RET_SUCC)
    {
        iFd = YMI_FsOpen(pszFileName, "w");
    }
    else
    {
        iFd = YMI_FsOpen(pszFileName, "r+");
    }
	if (iFd <= 0)
	{
		DEV_LOG("[ERR]%s file open ret=%d", pszFileName, iFd);
		return RET_FAIL;
	}
	iRet = YMI_FsSeek(iFd, 0, EM_FS_SEEK_END);
	if (iRet != RET_SUCC)
	{
		DEV_LOG("[ERR]%s file seek ret=%d", pszFileName, iRet);
		YMI_FsClose(iFd);
		return RET_FAIL;
	}
	iRet = YMI_FsWrite(iFd, pszData, wDataLen);
	if (iRet != wDataLen)
	{
		DEV_LOG("[ERR]%s file write ret=%d", pszFileName, iRet);
		YMI_FsClose(iFd);
		return RET_FAIL;
	}
	FREE(pszData);
	YMI_FsClose(iFd);
	DEV_LOG("save file done================>");

	return RET_SUCC;
}

int DevEventResDel(ST_Event *pstEvent)
{
	char *pszFileName;

	if (pstEvent == NULL)
	{
		return RET_FAIL;
	}

	pszFileName = (char *)pstEvent->wParam1;
	return YMI_FsDelete(pszFileName);
}

int DevEventPlayAds(ST_Event *pstEvent, ST_DevPlayAds **ppstPlay)
{
	if (pstEvent == NULL || pstEvent->wParam3 != sizeof(ST_DevPlayAds))
	{
		return RET_FAIL;
	}
	*ppstPlay = (ST_DevPlayAds *)pstEvent->wParam2;
	if(*ppstPlay == NULL)
	{
		return RET_FAIL;
	}
	if ((*ppstPlay)->iTimeMS == 0)
	{
		(*ppstPlay)->iTimeMS = DEV_PLAY_ADS_DEFAULT_TIMEOUT;
	}
	return RET_SUCC;
}
int ResFileWrite(char *pszFileName, char *pszData, uint wDataLen)
{
	T_Fd iFd = 0;
    int iRet;
	if (pszFileName == NULL || pszData == NULL || wDataLen == 0)
	{
		return RET_FAIL;
	}

	if (YMI_FsExist(pszFileName) != RET_SUCC)
    {
        iFd = YMI_FsOpen(pszFileName, "w");
    }
    else
    {
        iFd = YMI_FsOpen(pszFileName, "r+");
    }
	if (iFd <= 0)
	{
		DEV_LOG("[ERR]open file %s err", pszFileName);
		return RET_FAIL;
	}
	YMI_FsSeek(iFd, 0, EM_FS_SEEK_END);
	iRet = YMI_FsWrite(iFd, pszData, wDataLen);
	if (iRet != wDataLen)
	{
		DEV_LOG("[ERR]%s file write ret=%d", pszFileName, iRet);
		YMI_FsClose(iFd);
		return RET_FAIL;
	}
	YMI_FsClose(iFd);
	return RET_SUCC;
}
int DevInfoDel(ST_DevInfo *pstDevInfo)
{
	if (pstDevInfo == NULL)
	{
		return RET_FAIL;
	}
	YMI_FsDeleteDir(pstDevInfo->pszResDir);
	return RET_SUCC;
}
int DevInfoUpDate(ST_DevInfo *pstDevInfo)
{
	if(pstDevInfo == NULL)
	{
		return RET_FAIL;
	}
	memset(pstDevInfo, 0x00, sizeof(ST_DevInfo));
	if(s_stDevInfo.pszResDir == NULL || strcmp(s_stDevInfo.pszResDir, RES_DATA_FILE_DIR2) == 0)
	{
		pstDevInfo->pszResDir = RES_DATA_FILE_DIR1;
	}
	else
	{
		pstDevInfo->pszResDir = RES_DATA_FILE_DIR2;
	}
	return RET_SUCC;
}

int DevEventResSaveDone(ST_Event *pstEvent)
{
	char *pszFileName;
	ulong wFileSize;
    uint wDataOff, wDataLen;
	T_Fd iReadFd = 0;
    int iRet, iOff = 0, i = 0;
	char szBuff[64] = "", szFileName[32] = "", szHash[32] = "";
	ST_DevInfo *pstDevInfo = NULL;
	char *pszResData = NULL;

	if (pstEvent == NULL)
	{
		return RET_FAIL;
	}

	pszFileName = (char *)pstEvent->wParam1;
	iRet = YMI_FsFileSize(pszFileName, &wFileSize);
	if (iRet != RET_SUCC)
	{
		DEV_LOG("[ERR]%s file size get ret=%d", pszFileName, iRet);
		return RET_FAIL;
	}
	if (wFileSize != pstEvent->wParam3)
	{
		DEV_LOG("[ERR]save file size err != download file size %d!=%d", wFileSize, pstEvent->wParam3);
		return RET_FAIL;
	}
	iReadFd = YMI_FsOpen(pszFileName, "r");
	if (iReadFd < 0)
	{
		DEV_LOG("[ERR]open file %s err", pszFileName);
		return RET_FAIL;
	}
	iRet = YMI_FsRead(iReadFd, szBuff, RES_FILE_HEAD_SIZE);
	if (iRet != RES_FILE_HEAD_SIZE)
	{
		DEV_LOG("[ERR]read file ret=%d", iRet);
		iRet = RET_FAIL;
		goto exit;
	}

	if (memcmp(szBuff, RES_FILE_HEAD_CODE, RES_FILE_HEAD_CODE_SIZE))
	{
		LOG_HEX("res file head code", RES_FILE_HEAD_CODE, RES_FILE_HEAD_CODE_SIZE);
		LOG_HEX("[ERR]res file head", szBuff, RES_FILE_HEAD_CODE_SIZE);
		iRet = RET_FAIL;
		goto exit;
	}
	iOff += RES_FILE_HEAD_CODE_SIZE;
	wDataOff = (szBuff[iOff]) | (szBuff[iOff + 1] << 8) | (szBuff[iOff + 2] << 16) | (szBuff[iOff + 3] << 24);
	if (wDataOff != RES_FILE_HEAD_SIZE)
	{
		LOG_HEX("data off", szBuff + iOff, 4);
		DEV_LOG("[ERR]read file head size %d=%d", wDataOff, RES_FILE_HEAD_SIZE);
		iRet = RET_FAIL;
		goto exit;
	}
	iOff += 4;
	wDataLen = (szBuff[iOff]) | (szBuff[iOff + 1] << 8) | (szBuff[iOff + 2] << 16) | (szBuff[iOff + 3] << 24);
	if (wDataLen + wDataOff != wFileSize)
	{
		DEV_LOG("[ERR]read file data size %d=%d", wDataLen + wDataOff, wFileSize);
		iRet = RET_FAIL;
		goto exit;
	}
	if (wDataLen == 0)
	{
		DEV_LOG("[ERR]data len=%d", wDataLen);
		iRet = RET_FAIL;
		goto exit;
	}
	iOff += 4;
	memcpy(szHash, szBuff + iOff, RES_DATA_HASH_SIZE);
	iOff += RES_DATA_HASH_SIZE;
	pstDevInfo = MALLOC(1, ST_DevInfo);
	if (pstDevInfo == NULL)
	{
		DEV_LOG("[ERR]malloc dev info=%d", sizeof(ST_DevInfo));
		iRet = RET_FAIL;
		goto exit;
	}
	memset(pstDevInfo, 0x00, sizeof(ST_DevInfo));
	pszResData = MALLOC(READ_RES_DATA_MAX_SIZE, char);
	if (pszResData == NULL)
	{
		DEV_LOG("[ERR]malloc read res data size=%d", READ_RES_DATA_MAX_SIZE);
		iRet = RET_FAIL;
		goto exit;
	}
	DevInfoUpDate(pstDevInfo);
	i = 0;
	// ST_SHA256Context stSha256;
	// YMI_SHA256Starts(&stSha256);
	mbedtls_sha256_context stSha256;
	mbedtls_sha256_starts(&stSha256, 0);
	
	while (wDataLen)
	{
		iOff = 0;
		iRet = YMI_FsRead(iReadFd, szBuff, RES_DATA_HEAD_SIZE);
		if (iRet != RES_DATA_HEAD_SIZE)
		{
			DEV_LOG("[ERR]read file ret=%d", iRet);
			iRet = RET_FAIL;
			goto exit;
		}
		sprintf(szFileName, "%s", szBuff);
		mbedtls_sha256_update(&stSha256, (uchar *)szBuff, RES_DATA_HEAD_SIZE);
		iOff += RES_DATA_RES_NAME_SIZE;
		wFileSize = (szBuff[iOff]) | (szBuff[iOff + 1] << 8) | (szBuff[iOff + 2] << 16) | (szBuff[iOff + 3] << 24);
		wDataLen -= wFileSize + RES_DATA_HEAD_SIZE;
		if (wFileSize)
		{
			sprintf(pstDevInfo->astResInfo[i].szName, "%s/%s", pstDevInfo->pszResDir, szFileName);
			DEV_LOG("ready to save res file=%s", pstDevInfo->astResInfo[i].szName);
			while (wFileSize)
			{
				iOff = wFileSize > READ_RES_DATA_MAX_SIZE ? READ_RES_DATA_MAX_SIZE : wFileSize;
				iRet = YMI_FsRead(iReadFd, pszResData, iOff);
				if (iRet != iOff)
				{
					DEV_LOG("[ERR]read file ret=%d", iRet);
					iRet = RET_FAIL;
					goto exit;
				}
				if (ResFileWrite(pstDevInfo->astResInfo[i].szName, pszResData, iOff) != RET_SUCC)
				{
					DEV_LOG("[ERR]write res file off=%d, filesize=%d", iOff, wFileSize);
					iRet = RET_FAIL;
					goto exit;
				}
				mbedtls_sha256_update(&stSha256, (uchar *)pszResData, iOff);
				wFileSize -= iOff;
			}
		}
		i++;
		if(i >= DEV_INFO_RES_FILE_MAX_NUM)
		{
			DEV_LOG("[ERR]res file over max num=%d", DEV_INFO_RES_FILE_MAX_NUM);
			iRet = RET_FAIL;
			goto exit;
		}
	}
	iRet = RET_SUCC;
	// mbedtls_sha256_finish(&stSha256, (uchar *)szBuff);
	// if(memcmp(szBuff, szHash, 32))
	// {
	// 	DEV_LOG("[ERR]hash not same");
	// 	LOG_HEX("in hash", szHash, 32);
	// 	LOG_HEX("out hash", szBuff, 32);
	// 	iRet = RET_FAIL;
	// }
exit:
	YMI_FsClose(iReadFd);
	if (pstDevInfo)
	{
		pstDevInfo->wResNum = i;
	}
	if (iRet == RET_SUCC)
	{
		iRet = DevInfoSave(pstDevInfo, sizeof(ST_DevInfo));
	}
	if (iRet != RET_SUCC)
	{
		DevInfoDel(pstDevInfo);
	}
	pszFileName = (char *)pstEvent->wParam1;
	YMI_FsDelete(pszFileName);
	FREE(pszResData);
	FREE(pstDevInfo);
	return iRet;
}

int DevEventOtaSaveDone(ST_Event *pstEvent)
{
	char *pszFileName;
	ulong wFileSize;
	int iRet;

	if (pstEvent == NULL)
	{
		return RET_FAIL;
	}

	pszFileName = (char *)pstEvent->wParam1;
	iRet = YMI_FsFileSize(pszFileName, &wFileSize);
	if (iRet != RET_SUCC)
	{
		DEV_LOG("[ERR]%s file size get ret=%d", pszFileName, iRet);
		return RET_FAIL;
	}
	if (wFileSize != pstEvent->wParam3)
	{
		DEV_LOG("[ERR]save file size err != download file size %d!=%d", wFileSize, pstEvent->wParam3);
		return RET_FAIL;
	}

	OtaTaskInit(pszFileName);

	return iRet;
}

int DevEventPicZipSaveDone(ST_Event *pstEvent)
{
	char *pszFileName;
	ulong wFileSize;
	int iRet;

	if (pstEvent == NULL)
	{
		return RET_FAIL;
	}

	pszFileName = (char *)pstEvent->wParam1;
	iRet = YMI_FsFileSize(pszFileName, &wFileSize);
	if (iRet != RET_SUCC)
	{
		DEV_LOG("[ERR]%s file size get ret=%d", pszFileName, iRet);
		return RET_FAIL;
	}
	if (wFileSize != pstEvent->wParam3)
	{
		DEV_LOG("[ERR]save file size err != download file size %d!=%d", wFileSize, pstEvent->wParam3);
		return RET_FAIL;
	}
	UnzipFileTask(pszFileName);
	return iRet;
}
extern int PngFileDisp(char *pszFileName);

static void DevLcdShow(char *pszFileName)
{
	ST_PicDispConf stPic = {
		.iX = 0,
		.iY = 0,
		.szFilePath = "",
		.wWidth = 800,
		.wHeight = 1280,
	};
	if(pszFileName == NULL)
	{
		return;
	}
	sprintf(stPic.szFilePath, "%s", pszFileName);
	LVScreenDisp(&stPic, NULL, NULL, NULL, NULL);
	return;
}
static void DevResFileRest(void)
{
	ResDataFileRest();
	YMI_FsDelete(DEV_INFO_FILE_PATH);
}

static void DevTask(void *pvParam)
{
	bool bRun = TRUE;
	int iRet;
	ST_Event stEvent;
	ST_DevPlayAds *pstPlayAds = NULL;
	uint wTimeOut = 0;
	// DevResFileRest();
	YMI_FsDelete(RES_FILE_NAME);
	YMI_FsMakeDir(DEV_INFO_FILE_DIR);
	YMI_FsMakeDir(RES_DATA_FILE_DIR1);
	YMI_FsMakeDir(RES_DATA_FILE_DIR2);
	OTAFileReset();
	PicZipFileReset();
	memset(&s_stDevInfo, 0x00, sizeof(ST_DevInfo));
	DevInfoInit(&s_stDevInfo);
	// SLEEP_S(2);
	// DevLcdShow("res_dir1/Screen 7.png");
	while (bRun)
	{
		memset(&stEvent, 0x00, sizeof(ST_Event));
		if (YMI_EventWait(&stEvent, DEV_EVENT_WAIT_TIME_MS) == RET_SUCC)
		{
			DEV_LOG("get event id=%d", stEvent.wId);
			switch (stEvent.wId)
			{
			case TASK_MANAGE_EVENT_ID:
				if (stEvent.wParam1 == TASK_OPT_EXIT)
				{
					bRun = FALSE;
				}
				break;
			case DEV_EVENT_ID_STOP_ADS:
				s_stDevInfo.stPlay.cActive = FALSE;
				break;
			case DEV_EVENT_ID_PLAY_ADS:
				if (s_stDevInfo.wResNum == 0)
				{
					DEV_LOG("[ERR]no res file");
					break;
				}
				iRet = DevEventPlayAds(&stEvent, &pstPlayAds);
				if (iRet == RET_SUCC)
				{
					DEV_LOG("play info time=%d, index=%d", pstPlayAds->iTimeMS, pstPlayAds->iIndex);
					pstPlayAds->cActive = TRUE;
					pstPlayAds->iCnt = 0;
					memcpy(&s_stDevInfo.stPlay, pstPlayAds, sizeof(ST_DevPlayAds));
					FREE(pstPlayAds);
					wTimeOut = 0;
				}
				break;
			case DEV_EVENT_ID_SAVE_RES:
			case DEV_EVENT_ID_SAVE_OTA:
			case DEV_EVENT_ID_SAVE_PIC_ZIP:
				iRet = DevEventResSave(&stEvent);
				if (iRet != RET_SUCC)
				{
					DEV_LOG("[ERR]save file fails");
				}
				break;
			case DEV_EVENT_ID_SAVE_RES_START:
			case DEV_EVENT_ID_SAVE_RES_FAILS:
			case DEV_EVENT_ID_SAVE_PIC_ZIP_START:
			case DEV_EVENT_ID_SAVE_PIC_ZIP_FAILS:
				if(stEvent.wId == DEV_EVENT_ID_SAVE_RES_START || stEvent.wId == DEV_EVENT_ID_SAVE_PIC_ZIP_START)
				{
					s_stDevInfo.stPlay.cActive = FALSE;
				}
				DevEventResDel(&stEvent);
				break;
			case DEV_EVENT_ID_SAVE_RES_DONE:
				iRet = DevEventResSaveDone(&stEvent);
				if (iRet == RET_SUCC)
				{
					// stop play ads
					DevLcdShow(PIC_FILE_NAME_POWER_ON);
					s_stDevInfo.stPlay.cActive = FALSE;
					wTimeOut = 0;
					DevInfoDel(&s_stDevInfo);
					DevInfoInit(&s_stDevInfo);
					DEV_LOG("res save done");
				}
				break;
			case DEV_EVENT_ID_SAVE_OTA_START:
			case DEV_EVENT_ID_SAVE_OTA_FAILS:
				if(stEvent.wId == DEV_EVENT_ID_SAVE_OTA_START)
				{
					DevLcdShow(PIC_FILE_NAME_OTA);
				}
				else
				{
					DevLcdShow(PIC_FILE_NAME_POWER_ON);
				}
				s_stDevInfo.stPlay.cActive = FALSE;
				DevEventResDel(&stEvent);
				break;
			case DEV_EVENT_ID_SAVE_OTA_DONE:
				DEV_LOG("ota save done");
				DevEventOtaSaveDone(&stEvent);
				break;
			case DEV_EVENT_ID_SAVE_PIC_ZIP_DONE:
				DEV_LOG("pic zip save done");
				DevEventPicZipSaveDone(&stEvent);
				break;
			default:
				break;
			}
		}
		if (s_stDevInfo.stPlay.cActive && s_stDevInfo.wResNum)
		{
			if (s_stDevInfo.stPlay.iIndex == 0) // auto play with time
			{
				if (wTimeOut == 0 || wTimeOut > s_stDevInfo.stPlay.iTimeMS)
				{
					DevLcdShow(s_stDevInfo.astResInfo[s_stDevInfo.stPlay.iCnt].szName);
					s_stDevInfo.stPlay.iCnt = (s_stDevInfo.stPlay.iCnt + 1) >= s_stDevInfo.wResNum ? 0 : (s_stDevInfo.stPlay.iCnt + 1);
					wTimeOut = 0;
				}
				wTimeOut += DEV_EVENT_WAIT_TIME_MS;
			}
			else if (s_stDevInfo.stPlay.iIndex > 0 && s_stDevInfo.stPlay.iIndex <= s_stDevInfo.wResNum)
			{
				DevLcdShow(s_stDevInfo.astResInfo[s_stDevInfo.stPlay.iIndex - 1].szName);
				s_stDevInfo.stPlay.cActive = FALSE;
			}
			else
			{
				DevLcdShow(PIC_FILE_NAME_POWER_ON);
				s_stDevInfo.stPlay.cActive = FALSE;
			}
		}
	}
	DEV_LOG("============>out dev task");
	YMI_TaskDelete(s_tDevTaskID);
}
int DevTaskInit(void)
{
	int iRet;

	iRet = YMI_TaskCreat(&s_tDevTaskID, "dev_task", DEV_TASK_STACK_SIZE, DevTask, NULL, EM_TASK_PRIORITY_BELOW_NORMAL, DEV_TASK_EVENT_NUM);
	if (iRet != YMI_OK)
	{
		DEV_LOG("[ERR]dev task creat ret=%d", iRet);
		return RET_FAIL;
	}
	return RET_SUCC;
}

// end of file