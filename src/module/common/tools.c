/**********************************************************************
* Copyright (C)    Dspread
* File name: tool.c
* Content Summary: General Utility API
*
***********************************************************************/
//Header file
#include "var.h"

//Macro definitions
#define TOOLS_LOG_MODE
#ifdef TOOLS_LOG_MODE
#define TOOLS_LOG(pbyMsg, ...) LOG("[TOOLS]"pbyMsg"", ##__VA_ARGS__)
#else
#define TOOLS_LOG(pbyMsg, ...)
#endif

#define HEX_TO_CHAR
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))


//Constant

//Local variables

//Global variables
static const uint sc_wK[64] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

// r specifies the per-round shift amounts
static const uint sc_wR[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                      5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                      4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                      6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

//Function implementation
uchar HexToAsc(uchar ucHex)
{
    uchar ucAsc;

	ucHex = ucHex & 0x0F;
    if (ucHex < 10)
    {
        ucAsc = ucHex + '0';
    }
    else
    {
        ucAsc = ucHex - 10 + 'A';
    }
    return ucAsc;
}
uchar AscToHex(uchar ucAsc)
{
	if (ucAsc <= '9' && ucAsc >= '0')
	{
		return (ucAsc - '0');
	}
	if (ucAsc <= 'f' && ucAsc >= 'a')
	{
		return ((ucAsc - 'a') + 10);
	}
	if (ucAsc <= 'F' && ucAsc >= 'A')
	{
		return ((ucAsc - 'A') + 10);
	}
    return ucAsc;
}
uint HexToStr(uchar *pucHex, uint wLen, char *pszStr)
{
    int i = 0;
	if(pszStr == NULL || pucHex == NULL || wLen == 0)
	{
		return 0;
	}
    for(i = 0; i < wLen; i++)
    {
        pszStr[2 * i] = HexToAsc((pucHex[i] >> 4) & 0x0F);
        pszStr[2 * i + 1] = HexToAsc(pucHex[i] & 0x0F);
    }
    return wLen * 2;
}

void ToBytes(uint wVal, uchar *pbyBytes)
{
    pbyBytes[0] = (uchar) wVal;
    pbyBytes[1] = (uchar) (wVal >> 8);
    pbyBytes[2] = (uchar) (wVal >> 16);
    pbyBytes[3] = (uchar) (wVal >> 24);
}

uint ToInt32(const uchar *c_pbyBytes)
{
    return (uint) c_pbyBytes[0]
        | ((uint) c_pbyBytes[1] << 8)
        | ((uint) c_pbyBytes[2] << 16)
        | ((uint) c_pbyBytes[3] << 24);
}

void Md5(const uchar *c_byInitalMsg, uint wInitialLen, uchar *pbyDigest)
{
    // These vars will contain the hash
    uint wH0, wH1, wH2, wH3;
 
    // Message (to prepare)
    uchar *pbyMsg = NULL;
 
    uint wNewLen, wOffset;
    uint wW[16];
    uint wA, wB, wC, wD, wI, wF, wG, wTmp;
 
    // Initialize variables - simple count in nibbles:
    wH0 = 0x67452301;
    wH1 = 0xefcdab89;
    wH2 = 0x98badcfe;
    wH3 = 0x10325476;
 
    //Pre-processing:
    //append "1" bit to message    
    //append "0" bits until message length in bits ≡ 448 (mod 512)
    //append length mod (2^64) to message
 
    for (wNewLen = wInitialLen + 1; wNewLen % (512/8) != 448/8; wNewLen++)
        ;
 
    pbyMsg = (uchar*)YMI_SysMalloc(wNewLen + 8);
    if(pbyMsg == NULL){
        return;
    }
    memcpy(pbyMsg, c_byInitalMsg, wInitialLen);
    pbyMsg[wInitialLen] = 0x80; // append the "1" bit; most significant bit is "first"
    for (wOffset = wInitialLen + 1; wOffset < wNewLen; wOffset++)
        pbyMsg[wOffset] = 0; // append "0" bits
 
    // append the len in bits at the end of the buffer.
    ToBytes(wInitialLen*8, pbyMsg + wNewLen);
    // wInitialLen>>29 == wInitialLen*8>>32, but avoids overflow.
    ToBytes(wInitialLen>>29, pbyMsg + wNewLen + 4);
 
    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    for(wOffset=0; wOffset<wNewLen; wOffset += (512/8)) {
 
        // break chunk into sixteen 32-bit words wW[j], 0 ≤ j ≤ 15
        for (wI = 0; wI < 16; wI++)
            wW[wI] = ToInt32(pbyMsg + wOffset + wI*4);
 
        // Initialize hash value for this chunk:
        wA = wH0;
        wB = wH1;
        wC = wH2;
        wD = wH3;
 
        // Main loop:
        for(wI = 0; wI<64; wI++) {
            if (wI < 16) {
                wF = (wB & wC) | ((~wB) & wD);
                wG = wI;
            } else if (wI < 32) {
                wF = (wD & wB) | ((~wD) & wC);
                wG = (5*wI + 1) % 16;
            } else if (wI < 48) {
                wF = wB ^ wC ^ wD;
                wG = (3*wI + 5) % 16;          
            } else {
                wF = wC ^ (wB | (~wD));
                wG = (7*wI) % 16;
            }
            wTmp = wD;
            wD = wC;
            wC = wB;
            wB = wB + LEFTROTATE((wA + wF + sc_wK[wI] + wW[wG]), sc_wR[wI]);
            wA = wTmp;
        }
        // Add this chunk's hash to result so far:
        wH0 += wA;
        wH1 += wB;
        wH2 += wC;
        wH3 += wD;
    }
 
    // cleanup
    FREE(pbyMsg);
 
    //var char pbyDigest[16] := wH0 append wH1 append wH2 append wH3 //(Output is in little-endian)
    ToBytes(wH0, pbyDigest);
    ToBytes(wH1, pbyDigest + 4);
    ToBytes(wH2, pbyDigest + 8);
    ToBytes(wH3, pbyDigest + 12);
}

void Md5Encrypt(uchar *pbySrcbuf, uint wLen, uchar byDesbuf[16])
{
    Md5((const uchar *)pbySrcbuf, wLen, byDesbuf);
}

int UnpackIntNumber(void *pvInBuff, void *pvOutBuff)
{
	if(pvInBuff == NULL || pvOutBuff == NULL)
	 	return RET_FAIL;

	if(*(double *)pvInBuff >= MAX_INT)
	{
		return RET_FAIL;
	}
	*(int *)pvOutBuff = *(int *)pvInBuff;
	return RET_SUCC;
}

int UnpackString(void *pvInBuff, void *pvOutBuff)
{
	if(pvInBuff == NULL || pvOutBuff == NULL)
	 	return RET_FAIL;
	if(strlen((char *)pvInBuff) == 0)//Non essential data returns a null value and exits
		return RET_SUCC;
	if(strlen((char *)pvOutBuff))
	{
		if(strcmp((char *)pvInBuff, (char *)pvOutBuff))
		{
            TOOLS_LOG("[ERR]UnpackString data not same, %s!=%s", (char *)pvInBuff, (char *)pvOutBuff);
            return RET_FAIL;
		}
		return RET_SUCC;
	}
	sprintf((char *)pvOutBuff, "%s", (char *)pvInBuff);
	return RET_SUCC;
}

int UnpackNumber(void *pvInBuff, void *pvOutBuff)
{
	if(pvInBuff == NULL || pvOutBuff == NULL)
	 	return RET_FAIL;

	if(*(double *)pvInBuff >= MAX_INT)
	{
		*(double *)pvOutBuff = *(double *)pvInBuff;
		return RET_SUCC;
	}
	*(int *)pvOutBuff = *(int *)pvInBuff;
	return RET_SUCC;
}

int UnpackObject(void *pvInBuff, void *pvOutBuff)
{
	ST_MyJson *pstMyJson;
	if(pvInBuff == NULL || pvOutBuff == NULL)
	{
		return RET_FAIL;
	}
	pstMyJson = (ST_MyJson *)pvInBuff;
    *(ST_MyJson **)pvOutBuff = pstMyJson;
	return RET_SUCC;
}

int MyJsonCreate(ST_MyJson **ppstMyJson)
{
    ST_MyJson *pstMyJson = NULL;
    if (ppstMyJson == NULL)
    {
        TOOLS_LOG("[ERR]%s param NULL!", __func__);
        return YMI_ERR_PARA;
    }

    pstMyJson = (ST_MyJson *)cJSON_CreateObject();
    if (pstMyJson == NULL)
    {
        TOOLS_LOG("[ERR]create json fail!");
        return YMI_ERR_JSON_CREATE;
    }

    *(ST_MyJson **)ppstMyJson = pstMyJson;
    return YMI_OK;
}

int MyJsonCreateArray(ST_MyJson **ppstMyJson)
{
    ST_MyJson *pstMyJson = NULL;
    if (ppstMyJson == NULL)
    {
        TOOLS_LOG("[ERR]%s param NULL!", __func__);
        return YMI_ERR_PARA;
    }

    pstMyJson = (ST_MyJson *)cJSON_CreateArray();
    if (pstMyJson == NULL)
    {
        TOOLS_LOG("[ERR]create json fail!");
        return YMI_ERR_JSON_CREATE;
    }

    *(ST_MyJson **)ppstMyJson = pstMyJson;
    return YMI_OK;
}

int MyJsonDelete(ST_MyJson *pstMyJson)
{
    if (pstMyJson == NULL)
    {
        TOOLS_LOG("[ERR]%s param NULL!", __func__);
        return YMI_ERR_PARA;
    }

    cJSON_Delete((cJSON *)pstMyJson);
    return YMI_OK;
}

int MyJsonParse(void *pvJsonStr, ST_MyJson **ppstMyJson)
{
    if ((pvJsonStr == NULL) || (ppstMyJson == NULL))
    {
        TOOLS_LOG("[ERR]%s param NULL!", __func__);
        return YMI_ERR_PARA;
    }
    
    *ppstMyJson = (ST_MyJson *)cJSON_Parse(pvJsonStr);
    if (*ppstMyJson == NULL)
    {
        TOOLS_LOG("[ERR]json parse fail!");
        return YMI_ERR_JSON_PARSE;
    }
    return YMI_OK;
}

int MyJsonPrint(ST_MyJson *pstMyJson, char **ppszJsonStr)
{
    if ((ppszJsonStr == NULL) || (pstMyJson == NULL))
    {
        TOOLS_LOG("[ERR]%s param NULL!", __func__);
        return YMI_ERR_PARA;
    }
    
    *ppszJsonStr = cJSON_PrintUnformatted((cJSON *)pstMyJson);
    if (*ppszJsonStr == NULL)
    {
        TOOLS_LOG("[ERR]json print fail!");
        return YMI_ERR_JSON_PRINT;
    }

    return YMI_OK;
}

int JsonUnPack(ST_MyJson *pstMyJson, ST_JsonUnpackInfo *pstUnpackInfo)
{
    bool bEnterUnpackFunc = TRUE;
	void *pvTmp;
    cJSON *pcJSON = (cJSON *)pstMyJson;
	cJSON *pcJSONTmp;
    ST_JsonArrayUnpackInfo stArrayInfo = {0};
	if (pstMyJson == NULL || pstUnpackInfo == NULL)
		return YMI_ERR_PARA;
	while (1)
	{
		if (pstUnpackInfo->pszField == NULL)
		{
            break;
		}
		pcJSONTmp = cJSON_GetObjectItem(pcJSON, pstUnpackInfo->pszField);
		if (pcJSONTmp != NULL && pcJSONTmp->type != MYJSON_NULL)//Whether it is empty or another type, it is not to be judged if it is empty
		{
			if (pcJSONTmp->type != pstUnpackInfo->cType)
			{
                TOOLS_LOG("%s type:%d!=%d", pstUnpackInfo->pszField, pcJSONTmp->type, pstUnpackInfo->cType);
                return YMI_ERR_JSON_TYPE_NO_SAME;
			}
			switch(pcJSONTmp->type)
			{
			case MYJSON_STRING:
				pvTmp = pcJSONTmp->valuestring;
				TOOLS_LOG("json type=string, unpack:%s=%s", pstUnpackInfo->pszField, (char *)pvTmp);
				break;
			case MYJSON_NUMBER:
				if (pcJSONTmp->valuedouble >= MAX_INT)
				{
					pvTmp = &pcJSONTmp->valuedouble;
					TOOLS_LOG("json type=num double, unpack:%s=%.0f", pstUnpackInfo->pszField, *(double *)pvTmp);
				}
				else
				{
					pvTmp = &pcJSONTmp->valueint;
					TOOLS_LOG("json type=num, unpack:%s=%d", pstUnpackInfo->pszField, *(int *)pvTmp);
				}
				break;
			case MYJSON_OBJUECT:
				pvTmp = pcJSONTmp;
				TOOLS_LOG("json type=object, unpack:%s", pstUnpackInfo->pszField);
				break;
            case MYJSON_ARRAY:
            case MYJSON_ARRAYITEM:
                pvTmp = (void *)&stArrayInfo;
                stArrayInfo.iArrayCnt = cJSON_GetArraySize(pcJSONTmp);
                for (int i = 0; i < stArrayInfo.iArrayCnt; i++)
                {
                    stArrayInfo.iArrayOff = i;
                    stArrayInfo.pstMyJson = (ST_MyJson *)cJSON_GetArrayItem(pcJSONTmp, i);
                    if (stArrayInfo.pstMyJson != NULL)
                    {
                        if (pstUnpackInfo->cb_UnpackFunc != NULL)
                        {
                            if (pstUnpackInfo->cb_UnpackFunc(pvTmp, pstUnpackInfo->pvValue) != YMI_OK)
                            {
                                TOOLS_LOG("[ERR]unpack:%s,func err", pstUnpackInfo->pszField);
                                return YMI_ERR_JSON_UNPACK_FUNC;
                            }
                        }
                        TOOLS_LOG("json type=array/arrayitem, unpack:%s,%d", pstUnpackInfo->pszField, i);
                    }
                }
                bEnterUnpackFunc = FALSE;
                break;
			default:
                TOOLS_LOG("undefine joson type%d", pcJSONTmp->type);
				return YMI_ERR_JSON_TYPE_NO_FOUND;
			}
			if (bEnterUnpackFunc && (pstUnpackInfo->cb_UnpackFunc != NULL))
			{
                if (pstUnpackInfo->cb_UnpackFunc(pvTmp, pstUnpackInfo->pvValue) != YMI_OK)
				{
					TOOLS_LOG("[ERR]unpack:%s,func err", pstUnpackInfo->pszField);
					return YMI_ERR_JSON_UNPACK_FUNC;
				}
			}
		}
        else
        {

        }
		pstUnpackInfo ++;

    }
	return YMI_OK;
}

int JsonPack(ST_MyJson **ppstMyJson, ST_JsonPackInfo *pstPackInfo)
{
    int i;
    ST_JsonPackInfo *atPackInfo = pstPackInfo;
    cJSON **ppcJSON = (cJSON **)ppstMyJson;

    if ((ppstMyJson == NULL) || (pstPackInfo == NULL))
    {
        return YMI_ERR_PARA;
    }

	i = 0;
	while(atPackInfo[i].pszField != NULL)
	{
		if (atPackInfo[i].cb_PackFunc != NULL)
		{
			if (atPackInfo[i].cb_PackFunc(atPackInfo[i].pvInValue, atPackInfo[i].pvOutValue) != YMI_OK)
			{
				TOOLS_LOG("[ERR]%s file=%s cb_PackFunc err", __FUNCTION__, atPackInfo[i].pszField);
				return YMI_ERR_JSON_PACK_FUNC;
			}
		}
		if (atPackInfo[i].cType == MYJSON_NUMBER)
		{
            if (*(int *)atPackInfo[i].pvOutValue > MAX_INT)
            {
                cJSON_AddNumberToObject(*ppcJSON, atPackInfo[i].pszField, *(double *)atPackInfo[i].pvOutValue);
				TOOLS_LOG("double number pack:%s=%f", atPackInfo[i].pszField, *(double *)atPackInfo[i].pvOutValue);
            }
            else
            {
                cJSON_AddNumberToObject(*ppcJSON, atPackInfo[i].pszField, *(int *)atPackInfo[i].pvOutValue);
				TOOLS_LOG("number pack:%s=%d", atPackInfo[i].pszField, *(int *)atPackInfo[i].pvOutValue);
            }
		}
		else if (atPackInfo[i].cType == MYJSON_STRING && (char *)atPackInfo[i].pvOutValue && strlen((char *)atPackInfo[i].pvOutValue))
		{
			cJSON_AddStringToObject(*ppcJSON, atPackInfo[i].pszField, atPackInfo[i].pvOutValue);
			TOOLS_LOG("string pack:%s=%s", atPackInfo[i].pszField, (char *)atPackInfo[i].pvOutValue);
		}
        else if ((atPackInfo[i].cType == MYJSON_OBJUECT || atPackInfo[i].cType == MYJSON_ARRAY) && *(cJSON **)atPackInfo[i].pvOutValue != NULL)
		{
			cJSON_AddItemToObject(*ppcJSON, atPackInfo[i].pszField, *(cJSON **)atPackInfo[i].pvOutValue);
			TOOLS_LOG("object pack:%s=", atPackInfo[i].pszField);
		}
        else if (atPackInfo[i].cType == MYJSON_ARRAYITEM && *(cJSON **)atPackInfo[i].pvOutValue != NULL)
		{
			cJSON_AddItemToArray(*ppcJSON, *(cJSON **)atPackInfo[i].pvOutValue);
			TOOLS_LOG("array pack:%s=", atPackInfo[i].pszField);
		}
		i ++;
	}

    return YMI_OK;
}

int MyJsonPack(ST_JsonPackInfo *pstPackInfo, CB_SETSING_FUNC cb_SetSign, ST_MyJson **ppstOutMyJSON, char *pszOutJsonStr, int iOutSize)
{
    int iRet = YMI_ERR;
    char *pszStr = NULL;
    ST_MyJson *pstMyJson = NULL;

    if (pstPackInfo == NULL)
    {
        return YMI_ERR_PARA;
    }

    iRet = MyJsonCreate(&pstMyJson);
    if (iRet != YMI_OK)
    {
        return iRet;
    }

    iRet = JsonPack(&pstMyJson, pstPackInfo);
    if (iRet != YMI_OK)
    {
        MyJsonDelete(pstMyJson);
        return iRet;
    }

    if (cb_SetSign != NULL)
    {
        // if (cb_SetSign() != YMI_OK)
        // {
            // return YMI_ERR_JSON_SIGN;
        // }
    }

    if ((pszOutJsonStr != NULL) && iOutSize)
	{
        int iRet = MyJsonPrint(pstMyJson, &pszStr);
        if ((iRet != YMI_OK) || (pszStr == NULL))
        {
            return YMI_ERR_JSON_PRINT;
        }
        if (strlen(pszStr) >= iOutSize)
        {
            YMI_SysFree(pszStr);
            return YMI_ERR_JSON_UNPACK_OUTBUFF;
        }
        sprintf((char *)pszOutJsonStr, "%s", pszStr);
        YMI_SysFree(pszStr);
	}

    if (ppstOutMyJSON != NULL)
    {
        *ppstOutMyJSON = (ST_MyJson *)pstMyJson;
    }
    else
    {
        MyJsonDelete(pstMyJson);
    }
    return YMI_OK;
}

int MyJsonPackArray(ST_JsonPackInfo *pstPackInfo, CB_SETSING_FUNC cb_SetSign, ST_MyJson **ppstOutMyJSON, char *pszOutJsonStr, int iOutSize)
{
    int iRet = YMI_ERR;
    char *pszStr = NULL;
    ST_MyJson *pcMyArrayJson = NULL;

    if (pstPackInfo == NULL)
    {
        return YMI_ERR_PARA;
    }

    if ((ppstOutMyJSON) && (*ppstOutMyJSON != NULL))
    {
        pcMyArrayJson = *ppstOutMyJSON;
    }
    else
    {
        iRet = MyJsonCreateArray(&pcMyArrayJson);
        if (iRet != YMI_OK)
        {
            return YMI_ERR_JSON_CREATE;
        }
    }

    iRet = JsonPack(&pcMyArrayJson, pstPackInfo);
    if (iRet != YMI_OK)
    {
        MyJsonDelete(pcMyArrayJson);
        return iRet;
    }

    if (cb_SetSign != NULL)
    {
        // if (cb_SetSign() != YMI_OK)
        // {
            // return YMI_ERR_JSON_SIGN;
        // }
    }

    if ((pszOutJsonStr != NULL) && iOutSize)
	{
        int iRet = MyJsonPrint(pcMyArrayJson, &pszStr);
        if ((iRet != YMI_OK) || (pszStr == NULL))
        {
            return YMI_ERR_JSON_PRINT;
        }
        if (strlen(pszStr) >= iOutSize)
        {
            YMI_SysFree(pszStr);
            return YMI_ERR_JSON_UNPACK_OUTBUFF;
        }
        sprintf((char *)pszOutJsonStr, "%s", pszStr);
        YMI_SysFree(pszStr);
	}

    if (ppstOutMyJSON != NULL)
    {
        *ppstOutMyJSON = (ST_MyJson *)pcMyArrayJson;
    }
    else
    {
        MyJsonDelete(pcMyArrayJson);
    }

    return YMI_OK;
}

int MyJsonUnPack(ST_MyJson *pstMyJson, ST_JsonUnpackInfo *pstUnpackInfo)
{
    int iRet = YMI_ERR;

    if ((pstMyJson == NULL) || (pstUnpackInfo == NULL))
    {
        return YMI_ERR_PARA;
    }

    iRet = JsonUnPack(pstMyJson, pstUnpackInfo);
    if (iRet != YMI_OK)
    {
        return iRet;
    }

    return YMI_OK;
}

int MyJsonGetValueWithKey(ST_MyJson *pstMyJson, void *pszKey, ST_MyJson **ppstOutMyJson)
{
    int iRet = YMI_ERR;
    int iArraySize = 0, i = 0;
    ST_MyJson *pstMyJsonItem = NULL;

    if ((pszKey == NULL) || (ppstOutMyJson == NULL) || !strlen(pszKey))
    {
        return YMI_ERR_PARA;
    }
    
    iArraySize = cJSON_GetArraySize((cJSON *)pstMyJson);
    TOOLS_LOG("%s json item size = %d", pstMyJson->pszKeyString, iArraySize);
    for (i = 0; i < iArraySize; i++)
    {
        pstMyJsonItem = (ST_MyJson *)cJSON_GetArrayItem((cJSON *)pstMyJson, i);
        if (pstMyJsonItem == NULL)
        {
            continue;
        }
        TOOLS_LOG("%s type 0x%04x", pstMyJsonItem->pszKeyString, pstMyJsonItem->iType);
        
        if (pstMyJsonItem->pszKeyString && (strlen(pszKey) == strlen(pstMyJsonItem->pszKeyString)) && !memcmp(pstMyJsonItem->pszKeyString, pszKey, strlen(pszKey)))
        {
            TOOLS_LOG("fond key %s! type 0x%04x, value %s, %d, %f", pszKey, pstMyJsonItem->iType, pstMyJsonItem->pszValueString, pstMyJsonItem->iValueint, pstMyJsonItem->dValuedouble);
            *ppstOutMyJson = pstMyJsonItem;
            break;
        }

        if ((pstMyJsonItem->iType == MYJSON_ARRAY) || (pstMyJsonItem->iType == MYJSON_ARRAYITEM) || (pstMyJsonItem->iType == MYJSON_OBJUECT))
        {
            iRet = MyJsonGetValueWithKey(pstMyJsonItem, pszKey, ppstOutMyJson);
            if (iRet == YMI_OK)
            {
                break;
            }
        }
    }

    if (i >= iArraySize)
    {
        TOOLS_LOG("[ERR]no fond key %s", pszKey);
        return YMI_ERR_JSON_KEY_NOFOUND;
    }
    
    return YMI_OK;
}


//end of file
