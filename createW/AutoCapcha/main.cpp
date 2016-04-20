#include <windows.h>
//#include "Decrypt.h"
#include "app.h"

App* app = NULL;
bool g_ModuleExit = false;

BOOL WINAPI DllMain(HINSTANCE hmodule,DWORD reason,LPVOID lpreserved)
{
	switch (reason){
	case DLL_PROCESS_ATTACH:
		{
//		gInstance = hmodule;
			app = new App();
		}break;
	case DLL_PROCESS_DETACH:			
//		if (app) delete app;
//		DPrint("DllMain Exit...") ;
		break;
	case DLL_THREAD_ATTACH:		
		break;
	case DLL_THREAD_DETACH:	
		break;
	};
	return	true;
}