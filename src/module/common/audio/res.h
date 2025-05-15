#ifndef _RES_H_
#define _RES_H_
#if 0
typedef struct {
	const char *c_pszContent;
    const char *c_pszFileName;
}ST_RES_INFO;

// #define AUDIO_RESOURCE_ROOT_PATH_WAV	"wav/"

int ResGetExistLocalFilePath(char const *c_pszContent, void *pszPath, int iPathMaxLen);
int ResCheckWavFile(char const *c_pszContent, void *pvOutBuf, int iOutBufSize);
#endif
#endif