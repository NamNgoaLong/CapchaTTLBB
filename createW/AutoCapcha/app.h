

#include "Decrypt.h"
#include "CDati.h"
#include "lua.h"


typedef struct _myPathCapcha
{
	char path[MAX_PATH];
}myPathCapcha;

typedef struct _myInfoPlayer
{
	char infoPlayer[100];
}myInfoPlayer;

typedef struct _myResult
{
	char result[256];
	int codeID;
}myResult;

typedef struct _infoPlayer
{
	char szNamePlayer[128];
}infoPlayer;

//------------------------------- 

enum Result{result_nxdati_error = -1, result_nxdati_ok, nx_nameplayer, nx_pass_capcha};
enum RESULT_INIT_ADDR{ ERROR_INIT_OK = 0,  ERROR_INIT_ADDR = -1 , ERROR_INIT_OFFSET = -2 , ERROR_INIT_PACKET = -3 } ;

//---------------------------------------------------

class App
{
private:
	CRITICAL_SECTION m_cs;


public:
	App();
	~App();
	Decrypt decrypt ;
	CDati nxDati ;
	MyLua lua ;

	HANDLE hMappingFile ;
	infoPlayer info ;
	bool m_bLogin ;
	int m_nInitAddrOk ;

	HWND hwndGame ;
	WNDPROC		gameSrcProc ;



public:
	void lock( void ) ;
	void unLock( void ) ;
	bool nxSendMessage( int wParam, int lParam );
	bool nxCreateMappingFile( int nCodeName, bool bPassCapcha );

	
//	LRESULT CALLBACK gameSafeProc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam ) ;
	LRESULT CALLBACK gameAlterProc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam ) ;

	void s_dump_callstack( CONTEXT *context ) ;
	int s_seh_handler(int code , PEXCEPTION_POINTERS p) ;
	HWND getGameWindows( void ) ;

	void init( void ) ;

	void initThreadInfo( void );

	static unsigned int _stdcall updateInfo( LPVOID lParam );
	static unsigned int _stdcall initThreadMain( LPVOID lParam );
	static unsigned int _stdcall threadExitDll( LPVOID lParam );

	void exitDll( void );


};

//--------------------------------------------------------------------
