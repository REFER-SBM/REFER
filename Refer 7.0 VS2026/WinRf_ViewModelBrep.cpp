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

// ViewModelBrep.cpp : implementation file
//

#include <stdafx.h>
#include <cmath>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DB_Model_Maths.h"

#include "WinRf_TrackBall.h"
#include "WinRf_MainFrame.h"

#include "CueMainAxes.h"

#include "DialogDetailsInflation.h"

#include "WinRf_OpenGLDevice.h"
#include "WinRf_OpenGLViewInflation.h"
#include "WinRf_OpenGLViewModelBrep.h"
#include "WinRf_ViewModelBrep.h"

/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelBrep

IMPLEMENT_DYNCREATE(CWinRf_ViewModelBrep, CScrollView)

BEGIN_MESSAGE_MAP(CWinRf_ViewModelBrep, CScrollView)
	//{{AFX_MSG_MAP(CWinRf_ViewModelBrep)
	//Window
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()

	ON_WM_TIMER()

	//Mouse
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()

	//Toolbar ModelBrep
	ON_COMMAND(ID_MODEL_BREP_VIEW_INFLATION, OnViewInflation)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_VIEW_INFLATION, OnUpdateViewInflation)

	ON_COMMAND(ID_MODEL_BREP_CONFIGURE_VIEW_INFLATION, OnConfigureViewInflation)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_CONFIGURE_VIEW_INFLATION, OnUpdateConfigureViewInflation)

	ON_COMMAND(ID_MODEL_BREP_EVOLUTION, OnViewEvolution)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_EVOLUTION, OnUpdateViewEvolution)

	ON_COMMAND(ID_MODEL_BREP_NECKER_REVERSAL, OnViewNeckerReversal)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_NECKER_REVERSAL, OnUpdateViewNeckerReversal)

	ON_COMMAND(ID_MODEL_BREP_MODEL_INMAINAXES, OnViewModelInMainAxes)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_MODEL_INMAINAXES, OnUpdateViewModelInMainAxes)

	ON_COMMAND(ID_MODEL_BREP_FRONT, OnViewFront)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_FRONT, OnUpdateViewFront)

	ON_COMMAND(ID_MODEL_BREP_TOP, OnViewTop)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_TOP, OnUpdateViewTop)

	ON_COMMAND(ID_MODEL_BREP_SIDE, OnViewSide)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_SIDE, OnUpdateViewSide)

	ON_COMMAND(ID_MODEL_BREP_ISOMETRIC, OnViewIsometric)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_ISOMETRIC, OnUpdateViewIsometric)

	ON_COMMAND(ID_MODEL_BREP_AXES_3D, OnViewAxes3D)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_AXES_3D, OnUpdateViewAxes3D)

	ON_COMMAND(ID_MODEL_BREP_EDGES_3D, OnViewEdges3D)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_EDGES_3D, OnUpdateViewEdges3D)

	ON_COMMAND(ID_MODEL_BREP_THICK_EDGES, OnDisplayThickEdges)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_THICK_EDGES, OnUpdateDisplayThickEdges)

	ON_COMMAND(ID_MODEL_BREP_FACES_3D, OnViewFaces3D)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_FACES_3D, OnUpdateViewFaces3D)

	ON_COMMAND(ID_MODEL_BREP_TRIANGULATED_FACES_3D, OnViewTriangulatedFaces3D)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_TRIANGULATED_FACES_3D, OnUpdateViewTriangulatedFaces3D)

	ON_COMMAND(ID_MODEL_BREP_FACE_NORMALS_3D, OnViewFaceNormals3D)
	ON_UPDATE_COMMAND_UI(ID_MODEL_BREP_FACE_NORMALS_3D, OnUpdateViewFaceNormals3D)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelBrep drawing
CWinRf_ViewModelBrep::CWinRf_ViewModelBrep()
{
	dc = NULL;

	OpenGLViewModelBrep = new CWinRf_OpenGLViewModelBrep();
	OpenGLViewInflation = new CWinRf_OpenGLViewInflation();

	mode_Model_Brep= true;
	bMainAxes = false;
}

CWinRf_ViewModelBrep::~CWinRf_ViewModelBrep()
{
	if (OpenGLViewModelBrep) delete OpenGLViewModelBrep;
	if (OpenGLViewInflation) delete OpenGLViewInflation;
}


BOOL CWinRf_ViewModelBrep::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Add styles required by OpenGL
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC); // Private-DC constant 
	
	return CScrollView::PreCreateWindow(cs);
}


