/**********************************************************************
* Copyright (C)    Dspread
* File name: wavmix.c
* Content summary: Audio-related APIs
*
***********************************************************************/
//Header file
#include <stdarg.h>
#include "var.h"
#include "YMAPI.h"
#include "wavmix.h"

//Macro definitions
#define WAVMIX_LOG_MODE
#ifdef WAVMIX_LOG_MODE
#define WAVMIX_LOG(msg, ...) LOG("[WAVMIX]"msg"", ##__VA_ARGS__)
#else
#define WAVMIX_LOG(msg, ...)
#endif

#define WAV_MIX_TASK_STACK_SIZE (10 * 1024)
#define WAV_MIX_TASK_EVENT_NUM  (1)
#define WAV_PLAY_TIME_OUT_S     (5 * 1000)
#define WAV_MIX_LIST_WAITE_PLAY_FINISH_S  (5 * 1000)
#define WAV_MIX_PLAY_BUFF_MIN_SIZE  (10 * 1024)
#define WAV_MIX_PLAY_BUFF_MAX_SIZE  (180 * 1024)

//Constant
//Local variables
static T_Task s_tWavMixTaskId = 0;
EN_WAV_PLAY_STATUS s_eWavPlayStatus = WAV_PLAY_STOP;
int g_nPcmPlaying = 0;
static void *s_pvWavPlayBuff = NULL;
static void *s_pvWavPlayBuff1 = NULL;
static int s_wLenPlayBuff1 = 0;
static void *s_pvWavPlayBuff2 = NULL;
static int s_wLenPlayBuff2 = 0;
static uint32_t s_wSampleRate = 16000;
static uint32_t s_wChannels = 1;

