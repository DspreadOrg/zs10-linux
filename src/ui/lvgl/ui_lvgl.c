/**********************************************************************
* Copyright (C)    yunmazhineng
* The name of the file：ui_lvgl.c
* Summary of content：UI interface based on lvgl
*
***********************************************************************/
//Header file
#include "var.h"
#include "YMAPI.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include <unistd.h>
// #include "lv_port_fs.h"
// #include "lv_timer.h"

//Macro definitions
#define UI_LV_LOG_MODE
#ifdef UI_LV_LOG_MODE
#define UI_LV_LOG(msg, ...) LOG("[UILV]"msg"", ##__VA_ARGS__)
#endif
#define UI_QUEUE_SIZE (sizeof(ST_UIConf))
#define UI_QUEUE_NUM (20)

#define BIG_FONT_FILE_PATH "EFS:/AlibabaPuHuiTi-3-55-Regular.ttf"
#define NORMAL_FONT_FILE_PATH "EFS:/AlibabaPuHuiTi-small.ttf"
#define BIG_FONT_SIZE (60)
#define NORMAL_FONT_SIZE (20)
#define LV_COLOR_BLACK lv_color_make(0x00, 0x00, 0x00)
#define LV_COLOR_WHITE lv_color_make(0xFF, 0xFF, 0xFF)
//Constant


//Local variables
static T_Queue s_tUIQueue = NULL;
static lv_obj_t *s_pstMainPad = NULL;
static lv_obj_t *s_pstPic = NULL;
static lv_obj_t *s_pstExtraPic = NULL;
static lv_obj_t *s_pstTextPad = NULL;
static lv_obj_t *s_pstQRCodePad = NULL;
static lv_obj_t *s_pstTextLabel = NULL;
static lv_obj_t *s_pstOtherPad = NULL;
static lv_obj_t *s_pstOtherLabel = NULL;
static lv_obj_t *s_pstQRCode = NULL;

// static lv_style_t s_stBigFontStyle;
static lv_style_t s_stNorFontStyle;
static lv_style_t s_stSmallFontStyle;

//Global variables
//Function implementation

static lv_color_t LVColorSet(uint32_t wCol)
{
    uint8_t tR = 0, tG = 0, tB = 0;
    tR = (wCol >> 16) & 0xFF;
    tG = (wCol >> 8) & 0xFF;
    tB = wCol & 0xFF;
    UI_LV_LOG("tR = 0x%x, tG = 0x%x, tR = 0x%x", tR, tG, tB);
    return lv_color_make(tR, tG, tB);
}

void LVLog(const char * buf)
{
    UI_LV_LOG("%s", buf);
}

int LVQueueInit(void)
{
	int iRet;
	iRet = YMI_QueueCreat(&s_tUIQueue, UI_QUEUE_SIZE, UI_QUEUE_NUM);
	if(iRet != YMI_OK)
	{
		UI_LV_LOG("[ERR]ui queue creat ret=%d", iRet);
		return RET_FAIL;
	}
	return iRet;
}
void LVQueueDel(void)
{
	if(s_tUIQueue)
	{
		YMI_QueueDelete(s_tUIQueue);
	}
}
void LVQueueRead(void)
{
	int iRet;

	ST_UIConf stUI;

	memset(&stUI, 0x00, UI_QUEUE_SIZE);
	iRet = YMI_QueueRead(s_tUIQueue, (uint8_t *)&stUI, UI_QUEUE_SIZE, 10);
	if(iRet != YMI_OK)
	{
		return;
	}
	// UI_LV_LOG("lv queue read type=%d", stUI.eID);
	switch (stUI.eID)
	{
	case UI_CREATE:
	case UI_SCREEN_DISP:
	case UI_UPDATE_PIC:
	case UI_UPDATE_EXTRA_PIC:
		stUI.cb_Handle(stUI.eID, stUI.pvParent, stUI.pvInData, stUI.pvOutData);
		break;
	
	default:
		break;
	}
}

