// playerwnd.cpp : implementation file
//

#include "stdafx.h"
#include "sample.h"
#include "playerwnd.h"


// CPlayerWindow

IMPLEMENT_DYNAMIC(CPlayerWindow, CWnd)

CPlayerWindow::CPlayerWindow()
{

}

CPlayerWindow::~CPlayerWindow()
{
}


BEGIN_MESSAGE_MAP(CPlayerWindow, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()




BOOL CPlayerWindow::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}


void CPlayerWindow::OnPaint()
{
	CPaintDC dc(this);
	CBrush b(RGB(0,0,0));
	RECT r;
	GetClientRect(&r);
	dc.FillRect(&r,&b);
}
