#pragma once


#define MAX_ADDRESS 10
#define MAX_OFFSET 10

#define GAMEPACKET_EXE_INGONE	1
#define GAMEPACKET_EXE_LAST		2

#define MYWM_APP_CAPCHA		WM_USER + 5151
#define NAME_CAPTION_AUTO_CAPCHA	"Auto Capcha TTLBB"
#define NAME_MAP_FILE	"Global\\MyFileMappingObject"
#define MYWM_MESSAGE	WM_USER + 0x2333


#define __AutoFreeLibrary_SetExit g_ModuleExit = true 
#define __AutoFreeLibrary_IsNotExit false == g_ModuleExit