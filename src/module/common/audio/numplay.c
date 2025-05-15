/**********************************************************************
* Copyright (C)    Dspread
* File name: numplay.c
* Content summary: Audio-related APIs
*
***********************************************************************/
//Header file
#include <stdarg.h>
#include "var.h"
#include "YMAPI.h"
#include "numplay.h"
#include "wavmix.h"
#include "res.h"

//Macro definitions
// #define NUMPLAY_LOG_MODE
#ifdef NUMPLAY_LOG_MODE
#define NUMPLAY_LOG(msg, ...) LOG("[NUMPLAY]"msg"", ##__VA_ARGS__)
#else
#define NUMPLAY_LOG(msg, ...)
#endif

#define UTF8_ONE_CHINESE_SIZE   (3)

//Constant
//Local variables
static char s_szChineseBuf[1024] = {0};
char const *c_pszChineseNumStr = DIGITAL_COLLECTION;
static const char *sc_NumId[]={
	AUDIO_NUM0, AUDIO_NUM1, AUDIO_NUM2, AUDIO_NUM3, AUDIO_NUM4, AUDIO_NUM5, 
	AUDIO_NUM6, AUDIO_NUM7, AUDIO_NUM8, AUDIO_NUM9 //, AUDIO_DIAN, AUDIO_SHI
};

//Global variables
//Function implementation

int GetNumChineseSub(uint wNum, void *pvOutBuf, char *pcSpkZero)
{
    char *pcPtr;
	char cTmp;

    if (pvOutBuf == NULL)
    {
        NUMPLAY_LOG("[ERR]%s param fail!", __func__);
        return YMI_ERR;
    }
	
	if (wNum>=10000) return -1;
	if (wNum==0)
	{
		if (*pcSpkZero) *pcSpkZero=2;
		return 0;
	}
	
	pcPtr=pvOutBuf;
	
	cTmp=wNum/1000;
	wNum %=1000;
    NUMPLAY_LOG("wNum %d,mod %d,pcSpkZero 0x%x",cTmp,wNum,*pcSpkZero);
	if (cTmp)
	{
		if (*pcSpkZero==2)
		{
			memcpy(pcPtr, c_pszChineseNumStr, UTF8_ONE_CHINESE_SIZE);
            pcPtr += UTF8_ONE_CHINESE_SIZE;
		}
        memcpy(pcPtr, (c_pszChineseNumStr + cTmp * UTF8_ONE_CHINESE_SIZE), UTF8_ONE_CHINESE_SIZE);
        pcPtr += UTF8_ONE_CHINESE_SIZE;
		strcpy(pcPtr, AUDIO_QIAN);
		pcPtr += UTF8_ONE_CHINESE_SIZE;
		*pcSpkZero = 1;
	}
	else if (*pcSpkZero) *pcSpkZero = 2;
	
	cTmp = wNum / 100;
	wNum %= 100;
    NUMPLAY_LOG("wNum %d,mod %d,pcSpkZero 0x%x",cTmp,wNum,*pcSpkZero);
	if (cTmp)
	{
		if (*pcSpkZero==2)
		{
			memcpy(pcPtr, c_pszChineseNumStr, UTF8_ONE_CHINESE_SIZE);
            pcPtr += UTF8_ONE_CHINESE_SIZE;
		}
		memcpy(pcPtr, (c_pszChineseNumStr + cTmp * UTF8_ONE_CHINESE_SIZE), UTF8_ONE_CHINESE_SIZE);
        pcPtr += UTF8_ONE_CHINESE_SIZE;
		strcpy(pcPtr, AUDIO_BAI);
		pcPtr += UTF8_ONE_CHINESE_SIZE;
		*pcSpkZero = 1;
	}
	else if (*pcSpkZero) *pcSpkZero=2;
	
	cTmp = wNum / 10;
	wNum %= 10;
	NUMPLAY_LOG("wNum %d,mod %d,pcSpkZero 0x%x",cTmp,wNum,*pcSpkZero);
	if (cTmp)
	{
		if (*pcSpkZero == 2)
		{
			memcpy(pcPtr, c_pszChineseNumStr, UTF8_ONE_CHINESE_SIZE);
            pcPtr += UTF8_ONE_CHINESE_SIZE;
		}
		if ((*pcSpkZero) || (cTmp != 1))
		{
			memcpy(pcPtr, (c_pszChineseNumStr + cTmp * UTF8_ONE_CHINESE_SIZE), UTF8_ONE_CHINESE_SIZE);
            pcPtr += UTF8_ONE_CHINESE_SIZE;
		}
		strcpy(pcPtr, AUDIO_SHI);
		pcPtr += UTF8_ONE_CHINESE_SIZE;
		*pcSpkZero = 1;
	}
	else if (*pcSpkZero) *pcSpkZero = 2;
	
	cTmp = wNum;///1000;
	if (cTmp)
	{
		if (*pcSpkZero==2)
		{
			memcpy(pcPtr, c_pszChineseNumStr, UTF8_ONE_CHINESE_SIZE);
            pcPtr += UTF8_ONE_CHINESE_SIZE;
		}
		//if ((*pcSpkZero==0)&&(cTmp==2))//
		memcpy(pcPtr, (c_pszChineseNumStr + cTmp * UTF8_ONE_CHINESE_SIZE), UTF8_ONE_CHINESE_SIZE);
        pcPtr += UTF8_ONE_CHINESE_SIZE;
		*pcSpkZero = 1;
	}
	else if (*pcSpkZero) *pcSpkZero = 2;
	
	return (pcPtr - (char *)pvOutBuf);
}

