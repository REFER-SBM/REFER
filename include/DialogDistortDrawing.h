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

#if !defined(AFX_DIALOGDISTORTDRAWING_INCLUDED)
#define AFX_DIALOGDISTORTDRAWING_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDistortDrawing.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDistortDrawing dialog

class CDialogDistortDrawing : public CDialog
{
// Construction
public:
	void SetParams(CDataRefine2D R2D);
	void GetParams(CDataRefine2D &R2D);
	CDialogDistortDrawing(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDistortDrawing)
	enum { IDD = IDD_DIALOG_DistortDrawing };
	double	m_PDistorsion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDistortDrawing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDistortDrawing)
	afx_msg void OnRestoreDefaultDistort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDISTORTDRAWING_INCLUDED)
