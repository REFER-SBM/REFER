//+-----------------------------------------------------------------------------+
//| Copyright (C) 2009 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#if !defined(AFX_DIALOGCONFIGINFLATION_INCLUDED_)
#define AFX_DIALOGCONFIGINFLATION_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigInflation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigInflation dialog

class CDialogConfigInflation : public CDialog
{
// Construction
public:
	CDialogConfigInflation(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataRefer GeneralParams);
	void GetParams(CDataRefer &GeneralParams);

// Dialog Data
	//{{AFX_DATA(CDialogConfigInflation)
	enum { IDD = IDD_DIALOG_ConfigInflation };
	BOOL	m_DirectInflation;
	BOOL	m_OptimizationInflation;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigInflation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigInflation)
	virtual BOOL OnInitDialog();

	afx_msg void OnConfigDirectInflation();
	afx_msg void OnConfigOptimizationInflation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGINFLATION_INCLUDED_)