int GetNumChinese(uint wNum, int iDot, void *pvOutBuf, int iOutBufSize)
{
    uint wTmp = 0;
    char *pcStr = NULL;
    char cSpkZero=0;

    /* cSpkZero 0:The conversion was not started  1:The conversion has started and the zeros are not read  2:Read zero */
    pcStr = pvOutBuf;
    if (wNum)
    {
        wTmp = wNum / 100000000l;
        wNum %= 100000000l;
        if (wTmp)
        {
            pcStr += GetNumChineseSub(wTmp, pcStr, &cSpkZero);
            strcpy(pcStr, AUDIO_YI);
            pcStr += UTF8_ONE_CHINESE_SIZE;
            cSpkZero = 1;
        }
        else if (cSpkZero) cSpkZero=2;
        
        wTmp = wNum / 10000l;
        wNum %= 10000l;
        if (wTmp)
        {
            pcStr += GetNumChineseSub(wTmp, pcStr, &cSpkZero);
            strcpy(pcStr, AUDIO_WAN);
            pcStr += UTF8_ONE_CHINESE_SIZE;
            cSpkZero = 1;
        }
        else if (cSpkZero) cSpkZero=2;

        if (wNum)
        {
            pcStr += GetNumChineseSub(wNum, pcStr, &cSpkZero);
        }
    }
    else
    {
        memcpy(pcStr, c_pszChineseNumStr, UTF8_ONE_CHINESE_SIZE);
        pcStr += UTF8_ONE_CHINESE_SIZE;
    }
    
    if (iDot)
    {
        strcpy(pcStr, AUDIO_DIAN);
		pcStr += UTF8_ONE_CHINESE_SIZE;
		wNum = iDot / 10;
        memcpy(pcStr, (c_pszChineseNumStr + wNum * UTF8_ONE_CHINESE_SIZE), UTF8_ONE_CHINESE_SIZE);
        pcStr += UTF8_ONE_CHINESE_SIZE;
		wNum = iDot % 10;
		if (wNum)
		{
			memcpy(pcStr, (c_pszChineseNumStr + wNum * UTF8_ONE_CHINESE_SIZE), UTF8_ONE_CHINESE_SIZE);
            pcStr += UTF8_ONE_CHINESE_SIZE;
		}
    }
    *pcStr = 0;
    
    return (pcStr - (char *)pvOutBuf);
}

