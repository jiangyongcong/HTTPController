#pragma once

#include <string>
#include <map>
#include "../ShareSource/StringCoder.hpp"
#include "Request.h"

#define MAX_CONN_TIME 10    // 最长允许的连接时间

class CHttpManager;

namespace nsHttp
{
	class CCS
	{
	public:
		CCS(){};
		CCS( CRITICAL_SECTION *v_CS )
		{
			m_pCS = v_CS;
			::EnterCriticalSection(m_pCS);
		}
		~CCS()
		{
			::LeaveCriticalSection(m_pCS);
		}

	private:
		CRITICAL_SECTION *m_pCS;
	};

	class CHttpClient : public IConnection
	{
	public:
		CHttpClient(void);
		~CHttpClient(void);

	//继承自IConnection
	public:
		virtual void OnRead( char *v_pBuffRead, int nLen );
		virtual void OnConnectionEstablished( BOOL bConnEsted );
		//对方关闭连接
		virtual BOOL OnClose( );
		//有错误发生
		virtual void OnError( DWORD dwError,DWORD dwOpType );
		//写完成
		virtual void OnWrite( LPCSTR lpszBuff,int nLen );

	public:
		void ToClose(void);

	public:
		void OnReceive(char* v_szRecvBuf, int v_iRecvLen);
		bool GetLine(char* v_szRecvBuf, int v_iRecvLen, int& v_ndx);
		void ProcessLine(void);
		bool IsTimeOut();

	public:
		void ToResponseUpLoad( char *v_pContentType, char* v_pFileName, int v_iFileNameLen, bool v_bSucceed );
		void ToResponseDownLoad( char *v_pContentType, char* v_pData, int v_iDataLen, bool v_bSucceed );

	private:
		void DoProcess();

		BOOL GetFileListFromDir(CStringArray &v_arrCStr, LPCSTR pszDir, LPCSTR pszFilter /*= "*.*"*/);
		/*
		 * Content-Type:
		 *    image/jpeg
		 *    text/xml
		 *    application/json
		 *    application/xml
		 */
		void ToResponseGeneral( char *v_pContentType, char* v_pData, int v_iDataLen );
		void BulidGetFilesResp(CStringArray &v_arrFile, std::stringstream &v_ssContent, bool v_bSucceed);
		void BulidGetFileResp(CString &_strFile, std::stringstream &v_ssContent, bool v_bSucceed);


	private:
		void Reset(void);

	public:
		void CB_Upload( DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[] );
		void CB_Download_Mobile( DWORD _dwSN, BYTE _byFileType, DWORD   _dwFileLen, char    *_pFileBuff );
		void CB_Download_Web( DWORD _dwSN, BYTE    _byWebUrlLen, char    _szWebUrl[MAX_PATH] );

	private:
		bool BodySent(void);
		void AddToBody(unsigned char* v_szRecvBuf, int v_nBytes, int v_ndx);

	public:
		char    m_szClientIp[32];
		unsigned short  m_ui16Port;

	public:
		enum _enTCPCONNSTATUS
		{
			TCPCONN_IDLE = 0, 
			TCPCONN_ALIVE,  
			TCPCONN_CLOSED,
			TCPCONN_ERROR
		};
		_enTCPCONNSTATUS m_TcpConnStatus;


		CHttpManager *m_pHttpManager;
		CRequest   m_Request;

	public:
		enum enREQSTATUS
		{
			REQ_REQUEST=0, REQ_HEADER, REQ_BODY, REQ_SIMPLE, REQ_DONE
		};
		enREQSTATUS m_reqStatus;

	private:
		std::stringstream m_ssLine;

	private:
		unsigned long  m_ulLiveTime;
		CRITICAL_SECTION	m_csCS;
	};
}

using namespace nsHttp;