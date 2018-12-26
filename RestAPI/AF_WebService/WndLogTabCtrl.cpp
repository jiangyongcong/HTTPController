// WndLogTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WndLogTabCtrl.h"
#include ".\wndlogtabctrl.h"


// CWndLogTabCtrl

IMPLEMENT_DYNAMIC(CWndLogTabCtrl, CTabCtrl)
CWndLogTabCtrl::CWndLogTabCtrl()
{
}

CWndLogTabCtrl::~CWndLogTabCtrl()
{
}

void CWndLogTabCtrl::RemoveAllLogListCtrls()
{
	this->DeleteAllItems();

	CWndLogListCtrl*    pLogListCtrl = NULL;

	std::map<unsigned long, CWndLogListCtrl*>::iterator    iter;

	for( iter = m_mapWndLogListCtrls.begin(); iter != m_mapWndLogListCtrls.end(); ++ iter )
	{
		pLogListCtrl = iter->second;

		pLogListCtrl->DestroyWindow();

		delete pLogListCtrl;

		pLogListCtrl = NULL;
	}

	m_mapWndLogListCtrls.clear();
}

int CWndLogTabCtrl::CreateLogListCtrls(const CLogType v_arpLogTypes[], const int v_iTypeCount)
{
	const CLogType* pLogType = v_arpLogTypes;

	for( int i = 0; i < v_iTypeCount; ++ i )
	{
		if( -1 == this->AddLogListCtrl( *pLogType ) )
		{
			this->RemoveAllLogListCtrls();

			return -1;
		}

		pLogType ++;
	}

	return 0;
}

int CWndLogTabCtrl::CreateLogListCtrls(const std::list<CLogType> v_reflstLogTypes)
{
	std::list<CLogType>::const_iterator    iter;

	for( iter = v_reflstLogTypes.begin(); iter != v_reflstLogTypes.end(); ++ iter )
	{
		if( -1 == this->AddLogListCtrl( *iter ) )
		{
			this->RemoveAllLogListCtrls();

			return -1;
		}
	}

	return 0;
}

int CWndLogTabCtrl::AddLogListCtrl(const CLogType& v_refLogType)
{
	std::map<unsigned long, CWndLogListCtrl*>::iterator    iter;

	iter = m_mapWndLogListCtrls.find( v_refLogType.m_uiType );
	if( m_mapWndLogListCtrls.end() != iter )
	{
		ASSERT( "try to create the exist log listctrl..." );
		return -1;
	}

	CWndLogListCtrl* pLogListCtrl = NULL;
	try
	{
		pLogListCtrl = new CWndLogListCtrl;
		if( NULL == pLogListCtrl )
		{
			return -1;
		}
	}catch(...)
	{
		return -1;
	}

	const DWORD dwViewStyle =	
		LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	if( ! pLogListCtrl->Create( 
		dwViewStyle | LVS_REPORT, rectDummy, 
		this, v_refLogType.m_uiId
		) )
	{
		delete pLogListCtrl;
		pLogListCtrl = NULL;
		TRACE0("Failed to create log listctrl\n");
		return -1;      // fail to create
	}

	pLogListCtrl->InitColumn();

	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT|TCIF_PARAM;
	tcItem.pszText = (LPSTR)v_refLogType.m_szName;
	tcItem.lParam = (LPARAM)pLogListCtrl;

	this->InsertItem( this->GetItemCount(), &tcItem );
	this->SetWndLogListCtrlSize(pLogListCtrl);
	if( 1 == this->GetItemCount() )
	{
		pLogListCtrl->ShowWindow(TRUE);
		this->SetCurSel(0);
	}else
	{
		pLogListCtrl->ShowWindow(FALSE);
	}

	//int iCurSel = this->GetCurSel();
	//if( -1 != iCurSel )
	//{
	//	CWndLogListCtrl* pSelWndLogListCtrl = this->GetItemData( iCurSel );
	//	pSelWndLogListCtrl->ShowWindow(FALSE);
	//}
	m_mapWndLogListCtrls.insert( std::make_pair( v_refLogType.m_uiType, pLogListCtrl ) );

	return 0;
}

