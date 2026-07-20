#ifndef GLOBAL_XL_H
#define GLOBAL_XL_H

#include "vxlapi.h"

#define RX_QUEUE_SIZE     1024
#define RX_QUEUE_SIZE_FD  2048
#define ENABLE_CAN_FD_MODE_NO_ISO 0

extern char g_AppName[];
extern XLportHandle g_xlPortHandle;
extern XLapiIDriverConfigV1 g_xlDrvConfig;
extern XLdeviceDrvConfigListV1 g_xlDevConfig;
extern XLchannelDrvConfigListV1 g_xlChannelConfig;

extern XLaccess g_xlChannelIndex;
extern unsigned int g_BaudRate;
extern unsigned int g_canFdSupport;
extern unsigned int g_canFdModeNoIso;

#endif // GLOBAL_XL_H
