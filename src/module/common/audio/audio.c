/**********************************************************************
* Copyright (C)    Dspread
* File name: audio.c
* Content summary: audio playback module
*
* Historical Record:
* Modified by Date Describes the content of the modification
      lhx 2025.4.15
***********************************************************************/
//Header file
#include "var.h"
#include "numplay.h"
#include "wavmix.h"
#include "res.h"
#include <stdarg.h>

//Macro definitions
#define AUDIO_LOG_MODE
#ifdef AUDIO_LOG_MODE
#define AUDIO_LOG(msg, ...) LOG("[AUDIO]"msg"", ##__VA_ARGS__)
#else
#define AUDIO_LOG(msg, ...)
#endif

#define AUDIO_TASK_STACK_SIZE (22 * 1024)
#define AUDIO_TASK_EVENT_NUM  (5)
#define AUDIO_TASK_WAIT_TIME_MS  (100)
#define AUDIO_PALY_STATUS_WAITTIME  (500)
#define AUDIO_ITEM_VOICE_MSG_MAX  (256)
#define AUDIO_VOICE_MSG_MAX (1024)

#define AUDIO_ARGV_DELIMITER    '|'

//Constant
//Local variables
static T_Task s_tAudioTaskID = 0;
static T_Queue tPayQueue = 0;
static T_Queue tAdverQueue = 0;
static T_Queue tKeyQueue = 0;
static T_Queue tLowQueue = 0;
static EM_AUDIO_PRIORITY ePlayingPriorityLev = AUDIO_PRIORITY_NONE;
static ST_AudioPlayingInfo stPlayingInfo = {0};
//Global variables

//Function implementation
void WaitPlayEnd(void)
{
    int iRet = -1;
    iRet = YMI_AudioPlayStatus(AUDIO_PALY_STATUS_WAITTIME);
    AUDIO_LOG("play %d,%x", iRet, iRet);
    while (iRet != YMI_OK)
    {
        iRet = YMI_AudioPlayStatus(AUDIO_PALY_STATUS_WAITTIME);
        AUDIO_LOG("play %d", iRet);
    }
}

int AudioPlayOverCB(char *pData, int iLen, EM_AUD_PLAYER_STATE eState)
{
    AUDIO_LOG("%s start!", __func__);
    switch (eState)
    {
    case EM_AUD_PLAYER_ERROR:
        AUDIO_LOG("player err!");
        break;
    case EM_AUD_PLAYER_START:
        AUDIO_LOG("player start");
        break;
    case EM_AUD_PLAYER_PAUSE:
        AUDIO_LOG("player pause");
        break;
    case EM_AUD_PLAYER_FINISHED:
        AUDIO_LOG("player finished");
        break;
    case EM_AUD_PLAYER_CLOSE:
        AUDIO_LOG("player close");
        break;
    case EM_AUD_PLAYER_RESUME:
        AUDIO_LOG("player resume");
        break;
    default:
        break;
    }
    return YMI_OK;
}

void CleanPlayingMsg(void)
{
    AUDIO_LOG("%s start!", __func__);
    if (stPlayingInfo.pszVoiceMsg != NULL)
    {
        AUDIO_LOG("clean voicemsg start!");
        memset(stPlayingInfo.pszVoiceMsg, 0x00, stPlayingInfo.wVoiceMsgLen);
    }
}

static void AudioPlayStop(void)
{
    AUDIO_LOG("%s start!", __func__);
    CleanPlayingMsg();
    YMI_AudioTTSStop();
    YMI_AudioFileStop();
    // if (g_nPcmPlaying == 1)
    // {
    //     SLEEP_MS(10);
    //     YMI_AudioPCMDataStop();
    // }
}

void AudioQueueClean(T_Queue tQueue)
{
    AUDIO_LOG("%s start!", __func__);
    ST_AudioInfo stAudioInfo = {0};
    uint iCnt = 0;
    YMI_QueueGetCount(tQueue, &iCnt);
    for (int i = 0; i < iCnt; i++)
    {
        YMI_QueueRead(tQueue, (uint8_t *)&stAudioInfo, sizeof(ST_AudioInfo), 0);
        FREE(stAudioInfo.pszVoiceMsg);
        stAudioInfo.pszVoiceMsg = NULL;
    }
}

