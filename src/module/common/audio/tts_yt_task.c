/**********************************************************************
* Copyright (C)    Dspread
* File name: tts_yt_task.c
* Abstract: DS200 device TTS-related APIs
*
***********************************************************************/
//Header file
#include "var.h"
#include "tts_yt_task.h"

//Macro definitions
#define TTS_LOG_MODE
#ifdef TTS_LOG_MODE
#define TTS_LOG(msg, ...) LOG("[TTS]"msg"", ##__VA_ARGS__)
#else
#define TTS_LOG(msg, ...)
#endif
//Constant

//Local variables
//Global variables

//Function implementation
extern int AudioSend(void *pszVoceMsg, EM_AUDIO_PRIORITY ePriority, EM_AUDIO_MSG_TYPE eAudioType, uint wAmount, uint iTimeStamp, CB_ON_PLAYER cb_PlayOverFunc);
extern void AudioPlayStop(void);
extern void WaitPlayEnd(void);

AudioResInfo s_stRes[] = {
	{AUD_ID_NUM_0,								STR_ID_NUM_0,								NULL,							"res/wav/0.wav" 	},
	{AUD_ID_NUM_1,								STR_ID_NUM_1,								NULL,							"res/wav/1.wav" 	},
	{AUD_ID_NUM_2,								STR_ID_NUM_2,								NULL,							"res/wav/2.wav" 	},
	{AUD_ID_NUM_3,								STR_ID_NUM_3,								NULL,							"res/wav/3.wav" 	},
	{AUD_ID_NUM_4,								STR_ID_NUM_4,								NULL,							"res/wav/4.wav" 	},
	{AUD_ID_NUM_5,								STR_ID_NUM_5,								NULL,							"res/wav/5.wav" 	},
	{AUD_ID_NUM_6,								STR_ID_NUM_6,								NULL,							"res/wav/6.wav" 	},
	{AUD_ID_NUM_7,								STR_ID_NUM_7,								NULL,							"res/wav/7.wav" 	},
	{AUD_ID_NUM_8,								STR_ID_NUM_8,								NULL,							"res/wav/8.wav" 	},
	{AUD_ID_NUM_9,								STR_ID_NUM_9,								NULL,							"res/wav/9.wav" 	},
	{AUD_ID_NUM_10, 							STR_ID_NUM_10,								NULL,							"res/wav/ten.wav"		},
	{AUD_ID_NUM_11, 							STR_ID_NUM_11,								NULL,							"res/wav/eleven.wav" 	},
	{AUD_ID_NUM_12, 							STR_ID_NUM_12,								NULL,							"res/wav/twelve.wav" 	},
	{AUD_ID_NUM_13, 							STR_ID_NUM_13,								NULL,							"res/wav/thirteen.wav"		},
	{AUD_ID_NUM_14, 							STR_ID_NUM_14,								NULL,							"res/wav/fourteen.wav" 	},
	{AUD_ID_NUM_15, 							STR_ID_NUM_15,								NULL,							"res/wav/fifteen.wav" 	},
	{AUD_ID_NUM_16,								STR_ID_NUM_16,								NULL,							"res/wav/sixteen.wav"		},
	{AUD_ID_NUM_17, 							STR_ID_NUM_17,								NULL,							"res/wav/seventeen.wav" 	},
	{AUD_ID_NUM_18,				 				STR_ID_NUM_18,								NULL,							"res/wav/eighteen.wav" 	},
	{AUD_ID_NUM_19, 							STR_ID_NUM_19,								NULL,							"res/wav/nineteen.wav"		},
	
	{AUD_ID_NUM_20, 							STR_ID_NUM_20,								NULL,							"res/wav/twenty.wav" 	},
	{AUD_ID_NUM_30, 							STR_ID_NUM_30,								NULL,							"res/wav/thirty.wav"		},
	{AUD_ID_NUM_40, 							STR_ID_NUM_40,								NULL,							"res/wav/forty.wav" 	},
	{AUD_ID_NUM_50, 							STR_ID_NUM_50,								NULL,							"res/wav/fifty.wav" 	},
	{AUD_ID_NUM_60, 							STR_ID_NUM_60,								NULL,							"res/wav/sixty.wav"		},
	{AUD_ID_NUM_70, 							STR_ID_NUM_70,								NULL,							"res/wav/seventy.wav" 	},
	{AUD_ID_NUM_80, 							STR_ID_NUM_80,								NULL,							"res/wav/eighty.wav" 	},
	{AUD_ID_NUM_90, 							STR_ID_NUM_90,								NULL,							"res/wav/ninety.wav"		},
	{AUD_ID_HUNDRED,							STR_ID_HUNDRED, 							NULL,							"res/wav/hundred.wav"		},
	{AUD_ID_THOUSAND,							STR_ID_THOUSAND,							NULL,							"res/wav/thousand.wav"		},
	//{AUD_ID_DOT,								STR_ID_DOT,									NULL,							"res/wav/dot.wav"		},
	{AUD_ID_DOT,								STR_ID_VOL_UP,								NULL,							"res/anjianyin.mp3"		},


	{AUD_ID_MQTT_CONNECT_SUCESS,				STR_ID_MQTT_CONNECT_SUCESS,					NULL,							"res/MqttConnectSucess.mp3"		},
	{AUD_ID_MQTT_CONNECT_FAIL,					STR_ID_MQTT_CONNECT_FAIL,					NULL,							"res/MqttConnectFail.mp3"		},


#if BT_DEMO
	{AUD_ID_BT_CON, 							STR_ID_BT_CON,								NULL,							"res/BTcon.mp3"		},
	{AUD_ID_BT_DISCON,							STR_ID_BT_DISCON,							NULL,							"res/BTdiscon.mp3"		},
	{AUD_ID_BT_OPEN,							STR_ID_BT_OPEN, 							NULL,							"res/BTopen.mp3"		},
	{AUD_ID_BT_CLOSE,							STR_ID_BT_CLOSE,							NULL,							"res/BTclose.mp3" 	},
	{AUD_ID_BT_OTA, 							STR_ID_BT_OTA,								NULL,							"res/BTOTA.mp3" 	},
#endif

};

