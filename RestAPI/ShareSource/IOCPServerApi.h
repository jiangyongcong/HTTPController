//#pragma once

typedef unsigned long u_long;
typedef unsigned short u_short;

#ifdef  IOCPSERVER_EXPORTS
#define IOCP_API _declspec(dllexport)
#else
#define IOCP_API _declspec(dllimport)
#endif

//声明两个虚基类，作为应用层对象基类
class IConnection;
class IConnectionFactory;

//DLL导出函数声明
DWORD IOCP_API CreateServer();

BOOL IOCP_API StartServer( DWORD v_dwSvrID, int nPort, int nMaxConnections = 5000, 
				  int nMaxFreeBuffers = 1000, int nMaxFreeContexts = 500);

void IOCP_API StopServer( DWORD v_dwSvrID );

void IOCP_API SetConnCreateFactory( DWORD v_dwSvrID, IConnectionFactory *v_pICF);

BOOL IOCP_API Send( DWORD v_dwSvrID, IConnection *v_pConnection, LPCSTR pszText, int nLen );

void IOCP_API CloseConnection( DWORD v_dwSvrID, IConnection * v_pConnection );

BOOL IOCP_API CreateConnectionToRemote(  DWORD v_dwSvrID, IConnection * v_pConnection,LPCSTR v_lpRemotIP,
									   short sRemotePort,LPCSTR v_pSendData,int nLen );

BOOL IOCP_API SetLogInterFace( DWORD v_dwSvrID, CHAR* v_pIFileLog, CHAR* v_pIWndLog );

void IOCP_API DestoryServer( DWORD v_dwSvrID );

class IOCPDLL
{
public:
	static DWORD IOCP_CreateServer()
	{
		return CreateServer();
	}

	static void IOCP_DestoryServer( DWORD v_dwSvrID )
	{
		DestoryServer( v_dwSvrID );
	}

	static BOOL IOCP_StartServer(DWORD v_dwSvrID, int nPort, int nMaxConnections = 5000, 
		int nMaxFreeBuffers = 1000, int nMaxFreeContexts = 500)
	{
		return StartServer(v_dwSvrID, nPort, nMaxConnections, 
			nMaxFreeBuffers, nMaxFreeContexts );
	}

	static void IOCP_StopServer( DWORD v_dwSvrID )
	{
		StopServer( v_dwSvrID );
	}

	static void IOCP_SetConnCreateFactory( DWORD v_dwSvrID, IConnectionFactory *v_pICF)
	{
		SetConnCreateFactory( v_dwSvrID, v_pICF );
	}

	static void IOCP_CloseConnection( DWORD v_dwSvrID, IConnection * v_pConnection)
	{
		CloseConnection( v_dwSvrID, v_pConnection );
	}
	static BOOL IOCP_Send( DWORD v_dwSvrID, IConnection *v_pConnection, char *pszText, int nLen )
	{
		return Send( v_dwSvrID, v_pConnection,pszText,nLen );
	}

	static BOOL  IOCP_CreateConnectionToRemote( DWORD v_dwSvrID, IConnection * v_pConnection,LPCSTR v_lpRemotIP,
												short sRemotePort,LPCSTR v_pSendData,int nLen )
	{
		return CreateConnectionToRemote( v_dwSvrID, v_pConnection, v_lpRemotIP, sRemotePort, v_pSendData, nLen );
	}

	static BOOL IOCP_SetLogInterFace( DWORD v_dwSvrID, CHAR* v_pIFileLog, CHAR* v_pIWndLog )
	{
		return SetLogInterFace( v_dwSvrID, v_pIFileLog, v_pIWndLog );
	}

protected:
private:
};

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 一下为用户接口基类，用户需要派生自己的类进行处理                     */
/*W.S.H 2008.12.05                                                      */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////

namespace LogSpace
{

	enum _enWndLog
	{
		enDevInfo	= 0x00,	// 设备信息
		enRunInfo	= 0x01,	// 运行信息
		enCommInfo	= 0x02,	// 通信信息
		enErrorInfo	= 0x03	// 异常错误信息
	};
	// 文件日志接口
	class IFileLog
	{
	public:
		virtual ~IFileLog(){};
		/*
		*	函数名：toWriteFileLog
		*　参　数：v_szDescription---日志描述信息
		*			v_szBuf---日志内容
		*			v_iLenBuf---日志长度
		*	功　能：向日志文件中写入日志信息
		*	返回值：
		*	作　者：邱长丰
		*	时　间：2007-1-4 17:15
		*　备　注： 
		*/
		virtual int toWriteFileLog( const char v_szDescription[], const char v_szBuf[], int v_iLenBuf) = 0;
	};
	// 窗口日志接口
	class IWndLog
	{
	public:
		virtual ~IWndLog(){};
		/*
		*	函数名：toPrintWndLog
		*　参　数：v_iTypeLog---日志类型
		*			v_szLog---日志内容
		*	功　能：向窗口中写入日志,将日志显示出来
		*	返回值：
		*	作　者：邱长丰
		*	时　间：2007-1-4 17:17
		*　备　注：
		*/
		virtual int toShowWndLog(int v_iTypeLog, const char v_szLog[]) = 0;
	};
}


