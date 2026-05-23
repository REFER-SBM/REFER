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

#if !defined(AFX_FRCSG_INCLUDED)
#define AFX_FRCSG_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FrCSG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameModelCSG frame

class CWinRf_FrameModelCSG : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CWinRf_FrameModelCSG)
protected:
	CWinRf_FrameModelCSG();           // protected constructor used by dynamic creation

// Attributes
public:

	CToolBar    m_wndToolBar_CSG;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinRf_FrameModelCSG)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWinRf_FrameModelCSG();

	void getButtonAndIconSize(CSize& buttonSize, CSize& iconSize);

	// Generated message map functions
	//{{AFX_MSG(CWinRf_FrameModelCSG)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnViewToolbarCSG();
	afx_msg void OnUpdateViewToolbarCSG(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRCSG_INCLUDED)
