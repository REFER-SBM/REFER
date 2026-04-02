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

#if !defined(AFX_FRSHETC2D_INCLUDED)
#define AFX_FRSKETCH2D_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FrSketch2D.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameSketch2D frame

class CWinRf_FrameSketch2D : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CWinRf_FrameSketch2D)
protected:
	CWinRf_FrameSketch2D();           // protected constructor used by dynamic creation

protected:  // control bar embedded members
	void DockControlBarNextTo(CToolBar* Bar,CToolBar* Previous);

// Attributes
public:
	CToolBar   m_wndToolBar_Modes2D;
	CToolBar   m_wndToolBar_DisplaySketch2D;
	CToolBar   m_wndToolBar_DisplayCues;
	CToolBar   m_wndToolBar_DisplayInflation;
	CToolBar   m_wndToolBar_DisplayFeatures;
	CToolBar   m_wndToolBar_Edit2D;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinRf_FrameSketch2D)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWinRf_FrameSketch2D();

	// Generated message map functions
	//{{AFX_MSG(CWinRf_FrameSketch2D)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnViewToolbarModes2d();
	afx_msg void OnUpdateViewToolbarModes2d(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarDisplaySketch2d();
	afx_msg void OnUpdateViewToolbarDisplaySketch2d(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarDisplayCues();
	afx_msg void OnUpdateViewToolbarDisplayCues(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarDisplayInflation();
	afx_msg void OnUpdateViewToolbarDisplayInflation(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarDisplayFeatures();
	afx_msg void OnUpdateViewToolbarDisplayFeatures(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarEdit2D();
	afx_msg void OnUpdateViewToolbarEdit2D(CCmdUI* pCmdUI);

	afx_msg void OnViewAllToolbarsSketch2D();
	afx_msg void OnUpdateViewAllToolbarsSketch2D(CCmdUI* pCmdUI);

	afx_msg void OnHideAllToolbarsSketch2D();
	afx_msg void OnUpdateHideAllToolbarsSketch2D(CCmdUI* pCmdUI);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRSKETCH2D_INCLUDED)
