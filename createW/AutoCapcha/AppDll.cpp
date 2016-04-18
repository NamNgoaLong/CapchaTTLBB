#include "macro.h"
#include <windows.h>
#include "AppDll.h"
#include "Decrypt.h"
#include <map>
#include "cbuff.h"

//std::map<DWORD, stDataPacket> Decrypt::m_stPacketData;
BOOL AppDll::gameDecodeCaptchaData(DWORD dwKey, char* lpBuffer, int len){
	DWORD dwGameDecodeCaptchaDataCall = Decrypt::m_dwDatatAddr[GAMEADR_DECODE_CAPTCHA_DATA_CALL] ;
	__asm
	{
		push len
			push lpBuffer
			push dwKey
			push 0
			call dwGameDecodeCaptchaDataCall
			add esp, 10h
	}
	return TRUE;
}
void onRecvEx(char* lpBuffer , int uIndex , int uMsg , DWORD& dwOutExeFlag ){
	CBuff buf ;
	char byTemp[0x300] = {0};
	char byCaptchaData[0x240];
	memset( byTemp , 0 , sizeof(byTemp) ) ;
	memset( byCaptchaData , 0 , sizeof(byCaptchaData) ) ;

	switch( uIndex )
	{
	case GAMEPACKET_RECV_TRAINCAPTCHA:
		{
			buf.add_back(lpBuffer, 0x278);
//			buf.seek_set(0);
			buf.seek_set(0x10);
			DWORD dwFlag = buf.readd();

			memset( byTemp , 0 , sizeof(byTemp) ) ;
			buf.seek_set(0x14);
			buf.readm( byTemp , 32 ) ;//8*4 =32

			buf.seek_set(0x34) ;
			buf.readm(byCaptchaData, 0x240);

			buf.seek_set(0x274) ;
			DWORD dwKey = buf.readd();
/*
			if(dwFlag % 3 == 1){
				gameDecodeCaptchaData(dwKey, byCaptchaData, 0x240);
			}else if(dwFlag % 3 == 2){
				char byTempCaptch[0x240] = {0} ;
				memcpy( byTempCaptch , byCaptchaData , 0x240 ) ;
				gameDecodeCaptchaData_2(dwKey, byCaptchaData, 0x240);
			}
*/
			::WideCharToMultiByte(936,0,(LPCWSTR)byTemp,16, szAnserCapcha,sizeof(szAnserCapcha),NULL,FALSE ) ; 
			DPrint("Anser Reviev: %s", szAnser);

		}break;
	}
}

int AppDll::onInlineRecv(DWORD pPacketObj , DWORD pPacket , DWORD pNetman){
	std::map<DWORD, stDataPacket>::const_iterator ptr = Decrypt::m_stPacketData.find(pPacketObj);

	if(ptr == Decrypt::m_stPacketData.end()){
		return 2;
	}
	typedef int(*_call)(void);
	DWORD dwUmsgR = ((_call)(*(DWORD*)(pPacketObj + 0x10)))();
	dwUmsgR = dwUmsgR & 0xffff;
	if(dwUmsgR != ptr->second.wUmsg){
		return 2;
	}

	DWORD dwRCall = ptr->second.dwRealAddr;
	if(0 == dwRCall || INFINITE == dwRCall){
		return 2;
	}
	typedef int(*_ExeCall)(DWORD);
	int nResult = 2;
	DWORD dwMask = ptr->second.dwMask;
	if(!(dwMask & GAMEPACKET_EXE_LAST)){
		__asm
		{
			mov eax , pNetman
			push eax
			mov ecx , pPacket
			call dwRCall
			mov nResult , eax
		} ;
	
	}

	DWORD dwIngone = 0;
	onRecvEx( (char*)pPacket , it_packet->second.index , uMsgR , dwIngone) ;
	if(dwMask & GAMEPACKET_EXE_LAST){
		if(! (dwIngone & GAMEPACKET_EXE_INGONE)){
			__asm
			{
				mov eax , pNetman
				push eax
				mov ecx , pPacket
				call dwRCall
				mov nResult , eax
			} ;		
		}else
			DPrint("Igone Msg %04x" , uMsgR ) ;
	}
	return nResult;
}

void _declspec(naked) onInlineRecvHead( DWORD pNetman ){
	DWORD pPacket  ;
	DWORD pPacketObj ;
	__asm
	{
		mov pPacket, ecx 
		mov pPacketObj , edx
	} ;
	
	DPrint("/------- %08x -------\\" , pPacketObj ) ;
	OnInlineRecv( pPacketObj , pPacket , pNetman ) ;
	DPrint("\\-------------------/") ;

	__asm
	{
		ret
	}

}
BOOL AppDll::hookGameRecv(){
	std::map<DWORD, stDataPacket>::iterator it = Decrypt::m_stPacketData.begin();
	for(; it != Decrypt::m_stPacketData.end(); it++){
		if(it->second.dwRealAddr != INFINITE && it->second.dwRealAddr != 0)	{
//			it->second.dwNewAddr
		}
	}
}

