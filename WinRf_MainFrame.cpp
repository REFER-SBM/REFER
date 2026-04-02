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

// MainFrm.cpp : implementation of the CWinRf_MainFrame class
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"

#include "DialogSettingsRefer.h"

#include "WinRf_MainFrame.h"

/////////////////////////////////////////////////////////////////////////////
// CWinRf_MainFrame

IMPLEMENT_DYNAMIC(CWinRf_MainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CWinRf_MainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CWinRf_MainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()

	ON_COMMAND(ID_REFER_SETTINGS, OnSettingsRefer)

	ON_COMMAND(ID_VIEW_MAIN_TOOLBAR, OnViewToolbarMain)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MAIN_TOOLBAR, OnUpdateViewToolbarMain)

	ON_COMMAND(ID_VIEW_RECONSTRUCTION_TOOLBAR, OnViewToolbarReconstruction)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RECONSTRUCTION_TOOLBAR, OnUpdateViewToolbarReconstruction)

	ON_COMMAND(ID_VIEW_2D_VECTORIZE_TOOLBAR, OnViewToolbarVectorize2D)
	ON_UPDATE_COMMAND_UI(ID_VIEW_2D_VECTORIZE_TOOLBAR, OnUpdateViewToolbarVectorize2D)

	ON_COMMAND(ID_VIEW_2D_REFINE_TOOLBAR, OnViewToolbarRefine2D)
	ON_UPDATE_COMMAND_UI(ID_VIEW_2D_REFINE_TOOLBAR, OnUpdateViewToolbarRefine2D)

	ON_COMMAND(ID_VIEW_DETECTION_TOOLBAR, OnViewToolbarDetection)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DETECTION_TOOLBAR, OnUpdateViewToolbarDetection)

	ON_COMMAND(ID_VIEW_INFLATION_TOOLBAR, OnViewToolbarInflation)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INFLATION_TOOLBAR, OnUpdateViewToolbarInflation)

	ON_COMMAND(ID_VIEW_3D_REFINE_TOOLBAR, OnViewToolbarRefine3D)
	ON_UPDATE_COMMAND_UI(ID_VIEW_3D_REFINE_TOOLBAR, OnUpdateViewToolbarRefine3D)

	ON_COMMAND(ID_VIEW_3D_TESSELLATE_TOOLBAR, OnViewToolbarTessellate3D)
	ON_UPDATE_COMMAND_UI(ID_VIEW_3D_TESSELLATE_TOOLBAR, OnUpdateViewToolbarTessellate3D)

	ON_COMMAND(ID_VIEW_CSG_TOOLBAR, OnViewToolbarCSG)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CSG_TOOLBAR, OnUpdateViewToolbarCSG)

	ON_COMMAND(ID_VIEW_CUES_CONFIG_TOOLBAR, OnViewToolbarConfigCues)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CUES_CONFIG_TOOLBAR, OnUpdateViewToolbarConfigCues)

	ON_COMMAND(ID_VIEW_FEATURES_CONFIG_TOOLBAR, OnViewToolbarConfigFeatures)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FEATURES_CONFIG_TOOLBAR, OnUpdateViewToolbarConfigFeatures)

	ON_COMMAND(ID_VIEW_WINDOWS_TOOLBAR, OnViewToolbarWindows)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WINDOWS_TOOLBAR, OnUpdateViewToolbarWindows)

	ON_COMMAND(ID_VIEW_ALL_TOOLBARS_SHOW, OnViewAllToolbarsMain)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALL_TOOLBARS_SHOW, OnUpdateViewAllToolbarsMain)

	ON_COMMAND(ID_VIEW_ALL_TOOLBARS_HIDE, OnHideAllToolbarsMain)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALL_TOOLBARS_HIDE, OnUpdateHideAllToolbarsMain)

	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)

	//ON_COMMAND(ID_HELP_CONTENTS, CMDIFrameWnd::OnHelpContents)
	ON_COMMAND(ID_HELP_INDEX, CMDIFrameWnd::OnHelpIndex)
	//ON_COMMAND(ID_HELP_SEARCH, CMDIFrameWnd::OnHelpSearch)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	//Modifico los espacios disponibles en la barra de herramientas
	//para poner la información que deseo
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR,
};

/////////////////////////////////////////////////////////////////////////////
// CWinRf_MainFrame construction/destruction

CWinRf_MainFrame::CWinRf_MainFrame()
{
}

CWinRf_MainFrame::~CWinRf_MainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWinRf_MainFrame message handlers

