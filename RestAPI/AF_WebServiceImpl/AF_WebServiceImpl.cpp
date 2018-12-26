// AF_WebServiceImpl.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "AF_WebServiceImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CAF_WebServiceImplApp

BEGIN_MESSAGE_MAP(CAF_WebServiceImplApp, CWinApp)
END_MESSAGE_MAP()


// CAF_WebServiceImplApp 构造

CAF_WebServiceImplApp::CAF_WebServiceImplApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CAF_WebServiceImplApp 对象

CAF_WebServiceImplApp theApp;

BOOL WINAPI DLL_InitStartup( char *_pMyObj, CB_PRINT_LOG _fpCB_PrintLog, WORD _wListenPort, const char* _szDir )
{
	if( theApp.m_AF_WebServiceImpl.Init_Startup( _pMyObj, _fpCB_PrintLog, _wListenPort,_szDir ) == -1 )
	{
		return FALSE;
	}

	return TRUE;
}

void WINAPI DLL_Terminate()
{
	theApp.m_AF_WebServiceImpl.Stop_Release();
}


// CAF_WebServiceImplApp 初始化

BOOL CAF_WebServiceImplApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

// 定时线程
UINT WINAPI g_OnTimes_100ms(void* v_pParam)
{
	CAF_WebServiceImpl* pInstance =(CAF_WebServiceImpl*)v_pParam;
	pInstance->OnTimer_100ms();

	::_endthreadex(0);

	return 0;
}

CAF_WebServiceImpl::CAF_WebServiceImpl(void)
{
	// 日志输出相关
	m_fpCB_PrintLog = NULL;
	m_pMyObj = NULL;
	m_hTimerThread_100ms = NULL;				
	m_uiTimerThreadID = 0;	
	m_bExit = false;
	m_bInitSucceed = false;
	m_hExitEvent = NULL;

	m_lstLogObject.clear();

	::InitializeCriticalSection(&m_csLog);
}

CAF_WebServiceImpl::~CAF_WebServiceImpl(void)
{
	_ClearLogObject();

	::DeleteCriticalSection(&m_csLog);
}

int CAF_WebServiceImpl::Init_Startup( char *_pMyObj, CB_PRINT_LOG _fpCB_PrintLog, WORD _wListenPort, const char* _szDir )
{
	if( m_bInitSucceed )
	{
		return 0;
	}
	g_pAF_WebServiceImpl = this; 

	m_pMyObj = _pMyObj;
	m_fpCB_PrintLog = _fpCB_PrintLog;
	m_wLisenPort = _wListenPort;
	m_strDir.assign(_szDir);
	int nLen = (int)m_strDir.length();
	if ( '\\'!=m_strDir[nLen-1] )
		m_strDir.append("\\");

	// 创建等待退出时间
	m_hExitEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( m_hExitEvent == NULL )
	{
		return -1;
	}

	// 启动日志输出线程
	m_hTimerThread_100ms = (HANDLE)_beginthreadex( NULL ,0 ,g_OnTimes_100ms ,(void*)this ,0 ,&m_uiTimerThreadID);
	if( NULL == m_hTimerThread_100ms )
	{
		return -1;
	}

	//##>>初始化HTTP通信层
	if( -1 == m_HttpManager.Init( _wListenPort ) )
	{
		toPrintLog(_enError,"[CAF_WebServiceImpl::Init_Startup]: 初始化通信层失败!");
		return -1;
	}
	toPrintLog(_enComm,"[CAF_WebServiceImpl::Init_Startup]: 初始化通信层成功...");

	toPrintLog(_enComm,"[CAF_WebServiceImpl::Init_Startup]: 初始化成功...");

	return 0;
}

void CAF_WebServiceImpl::Stop_Release()
{
	// 标志线程退出
	m_bExit = true;

	::WaitForSingleObject( m_hExitEvent, INFINITE );

	if( m_hExitEvent != NULL )
	{// 关闭时间句柄
		::CloseHandle(m_hExitEvent);
	}

	m_HttpManager.Release();

	_ClearLogObject();
}

void CAF_WebServiceImpl::OnTimer_100ms()
{
	std::list<CLogObject*> clsLstLog;
	CLogObject* pLogObj = NULL;

	static LONG lCount = 0;
	while(true)
	{
		Sleep(100);  // 100ms统一输出一次

		if( m_bExit )
		{
			::SetEvent(m_hExitEvent);
			return;
		}

		OnTimer_LogOutPut();

		if( ::InterlockedIncrement( &lCount ) == 10 )
		{
			m_HttpManager.TimerService_1000ms();

			::InterlockedExchange(&lCount,0);
		}
	}
}

void CAF_WebServiceImpl::OnTimer_LogOutPut()
{
	std::list<CLogObject*> clsLstLog;
	CLogObject* pLogObj = NULL;
	clsLstLog.clear();

	::EnterCriticalSection(&m_csLog);
	{
		if( m_lstLogObject.size() == 0 )
		{
			::LeaveCriticalSection(&m_csLog);
		}

		for( int i=0; i<50 && !m_lstLogObject.empty(); i++ )
		{
			pLogObj = m_lstLogObject.front();

			if( pLogObj == NULL )
			{
				m_lstLogObject.pop_front();
				continue;
			}
			clsLstLog.push_back(pLogObj);
			m_lstLogObject.pop_front();
		}
	}
	::LeaveCriticalSection(&m_csLog);

	while( !clsLstLog.empty() )
	{
		pLogObj = clsLstLog.front();
		clsLstLog.pop_front();

		if( pLogObj == NULL )
		{
			continue;
		}

		if( m_fpCB_PrintLog != NULL )
		{
			m_fpCB_PrintLog( m_pMyObj, pLogObj->m_iLogType,pLogObj->m_strLogDesc.c_str(),pLogObj->m_strLogText.c_str());
		}

		if( pLogObj != NULL )
		{
			delete pLogObj;
			pLogObj = NULL;
		}
	}
	clsLstLog.clear();
}

std::string& CAF_WebServiceImpl::GetDir()
{
	return m_strDir;
}

void CAF_WebServiceImpl::toPrintLog( int _iLogType, const char _szLogText[] )
{
	CLogObject *pLogObj = NULL;
	try
	{
		pLogObj = new CLogObject();
		pLogObj->m_iLogType = _iLogType;
		pLogObj->m_strLogDesc = "AF_WebServiceImpl.dll";
		pLogObj->m_strLogText = _szLogText;

		::EnterCriticalSection(&m_csLog);
		m_lstLogObject.push_back(pLogObj);
		::LeaveCriticalSection(&m_csLog);
	}
	catch(...)
	{
		if( pLogObj != NULL )
		{
			delete pLogObj;
			pLogObj = NULL;
		}
	}
}

void CAF_WebServiceImpl::_ClearLogObject()
{
	::EnterCriticalSection(&m_csLog);
	{
		CLogObject *pLogObj = NULL;
		while( !m_lstLogObject.empty() )
		{
			pLogObj = m_lstLogObject.front();
			if( pLogObj != NULL )
			{
				delete pLogObj;
				pLogObj = NULL;
			}

			m_lstLogObject.pop_front();
		}
	}
	::LeaveCriticalSection(&m_csLog);

	m_lstLogObject.clear();
}