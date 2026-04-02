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

#if !defined(AFX_DIALOGCONFIGREFINE3D_INCLUDED_)
#define AFX_DIALOGCONFIGREFINE3D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigRefine3D.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigRefine3D dialog

class CDialogConfigRefine3D : public CDialog
{
// Construction
public:
	CDialogConfigRefine3D(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataRefine3D R3D);
	void GetParams(CDataRefine3D &R3D);

// Dialog Data
	//{{AFX_DATA(CDialogConfigRefine3D)
	enum { IDD = IDD_DIALOG_ConfigRefine3D };
	BOOL	m_Refine3D;
	BOOL	m_RefineFaces;
	BOOL	m_AlignFaces;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigRefine3D)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigRefine3D)
	virtual BOOL OnInitDialog();
	afx_msg void OnBUTTONRestoreDefault();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGREFINE3D_INCLUDED_)
