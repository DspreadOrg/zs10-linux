#ifndef _YT_TTS_TASK_H_
#define _YT_TTS_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	char const * Data;
	unsigned int dlen;
}AudioArrayInfo;

typedef struct {
	const unsigned int Idx;
	const char * TTSstr;
	const AudioArrayInfo * Array;
	const char *filePinyinName;
}AudioResInfo;

typedef enum
{
	AUD_ID_GROUP_IDX,
	AUD_ID_SALE_ID_INFO,
	AUD_ID_SALE_STR_INFO,
	AUD_ID_USER_DEF,
	AUD_ID_RECODE_PLAY,

	AUD_ID_FIX_AUDIO_BASE=2000,
	AUD_ID_FAC_TEST_MODE,
	AUD_ID_FAC_TEST_COMPLETE,
	AUD_ID_FAC_TEST_AUDIO,
	AUD_ID_FAC_TEST_PWROFF,
	AUD_ID_FAC_TEST_GET_VESION_FAIL,
	AUD_ID_FAC_TEST_SIM_NOT_FOUND,
	AUD_ID_FAC_TEST_GET_LEV_FAIL,
	AUD_ID_CUR_SIGNAL,

	AUD_ID_SALE_CANCLE,
	AUD_ID_NUM_0,
	AUD_ID_NUM_1,
	AUD_ID_NUM_2,
	AUD_ID_NUM_3,
	AUD_ID_NUM_4,
	AUD_ID_NUM_5,
	AUD_ID_NUM_6,
	AUD_ID_NUM_7,
	AUD_ID_NUM_8,
	AUD_ID_NUM_9,
	AUD_ID_NUM_10,
	AUD_ID_NUM_11,
	AUD_ID_NUM_12,
	AUD_ID_NUM_13,
	AUD_ID_NUM_14,
	AUD_ID_NUM_15,
	AUD_ID_NUM_16,
	AUD_ID_NUM_17,
	AUD_ID_NUM_18,
	AUD_ID_NUM_19,
	AUD_ID_NUM_20,
	AUD_ID_NUM_30,
	AUD_ID_NUM_40,
	AUD_ID_NUM_50,
	AUD_ID_NUM_60,
	AUD_ID_NUM_70,
	AUD_ID_NUM_80,
	AUD_ID_NUM_90,
	AUD_ID_HUNDRED,
	AUD_ID_THOUSAND,
	AUD_ID_MILION,
	AUD_ID_DOT,
	AUD_ID_UNIT,
	AUD_ID_PAYMENT,



	AUD_ID_WELCOME,
	AUD_ID_KEY_BEEP,
	AUD_ID_KEY_ERR_BEEP,
	AUD_ID_LOG_MODE_USB,
	AUD_ID_OR,
	AUD_ID_PLEASE_WAITING,
	AUD_ID_SIM_NOT_FOUND,
	AUD_ID_SIM_INSERT_REQ,
	AUD_ID_NET_FAULT,
	AUD_ID_NET_CONNECTING,
	AUD_ID_NET_CONNECT_FAIL,
	AUD_ID_NET_CONNECT_SUCESS,
	AUD_ID_NET_NOT_CONNECT,
	AUD_ID_NET_CONNECT,
	AUD_ID_NET_MODE_GPRS,
	AUD_ID_NET_MODE_WIFI,
	AUD_ID_NET_MODE_WIFI_AIRKISS,
	AUD_ID_NET_MODE_WIFI_AP,
	AUD_ID_MQTT_CONNECT_SUCESS,
	AUD_ID_MQTT_CONNECT_FAIL,
	AUD_ID_HOW_GPRS_TO_WIFI,
	AUD_ID_HOW_WIFI_TO_GPRS,
	AUD_ID_HOW_WIFI_TO_AP,
	AUD_ID_HOW_WIFI_TO_AIRKISS,
	AUD_ID_EXIT_WIFI_SET,
	AUD_ID_PWR_LOWBAT,
	AUD_ID_PWR_OFF,
	AUD_ID_PWR_LOWBATOFF,
	AUD_ID_PWR_PER,
	AUD_ID_VOL_MAX,
	AUD_ID_VOL_MIN,
	AUD_ID_EXIT_RECODE,
	AUD_ID_RECODE_NOT_FOUND,
	AUD_ID_RECODE_FIRST,
	AUD_ID_RECODE_LAST,
	AUD_ID_RECOVER_FACTORY,
	AUD_ID_PARAM_EMPTY,
	AUD_ID_ACTIVE_FAIL,
	AUD_ID_ACTIVE_SUCESS,
	AUD_ID_GET_SIGNAL_FAIL,
	AUD_ID_CHARGE_IN,
	AUD_ID_CHARGE_OUT,
	AUD_ID_CHARGE_FULL,
	AUD_ID_UPDATE_START,
	AUD_ID_UPDATE_SUCESS,
	AUD_ID_UPDATE_FAIL,

	AUD_ID_GET_GPS_SUCESS,
#if BT_DEMO
	AUD_ID_ON_MUSIC,
	AUD_ID_BT_CON,
	AUD_ID_BT_DISCON,
	AUD_ID_BT_OPEN,
	AUD_ID_BT_CLOSE,
	AUD_ID_BT_OTA,
#endif

	AUD_ID_MAX
}AudioPlayIdxDef;


