// MemPool.cpp: implementation of the CMemPool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemPool.h"
#include <assert.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemPool::CMemPool(unsigned int poolsize, unsigned int unitsize)
{
	m_nPoolSize      = poolsize;
	m_nBlockUnitSize = unitsize;
	InitMemPool(); // Init the Memory Pool
}


CMemPool::~CMemPool()
{
	ClearMemPool(); 
	FreeMemory();
}	


//计算要求划分成多少块
unsigned int CMemPool::CalcBlockSize(unsigned int blocksize)
{
	assert(blocksize>0);
	unsigned int size = 0;
	try
	{
		size = blocksize / m_nBlockUnitSize;
		if (blocksize % m_nBlockUnitSize)
		{
			size++;
		}
	}
	catch (...)
	{
		size = 0;	
	}
	return size;
}



void CMemPool::InitMemPool()
{
	try
	{
		m_blocksize = m_nPoolSize / m_nBlockUnitSize;
		m_MemoryUnitList = new MemoryBlock[m_blocksize];
		m_pPoolEntry = (char*) new char[m_nPoolSize];

		memset(m_MemoryUnitList, 0x00, m_blocksize);
		memset(m_pPoolEntry, 0x00, m_nPoolSize);
		
		for (unsigned int i = 0; i < m_blocksize; i++)
		{
			m_MemoryUnitList[i].pData = (char*)(m_pPoolEntry + i * m_nBlockUnitSize);
			m_MemoryUnitList[i].DataSize = 0;
		}
	}
	catch (...)
	{
		// TODO : Catch the Exception of Memory allocation
	}
}


char* CMemPool::AllocateMemory(unsigned int size)
{
	assert(size > 0);

	PMemoryBlock pMemoryBlock = NULL;
	try
	{
		pMemoryBlock = RequestBlock(size);
	}
	catch (...)
	{
		pMemoryBlock=NULL;
	}

	if (NULL!=pMemoryBlock)
	{
		return pMemoryBlock->pData;
	}
	else
	{
		return NULL;
	}
}



PMemoryBlock CMemPool::RequestBlock(unsigned int blocksize)
{
	assert(blocksize > 0);
	unsigned int total = 0;
	PMemoryBlock pMemoryBlock = NULL;
	//计算用户申请的内存大小，需要提供的内存块数量
	unsigned int unitsize  = CalcBlockSize(blocksize);
	for (unsigned int i = 0; i < m_blocksize; i++)
	{
		if(m_MemoryUnitList[i].DataSize == 0)
		{
			total ++;
		}
		else
		{
			total = 0;
		}

		if (total == unitsize)
		{
			//计算预分配给用户的内存块首地址
			unsigned int index = i - unitsize + 1;
			pMemoryBlock = m_MemoryUnitList + index;
			for (unsigned int k = 0; k < unitsize; k++)
			{
				m_MemoryUnitList[index + k].DataSize = unitsize - k;
			}
			break;
		}
	}
	return pMemoryBlock;
}




BOOL CMemPool::ReleaseMemory(char * pData)
{
	assert(NULL != pData);
	for (unsigned int i = 0; i < m_blocksize; i++)
	{
		if(m_MemoryUnitList[i].pData == pData)
		{
			unsigned int DataSize = m_MemoryUnitList[i].DataSize;
			for(unsigned int k = 0; k < DataSize; k++)
			{
				m_MemoryUnitList[i + k].DataSize = 0;
			}
			return TRUE;
		}
	}
	return FALSE;
}


void CMemPool::FreeMemory()
{
	if (m_pPoolEntry)
	{
		delete[] m_pPoolEntry; // Free the memory
		m_pPoolEntry = NULL; // Free the Pointer
	}
	
	if (m_MemoryUnitList)
	{
		delete [] m_MemoryUnitList;
		m_MemoryUnitList = NULL;
	}
}


