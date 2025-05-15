/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MQTT_EXAMPLE_H_
#define _MQTT_EXAMPLE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	MES_DIARVE	= 1,
	MES_ARVE	= 2
} MESSAGE_STATUS;

typedef enum {
	MQTT_DICACK	= 1,
	MQTT_CACK		= 2
} MQTT_CALLBACK;

typedef struct mqt_cal{
	unsigned char mqtt_con;
	unsigned char mqtt_quit;
	unsigned char mqtt_sub;
	unsigned char mqtt_pub;
}mqt_cal;

extern mqt_cal cal_set;


extern void start_Mqtt_task(void);

#define MqttMaskA	0x1
#define MqttMaskB	0x2

typedef struct sysparam {
	int battery;
	int flash;
	int microphone;
	int led;
	int speaker;
	int register_state;
	int volume;
	char firmware_version[50];
	char device_secret[64];
	char product_key[64];
	//char product_secret[64];
	char device_name[64];
	char device_SN[64];
	char server_url[64];
	char hwver[32];
	char server_mqtt[64];
	int uart_log;
	// struct sysinfo_wlan_sta_param wlan_sta_param;
	char WifiVer[20];
	char Topic[64];
	int backlight_level;
	int device_type; // 0:Pure 4 g device, 1:WIFI+4G devices
	int NetChanlLTE;
	int airkiss;
	int button_level;
	int ota;
	int first_boot;
	int example[512];//Example parameter replacement
	int unuse[100];
	char Md5[16];
}sysparam_t;


// #define AWS_SERVER


