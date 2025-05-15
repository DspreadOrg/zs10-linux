#ifndef _OTA_H_
#define _OTA_H_

#define OTA_OPT_EXTRA_VERIFY_CRC32      1
#define OTA_OPT_EXTRA_VERIFY_MD5        1
#define OTA_OPT_EXTRA_VERIFY_SHA1       1
#define OTA_OPT_EXTRA_VERIFY_SHA256     1

typedef enum ota_verify {
	OTA_VERIFY_NONE		= 0,
#if OTA_OPT_EXTRA_VERIFY_CRC32
	OTA_VERIFY_CRC32	= 1,
#endif
#if OTA_OPT_EXTRA_VERIFY_MD5
	OTA_VERIFY_MD5		= 2,
#endif
#if OTA_OPT_EXTRA_VERIFY_SHA1
	OTA_VERIFY_SHA1		= 3,
#endif
#if OTA_OPT_EXTRA_VERIFY_SHA256
	OTA_VERIFY_SHA256	= 4,
#endif
} ota_verify_t;

typedef struct {
	uint32_t		size;
	char 			*version;
	char 			*url;
	ota_verify_t	verify;
	char			*verify_value;
}ota_params;


int OtaTaskInit(char *pszFileName);
int OtaDown(void *pvOtaParam);
void OTAFileReset(void);
int UnzipFileTask(void *pvUnzip);
void PicZipFileReset(void);
#endif