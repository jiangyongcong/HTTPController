#include "StdAfx.h"
#include ".\tcpconn.h"

namespace nsTcpConn
{
	CTcpConn::CTcpConn(void)
	{
		::memset(m_szIp,0,32);
		m_uiIpAddr = 0;
		m_usPort = 0;

		m_pBufLeave = NULL;
		m_iBufPos = 0;
		m_Flag7e = 0;
		m_Flag7d = 0;

		m_uiTotalSend = 0;
		m_uiTotalRecv = 0;
		m_iFrameCheckCount = 0;
		m_iFrameSendCount = 0;
	}

	CTcpConn::~CTcpConn(void)
	{
		this->Reset();
	}

	void CTcpConn::OnRead( char *v_pBuffRead, int nLen )
	{
		this->OnReceive((unsigned char *)v_pBuffRead, nLen);
	}

	void CTcpConn::OnConnectionEstablished( BOOL bConnEsted )
	{
		m_ui8ConnStatus = bConnEsted ? enConnOK : enConnFail;
		if(bConnEsted)
		{
		}
	}

	BOOL CTcpConn::OnClose( )
	{
		return true;
	}

	void CTcpConn::OnError( DWORD dwError,DWORD dwOpType )
	{
	}

	void CTcpConn::OnWrite( LPCSTR lpszBuff,int nLen )
	{
		m_uiTotalSend += nLen;
	}

	void CTcpConn::Reset(void)
	{
		if(NULL != m_pBufLeave)
		{
			delete[] m_pBufLeave;
			m_pBufLeave = NULL;
		}
		m_iBufPos = 0;
		m_Flag7d = 0;
		m_Flag7e = 0;
	}

	void CTcpConn::OnReceive(unsigned char* v_szRecvBuf, int v_iRecvLen)
	{
		m_uiTotalRecv += v_iRecvLen;

		if( NULL == m_pBufLeave )
		{
			m_pBufLeave = new unsigned char[MAX_BUF];
			memset( m_pBufLeave, 0x00, MAX_BUF);
			m_iBufPos = 0;
			m_Flag7d = 0;
			m_Flag7e = 0;
		}

		int    i;
		for( i = 0; i < v_iRecvLen; i ++ )
		{
			
			if( MAX_BUF <= m_iBufPos )
			{
				m_Flag7e = m_Flag7d = 0;
				m_iBufPos = 0;
			}

			switch( v_szRecvBuf[i] )
			{
			case 0x7e:
				if( 1 == m_Flag7e )
				{
					if( 0 == m_iBufPos )
					{
						m_Flag7d = 0;
					}else
					{
						OneFrameChecked();
						m_iFrameCheckCount ++;
						memset( m_pBufLeave, 0x00, MAX_BUF );
						m_iBufPos = 0;
						m_Flag7e = m_Flag7d = 0;
					}
				}else
				{
					m_iBufPos = 0;
					m_Flag7e = 1;
				}
				break;
			case 0x7d:
				if( 1 == m_Flag7d )
				{
					m_iBufPos = 0;
					m_Flag7e = m_Flag7d = 0;
				}
				else
				{
					m_Flag7d = 1;
				}
				break;
			case 0x00:
				if( 1 == m_Flag7d )
				{
					m_pBufLeave[m_iBufPos++] = 0x7d;
					m_Flag7d = 0;
				}else
				{
					m_pBufLeave[m_iBufPos++] = 0x00;
				}
				break;
			case 0x01:
				if( 1 == m_Flag7d )
				{
					m_pBufLeave[m_iBufPos++] = 0x7e;
					m_Flag7d = 0;
				}else
				{
					m_pBufLeave[m_iBufPos++] = 0x01;
				}
				break;
			default:
				if( 1 == m_Flag7d )
				{
					m_iBufPos = 0;
					m_Flag7e = m_Flag7d = 0;
				}
				else
				{
					m_pBufLeave[m_iBufPos++] = v_szRecvBuf[i];
				}
				break;
			}
		}

		if( 0 == m_iBufPos )
		{
			if( NULL != m_pBufLeave )
			{
				memset( m_pBufLeave, 0x00, MAX_BUF);
				//m_Flag7d = 0; // 这里一定要把状态保存下来，不能重置，否则可能会丢帧
				//m_Flag7e = 0;
			}
		}
	}
	int CTcpConn::AddFsynToBuf(const unsigned char* v_szOldBuf, unsigned char* v_szNewBuf, const int v_iBufLen)
	{
		int    iResult = 1, i;

		for( i = 0; i < v_iBufLen; i++ )
		{
			switch( v_szOldBuf[i] )
			{
			case 0x7e:
				v_szNewBuf[iResult++] = 0x7d;
				v_szNewBuf[iResult++] = 0x01;
				break;
			case 0x7d:
				v_szNewBuf[iResult++] = 0x7d;
				v_szNewBuf[iResult++] = 0x00;
				break;
			default:
				v_szNewBuf[iResult++] = v_szOldBuf[i];
				break;
			}
		}

		v_szNewBuf[0] = 0x7e;
		v_szNewBuf[iResult++] = 0x7e;
		
		return iResult;
	}
	int CTcpConn::TcpSend(const CFrameBuffer* const v_pFrameBuffer)
	{
		unsigned char szFrameData[MAX_BUF];
		int iFrameLen = this->AddFsynToBuf((unsigned char*)(v_pFrameBuffer), szFrameData,  2+v_pFrameBuffer->m_iFrameLen);

		if( ! (this->Send(reinterpret_cast<char*>(szFrameData), iFrameLen) ) )
		{
			//this->CloseConnection();
			return -1;
		}
		m_iFrameSendCount ++;
		return 0;
	}
}
