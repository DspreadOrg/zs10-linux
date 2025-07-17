#ifndef __YMZN_API_H_
#define __YMZN_API_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#undef FD_SETSIZE
#define FD_SETSIZE  (15)
#define WAIT_FOREVER    (0xFFFFFFFFUL)
#define WAIT_NO_TIME    (0)
typedef struct {
  unsigned char fd_bits [(FD_SETSIZE + 7) / 8];
} ST_FD_SET;

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned long T_Fd;


typedef unsigned long T_Task;
typedef void * T_Sem;
typedef void * T_Mutex;
typedef void * T_Queue;

typedef void (*powerkey_long_press_cb)(void);

#define WLAN_SSID_MAX_LEN               32
#define MAX_INT (((unsigned int)(-1))>>1)

typedef enum {
    YMI_OK,                                     /* Operation successful */
    YMI_ERR_COMMON_BASE = 0,                    /* Public error code */
    YMI_ERR = YMI_ERR_COMMON_BASE - 1,          /**<Operation failed*/
    YMI_ERR_PARA = YMI_ERR_COMMON_BASE - 2,     /**<Illegal parameter*/
    YMI_ERR_PATH = YMI_ERR_COMMON_BASE - 3,     /**<Invalid file path*/
    YMI_ERR_READ = YMI_ERR_COMMON_BASE - 4,     /**<Failed to read the file*/
    YMI_ERR_WRITE = YMI_ERR_COMMON_BASE - 5,    /**<Failed to write the file*/
    YMI_ERR_TIMEOUT = YMI_ERR_COMMON_BASE - 7,  /**<Timeout error*/
    YMI_ERR_OPEN_DEV = YMI_ERR_COMMON_BASE - 9, /**<Error occurred while opening the device file*/
    YMI_ERR_NVM_WRITE = YMI_ERR_COMMON_BASE - 10,
    YMI_ERR_NVM_READ = YMI_ERR_COMMON_BASE - 11,
    YMI_ERR_MEM_MALLOC = YMI_ERR_COMMON_BASE - 12,//malloc failure
    YMI_ERR_GET_APP_DIR = YMI_ERR_COMMON_BASE - 13,
    YMI_ERR_NVM_OPEN = YMI_ERR_COMMON_BASE - 14,//Failed to open flash
    YMI_ERR_NVM_ERR_FILE = YMI_ERR_COMMON_BASE - 15,//Incorrect file
    YMI_ERR_NVM_UNSUPPORT = YMI_ERR_COMMON_BASE - 16,//Writing to flash is not supported
    YMI_ERR_NVM_READ_OVERSIZE = YMI_ERR_COMMON_BASE - 17,//The read data exceeds the saved cache

    YMI_ERR_SYSTEM_BASE = -100,
    YMI_ERR_SYSTEM_ENVPARAM_WRITE = YMI_ERR_SYSTEM_BASE - 1,
    YMI_ERR_SYSTEM_ENVPARAM_READ = YMI_ERR_SYSTEM_BASE - 2,
    YMI_ERR_SYSTEM_OVER_MAX_LENGTH = YMI_ERR_SYSTEM_BASE - 3,
    YMI_ERR_SYSTEM_INVALID_PARTITION = YMI_ERR_SYSTEM_BASE - 4,
    YMI_ERR_SYSTEM_SN_SET_REPEAT = YMI_ERR_SYSTEM_BASE - 5,//sn Repeat setting
    YMI_ERR_SYSTEM_SN_OVER_SIZE = YMI_ERR_SYSTEM_BASE - 6,//sn exceeds the limit length
    YMI_ERR_SYSTEM_SN_SET_FAIL = YMI_ERR_SYSTEM_BASE - 7,//sn setting failed
    YMI_ERR_SYSTEM_SN_GET_OVERSIZE = YMI_ERR_SYSTEM_BASE - 8,//The obtained sn exceeds the length of the cache

    YMI_ERR_SEMAPHORE_BASE = -150,  /* Semaphore */
    YMI_ERR_SEMAPHORE_CREATE_FAILED = YMI_ERR_SEMAPHORE_BASE - 1,
    YMI_ERR_SEMAPHORE_WAIT_FAILED = YMI_ERR_SEMAPHORE_BASE - 2,
    YMI_ERR_SEMAPHORE_RELEASE_FAILED = YMI_ERR_SEMAPHORE_BASE - 3,
    YMI_ERR_SEMAPHORE_GET_COUNTS_FAILED = YMI_ERR_SEMAPHORE_BASE - 4,
    YMI_ERR_SEMAPHORE_DELETE_FAILED = YMI_ERR_SEMAPHORE_BASE - 5,

    YMI_ERR_MUTEX_BASE = -160,  /* Mutex */
    YMI_ERR_MUTEX_CREATE_FAILED = YMI_ERR_MUTEX_BASE - 1,
    YMI_ERR_MUTEX_LOCK_FAILED = YMI_ERR_MUTEX_BASE - 2,
    YMI_ERR_MUTEX_UNLOCK_FAILED = YMI_ERR_MUTEX_BASE - 3,
    YMI_ERR_MUTEX_DELETE_FAILED = YMI_ERR_MUTEX_BASE - 4,

    YMI_ERR_QUEUE_BASE = -170,  /* Queue */
    YMI_ERR_QUEUE_CREATE_FAILED = YMI_ERR_QUEUE_BASE - 1,
    YMI_ERR_QUEUE_READ_FAILED = YMI_ERR_QUEUE_BASE - 2,
    YMI_ERR_QUEUE_WRITE_FAILED = YMI_ERR_QUEUE_BASE - 3,
    YMI_ERR_QUEUE_GET_COUNTS_FAILED = YMI_ERR_QUEUE_BASE - 4,
    YMI_ERR_QUEUE_DELETE_FAILED = YMI_ERR_QUEUE_BASE - 5,
    YMI_ERR_QUEUE_READ_OUT_TIME = YMI_ERR_QUEUE_BASE - 6,
    YMI_ERR_QUEUE_READ_OVER_SIZE = YMI_ERR_QUEUE_BASE - 7,
    YMI_ERR_QUEUE_WRITE_OUT_TIME = YMI_ERR_QUEUE_BASE - 8,

    YMI_ERR_TASK_BASE = -180,       /* Task */
    YMI_ERR_TASK_CREAT_FAILED = YMI_ERR_TASK_BASE - 1,
    YMI_ERR_TASK_DELETE_FAILED = YMI_ERR_TASK_BASE - 2,
    YMI_ERR_TASK_SUSPEND_FAILED = YMI_ERR_TASK_BASE - 3,
    YMI_ERR_TASK_RESUME_FAILED = YMI_ERR_TASK_BASE - 4,
    YMI_ERR_TASK_GET_STATUS_FAILED = YMI_ERR_TASK_BASE - 5,

    YMI_ERR_EVENT_BASE = -190,      /* Event */
    YMI_ERR_EVENT_SEND_FAILED = YMI_ERR_EVENT_BASE - 1,
    YMI_ERR_EVENT_WAIT_FAILED = YMI_ERR_EVENT_BASE - 2,
    YMI_ERR_EVENT_WAIT_TIMEOUT = YMI_ERR_EVENT_BASE - 3,

    YMI_ERR_BASE64_BASE = -200,      /* Base64 */
    YMI_ERR_BASE64_BUFFER_TOO_SMALL = YMI_ERR_BASE64_BASE - 1,
    YMI_ERR_BASE64_INVALID_CHARACTER = YMI_ERR_BASE64_BASE - 2,
    YMI_ERR_BASE64_UNKNOWN = YMI_ERR_BASE64_BASE - 9,

    YMI_ERR_FOTA_MASTER_APPLICATION_BASE = -220,  /* Master Application Fota */
    YMI_ERR_FOTA_ERR = YMI_ERR_FOTA_MASTER_APPLICATION_BASE - 1,

    YMI_ERR_AUDIO = -250,      /* Audio */
    YMI_ERR_AUDIO_PCM_OPEN = YMI_ERR_AUDIO - 1,
    YMI_ERR_AUDIO_PCM_CLOSE = YMI_ERR_AUDIO - 2,
    YMI_ERR_AUDIO_PCM_WRITE = YMI_ERR_AUDIO - 3,
    YMI_ERR_AUDIO_TTS_START = YMI_ERR_AUDIO - 4,
    YMI_ERR_AUDIO_FILE_START = YMI_ERR_AUDIO - 5,
    YMI_ERR_AUDIO_FILE_STOP = YMI_ERR_AUDIO - 6,
    YMI_ERR_AUDIO_ENGINE_INIT = YMI_ERR_AUDIO - 7,
    YMI_ERR_AUDIO_GET_VOLUME = YMI_ERR_AUDIO - 8,
    YMI_ERR_AUDIO_SET_VOLUME = YMI_ERR_AUDIO - 9,
    YMI_ERR_AUDIO_PLAY_BUSY = YMI_ERR_AUDIO - 10,
    YMI_ERR_AUDIO_PLAY_ERROR = YMI_ERR_AUDIO - 11,
    YMI_ERR_AUDIO_FILE_NOT_EXIST = YMI_ERR_AUDIO - 12,

    YMI_ERR_WIRE_BASE = -270,  /* DEV */
    YMI_ERR_WIRE_SET_MODEM_FUN = YMI_ERR_WIRE_BASE - 1,
    YMI_ERR_WIRE_GET_IMEI = YMI_ERR_WIRE_BASE - 2,
    YMI_ERR_WIRE_GET_VERSION = YMI_ERR_WIRE_BASE - 3,
    YMI_ERR_WIRE_GET_REG_STATUS = YMI_ERR_WIRE_BASE - 4,/* NW Network Information */
    YMI_ERR_WIRE_CSQ = YMI_ERR_WIRE_BASE - 5,
    YMI_ERR_WIRE_GET_CELL_INFO = YMI_ERR_WIRE_BASE - 6,
    YMI_ERR_WIRE_GET_SELECTION= YMI_ERR_WIRE_BASE - 7,
    YMI_ERR_WIRE_GET_OPERATORS= YMI_ERR_WIRE_BASE - 8,
    YMI_ERR_WIRE_GET_STATUS = YMI_ERR_WIRE_BASE - 9,/* SIM */
    YMI_ERR_WIRE_GET_ICCID = YMI_ERR_WIRE_BASE - 10,
    YMI_ERR_WIRE_SET_EXEC_MODE = YMI_ERR_WIRE_BASE - 11, /* DATACALL */
    YMI_ERR_WIRE_START = YMI_ERR_WIRE_BASE - 12,
    YMI_ERR_WIRE_STOP = YMI_ERR_WIRE_BASE - 13,
    YMI_ERR_WIRE_GET_INFO = YMI_ERR_WIRE_BASE - 14,
    YMI_ERR_WIRE_DNSGIP = YMI_ERR_WIRE_BASE - 15,
    YMI_ERR_WIRE_OPEN = YMI_ERR_WIRE_BASE - 16,/* SOCKET */
    YMI_ERR_WIRE_CLOSE = YMI_ERR_WIRE_BASE - 17,
    YMI_ERR_WIRE_CONNECT = YMI_ERR_WIRE_BASE - 18,
    YMI_ERR_WIRE_WRITE = YMI_ERR_WIRE_BASE - 19,
    YMI_ERR_WIRE_READ = YMI_ERR_WIRE_BASE - 20,
    YMI_ERR_WIRE_SLECT = YMI_ERR_WIRE_BASE - 21,
    YMI_ERR_WIRE_GETERR = YMI_ERR_WIRE_BASE - 22,
    YMI_ERR_WIRE_GET_SN = YMI_ERR_WIRE_BASE - 23,
    YMI_ERR_WIRE_GET_IMSI = YMI_ERR_WIRE_BASE - 24,

    YMI_ERR_RTC_BASE = -300,  /* DEV */
    YMI_ERR_RTC_GET_TIME = YMI_ERR_RTC_BASE - 1,
    YMI_ERR_RTC_GET_LOCAL_TIME = YMI_ERR_RTC_BASE - 2,
    YMI_ERR_RTC_SET_TIME = YMI_ERR_RTC_BASE - 3,
    
    YMI_ERR_WIFI_BASE = -320,  /* wifi */
    YMI_ERR_WIFI_OPEN = YMI_ERR_WIFI_BASE - 1,
    YMI_ERR_WIFI_CONNECT_TYPE = YMI_ERR_WIFI_BASE - 2,
    YMI_ERR_WIFI_CONNECT_AP = YMI_ERR_WIFI_BASE - 3,
    YMI_ERR_WIFI_CONNECT_DIR = YMI_ERR_WIFI_BASE - 4,
    YMI_ERR_WIFI_SET_STA = YMI_ERR_WIFI_BASE - 5,
    YMI_ERR_WIFI_QUESTAT = YMI_ERR_WIFI_BASE - 6,
    YMI_ERR_WIFI_GET_HOST = YMI_ERR_WIFI_BASE - 7,
    YMI_ERR_WIFI_GET_AP_INFO = YMI_ERR_WIFI_BASE - 8,
    YMI_ERR_WIFI_SET_RF = YMI_ERR_WIFI_BASE - 9,
    YMI_ERR_WIFI_GET_AP_LEVEL = YMI_ERR_WIFI_BASE - 10,
    YMI_ERR_WIFI_READ_MAC = YMI_ERR_WIFI_BASE - 11,
    YMI_ERR_WIFI_SELF_UPDATE = YMI_ERR_WIFI_BASE - 12,
    YMI_ERR_WIFI_NET_POOR_FAILED = YMI_ERR_WIFI_BASE - 13,
    YMI_ERR_WIFI_NET_CONN_FAILED = YMI_ERR_WIFI_BASE - 14,
    YMI_ERR_WIFI_NET_DISCONNECT = YMI_ERR_WIFI_BASE - 15,
    YMI_ERR_WIFI_NET_MEM_FAILED = YMI_ERR_WIFI_BASE - 16,
    YMI_ERR_WIFI_NET_RECV_FAILED = YMI_ERR_WIFI_BASE - 17,
    YMI_ERR_WIFI_NET_BUG_FAILED = YMI_ERR_WIFI_BASE - 18,
    YMI_ERR_WIFI_NET_INVALID_CONTEXT = YMI_ERR_WIFI_BASE - 19,
    YMI_ERR_WIFI_NET_SEND_FAILED = YMI_ERR_WIFI_BASE - 20,

    YMI_ERR_HTTP_BASE = -400,//http
    YMI_ERR_HTTP_PARSE_URL = YMI_ERR_HTTP_BASE - 1,
    YMI_ERR_HTTP_SYSDEP_GET_PORTFILE = YMI_ERR_HTTP_BASE - 2,
    YMI_ERR_HTTP_CORE_INIT = YMI_ERR_HTTP_BASE - 3,
    YMI_ERR_HTTP_CORE_SET_OPT = YMI_ERR_HTTP_BASE - 4,
    YMI_ERR_HTTP_CORE_CONNECT = YMI_ERR_HTTP_BASE - 5,
    YMI_ERR_HTTP_CORE_SEND = YMI_ERR_HTTP_BASE - 6,
    YMI_ERR_HTTP_RET_CONTENT = YMI_ERR_HTTP_BASE - 7,//No return of content
    YMI_ERR_HTTP_RET_CONTENT_CODE = YMI_ERR_HTTP_BASE - 8,//code is not 20
    YMI_ERR_HTTP_RET_CONTENT_OVERSIZE = YMI_ERR_HTTP_BASE - 9,//Return when the size of the content exceeds the saved cache capacity
    YMI_ERR_HTTP_DL_INIT = YMI_ERR_HTTP_BASE - 10,//Download initialization failed
    YMI_ERR_HTTP_DL_SET_OPT = YMI_ERR_HTTP_BASE - 11,//Parameter setting failed
    YMI_ERR_HTTP_DL_SEND_REQ = YMI_ERR_HTTP_BASE - 12,//The download request sent failed.
    YMI_ERR_HTTP_DL_SAVE = YMI_ERR_HTTP_BASE - 13,//Save failed
    YMI_ERR_HTTP_DL_RECV = YMI_ERR_HTTP_BASE - 14,//Receiving failed
    YMI_ERR_HTTP_CONNECT_TIMEOUT = YMI_ERR_HTTP_BASE - 15,//Connection timeout
    YMI_ERR_HTTP_RECV_TIMEOUT = YMI_ERR_HTTP_BASE - 16,//Receive timeout

    YMI_ERR_JSON_BASE = -440,//json
    YMI_ERR_JSON_CREATE = YMI_ERR_JSON_BASE - 1, /* Error occurred while creating JSON. */
    YMI_ERR_JSON_PARSE = YMI_ERR_JSON_BASE - 2, /* Error parsing JSON. */
    YMI_ERR_JSON_GET_ITEM = YMI_ERR_JSON_BASE - 3,  /* Error in obtaining JSON object */
    YMI_ERR_JSON_SIGN = YMI_ERR_JSON_BASE - 4,
    YMI_ERR_JSON_TYPE_NO_SAME = YMI_ERR_JSON_BASE - 5, /* The parsed field type is incorrect. */
    YMI_ERR_JSON_TYPE_NO_FOUND = YMI_ERR_JSON_BASE - 6, /* No corresponding type parsing found */
    YMI_ERR_JSON_UNPACK_FUNC = YMI_ERR_JSON_BASE - 7, /* Error in custom parsing function */
    YMI_ERR_JSON_UNPACK_OUTBUFF = YMI_ERR_JSON_BASE - 8, /* The length of the string data after parsing is greater than the length of the input data */
    YMI_ERR_JSON_PRINT = YMI_ERR_JSON_BASE - 9, /* Failed to convert JSON to string. */
    YMI_ERR_JSON_PACK_FUNC = YMI_ERR_JSON_BASE - 10, /* Error in custom parsing function */
    YMI_ERR_JSON_KEY_NOFOUND = YMI_ERR_JSON_BASE - 11, /* The specified key does not correspond to any key-value pair */

    YMI_ERR_FLASH_BASE = -460,                     /* Flash */
    YMI_ERR_FLASH_ERASE = YMI_ERR_FLASH_BASE - 1,
    YMI_ERR_FLASH_WRITE = YMI_ERR_FLASH_BASE - 2,
    YMI_ERR_FLASH_READ = YMI_ERR_FLASH_BASE - 3,  

    YMI_ERR_FS_BASE = -480,                     /* FS */
    YMI_ERR_FS_OPEN = YMI_ERR_FS_BASE - 1,
    YMI_ERR_FS_CLOSE = YMI_ERR_FS_BASE - 2,
    YMI_ERR_FS_READ = YMI_ERR_FS_BASE - 3,  
    YMI_ERR_FS_WRITE = YMI_ERR_FS_BASE - 4,  
    YMI_ERR_FS_SEEK = YMI_ERR_FS_BASE - 5,  
    YMI_ERR_FS_DEL = YMI_ERR_FS_BASE - 6,  
    YMI_ERR_FS_SIZE = YMI_ERR_FS_BASE - 7,  
    YMI_ERR_FS_EXIST = YMI_ERR_FS_BASE - 8,  
    YMI_ERR_FS_TELL = YMI_ERR_FS_BASE - 9,  
    YMI_ERR_FS_TRUNCATE = YMI_ERR_FS_BASE - 10,  
    YMI_ERR_FS_RENAME = YMI_ERR_FS_BASE - 11,  
    YMI_ERR_FS_DIR = YMI_ERR_FS_BASE - 12,  
    YMI_ERR_FS_SYNC = YMI_ERR_FS_BASE - 13,
    YMI_ERR_FS_ILLEGAL_NAME = YMI_ERR_FS_BASE - 14,
    YMI_ERR_FS_MAKE_DIR = YMI_ERR_FS_BASE - 15,   
    YMI_ERR_FS_DEL_DIR = YMI_ERR_FS_BASE - 16, 
    YMI_ERR_FS_DIR_OVER_BUF = YMI_ERR_FS_BASE - 17, 
    YMI_ERR_FS_NAME_TO_LONG = YMI_ERR_FS_BASE - 18,
    YMI_ERR_FS_GET_SPACE = YMI_ERR_FS_BASE - 19,

    YMI_ERR_PORT_BASE = -500,                     /* Port */
    YMI_ERR_PORT_OPEN = YMI_ERR_PORT_BASE - 1,
    YMI_ERR_PORT_CLOSE = YMI_ERR_PORT_BASE - 2, 
    YMI_ERR_PORT_READ = YMI_ERR_PORT_BASE - 3,
    YMI_ERR_PORT_WRITE = YMI_ERR_PORT_BASE - 4,
    YMI_ERR_PORT_CLEAR_READBUF = YMI_ERR_PORT_BASE - 5,
    YMI_ERR_PORT_NO_SUPPORT = YMI_ERR_PORT_BASE - 6,
    YMI_ERR_PORT_ENABLE = YMI_ERR_PORT_BASE - 7,
    
    YMI_ERR_HID_BASE = -510,                     /* HID */
    YMI_ERR_HID_OPEN = YMI_ERR_HID_BASE - 1,
    YMI_ERR_HID_CLOSE = YMI_ERR_HID_BASE - 2, 
    YMI_ERR_HID_READ = YMI_ERR_HID_BASE - 3,
    YMI_ERR_HID_WRITE = YMI_ERR_HID_BASE - 4,
    YMI_ERR_HID_CLEAR_READBUF = YMI_ERR_HID_BASE - 5,
    YMI_ERR_HID_KEYBOARD_SEND = YMI_ERR_HID_BASE - 6,

    YMI_ERR_DISP_BASE = -520,                     /* Display */
    YMI_ERR_DISP_IMAGE = YMI_ERR_DISP_BASE - 1,
    YMI_ERR_DISP_CLEAR = YMI_ERR_DISP_BASE - 2,
    YMI_ERR_DISP_JPG_TO_BMP = YMI_ERR_DISP_BASE - 3,
    YMI_ERR_DISP_GET_JPG_INFO = YMI_ERR_DISP_BASE - 4,

    YMI_ERR_FOTA_SE_BASE = -540, /* SE Fota */
    YMI_ERR_FOTA_SE_GET_BOOT_VER = YMI_ERR_FOTA_SE_BASE - 1,
    YMI_ERR_FOTA_SE_BOOT_VER_INVALID = YMI_ERR_FOTA_SE_BASE - 2,
    YMI_ERR_FOTA_SE_GET_COMMAND_VER = YMI_ERR_FOTA_SE_BASE - 3,
    YMI_ERR_FOTA_SE_INVALID_CFG_FILE = YMI_ERR_FOTA_SE_BASE - 4,
    YMI_ERR_FOTA_SE_MAX_RETRY = YMI_ERR_FOTA_SE_BASE - 5,
    YMI_ERR_FOTA_SE_CONFIG_FAIL_BASE = -570,
    YMI_ERR_FOTA_SE_FIRMWARE_FAIL_BASE = -580,
    YMI_ERR_FOTA_SE_APPLICATION_FAIL_BASE = -590,

    YMI_ERR_SLAVE_BASE = -600,                     /* Slave */
    YMI_ERR_SLAVE_WRITE = YMI_ERR_SLAVE_BASE - 1,
    YMI_ERR_SLAVE_READ = YMI_ERR_SLAVE_BASE - 2,
    YMI_ERR_SLAVE_GET_TAMPER_CODE = YMI_ERR_SLAVE_BASE - 3,

    YMI_ERR_RFCARD_BASE = -620,                     /* RFCard */
    YMI_ERR_RFCARD_OPEN = YMI_ERR_RFCARD_BASE - 1,
    YMI_ERR_RFCARD_CLOSE = YMI_ERR_RFCARD_BASE - 2,
    YMI_ERR_RFCARD_SET_TYPE = YMI_ERR_RFCARD_BASE - 3,
    YMI_ERR_RFCARD_POWER_OFF = YMI_ERR_RFCARD_BASE - 4,
    YMI_ERR_RFCARD_POWER_ON_DETECT = YMI_ERR_RFCARD_BASE - 5,
    YMI_ERR_RFCARD_APDU = YMI_ERR_RFCARD_BASE - 6,

    YMI_ERR_POWER_BASE = -640,                     /* Power */
    YMI_ERR_POWER_USB_INSERT = YMI_ERR_POWER_BASE - 1,

    YMI_ERR_FOTA_FILE_ANALYSE_BASE = -650,  /* Fota File Analyse */
    YMI_ERR_FOTA_FILE_ANALYSE_NOT_EXIST = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 1,
    YMI_ERR_FOTA_FILE_ANALYSE_FILE_OPEN = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 2,
    YMI_ERR_FOTA_FILE_ANALYSE_FILE_HEAD = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 3,
    YMI_ERR_FOTA_FILE_ANALYSE_FILE_NAME_LENGTH = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 4,
    YMI_ERR_FOTA_FILE_ANALYSE_FILE_NAME = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 5,
    YMI_ERR_FOTA_FILE_ANALYSE_FILE_LENGTH = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 6,
    YMI_ERR_FOTA_FILE_ANALYSE_FILE_READ = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 7,
    YMI_ERR_FOTA_FILE_ANALYSE_FILE_SIZE = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 8,
    YMI_ERR_FOTA_FILE_ANALYSE_FILE_WRITE = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 9,
    YMI_ERR_FOTA_FILE_ANALYSE_MD5_ILLEGAL = YMI_ERR_FOTA_FILE_ANALYSE_BASE - 10,

    YMI_ERR_ZIP_BASE = -670,  /*  */
    YMI_ERR_ZIP_CREATE_FAILS = YMI_ERR_ZIP_BASE - 1,
    YMI_ERR_ZIP_ILLEGAL_NAME = YMI_ERR_ZIP_BASE - 2,
    YMI_ERR_ZIP_ADD_FILE_FAILS = YMI_ERR_ZIP_BASE - 3,
    YMI_ERR_ZIP_UNZIP_FAILS = YMI_ERR_ZIP_BASE - 4,
    YMI_ERR_ZIP_CLOSE_FAILS = YMI_ERR_ZIP_BASE - 5,
    YMI_ERR_ZIP_NAME_TO_LONG = YMI_ERR_ZIP_BASE - 6,
    YMI_ERR_ZIP_OPEN_FAILS = YMI_ERR_ZIP_BASE - 7,
    YMI_ERR_ZIP_MAKE_DIR_FAILS = YMI_ERR_ZIP_BASE - 8,
    YMI_ERR_ZIP_MALLOC_FAILS = YMI_ERR_ZIP_BASE - 9,

    YMI_ERR_VIDEO_PLAY_BASE = -680,
    YMI_ERR_VIDEP_PLAY_INIT = YMI_ERR_VIDEO_PLAY_BASE - 1,
    YMI_ERR_VIDEP_PLAY_DEINIT = YMI_ERR_VIDEO_PLAY_BASE - 2,
    YMI_ERR_VIDEP_PLAY_START = YMI_ERR_VIDEO_PLAY_BASE - 3,
    YMI_ERR_VIDEP_PLAY_UPDATE_WINDOWS = YMI_ERR_VIDEO_PLAY_BASE - 4,
    YMI_ERR_VIDEP_PLAY_STOP = YMI_ERR_VIDEO_PLAY_BASE - 5,


    YMI_ERR_NTP_BASE = 700,
    YMI_ERR_NTP_SET_TIME = YMI_ERR_NTP_BASE - 1,
    YMI_ERR_NOT_SUPPORT = -9999,
}EM_YMI_ERR;

