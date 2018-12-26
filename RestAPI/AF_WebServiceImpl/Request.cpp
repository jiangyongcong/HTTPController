#include "StdAfx.h"
#include ".\request.h"

CRequest::CRequest(void)
{
	m_dwExecute = 0;
	m_cbBody = 0;
	m_bIsParaseArgs = false;
	m_strTmpKey = "";
}

CRequest::~CRequest(void)
{
}

void CRequest::Reset(void)
{
	m_mbBody.Clear();
	m_mapArgs.clear();
	m_dwExecute = 0;
	m_cbBody = 0;
	m_bIsParaseArgs = false;
}

void CRequest::Split( const std::string& str,std::map<std::string,std::string>& mapStr,char chSpliter1,char chSpliter2 )
{
	std::stringstream  ssTemp;
	ssTemp.str(str);
	std::string	strName,strValue;
	mapStr.clear();
	while(getline(ssTemp,strName,chSpliter1)&&getline(ssTemp,strValue,chSpliter2))
	{//:Ãû³Æ=Öµ &
		mapStr[strName] = strValue;
	}
}

std::string CRequest::GetUrl()
{
	return m_strUrl;
}

std::string CRequest::GetQueryArgs( char v_szKey[] )
{
	if( !m_bIsParaseArgs )
	{
		m_bIsParaseArgs = true;
		Split(m_strArgs,m_mapArgs,'=','&');
	}

	if( m_mapArgs.find(v_szKey) != m_mapArgs.end() )
	{
		return m_mapArgs[v_szKey];
	}

	return "";
}