void AudioPriorityQueueClean(EM_AUDIO_PRIORITY ePriority)
{
    switch (ePriority)
    {
    case AUDIO_PRIORITY_HIGHEST:
        AudioQueueClean(tPayQueue);
        break;
    case AUDIO_PRIORITY_ABOVE:
        AudioQueueClean(tAdverQueue);
        break;
    case AUDIO_PRIORITY_MIDDLE:
        AudioQueueClean(tKeyQueue);
        break;
    case AUDIO_PRIORITY_LOWEST:
        AudioQueueClean(tLowQueue);
        break;
    default:
        break;
    }
}

int AudioSendQueue(ST_AudioInfo *pstAudioInfo, EM_AUDIO_PRIORITY ePriority)
{
    AUDIO_LOG("%s start!", __func__);
    int iRet = YMI_ERR;

    if (pstAudioInfo == NULL)
    {
        AUDIO_LOG("[ERR]%s param fail!", __func__);
        return YMI_ERR;
    }
    AUDIO_LOG("%s start!", __func__);
    switch (ePriority)
    {
    case AUDIO_PRIORITY_HIGHEST:
        iRet = YMI_QueueWrite(tPayQueue, (uint8_t *)pstAudioInfo, sizeof(ST_AudioInfo), 0);
        break;
    case AUDIO_PRIORITY_ABOVE:
        iRet = YMI_QueueWrite(tAdverQueue, (uint8_t *)pstAudioInfo, sizeof(ST_AudioInfo), 0);
        break;
    case AUDIO_PRIORITY_MIDDLE:
        iRet = YMI_QueueWrite(tKeyQueue, (uint8_t *)pstAudioInfo, sizeof(ST_AudioInfo), 0);
        break;
    case AUDIO_PRIORITY_LOWEST:
        iRet = YMI_QueueWrite(tLowQueue, (uint8_t *)pstAudioInfo, sizeof(ST_AudioInfo), 0);
        break;
    default:
        break;
    }

    if (iRet != YMI_OK)
    {
        FREE(pstAudioInfo->pszVoiceMsg);
        pstAudioInfo->pszVoiceMsg = NULL;
    }
    return iRet;
}

int AudioSend(void *pszVoceMsg, EM_AUDIO_PRIORITY ePriority, EM_AUDIO_MSG_TYPE eAudioType, uint wAmount, uint iTimeStamp, CB_ON_PLAYER cb_PlayOverFunc)
{
    AUDIO_LOG("%s start!", __func__);
    ulong wMsgLen = 0;
    ST_AudioInfo stAudioInfo = {0};

    if (pszVoceMsg == NULL)
    {
        AUDIO_LOG("[ERR]queue fail!");
        return YMI_ERR;
    }
    
    if ((tKeyQueue == 0) || (tAdverQueue == 0) || (tPayQueue == 0) || (tLowQueue == 0))
    {
		AUDIO_LOG("[ERR]queue fail!");
        return YMI_ERR;
    }

    wMsgLen = strlen(pszVoceMsg);
    stAudioInfo.pszVoiceMsg = MALLOC(wMsgLen+1, char);
    if (stAudioInfo.pszVoiceMsg == NULL)
        return YMI_ERR;
    memset(stAudioInfo.pszVoiceMsg, 0x00, wMsgLen+1);
    memcpy(stAudioInfo.pszVoiceMsg, pszVoceMsg, wMsgLen);
    stAudioInfo.wVoiceMsgLen = wMsgLen;
    stAudioInfo.ePriority = ePriority;
    stAudioInfo.eAudioType = eAudioType;
    stAudioInfo.wAmount = wAmount;
    stAudioInfo.iTimeStamp = iTimeStamp;
    if (cb_PlayOverFunc)
    {
        stAudioInfo.cb_PlayOverFunc = cb_PlayOverFunc;
    }
    else
    {
        stAudioInfo.cb_PlayOverFunc = AudioPlayOverCB;
    }
    
    return AudioSendQueue(&stAudioInfo, ePriority);
}

