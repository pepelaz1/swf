
// sampledlg.cpp : implementation file
//

#include "stdafx.h"
#include "sample.h"
#include "sampledlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 #pragma warning( disable : 4995)

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSampleDlg dialog

CSampleDlg::CSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSampleDlg::IDD, pParent)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAY_BTN, m_btnPlay);
	DDX_Control(pDX, IDC_PAUSE_BTN, m_btnPause);
	DDX_Control(pDX, IDC_STOP_BTN, m_btnStop);
}

BEGIN_MESSAGE_MAP(CSampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY_BTN, &CSampleDlg::OnBnClickedPlayBtn)
	ON_BN_CLICKED(IDC_PAUSE_BTN, &CSampleDlg::OnBnClickedPauseBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &CSampleDlg::OnBnClickedStopBtn)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CSampleDlg message handlers

BOOL CSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CoInitialize(NULL);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect clrect;
	GetClientRect(&clrect);
	m_playerWnd.Create(_T("STATIC"), "", WS_CHILD | WS_VISIBLE,
		CRect(clrect.left+10, clrect.top+10, clrect.right-10, clrect.bottom-40), this, 1234);


	InitPlayer();
	return TRUE; 
}

void CSampleDlg::InitPlayer()
{
	CString filename;
	GetDlgItemText(IDC_FILE_EDIT, filename);
	m_player.SetFile("test.avi");
	m_player.Init(m_playerWnd.m_hWnd);
}

void CSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		m_player.RepaintVW();
	}
}


HCURSOR CSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CSampleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}


void CSampleDlg::OnBnClickedPlayBtn()
{
	m_player.Play();
	m_btnPlay.EnableWindow(FALSE);
	m_btnPause.EnableWindow(TRUE);
	m_btnStop.EnableWindow(TRUE);
}


void CSampleDlg::OnBnClickedPauseBtn()
{
	m_player.Pause();
	m_btnPlay.EnableWindow(TRUE);
	m_btnPause.EnableWindow(FALSE);
	m_btnStop.EnableWindow(TRUE);
}



void CSampleDlg::OnBnClickedStopBtn()
{
	m_player.Stop();
	m_btnPlay.EnableWindow(TRUE);
	m_btnPause.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);
}


void CSampleDlg::OnBnClickedSetBtn()
{
	InitPlayer();
}

void CSampleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	MoveControls();	
}


void CSampleDlg::MoveControls()
{
	CRect r;
	GetClientRect(&r);

	if (m_playerWnd.m_hWnd)
		m_playerWnd.MoveWindow(r.left+10, r.top+10, r.right-20, r.bottom-50);
	m_player.UpdateVWPos();

	CRect r1;
	if (m_btnPlay.m_hWnd != NULL)
	{
		m_btnPlay.GetClientRect(&r1);
		r1.MoveToY(r.bottom-31);
		r1.MoveToX(11);
		m_btnPlay.MoveWindow(&r1);
	}

	if (m_btnPause.m_hWnd != NULL)
	{
		m_btnPause.GetClientRect(&r1);
		r1.MoveToY(r.bottom-31);
		r1.MoveToX(92);
		m_btnPause.MoveWindow(&r1);
	}

	if (m_btnStop.m_hWnd != NULL)
	{
		m_btnStop.GetClientRect(&r1);
		r1.MoveToY(r.bottom-31);
		r1.MoveToX(173);
		m_btnStop.MoveWindow(&r1);
	}
	RedrawWindow();
}

void CSampleDlg::Finalize()
{
	m_player.Reset();
}

void CSampleDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	m_player.UpdateVWPos();
}


void CSampleDlg::OnClose()
{
	m_player.Reset();
	CDialogEx::OnClose();
}

