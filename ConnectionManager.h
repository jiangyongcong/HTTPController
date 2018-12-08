#ifndef ___SOCKET_SERVER_H_INCLUDED___
#define ___SOCKET_SERVER_H_INCLUDED___

#include <winsock2.h>
#include "MemManager.h"
#include "Connection.h"

namespace HTTP_CC
{

	///////////////////////////////////////////////////////////////////////////////////////////////
	//---------------------------- CLASS ----------------------------------------------------------
	// A class for handling basic server socket operations.
	template<class T>
	class CConnectionManager {
	private:

		// Pool of client sockets.
		QueuedBlocks<CConnection<T> > m_SockPool;

	public:
		// Set the size of the socket pool to the maximum number of accepted
		// client connections.
		CConnectionManager(unsigned int nMaxClients): m_SockPool(nMaxClients)
		{}

		// 新建一个异步的连接
		CConnection<T>* CreateAsyncConn(const char *v_szHost, unsigned short v_usPort)
		{
			CConnection<T> *pConn = NULL;		
			pConn = m_SockPool.GetFromQueue();		
			if (pConn != NULL) 
			{
				if (pConn->NewConnection(v_szHost, v_usPort) != 0)
				{
					Release(pConn);
					pConn = NULL;
				}				
			}

			return pConn;
		}
		
		// Release the client socket, will try closing connection and 
		// will place it back to the pool.
		void Release(CConnection<T> *sock)
		{
			if (sock != NULL) {
				// check if client socket object is 
				// assigned a socket handler. If yes,
				// close it.
				if (sock->IsBusy()) sock->CloseSocket();

				// Place it back to the pool.
				m_SockPool.Release(sock);		
			}
		}

		vector<CConnection<T> *> *GetPool() 
		{
			return m_SockPool.GetBlocks();
		}

		~CConnectionManager()
		{
		}
	};

}

using namespace HTTP_CC;

#endif