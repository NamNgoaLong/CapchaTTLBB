

#include "Decrypt.h"
#include "CDati.h"



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
//---------------------------------------------------

class App
{
public:
	App();
	~App();
	Decrypt decrypt;
	CDati nxDati;
	HANDLE hMappingFile;
	infoPlayer info;
	bool m_bLogin;
	bool m_bInitAddrOk;
	static unsigned int _stdcall updateInfo( LPVOID lParam );
//	static unsigned int _stdcall initAddrData( LPVOID lParam );
//	void initData(void);

public:
	bool nxSendMessage( int wParam, int lParam );
	BOOL nxCreateMappingFile( int nCodeName, bool bPassCapcha );

};

//--------------------------------------------------------------------