/* Log API*/
void YMI_Log(const char *pszFmt, ...);

/* Port API */
/**
 *@brief  Serial port selection
*/
typedef enum {
    PORT_COM1 = 0,      /**<Serial Port 1*/
    PORT_COM2 = 1,      /**<Serial Port 2*/
    PORT_USB = 2,       /**<USB module*/
    PORT_MODEM = 3,     /**<Modem port*/
    PORT_MAX_NUM
} EM_PORT;

/**
 *@brief    Initialize the serial port and set the serial port baud rate, data bits, parity bits, stop bits, etc. It is recommended to call this initialization function before using the serial port each time. (For USB, pszAttr does not need to be set; pszAttr can be NULL.)
                The supported baud rates are {300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}
                The supported data bits are {8, 7, 6, 5}
                The parity check mode selection is {N(n): no parity check; O(o): odd parity check; E(e): even parity check}
                The supported stop bits are {1, 2}
 *@param    ePort  The designated serial port
 *@param    c_pszAttr Communication rate and format string, for example "115200,8,N,1"
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(pszAttr is NULL and emPort is an illegal serial port type)
 *@li   \ref YMI_ERR_PORT_OPEN "YMI_ERR_PORT_OPEN"    Operation failed
*/
int YMI_PortOpen(EM_PORT ePort, const char *c_pszAttr);

