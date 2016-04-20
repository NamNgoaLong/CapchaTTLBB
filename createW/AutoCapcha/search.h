#pragma once

#define SIZE_OF_NT_SIGNATURE sizeof(DWORD)
#define OPTHDROFFSET(a) ((LPVOID)((BYTE *)a + \
	((PIMAGE_DOS_HEADER)a)->e_lfanew + \
	SIZE_OF_NT_SIGNATURE + \
	sizeof(IMAGE_FILE_HEADER)))

namespace namespcace_MySearch
{

	void DPrint( char *format, ... )
	{
		va_list arg_ptr;
		char outstr[0x1000];
		va_start( arg_ptr, format );
		vsprintf_s( outstr, sizeof(outstr), format, arg_ptr );
		OutputDebugStringA( outstr );
		va_end( arg_ptr );
	}

	DWORD SearchFor( char *markCode, int checkOffset, char *checkCode, 
		DWORD beginAddr = 0x00400000, DWORD endAddr = 0x7FFFFFFF )
	{
		DWORD _break = 0;
		int mlen;
		int clen;
		BYTE* mcode = NULL;
		BYTE* ccode = NULL;
		DWORD tmpAddr = beginAddr;

		if ( markCode == NULL )
		{
			DPrint(TEXT("特征码指针为空！\r\n"));
			return 0;
		}

		if ( strlen(markCode) < 2 )
		{
			DPrint(TEXT("特征码长度错误！\r\n"));
			return 0;
		}

		if ( strlen(markCode ) % 2 != 0) 
		{
			DPrint(TEXT("特征码格式错误！%d\r\n") , strlen(markCode));
			return 0;
		}

		if ( checkOffset != 0 && checkCode )
		{
			if (strlen(checkCode) % 2 != 0) 
			{
				DPrint(TEXT("检测特征码格式错误！\r\n"));
				return 0;
			}
		}

		mlen = strlen(markCode) / 2;
		mcode = new BYTE[mlen];
		for (int i = 0; i < mlen; i++)
		{
			char c[] = {markCode[i*2], markCode[i*2+1], '\0'};
			mcode[i] = (BYTE)strtol(c, NULL, 16); // Moi mcode = 2 kytu = 1 thanh ghi 16bit
		}

		if ( checkOffset && checkCode )
		{
			clen = strlen(checkCode) / 2;
			ccode = new BYTE[clen];
			for (int i = 0; i < clen; i++)
			{
				char c[] = {checkCode[i*2], checkCode[i*2+1], '\0'};
				ccode[i] = (BYTE)strtol(c, NULL, 16);
			}
		}

		__try
		{
			while ( tmpAddr <= endAddr - mlen )
			{
				if ( IsBadReadPtr((PVOID)tmpAddr, mlen) == 0 )
				{
					if ( memcmp((PVOID)tmpAddr, mcode, mlen) == 0 )
					{
						if ( checkOffset != 0 )
						{
							if ( memcmp((PVOID)(tmpAddr+checkOffset), ccode, clen) == 0 )
							{
								_break = tmpAddr + mlen; 
								break;
							}
						}
						else
						{
							_break = tmpAddr + mlen; //1mlen = 2byte
							break;
						}
					}
				}
				tmpAddr++;
			}
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			DPrint("%s 产生异常!!\n", __FUNCTION__);
		}

		delete mcode;
		if ( ccode ) delete ccode;

		return _break;
	}

