#pragma once

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


// 客户端文件上传结果回调，如果上传成功返回文件名称，如果返回的文件名称长度为0表明上传失败（将来可能要返回失败码表明失败类型）
typedef int (CALLBACK* CBF_Upload)(DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[]);

// 手机终端文件下载结果回调，如果下载成功返回文件数据，如果返回的文件长度为0表明下载失败（将来可能要返回失败码表明失败类型）
typedef int (CALLBACK* CBF_Download_Mobile)(DWORD   _dwSN,// 业务流水号
										   BYTE    _byFileType,// 文件类型
										   DWORD   _dwFileLen, // 文件大小
										   char    *_pFileBuff) ;// 文件数据);

// 网页终端文件下载结果回调，如果下载成功返回文件Url，如果返回的Url长度为0表明下载失败（将来可能要返回失败码表明失败类型）
typedef int (CALLBACK* CBF_Download_Web)(DWORD   _dwSN, // 业务流水号
										BYTE    _byWebUrlLen, // 文件Web访问地址长度（只针对图像文件）
										char    _szWebUrl[MAX_PATH]); // Web访问地址);

typedef DWORD (WINAPI* FP_Create)();

typedef BOOL (WINAPI* FP_Init_Startup)( DWORD _dwObjId, int v_iNodeCnt, char* v_szIp[], WORD v_wPort[] );

typedef BOOL (WINAPI* FP_Terminate)( DWORD _dwObjId );

typedef BOOL (WINAPI* FP_ToUpload)( DWORD _dwObjId , DWORD   _dwSN,BYTE    _byFileType,DWORD   _dwFileLen,char    *_pFileBuff);

typedef BOOL (WINAPI* FP_ToDownload)( DWORD _dwObjId ,
									  DWORD   _dwSN,
									  BYTE    _byFileNameLen,
									  char    _szFileName[],
									  const BYTE v_byDownloadType,
									  const BYTE v_byFileProp );


// 客户端同步文件下载
typedef int (WINAPI* FP_ToDownload_Sync)( DWORD _dwObjId, 
										  DWORD   _dwSN,// 业务流水号
										  BYTE    _byFileNameLen,// 文件名长度
										  char    _szFileName[], // 文件名（用文件名索引文件数据）
										  const BYTE v_byDownloadType, //下载类型(手机or网页)
										  const BYTE v_byFileProp,
										  char* _szFileContent,
										  int&	 _iLen
										 );//文件内容

typedef void (WINAPI* FP_SetICallback_Dfs_Client_Impl)( DWORD _dwObjId,
														CBF_Upload		   _cb_Upload,
														CBF_Download_Mobile  _cb_Download_Mobile,
														CBF_Download_Web     _cb_Download_Web );

class CClientImpl
{
public:	
	CClientImpl();
	~CClientImpl();

public:
	BOOL Load_Impl();
	void UnLoad_Impl();
	BOOL InitStartup( int v_iNodeCnt, char* v_szIp[], WORD v_wPort[] );
	BOOL Terminate();
	BOOL ToUpload(DWORD _dwSN,BYTE _byFileType,DWORD _dwFileLen,char *_pFileBuff);
	BOOL ToDownload( DWORD _dwSN, BYTE _byFileNameLen, char _szFileName[], const BYTE v_byDownloadType, const BYTE v_byFileProp );
	int  ToDownload_Sync( DWORD   _dwSN,// 业务流水号
						 BYTE    _byFileNameLen,// 文件名长度
						 char    _szFileName[], // 文件名（用文件名索引文件数据）
						 const BYTE v_byDownloadType, //下载类型(手机or网页)
						 const BYTE v_byFileProp,
						 char* _szFileContent,
						 int&	 _iLen
						);//文件内容

	void SetICallback_Dfs_Client_Impl( CBF_Upload		   _cb_Upload,
									   CBF_Download_Mobile  _cb_Download_Mobile,
									   CBF_Download_Web     _cb_Download_Web );

private:
	DWORD Create();

private:
	DWORD m_dwObjId;
private:
	HINSTANCE m_hInstance; 
	FP_Create	m_Create;
	FP_Init_Startup m_InitStartup;
	FP_Terminate m_Stop_Release;
	FP_ToUpload		m_ToUpload;
	FP_ToDownload   m_ToDownLoad;
	FP_ToDownload_Sync	m_ToDownLoad_Sync;
	FP_SetICallback_Dfs_Client_Impl	m_SetICallback_Dfs_Client_Impl;
};