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

// FrCSG.cpp : implementation file
//

#include <stdafx.h>
#include "resource.h"
#include "WinRf_FrameModelCSG.h"

///////////////////////CSGInflation

IMPLEMENT_DYNCREATE(CWinRf_FrameModelCSG, CMDIChildWnd)


BEGIN_MESSAGE_MAP(CWinRf_FrameModelCSG, CMDIChildWnd)
	//{{AFX_MSG_MAP(CWinRf_FrameModelCSG)
	ON_WM_CREATE()
	ON_COMMAND(ID_CSG_TOOLBAR, OnViewToolbarCSG)
	ON_UPDATE_COMMAND_UI(ID_CSG_TOOLBAR, OnUpdateViewToolbarCSG)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameModelCSG construction/destruction
CWinRf_FrameModelCSG::CWinRf_FrameModelCSG()
{
}


CWinRf_FrameModelCSG::~CWinRf_FrameModelCSG()
{
}


/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameModelCSG message handlers

BOOL CWinRf_FrameModelCSG::PreCreateWindow(CREATESTRUCT& cs) 
// Called during execution of CWinApp::OpenDocumentFile

// Called by:
//     CFrameWnd::GetIconWndClass
//     CMDIChildWnd::Create
//     CMDIChildWnd::OnNcCreate
{
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
//	cs.lpszName = "CSG Model";

	return CMDIChildWnd::PreCreateWindow(cs);
}


int CWinRf_FrameModelCSG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
// Called during execution of CWinApp::OpenDocumentFile
// Called by CWnd::OnWndMsg
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create Toolbar
	if (!m_wndToolBar_CSG.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_CSG.LoadToolBar(IDR_TB_DISPLAY_CSG_MODEL))
	{
		TRACE0("Failed to create  Display CSG Model toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_CSG.SetWindowText( _T("Display CSG Model") );

	m_wndToolBar_CSG.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar_CSG, AFX_IDW_DOCKBAR_LEFT);

	return 0;
}


void CWinRf_FrameModelCSG::OnViewToolbarCSG() 
{
	BOOL bSoyVisible= ( (m_wndToolBar_CSG.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_CSG, !bSoyVisible, false );
	m_wndToolBar_CSG.Invalidate();
}


void CWinRf_FrameModelCSG::OnUpdateViewToolbarCSG(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_CSG.GetStyle() & WS_VISIBLE) != 0 );
}