int LVQueueWrite(EM_UI_HANDEL_ID eHandleID, void *pvParent, CB_UI_HANDLE cb_UIHandle, void *pvInData, void *pvOutData)
{
	int iRet;
	ST_UIConf stUI;

	memset(&stUI, 0x00, UI_QUEUE_SIZE);
	stUI.eID = eHandleID;
	stUI.pvParent = pvParent;
	stUI.cb_Handle = cb_UIHandle;
	stUI.pvInData = pvInData;
	stUI.pvOutData = pvOutData;
	iRet = YMI_QueueWrite(s_tUIQueue, (uint8_t *)&stUI, UI_QUEUE_SIZE, 1000);
	if(iRet != YMI_OK)
	{
		UI_LV_LOG("[ERR]queue write ret=%d", iRet);
		return RET_FAIL;
	}

	return iRet;
}
lv_obj_t *LVNormalPadCreate(lv_obj_t *pstParent, uint wWidth, uint wHeight)
{
	lv_obj_t *pstPad = lv_obj_create(pstParent);
	// lv_obj_set_size(pstPad, wWidth, wHeight);
    lv_obj_set_style_bg_color(pstPad, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_pad_all(pstPad, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(pstPad, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(pstPad, 0, LV_PART_MAIN);
	return pstPad;
}
int LVMainPadCreateHandle(EM_UI_HANDEL_ID eID, void *pvParent, void *pvInData, void *pvOutData)
{
	s_pstMainPad = lv_obj_create(lv_scr_act());
	lv_obj_set_size(s_pstMainPad, LCD_DISP_WIDTH, LCD_DISP_HEIGHT);
    lv_obj_set_style_bg_color(s_pstMainPad, LV_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_pstMainPad, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(s_pstMainPad, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(s_pstMainPad, 0, LV_PART_MAIN);
	lv_obj_align(s_pstMainPad, LV_ALIGN_TOP_LEFT, 0, 0);

	s_pstPic = lv_img_create(s_pstMainPad);
	lv_obj_add_flag(s_pstPic, LV_OBJ_FLAG_HIDDEN);
	s_pstExtraPic = lv_img_create(s_pstMainPad);
	lv_obj_add_flag(s_pstExtraPic, LV_OBJ_FLAG_HIDDEN);
	s_pstTextPad = LVNormalPadCreate(s_pstMainPad, 0, 0);
	lv_obj_add_flag(s_pstTextPad, LV_OBJ_FLAG_HIDDEN);
	s_pstTextLabel = lv_label_create(s_pstTextPad);
	lv_obj_align(s_pstTextLabel, LV_ALIGN_CENTER, 0, 12);
	lv_obj_set_style_text_align(s_pstTextLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

	s_pstOtherPad = LVNormalPadCreate(s_pstMainPad, 0, 0);
	lv_obj_add_flag(s_pstOtherPad, LV_OBJ_FLAG_HIDDEN);
	// s_pstOtherLabel = lv_label_create(s_pstOtherPad);
	// lv_obj_align(s_pstOtherLabel, LV_ALIGN_CENTER, 0, 0);
	// lv_obj_set_style_text_align(s_pstOtherLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

	s_pstQRCodePad = LVNormalPadCreate(s_pstMainPad, 0, 0);
	lv_obj_add_flag(s_pstQRCodePad, LV_OBJ_FLAG_HIDDEN);
	s_pstQRCode = lv_qrcode_create(s_pstQRCodePad, 240, LV_COLOR_BLACK, LV_COLOR_WHITE);
	return RET_SUCC;
}

void LVMainPadCreate(void)
{
	LVQueueWrite(UI_CREATE, NULL, LVMainPadCreateHandle, NULL, NULL);
}
void LVMainPadDel(void)
{
	lv_obj_del(s_pstMainPad);
}


static void ofs_y_anim(void * img, int32_t v)
{
    lv_img_set_offset_y(img, v);
	SLEEP_MS(300);
}


static void ImgEventHandle(lv_event_t * pstEvent)
{
    lv_event_code_t eCode = lv_event_get_code(pstEvent);
    lv_obj_t * pstPic = lv_event_get_target(pstEvent);

	// UI_LV_LOG("envet id=%d", eCode);
	if(eCode == LV_EVENT_DRAW_MAIN_END)
	{
		if(lv_obj_has_flag(pstPic, LV_OBJ_FLAG_USER_2))
		{
			lv_obj_add_flag(pstPic, LV_OBJ_FLAG_USER_1);//not fresh
			lv_obj_clear_flag(pstPic, LV_OBJ_FLAG_USER_2);
			lv_img_cache_invalidate_src(lv_img_get_src(pstPic));
		}
		if (lv_img_get_src(pstPic))
		{
			lv_img_cache_invalidate_src(lv_img_get_src(pstPic));
		}
	}
}
static void ui_lv_img_set_src(lv_obj_t * obj, char* fileName){
    char filePath[256] = {0};
    char appPath[256] = {0};

    memset(filePath,0x0,sizeof(filePath));
    getcwd(appPath, sizeof(appPath));
	sprintf(filePath, "A:%s/%s", appPath, fileName);
    UI_LV_LOG("filePath = %s",filePath);
    lv_img_set_src(obj, filePath);
}

int LVDispPicHandle(EM_UI_HANDEL_ID eID, void *pvParent, void *pvInData, void *pvOutData)
{
	// lv_obj_t *pstPad = (lv_obj_t *)pvParent;
	ST_ScreenDispConf *pstScreenDispConf = (ST_ScreenDispConf *)pvInData;

	// UI_LV_LOG("disp pic parent=%p, indata=%p", pvParent, pvInData);
	if(pvParent == NULL || pvInData == NULL)
	{
		return RET_FAIL;
	}
	
	if(pstScreenDispConf->pvPic)
	{
		ST_PicDispConf *pstPicDispConf = (ST_PicDispConf *)pstScreenDispConf->pvPic;
		if(s_pstPic == NULL)
		{
			s_pstPic = lv_img_create(s_pstMainPad);
		}
		lv_obj_align(s_pstPic, LV_ALIGN_TOP_LEFT, pstPicDispConf->iX, pstPicDispConf->iY);
		lv_obj_set_size(s_pstPic, pstPicDispConf->wWidth, pstPicDispConf->wHeight);
		lv_obj_clear_flag(s_pstPic, LV_OBJ_FLAG_HIDDEN);
		lv_obj_set_style_pad_all(s_pstPic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_width(s_pstPic, 0, LV_PART_MAIN);
    	lv_obj_set_style_radius(s_pstPic, 0, LV_PART_MAIN);
		ui_lv_img_set_src(s_pstPic, pstPicDispConf->szFilePath);
		// UI_LV_LOG("disp pic file name=%s", pstPicDispConf->szFilePath);
		// lv_obj_add_event_cb(s_pstPic, ImgEventHandle, LV_EVENT_ALL, NULL);
		UI_LV_LOG("pic=%s, x=%d, y=%d, width=%d, height=%d", 
			pstPicDispConf->szFilePath, pstPicDispConf->iX, pstPicDispConf->iY, 
			pstPicDispConf->wWidth, pstPicDispConf->wHeight);
		// lv_obj_clean(s_pstPic);
		// s_pstPic = NULL;
		// lv_obj_clear_flag(s_pstPic, LV_OBJ_FLAG_USER_1);
		// lv_obj_add_flag(s_pstPic, LV_OBJ_FLAG_USER_2);
	}
	else
	{
		if(s_pstPic)
		{
			lv_obj_add_flag(s_pstPic, LV_OBJ_FLAG_HIDDEN);
		}
	}
	if(pstScreenDispConf->pvExtraPic)
	{
		ST_PicDispConf *pstExtraPicDispConf = (ST_PicDispConf *)pstScreenDispConf->pvExtraPic;
		lv_obj_align(s_pstExtraPic, LV_ALIGN_TOP_LEFT, pstExtraPicDispConf->iX, pstExtraPicDispConf->iY);
		lv_obj_set_size(s_pstExtraPic, pstExtraPicDispConf->wWidth, pstExtraPicDispConf->wHeight);
		lv_obj_clear_flag(s_pstExtraPic, LV_OBJ_FLAG_HIDDEN);
		lv_obj_set_style_pad_all(s_pstExtraPic, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_width(s_pstExtraPic, 0, LV_PART_MAIN);
    	lv_obj_set_style_radius(s_pstExtraPic, 0, LV_PART_MAIN);
		ui_lv_img_set_src(s_pstExtraPic, pstExtraPicDispConf->szFilePath);
		// UI_LV_LOG("disp extra pic file name=%s", pstExtraPicDispConf->szFilePath);
		// lv_obj_add_event_cb(s_pstExtraPic, ImgEventHandle, LV_EVENT_ALL, NULL);
		UI_LV_LOG("pic=%s, x=%d, y=%d, width=%d, height=%d", 
			pstExtraPicDispConf->szFilePath, pstExtraPicDispConf->iX, pstExtraPicDispConf->iY, 
			pstExtraPicDispConf->wWidth, pstExtraPicDispConf->wHeight);
	}
	else
	{
		lv_obj_add_flag(s_pstExtraPic, LV_OBJ_FLAG_HIDDEN);
	}
	
	if(pstScreenDispConf->pvQRCode)
	{
		ST_QRCodeDispConf *pstQRCodeDispConf = (ST_QRCodeDispConf *)pstScreenDispConf->pvQRCode;
		lv_obj_set_style_pad_all(s_pstQRCodePad, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_width(s_pstQRCodePad, 0, LV_PART_MAIN);
		lv_obj_set_style_radius(s_pstQRCodePad, 0, LV_PART_MAIN);
		lv_obj_set_size(s_pstQRCodePad, pstQRCodeDispConf->wSize, pstQRCodeDispConf->wSize);
		lv_obj_align(s_pstQRCodePad, LV_ALIGN_TOP_LEFT, pstQRCodeDispConf->iX, pstQRCodeDispConf->iY);
		lv_obj_clear_flag(s_pstQRCodePad, LV_OBJ_FLAG_HIDDEN);
		lv_qrcode_delete(s_pstQRCode);
		// s_pstQRCode = lv_qrcode_create(s_pstQRCodePad, pstQRCodeDispConf->wSize, lv_color_hex(0x1E90FF), lv_color_hex(0xFFFF00));
		s_pstQRCode = lv_qrcode_create(s_pstQRCodePad, pstQRCodeDispConf->wSize, LV_COLOR_BLACK, LV_COLOR_WHITE);
		lv_qrcode_update(s_pstQRCode, pstQRCodeDispConf->szQRCode, strlen(pstQRCodeDispConf->szQRCode));

		// lv_obj_align(s_pstQRCode, LV_ALIGN_TOP_LEFT, 0, 0);
		// lv_anim_t a;
		// lv_anim_init(&a);
		// lv_anim_set_var(&a, s_pstQRCode);
		// lv_anim_set_exec_cb(&a, ofs_y_anim);
		// // lv_anim_set_get_value_cb(&a, get_value);
		// lv_anim_set_values(&a, 0, pstQRCodeDispConf->wSize);
		// lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
		// lv_anim_set_repeat_delay(&a, 1000);
		// lv_anim_start(&a);
	}
	else
	{
		lv_obj_add_flag(s_pstQRCodePad, LV_OBJ_FLAG_HIDDEN);
	}
	if(pstScreenDispConf->pvText)
	{
		ST_TextDispConf *pstTextDispConfg = (ST_TextDispConf *)pstScreenDispConf->pvText;
		lv_obj_set_style_pad_all(s_pstTextPad, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_width(s_pstTextPad, 0, LV_PART_MAIN);
		lv_obj_set_style_radius(s_pstTextPad, 0, LV_PART_MAIN);
		lv_obj_align(s_pstTextPad, LV_ALIGN_TOP_LEFT, pstTextDispConfg->iX, pstTextDispConfg->iY);
		lv_obj_set_style_bg_color(s_pstTextPad, LVColorSet(pstTextDispConfg->wBgColor), LV_PART_MAIN);
		lv_obj_set_size(s_pstTextPad, LCD_DISP_WIDTH, pstTextDispConfg->wFontSize);
		lv_obj_clear_flag(s_pstTextPad, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(s_pstTextPad, LV_OBJ_FLAG_SCROLLABLE);

		lv_obj_set_size(s_pstTextLabel, LCD_DISP_WIDTH, pstTextDispConfg->wFontSize);
		// lv_label_set_long_mode(s_pstTextLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
		if(pstTextDispConfg->wFontSize < 48)
		{
			lv_obj_add_style(s_pstTextLabel, &s_stNorFontStyle, LV_PART_MAIN);
		}
		lv_obj_set_style_text_color(s_pstTextLabel, LVColorSet(pstTextDispConfg->wFontColor), LV_PART_MAIN);
		// lv_obj_set_style_text_letter_space(s_pstTextLabel, 5, LV_PART_MAIN);
		lv_label_set_text(s_pstTextLabel, pstTextDispConfg->szText);
		// lv_obj_align(s_pstQRCode, LV_ALIGN_TOP_LEFT, 0, 0);
		// lv_obj_set_size(s_pstTextPad, LCD_DISP_WIDTH, lv_font_get_line_height(lv_obj_get_style_text_font(s_pstTextLabel, LV_PART_MAIN)));
	}
	else
	{
		lv_obj_add_flag(s_pstTextPad, LV_OBJ_FLAG_HIDDEN);
	}

	if(pstScreenDispConf->pvOther)
	{
#if 0
		ST_TextDispConf *pstTextDispConfg = (ST_TextDispConf *)pstScreenDispConf->pvOther;
		lv_obj_set_style_pad_all(s_pstOtherPad, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_width(s_pstOtherPad, 0, LV_PART_MAIN);
		lv_obj_set_style_radius(s_pstOtherPad, 0, LV_PART_MAIN);
		lv_obj_align(s_pstOtherPad, LV_ALIGN_TOP_LEFT, pstTextDispConfg->iX, pstTextDispConfg->iY);
		lv_obj_set_style_bg_color(s_pstOtherPad, LVColorSet(pstTextDispConfg->wBgColor), LV_PART_MAIN);
		lv_obj_set_size(s_pstOtherPad, LCD_DISP_WIDTH, pstTextDispConfg->wFontSize * 2);
		lv_obj_clear_flag(s_pstOtherPad, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(s_pstOtherPad, LV_OBJ_FLAG_SCROLLABLE);

		// lv_obj_set_size(s_pstOtherLabel, LCD_DISP_WIDTH - 10, (pstTextDispConfg->wFontSize * 2 + 10));
		lv_obj_set_width(s_pstOtherLabel, LCD_DISP_WIDTH - 10);
		// lv_label_set_long_mode(s_pstOtherLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_add_style(s_pstOtherLabel, &s_stSmallFontStyle, LV_PART_MAIN);
		lv_obj_set_style_text_color(s_pstOtherLabel, LVColorSet(pstTextDispConfg->wFontColor), LV_PART_MAIN);
		// lv_obj_set_style_text_letter_space(s_pstOtherLabel, 5, LV_PART_MAIN);
		// lv_obj_set_style_text_align(s_pstOtherLabel, LV_TEXT_ALIGN_LEFT, 0);
		lv_label_set_text(s_pstOtherLabel, pstTextDispConfg->szText);
		// lv_obj_set_size(s_pstOtherPad, LCD_DISP_WIDTH, lv_font_get_line_height(lv_obj_get_style_text_font(s_pstOtherLabel, LV_PART_MAIN)));
#endif
		ST_VideoDispConf *pstVideoDispConf = (ST_VideoDispConf *)pstScreenDispConf->pvOther;
		if(strstr(pstVideoDispConf->szFilePath, ".mp4"))
		{
			lv_obj_set_style_pad_all(s_pstOtherPad, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_border_width(s_pstOtherPad, 0, LV_PART_MAIN);
			lv_obj_set_style_radius(s_pstOtherPad, 0, LV_PART_MAIN);
			lv_obj_align(s_pstOtherPad, LV_ALIGN_TOP_LEFT, pstVideoDispConf->iX, pstVideoDispConf->iY);
			lv_obj_set_style_bg_color(s_pstOtherPad, LV_COLOR_BLACK, LV_PART_MAIN);
			lv_obj_set_size(s_pstOtherPad, pstVideoDispConf->wWidth, pstVideoDispConf->wHeight);
			lv_obj_clear_flag(s_pstOtherPad, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(s_pstOtherPad, LV_OBJ_FLAG_SCROLLABLE);
			char filePath[256] = {0};
			char appPath[256] = {0};
			memset(filePath,0x0,sizeof(filePath));
			getcwd(appPath, sizeof(appPath));
			sprintf(filePath, "%s/%s", appPath, pstVideoDispConf->szFilePath);
			UI_LV_LOG("vedio filePath = %s",filePath);
			YMI_VideoPlayStart(pstVideoDispConf->iX, pstVideoDispConf->iY, 
				pstVideoDispConf->wWidth, pstVideoDispConf->wHeight, filePath, pstVideoDispConf->wTimes);
		}
	}
	else
	{
		YMI_VideoPlayStop();
		lv_obj_add_flag(s_pstOtherPad, LV_OBJ_FLAG_HIDDEN);
	}
exit:
	FREE(pstScreenDispConf->pvPic)
	FREE(pstScreenDispConf->pvQRCode);
	FREE(pstScreenDispConf->pvText);
	FREE(pstScreenDispConf->pvExtraPic);
	FREE(pstScreenDispConf->pvOther);
	FREE(pstScreenDispConf);

	return RET_SUCC;
}

int LVScreenUpdateHandle(EM_UI_HANDEL_ID eID, void *pvParent, void *pvInData, void *pvOutData)
{
	
	switch (eID)
	{
	case UI_UPDATE_PIC:
		if(pvInData)
		{
			ST_PicDispConf *pstPicDispConf = (ST_PicDispConf *)pvInData;
			if (lv_img_get_src(s_pstPic))
			{
				lv_img_cache_invalidate_src(lv_img_get_src(s_pstPic));
			}
			SLEEP_MS(100);
			lv_img_set_src(s_pstPic, pstPicDispConf->szFilePath);
			lv_obj_align(s_pstPic, LV_ALIGN_TOP_LEFT, pstPicDispConf->iX, pstPicDispConf->iY);
			// lv_obj_clear_flag(s_pstPic, LV_OBJ_FLAG_USER_1);//To update an image, you need to refresh all the images, otherwise a 5-pixel white bar will appear at the connection
			// lv_obj_clear_flag(s_pstPic, LV_OBJ_FLAG_USER_2);
			// lv_obj_clear_flag(s_pstExtraPic, LV_OBJ_FLAG_USER_1);
			// lv_obj_clear_flag(s_pstExtraPic, LV_OBJ_FLAG_USER_2);
			lv_obj_clear_flag(s_pstPic, LV_OBJ_FLAG_HIDDEN);
		}
		break;
	case UI_UPDATE_EXTRA_PIC:
		if(pvInData)
		{
			ST_PicDispConf *pstPicDispConf = (ST_PicDispConf *)pvInData;
			if (lv_img_get_src(s_pstExtraPic))
			{
				lv_img_cache_invalidate_src(lv_img_get_src(s_pstExtraPic));
			}
			SLEEP_MS(100);
			lv_img_set_src(s_pstExtraPic, pstPicDispConf->szFilePath);
			lv_obj_align(s_pstExtraPic, LV_ALIGN_TOP_LEFT, pstPicDispConf->iX, pstPicDispConf->iY);
			// lv_obj_clear_flag(s_pstPic, LV_OBJ_FLAG_USER_1);
			// lv_obj_clear_flag(s_pstPic, LV_OBJ_FLAG_USER_2);
			// lv_obj_clear_flag(s_pstExtraPic, LV_OBJ_FLAG_USER_1);
			// lv_obj_clear_flag(s_pstExtraPic, LV_OBJ_FLAG_USER_2);
			lv_obj_clear_flag(s_pstExtraPic, LV_OBJ_FLAG_HIDDEN);
		}
		break;
	
	default:
		break;
	}

	return RET_SUCC;
}

void LVScreenDisp(void *pvPic, void *pvQrcode, void *pvText, void *pvExtraPic, void *pvOther)
{
	ST_ScreenDispConf *pstConf = NULL;
	int iRet = RET_SUCC;

	pstConf = MALLOC(1, ST_ScreenDispConf);
	if(pstConf == NULL)
	{
		UI_LV_LOG("[ERR] jgp disp conf malloc");
		return ;
	}
	memset(pstConf, 0x00, sizeof(ST_ScreenDispConf));
	if(pvPic)
	{
		pstConf->pvPic = MALLOC(1, ST_PicDispConf);
		if(pstConf->pvPic == NULL)
		{
			UI_LV_LOG("[ERR] pic disp conf malloc");
			iRet = RET_FAIL;
			goto exit;
		}
		memset(pstConf->pvPic, 0x00, sizeof(ST_PicDispConf));
		memcpy(pstConf->pvPic, pvPic, sizeof(ST_PicDispConf));
	}
	if(pvQrcode)
	{
		pstConf->pvQRCode = MALLOC(1, ST_QRCodeDispConf);
		if(pstConf->pvQRCode == NULL)
		{
			UI_LV_LOG("[ERR] qrcode disp conf malloc");
			iRet = RET_FAIL;
			goto exit;
		}
		memset(pstConf->pvQRCode, 0x00, sizeof(ST_QRCodeDispConf));
		memcpy(pstConf->pvQRCode, pvQrcode, sizeof(ST_QRCodeDispConf));
	}
	if(pvText)
	{
		pstConf->pvText = MALLOC(1, ST_TextDispConf);
		if(pstConf->pvText == NULL)
		{
			UI_LV_LOG("[ERR] text disp conf malloc");
			iRet = RET_FAIL;
			goto exit;
		}
		memset(pstConf->pvText, 0x00, sizeof(ST_TextDispConf));
		memcpy(pstConf->pvText, pvText, sizeof(ST_TextDispConf));
	}
	if(pvExtraPic)
	{
		pstConf->pvExtraPic = MALLOC(1, ST_PicDispConf);
		if(pstConf->pvExtraPic == NULL)
		{
			UI_LV_LOG("[ERR] extra pic disp conf malloc");
			iRet = RET_FAIL;
			goto exit;
		}
		memset(pstConf->pvExtraPic, 0x00, sizeof(ST_PicDispConf));
		memcpy(pstConf->pvExtraPic, pvExtraPic, sizeof(ST_PicDispConf));
	}
	if(pvOther)
	{
		pstConf->pvOther = MALLOC(1, ST_VideoDispConf);
		if(pstConf->pvOther == NULL)
		{
			UI_LV_LOG("[ERR] extra pic disp conf malloc");
			iRet = RET_FAIL;
			goto exit;
		}
		memset(pstConf->pvOther, 0x00, sizeof(ST_VideoDispConf));
		memcpy(pstConf->pvOther, pvOther, sizeof(ST_VideoDispConf));
	}
	while (s_pstMainPad == NULL)
	{
		SLEEP_MS(10);
	}
	
	LVQueueWrite(UI_SCREEN_DISP, s_pstMainPad, LVDispPicHandle, pstConf, NULL);
exit:
	if(iRet != RET_SUCC)
	{
		FREE(pstConf->pvPic);
		FREE(pstConf->pvQRCode);
		FREE(pstConf->pvText);
		FREE(pstConf->pvExtraPic);
		FREE(pstConf->pvOther);
		FREE(pstConf);
	}
}
void LVUpdatePic(ST_PicDispConf *pstPicDisp)
{	
	ST_PicDispConf *pstConf = NULL;

	if(pstPicDisp == NULL)
	{
		return ;
	}
	
	pstConf = MALLOC(1, ST_PicDispConf);
	if(pstConf == NULL)
	{
		UI_LV_LOG("[ERR] pic disp conf malloc");
		return ;
	}
	memcpy(pstConf, pstPicDisp, sizeof(ST_PicDispConf));
	LVQueueWrite(UI_UPDATE_PIC, s_pstMainPad, LVScreenUpdateHandle, pstConf, NULL);
}
void LVUpdateExtraPic(ST_PicDispConf *pstPicDisp)
{	
	ST_PicDispConf *pstConf = NULL;

	if(pstPicDisp == NULL)
	{
		return ;
	}
	
	pstConf = MALLOC(1, ST_PicDispConf);
	if(pstConf == NULL)
	{
		UI_LV_LOG("[ERR]extra pic disp conf malloc");
		return ;
	}
	memcpy(pstConf, pstPicDisp, sizeof(ST_PicDispConf));
	LVQueueWrite(UI_UPDATE_EXTRA_PIC, s_pstMainPad, LVScreenUpdateHandle, pstConf, NULL);

}

void LVFontInit(void)
{
	//tiny ttf set two font big and small
#ifdef FONT_EXTERNAL
	lv_font_t *pstBigFont = NULL, *pstNorFont = NULL;
    pstBigFont = lv_tiny_ttf_create_file(BIG_FONT_FILE_PATH, BIG_FONT_SIZE);
    pstNorFont = lv_tiny_ttf_create_file(NORMAL_FONT_FILE_PATH, NORMAL_FONT_SIZE);
    if(pstBigFont)
    {
        lv_style_init(&s_stBigFontStyle);
        lv_style_set_text_font(&s_stBigFontStyle, pstBigFont);
        UI_LV_LOG("set big font style");
    }
    if(pstNorFont)
    {
        lv_style_init(&s_stNorFontStyle);
        lv_style_set_text_font(&s_stNorFontStyle, pstNorFont);
        UI_LV_LOG("set normal font style");
    }
#else
	lv_style_init(&s_stNorFontStyle);
	lv_style_set_text_font(&s_stNorFontStyle, &lv_font_montserrat_48);
	lv_style_init(&s_stSmallFontStyle);
	lv_style_set_text_font(&s_stSmallFontStyle, &lv_font_montserrat_22);
#endif
}

void UITaskHandle(void *pvArg)
{
    ST_Event stEvent;
	T_Task tTaskID = *(T_Task *)pvArg;

	YMI_VideoPlayInit();
	LVQueueInit();
	LOG("====================>in ui task");
    // YMI_DispSetBackLight(1);
    lv_init();
	lv_port_disp_init();
    // lv_port_fs_init();
	// lv_port_indev_init();//Keyboard, touch screen
    lv_log_register_print_cb(LVLog);
	LVMainPadCreate();
	LVFontInit();
	while (1)
    {
        if(YMI_EventWait(&stEvent, 10) == YMI_OK)
		{
			if(stEvent.wId == TASK_MANAGE_EVENT_ID && stEvent.wParam1 == TASK_OPT_EXIT)
			{
				break;
			}
		}
		LVQueueRead();
        SLEEP_MS(5);
        lv_timer_handler();
    }
	LVQueueDel();
	disp_disable_update();
	// lv_port_disp_del_draw_buf();
	lv_disp_remove(lv_disp_get_default());
	LVMainPadDel();
	// PngDecoDeinit();
	YMI_VideoPlayStop();
	YMI_VideoPlayDeinit();
    YMI_TaskDelete(tTaskID);
}


//end of file