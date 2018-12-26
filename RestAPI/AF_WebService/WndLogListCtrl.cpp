// WndLogListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WndLogListCtrl.h"


// CWndLogListCtrl

IMPLEMENT_DYNAMIC(CWndLogListCtrl, CListCtrl)
CWndLogListCtrl::CWndLogListCtrl()
{
}

CWndLogListCtrl::~CWndLogListCtrl()
{
}


BEGIN_MESSAGE_MAP(CWndLogListCtrl, CListCtrl)
END_MESSAGE_MAP()



// CWndLogListCtrl message handlers

int CWndLogListCtrl::InitColumn()
{
	InsertColumn( 0, "", LVCFMT_LEFT, 20 );
	InsertColumn( 1, "日期", LVCFMT_LEFT,  70 );
	InsertColumn( 2, "时间", LVCFMT_LEFT,  60 );
	InsertColumn( 3, "事件", LVCFMT_LEFT, 570 );

	::SendMessage( 
		m_hWnd, 
		LVM_SETEXTENDEDLISTVIEWSTYLE, 
		0, 
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES 
		);

	return 0;
}

int CWndLogListCtrl::push_back( const char v_szDate[], const char v_szTime[], unsigned long v_iLogLen, const char v_szLog[])
{
	if( 1000 <= GetItemCount()  )
	{// 日志列表对话框已满, 需要删除一条日志
		DeleteItem( 999 );
	}

	InsertItem(  0, "", 2 );
	SetItemText( 0, 1, v_szDate );
	SetItemText( 0, 2, v_szTime );
	SetItemText( 0, 3, v_szLog );

	return 0;
}