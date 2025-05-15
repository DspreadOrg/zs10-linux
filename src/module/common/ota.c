/**********************************************************************
* Copyright (C)    Dspread
* File name: key.c
* Content summary: Key-related APIs
*
***********************************************************************/
//Header file
#include "var.h"

//Macro definitions
#define OTA_LOG_MODE
#ifdef OTA_LOG_MODE
#define OTA_LOG(msg, ...) LOG("[OTA]"msg"", ##__VA_ARGS__)
#else
#define OTA_LOG(msg, ...)
#endif
#define OTA_FILE_DIR "ota_dir"
#define FILE_SUFFIX_FIRMWARE ".pac"
#define FILE_SUFFIX_APP ".YMG"
#define FILE_SUFFIX_APP_SIGN ".YSIG"

#define PIC_ZIP_FILE_DIR "pic_dir"
//Constant
//Local variables
static T_Task s_tOtaTaskID = 0;
static T_Task s_tOtaDownTaskID = 0;
static T_Task s_tUnzipFileTaskID = 0;
static bool bOtaing = FALSE;
//Global variables
//Function implementation


int UnzipOtaFile(char *pszFileName)
{
	int iRet, iCnt = 0, i, iFlag = FALSE;
	char szBuff[256] = "";
	char *pszName = NULL;
	char szPath[128] = "";

	if(pszFileName == NULL)
	{
		return RET_FAIL;
	}

	// YMI_FsMakeDir(OTA_FILE_DIR);
	// OTA_LOG("ready to unzip file=%s to dri=%s", pszFileName, OTA_FILE_DIR);
	// iRet = YMI_ZipUnzip(pszFileName, OTA_FILE_DIR);
	// if(iRet != RET_SUCC)
	// {
	// 	OTA_LOG("[ERR]file unzip ret=%d", iRet);
	// 	return RET_FAIL;
	// }
	// iRet = YMI_FsListInDir(OTA_FILE_DIR, szBuff, (uint *)&iCnt, sizeof(szBuff));
	// if(iRet != RET_SUCC)
	// {
	// 	OTA_LOG("[ERR]unzip dir %s ret=%d", OTA_FILE_DIR, iRet);
	// 	return RET_FAIL;
	// }
	// if(iCnt == 0)
	// {
	// 	OTA_LOG("[ERR]unzip file num=%d", iCnt);
	// 	return RET_FAIL;
	// }
	// pszName = szBuff;
	// for(i = 0; i < iCnt; i ++)
	// {
	// 	sprintf(szPath, "%s/%s", OTA_FILE_DIR, pszName);
	// 	OTA_LOG("unzip file=%s", szPath);
	// 	if(strstr(szPath, FILE_SUFFIX_FIRMWARE))
	// 	{
	// 		iRet = YMI_Fota(szPath);
	// 		if(iRet != RET_SUCC)
	// 		{
	// 			bOtaing = FALSE;
	// 			OTA_LOG("[ERR]foa firmware %s ret=%d", szPath, iRet);
	// 			break;
	// 		}
	// 		iFlag = TRUE;
	// 	}
	// 	else if(strstr(szPath, FILE_SUFFIX_APP) || strstr(szPath, FILE_SUFFIX_APP_SIGN))
	// 	{
	// 		iRet = YMI_FotaMasterApp(szPath, 1);
	// 		if (iRet != RET_SUCC)
	// 		{
	// 			bOtaing = FALSE;
	// 			OTA_LOG("[ERR]foa app %s ret=%d", szPath, iRet);
	// 		}
	// 		iFlag = TRUE;
	// 	}
	// 	pszName += strlen(pszName) + 1;
	// }
	// if(iFlag)
	// {
	// 	OTA_LOG("ready to reboot=============>");
	// 	YMI_SysReboot();
	// }

	return iRet;
}
void OTAFileReset(void)
{
	YMI_FsDeleteDir(OTA_FILE_DIR);
	YMI_FsDelete(OTA_FILE_NAME);
}
static void OtaTask(void *pvParam)
{
	char *pszFileName = (char *)pvParam;
	int iRet;

	iRet = UnzipOtaFile(pszFileName);
	if(iRet != RET_SUCC)
	{
		OTAFileReset();
		bOtaing = FALSE;
		DevEventSent(DEV_EVENT_ID_SAVE_OTA_FAILS, OTA_FILE_NAME, NULL, 0);
	}
    YMI_TaskDelete(s_tOtaTaskID);
}
int OtaTaskInit(char *pszFileName)
{
	int iRet;
	iRet = YMI_TaskCreat(&s_tOtaTaskID, "ota_task", 4 * 1024, OtaTask, pszFileName, EM_TASK_PRIORITY_NORMAL, 1);
	if (iRet != YMI_OK)
	{
		OTA_LOG("[ERR]ota task creat ret=%d", iRet);
		return RET_FAIL;
	}
	return RET_SUCC;
}

