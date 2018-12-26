#pragma once

#include "StdAfx.h"
#include "HttpManager.h"

CHttpManager* CHttpManager::_s_pHttpManager = NULL;

CHttpManager::CHttpManager(void)
{
	_s_pHttpManager = this;
	m_usSvrPort = 0;
	m_dwSN = 0;
	m_mapSNToConnId.clear();
	m_bCanAccept = false;

	::memset(m_aryHttpClient,0x00,sizeof(m_aryHttpClient));
	::InitializeCriticalSection(&m_csClients);
	::InitializeCriticalSection(&m_csMapSNToConnId);
}

CHttpManager::~CHttpManager(void)
{
	m_mapSNToConnId.clear();

	::DeleteCriticalSection(&m_csClients);
	::DeleteCriticalSection(&m_csMapSNToConnId);
}

void CHttpManager::ShowWndLog(const int v_iWndLogType, const char v_szWndLog[])
{
	if( v_iWndLogType == LogSpace::enRunInfo )
	{
		toShowWndLog(LogSpace::enRunInfo,v_szWndLog);
	}
	else if( v_iWndLogType == LogSpace::enErrorInfo )
	{
		toShowWndLog(LogSpace::enErrorInfo,v_szWndLog);
	}
	else
	{
		toShowWndLog(LogSpace::enRunInfo,v_szWndLog);
	}
}

int CHttpManager::CallBack_CB_Upload( DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[] )
{
	if( CHttpManager::_s_pHttpManager != NULL )
	{
		return CHttpManager::_s_pHttpManager->CB_Upload(_dwSN,_byFileNameLen,_szFileName);
	}

	return -1;
}

int CHttpManager::CallBack_CB_Download_Mobile( DWORD _dwSN, BYTE _byFileType, DWORD   _dwFileLen, char    *_pFileBuff )
{
	if( CHttpManager::_s_pHttpManager != NULL )
	{
		return CHttpManager::_s_pHttpManager->CB_Download_Mobile(_dwSN,_byFileType,_dwFileLen,_pFileBuff);
	}

	return -1;
}

int CHttpManager::CallBack_CB_Download_Web( DWORD _dwSN, BYTE    _byWebUrlLen, char    _szWebUrl[MAX_PATH] )
{
	if( CHttpManager::_s_pHttpManager != NULL )
	{
		return CHttpManager::_s_pHttpManager->CB_Download_Web(_dwSN,_byWebUrlLen,_szWebUrl);
	}

	return -1;
}

int CHttpManager::CB_Upload( DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[] )
{
	if ( !m_bCanAccept )
	{
		return -1;
	}

	int iConnId = -1;
	{
		CCS CS(&m_csMapSNToConnId);
		std::map<DWORD,DWORD>::iterator itor;
		itor = m_mapSNToConnId.find(_dwSN);
		if( itor != m_mapSNToConnId.end() )
		{
			iConnId = itor->second;
			m_mapSNToConnId.erase(itor++);
		}
		else
		{
			toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号找不到对应的链路!");
			return -1;
		}
	}

	if( iConnId == -1 || iConnId > MAX_CONN_SIZE )
	{
		toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号找不到的链路ID非法!");
		return -1;
	}
	else
	{
		CCS CS(&m_csClients);
		CHttpClient *pHttpClinet = m_aryHttpClient[iConnId];
		if( pHttpClinet == NULL )
		{
			toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号对应的链路已经断开!");
			return -1;
		}
		else
		{
			pHttpClinet->CB_Upload(_dwSN,_byFileNameLen,_szFileName);
		}
	}

	return 0;
}

int CHttpManager::CB_Download_Mobile( DWORD _dwSN, BYTE _byFileType, DWORD   _dwFileLen, char    *_pFileBuff )
{
	if ( !m_bCanAccept )
	{
		return -1;
	}

	int iConnId = -1;
	{
		CCS CS(&m_csMapSNToConnId);
		std::map<DWORD,DWORD>::iterator itor;
		itor = m_mapSNToConnId.find(_dwSN);
		if( itor != m_mapSNToConnId.end() )
		{
			iConnId = itor->second;
			m_mapSNToConnId.erase(itor++);
		}
		else
		{
			toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号找不到对应的链路!");
			return -1;
		}
	}

	if( iConnId == -1 || iConnId > MAX_CONN_SIZE )
	{
		toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号找不到的链路ID非法!");
		return -1;
	}
	else
	{
		CCS CS(&m_csClients);
		CHttpClient *pHttpClinet = m_aryHttpClient[iConnId];
		if( pHttpClinet == NULL )
		{
			toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号对应的链路已经断开!");
			return -1;
		}
		else
		{
			pHttpClinet->CB_Download_Mobile(_dwSN,_byFileType,_dwFileLen,_pFileBuff);
		}
	}

	return 0;
}

