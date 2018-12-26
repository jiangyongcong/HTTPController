#include "StdAfx.h"
#include "AlarmFileMgr.h"


CAlarmFileMgr::CAlarmFileMgr(void)
{

}

CAlarmFileMgr::~CAlarmFileMgr(void)
{
}

bool CAlarmFileMgr::GetFileList(CStringArray& v_arrCStr, const char* _szMobile, const char* _szDate, BYTE _byType)
{
	std::string strDir(_szMobile);
	strDir += "\\";
	strDir.append(_szDate);
	strDir += "\\";
	char sz[4] = {0};
	sprintf_s(sz, 16, "%d\\",_byType);
	strDir.append(sz);

	// 文件类型：00——图片；01——音频；02——视频；03——记录文件；04——其它。
	// 后缀名：图片文件为jpg或png，音频文件为wav，视频文件为h264，记录文件为bin
/*
	std::string strFilter;
	switch (_byType)
	{
	case 0:
		strFilter="*.jpg";
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;		
	}*/
	return true;
}
