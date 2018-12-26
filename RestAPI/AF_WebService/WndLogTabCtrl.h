#pragma once

#include "WndLogListCtrl.h"
#include <map>

// CWndLogTabCtrl

class CWndLogTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CWndLogTabCtrl)

public:
	CWndLogTabCtrl();
	virtual ~CWndLogTabCtrl();

public:
	void RemoveAllLogListCtrls();
	int  CreateLogListCtrls(const CLogType v_arpLogTypes[], const int v_iTypeCount);
	int  CreateLogListCtrls(const std::list<CLogType> v_reflstLogTypes);
	int  AddLogListCtrl(const CLogType& v_refLogType);

	int  OutputLog(
		unsigned long v_iLogType, unsigned long v_iActive, 
		const char v_szDate[], const char v_szTime[], 
		unsigned long v_iLogLen, const char v_szLog[]
		);

protected:
	DECLARE_MESSAGE_MAP()

private:
	void SetWndLogListCtrlSize();
	void SetWndLogListCtrlSize(CWndLogListCtrl* v_pWndLogListCtrl);

private:
	std::map<unsigned long, CWndLogListCtrl*>    m_mapWndLogListCtrls;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);

private:
	CFont m_font;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL DestroyWindow();
	afx_msg void OnDestroy();
};


