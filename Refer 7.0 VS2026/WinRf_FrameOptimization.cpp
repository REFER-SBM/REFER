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

// FrOptimization.cpp : implementation file
//

#include <stdafx.h>
#include <vector>
#include "Refer.h"
#include "resource.h"
#include "WinRf_FrameOptimization.h"

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameOptimization

IMPLEMENT_DYNCREATE(CWinRf_FrameOptimization, CMDIChildWnd)

CWinRf_FrameOptimization::CWinRf_FrameOptimization()
{
}

CWinRf_FrameOptimization::~CWinRf_FrameOptimization()
{
}


BEGIN_MESSAGE_MAP(CWinRf_FrameOptimization, CMDIChildWnd)
	//{{AFX_MSG_MAP(CWinRf_FrameOptimization)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_OPTIMIZATION_DISPLAY_TOOLBAR, OnViewToolbarDisplayOptimization)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIMIZATION_DISPLAY_TOOLBAR, OnUpdateViewToolbarDisplayOptimization)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameOptimization message handlers

BOOL CWinRf_FrameOptimization::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~(LONG)FWS_ADDTOTITLE;		
//	cs.lpszName = "Evolucion Optimización";

	return CMDIChildWnd::PreCreateWindow(cs);
}


void CWinRf_FrameOptimization::getButtonAndIconSize(CSize& buttonSize, CSize& iconSize)
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


int CWinRf_FrameOptimization::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);

	if (!m_wndToolBar_Optimizacion_Diagrams.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Optimizacion_Diagrams.LoadToolBar(IDR_TB_OPTIMIZATION))
	{
		TRACE0("Failed to create Optimization diagrams toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Optimizacion_Diagrams.SetWindowText( _T("Optimization diagrams") );
	m_wndToolBar_Optimizacion_Diagrams.SetSizes(buttonSize, iconSize);

	m_wndToolBar_Optimizacion_Diagrams.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar_Optimizacion_Diagrams, AFX_IDW_DOCKBAR_LEFT);

	return 0;
}

void CWinRf_FrameOptimization::OnViewToolbarDisplayOptimization() 
{
	BOOL bSoyVisible= ( (m_wndToolBar_Optimizacion_Diagrams.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Optimizacion_Diagrams, !bSoyVisible, false );
	m_wndToolBar_Optimizacion_Diagrams.Invalidate();

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);
	m_wndToolBar_Optimizacion_Diagrams.SetSizes(buttonSize, iconSize);
}

void CWinRf_FrameOptimization::OnUpdateViewToolbarDisplayOptimization(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Optimizacion_Diagrams.GetStyle() & WS_VISIBLE) != 0 );
}