//Global variables
//Function implementation
static int GetWavInfo(const char *c_pszFileName, ST_WAV_INFO *pWavInfo)
{
    T_Fd iFd = 0; 
    int iRet = 0;
    uchar abyRbuff[64] = {0};
    ulong ulOffset = 0;

    iRet = YMI_FsExist(c_pszFileName);
    if(iRet != YMI_OK){
        WAVMIX_LOG("[ERR]wav file no exist:%d", iRet);
        return YMI_ERR;
    }
    iFd = YMI_FsOpen(c_pszFileName, "r");
    if(iFd <= 0){
        WAVMIX_LOG("[ERR]wav open fail:%d", iFd);
        return YMI_ERR_OPEN_DEV;
    }

    memset(pWavInfo, 0,sizeof(ST_WAV_INFO));

    // read RIFF
    iRet = YMI_FsRead(iFd, pWavInfo->stRiff.acChunkID, sizeof(pWavInfo->stRiff.acChunkID));
    if(iRet <= 0){
        WAVMIX_LOG("wav file stRiff read:%d", iRet);
        goto get_wav_info_fail;
    }
    
    iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint32_t));
    if(iRet <= 0){
        WAVMIX_LOG("wav file length read:%d", iRet);
        goto get_wav_info_fail;
    }
    pWavInfo->stRiff.wChunkSize = LITTLE_TO_U32(abyRbuff);

    iRet = YMI_FsRead(iFd, pWavInfo->stRiff.acFormat, sizeof(pWavInfo->stRiff.acFormat));
    if(iRet <= 0){
        WAVMIX_LOG("wav file acFormat read:%d", iRet);
        goto get_wav_info_fail;
    }

    if(memcmp(pWavInfo->stRiff.acFormat,"WAVE",4) && memcmp(pWavInfo->stRiff.acFormat,"wave",4))
    {
        WAVMIX_LOG("%s is not a valid wav file\n", c_pszFileName);
        iRet = -1;
        goto get_wav_info_fail;
    }
    
    // read FMT
    while (1)
    {
        memset(&pWavInfo->stFmt, 0, sizeof(ST_FMT));
        memset(abyRbuff, 0, sizeof(abyRbuff));
        iRet = YMI_FsRead(iFd, pWavInfo->stFmt.acSubchunk1ID, sizeof(pWavInfo->stFmt.acSubchunk1ID));
        if( iRet <= 0 )
        {
            WAVMIX_LOG("%s: read stFmt id fail\n",__func__);
            goto get_wav_info_fail;
        }
        iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint32_t));
        if( iRet <= 0 )
        {
            WAVMIX_LOG("%s: read stFmt len fail\n",__func__);
            goto get_wav_info_fail;
        }
        pWavInfo->stFmt.wSubchunk1Size = LITTLE_TO_U32(abyRbuff);

        if (memcmp(pWavInfo->stFmt.acSubchunk1ID, "stFmt ", 4) || memcmp(pWavInfo->stFmt.acSubchunk1ID, "FMT ", 4))
        {
            // found FMT info
            break;
        }
        // other info,show them
    }


    iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint16_t));
    if( iRet <= 0 )
    {
        WAVMIX_LOG("%s: read FMT acFormat fail\n",__func__);
        goto get_wav_info_fail;
    }
    pWavInfo->stFmt.wAudioFormat = LITTLE_TO_U16(abyRbuff);
    if( pWavInfo->stFmt.wAudioFormat != 1 )
    {
        // not a pcm acFormat
        WAVMIX_LOG("%s: wav acFormat is not pcm\n",__func__);
        goto get_wav_info_fail;
    }
    // read channel num
    iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint16_t));
    if( iRet <= 0 )
    {
        WAVMIX_LOG("%s: read FMT channel fail\n",__func__);
        goto get_wav_info_fail;
    }
    pWavInfo->stFmt.wNumChannels = LITTLE_TO_U16(abyRbuff);
    // read samplerate
    iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint32_t));
    if( iRet <= 0 )
    {
        WAVMIX_LOG("%s: read FMT samplerate fail\n",__func__);
        goto get_wav_info_fail;
    }
    pWavInfo->stFmt.wSampleRate = LITTLE_TO_U32(abyRbuff);
    // read byterate
    iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint32_t));
    if( iRet <= 0 )
    {
        WAVMIX_LOG("%s: read FMT byterate fail\n",__func__);
        goto get_wav_info_fail;
    }
    s_wSampleRate = pWavInfo->stFmt.wSampleRate;
    pWavInfo->stFmt.wByteRate = LITTLE_TO_U32(abyRbuff);
     // read blockalign
    iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint16_t));
    if( iRet <= 0 )
    {
        WAVMIX_LOG("%s: read FMT blockalign fail\n",__func__);
        goto get_wav_info_fail;
    }
    s_wChannels = pWavInfo->stFmt.wNumChannels;
    pWavInfo->stFmt.wBlockAlign = LITTLE_TO_U16(abyRbuff);
    // read bitspersample
    iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint16_t));
    if( iRet <= 0 )
    {
        WAVMIX_LOG("%s: read FMT bitspersample fail\n",__func__);
        goto get_wav_info_fail;
    }
    pWavInfo->stFmt.wBitsPerSample = LITTLE_TO_U16(abyRbuff);
    
    //normally,the FMT size is 16,but maybe include extract info,so skip the extra info
    if( pWavInfo->stFmt.wSubchunk1Size != 16 )
    {
        WAVMIX_LOG("%s: skip %ld byte in FMT\n",__func__,pWavInfo->stFmt.wSubchunk1Size - 16);
        YMI_FsSeek(iFd, pWavInfo->stFmt.wSubchunk1Size - 16, EM_FS_SEEK_CUR);
    }

    // read DATA info
    while(1)
    {
        memset(&pWavInfo->stData, 0, sizeof(ST_WAV_DATA));
        memset(abyRbuff, 0, sizeof(abyRbuff));
        iRet = YMI_FsRead(iFd, pWavInfo->stData.acSubchunk2ID,sizeof(pWavInfo->stData.acSubchunk2ID));
        if( iRet <= 0 )
        {
            WAVMIX_LOG("%s: read stData id fail\n",__func__);
            goto get_wav_info_fail;
        }

        YMI_FsTell(iFd, &ulOffset);
        iRet = YMI_FsRead(iFd, (char *)abyRbuff, sizeof(uint32_t));
        if( iRet <= 0 )
        {
            WAVMIX_LOG("%s: read stData len fail\n",__func__);
            goto get_wav_info_fail;
        }
        pWavInfo->stData.wSubchunk2Size = LITTLE_TO_U32(abyRbuff);

        if (memcmp(pWavInfo->stData.acSubchunk2ID, "stData", 4) || memcmp(pWavInfo->stData.acSubchunk2ID, "DATA", 4))
        {
            // found DATA info
            YMI_FsTell(iFd, &ulOffset);
            pWavInfo->stData.wDataOffset = ulOffset;
            break;
        }
        // other info,show them
    }

    iRet = 0;
