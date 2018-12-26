#pragma once

#include "stdafx.h"
#include "YxTmpFIleMgr.h"

CYxTmpFileMgr::CYxTmpFileMgr()
{
	m_hInstance			=NULL;
	m_fpInit_Startup	=NULL;
	m_fpStopRelease		=NULL;
	m_fpOpt_Add			=NULL;
	m_fpOpt_Get			=NULL;
	m_fpOpt_Free		=NULL;
}

CYxTmpFileMgr::~CYxTmpFileMgr()
{
	FreeRes();
}

BOOL CYxTmpFileMgr::LoadRes()
{
	if (m_hInstance==NULL)
	{
		m_hInstance=LoadLibrary("YxTmpFileMgr.dll");
		if (m_hInstance==NULL)
		{
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}	

	m_fpInit_Startup=NULL;
	m_fpInit_Startup=(FP_Init_StartUp)GetProcAddress(m_hInstance,"DLLInit_StartUp");
	if (m_fpInit_Startup==NULL)
	{
		return FALSE;
	}

	m_fpStopRelease=NULL;
	m_fpStopRelease=(FP_Stop_Release)GetProcAddress(m_hInstance,"DLLStop_Release");
	if (m_fpStopRelease==NULL)
	{
		return FALSE;
	}

	m_fpOpt_Add=NULL;
	m_fpOpt_Add=(FP_Opt_Add)GetProcAddress(m_hInstance,"DLLOpt_Add");
	if (m_fpOpt_Add==NULL)
	{
		return FALSE;
	}

	m_fpOpt_Get=NULL;
	m_fpOpt_Get=(FP_Opt_Get)GetProcAddress(m_hInstance,"DLLOpt_Get");
	if (m_fpOpt_Get==NULL)
	{
		return FALSE;
	}

	m_fpOpt_Free=NULL;
	m_fpOpt_Free=(FP_Opt_Free)GetProcAddress(m_hInstance,"DLLOpt_Free");
	if (m_fpOpt_Free==NULL)
	{
		return FALSE;
	}

	return TRUE;
}

void CYxTmpFileMgr::FreeRes()
{
	if( m_hInstance != NULL )
	{
		if( m_fpStopRelease != NULL )
		{
			m_fpStopRelease();
		}
		FreeLibrary(m_hInstance);
		m_hInstance = NULL;
	}
}

BOOL CYxTmpFileMgr::Init_StartUp( const int v_iTmpSize )
{
	if( m_hInstance != NULL&&m_fpInit_Startup != NULL )
	{
		return m_fpInit_Startup(v_iTmpSize);
	}

	return FALSE;
}

void CYxTmpFileMgr::Stop_Release()
{
	if( m_hInstance != NULL&&m_fpStopRelease != NULL )
	{
		m_fpStopRelease();
	}
}

int CYxTmpFileMgr::Opt_Add( const char *v_szKey, const char* v_pData, const int v_iDataLen )
{
	if( m_hInstance != NULL&&m_fpOpt_Add != NULL )
	{
		return m_fpOpt_Add(v_szKey,v_pData,v_iDataLen);
	}

	return FALSE;
}

char* CYxTmpFileMgr::Opt_Get( const char *v_szKey, int &v_iDataLen )
{
	if( m_hInstance != NULL&&m_fpOpt_Get != NULL )
	{
		return m_fpOpt_Get( v_szKey, v_iDataLen );
	}

	return NULL;
}

void CYxTmpFileMgr::Opt_Free( const char *v_pData )
{
	if( m_hInstance != NULL&&m_fpOpt_Free != NULL )
	{
		m_fpOpt_Free( v_pData );
	}
}