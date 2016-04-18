
#include <windows.h>
#include <Psapi.h>
#include <map>
#include "search.h"
#include "Decrypt.h"
#pragma comment( lib, "psapi.lib")

using namespace namespcace_MySearch ;

BOOL Decrypt::initPacketMsgProcess(void){
	MODULEINFO mod;
	DWORD dwBeginAddr = 0, dwEndAddr = 0;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	if(!GetModuleInformation(hProcess, GetModuleHandle(NULL), &mod, sizeof(mod))){
		DPrint("Error GetModuleInformation: %x", GetLastError());
		return false;
	}
	dwBeginAddr = (DWORD)mod.lpBaseOfDll;
	dwEndAddr = (DWORD)mod.lpBaseOfDll + mod.SizeOfImage;
	WORD wCodeMsg = 0;
	DWORD dwCodeMsgAddr = 0, dwCodeExeAddr = 0;
	DWORD dwMaxMsg = 0;
	std::multimap<WORD, DWORD>	listCodeMsg; // list packet need securies
	DPrint("dwBeginAddr: %x, dwEndAddr: %x", dwBeginAddr, dwEndAddr);
	for(DWORD dwSearch = dwBeginAddr; dwSearch + 0x10 < dwEndAddr; dwSearch += 4){
		if(::IsBadReadPtr((void*)dwSearch, sizeof(DWORD)*5))
			continue;
		dwCodeMsgAddr = *(DWORD*)(dwSearch + 0x10);
		dwCodeExeAddr = *(DWORD*)(dwSearch + 0x0c);
		if(dwCodeMsgAddr < dwBeginAddr || dwCodeMsgAddr >= (dwEndAddr - 4))
			continue;

		if(::IsBadCodePtr((FARPROC)(dwCodeMsgAddr)) || ::IsBadCodePtr((FARPROC)(dwCodeExeAddr)))
			continue;
		if(*(BYTE*)(dwCodeMsgAddr + 0) == 0x66 && *(BYTE*)(dwCodeMsgAddr + 1) == 0xb8 && *(BYTE*)(dwCodeMsgAddr + 4) == 0xc3){
			wCodeMsg = *(WORD*)(dwCodeMsgAddr + 2);
			listCodeMsg.insert(std::make_pair(wCodeMsg, dwSearch));
			dwMaxMsg++;
//			DPrint("%x ", dwSearch);
		}
	}
	DPrint("dwMaxMsg %d", dwMaxMsg);

	char szNameStr[64] = {0} ;
	char szNoteStr[64] = {0} ;
	DWORD dwIndex = 0;
	DWORD dwPtrAddr = 0;
	std::multimap<WORD, DWORD>::const_iterator ptr = listCodeMsg.begin();
	for(int i = 0; ptr != listCodeMsg.end(); ptr++, i++){
		wCodeMsg = ptr->first;
		dwPtrAddr = ptr->second;
		dwCodeExeAddr = *(DWORD*)(dwPtrAddr + 0xc);
		if(matchPacketCode(GetModuleHandle(NULL), dwCodeExeAddr, dwIndex, szNameStr, sizeof(szNameStr), szNoteStr, sizeof(szNoteStr))){
			if(dwIndex >= GAME_PACKET_MAXINDEX)
				continue;
		}
		if(dwIndex == GAMEPACKET_RECV_TRAINCAPTCHA){
			DPrint("GAMEPACKET_RECV_TRAINCAPTCHA %x", dwCodeExeAddr);
			m_stPacketData[dwPtrAddr].dwMask = 0;
			m_stPacketData[dwPtrAddr].dwNewAddr = 0;
			m_stPacketData[dwPtrAddr].dwRealAddr = dwCodeExeAddr;
			m_stPacketData[dwPtrAddr].dwIndex = dwIndex;
			m_stPacketData[dwPtrAddr].wUmsg = wCodeMsg;
		}
		
	}
}
BOOL Decrypt::matchPacketCode(HMODULE hModule, DWORD dwFuncAdr, DWORD &uIndex, char *lpOutNameStr, int nNameStrLen, char *lpOutNoteStr, int nNoteStrLen){
	uIndex = INFINITE;
	BOOL bIsReturn = FALSE;
	DWORD dwCall1 = 0, dwTmp = 0, dwAddr = 0;
	dwCall1 = _SRCforModule((HMODULE)hModule, 1, dwFuncAdr, dwFuncAdr+0x20, -7 , "83c408", 0, "", NULL, NULL );	
	if(::IsBadCodePtr((FARPROC)dwCall1)){
		return FALSE;
	}
	if(0 != _SRCforModule((HMODULE)hModule, 0, dwCall1, dwCall1 + 0x50, 0 , "8b75088b8e740200008b56105152", 0, "", NULL, NULL )){
		if(uIndex != INFINITE)
			DPrint("Goi tin bi trung: %d", uIndex);
		uIndex = GAMEPACKET_RECV_TRAINCAPTCHA;
		return TRUE;
	}
	return FALSE;
}