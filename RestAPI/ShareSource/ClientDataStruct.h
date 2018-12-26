#pragma once

#include <eh.h>
class CSE
{
public:

	static void MapSEtoCE()
	{ 
		_set_se_translator(TranslateSEtoCE); 
	}

	operator DWORD() 
	{ 
		return (m_er.ExceptionCode); 
	}
	PVOID GetExpAddress()
	{
		return (m_er.ExceptionAddress);
	}	
private:

	CSE(PEXCEPTION_POINTERS pep)
	{
		m_er      = *pep->ExceptionRecord;
		m_context = *pep->ContextRecord;
	}

	static void _cdecl TranslateSEtoCE(UINT dwEC,
		PEXCEPTION_POINTERS pep)
	{
		throw CSE(pep);
	}

private:
	EXCEPTION_RECORD m_er;      // CPU independent exception information
	CONTEXT          m_context; // CPU dependent exception information
};

namespace nsDfs
{
	enum _enFileType // 文件类型
	{
		enJpg = 0,
	};

	enum _enDownloadType // 下载类型
	{
		enMobile = 0,
		enWeb = 1,
	};

	enum _enFileProp // 文件属性
	{
		enOrgFile = 0, // 源文件
		enSimPic16 = 1,// 缩略图（只针对图片 16*16）
		enSimPic32 = 2,// 缩略图（只针对图片 32*32）
		enSimPic48 = 3,// 缩略图（只针对图片 48*48）
		enSimPic64 = 4,// 缩略图（只针对图片 64*64)
		enSimPic128 = 5,// 缩略图（只针对图片 128*128）
		enSrcFile = 0xFF, // 最大缩略图，上传的原始图片
		enSimSize = 6,
	};

	enum _enRetType
	{
		en_Succes=1,//成功
		en_Fail=-1,//失败
		en_BufferTooSmall=-2,//内存不足
		en_OverTime=-3,//超时
	};

	#pragma  pack( push, 1 )
	typedef struct _tagFileDataBuffer // 文件数据信息
	{
		DWORD   m_dwSN; // 业务流水号
		BYTE    m_byFileType; // 文件类型
		DWORD   m_dwFileLen;  // 文件大小
		char    *m_pFileBuff; // 文件数据
	}CFileDataBuffer;

	typedef struct _tagFileIndexBuffer // 文件索引信息
	{
		DWORD   m_dwSN; // 业务流水号
		BYTE    m_byFileNameLen; // 文件名长度
		char    m_szFileName[MAX_PATH]; // 文件名（用文件名索引文件数据）
	}CFileIndexBuffer;

	typedef struct _tagFileWebUrl // 文件Web地址
	{
		DWORD   m_dwSN; // 业务流水号
		BYTE    m_byWebUrlLen; // 文件Web访问地址长度（只针对图像文件）
		char    m_szWebUrl[MAX_PATH]; // Web访问地址
	}CFileWebUrl;
	#pragma  pack( pop )


	// 客户端文件上传结果回调，如果上传成功返回文件名称，如果返回的文件名称长度为0表明上传失败（将来可能要返回失败码表明失败类型）
	typedef int (CALLBACK* CBF_Upload)(DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[]);

	// 手机终端文件下载结果回调，如果下载成功返回文件数据，如果返回的文件长度为0表明下载失败（将来可能要返回失败码表明失败类型）
	typedef int (CALLBACK*CBF_Download_Mobile)(DWORD   _dwSN,// 业务流水号
		BYTE    _byFileType,// 文件类型
		DWORD   _dwFileLen, // 文件大小
		char    *_pFileBuff) ;// 文件数据);

	// 网页终端文件下载结果回调，如果下载成功返回文件Url，如果返回的Url长度为0表明下载失败（将来可能要返回失败码表明失败类型）
	typedef int (CALLBACK*CBF_Download_Web)(DWORD   _dwSN, // 业务流水号
		BYTE    _byWebUrlLen, // 文件Web访问地址长度（只针对图像文件）
		char    _szWebUrl[MAX_PATH]); // Web访问地址);

	class C_Dfs_Client_CB// Dfs客户端业务回调接口
	{
	public:
		CBF_Upload	m_cbUpload;
		CBF_Download_Mobile m_cbDownload;
		CBF_Download_Web    m_cbDownload_Web;
	public:
		C_Dfs_Client_CB(void):m_cbUpload(NULL),m_cbDownload(NULL),m_cbDownload_Web(NULL)
		{

		}

		virtual ~C_Dfs_Client_CB(void){};

	public:

		// 客户端文件上传结果回调，如果上传成功返回文件名称，如果返回的文件名称长度为0表明上传失败（将来可能要返回失败码表明失败类型）
		int CB_Upload(DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[])
		{
			if (m_cbUpload!=NULL)
			{

				return m_cbUpload(_dwSN,_byFileNameLen,_szFileName);
			}
			return 0;
		}

		// 手机终端文件下载结果回调，如果下载成功返回文件数据，如果返回的文件长度为0表明下载失败（将来可能要返回失败码表明失败类型）
		int CB_Download_Mobile(DWORD   _dwSN,// 业务流水号
			BYTE    _byFileType,// 文件类型
			DWORD   _dwFileLen, // 文件大小
			char    *_pFileBuff) // 文件数据);
		{
			if (m_cbDownload!=NULL)
			{
				return m_cbDownload(_dwSN,_byFileType,_dwFileLen,_pFileBuff);
			}
			return 0;
		}

		// 网页终端文件下载结果回调，如果下载成功返回文件Url，如果返回的Url长度为0表明下载失败（将来可能要返回失败码表明失败类型）
		int CB_Download_Web(DWORD   _dwSN, // 业务流水号
			BYTE    _byWebUrlLen, // 文件Web访问地址长度（只针对图像文件）
			char    _szWebUrl[MAX_PATH])
		{
			if (m_cbDownload_Web!=NULL)
			{
				m_cbDownload_Web(_dwSN,_byWebUrlLen,_szWebUrl);
			}
			return 0;
		}

	};
}

using namespace nsDfs;

