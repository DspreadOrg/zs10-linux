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


int getGmtTimeFromBody(char*input,char *result) {
  
    char *date_start = strstr(input, "Date: ");
    if (date_start == NULL) {
        return 1;
    }
    
    date_start += strlen("Date: ");
    
    char *gmt_end = strstr(date_start, "\r\n");
    if (gmt_end == NULL) {
        return 1;
    }
    
    size_t length = gmt_end - date_start;
    
    strncpy(result, date_start, length);
    result[length] = '\0';  // 确保字符串以空字符结尾
    LOG("result = %s",result);
    return 0;
}
const char http_date [] = "Wed, 11 Jun 2025 10:15:47 GMT";
int http_date_to_set(const char *http_date) {
    if (!http_date ) {
        return -1;
    }
    struct tm tm_time;
	
    char buf[128];
    char weekday[10], month[10], zone[10];
    int day, year, hour, min, sec;
    
    strncpy(buf, http_date, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    
    if (sscanf(buf, "%s %d %s %d %d:%d:%d %s", 
               weekday, &day, month, &year, &hour, &min, &sec, zone) != 8) {
        return -1;
    }
    LOG("=weekday=%s d= %d m=%s y=%d h%d f%d s%d==\n",weekday,day,month,year,hour,min,sec);
    memset(&tm_time, 0, sizeof(struct tm));
    
    if (strcmp(month, "Jan") == 0) tm_time.tm_mon = 0;
    else if (strcmp(month, "Feb") == 0) tm_time.tm_mon = 1;
    else if (strcmp(month, "Mar") == 0) tm_time.tm_mon = 2;
    else if (strcmp(month, "Apr") == 0) tm_time.tm_mon = 3;
    else if (strcmp(month, "May") == 0) tm_time.tm_mon = 4;
    else if (strcmp(month, "Jun") == 0) tm_time.tm_mon = 5;
    else if (strcmp(month, "Jul") == 0) tm_time.tm_mon = 6;
    else if (strcmp(month, "Aug") == 0) tm_time.tm_mon = 7;
    else if (strcmp(month, "Sep") == 0) tm_time.tm_mon = 8;
    else if (strcmp(month, "Oct") == 0) tm_time.tm_mon = 9;
    else if (strcmp(month, "Nov") == 0) tm_time.tm_mon = 10;
    else if (strcmp(month, "Dec") == 0) tm_time.tm_mon = 11;

	YMI_SysSetDevTime(tm_time);
    return 0;
}
extern int g_iNetOnline;
void syncTimeWithHTTP(){
    char request_headers[1024] = {0};
    char request_body[1024] = {0};
	char timeStr[64]={0};
    char response[1024*3] = {0};

    strcat(request_headers,"GET /get HTTP/1.1\r\nHost: httpbin.org\r\nConnection: close\r\nAccept: */*\r\n\r\n");
    LOG("request_headers = %s",request_headers);
    while (!g_iNetOnline)
    {
        SLEEP_S(5);
    }

    int ret = YMI_HttpCommuLKL(SOCKET_TCP_CLIENT,"https://httpbin.org/get",request_headers,request_body,0,response,sizeof(response));

	if(ret >30){
		ret = getGmtTimeFromBody(response,timeStr);
		if(ret == 0){
			http_date_to_set(timeStr);
		}
	}
}