int CWndLogTabCtrl::OutputLog(unsigned long v_iLogType, unsigned long v_iActive, 
								const char v_szDate[], const char v_szTime[], 
								unsigned long v_iLogLen, const char v_szLog[])
{
	std::map<unsigned long, CWndLogListCtrl*>::iterator    iter;

	iter = m_mapWndLogListCtrls.find( v_iLogType );
	if( m_mapWndLogListCtrls.end() == iter )
	{
		return -1;
	}

	CWndLogListCtrl* pLogListCtrl = iter->second;
	if( NULL == pLogListCtrl ) return -1;

	pLogListCtrl->push_back( v_szDate, v_szTime, v_iLogLen, v_szLog );
	return 0;
}

void CWndLogTabCtrl::SetWndLogListCtrlSize()
{
	CRect  rtClient;
	this->GetClientRect( &rtClient );
	CRect rtItem;
	this->GetItemRect( 0, &rtItem );
	rtClient.bottom -= ( rtItem.Height() + 10 );

	rtClient.top +=5;
	rtClient.left +=5;
	rtClient.right -=5;

	CWndLogListCtrl*    pLogListCtrl = NULL;

	std::map<unsigned long, CWndLogListCtrl*>::iterator    iter;

	for( iter = m_mapWndLogListCtrls.begin(); iter != m_mapWndLogListCtrls.end(); ++ iter )
	{
		pLogListCtrl = iter->second;

		pLogListCtrl->MoveWindow(&rtClient);

		pLogListCtrl = NULL;
	}
}

void CWndLogTabCtrl::SetWndLogListCtrlSize(CWndLogListCtrl* v_pWndLogListCtrl)
{
	CRect  rtClient;
	this->GetClientRect( &rtClient );
	CRect rtItem;
	this->GetItemRect( 0, &rtItem );
	rtClient.bottom -= ( rtItem.Height() + 10 );

	rtClient.top +=5;
	rtClient.left +=5;
	rtClient.right -=5;

	v_pWndLogListCtrl->MoveWindow(&rtClient);
}

BEGIN_MESSAGE_MAP(CWndLogTabCtrl, CTabCtrl)
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTcnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnTcnSelchanging)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CWndLogTabCtrl message handlers


void CWndLogTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	this->SetWndLogListCtrlSize();
}

void CWndLogTabCtrl::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	int iCurSel = this->GetCurSel();
	if( -1 != iCurSel )
	{
		TCITEM tcItem;
		tcItem.mask=TCIF_PARAM;
		this->GetItem(iCurSel, &tcItem );
		CWndLogListCtrl* pWndLogListCtrl = (CWndLogListCtrl *)tcItem.lParam;
		if( NULL != pWndLogListCtrl )
		{
			pWndLogListCtrl->ShowWindow(TRUE);
		}
	}

	*pResult = 0;
}

void CWndLogTabCtrl::OnTcnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int iCurSel = this->GetCurSel();
	if( -1 != iCurSel )
	{
		TCITEM tcItem;
		tcItem.mask=TCIF_PARAM;
		this->GetItem(iCurSel, &tcItem );
		CWndLogListCtrl* pWndLogListCtrl = (CWndLogListCtrl *)tcItem.lParam;
		if( NULL != pWndLogListCtrl )
		{
			pWndLogListCtrl->ShowWindow(FALSE);
		}
	}

	*pResult = 0;
}

int CWndLogTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	VERIFY(m_font.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial"));                 // lpszFacename

	this->SetFont( &m_font );

	return 0;
}

BOOL CWndLogTabCtrl::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	return CTabCtrl::DestroyWindow();
}

void CWndLogTabCtrl::OnDestroy()
{
	CTabCtrl::OnDestroy();

	// TODO: Add your message handler code here
	this->RemoveAllLogListCtrls();
}
