// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"
#include <stdio.h>

// #ifdef _DEBUG
// #undef THIS_FILE
// static char THIS_FILE[]=__FILE__;
// #define new DEBUG_NEW
// #endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile()
{
	char   szDriver[MAX_PATH] = {0};
	char   szDir[MAX_PATH] = {0};
	char   szFName[MAX_PATH] = {0};
	char   szExt[MAX_PATH] = {0};

	::GetModuleFileName( NULL, m_szModuleDir, sizeof(m_szModuleDir) );
	::_splitpath_s( m_szModuleDir,szDriver,sizeof(szDriver),szDir,sizeof(szDir),szFName,sizeof(szFName),szExt,sizeof(szExt));
	::_makepath_s( m_szModuleDir,sizeof(m_szModuleDir), szDriver, szDir, NULL, NULL ); //路径中已包含路径符 /
}

CIniFile::~CIniFile()
{
	//delete[] m_szFileName;
}


bool CIniFile::SetFileName(const char* _szFileName)
{
	//char szDir[500] = {0};
	//::GetCurrentDirectory(500, szDir);  //不包含目录符路径符/，而且会受打开文件对话框选择的路径所影响

	//m_szFileName = new char[strlen(szDir)+strlen(_szFileName)+1+1];
	strcpy(m_szFileName, m_szModuleDir);
	//strcat(m_szFileName, "\\");//+1
	strcat(m_szFileName, _szFileName);

	return true;
}

int CIniFile::GetProfileString(const char* _szSection, const char* _szKey, const char* _szDefault, char* _szResult, int _nLen)
{
	return	GetPrivateProfileString(_szSection, _szKey, _szDefault, _szResult, _nLen, m_szFileName);
}

int CIniFile::GetProfileInt(const char* _szSection, const char* _szKey, const int _nDefault)
{
	return GetPrivateProfileInt(_szSection, _szKey, _nDefault, m_szFileName);
}

int CIniFile::WriteProfileString(const char* _szSection, const char* _szKey, const char* _szText)
{
	return WritePrivateProfileString(_szSection, _szKey, _szText, m_szFileName);
}

int CIniFile::WriteProfileInt(const char* _szSection, const char*_szKey, int _iValue)
{
	char v_sztemp[STR_VALUE_LEN] = {0};
	sprintf(v_sztemp, "%d",_iValue);
	return WritePrivateProfileString(_szSection,_szKey,v_sztemp,m_szFileName);
}

