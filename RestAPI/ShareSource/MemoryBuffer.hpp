#pragma once
namespace nsShare_Inc
{
	template<unsigned BLOCK_SIZE>
	class CMemoryBuffer
	{
	private:
		class _node_t
		{
		public:
			_node_t(void)
			{
				m_pNextNode  = NULL;
				memset( m_szBlock, 0x00, BLOCK_SIZE );
			}
			~_node_t(void)
			{

			}

		public:
			_node_t*    m_pNextNode;
			unsigned char       m_szBlock[BLOCK_SIZE];//存放实际数据的地方
		};

	public:
		class _pos_t
		{
			friend class CMemoryBuffer;

		public:
			_pos_t(void)
			{
				m_iPos  = 0;
				m_pNode = NULL;
				m_iPosInNode = 0;
			}
			~_pos_t(void)
			{

			}

		public:
			bool operator<(const _pos_t right)
			{
				return this->m_iPos < right.m_iPos;
			}
			_pos_t operator++(int)
			{
				_pos_t __temp(*this);

				m_iPosInNode ++;
				m_iPos ++;

				if( m_iPosInNode >= BLOCK_SIZE )
				{
					if( NULL == m_pNode->m_pNextNode )
					{
						m_pNode->m_pNextNode = new _node_t;
					}
					m_pNode = m_pNode->m_pNextNode;
					m_iPosInNode = 0;
				}

				return __temp;
			}

		private:
			int         m_iPos;
			_node_t*    m_pNode;
			int         m_iPosInNode;
		};

	public:
		CMemoryBuffer(void)
		{
			m_posEnd.m_iPos = 0;
			m_posEnd.m_pNode = &m_Head;
			m_posEnd.m_iPosInNode = 0;
		}
		~CMemoryBuffer(void)
		{
			this->Clear();
			this->Shrink();
		}

	private:
		_node_t    m_Head;

	public:
		_pos_t     m_posEnd;

	public:
		_pos_t begin()
		{
			_pos_t __begin;
			__begin.m_iPos = 0;
			__begin.m_pNode = &m_Head;
			__begin.m_iPosInNode = 0;
			return __begin;
		}
		_pos_t end() const
		{
			return m_posEnd;
		}
		int data_size(void) const
		{
			return m_posEnd.m_iPos;
		}
		unsigned char operator[](_pos_t __pos)
		{
			return __pos.m_pNode->m_szBlock[__pos.m_iPosInNode];
		}

	public:
		
		void Shrink()	//收缩不使用的内存
		{
			_node_t* pNode = m_posEnd.m_pNode->m_pNextNode;
			while( NULL != pNode )
			{
				m_posEnd.m_pNode->m_pNextNode = pNode->m_pNextNode;
				delete pNode;
				pNode = m_posEnd.m_pNode->m_pNextNode;
			}
		}
		void Clear()
		{
			m_posEnd.m_iPos = 0;
			m_posEnd.m_pNode = &m_Head;
			m_posEnd.m_iPosInNode = 0;
		}
		void Clean()
		{
			this->Clear();
			this->Shrink();
		}
		int Append(const unsigned char* v_szBuf, int v_iCount)
		{
			int iLeftAppend = v_iCount;
			while( 0 < iLeftAppend )
			{
				int iLeftInNode = BLOCK_SIZE - m_posEnd.m_iPosInNode;
				int iCopy = min( iLeftInNode, iLeftAppend );
				memcpy( 
					m_posEnd.m_pNode->m_szBlock + m_posEnd.m_iPosInNode, 
					v_szBuf + ( v_iCount - iLeftAppend ), 
					iCopy
					);
				m_posEnd.m_iPosInNode += iCopy;
				m_posEnd.m_iPos += iCopy;
				if( BLOCK_SIZE == m_posEnd.m_iPosInNode )
				{
					m_posEnd.m_iPosInNode --;
					m_posEnd.m_iPos --;
					m_posEnd ++;
				}

				iLeftAppend -= iCopy;
			}

			return 0;
		}
		int Append(unsigned char __ui8)
		{
			m_posEnd.m_pNode->m_szBlock[m_posEnd.m_iPosInNode] = __ui8;
			m_posEnd ++;
			return 0;
		}
		int CopyTo(unsigned char v_szBuf[], int v_iLen)
		{
			int iLeftToCopy = min( v_iLen, this->data_size() );
			int iPos = 0;

			_node_t* pNode = &m_Head;
			while( 0 < iLeftToCopy )
			{
				if( iLeftToCopy < BLOCK_SIZE )
				{
					memcpy( v_szBuf + iPos, pNode->m_szBlock, iLeftToCopy );
					iPos += iLeftToCopy;
					iLeftToCopy = 0;
					break;
				}else
				{
					memcpy( v_szBuf + iPos, pNode->m_szBlock, BLOCK_SIZE );
					iPos += BLOCK_SIZE;
					iLeftToCopy -= BLOCK_SIZE;
					pNode = pNode->m_pNextNode;
					if( NULL == pNode )
					{
						break;
					}
				}
			}

			return iPos;
		}
	};

}