#ifdef AWS_SERVER
const char *aws_client_crt =  {
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDWjCCAkKgAwIBAgIVAJg2BqmRfeFod8yxF9/bFI+axTEBMA0GCSqGSIb3DQEB\r\n"
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\r\n"
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDExMjUwMjI4\r\n"
"MzRaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\r\n"
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC6+VHSk5yZSiwF8GDx\r\n"
"oTgzpm1Ad7f4Cj9S4KEzrdeACP1HgA3UK15u2dSNbAw+0ZOaQgtMRSB4GK+tc/r/\r\n"
"09i9SXcsUdQ6Qvt+UnZGl4SuGMr9jCP57Ap4a5YmP7mzsZRF+ibPtoYgIb95qINO\r\n"
"DJPj94Gqbmcnhzo1ejGOMma9ZMsazJKUI2wBnTbeAmYBogrKw6/Uuh+MesZbu+UA\r\n"
"3gO4AB9On51gLIPrOkB8yVFWrBD/UTpJLjXELvV1K6ehzw83CMKHAiZHX4xYqge+\r\n"
"InLjMQ6yqXEq3oyUfWR6Fe5o59OwH4nQmJ7wQ+2z3ICMAFCYgybfFsyMG7JwJsh5\r\n"
"VqL1AgMBAAGjYDBeMB8GA1UdIwQYMBaAFOpQr7VSWEqtFDCq0gJaKIY22veGMB0G\r\n"
"A1UdDgQWBBS0uaCdYba0iyYYS7ZWbT6Le0WStzAMBgNVHRMBAf8EAjAAMA4GA1Ud\r\n"
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAl/FZITpHsFdMlxcTgzrYPNLp\r\n"
"A+JfpFR4F61XeOOn+GhVz7T6ZHpZgvnXUTBGXvsWqcYp3v7rnPZBlCbdqoUP52y/\r\n"
"LgL1I/Do1n6IRKobK5ffGbwDDk0b5+i4tBd2JuV1MC8eAJpf14AAWOM8rfkDLh8T\r\n"
"vcmv07sa4OkpYsL8Ru2Mey4iMe63WR4mLZbciI8lzoSbejJKvXL8upWNc5PyEGMz\r\n"
"uhlop2DceaCUgX4XtE/1I5QiG95YyrkU1clrjhnKx9a5rXH9PaAQcUETq8UhtkRl\r\n"
"1GXK9Ld4MVGE9K0DmiOuHXB8P3JyCbePFzXpr8KhbpRlPrd3c1tWyoWbw3aRyA==\r\n"
"-----END CERTIFICATE-----\r\n"};
const char *aws_client_key_crt =  {
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEAuvlR0pOcmUosBfBg8aE4M6ZtQHe3+Ao/UuChM63XgAj9R4AN\r\n"
"1CtebtnUjWwMPtGTmkILTEUgeBivrXP6/9PYvUl3LFHUOkL7flJ2RpeErhjK/Ywj\r\n"
"+ewKeGuWJj+5s7GURfomz7aGICG/eaiDTgyT4/eBqm5nJ4c6NXoxjjJmvWTLGsyS\r\n"
"lCNsAZ023gJmAaIKysOv1LofjHrGW7vlAN4DuAAfTp+dYCyD6zpAfMlRVqwQ/1E6\r\n"
"SS41xC71dSunoc8PNwjChwImR1+MWKoHviJy4zEOsqlxKt6MlH1kehXuaOfTsB+J\r\n"
"0Jie8EPts9yAjABQmIMm3xbMjBuycCbIeVai9QIDAQABAoIBACSKZTtKLeinXkMq\r\n"
"30dZwqoVdHSM8R9AoW+JBvgamWskVpuKoyk78JCpcajR3PLJ8pbLIXoeaGIQMQUt\r\n"
"DXjB5jHYEbpPo7JfNNdTNlpFcmCz57f1k3Jcdw9ht05yVdCQe/HRGZSujzfoYIte\r\n"
"K4stWhnLXUP5n59SBkY9Q0IPB5UrsCHuMXaaoM+22YNz78WYz2rxQ77mBpuPE137\r\n"
"+wdLOppQ27sHoy6+yJ2IeZwW4cQ4vMm6YRu6yZdCrFUXSTotohkOaFTmSgMVZENr\r\n"
"bzAPvweMZduc+y5OuQYzpq2LATCG2+2IqF0UxwRiFwIbH8VkEaHe/jUoQZ9uh9dl\r\n"
"iGVHHwECgYEA25QIVyzFqbLUBx74IVnIC4Vz+NEayKyBzVHWL9srryP7fJuq4NX+\r\n"
"zIGm0cIfGCgoB0kiHOpvk3SxQsP3HstjDv0Nl6O/gUvX6EqAFLQjv/14v7kyLftz\r\n"
"4g/r7j5tKD8EFSKrryjP9gY0Iv0c+T1v8UOSRQ7LIoR4XTJdthpBXekCgYEA2fzO\r\n"
"Zddf2XJKXHjlG17s7M6m4vvAfQ8wesbAly40xVtYJHXOjlxoaZ1cmpxGKzVXZ9fw\r\n"
"iYAV/jZNLS4su6gS2eBtTvWEOamDVirzsO4xkLFdN6D8AOu9quYsd6X1jSsq+IIq\r\n"
"+1H5geAgMZsK9r83fMtqFbK+qB4FXcHqVcsGeS0CgYEAzZcTIBxJjpRjta8ngIKB\r\n"
"rVfsuAGBZHcwqb+AHhrGEhxMseVG33E+OtkCCtAJMSMYJ4owKIbQpW3TMsItNWYV\r\n"
"f8kr2tl0y2jp5RMsTTZFatO3JTwPYcJYREfIDa/hoJcwCWIHAd/ip2YzlrcLU8pU\r\n"
"b79mysYJKG6EDC7GU2TN41ECgYEAjIOmdAVZ9qvyfRs86+DLrnYjI2uOj86rvGjD\r\n"
"mnAhehpAffgmdqPgdjwGhm832ABhYEHU/n0/MAfGxdne4fQceq9tJpmiaxz22SOE\r\n"
"xEc2UgPSh9pQtzM+ppwKi/HBkAcEgv49QJV91RcpCXDcp2NpHBtvPZ9jwLDhWqd2\r\n"
"K2lcud0CgYBQgsESu8QTjSune43qYHr9SqMe56lSS1uNVXTEMELAVbegpRYWGvdb\r\n"
"UIfwUZzpJm8z6JfWo0V3qMX3yiseElyGYtDRFvGjUOryxSvCkk94suf2MMRwhHrS\r\n"
"4/5QXWCUDISgePlZXgD8NcXjiys9zAuD8kGTtNLuNC9TsmuAvO/5QA==\r\n"
"-----END RSA PRIVATE KEY-----\r\n"};

const char *aws_ca_crt =  {
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n"
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n"
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n"
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n"
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n"
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n"
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n"
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n"
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n"
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n"
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n"
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n"
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n"
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n"
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n"
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n"
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n"
"rqXRfboQnoZsG4q5WTP468SQvvG5\r\n"
"-----END CERTIFICATE-----"};
#endif

#define HOST_ADDRESS_LEN    	(64)
#define HOST_PORT_LEN       	(8)
#define CLIENT_ID_LEN       	(128)
#define USER_NAME_LEN       	(128)
#define PASSWORD_LEN        	(40)

#define LPM_LOCK_MQTT              4

typedef enum {
   Mode_NULL = 0,
   WIFI_MODE = 1,
   GPRS_MODE = 2,
   WIFI_BAKE_MODE = 3,
   GPRS_BAKE_MODE = 4,
}Enum_Device_Mode;

typedef enum net_device_states {
	NET_DEVICE_STATE_DISCONNECTED = 0,
	NET_DEVICE_STATE_CONNECTED = 1,
} net_device_states_t;

