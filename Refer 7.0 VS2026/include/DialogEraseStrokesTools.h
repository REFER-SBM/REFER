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

#if !defined(AFX_DIALOGERASESTROKESTOOLS_INCLUDED)
#define AFX_DIALOGERASESTROKESTOOLS_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogEraseStrokesTools.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogEraseStrokesTools dialog

class CDialogEraseStrokesTools : public CDialog
{
// Construction
public:
	void SetParams(long Eraser_Radius, long Eraser_Type);
	void GetParams(long &Eraser_Radius, long &Eraser_Type);
	CDialogEraseStrokesTools(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogEraseStrokesTools)
	enum { IDD = IDD_DIALOG_EraseStrokesTools };
	long m_Eraser_Radius;
	int m_Eraser_Type;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogEraseStrokesTools)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogEraseStrokesTools)
	afx_msg void OnRestoreDefaultEraserTools();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DialogEraseStrokesTools_INCLUDED)
