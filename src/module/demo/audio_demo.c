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

//Macro definitions
#define AUDIOD_LOG_MODE
#ifdef AUDIOD_LOG_MODE
#define AUDIOD_LOG(msg, ...) LOG("[AUDIOD]"msg"", ##__VA_ARGS__)
#else
#define AUDIOD_LOG(msg, ...)
#endif

#define AUDIO_PCMBUF "pcmbuf"
#define AUDIO_44100PCMFILE "44100pcmfile"
#define AUDIO_16000PCMFILE "16000pcmfile"
#define AUDIO_SAOZHANG "credited"
#define AUDIO_SHOUKAN "Receipt"
#define AUDIO_SYLL "Remaining data is insufficient, please pay attention to renewal"
#define AUDIO_QSYLKLAPPSMJH "Please use the  APP to scan the code for activation"
#define AUDIO_CONGDZ "Charging"


//Constant
//Local variables
//Global variables

//Function implementation
extern void AudioPlayPcmBuf(EM_AUDIO_PRIORITY ePriority, int nSampleRate, int nChannels, void *pacPcm, int iPcmSize);

void pcmbuftest(EM_AUDIO_PRIORITY ePriority, int nSampleRate, int nChannels, char *pszFile)
{
    int nRet = 0;
    ulong iFileSize = 0;
    T_Fd tFd = 0;
    char *pszBuf = NULL;

    if (!pszFile || !strlen(pszFile))
    {
        AUDIOD_LOG("[ERR]file path fail!");

        return;
    }
    AUDIOD_LOG("pcmbuf test file = %s", pszFile);

    pszBuf = YMI_SysMalloc(16*1024);
    if (pszBuf == NULL)
    {
        AUDIOD_LOG("malloc audio msg err %d", 16*1024);
        return;
    }
    memset(pszBuf, 0, 16*1024);

    if (YMI_FsFileSize(pszFile, &iFileSize))
    {
        AUDIOD_LOG("[ERR]file size fail!");
        FREE(pszBuf);
        return;
    }
    AUDIOD_LOG("file size = %d", iFileSize);
    
    tFd = YMI_FsOpen(pszFile, "r+");
    if (tFd <= 0)
    {
        AUDIOD_LOG("open file %s err %d, %s", pszFile, tFd, strerror(errno));
        FREE(pszBuf);
        return;
    }
    YMI_FsRead(tFd, pszBuf, iFileSize);
    YMI_FsClose(tFd);
    AudioPlayPcmBuf(ePriority, nSampleRate, nChannels, pszBuf, iFileSize);
    FREE(pszBuf);
}

