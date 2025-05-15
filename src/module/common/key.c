/**********************************************************************
* Copyright (C)    Dspread
* File name: key.c
* Content summary: Key-related APIs
*
***********************************************************************/
//Header file
#include "var.h"

//Macro definitions
#define KEY_LOG_MODE
#ifdef KEY_LOG_MODE
#define KEY_LOG(msg, ...) LOG("[KEY]"msg"", ##__VA_ARGS__)
#else
#define KEY_LOG(msg, ...)
#endif

#define KEY_TASK_STACK_SIZE (5 * 1024)
#define KEY_TASK_EVENT_NUM (1)
#define KEY_LONGPRESS_MODE
#define KEY_QUEUE_NUM (1)
#define KEY_QUEUE_SIZE (sizeof(ST_InputEvent))
#define KEY_QUEUE_READ_TIMEOUT_MS (200)
#define KEY_LONG_PRESS_TIMEOUT_MS (3 * 1000)
#define KEY_TASK_EVENT_WAIT_TIMEOUT_MS (100)

#define KEY_CODE_VOL_DOWN (K_VOL_DOWN)
#define KEY_CODE_VOL_UP (K_VOL_UP)
#define KEY_CODE_CONF (K_MENU)
#define KEY_CODE_PWRKEY (K_POWER)
#define KEY_CODE_UNKNOWN (0xFE)
#define KEY_VALUE_PRESS (EM_KEY_SHORTPRESS)
#define KEY_VALUE_RELEASE (EM_KEY_RELEASE)
typedef enum {
   POWERKEY_SHUTDOWN_START = (1 | (0x8211 << 16)),
   POWERKEY_PRESS,
   POWERKEY_RELEASE,
   POWERKEY_LONGPRESS,//press over 3s return this value
}EM_PWRKEY_EVENT_ID;
//Constant
//Local variables
static T_Task s_tKeyTaskID = 0;
//Global variables
//Function implementation
static int KeyShortPressProc(uint wKeyCode)
{
	int iRet = RET_SUCC;

	KEY_LOG("key short press code=%d", wKeyCode);
	switch (wKeyCode)
	{
	case KEY_CODE_VOL_DOWN:
		AudioPlay(AUDIO_PRIORITY_LOWEST, FILE_TEXT, 0, 0, NULL, 1, "volume down");
		break;
	case KEY_CODE_VOL_UP:
		AudioPlay(AUDIO_PRIORITY_LOWEST, FILE_TEXT, 0, 0, NULL, 1, "volume up");
		break;
	case KEY_CODE_CONF:
		break;
	case KEY_CODE_PWRKEY:
		break;
	
	default:
		break;
	}
	
	return iRet;

}
static int KeyLongPressProc(uint wKeyCode)
{
	int iRet = RET_SUCC;

	KEY_LOG("key long press code=%d", wKeyCode);
	switch (wKeyCode)
	{
	case KEY_CODE_VOL_DOWN:
		break;
	case KEY_CODE_VOL_UP:
		break;
	case KEY_CODE_CONF:
		break;
	case KEY_CODE_PWRKEY:
		iRet = RET_EXIT_APP;
		break;
	
	default:
		break;
	}
	
	return iRet;
}


void KeyTask(void *pvParam)
{
	int iRet;
	int64_t llTimeStart = 0, llTimeEnd = 1;
	uint wKeyCodeBK = KEY_CODE_UNKNOWN;
	ST_InputEvent stKeyInput = {0};
	ST_Event stEvent;

	memset(&stKeyInput, 0x00, sizeof(ST_InputEvent));
	while(1)
    {
		if(YMI_KbGetValue(&stKeyInput, KEY_QUEUE_READ_TIMEOUT_MS) == YMI_OK)
        {
			if((stKeyInput.wValue == KEY_VALUE_PRESS) && llTimeStart == 0)//when last key get release then set next key
			{
				llTimeStart = YMI_SysGetTick();
				llTimeEnd = 0;
				wKeyCodeBK = stKeyInput.wCode;
			}
			else if((stKeyInput.wValue == KEY_VALUE_RELEASE) && stKeyInput.wCode == wKeyCodeBK)//avoid press other key
			{
				llTimeEnd = YMI_SysGetTick();
			}
			// else if(stKeyInput.wValue == POWERKEY_LONGPRESS)//test by tangq
			// {
			// 	iRet = RET_EXIT_APP;
			// 	break;
			// }
            KEY_LOG("get key=%d, value=%d, %lld, %lld", stKeyInput.wCode, stKeyInput.wValue, llTimeStart, YMI_SysGetTick());
        }
		if(YMI_EventWait(&stEvent, KEY_TASK_EVENT_WAIT_TIMEOUT_MS) == YMI_OK)
		{
			if(stEvent.wId == TASK_MANAGE_EVENT_ID && stEvent.wParam1 == TASK_OPT_EXIT)
			{
				break;
			}
		}
		//new key press time cnt
		if(llTimeEnd == 0)
		{
			// long press time cnt
			if(YMI_SysGetTick() - llTimeStart >= KEY_LONG_PRESS_TIMEOUT_MS)
			{
				llTimeEnd = YMI_SysGetTick();
				KEY_LOG("key press over 3s end=%lld", llTimeEnd);
			}
		}
		if(llTimeEnd > 0 && wKeyCodeBK != KEY_CODE_UNKNOWN)
		{
			if(llTimeEnd - llTimeStart >= KEY_LONG_PRESS_TIMEOUT_MS)
			{
				iRet = KeyLongPressProc(stKeyInput.wCode);
			}
			else
			{
				iRet = KeyShortPressProc(stKeyInput.wCode);
			}
			wKeyCodeBK = KEY_CODE_UNKNOWN;
			llTimeStart = 0;
		}
		if(iRet == RET_EXIT_APP)
		{
			YMI_AppWantToExit();
		}
    }
	KEY_LOG("============>out key task");
	YMI_TaskDelete(s_tKeyTaskID);
}

int KeyTaskInit(void)
{
	int iRet;

	iRet = YMI_TaskCreat(&s_tKeyTaskID, "key_task", KEY_TASK_STACK_SIZE, KeyTask, NULL, EM_TASK_PRIORITY_BELOW_HIGH, KEY_TASK_EVENT_NUM);
	if(iRet != YMI_OK)
	{
		KEY_LOG("[ERR]task creat ret=%d", iRet);
		return YMI_ERR;
	}
	return YMI_OK;
}


//end of file