/**
 *@brief    Close the serial port
 *@param    ePort  The designated serial port
 *@return
 *@li   YMI_OK      Operation successful
 *@li   \ref YMI_ERR_PORT_CLOSE "YMI_ERR_PORT_CLOSE"      Operation failed
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter(Invalid serial port type)
*/
int YMI_PortClose(EM_PORT ePort);

/**
 *@brief    Determine how many bytes are to be read from the buffer (if the expected data length is not fully retrieved in one attempt, multiple retrievals are carried out in cooperation with the \ref YMI_PortRead "YMI_PortRead()" function, and the lengths retrieved each time are accumulated)
 *@param    ePort  The designated serial port
 *@retval   piInFIFOLen   The length of the data that has been read from the buffer for return
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(piInFIFOLen is NULL, emPort is an illegal serial port type)
*/
int YMI_PortGetRecvDataLen(EM_PORT ePort,int *piInFIFOLen);
/**
 *@brief    During the specified timeout period, read data of the designated length from the designated serial port and store it in pszOutbuf.
 *@param    ePort  The designated serial port
 *@param    wRequestReadLen: Specifies the length of data to be read, greater than 0 (less than 4K)
 *@param    iTimeoutMs: Waiting time, in milliseconds
 *@retval   pcRecvBuffer: Pointer to the head of the received data buffer
 *@retval   piActualReadLen: Actual read length returned
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(wRequestReadLen <= 0||pcRecvBuffer == NULL||iTimeoutMs <= 0||piActualReadLen == NULL||ePort >= PORT_MAX_NUM)
 *@li   \ref YMI_ERR_PORT_READ "YMI_ERR_PORT_READ"                Failed to read data
 *@li   \ref YMI_ERR_TIMEOUT "YMI_ERR_TIMEOUT"      Timeout error (serial port read timeout)
*/
int YMI_PortRead(EM_PORT ePort, uint wRequestReadLen, char *pcRecvBuffer, int iTimeoutMs, int *piActualReadLen);

/**
 *@brief    Send a specified length of data to the designated serial port. 
 *@param    ePort  The designated serial port
 *@param    wRequestSendLen: Specifies the length of data to be written.
 *@param    iTimeoutMs: Sets the timeout duration for data transmission.
 *@retval   c_pcSendBuffer: The buffer for data transmission.
 *@retval   piActualSendLen: The actual length of data sent.
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(wRequestSendLen <= 0||c_pcSendBuffer == NULL||iTimeoutMs <= 0||ePort >= PORT_MAX_NUM||piActualSendLen == NULL)
 *@li   \ref YMI_ERR_PORT_WRITE "YMI_ERR_PORT_WRITE"              Data writing failed
 *@li   \ref YMI_ERR_TIMEOUT "YMI_ERR_TIMEOUT"      Timeout error (serial port transmission timeout)
*/
int YMI_PortWrite(EM_PORT ePort, uint wRequestSendLen,const char *c_pcSendBuffer, int iTimeoutMs, int *piActualSendLen);

/**
 *@brief    Clear the receive buffer of the specified serial port
 *@param    ePort  The designated serial port
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter (Invalid emPort serial port type)
 *@li   \ref YMI_ERR_PORT_CLEAR_READBUF "YMI_ERR_PORT_CLEAR_READBUF"         Failure in clearing read buffer
*/
int YMI_PortClearRecvBuffer(EM_PORT ePort);

typedef enum {
    PORT_EVENT_RECV_DATA = 0,   //Received data
    PORT_EVENT_RECV_OVERFLOW,   //Receive data overflow
    PORT_EVENT_SEND_COMPLETE,   //Send completed
} EM_PORT_EVENT;

/**
 *@brief    Register event notifications for the specified serial port
 *@param    ePort  The designated serial port
 *@param    tTaskID  The notification thread corresponding to the serial port event
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(ePort >= PORT_MAX_NUM || tTaskID == 0)
 *@li   \ref YMI_ERR_NOT_SUPPORT "YMI_ERR_NOT_SUPPORT"  Serial ports are not supported
*/
int YMI_PortRegisterEventNotify(EM_PORT ePort, T_Task tTaskID);

/**
 *@brief    Log out the event notification for the specified serial port
 *@param    ePort  The designated serial port
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(ePort >= PORT_MAX_NUM)
 *@li   \ref YMI_ERR_NOT_SUPPORT "YMI_ERR_NOT_SUPPORT"  Serial ports are not supported
*/
int YMI_PortDeregisterEventNotify(EM_PORT ePort);

/* System API */
/**
 *@brief     Unit delay (with a unit time of 1ms)
 *@details
 *@param    unDelayTime Delay time
 *@return
 *@li    YMI_OK             Operation successful
*/
int YMI_SysMsDelay(uint wDelayTime);

/**
 *@brief     Device restart
 *@details
 *@return
 *@li    YMI_OK             Operation successful
*/
int YMI_SysReboot(void);

/**
 *@brief     System shutdown
 *@details
 *@return
 *@li    YMI_OK             Operation successful
*/
int YMI_SysShutDown(void);

/**
 *@brief    Get the system tick
 *@details  Monocline system time (in milliseconds)
 *@retval   Returns the time elapsed from system boot to the present
*/
int64_t YMI_SysGetTick(void);

/**
 *@brief    Dynamic memory request
 *@param    wSize The size of the memory allocation
*/
void * YMI_SysMalloc(uint wSize);

/**
 *@brief    Dynamically apply for memory release
 *@param    pvMem malloc, calloc, or realloc dynamically allocates the address of the memory
*/
void YMI_SysFree(void *pvMem);

/**
 *@brief    Dynamic memory allocation, which is used to reallocate the size of memory blocks that have been allocated before
 *@param    pvMemAddr malloc, calloc, or realloc dynamically allocates the address of the memory
 *@param    wNewSize The size of the memory reallocation
*/
void *YMI_SysRealloc(void *pvMemAddr, uint wNewSize);

/**
 *@brief    Gets system heap usage
 *@param    pwTotalSize The total heap size of memory
 *@param    pwFreeSize Remaining memory size
 *@param    pwMaxBlockSize is the maximum allocable block size
 *@return
 *@li    YMI_OK             Operation successful
 *@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter(pwTotalSize == NULL || pwFreeSize == NULL || pwMaxBlockSize == NULL)
 *@li    \ref YMI_ERR "YMI_ERR"     Operation failed
*/
int YMI_SysGetHeapInfo(uint *pwTotalSize, uint *pwFreeSize, uint *pwMaxBlockSize);

/**
 *@brief    Checksums load the specified library
 *@details
 *@param    pszLibName The name of the library
 *@param    apiStruct Pointers to interface functions exported by the library
 *@return
 *@li    YMI_OK             Operation successful
 *@li   \ref YMI_ERR "YMI_ERR"      Operation failed
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (pszLibName == NULL)
*/
int YMI_SysLibraryLoad(const char * pszLibName, void ** ppApiStruct);

/**
 *@brief     Obtain partition version information
 *@details
 *@param    c_pszPartName The name of the partition
 *@param    pp_pszVersion Partition the version information buffer
 *@return
 *@li   \ref YMI_OK             Operation successful
 *@li   \ref YMI_ERR_NOT_SUPPORT "YMI_ERR_NOT_SUPPORT" Not supported
 *@li   \ref YMI_ERR_SYSTEM_INVALID_PARTITION "YMI_ERR_SYSTEM_INVALID_PARTITION" Partition verification failed
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (c_pszPartName == NULL || pp_pszVersion == NULL)
*/
int YMI_SysGetPartitionVersion(const char * c_pszPartName, char ** pp_pszVersion);

/* Thread API */
typedef enum
{
	EM_TASK_RUNNING = 0,	/* A task is querying the state of itself, so must be running. */
	EM_TASK_READY,			/* The task being queried is in a read or pending ready list. */
	EM_TASK_BLOCK,		    /* The task being queried is in the Blocked state. */
	EM_TASK_SUSPENDED,		/* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
	EM_TASK_DELETE,		/* The task being queried has been deleted, but its TCB has not yet been freed. */
	EM_TASK_INVAILD		/* Used as an 'invalid state' value. */
} EM_TASK_STATE;

typedef struct 
{
	int		        iTaskID;			    /* The handle of the task to which the rest of the information in the structure relates. */
	const char *    c_pszName;			    /* A pointer to the task's name.  This value will be invalid if the task was deleted since the structure was populated! */
	EM_TASK_STATE 	eState;		        /* The state in which the task existed when the structure was populated. */
	unsigned long 	ulPriority;	        /* The priority at which the task was running (may be inherited) when the structure was populated. */
	uint16_t	    wStackHighWaterMark;	/* The minimum amount of stack space that has remained for the task since the task was created.  The closer this value is to zero the closer the task has come to overflowing its stack. */
} ST_TaskStatus;

typedef enum
{
	EM_TASK_PRIORITY_IDLE = 1, // reserved
    EM_TASK_PRIORITY_LOW = 4,
    EM_TASK_PRIORITY_BELOW_NORMAL = 8,
    EM_TASK_PRIORITY_NORMAL = 12,
    EM_TASK_PRIORITY_ABOVE_NORMAL = 16,
    EM_TASK_PRIORITY_BELOW_HIGH = 25,
    EM_TASK_PRIORITY_HIGH = 30  
}EM_TASK_PRIORITY;

typedef struct
{
    uint wId;     ///< event identifier
    ulong wParam1; ///< 1st parameter
    ulong wParam2; ///< 2nd parameter
    ulong wParam3; ///< 3rd parameter
}ST_Event;

/**
*@brief    Create a task
*@param    ptTaskId  Task handles
*@param    pszName   The name of the task
*@param    wStackSize   Task stack size, maximum: 128 k; Unit: bytes
*@param    pvTaskFunc   The entry function of the task
*@param    pvArgv   Parameters passed to the task
*@param    ePriority   Task priority, range: 0~30.
*@param    wEventCnt   The maximum number of pending events held in the task
*@return
*@li    YMI_OK             Operation successful
*@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter (ptTaskId，pszName，pvTaskFunc is NULL,wStackSize == 0，ePriorityIllegal parameter)
*@li    \ref YMI_ERR_TASK_CREAT_FAILED "YMI_ERR_TASK_CREAT_FAILED"    The task failed to be created
*/
int YMI_TaskCreat(T_Task *ptTaskId, char *pszName, uint wStackSize, void *pvTaskFunc, void *pvArgv, EM_TASK_PRIORITY ePriority, uint wEventCnt);

/**
*@brief    Delete the task
*@param    tTaskID  Task handles
*@return
*@li    YMI_OK             Operation successful
*@li    \ref YMI_ERR_TASK_DELETE_FAILED "YMI_ERR_TASK_DELETE_FAILED"    Failed to delete the task
*/
int YMI_TaskDelete(T_Task tTaskID);

/* Semaphore API */
/**
*@brief    Create a semaphore
*@param    ptSemId  Semaphore
*@param    wInitCount   The initial semaphore value
*@param    wMaxCount   The maximum semaphore
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (ptSemId == NULL)
*@li   \ref YMI_ERR_SEMAPHORE_CREATE_FAILED "YMI_ERR_SEMAPHORE_CREATE_FAILED"    Failed to create semaphore
*/
int YMI_SemaphoreCreat(T_Sem *ptSemId, uint wInitCount, uint wMaxCount);

