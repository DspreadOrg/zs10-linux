/**********************************************************************
* Copyright (C)    Dspread
* File name: system.c
* Content summary: system-related APIs
*
***********************************************************************/
//Header file
#include "stddef.h"
#include "YMAPI.h"
#include "var.h"

//Macro definitions
#define SYS_LOG_MODE
#ifdef SYS_LOG_MODE
#define SYS_LOG(msg, ...) LOG("[SYS]"msg"", ##__VA_ARGS__)
#define SYS_HEX_LOG(title, data, len) LOG_HEX("[SYS]"title" :", data, len)
#else
#define SYS_LOG(msg, ...)
#define SYS_HEX_LOG(title, data, len)
#endif

#define SYS_VER "YM_CS80_2.1"
#define SDK_VER "EC600U_2.6"
#define SYSINFO_HEAD_SIZE   (100)
#define DEV_LTE_PROFILE_INDEX 1
#define SYSTEM_PARAM_FILENAME "res/sysparam.txt"
#define SYSTEM_PARAM_INI_FILE "res/param.ini"
#define SYSTEM_PARAM_INI_HEAD "[PARAM]"

#define OFFSETOF(type, name)    (int)&(((type *)0)->name)
#define	OFFSET(x)               OFFSETOF(ST_SysInfo, x)
#define SYSINFO_SIZE            sizeof(ST_SysInfo)

//Constant

//Local variables
static ST_SysInfo *s_pstSysInfo = NULL;
static T_Mutex s_tSysParamFileMutex = NULL;

int SysInfoSave(void);
int SysInfoSaveToNv(void);

//Global variables
extern char firmware_version[50];

const ST_YMSettingItem astYMSetItems[] =
{
	{"Battery",     sizeof(s_pstSysInfo->iBattery),         OFFSET(iBattery),       PARAM_TYPE_NUM},
	{"Flash",       sizeof(s_pstSysInfo->iFlash),           OFFSET(iFlash),         PARAM_TYPE_NUM},
	{"Microphone",  sizeof(s_pstSysInfo->iMicrophone),      OFFSET(iMicrophone),    PARAM_TYPE_NUM},
	{"Led",         sizeof(s_pstSysInfo->iLed),             OFFSET(iLed),           PARAM_TYPE_NUM},
	{"Speaker",     sizeof(s_pstSysInfo->iSpeaker),         OFFSET(iSpeaker),       PARAM_TYPE_NUM},
	{"Status",      sizeof(s_pstSysInfo->iRegisterState),   OFFSET(iRegisterState), PARAM_TYPE_NUM},
	{"Volume",      sizeof(s_pstSysInfo->iVolume),          OFFSET(iVolume),        PARAM_TYPE_NUM},
	{"LogStatus",   sizeof(s_pstSysInfo->iUartLog),         OFFSET(iUartLog),       PARAM_TYPE_NUM},
	{"AppVer",      sizeof(s_pstSysInfo->szFirmwareVersion),OFFSET(szFirmwareVersion),      PARAM_TYPE_STR},
    {"WifiVer",     sizeof(s_pstSysInfo->szWifiVer),        OFFSET(szWifiVer),              PARAM_TYPE_STR},
	{"SecretKey",   sizeof(s_pstSysInfo->szDeviceSecret),   OFFSET(szDeviceSecret),         PARAM_TYPE_STR},
	{"ProductKey",  sizeof(s_pstSysInfo->szProductKey),     OFFSET(szProductKey),           PARAM_TYPE_STR},
	{"DevName",     sizeof(s_pstSysInfo->szDeviceName),     OFFSET(szDeviceName),           PARAM_TYPE_STR},
	{"Serial",      sizeof(s_pstSysInfo->szDeviceSN),       OFFSET(szDeviceSN),             PARAM_TYPE_STR},
	{"Url",         sizeof(s_pstSysInfo->szServerUrl),      OFFSET(szServerUrl),            PARAM_TYPE_STR},
	{"MqttUrl",     sizeof(s_pstSysInfo->szServerMqtt),     OFFSET(szServerMqtt),           PARAM_TYPE_STR},
	{"Topic",       sizeof(s_pstSysInfo->szTopic),          OFFSET(szTopic),                PARAM_TYPE_STR},
	{"HWVer",       sizeof(s_pstSysInfo->szHwver),          OFFSET(szHwver),                PARAM_TYPE_STR},
    {"WifiExist",   sizeof(s_pstSysInfo->iDeviceType),      OFFSET(iDeviceType),    PARAM_TYPE_NUM},
};

