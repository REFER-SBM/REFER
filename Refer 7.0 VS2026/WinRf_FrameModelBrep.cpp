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

// FrModelBrep.cpp : implementation file
//

#include <stdafx.h>
#include <vector>
#include "Refer.h"
#include "resource.h"
#include "WinRf_FrameModelBrep.h"

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameModelBrep

IMPLEMENT_DYNCREATE(CWinRf_FrameModelBrep, CMDIChildWnd)

CWinRf_FrameModelBrep::CWinRf_FrameModelBrep()
{
}

CWinRf_FrameModelBrep::~CWinRf_FrameModelBrep()
{
}


BEGIN_MESSAGE_MAP(CWinRf_FrameModelBrep, CMDIChildWnd)
	//{{AFX_MSG_MAP(CWinRf_FrameModelBrep)
	ON_WM_CREATE()

	ON_COMMAND(ID_VIEW_BREP_MODEL_TOOLBAR, OnViewToolbarModelBrep)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BREP_MODEL_TOOLBAR, OnUpdateViewToolbarModelBrep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameModelBrep message handlers


BOOL CWinRf_FrameModelBrep::PreCreateWindow(CREATESTRUCT& cs) 
// Called during execution of CWinApp::OpenDocumentFile

// Called by:
//     CFrameWnd::GetIconWndClass
//     CMDIChildWnd::Create
//     CMDIChildWnd::OnNcCreate
{
	cs.style &= ~(LONG)FWS_ADDTOTITLE;  //  For the title bar to automatically display the name of the document represented in the window
//	cs.lpszName = "Model Brep";
	
	return CMDIChildWnd::PreCreateWindow(cs);
}


void CWinRf_FrameModelBrep::getButtonAndIconSize(CSize& buttonSize, CSize& iconSize)
//Uses the global parameter "toolbarButtonsSize" 
//to calculate the size of both buttons and icons of toolbars
{
	int scale;
	if (ReferApp.GeneralParams.toolbarButtonsSize <= 0) {
		//Scale to 1 for "low" resolution screens, and 2, 3 etc for higher resolutions
		int screenSizeX = GetSystemMetrics(SM_CXFULLSCREEN);
		scale = (int)((float)screenSizeX / 1000.);
	}
	else {
		//Scale as selected by the user in the INI file
		scale = ReferApp.GeneralParams.toolbarButtonsSize - 1;
	}

	buttonSize.SetSize((int)(24 + scale * 12), (int)(22 + scale * 11)); // Button size
	iconSize.SetSize(16, 16);   // Icon size if fixed to the size of the toolbar bmp icons
}


int CWinRf_FrameModelBrep::OnCreate(LPCREATESTRUCT lpCreateStruct) 
// Called during execution of CWinApp::OpenDocumentFile
// Called by CWnd::OnWndMsg
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);

	// Create Toolbar
	if (!m_wndToolBar_Model_BRep.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Model_BRep.LoadToolBar(IDR_TB_DISPLAY_BREP_MODEL))
	{
		TRACE0("Failed to create Display BRep Model toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Model_BRep.SetWindowText( _T("Display BRep Model") );
	m_wndToolBar_Model_BRep.SetSizes(buttonSize, iconSize);

	m_wndToolBar_Model_BRep.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar_Model_BRep, AFX_IDW_DOCKBAR_LEFT);

	return 0;
}


void CWinRf_FrameModelBrep::OnViewToolbarModelBrep() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Model_BRep.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Model_BRep, !bSoyVisible, false );
	m_wndToolBar_Model_BRep.Invalidate();

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);
	m_wndToolBar_Model_BRep.SetSizes(buttonSize, iconSize);
}


void CWinRf_FrameModelBrep::OnUpdateViewToolbarModelBrep(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndToolBar_Model_BRep.GetStyle() & WS_VISIBLE) != 0 );
}
