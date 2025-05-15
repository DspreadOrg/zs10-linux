#ifndef _NET_H_
#define _NET_H_

typedef enum {
	COMMU_TYPE_LTE,
	COMMU_TYPE_WIFI,
	COMMU_TYPE_UNKNOWN,
}EM_COMMU_TYPE;

typedef enum
{
	WIFI_SET_TYPE_UNKONW,
	WIFI_SET_TYPE_AP,
	WIFI_SET_TYPE_QUICK,
}EM_WIFI_SET_TYPE;

int NetTaskInit(void);

#endif