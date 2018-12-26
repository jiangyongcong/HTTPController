#pragma once

typedef BOOL (WINAPI* FP_Init_StartUp)( const int v_iTmpSize );

typedef void (WINAPI* FP_Stop_Release)();

typedef int (WINAPI* FP_Opt_Add)( const char *v_szKey, const char* v_pData, const int v_iDataLen );

typedef char* (WINAPI* FP_Opt_Get)( const char *v_szKey, int &v_iDataLen );

typedef void (WINAPI* FP_Opt_Free)( const char *v_pData );

class CYxTmpFileMgr
{
public:
	CYxTmpFileMgr();
	~CYxTmpFileMgr();

public:
	// 加载基础资源库RsCommIO
	BOOL LoadRes();
	// 释放基础资源库RsCommIO
	void FreeRes();

public:
	// 初始化接口
	BOOL Init_StartUp( const int v_iTmpSize );
	// 终止接口
	void Stop_Release();
	// 保存缓存数据
	int Opt_Add( const char *v_szKey, const char* v_pData, const int v_iDataLen );
	// 获取缓存数据
	char* Opt_Get( const char *v_szKey, int &v_iDataLen );
	// 释放内存空间
	void Opt_Free( const char* v_pData );

private:
	HINSTANCE			m_hInstance;

private:
	FP_Init_StartUp		m_fpInit_Startup;
	FP_Stop_Release		m_fpStopRelease;
	FP_Opt_Add			m_fpOpt_Add;
	FP_Opt_Get			m_fpOpt_Get;
	FP_Opt_Free			m_fpOpt_Free;
};