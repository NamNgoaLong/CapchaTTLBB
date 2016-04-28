#include <iostream>
#include <Windows.h> 
#include "CIniFile.h"

CIniFile::CIniFile( char *szFileName ) : CIniReader( szFileName ), CIniWriter( szFileName ){
}
//------------------------------------

void CIniFile::CIniFileWrInteger( char *szSection, char *szKey, int iValue )
{
	WriteInteger( szSection, szKey, iValue );
}
void CIniFile::CIniFileWrFloat( char* szSection, char* szKey, float fltValue ) 
{
	WriteFloat( szSection, szKey, fltValue );
}
void CIniFile::CIniFileWrBoolean( char* szSection, char* szKey, bool bolValue )
{
	WriteBoolean( szSection, szKey, bolValue );
}
void CIniFile::CIniFileWrString( char* szSection, char* szKey, char* szValue )
{
	WriteString( szSection, szKey, szValue );
}
//----------------------------------------------
int CIniFile::CIniFileReInteger( char* szSection, char* szKey, int iDefaultValue )
{
	return ReadInteger( szSection, szKey, iDefaultValue );
}
float CIniFile::CIniFileReFloat( char* szSection, char* szKey, float fltDefaultValue )
{
	return ReadFloat( szSection, szKey, fltDefaultValue );
}
bool CIniFile::CIniFileReBoolean( char* szSection, char* szKey, bool bolDefaultValue )
{
	return ReadBoolean( szSection, szKey, bolDefaultValue );
}

char* CIniFile::CIniFileReString( char* szSection, char* szKey, const char* szDefaultValue )
{
	return ReadString( szSection, szKey, szDefaultValue );
}

//************************************************
CIniReader::CIniReader( char* szFileName )
{
	memset( m_szFileName, 0x00, 255 );
	memcpy( m_szFileName, szFileName, strlen( szFileName ) + 1 );
}
int CIniReader::ReadInteger( char* szSection, char* szKey, int iDefaultValue )
{
	int iResult = GetPrivateProfileInt( szSection,  szKey, iDefaultValue, m_szFileName ); 
	return iResult;
}
float CIniReader::ReadFloat( char* szSection, char* szKey, float fltDefaultValue )
{
	char szResult[255];
	char szDefault[255];
	float fltResult;
	sprintf_s( szDefault, sizeof( szDefault ) , "%f", fltDefaultValue );
	GetPrivateProfileString( szSection,  szKey, szDefault, szResult, 255, m_szFileName ); 
	fltResult =  (float)atof( szResult );
	return fltResult;
}
bool CIniReader::ReadBoolean( char* szSection, char* szKey, bool bolDefaultValue )
{
	char szResult[255];
	char szDefault[255];
	bool bolResult;
	sprintf_s( szDefault, sizeof(szDefault), "%s", bolDefaultValue ? "True"  :  "False" );
	GetPrivateProfileString( szSection, szKey, szDefault, szResult, 255, m_szFileName ); 
	bolResult =  ( strcmp( szResult, "True" ) == 0 || 
		strcmp( szResult, "true" ) == 0 ) ? true : false;
	return bolResult;
}
char* CIniReader::ReadString( char* szSection, char* szKey, const char* szDefaultValue )
{
	char* szResult = new char[255];
	memset( szResult, 0x00, 255 );
	GetPrivateProfileString( szSection,  szKey, 
		szDefaultValue, szResult, 255, m_szFileName ); 
	return szResult;
}
//**********************************************
CIniWriter::CIniWriter( char* szFileName )
{
	memset( m_szFileName, 0x00, 255 );
	memcpy( m_szFileName, szFileName, strlen( szFileName ) + 1 );
}
void CIniWriter::WriteInteger( char* szSection, char* szKey, int iValue )
{
	char szValue[255];
	sprintf_s( szValue, sizeof( szValue ), "%d", iValue );
	WritePrivateProfileString( szSection,  szKey, szValue, m_szFileName ); 
}
void CIniWriter::WriteFloat( char* szSection, char* szKey, float fltValue )
{
	char szValue[255];
	sprintf_s( szValue, sizeof( szValue ), "%f", fltValue );
	WritePrivateProfileString( szSection,  szKey, szValue, m_szFileName ); 
}
void CIniWriter::WriteBoolean( char* szSection, char* szKey, bool bolValue )
{
	char szValue[255];
	sprintf_s( szValue, sizeof( szValue ), "%s", bolValue ? "True" : "False" );
	WritePrivateProfileString( szSection,  szKey, szValue, m_szFileName ); 
}
void CIniWriter::WriteString( char* szSection, char* szKey, char* szValue )
{
	WritePrivateProfileString( szSection,  szKey, szValue, m_szFileName );
}