void audiotest(void)
{
    AUDIOD_LOG("%s start!", __func__);
    int i = 0;
    ST_Event stEvent = {0};

    while (YMI_AppIsReadyExit() != YMI_OK)
    {
        if(YMI_EventWait(&stEvent, 100) == YMI_OK)
		{
			if(stEvent.wId == TASK_MANAGE_EVENT_ID && stEvent.wParam1 == TASK_OPT_EXIT)
			{
				break;
			}
		}

        // YMI_AudioFileStart(EM_FILE_TYPE_MP3, "res/chongdianzhong.mp3", NULL);
        // WaitPlayEnd();
        // YMI_AudioFileStart(EM_FILE_TYPE_WAV, "res/syllybzqzyxf.wav", NULL);
        // WaitPlayEnd();
        // pcmbuftest(AUDIO_PRIORITY_HIGHEST, 16000, 1, "res/anjianyin16000.pcm");
        // YMI_AudioFileStart(EM_FILE_TYPE_WAV, "res/syllybzqzyxf.wav", NULL);
        // WaitPlayEnd();

        switch ((i++)%4)
        {
        case 0:
            // AudioPlay(AUDIO_PRIORITY_LOWEST, FILE_TEXT, 0, 0, NULL, 2, AUDIO_DAOZHANG, AUDIO_CHONGDZ);
            // SLEEP_MS(1000);
            // AudioPlay(AUDIO_PRIORITY_MIDDLE, FILE_TEXT, 0, 0, NULL, 1, AUDIO_CHONGDZ);
            // AudioPlay(AUDIO_PRIORITY_LOWEST, AMT_TEXT, 0, 0, NULL, 3, AUDIO_SHOUKAN, "{$100.25}", "{$56.00}");
            // SLEEP_MS(2000);
            // AudioPlay(AUDIO_PRIORITY_LOWEST, AMT_TEXT, 0, 0, NULL, 4, AUDIO_SHOUKAN, "{$0.5}", "{$1.02}", AUDIO_CHONGDZ);
            // SLEEP_MS(2000);
            // AudioPlay(AUDIO_PRIORITY_LOWEST, AMT_TEXT, 0, 0, NULL, 3, AUDIO_SHOUKAN, "{$123456}", AUDIO_SHOUKAN);
            // SLEEP_MS(2000);
            AudioPlay(AUDIO_PRIORITY_LOWEST, FILE_TEXT, 0, 0, NULL, 1, AUDIO_QSYLKLAPPSMJH);
            SLEEP_MS(1000);
            AudioPlay(AUDIO_PRIORITY_LOWEST, FILE_TEXT, 0, 0, NULL, 2, AUDIO_SHOUKAN, AUDIO_QSYLKLAPPSMJH);
            SLEEP_MS(1000);
            break;
        case 1:
            // AudioPlay(AUDIO_PRIORITY_ABOVE, FILE_PATH, 0, 0, NULL, 1, "res/syllybzqzyxf.wav", "res/chongdianzhong.mp3");
            // SLEEP_MS(1000);
            // AudioPlay(AUDIO_PRIORITY_MIDDLE, FILE_PATH, 0, 0, NULL, 1, "res/chongdianzhong.mp3");
            // SLEEP_MS(1000);
            AudioPlay(AUDIO_PRIORITY_MIDDLE, FILE_PATH, 0, 0, NULL, 2, "res/syllybzqzyxf.wav", "res/chongdianzhong.mp3");
            SLEEP_MS(1000);
            AudioPlay(AUDIO_PRIORITY_MIDDLE, FILE_PATH, 0, 0, NULL, 1, "res/chongdianzhong.mp3");
            SLEEP_MS(1000);
            break;
        case 2:
            // AudioPlay(AUDIO_PRIORITY_MIDDLE, AMT_TEXT, 0, 0, NULL, 5, AUDIO_SHOUKAN, "{$100.25}", "{$1.02}", "{$52.30}", "{$56.00}");
            AudioPlay(AUDIO_PRIORITY_ABOVE, FILE_TEXT, 0, 0, NULL, 1, AUDIO_QSYLKLAPPSMJH);
            SLEEP_MS(1000);
            break;
        case 3:
            // pcmbuftest(AUDIO_PRIORITY_HIGHEST, 16000, 1, "res/anjianyin16000.pcm");
            // pcmbuftest(AUDIO_PRIORITY_HIGHEST, 44100, 1, "res/qsylklappsmjh44100.pcm");
            AudioPlay(AUDIO_PRIORITY_HIGHEST, FILE_TEXT, 0, 0, NULL, 1, AUDIO_QSYLKLAPPSMJH);
            SLEEP_MS(5000);
            // AudioPlay(AUDIO_PRIORITY_LOWEST, AMT_TEXT, 0, 0, NULL, 3, AUDIO_SHOUKAN, "{$100.25}", "{$56.00}");
            // SLEEP_MS(5000);
            // AudioPlay(AUDIO_PRIORITY_LOWEST, AMT_TEXT, 0, 0, NULL, 4, AUDIO_SHOUKAN, "{$0.5}", "{$1.02}", AUDIO_CHONGDZ);
            // SLEEP_MS(5000);
            // AudioPlay(AUDIO_PRIORITY_LOWEST, AMT_TEXT, 0, 0, NULL, 3, AUDIO_SHOUKAN, "{$123456}", AUDIO_SHOUKAN);
            // SLEEP_MS(5000);
            break;
        default:
            break;
        }
        AUDIOD_LOG("audio test task run!");
        YMI_SysMsDelay(10 * 1000);
    }
    AUDIOD_LOG("audio test exit!");
}

