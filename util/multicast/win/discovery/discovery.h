// discovery.h : main header file for the DISCOVERY application
//

#if !defined(AFX_DISCOVERY_H__472D0C09_85FE_4D87_8DEF_84F4EA5DE236__INCLUDED_)
#define AFX_DISCOVERY_H__472D0C09_85FE_4D87_8DEF_84F4EA5DE236__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDiscoveryApp:
// See discovery.cpp for the implementation of this class
//

class CDiscoveryApp : public CWinApp
{
public:
	CDiscoveryApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiscoveryApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDiscoveryApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISCOVERY_H__472D0C09_85FE_4D87_8DEF_84F4EA5DE236__INCLUDED_)
