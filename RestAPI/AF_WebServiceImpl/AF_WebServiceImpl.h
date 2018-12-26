// AF_WebServiceImpl.h : AF_WebServiceImpl DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号
#include "HttpManager.h"

// CAF_WebServiceImplApp
// 有关此类实现的信息，请参阅 YxAF_WebServiceImpl.cpp
//
typedef void (CALLBACK * CB_PRINT_LOG)( char *_pMyObj, const int _iLogType, const char _szLogDesc[], const char _szLogText[] );

enum _enLogType
{
	_enComm = 0,	//普通日志
	_enWaring = 1,  //警告日志
	_enError = 2,	//错误日志
};

typedef struct _tagLogObject
{
	int			m_iLogType;
	std::string m_strLogDesc;
	std::string m_strLogText;
}CLogObject;

class CAF_WebServiceImpl
{
public:
	CAF_WebServiceImpl(void);
	~CAF_WebServiceImpl(void);


public:
	int  Init_Startup( char *_pMyObj, CB_PRINT_LOG _fpCB_PrintLog,  WORD _wListenPort, const char* _szDir );
	void Stop_Release();
	void toPrintLog( int _iLogType, const char _szLogText[] );
	void OnTimer_100ms();
	void OnTimer_LogOutPut();

	std::string& GetDir();

private:
	void _ClearLogObject();

private:
	// 日志输出相关
	char					*m_pMyObj;
	CB_PRINT_LOG			m_fpCB_PrintLog;
	std::list<CLogObject*>	m_lstLogObject;
	CRITICAL_SECTION		m_csLog;
	HANDLE					m_hTimerThread_100ms;				
	unsigned int			m_uiTimerThreadID;	
	bool					m_bExit;// 结束运行
	bool					m_bInitSucceed; //是否初始化成功
	HANDLE					m_hExitEvent;// 等待线程退出事件

	std::string  m_strDir;
	WORD  m_wLisenPort;

private:
	CHttpManager m_HttpManager;
};

class CAF_WebServiceImplApp : public CWinApp
{
public:
	CAF_WebServiceImplApp();

	// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

public:
	CAF_WebServiceImpl m_AF_WebServiceImpl;
	
};