void AuidoVolumeSet(EM_VOL_LEVEL eVol)
{
	YMI_AudioSetVolume(eVol);
}

void WaitAudioPlayOver(void)
{
	WaitPlayEnd();
}

int ResGetExistLocalFilePath(char const *c_pszContent, void *pszPath, int iPathMaxLen)
{
	int iDex;

	for(iDex=0;iDex<sizeof(s_stRes)/sizeof(s_stRes[0]);iDex++)
	{
		if ((strlen(c_pszContent) == strlen(s_stRes[iDex].TTSstr)) && (!memcmp(c_pszContent, s_stRes[iDex].TTSstr, strlen(s_stRes[iDex].TTSstr))))
			break;
	}

	if (iDex >= sizeof(s_stRes)/sizeof(s_stRes[0]))
	{
		TTS_LOG("[ERR]no fond!");
		return RET_FAIL;
	}
	
	if (YMI_FsExist(s_stRes[iDex].filePinyinName) != YMI_OK)
	{
		memset(pszPath, 0x00, iPathMaxLen);
		TTS_LOG("[ERR]%s file no exit!", s_stRes[iDex].filePinyinName);
		return RET_FAIL;
	}

	memset(pszPath, 0x00, iPathMaxLen);
    snprintf(pszPath, iPathMaxLen, "%s", s_stRes[iDex].filePinyinName);
	
	memcpy(pszPath, s_stRes[iDex].filePinyinName, strlen(s_stRes[iDex].filePinyinName));

	return RET_SUCC;
}

