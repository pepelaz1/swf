#pragma once


// CPlayerWindow

class CPlayerWindow : public CWnd
{
	DECLARE_DYNAMIC(CPlayerWindow)

public:
	CPlayerWindow();
	virtual ~CPlayerWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint( );
protected:
	DECLARE_MESSAGE_MAP()
};


