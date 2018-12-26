#pragma once

class CAlarmFileMgr
{
public:
	CAlarmFileMgr(void);
	virtual ~CAlarmFileMgr(void);

	bool GetFileList(CStringArray& v_arrCStr, const char* _szMobile, const char* _szDate, BYTE _byType);
	bool GetFile();

private:
	char* m_pBuf;
};