get_wav_info_fail:
    YMI_FsClose(iFd);

    return iRet;
}


int WavMixList(int iCnt, void *pavFileArray[])
{
    WAVMIX_LOG("%s start", __func__);
    int iRet = YMI_ERR;
    int iIdex = 0;
    int iReadSize = 0;
    int iBlockSize = WAV_BLOCK_SIZE;
    char szFilePath[AUDIO_FILE_PATH_MAX] = {0};
    char *pcMixBuff = NULL;
    char *pcBuff = NULL;
    uint wMixBuffLen = 0;
    uint wMixBufSize = WAV_MIX_PLAY_BUFF_MAX_SIZE;
    uint wOffset = 0;
    ST_MIX_CTRL stMixc = {0};

    if ((pavFileArray == NULL) || (iCnt <= 0))
    {
        WAVMIX_LOG("%s param fail!", __func__);
        return YMI_ERR;
    }
    
    BUFCLR(szFilePath);
    snprintf(szFilePath, sizeof(szFilePath), "%s", pavFileArray[iIdex]);
    WAVMIX_LOG("get wav file path = %s", szFilePath);

    while (1)
    {
        s_pvWavPlayBuff = MALLOC(wMixBufSize, char);
        if (s_pvWavPlayBuff == NULL)
        {
            wMixBufSize = wMixBufSize * 2 / 3;
            if (wMixBufSize <= WAV_MIX_PLAY_BUFF_MIN_SIZE)
            {
                WAVMIX_LOG("[ERR]mixbuf malloc fail!");
                goto wav_mix_list_ex_end;
            }
            continue;
        }
        memset(s_pvWavPlayBuff, 0x00, wMixBufSize);
        wMixBufSize /= 3;
        break;
    }
    pcMixBuff = s_pvWavPlayBuff;
    s_wLenPlayBuff1 = 0;
    s_wLenPlayBuff2 = 0;
    s_pvWavPlayBuff1 = s_pvWavPlayBuff + wMixBufSize;
    s_pvWavPlayBuff2 = s_pvWavPlayBuff + 2 * wMixBufSize;
    WAVMIX_LOG("s_pvWavPlayBuff1 = %p, s_pvWavPlayBuff2 = %p, wMixBufSize = %d", s_pvWavPlayBuff1, s_pvWavPlayBuff2, wMixBufSize);

    memset((void *)&stMixc, 0x00, sizeof(ST_MIX_CTRL));
    iRet = GetWavInfo(szFilePath, &stMixc.stWavInfo);
    if (iRet != YMI_OK)
    {
        WAVMIX_LOG("[ERR]get wav info fail!");
        goto wav_mix_list_ex_end;
    }

    stMixc.iFp = YMI_FsOpen(szFilePath, "rb");
    if (stMixc.iFp <= 0)
    {
        WAVMIX_LOG("[ERR]%s open fail!%d,%x", szFilePath, stMixc.iFp, stMixc.iFp);
        goto wav_mix_list_ex_end;
    }
    
    YMI_FsSeek(stMixc.iFp, stMixc.stWavInfo.stData.wDataOffset, EM_FS_SEEK_STA);
    stMixc.iStartOffset = stMixc.stWavInfo.stData.wSubchunk2Size - stMixc.stWavInfo.stFmt.wByteRate * WAV_MIX_TIME_MS / 1000;
    if (stMixc.iStartOffset < 0)
    {
        stMixc.iStartOffset = 0;
    }

    s_eWavPlayStatus = WAV_PLAY_WAITING;
    while (1)
    {
        pcBuff = pcMixBuff + wMixBuffLen;
        iReadSize = YMI_FsRead(stMixc.iFp, pcBuff, wMixBufSize - wMixBuffLen);
        if (iReadSize <= 0)
        {// read end or error,switch to next
            YMI_FsClose(stMixc.iFp);
            memset((void *)&stMixc, 0x00, sizeof(ST_MIX_CTRL));

            iIdex ++;
            if (pavFileArray[iIdex] && strlen(pavFileArray[iIdex]))
            {
                BUFCLR(szFilePath);
                snprintf(szFilePath, sizeof(szFilePath), "%s", pavFileArray[iIdex]);
                WAVMIX_LOG("get wav file path = %s", szFilePath);

                iRet = GetWavInfo(szFilePath, &stMixc.stWavInfo);
                if (iRet != YMI_OK)
                {
                    WAVMIX_LOG("[ERR]get wav info fail!");
                    iRet = YMI_ERR;
                    goto wav_mix_list_ex_end;
                }

                stMixc.iFp = YMI_FsOpen(szFilePath, "rb");
                if (stMixc.iFp <= 0)
                {
                    WAVMIX_LOG("[ERR]%s open fail!%d,%x", szFilePath, stMixc.iFp, stMixc.iFp);
                    iRet = YMI_ERR;
                    goto wav_mix_list_ex_end;
                }
                
                YMI_FsSeek(stMixc.iFp, stMixc.stWavInfo.stData.wDataOffset, EM_FS_SEEK_STA);
                stMixc.iStartOffset = stMixc.stWavInfo.stData.wSubchunk2Size - stMixc.stWavInfo.stFmt.wByteRate * WAV_MIX_TIME_MS / 1000;
                if (stMixc.iStartOffset < 0)
                {
                    stMixc.iStartOffset = 0;
                }
            }
            else
            {
                WAVMIX_LOG("end of list");
                goto wav_mix_list_ex_done;
            }
            continue;
        }
        // optimize start
        wOffset = 0;
        if (stMixc.wRcount < stMixc.iStartOffset)
        {
            if ((stMixc.iStartOffset - stMixc.wRcount) >= iReadSize)
            {
                wOffset = iReadSize / iBlockSize - 1;
            }
            else
            {
                wOffset = (stMixc.iStartOffset - stMixc.wRcount) / iBlockSize - 1;
            }
        }
        stMixc.wRcount += wOffset * iBlockSize;
        wMixBuffLen += wOffset * iBlockSize;
        // optimize end

        for (int i = wOffset; i < iReadSize / iBlockSize; i++)
        {
            int iLen = iBlockSize * i;
            stMixc.iVoiceData = (pcBuff[iLen + 1] << 8) + pcBuff[iLen];
            stMixc.wRcount += iBlockSize;
            
            pcMixBuff[wMixBuffLen] = stMixc.iVoiceData;
            pcMixBuff[wMixBuffLen + 1] = stMixc.iVoiceData >> 8;
            wMixBuffLen += iBlockSize;

            if (wMixBuffLen >= wMixBufSize)
            {
                while (1)
                {
                    if (s_wLenPlayBuff1 == 0)
                    {
                        memset(s_pvWavPlayBuff1, 0x00, wMixBuffLen);
                        memcpy(s_pvWavPlayBuff1, pcMixBuff, wMixBuffLen);
                        s_wLenPlayBuff1 = wMixBuffLen;
                        WAVMIX_LOG("copy %ld bytes to buff1", wMixBuffLen);
                        break;
                    }
                    else if (s_wLenPlayBuff2 == 0)
                    {
                        memset(s_pvWavPlayBuff2, 0x00, wMixBuffLen);
                        memcpy(s_pvWavPlayBuff2, pcMixBuff, wMixBuffLen);
                        s_wLenPlayBuff2 = wMixBuffLen;
                        WAVMIX_LOG("copy %ld bytes to buff2", wMixBuffLen);
                        break;
                    }
                    SLEEP_MS(10);
                }
                wMixBuffLen = 0;
            }
        }
    }
    
wav_mix_list_ex_done:
    if (wMixBuffLen)
    {
        while (1)
        {
            if (s_wLenPlayBuff1 == 0)
            {
                memset(s_pvWavPlayBuff1, 0x00, wMixBuffLen);
                memcpy(s_pvWavPlayBuff1, pcMixBuff, wMixBuffLen);
                s_wLenPlayBuff1 = wMixBuffLen;
                WAVMIX_LOG("copy %ld bytes to buff1", wMixBuffLen);
                break;
            }
            else if (s_wLenPlayBuff2 == 0)
            {
                memset(s_pvWavPlayBuff2, 0x00, wMixBuffLen);
                memcpy(s_pvWavPlayBuff2, pcMixBuff, wMixBuffLen);
                s_wLenPlayBuff2 = wMixBuffLen;
                WAVMIX_LOG("copy %ld bytes to buff2", wMixBuffLen);
                break;
            }
            SLEEP_MS(10);
        }
        wMixBuffLen = 0;
    }

    // wait play finish
    uint wTimerWaitFinish = 0;
    uint wTimer = 0;
    YMI_SysReadTimer(&wTimer);
    wTimerWaitFinish = wTimer + WAV_MIX_LIST_WAITE_PLAY_FINISH_S;
    while (s_eWavPlayStatus != WAV_PLAY_STOP)
    {
        if (!s_wLenPlayBuff1 && !s_wLenPlayBuff2 && (s_eWavPlayStatus == WAV_PLAY_WAITING))
        {
            s_eWavPlayStatus = WAV_PLAY_STOP;
        }
        YMI_SysReadTimer(&wTimer);
        if (wTimer > wTimerWaitFinish)
        {
            WAVMIX_LOG("wait play finsh timout!");
            break;
        }
        SLEEP_MS(100);
    }
    
wav_mix_list_ex_end:
    if (stMixc.iFp)
    {
        YMI_FsClose(stMixc.iFp);
    }
    if (s_pvWavPlayBuff)
    {
        YMI_SysFree(s_pvWavPlayBuff);
        s_pvWavPlayBuff = NULL;
        s_pvWavPlayBuff1 = NULL;
        s_pvWavPlayBuff2 = NULL;
        s_wLenPlayBuff1 = 0;
        s_wLenPlayBuff2 = 0;
    }
    WAVMIX_LOG("%s end", __func__);
    return YMI_OK;
}

