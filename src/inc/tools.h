#ifndef _TOOLS_H
#define _TOOLS_H

#include "YMAPI.h"

typedef enum {
    MYJSON_INVALID   =  (0),
    MYJSON_FALSE     =  (1 << 0),
    MYJSON_TRUE      =  (1 << 1),
    MYJSON_NULL      =  (1 << 2),
    MYJSON_NUMBER    =  (1 << 3),
    MYJSON_STRING    =  (1 << 4),
    MYJSON_ARRAY     =  (1 << 5),
    MYJSON_ARRAYITEM =  (1 << 6),
    MYJSON_OBJUECT   =  (1 << 7),
    MYJSON_RAW       =  (1 << 8),
}EN_MY_JSON_TYPE;

typedef struct MyJson
{
    struct ST_MyJson *pstNext;
    struct ST_MyJson *pstPrev;
    struct ST_MyJson *pszChild;
    int iType;
    char *pszValueString;
    int iValueint;
    double dValuedouble;
    char *pszKeyString;
}ST_MyJson;

typedef int (* CB_PACK_FUNC)(void *pvInBuff, void *pvOutBuff);
typedef int (* CB_SETSING_FUNC)(void *pvInBuff, void *pvOutBuff);
typedef struct{
    char *pszField;
    char cType;
	void *pvInValue;
    void *pvOutValue;
	CB_PACK_FUNC cb_PackFunc;
}ST_JsonPackInfo;

typedef int (* CB_UNPACK_FUNC)(void *pvInBuff, void *pvOutBuff);
typedef struct{
	char *pszField;
    char cType;
	void *pvValue;
	CB_UNPACK_FUNC cb_UnpackFunc;
}ST_JsonUnpackInfo;

typedef struct {
    int iArrayCnt;
    int iArrayOff;
    ST_MyJson *pstMyJson;
}ST_JsonArrayUnpackInfo;


uchar AscToHex(uchar ucAsc);
uint HexToStr(uchar *pucHex, uint wLen, char *pszStr);
void Md5Encrypt(uchar *pbySrcbuf, uint wLen, uchar byDesbuf[16]);
int UnpackIntNumber(void *pvInBuff, void *pvOutBuff);
int UnpackString(void *pvInBuff, void *pvOutBuff);
int UnpackNumber(void *pvInBuff, void *pvOutBuff);
int UnpackObject(void *pvInBuff, void *pvOutBuff);
unsigned char * base64_encode(const unsigned char *src, int len, int *out_len);
unsigned char *aes_cbc_encrypt(const unsigned char *data, int in_len, unsigned int *out_len, const unsigned char *key, const unsigned char *iv);
int MyJsonCreate(ST_MyJson **ppstMyJson);
int MyJsonDelete(ST_MyJson *pstMyJson);
int MyJsonParse(void *pvMyJsonStr, ST_MyJson **ppstMyJson);
int MyJsonPrint(ST_MyJson *pstMyJson, char **ppszJsonStr);
int MyJsonPack(ST_JsonPackInfo *pstPackInfo, CB_SETSING_FUNC cb_SetSign, ST_MyJson **ppstOutMyJSON, char *pszOutJsonStr, int iOutSize);
int MyJsonPackArray(ST_JsonPackInfo *pstPackInfo, CB_SETSING_FUNC cb_SetSign, ST_MyJson **ppstOutMyJSON, char *pszOutJsonStr, int iOutSize);
int MyJsonUnPack(ST_MyJson *pstMyJson, ST_JsonUnpackInfo *ptUnPackInfo);
int MyJsonGetValueWithKey(ST_MyJson *pstMyJson, void *pszKey, ST_MyJson **ppstOutMyJson);

#endif
//end of file