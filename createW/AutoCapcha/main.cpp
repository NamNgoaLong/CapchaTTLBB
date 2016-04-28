#include <windows.h>
//#include "Decrypt.h"
#include "app.h"
//
//#include <new>
//#include <iostream>
//#include <new>
//using namespace std;

App* app = NULL;
bool g_ModuleExit = false;

BOOL WINAPI DllMain( HINSTANCE hmodule, DWORD reason, LPVOID lpreserved )
{
	UNREFERENCED_PARAMETER( hmodule );
	UNREFERENCED_PARAMETER( reason );
	UNREFERENCED_PARAMETER( lpreserved );
	switch (reason){
	case DLL_PROCESS_ATTACH:
		{
//		gInstance = hmodule;
			app = new App();
			app->init();
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			if( app ) delete app ;

		}
		break;
	case DLL_THREAD_ATTACH:		
		break;
	case DLL_THREAD_DETACH:	
		break;
	};
	return	true;
}