int CHttpManager::CB_Download_Web( DWORD _dwSN, BYTE    _byWebUrlLen, char    _szWebUrl[MAX_PATH] )
{
	if ( !m_bCanAccept )
	{
		return -1;
	}

	int iConnId = -1;
	{
		CCS CS(&m_csMapSNToConnId);
		std::map<DWORD,DWORD>::iterator itor;
		itor = m_mapSNToConnId.find(_dwSN);
		if( itor != m_mapSNToConnId.end() )
		{
			iConnId = itor->second;
			m_mapSNToConnId.erase(itor++);
		}
		else
		{
			toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号找不到对应的链路!");
			return -1;
		}
	}

	if( iConnId == -1 || iConnId > MAX_CONN_SIZE )
	{
		toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号找不到的链路ID非法!");
		return -1;
	}
	else
	{
		CCS CS(&m_csClients);
		CHttpClient *pHttpClinet = m_aryHttpClient[iConnId];
		if( pHttpClinet == NULL )
		{
			toShowWndLog(LogSpace::enErrorInfo,"[错误]:流水号对应的链路已经断开!");
			return -1;
		}
		else
		{
			pHttpClinet->CB_Download_Web(_dwSN,_byWebUrlLen,_szWebUrl);
		}
	}

	return 0;
}

void CHttpManager::Release()
{
	m_bCanAccept = false;

	CHttpClient *pHttpClient = NULL;

	::EnterCriticalSection(&m_csClients);
	for( int i=0; i < MAX_CONN_SIZE; i++ )
	{
		pHttpClient = m_aryHttpClient[i];
		if(NULL != pHttpClient)
		{
			delete pHttpClient;
			pHttpClient = NULL;
			m_aryHttpClient[i] = NULL;
		}
	}
	::LeaveCriticalSection(&m_csClients);
	
	::EnterCriticalSection(&m_csMapSNToConnId);
	m_mapSNToConnId.clear();
	::LeaveCriticalSection(&m_csMapSNToConnId);

	StopServer();
	DestoryServer();
}

int CHttpManager::Init(unsigned short v_usPort)
{
	m_usSvrPort = v_usPort;

	if( CreateServer() == -1 )
	{
		toShowWndLog( LogSpace::enErrorInfo, "[CHttpManager::Init]: 创建IOCP服务对象失败！");						
		return -1;
	}
	toShowWndLog( LogSpace::enRunInfo, "[CHttpManager::Init]: 创建IOCP服务对象成功......");	

	SetConnCreateFactory( (IConnectionFactory*)this );

	// >>> 设置日志接口 
	LogSpace::IFileLog*	pIFileLog	= static_cast<LogSpace::IFileLog*>(this);
	LogSpace::IWndLog*	pIWndLog	= static_cast<LogSpace::IWndLog*>(this);
	SetLogInterFace( (CHAR*)(pIFileLog), (CHAR*)(pIWndLog) );

	if(!( StartServer(m_usSvrPort) ))
	{	
		toShowWndLog( LogSpace::enErrorInfo, "[CHttpManager::Init]: 启动IOCP服务失败！");
		return -1;
	}
	toShowWndLog( LogSpace::enRunInfo, "[CHttpManager::Init]: 启动IOCP服务成功......");

	m_bCanAccept = true;

	return 0;
}

int CHttpManager::toShowWndLog(int v_iTypeLog, const char v_szLog[])
{
	std::stringstream    ssLog;

	ssLog.str("");
	ssLog.clear();
	ssLog <<  v_szLog;

	switch(v_iTypeLog)
	{
	case LogSpace::enRunInfo:
		{
			g_pAF_WebServiceImpl->toPrintLog(_enComm,ssLog.str().c_str());
		}
		return 0;
	case LogSpace::enErrorInfo:
		{
			g_pAF_WebServiceImpl->toPrintLog(_enError,ssLog.str().c_str());
		}
		return 0;
	default:
		return 0;
	}					
	return 0;
}

