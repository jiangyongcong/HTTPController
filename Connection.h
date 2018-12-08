#ifndef ___SOCKET_CLIENT_H_INCLUDED___
#define ___SOCKET_CLIENT_H_INCLUDED___

#include <winsock2.h>
#include "MemManager.h"

#define RECV_BUFFER_EMPTY	-2

enum IO_SOCK_MODES {_CLOSE, _CONNECT, _ACCEPT, _READ, _WRITE, _PENDING};

namespace HTTP_CC
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	//---------------------------- CLASS ----------------------------------------------------------
	// Overlapped structure used with this implementation of the IOCP.
	// Only the "Overlapped" member is used in context with the
	// standard IOCP API, anything else is just to help this particular
	// implementation.
	typedef struct {
		// It is very important "Overlapped" to be the very first 
		// member of the structure because pointer pointing to 
		// "this" is == to the address of the first member. 
		// This makes usage of the MYOVERLAPPED equivalent 
		// to the OVERLAPPED in context of the standard IOCP API (just a trick).
		OVERLAPPED				Overlapped;

		// A structure that points to the data packet and size of the data 
		// packet associated with current overlapped operation.
		WSABUF					DataBuf;

		// Operation type associated with current overlapped operation.
		IO_SOCK_MODES			OperationType;

		// Internally used. If nSession != CConnection::m_nSession
		// that means that socket was closed (recently) and
		// operation retrieved from IOCP queue or TaskPool queue 
		// is not relevant, to the current session of the socket.
		volatile unsigned int	nSession;

		// Method is used by 'QueuedBlocks' from
		// "MemManager.h".
		void Clear() {
			memset(this, 0, sizeof(MYOVERLAPPED));
		};
	} MYOVERLAPPED;

	// a pool of the MYOVERLAPPED structures
	typedef StaticBlocks<MYOVERLAPPED> Overlapped;
	QueuedBlocks<MYOVERLAPPED> *Overlapped::blocks = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//---------------------------- CLASS ----------------------------------------------------------
	// A class wrapping basic client socket's operations
	template<class T>
	class CConnection {
	private:
		SOCKET					m_ClientSock;
		volatile unsigned int	m_nSession;
		QMutex					m_qMutex;
		struct sockaddr_in		m_psForeignAddIn;
		volatile bool			m_blnIsBusy;
		T						m_objAttachment;

	protected:
	public:
		CConnection(): m_objAttachment(), m_qMutex() {
			this->m_nSession = 1;
			this->m_ClientSock = 0;
			this->m_blnIsBusy = false;
		};

		~CConnection() { 
			if (this->m_blnIsBusy) this->CloseSocket(); 
		};

		// Is the object assigned a socket.
		bool IsBusy() { return this->m_blnIsBusy; };

		// Method is used by 'QueuedBlocks' from
		// "MemManager.h".
		void Clear() { CloseSocket(); };
		
		// Returns the socket associated with the object.
		SOCKET GetSocket() { return this->m_ClientSock; };

		// Returns the session of the current client socket.
		// Technically, this is the counter of how may times
		// current instance was pooled, but really, this is used
		// to check if session of operation (MYOVERLAPPED) 
		// is == with the session of the socket.
		unsigned int GetSession() { return this->m_nSession; };

		// Internally used by this implementation. Don't call it.
		void Lock() { m_qMutex.Lock(); };

		// Internally used by this implementation. Don't call it.
		void UnLock() { m_qMutex.Unlock(); };

		// Associate the object with a socket.
		int Associate(SOCKET sSocket, struct sockaddr_in *psForeignAddIn) {
			int nRet = SOCKET_ERROR;

			if (sSocket < 0) return nRet;
			if (this->m_blnIsBusy) return nRet;

			this->m_blnIsBusy = true;
			this->m_ClientSock = sSocket;
			memcpy(&m_psForeignAddIn, psForeignAddIn, sizeof(struct sockaddr_in));
			m_objAttachment.Clear();

			return (int) this->m_ClientSock;
		};

		// Returns the attachment of the object.
		// Use the attachment to associate the socket with any other
		// additional information required.
		T* GetAttachment() 
		{ 
			return &m_objAttachment;
		}

		int NewConnection(const char *v_szHost, unsigned short v_usPort)
		{
			if (m_blnIsBusy || !v_szHost)
			{
				return -1;
			}

			SOCKET	sock = INVALID_SOCKET;
			int nRet;
			unsigned long lngMode = 1;		
			struct sockaddr_in sForeignAddIn;

			sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (sock == INVALID_SOCKET) 
			{
				//LogMsg(mt_Warning, "Socket 创建失败!");
				return -1;
			}

			struct sockaddr_in sin;
			sin.sin_family = AF_INET;
			sin.sin_port = htons(v_usPort);

			struct hostent * host_addr = gethostbyname(v_szHost);
			if (host_addr == NULL) 
			{
				sin.sin_addr.s_addr = inet_addr(v_szHost);
				if (sin.sin_addr.s_addr == -1)
				{
					closesocket(sock);
					return -1;
				}
			}
			else
			{
				sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list) ;
			}

			nRet = connect(sock, (const struct sockaddr *)&sin, sizeof(sockaddr_in));
			if (nRet != 0)
			{
				//LogMsg(mt_Warning, "创建远程连接失败!");
				closesocket(sock);
				return -1;			
			}

			nRet = ioctlsocket(sock, FIONBIO, (u_long FAR*) &lngMode);
			if (nRet < 0) 
			{
				closesocket(sock);			
				return -1;				 
			}

			Lock();
			Associate(sock, &sForeignAddIn);
			UnLock();

			return 0;
		}

		// Write data to the socket,
		// uses overlapped style.
		int WriteToSocket(char *pBuffer, DWORD buffSize) {
			int nRet = SOCKET_ERROR ;
			DWORD dwSendBytes = 0;
			DWORD dwFlags = 0;

			if ( this->m_blnIsBusy ) {
				// Get a free instance of the structure
				// from the pool.
				MYOVERLAPPED *mov = Overlapped::Get();

				if ( mov != NULL ) {
					mov->DataBuf.buf = pBuffer;
					mov->DataBuf.len = buffSize;
					mov->OperationType = _WRITE;
					mov->nSession = GetSession();

					nRet = WSASend(this->m_ClientSock, &mov->DataBuf, 1, 
						&dwSendBytes, dwFlags, (OVERLAPPED *) mov, NULL);

					if (nRet == SOCKET_ERROR) {
						if ((WSAGetLastError()) == WSA_IO_PENDING) nRet = 0;
					}
				}
			}
			return nRet;
		};

		// Read data from the socket,
		// uses overlapped style.
		int ReadFromSocket(char *pBuffer, DWORD buffSize) 
		{
			int nRet = SOCKET_ERROR ;
			DWORD dwRecvBytes;
			DWORD dwFlags = 0;

			if ( this->m_blnIsBusy ) {
				if (buffSize > 0) {
					// Get a free instance of the structure
					// from the pool.
					MYOVERLAPPED *mov = Overlapped::Get();

					if ( mov != NULL ) {
						mov->DataBuf.buf = pBuffer;
						mov->DataBuf.len = buffSize;
						mov->OperationType = _READ;
						mov->nSession = GetSession();

						nRet = WSARecv(this->m_ClientSock, &mov->DataBuf, 1, 
							&dwRecvBytes, &dwFlags, (OVERLAPPED *) mov, NULL);

						if (nRet == SOCKET_ERROR) {
							int ero = WSAGetLastError();
							if ((WSAGetLastError()) == WSA_IO_PENDING) nRet = 0;
						}
					}
				}
				else nRet = RECV_BUFFER_EMPTY;
			}
			return nRet;
		};

		// Closes the socket associated with this instance 
		// of the object and performs clean-up.
		void CloseSocket()
		{
			if ( this->m_blnIsBusy ) {
				this->m_blnIsBusy = false;
				if ((this->m_nSession++) > 0x0FFFD) this->m_nSession = 1;

				shutdown(this->m_ClientSock, 2);
				closesocket(this->m_ClientSock);

				this->m_ClientSock = 0;
				memset(&m_psForeignAddIn, 0 , sizeof(struct sockaddr_in));
				m_objAttachment.Clear();
			}
		};
	};

}

using namespace HTTP_CC;


#endif