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

#if !defined(AFX_DIALOGDETAILSPREINFLATEAUTOMATIC_H_INCLUDED_)
#define AFX_DIALOGDETAILSPREINFLATEAUTOMATIC_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsPREINFLATEAutomatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPREINFLATEAutomatic dialog

class CDialogDetailsPREINFLATEAutomatic : public CDialog
{
// Construction
public:
	void SetParams(CDataDirectInflation DI);
	void GetParams(CDataDirectInflation &DI);
	CDialogDetailsPREINFLATEAutomatic(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsPREINFLATEAutomatic)
	enum { IDD = IDD_DIALOG_DetailsPreInflateAutomatic };
	int		m_PreInfationIfNormalon;
	int		m_PreInfationIfQuasi;
	int		m_PreInfationIfUndefinedD;
	int		m_PreInfationIfUndefinedND;
	int		m_PreInfationIfPrism;
	int		m_PreInfationIfPyramid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsPREINFLATEAutomatic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsPREINFLATEAutomatic)
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSPREINFLATEAUTOMATIC_H_INCLUDED_)
