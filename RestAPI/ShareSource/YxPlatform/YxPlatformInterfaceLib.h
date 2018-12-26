#pragma once
#include <windows.h>

typedef BOOL (CALLBACK *LPGETLOGFILE) ();
typedef LPVOID (WINAPI* PFNYxPlatform_Install)(LPCTSTR lpcszTo, LPCTSTR lpcszSubject,LPCTSTR lpSoftName);
typedef void (WINAPI* PFNYxPlatform_Uninstall)(LPVOID lpState);
typedef BOOL (WINAPI* PFNYxPlatform_BusinessResister)(int nBusCount,LPCTSTR strBusName[],LPCTSTR strBusDesc[],int nBusID[]);
typedef BOOL (WINAPI* PFNYxPlatform_BusinessRunInfo)(int nBusCount,int nBusID[],LPCTSTR strInfoString []);
typedef BOOL (WINAPI* PFNYxPlatform_BusPerfCounter)(int nBusCount,int nBusID[],int nBusCounter []);


namespace nsYxPlatformInterface
{
	class CYxPlatformInterfaceLib
	{
	public:
		CYxPlatformInterfaceLib(void);
		~CYxPlatformInterfaceLib(void);
	protected:
		BOOL LoadYxPlatformLib();
		void FreeLib();
	public:
		LPVOID  YxPlatform_Install( char *szAppName );
		BOOL  YxPlatform_BusinessResister(int nBusCount,LPCTSTR strBusName[],LPCTSTR strBusDesc[],int nBusID[]);
		BOOL  YxPlatform_BusinessRunInfo(int nBusCount,int nBusID[],LPCTSTR strInfoString []);
		BOOL  YxPlatform_BusPerfCounter( int nBusNum,int nBusID[],int nBusCounter[] );

	private:
		LPVOID		m_pImpl;
		HMODULE		m_hModule;
		PFNYxPlatform_Install				m_pfnYxPlatform_Install;
        PFNYxPlatform_Uninstall				m_pfnYxPlatform_Uninstall;
		PFNYxPlatform_BusinessResister      m_pfnYxPlatform_BusinessResister;
		PFNYxPlatform_BusinessRunInfo       m_pfnYxPlatform_BusinessRunInfo;
		PFNYxPlatform_BusPerfCounter		m_pfnYxPlatform_BusPerfCounter;
	};
}

using namespace nsYxPlatformInterface;