/**
*@brief    Set the semaphore wait time
*@param    SemId  Semaphore
*@param    MsTimeout   Semaphore wait time. Unit: ms. 0x ffffffff: indicates that the wait is permanent.
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tSemId == 0)
*@li   \ref YMI_ERR_SEMAPHORE_WAIT_FAILED "YMI_ERR_SEMAPHORE_WAIT_FAILED"    Semaphore acquisition failed
*/
int YMI_SemaphoreWait(T_Sem tSemId, uint wMsTimeout);

/**
*@brief    Release semaphore
*@param    tSemId  Semaphore
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter
*@li   \ref YMI_ERR_SEMAPHORE_RELEASE_FAILED "YMI_ERR_SEMAPHORE_RELEASE_FAILED"    Failed to release semaphore
*/
int YMI_SemaphoreRelease(T_Sem tSemId);

/**
*@brief    Gets the semaphore value
*@param    SemId  Semaphore
*@param    Counts   The value of the output semaphore
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tSemId == 0 || pwCounts == NULL)
*@li   \ref YMI_ERR_SEMAPHORE_GET_COUNTS_FAILED "YMI_ERR_SEMAPHORE_GET_COUNTS_FAILED"    Failed to get semaphore value
*/
int YMI_SemaphoreGetCount(T_Sem tSemId, uint *pwCounts);
/**
*@brief    Delete the semaphore
*@param    SemId  Semaphore
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tSemId == 0)
*@li   \ref YMI_ERR_SEMAPHORE_DELETE_FAILED "YMI_ERR_SEMAPHORE_DELETE_FAILED"   Failed to delete semaphore
*/
int YMI_SemaphoreDelete(T_Sem tSemId);

/**
*@brief    Create a mutex
*@param    MutexId  Mutex
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (ptMutexId == NULL)
*@li   \ref YMI_ERR_MUTEX_CREATE_FAILED "YMI_ERR_MUTEX_CREATE_FAILED"   Failed to create a mutex
*/
int YMI_MutexCreat(T_Mutex *ptMutexId);

/**
*@brief    Get a mutex
*@param    MutexId  Mutex
*@param    MsTimeout   Mutex wait time. Unit: ms. 0xffffffff: indicates that the wait is permanent
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tMutexId == 0)
*@li   \ref YMI_ERR_MUTEX_LOCK_FAILED "YMI_ERR_MUTEX_LOCK_FAILED"   Failed to obtain mutex
*/
int YMI_MutexLock(T_Mutex tMutexId, uint wMsTimeout);

/**
*@brief    Release the mutex
*@param    MutexId  Mutex
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tMutexId == 0)
*@li    \ref YMI_ERR_MUTEX_UNLOCK_FAILED "YMI_ERR_MUTEX_UNLOCK_FAILED"   Failed to release the mutex
*/
int YMI_MutexUnLock(T_Mutex tMutexId);

/**
*@brief    Remove mutexes
*@param    MutexId  Mutex
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter YMI_MutexLock
*@li   \ref YMI_ERR_MUTEX_DELETE_FAILED "YMI_ERR_MUTEX_DELETE_FAILED"   Failed to remove mutex
*/
int YMI_MutexDelete(T_Mutex tMutexId);

/**
*@brief    Create a message queue
*@param    QueueId  Message queues
*@param    QueueSize  The maximum message size supported by Message Queuing. Unit: bytes
*@param    QueueNum  The maximum number of messages that can be supported in a message queue
*@return
*@li    YMI_OK             Operation successful 
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (ptQueueId == NULL || wQueueSize == 0 || wQueueNum == 0)
*@li   \ref YMI_ERR_QUEUE_CREATE_FAILED "YMI_ERR_QUEUE_CREATE_FAILED"   Failed to create a message queue
*/
int YMI_QueueCreat(T_Queue *ptQueueId, uint wQueueSize, uint wQueueNum);

/**
*@brief     Wait for messages in the queue
*@param     QueueId message queues
*@param     Msg receives the message pointer
*@param     MsgSize This parameter size is fixed at the queue size when the queue was created.
*@param     MsTimeout queue message wait time. Unit: ms. 0xFFFFFFFF means to wait forever
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tQueueId == 0 || pvMsg == NULL)
*@li   \ref YMI_ERR_QUEUE_READ_FAILED "YMI_ERR_QUEUE_READ_FAILED"   Wait for the message to fail
*/
int YMI_QueueRead(T_Queue tQueueId, uint8_t *pvMsg, uint wMsgSize, uint wMsTimeout);

/**
*@brief     Release the message queue
*@param     QueueId message queues
*@param     wMsgSize message size. Unit: bytes.
*@param     The start address of the data to be sent by pwMsg
*@param     wMsTimeout queue release wait time. Unit: milliseconds. 0xFFFFFFFF means to wait forever
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tQueueId == 0 || wMsgSize == 0 || pvMsg == NULL)
*@li   \ref YMI_ERR_QUEUE_WRITE_FAILED "YMI_ERR_QUEUE_WRITE_FAILED"   Failed to release the message queue
*/
int YMI_QueueWrite(T_Queue tQueueId, uint8_t *pvMsg, uint wMsgSize, uint wMsTimeout);

/**
*@brief    Gets the number of messages in the queue
*@param    QueueId  Message queues
*@param    Counts  The number of messages stored in the queue
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tQueueId == 0 || pwCounts == NULL)
*@li   \ref YMI_ERR_QUEUE_GET_COUNTS_FAILED "YMI_ERR_QUEUE_GET_COUNTS_FAILED"  Failed to get the number of queue messages
*/
int YMI_QueueGetCount(T_Queue tQueueId, uint *pwCounts);

/**
*@brief    Delete the message queue
*@param    QueueId  Message queues
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tQueueId == 0)
*@li   \ref YMI_ERR_QUEUE_DELETE_FAILED "YMI_ERR_QUEUE_DELETE_FAILED"   Failed to create a message queue
*/
int YMI_QueueDelete(T_Queue tQueueId);

/**
*@brief    Send events
*@param    tTaskID  Task handles
*@param    pstEvent Event structs, which cannot be empty
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (tTaskID == 0 || pstEvent == NULL)
*@li   \ref YMI_ERR_EVENT_SEND_FAILED "YMI_ERR_EVENT_SEND_FAILED"   Failed to send the event
*/
int YMI_EventSend(T_Task tTaskID, ST_Event *pstEvent);

/**
*@brief    Wait for the event
*@param    pstEvent  Event structs, which cannot be empty
*@param    wMsTimeout Event wait time. Unit: ms. 0xFFFFFFFF means to wait forever
*@return
*@li    YMI_OK             Operation successful
*@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (pstEvent == NULL)
*@li   \ref YMI_ERR_EVENT_WAIT_FAILED "YMI_ERR_EVENT_WAIT_FAILED"   Wait for the event to fail
*/
int YMI_EventWait(ST_Event *pstEvent, uint wMsTimeout);

/** @addtogroup file system
* @
*/
typedef enum {
    EM_FS_SEEK_STA = 0,    /**Start with the header of the file*/
    EM_FS_SEEK_CUR = 1,    /**Start at your current location*/
    EM_FS_SEEK_END = 2,    /**Start at the end of the file*/
}EM_FS_SEEK;

typedef enum {
    EM_FS_GET_SPACE_USED = 0,    /**Disk space used*/
    EM_FS_GET_SPACE_UNUSED = 1,  /**Disk space remaining*/
}EM_FS_GET_SPACE;

/**
 *@brief        Open the file.
 *@details
 *@param    c_pszName filename
 *@param    c_pszMode Open mode "r" (open as read-only, fail if it doesn't exist) or "w" (open as write, create if the file doesn't exist).
 *@return
 *@li    fd             >0 means that the operation is successful and returns the file descriptor, others means that it fails
*/
T_Fd YMI_FsOpen(const char *c_pszName,const char *c_pszMode);

/**
 *@brief        Close the file.
 *@details
 *@param    iHandle File handles
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(iHandle <= 0) 
 *@li   \ref YMI_ERR_FS_CLOSE "YMI_ERR_FS_CLOSE"      Operation failed
*/
int YMI_FsClose(T_Fd iHandle);

/**
 *@brief        The current pointer from the open i handle file reads w length characters to the psz buffer buffer
 *@details
 *@param    iHandle File handles
 *@param    wLength    The length of the character that needs to be read
 *@retval   pszBuffer    The buffer that needs to be read in should be sufficient to read wLength bytes
 *@return
 *@li   Returns the actual read data length
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"                The parameter is incorrect(pszBuffer == NULL || iHandle <= 0 || wLength == 0)
 *@li   \ref YMI_ERR_FS_READ "YMI_ERR_FS_READ"        Failed to read the file
*/
int YMI_FsRead(T_Fd iHandle, char *pszBuffer, ulong wLength);

/**
 *@brief        Write wLength bytes to the open iHandle file.
 *@details
 *@param    iHandle File handles
 *@param    c_pszBuffer    A buffer that needs to be written to the contents of the file
 *@param    wLength    The length of time that needs to be written
 *@return
 *@li   Returns the actual length of data written
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"                The parameter is incorrect(c_pszBuffer == NULL || iHandle <= 0 || wLength == 0)
 *@li   \ref YMI_ERR_FS_WRITE "YMI_ERR_FS_WRITE"      Failed to write file
*/
int YMI_FsWrite(T_Fd iHandle, const char *c_pszBuffer, ulong wLength);

/**
 *@brief        The distance by which the file pointer moves wOffset from eSeek
 *@details
 *@param    iHandle File handles
 *@param    wOffset  The distance the file pointer moves
 *@param    eSeek  The start position of the file pointer
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect(iHandle <= 0)
 *@li   \ref YMI_ERR_FS_SEEK "YMI_ERR_FS_SEEK"      Operation failed
*/
int YMI_FsSeek(T_Fd iHandle, ulong wOffset, EM_FS_SEEK eSeek);

/**
 *@brief        Deletes the specified file
 *@details
 *@param    c_pszName The name of the file to be deleted
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect(c_pszName is NULL)
 *@li   \ref YMI_ERR_FS_DEL "YMI_ERR_FS_DEL"              Operation failed(Failed to call remove())
*/
int YMI_FsDelete(const char *c_pszName);

/**
 *@brief        File length
 *@details
 *@param    c_pszName filename
 *@retval   pwSize File size returns
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect(c_pszName、pwSize is NULL)
 *@li   \ref YMI_ERR_FS_SIZE "YMI_ERR_FS_SIZE"              Operation failed
*/
int YMI_FsFileSize(const char *c_pszName, ulong *pwSize);

/**
 *@brief        Test if the file exists
 *@details
 *@param    c_pszName filename
 *@return
 *@li   YMI_OK              Operation successful(The file exists)
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect(c_pszName is NULL)
 *@li   \ref YMI_ERR_FS_EXIST "YMI_ERR_FS_EXIST"      Operation failed
*/
int YMI_FsExist(const char *c_pszName);

/**
 *@brief        Read the file stream location
 *@details   Used to obtain the current read and write location of the file stream
 *@param    iHandle File handles
 *@retval   pwCursor File stream location
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect(pwCursor == NULL || iHandle <= 0)
 *@li   \ref YMI_ERR_FS_TELL "YMI_ERR_FS_TELL"              Operation failed
*/
int YMI_FsTell(T_Fd iHandle, ulong *pwCursor);

/**
 *@brief        File truncation
 *@details   YMI_FsTruncate() changes the file size specified by parameter c_pszPath to the size specified by parameter wLen. If the original file size is larger than the wLen parameter, the excess will be deleted.
                    If the size of the original file is smaller than wLen, the missing part will be made up for 0xff
 *@param    c_pszPath File path
 *@param    wLen The length to be truncated
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect(c_pszPath is NULL)
 *@li   \ref YMI_ERR_FS_TRUNCATE "YMI_ERR_FS_TRUNCATE"      Operation failed
*/
int YMI_FsTruncate(const char *c_pszPath, ulong wLen);

/**
 *@brief        File rename
 *@details
 *@param    c_pszSrcname Original file name
 *@param    c_pszDstname The name of the target file
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect(pszsSrcname、pszDstname is NULL)
 *@li   \ref YMI_ERR_FS_RENAME "YMI_ERR_FS_RENAME"          Operation failed
*/
int YMI_FsRename(const char *c_pszSrcname, const char *c_pszDstname);

/**
 *@brief        Lists all the files in the specified directory
 *@details      The size of the incoming pszBuf must be large enough, otherwise there will be overflow, and '\0' will be added to the end of each file name
 *@param       c_pszPath Specify the directory to read
 *@retval      pszBuf Storing the file name to the pszBuf always returns
 *@retval      pwNum Returns the total number of files in that folder directory
 *@retval      wBufSize The maximum value of the pszBuf buffer
 *@return       
 *@li    YMI_OK             Operation successful
 *@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter(pszPath == NULL || pszBuf == NULL || pwNum == NULL || wBufSize == 0)
 *@li    \ref YMI_ERR "YMI_ERR"                 The total length of each file name in the directory is greater than the buffer length
 *@li    \ref YMI_ERR_FS_DIR "YMI_ERR_FS_DIR"   Returns when the cumulative length of all file names in the folder directory is greater than w len
*/
int YMI_FsListInDir(char *pszPath,  char *pszBuf, uint *pwNum, uint wBufSize);

/**
 *@brief        Retrieves the usage of disk space in the file system
 *@details
 *@param        eGetType How to get it
 *@retval       pwSpace Returns the amount of disk space
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect(pwSpace为NULL)
*/
int YMI_FsGetDiskSpace(EM_FS_GET_SPACE eGetType, ulong *pwSpace);

