//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#if !defined(AFX_DIALOGDISPLAYVALENCES_INCLUDED)
#define AFX_DIALOGDETAILSVALENCES_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDetailsValences.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsValences dialog

class CDialogDetailsValences : public CDialog
{
// Construction
public:
	void SetParams(CDataValences VA);
	void GetParams(CDataValences &VA);
	CDialogDetailsValences(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsValences)
	enum { IDD = IDD_DIALOG_Details_Valences };
	BOOL	m_CollinearAsSingle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsValences)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsValences)
	afx_msg void OnRestoreDefaultDetailsValences();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DDIALOGDETAILSVALENCES_INCLUDED)
