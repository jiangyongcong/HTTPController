#pragma once
#include "TemplateImpl.h"

namespace HTTP_CC
{
	class CHttpController
	{
	public:
		CHttpController(void);
		~CHttpController(void);
	public:
		void Start(CBusinessHandler *v_lpHandler);
		void Stop();

		BOOL POST( LPCSTR v_lpszProxyURL,USHORT v_usProxyPort,
			LPCSTR v_lpszURL,LPCSTR v_lpszHostIP,
			USHORT v_usHostPort,LPCSTR v_lpszPostData,
			LPVOID v_lpCustom );


	private:
		TMainController *m_pMainController;
		MySockEventHandler *m_pSockEventHndl;
	};

}

using namespace HTTP_CC;

