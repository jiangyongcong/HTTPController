// MyTool.h: interface for the CMyTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYTOOL_H__6B7665CE_BD59_46DB_B704_4566D6D51866__INCLUDED_)
#define AFX_MYTOOL_H__6B7665CE_BD59_46DB_B704_4566D6D51866__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <windows.h>
//#include <stdlib.h>
//#include <assert.h>
//#include <stdarg.h>

#include <direct.h>
#include <errno.h>
#include <io.h>
#include "tlhelp32.h"
#include "winsock2.h"
#include "afxsock.h"

class CMyTool  
{
public:
	CMyTool();
	virtual ~CMyTool();
	
	// SetFileToCurrentTime - sets last write time to current system time
	// Return value - TRUE if successful, FALSE otherwise
	// hFile  - must be a valid file handle
	static BOOL SetFileToCurrentTime(HANDLE hFile);

	/*
	* 检查应用程序进程是否已经存在
	* 输入应用程序名字m_pszExeName和进程ID GetCurrentProcessId()
	*/
	static BOOL IsProcessExist(LPCSTR lpcstrExeName, const DWORD dwProcessId);
	
	/// 判断文件或路径是否存在
	static BOOL IsFileOrDirExists(const char *v_pszPath);

	/// 创建路径，包括子目录
	static BOOL MakeMultiDir(const char v_szPath[]);


	/** 
	 * @brief 获取指定路径下的文件列表
	 * @param[out] v_arrCStr 返回文件列表
	 * @param[in] pszDir 指定路径
	 * @param[in] pszFilter 指定文件扩展名，*.*表示所有文件，*.txt表示文本文件
	 *
	 * @return void
	 */
	static BOOL GetFileListFromDir(CStringArray &v_arrCStr, LPCSTR pszDir, LPCSTR pszFilter = "*.*");

	/** 
	 * @brief 删除指定类型文件及目录
	 * @param[in] pszDir 指定目录
	 * @param[in] pszFilter 指定文件扩展名，*.*表示所有文件，*.txt表示文本文件
	 * @param[in] bIsDelDir 指定是否删除目录，TRUE-删除，FALSE-不删除
	 *
	 * @return void
	 */
	static void DeleteDirAndFile(LPCSTR pszDir, LPCSTR pszFilter = "*.*", BOOL bIsDelDir = FALSE);

	/** 
	 * @brief 获取模块当前路径及模块名称
	 * @param[out] v_pModuleDir 输出当前模块路径
	 * @param[in] _nDirLen 指定存放路径缓冲区长度
	 * @param[out] v_pModuleName 输出当前模块名称，包含扩展名
	 * @param[in] _nNameLen 指定存放名称缓冲区长度
	 *
	 * @return void
	 */
	static void GetCurrModulePath( char* v_pModuleDir, int _nDirLen, char* v_pModuleName, int _nNameLen );

	/** 
	 * @brief 从指定文件全路径中提取路径和文件名称
	 * @param[in] _pszFilePath 指定当前文件全路径
	 * @param[out] v_pszFileDir 输出该路径
	 * @param[in] _nDirSize 指定存放路径缓冲区长度
	 * @param[out] v_pszFileName 输出文件名称，包含扩展名
	 * @param[in] _nNameSize 指定存放名称缓冲区长度
	 * @param[in] _bIncExt 文件名是否包含扩展名
	 *
	 * @return void
	 */
	static void ExtractFilePath(
		LPCSTR _pszFilePath, 
		LPSTR v_pszFileDir, int _nDirSize, 
		LPSTR v_pszFileName, int _nNameSize,
		BOOL _bIncExt = FALSE);


	/** 
	 * @brief 按照指定的分隔符提取子串
	 * @param[in] v_szSrc 原字符串
	 * @param[out] v_szDest 目标缓存区，返回提取的字符串
	 * @param[in] v_iMaxLen 限定目标缓冲区大小
	 * @param[in] v_iSubStr 指定提取分隔后的字符串索引
	 * @param[in] v_cDelim  指定分隔符
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */
	static BOOL ExtractSubString(
		const char* v_szSrc, 
		char* v_szDest, 
		int v_iMaxLen,
		int v_iSubStr, 
		char v_cDelim);

	static BOOL StrTokenEx();