void AudioPriorityAnaly(EM_AUDIO_PRIORITY ePriority)
{
    AUDIO_LOG("%s start!", __func__);
    AUDIO_LOG("playing priority = %d, new priority = %d", ePlayingPriorityLev, ePriority);
    switch (ePriority)
    {
    case AUDIO_PRIORITY_HIGHEST:
        if ((ePlayingPriorityLev != AUDIO_PRIORITY_NONE) && (!(ePlayingPriorityLev & AUDIO_PRIORITY_HIGHEST)))
        {
            AudioPlayStop();
        }
        break;
    case AUDIO_PRIORITY_ABOVE:
        if ((ePlayingPriorityLev != AUDIO_PRIORITY_NONE) && (!((ePlayingPriorityLev & AUDIO_PRIORITY_HIGHEST) || (ePlayingPriorityLev & AUDIO_PRIORITY_ABOVE))))
        {
            AudioPlayStop();
        }
        break;
    case AUDIO_PRIORITY_MIDDLE:
        if((ePlayingPriorityLev != AUDIO_PRIORITY_NONE) && (ePlayingPriorityLev & AUDIO_PRIORITY_LOWEST))
        {
            AudioPlayStop();
        }
        break;
    case AUDIO_PRIORITY_LOWEST:
        break;
    default:
        break;
    }
}

/* voiceMsg format: nSampleRate|nChannels|xxx */
void AudioPlayPcmBuf(EM_AUDIO_PRIORITY ePriority, int nSampleRate, int nChannels, void *pacPcm, int iPcmSize)
{
    ulong wMsgLen = 0;
    char *pszBuf = NULL, szTmp[32] = {0};
    ST_AudioInfo stAudioInfo = {0};
    
    if ((tKeyQueue == 0) || (tAdverQueue == 0) || (tPayQueue == 0) || (tLowQueue == 0))
    {
		AUDIO_LOG("[ERR]queue fail!");
        return;
    }

    sprintf(szTmp, "%d|%d|", nSampleRate, nChannels);
    wMsgLen = iPcmSize + strlen(szTmp);
    pszBuf = MALLOC(wMsgLen + 1, char);
    if (pszBuf == NULL)
    {
        AUDIO_LOG("[ERR]voice msg tmp malloc fail!");
        return;
    }
    memset(pszBuf, 0x00, wMsgLen + 1);
    memcpy(pszBuf, szTmp, strlen(szTmp));
    memcpy(pszBuf + strlen(szTmp), pacPcm, iPcmSize);
    stAudioInfo.pszVoiceMsg = pszBuf;
    stAudioInfo.wVoiceMsgLen = wMsgLen;
    stAudioInfo.ePriority = ePriority;
    stAudioInfo.eAudioType = PCM_BUF;

    AudioPriorityAnaly(ePriority);
    AudioSendQueue(&stAudioInfo, ePriority);
}

void AudioPlay(EM_AUDIO_PRIORITY ePriority, EM_AUDIO_MSG_TYPE eAudioType, uint wAmount, uint iTimeStamp, CB_ON_PLAYER cb_PlayOverFunc, int iCnt, ...)
{
    AUDIO_LOG("%s start!", __func__);
    va_list tArg;
    const char *pTmp = NULL;
    char *pszBuf = NULL;

    va_start(tArg, iCnt);

    if (!iCnt)
    {
        AUDIO_LOG("[ERR]");
        goto exit;
    }
    
    pszBuf = MALLOC(AUDIO_VOICE_MSG_MAX, char);
    if (pszBuf == NULL)
    {
        AUDIO_LOG("[ERR]voice msg tmp malloc fail!");
        return;
    }
    memset(pszBuf, 0x00, AUDIO_VOICE_MSG_MAX);

    for (int i = 0; i < iCnt; i++)
    {
        pTmp = va_arg(tArg, char *);
        AUDIO_LOG("arg %d %s", i, pTmp);
        if (strlen(pszBuf) + strlen(pTmp) >= AUDIO_VOICE_MSG_MAX)
        {
            AUDIO_LOG("[ERR]audio msg to long");
            goto exit;
        }
        sprintf(pszBuf+strlen(pszBuf), "%s|", pTmp);
    }
    // pszBuf[strlen(pszBuf) - 1] = 0;
    AUDIO_LOG("pszBuf = %s", pszBuf);
    AudioPriorityAnaly(ePriority);
    AudioSend(pszBuf, ePriority, eAudioType, wAmount, iTimeStamp, cb_PlayOverFunc);
exit:
    va_end(tArg);
    FREE(pszBuf);
}

