// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__A7F682B7_9100_4345_B232_701448320253__INCLUDED_)
#define AFX_INIFILE_H__A7F682B7_9100_4345_B232_701448320253__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


const int STR_VALUE_LEN = 512;

class CIniFile
{
public:
    CIniFile();
    virtual ~CIniFile();

public:
	bool SetFileName(const char* _szFileName);
    int GetProfileString(const char* _szSection, const char* _szKey, const char* _szDefault, char* _szResult, int _nLen);	
    int GetProfileInt(const char* _szSection, const char* _szKey, const int _nDefault);

    int WriteProfileString(const char*, const char*, const char*);
    int WriteProfileInt(const char*, const char*, int );

private:
	char  m_szModuleDir[MAX_PATH];  //路径中已包含路径符 /
    char  m_szFileName[MAX_PATH];

};

#endif // !defined(AFX_INIFILE_H__A7F682B7_9100_4345_B232_701448320253__INCLUDED_)