void CWinRf_ViewModelBrep::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// CScrollViews must set scroll sizes
	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	//ModelBrep toolbar parameters
	bShowAxesModelBrep = true;
	bShowNeckerReversal = false;
	bMainAxes = false;
	bShowEdges3D = true;
	bThickEdges = true;
	bShowFaces3D = false;
	bShowTriangulatedFaces3D = false;
	bShowFaceNormals3D = false;

	//Inflation toolbar parameters

	//OpenGL Parameters
	OpenGLViewModelBrep->Camera.Radius=5;
	OpenGLViewModelBrep->Camera.Accuracy=5;
	OpenGLViewModelBrep->Camera.dIncrementAngleX = OpenGLViewModelBrep->Camera.dIncrementAngleY = 0.0;

	Ctrackball T;
	float q1[4]; float q2[4];  //Get isometric view as a combination of two rotations
	T.calc_quat( q1, 0.0f, 0.50f, 0.0f, 0.0f ); //From half top to front
	T.calc_quat( q2, 0.50f, 0.0f, 0.0f, 0.0f ); //From half side to front
	T.add_quats(q1, q2, OpenGLViewModelBrep->Camera.curquat);
	OpenGLViewModelBrep->VolumeDDBB.dZoom = 1.0;


	OpenGLViewInflation->Camera.Radius=5;
	OpenGLViewInflation->Camera.Accuracy=5;
	OpenGLViewInflation->Camera.dIncrementAngleX = OpenGLViewInflation->Camera.dIncrementAngleY = 0.0;

	T.calc_quat( OpenGLViewInflation->Camera.curquat, 0.95f, 0.0f, 0.0f, -0.1f );
	OpenGLViewInflation->VolumeDDBB.dZoom = 1.0;

	//Trackball parameters
	m_EstateTimer = Inactive;
	OpenGLViewModelBrep->Camera.PosMousePrev = OpenGLViewModelBrep->Camera.PosMouseCurrent = CPoint(0,0);
	OpenGLViewModelBrep->Camera.IncrementPosMouse = CSize(0,0);

	OpenGLViewInflation->Camera.PosMousePrev = OpenGLViewInflation->Camera.PosMouseCurrent = CPoint(0,0);
	OpenGLViewInflation->Camera.IncrementPosMouse = CSize(0,0);
}


int CWinRf_ViewModelBrep::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Get the handle to the ReferDoc Document
	pDoc= GetDocument();

	//Get the handle to two Device Contexts and link OpenGL with them
	dc = new CClientDC(this);
	OpenGLDeviceInflation.create(dc->m_hDC);   //To display the inflation
	OpenGLDeviceModelBrep.create(dc->m_hDC);   //To display the Brep model

	//Make the rendering contexts current, perform initialization, then deselect them
	OpenGLDeviceInflation.makeCurrent();
		OpenGLViewInflation->GLSetupRC_Inflation(dc->m_hDC);
	OpenGLDeviceInflation.makeCurrentNull();

	OpenGLDeviceModelBrep.makeCurrent();
		OpenGLViewModelBrep->GLSetupRC_ModelBrep(dc->m_hDC);
	OpenGLDeviceModelBrep.makeCurrentNull();

	return 0;
}


void CWinRf_ViewModelBrep::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
}


BOOL CWinRf_ViewModelBrep::OnEraseBkgnd(CDC* pDC) 
// Overrides to prevent default background erase
{
	// Return TRUE indicates that no further erasing is required
	// (this will avoid GDI clearing the OpenGL windows' background)
	return true ;
}


