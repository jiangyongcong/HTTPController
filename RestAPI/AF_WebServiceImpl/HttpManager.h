#pragma once

#include "HttpClient.h"

#define  MAX_CONN_SIZE 65536 + 1

class CHttpManager : public IConnectionFactory
	,public LogSpace::IWndLog
	,public LogSpace::IFileLog
{

public:
	static CHttpManager* _s_pHttpManager;
public:
	CHttpManager(void);
	~CHttpManager(void);

public:	
	int  Init( unsigned short v_usPort );
	void Release();

public:
	//上层应用必须重载此函数，来创建自己需要的IConnection对象
	virtual IConnection* CreateConnectionFromRemote( 
		IConnection**v_ppConnection, DWORD v_dwSvrID,
		int v_ConnID, char *v_pBuffRead, int nLen,
		u_long ulRemoteAddr, u_short sPort );

public:
	//窗口日志输出
	virtual int toShowWndLog(int v_iTypeLog, const char v_szLog[]);
	//文件日志输出
	virtual int toWriteFileLog( const char v_szDescription[], const char v_szBuf[], int v_iLenBuf);
	void TimerService_1000ms();

public: // 重载IWndLog接口
	virtual void ShowWndLog(const int v_iWndLogType, const char v_szWndLog[]);

public: // 重载ICB_Dfs_Client_Impl接口
	static int CALLBACK CallBack_CB_Upload( DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[] );
	static int CALLBACK CallBack_CB_Download_Mobile( DWORD _dwSN, BYTE _byFileType, DWORD   _dwFileLen, char    *_pFileBuff );
	static int CALLBACK CallBack_CB_Download_Web(DWORD _dwSN, BYTE    _byWebUrlLen, char    _szWebUrl[MAX_PATH] );

	int CB_Upload( DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[] );
	int CB_Download_Mobile( DWORD _dwSN, BYTE _byFileType, DWORD   _dwFileLen, char    *_pFileBuff );
	int CB_Download_Web( DWORD _dwSN, BYTE    _byWebUrlLen, char    _szWebUrl[MAX_PATH] );

public:
	int ToUpload(DWORD _dwSN,BYTE _byFileType,DWORD _dwFileLen,char *_pFileBuff);
	int ToDownload( DWORD _dwSN, BYTE _byFileNameLen, char _szFileName[], const BYTE v_byDownloadType, const BYTE v_byFileProp ); 
	DWORD GetSN();
	void BindSnToConnId( DWORD v_dwSN, DWORD v_dwConnId );

private:
	bool m_bCanAccept;
	unsigned short m_usSvrPort;
	CHttpClient* m_aryHttpClient[MAX_CONN_SIZE];
	CRITICAL_SECTION m_csClients;

private:
	DWORD	m_dwSN;
	std::map<DWORD,DWORD> m_mapSNToConnId;
	CRITICAL_SECTION m_csMapSNToConnId;

	//std::map<std::string, IDealUrl*> m_mapDealUrl;

public:
	//bool DealUrl( CRequest& _Request );
};