//Function implementation
void SysInfoDebug(void)
{
    SYS_LOG("Battery = %d", s_pstSysInfo->iBattery);
    SYS_LOG("Flash = %d", s_pstSysInfo->iFlash);
    SYS_LOG("Microphone = %d", s_pstSysInfo->iMicrophone);
    SYS_LOG("Led = %d", s_pstSysInfo->iLed);
    SYS_LOG("Speaker = %d", s_pstSysInfo->iSpeaker);
    SYS_LOG("RegisterState = %d", s_pstSysInfo->iRegisterState);
    SYS_LOG("Volume = %d", s_pstSysInfo->iVolume);
    SYS_LOG("Vailed = %d", s_pstSysInfo->iVailed);
    SYS_LOG("FirmwareVersion = %s", s_pstSysInfo->szFirmwareVersion);
    SYS_LOG("DeviceSecret = %s", s_pstSysInfo->szDeviceSecret);
    SYS_LOG("ProductKey = %s", s_pstSysInfo->szProductKey);
    SYS_LOG("DeviceName = %s", s_pstSysInfo->szDeviceName);
    SYS_LOG("DeviceSN = %s", s_pstSysInfo->szDeviceSN);
    SYS_LOG("ServerUrl = %s", s_pstSysInfo->szServerUrl);
    SYS_LOG("Hwver = %s", s_pstSysInfo->szHwver);
    SYS_LOG("ServerMqtt = %s", s_pstSysInfo->szServerMqtt);
    SYS_LOG("UartLog = %d", s_pstSysInfo->iUartLog);
    SYS_LOG("wifi ssid = %s", s_pstSysInfo->stWlanStaParam.szSsid);
    SYS_LOG("wifi ssid len = %d", s_pstSysInfo->stWlanStaParam.cSsidLen);
    SYS_LOG("wifi psk = %s", s_pstSysInfo->stWlanStaParam.szPsk);
    SYS_LOG("WifiVer = %s", s_pstSysInfo->szWifiVer);
    SYS_LOG("Topic = %s", s_pstSysInfo->szTopic);
    SYS_LOG("BacklightLevel = %d", s_pstSysInfo->iBacklightLevel);
    SYS_LOG("DeviceType = %d", s_pstSysInfo->iDeviceType);
    SYS_LOG("NetChanlLTE = %d", s_pstSysInfo->iNetChanlLTE);
    SYS_LOG("Airkiss = %d", s_pstSysInfo->iAirkiss);
    SYS_LOG("ButtonLevel = %d", s_pstSysInfo->iButtonLevel);
    SYS_LOG("Ota = %d", s_pstSysInfo->iOta);
    SYS_LOG("Vailed = %d", s_pstSysInfo->iVailed);
}

int SysParamItemParse(char *pszCont, ST_SysInfo *pstSysInfo)
{
    int iItemTotalLen = 0, iOffset = 0, iContLen = 0;
    char *pLineEnd = NULL, *pLineCont = NULL;
    char szItemName[32] = {0};

    if (!pszCont || !pstSysInfo)
    {
        SYS_LOG("[ERR]param fail!");
        return -1;
    }

    if ((strlen(pszCont) < strlen(SYSTEM_PARAM_INI_HEAD)) || memcmp(pszCont, SYSTEM_PARAM_INI_HEAD, strlen(SYSTEM_PARAM_INI_HEAD)))
    {
        SYS_LOG("[ERR]%s file head fail", SYSTEM_PARAM_INI_FILE);
        return -1;
    }
    
    iContLen = strlen(pszCont);
    iOffset = strlen(SYSTEM_PARAM_INI_HEAD) + strlen("\r\n");
    while (1)
    {
        pLineEnd = strstr(pszCont + iOffset, "\r\n");
        if (!pLineEnd)
        {
            SYS_LOG("iOffset = %d, pscCont len = %d", iOffset, iContLen);
            if (iOffset >= iContLen)
            {
                break;
            }
            pLineEnd = pszCont + iContLen + 1;
        }
        iItemTotalLen = pLineEnd - (pszCont + iOffset);
        pLineCont = strchr(pszCont + iOffset, '=');
        if (!pLineCont)
        {
            break;
        }
        BUFCLR(szItemName);
        memcpy(szItemName, (pszCont + iOffset), (pLineCont - (pszCont + iOffset)));
        pLineCont ++;
        *pLineEnd = 0;

        for (int i = 0; i < sizeof(astYMSetItems)/sizeof(astYMSetItems[0]); i++)
        {
            if ((strlen(astYMSetItems[i].pszItemName) == strlen(szItemName)) && !memcmp(astYMSetItems[i].pszItemName, szItemName, strlen(szItemName)))
            {
                SYS_LOG("param item name = %s", astYMSetItems[i].pszItemName);
                char *pcPtr = (void *)pstSysInfo + astYMSetItems[i].iOff;
                int wLen = astYMSetItems[i].wSize;
                // char *pcPtr = (void *)&(pstSysInfo[astYMSetItems[i].iOff]);
                SYS_LOG("pcPtr = 0x%x", pcPtr);
                SYS_LOG("astYMSetItems[%d].iOff = %d, size = %d", i, astYMSetItems[i].iOff, astYMSetItems[i].wSize);
                if (astYMSetItems[i].byType == PARAM_TYPE_STR)
                {
                    if (astYMSetItems[i].wSize < strlen(pLineCont))
                    {
                        continue;
                    }
                    memset(pcPtr, 0x00, astYMSetItems[i].wSize);
                    memcpy(pcPtr, pLineCont, strlen(pLineCont));
                }
                else if (astYMSetItems[i].byType == PARAM_TYPE_NUM)
                {
                    int iNum = 0;
                    sscanf(pLineCont, "%d", &iNum);
                    if (astYMSetItems[i].wSize > sizeof(char))
                    {
                        int *piInt = (int *)pcPtr;
                        *piInt = iNum;
                    }
                    else
                    {
                        *pcPtr = (char)iNum;
                    }
                }
                SYS_HEX_LOG("pLineCont", pLineCont, pLineEnd - pLineCont);
                SYS_HEX_LOG("pstSysInfo", pcPtr, wLen);
                break;
            }
        }
        iOffset += iItemTotalLen + strlen("\r\n");
    }

    return 0;
}

