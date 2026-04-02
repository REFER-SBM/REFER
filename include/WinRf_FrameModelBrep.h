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

#if !defined(AFX_FRMODELBREP_INCLUDED)
#define AFX_FRMODELBREP_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FrModelBrep.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameModelBrep frame

class CWinRf_FrameModelBrep : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CWinRf_FrameModelBrep)
protected:
	CWinRf_FrameModelBrep();           // protected constructor used by dynamic creation

// Attributes
public:
	CToolBar  m_wndToolBar_Model_BRep;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinRf_FrameModelBrep)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWinRf_FrameModelBrep();

	// Generated message map functions
	//{{AFX_MSG(CWinRf_FrameModelBrep)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewToolbarModelBrep();
	afx_msg void OnUpdateViewToolbarModelBrep(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRMODELBREP_INCLUDED)
