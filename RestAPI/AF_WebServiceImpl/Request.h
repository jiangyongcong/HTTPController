#pragma once

#include "../ShareSource/MemoryBuffer.hpp"
using namespace nsShare_Inc;

class CRequest
{
public:
	CRequest(void);
	~CRequest(void);

public:
	void Reset(void);
	void ParaseArgs();

public:
	std::string GetUrl();
	std::string GetQueryArgs( char v_szKey[] );

private:
	void Split( const std::string& str,std::map<std::string,std::string>& mapStr,char chSpliter1,char chSpliter2 );

public:
	std::string				m_strMethod;  // GET, HEAD or POST
	std::string				m_strUrl;	 // host's address
	std::string				m_strArgs; // string after '?'
	DWORD					m_dwExecute; // executable type
	std::string				m_strVersion; // HTTP/1.0
	CMemoryBuffer<128>		m_mbBody;
	int						m_cbBody;
	bool					m_bIsParaseArgs;

	std::string				m_strTmpKey;



	enum AppFlags {
		APP_NONE    = 0x0000,
		APP_EXECUTE = 0x0001,
		APP_CGI     = 0x0002,
		APP_ISAPI   = 0x0004 };
		AppFlags	m_AppFlags;

private:
	std::map<std::string,std::string> m_mapArgs;
};
