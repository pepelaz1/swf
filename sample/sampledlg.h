
// sampledlg.h : header file
//

#pragma once
#include "player.h"
#include "afxwin.h"
#include "afxcmn.h"

// CSampleDlg dialog
class CSampleDlg : public CDialogEx
{
private:
	CPlayer m_player;
	CPlayerWindow m_playerWnd;

	void MoveControls();
	void InitPlayer();
public:
	void Finalize();
	// Construction
public:
	CSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC,CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlayBtn();
	afx_msg void OnBnClickedPauseBtn();
	afx_msg void OnBnClickedStopBtn();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_btnPlay;
	CButton m_btnPause;
	CButton m_btnStop;


	afx_msg void OnMove(int x, int y);
	CStatic m_stcWidth;
	CStatic m_stcHeight;
	CEdit m_edtWidth;
	CEdit m_edtHeight;
	CButton m_btnSet;
	afx_msg void OnBnClickedSetBtn();
	CStatic m_stcPptx;
	CEdit m_edtFile;
	CButton m_btnBrowse;
	afx_msg void OnBnClickedBrowseBtn();
	CSpinButtonCtrl m_spnFps;
	CEdit m_edtFps;
	CStatic m_stcFps;
	CButton m_chkShowStat;
	afx_msg void OnClose();
};