class IConnection
{
public:
	IConnection(void):m_iConnectID(-1),m_dwTickCount(0),m_lSerialNo(0),m_dwSvrID(-1){};
	~IConnection(void){};
public:
//请不要更改以下代码
	void AttachID(int newID)
	{
		m_iConnectID = newID;
	}
	inline int GetID()
	{
		return m_iConnectID;
	}

	void AttchSvrID( DWORD v_dwSvrID )
	{
		m_dwSvrID = v_dwSvrID;
	}
//请不要更改以上代码

// 绑定服务器ID


	enum _enOpType
	{
		enOpNone	= 0,
		enOpAccept	= 1,
		enOpWrite	= 2, //写
		enOpRead	= 3, //读
		enOpConnect = 4
	};
public://以下的函数都是线程安全的。如果要考虑在链接之间共享数据，那么是非安全的，需要自己考虑线程同步问题。
	
	//数据到达,这个是线程安全的，有互斥滴
	virtual void OnRead( char *v_pBuffRead,int nLen ) { };

	//对方关闭连接
	//返回TRUE则IOCP会立即释放相关资源，否则上层应用应该在适当的时候调用CloseConnection();
	//以释放相关的资源!
	virtual BOOL OnClose( ) { return TRUE; }

	//有错误发生
	virtual void OnError( DWORD dwError,DWORD dwOpType ) {}

	//发送数据
	virtual BOOL Send( char *v_pBuffSend,int nLen ) { return IOCPDLL::IOCP_Send(m_dwSvrID,this,v_pBuffSend,nLen);}

	//主动关闭连接或者指示IOCP关闭连接
	//如果在收到的OnClose通知中返回了FALSE，那么还必须调用CloseConnection()以指示IOCP
	//释放相关资源。
	virtual void CloseConnection() { IOCPDLL::IOCP_CloseConnection( m_dwSvrID, this ); }

	//连接已经建立,可以是本地到远程服务器的连接，也可以是远程客户端到本地的连接
	virtual void OnConnectionEstablished( BOOL bConnEsted ) { 

	};

	//写完成
	virtual void OnWrite( LPCSTR lpszBuff,int nLen ) {};
public:
	DWORD m_dwTickCount; //时间戳
	LONG  m_lSerialNo;	 //流水号，每个链接递增的
	DWORD m_dwSvrID;     //IOCP服务对象ID
private:
	//连接ID，由底层IOCP模型指派，上层只能读取不应该去修改它，
	//可以通过IConnectionFactory::CreateConnection传入的参数id调用相应的 Attach函数，
	//且不应该Attach两次!
	int  m_iConnectID;
	
};

class IConnectionFactory
{
public:
	IConnectionFactory():m_dwSvrID(-1){};

public:
	//上层应用必须重载此函数，来创建自己需要的IConnection对象
	virtual DWORD CreateServer()
	{
		m_dwSvrID = IOCPDLL::IOCP_CreateServer();
		return m_dwSvrID;
	}

	virtual void DestoryServer()
	{
		IOCPDLL::IOCP_DestoryServer( m_dwSvrID );
	}

	virtual BOOL StartServer(int nPort, int nMaxConnections = 5000, 
		int nMaxFreeBuffers = 1000, int nMaxFreeContexts = 500)
	{
		return IOCPDLL::IOCP_StartServer(m_dwSvrID,nPort,nMaxConnections,nMaxFreeBuffers,nMaxFreeContexts);
	}

	virtual void StopServer()
	{
		return IOCPDLL::IOCP_StopServer( m_dwSvrID );
	}

	virtual void SetConnCreateFactory( IConnectionFactory *v_pICF)
	{
		IOCPDLL::IOCP_SetConnCreateFactory(m_dwSvrID, v_pICF);
	}

	virtual BOOL SetLogInterFace( CHAR* v_pIFileLog, CHAR* v_pIWndLog )
	{
		return IOCPDLL::IOCP_SetLogInterFace(m_dwSvrID,v_pIFileLog,v_pIWndLog);
	}

	virtual IConnection* CreateConnectionFromRemote( OUT IConnection**v_ppConnection, IN DWORD v_dwSvrID,
													 IN int v_ConnID, IN char *v_pBuffRead, IN int nLen
													,u_long ulRemoteAddr,u_short sPort )
	{
		//函数必须返回新创建的IConnection对象指针
		//如果想要在此函数中立即调用发送函数，
		//那么函数头部应该如下所示代码，注释部分
		//否则可以不添加如下代码。
		//CMyConnection *p =  new CMyConnection;
		//*v_ppConnection = p;
		//p->AttachID( id );

		*v_ppConnection = NULL;
		return NULL;
	}

	//上层应用事先创建好连接对象IConnection，并将其传入，
	//在提供了远程ip和端口号的情况下，IOCP会启动连接过程。
	//返回值为TRUE表示成功启动连接过程，会在随后以IConnection::OnConnectionEstablished()的方式通知。
	virtual BOOL CreateConnectionToRemote( IConnection * v_pConnection,LPCSTR v_lpRemotIP,short sRemotePort,LPCSTR v_pSendData,int nLen )
	{
		if( m_dwSvrID == -1 )
			return FALSE;

		return IOCPDLL::IOCP_CreateConnectionToRemote( m_dwSvrID, v_pConnection,v_lpRemotIP,sRemotePort,v_pSendData,nLen );
	}

public:
	DWORD m_dwSvrID;

protected:
private:
};