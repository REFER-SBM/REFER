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

#if !defined(AFX_DIALOGOPTIMIZATIONPAN_INCLUDED)
#define AFX_DIALOGOPTIMIZATIONPAN_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogOptimizationPan.h : header file
//
class CReferDoc;

/////////////////////////////////////////////////////////////////////////////
// CDialogOptimizationPan dialog

class CDialogOptimizationPan : public CDialog
{
// Construction
private:
	CParamsIsolines* pParamsIsolines;
public:
	void SetParams(CParamsIsolines* pParamsIsolines);
	CDialogOptimizationPan(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogOptimizationPan)
	enum { IDD = IDD_DIALOG_OptimizationPan };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogOptimizationPan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogOptimizationPan)
	afx_msg void OnButtonDerecha();
	afx_msg void OnButtonInferior();
	afx_msg void OnButtonIzquierda();
	afx_msg void OnButtonSuperior();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DialogOptimizationPan_INCLUDED)
