// AF_WebServiceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AF_WebService.h"
#include "AF_WebServiceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAF_WebServiceDlg 对话框


CAF_WebServiceDlg::CAF_WebServiceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAF_WebServiceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAF_WebServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

static UINT status_id[]=
{  
	//-------设置5个小格格
	ID_SEPARATOR,
		ID_SEPARATOR, //可以多个
		ID_INDICATOR_CAPS, //可以没有
		ID_INDICATOR_NUM,  //
		ID_INDICATOR_SCRL  //
};

BEGIN_MESSAGE_MAP(CAF_WebServiceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAF_WebServiceDlg 消息处理程序

BOOL CAF_WebServiceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CLogType clsLogType;
	clsLogType.m_uiId = 1;
	clsLogType.m_uiType = 0;
	::lstrcpy( clsLogType.m_szName, "通用日志" );
	m_wndWndLogTabCtrl.AddLogListCtrl( clsLogType );

	clsLogType.m_uiId = 2;
	clsLogType.m_uiType = 2;
	::lstrcpy( clsLogType.m_szName, "错误" );
	m_wndWndLogTabCtrl.AddLogListCtrl( clsLogType );

	char szIp[64] = {0};
	char szFileDir[MAX_PATH] = {0};
	CIniFile  m_iniFile;
	m_iniFile.SetFileName("AF_WebService.ini");
	m_iniFile.GetProfileString("net","ip","127.0.0.1",szIp,64);
	int nPort = m_iniFile.GetProfileInt("net","port",10110);
	m_iniFile.GetProfileString("path","dir","",szFileDir,MAX_PATH);
	
	BOOL bRet = FALSE;
	bRet = m_WebServiceImpl.InitStartup((char*)this,CB_PrintLog,nPort,szFileDir);
	if( !bRet )
	{
		MessageBox("Web服务内核模块初始化失败！");
		return FALSE;
	}
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CAF_WebServiceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAF_WebServiceDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CAF_WebServiceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CAF_WebServiceDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if( !this->m_status.Create(this) )
	{
		return -1;
	}
	if( ! this->m_status.SetIndicators(status_id,sizeof(status_id)/sizeof(UINT)) )
	{
		return -1;
	}
	m_status.SetPaneInfo(0,ID_SEPARATOR,SBPS_STRETCH,100); 
	m_status.SetPaneText(0,"报警文件系统_Web服务器");
	m_status.SetPaneInfo(1,ID_SEPARATOR,SBPS_STRETCH,100);
	m_status.SetPaneText(1,"雅迅网络-Yaxon Network");

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	CRect    rectDummy( 0, 0, 0, 0 );

	if( ! m_wndWndLogTabCtrl.Create( WS_CHILD|WS_VISIBLE|TCS_BOTTOM , rectDummy, this, 2 ) )
	{
		return -1;
	}

	return 0;
}

void CAF_WebServiceDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CAF_WebServiceDlg::RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

	CRect    ClientRect; 
	CAF_WebServiceDlg::RepositionBars(0,-1,0,CWnd::reposQuery,&ClientRect,NULL,TRUE);

	CRect    rectWndLogTabCtrl = ClientRect;

	rectWndLogTabCtrl.left += 5;
	rectWndLogTabCtrl.right -= 5;
	rectWndLogTabCtrl.bottom -= 5;
	rectWndLogTabCtrl.top += 5;

	m_wndWndLogTabCtrl.MoveWindow( &rectWndLogTabCtrl );
}

void CALLBACK CAF_WebServiceDlg::CB_PrintLog( char *_pMyObj, const int _iLogType, const char _szLogDesc[], const char _szLogText[] )
{
	CAF_WebServiceDlg *pThis = (CAF_WebServiceDlg*)_pMyObj;
	pThis->PrintLog(_iLogType,_szLogDesc,_szLogText);
}

void CAF_WebServiceDlg::PrintLog(  const int _iLogType, const char _szLogDesc[], const char _szLogText[] )
{
	COleDateTime tm = COleDateTime::GetCurrentTime();
	char szDate[256];
	char szTime[256];
	sprintf_s(szDate,sizeof(szDate),"%04d-%02d-%02d",tm.GetYear(),tm.GetMonth(),tm.GetDay());
	sprintf_s(szTime,sizeof(szTime),"%02d:%02d:%02d",tm.GetHour(),tm.GetMinute(),tm.GetSecond());

	m_wndWndLogTabCtrl.OutputLog(_iLogType,0,szDate,szTime,(int)strlen(_szLogText),_szLogText);
}

void CAF_WebServiceDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	m_WebServiceImpl.UnLoad_Impl();
}