int SysParamIniParse(void)
{
    int iRet = 0, iFd = 0;
    ulong iLen = 0;
    char *pszCnfCont = NULL;
    if (!s_pstSysInfo)
    {
        SYS_LOG("[ERR]param fail!");
        return -1;
    }
    
    ASSERT_FAIL(YMI_FsExist(SYSTEM_PARAM_INI_FILE));

    iRet = YMI_FsFileSize(SYSTEM_PARAM_INI_FILE, &iLen);
    if ((iRet != YMI_OK) || (iLen <= 0))
    {
        SYS_LOG("[ERR]%s file get size fail!%d,%x", SYSTEM_PARAM_INI_FILE, iRet, iRet);
        return -1;
    }

    pszCnfCont = YMI_SysMalloc(iLen + 1);
    if (pszCnfCont == NULL)
    {
        SYS_LOG("[ERR]malloc fail!");
        return -1;
    }
    memset(pszCnfCont, 0x00, iLen + 1);
    
    iFd = YMI_FsOpen(SYSTEM_PARAM_INI_FILE, "r");
    if (iFd <= 0)
    {
        SYS_LOG("[ERR]%s file open fail!%d,%x", SYSTEM_PARAM_INI_FILE, iFd, iFd);
        iRet = -1;
        goto exit;
    }

    iRet = YMI_FsRead(iFd, pszCnfCont, iLen);
    if (iRet <= 0)
    {
        SYS_LOG("[ERR]%s file read fail!%d,%x", SYSTEM_PARAM_INI_FILE, iRet, iRet);
        iRet = -1;
        goto exit;
    }

    iRet = SysParamItemParse(pszCnfCont, s_pstSysInfo);
    if (iRet != RET_SUCC)
    {
        SYS_LOG("[ERR]sys param item parse fail1");
        iRet = -1;
        goto exit;
    }
    
    SysInfoDebug();
    SysInfoSave();
    SysInfoSaveToNv();
    
exit:
    if (iFd)
    {
        YMI_FsClose(iFd);
    }
    if (iRet == RET_SUCC)
    {
        YMI_FsDelete(SYSTEM_PARAM_INI_FILE);
    }
    FREE(pszCnfCont);
    return iRet;
}

int SysInfoMd5(void *pvJsonStr, int iJsonLen, void *pvMd5Str, void *pvMd5Hex)
{
    uchar byMd5Hex[32 + 1] = {0};

    if (pvJsonStr == NULL)
    {
        SYS_LOG("[ERR]%s param fail!");
        return RET_FAIL;
    }

    BUFCLR(byMd5Hex);
    Md5Encrypt((uchar *)pvJsonStr, iJsonLen, byMd5Hex);
    SYS_HEX_LOG("md5 hex", byMd5Hex, 16);
    if (pvMd5Str)
    {
        HexToStr(byMd5Hex, 16, pvMd5Str);
        SYS_LOG("md5 = %s", pvMd5Str);
    }
    if (pvMd5Hex)
    {
        memcpy(pvMd5Hex, byMd5Hex, 16);
    }

    return RET_SUCC;
}