void CMemPool::TraceMemory()
{
	unsigned int DataSize = 0; 
	unsigned int m_UsedTotal = 0;
	unsigned int m_ApplyTotal = 0;
	for (unsigned int i = 0; i < m_blocksize; i++)
	{
		if(m_MemoryUnitList[i].DataSize >= 1)
		{
			m_UsedTotal ++;
			if (m_MemoryUnitList[i].DataSize >= DataSize)
			{
				Trace("block(%d):PoolEntry Address(0x%0x), UnitList Address:(0x%0x), (%4d -> %4d)\n", m_ApplyTotal, m_MemoryUnitList[i].pData, m_MemoryUnitList + i, i, m_MemoryUnitList[i].DataSize + i - 1);
			}
		}
		if(m_MemoryUnitList[i].DataSize == 1)
		{
			m_ApplyTotal ++;
		}
		DataSize = m_MemoryUnitList[i].DataSize;
	}

	Trace("m_nPoolSize:%dM\n", m_nPoolSize / 1024 / 1024);
	Trace("m_nBlockUnitSize:%dKB\n", m_nBlockUnitSize / 1024);
	Trace("m_blocksize:%d\n", m_blocksize);
	Trace("m_pPoolEntry Address :0x%0x\n", m_pPoolEntry);
	Trace("m_MemoryUnitList Address:0x%0x\n", m_MemoryUnitList);
	Trace("Apply Total:%d\n", m_ApplyTotal);
	Trace("Used  Total:%d\n", m_UsedTotal);
}

void CMemPool::ClearMemPool()
{
	for (unsigned int i = 0; i < m_blocksize; i++)
	{
		m_MemoryUnitList[i].DataSize = 0;
	}
}

char* CMemPool::GetMemPoolEntry() const
{
	return m_pPoolEntry;
}

unsigned int CMemPool::GetMemPoolSize() const
{
	return m_nPoolSize;
}

unsigned int CMemPool::GetCurUsedSize() const
{
	int total = 0;
	for (unsigned int i = 0; i < m_blocksize; i++)
	{
		if(m_MemoryUnitList[i].DataSize != 0)
		{
			total ++;
		}
	}
	return total;
}


BOOL CMemPool::IsFree() const
{
	for (unsigned int i = 0; i < m_blocksize; i++)
	{
		if(m_MemoryUnitList[i].DataSize != 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}


static char g_sProgName[128] = {"MemPool"};
static char g_sLogFile[128]  = {"MemPool.txt"};

/*-----------------------------------------------------------------------
* name:    取当前时间
* input:
* output:  _stime  -- 时间字符串
* return:
*-----------------------------------------------------------------------*/
void TraceCurrenttime(char * _stime) 
{
	time_t timep;
	struct tm * p;
	time(&timep);
	p = localtime(&timep);
	sprintf(_stime, "%4d-%02d-%02d %02d:%02d:%02d ", (1900 + p->tm_year), (1
		+ p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}

void TraceCurrentFile(char* _szFile)
{
	time_t timep;
	struct tm * p;
	time(&timep);
	p = localtime(&timep);
	sprintf(_szFile, "(%4d-%02d-%02d %02dh)%s", (1900 + p->tm_year), (1+ p->tm_mon), p->tm_mday, p->tm_hour, g_sLogFile);
}




void Trace(char *szFormat, ...)
{
	FILE *m_nLogFd = NULL;
	char _stime[128] = {0x00};
	memset(_stime, 0, sizeof(_stime));

	char v_szLogFile[256] = {0};
	TraceCurrentFile(v_szLogFile);

	char sLogFile[256] = {0x00};
	memset(sLogFile, 0, sizeof(sLogFile));
    sprintf(sLogFile, (char *)"%s\\%s", g_sProgName, v_szLogFile);

	char szBuffer[4096] = {0x00};
	memset(szBuffer, 0, sizeof(szBuffer));

	if (!CreateDirectory(g_sProgName, NULL))
    {
#ifdef _DEBUG
		OutputDebugString("创建目录不成功\r\n");      
#endif
	}

	TraceCurrenttime(_stime);

	try
	{
		va_list pArguments;
		va_start(pArguments, szFormat);
		vsprintf(szBuffer, szFormat, pArguments);
		va_end(pArguments);

		if((m_nLogFd = fopen(sLogFile, "a")) == NULL)
		{
			return ;
		}
		fwrite((_stime),strlen(_stime), 1, m_nLogFd);
		fwrite((szBuffer),strlen(szBuffer), 1, m_nLogFd);
		fflush(m_nLogFd);
		fclose(m_nLogFd);
		m_nLogFd = NULL;
	}
	catch (...)
	{
		if (m_nLogFd)
		{
			fclose(m_nLogFd);
			m_nLogFd = NULL;
		}
	}
}