/**
 *@brief     Synchronize modified file data from memory to the storage device
 *@param    iHandle File handles
 *
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect (iHandle <= 0)
 *@li   \ref YMI_ERR_FS_SYNC "YMI_ERR_FS_SYNC"              Operation failed
*/
int YMI_FsSync(T_Fd iHandle);

/**
 *@brief    Create a catalog
 *@param    pszPath   Directory path
 *
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect (pszPath == NULL)
 *@li   \ref YMI_ERR_FS_MAKE_DIR "YMI_ERR_FS_MAKE_DIR"              Operation failed
*/
int YMI_FsMakeDir(char *pszPath);

/**
 *@brief    Delete the directory and delete the subdirectories and files in the directory
 *@param    pszPath   Directory path
 *
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        The parameter is incorrect (pszPath == NULL)
 *@li   \ref YMI_ERR_FS_DEL_DIR "YMI_ERR_FS_DEL_DIR"              Operation failed
*/
int YMI_FsDeleteDir(char *pszPath);

/**
 *@brief    Deletes the user files in the file system
 *@param    
 *
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_FS_DEL "YMI_ERR_FS_DEL"    File deletion failed
 *@li   \ref YMI_ERR_FS_DEL_DIR "YMI_ERR_FS_DEL_DIR"  Folder deletion failed
*/
int YMI_FsCleanCustomerFile(void);

/* Watcher API*/
/**
 *@brief        Start the stopwatch and start the timer
 *@details  Used by YMI_SysCreateTimer() in conjunction with YMI_SysCloseTimer(). The accuracy is around 20 milliseconds
 *@return
 *@li    YMI_OK             Operation successful
*/

int YMI_SysCreateTimer(void);

/**
 *@brief        Get the count value
 *@details
 *@retval   pwTime The current count value
 *@return
 *@li    YMI_OK             Operation successful
 *@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"   Illegal parameter(pwTime is NULL)
*/
int YMI_SysReadTimer(uint *pwTime);

/**
 *@brief        Stop running the meter and save the count value
 *@details
 *@retval   pwTime The count value at the end of the stopmeter
 *@return
 *@li    YMI_OK             Operation successful
 *@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"   Illegal parameter(pwTime is NULL)
*/
int YMI_SysCloseTimer(uint *pwTime);

/* Gpio Keys API */
typedef enum{
    K_VOL_DOWN  = 0x01,
    K_VOL_UP    = 0x02,
    K_MENU      = 0x03,
    K_POWER     = 0x0A,

}EM_KEY_CODE;

typedef enum{
    EM_KEY_RELEASE = 0,
    EM_KEY_SHORTPRESS,
    EM_PWRKEY_LONGPRESS,
    EM_SEKEY_HIT,
    EM_KEY_INVALID_STATE = 0xFFFF,
}EM_KEY_STATE;

typedef struct
{
    uint16_t wType;
	EM_KEY_CODE wCode;
	EM_KEY_STATE wValue;
	struct timeval stTime;
} ST_InputEvent;

int YMI_GKbRegister(T_Queue tQueueId);
int YMI_GKbUnregister(void);
int YMI_KbGetValue(ST_InputEvent *pstEvent, int iTimeoutMs);

/* Led API */
typedef enum{
    EM_LED_RED = 0,
    EM_LED_BLUE = 1,
    EM_LED_GREEN = 2,
    EM_LED_ALL = 3,
} EM_LED_COLOR;
typedef enum{
    EM_LED_NAME_STATE = 0,
} EM_LED_NAME;

#define EM_LED_XXX_COMBINE(name, color, on) ((1 << (name)) | ((on) << ((name) + 1)) | ((color) << ((name) + 2)))
typedef enum {
    EM_LED_STATE_ALL_ON     = EM_LED_XXX_COMBINE(EM_LED_NAME_STATE, EM_LED_ALL, 1),
    EM_LED_STATE_ALL_OFF    = EM_LED_XXX_COMBINE(EM_LED_NAME_STATE, EM_LED_ALL, 0),
    EM_LED_STATE_RED_ON     = EM_LED_XXX_COMBINE(EM_LED_NAME_STATE, EM_LED_RED, 1),
    EM_LED_STATE_RED_OFF    = EM_LED_XXX_COMBINE(EM_LED_NAME_STATE, EM_LED_RED, 0),
    EM_LED_STATE_BLUE_ON    = EM_LED_XXX_COMBINE(EM_LED_NAME_STATE, EM_LED_BLUE, 1),
    EM_LED_STATE_BLUE_OFF   = EM_LED_XXX_COMBINE(EM_LED_NAME_STATE, EM_LED_BLUE, 0),
    EM_LED_STATE_GREEN_ON   = EM_LED_XXX_COMBINE(EM_LED_NAME_STATE, EM_LED_GREEN, 1),
    EM_LED_STATE_GREEN_OFF  = EM_LED_XXX_COMBINE(EM_LED_NAME_STATE, EM_LED_GREEN, 0),
} EM_LED_STATE;

/**
 *@brief        Set the LED light status
 *@details
 *@retval   eLedState LED灯状态
 *@return
 *@li    YMI_OK             Operation successful
*/
int YMI_LedSetState(EM_LED_STATE eLedState);

/* Fota API */
/**
 *@brief   Fote Upgrade, determine the upgrade type based on the suffix of the input file name, and restart after the upgrade
 *@param   pszPath Upgrade files
 *@return
 *@li    YMI_OK             Operation successful
 *@li   \ref Other operation failed
 **/
int YMI_Fota(char *pszPath);
/* Audio API */
/** @addtogroup Audio Play
* @{
*/
typedef enum {
    EM_VOL_LEVEL_1 = 40,
    EM_VOL_LEVEL_2 = 50,
    EM_VOL_LEVEL_3 = 60,
    EM_VOL_LEVEL_4 = 70,
    EM_VOL_LEVEL_5 = 80,
    EM_VOL_LEVEL_MAX = EM_VOL_LEVEL_5,
}EM_VOL_LEVEL;
typedef enum
{
	EM_AUD_PLAYER_ERROR = -1,
	EM_AUD_PLAYER_START = 0,	//audio player start play
	EM_AUD_PLAYER_PAUSE,		//audio player has been paused by call or pause api
	EM_AUD_PLAYER_FINISHED,    //audio player finish current playing
	EM_AUD_PLAYER_CLOSE,       //audio player closed
	EM_AUD_PLAYER_RESUME,      //resume player from pause
}EM_AUD_PLAYER_STATE;

typedef int(*CB_ON_PLAYER)(char *pData, int iLen, EM_AUD_PLAYER_STATE eState);
/**
 *@brief    Get the playback volume
 *@details  
 *@return
 *@li    YMI_OK             Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter (eLevel == NULL)
 *@li   \ref YMI_ERR_AUDIO_GET_VOLUME "YMI_ERR_AUDIO_GET_VOLUME"   Failed to get volume
 */
int YMI_AudioGetVolume(EM_VOL_LEVEL *eLevel);

/**
 *@brief    Set the playback volume
 *@details  
 *@return
 *@li    YMI_OK             Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter (eLevel >= EM_VOL_LEVEL_MAX)
 *@li   \ref YMI_ERR_AUDIO_SET_VOLUME "YMI_ERR_AUDIO_SET_VOLUME"   Failed to set the volume
 */
int YMI_AudioSetVolume(EM_VOL_LEVEL eLevel);

typedef enum {
    EM_TTS_TYPE_GBK = 0,      
    EM_TTS_TYPE_UTF8 = 1,      
    EM_TTS_MAX_NUM
}EM_TTS_TYPE;

/**
 *@brief    Play audio data
 *@details  
 *@param    eType        Audio data type
 *@param    pszAudioData Audio strings
 *@param    wLen         The length of the audio data played
 *@return
 *@li       YMI_OK       Operation successful
 *@li       \ref YMI_ERR_AUDIO_TTS_START "YMI_ERR_AUDIO_TTS_START"     Operation failed
 *@li       \ref YMI_ERR_PARA "YMI_ERR_PARA" Illegal parameter(eType >= EM_AUDIO_PLAY_TTS_GBK || pszAudioData == NULL || wLen == 0)
*/
int YMI_AudioTTSStart(EM_TTS_TYPE eType, char *pszAudioData, uint wLen);

/**
 *@brief    Stop playing TTS
 *@details  
 *@return
 *@li       YMI_OK       Operation successful
 *@li       \ref YMI_ERR_AUDIO_PCM_CLOSE "YMI_ERR_AUDIO_PCM_CLOSE"     Operation failed
*/
int YMI_AudioTTSStop(void);

typedef enum {
    EM_FILE_TYPE_MP3 = 0,
    EM_FILE_TYPE_WAV = 1,
    EM_FILE_TYPE_PCM = 2,
    EM_FILE_MAX_NUM
}EM_FILE_TYPE;

/**
*@brief    Play audio data
 *@details  
 *@param    eType        Audio data type(Can only be played EM_AUDIO_PLAY_MP3, EM_AUDIO_PLAY_WAV)
 *@param    pszAudiFile  Audio files
 *@param    cb_Player    When the state in the EM_AUD_PLAYER_STATE enumeration is generated during playback, the callback function is triggered. cb_Player can be null
 *@return
 *@li       YMI_OK       Operation successful
 *@li       \ref YMI_ERR_AUDIO_FILE_START "YMI_ERR_AUDIO_FILE_START"     Operation failed
 *@li       \ref YMI_ERR_PARA "YMI_ERR_PARA" Illegal parameter(eType > EM_AUDIO_PLAY_WAV || eType <EM_AUDIO_PLAY_MP3 || pszAudiFile == NULL)
*/
int YMI_AudioFileStart(EM_FILE_TYPE eType, char *pszAudiFile, CB_ON_PLAYER cb_Player);

/**
 *@brief    Stop playing the local file(EM_AUDIO_PLAY_MP3, EM_AUDIO_PLAY_WAV)
 *@details  
 *@return
 *@li       YMI_OK       Operation successful
 *@li       \ref YMI_ERR_AUDIO_FILE_STOP "YMI_ERR_AUDIO_FILE_STOP"     Operation failed
*/
int YMI_AudioFileStop(void);

/**
 *@brief    Play audio data
 *@details  
 *@param    nSampleRate Sample rate
 *@param    nChannels Number of channels
 *@return
 *@li       YMI_OK       Operation successful
*/
void YMI_AudioPCMComfig(int nSampleRate, int nChannels);

/**
 *@brief    Play audio data
 *@details  
 *@param    ppvHandle The returned PCM handler
 *@return
 *@li       YMI_OK       Operation successful
 *@li       \ref YMI_ERR_AUDIO_PCM_OPEN "YMI_ERR_AUDIO_PCM_OPEN"     Operation failed
*/
int YMI_AudioPCMDataStart(void);

/**
 *@brief    Stop playing PCM
 *@details  
 *@param    pvHandle     PCM handler
 *@return
 *@li       YMI_OK       Operation successful
 *@li       \ref YMI_ERR_AUDIO_PCM_CLOSE "YMI_ERR_AUDIO_PCM_CLOSE"     Operation failed
*/
int YMI_AudioPCMDataStop(void);

/**
 *@details  
 *@param    pszAudioData Audio strings
 *@param    wLen         The length of the audio data played
 *@param    pvHandle     PCM handler
 *@return
 *@li       YMI_OK       Operation successful
 *@li       \ref YMI_ERR_AUDIO_PCM_WRITE "YMI_ERR_AUDIO_PCM_WRITE"      Operation failed
 *@li       \ref YMI_ERR_PARA "YMI_ERR_PARA" Illegal parameter(pszAudioData == NULL || wLen <= 0)
*/
int YMI_AudioPCMPlay(char *pszAudioData, uint wLen);

/**
 *@brief    It is used to wait for the audio playback to complete and return to the audio state
 *@details  
 *@param    iTimeout Wait time
 *@return
 *@li       YMI_OK       Audio playback is complete
 *@li       \ref YMI_ERR_PARA "YMI_ERR_PARA" Illegal parameter (iTimeout <= 0)
 *@li       \ref YMI_ERR_AUDIO_PLAY_BUSY  "YMI_ERR_AUDIO_PLAY_BUSY"    Audio is playing 
 *@li       \ref YMI_ERR_AUDIO_PLAY_ERROR "YMI_ERR_AUDIO_PLAY_ERROR"  Audio playback failed 
*/
int YMI_AudioPlayStatus(int iTimeout);

/** @addtogroup Wireless module
* @{
*/
typedef enum{
    EM_WLM_SIM_STATUS_READY=0,        /**<SIM card has been inserted.*/
    EM_WLM_SIM_STATUS_NOSIM=1,        /**<SIM card not inserted*/
}EM_WLM_SIM_STATUS;
/**
 *@brief  The wireless module returns a status definition
*/

/**
 *@brief  The wireless module CCID,IMSI,IMEI
*/
typedef enum{
    EM_WLM_INFO_CCID=0,        /**<Wireless CCID enumeration*/
    EM_WLM_INFO_IMSI=1,        /**<Wireless IMSI enumeration*/
    EM_WLM_INFO_IMEI=2,        /**<Wireless IMEI enumeration*/
    EM_WLM_INFO_OPERATORS=3,   /**<Get the current carrier information*/
    EM_WLM_INFO_SN=4,          /**<Wireless module SN enumeration*/
}EM_WLM_TYPE_INFO;

