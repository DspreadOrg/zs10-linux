#ifndef _KEY_H_
#define _KEY_H_

#include "YMAPI.h"

#define KEY_CODE_VOL_DOWN (K_VOL_DOWN)
#define KEY_CODE_VOL_UP (K_VOL_UP)
#define KEY_CODE_CONF (K_MENU)
#define KEY_CODE_PWRKEY (K_POWER)
#define KEY_CODE_UNKNOWN (0xFE)
#define KEY_VALUE_PRESS (EM_KEY_SHORTPRESS)
#define KEY_VALUE_RELEASE (EM_KEY_RELEASE)


int KeyTaskInit(void);


#endif