void WavPcmPlayTask(void *pvParam)
{
    WAVMIX_LOG("%s start", __func__);
    // int g_nPcmPlaying = 0;
    ST_Event stEvent = {0};
    
    while (1)
    {
        if (s_eWavPlayStatus != WAV_PLAY_STOP)
        {
            if (!g_nPcmPlaying)
            {
                LOG("%s start pcm play!", __func__);
                g_nPcmPlaying = 1;
                YMI_AudioPCMComfig(s_wSampleRate, s_wChannels);
                YMI_AudioPCMDataStart();
            }

            if (s_wLenPlayBuff1 || s_wLenPlayBuff2)
            {
                s_eWavPlayStatus = WAV_PLAY_PLAYING;

                if (s_wLenPlayBuff1 && s_pvWavPlayBuff1)
                {
                    WAVMIX_LOG("s_pvWavPlayBuff1 = %p, len = %d", s_pvWavPlayBuff1, s_wLenPlayBuff1);
                    YMI_AudioPCMPlay(s_pvWavPlayBuff1, s_wLenPlayBuff1);
                    s_wLenPlayBuff1 = 0;
                }
                if (s_wLenPlayBuff2 && s_pvWavPlayBuff2)
                {
                    WAVMIX_LOG("s_pvWavPlayBuff2 = %p, len = %d", s_pvWavPlayBuff2, s_wLenPlayBuff2);
                    YMI_AudioPCMPlay(s_pvWavPlayBuff2, s_wLenPlayBuff2);
                    s_wLenPlayBuff2 = 0;
                }
            }
            else
            {
                s_eWavPlayStatus = WAV_PLAY_WAITING;
                SLEEP_MS(10);
            }
        }
        else
        {
            if (g_nPcmPlaying)
            {
                // WaitPlayEnd();
                g_nPcmPlaying = 0;
                YMI_AudioPCMDataStop();
            }
            
            if(YMI_EventWait(&stEvent, 100) == YMI_OK)
            {
                if(stEvent.wId == TASK_MANAGE_EVENT_ID && stEvent.wParam1 == TASK_OPT_EXIT)
                {
                    break;
                }
            }
        }
    }
    
    YMI_TaskDelete(s_tWavMixTaskId);
}