int AudioPlayPcmFile(void *pszPath)
{
    AUDIO_LOG("%s start!", __func__);
    EM_FILE_TYPE eType = EM_FILE_MAX_NUM;

    if ((pszPath == NULL) || (!strlen(pszPath)))
    {
        AUDIO_LOG("[ERR]path fail");
        return YMI_ERR;
    }

    if (YMI_FsExist(pszPath) != YMI_OK)
    {
        AUDIO_LOG("[ERR]%s path no exist!", pszPath);
        return YMI_ERR;
    }

    // return YMI_AudioFileStart(eType, pszPath, AudioPlayOverCB);
    return YMI_OK;
}

int AudioPlayFile(EM_FILE_TYPE eType, void *pszPath, CB_ON_PLAYER cb_PlayOverFunc)
{
    int iRet = YMI_ERR;
    CB_ON_PLAYER cb_PlayOver = NULL;

    if ((pszPath == NULL) || (!strlen(pszPath)))
    {
        AUDIO_LOG("[ERR]path fail");
        return YMI_ERR;
    }

    if (YMI_FsExist(pszPath) != YMI_OK)
    {
        AUDIO_LOG("[ERR]%s path no exist!", pszPath);
        return YMI_ERR;
    }

    if (cb_PlayOverFunc)
    {
        cb_PlayOver = cb_PlayOverFunc;
    }
    else
    {
        cb_PlayOver = AudioPlayOverCB;
    }

    WaitPlayEnd();
    iRet = YMI_AudioFileStart(eType, pszPath, cb_PlayOver);
    if (iRet != YMI_OK)
    {
        YMI_AudioFileStop();
        AUDIO_LOG("[ERR]%s play fail!%d,%x", pszPath, iRet, iRet);
        return YMI_ERR;
    }
    WaitPlayEnd();
    YMI_AudioFileStop();
}

int AudioPlayLocalFile(void *pszPath, CB_ON_PLAYER cb_PlayOverFunc)
{
    AUDIO_LOG("%s start!", __func__);
    EM_FILE_TYPE eType = EM_FILE_MAX_NUM;

    if ((pszPath == NULL) || (!strlen(pszPath)))
    {
        AUDIO_LOG("[ERR]path fail");
        return YMI_ERR;
    }

    if (YMI_FsExist(pszPath) != YMI_OK)
    {
        AUDIO_LOG("[ERR]%s path no exist!", pszPath);
        return YMI_ERR;
    }

    if (strstr(pszPath, ".mp3") || strstr(pszPath, ".MP3") || strstr(pszPath, ".amr") || strstr(pszPath, ".AMR"))
    {
        eType = EM_FILE_TYPE_MP3;
    }
    else if (strstr(pszPath, ".wav") || strstr(pszPath, ".WAV"))
    {
        eType = EM_FILE_TYPE_WAV;
    }
    else if (strstr(pszPath, ".pcm") || strstr(pszPath, ".PCM"))
    {
        eType = EM_FILE_TYPE_PCM;
    }
    else
    {
        AUDIO_LOG("[ERR]%s file type err!", pszPath);
        return YMI_ERR;
    }

    switch (eType)
    {
    case EM_FILE_TYPE_MP3:
    case EM_FILE_TYPE_WAV:
        AudioPlayFile(eType, pszPath, cb_PlayOverFunc);
        break;
    case EM_FILE_TYPE_PCM:
        AudioPlayPcmFile(pszPath);
        break;
    default:
        break;
    }

    
    return YMI_OK;
}

