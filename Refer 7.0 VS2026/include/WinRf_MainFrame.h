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

// WinRf_MainFrame.h : interface of the CWinRf_MainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_DEFINED)
#define AFX_MAINFRM_DEFINED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CWinRf_MainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CWinRf_MainFrame)
public:
	CWinRf_MainFrame();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinRf_MainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	CMDIChildWnd* OnCreateWindowSketch2D(CDocument *pDoc);
	CMDIChildWnd* OnCreateWindowModelBrep(CDocument *pDoc);
	CMDIChildWnd* OnCreateWindowModelCSG(CDocument *pDoc);
	CMDIChildWnd* OnCreateWindowOptimization(CDocument *pDoc);
	virtual ~CWinRf_MainFrame();

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	void DockControlBarNextTo(CToolBar* Bar,CToolBar* Previous);

// Generated message map functions
public:
	CToolBar  m_wndToolBar_Main,
			  m_wndToolBar_Reconstruction,
			  m_wndToolBar_Vectorize2D,
			  m_wndToolBar_Refine2D,
			  m_wndToolBar_Detection,
			  m_wndToolBar_Inflation,
			  m_wndToolBar_Refine3D,
			  m_wndToolBar_Tessellate3D,
			  m_wndToolBar_CSG,
			  m_wndToolBar_Config_Cues,
			  m_wndToolBar_Config_Features,
			  m_wndToolBar_Windows;

	void getButtonAndIconSize(CSize& buttonSize, CSize& iconSize);

	//{{AFX_MSG(CWinRf_MainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnSettingsRefer();

	afx_msg void OnViewToolbarMain();
	afx_msg void OnUpdateViewToolbarMain(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarReconstruction();
	afx_msg void OnUpdateViewToolbarReconstruction(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarVectorize2D();
	afx_msg void OnUpdateViewToolbarVectorize2D(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarRefine2D();
	afx_msg void OnUpdateViewToolbarRefine2D(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarDetection();
	afx_msg void OnUpdateViewToolbarDetection(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarInflation();
	afx_msg void OnUpdateViewToolbarInflation(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarRefine3D();
	afx_msg void OnUpdateViewToolbarRefine3D(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarTessellate3D();
	afx_msg void OnUpdateViewToolbarTessellate3D(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarCSG();
	afx_msg void OnUpdateViewToolbarCSG(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarConfigCues();
	afx_msg void OnUpdateViewToolbarConfigCues(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarConfigFeatures();
	afx_msg void OnUpdateViewToolbarConfigFeatures(CCmdUI* pCmdUI);

	afx_msg void OnViewToolbarWindows();
	afx_msg void OnUpdateViewToolbarWindows(CCmdUI* pCmdUI);

	afx_msg void OnViewAllToolbarsMain();
	afx_msg void OnUpdateViewAllToolbarsMain(CCmdUI* pCmdUI);

	afx_msg void OnHideAllToolbarsMain();
	afx_msg void OnUpdateHideAllToolbarsMain(CCmdUI* pCmdUI);

	afx_msg void WinHelp(DWORD dwData, UINT nCmd);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//afx_msg void OnHelpContents();
	afx_msg void OnHelpIndex();
	//afx_msg void OnHelpSearch();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_DEFINED)
