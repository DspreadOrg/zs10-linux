#ifndef VAR_H
#define VAR_H
#include "YMAPI.h"
#include <errno.h>
#include <time.h>
#include "audio.h"
#include "utf8str.h"
#include "key.h"
#include "dev.h"
#include "ota.h"
#include "hid.h"
#include "ui_api.h"
#include "ui_lvgl.h"
#include "mbedtls_aes_rsa.h"
#include "tts_yt_task.h"
#include "system.h"
#include "tools.h"
#include "net.h"
#include "mqtt.h"
#include "cJSON.h"

#define RET_SUCC 		(0)
#define RET_FAIL 		(-1)
#define RET_QUIT        (-2)
#define RET_TIMEOUT 	(-3)
#define RET_OTHER	    (-5)    
#define RET_EXIT_APP	(-10)    
#define WAIT_FOREVER    (0xFFFFFFFFUL)
#define WAIT_NO_TIME    (0)

#define TRUE    (1)
#define FALSE   (0)

#define FILE_NAME(x) strrchr(x,'/')?strrchr(x,'/')+1:x

#define DEBUG
#ifdef DEBUG
#define DEBUG_LOGCAT
#ifdef DEBUG_LOGCAT
#define LOG(fmt, args...) YMI_Log("[%s][%d]>>>" fmt "\r\n", FILE_NAME(__FILE__), __LINE__, ##args);
#define LOG_ERR(fmt, args...) YMI_Log("<ERR>[%s][%d]>>>" fmt "\r\n", FILE_NAME(__FILE__), __LINE__, ##args);
#define LOG_HEX(info, data, len) do{\
			int i = 0;\
			YMI_Log("[%s][%d]%s len=%d>>>\r\n", FILE_NAME(__FILE__), __LINE__, info, len);\
			while (i < len)\
			{\
				YMI_Log("%02X ", (unsigned char)*(data + i));\
				if(i == 15 || ((i - 15) % 16 == 0 && i > 15))\
				{\
					YMI_Log("\r\n");\
				}\
				i ++;\
			}\
			YMI_Log("\r\n");\
		}while(0)

#else
#define LOG(fmt, args...) printf("[%s][%d]>>>" fmt "\r\n", FILE_NAME(__FILE__), __LINE__, ##args)
#define LOG_ERR(fmt, args...) printf("<ERR>[%s][%d]>>>" fmt "\r\n", FILE_NAME(__FILE__), __LINE__, ##args)
#define LOG_HEX(info, data, len) do{\
			int i = 0;\
			printf("[%s][%d]%s len=%d>>>\r\n", FILE_NAME(__FILE__), __LINE__, info, len);\
			while (i < len)\
			{\
				printf("%02X ", (unsigned char)*(data + i));\
				if(i == 15 || ((i - 15) % 16 == 0 && i > 15))\
				{\
					printf("\r\n");\
				}\
				i ++;\
			}\
			printf("\r\n");\
		}while(0)
#endif
#else
#define LOG(fmt, args...) 
#define LOG_ERR(fmt, args...) 
#define LOG_HEX(data, len)
#endif

#define FREE(data)\
	if(data != NULL)\
	{\
		free(data);\
		data = NULL;\
	}
#define MALLOC(n, type) ((type *) malloc((n)* sizeof(type)))
#define BUFCLR(szBuf) memset(szBuf, 0x00, sizeof(szBuf))
#define ASSERT(e) e
#define ASSERT_FAIL(e) \
	if (ASSERT(e) != RET_SUCC)\
	{\
		return RET_FAIL;\
	}
#define SLEEP_MS(x) YMI_SysMsDelay(x)
#define SLEEP_S(x) YMI_SysMsDelay(x*1000)

#endif /* VAR_H */