int ResCheckWavFile(char const *c_pszContent, void *pszPath, int iPathMaxLen)
{
    if ((c_pszContent == NULL) || (pszPath == NULL) || (!iPathMaxLen))
    {
        TTS_LOG("%s param fail!");
        return RET_FAIL;
    }

    int iDex = 0;
    for (iDex = 0; iDex < sizeof(s_stRes)/sizeof(s_stRes[0]); iDex++)
    {
        if ((strlen(c_pszContent) == strlen(s_stRes[iDex].TTSstr)) && (!memcmp(c_pszContent, s_stRes[iDex].TTSstr, strlen(s_stRes[iDex].TTSstr))))
        {
            break;
        }
    }
    
    if (iDex >= sizeof(s_stRes)/sizeof(s_stRes[0]))
    {
        TTS_LOG("[ERR]no fond same str");
        return RET_FAIL;
    }
    
    memset(pszPath, 0x00, iPathMaxLen);
    snprintf(pszPath, iPathMaxLen, "%s", s_stRes[iDex].filePinyinName);
    char *pcFormat = strstr(pszPath, ".mp3");
    if (pcFormat)
    {
        pcFormat[1] = 'w';
        pcFormat[2] = 'a';
        pcFormat[3] = 'v';
        pcFormat[4] = 0;
    }
    
    if (YMI_FsExist(pszPath) != RET_SUCC)
    {
        TTS_LOG("[ERR]%s file no exist!", pszPath);
        memset(pszPath, 0x00, iPathMaxLen);
        return RET_FAIL;
    }
    
    TTS_LOG("%s file exist!", pszPath);
    return RET_SUCC;
}

AudioResInfo *GetResInfo(int Idx)
{
	int iDex;
	for(iDex=0;iDex<sizeof(s_stRes)/sizeof(s_stRes[0]);iDex++)
	{
		if (Idx==s_stRes[iDex].Idx)
			return &s_stRes[iDex];
	}

	TTS_LOG("Audio Idx=%d not found!",Idx);
	return NULL;
}

int tts_play_set_idx(int idx, int BreakEna,int playnow )
{
	EM_AUDIO_PRIORITY ePriority = AUDIO_PRIORITY_LOWEST;
	AudioResInfo *pstResInfo = NULL;
	
	pstResInfo = GetResInfo(idx);
	if(pstResInfo == NULL)
	{
		return RET_FAIL;
	}
	if(playnow)
	{
		ePriority = AUDIO_PRIORITY_ABOVE;
	}

	AudioPlay(ePriority, FILE_TEXT, 0, 0, NULL, 1, pstResInfo->TTSstr);
	return RET_SUCC;
}

int tts_play_set_group(short const * Group,int size,int BreakEna,int playnow )
{

	EM_AUDIO_PRIORITY ePriority = AUDIO_PRIORITY_LOWEST;
	AudioResInfo **pastResInfo = NULL;
	char *pszBuff = NULL;
	int i, iSize = 0;
	if(size == 0 || Group == NULL)
	{
		return RET_FAIL;
	}

	pastResInfo = MALLOC(size, AudioResInfo *);
	for(i = 0; i < size; i ++)
	{
		pastResInfo[i] = GetResInfo(Group[i]);
		if(pastResInfo[i] == NULL)
		{
			FREE(pastResInfo);
			return RET_FAIL;
		}
		iSize += strlen(pastResInfo[i]->TTSstr) + 1;
	}
	pszBuff = MALLOC(iSize, char);
	if(pszBuff == NULL)
	{
		FREE(pastResInfo);
		return RET_FAIL;
	}
	if(playnow)
	{
		ePriority = AUDIO_PRIORITY_ABOVE;
	}
	for(i = 0; i < size; i ++)
	{
		sprintf(pszBuff + strlen(pszBuff), "%s|", pastResInfo[i]->TTSstr);
		pastResInfo ++;
	}
	AudioSend(pszBuff, ePriority, FILE_TEXT, 0, 0, NULL);
	FREE(pszBuff);
	FREE(pastResInfo);

	return RET_SUCC;
}

int tts_play_set_sale_id(const char * msgid,const char * url,int SalePlayId,uint64_t Amount,uint32_t timestamp)
{

	return 0;
}

int tts_play_set_sale_str(const char * msgid,const char * url,char const * salemsg,uint64_t Amount,uint32_t timestamp)
{

	return 0;
}

int tts_play_set_user_def(char const * usermsg,int BreakEna,int playnow )
{
	return 0;
}