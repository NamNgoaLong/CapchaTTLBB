#pragma once


#define MAX_ADDRESS 10
#define MAX_OFFSET 10

#define GAMEPACKET_EXE_INGONE	1
#define GAMEPACKET_EXE_LAST		2

#define MYWM_APP_CAPCHA		WM_USER + 5151
#define NAME_CAPTION_AUTO_CAPCHA	"Auto Capcha TTLBB"
#define NAME_MAP_FILE	"Global\\MyFileMappingObject"
#define MYWM_FROM_MYCAPTCHA_DLL	WM_USER + 0x2333


#define __GAMEDLL_NAME__ "MyCapchaVerM.dll"

#define __AutoFreeLibrary_SetExit g_ModuleExit = true 
#define __AutoFreeLibrary_IsNotExit false == g_ModuleExit

#define __AutoFreeLibrary_EnterProcess  HMODULE _hModule = NULL ; ::GetModuleHandleEx(0 , __GAMEDLL_NAME__ , &_hModule ) 
#define __AutoFreeLibrary_EnterExitProcess HMODULE _hModule = NULL ; ::GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT , __GAMEDLL_NAME__ , &_hModule )
#define __AutoFreeLibrary_LeaveProcess	FreeLibraryAndExitThread( _hModule , 0 )

enum ENUM_DATI_CODE { EDT_GETANSWER , EDT_REPORTERROR };
#define BUF_SIZE_BMP 0x240

typedef struct _stBmpCapcha
{
	char szDataBmp[BUF_SIZE_BMP];
	char szAnser[128];
}stBmpCapcha;

enum LparamMessage{ lParam_Exit_Dll = 333 , lParam_Get_Account = -1 , lParam_Close_Mapfile = 777 , lParam_Recognize_Error = 555 };

	#define TRACE_PACKET_BIN_ANALYZE(src, len) \
	{\
		char *tbuff = (char *)malloc(len * 5);\
		BinToHex(src, len, tbuff, 0, " ", 0x10);\
		free(tbuff);\
	}