int AudioPlayFileUTF8(void *pvVoiceMsg, CB_ON_PLAYER cb_PlayOverFunc)
{
    AUDIO_LOG("%s start!", __func__);
    int iRet = YMI_ERR;
    char szPath[AUDIO_FILE_PATH_MAX] = {0};

    if ((pvVoiceMsg == NULL) || (!strlen(pvVoiceMsg)))
    {
        AUDIO_LOG("[ERR]%s param fail!", __func__);
        return YMI_ERR;
    }
     AUDIO_LOG("[%s] [%s]!", pvVoiceMsg,szPath);
    if (ResGetExistLocalFilePath(pvVoiceMsg, szPath, sizeof(szPath)) == YMI_OK)
    {
        iRet = AudioPlayLocalFile(szPath, cb_PlayOverFunc);
    }
    else
    {
        AUDIO_LOG("[ERR]local path get fail or path no exist!");
        // AUDIO_LOG("start utf8 str");
        // iRet = AudioPlayUTF8(pvVoiceMsg, strlen(pvVoiceMsg));
        // SLEEP_MS(100);
    }
    return iRet;
}

int AudioPlayFileMulti(ST_AudioInfo *pstAudioInfo)
{
    if ((pstAudioInfo == NULL) && (pstAudioInfo->pszVoiceMsg == NULL))
    {
        AUDIO_LOG("[ERR]%s param or voicemsg is null", __func__);
        return YMI_ERR;
    }

    char acItemMsg[AUDIO_ITEM_VOICE_MSG_MAX] = {0};
    char *pcStart = pstAudioInfo->pszVoiceMsg;
    char *pcEnd = pstAudioInfo->pszVoiceMsg + strlen(pstAudioInfo->pszVoiceMsg);
    char *pcCut = strchr(pcStart, AUDIO_ARGV_DELIMITER);

    while (pcCut && (pcCut < pcEnd) && (pcCut >= pcStart))
    {
        if (pcCut - pcStart > 0)
        {
            BUFCLR(acItemMsg);
            memcpy(acItemMsg, pcStart, pcCut - pcStart);
            AudioPlayFileUTF8(acItemMsg, pstAudioInfo->cb_PlayOverFunc);
        }
        pcCut ++;
        if (pcCut < pcEnd)
        {
            pcStart = pcCut;
            pcCut = strchr(pcStart, AUDIO_ARGV_DELIMITER);
        }
    }
    return YMI_OK;
}

int AudioPlayFilePathMulti(ST_AudioInfo *pstAudioInfo)
{
    if ((pstAudioInfo == NULL) && (pstAudioInfo->pszVoiceMsg == NULL))
    {
        AUDIO_LOG("[ERR]%s param or voicemsg is null", __func__);
        return YMI_ERR;
    }

    char acItemMsg[AUDIO_ITEM_VOICE_MSG_MAX] = {0}, szMp3FileName[AUDIO_ITEM_VOICE_MSG_MAX] = {0};
    char *pcStart = pstAudioInfo->pszVoiceMsg;
    char *pcEnd = pstAudioInfo->pszVoiceMsg + strlen(pstAudioInfo->pszVoiceMsg);
    char *pcCut = strchr(pcStart, AUDIO_ARGV_DELIMITER);

    while (pcCut && (pcCut < pcEnd) && (pcCut >= pcStart))
    {
        if (pcCut - pcStart > 0)
        {
            BUFCLR(acItemMsg);
            memcpy(acItemMsg, pcStart, pcCut - pcStart);
            AudioPlayLocalFile(acItemMsg, pstAudioInfo->cb_PlayOverFunc);
            BUFCLR(szMp3FileName);
        }
        pcCut ++;
        if (pcCut < pcEnd)
        {
            pcStart = pcCut;
            pcCut = strchr(pcStart, AUDIO_ARGV_DELIMITER);
        }
    }

    return YMI_OK;
}

