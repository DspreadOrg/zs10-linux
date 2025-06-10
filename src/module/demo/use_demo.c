#include "var.h"

void print_time_info(const char *prefix, const struct tm *time) {
    char time_str[128]={0};
    snprintf(time_str, sizeof(time_str), 
         "%d-%d-%d %02d:%02d:%02d",
         time->tm_year,         
         time->tm_mon + 1,      
         time->tm_mday,         
         time->tm_hour,         
         time->tm_min,          
         time->tm_sec);         
    LOG("%s: %s\n", prefix, time_str);
}
int demo_set_time(void) {
    struct tm current_time;
    int result = 0;
    //set Date 2025-6-10 14:00:00
    current_time.tm_year=2025;
    current_time.tm_mon=6-1;// =mon-1
    current_time.tm_mday=10;
    current_time.tm_hour=14;
    current_time.tm_min=0;
    current_time.tm_sec=0;
    result = YMI_SysSetDevTime(current_time);
    LOG("set time 2025-6-10 14:00:00 ret= %d",result);
    memset(&current_time,0,sizeof(struct tm));
    result = YMI_SysGetDevTime(&current_time);
    if (result == YMI_OK) {
        LOG("=1===%d",current_time.tm_year);
        print_time_info("current time", &current_time);
    } else {
        LOG("get time error ret= %d",result);
        return -1;
    }
    return 0;
}

void SnWriteDemo(){    
    char temp[64]={0};
    int snLen =64;
    int ret =0;

    ret = YMI_SysGetDevInfo(DEV_INFO_SN, &snLen, temp);
    LOG("[1]get sn ret = %d sn %d= %s",ret,snLen, temp);

    memset(temp,0,sizeof(temp));
    strcpy(temp,"123465");
    ret = YMI_SysSetDevInfo(DEV_INFO_SN, temp);
    LOG("[2]set sn ret = %d",ret);
    memset(temp,0,sizeof(temp));

    snLen=sizeof(temp);
    ret = YMI_SysGetDevInfo(DEV_INFO_SN, &snLen, temp);
    LOG("[3]get ret = %d sn %d= %s",ret,snLen, temp);
}

