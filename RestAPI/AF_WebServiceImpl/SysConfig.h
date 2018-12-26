#pragma once

class CSysConfig
{
public:
	CSysConfig(void);
	~CSysConfig(void);

public:
	static CSysConfig    s_SysConf;

public:
	int load();

private:
	void LoadFavicon( char v_szModuleDir[] );

public:
	unsigned short    m_ui16ListenPort;	// ±¾µØ¼àÌý¶Ë¿Ú
	std::string  m_strFileDir;
	int		  m_iMaxTmpSize;
	char*	  m_pFaviconData;
	int		  m_iFaviconDataLen;

private:
	std::map<std::string,std::string> m_mapValiCode;
};
