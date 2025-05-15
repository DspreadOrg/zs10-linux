#ifndef _APN_H_
#define _APN_H_

typedef enum 
{
	QL_SUCCESS,
	QL_GENERIC_FAILURE,
}QL_ERROR_CODE_E;


typedef struct APNinfo{
	const char* plmn;
	const char* apn;
	const char* user;
	const char* password;
}StructAPNinfo;

StructAPNinfo const * GetApnInfo(const char*imsi);

#endif
