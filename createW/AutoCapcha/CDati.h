
#include <windows.h>
//#include "app.h"
#include "macro.h"

/*
#define BUF_SIZE_BMP 0x240

typedef struct _stBmpCapcha
{
	char szDataBmp[BUF_SIZE_BMP];
	char szAnser[128];
}stBmpCapcha;

*/

class CDati
{
private:
//	CRITICAL_SECTION m_cs ;

public:
	ENUM_DATI_CODE m_dwCode;
	stBmpCapcha stDataCaptcha;
	char szCapchaCodeId[128];
	int m_result;
	HANDLE hEventDati;

public:
	CDati();
	~CDati();

//	void lock( void );
//	void unLock( void );

	bool anserCapchaDati();
	bool reportError();	
	static unsigned __stdcall DaTiProc( LPVOID lParam );
};
