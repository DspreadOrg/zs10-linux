#include "var.h"

int main(int argc, char *argv[])
{
    YMI_AppStart(argc, argv);
    LOG("[APP]start!");
    YMI_AudioSetVolume(EM_VOL_LEVEL_2);
    UITaskInit();
    ST_PicDispConf stPicUp = {
        .iX = 0,
        .iY = 0,
        .szFilePath = "res/power_on.jpg",
        .wWidth = 800,
        .wHeight = 1280,
    };
    ST_VideoDispConf stVideo = {
        .szFilePath = "res/ads.mp4", 
        .iX = 100,
        .iY = 200,
        .wWidth = 400,
        .wHeight = 300,
        .wTimes = VIDEO_PLAY_ON_LOOP,
    };
    LVScreenDisp(&stPicUp, NULL, NULL, NULL, NULL);
    SysInfoInit();
    DevTaskInit();
    NetTaskInit();
    HIDTaskInit();
    AudioTaskInit();
    KeyTaskInit();
    while (YMI_AppIsReadyExit() != YMI_OK)
    {
        SLEEP_MS(100);
        // LOG("[APP]wait exit");
    }
    LOG("[APP]exit!");
    YMI_AppEnd();
    LOG("[APP]end!");
    return 0;
}