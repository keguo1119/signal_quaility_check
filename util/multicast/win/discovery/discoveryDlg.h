// discoveryDlg.h : header file
//

#if !defined(AFX_DISCOVERYDLG_H__C31A7D2A_7448_4411_BB75_547E82B3DCDE__INCLUDED_)
#define AFX_DISCOVERYDLG_H__C31A7D2A_7448_4411_BB75_547E82B3DCDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDiscoveryDlg dialog

class CDiscoveryDlg : public CDialog
{
// Construction
public:
	CDiscoveryDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDiscoveryDlg)
	enum { IDD = IDD_DISCOVERY_DIALOG };
	CStatic	m_devIpShow;
	CIPAddressCtrl	m_ip;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiscoveryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDiscoveryDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISCOVERYDLG_H__C31A7D2A_7448_4411_BB75_547E82B3DCDE__INCLUDED_)