/**
 *@brief  Base station information
*/
typedef struct station_info
{
    uint wMCC;  /**<Mobile country number*/
    uint wMNC; /**<Mobile network number*/
    uint wLac;/**<Location area code*/
    uint wCi;/**<Cell number*/
    int idbm;/**<Signal strength*/
}ST_StationInfo;

typedef struct
{
    ST_StationInfo stMainStation;/**<The Gprs module attaches the main base station*/
}ST_GPRSStationInfo;

typedef union
{
    ST_GPRSStationInfo stGPRSStation;  /**<GPRS*/
}UN_ModuleInfo;
typedef enum
{
    EM_MODULE_TYPE_GPRS,       /**<GPRS*/
    EM_MODULE_TYPE_LTE,        /**<4g*/
    EM_MODULE_TYPE_NB,         /**<NB*/
}EM_MODULE_TYPE;

typedef struct
{
    EM_MODULE_TYPE eModuleType;        /**<Module type*/
    UN_ModuleInfo uModuleStationInfo;/**<Module base station information*/
}ST_MobileStationInfo;

typedef enum{
    EM_WLM_REG_NOT_REGISTERED = 0,        /**Not registered, MT is not currently searching for a registered carrier*/
    EM_WLM_REG_HOME_NETWORK,              /**Registered, belonging to the local network*/
    EM_WLM_REG_TRYING_ATTACH_OR_SEARCHING,/**Not registered, MT is currently trying to bet or searching for an operator to bet on the net**/
    EM_WLM_REG_DENIED,                    /**Registration denied*/
    EM_WLM_REG_UNKNOW,                    /**Unknown status*/
    EM_WLM_REG_ROAMING,                   /**Registered, roaming status**/ 
}EM_WLM_REG_STATUS;

/**
*@brief    Hardware reset of the wireless modem
*@return
*@li   YMI_OK              Operation successful
*@li   \ref YMI_ERR_WIRE_SET_MODEM_FUN "YMI_ERR_WIRE_SET_MODEM_FUN"        Failed to open the device file
*/
int YMI_WlModemReset(void);

/**
 *@brief   Get the wireless module version
 *@param    Version   Wireless module version
 *@return
 *@li    YMI_OK             Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"  Illegal parameter (pszVersion == NULL)
 *@li   \ref YMI_ERR_DEV_GET_VERSION "YMI_ERR_DEV_GET_VERSION"     Failed to get
*/
int YMI_WlVersion(char *pszVersion);

/**
 *@brief    Check whether the on-premises network is successful
 *@return
 *@li   YMI_OK  The network was successfully registered
 *@li   \ref YMI_ERR  The on-shore network failed
*/
int YMI_WlResidentStatus(void);

/**
*@brief		Detect SIM card    0 means the SIM card is inserted, 1 means the SIM card is not inserted
*@param    status 		sim Card status
*@li	YMI_OK		    Operation successful,Return to SIM card status
*@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(pnSq is NULL)
*@li    \ref YMI_ERR_WIRE_GET_STATUS "YMI_ERR_WIRE_GET_STATUS"    Operation failed
*/
int YMI_WlCheckSIM(EM_WLM_SIM_STATUS *eStatus);

/**
 *@brief    Obtain wireless MODEM signal strength
 *@retval   pnSq    The strength of the signal obtained, the value taken    0-31 is a success, 99 is unknown, and -1 is a failure
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(pnSq为NULL)
 *@li   \ref YMI_ERR_WIRE_GET_STATUS    Operation failed
*/
int YMI_WlModemGetSQ(int *piCsq);

 /**
 *@brief        Get wireless CCID, IMSI, IMEI, carrier information
 *@param        eType      Enumeration of wireless CCID, IMSI, IMEI, carriers, etc
 *@param        pszValue    Wireless CCID, IMSI, IMEI, carrier information (Mobile: CMCC, Telecom: CT, Unicom: UNICOM)
 *@param        iBufLen     The length of the buffer must be >=21, the total length of the CCID is 20 digits, and it is composed of 0 F, the total length of IMSI is not more than 15, and it is composed of 0 9, and the total length of the IMEI is not more than 20, which is composed of 0 F, and if the operator information is obtained, the length is >=0.
 *@li   YMI_OK             Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter(emType illegal 、pszValue is NULL)
 *@li   \ref YMI_ERR "YMI_ERR"             Operation failed
*/
int YMI_WlGetInfo(EM_WLM_TYPE_INFO eType,char *pszValue, int iBufLen);

/**
 *@brief    Obtain the information of this base station and adjacent base stations, including operators, location area codes, and cell codes

 *@retval   pstStationInfo  Base station information
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter(pstStationInfo为NULL)
*/
int YMI_WlGetStationInfo(ST_MobileStationInfo * pstStationInfo);

/**
 *@brief    Domain name resolution

 *@param   pszDnsIp  IP address resolved
 *@param   pszDnsName  domain name
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter(pszDnsIp，pszDnsName is NULL)
*/
int YMI_NetDnsResolv(char *pszDnsIp, char *pszDnsName);

/**
 *@brief    Obtain the resident status

 *@param   eStatus  Resident status
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter(eStatus为NULL)
*/
int YMI_WlGetResidentStatus(EM_WLM_REG_STATUS *eStatus);

/**
 *@brief    PPP dial-up
 *@param    iProfileIdx PDP index, values 1-7. 
 *@param    pszApnName apnName. When apn is set to NULL, the default value is "CMNET"
 *@param    pszUserName username. The default setting is NULL
 *@param    pszPassword user password. The default setting is NULL
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter(iProfileIdx < 0 || iProfileIdx > 7)
*/
int YMI_PppDial(int iProfileIdx, char *pszApnName, char *pszUserName, char *pszPassword);

/**
 *@brief    PPP Hangup
 *@param  	iProfileIdx PDP index, value 1-7.
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter(iProfileIdx < 0 || iProfileIdx > 7)
*/
int YMI_PppHangup(int iProfileIdx);

/**
 *@brief    Check the PPP dial-up status
 *@param  	pwStatus Success when the status is equal to 1 and failure when it is equal to 0
 *@param  	iProfileIdx PDP index, value 1-7.
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter(pwStatus==NULL, iProfileIdx < 0 || iProfileIdx > 7)
*/
int YMI_PppCheck(uint *pwStatus, int iProfileIdx);

/**
 *@brief    Obtain a local IP address
 *@param  	pszIp The IPv4 address obtained
 *@param  	iProfileIdx PDP index，Valid values: 1-7
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"    Illegal parameter (iFd <= 0 || piValue == NULL)
 *@li   \ref YMI_ERR_WIRE_GET_INFO "YMI_ERR_WIRE_GET_INFO"    Failed to obtain the local IP address
*/
int YMI_PppGetNetAddr(char *pszIp, int iProfileIdx);

/**
 *@brief        Gets the current time of the system
 *@details
 *@param     pstTime  Enter a pointer of the TM struct type to return the current POS time
 *@return
 *@li    YMI_OK             Operation successful
 *@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter(pstTime is NULL)
*/
int YMI_SysGetDevTime(struct tm *pstTime);

/**
 *@brief        Set the current time of the system
 *@details
 *@param     stTime  Enter the variable of the TM struct type, and set the time of the pos time to the variable stTime
 *@return
 *@li    YMI_OK             Operation successful
 *@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter(Illegal stTime)
 *@li    \ref YMI_ERR "YMI_ERR"     Operation failed(Return if the calls to mktime() and stime() fail.)
*/
int YMI_SysSetDevTime(struct tm stTime);

//app api
typedef enum{
	TASK_OPT_EXIT = 3,
}EM_TAKS_OPT;
#define TASK_MANAGE_EVENT_ID (1)
typedef enum{
    TASK_EVENT_ID_APP_START = 0x0100,
    DEV_EVENT_ID_SAVE_RES_START,
    DEV_EVENT_ID_SAVE_RES,
    DEV_EVENT_ID_SAVE_RES_DONE,
    DEV_EVENT_ID_SAVE_RES_FAILS,
    DEV_EVENT_ID_PLAY_ADS,
    DEV_EVENT_ID_STOP_ADS,
    DEV_EVENT_ID_SAVE_OTA_START,
    DEV_EVENT_ID_SAVE_OTA,
    DEV_EVENT_ID_SAVE_OTA_DONE,
    DEV_EVENT_ID_SAVE_OTA_FAILS,
    DEV_EVENT_ID_SAVE_PIC_ZIP_START,
    DEV_EVENT_ID_SAVE_PIC_ZIP,
    DEV_EVENT_ID_SAVE_PIC_ZIP_DONE,
    DEV_EVENT_ID_SAVE_PIC_ZIP_FAILS,

    DEV_EVENT_ID_VIDEO_PLAY,
    DEV_EVENT_ID_VIDEO_STOP,

    TASK_EVENT_ID_APP_MAX = 0xFF00,
}EM_YMI_EVENT_ID;
/**
 *@brief     The APP is ready to exit.
 *@details
 *@param     not
 *@return
 *@li    YMI_OK     Operation successful
 *@li    YMI_ERR    Operation failed
*/
int YMI_AppIsReadyExit(void);
/**
 *@brief     When the APP is to exit, notify other threads to exit as well.
 *@details
 *@param     not
 *@return
 *@li    YMI_OK     Operation successful
 *@li    YMI_ERR    Operation failed
*/
int YMI_AppWantToExit(void);
/**
 *@brief     The APP starts (necessary), notifying the system that the application has entered the background.
 *@details
 *@param     argc Same as the parameters passed to main function
 *@param     argv Same as the parameters passed to main function
 *@return    not
*/
void YMI_AppStart(int argc, char *argv[]);
/**
 *@brief     APP has ended (necessary), notify the system that the application has exited.
 *@details
 *@param     not
 *@return    not
*/
void YMI_AppEnd(void);

/*WiFi*/
typedef int (*CB_WIFI_LOOP_BREAK)(void);

typedef struct 
{
	const char *c_pszSsid;
	const char *c_pszIp;
	const char *c_pszPsk;
	int iTimeout;
	CB_WIFI_LOOP_BREAK cb_APBreak;
} ST_WIFIAPConf;

typedef struct 
{
	const char *c_pszKey;
	int iTimeout;
	CB_WIFI_LOOP_BREAK DirBreak;
} ST_WIFIDirConf;

typedef struct wlan_ssid {
	uint8_t wSsid[WLAN_SSID_MAX_LEN];
	uint8_t wSsidLen;
} ST_WIFISSID;

typedef struct wlan_sta_ap {
	ST_WIFISSID stSsid;
	uint8_t		wBssid[6];
	int		iLevel;	/* signal level, unit is dbm */
} ST_WIFIAPInfo;

typedef enum {
    WIFIRF_OFF ,         
    WIFIRF_ON,           
}EM_WIFIRF;

/**
 *@brief        Turn on WiFi
 *@details
 *@return
 *@li    YMI_OK             Operation successful
 *@li    \ref YMI_ERR_WIFI_OPEN "YMI_ERR_WIFI_OPEN"       Failed to open
*/
int YMI_WiFiOpen(void);

/**
 *@brief       Turn off WiFi
*/
void YMI_WiFiClose(void);

//Set the hotspot name, password
int YMI_WiFiStaSet(char const *c_pszSsid, char const * c_pszKey);

//Query status
int YMI_WiFiQueStat(int *piStat);
//Time synchronization via IP, NTP
int YMI_WiFiSntpTime(char const *c_pszHost);

/**
 *@brief        Domain name resolution, which allows you to obtain an IP address from a domain name
 *@details
 *@param     c_pszName  domain name
 *@param     piAddr  IP address resolved
 *@return
 *@li    YMI_OK             Operation successful
 *@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter (piAddr == NULL || c_pszName == NULL)
 *@li    \ref YMI_ERR_WIFI_GET_HOST "YMI_ERR_WIFI_GET_HOST"     Operation failed
*/
int YMI_WiFiGetHostName(char const *c_pszName, int *piAddr);

//ap information
int YMI_WiFiGetApInfo(ST_WIFIAPInfo *stApInfo);

int YMI_WiFiSetRF(EM_WIFIRF emRF);
//WiFi signal strength
int YMI_WiFiGetApLevel(int *piLevel);

//WiFi MAC address
int YMI_WiFiReadMac(char *pszMac, uint wState);

//wifi 
int YMI_WiFiSelfUpdate(char const * c_pszUrl);

//WiFi connect to a hotspot (AP)
int YMI_WiFiConAp(ST_WIFIAPConf stWiFiAp, char *pszSsid, char *pszPsk);

//wifi connect to a hotspot (airkiss)
int YMI_WiFiDirect(ST_WIFIDirConf stWiFiDir, char *pszSsid, char *pszPsk);

//wifi sokcet
int YMI_WiFiSocketConnect(int *piFd, const char *c_pszHost,int iPort,int iProto);
int YMI_WiFiSocketRead(int iIdx, unsigned char *c_pszBuffer, ulong ulBufLen, uint wTimeout);
int YMI_WiFiSocketWrite(int iIdx, const char *c_pszBuffer, ulong ulLen, uint wTimeout);
int YMI_WiFiSocketFree(int iIdx);

/* NVM API */
/**
 *@brief     Write to the configuration file
 *@details  
 *@param    pvData            Data to be written
 *@param    wDataSize         The number of blocks to be written

 *@return
 *@li       >0                    The number of bytes successfully written
 *@li       YMI_ERR_NVM_WRITE     Operation failed
 *@li       YMI_ERR_PARA Illegal parameter
*/
int YMI_NvmWrite(void *pvData, uint wDataSize);
/**
 *@brief     Read the configuration file.
 *@details  
 *@param    pvData            The data read
 *@param    wDataSize         The number of blocks to be read

 *@return
 *@li       >0                   The number of bytes that were successfully read.
 *@li       YMI_ERR_NVM_READ     Operation failed
 *@li       YMI_ERR_PARA Illegal parameter
*/
int YMI_NvmRead(void *pvData, uint wDataSize);

