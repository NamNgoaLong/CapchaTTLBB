#pragma once
#include <iostream>
#include <Windows.h> 



class CIniReader
{
public:
	CIniReader( char* szFileName ); 
	int ReadInteger( char* szSection, char* szKey, int iDefaultValue );
	float ReadFloat( char* szSection, char* szKey, float fltDefaultValue );
	bool ReadBoolean( char* szSection, char* szKey, bool bolDefaultValue );
	char* ReadString( char* szSection, char* szKey, const char* szDefaultValue );
private:
	char m_szFileName[255];
};

//***************************************************
class CIniWriter
{
public:
	CIniWriter( char* szFileName ); 
	void WriteInteger( char* szSection, char* szKey, int iValue );
	void WriteFloat( char* szSection, char* szKey, float fltValue );
	void WriteBoolean( char* szSection, char* szKey, bool bolValue );
	void WriteString( char* szSection, char* szKey, char* szValue );
private:
	char m_szFileName[255];
};

//*********************************************
class CIniFile: public CIniReader, CIniWriter
{
public:
	CIniFile( char* szFileName );
	void CIniFileWrInteger( char* szSection, char* szKey, int iValue );
	void CIniFileWrFloat( char* szSection, char* szKey, float fltValue );
	void CIniFileWrBoolean( char* szSection, char* szKey, bool bolValue );
	void CIniFileWrString( char* szSection, char* szKey, char* szValue );

	int CIniFileReInteger( char* szSection, char* szKey, int iDefaultValue );
	float CIniFileReFloat( char* szSection, char* szKey, float fltDefaultValue );
	bool CIniFileReBoolean( char* szSection, char* szKey, bool bolDefaultValue );
	char* CIniFileReString( char* szSection, char* szKey, const char* szDefaultValue );

};