int AudioPlayQlcyFile(const char *c_pszPrePath, const char *c_pszFileName, CB_ON_PLAYER cb_PlayOverFunc)
{
    AUDIO_LOG("%s start!", __func__);
    char acPath[AUDIO_FILE_PATH_MAX] = {0};
    if (c_pszFileName == NULL)
    {
        AUDIO_LOG("[ERR]%s param fail!", __func__);
        return YMI_ERR;
    }

	snprintf(acPath,sizeof(acPath),"%s%s",c_pszPrePath,c_pszFileName);
    AUDIO_LOG("qlcy file path = %s", acPath);
    if(YMI_FsExist(acPath) != YMI_OK)
    {
        AUDIO_LOG("%s: file %s is not exist",acPath);
        return YMI_ERR;
    }

    return AudioPlayLocalFile(acPath, cb_PlayOverFunc);
}

/* 
 * isN Whether or not to simply broadcast numbers, 0 broadcast the amount, 1 Broadcast the numbers
 * Yuan Whether to increase the monetary unit, 0 Increase the unit of currency, 1 No increase in monetary units
*/
int AudioPlayNum(const char *c_pszNumStr, bool bIsN, bool bYuan)
{
    AUDIO_LOG("num str = %s, isN = %d, yuan = %d", c_pszNumStr, bIsN, bYuan);
    if (YmWavPlayNum(c_pszNumStr, bIsN, bYuan) != YMI_OK)
    {   /* wav file broadcast failed, tts play */
        char acItemMsg[AUDIO_ITEM_VOICE_MSG_MAX * 2] = {0};
        BUFCLR(acItemMsg);
        if (!bIsN)
        {
            if (bYuan)
            {
                sprintf(acItemMsg, "%s%s", c_pszNumStr, AUDIO_YUAN);
            }
            else
            {
                memcpy(acItemMsg, c_pszNumStr, strlen(c_pszNumStr));
            }
        }
        else
        {
            for (int i = 0; i < strlen(c_pszNumStr); i++)
            {
                acItemMsg[2*i] = c_pszNumStr[i];
                acItemMsg[2*i+1] = ' ';
            }
            if (bYuan)
            {
                memcpy(&acItemMsg[strlen(acItemMsg)-1], AUDIO_YUAN, strlen(AUDIO_YUAN));
            }
        }
        // AudioPlayUTF8(acItemMsg, strlen(acItemMsg));
    }

    return YMI_OK;
}

int PayMsgAnaly(ST_AudioInfo *pstAudioInfo)
{
    AUDIO_LOG("%s start!", __func__);
    if (pstAudioInfo == NULL)
    {
        AUDIO_LOG("[ERR]%s param is null", __func__);
        return YMI_ERR;
    }
    if ((pstAudioInfo->pszVoiceMsg == NULL) || !strlen(pstAudioInfo->pszVoiceMsg))
    {
        AUDIO_LOG("[ERR]%s voicemsg is null", __func__);
        return YMI_ERR;
    }

    bool bIsQlcy = false;
    char acItemMsg[AUDIO_ITEM_VOICE_MSG_MAX] = {0}, acPrePath[AUDIO_FILE_PATH_MAX] = {0};;
    char *pcStart = pstAudioInfo->pszVoiceMsg;
    char *pcEnd = pstAudioInfo->pszVoiceMsg + strlen(pstAudioInfo->pszVoiceMsg);
    char *pcCut = strchr(pcStart, AUDIO_ARGV_DELIMITER);

    if (pcCut && (pcCut <= pcEnd) && (pcStart[0] == '#') && (pcStart[1] == '#'))
    {
        memcpy(acPrePath, (const void *)&pcStart[2], pcCut - &pcStart[2]);
        AUDIO_LOG("qlcy pre path = %s", acPrePath);
        bIsQlcy = true;
        pcStart = pcCut + 1;
        pcCut = strchr(pcStart, AUDIO_ARGV_DELIMITER);
    }

    while (pcCut && (pcCut < pcEnd) && (pcCut >= pcStart))
    {
        if (pcCut - pcStart > 0)
        {
            BUFCLR(acItemMsg);
            memcpy(acItemMsg, pcStart, pcCut - pcStart);
            AUDIO_LOG("voice msg = %s", acItemMsg);
            if (acItemMsg[0] == '{')
            {
                acItemMsg[strlen(acItemMsg) - 1] = 0;   /* remove "}" */
                if (acItemMsg[1] == '$')
                {
                    AUDIO_LOG("play amt str %s", &acItemMsg[2]);
                    AudioPlayNum(&acItemMsg[2], false, true);
                }
                else if (acItemMsg[1] == 'N')
                {
                    AUDIO_LOG("play num str %s", &acItemMsg[2]);
                    AudioPlayNum(&acItemMsg[2], true, false);
                }
            }
            else
            {
                if (bIsQlcy)
                {
                    AudioPlayQlcyFile(acPrePath, acItemMsg, pstAudioInfo->cb_PlayOverFunc);
                }
                else
                {
                    AudioPlayFileUTF8(acItemMsg, pstAudioInfo->cb_PlayOverFunc);
                }
            }
        }
        pcCut ++;
        if (pcCut < pcEnd)
        {
            pcStart = pcCut;
            pcCut = strchr(pcStart, AUDIO_ARGV_DELIMITER);
        }
    }

    return YMI_OK;
}

