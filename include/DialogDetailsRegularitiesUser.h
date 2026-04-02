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

#if !defined(AFX_DIALOGDETAILSREGULARITIESUSER__INCLUDED_)
#define AFX_DIALOGDETAILSREGULARITIESUSER__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsRegularitiesUser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesUser dialog

class CDialogDetailsRegularitiesUser : public CDialog
{
// Construction
public:
	void SetParams(CDataInflation In);
	void GetParams(CDataInflation &In);
	CDialogDetailsRegularitiesUser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsRegularitiesUser)
	enum { IDD = IDD_DIALOG_DetailsRegularitiesUser };
	BOOL	m_DA;
	BOOL	m_FP;

	BOOL	m_PL;
	BOOL	m_CL;
	BOOL	m_VL;
	BOOL	m_OL;
	BOOL	m_I;
	BOOL	m_CC;
	BOOL	m_PEC;
	BOOL	m_OFO;
	BOOL	m_SFO;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsRegularitiesUser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsRegularitiesUser)
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSREGULARITIESUSER__INCLUDED_)
