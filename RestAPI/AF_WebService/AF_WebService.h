// AF_WebService.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
//#include "..\ShareSource\YxPlatform\YxPlatformInterfaceLib.h"

// CAF_WebServiceApp:
// 有关此类的实现，请参阅 AF_WebService.cpp
//

class CAF_WebServiceApp : public CWinApp
{
public:
	CAF_WebServiceApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现
public:
	//CYxPlatformInterfaceLib	m_YxPlatform;

	DECLARE_MESSAGE_MAP()

private:
	BOOL IsExist(const CString &strExeName, const DWORD dwProcessId);
};

extern CAF_WebServiceApp theApp;
