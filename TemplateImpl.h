#pragma once

#include <sstream>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "MainController.h"

#define SEND_BUFF_SIZE 4096
#define RECV_BUFF_SIZE 2048

// #define MAX_REQS 1000
// #define MAX_CONNECTIONS 500
// #define NUM_THREADS 4
// #define TIME_OUT 10
// #define PORT 8080
namespace HTTP_CC
{

	//---------------------------------------------------------------------------------
	struct Attachment
	{
		enum {_READ_HEADER, _READ_CONTENT};

		CBusinessHandler			*m_pReqEventHandler;
		LPVOID						m_lpCustom;

		volatile time_t				m_tmLastActionTime;
		char						m_szSendBuf[SEND_BUFF_SIZE];
		int							m_iSendDatLen;
		int							m_iHasSendLen;
		char						m_szRecvBuf[RECV_BUFF_SIZE];
		int							m_iRecvDatLen;

		std::string					m_strMessage;
		int							m_iLineBytes;
		int							m_iReadStep;
		
		//enum { _SUCCED,_FAILED_TIMEOUT,_FAILED_NETERRO };
		//DWORD						m_dwFailedReson;

		Attachment() 
		{ 
			Clear();
		}

		// as requested by the API of the framework
		void Clear() 
		{
			m_strMessage.clear();
			m_pReqEventHandler = NULL;
			m_lpCustom = NULL;
			m_tmLastActionTime = 0;
			memset(m_szSendBuf, 0, sizeof(m_szSendBuf));
			m_iSendDatLen = 0;
			m_iHasSendLen = 0;
			memset(m_szRecvBuf, 0, sizeof(m_szRecvBuf));
			m_iRecvDatLen = 0;		
			m_iLineBytes = -1;
			m_iReadStep = _READ_HEADER;

			//m_dwFailedReson = _SUCCED;
		}

		void SetReqEventHandler(CBusinessHandler *v_pReqEventHandler)
		{
			m_pReqEventHandler = v_pReqEventHandler;
		}

		void SetCustomData( LPVOID v_lpCustom )
		{
			m_lpCustom = v_lpCustom;
		}

		void SetSendBuf(const char *v_pDat, int v_iDatLen)
		{
			m_iSendDatLen = 0;
			if (!v_pDat || v_iDatLen <= 0)
			{
				return;
			}

			if (v_iDatLen > SEND_BUFF_SIZE)
			{
				m_iSendDatLen = SEND_BUFF_SIZE;
			}
			else
			{
				m_iSendDatLen = v_iDatLen;
			}

			memcpy(m_szSendBuf, v_pDat, m_iSendDatLen);
		}

		//void SetFailedReson( DWORD v_dwReson )
		//{
		//	m_dwFailedReson = v_dwReson;
		//}


		int Commit( DWORD dwBytesTransferred )
		{
			int iRet = -1;

			switch(m_iReadStep)
			{
			case _READ_HEADER:
				iRet = ReadHeader(dwBytesTransferred);
				break;
			case _READ_CONTENT:
				iRet = ReadContent(dwBytesTransferred);
				break;
			default:
				break;
			}

			return iRet;		

		}

		int ReadHeader(DWORD dwBytesTransferred)
		{
			bool bFound = false;
			int iCnt = 0;

			while(!bFound && (iCnt < (int)dwBytesTransferred))
			{
				char cTmp = m_szRecvBuf[iCnt++];
				if(cTmp == '\n') 
				{
					if(m_iLineBytes == 0)
					{
						bFound = true;
					}

					m_iLineBytes = 0;				
				}
				else if(cTmp != '\r')
				{
					m_iLineBytes++;
				}

				m_strMessage += cTmp;			
			}

			if (bFound)
			{

				if (!CheckHeaderStatus(m_strMessage))
				{
					return -1; // 表示失败
				}	

				//m_iRecvDatLen = CalContentLen(m_strMessage);

				m_strMessage.clear();


				//if (m_iRecvDatLen < 0)
				//{
				//	return -1;			

				//}
				
				m_iReadStep = _READ_CONTENT;

				while(iCnt < (int)dwBytesTransferred)
				{
					m_strMessage += m_szRecvBuf[iCnt++];
/*					if (--m_iRecvDatLen <= 0)
					{
						return 0;
					}	*/			
				}

				if ( CheckCompelete() )
				{
					return 0;
				}

				return RECV_BUFF_SIZE/*m_strMessage.length()*/;
				
			}
			else
			{
				return RECV_BUFF_SIZE;
			}

		}

		int ReadContent(DWORD dwBytesTransferred)
		{
			int iCnt = 0;

			while(iCnt < (int)dwBytesTransferred)
			{
				m_strMessage += m_szRecvBuf[iCnt++];
/*				if (--m_iRecvDatLen <= 0)
				{
					return 0;
				}		*/		
			}

			if ( CheckCompelete() )
			{
				return 0;
			}

			return RECV_BUFF_SIZE;
		}

		// 判断状态
		bool CheckHeaderStatus(const std::string &v_strHeader)
		{		
			std::istringstream response_stream(v_strHeader);
			std::string http_version;
			response_stream >> http_version;
			unsigned int status_code;
			response_stream >> status_code;
			std::string status_message;
			std::getline(response_stream, status_message);
			if (!response_stream || http_version.substr(0, 5) != "HTTP/")
			{			
				return false;
			}
			if (status_code != 200)
			{
				return false;
			}

			return true;
		}