void CWinRf_ViewModelBrep::OnDestroy() 
{
	CScrollView::OnDestroy();

	// Clean up rendering context stuff
	OpenGLDeviceModelBrep.destroy();
	OpenGLDeviceInflation.destroy();
	if (dc) delete dc;

	pDoc->m_pFrameModelBrep = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelBrep drawing


void CWinRf_ViewModelBrep::DetermineModelBrepVolume(CDB_Model *DB_Aux)
{
	DB_Aux->GetBoundingBox(OpenGLViewModelBrep->VolumeDDBB.dXmin, 
							OpenGLViewModelBrep->VolumeDDBB.dYmin, 
							OpenGLViewModelBrep->VolumeDDBB.dZmin,
							OpenGLViewModelBrep->VolumeDDBB.dXmax, 
							OpenGLViewModelBrep->VolumeDDBB.dYmax, 
							OpenGLViewModelBrep->VolumeDDBB.dZmax );

	// Centre of the model
	OpenGLViewModelBrep->VolumeDDBB.dMidPointX = (OpenGLViewModelBrep->VolumeDDBB.dXmax + OpenGLViewModelBrep->VolumeDDBB.dXmin) / 2.0;
	OpenGLViewModelBrep->VolumeDDBB.dMidPointY = (OpenGLViewModelBrep->VolumeDDBB.dYmax + OpenGLViewModelBrep->VolumeDDBB.dYmin) / 2.0;
	OpenGLViewModelBrep->VolumeDDBB.dMidPointZ = (OpenGLViewModelBrep->VolumeDDBB.dZmax + OpenGLViewModelBrep->VolumeDDBB.dZmin) / 2.0;

	// Intervals (min, max) for every axis
	OpenGLViewModelBrep->VolumeDDBB.dRangeX = std::fabs(OpenGLViewModelBrep->VolumeDDBB.dXmax - OpenGLViewModelBrep->VolumeDDBB.dXmin);
	OpenGLViewModelBrep->VolumeDDBB.dRangeY = std::fabs(OpenGLViewModelBrep->VolumeDDBB.dYmax - OpenGLViewModelBrep->VolumeDDBB.dYmin);
	OpenGLViewModelBrep->VolumeDDBB.dRangeZ = std::fabs(OpenGLViewModelBrep->VolumeDDBB.dZmax - OpenGLViewModelBrep->VolumeDDBB.dZmin);

	// Calculate the coordinates of PlaneXY, which is centered around (0,0,-0.01)
	double dRange= max(OpenGLViewModelBrep->VolumeDDBB.dRangeX,
					   max(OpenGLViewModelBrep->VolumeDDBB.dRangeY,
						   OpenGLViewModelBrep->VolumeDDBB.dRangeZ / 2));

	OpenGLViewModelBrep->VolumeDDBB.dPlaneXmax = OpenGLViewModelBrep->VolumeDDBB.dPlaneYmax = (dRange/2.0) * 1.2;	 
	OpenGLViewModelBrep->VolumeDDBB.dPlaneXmin = OpenGLViewModelBrep->VolumeDDBB.dPlaneYmin = - OpenGLViewModelBrep->VolumeDDBB.dPlaneXmax;
	OpenGLViewModelBrep->VolumeDDBB.dPlaneZ = - OpenGLViewModelBrep->VolumeDDBB.dMidPointZ - dRange/200;

	OpenGLViewModelBrep->VolumeDDBB.dSizeCoordAxes = dRange / 5.0;
}


void CWinRf_ViewModelBrep::OnDrawModelBrep(CDC* pDC)
// Draws 3D model by way of OpenGL
// Distinguishes between two different 3D models: with inflation axis or with main axis
{
	// Determine model to display
	CDB_Model DB_Aux;
	if (bMainAxes){
		if(pDoc->DB_Brep_MainAxes->GetSizeEdges() == 0){
			//Update a 3D model oriented according to the main axes
			//(To further use it in subsequent visualizations)
			*pDoc->DB_Brep_MainAxes= *pDoc->DB_Brep_Model;
			CCueMainAxes MainAxes;
			MainAxes.TransformToMainAxes(pDoc->DB_Brep_MainAxes, true);
		}

		DB_Aux= *pDoc->DB_Brep_MainAxes;
	}
	else
		DB_Aux= *pDoc->DB_Brep_Model;

	// Determine visualization volume
	DetermineModelBrepVolume(&DB_Aux);

	// Limits of the visualization window (Replaces OnSize)
	CRect rect;
	GetClientRect(&rect);
	OpenGLViewModelBrep->Camera.SizeWindow.cx = rect.right;
	OpenGLViewModelBrep->Camera.SizeWindow.cy = rect.bottom;

	// Make the rendering context current
	OpenGLDeviceModelBrep.makeCurrent();

		// Update the background color
		OpenGLViewModelBrep->GLSetupBackground(ReferApp.m_BackgroundColor);

		// Update sizes to accomodate changes in Zoom and Pan
		OpenGLViewModelBrep->GLResize_ModelBrep(rect.right, rect.bottom);

		// Update scene
		OpenGLViewModelBrep->GLRenderScene(&DB_Aux,
											bShowAxesModelBrep,
											bMainAxes,
											bShowEdges3D,
											bThickEdges,
											bShowFaces3D,
											bShowTriangulatedFaces3D,
											bShowFaceNormals3D);
		
		// Swap our scene to the front
		SwapBuffers(dc->m_hDC);

	// Deselect the rendering context to allow other rendering contexts to co-exist
	OpenGLDeviceModelBrep.makeCurrentNull();
};


void CWinRf_ViewModelBrep::DetermineInflationVolume()
{
	pDoc->DB_Brep_Inflation->GetBoundingBox(OpenGLViewInflation->VolumeDDBB.dXmin,
									   OpenGLViewInflation->VolumeDDBB.dYmin,
									   OpenGLViewInflation->VolumeDDBB.dZmin,
									   OpenGLViewInflation->VolumeDDBB.dXmax,
									   OpenGLViewInflation->VolumeDDBB.dYmax,
									   OpenGLViewInflation->VolumeDDBB.dZmax);

	// Centre of the model
	OpenGLViewInflation->VolumeDDBB.dMidPointX = (OpenGLViewInflation->VolumeDDBB.dXmax + OpenGLViewInflation->VolumeDDBB.dXmin) / 2.0;
	OpenGLViewInflation->VolumeDDBB.dMidPointY = (OpenGLViewInflation->VolumeDDBB.dYmax + OpenGLViewInflation->VolumeDDBB.dYmin) / 2.0;
	OpenGLViewInflation->VolumeDDBB.dMidPointZ = (OpenGLViewInflation->VolumeDDBB.dZmax + OpenGLViewInflation->VolumeDDBB.dZmin) / 2.0;

	// Intervals (min, max) for every axis
	OpenGLViewInflation->VolumeDDBB.dRangeX = std::fabs(OpenGLViewInflation->VolumeDDBB.dXmax - OpenGLViewInflation->VolumeDDBB.dXmin);
	OpenGLViewInflation->VolumeDDBB.dRangeY = std::fabs(OpenGLViewInflation->VolumeDDBB.dYmax - OpenGLViewInflation->VolumeDDBB.dYmin);
	OpenGLViewInflation->VolumeDDBB.dRangeZ = std::fabs(OpenGLViewInflation->VolumeDDBB.dZmax - OpenGLViewInflation->VolumeDDBB.dZmin);

	// Calculate the coordinates of PlaneXY, which is centered around (0,0,-0.01)
	double dRangeFrame= max(OpenGLViewInflation->VolumeDDBB.dRangeX,
							OpenGLViewInflation->VolumeDDBB.dRangeY);

	OpenGLViewInflation->VolumeDDBB.dPlaneXmax = OpenGLViewInflation->VolumeDDBB.dPlaneYmax = (dRangeFrame/2.0) * 1.65;
	OpenGLViewInflation->VolumeDDBB.dPlaneXmin = OpenGLViewInflation->VolumeDDBB.dPlaneYmin = - OpenGLViewInflation->VolumeDDBB.dPlaneXmax;

	OpenGLViewInflation->VolumeDDBB.dSizeCoordAxes = dRangeFrame / 4.0;

	dRangeFrame= max(dRangeFrame, OpenGLViewInflation->VolumeDDBB.dRangeZ);

	OpenGLViewInflation->VolumeDDBB.dPlaneZ = - OpenGLViewInflation->VolumeDDBB.dMidPointZ - dRangeFrame/200;
}


void CWinRf_ViewModelBrep::OnDrawInflation(CDC* pDC)
// Renders an image of the document in the context device pointed by pDC 
// Uses OpenGL to display the inflation proccess in 3D
{
	// Determine visualization volume
	DetermineInflationVolume();

	// Limits of the visualization window (Replaces OnSize)
	CRect rect;
	GetClientRect( &rect );
	OpenGLViewInflation->Camera.SizeWindow.cx = rect.right;
	OpenGLViewInflation->Camera.SizeWindow.cy = rect.bottom;

	// Make the rendering context current
	OpenGLDeviceInflation.makeCurrent();

		// Update the background color
		OpenGLViewInflation->GLSetupBackground(ReferApp.m_BackgroundColor);

		// Update sizes to accomodate changes in Zoom and Pan
		OpenGLViewInflation->GLResize_ViewInflation( rect.right,
													 rect.bottom);

		//Update scene
		OpenGLViewInflation->GLRenderSceneInflation(pDoc);
		
		// Swap our scene to the front
		SwapBuffers(dc->m_hDC);

	// Deselect the rendering context to allow other rendering contexts to co-exist
	OpenGLDeviceInflation.makeCurrentNull();
}


void CWinRf_ViewModelBrep::OnDraw(CDC* pDC)
// Draws 3D model by way of OpenGL
// Distinguishes between two different 3D models:
//		Whole 3D Brep model
//		Inflation in explosion
{
	// Determine visualization volume
	if (mode_Model_Brep)
		OnDrawModelBrep(pDC);
	else
		OnDrawInflation(pDC);

};


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelBrep diagnostics

#ifdef _DEBUG
//void CWinRf_ViewModelBrep::AssertValid() const
//{
//	CScrollView::AssertValid();
//}
//
//void CWinRf_ViewModelBrep::Dump(CDumpContext& dc) const
//{
//	CScrollView::Dump(dc);
//}

CReferDoc* CWinRf_ViewModelBrep::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CReferDoc)));
	return (CReferDoc*)m_pDocument;
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelBrep message handlers