int SysInfoPack(ST_SysInfo *pstSysInfo, void **pvOutStr, int isNv)
{
    int iRet = RET_FAIL;
    ST_MyJson *pstMyJson = NULL;

    if ((pstSysInfo == NULL) || (pvOutStr == NULL))
    {
        SYS_LOG("%s param fail!", __func__);
        return RET_FAIL;
    }

    ST_JsonPackInfo astPackInfo[] = {
		{"Battery", MYJSON_NUMBER, NULL, &pstSysInfo->iBattery, NULL},
		{"Flash", MYJSON_NUMBER, NULL, &pstSysInfo->iFlash, NULL},
        {"Microphone", MYJSON_NUMBER, NULL, &pstSysInfo->iMicrophone, NULL},
        {"Led", MYJSON_NUMBER, NULL, &pstSysInfo->iLed, NULL},
        {"Speaker", MYJSON_NUMBER, NULL, &pstSysInfo->iSpeaker, NULL},
        {"RegisterState", MYJSON_NUMBER, NULL, &pstSysInfo->iRegisterState, NULL},
        {"Volume", MYJSON_NUMBER, NULL, &pstSysInfo->iVolume, NULL},
        {"SoftVer", MYJSON_STRING, NULL, pstSysInfo->szFirmwareVersion, NULL},
        {"DeviceSecret", MYJSON_STRING, NULL, pstSysInfo->szDeviceSecret, NULL},
        {"ProductKey", MYJSON_STRING, NULL, pstSysInfo->szProductKey, NULL},
        {"DeviceName", MYJSON_STRING, NULL, pstSysInfo->szDeviceName, NULL},
        {"DeviceSN", MYJSON_STRING, NULL, pstSysInfo->szDeviceSN, NULL},
        {"ServerUrl", MYJSON_STRING, NULL, pstSysInfo->szServerUrl, NULL},
        {"Hwver", MYJSON_STRING, NULL, pstSysInfo->szHwver, NULL},
        {"ServerMqtt", MYJSON_STRING, NULL, pstSysInfo->szServerMqtt, NULL},
        {"UartLog", MYJSON_NUMBER, NULL, &pstSysInfo->iUartLog, NULL},
        {"WifiSsid", MYJSON_STRING, NULL, pstSysInfo->stWlanStaParam.szSsid, NULL},
        {"WifiSsidLen", MYJSON_NUMBER, NULL, &pstSysInfo->stWlanStaParam.cSsidLen, NULL},
        {"WifiPsk", MYJSON_STRING, NULL, pstSysInfo->stWlanStaParam.szPsk, NULL},
        {"WifiVer", MYJSON_STRING, NULL, pstSysInfo->szWifiVer, NULL},
        {"Topic", MYJSON_STRING, NULL, pstSysInfo->szTopic, NULL},
        {"BacklightLevel", MYJSON_NUMBER, NULL, &pstSysInfo->iBacklightLevel, NULL},
        {"DeviceType", MYJSON_NUMBER, NULL, &pstSysInfo->iDeviceType, NULL},
        {"NetChanlLTE", MYJSON_NUMBER, NULL, &pstSysInfo->iNetChanlLTE, NULL},
        {"Airkiss", MYJSON_NUMBER, NULL, &pstSysInfo->iAirkiss, NULL},
        {"ButtonLevel", MYJSON_NUMBER, NULL, &pstSysInfo->iButtonLevel, NULL},
        {"Ota", MYJSON_NUMBER, NULL, &pstSysInfo->iOta, NULL},
        {"FirstBoot", MYJSON_NUMBER, NULL, &pstSysInfo->iFirstBoot, NULL},
		{NULL, MYJSON_STRING, NULL, NULL, NULL},
	};

    ST_JsonPackInfo astPackNvInfo[] = {
        {"DeviceSN", MYJSON_STRING, NULL, pstSysInfo->szDeviceSN, NULL},
		{NULL, MYJSON_STRING, NULL, NULL, NULL},
	};

    MyJsonCreate(&pstMyJson);
    if(pstMyJson == NULL)
    {
        SYS_LOG("[ERR]myjson create fail!");
        return RET_FAIL;
    }

    if (isNv)
    {
        iRet = MyJsonPack(astPackNvInfo, NULL, &pstMyJson, NULL, 0);
        if (iRet != RET_SUCC)
        {
            SYS_LOG("[ERR]myjson pack fail!");
            iRet = RET_FAIL;
            goto exit;
        }
    }
    else
    {
        iRet = MyJsonPack(astPackInfo, NULL, &pstMyJson, NULL, 0);
        if (iRet != RET_SUCC)
        {
            SYS_LOG("[ERR]myjson pack fail!");
            iRet = RET_FAIL;
            goto exit;
        }
    }
    
    MyJsonPrint(pstMyJson, (char **)pvOutStr);
    SYS_LOG("out str = %s", *pvOutStr);
    iRet = RET_SUCC;
exit:
    if (pstMyJson)
    {
        MyJsonDelete(pstMyJson);
        pstMyJson = NULL;
    }
    return iRet;
}