void AudioPlayAMT(ST_AudioInfo *pstAudioInfo)
{
    AUDIO_LOG("%s start!", __func__);
    PayMsgAnaly(pstAudioInfo);
}

void AudioPcmBufPlay(ST_AudioInfo *pstAudioInfo)
{
    AUDIO_LOG("%s start!", __func__);
    char *pTmp = NULL, *pPcmBuf = NULL;
    int iPcmBufLen = 0, nSampleRate = 0, nChannels = 0;

    if ((pstAudioInfo == NULL) || (pstAudioInfo->pszVoiceMsg == NULL))
    {
        AUDIO_LOG("[ERR]%s param fail!", __func__);
        return;
    }

    pTmp = strchr(pstAudioInfo->pszVoiceMsg, AUDIO_ARGV_DELIMITER);
    if (pTmp == NULL)
    {
        AUDIO_LOG("[ERR]%s pcm buf param fail!", __func__);
        return;
    }
    pTmp = strchr(++pTmp, AUDIO_ARGV_DELIMITER);
    if (pTmp == NULL)
    {
        AUDIO_LOG("[ERR]%s pcm buf param fail!", __func__);
        return;
    }
    pPcmBuf = pTmp + 1;
    if (pstAudioInfo->pszVoiceMsg[pstAudioInfo->wVoiceMsgLen - 1] == AUDIO_ARGV_DELIMITER)
    {
        pstAudioInfo->pszVoiceMsg[pstAudioInfo->wVoiceMsgLen - 1] = 0;
        iPcmBufLen = pstAudioInfo->wVoiceMsgLen - (pPcmBuf - pstAudioInfo->pszVoiceMsg) - 1;
    }
    else
    {
        iPcmBufLen = pstAudioInfo->wVoiceMsgLen - (pPcmBuf - pstAudioInfo->pszVoiceMsg);
    }
    sscanf(pstAudioInfo->pszVoiceMsg, "%d|%d|", &nSampleRate, &nChannels);
    AUDIO_LOG("pcm buf len = %d, sample rate = %d, channels = %d", iPcmBufLen, nSampleRate, nChannels);

    YMI_AudioPCMComfig(nSampleRate, nChannels);
    YMI_AudioPCMDataStart();
    YMI_AudioPCMPlay(pPcmBuf, iPcmBufLen);
    YMI_AudioPCMDataStop();
}

