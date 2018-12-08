#ifndef ___SERVER_SERVICE_H_INCLUDED___
#define ___SERVER_SERVICE_H_INCLUDED___

#include "Threading.h"
#include "ConnectionManager.h"
#include "iocp.h"
#include "BusinessHandler.h"
#include "Utf.h"

using namespace std;

namespace HTTP_CC
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	//---------------------------- CLASS ----------------------------------------------------------
	// A template class designed for verifying client sockets
	// against time-out cases. Time-out case = if no I/O actions
	// happen with a socket during a configured number of
	// seconds. This implies that the attachment of the
	// client socket must implement "GetTimeElapsed()" and "ResetTime(...)"
	// methods.
	template<class T>
	class CTimeOutChecker: public IRunnable {
	private:
		unsigned int				m_nTimeOutValue;
		vector<CConnection<T> *>	*m_arrSocketPool;
		CIOCPWrapper<T>				*m_hIocp;

	protected:
		// method checks sockets to detect time-out cases
		virtual void run() 
		{
			CConnection<T>* pSocket = NULL;
			unsigned int i = 0;
			long lngTimeElapsed;		

			vector<CConnection<T> *>::iterator itPos = m_arrSocketPool->begin();

			while ( !CThread::currentThread().isInterrupted() ) 
			{
		
				if ( itPos >= m_arrSocketPool->end() ) {
					itPos = m_arrSocketPool->begin();
					i = 0;
				}

				pSocket = *itPos;

				pSocket->Lock();
				// check if client socket object is 
				// assigned a socket handler.
				if ( pSocket->IsBusy() ) {
					// call "GetTimeElapsed()" of the socket's
					// attachment.
					lngTimeElapsed = pSocket->GetAttachment()->GetTimeElapsed();

					// check elapsed time, since last registered action,
					// with the configured time-out.
					if ( lngTimeElapsed > (long)this->m_nTimeOutValue ) {
						// clear the time and push the socket to 
						// the IOCP with status _CLOSE. Socket 
						// will be closed shortly.
						pSocket->GetAttachment()->ResetTime( true );
						m_hIocp->SetCloseMode( pSocket );
					}
				}
				pSocket->UnLock();

				// sleep 2 milliseconds after every 2 iterations
				if ( ( i % 2 ) == 0 ) 
				{
					::Sleep( 2 );
				}

				itPos++;
				i++;
			}		
		};

	public:
		// Constructor of the template class. Requires:
		// - a pointer to a collection of pointers to client sockets.
		//   This is the collection of sockets to be checked against
		//   time-out cases.
		// - a pointer to the IO completion port object responsible
		//   for checking I/O events against passed collection of 
		//   client sockets.
		// - value of the time-out, in seconds.
		CTimeOutChecker( vector<CConnection<T> *> *arrSocketPool, 
			CIOCPWrapper<T> *hIocp, unsigned int nTimeOutValue ) {
			m_arrSocketPool = arrSocketPool;
			m_hIocp = hIocp;

			// If time-out is less than 5 seconds, drop it to 5 seconds.
			// It is expected that configured time-out value to be >= 5!
			if ( nTimeOutValue > 5 ) this->m_nTimeOutValue = nTimeOutValue;
			else this->m_nTimeOutValue = 5;
		};

		// Nothing to destruct as inside the template class
		// we keep/use just pointers obtained from external
		// sources.
		~CTimeOutChecker() {};
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	//---------------------------- CLASS ----------------------------------------------------------
	// A template interface showing how the client socket event 
	// handler should look like.
	template<class T>
	class ISockHandler {
	public:
		// Client socket ("pSocket") is about to be closed.
		virtual void OnClose( CConnection<T> *pSocket, 
				MYOVERLAPPED *pOverlap,
				CConnectionManager<T> *pServerSocket,
				CIOCPWrapper<T> *pHIocp
			) = 0;


		// Client socket ("pSocket") was just accepted by
		// the server socket (new connection with a client
		// is created). 
		virtual void OnConnect( CConnection<T> *pSocket, 
				MYOVERLAPPED *pOverlap,
				CConnectionManager<T> *pServerSocket,
				CIOCPWrapper<T> *pHIocp
			) = 0;

		// Client socket ("pSocket") was returned from the IOCP
		// queue with status _PENDING. For more details see
		// "CIOCPWrapper<T>::SetPendingMode(...)". This method
		// is invoked only if there was a call to 
		// "CIOCPWrapper<T>::SetPendingMode(...)", performed by a 
		// user code, internally "SetPendingMode(...)"
		// is never called.
		virtual void OnPending( CConnection<T> *pSocket, 
				MYOVERLAPPED *pOverlap,
				CConnectionManager<T> *pServerSocket,
				CIOCPWrapper<T> *pHIocp
			) = 0;

		// Client socket ("pSocket") was returned from IOCP
		// queue with status _READ. This means that overlapped
		// reading operation, started previously with 
		// "CConnection<T>::ReadFromSocket(...)", was 
		// successfully finished. 
		// - "pOverlap->DataBuf" structure points to the data 
		//   buffer and buffer's size that were passed to the
		//   "CConnection<T>::ReadFromSocket(...)".
		// - "dwBytesTransferred" will indicate how many 
		//   bytes were read.
		virtual void OnReadFinalized( CConnection<T> *pSocket, 
				MYOVERLAPPED *pOverlap,
				DWORD dwBytesTransferred,
				CConnectionManager<T> *pServerSocket,
				CIOCPWrapper<T> *pHIocp
			) = 0;

		// Client socket ("pSocket") was returned from IOCP
		// queue with status _WRITE. This means that overlapped
		// writting operation, started previously with 
		// "CConnection<T>::WriteToSocket(...)", was 
		// successfully finished. 
		// - "pOverlap->DataBuf" structure points to the data 
		//   buffer and buffer's size that were passed to the
		//   "CConnection<T>::WriteToSocket(...)".
		// - "dwBytesTransferred" will indicate how many 
		//   bytes were written.
		virtual void OnWriteFinalized( CConnection<T> *pSocket, 
				MYOVERLAPPED *pOverlap,
				DWORD dwBytesTransferred,
				CConnectionManager<T> *pServerSocket,
				CIOCPWrapper<T> *pHIocp
			) = 0;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	//---------------------------- CLASS ----------------------------------------------------------
	// Class is used internally by "CMainController<T>" as a
	// Task entity to be submitted to "CMainController<T>::m_ThPool".
	template<class T>
	class CSockTask: public IRunnable {
	private:
		MYOVERLAPPED	*m_pOverlap;
		CConnection<T>	*m_pConn;
		ISockHandler<T>	*m_pEventHandler;
		CConnectionManager<T>	*m_pConnMnger;
		CIOCPWrapper<T>	*m_pHIocp;
		DWORD			m_dwBytesTransferred;
		QueuedBlocks<CSockTask<T> > *m_pSockEventTaskPool;	

	protected:
		virtual void run() {
			if ( m_pEventHandler == NULL ) return;

			m_pConn->Lock();

			// check if client socket object is assigned a socket 
			// handler and session assigned to operation is equal
			// to the session of the socket.
			if ( m_pConn->IsBusy() && ( m_pOverlap->nSession == m_pConn->GetSession() ) ) 
			{
				switch ( m_pOverlap->OperationType ) {
				// invoke the relevant event handler
				case _CLOSE:
					//LogMsg(mt_Info, "CloseSocket request %d.", m_pConn->GetSocket());
					m_pEventHandler->OnClose( m_pConn, m_pOverlap, m_pConnMnger, m_pHIocp );
					// Make sure (double check) the socket 
					// is closed.
					m_pConnMnger->Release( m_pConn );
					break;

				case _CONNECT:
					m_pEventHandler->OnConnect( m_pConn, m_pOverlap, m_pConnMnger, m_pHIocp );
					break;

				case _READ:
					m_pEventHandler->OnReadFinalized( m_pConn, m_pOverlap, m_dwBytesTransferred,
						m_pConnMnger, m_pHIocp );
					break;

				case _PENDING:
					m_pEventHandler->OnPending( m_pConn, m_pOverlap, m_pConnMnger, m_pHIocp );
					break;

				case _WRITE:
					m_pEventHandler->OnWriteFinalized( m_pConn, m_pOverlap, m_dwBytesTransferred,
						m_pConnMnger, m_pHIocp );
					break;

				default:
					//LogMsg(mt_Warning, "Oops..." );
					break;
				}
			}

			m_pConn->UnLock();

			// Place the structure back to the pool.
			Overlapped::Release( m_pOverlap );
			m_pSockEventTaskPool->Release( this );
		};

	public:
		CSockTask() { Clear(); };

		// Set everything required to the Task to be 
		// able to invoke the relevant event handler.
		void Set( QueuedBlocks<CSockTask<T> > *pSockEventTaskPool,
				ISockHandler<T> *pSEvent, 
				CConnection<T> *pSocket, 
				MYOVERLAPPED *pOverlap,
				DWORD dwBytesTransferred,
				CConnectionManager<T> *pServerSocket,
				CIOCPWrapper<T> *pHIocp
			) {
			m_pSockEventTaskPool = pSockEventTaskPool;
			m_pEventHandler = pSEvent;
			m_pOverlap = pOverlap;
			m_dwBytesTransferred = dwBytesTransferred;
			m_pConn = pSocket;
			m_pConnMnger = pServerSocket;
			m_pHIocp = pHIocp;
		};

		// Method is used by 'QueuedBlocks' from
		// "MemManager.h".
		void Clear() {
			m_dwBytesTransferred = 0;
			m_pSockEventTaskPool = NULL;
			m_pEventHandler = NULL;
			m_pOverlap = NULL;
			m_pConn = NULL;
			m_pConnMnger = NULL;
			m_pHIocp = NULL;
		};
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	//---------------------------- CLASS ----------------------------------------------------------
	// Well, finally, here is the template class joining all
	// the stuff together. Considering the Aspect Oriented paradigm, 
	// this template class may be seen as an Aspect. The "individualizations"
	// of this aspect are "ISockEventHandler<T>" and "T" itself. "T" is nothing
	// else but attachment of the client socket (see CConnection<T> template
	// class for more details). Implementing "ISockEventHandler<T>" and "T" will
	// define the individual behaviour of this aspect.
	// It is a composition of the IOCP, server socket, time-out checker 
	// and thread pool. Class implements the business logic that makes all 
	// these entities working together.
	template<class T>
	class CMainController: public IRunnable {
	private:
		CConnectionManager<T>	m_ConnManager;
		CIOCPWrapper<T>	m_IocpWrapper;
		ISockHandler<T> *m_pSockHandler;

		CTimeOutChecker<T>	*m_pTimingChecker;

		// thread pool which will execute the tasks
		CThreadPool	*m_pThPool;

		// a pool of the CSockEventTask<T> objects
		QueuedBlocks<CSockTask<T> > m_SockTaskPool;

		CBusinessHandler* m_pBusiHandler; // 业务处理器

	protected:

		// This method will be executed by a thread
		// of the task pool.
		virtual void run() 
		{
			int nRet;
			DWORD dwBytesTransferred;

			MYOVERLAPPED *pOverlap = NULL;
			CConnection<T> *pSocket = NULL;	

			while ( !CThread::currentThread().isInterrupted() )
			{
				//::Sleep( 1 );

				dwBytesTransferred = 0;
				pSocket = NULL;

				// check the IOCP queue for:
				// - a completed I/O operation against a socket or
				// - a queued socket with a fake status like _ACCEPT, 
				//   _CLOSE or _PENDING
				//
				// When an overlapped I/O completes, an I/O completion packet 
				// arrives at the IOCP and GetQueuedCompletionStatus returns.
				nRet = m_IocpWrapper.GetQueuedCompletionStatus( &dwBytesTransferred, 
					&pSocket, &pOverlap );

				// something is wrong
				if ( nRet == 0 ) {
					if ( ::GetLastError() == WAIT_TIMEOUT ) continue;

					if ( pOverlap != NULL ) dwBytesTransferred = 0;
					else continue;
				}

				if ( pSocket == NULL ) {
					Overlapped::Release( pOverlap );
					continue;
				}

				// not good if zero bytes were transferred
				if ( (dwBytesTransferred == 0) && 
					!(pOverlap && pOverlap->OperationType == _CONNECT))
				{
					// clear the time and push the socket to 
					// the IOCP with status _CLOSE. Socket 
					// will be closed shortly.
					if ( pSocket->IsBusy() && ( pOverlap->nSession == pSocket->GetSession() ) ) {
						pSocket->GetAttachment()->ResetTime( true );
						if ( !( m_IocpWrapper.SetCloseMode( pSocket, pOverlap ) ) ) {
							m_ConnManager.Release( pSocket );
						}
					}

					Overlapped::Release( pOverlap );
				}
				else 
				{
					// obtain a free instance of the event task
					// which will be submitted to the threat pool
					CSockTask<T> *pCSockEv = m_SockTaskPool.Get();
					if ( pCSockEv != NULL )
					{
						pCSockEv->Set( &m_SockTaskPool, m_pSockHandler, 
							pSocket, pOverlap, dwBytesTransferred,
							&m_ConnManager, &m_IocpWrapper
						);

						// _CLOSE operation type will have a lower
						// priority that other operation types.
						int priority = ( pOverlap->OperationType == _CLOSE )?1:2;

						// submit the task to the thread pool
						m_pThPool->submit( pCSockEv, priority );
					}
					else 
					{
						// no free resources, so close the socket
						m_ConnManager.Release( pSocket );
						Overlapped::Release( pOverlap );
					}
				}
			}		
		};

	public:
		// Constructor or the class.
		// v_pSockHandler      - pointer to an instance implementing 
		//                ISockEventHandler<T>. This instance will be used
		//                as a client socket event handler.	
		// nMaxClients  - the maximum number of accepted (concurrent)
		//                client connections. To be passed to
		//                the server socket and also will be used 
		//                as the initial size for the pool of the 
		//                CSockEventTask<T> objects.
		// nNoThreads   - indicative (the real one is computed,
		//                see below) number of the threads
		//                to be created by the thread pool.
		// timeout      - the value of the time-out, in seconds.
		//                Will be passed to the time-out checker.
		//                If time-out is zero, time-out checker
		//                will not be created.
		CMainController( ISockHandler<T> *v_pSockHandler, CBusinessHandler* v_pBusiHandler): 
					m_ConnManager(/*v_SysCfg.m_iMaxConnects*/2000), m_IocpWrapper(1), 
					m_SockTaskPool(/*v_SysCfg.m_iMaxConnects*/2000),
					m_pBusiHandler(v_pBusiHandler)
		{

			unsigned int iNumThreads = 0;
			SYSTEM_INFO si;		

			if ( v_pSockHandler == NULL ) {
				throw std::exception("NULL pointer set for socket event handler.");
			}

			m_pSockHandler = v_pSockHandler;

			// Query system info in order to check
			// how many CPU's are there installed in
			// the system.
			::GetSystemInfo( &si );

			// Compute the real number of the threads
			// to be created by the thread pool.
			iNumThreads = 0/*v_SysCfg.m_iNumWorkthreads*/ + 2/*si.dwNumberOfProcessors*/;

			// If configured time-out is zero, then don't
			// create time-out checker object. Client sockets
			// will not be checked against time-out cases.
			if ( /*v_SysCfg.m_iTimeOut*/30 > 0 ) 
			{
				m_pTimingChecker = new CTimeOutChecker<T>( m_ConnManager.GetPool(), &m_IocpWrapper, 30/*v_SysCfg.m_iTimeOut*/ );

				// Increase the number of the threads,
				// +1 for the time-out checker.
				iNumThreads++;
			} else m_pTimingChecker = NULL;

			// Create the thread pool which will handle
			// the tasks. "nMaxClients" is used as the
			// initial capacity of the priority queue
			// of (associated with) the thread pool.
			m_pThPool = new CThreadPool( iNumThreads + 2, 2 * /*v_SysCfg.m_iMaxConnects*/2000 );

			// Submit this instance (as a task) to the
			// thread pool, as many times as CPU's are installed
			// in the system. The "run()" method will be executed
			// by pool's threads.
			for (iNumThreads = 0; iNumThreads < 2/*si.dwNumberOfProcessors*/; iNumThreads++) 
			{
				m_pThPool->submit( this, 1 );
			}

			// If time-out checker was created, then
			// submit it to the thread pool (as a task).
			if ( m_pTimingChecker != NULL ) 
			{
				m_pThPool->submit( m_pTimingChecker );
			}
			
		}

		virtual ~CMainController()
		{
			// Stop all the pool's threads.
			m_pThPool->shutdown();

			// If time-out checker was created, then
			// delete it.
			if (m_pTimingChecker != NULL)
			{
				delete m_pTimingChecker;
			}

			// Delete the thread pool.
			delete m_pThPool;		
		}

		// Start the threat pool (== all the 
		// threads in the pool).
		void Start() 
		{
			m_pThPool->startAll(); 
		}

		BOOL POST( LPCSTR v_lpszProxyURL,USHORT v_usProxyPort,
			LPCSTR v_lpszURL,LPCSTR v_lpszHostIP,
			USHORT v_usHostPort,LPCSTR v_lpszPostData,
			LPVOID v_lpCustom )
		{
			// 向远程服务器发送请求
			CConnection<T> *pConn = NULL;
			pConn = m_ConnManager.CreateAsyncConn(v_lpszProxyURL, v_usProxyPort);
			if (pConn != NULL)
			{
				ostringstream oss;

				std::string strData = Convert::GBToUTF8( v_lpszPostData );

				/// HTTP请求头
				oss << "POST " << v_lpszURL << " HTTP/1.0\r\n"
					<< "Accept: */*\r\n"
					<< "User-Agent: Mozilla/4.0\r\n"
					<< "Content-Length: " << (int)strData.size()<< "\r\n"
					<< "Host: " << v_lpszHostIP << ":" << v_usHostPort << "\r\n"
					<< "Content-Type: application/x-www-form-urlencoded\r\n"
					//<< "Content-Type: text/plain\r\n"
					<< "\r\n"
					//<< "\r\n"
					<< strData;

				string strTmp = oss.str();

				pConn->GetAttachment()->SetSendBuf(strTmp.c_str(), (int)strTmp.length());
				pConn->GetAttachment()->SetCustomData(v_lpCustom);
				pConn->GetAttachment()->SetReqEventHandler(m_pBusiHandler);
				pConn->GetAttachment()->ResetTime( false );

				m_IocpWrapper.AssociateSocket( pConn );

				if (! m_IocpWrapper.SetConnectMode(pConn)) 
				{
					m_ConnManager.Release( pConn );
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}

			return TRUE;
		}
	};
}

using namespace HTTP_CC;



#endif
