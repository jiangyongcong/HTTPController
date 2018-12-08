#include "StdAfx.h"
#include "httpcontroller.h"

namespace HTTP_CC
{
	CHttpController::CHttpController(void)
	{
	}

	CHttpController::~CHttpController(void)
	{
	}

	void CHttpController::Start( CBusinessHandler *v_lpHandler )
	{
		WSAData	wsData;
		WSAStartup(MAKEWORD(2,2),&wsData);

		try
		{
			Overlapped::Init( 2000 );
			m_pSockEventHndl = new MySockEventHandler();
			m_pMainController = new TMainController((TSockEventHandler *) m_pSockEventHndl, v_lpHandler);
		}
		catch (std::exception &e)
		{
			return ;
		}

		m_pMainController->Start();

	}

	void CHttpController::Stop()
	{
		if (m_pMainController)
		{
			delete m_pMainController;
			m_pMainController = NULL;
		}

		if (m_pSockEventHndl)
		{
			delete m_pSockEventHndl;
			m_pSockEventHndl = NULL;
		}

		Overlapped::Destroy();
	}

	BOOL CHttpController::POST( LPCSTR v_lpszProxyURL,USHORT v_usProxyPort, LPCSTR v_lpszURL,LPCSTR v_lpszHostIP, USHORT v_usHostPort,LPCSTR v_lpszPostData, LPVOID v_lpCustom )
	{
		if (m_pMainController)
		{
			return m_pMainController->POST( v_lpszProxyURL, v_usProxyPort,  
				v_lpszURL, 
				v_lpszHostIP,  v_usHostPort, 
				v_lpszPostData,  
				v_lpCustom );
		}

		return FALSE;
	}
}

using namespace HTTP_CC;