int CWinRf_MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
// Creates the toolbars linked to the mainframe
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Creates the main toolbar
	//------------------------
	if (!m_wndToolBar_Main.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Main.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create Main toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Main.SetWindowText( _T("Main") );

	//Creates main flow toolbars
	//--------------------------
	if (!m_wndToolBar_Reconstruction.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Reconstruction.LoadToolBar(IDR_TB_RECONSTRUCTION))
	{
		TRACE0("Failed to create Reconstruction toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Reconstruction.SetWindowText( _T("Reconstruction") );

	if (!m_wndToolBar_Vectorize2D.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Vectorize2D.LoadToolBar(IDR_TB_2D_VECTORIZE))
	{
		TRACE0("Failed to create 2D Vectorize toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Vectorize2D.SetWindowText( _T("2D Vectorize") );

	if (!m_wndToolBar_Refine2D.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Refine2D.LoadToolBar(IDR_TB_2D_REFINE))
	{
		TRACE0("Failed to create 2D Refine toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Refine2D.SetWindowText( _T("2D Refine") );

	if (!m_wndToolBar_Detection.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Detection.LoadToolBar(IDR_TB_DETECTION))
	{
		TRACE0("Failed to create Detection toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Detection.SetWindowText( _T("Detection") );

	if (!m_wndToolBar_Inflation.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Inflation.LoadToolBar(IDR_TB_INFLATION))
	{
		TRACE0("Failed to create Inflation toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Inflation.SetWindowText( _T("Inflation") );

	if (!m_wndToolBar_Refine3D.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Refine3D.LoadToolBar(IDR_TB_3D_REFINE))
	{
		TRACE0("Failed to create Refine 3D toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Refine3D.SetWindowText( _T("Refine 3D") );

	if (!m_wndToolBar_Tessellate3D.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Tessellate3D.LoadToolBar(IDR_TB_3D_TESSELLATE))
	{
		TRACE0("Failed to create 3D Tessellate toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Tessellate3D.SetWindowText( _T("3D Tessellate") );

	if (!m_wndToolBar_CSG.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_CSG.LoadToolBar(IDR_TB_CSG))
	{
		TRACE0("Failed to create CSG toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_CSG.SetWindowText( _T("CSG") );

	//Creates configuration toolbars
	//------------------------------
	if (!m_wndToolBar_Config_Cues.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Config_Cues.LoadToolBar(IDR_TB_CONFIG_CUES))
	{
		TRACE0("Failed to create Cues Configuration toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Config_Cues.SetWindowText( _T("Cues Configuration") );

	if (!m_wndToolBar_Config_Features.Create(this,
		WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar_Config_Features.LoadToolBar(IDR_TB_CONFIG_FEATURES))
	{
		TRACE0("Failed to create Features Configuration toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Config_Features.SetWindowText( _T("Features Configuration") );


	//Creates the windows toolbar
	//---------------------------
	if (!m_wndToolBar_Windows.Create(this,
		WS_CHILD | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||  // Not visible
		!m_wndToolBar_Windows.LoadToolBar(IDR_TB_WINDOWS))
	{
		TRACE0("Failed to create Windows toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar_Windows.SetWindowText( _T("Windows") );

	// Creates the status bar
	//-----------------------
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	//Modifies the size of the status bar
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	m_wndStatusBar.SetPaneInfo(1, 0, SBPS_POPOUT, 210);
	m_wndStatusBar.SetPaneInfo(2, 0, SBPS_POPOUT, 170);

	return 0;
}


void CWinRf_MainFrame::DockControlBarNextTo(CToolBar* Bar,CToolBar* Previous)
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


BOOL CWinRf_MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CWinRf_MainFrame message handlers


void CWinRf_MainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	//First row menu
	m_wndToolBar_Main.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Reconstruction.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Vectorize2D.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Refine2D.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Detection.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Inflation.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Refine3D.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Tessellate3D.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_CSG.EnableDocking(CBRS_ALIGN_ANY);

	m_wndToolBar_Config_Cues.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Config_Features.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar_Windows.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar_Main, AFX_IDW_DOCKBAR_TOP);
	DockControlBarNextTo(&m_wndToolBar_Reconstruction, &m_wndToolBar_Main);
	DockControlBarNextTo(&m_wndToolBar_Vectorize2D, &m_wndToolBar_Reconstruction);
	DockControlBarNextTo(&m_wndToolBar_Refine2D, &m_wndToolBar_Vectorize2D);

	DockControlBarNextTo(&m_wndToolBar_Detection, &m_wndToolBar_Refine2D);
	DockControlBarNextTo(&m_wndToolBar_Inflation, &m_wndToolBar_Detection);
	DockControlBarNextTo(&m_wndToolBar_Refine3D, &m_wndToolBar_Inflation);
	DockControlBarNextTo(&m_wndToolBar_Tessellate3D, &m_wndToolBar_Refine3D);
	DockControlBarNextTo(&m_wndToolBar_CSG, &m_wndToolBar_Tessellate3D);

	//Second row menu
	DockControlBar(&m_wndToolBar_Config_Cues, AFX_IDW_DOCKBAR_TOP);
	DockControlBarNextTo(&m_wndToolBar_Config_Features, &m_wndToolBar_Config_Cues);
	DockControlBarNextTo(&m_wndToolBar_Windows, &m_wndToolBar_Config_Features);
}


CMDIChildWnd* CWinRf_MainFrame::OnCreateWindowSketch2D(CDocument *pDoc)
{
	CMDIChildWnd* pNewFrame= (CMDIChildWnd*) (ReferApp.m_pTemplateFrSketch2D->CreateNewFrame(pDoc, NULL));

	if (pNewFrame == NULL)
		return pNewFrame;     // not created
	ASSERT_KINDOF(CMDIChildWnd, pNewFrame);

	// Calling this function causes the views in that frame window to receive their OnInitialUpdate calls
	// Also, if there was not previously an active view, the primary view of the frame window is made active
	// Finally, the frame window is made visible
	ReferApp.m_pTemplateFrSketch2D->InitialUpdateFrame(pNewFrame, pDoc);

	return pNewFrame;
}


CMDIChildWnd* CWinRf_MainFrame::OnCreateWindowModelBrep(CDocument *pDoc)
{
	CMDIChildWnd* pNewFrame	= (CMDIChildWnd*) (ReferApp.m_pTemplateFrModelBrep->CreateNewFrame(pDoc, NULL));

	if (pNewFrame == NULL)
		return pNewFrame;     // not created
	ASSERT_KINDOF(CMDIChildWnd, pNewFrame);
	ReferApp.m_pTemplateFrModelBrep->InitialUpdateFrame(pNewFrame, pDoc);
	return pNewFrame;
}


CMDIChildWnd* CWinRf_MainFrame::OnCreateWindowModelCSG(CDocument *pDoc)
{
	CMDIChildWnd* pNewFrame	= (CMDIChildWnd*) (ReferApp.m_pTemplateFrModelCSG->CreateNewFrame(pDoc, NULL));

	if (pNewFrame == NULL)
		return pNewFrame;     // not created
	ASSERT_KINDOF(CMDIChildWnd, pNewFrame);
	ReferApp.m_pTemplateFrModelCSG->InitialUpdateFrame(pNewFrame, pDoc);
	return pNewFrame;
}


CMDIChildWnd* CWinRf_MainFrame::OnCreateWindowOptimization(CDocument *pDoc)
{
	CMDIChildWnd* pNewFrame	= (CMDIChildWnd*) (ReferApp.m_pTemplateFrOptimization->CreateNewFrame(pDoc, NULL));

	if (pNewFrame == NULL)
		return pNewFrame;     // not created
	ASSERT_KINDOF(CMDIChildWnd, pNewFrame);
	ReferApp.m_pTemplateFrOptimization->InitialUpdateFrame(pNewFrame, pDoc);
	return pNewFrame;
}

void CWinRf_MainFrame::OnViewToolbarMain()
{
	BOOL bSoyVisible = ( (m_wndToolBar_Main.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Main, !bSoyVisible, false );
	m_wndToolBar_Main.Invalidate();
}

void CWinRf_MainFrame::OnUpdateViewToolbarMain(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( (m_wndToolBar_Main.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarReconstruction() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Reconstruction.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Reconstruction, !bSoyVisible, false );
	m_wndToolBar_Reconstruction.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarReconstruction(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Reconstruction.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarVectorize2D() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Vectorize2D.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Vectorize2D, !bSoyVisible, false );
	m_wndToolBar_Vectorize2D.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarVectorize2D(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Vectorize2D.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarRefine2D() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Refine2D.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Refine2D, !bSoyVisible, false );
	m_wndToolBar_Refine2D.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarRefine2D(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Refine2D.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarDetection()
{
	BOOL bSoyVisible = ( (m_wndToolBar_Detection.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Detection, !bSoyVisible, false );
	m_wndToolBar_Detection.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarDetection(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( (m_wndToolBar_Detection.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarInflation() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Inflation.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Inflation, !bSoyVisible, false );
	m_wndToolBar_Inflation.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarInflation(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Inflation.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarRefine3D() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Refine3D.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Refine3D, !bSoyVisible, false );
	m_wndToolBar_Refine3D.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarRefine3D(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Refine3D.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarTessellate3D() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Tessellate3D.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Tessellate3D, !bSoyVisible, false );
	m_wndToolBar_Tessellate3D.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarTessellate3D(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Tessellate3D.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarCSG() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_CSG.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_CSG, !bSoyVisible, false );
	m_wndToolBar_CSG.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarCSG(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_CSG.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarConfigCues() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Config_Cues.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Config_Cues, !bSoyVisible, false );
	m_wndToolBar_Config_Cues.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarConfigCues(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Config_Cues.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarConfigFeatures() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Config_Features.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Config_Features, !bSoyVisible, false );
	m_wndToolBar_Config_Features.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarConfigFeatures(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Config_Features.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewToolbarWindows() 
{
	BOOL bSoyVisible = ( (m_wndToolBar_Windows.GetStyle() & WS_VISIBLE) != 0 );
	ShowControlBar( &m_wndToolBar_Windows, !bSoyVisible, false );
	m_wndToolBar_Windows.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewToolbarWindows(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( (m_wndToolBar_Windows.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnViewAllToolbarsMain() 
{
	ShowControlBar( &m_wndToolBar_Main, true, false );
	m_wndToolBar_Main.Invalidate();
	ShowControlBar( &m_wndToolBar_Reconstruction, true, false );
	m_wndToolBar_Reconstruction.Invalidate();
	ShowControlBar( &m_wndToolBar_Vectorize2D, true, false );
	m_wndToolBar_Vectorize2D.Invalidate();
	ShowControlBar( &m_wndToolBar_Refine2D, true, false );
	m_wndToolBar_Refine2D.Invalidate();
	ShowControlBar( &m_wndToolBar_Detection, true, false );
	m_wndToolBar_Detection.Invalidate();
	ShowControlBar( &m_wndToolBar_Inflation, true, false );
	m_wndToolBar_Inflation.Invalidate();
	ShowControlBar( &m_wndToolBar_Refine3D, true, false );
	m_wndToolBar_Refine3D.Invalidate();
	ShowControlBar( &m_wndToolBar_Tessellate3D, true, false );
	m_wndToolBar_Tessellate3D.Invalidate();
	ShowControlBar( &m_wndToolBar_CSG, true, false );
	m_wndToolBar_CSG.Invalidate();
	ShowControlBar( &m_wndToolBar_Config_Cues, true, false );
	m_wndToolBar_Config_Cues.Invalidate();
	ShowControlBar( &m_wndToolBar_Config_Features, true, false );
	m_wndToolBar_Config_Features.Invalidate();
	ShowControlBar( &m_wndToolBar_Windows, true, false );
	m_wndToolBar_Windows.Invalidate();
}


void CWinRf_MainFrame::OnUpdateViewAllToolbarsMain(CCmdUI* pCmdUI) 
{
	//pCmdUI->SetCheck( (m_wndToolBar_Main.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Reconstruction.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Vectorize2D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Refine2D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Detection.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Inflation.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Refine3D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Tessellate3D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_CSG.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Config_Cues.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Config_Features.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Windows.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnHideAllToolbarsMain() 
{
	ShowControlBar( &m_wndToolBar_Main, false, false );
	m_wndToolBar_Main.Invalidate();
	ShowControlBar( &m_wndToolBar_Reconstruction, false, false );
	m_wndToolBar_Reconstruction.Invalidate();
	ShowControlBar( &m_wndToolBar_Vectorize2D, false, false );
	m_wndToolBar_Vectorize2D.Invalidate();
	ShowControlBar( &m_wndToolBar_Refine2D, false, false );
	m_wndToolBar_Refine2D.Invalidate();
	ShowControlBar( &m_wndToolBar_Detection, false, false );
	m_wndToolBar_Detection.Invalidate();
	ShowControlBar( &m_wndToolBar_Inflation, false, false );
	m_wndToolBar_Inflation.Invalidate();
	ShowControlBar( &m_wndToolBar_Refine3D, false, false );
	m_wndToolBar_Refine3D.Invalidate();
	ShowControlBar( &m_wndToolBar_Tessellate3D, false, false );
	m_wndToolBar_Tessellate3D.Invalidate();
	ShowControlBar( &m_wndToolBar_CSG, false, false );
	m_wndToolBar_CSG.Invalidate();
	ShowControlBar( &m_wndToolBar_Config_Cues, false, false );
	m_wndToolBar_Config_Cues.Invalidate();
	ShowControlBar( &m_wndToolBar_Config_Features, false, false );
	m_wndToolBar_Config_Features.Invalidate();
	ShowControlBar( &m_wndToolBar_Windows, false, false );
	m_wndToolBar_Windows.Invalidate();
}


void CWinRf_MainFrame::OnUpdateHideAllToolbarsMain(CCmdUI* pCmdUI) 
{
	//pCmdUI->SetCheck( (m_wndToolBar_Main.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Reconstruction.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Vectorize2D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Refine2D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Detection.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Inflation.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Refine3D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Tessellate3D.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_CSG.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Config_Cues.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Config_Features.GetStyle() & WS_VISIBLE) != 0 );
	//pCmdUI->SetCheck( (m_wndToolBar_Windows.GetStyle() & WS_VISIBLE) != 0 );
}


void CWinRf_MainFrame::OnSettingsRefer() 
// Opens the dialog to let the user modify the basic Refer settings
{
	CDialogSettingsRefer Dlg;
	Dlg.SetParams();
	if (Dlg.DoModal() == IDOK){
		Dlg.GetParams();
		ReferApp.SaveParamsToINI();
	}
}


////////////////////////////////HELP//////////////////////////////////////////

void CWinRf_MainFrame::WinHelp(DWORD dwData, UINT nCmd)
{
	//HWND hWnd = ::HtmlHelp(m_hWnd, AfxGetApp()->m_pszHelpFilePath, nCmd, dwData);

	////CWinApp* theApp = AfxGetApp();
	////CString helpFilePath = theApp->m_pszHelpFilePath;

	////switch(dwData){
	////	case HIDR_MAINFRAME:
	////	case HIDR_MFCWIZTYPE:
	////	case HIDD_ABOUTBOX:
	////	case HIDD_TESTDLG:
	////	//Topics that need to go into the main window.
	////	HtmlHelp(m_hWnd, helpFilePath, HH_HELP_CONTEXT, dwData);
	////	break;

	////	case WHATEVER
	////	//Topics that need to go into a different secondary window

	////	break;

	////	default:
	////	//All the rest are popups
	////	HtmlHelp(m_hWnd, helpFilePath, HH_HELP_CONTEXT, dwData);
	////	break;
	////}
}


BOOL CWinRf_MainFrame::OnHelpInfo(HELPINFO* pHelpInfo)
{
	////Get the control ID
	//DWORD cControlID = pHelpInfo->iCtrlId;
	////Only proceed if it's not a static control
	//if (cControlID != 65535){ //65535 is IDC_STATIC
	//	//Get the context ID for the control
	//	DWORD cTopic = pHelpInfo->dwContextId;
	//	//Get the hWnd for the app
	//	HWND hwndApp = m_hWnd;
	//	//Create an app object for this app
	//	CWinApp* theApp = AfxGetApp();
	//	//Get the help file name
	//	CString helpFilePath = theApp->m_pszHelpFilePath;
	//	//Call the HtmlHelp API. This bypasses all MFC code.
	//	HtmlHelp(m_hWnd, helpFilePath, HH_DISPLAY_TEXT_POPUP, cTopic);
	//}
	return TRUE;
}


//void CWinRf_MainFrame::OnHelpContents()
//{
//	HWND hWnd = HtmlHelp(0, AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_TOC, NULL);
//}


void CWinRf_MainFrame::OnHelpIndex()
{
	//HWND hWnd = HtmlHelp(0, AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_INDEX, NULL);
}


//void CWinRf_MainFrame::OnHelpSearch()
//{
//	HH_FTS_QUERY q;
//	memset(&q, 0, sizeof(HH_FTS_QUERY));
//	q.cbStruct = sizeof(HH_FTS_QUERY);
//	q.fUniCodeStrings = FALSE;
//	q.pszSearchQuery = "";
//	q.iProximity = HH_FTS_DEFAULT_PROXIMITY;
//	q.fStemmedSearch = FALSE;
//	q.fTitleOnly = FALSE;
//	q.fExecute = TRUE;
//	q.pszWindow = NULL;
//	HWND hWnd = HtmlHelp(0, AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_SEARCH, (DWORD)&q);
//}