int SysInfoUnpack(void *pvJsonStr, ST_SysInfo *pstSysInfo, int isNv)
{
    int iRet = RET_FAIL;
    ST_MyJson *pstMyJson = NULL;

    if ((pvJsonStr == NULL) || (pstSysInfo == NULL))
    {
        SYS_LOG("%s param fail!", __func__);
        return RET_FAIL;
    }

    MyJsonParse(pvJsonStr, &pstMyJson);
    if (pstMyJson == NULL)
    {
        SYS_LOG("[ERR]myjson parse fail!");
        return RET_FAIL;
    }

    ST_JsonUnpackInfo atDataUnpackInfo[] = {
        {"Battery", MYJSON_NUMBER, &pstSysInfo->iBattery, UnpackNumber},
        {"Flash", MYJSON_NUMBER, &pstSysInfo->iFlash, UnpackNumber},
        {"Microphone", MYJSON_NUMBER, &pstSysInfo->iMicrophone, UnpackNumber},
		{"Led", MYJSON_NUMBER, &pstSysInfo->iLed, UnpackNumber},
		{"Speaker", MYJSON_NUMBER, &pstSysInfo->iSpeaker, UnpackNumber},
        {"RegisterState", MYJSON_NUMBER, &pstSysInfo->iRegisterState, UnpackNumber},
        {"Volume", MYJSON_NUMBER, &pstSysInfo->iVolume, UnpackNumber},
        {"SoftVer", MYJSON_STRING, pstSysInfo->szFirmwareVersion, UnpackString},
        {"DeviceSecret", MYJSON_STRING, pstSysInfo->szDeviceSecret, UnpackString},
        {"ProductKey", MYJSON_STRING, pstSysInfo->szProductKey, UnpackString},
        {"DeviceName", MYJSON_STRING, pstSysInfo->szDeviceName, UnpackString},
        {"DeviceSN", MYJSON_STRING, pstSysInfo->szDeviceSN, UnpackString},
        {"ServerUrl", MYJSON_STRING, pstSysInfo->szServerUrl, UnpackString},
        {"Hwver", MYJSON_STRING, pstSysInfo->szHwver, UnpackString},
        {"ServerMqtt", MYJSON_STRING, pstSysInfo->szServerMqtt, UnpackString},
        {"UartLog", MYJSON_NUMBER, &pstSysInfo->iUartLog, UnpackNumber},
        {"WifiSsid", MYJSON_STRING, pstSysInfo->stWlanStaParam.szSsid, UnpackString},
        {"WifiSsidLen", MYJSON_NUMBER, &pstSysInfo->stWlanStaParam.cSsidLen, UnpackNumber},
        {"WifiPsk", MYJSON_STRING, pstSysInfo->stWlanStaParam.szPsk, UnpackString},
        {"WifiVer", MYJSON_STRING, pstSysInfo->szWifiVer, UnpackString},
        {"Topic", MYJSON_STRING, pstSysInfo->szTopic, UnpackString},
        {"BacklightLevel", MYJSON_NUMBER, &pstSysInfo->iBacklightLevel, UnpackNumber},
        {"DeviceType", MYJSON_NUMBER, &pstSysInfo->iDeviceType, UnpackNumber},
        {"NetChanlLTE", MYJSON_NUMBER, &pstSysInfo->iNetChanlLTE, UnpackNumber},
        {"Airkiss", MYJSON_NUMBER, &pstSysInfo->iAirkiss, UnpackNumber},
        {"ButtonLevel", MYJSON_NUMBER, &pstSysInfo->iButtonLevel, UnpackNumber},
        {"Ota", MYJSON_NUMBER, &pstSysInfo->iOta, UnpackNumber},
        {"FirstBoot", MYJSON_NUMBER, &pstSysInfo->iFirstBoot, UnpackNumber},
		{NULL, MYJSON_STRING, NULL, NULL},
	};

    ST_JsonUnpackInfo atDataUnpackNvInfo[] = {
        {"DeviceSN", MYJSON_STRING, pstSysInfo->szDeviceSN, UnpackString},
		{NULL, MYJSON_STRING, NULL, NULL},
	};

    if (isNv)
    {
        iRet = MyJsonUnPack(pstMyJson, atDataUnpackNvInfo);
        if (iRet != RET_SUCC)
        {
            SYS_LOG("[ERR]json unpack fail!");
            iRet = RET_FAIL;
            goto exit;
        }
    }
    else
    {
        iRet = MyJsonUnPack(pstMyJson, atDataUnpackInfo);
        if (iRet != RET_SUCC)
        {
            SYS_LOG("[ERR]json unpack fail!");
            iRet = RET_FAIL;
            goto exit;
        }
    }

exit:
    if (pstMyJson)
    {
        MyJsonDelete(pstMyJson);
        pstMyJson = NULL;
    }
    return iRet;
}

int SysInfoCreateFile(const char *c_pszFile)
{
    int iFd;
    iFd = YMI_FsOpen(c_pszFile, "w+");
    if(iFd <= 0)
    {
        SYS_LOG("[ERR]YMI_FsOpen iRet=%d", iFd);
        return RET_FAIL;
    }
    YMI_FsClose(iFd);
    return RET_SUCC;
}

int _SysInfoLoad(const char *c_pszPath, ST_SysInfo *pstSysInfo)
{
    int iFd;
    int iRet = -1;
    int iJsonLen = 0;
    ulong iFileSize = 0;
    char szHead[SYSINFO_HEAD_SIZE] = {0};
    char szMd5[32 + 1] = {0};
    char *pcReadBuf = NULL;

    if (c_pszPath == NULL || pstSysInfo == NULL)
    {
        SYS_LOG("[ERR]c_pszpath is NULL");
        return RET_FAIL;
    }

    iRet = YMI_FsFileSize(c_pszPath, &iFileSize);
    SYS_LOG("%s file size = %d", c_pszPath, iFileSize);
    if (iRet != RET_SUCC)
    {
        SYS_LOG("[ERR]file size get fail!%d,%x", iRet, iRet);
        return RET_FAIL;
    }

    pcReadBuf = YMI_SysMalloc(iFileSize + 1);
    if (pcReadBuf == NULL)
    {
        SYS_LOG("[ERR]sdk malloc fail!");
        return RET_FAIL;
    }
    memset(pcReadBuf, 0x00, iFileSize + 1);

    iFd = YMI_FsOpen(c_pszPath, "r");
    if (iFd <= 0)
    {
        SYS_LOG("[ERR]YMI_FsOpen %s iRet=%d", c_pszPath, iFd);
        iRet = RET_FAIL;
        goto exit;
    }

    iRet = YMI_FsRead(iFd, pcReadBuf, iFileSize);
    if (iRet < 0)
    {
        SYS_LOG("[ERR]YMI_FsRead %s iRet = %d,%x", c_pszPath, iRet, iRet);
        iRet = RET_FAIL;
        goto exit;
    }

    BUFCLR(szHead);
    memcpy(szHead, pcReadBuf, SYSINFO_HEAD_SIZE);
    sscanf(szHead, "%d", &iJsonLen);
    memcpy(pstSysInfo->acMd5, pcReadBuf + SYSINFO_HEAD_SIZE + iJsonLen, 32);
    pcReadBuf[SYSINFO_HEAD_SIZE + iJsonLen] = 0;

    iRet = SysInfoUnpack(pcReadBuf + SYSINFO_HEAD_SIZE, pstSysInfo, 0);
    if (iRet != RET_SUCC)
    {
        SYS_LOG("[ERR]sysinfo unpack fail!");
        pstSysInfo->iVailed = -1;
        iRet = RET_FAIL;
        goto exit;
    }
    
    BUFCLR(szMd5);
    SysInfoMd5(pcReadBuf + SYSINFO_HEAD_SIZE, iJsonLen, szMd5, NULL);
    SYS_LOG("Verify:  MD5 %s, actual %s", pstSysInfo->acMd5, szMd5);

    if (memcmp(szMd5, pstSysInfo->acMd5, 32) != 0)
    {
        SYS_LOG("[ERR]md5 err!");
        pstSysInfo->iVailed = -1;
        iRet = -2;
        goto exit;
    }
    pstSysInfo->iVailed = 1;

    iRet = RET_SUCC;
exit:
    if (iFd)
    {
        YMI_FsClose(iFd);
    }
    FREE(pcReadBuf);
    pcReadBuf = NULL;
    return iRet;
}

