#include "StdAfx.h"
#include ".\sysconfig.h"

CSysConfig    CSysConfig::s_SysConf;

CSysConfig::CSysConfig(void)
{
	m_ui16ListenPort = 7779;
	m_iMaxTmpSize = 1500;
	m_pFaviconData = NULL;
	m_iFaviconDataLen = 0;
}

CSysConfig::~CSysConfig(void)
{
	if( m_pFaviconData != NULL )
	{
		delete m_pFaviconData;
		m_pFaviconData = NULL;
	}

	m_iFaviconDataLen = 0;
}

int CSysConfig::load()
{
	char           szModuleDir[MAX_PATH];
	std::string    strInIFile;

	char   szDriver[MAX_PATH];
	char   szDir[MAX_PATH];
	char   szFName[MAX_PATH];
	char   szExt[MAX_PATH];

	::GetModuleFileName( NULL, szModuleDir, sizeof(szModuleDir) );

	::_splitpath_s( szModuleDir, szDriver,sizeof(szDriver), szDir,sizeof(szDir), szFName,sizeof(szFName), szExt,sizeof(szExt) );
	::_makepath_s( szModuleDir,sizeof(szDriver), szDriver, szDir, NULL, NULL );

	// 加载favicon.ico
	LoadFavicon(szModuleDir);

	return 0;
}

void CSysConfig::LoadFavicon( char v_szModuleDir[] )
{
	// 开始写文件
	std::string strFavicon = v_szModuleDir;
	strFavicon += "favicon.ico";
	CFile	clsFile;
	try
	{
		// 打开文件
		if( !clsFile.Open(strFavicon.c_str(),CFile::modeRead) )
		{
			return;
		}

		// 判断文件句柄是否非法
		if( clsFile.m_hFile == CFile::hFileNull )
		{
			return;
		}

		// 获取文件大小
		m_iFaviconDataLen = (int)clsFile.GetLength();
		if( m_iFaviconDataLen <= 0 )
		{
			return;
		}

		// 分配足够的内存空间
		m_pFaviconData = new char[m_iFaviconDataLen];
		if( m_pFaviconData == NULL )
		{
			return;
		}

		// 移动至文件开头
		clsFile.SeekToBegin();
		if( clsFile.Read(m_pFaviconData,m_iFaviconDataLen) != m_iFaviconDataLen )
		{
			return;
		}
	}
	catch(...)
	{
		if( m_pFaviconData != NULL )
		{
			delete m_pFaviconData;
			m_pFaviconData = NULL;
		}

		m_iFaviconDataLen = 0;

		return;
	}

	// 清空缓存区，关闭文件
	clsFile.Close();
}