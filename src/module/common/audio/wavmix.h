#ifndef _WAVMIX_H_
#define _WAVMIX_H_

#define PLAYLIST_ITEM_CNT           50
#define PLAYLIST_WAV_NAME_LEN       (2 * 1024)
#define WAV_MIX_TIME_MS             90
#define WAV_BLOCK_SIZE              2   /* default to 16bit,2byte */

typedef enum{
    WAV_PLAY_STOP,
    WAV_PLAY_WAITING,
    WAV_PLAY_PLAYING,
    WAV_PLAY_FINISH,
}EN_WAV_PLAY_STATUS;

typedef struct {
	int iTotalCnt;
	char *pcFileNameAddr[PLAYLIST_ITEM_CNT];
	char pcPlayList[PLAYLIST_WAV_NAME_LEN];
}ST_PLAY_LIST_INFO;

typedef struct
{
    /* chunk "riff" */
    char acChunkID[4]; /* "RIFF" */
    /* sub-chunk-size */
    uint32_t wChunkSize; /* 36 + Subchunk2Size */
    /* sub-chunk-data */
    char acFormat[4]; /* "WAVE" */
}ST_RIFF;

typedef struct
{
    /* sub-chunk "fmt" */
    char acSubchunk1ID[4]; /* "fmt " */
    /* sub-chunk-size */
    uint32_t wSubchunk1Size; /* 16 for PCM */
    /* sub-chunk-data */
    uint16_t wAudioFormat; /* PCM = 1*/
    uint16_t wNumChannels; /* Mono = 1, Stereo = 2, etc. */
    uint32_t wSampleRate; /* 8000, 44100, etc. */
    uint32_t wByteRate; /* = SampleRate * NumChannels * BitsPerSample/8 */
    uint16_t wBlockAlign; /* = NumChannels * BitsPerSample/8 */
    uint16_t wBitsPerSample; /* 8bits, 16bits, etc. */
}ST_FMT;

typedef struct
{
    /* sub-chunk "data" */
    char acSubchunk2ID[4]; /* "data" */
    /* sub-chunk-size */
    uint32_t wSubchunk2Size; /* data size */
    /* sub-chunk-data */
    //    Data_block_t block;
    ulong wDataOffset;
}ST_WAV_DATA;

typedef struct WAV_fotmat
{
    ST_RIFF stRiff;
    ST_FMT stFmt;
    ST_WAV_DATA stData;
}ST_WAV_INFO;

typedef struct
{
    int iVoiceData;
    int iStartOffset;
    T_Fd iFp;
    uint wRcount;
    ST_WAV_INFO stWavInfo;
}ST_MIX_CTRL;

extern int g_nPcmPlaying;
int WavPcmPlayTaskInit(void);
int WavMixPlayListStart(void *pvPlayList);
void *WavMixPlayListCreate(void);
void WavMixPlayListDel(void *pvPlayList);
int WavMixPlayListAdd(void *pvPlayList, char *pcFilePath);


#endif