int SysInfoLoad(ST_SysInfo *pstSysInfo)
{
    int iRet = -1;

    YMI_MutexLock(s_tSysParamFileMutex, WAIT_FOREVER);
    iRet = _SysInfoLoad(SYSTEM_PARAM_FILENAME, pstSysInfo);
    YMI_MutexUnLock(s_tSysParamFileMutex);

    return iRet;
}

int SysInfoLoadNV(ST_SysInfo *pstSysInfo)
{//读取 systeminfo 校验md5 更新参数
    int iRet = -1;
    int iJsonLen = 0;
    char szMd5[32 + 1] = {0};
    char szHead[SYSINFO_HEAD_SIZE] = {0};
    char *pcReadBuf = NULL;

    YMI_MutexLock(s_tSysParamFileMutex, WAIT_FOREVER);
    // iRet = YMI_NvmRead("systeminfo", szHead, SYSINFO_HEAD_SIZE, 1);
    if(iRet <= 0){
        pstSysInfo->iVailed = -1;
        SYS_LOG("[ERR]load sysparam from nv,error");
        goto exit;
    }

    sscanf(szHead, "%d", &iJsonLen);
    
    pcReadBuf = YMI_SysMalloc(SYSINFO_HEAD_SIZE + iJsonLen + 32 + 1);
    if (pcReadBuf == NULL)
    {
        pstSysInfo->iVailed = -1;
        SYS_LOG("[ERR]sdk malloc fail!");
        goto exit;
    }
    memset(pcReadBuf, 0x00, SYSINFO_HEAD_SIZE + iJsonLen + 32 + 1);

    // iRet = YMI_NvmRead("systeminfo", pcReadBuf, SYSINFO_HEAD_SIZE + iJsonLen + 32, 1);
    if(iRet <= 0){
        pstSysInfo->iVailed = -1;
        SYS_LOG("[ERR]load sysparam from nv,error");
        goto exit;
    }

    memcpy(pstSysInfo->acMd5, pcReadBuf + SYSINFO_HEAD_SIZE + iJsonLen, 32);
    pcReadBuf[SYSINFO_HEAD_SIZE + iJsonLen] = 0;

    iRet = SysInfoUnpack(pcReadBuf + SYSINFO_HEAD_SIZE, pstSysInfo, 1);
    if (iRet != RET_SUCC)
    {
        SYS_LOG("[ERR]sysinfo unpack fail!");
        pstSysInfo->iVailed = -1;
        goto exit;
    }

    BUFCLR(szMd5);
    SysInfoMd5(pcReadBuf + SYSINFO_HEAD_SIZE, iJsonLen, szMd5, NULL);
    SYS_LOG("Verify:  MD5 %s, actual %s", pstSysInfo->acMd5, szMd5);

    if (memcmp(szMd5, pstSysInfo->acMd5, 32) != 0)
    {
        SYS_LOG("[ERR]md5 err!");
        pstSysInfo->iVailed = -1;
        iRet = -2;
    }
    else
    {
        pstSysInfo->iVailed = 1;
        iRet = RET_SUCC;
    }
    
exit:
    SYS_LOG("nv SN = %s", pstSysInfo->szDeviceSN);
    SYS_LOG("load sysparam from nv,iVailed:%d",pstSysInfo->iVailed);
    YMI_MutexUnLock(s_tSysParamFileMutex);
    FREE(pcReadBuf);
    return iRet;
}

