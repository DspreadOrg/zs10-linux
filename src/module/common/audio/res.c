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
#include "res.h"

//Macro definitions
#define RES_LOG_MODE
#ifdef RES_LOG_MODE
#define RES_LOG(msg, ...) LOG("[RES]"msg"", ##__VA_ARGS__)
#else
#define RES_LOG(msg, ...)
#endif

#if 0
//Constant
//Local variables
static ST_RES_INFO s_stRes[] = {
	{ AUDIO_NUM0, "res/wav/0.wav" },
	{ AUDIO_NUM1, "res/wav/1.wav" },
	{ AUDIO_NUM2, "res/wav/2.wav" },
	{ AUDIO_NUM3, "res/wav/3.wav" },
	{ AUDIO_NUM4, "res/wav/4.wav" },
	{ AUDIO_NUM5, "res/wav/5.wav" },
	{ AUDIO_NUM6, "res/wav/6.wav" },
	{ AUDIO_NUM7, "res/wav/7.wav" },
	{ AUDIO_NUM8, "res/wav/8.wav" },
	{ AUDIO_NUM9, "res/wav/9.wav" },
	{ AUDIO_DIAN, "res/wav/dian.wav" },
	{ AUDIO_SHI, "res/wav/shi.wav" },
	{ AUDIO_BAI, "res/wav/bai.wav" },
	{ AUDIO_QIAN, "res/wav/qian.wav" },
	{ AUDIO_WAN, "res/wav/wan.wav" },
	{ AUDIO_YUAN, "res/wav/yuan.wav" },
    { AUDIO_ANJIANYIN, "res/anjianyin.mp3" },
    { AUDIO_CHONGDZ, "res/chongdianzhong.mp3" },
    { AUDIO_SYLL, "res/wav/syllybzqzyxf.wav" },
    { AUDIO_DAOZHANG, "res/wav/daozhang.wav" },
    { AUDIO_SHOUKAN, "res/wav/shoukuan.wav" },
    { AUDIO_QSYLKLAPPSMJH, "res/wav/qsylklappsmjh.wav" },
    { AUDIO_44100PCMFILE, "res/qsylklappsmjh44100.pcm" },
    { AUDIO_16000PCMFILE, "res/anjianyin16000.pcm" },
};

//Global variables
//Function implementation

int ResGetExistLocalFilePath(char const *c_pszContent, void *pszPath, int iPathMaxLen)
{
    RES_LOG("%s start!", __func__);
    if ((c_pszContent == NULL) || (!strlen(c_pszContent)) || (pszPath == NULL))
    {
        RES_LOG("[ERR]%s param fail!");
        return YMI_ERR;
    }

    int iDex = 0;
    int iResCount = sizeof(s_stRes)/sizeof(s_stRes[0]);
    RES_LOG("c_pszContent = %s", c_pszContent);

    for (iDex = 0; iDex < iResCount; iDex++)
    {
        if ((strlen(c_pszContent) == strlen(s_stRes[iDex].c_pszContent)) && (!memcmp(c_pszContent, s_stRes[iDex].c_pszContent, strlen(s_stRes[iDex].c_pszContent))))
        {
            break;
        }
    }
    
    if (iDex >= iResCount)
    {
        RES_LOG("[ERR]no fond same str");
        return YMI_ERR;
    }

    memset(pszPath, 0x00, iPathMaxLen);
    snprintf(pszPath, iPathMaxLen, "%s", s_stRes[iDex].c_pszFileName);

    if (YMI_FsExist(pszPath) != YMI_OK)
    {
        RES_LOG("[ERR]%s file no exist!", pszPath);
        memset(pszPath, 0x00, iPathMaxLen);
        return YMI_ERR;
    }

    RES_LOG("%s file exist!", pszPath);
    return YMI_OK;
}

int ResCheckWavFile(char const *c_pszContent, void *pszPath, int iPathMaxLen)
{
    if ((c_pszContent == NULL) || (pszPath == NULL) || (!iPathMaxLen))
    {
        RES_LOG("%s param fail!");
        return YMI_ERR;
    }

    int iDex = 0;
    int iResCount = sizeof(s_stRes)/sizeof(s_stRes[0]);
    RES_LOG("c_pszContent = %s", c_pszContent);

    for (iDex = 0; iDex < iResCount; iDex++)
    {
        if ((strlen(c_pszContent) == strlen(s_stRes[iDex].c_pszContent)) && (!memcmp(c_pszContent, s_stRes[iDex].c_pszContent, strlen(s_stRes[iDex].c_pszContent))))
        {
            break;
        }
    }
    
    if (iDex >= iResCount)
    {
        RES_LOG("[ERR]no fond same str");
        return YMI_ERR;
    }
    
    memset(pszPath, 0x00, iPathMaxLen);
    // snprintf(pszPath, iPathMaxLen, "%s%s", AUDIO_RESOURCE_ROOT_PATH_WAV, s_stRes[iDex].c_pszFileName);
    snprintf(pszPath, iPathMaxLen, "%s", s_stRes[iDex].c_pszFileName);
    char *pcFormat = strstr(pszPath, ".mp3");
    if (pcFormat)
    {
        pcFormat[1] = 'w';
        pcFormat[2] = 'a';
        pcFormat[3] = 'v';
        pcFormat[4] = 0;
    }
    
    if (YMI_FsExist(pszPath) != YMI_OK)
    {
        RES_LOG("[ERR]%s file no exist!", pszPath);
        memset(pszPath, 0x00, iPathMaxLen);
        return YMI_ERR;
    }
    
    RES_LOG("%s file exist!", pszPath);
    return YMI_OK;
}
#endif