/*************The comment at the beginning of STR is a Chinese interpretation and has no practical effect****************/
#define STR_ID_NUM_0					"zero"
#define STR_ID_NUM_1					"one"	
#define STR_ID_NUM_2					"two"	
#define STR_ID_NUM_3					"three"
#define STR_ID_NUM_4					"four"
#define STR_ID_NUM_5					"five"
#define STR_ID_NUM_6					"six"
#define STR_ID_NUM_7					"seven"
#define STR_ID_NUM_8					"eight"
#define STR_ID_NUM_9					"nine"
#define STR_ID_NUM_10					"ten"
#define STR_ID_NUM_11					"eleven"
#define STR_ID_NUM_12					"twelve"
#define STR_ID_NUM_13					"thirteen"
#define STR_ID_NUM_14					"fourteen"
#define STR_ID_NUM_15					"fifteen"
#define STR_ID_NUM_16					"sixteen"
#define STR_ID_NUM_17					"seventeen"
#define STR_ID_NUM_18					"eighteen"
#define STR_ID_NUM_19					"nineteen"
#define STR_ID_NUM_20					"twenty"
#define STR_ID_NUM_30					"thirty"
#define STR_ID_NUM_40					"forty"
#define STR_ID_NUM_50					"fifty"
#define STR_ID_NUM_60					"sixty"
#define STR_ID_NUM_70					"seventy"
#define STR_ID_NUM_80					"eighty"
#define STR_ID_NUM_90					"ninety"
#define STR_ID_HUNDRED						"hundred"
#define STR_ID_THOUSAND						"thousand"
#define STR_ID_DOT						"dot"
#define STR_ID_VOL_UP						"volume up"

#define STR_ID_MQTT_CONNECT_SUCESS		"Service connection successful"
#define STR_ID_MQTT_CONNECT_FAIL			"Service connection failed"


/*************The comment at the beginning of STR is a Chinese interpretation and has no practical effect****************/



extern void AuidoVolumeSet(EM_VOL_LEVEL eVol);
extern void tts_play_init(void);
extern int tts_play_set_idx(int idx, int BrkEna,int playnow);
extern int tts_play_set_group(short const * Group,int size,int BreakEna,int playnow );
extern int tts_play_set_sale_id(const char * msgid,const char * url,int SalePlayId,uint64_t Amount,uint32_t timestamp);
extern int tts_play_set_sale_str(const char * msgid,const char * url,char const * salemsg,uint64_t Amount,uint32_t timestamp);
extern int tts_play_set_user_def(char const * usermsg,int BreakEna,int playnow );
//extern int tts_play_text(char* text_str, tts_text_type type);
//void tts_play_set(const char * text_str,int len, tts_text_type type);
//extern int tts_play_set(const char* buf,int len, tts_text_type type);
//extern int tts_play_immediately(const char* buf,int len, tts_text_type type);
extern int Module_TTS_Play_Factory_Text(char * tts_buf,void (*Switch)(uint8_t OnOff));
/***********************************************
WaitAudioPlayOver，
If the call is made after sending a voice message, please delay the call for more than 500 ms
Make sure that the sent voice message has entered the playback state
When the voice playback ends, it may take about a second to exit.
***********************************************/
extern void WaitAudioPlayOver(void);
extern void AudioPlayHalt(void);
extern void AudioPlayContinue(void);
extern int ResGetExistLocalFilePath(char const *c_pszContent, void *pszPath, int iPathMaxLen);
extern int ResCheckWavFile(char const *c_pszContent, void *pszPath, int iPathMaxLen);

#ifdef __cplusplus
}
#endif

#endif /* _AD_BUTTON_TASK_H_ */