int _SysInfoSave(const char *c_pszPath, ST_SysInfo *pstSysInfo)
{
    int iFd = 0;
    int iRet = RET_FAIL;
    int iPackLen = 0, iWirteLen = 0;
    void *pszPackStr = NULL;
    void *pvSaveStr = NULL;
    char szMd5[32 + 1] = {0};

    if ((c_pszPath == NULL) || (pstSysInfo == NULL))
    {
        SYS_LOG("%s param fail!", __func__);
        return RET_FAIL;
    }

    iRet = SysInfoPack(pstSysInfo, &pszPackStr, 0);
    if (iRet != RET_SUCC)
    {
        SYS_LOG("[ERR]sys info pack fail!");
        iRet = RET_FAIL;
        goto exit;
    }
    SYS_LOG("pack str = %s", pszPackStr);

    if (pszPackStr == NULL)
    {
        SYS_LOG("[ERR]sys info pack fail!");
        iRet = RET_FAIL;
        goto exit;
    }

    iPackLen = strlen(pszPackStr);
    pvSaveStr = YMI_SysMalloc(SYSINFO_HEAD_SIZE + iPackLen + sizeof(szMd5) + 1);
    if (pvSaveStr == NULL)
    {
        SYS_LOG("[ERR]sys malloc fail!");
        iRet = RET_FAIL;
        goto exit;
    }
    memset(pvSaveStr, 0x00, SYSINFO_HEAD_SIZE + iPackLen + sizeof(szMd5) + 1);

    BUFCLR(szMd5);
    SysInfoMd5(pszPackStr, iPackLen, szMd5, NULL);
    sprintf(pvSaveStr, "%d", iPackLen);
    memcpy(pvSaveStr + SYSINFO_HEAD_SIZE, pszPackStr, iPackLen);
    memcpy(pvSaveStr + SYSINFO_HEAD_SIZE + iPackLen, szMd5, strlen(szMd5));

    SYS_LOG("head = %s", pvSaveStr);
    SYS_LOG("json = %s", pvSaveStr + SYSINFO_HEAD_SIZE);
    SYS_LOG("md5 = %s", pvSaveStr + SYSINFO_HEAD_SIZE + iPackLen);

    YMI_FsDelete(c_pszPath);

    iFd = YMI_FsOpen(c_pszPath, "w+");
    if(iFd <= 0)
    {
        SYS_LOG("[ERR]YMI_FsOpen iRet=%d", iFd);
        iRet = RET_FAIL;
        goto exit;
    }

    /* System parameter file：Head Json Md5 */
    iWirteLen = SYSINFO_HEAD_SIZE + iPackLen + strlen(szMd5);
    iRet = YMI_FsWrite(iFd, pvSaveStr, iWirteLen);
    if (iRet < 0)
    {
        SYS_LOG("[ERR]YMI_FsWrite %s iRet=%d", c_pszPath, iRet);
        iRet = RET_FAIL;
        goto exit;
    }

    iRet = YMI_FsSync(iFd);
    if (iRet != RET_SUCC)
    {
        SLEEP_S(1);
        iRet = YMI_FsSync(iFd);
        if (iRet != RET_SUCC)
        {
            SYS_LOG("[ERR]YMI_FsSync iRet=%d", iRet);
            iRet = RET_FAIL;
            goto exit;
        }
    }
    SYS_LOG("save sysparam to flash %s", c_pszPath);

exit:
    if (iFd)
    {
        YMI_FsClose(iFd);
    }
    FREE(pvSaveStr);
    FREE(pszPackStr);
    return iRet;
}

int SysInfoSave(void)
{
    int iRet = 0;
    YMI_MutexLock(s_tSysParamFileMutex, WAIT_FOREVER);
    iRet = _SysInfoSave(SYSTEM_PARAM_FILENAME, s_pstSysInfo);
    YMI_MutexUnLock(s_tSysParamFileMutex);
    return iRet;
}

int SysInfoSaveToNv(void)
{
    int iRet = 0;
    int iPackLen = 0;
    void *pszPackStr = NULL;
    void *pvSaveStr = NULL;
    char szMd5[32 + 1] = {0};

    YMI_MutexLock(s_tSysParamFileMutex, WAIT_FOREVER);
    if(s_pstSysInfo != NULL){
        iRet = SysInfoPack(s_pstSysInfo, &pszPackStr, 1);
        if (iRet != RET_SUCC)
        {
            SYS_LOG("[ERR]sys info pack fail!");
            iRet = RET_FAIL;
            goto exit;
        }
        SYS_LOG("pack str = %s", pszPackStr);

        if (pszPackStr == NULL)
        {
            SYS_LOG("[ERR]sys info pack fail!");
            iRet = RET_FAIL;
            goto exit;
        }

        iPackLen = strlen(pszPackStr);
        pvSaveStr = YMI_SysMalloc(SYSINFO_HEAD_SIZE + iPackLen + 32 + 1);
        if (pvSaveStr == NULL)
        {
            SYS_LOG("[ERR]sys malloc fail!");
            iRet = RET_FAIL;
            goto exit;
        }
        memset(pvSaveStr, 0x00, SYSINFO_HEAD_SIZE + iPackLen + 32 + 1);

        BUFCLR(szMd5);
        SysInfoMd5(pszPackStr, iPackLen, szMd5, NULL);
        sprintf(pvSaveStr, "%d", iPackLen);
        memcpy(pvSaveStr + SYSINFO_HEAD_SIZE, pszPackStr, iPackLen);
        memcpy(pvSaveStr + SYSINFO_HEAD_SIZE + iPackLen, szMd5, strlen(szMd5));

        SYS_LOG("head = %s", pvSaveStr);
        SYS_LOG("json = %s", pvSaveStr + SYSINFO_HEAD_SIZE);
        SYS_LOG("md5 = %s", pvSaveStr + SYSINFO_HEAD_SIZE + iPackLen);

        int iWirteLen = SYSINFO_HEAD_SIZE + iPackLen + strlen(szMd5);
        // iRet = YMI_NvmWrite("systeminfo", pvSaveStr, iWirteLen, 1);
        SYS_LOG("system param write nv, iRet = %d", iRet);
    }
exit:
    FREE(pvSaveStr);
    FREE(pszPackStr);
    YMI_MutexUnLock(s_tSysParamFileMutex);
    return iRet;
}

