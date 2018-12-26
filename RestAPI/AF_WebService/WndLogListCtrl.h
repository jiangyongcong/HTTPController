#pragma once


class CLogType
{
public:
	UINT    m_uiId;
	unsigned long  m_uiType;
	char    m_szName[12];

public:
	CLogType()
	{
		m_uiId = 0;
		m_uiType = 0;
		::memset( m_szName, 0x00, sizeof(m_szName) );
	}
	~CLogType()
	{
	}
};

// CWndLogListCtrl

class CWndLogListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CWndLogListCtrl)

public:
	CWndLogListCtrl();
	virtual ~CWndLogListCtrl();

public:
	int  InitColumn();
	int  push_back( 
		const char v_szDate[], const char v_szTime[], 
		unsigned long v_iLogLen, const char v_szLog[] 
		);

protected:
	DECLARE_MESSAGE_MAP()

private:
	CLogType    m_clsLogType;
};