int CHttpManager::toWriteFileLog( const char v_szDescription[], const char v_szBuf[], int v_iLenBuf)
{					
	return 0;
}

IConnection* CHttpManager::CreateConnectionFromRemote( 
	IConnection**v_ppConnection, DWORD v_dwSvrID,
	int v_ConnID, char *v_pBuffRead, int nLen,
	u_long ulRemoteAddr, u_short sPort )
{
	if ( !m_bCanAccept )
	{
		return NULL;
	}

	SOCKADDR_IN		ClientAddr;
	ClientAddr.sin_addr.s_addr = ulRemoteAddr;

	char szLog[256];
	sprintf_s(szLog,sizeof(szLog),"<==【请求建链】【%s:%d】...",inet_ntoa((in_addr)ClientAddr.sin_addr),sPort);
	toShowWndLog(LogSpace::enRunInfo,szLog);

	CHttpClient *pTcpDataConn = new CHttpClient();
	if(NULL == pTcpDataConn)
	{
		return NULL;
	}

	strcpy_s( pTcpDataConn->m_szClientIp,sizeof(pTcpDataConn->m_szClientIp),inet_ntoa((in_addr)ClientAddr.sin_addr) );
	pTcpDataConn->m_ui16Port = sPort;
	pTcpDataConn->m_pHttpManager = this;
	*v_ppConnection = pTcpDataConn;
	pTcpDataConn->AttachID(v_ConnID);
	pTcpDataConn->AttchSvrID(v_dwSvrID);

	// 加入到数组中
	::EnterCriticalSection(&m_csClients);
	if( m_aryHttpClient[v_ConnID] != NULL )
	{
		toShowWndLog(LogSpace::enErrorInfo,"[错误]:IOCP组件连接索引ID冲突！！！");

		::LeaveCriticalSection(&m_csClients);
		return NULL;
	}
	m_aryHttpClient[v_ConnID] = pTcpDataConn;
	::LeaveCriticalSection(&m_csClients);

	pTcpDataConn->OnReceive( v_pBuffRead, nLen );

	return pTcpDataConn;
}

void CHttpManager::TimerService_1000ms()
{
	if ( !m_bCanAccept )
	{
		return;
	}

	CHttpClient* pHttpClient = NULL;

	::EnterCriticalSection(&m_csClients);
	for( int i=0; i < MAX_CONN_SIZE; i++ )
	{
		pHttpClient = m_aryHttpClient[i];
		if(NULL != pHttpClient)
		{
			if( pHttpClient->IsTimeOut() )
			{
				// 输出日志
				char szLog[256];
				sprintf_s(szLog,sizeof(szLog),"==>【空闲超时断链】【%s:%d】...",pHttpClient->m_szClientIp,pHttpClient->m_ui16Port);
				toShowWndLog(LogSpace::enRunInfo,szLog);

				m_aryHttpClient[i] = NULL;
				delete pHttpClient;
				pHttpClient = NULL;
			}
		}
	}
	::LeaveCriticalSection(&m_csClients);
}

int CHttpManager::ToUpload(DWORD _dwSN,BYTE _byFileType,DWORD _dwFileLen,char *_pFileBuff)
{
	if ( !m_bCanAccept )
	{
		return -1;
	}
	return 0;
}

int CHttpManager::ToDownload( DWORD _dwSN, BYTE _byFileNameLen, char _szFileName[], const BYTE v_byDownloadType, const BYTE v_byFileProp )
{
	if ( !m_bCanAccept )
	{
		return -1;
	}
	return 0;
}

DWORD CHttpManager::GetSN() 
{
	return ::InterlockedIncrement((LONG*)&m_dwSN); 
}

void CHttpManager::BindSnToConnId( DWORD v_dwSN, DWORD v_dwConnId )
{
	if ( !m_bCanAccept )
	{
		return;
	}

	CCS CS(&m_csMapSNToConnId);
	m_mapSNToConnId.insert(std::make_pair(v_dwSN,v_dwConnId));
};