int SysInfoDefault(void)
{
	int iRet = RET_FAIL;

    if(s_pstSysInfo == NULL)
    {
        s_pstSysInfo = MALLOC(1, ST_SysInfo);
        if (s_pstSysInfo == NULL)
        {
            SYS_LOG("[ERR]system MALLOC err");
            return iRet;
        }
    }
	
    memset(s_pstSysInfo, 0x00, SYSINFO_SIZE);
	s_pstSysInfo->iBattery = 0x00;
	s_pstSysInfo->iFlash = 0x01;
	s_pstSysInfo->iLed = 0x01;
	s_pstSysInfo->iMicrophone = 0x00;
	s_pstSysInfo->iSpeaker = 0x01;
	s_pstSysInfo->iRegisterState = 0x00;
	s_pstSysInfo->iVolume = EM_VOL_LEVEL_5;
	s_pstSysInfo->iUartLog = 0x00;
	s_pstSysInfo->iButtonLevel = 0x00;
	s_pstSysInfo->iAirkiss = 0x01;
    s_pstSysInfo->iFirstBoot = 0x01;
    s_pstSysInfo->iDeviceType = 0;

	s_pstSysInfo->iNetChanlLTE = 4; // default is 4G
	s_pstSysInfo->iBacklightLevel = 10;
	
	memcpy(s_pstSysInfo->szHwver, "1.2", strlen("1.2"));
	memset(s_pstSysInfo->szDeviceSN, 0, sizeof(s_pstSysInfo->szDeviceSN)/sizeof(s_pstSysInfo->szDeviceSN[0]));
	memcpy(s_pstSysInfo->szFirmwareVersion, firmware_version, strlen(firmware_version));
    memcpy(s_pstSysInfo->szTopic, "user/message", strlen("user/message"));
	
	return RET_SUCC;
}

int SysInfoInit(void)
{
    int iRet = -1;

    YMI_MutexCreat(&s_tSysParamFileMutex);
    //MAIN SYSTEM PARMS
    s_pstSysInfo = MALLOC(1, ST_SysInfo);
    if (s_pstSysInfo == NULL)
    {
        SYS_LOG("[ERR]system YMI_SysMalloc err");
        return RET_FAIL;
    }
    memset(s_pstSysInfo, 0x00, SYSINFO_SIZE);
    iRet = YMI_FsExist(SYSTEM_PARAM_FILENAME);
    if (iRet != RET_SUCC) // file not exist
    {
        SysInfoCreateFile(SYSTEM_PARAM_FILENAME);
    }
    else
    {
        SysInfoLoad(s_pstSysInfo);
    }

    //NV SYSTEM PARMS
    ST_SysInfo *pstSysInfoNv = NULL;
    pstSysInfoNv = MALLOC(1, ST_SysInfo);
    if (pstSysInfoNv == NULL)
    {
        return RET_FAIL;
    }
    memset(pstSysInfoNv, 0x00, SYSINFO_SIZE);
    SysInfoLoadNV(pstSysInfoNv);

    SYS_LOG("vailed1 %d vailednv %d", s_pstSysInfo->iVailed, pstSysInfoNv->iVailed);
    if (s_pstSysInfo->iVailed == 1)
    {
        if (strcmp(s_pstSysInfo->szFirmwareVersion, firmware_version))
        {
            BUFCLR(s_pstSysInfo->szFirmwareVersion);
            memcpy(s_pstSysInfo->szFirmwareVersion, firmware_version, strlen(firmware_version));
            SysInfoSave();
        }
    }
    else if (pstSysInfoNv->iVailed == 1)
    {
        memcpy(s_pstSysInfo, pstSysInfoNv, SYSINFO_SIZE);
        if (strcmp(s_pstSysInfo->szFirmwareVersion, firmware_version))
        {
            BUFCLR(s_pstSysInfo->szFirmwareVersion);
            memcpy(s_pstSysInfo->szFirmwareVersion, firmware_version, strlen(firmware_version));
        }
        SysInfoSave();
    }
    else
    {
        SysInfoDefault();
    }

    SysInfoDebug();
    FREE(pstSysInfoNv);
    SysParamIniParse();
    return 0;
}

ST_SysInfo* SysInfoGet(void)
{
	return s_pstSysInfo;
}



//end of file