static void OtaDownTask(void *pvParam)
{
	ota_params *pstOtaPar = (ota_params *)pvParam;
	if (!pstOtaPar)
	{
		return ;
	}

	int iRet = 0;
	EM_SOCKET_TYPE eSocketType = SOCKET_TCP_CLIENT;
	bOtaing = TRUE;

	iRet = YMI_HttpDownLoadFile(eSocketType, pstOtaPar->url, pstOtaPar->size, OTA_FILE_NAME);
	OTA_LOG("ota file down ret=%d,%x", iRet, iRet);
	if (iRet == RET_SUCC)
	{
		OtaTaskInit(OTA_FILE_NAME);
	}
	else
	{
		bOtaing = FALSE;
	}
	
	FREE(pstOtaPar->url);
	FREE(pstOtaPar->verify_value);
	FREE(pstOtaPar->version);
	FREE(pstOtaPar);
	
	YMI_TaskDelete(s_tOtaDownTaskID);
}

int OtaDown(void *pvOtaParam)
{
	int iRet = RET_FAIL;
	OTA_LOG("bOtaing = %d", bOtaing);
	if (bOtaing)
	{
		return RET_FAIL;
	}

	iRet = YMI_TaskCreat(&s_tOtaDownTaskID, "ota_down_task", 4 * 1024, OtaDownTask, pvOtaParam, EM_TASK_PRIORITY_NORMAL, 1);
	if (iRet != YMI_OK)
	{
		OTA_LOG("[ERR]ota task creat ret=%d", iRet);
		return RET_FAIL;
	}
	return RET_SUCC;
}


int UnzipPicZip(char *pszFileName)
{
	int iRet, iCnt = 0, i;
	// char szBuff[256] = "";
	// char *pszName = NULL;
	// // char szPath[128] = "";
	// ST_PicDispConf stPicUp = {
	// 	.iX = 0,
	// 	.iY = 0,
	// 	.szFilePath = "",
	// 	.wWidth = 800,
	// 	.wHeight = 1280,
	// };

	// if(pszFileName == NULL)
	// {
	// 	return RET_FAIL;
	// }

	// YMI_FsMakeDir(PIC_ZIP_FILE_DIR);
	// OTA_LOG("ready to unzip file=%s to dri=%s", pszFileName, PIC_ZIP_FILE_DIR);
	// iRet = YMI_ZipUnzip(pszFileName, PIC_ZIP_FILE_DIR);
	// if(iRet != RET_SUCC)
	// {
	// 	OTA_LOG("[ERR]file unzip ret=%d", iRet);
	// 	return RET_FAIL;
	// }
	// iRet = YMI_FsListInDir(PIC_ZIP_FILE_DIR, szBuff, (uint *)&iCnt, sizeof(szBuff));
	// if(iRet != RET_SUCC)
	// {
	// 	OTA_LOG("[ERR]unzip dir %s ret=%d", PIC_ZIP_FILE_DIR, iRet);
	// 	return RET_FAIL;
	// }
	// if(iCnt == 0)
	// {
	// 	OTA_LOG("[ERR]unzip file num=%d", iCnt);
	// 	return RET_FAIL;
	// }
	// pszName = szBuff;
	// for(i = 0; i < iCnt; i ++)
	// {
	// 	sprintf(stPicUp.szFilePath, "%s/%s", PIC_ZIP_FILE_DIR, pszName);
	// 	OTA_LOG("unzip file=%s", stPicUp.szFilePath);
	// 	LVScreenDisp(&stPicUp, NULL, NULL, NULL, NULL);//for test
	// 	SLEEP_S(3);
	// 	pszName += strlen(pszName) + 1;
	// }
	return iRet;
}
static void UnzipFileHandle(void *pvParam)
{
	char *pszFileName = (char *)pvParam;
	
	UnzipPicZip(pszFileName);
    YMI_TaskDelete(s_tUnzipFileTaskID);
}
void PicZipFileReset(void)
{
	YMI_FsDeleteDir(PIC_ZIP_FILE_DIR);
	YMI_FsDelete(PIC_ZIP_FILE_NAME);
}
int UnzipFileTask(void *pvUnzip)
{
	int iRet = RET_FAIL;
	OTA_LOG("bOtaing = %d", bOtaing);
	if (bOtaing)
	{
		return RET_FAIL;
	}

	iRet = YMI_TaskCreat(&s_tUnzipFileTaskID, "unzip file", 4 * 1024, UnzipFileHandle, pvUnzip, EM_TASK_PRIORITY_NORMAL, 1);
	if (iRet != YMI_OK)
	{
		OTA_LOG("[ERR]ota task creat ret=%d", iRet);
		return RET_FAIL;
	}
	return RET_SUCC;
}
//end of file