		int CalContentLen(const std::string &v_message)
		{
			int iContentLen = -1;
			
			int iPos = (int)v_message.find("Content-Length:");		

			if(iPos != string::npos)
			{
				iPos = iPos + (int)strlen("Content-Length:");

				string strBytes;
				bool bLine = false;
				while(!bLine)
				{
					switch (v_message[iPos])
					{
					case '\r':					
						break;
					case '\n':
						bLine = true;
						break;
					default:
						strBytes += v_message[iPos];
					}

					++iPos;			
				}

				iContentLen = atoi(strBytes.c_str());
			}

			return iContentLen;
		}

		BOOL CheckCompelete()
		{
			int nRet = (int)m_strMessage.rfind( "</result>" );
			if ( nRet!=-1 )
			{
				return TRUE;
			}
			
			nRet = (int)m_strMessage.rfind( "</Result>" );
			if ( nRet!=-1 )
			{
				return TRUE;
			}

			return FALSE;
		}

		// as requested by the API of the framework
		void ResetTime( bool toZero ) { 
			if (toZero) m_tmLastActionTime = 0;
			else {
				time_t	lLastActionTime;
				time(&lLastActionTime); 
				m_tmLastActionTime = lLastActionTime;
			}
		}
		// as requested by the API of the framework
		long GetTimeElapsed() {
			time_t tmCurrentTime;

			if (0 == m_tmLastActionTime) return 0;

			time(&tmCurrentTime);
			return (long)(tmCurrentTime - m_tmLastActionTime);
		}
	};

	//---------------------------------------------------------------------------------
	typedef CConnection<Attachment> TConnection;
	typedef CConnectionManager<Attachment> TConnManager;
	typedef CIOCPWrapper<Attachment> TIOCPWrapper;
	typedef ISockHandler<Attachment> TSockEventHandler;
	//typedef CBusinessHandler<Attachment> TReqEventHandler;
	typedef CMainController<Attachment> TMainController;
	//---------------------------------------------------------------------------------

	class MySockEventHandler: public TSockEventHandler {
	public:
		MySockEventHandler() {};
		~MySockEventHandler() {};

		// empty method, not used
		virtual void OnClose( TConnection *v_pConn, MYOVERLAPPED *v_pOverlap, 
			TConnManager *v_pConnMnger, TIOCPWrapper *v_pIocpWraper ) 
		{
			v_pConnMnger->Release( v_pConn );
		}


		virtual void OnConnect( TConnection *pSocket, MYOVERLAPPED *pOverlap, 
			TConnManager *pServerSocket, TIOCPWrapper *pHIocp ) 
		{
			int nRet;
			DWORD dwSize;
			char *temp;

			temp = pSocket->GetAttachment()->m_szSendBuf;
			dwSize = pSocket->GetAttachment()->m_iSendDatLen;

			nRet = pSocket->WriteToSocket( temp, dwSize );

			pSocket->GetAttachment()->ResetTime( false );
			if (nRet == SOCKET_ERROR ) 
			{
				//pSocket->GetAttachment()->SetFailedReson( Attachment::_FAILED_NETERRO );
				pServerSocket->Release( pSocket );
			}
		}

		// empty method, not used
		virtual void OnPending( TConnection *v_pConn, MYOVERLAPPED *v_pOverlap, 
			TConnManager *v_pConnMnger, TIOCPWrapper *v_pIocpWrapper ) {};

		virtual void OnReadFinalized( TConnection *v_pConn, MYOVERLAPPED *v_pOverlap,
			DWORD v_dwBytesTransferred, TConnManager *v_pConnMnger, TIOCPWrapper *v_pIocpWraper )
		{
				int nRet;
				DWORD dwSize;
				char *temp;

				// finalize the filling of the buffer
				nRet = v_pConn->GetAttachment()->Commit(v_dwBytesTransferred);

				if (nRet < 0)
				{				
					v_pConnMnger->Release( v_pConn );
				}
				else if (nRet == 0)
				{
					v_pConnMnger->Release( v_pConn );
				}
				else
				{
					dwSize = (DWORD)nRet;			
					temp = v_pConn->GetAttachment()->m_szRecvBuf;

					nRet = v_pConn->ReadFromSocket(temp, dwSize);
					v_pConn->GetAttachment()->ResetTime( false );

					if ( nRet == SOCKET_ERROR )
					{
						//v_pConn->GetAttachment()->SetFailedReson( Attachment::_FAILED_NETERRO );
						v_pConnMnger->Release( v_pConn );
					}
				}


		};

		virtual void OnWriteFinalized( TConnection *pSocket, MYOVERLAPPED *pOverlap,
			DWORD dwBytesTransferred, TConnManager *pServerSocket, TIOCPWrapper *pHIocp ) 
		{
			int nRet;
			DWORD dwSize;
			char *temp;

			Attachment* attach = pSocket->GetAttachment();

			attach->m_iHasSendLen+=dwBytesTransferred;

			if ( attach->m_iSendDatLen>attach->m_iHasSendLen )
			{//未发送完成
				dwSize = attach->m_iSendDatLen  - attach->m_iHasSendLen;
				temp = attach->m_szSendBuf + attach->m_iHasSendLen;

				nRet = pSocket->ReadFromSocket(temp, dwSize);
				attach->ResetTime( false );

				if ( nRet == SOCKET_ERROR )
				{
					//attach->SetFailedReson( Attachment::_FAILED_NETERRO );
					pServerSocket->Release( pSocket );
				}
			}
			else
			{//发送完成
				//，开始接收
				dwSize = RECV_BUFF_SIZE;
				temp = attach->m_szRecvBuf;

				nRet = pSocket->ReadFromSocket( temp, dwSize );
				attach->ResetTime(false);

				if ( nRet == SOCKET_ERROR ) 
				{
					//attach->SetFailedReson( Attachment::_FAILED_NETERRO );
					pServerSocket->Release( pSocket );
				}
			}

		}
	};
}

using namespace HTTP_CC;
