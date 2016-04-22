#pragma once
//#include <windows.h>
#include "nxdati.h"
#include "CIniFile.h"
#include <map>
#include <vector>
#include "macro.h"
#include "Inject.h"


class App : public NXDATI
{
public:
	bool bAutoStart;
	bool bCheckGain;
	bool bSavePass;

	CIniFile* ciniFile; //kiemtra
	static HINSTANCE hInstMain;
	static HWND hwndMain;
	static HWND hwndRegister; // hwnd dialog registry account in NXDati
	static HWND hwndListProcess; // hwnd list view process game running in compute
	static bool bStop; // stop thread
	HBITMAP hBmp;

	Inject objInject;

public:
	App();

	bool setConfigUser( void );
	void getConfigUser( void );

	bool loadProcessGame( void );

	bool enCryptPassword( char *password,  char *outPass );
	bool deCryptPassword( char *password, char *outPass );	

	BOOL readFileMapping( int nCodeName, int nPID, bool bPassCapcha );
	char* createCaptchaBmp( int nPID, HANDLE &hMapFile , bool bCheck, char* lpBuffer, int nWidth, int nHeight, int* nlpOutLen, BOOL IsSaveFile,  ... );

	BOOL createMappingBmp( int nCodeName, HANDLE &hMapFile, stObjRecognize &stObj );
	BOOL readFileMappingBMP( stCodeResult* objCodeRet );
	bool returnObjMapping( int nCodeName, stObjRecognize *stObjRet );

	void loopMessageCapcha( void );


};


App::App()
{
	bAutoStart = false;
	bCheckGain = false;
	bSavePass = false;
	hBmp = NULL;
	NXDATI::NXDATI();	
	Inject::Inject();
	ciniFile = new CIniFile( NAME_FILE_CONFIG );
}
