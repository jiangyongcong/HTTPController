#pragma once

typedef void (CALLBACK * CB_PRINT_LOG)( char *_pMyObj, const int _iLogType, const char _szLogDesc[], const char _szLogText[] );
typedef BOOL (WINAPI * FP_InitStartup)( char *_pMyObj, CB_PRINT_LOG  _fpCB_PrintLog, WORD _wListenPort, const char* _szDir );
typedef void (WINAPI * FP_Terminate)();

class CWebServiceImpl
{
public:	
	CWebServiceImpl();
	~CWebServiceImpl();

public:
	BOOL Load_Impl();
	void UnLoad_Impl();
	BOOL InitStartup( char *_pMyObj, CB_PRINT_LOG _fpCB_PrintLog, WORD _wListenPort, const char* _szDir );
	void Terminate();

private:
	HINSTANCE m_hInstance; 
	FP_InitStartup m_InitStartup;
	FP_Terminate m_Terminate;
};