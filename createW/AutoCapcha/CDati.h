
#include <windows.h>


#define BUF_SIZE_BMP 0x240

typedef struct _stBmpCapcha{
	char szDataBmp[BUF_SIZE_BMP];
	char szAnser[128];
}stBmpCapcha;


enum ENUM_DATI_CODE { EDT_GETANSWER , EDT_REPORTERROR};


class CDati{
public:
	CDati();
//	~CDati();
public:
	ENUM_DATI_CODE m_dwCode;
	stBmpCapcha stDataCaptcha;
	char szCapchaCodeId[128];
	HANDLE hEventDati;
	BOOL anserCapchaDati();
	BOOL reportError();	
	static unsigned __stdcall DaTiProc(LPVOID lParam);
};