int WavPcmPlayTaskInit(void)
{
    int iRet = YMI_ERR;
    s_eWavPlayStatus = WAV_PLAY_STOP;
    iRet = YMI_TaskCreat(&s_tWavMixTaskId, "wav_pcm_play_task", WAV_MIX_TASK_STACK_SIZE, WavPcmPlayTask, NULL, EM_TASK_PRIORITY_BELOW_HIGH, WAV_MIX_TASK_EVENT_NUM);
    if (iRet != YMI_OK)
    {
        WAVMIX_LOG("[ERR]wav_mix_task create fail!");
        return iRet;
    }
    
    return iRet;
}

int WavMixPlayListStart(void *pvPlayList)
{
    ST_PLAY_LIST_INFO *pstPlayList = (ST_PLAY_LIST_INFO *)pvPlayList;
    if (pvPlayList == NULL)
    {
        WAVMIX_LOG("[ERR]%s param fail!");
        return YMI_ERR;
    }
    
    int iRet = WavMixList(pstPlayList->iTotalCnt, (void *)pstPlayList->pcFileNameAddr);
    if (iRet != YMI_OK)
    {
        WAVMIX_LOG("[ERR]wav mix list parse fail!");
        return YMI_ERR;
    }

    return YMI_OK;
}

void *WavMixPlayListCreate(void)
{
    ST_PLAY_LIST_INFO *pstPlayList = MALLOC(1, ST_PLAY_LIST_INFO);
    if (pstPlayList == NULL)
    {
        WAVMIX_LOG("[ERR]playlist malloc fail!");
    }
    memset(pstPlayList, 0x00, sizeof(ST_PLAY_LIST_INFO));
    return (void *)pstPlayList;
}

