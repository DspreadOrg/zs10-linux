#ifndef _UI_LVGL_H_
#define _UI_LVGL_H_

typedef enum ui_handel_id{
	UI_CREATE,
	UI_SCREEN_DISP,
	UI_UPDATE_PIC,
	UI_UPDATE_EXTRA_PIC,
}EM_UI_HANDEL_ID;

typedef int (*CB_UI_HANDLE)(EM_UI_HANDEL_ID eID, void *pvParent, void *pvInData, void *pvOutData);
typedef struct ui_queue
{
	EM_UI_HANDEL_ID eID;
	CB_UI_HANDLE cb_Handle;
	void *pvParent;
	void *pvInData;
	void *pvOutData;
}ST_UIConf;

typedef struct qrcode_disp_conf
{
	char szQRCode[256];
	int iX;
	int iY;
	uint wSize;
}ST_QRCodeDispConf;

typedef struct text_disp_conf
{
	char szText[256];
	int iX;
	int iY;
	uint wFontColor;
	uint wBgColor;
	uint wFontSize;
}ST_TextDispConf;

typedef struct pic_disp_conf
{
	char szFilePath[128];
	int iX;
	int iY;
	uint wWidth;
	uint wHeight;
}ST_PicDispConf;

typedef struct video_disp_conf
{
	char szFilePath[128];
	int iX;
	int iY;
	uint wWidth;
	uint wHeight;
	uint wTimes;
}ST_VideoDispConf;
typedef struct screen_disp_conf
{
	void *pvPic;
	void *pvQRCode;
	void *pvText;
	void *pvExtraPic;
	void *pvOther;
}ST_ScreenDispConf;

void UITaskHandle(void *pvArg);
void LVScreenDisp(void *pvPic, void *pvQrcode, void *pvText, void *pvExtraPic, void *pvOther);
void LVUpdatePic(ST_PicDispConf *pstPicDisp);
void LVUpdateExtraPic(ST_PicDispConf *pstPicDisp);
#endif