	/// 按指定的分隔符拆解字符串，分隔符支持子串
	static int SplitCString(LPCSTR _lpcstrSrc, LPCSTR _lpcstrSplit, CStringArray& v_arrCString);

	/*
	* 获取当前系统日期时间
	*
	*/
	static void GetCurrDateTime(char *v_pszCurrTiem, int nLen, int nDateType/*=0*/);

	/*
	* 构造指定的时间
	*
	*/
	static time_t ConstructDatetime(int _nY, int _nMon, int _nD, int _H, int _nMin, int _nS);
	

	/// 将标准日期串2017-06-14转换为3字节数组
	static void StrDateToChar(LPCSTR _lpcstrDate, char v_szDate[]);

	/// 将标准时间串2017-06-14 12:30:56转换为6字节数组
	static void StrDatetimeToChar(LPCSTR _lpcstrDatetime, char v_szDatetime[]);


	//static void CharToStrDate(LPCSTR _lpcstrDate, PCHAR v_pszDate);
	

	/*
	 * @brief 添加7E头尾并转义
	 *
	 * @param[in] _szOldBuf 处理前的数据
	 * @param[in] _iOldLen 处理前的数据长度
	 * @param[out] _szNewBuf 处理后的数据
	 *
	 * @return 处理后的数据长度
	 * @see 转义说明
			7E-->7D+02
			7D-->7D+01
	*/
	static int Add7EforData(char _szOldBuf[], char _szNewBuf[], int _iOldLen);

	/** 
	 * @brief 计算异或校验和 \n
	 * 从第一个字节开始，逐个字节进行异或，占用1个字节
	 * @param[in] _szBuf 待处理的数据
	 * @param[in] _iLen 数据长度
	 *
	 * @return 校验和
	 *
	 */
	static BYTE GetXorCheckSum(const BYTE *_szBuf, const int _iLen);

	//手机号不足15时填充空格0x20
	static void FillBlackToMobile(char _szMobile[], int _iLen);

	static BOOL ByteTimeToCTime(const char szDatetime[], CTime &ctTime);
    static BOOL CheckTimeLimit(char* pcCurTime, char*pcTimeBegin, char*pcTimeEnd);

	/** 
	 * @brief 获取本机地址
	 *
	 * @param[out] strIPAddr 本机IP地址字符串
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 */
	static BOOL GetLocalHostIP(CString& strIPAddr);

	/** 
	 * @brief 根据进程名称获取进程ID
	 *
	 * @param[in] 进程名称
	 *
	 * @return 进程ID值
	 *
	 */
	static DWORD GetProcessIdByName( LPCTSTR pszExeFile );

	/** 
	 * @brief 根据获取进程ID进程名称
	 *
	 * @param[in] 进程ID
	 * @param[out] 进程名称
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */	
	static BOOL GetProcessNameById( DWORD dwProcessId, CString& lpstrProcName );

	/** 
	 * @brief 启动进程
	 *
	 * @param[in] 进程ID
	 * @param[out] 进程名称
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */		
	static BOOL StartProcess( LPCTSTR lpcstrProcessName, LPTSTR lpstrProcessPath );

	/** 
	 * @brief 关闭进程
	 *
	 * @param[in] 进程ID
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */		
	static BOOL CloseProcess( DWORD dwProcessId );


	
	/** 
	 * @brief 判断Ocx是否已注册
	 *
	 * @param[in] Ocx的ClsId串，如：{BDEB0088-66F9-4A55-ABD2-0BF8DEEC1196}
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */		
	static BOOL IsRegOcx( LPCSTR lpcstrCLSID );

	/** 
	 * @brief 注册Ocx
	 *
	 * @param[in] Ocx文件的所在路径串，如：C:\TeeChart\Teechart8.ocx
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */
	static BOOL RegOcx( LPCSTR lpcstrOcxPath );

	/** 
	 * @brief 反注册Ocx
	 *
	 * @param[in] Ocx文件的所在路径串，如：C:\TeeChart\Teechart8.ocx
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */
	static BOOL UnRegOcx( LPCSTR lpcstrOcxPath );

	
};

#endif // !defined(AFX_MYTOOL_H__6B7665CE_BD59_46DB_B704_4566D6D51866__INCLUDED_)