void WavMixPlayListDel(void *pvPlayList)
{
    FREE(pvPlayList);
}

int WavMixPlayListAdd(void *pvPlayList, char *pcFilePath)
{
    ST_PLAY_LIST_INFO *pstPlayList = (ST_PLAY_LIST_INFO *)pvPlayList;
    int iDex = pstPlayList->iTotalCnt;
    int iLen = 0;

    if ((pvPlayList == NULL) || (pcFilePath == NULL))
    {
        WAVMIX_LOG("%s param fail!");
        return YMI_ERR;
    }

    if (YMI_FsExist(pcFilePath) != YMI_OK)
    {
        WAVMIX_LOG("[ERR]%s no exist!");
        return YMI_ERR;
    }
    
    if (iDex == 0)
    {
        pstPlayList->pcFileNameAddr[iDex] = pstPlayList->pcPlayList;
    }
    else
    {
        if (iDex >= (sizeof(pstPlayList->pcFileNameAddr)/sizeof(pstPlayList->pcFileNameAddr[0])))
        {
            WAVMIX_LOG("[ERR]playlist is full");
            return YMI_ERR;
        }
        pstPlayList->pcFileNameAddr[iDex] = pstPlayList->pcFileNameAddr[iDex-1] + strlen(pstPlayList->pcFileNameAddr[iDex-1]) + 1;
    }

    iLen = sizeof(pstPlayList->pcPlayList) - (pstPlayList->pcFileNameAddr[iDex] - pstPlayList->pcPlayList);
    if (iLen < (strlen(pcFilePath) + 1))
    {
        WAVMIX_LOG("[ERR]Insufficient remaining space");
        return YMI_ERR;
    }
    memcpy(pstPlayList->pcFileNameAddr[iDex], pcFilePath, strlen(pcFilePath));
    pstPlayList->iTotalCnt ++;
    
    return YMI_OK;
}