int NumToChinese(const char *c_pszNumStr, void *pvOutBuf, int iOutBufSize, bool bIsN)
{
    uint wNum = 0;
    int iLen = 0, iDot = 0;
    char *pcStr = NULL;
    char cTmp = 0;

    if ((c_pszNumStr == NULL) || (pvOutBuf == NULL) || (!strlen(c_pszNumStr)))
    {
        NUMPLAY_LOG("[ERR]%s param fail!", __func__);
        return YMI_ERR;
    }
    
    iLen = strlen(c_pszNumStr);
    if (iLen >= iOutBufSize)
    {
        NUMPLAY_LOG("[ERR]input str len %d >= out buf size %d", iLen, iOutBufSize);
        return YMI_ERR;
    }
    
    NUMPLAY_LOG("c_pszNumStr = %s", c_pszNumStr);
    pcStr = (char *)c_pszNumStr;
    while (*pcStr)
    {
        cTmp = *pcStr++;
        if (((cTmp<'0')||(cTmp>'9'))&&(cTmp!='.'))
        {
            NUMPLAY_LOG("[ERR]num str fail!%c", cTmp);
            return YMI_ERR;
        }
    }
    
    if (bIsN)
    {   /* broadcast the numbers */
        pcStr = pvOutBuf;
        while (*c_pszNumStr)
        {
            cTmp = *c_pszNumStr++;

            if (iOutBufSize < (pcStr - (char *)pvOutBuf + UTF8_ONE_CHINESE_SIZE))
            {
                NUMPLAY_LOG("[ERR]out buf to short!");
                return YMI_ERR;
            }
            
            if (cTmp == '.')
            {
                memcpy(pcStr, AUDIO_DIAN, UTF8_ONE_CHINESE_SIZE);
                pcStr += UTF8_ONE_CHINESE_SIZE;
            }
            else
            {
                int iOffset = (cTmp - '0') * UTF8_ONE_CHINESE_SIZE;
                memcpy(pcStr, c_pszChineseNumStr+iOffset, UTF8_ONE_CHINESE_SIZE);
                pcStr += UTF8_ONE_CHINESE_SIZE;
            }
        }
        iLen = pcStr - (char *)pvOutBuf;
        NUMPLAY_LOG("pvOutBuf = %s, len = %d", pvOutBuf, iLen);
    }
    else
    {   /* broadcast the amount */
        memset(pvOutBuf, 0x00, iOutBufSize);
        memcpy(pvOutBuf, c_pszNumStr, strlen(c_pszNumStr));

        pcStr = strchr(pvOutBuf, '.');
        if (pcStr != NULL)
        {
            *pcStr++ = 0;
            if (strchr(pcStr, '.'))
            {
                NUMPLAY_LOG("[ERR]dot to many");
                return YMI_ERR;
            }
        }

        sscanf(pvOutBuf, "%d", &wNum);
        if (pcStr != NULL)
        {
            iLen = strlen(pcStr);
            if (iLen > 2)pcStr[2] = 0;
            sscanf(pcStr, "%d", &iDot);
            // iDot = (uint)atoi(pcStr);
            if (iLen == 0)iDot = 0;
            else if (iLen == 1)iDot *= 10;
        }
        else
        {
            iDot = 0;
        }

        NUMPLAY_LOG("num = %d, dot = %d", wNum, iDot);
        memset(pvOutBuf, 0x00, iOutBufSize);
        iLen = GetNumChinese(wNum, iDot, pvOutBuf, iOutBufSize);
        if (iLen < 0)
        {
            NUMPLAY_LOG("[ERR]wNum get chinese fail!");
            return YMI_ERR;
        }
        NUMPLAY_LOG("pvOutBuf = %s, len = %d", pvOutBuf, iLen);
    }

    return iLen;
}

int YmWavMixPlay(const char *c_pszPlayStr)
{
    int iRet = YMI_ERR;
    int iOffset = 0, iDex = 0;
    char szUtf8[4] = {0};
    char szFilePath[AUDIO_FILE_PATH_MAX] = {0};
    void *pvPlayList = NULL;

    pvPlayList = WavMixPlayListCreate();
    if (pvPlayList == NULL)
    {
        NUMPLAY_LOG("[ERR]create palylist fail!");
        return YMI_ERR;
    }

    for (iOffset = 0; iOffset < strlen(c_pszPlayStr); iOffset += UTF8_ONE_CHINESE_SIZE)
    {
        BUFCLR(szUtf8);
        BUFCLR(szFilePath);
        memcpy(szUtf8, c_pszPlayStr + iOffset, UTF8_ONE_CHINESE_SIZE);

        for (iDex = 0; iDex < (sizeof(sc_NumId)/sizeof(sc_NumId[0])); iDex++)
        {
            if (!memcmp(szUtf8, &c_pszChineseNumStr[iDex * UTF8_ONE_CHINESE_SIZE], UTF8_ONE_CHINESE_SIZE))
            {
                BUFCLR(szUtf8);
                memcpy(szUtf8, sc_NumId[iDex], strlen(sc_NumId[iDex]));
                break;
            }
        }
        NUMPLAY_LOG("utf8 str = %s, idex = %d", szUtf8, iDex);

        if (ResCheckWavFile(szUtf8, szFilePath, sizeof(szFilePath)) != YMI_OK)
        {
            NUMPLAY_LOG("[ERR]wav file fond fail!");
            iRet = YMI_ERR;
            goto exit;
        }
        
        if (WavMixPlayListAdd(pvPlayList, szFilePath) != YMI_OK)
        {
            NUMPLAY_LOG("[ERR]wav file append fail!");
            iRet = YMI_ERR;
            goto exit;
        }
    }

    iRet = WavMixPlayListStart(pvPlayList);
    SLEEP_MS(500);
    WaitPlayEnd();
    
exit:
    WavMixPlayListDel(pvPlayList);
    return iRet;
}

int YmWavPlayNum(const char *c_pszNumStr, bool bIsN, bool bYuan)
{
    BUFCLR(s_szChineseBuf);
    if (NumToChinese(c_pszNumStr, s_szChineseBuf, sizeof(s_szChineseBuf), bIsN) <= 0)
    {
        NUMPLAY_LOG("[ERR]num to chinese fail!");
        return YMI_ERR;
    }

    if (bYuan)
    {
        strcat(s_szChineseBuf, AUDIO_YUAN);
    }
    NUMPLAY_LOG("numstr = %s, chinese = %s", c_pszNumStr, s_szChineseBuf);
    
    return YmWavMixPlay(s_szChineseBuf);
}

