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

#if !defined(AFX_DIALOGDETAILSMAINAXES_INCLUDED)
#define AFX_DIALOGDETAILSMAINAXES_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDetailsMainAxes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsMainAxes dialog

class CDialogDetailsMainAxes : public CDialog
{
// Construction
public:
	CDialogDetailsMainAxes(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataMainAxes MA);
	void GetParams(CDataMainAxes &MA);


// Dialog Data
	//{{AFX_DATA(CDialogDetailsMainAxes)
	enum { IDD = IDD_DIALOG_MainAxes };
	int		m_ApproachMainAngles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsMainAxes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsMainAxes)
	afx_msg void OnMainAxesRules();
	afx_msg void OnMainAxesManual();
	afx_msg void OnBUTTONDetailsMainAxesDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSMAINAXES_INCLUDED)