/* Display API */
typedef enum {
    BACKLIGHT_OFF = 0,     /**<Turn off the LCD backlight*/
    BACKLIGHT_ON  = 1,     /**<Turn on the LCD backlight*/
} EM_BACKLIGHT;

typedef enum {
    FONT_TEXTCOLOR = 0,    /**<The font color when the font is displayed normally*/
	FONT_BACKCOLOR,	       /**<The background color when the font is displayed normally*/
}EM_FONT_COLOR;

/**
 *@brief    Display the given image data within a specified area
 *@param    wX         The abscissa of the start point of the image (in pixels)
 *@param    wY         The start ordinate of the image in pixels
 *@param    wWidth     The width of the image in pixels
 *@param    wHeight    The height of the image (in pixels)
 *@param    pucBuffer   Image data  <0-0xFFFF>
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_DISP_IMAGE "YMI_ERR_DISP_IMAGE"      Operation failed
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"      Illegal parameter 
        (wWidth == 0，wHeight == 0，pwBuffer == NULL，the width (wWidth) and height (wHeight) exceed the width or height of the LCD， The coordinates (wX+wWidth, wY+wHeight) exceed the width or height of the LCD)
*/
int YMI_DispBlockUpdate(uint wX, uint wY, uint wWidth, uint wHeight, uchar * pucBuffer);

/**
 *@brief    Switch backlight operation.
 *@param    eBackLight  BACKLIGHT_OFF Turn off the LCD backlight
                        BACKLIGHT_ON Turn on the LCD backlight
 *@return
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"      Illegal parameter (eBackLight > BACKLIGHT_ON)
 *@li   YMI_OK              Operation successful
*/
int YMI_DispSetBackLight(EM_BACKLIGHT eBackLight);

/**
 *@brief    Clear the screen and set the full-screen color
 *@param    wBGColor The color of the setting
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_DISP_CLEAR "YMI_ERR_DISP_CLEAR"      Operation failed
*/
int YMI_DispClrs(uint16_t wBGColor);

/**
 *@brief    Set the font color and background color
 *@param    wFontColor Font color
 *@param    wBGColor   Background color
 *@return
 *@li   YMI_OK              Operation successful
*/
int YMI_DispSetBitMapColor(uint16_t wFontColor, uint16_t wBGColor);

/**
 *@brief    The Bitmap image is displayed in the display area.
 *@details  Bitmap format: 1 byte corresponds to 8 pixels. 0 represents white pixels and 1 represents black pixels. The display data is arranged horizontally as shown in the following figure:\n
-----------------D7~~D0--------------D7~~D0------------------\n
Byte 1: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte2 \n
Byte 3: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte4 \n
Byte 5: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte6 \n
Byte 7: ○ ○ ● ● ● ● ● ●  ●  ● ● ● ● ● ○ ○ Byte8 \n
Byte 9: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte10    \n
Byte11: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte12    \n
Byte13: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte14    \n
Byte15: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte16    \n
---------------------------------------------------------------\n
 *@param    x               The abscissa of the image in the upper left corner of the display area (unit: pixels)
 *@param    y               The ordinate of the image in the upper left corner of the display area (unit: pixels)
 *@param    width           Image width (in pixels)
 *@param    height          Image height (in pixels)
 *@param    dot_buffer      Bitmap image data
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_DISP_IMAGE "YMI_ERR_DISP_IMAGE"      Operation failed
 *@li   \ref YMI_ERR_MEM_MALLOC "YMI_ERR_MEM_MALLOC"      Out of memory
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter 
        (wWidth == 0，wHeight ==0, pacBitMap == NULL，the width (wWidth) and height (wHeight) exceed the width or height of the LCD， The coordinates (wX+wWidth, wY+wHeight) exceed the width or height of the LCD)
*/
int YMI_DispBitMap(uint wX, uint wY, uint wWidth, uint wHeight, char *pacBitMap);

/**
 *@brief    Images in JPG format are displayed in the display area
 *@param    c_pszFilePath   The path where the image file is located.
 *@param    wX              The abscissa of the upper left corner of the image (in pixels)
 *@param    wY              The upper-left ordinate of the image display (in pixels)
 *@param    wWidth          The width of the image (in pixels)
 *@param    wHeight         The height of the image display (in pixels)
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_DISP_IMAGE "YMI_ERR_DISP_IMAGE"      Operation failed
 *@li   \ref YMI_ERR_DISP_JPG_TO_BMP "YMI_ERR_DISP_JPG_TO_BMP"      JPG to RGB failed
 *@li   \ref YMI_ERR_FS_ILLEGAL_NAME "YMI_ERR_FS_ILLEGAL_NAME"      The file path is incorrect
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter 
        (wWidth == 0，wHeight ==0, c_pszFilePath == NULL，the width (wWidth) and height (wHeight) exceed the width or height of the LCD， The coordinates (wX+wWidth, wY+wHeight) exceed the width or height of the LCD)
*/
int YMI_DispJPG(const char * c_pszFilePath, uint wX, uint wY, uint wWidth, uint wHeight);

/**
 *@brief    Get the width and height of the LCD screen.
 *@param    pwWidth   LCD screen width (unit: pixels)
 *@param    pwHeight  LCD screen height (unit: pixels)      
 *@return
 *@li   YMI_OK              Operation successful
*/
int YMI_DispGetScreenSize(uint *pwWidth, uint *pwHeight);

/**
 *@brief    Get the width and height of the image in JPG format.
 *@param    c_pszFilePath   The path where the image file is located.
 *@param    pwWidth   Image width (in pixels)
 *@param    pwHeight  Image height (in pixels)      
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_DISP_GET_JPG_INFO "YMI_ERR_DISP_GET_JPG_INFO"      Operation failed
 *@li   \ref YMI_ERR_FS_ILLEGAL_NAME "YMI_ERR_FS_ILLEGAL_NAME"      The file path is incorrect
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter 
        (c_pszFilePath == NULL || pwWidth == NULL || pwHeight == NULL)
*/
int YMI_DispGetJPGInfo(const char * c_pszFilePath, uint * pwWidth, uint * pwHeight);

/**
 *@brief    JPG to RGB image
 *@param    c_pszFilePath   : The path of the image file must be absolute
 *@retval   pwWidth  : wide
 *@retval   pwHeight : high
 *@retval   pacRGBBuffer : RGB data buffer
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_DISP_JPG_TO_BMP "YMI_ERR_DISP_JPG_TO_BMP"      Operation failed
 *@li   \ref YMI_ERR_FS_ILLEGAL_NAME "YMI_ERR_FS_ILLEGAL_NAME"      The file path is incorrect
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter 
        (c_pszFilePath == NULL || pwWidth == 0 || pwHeight == 0 || pacRGBBuffer == NULL)
*/
int YMI_DispJPG2RGB565(const char * c_pszFilePath, uint pwWidth, uint pwHeight, char * pacRGBBuffer);

/**
 *@brief    Displays the string.
 *@param    wX Displays the abscissa of the string position
 *@param    wY Displays the ordinate of the string position
 *@param    pszStr    The string pointer to display
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_DISP_IMAGE "YMI_ERR_DISP_IMAGE"      Operation failed
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter(wX and wY exceed the width or height of the LCD panel, pszStr == NULL）
*/
int YMI_DispString(uint wX, uint wY, char *pszStr);


 /**
 *@brief    Gets the current pixel cursor position
 *@retval   pwX Returns the abscissa of the cursor position in pixels
 *@retval   pwY Returns the ordinate of the cursor position in pixels
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter
*/
int YMI_DispGetCursor(uint *pwX, uint *pwY);

/**
 *@brief    The screen shows the formatted output, which can be used in the same way as printf
 *@param    c_pszFormat    The format of the parameter output
  *@return
 *@li   YMI_OK              Operation successful
*/
int YMI_DispPrintf(const char *c_pszFormat, ...);

/**
 *@brief    Set the font and its background color
 *@param    wColor    color
 *@param    eType     Font/Background Refer to "EM_FONT_COLOR"
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Illegal parameter
*/
int YMI_DispSetFontColor(uint16_t wColor, EM_FONT_COLOR eType);

/* MQTT API */
#define MQTT_TOPIC_BUFF_MAX_SIZE (256)
#define MQTT_SUB_TOPIC_MAX_SIZE 6
#define MQTT_PUB_TOPIC_MAX_SIZE 5

typedef void (*CB_MQTT_SUB_RECV)(char *pszTopic, uchar *pbyRecvData, uint wRecvDataLen);
typedef	int (*CB_MQTT_PUB_MSG_PACK)(char *pszOutData);
typedef void (*CB_MQTT_PROCESS)(void *pvParam);
typedef int32_t (* CB_MQTT_STATE_LOG)(int32_t iCode, char *pszMsg);
typedef enum{
	MQTT_CON_TYPE_TLS,
	MQTT_CON_TYPE_TCP,
}EM_MQTT_CON_TYPE;
typedef enum {
    SOCKET_TCP_CLIENT,
    SOCKET_TCP_SERVER,
    SOCKET_UDP_CLIENT,
    SOCKET_UDP_SERVER,
    SOCKET_WIFI_TCP_CLIENT,
    SOCKET_WIFI_UDP_CLIENT,
} EM_SOCKET_TYPE;

typedef struct {
	bool bIsOta;
	char szSubTopic[MQTT_TOPIC_BUFF_MAX_SIZE];
	CB_MQTT_SUB_RECV cb_SubRecv;
    int iSubRet;
}ST_MQTTSubConf;
typedef struct {
	char szPubTopic[MQTT_TOPIC_BUFF_MAX_SIZE];
	CB_MQTT_PUB_MSG_PACK cb_PubMsgPack;
    int iPubRet;
}ST_MQTTPubConf;
typedef struct 
{
	char szUrl[128];
	char szProductKey[64];//Product Key
	char szDeviceName[64];//The name of the device
	char szDeviceSecret[64];//Device key
	char szClientId[64];
    char szUserName[64];
    char szPassWord[1024];
    char szOTASavePath[64];
	int iSubNum;
	ST_MQTTSubConf astSubList[MQTT_SUB_TOPIC_MAX_SIZE];
	int iPubNum;
	ST_MQTTPubConf astPubList[MQTT_PUB_TOPIC_MAX_SIZE];
	EM_MQTT_CON_TYPE eConType;
	int iPort;
	int iConnectTimeOut;
	EM_SOCKET_TYPE eSocketType;
	bool isP2P;
	int iHeartBeatIntervalTime;//Heartbeat transmission interval time unit ms
	int iHeartBeatLostMaxTime;//The number of heartbeats lost
	uint8_t ucCleanSession;
	uint16_t wNoSSLVerify;
	int iKeepAlive;
    const char *c_pszServerCert;
    int iServerCertLen;
    const char *c_pszClientCert;
    int iClientCertLen;
    const char *c_pszClientPrivkey;
    int iClientPrivkeyLen;
    CB_MQTT_STATE_LOG cb_MQTTStateLog;//Log output callbacks
    CB_MQTT_PROCESS cb_MQTTConnetSucc;//MQTT connection successful
    CB_MQTT_PROCESS cb_MQTTConnetErr;//MQTT connection failed
    CB_MQTT_PROCESS cb_MQTTLostHeartBeat;//MQTT heartbeat lost
    CB_MQTT_PROCESS cb_MQTTReconnet;//MQTT reconnection
    CB_MQTT_PROCESS cb_OTAStart;//"ota start" -> "The Ota (Over-the-Air) update has begun. Return the start time of the upgrade in the format of "yyyy-mm-dd hh-mm-ss"."
    CB_MQTT_PROCESS cb_OTASucc;//OTA upgrade was successful
    CB_MQTT_PROCESS cb_OTAFail;//OTA upgrade failed
}ST_MQTTConf;
typedef struct 
{
	char szTopic[MQTT_TOPIC_BUFF_MAX_SIZE];
	char *pszData; 
	uint wDataLen;
}ST_MQTTPubEventInfo;

/**
 *@brief    The MQTT processing flow is invoked when creating the MQTT thread
 *@param  	pvParam MQTT configuration parameters（ST_MQTTConf）
 *@return
 *@li       not
*/
void YMI_MQTTProce(void *pvParam);

/**
 *@brief    Specify the MQTT thread to exit
 *@param  	tTaskID MQTT task ID
 *@return
 *@li       YMI_OK Operation successful
 *@li       YMI_ERR_PARA Illegal parameter
*/
int YMI_MQTTExit(T_Task tTaskID);

/**
 *@brief    Specify the MQTT thread for PUB events
 *@param  	tTaskID MQTT task ID
 *@param  	pstInfo PUB Incident Related Information
 *@return
 *@li       YMI_OK Operation successful
 *@li       YMI_ERR_PARA Illegal parameter
*/
int YMI_MQTTPubMsg(T_Task tTaskID, ST_MQTTPubEventInfo *pstInfo);

