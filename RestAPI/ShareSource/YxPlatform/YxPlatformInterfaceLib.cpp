// CrashInterfaceLib.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "YxPlatformInterfaceLib.h"

namespace nsYxPlatformInterface
{
	CYxPlatformInterfaceLib::CYxPlatformInterfaceLib(void)
	:m_hModule(NULL)
	,m_pfnYxPlatform_Install(NULL)
	,m_pfnYxPlatform_Uninstall(NULL)
	,m_pfnYxPlatform_BusinessResister(NULL)
	,m_pfnYxPlatform_BusinessRunInfo(NULL)
	,m_pfnYxPlatform_BusPerfCounter(NULL)
	{
		LoadYxPlatformLib();
	}

	CYxPlatformInterfaceLib::~CYxPlatformInterfaceLib(void)
	{
		FreeLib();
	}

	BOOL CYxPlatformInterfaceLib::LoadYxPlatformLib()
	{
		LPCSTR lpszFileName = "YxPlatform.dll";
		m_hModule = ::LoadLibrary( lpszFileName );

		if ( NULL==m_hModule )
		{
			return FALSE;
		}

		m_pfnYxPlatform_Install = (PFNYxPlatform_Install)GetProcAddress(m_hModule ,"YxPlatform_Install");
		if ( NULL==m_pfnYxPlatform_Install )
		{
			::FreeLibrary( m_hModule );
			m_hModule = NULL;
			return FALSE;
		}

		m_pfnYxPlatform_Uninstall = (PFNYxPlatform_Uninstall)GetProcAddress(m_hModule ,"YxPlatform_Uninstall");
		if ( NULL==m_pfnYxPlatform_Uninstall )
		{
			::FreeLibrary( m_hModule );
			m_hModule = NULL;
			return FALSE;
		}

		m_pfnYxPlatform_BusinessResister = (PFNYxPlatform_BusinessResister)GetProcAddress(m_hModule ,"YxPlatform_BusinessResister");
		if ( NULL==m_pfnYxPlatform_BusinessResister )
		{
			::FreeLibrary( m_hModule );
			m_hModule = NULL;
			return FALSE;
		}

		m_pfnYxPlatform_BusinessRunInfo = (PFNYxPlatform_BusinessRunInfo)GetProcAddress(m_hModule ,"YxPlatform_BusinessRunInfo");
		if ( NULL==m_pfnYxPlatform_BusinessRunInfo )
		{
			::FreeLibrary( m_hModule );
			m_hModule = NULL;
			return FALSE;
		}

		m_pfnYxPlatform_BusPerfCounter = (PFNYxPlatform_BusPerfCounter)GetProcAddress(m_hModule ,"YxPlatform_BusPerfCounter");
		if ( NULL==m_pfnYxPlatform_BusPerfCounter )
		{
			::FreeLibrary( m_hModule );
			m_hModule = NULL;
			return FALSE;
		}

		return TRUE;
	}

	void CYxPlatformInterfaceLib::FreeLib()
	{
		if ( NULL != m_pfnYxPlatform_Uninstall )
		{
			m_pfnYxPlatform_Uninstall(m_pImpl);
		}
		if ( NULL!=m_hModule )
		{
			::FreeLibrary( m_hModule );
		}
	}

	LPVOID CYxPlatformInterfaceLib::YxPlatform_Install( char * v_szAppName )
	{
		m_pImpl = m_pfnYxPlatform_Install(NULL ,NULL,v_szAppName);
		return m_pImpl;
	}

	BOOL  CYxPlatformInterfaceLib::YxPlatform_BusinessResister(int nBusCount,LPCTSTR strBusName[],LPCTSTR strBusDesc[],int nBusID[])
	{
		return m_pfnYxPlatform_BusinessResister(nBusCount,strBusName,strBusDesc,nBusID);
	}

	BOOL  CYxPlatformInterfaceLib::YxPlatform_BusinessRunInfo(int nBusCount,int nBusID[],LPCTSTR strInfoString [])
	{
		return m_pfnYxPlatform_BusinessRunInfo(nBusCount,nBusID,strInfoString);
	}

	BOOL  CYxPlatformInterfaceLib::YxPlatform_BusPerfCounter( int nBusNum,int nBusID[],int nBusCounter[] )
	{
		return m_pfnYxPlatform_BusPerfCounter(nBusNum,nBusID,nBusCounter);
	}
}