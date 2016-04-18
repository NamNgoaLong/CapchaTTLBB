#include "macro.h"
#include <windows.h>



class AppDll{
public:
	char szAnserCapcha[128];
public:
	BOOL hookGameRecv(void);
	int _fastcall onInlineRecv(DWORD pPacketObj , DWORD pPacket , DWORD pNetman);
	void onRecvEx(char* lpBuffer , int uIndex , int uMsg , DWORD& dwOutExeFlag );
	BOOL gameDecodeCaptchaData(DWORD dwKey, char* lpBuffer, int len);
};