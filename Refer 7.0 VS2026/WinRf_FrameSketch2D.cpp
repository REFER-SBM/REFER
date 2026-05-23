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

// FrSketch2D.cpp : implementation file

#include <stdafx.h>
#include <vector>
#include "Refer.h"
#include "resource.h"

#include "WinRf_FrameSketch2D.h"

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameSketch2D

IMPLEMENT_DYNCREATE(CWinRf_FrameSketch2D, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CWinRf_FrameSketch2D, CMDIChildWnd)
	//{{AFX_MSG_MAP(CWinRf_FrameSketch2D)
	ON_WM_CREATE()
	ON_WM_SIZE()

	ON_WM_SYSCOMMAND()

	ON_COMMAND(ID_2D_SKETCH_VIEW_2D_MODES_TOOLBAR, OnViewToolbarModes2d)
	ON_UPDATE_COMMAND_UI(ID_2D_SKETCH_VIEW_2D_MODES_TOOLBAR, OnUpdateViewToolbarModes2d)

	ON_COMMAND(ID_2D_SKETCH_VIEW_2D_SKETCH_TOOLBAR, OnViewToolbarDisplaySketch2d)
	ON_UPDATE_COMMAND_UI(ID_2D_SKETCH_VIEW_2D_SKETCH_TOOLBAR, OnUpdateViewToolbarDisplaySketch2d)

	ON_COMMAND(ID_2D_SKETCH_VIEW_CUES_TOOLBAR, OnViewToolbarDisplayCues)
	ON_UPDATE_COMMAND_UI(ID_2D_SKETCH_VIEW_CUES_TOOLBAR, OnUpdateViewToolbarDisplayCues)

	ON_COMMAND(ID_2D_SKETCH_VIEW_INFLATION_TOOLBAR, OnViewToolbarDisplayInflation)
	ON_UPDATE_COMMAND_UI(ID_2D_SKETCH_VIEW_INFLATION_TOOLBAR, OnUpdateViewToolbarDisplayInflation)

	ON_COMMAND(ID_2D_SKETCH_VIEW_FEATURES_TOOLBAR, OnViewToolbarDisplayFeatures)
	ON_UPDATE_COMMAND_UI(ID_2D_SKETCH_VIEW_FEATURES_TOOLBAR, OnUpdateViewToolbarDisplayFeatures)

	ON_COMMAND(ID_2D_SKETCH_VIEW_2D_EDIT_TOOLBAR, OnViewToolbarEdit2D)
	ON_UPDATE_COMMAND_UI(ID_2D_SKETCH_VIEW_2D_EDIT_TOOLBAR, OnUpdateViewToolbarEdit2D)

	ON_COMMAND(ID_2D_SKETCH_SHOW_ALL_TOOLBARS, OnViewAllToolbarsSketch2D)
	ON_UPDATE_COMMAND_UI(ID_2D_SKETCH_SHOW_ALL_TOOLBARS, OnUpdateViewAllToolbarsSketch2D)

	ON_COMMAND(ID_2D_SKETCH_HIDE_ALL_TOOLBARS, OnHideAllToolbarsSketch2D)
	ON_UPDATE_COMMAND_UI(ID_2D_SKETCH_HIDE_ALL_TOOLBARS, OnUpdateHideAllToolbarsSketch2D)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameSketch2D construction/destruction

CWinRf_FrameSketch2D::CWinRf_FrameSketch2D()
{
}

CWinRf_FrameSketch2D::~CWinRf_FrameSketch2D()
// Called when MDIDestroy() is activated
{
}

/////////////////////////////////////////////////////////////////////////////
// CWinRf_FrameSketch2D message handlers

BOOL CWinRf_FrameSketch2D::PreCreateWindow(CREATESTRUCT& cs) 
// Called during execution of CWinApp::OpenDocumentFile

// Called by:
//     CFrameWnd::GetIconWndClass
//     CMDIChildWnd::Create
//     CMDIChildWnd::OnNcCreate
{
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
//	cs.lpszName = "2D Sketch";

	return CMDIChildWnd::PreCreateWindow(cs);
}


