/**********************************************************************
* Copyright (C)    Dspread
* The name of the file：ui_api.c
* Summary of content：UI API
*
***********************************************************************/
//Header file
#include "var.h"
#include "ui_lvgl.h"

//Macro definitions
#define UI_LOG_MODE
#ifdef UI_LOG_MODE
#define UI_LOG(msg, ...) LOG("[UI]"msg"", ##__VA_ARGS__)
#endif

#define UI_TASK_STACK_SIZE (24 * 1024)
#define UI_TASK_EVENT_NUM (10)

//Constant

//Local variables
static T_Task s_tUITaskID = 0;
//Global variables

//Function implementation


int UITaskInit(void)
{
	int iRet;

	iRet = YMI_TaskCreat(&s_tUITaskID, "ui_task", UI_TASK_STACK_SIZE, UITaskHandle, &s_tUITaskID, EM_TASK_PRIORITY_NORMAL, UI_TASK_EVENT_NUM);
	if(iRet != YMI_OK)
	{
		UI_LOG("[ERR]ui task creat ret=%d", iRet);
		return -1;
	}
	return 0;
}

//end of file