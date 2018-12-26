// MemPoolMgr.cpp: implementation of the CMemPoolMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemPoolMgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#define _MEMPOOL_DEBUG

CMemPoolMgr::CMemPoolMgr(int poolsize, int unitsize, unsigned int freesize)
{

	m_MaxPoolSize = poolsize;
	m_MaxUnitSize = unitsize;
	m_MaxFreeSize = freesize;
	Trace("POOLSIZE:%dM, UNITSIZE:%dKB, FREESIZE:%d\n\n", m_MaxPoolSize, m_MaxUnitSize, m_MaxFreeSize);
	CLock v_lock(&m_MemPoolListLock);
	CMemPool *v_pMemPool = new CMemPool(m_MaxPoolSize * 1024 * 1024, m_MaxUnitSize * 1024);
	m_MemPoolList.push_back((int)v_pMemPool);
}


CMemPoolMgr::~CMemPoolMgr()
{
	FreeMemory();
}

char* CMemPoolMgr::AllocateMemory(unsigned int size)
{
	char* pData = NULL;
	CLock v_lock(&m_MemPoolListLock);

	list <int> ::iterator v_iter = m_MemPoolList.begin();
	for (v_iter; v_iter != m_MemPoolList.end(); v_iter++)
	{
		pData = ((CMemPool*)(*v_iter))->AllocateMemory(size);
		if (NULL != pData)
			break;
	}

	//分配失败, 从空闲队列中取一个出来分配。
	if (NULL == pData)
	{
		CLock v_lock(&m_MemPoolListFreeLock);
		v_iter = m_MemPoolListFree.begin();
		for (v_iter; v_iter != m_MemPoolListFree.end(); v_iter++)
		{
			pData = ((CMemPool*)(*v_iter))->AllocateMemory(size);
			if (NULL != pData)
			{
				m_MemPoolList.push_back((int)(*v_iter));
				m_MemPoolListFree.erase(v_iter);
				break;
			}
		}
	}

	//分配失败, 重新申请一个
	if (NULL == pData)
	{
		CMemPool *v_pMemPool = new CMemPool(m_MaxPoolSize * 1024 * 1024, m_MaxUnitSize * 1024);
		m_MemPoolList.push_back((int)v_pMemPool);
		pData = v_pMemPool->AllocateMemory(size);
	}

#ifdef _MEMPOOL_DEBUG
	Trace("AllocateMemory 内存地址：0x%0x\n", pData);
	TraceMemory();
#endif
	return pData;
}


void CMemPoolMgr::ReleaseMemory(char * pData)
{

#ifdef _MEMPOOL_DEBUG
	Trace("ReleaseMemory 内存地址：0x%0x\n", pData);
#endif

	BOOL bRelease = FALSE;
	list<int>::iterator v_iter;
	CLock v_lock(&m_MemPoolListLock);
	for (v_iter = m_MemPoolList.begin(); v_iter != m_MemPoolList.end(); v_iter++)	//查找要释放的内存所处在的内在块
	{
		if(TRUE == ((CMemPool*)(*v_iter))->ReleaseMemory(pData))	//是否找到
		{
			if(TRUE == ((CMemPool*)(*v_iter))->IsFree())			//检测是否空闭， 若空闭放入空闭队列
			{
				CLock v_lock(&m_MemPoolListFreeLock);
				if (m_MemPoolListFree.size() < m_MaxFreeSize)
				{
					CMemPool *v_pMemPool = (CMemPool*)(*v_iter);
					m_MemPoolListFree.push_back((int)v_pMemPool);
				}
				else
				{
					delete (CMemPool*)(*v_iter);
				}
				m_MemPoolList.erase(v_iter);
			}
			bRelease = TRUE;
			break;
		}
	}

	if (!bRelease)
	{
		Trace("ReleaseMemory 内存地址：%0x没有被释放\n", pData);
	}

#ifdef _MEMPOOL_DEBUG
	TraceMemory();
#endif

}


void CMemPoolMgr::TraceMemory()
{
	Trace("正在使用的队列情况\n\n");
	list <int> ::iterator v_iter;
	CLock v_lock1(&m_MemPoolListLock);
	v_iter = m_MemPoolList.begin();
	for (v_iter = m_MemPoolList.begin(); v_iter != m_MemPoolList.end(); v_iter++)
	{
		((CMemPool*)(*v_iter))->TraceMemory();
	}

	Trace("空闭队列情况\n\n");
	CLock v_lock2(&m_MemPoolListFreeLock);
	v_iter = m_MemPoolListFree.begin();
	for (v_iter = m_MemPoolListFree.begin(); v_iter != m_MemPoolListFree.end(); v_iter++)
	{
		((CMemPool*)(*v_iter))->TraceMemory();
	}
	Trace("*******************************************************\n\n");
}



void CMemPoolMgr::FreeMemory(void)
{
	Trace("******************FreeMemory()******************\n");
	TraceMemory();

	list <int> ::iterator v_iter; 
	CLock v_lock1(&m_MemPoolListLock);
	v_iter = m_MemPoolList.begin();
	while(v_iter != m_MemPoolList.end())
	{
		delete (CMemPool*)(*v_iter);
		m_MemPoolList.erase(v_iter);
		v_iter = m_MemPoolList.begin();
	}

	CLock v_lock2(&m_MemPoolListFreeLock);
	v_iter = m_MemPoolListFree.begin();
	while(v_iter != m_MemPoolListFree.end())
	{
		delete (CMemPool*)(*v_iter);
		m_MemPoolListFree.erase(v_iter);
		v_iter = m_MemPoolListFree.begin();
	}
}