void AudioTask(void *pvParam)
{
    ST_Event stEvent = {0};
    ST_AudioInfo stAudioInfo = {0};
    EM_VOL_LEVEL eVol = 0;

    while(1)
    {
        if(YMI_EventWait(&stEvent, 100) == YMI_OK)
		{
			if(stEvent.wId == TASK_MANAGE_EVENT_ID && stEvent.wParam1 == TASK_OPT_EXIT)
			{
				break;
			}
		}

        if (YMI_QueueRead(tPayQueue, (uint8_t *)&stAudioInfo, sizeof(ST_AudioInfo), AUDIO_TASK_WAIT_TIME_MS) != YMI_OK)
        {
            if (YMI_QueueRead(tAdverQueue, (uint8_t *)&stAudioInfo, sizeof(ST_AudioInfo), AUDIO_TASK_WAIT_TIME_MS) != YMI_OK)
            {
                if (YMI_QueueRead(tKeyQueue, (uint8_t *)&stAudioInfo, sizeof(ST_AudioInfo), AUDIO_TASK_WAIT_TIME_MS) != YMI_OK)
                {
                    if (YMI_QueueRead(tLowQueue, (uint8_t *)&stAudioInfo, sizeof(ST_AudioInfo), AUDIO_TASK_WAIT_TIME_MS) != YMI_OK)
                    {
                        continue;
                    }
                }
            }
        }

        if (!stAudioInfo.pszVoiceMsg || !strlen(stAudioInfo.pszVoiceMsg))
        {
            continue;
        }
        stPlayingInfo.pszVoiceMsg = stAudioInfo.pszVoiceMsg;
        stPlayingInfo.wVoiceMsgLen = stAudioInfo.wVoiceMsgLen;
        ePlayingPriorityLev = stAudioInfo.ePriority;
        AUDIO_LOG("play %s, type = %d", stAudioInfo.pszVoiceMsg, stAudioInfo.eAudioType);
        switch (stAudioInfo.eAudioType)
        {
        case FILE_TEXT:
            AudioPlayFileMulti(&stAudioInfo);
            break;
        case FILE_PATH:
            AudioPlayFilePathMulti(&stAudioInfo);
            break;
        case AMT_TEXT:
            AudioPlayAMT(&stAudioInfo);
            break;
        case PCM_BUF:
            AudioPcmBufPlay(&stAudioInfo);
            break;
        default:
            break;
        }
        FREE(stAudioInfo.pszVoiceMsg);
        stAudioInfo.pszVoiceMsg = NULL;
        memset(&stAudioInfo, 0x00, sizeof(ST_AudioInfo));
        stPlayingInfo.pszVoiceMsg = NULL;
        stPlayingInfo.wVoiceMsgLen = 0;
    }
    YMI_AudioFileStop();
    YMI_TaskDelete(s_tAudioTaskID);
}

int AudioTaskInit(void)
{
    AUDIO_LOG("%s start!", __func__);
    int iRet;
    
    iRet = YMI_QueueCreat(&tPayQueue, sizeof(ST_AudioInfo), 50);
    if (iRet != YMI_OK)
    {
        AUDIO_LOG("[ERR]pay queue creat iRet=%d", iRet);
		return YMI_ERR;
    }
    LOG("tPayQueue = %d,%x",tPayQueue, tPayQueue);

    iRet = YMI_QueueCreat(&tAdverQueue, sizeof(ST_AudioInfo), 50);
    if (iRet != YMI_OK)
    {
        AUDIO_LOG("[ERR]pay queue creat iRet=%d", iRet);
		return YMI_ERR;
    }
    LOG("tAdverQueue = %d,%x",tAdverQueue, tAdverQueue);

    iRet = YMI_QueueCreat(&tKeyQueue, sizeof(ST_AudioInfo), 50);
    if (iRet != YMI_OK)
    {
        AUDIO_LOG("[ERR]pay queue creat iRet=%d", iRet);
		return YMI_ERR;
    }
    LOG("tKeyQueue = %d,%x",tKeyQueue, tKeyQueue);
    
    iRet = YMI_QueueCreat(&tLowQueue, sizeof(ST_AudioInfo), 1);
    if (iRet != YMI_OK)
    {
        AUDIO_LOG("[ERR]pay queue creat iRet=%d", iRet);
		return YMI_ERR;
    }
    LOG("tLowQueue = %d,%x",tLowQueue, tLowQueue);

    WavPcmPlayTaskInit();

	iRet = YMI_TaskCreat(&s_tAudioTaskID, "audio_task", AUDIO_TASK_STACK_SIZE, AudioTask, NULL, EM_TASK_PRIORITY_BELOW_HIGH, AUDIO_TASK_EVENT_NUM);
	if(iRet != YMI_OK)
	{
		AUDIO_LOG("[ERR]task creat iRet=%d", iRet);
		return YMI_ERR;
	}
    
	return YMI_OK;
}
