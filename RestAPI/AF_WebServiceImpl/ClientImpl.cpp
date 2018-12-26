#pragma once

#include "stdafx.h"
#include "ClientImpl.h"

CClientImpl::CClientImpl()
{
	m_dwObjId = -1;
	m_hInstance = NULL;
	m_Create = NULL;
	m_InitStartup = NULL;
	m_Stop_Release = NULL;
	m_ToUpload = NULL;
	m_ToDownLoad = NULL;
	m_ToDownLoad_Sync = NULL;
	m_SetICallback_Dfs_Client_Impl = NULL;
}

CClientImpl::~CClientImpl()
{
	UnLoad_Impl();
}

BOOL CClientImpl::Load_Impl()
{
	if (m_hInstance==NULL)
	{
		m_hInstance=LoadLibrary("YxDfs_Client.dll");
		if (m_hInstance==NULL)
		{
			return FALSE;
		}
	}

	if (m_Create == NULL)
	{
		m_Create=(FP_Create)GetProcAddress(m_hInstance,"DLL_Create");
		if (m_Create==NULL)
		{
			return FALSE;
		}
	}

	if (m_InitStartup==NULL)
	{
		m_InitStartup=(FP_Init_Startup)GetProcAddress(m_hInstance,"DLL_Init_Startup");
		if (m_InitStartup==NULL)
		{
			return FALSE;
		}
	}	

	if (m_Stop_Release==NULL)
	{
		m_Stop_Release=(FP_Terminate)GetProcAddress(m_hInstance,"DLL_Stop_Release");
		if (m_Stop_Release==NULL)
		{
			return FALSE;
		}
	}

	if (m_ToUpload==NULL)
	{
		m_ToUpload=(FP_ToUpload)GetProcAddress(m_hInstance,"DLL_ToUpload");
		if (m_ToUpload==NULL)
		{
			return FALSE;
		}
	}

	if (m_ToDownLoad==NULL)
	{
		m_ToDownLoad=(FP_ToDownload)GetProcAddress(m_hInstance,"DLL_ToDownload");
		if (m_ToDownLoad==NULL)
		{
			return FALSE;
		}
	}

	if (m_ToDownLoad_Sync==NULL)
	{
		m_ToDownLoad_Sync=(FP_ToDownload_Sync)GetProcAddress(m_hInstance,"DLL_ToDownload_Sync");
		if (m_ToDownLoad_Sync==NULL)
		{
			return FALSE;
		}
	}

	if (m_SetICallback_Dfs_Client_Impl==NULL)
	{
		m_SetICallback_Dfs_Client_Impl=(FP_SetICallback_Dfs_Client_Impl)GetProcAddress(m_hInstance,"DLL_SetICallback_Dfs_Client_Impl");
		if (m_SetICallback_Dfs_Client_Impl==NULL)
		{
			return FALSE;
		}
	}

	if( m_dwObjId == -1 && (m_dwObjId = Create()) == -1 )
	{
		return FALSE;
	}

	return TRUE;
}

void CClientImpl::UnLoad_Impl()
{
	if ( m_hInstance!=NULL )
	{
		if( m_Stop_Release != NULL )
		{
			m_Stop_Release(m_dwObjId);
		}

		FreeLibrary(m_hInstance);
		m_hInstance=NULL;
		m_Stop_Release = NULL;
		m_dwObjId = -1;
	}
}

DWORD CClientImpl::Create()
{
	if( m_hInstance != NULL && m_Create != NULL )
	{
		return m_Create();
	}

	return -1;
}

BOOL CClientImpl::InitStartup( int v_iNodeCnt, char* v_szIp[], WORD v_wPort[] )
{
	if( !Load_Impl() )
	{
		return FALSE;
	}

	if ((m_hInstance!=NULL)&&(m_InitStartup!=NULL))
	{
		return m_InitStartup(m_dwObjId, v_iNodeCnt,v_szIp,v_wPort);
	}

	return FALSE;
}

BOOL CClientImpl::Terminate()
{
	if( !Load_Impl() )
	{
		return FALSE;
	}

	if ((m_hInstance!=NULL)&&(m_Stop_Release!=NULL))
	{
		m_Stop_Release(m_dwObjId);
		m_Stop_Release = NULL;
		return TRUE;
	}

	return FALSE;
}

BOOL CClientImpl::ToUpload(DWORD _dwSN,BYTE _byFileType,DWORD _dwFileLen,char *_pFileBuff)
{
	if( !Load_Impl() )
	{
		return FALSE;
	}

	if ((m_hInstance!=NULL)&&(m_ToUpload!=NULL))
	{
		return m_ToUpload(m_dwObjId,_dwSN,_byFileType,_dwFileLen,_pFileBuff);
	}

	return FALSE;
}

BOOL CClientImpl::ToDownload( DWORD _dwSN, BYTE _byFileNameLen, char _szFileName[], const BYTE v_byDownloadType, const BYTE v_byFileProp )
{
	if( !Load_Impl() )
	{
		return FALSE;
	}

	if ((m_hInstance!=NULL)&&(m_ToDownLoad!=NULL))
	{
		return m_ToDownLoad(m_dwObjId,_dwSN,_byFileNameLen,_szFileName,v_byDownloadType,v_byFileProp);
	}

	return FALSE;
}

BOOL  CClientImpl::ToDownload_Sync( DWORD   _dwSN,// 业务流水号
					 BYTE    _byFileNameLen,// 文件名长度
					 char    _szFileName[], // 文件名（用文件名索引文件数据）
					 const BYTE v_byDownloadType, //下载类型(手机or网页)
					 const BYTE v_byFileProp,
					 char* _szFileContent,
					 int&	 _iLen
					 )
{
	if( !Load_Impl() )
	{
		return FALSE;
	}

	if ((m_hInstance!=NULL)&&(m_ToDownLoad_Sync!=NULL))
	{
		return m_ToDownLoad_Sync(m_dwObjId,_dwSN,_byFileNameLen,_szFileName,v_byDownloadType,v_byFileProp,_szFileContent,_iLen);
	}

	return FALSE;
}

void CClientImpl::SetICallback_Dfs_Client_Impl( CBF_Upload		   _cb_Upload,
												CBF_Download_Mobile  _cb_Download_Mobile,
												CBF_Download_Web     _cb_Download_Web )
{
	if( !Load_Impl() )
	{
		return;
	}

	if ((m_hInstance!=NULL)&&(m_SetICallback_Dfs_Client_Impl!=NULL))
	{
		m_SetICallback_Dfs_Client_Impl(m_dwObjId,_cb_Upload,_cb_Download_Mobile,_cb_Download_Web);
	}
}