/**
 *@brief    HTTP communication
 *@param  	eSocketType Socket type, according to the communication type, mainly divided into LTE and WiFi, according to the type of equipment divided into server and terminal (general terminal)
 *@param  	pszUrl The connection address
 *@param  	pszHead The content of the HTTP header information. If it is empty, it will be assigned according to the format of the sent content (currently supported formats are JSON, and non-JSON: x-www-form-urlencode).
 *@param  	pbyInData When sending data via HTTP, if the data is empty, it will use the "GET" method; otherwise, it will use the "POST" method.
 *@param  	wInDataLen The length of the http send data. If it is 0, it will use the "GET" method; if it is greater than 0, it will use the "POST" method.
 *@param  	pbyOutData Returns the data receiving cache
 *@param  	wOutDataMaxSize The size of the received return data cache
 *@param  	wConnectTimeoutMS Connection timeout period. If it is 0, the default value of 10 seconds will be used
 *@param  	wRecvTimeoutMS The receive timeout period. If it is 0, the default value of 5 seconds will be used
 *@return   Returns the length of the received data
 *@li       YMI_ERR_PARA Illegal parameter
 *@li       YMI_ERR_HTTP_PARSE_URL Illegal URL
 *@li       YMI_ERR_HTTP_SYSDEP_GET_PORTFILE Failed to obtain the underlying parameters
 *@li       YMI_ERR_HTTP_CORE_INIT Initialization failed
 *@li       YMI_ERR_HTTP_CORE_SET_OPT Failed to set the parameters
 *@li       YMI_ERR_HTTP_CORE_CONNECT The connection failed
 *@li       YMI_ERR_HTTP_CORE_SEND Failed to send
 *@li       YMI_ERR_HTTP_RET_CONTENT Returns a data exception
 *@li       YMI_ERR_HTTP_RET_CONTENT_CODE Returns a code other than 200
 *@li       YMI_ERR_HTTP_RET_CONTENT_OVERSIZE The returned data exceeds the cache size
*/
int YMI_HttpCommu(EM_SOCKET_TYPE eSocketType, char *pszUrl, char *pszHead, 
    uchar *pbyInData, uint wInDataLen, uchar *pbyOutData, uint wOutDataMaxSize, 
    uint wConnectTimeoutMS, uint wRecvTimeoutMS);
int YMI_HttpCommuLKL(EM_SOCKET_TYPE eSocketType, char *pszUrl, char *pszHead, 
    uchar *pbyInData, uint wInDataLen, uchar *pbyOutData, uint wOutDataMaxSize, uint wConnectTimeoutMS, uint wRecvTimeoutMS);
/**
 *@brief    HTTP download file
 *@param  	eSocketType Socket type, according to the communication type, mainly divided into LTE and WiFi, according to the type of equipment divided into server and terminal (general terminal)
 *@param  	pszUrl The connection address
 *@param  	wFileSize The size of the file to be downloaded, if it is 0, will download the file according to the returned content length
 *@param  	pszSaveFile The path where the file is saved
 *@return
 *@li       YMI_OK Operation successful
 *@li       YMI_ERR_PARA Illegal parameter
 *@li       YMI_ERR_HTTP_DL_INIT Initialization failed
 *@li       YMI_ERR_HTTP_DL_SET_OPT Failed to set the parameters
 *@li       YMI_ERR_HTTP_DL_SEND_REQ Failed to send a download request
 *@li       YMI_ERR_HTTP_DL_SAVE Save failed
 *@li       YMI_ERR_HTTP_DL_RECV Received failed
*/
int YMI_HttpDownLoadFile(EM_SOCKET_TYPE eSocketType, char *pszUrl, uint wFileSize, char *pszSaveFile);

/* Flash API */
/**
*@brief    Erase off-chip flash data
*@param    address  The start address of the Flash
*@param    length   The length of the erased data
*@return
*@li    YMI_OK             Operation successful
*@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter (length == 0) 
*@li    \ref YMI_ERR_FLASH_ERASE "YMI_ERR_FLASH_ERASE"    Erase failed
*/
int YMI_SpiFlashErase(uint wAddress, size_t ullLength);

/**
*@brief    Off-chip flash writes data
*@param    address  The start address of the Flash
*@param    buffer   Data written
*@param    length   The length of the data written
*@return
*@li    YMI_OK             Operation successful
*@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter (buffer == NULL || length == 0)
*@li    \ref YMI_ERR_FLASH_WRITE "YMI_ERR_FLASH_WRITE"    Write failed
*/
int YMI_SpiFlashWrite(uint wAddress, char * pszBuffer, size_t ullLength);

/**
*@brief    Off-chip flash reads data
*@param    address  The start address of the Flash
*@param    buffer   The data read
*@param    length   The length of the data read
*@return
*@li    YMI_OK             Operation successful
*@li    \ref YMI_ERR_PARA "YMI_ERR_PARA"       Illegal parameter (buffer == NULL || length == 0)
*@li    \ref YMI_ERR_FLASH_READ "YMI_ERR_FLASH_READ"    Read failed
*/
int YMI_SpiFlashRead(uint wAddress, char * pszBuffer, size_t ullLength);

/**
 *@brief   Obtain the device security code
 *@param    pwCode   Device security code
 *@return
 *@li    YMI_OK             Operation successful
 *@li   \ref YMI_ERR_SLAVE_GET_TAMPER_CODE  Operation failed
*/
int YMI_PciGetTamperCode(uint32_t *pwCode);

typedef enum {
    EM_TAMPER_STATUS_NONE = 0,
    EM_TAMPER_STATUS_ATTACK = 1,
} EM_TAMPER_STATUS;

/**
 *@brief    Obtain the security status of the device
 *@param    eStatus   Device security status
 *@return
 *@li    YMI_OK             Operation successful
 *@li   \ref YMI_ERR_PARA   Illegal parameters(eStatus == NULL)
 *@li   \ref YMI_ERR_NOT_SUPPORT   Features are not supported
*/
int YMI_PciGetTamperStatus(EM_TAMPER_STATUS * eStatus);

/**
 *@brief  Fetch the device information index
*/
typedef enum {
    DEV_INFO_TYPE = 0,              /** <Device model */
    DEV_INFO_HARDWARE,              /** <Device hardware module configuration*/
    DEV_INFO_SN,                    /** <Device Serial Number (SN) */
    DEV_INFO_SE_VERSION,            /** <Device SE version information>*/
    DEV_INFO_AP_ALL_VER,            /** <AP version information>*/
    DEV_INFO_MAX,
} EM_DEVICE_INFO;
int YMI_SysSetDevInfo(EM_DEVICE_INFO eInfo, const char * c_pszBuffer);
int YMI_SysGetDevInfo(EM_DEVICE_INFO eInfo, uint * pwLen, char * pcBuffer);

/**
 *@brief  HID API
*/
int YMI_HIDOpen(void);
int YMI_HIDClose(void);
int YMI_HIDRead(uint wRequestReadLen, char *pcRecvBuffer, uint wMsTimeout, int *piActualReadLen);
int YMI_HIDWrite(uint wRequestSendLen, const char *c_pcSendBuffer, uint wMsTimeout, int *piActualSendLen);
int YMI_HIDClearRecvBuffer(void);

typedef void *T_ZH;//zip handle
/**
 *@brief        Create zip file
 *@details
 *@param    c_pszFileName file name
 *@return
 *@li    hz  The operation returns the zip descriptor successfully
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"        Error parameters(File name is null, mode is incorrect)
 *@li   \ref YMI_ERR_ZIP_CREATE_FAILS "YMI_ERR_FS_OPEN"        the zip file creation failed
*/
T_ZH YMI_ZipCreate(const char *c_pszFileName);

/**
 *@brief        Add file to zip
 *@details
 *@param    iHandle the zip handle
 *@param    c_pszFileName Added file name
 *@return
 *@li   YMI_OK                  The operation returns successfully
 *@li   \ref YMI_ERR_PARA Error parameters(File name is null, mode is incorrect)
 *@li   \ref YMI_ERR_FS_ILLEGAL_NAME The file name format is incorrect
 *@li   \ref YMI_ERR_ZIP_ADD_FILE_FAILS File addition failed
*/
int YMI_ZipAddFile(T_ZH tHandle, const char *c_pszFileName);

/**
 *@brief        close zip file
 *@details
 *@param    iHandle the zip handle
 *@return
 *@li   YMI_OK                  The operation returns successfully
 *@li   \ref YMI_ERR_PARA Error parameters
 *@li   \ref YMI_ZIP_CLOSE_FAILS the zip file close failed
*/
int YMI_ZipClose(T_ZH tHandle);

/**
 *@brief        Unzip zip file
 *@details
 *@param    c_pszFileName zip file name
 *@param    pszUnzipPath Unzip the file path
 *@return
 *@li   YMI_OK                  The operation returns successfully
 *@li   \ref YMI_ERR_PARA Error parameters
 *@li   \ref YMI_ERR_FS_ILLEGAL_NAME The file name format is incorrect
 *@li   \ref YMI_ERR_ZIP_UNZIP_FAILS Zip decompression failed
*/
int YMI_ZipUnzip(const char *c_pszFileName, char *pszUnzipPath);

/* Seg LCD API */
typedef enum {
    SEGLCD_MASTER_LCD = 0,   /* Home screen   wStatus : 0. Not shown   1. Displays incoming string data   pszString : Display data */
    SEGLCD_SLAVE_LCD = 1,    /* Secondary screen   wStatus : 0. Not shown   1. Displays incoming string data   pszString : Display data */
    SEGLCD_LITTLE_NUM = 2,   /* Small   wStatus : 0. Not shown   1. Displays incoming string data   pszString : Display data */
    SEGLCD_CURRENCY = 3,    /* Monetary unit wStatus : 0. Not shown  1. Display */
    SEGLCD_CHARGING = 4,    /* Charging icon wStatus : 0. Not shown  1. Display */
    SEGLCD_BATTERY_CELLS = 5,  /* Battery   wStatus : 0. Not shown   1~4. Indicates 1~4 grids of power 0xFF. Indicates 0 bar power */
    SEGLCD_LTE_TYPE = 6,    /* Wireless type wStatus : 0. Not shown  1. 4G  2. 4G*   */
    SEGLCD_LTE_SIGNAL = 7,   /* Wireless signal wStatus : 0. Not shown  1~4. It represents a signal of 1~4 bars  */
    SEGLCD_WIFI_CONNET = 8,   /* WiFi connection wStatus : 0. Not shown  1. ·  2. ·*    */
    SEGLCD_WIFI_SIGNAL = 9,   /* WiFi signal wStatus : 0. Not shown  1~3. Indicates 1~3 grid signals */
    SEGLCD_SPEAKER = 10,    /* Horn   wStatus : 0. Not shown  1. Display       */
    SEGLCD_VOLUME = 11,     /* Volume   wStatus : 0. Not shown  1~5. Indicates 1~5 bars of volume */
    SEGLCD_ALL_ICON = 12,    /* All icons  wStatus ： 0. Not shown  1. Display  */
} EM_SEGLCD_ICON_TYPE;

typedef enum {
    /* Used to control the icon Display/Not shown */
    SEGLCD_ICON_OFF = 0,
    SEGLCD_ICON_ON = 1,

    /* Icons for multi-grid statuses, such as: Battery level, Wireless signal, WiFi signal, Wolume */
    SEGLCD_ICON_CELL_0 = 0xFF,
    SEGLCD_ICON_CELL_1 = 1,
    SEGLCD_ICON_CELL_2 = 2,
    SEGLCD_ICON_CELL_3 = 3,
    SEGLCD_ICON_CELL_4 = 4,
    SEGLCD_ICON_CELL_5 = 5,

    /* Used for wireless type icons */
    SEGLCD_ICON_LTE_4G = 1,
    SEGLCD_ICON_LTE_4G_DISCONNECT = 2,

    /* Icon for WiFi connection */
    SEGLCD_ICON_WIFI = 1,
    SEGLCD_ICON_WIFI_DISCONNECT = 2,
} EM_SEGLCD_ICON_STATE;

/**
*@brief  Segment code screen display
*@param  eSLCDIcon    Refer to "EM_SEGLCD_ICON_TYPE"
*@param  wStatus     Refer to the description of "EM_SEGLCD_ICON_STATE"
*@param  pszString    
*@return
*@li  YMI_OK       Operation successful
*@li  \ref YMI_ERR_PARA "YMI_ERR_PARA"   Illegal parameter 
*/
int YMI_SegLCDShow(EM_SEGLCD_ICON_TYPE eSLCDIcon, EM_SEGLCD_ICON_STATE eStatus, char * pszString);

/**
 *@brief    Segment screen backlight control
 *@param    eBackLight       Refer to"EM_BACKLIGHT"
 *@return
 *@li   YMI_OK              Operation successful
 *@li   \ref YMI_ERR_PARA "YMI_ERR_PARA"      Illegal parameter 
*/
int YMI_SegLCDSetBackLight(EM_BACKLIGHT eBackLight);

/* video play API */

#define VIDEO_PLAY_ON_LOOP (0xFFFFFFFF)

typedef enum{
    VIDEO_PLAY_STATUS_STOP,
    VIDEO_PLAY_STATUS_PLAYING,
}EM_VIDEO_PLAY_STATUS;


int YMI_VideoPlayInit(void);
int YMI_VideoPlayDeinit(void);
int YMI_VideoPlayStart(int iX, int iY, uint wWidth, uint wHeight, char *pszFileName, uint wTimes);
int YMI_VideoPlayUpdateWindows(int iX, int iY, uint wWidth, uint wHeight);
EM_VIDEO_PLAY_STATUS YMI_VideoPlayGetStatus(void);
int YMI_VideoPlayStop(void);
#endif
