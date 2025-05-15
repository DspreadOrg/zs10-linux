#ifndef _AUDIO_H_
#define _AUDIO_H_

#define LITTLE_TO_U32(P)    ((P[3] << 24) + (P[2] << 16) + (P[1] << 8) + P[0])
#define LITTLE_TO_U16(P)    ((P[1] << 8) + P[0])
#define U32_TO_LITTLE(P,B)  {B[0] = P&0xff; B[1] = (P >> 8) & 0xff; B[2] = (P >> 16) & 0xff; B[3] = (P >> 24) & 0xff;}
#define U16_TO_LITTLE(P,B)  {B[0] = P&0xff; B[1] = (P >> 8) & 0xff; }

#define AUDIO_FILE_PATH_MAX (128)

typedef enum
{
    AUDIO_PRIORITY_NONE     = (0x00),
    AUDIO_PRIORITY_HIGHEST  = (0x01 << 0),
	AUDIO_PRIORITY_ABOVE    = (0x01 << 1),
    AUDIO_PRIORITY_MIDDLE   = (0x01 << 2),
    AUDIO_PRIORITY_LOWEST   = (0x01 << 3),
}EM_AUDIO_PRIORITY;

typedef enum 
{
    FILE_TEXT,
    FILE_PATH,
    AMT_TEXT,
    PCM_BUF,
}EM_AUDIO_MSG_TYPE;

typedef struct 
{
    char *pszVoiceMsg;
    ulong wVoiceMsgLen;
    EM_AUDIO_MSG_TYPE eAudioType;
    EM_AUDIO_PRIORITY ePriority;
    uint64_t wAmount;
    uint32_t iTimeStamp;
    // int nSampleRate;
    // int nChannels;
    CB_ON_PLAYER cb_PlayOverFunc;
}ST_AudioInfo;

typedef struct 
{
    char *pszVoiceMsg;
    ulong wVoiceMsgLen;
}ST_AudioPlayingInfo;

void WaitPlayEnd(void);
int AudioTaskInit(void);
void AudioPlay(EM_AUDIO_PRIORITY ePriority, EM_AUDIO_MSG_TYPE eAudioType, uint wAmount, uint iTimeStamp, CB_ON_PLAYER cb_PlayOverFunc, int iCnt, ...);
int AudioPlayOverCB(char *pData, int iLen, EM_AUD_PLAYER_STATE eState);
void AudioPriorityQueueClean(EM_AUDIO_PRIORITY ePriority);

#endif