void CWinRf_FrameSketch2D::getButtonAndIconSize(CSize& buttonSize, CSize& iconSize)
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


void CWinRf_FrameSketch2D::DockControlBarNextTo(CToolBar* Bar,CToolBar* Previous)
// Places ToolBar 'Bar' next to 'Previous'
{
	CRect rect;
	DWORD dw;
	UINT n;

	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout();
	Previous->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=Previous->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a separate line.  By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar, n, &rect);
}


int CWinRf_FrameSketch2D::OnCreate(LPCREATESTRUCT lpCreateStruct) 
// Called by the framework after the frame is created
// Return 0 to continue with the frame creation; -1 to destroy the frame

// Called during execution of CWinApp::OpenDocumentFile
// Called by CWnd::OnWndMsg
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);

	// Configure and create the Toolbar Modes 2D
	if (!m_wndToolBar_Modes2D.Create(this,  //Pointer to the window that is the toolbar's parent
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Modes2D.LoadToolBar(IDR_TB_2D_MODES))
	{
		TRACE0("Failed to create 2D Modes toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Modes2D.SetWindowText( _T("2D Modes") );
	m_wndToolBar_Modes2D.SetSizes(buttonSize, iconSize);

	// Configure and create the Toolbar Display 2D Sketch
	if (!m_wndToolBar_DisplaySketch2D.Create(this,  //Pointer to the window that is the toolbar's parent
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_DisplaySketch2D.LoadToolBar(IDR_TB_DISPLAY_2D_SKETCH))
	{
		TRACE0("Failed to create Display 2D Sketch toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_DisplaySketch2D.SetWindowText( _T("Display 2D Sketch") );
	m_wndToolBar_DisplaySketch2D.SetSizes(buttonSize, iconSize);

	// Configure and create the Toolbar Display Cues
	if (!m_wndToolBar_DisplayCues.Create(this,  //Pointer to the window that is the toolbar's parent
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_DisplayCues.LoadToolBar(IDR_TB_DISPLAY_CUES))
	{
		TRACE0("Failed to create Display Cues toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_DisplayCues.SetWindowText( _T("Display Cues") );
	m_wndToolBar_DisplayCues.SetSizes(buttonSize, iconSize);

	// Configure and create the Toolbar Display Inflation
	if (!m_wndToolBar_DisplayInflation.Create(this,  //Pointer to the window that is the toolbar's parent
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_DisplayInflation.LoadToolBar(IDR_TB_DISPLAY_INFLATION))
	{
		TRACE0("Failed to create Display Inflation toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_DisplayInflation.SetWindowText( _T("Display Inflation") );
	m_wndToolBar_DisplayInflation.SetSizes(buttonSize, iconSize);

	// Configure and create the Toolbar Display Features
	if (!m_wndToolBar_DisplayFeatures.Create(this,  //Pointer to the window that is the toolbar's parent
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_DisplayFeatures.LoadToolBar(IDR_TB_DISPLAY_FEATURES))
	{
		TRACE0("Failed to create Display Features toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_DisplayFeatures.SetWindowText( _T("Display Features") );
	m_wndToolBar_DisplayFeatures.SetSizes(buttonSize, iconSize);

	if (!m_wndToolBar_Edit2D.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		//WS_CHILD | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Edit2D.LoadToolBar(IDR_TB_2D_EDITION))
	{
		TRACE0("Failed to create 2D Edit toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Edit2D.SetWindowText( _T("2D Edit") );
	m_wndToolBar_Edit2D.SetSizes(buttonSize, iconSize);

	//Make the toolbars dockable
	m_wndToolBar_DisplayFeatures.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_DisplayInflation.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_DisplayCues.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Edit2D.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_DisplaySketch2D.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Modes2D.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar_DisplaySketch2D, AFX_IDW_DOCKBAR_LEFT);
	DockControlBarNextTo(&m_wndToolBar_Modes2D, &m_wndToolBar_DisplaySketch2D);
	DockControlBar(&m_wndToolBar_DisplayCues, AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndToolBar_DisplayInflation, AFX_IDW_DOCKBAR_LEFT);
	DockControlBarNextTo(&m_wndToolBar_Edit2D, &m_wndToolBar_DisplayInflation);
	DockControlBarNextTo(&m_wndToolBar_DisplayFeatures, &m_wndToolBar_Edit2D);

	return 0;
}


void CWinRf_FrameSketch2D::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);

	////Make the toolbars dockable
	//m_wndToolBar_Modes2D.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar_DisplaySketch2D.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar_DisplayCues.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar_DisplayInflation.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar_DisplayFeatures.EnableDocking(CBRS_ALIGN_ANY);

	//EnableDocking(CBRS_ALIGN_ANY);

	//DockControlBar(&m_wndToolBar_Modes2D, AFX_IDW_DOCKBAR_LEFT);
	//DockControlBarNextTo(&m_wndToolBar_DisplaySketch2D, &m_wndToolBar_Modes2D);
	//DockControlBarNextTo(&m_wndToolBar_DisplayCues, &m_wndToolBar_DisplaySketch2D);
	//DockControlBarNextTo(&m_wndToolBar_DisplayInflation, &m_wndToolBar_DisplayCues);
	//DockControlBarNextTo(&m_wndToolBar_DisplayFeatures, &m_wndToolBar_DisplayInflation);
}

void CWinRf_FrameSketch2D::OnViewToolbarModes2d() 
{
	BOOL bSoyVisible= ( (m_wndToolBar_Modes2D.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Modes2D, !bSoyVisible, false );
	m_wndToolBar_Modes2D.Invalidate();

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);
	m_wndToolBar_Modes2D.SetSizes(buttonSize, iconSize);
}

void CWinRf_FrameSketch2D::OnUpdateViewToolbarModes2d(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndToolBar_Modes2D.GetStyle() & WS_VISIBLE) != 0 );
}

void CWinRf_FrameSketch2D::OnViewToolbarDisplaySketch2d() 
{
	BOOL bSoyVisible= ( (m_wndToolBar_DisplaySketch2D.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_DisplaySketch2D, !bSoyVisible, false );
	m_wndToolBar_DisplaySketch2D.Invalidate();

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);
	m_wndToolBar_DisplaySketch2D.SetSizes(buttonSize, iconSize);
}


void CWinRf_FrameSketch2D::OnUpdateViewToolbarDisplaySketch2d(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndToolBar_DisplaySketch2D.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_FrameSketch2D::OnViewToolbarDisplayCues() 
{
	BOOL bSoyVisible= ( (m_wndToolBar_DisplayCues.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_DisplayCues, !bSoyVisible, false );
	m_wndToolBar_DisplayCues.Invalidate();

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);
	m_wndToolBar_DisplayCues.SetSizes(buttonSize, iconSize);
}


void CWinRf_FrameSketch2D::OnUpdateViewToolbarDisplayCues(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndToolBar_DisplayCues.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_FrameSketch2D::OnViewToolbarDisplayInflation() 
{
	BOOL bSoyVisible= ( (m_wndToolBar_DisplayInflation.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_DisplayInflation, !bSoyVisible, false );
	m_wndToolBar_DisplayInflation.Invalidate();

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);
	m_wndToolBar_DisplayInflation.SetSizes(buttonSize, iconSize);
}


void CWinRf_FrameSketch2D::OnUpdateViewToolbarDisplayInflation(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndToolBar_DisplayInflation.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_FrameSketch2D::OnViewToolbarDisplayFeatures() 
{
	BOOL bSoyVisible= ( (m_wndToolBar_DisplayFeatures.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_DisplayFeatures, !bSoyVisible, false );
	m_wndToolBar_DisplayFeatures.Invalidate();

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);
	m_wndToolBar_DisplayFeatures.SetSizes(buttonSize, iconSize);
}


void CWinRf_FrameSketch2D::OnUpdateViewToolbarDisplayFeatures(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndToolBar_DisplayFeatures.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_FrameSketch2D::OnViewToolbarEdit2D() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Edit2D.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Edit2D, !bSoyVisible, false );
	m_wndToolBar_Edit2D.Invalidate();

	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);
	m_wndToolBar_Edit2D.SetSizes(buttonSize, iconSize);
}


void CWinRf_FrameSketch2D::OnUpdateViewToolbarEdit2D(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Edit2D.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_FrameSketch2D::OnViewAllToolbarsSketch2D() 
{
	CSize buttonSize, iconSize;
	getButtonAndIconSize(buttonSize, iconSize);

	ShowControlBar( &m_wndToolBar_Modes2D, true, false );
	m_wndToolBar_Modes2D.Invalidate();
	m_wndToolBar_Modes2D.SetSizes(buttonSize, iconSize);
	ShowControlBar( &m_wndToolBar_DisplaySketch2D, true, false );
	m_wndToolBar_DisplaySketch2D.Invalidate();
	m_wndToolBar_DisplaySketch2D.SetSizes(buttonSize, iconSize);
	ShowControlBar( &m_wndToolBar_DisplayCues, true, false );
	m_wndToolBar_DisplayCues.Invalidate();
	m_wndToolBar_DisplayCues.SetSizes(buttonSize, iconSize);
	ShowControlBar( &m_wndToolBar_DisplayInflation, true, false );
	m_wndToolBar_DisplayInflation.Invalidate();
	m_wndToolBar_DisplayInflation.SetSizes(buttonSize, iconSize);
	ShowControlBar( &m_wndToolBar_DisplayFeatures, true, false );
	m_wndToolBar_DisplayFeatures.Invalidate();
	m_wndToolBar_DisplayFeatures.SetSizes(buttonSize, iconSize);
	ShowControlBar( &m_wndToolBar_Edit2D, true, false );
	m_wndToolBar_Edit2D.Invalidate();
	m_wndToolBar_Edit2D.SetSizes(buttonSize, iconSize);
}


void CWinRf_FrameSketch2D::OnUpdateViewAllToolbarsSketch2D(CCmdUI* pCmdUI) 
{
	//pCmdUI->SetCheck((m_wndToolBar_Modes2D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck((m_wndToolBar_DisplaySketch2D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck((m_wndToolBar_DisplayCues.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck((m_wndToolBar_DisplayInflation.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck((m_wndToolBar_DisplayFeatures.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Edit2D.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_FrameSketch2D::OnHideAllToolbarsSketch2D() 
{
	ShowControlBar( &m_wndToolBar_Modes2D, false, false );
	m_wndToolBar_Modes2D.Invalidate();
	ShowControlBar( &m_wndToolBar_DisplaySketch2D, false, false );
	m_wndToolBar_DisplaySketch2D.Invalidate();
	ShowControlBar( &m_wndToolBar_DisplayCues, false, false );
	m_wndToolBar_DisplayCues.Invalidate();
	ShowControlBar( &m_wndToolBar_DisplayInflation, false, false );
	m_wndToolBar_DisplayInflation.Invalidate();
	ShowControlBar( &m_wndToolBar_DisplayFeatures, false, false );
	m_wndToolBar_DisplayFeatures.Invalidate();
	ShowControlBar( &m_wndToolBar_Edit2D, false, false );
	m_wndToolBar_Edit2D.Invalidate();
}


void CWinRf_FrameSketch2D::OnUpdateHideAllToolbarsSketch2D(CCmdUI* pCmdUI) 
{
	//pCmdUI->SetCheck((m_wndToolBar_Modes2D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck((m_wndToolBar_DisplaySketch2D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck((m_wndToolBar_DisplayCues.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck((m_wndToolBar_DisplayInflation.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck((m_wndToolBar_DisplayFeatures.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Edit2D.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_FrameSketch2D::OnSysCommand(UINT nID, LPARAM lParam)
// The framework calls this member function when the user selects
// a command from the Control menu, or when the user selects
// the Maximize or the Minimize button
{
	if(nID == SC_CLOSE){
		/////////////////////////////// ERROR //////////////////////////////////////
		// Closing the window here produces an error in OnCreate
		AfxMessageBox("Close the document from the file->close menu");
		return;
		/////////////////////////////// ERROR //////////////////////////////////////
		//OnFileClose();

	}

	CMDIChildWnd::OnSysCommand(nID, lParam);
}
