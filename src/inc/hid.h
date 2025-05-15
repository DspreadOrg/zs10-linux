#ifndef _HID_H_
#define _HID_H_

#include "YMAPI.h"

typedef enum{
	DOWNLOAD_FILE_TYPE_UNKNOWN = 0,
	DOWNLOAD_FILE_TYPE_OTA,
	DOWNLOAD_FILE_TYPE_RES,
}EM_DOWNLOAD_FILA_TYPE;

#define RES_FILE_HEAD_CODE "\x01\x00\x26\xA7"
#define RES_FILE_HEAD_CODE_SIZE (4)


int HIDTaskInit(void);


#endif