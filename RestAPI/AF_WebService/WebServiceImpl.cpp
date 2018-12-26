#pragma once

#include "stdafx.h"
#include "WebServiceImpl.h"

CWebServiceImpl::CWebServiceImpl()
{
	m_hInstance = NULL;
	m_InitStartup = NULL;
	m_Terminate = NULL;
	Load_Impl();
}

CWebServiceImpl::~CWebServiceImpl()
{
	UnLoad_Impl();
}

BOOL CWebServiceImpl::Load_Impl()
{
	if (m_hInstance==NULL)
	{
		m_hInstance=LoadLibrary("AF_WebServiceImpl.dll");
		if (m_hInstance==NULL)
		{
			return FALSE;
		}
	}

	if (m_InitStartup==NULL)
	{
		m_InitStartup=(FP_InitStartup)GetProcAddress(m_hInstance,"DLL_InitStartup");
		if (m_InitStartup==NULL)
		{
			return FALSE;
		}
	}	

	if (m_Terminate==NULL)
	{
		m_Terminate=(FP_Terminate)GetProcAddress(m_hInstance,"DLL_Terminate");
		if (m_Terminate==NULL)
		{
			return FALSE;
		}
	}

	return TRUE;
}


void CWebServiceImpl::Terminate()
{
	if ((m_hInstance!=NULL)&&(m_Terminate!=NULL))
	{
		m_Terminate();
		m_Terminate = NULL;
	}
}

BOOL CWebServiceImpl::InitStartup( char *_pMyObj, CB_PRINT_LOG _fpCB_PrintLog, WORD _wListenPort, const char* _szDir )
{
	if ((m_hInstance!=NULL)&&(m_InitStartup!=NULL))
	{
		return m_InitStartup(_pMyObj,_fpCB_PrintLog,_wListenPort,_szDir);
	}

	return FALSE;
}

void CWebServiceImpl::UnLoad_Impl()
{
	if ( m_hInstance!=NULL )
	{
		if( m_Terminate != NULL )
		{
			m_Terminate();
		}

		FreeLibrary(m_hInstance);
		m_hInstance=NULL;
		m_Terminate = NULL;
	}
}