typedef enum {
    XR_MQTT_QOS0 = 0,
    XR_MQTT_QOS1,
    XR_MQTT_QOS2
}xr_mqtt_qos_t;

typedef struct
{
    char Debug;
    // display_if_t disp;
	char buttonAP;
	char SIMState;
	char SntpOk;
	//char MSntpOk;
	//char TTSHard;
	char Charge;
	char ChargeOld;
	uint8_t BatRemain;
	char LowBat;
	char ChargeFull;
	char NetMode;
	int NetStatBak;
	char MqttIsRuning;
	char ServiceOnline;
	char OTAMode;
	char PlayRecordMode;
	unsigned int RepTim;
	unsigned int FailTime;
	char PlayNetSucc;
	int (*Repeat)(void);
#if BT
	    char BTMusicState;
#endif
//	void (*NetStatBreak)(void);
}StructTermInfo;


typedef struct
{
	int len;
	char* data;
} MQTTLenString;

typedef struct
{
	char* cstring;
	MQTTLenString lenstring;
} MQTTString;

enum QoS { QOS0, QOS1, QOS2, SUBFAIL=0x80 };
struct MQTTMessage
{
    enum QoS qos;
    unsigned char retained;
    unsigned char dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
};
typedef struct MQTTMessage MQTTMessage;
struct MessageData
{
    MQTTMessage* message;
    MQTTString* topicName;
};
typedef struct MessageData MessageData;
typedef struct
{
	/** The eyecatcher for this structure.  must be MQTC. */
	char struct_id[4];
	/** The version number of this structure.  Must be 0 */
	int struct_version;
	/** Version of MQTT to be used.  3 = 3.1 4 = 3.1.1
	  */
	unsigned char MQTTVersion;
	MQTTString clientID;
	unsigned short keepAliveInterval;
	unsigned char cleansession;
	unsigned char willFlag;
	// MQTTPacket_willOptions will;
	MQTTString username;
	MQTTString password;
} MQTTPacket_connectData;

struct Client {
    unsigned int next_packetid;
    unsigned int command_timeout_ms;
    size_t buf_size, readbuf_size;
    unsigned char *buf;  
    unsigned char *readbuf; 
    unsigned int keepAliveInterval;
    char ping_outstanding;
    int isconnected;
	int isDiscon;	// MQTT disconnection
    int cleansession;
    char ping_cnt;
	void (*defaultMessageHandler) (MessageData*);
};

typedef struct {

    uint16_t		port;
    char			host_name[HOST_ADDRESS_LEN + 1];

    uint32_t		command_timeout_ms;

    unsigned char	*send_buf;
    uint32_t		send_buf_size;
    unsigned char	*read_buf;
    uint32_t 		read_buf_size;

    void (*defaultMessageHandler) (MessageData*);
} xr_mqtt_param_t, *xr_mqtt_param_pt;

typedef struct {
    char client_id[CLIENT_ID_LEN + 1];
    char user_name[USER_NAME_LEN + 1];
    char password[PASSWORD_LEN + 1];
	char host_name[HOST_ADDRESS_LEN + 1];
	uint16_t port;
} aliot_devc_info_t, *aliot_devc_info_pt;

typedef void (*messageHandler)(void*);

typedef struct Client Client;


extern StructTermInfo TermInfo;
extern aliot_devc_info_t xr_devc_info;
extern xr_mqtt_param_t	xr_mqtt_para;

void lpm_set(uint8_t lock_type,uint8_t value);
int mqtt_client_close(Client* c);
void alink_devc_init(void);
void mqtt_para_init(void);
int tcp_mqtt_client(Client* c);
int ssl_mqtt_client(Client* client, 
	const char *server_cert, uint32_t server_cert_len, 
	const char *client_cert, uint32_t client_cert_len,
	const char *client_prikey, uint32_t client_prikey_len);
void aliot_device_info_ex( const char *url,
	const char *product_key,
	const char *device_name,
	const char *device_secret,
	const char *dev_client_id,
	const char *secure_mode
	);
void aliot_device_info(
	const char *product_key,
	const char *device_name,
	const char *device_secret,
	const char *dev_client_id,
	const char *secure_mode);
int set_iot_devc_info(aliot_devc_info_t *devc_info);
void MQTTConnectErr(void *pvParam);
void MQTTConnectSucc(void *pvParam);

int MQTTDisconnect(Client* pstClient);

void messageArrived(char *pszTopic, uchar *pbyRecv, uint wRecvLen);
int check_mqtt_server(Client * c);
int PubTermInfo(char *pszOut);
//#define DUAL_MQTT_SUPPORT

#ifdef __cplusplus
}
#endif

#endif
