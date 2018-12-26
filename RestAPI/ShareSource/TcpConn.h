#pragma once

#define MAX_BUF 4096
#define MAX_BUFF_FRAME_COUNT 10000

namespace nsTcpConn
{
	#pragma  pack( push, 1 )
	typedef struct _tagFrameBuffer
	{
		BYTE  m_ui8ServiceType;
		BYTE  m_ui8DataType;
		char   m_szFrameBuf[MAX_BUF];
		int    m_iFrameLen;
	}CFrameBuffer;
	#pragma  pack( pop )

	class CTcpConn : public IConnection
	{
	public:
		enum _enConnStatus
		{
			enDisConn = 0,  // 未链接成功
			enConning = 1,  // 正在链接中
			enConnOK  = 2,  // 链接成功
			enConnFail = 3, // 链接失败
			enConnClose = 4,// 链接关闭
			enConnError = 5,// 链接错误
		};

	public:
		CTcpConn(void);
		virtual ~CTcpConn(void);

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
		void Reset(void);
		void OnReceive(unsigned char* v_szRecvBuf, int v_iRecvLen);
		int TcpSend(const CFrameBuffer* const v_pFrameBuffer);
		int AddFsynToBuf(const unsigned char* v_szOldBuf, unsigned char* v_szNewBuf, const int v_iBufLen);
		virtual int OneFrameChecked(void) = 0;

	public:
		char    m_szIp[32];
		unsigned long          m_uiIpAddr;
		unsigned short  m_usPort;

		unsigned long m_uiTotalSend; // 发送总字节数统计
		unsigned long m_uiTotalRecv; // 接收总字节数统计
		int    m_iFrameCheckCount; // 检帧总帧数统计
		int    m_iFrameSendCount;  // 发送总帧数统计

	protected:
		//检帧结构
		unsigned char*	m_pBufLeave;
		int		m_iBufPos;
		int	    m_Flag7e;
		int	    m_Flag7d;
	
	public:
		//链路状态
		unsigned char  m_ui8ConnStatus;
	};
}

using namespace nsTcpConn;