	//直接读取PE头.有的程序加壳后用GetModuleInformation得到的信息错误.
	BOOL MyGetModuleInformation(HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb)
	{
		if (IsBadReadPtr((LPCVOID)hModule, sizeof(HMODULE)))
			return FALSE;

		if (IsBadWritePtr((LPVOID)lpmodinfo, cb))
			return FALSE;

		PIMAGE_OPTIONAL_HEADER poh;
		poh = (PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(hModule);
		if (poh != NULL)
		{
			lpmodinfo->EntryPoint = (LPVOID)poh->AddressOfEntryPoint;
			lpmodinfo->lpBaseOfDll = (LPVOID)poh->ImageBase;
			lpmodinfo->SizeOfImage = poh->SizeOfImage;
			return TRUE;
		}
		else
			return FALSE;
	}


//_SRCforModule((HMODULE)hModule, 1, dwFuncAdr, dwFuncAdr+0x20, -7 , "83c408", 0, "", &dwAdr , NULL );

	DWORD _SRCforModule(HMODULE hModule, int findMode, DWORD Start, DWORD End, 
		int Offset, char *markCode, int checkOffset, char *checkCode, 
		LPDWORD dwAddr, LPDWORD dwOffset)
	{
		MODULEINFO mod;
		DWORD	beginAddr;
		DWORD	endAddr;
		DWORD	addr;
		DWORD	_break = 0;

		if ( hModule && markCode != NULL )
		{
			if ( GetModuleInformation(GetCurrentProcess(), hModule, &mod, sizeof(mod)) )
				//if ( MyGetModuleInformation(hModule, &mod, sizeof(mod)) )
			{
				beginAddr = /*(DWORD)mod.lpBaseOfDll + */Start; 
				endAddr = /*(DWORD)mod.lpBaseOfDll + mod.SizeOfImage - */End;

				//TRACE_DEBUG("SearchFor,markCode=%s, checkOffset=%08x, checkCode=%08x, beginAddr=%08x, endAddr=%08x",markCode, checkOffset, checkCode, beginAddr, endAddr);
				addr = SearchFor(markCode, checkOffset, checkCode, beginAddr, endAddr);
// Gia tri tra ve addr = address + mlen(=16bit) [Tuc la addr = dia chi tai cuoi chuoi]

//_SRCforModule(				"83c408",		0,			"",		begin,		end)
				if ( addr )
				{
					if ( findMode == 0 )
					{
						_break = *(DWORD*)(addr + Offset);
					}
					else
					{
						_break = *(DWORD*)(addr + Offset) + (addr + Offset) + 4;
					}
					if ( dwAddr ) *dwAddr = addr + Offset;
					if ( dwOffset ) *dwOffset = (_break - (DWORD)mod.lpBaseOfDll) + Offset; // Tinh chieu dai offset
				}
			}
		}
		return _break;
	}

	DWORD SRCforModule(HMODULE hModule, int findMode, DWORD Start, DWORD End, 
		int Offset, char *markCode, int checkOffset, char *checkCode, 
		LPDWORD dwAddr, LPDWORD dwOffset)
	{
		MODULEINFO mod;
		DWORD	beginAddr;
		DWORD	endAddr;
		DWORD	addr;
		DWORD	_break = 0;

		if ( hModule && markCode != NULL )
		{
			if ( GetModuleInformation(GetCurrentProcess(), hModule, &mod, sizeof(mod)) )
				//if ( MyGetModuleInformation(hModule, &mod, sizeof(mod)) )
			{
				beginAddr = (DWORD)mod.lpBaseOfDll + Start; 
				endAddr = (DWORD)mod.lpBaseOfDll + mod.SizeOfImage - End;

				//TRACE_DEBUG("SearchFor,markCode=%s, checkOffset=%08x, checkCode=%08x, beginAddr=%08x, endAddr=%08x",markCode, checkOffset, checkCode, beginAddr, endAddr);
				addr = SearchFor(markCode, checkOffset, checkCode, beginAddr, endAddr);

				if ( addr )
				{
					if ( findMode == 0 )
					{
						_break = *(DWORD*)(addr + Offset);
					}
					else
					{
						_break = *(DWORD*)(addr + Offset) + (addr + Offset) + 4;
					}
					if ( dwAddr ) *dwAddr = addr + Offset;
					if ( dwOffset ) *dwOffset = (_break - (DWORD)mod.lpBaseOfDll) + Offset;
				}
			}
		}
		return _break;
	}
}