void CWinRf_ViewModelBrep::OnViewInflation()
//Switches the parameter that controls
//the visualization of the exploded inflation, instead of the model BRep
{
	mode_Model_Brep = !mode_Model_Brep;

	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewInflation(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!mode_Model_Brep);
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnConfigureViewInflation()
//Updates the number of intermediate solutions
//in the visualization of the exploded inflation
{
	CDialogDetailsInflation Dlg;
	Dlg.SetParams(ReferApp.In);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.In);
}


void CWinRf_ViewModelBrep::OnUpdateConfigureViewInflation(CCmdUI* pCmdUI)
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(!mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnViewEvolution()
//Switches the parameter that controls
//the visualization of the step-by-step evolution 
//of the inflation process in the model BRep window
{
	ReferApp.In.bShowInflationInProgress =
		!ReferApp.In.bShowInflationInProgress;
}


void CWinRf_ViewModelBrep::OnUpdateViewEvolution(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(ReferApp.In.bShowInflationInProgress);
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnViewNeckerReversal()
//Switches the Z coordinates of the model to display its Necker reversal

//Normals to faces are recalculated as well
{
	bShowNeckerReversal = !bShowNeckerReversal;

	if(bShowNeckerReversal){
		bMainAxes= false;
		bShowFaceNormals3D= false;
	}

	// Load the suitable database
	CDB_Model DB_Aux= *pDoc->DB_Brep_Model;

	//Reverse inflation
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DB_Aux.GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	if (dZmin != dZmax){   //Only makes sense if model is no-flat
		double MidPoint= (dZmax-dZmin)/2;

		//Reverse model vertices
		for (long i=0; i<DB_Aux.GetSizeVertices(); i++){
			CVertex Vertice= DB_Aux.GetVertex(i);
			if (Vertice.z > MidPoint) 
				Vertice.z= MidPoint-(Vertice.z-MidPoint);
			else 
				Vertice.z= MidPoint+(MidPoint-Vertice.z);
			DB_Aux.SetVertex(i, Vertice);
		}

		//Traslate the reversed model
		DB_Aux.GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
		for (long i=0; i<DB_Aux.GetSizeVertices(); i++){
			CVertex Vertice= DB_Aux.GetVertex(i);
			Vertice.z= Vertice.z-dZmin;
			DB_Aux.SetVertex(i, Vertice);
		}

		///////////////////////////////////////////////////////////
		//Reverse the windings of the faces
		////for(long i=0; i < DB_Aux.GetSizeFaces(); i++) {
		////	CFace face= DB_Aux.GetFace(i);
		////	face.ReverseFace();
		////	DB_Aux.SetFace(i, face);
		////}

		////if(ReferApp.T3D.NormalsToFaces){
		////	//Recalculate face normals
		////	GetFaceNormals(&DB_Aux);

		////	for(long i=0; i < DB_Aux.GetSizeFaces(); i++) {
		////		CFace currentFace= DB_Aux.GetFace(i);
		////		std::vector<CVector> Normals= currentFace.GetFaceTrianglesNormals();
		////		for(long n=0; n<(long)Normals.size(); n+=1 ) {
		////			Normals[n].x= -Normals[n].x;
		////			Normals[n].y= -Normals[n].y;
		////			Normals[n].z= -Normals[n].z;
		////		}
		////		currentFace.SetFaceTrianglesNormals(Normals);

		////		CVector Normal= currentFace.GetNormal();
		////		Normal.x= -Normal.x;
		////		Normal.y= -Normal.y;
		////		Normal.z= -Normal.z;
		////		currentFace.SetNormal(Normal);

		////		DB_Aux.SetFace(i, currentFace);
		////	}
		////}
		///////////////////////////////////////////////////////////
	}

	*pDoc->DB_Brep_Model= DB_Aux;

	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewNeckerReversal(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(bShowNeckerReversal);
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep &&
				   !bMainAxes);
}


void CWinRf_ViewModelBrep::OnViewModelInMainAxes()
//Switches the parameter that controls
//the visualization of 3D model related to the main axes of coordinates

//Coordinates of BRep model relative to the main axes are calculated as well
{
	bMainAxes = !bMainAxes;
	if(bMainAxes){
		//Update a 3D model oriented according to the main axes
		//(To further use it in subsequent visualizations)
		*pDoc->DB_Brep_MainAxes= *pDoc->DB_Brep_Model;
		CCueMainAxes MainAxes;
		MainAxes.TransformToMainAxes(pDoc->DB_Brep_MainAxes, true);
	};
	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewModelInMainAxes(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(bMainAxes);
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(mode_Model_Brep && ReferApp.Cu.MA.MainAngles.size()>0 &&
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep &&
				   !bShowNeckerReversal);
}


void CWinRf_ViewModelBrep::OnViewFront()
//Switches the parameter that controls
//the visualization of the model from the front
{
	Ctrackball T;
	if(bMainAxes){
		T.Euler_to_quat(OpenGLViewModelBrep->Camera.curquat,
						DEGREES_TO_RADIANS(90),    //Axis y (since trackball refers to inflation system)
						DEGREES_TO_RADIANS(90),    //Axis z (since trackball refers to inflation system)
						DEGREES_TO_RADIANS(0));    //Axis x (since trackball refers to inflation system)
	}
	else
		T.calc_quat(OpenGLViewModelBrep->Camera.curquat, 0.0f, 0.0f, 0.0f, 0.0f); //Null rotation: from front to front
	//OpenGLViewModelBrep->VolumeDDBB.dZoom = 1.0;
	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewFront(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnViewTop()
//Switches the parameter that controls
//the visualization of the model from above
{
	Ctrackball T;
	if(bMainAxes)
		T.Euler_to_quat(OpenGLViewModelBrep->Camera.curquat,
						DEGREES_TO_RADIANS(0),    //Axis y (since trackball refers to inflation system)
						DEGREES_TO_RADIANS(90),   //Axis z (since trackball refers to inflation system)
						DEGREES_TO_RADIANS(0));   //Axis x (since trackball refers to inflation system)
	else
		T.calc_quat(OpenGLViewModelBrep->Camera.curquat, 0.0f, 1.0f, 0.0f, 0.0f); //From top to front
	//OpenGLViewModelBrep->VolumeDDBB.dZoom = 1.0;
	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewTop(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnViewSide()
//Switches the parameter that controls
//the visualization of the model from the side
{
	Ctrackball T;
	if(bMainAxes)
		T.Euler_to_quat(OpenGLViewModelBrep->Camera.curquat,
						DEGREES_TO_RADIANS(180),    //Axis y (since trackball refers to inflation system)
						DEGREES_TO_RADIANS(0),      //Axis z (since trackball refers to inflation system)
						DEGREES_TO_RADIANS(-90));   //Axis x (since trackball refers to inflation system)
	else
		T.calc_quat(OpenGLViewModelBrep->Camera.curquat, 1.0f, 0.0f, 0.0f, 0.0f);  //From side to front
	//OpenGLViewModelBrep->VolumeDDBB.dZoom = 1.0;
	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewSide(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnViewIsometric()
//Switches the parameter that controls
//the visualization of the model from isometric view
{
	Ctrackball T;
	float q1[4]; float q2[4];
	if(bMainAxes){
		//Two steps transformation to guarantee a vertical z axis
		T.Euler_to_quat(q1,
						DEGREES_TO_RADIANS(-45),   //Green
						DEGREES_TO_RADIANS(90),    //Blue
						DEGREES_TO_RADIANS(90));   //Red
		T.Euler_to_quat(q2,
						DEGREES_TO_RADIANS(0),     //Green
						DEGREES_TO_RADIANS(45),    //Blue
						DEGREES_TO_RADIANS(0));    //Red
		T.add_quats(q1, q2, OpenGLViewModelBrep->Camera.curquat);
	}
	else{
		//Two steps transformation to guarantee a vertical y axis
		T.calc_quat(q1, 0.0f, 0.50f, 0.0f, 0.0f); //From half top to front
		T.calc_quat(q2, 0.50f, 0.0f, 0.0f, 0.0f); //From half side to front
		T.add_quats(q1, q2, OpenGLViewModelBrep->Camera.curquat);
	}

	//OpenGLViewModelBrep->VolumeDDBB.dZoom = 1.0;
	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewIsometric(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnViewAxes3D()
//Switches the parameter that controls
//the visualization of the coordinate system in the model BRep window
{
	bShowAxesModelBrep = !bShowAxesModelBrep;
	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewAxes3D(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;

	pCmdUI->SetCheck( bShowAxesModelBrep );
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnViewEdges3D()
//Switches the parameter to view edges in the Model Brep window
{
	bShowEdges3D = !bShowEdges3D;
	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewEdges3D(CCmdUI* pCmdUI)
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;

	pCmdUI->SetCheck( bShowEdges3D );
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnDisplayThickEdges()
// Switches the parameter that controls the appeareance of thick edges
// in the model Brep window
{
	bThickEdges = !bThickEdges;
	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateDisplayThickEdges(CCmdUI* pCmdUI)
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;

	pCmdUI->SetCheck( bThickEdges );
	pCmdUI->Enable(mode_Model_Brep && pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep);
}


void CWinRf_ViewModelBrep::OnViewFaces3D()
// Switches the parameter that controls the display of faces in the model Brep window
{
	bShowFaces3D = !bShowFaces3D;

	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewFaces3D(CCmdUI* pCmdUI)
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;

	pCmdUI->SetCheck( bShowFaces3D );
	pCmdUI->Enable(mode_Model_Brep && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep &&
				   pDoc->DB_Brep_Model->GetSizeFaces()!=0);
}


void CWinRf_ViewModelBrep::OnViewTriangulatedFaces3D()
// Switches the parameter that controls the display of triangulated faces in the model Brep window
{
	bShowTriangulatedFaces3D = !bShowTriangulatedFaces3D;

	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewTriangulatedFaces3D(CCmdUI* pCmdUI)
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;

	pCmdUI->SetCheck( bShowTriangulatedFaces3D );
	pCmdUI->Enable(mode_Model_Brep && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep &&
				   pDoc->DB_Brep_Model->GetSizeFaces()!=0);
}


void CWinRf_ViewModelBrep::OnViewFaceNormals3D()
// Switches the parameter that controls the display of faces in the model Brep window
// If faces must be displayed but are not yet calculated, sends a message to calculate them
{
	bShowFaceNormals3D = !bShowFaceNormals3D;

	Invalidate();
}


void CWinRf_ViewModelBrep::OnUpdateViewFaceNormals3D(CCmdUI* pCmdUI)
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;

	pCmdUI->SetCheck( bShowFaceNormals3D );
	pCmdUI->Enable(mode_Model_Brep && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelBrep &&
				   pDoc->DB_Brep_Model->GetSizeFaces()!=0 &&
				   !bShowNeckerReversal);
}


////////////////////// MOUSE ////////////////////////////


// TRACKBALL SIMULATED WITH MOUSE/PEN MOVEMENT BETWEEN LEFT BUTTON DOWN AND UP

void CWinRf_ViewModelBrep::OnLButtonDown(UINT nFlags, CPoint point) 
// Starts capture of mouse movement, to determine rotation axis and speed
{
	SetCapture();
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandClosed));

	// Activa un Timer para ver como se mueve el raton
	if (m_EstateTimer != Inactive) {
		BOOL Resultado = KillTimer( m_Timer );
		ASSERT( Resultado != 0 );
	};
	m_Timer = (UINT)SetTimer(1, 100, NULL);
	ASSERT( m_Timer != 0 );
	m_EstateTimer = CalculatingSpeed;
	m_dwMilliseconds = GetTickCount();

	m_PosMouseFromTimer = point;
	m_PosMouseInitial = point;
	m_PosMousePrev = point;

	if (mode_Model_Brep){
		OpenGLViewModelBrep->Camera.PosMousePrev = OpenGLViewModelBrep->Camera.PosMouseCurrent = point;
		OpenGLViewModelBrep->Camera.IncrementPosMouse = CSize(0,0);
	}
	else{
		OpenGLViewInflation->Camera.PosMousePrev = OpenGLViewInflation->Camera.PosMouseCurrent = point;
		OpenGLViewInflation->Camera.IncrementPosMouse = CSize(0,0);
	}

	CScrollView::OnLButtonDown(nFlags, point);
}


void CWinRf_ViewModelBrep::OnLButtonUp(UINT nFlags, CPoint point) 
// Ends capture of mouse movement, and determines rotation axis and speed
{
	if (m_EstateTimer != Inactive) {
		BOOL Resultado = KillTimer(m_Timer);
		ASSERT( Resultado != 0 );
		m_EstateTimer = Inactive;

		// Si el camino recorrido desde el ultimo Timer es 0, entonces no hay que
		// calcular la velocidad de giro automatico.

		if ( (std::fabs((float)point.x - (float)m_PosMouseFromTimer.x) > 0) ||
			  (std::fabs((float)point.y - (float)m_PosMouseFromTimer.y) > 0) ) {
			DWORD dwTiempo = GetTickCount();
			dwTiempo -= m_dwMilliseconds;
			if (dwTiempo < 1)
				dwTiempo = 1;

			if (mode_Model_Brep){
				OpenGLViewModelBrep->Camera.dIncrementAngleX = ((float)(point.y - m_PosMouseInitial.y) / (float)dwTiempo) * 50.0;
				OpenGLViewModelBrep->Camera.dIncrementAngleY = ((float)(point.x - m_PosMouseInitial.x) / (float)dwTiempo) * 50.0;
			}
			else{
				OpenGLViewInflation->Camera.dIncrementAngleX = ((float)(point.y - m_PosMouseInitial.y) / (float)dwTiempo) * 50.0;
				OpenGLViewInflation->Camera.dIncrementAngleY = ((float)(point.x - m_PosMouseInitial.x) / (float)dwTiempo) * 50.0;
			}

			m_EstateTimer = SelfRotation;
			m_Timer = (UINT)SetTimer(1, 50, NULL);
			ASSERT( m_Timer != 0 );
		}
	};

	ReleaseCapture();
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));

	CScrollView::OnLButtonUp(nFlags, point);
}


void CWinRf_ViewModelBrep::OnTimer(UINT_PTR nIDEvent) 
{
	switch (m_EstateTimer) {
	case CalculatingSpeed:
		m_PosMouseFromTimer = m_PosMousePrev;
		break;

	case SelfRotation: 
		//Mandatory to control small movements
		if (mode_Model_Brep){
			OpenGLViewModelBrep->Camera.PosMouseCurrent.x += (long)OpenGLViewModelBrep->Camera.dIncrementAngleY;
			OpenGLViewModelBrep->Camera.PosMouseCurrent.y += (long)OpenGLViewModelBrep->Camera.dIncrementAngleX;
		}
		else{
			OpenGLViewInflation->Camera.PosMouseCurrent.x += (long)OpenGLViewInflation->Camera.dIncrementAngleY;
			OpenGLViewInflation->Camera.PosMouseCurrent.y += (long)OpenGLViewInflation->Camera.dIncrementAngleX;
		}

		Invalidate();
		break;
	};

	CScrollView::OnTimer(nIDEvent);
}


//ZOOM VARIATION BETWEEN RIGHT BUTON DOWN AND UP

void CWinRf_ViewModelBrep::OnRButtonDown(UINT nFlags, CPoint point) 
// Starts captuting mouse/pen movement to determine zoom variation
{
	SetCapture();
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));

	m_PosMouseInitial = point;
	m_PosMousePrev = point;

	CScrollView::OnRButtonDown(nFlags, point);
}


void CWinRf_ViewModelBrep::OnRButtonUp(UINT nFlags, CPoint point) 
// Ends captuting mouse/pen movement and determines zoom variation
{
	ReleaseCapture();
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));

	CScrollView::OnRButtonUp(nFlags, point);
}


void CWinRf_ViewModelBrep::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( GetCapture() != this ) {
		SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));
		ShowCursor(true);
		return;
	};
	ShowCursor(true);

	if ( nFlags & MK_RBUTTON ) {
		// Update Zoom following the vertical displacement of the mouse
		long IncrementZoom = (point.y - m_PosMousePrev.y);

		if (mode_Model_Brep){
			OpenGLViewModelBrep->VolumeDDBB.dZoom += (float) IncrementZoom / 50.0;
			if ( OpenGLViewModelBrep->VolumeDDBB.dZoom < 0.2 )
				OpenGLViewModelBrep->VolumeDDBB.dZoom = 0.2;
			if ( OpenGLViewModelBrep->VolumeDDBB.dZoom > 2 )
				OpenGLViewModelBrep->VolumeDDBB.dZoom = 2;
		}
		else{
			OpenGLViewInflation->VolumeDDBB.dZoom += (float) IncrementZoom / 50.0;
			if ( OpenGLViewInflation->VolumeDDBB.dZoom < 0.2 )
				OpenGLViewInflation->VolumeDDBB.dZoom = 0.2;
			if ( OpenGLViewInflation->VolumeDDBB.dZoom > 2 )
				OpenGLViewInflation->VolumeDDBB.dZoom = 2;
		}
	}
	else {
		//Mandatory to control small movements
		if (mode_Model_Brep){
			OpenGLViewModelBrep->Camera.PosMouseCurrent = point;
		}
		else{
			OpenGLViewInflation->Camera.PosMouseCurrent = point;
		}
	};

	m_PosMousePrev = point;
	Invalidate();

	CScrollView::OnMouseMove(nFlags, point);
}


// ZOOM VARIATION BY MOUSE WHEEL ROTATION

BOOL CWinRf_ViewModelBrep::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
//Captures wheel rotation and determines zoom variation

//The zDelta value is expressed in multiples or divisions of WHEEL_DELTA, which is 120
{
	if (mode_Model_Brep){
		OpenGLViewModelBrep->VolumeDDBB.dZoom += ((double)zDelta / 120.0) / 10.0;
		if ( OpenGLViewModelBrep->VolumeDDBB.dZoom < 0.2 )  OpenGLViewModelBrep->VolumeDDBB.dZoom = 0.2;
		if ( OpenGLViewModelBrep->VolumeDDBB.dZoom > 2 )	 OpenGLViewModelBrep->VolumeDDBB.dZoom = 2;
	}
	else{
		OpenGLViewInflation->VolumeDDBB.dZoom += ((double)zDelta / 120.0) / 10.0;
		if ( OpenGLViewInflation->VolumeDDBB.dZoom < 0.2 )  OpenGLViewInflation->VolumeDDBB.dZoom = 0.2;
		if ( OpenGLViewInflation->VolumeDDBB.dZoom > 2 )    OpenGLViewInflation->VolumeDDBB.dZoom = 2;
	}

	Invalidate();

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CWinRf_ViewModelBrep::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
// This function must be overriden to prevent cursor icon from flickering
{
	return true;

	//return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

////////////////////// MOUSE ////////////////////////////
