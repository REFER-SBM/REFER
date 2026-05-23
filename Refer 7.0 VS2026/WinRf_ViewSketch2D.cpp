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

// WinRf_ViewSketch2D.cpp : implementation of the CWinRf_ViewSketch2D class
//

#include <stdafx.h>
#include <cmath>
////#include <windows.h> //VS2015

#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DialogToolsDisplayStrokes.h"
#include "DialogSelectTree.h"

#include "DialogConfigDisplayFeatures.h"

#include "DialogEraseStrokesTools.h"
#include "DialogDistortDrawing.h"
#include "DialogRotateAngle.h"
#include "DialogScaleStroke.h"
#include "DialogFilterStroke.h"
#include "DialogPreviousNext.h"
#include "DialogLabelsFont.h"

#include "WinRf_MainFrame.h"
#include "WinRf_GDI_Transform.h"

#include "DB_Sketch.h"
#include "StrokeCornerFinder.h"  //Resampled strokes

#include "CueFaces.h"   // Imploded faces
#include "CueMainAxes.h"
#include "CueSubGraphs.h"

#include "Gestures.h"

#include "DialogProgress.h"
#include "Vectorization.h"
#include "Refine2D.h"

#include "InOutSketch.h"

#include "WinRf_FrameSketch2D.h"
#include "WinRf_ViewSketch2D.h"

//Based on:
	//Company P. and Varley P.A.C. (2009)
	//Operating modes in actual versus virtual paper-and-pencil design scenarios.
	//Intelligent User Interfaces (IUI) Workshop on Sketch Recognition, 2009


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewSketch2D

IMPLEMENT_DYNCREATE(CWinRf_ViewSketch2D, CScrollView)

BEGIN_MESSAGE_MAP(CWinRf_ViewSketch2D, CScrollView)
	//Window
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()

	//Mouse
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()

	//Keyboard
	ON_WM_KEYDOWN()

	//Toolbar Modes 2D
	ON_COMMAND(ID_MODES2D_SKETCHSTROKES, OnSketchStrokes)
	ON_UPDATE_COMMAND_UI(ID_MODES2D_SKETCHSTROKES, OnUpdateSketchStrokes)
	ON_COMMAND(ID_MODES2D_SELECTSTROKES, OnSelectStrokes)
	ON_UPDATE_COMMAND_UI(ID_MODES2D_SELECTSTROKES, OnUpdateSelectStrokes)

	ON_COMMAND(ID_MODES2D_DRAWLINES, OnDrawLines)
	ON_UPDATE_COMMAND_UI(ID_MODES2D_DRAWLINES, OnUpdateDrawLines)
	ON_COMMAND(ID_MODES2D_ERASESTROKES, OnEraseStrokes)
	ON_UPDATE_COMMAND_UI(ID_MODES2D_ERASESTROKES, OnUpdateEraseStrokes)

	ON_COMMAND(ID_MODES2D_ERASESTROKESTOOLS, OnEraseStrokesTools)
	ON_UPDATE_COMMAND_UI(ID_MODES2D_ERASESTROKESTOOLS, OnUpdateEraseStrokesTools)

	//Toolbar View Sketch2D
	ON_COMMAND(ID_VIEW2D_INPUT_SKETCH, OnDisplayInputSketch)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_INPUT_SKETCH, OnUpdateDisplayInputSketch)
	ON_COMMAND(ID_VIEW2D_WORKING_SKETCH, OnDisplayWorkingSketch)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_WORKING_SKETCH, OnUpdateDisplayWorkingSketch)
	ON_COMMAND(ID_VIEW2D_DRAWING2D_ONLINE, OnDisplayDrawing2DOnLine)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_DRAWING2D_ONLINE, OnUpdateDisplayDrawing2DOnLine)
	ON_COMMAND(ID_VIEW2D_DRAWING2D_OFFLINE, OnDisplayDrawing2DOffLine)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_DRAWING2D_OFFLINE, OnUpdateDisplayDrawing2DOffLine)
	ON_COMMAND(ID_VIEW2D_MODEL_BREP, OnDisplayModelBrep)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_MODEL_BREP, OnUpdateDisplayModelBrep)
	ON_COMMAND(ID_VIEW2D_NODES_AND_CORNERS, OnDisplayNodesAndCorners)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_NODES_AND_CORNERS, OnUpdateDisplayNodesAndCorners)
	ON_COMMAND(ID_VIEW2D_SLIDING_STRIPS, OnDisplaySlidingStrips)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_SLIDING_STRIPS, OnUpdateDisplaySlidingStrips)
	ON_COMMAND(ID_VIEW2D_TOLERANCE_STRIPS, OnDisplayToleranceBands)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_TOLERANCE_STRIPS, OnUpdateDisplayToleranceBands)
	ON_COMMAND(ID_TOOLS_DISPLAY_STROKES, OnToolsDisplayStrokes)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_DISPLAY_STROKES, OnUpdateToolsDisplayStrokes)

	ON_COMMAND(ID_VIEW2D_ZOOM_RESET, OnZoom100)

	ON_COMMAND(ID_VIEW2D_PAINT_IN_BLACK, OnDisplayInBlack)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_PAINT_IN_BLACK, OnUpdateDisplayInBlack)
	ON_COMMAND(ID_VIEW2D_NumbersOfStrokes, OnDisplayNumStrokes)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_NumbersOfStrokes, OnUpdateDisplayNumStrokes)
	ON_COMMAND(ID_VIEW2D_NumbersOfVertices, OnDisplayNumVertices)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_NumbersOfVertices, OnUpdateDisplayNumVertices)
	ON_COMMAND(ID_VIEW2D_MeritsOfVertices, OnDisplayMeritsOfVertices)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_MeritsOfVertices, OnUpdateDisplayMeritsOfVertices)
	ON_COMMAND(ID_VIEW2D_CoordinatesOfVertices, OnDisplayCoordinates)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_CoordinatesOfVertices, OnUpdateDisplayCoordinates)
	ON_COMMAND(ID_VIEW2D_NumbersOfEdges, OnDisplayNumedges)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_NumbersOfEdges, OnUpdateDisplayNumedges)

	// Not included in the toolbar (only in menu)------------
	ON_COMMAND(ID_VIEW2D_LabelsColor, OnSelectLabelsColor)
	ON_COMMAND(ID_VIEW2D_LabelsFont, OnSelectLabelsFont)
	// -----------------------------------------------------------

	//Toolbar Display Cues
	ON_COMMAND(ID_VIEW2D_DashedStrokes, OnDisplayDashedStrokes)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_DashedStrokes, OnUpdateDisplayDashedStrokes)

	ON_COMMAND(ID_VIEW2D_Valences, OnDisplayValences)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_Valences, OnUpdateDisplayValences)

	ON_COMMAND(ID_VIEW2D_ParallelEdges, OnDisplayParallelEdges)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_ParallelEdges, OnUpdateDisplayParallelEdges)
	ON_COMMAND(ID_VIEW2D_PEInSequence, OnDisplayPEInSequence)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_PEInSequence, OnUpdateDisplayPEInSequence)
	ON_COMMAND(ID_VIEW2D_NextPEInSequence, OnDisplayNextPEInSequence)
	ON_COMMAND(ID_VIEW2D_PreviousPEInSequence, OnDisplayPreviousPEInSequence)

	ON_COMMAND(ID_VIEW2D_VanishingPoints, OnDisplayVanishingPoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_VanishingPoints, OnUpdateDisplayVanishingPoints)
	ON_COMMAND(ID_VIEW2D_VPInSequence, OnDisplayVPInSequence)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_VPInSequence, OnUpdateDisplayVPInSequence)
	ON_COMMAND(ID_VIEW2D_NextVPInSequence, OnDisplayNextVPInSequence)
	ON_COMMAND(ID_VIEW2D_PreviousVPInSequence, OnDisplayPreviousVPInSequence)

	ON_COMMAND(ID_VIEW2D_SubGraphsInSequence, OnDisplaySubGraphsInSequence)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_SubGraphsInSequence, OnUpdateDisplaySubGraphsInSequence)
	ON_COMMAND(ID_VIEW2D_NextSubGraphInSequence, OnDisplayNextSubGraphInSequence)
	ON_COMMAND(ID_VIEW2D_PreviousSubGraphInSequence, OnDisplayPreviousSubGraphInSequence)

	ON_COMMAND(ID_VIEW2D_LabelsVertices, OnDisplayLabelsVertices)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_LabelsVertices, OnUpdateDisplayLabelsVertices)
	ON_COMMAND(ID_VIEW2D_LabelsEdges, OnDisplayLabelsEdges)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_LabelsEdges, OnUpdateDisplayLabelsEdges)
	ON_COMMAND(ID_VIEW2D_PerimeterInSequence, OnDisplayPerimeterInSequence)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_PerimeterInSequence, OnUpdateDisplayPerimeterInSequence)
	ON_COMMAND(ID_VIEW2D_NextPerimeterInSequence, OnDisplayNextPerimeterInSequence)
	ON_COMMAND(ID_VIEW2D_PreviousPerimeterInSequence, OnDisplayPreviousPerimeterInSequence)

	ON_COMMAND(ID_VIEW2D_MainAxes, OnDisplayMainAxes)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_MainAxes, OnUpdateDisplayMainAxes)
	ON_COMMAND(ID_VIEW2D_FacesInImplosion, OnDisplayImplosionFaces)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_FacesInImplosion, OnUpdateDisplayImplosionFaces)
	ON_COMMAND(ID_VIEW2D_FacesInSequence, OnDisplayFacesInSequence)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_FacesInSequence, OnUpdateDisplayFacesInSequence)
	ON_COMMAND(ID_VIEW2D_NextFaceInSequence, OnDisplayNextFaceInSequence)
	ON_COMMAND(ID_VIEW2D_PreviousFaceInSequence, OnDisplayPreviousFaceInSequence)
	ON_COMMAND(ID_VIEW2D_HiddenEdges, OnDisplayHiddenEdges)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_HiddenEdges, OnUpdateDisplayHiddenEdges)
	ON_COMMAND(ID_VIEW2D_LevelsOfVertices, OnDisplayLevels)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_LevelsOfVertices, OnUpdateDisplayLevels)

	ON_COMMAND(ID_VIEW2D_SymmetryPlanes, OnDisplaySymmetryPlanes)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_SymmetryPlanes, OnUpdateDisplaySymmetryPlanes)
	ON_COMMAND(ID_VIEW2D_SymmetryPlanesInSequence, OnDisplaySymmetryPlanesInSequence)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_SymmetryPlanesInSequence, OnUpdateDisplaySymmetryPlanesInSequence)
	ON_COMMAND(ID_VIEW2D_NextSymmetryPlaneInSequence, OnDisplayNextSymmetryPlaneInSequence)
	ON_COMMAND(ID_VIEW2D_PreviousSymmetryPlaneInSequence, OnDisplayPreviousSymmetryPlaneInSequence)
	ON_COMMAND(ID_VIEW2D_SymmetrySubGraphsInSequence, OnDisplaySymmetrySubGraphsInSequence)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_SymmetrySubGraphsInSequence, OnUpdateDisplaySymmetrySubGraphsInSequence)
	ON_COMMAND(ID_VIEW2D_NextSymmetrySubGraphInSequence, OnDisplayNextSymmetrySubGraphInSequence)
	ON_COMMAND(ID_VIEW2D_PreviousSymmetrySubGraphInSequence, OnDisplayPreviousSymmetrySubGraphInSequence)
	ON_COMMAND(ID_VIEW2D_SymmetryAxesInFaces, OnDisplaySymmetryAxesInFaces)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_SymmetryAxesInFaces, OnUpdateDisplaySymmetryAxesInFaces)

	ON_COMMAND(ID_VIEW2D_Normalon, OnDisplayNormalon)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_Normalon, OnUpdateDisplayNormalon)
	ON_COMMAND(ID_VIEW2D_DISPLAY_TREE, OnDisplayTree)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_DISPLAY_TREE, OnUpdateDisplayTree)
	ON_COMMAND(ID_VIEW2D_TREE_FORWARD, OnTreeForward)
	ON_COMMAND(ID_VIEW2D_TREE_BACKWARDS, OnTreeBackward)
	ON_COMMAND(ID_VIEW2D_SELECT_TREE, OnSelectTree)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_SELECT_TREE, OnUpdateSelectTree)

	//Toolbar View Features
	ON_COMMAND(ID_VIEW2D_RibsAndDividers, OnDisplayRibsAndDividers)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_RibsAndDividers, OnUpdateDisplayRibsAndDividers)
	ON_COMMAND(ID_VIEW2D_RailsAndSlots, OnDisplayRailsAndSlots)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_RailsAndSlots, OnUpdateDisplayRailsAndSlots)
	ON_COMMAND(ID_VIEW2D_StepsAndPockets, OnDisplayStepsAndPockets)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_StepsAndPockets, OnUpdateDisplayStepsAndPockets)
	ON_COMMAND(ID_VIEW2D_ChamfersAndRounds, OnDisplayChamfersAndRounds)    //TBD ChamfersAndRounds
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_ChamfersAndRounds, OnUpdateDisplayChamfersAndRounds)    //TBD ChamfersAndRounds
	ON_COMMAND(ID_VIEW2D_ConfigDisplayFeatures, OnConfigDisplayFeatures)
	ON_UPDATE_COMMAND_UI(ID_VIEW2D_ConfigDisplayFeatures, OnUpdateConfigDisplayFeatures)

	//Toolbar Edit 2D
	ON_COMMAND(ID_EDIT2D_TRANSLATESTROKE, OnTranslateStrokes)
	ON_UPDATE_COMMAND_UI(ID_EDIT2D_TRANSLATESTROKE, OnUpdateTranslateStrokes)
	ON_COMMAND(ID_EDIT2D_COPYSTROKE, OnCopyStrokes)
	ON_UPDATE_COMMAND_UI(ID_EDIT2D_COPYSTROKE, OnUpdateCopyStrokes)
	ON_COMMAND(ID_EDIT2D_ROTATESTROKE, OnRotateStrokes)
	ON_UPDATE_COMMAND_UI(ID_EDIT2D_ROTATESTROKE, OnUpdateRotateStrokes)
	ON_COMMAND(ID_EDIT2D_SCALESTROKE, OnScaleStrokes)
	ON_UPDATE_COMMAND_UI(ID_EDIT2D_SCALESTROKE, OnUpdateScaleStrokes)
	ON_COMMAND(ID_EDIT2D_FILTERSTROKE, OnFilterStrokes)
	ON_UPDATE_COMMAND_UI(ID_EDIT2D_FILTERSTROKE, OnUpdateFilterStrokes)

	ON_COMMAND(ID_EDIT2D_ROTATE2DDRAWING, OnRotate2Ddrawing)
	ON_UPDATE_COMMAND_UI(ID_EDIT2D_ROTATE2DDRAWING, OnUpdateRotate2Ddrawing)
	ON_COMMAND(ID_EDIT2D_DISTORT2DDRAWING, OnDistort2DDrawing)
	ON_UPDATE_COMMAND_UI(ID_EDIT2D_DISTORT2DDRAWING, OnUpdateDistort2DDrawing)

	//{{AFX_MSG_MAP(CWinRf_ViewSketch2D)
	//}}AFX_MSG_MAP

	//Messages to manage selection list
	ON_MESSAGE(WM_SET_SELECTION, OnSetSelection)
	ON_MESSAGE(WM_GET_SELECTION, OnGetSelection)
	ON_MESSAGE(WM_GET_PIXEL_SIZE, OnGetPixelSize)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewSketch2D construction/destruction

CWinRf_ViewSketch2D::CWinRf_ViewSketch2D()
{
	DisplayNumStrokes= false;

	DisplayNumVertices= false;
	DisplayMeritsOfVertices= false;
	DisplayCoordVertices= false;
	DisplayNumEdges= false;

	DisplayDashedStrokes= false;

	DisplaySubGraphsInSequence= false;
	NumSubGraphInSequence= 0;
	DisplayValences= false;
	DisplayVertexLabelling= false;
	DisplayLabelsEdges= false;
	DisplayPerimeterInSequence= false;
	NumPerimeterInSequence= 0;
	DisplayParallelEdges= false;
	DisplayPEInSequence= false;
	NumPEInSequence= 0;
	DisplayVanishingPoints= false;
	DisplayVPInSequence= false;
	NumVPInSequence= 0;
	DisplayMainAxes= false;
	DisplayFaces= false;
	DisplayFacesInSequence= false;
	NumFaceInSequence= 0;
	DisplayHiddenEdges= false;
	DisplayHiddenArcs= false;
	DisplayLevels= false;
	DisplaySymmetryAxesInFaces= false;
	DisplaySymmetryPlanes= false;
	DisplaySymmetryPlanesInSequence= false;
	NumSymmetryPlaneInSequence= 0;
	DisplaySymmetrySubGraphsInSequence= false;
	NumSymmetrySubGraphInSequence= 0;

	DisplayNormalon= false;

	DisplayRibsAndDividers= false;
	DisplayRailsAndSlots= false;
	DisplayStepsAndPockets= false;
	DisplayChamfersAndRounds= false;    //TBD ChamfersAndRounds

	ScaleFeatureLabels= 1.0;
	DisplayNamesOfFeatures= true;
	DisplayMeritsOfFeatures= true;
	DisplayContainerFaces= false;

	Eraser_Radius= 10; //pixels
	Eraser_Type= 0; //Erase nodes and split into separate strokes= 0, Erase nodes only= 1

	ColorStroke = RGB(0, 0, 200);
	TextLabelsColor = RGB(87,87,90);
	TextLabelsSizeScale= 1.5; //150% of standard height, which is set as 1% of the client area
}


CWinRf_ViewSketch2D::~CWinRf_ViewSketch2D()
{
}


void CWinRf_ViewSketch2D::OnDestroy()
{
	CScrollView::OnDestroy();

	pDoc->m_pFrameSketch2D = NULL;
	pDoc->m_pViewSketch2D = NULL;
}


BOOL CWinRf_ViewSketch2D::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScrollView::PreCreateWindow(cs);
}


int CWinRf_ViewSketch2D::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Get the handle to the ReferDoc Document
	pDoc= GetDocument();

	pDoc->m_pViewSketch2D = this;

	return 0;
}


void CWinRf_ViewSketch2D::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	ResetViewport();
	ResetWindow();
}


void CWinRf_ViewSketch2D::ResetViewport()
// Updates the Viewport transformation
{
	CRect rect;
	GetClientRect(&rect);

	if (rect.right== 0 || rect.bottom== 0) return;	 //no client area

	ViewportWide= rect.right - rect.left;
	ViewportHigh  = rect.bottom - rect.top;

	Transf.GDI_SetViewport(rect.left, rect.top, rect.right, rect.bottom);
}


void CWinRf_ViewSketch2D::Get2DModelBoundingBox(double &wXmin, double &wYmin,
												double &wXmax, double &wYmax)
// Recalculates the dimensions of the drawing, 
// depending on the type of the 2D model
{
	wXmin= wYmin= wXmax= wYmax= 0;

	//Case of dxf 2D line-drawing
	if(pDoc->DB_DrawingOff!= NULL && pDoc->DB_DrawingOff->GetSizeVertices() > 0){
		double wZmin= 0, wZmax= 0;
		pDoc->DB_DrawingOff->GetBoundingBox(wXmin, wYmin, wZmin, wXmax, wYmax, wZmax);
	}

	//Case of background bmp image
	else if(ReferApp.BackgroundBitMap){
		//Load the Image File
		HBITMAP hBmp = (HBITMAP)::LoadImage(NULL, ReferApp.BackgroundBitMapFile,
											IMAGE_BITMAP, 0, 0,
											LR_LOADFROMFILE|LR_CREATEDIBSECTION);

		//Create a bitmap object and attach it to the object
		CBitmap CurrentBMP;
		CurrentBMP.Attach(hBmp);

		//Get the BMP Height and Width
		BITMAP BitmapInfo;
		CurrentBMP.GetBitmap(&BitmapInfo);

		wXmin= -BitmapInfo.bmWidth/2;
		wXmax= +BitmapInfo.bmWidth/2;
		wYmin= -BitmapInfo.bmHeight/2;
		wYmax= +BitmapInfo.bmHeight/2;
	}

	//Case of stk sketch
	else if (ReferApp.SketchMode && pDoc->DB_InputSketch!= NULL && pDoc->DB_InputSketch->GetNumStrokes()>0){
		RECTANGLE2D bounds= pDoc->DB_InputSketch->GetBounds();

		wXmin= bounds.left;
		wXmax= bounds.right;
		wYmin= bounds.bottom;
		wYmax= bounds.top;
	}

	//Case of null 2D model
	if (wXmin==wXmax && wYmin==wYmax){
		CRect rect;
		GetClientRect(&rect);

		//Assign the viewport as window
		wXmin= rect.left;
		wXmax= rect.right;
		wYmin= rect.top;
		wYmax= rect.bottom;
	}
	//Case of unidimensional 2D model
	else if (wXmin==wXmax){
		wXmin-= std::fabs(wYmin-wYmax);
		wXmax+= std::fabs(wYmin-wYmax);
	}
	else if (wYmin==wYmax){
		wYmin-= std::fabs(wXmin-wXmax);
		wYmax+= std::fabs(wXmin-wXmax);
	}
}


void CWinRf_ViewSketch2D::UpdateWindow()
// Updates the pan and zoom in the current window
{
	Transf.GDI_SetWorld(-WindowLength/2 * CameraZoom - CameraPanX,
						-WindowHigh/2   * CameraZoom - CameraPanY,
						+WindowLength/2 * CameraZoom - CameraPanX,
						+WindowHigh/2   * CameraZoom - CameraPanY);
}


void CWinRf_ViewSketch2D::ResetWindow()
// Reset the window, to fit the current 2D model into the current window
{
	//Drawing size
	double wXmin= 0, wYmin= 0, wXmax= 0, wYmax= 0;
	Get2DModelBoundingBox(wXmin, wYmin, wXmax, wYmax);

	//Resize the window to fit the drawing while keeping its aspect ratio
	WindowLength= 1.5*(wXmax-wXmin);
	WindowHigh  = 1.5*(wYmax-wYmin);

	if(ViewportHigh>0 && ViewportWide>0){
		double AspectViewport= ViewportHigh/ViewportWide;
		double AspectWindow= WindowHigh/WindowLength;
		if(AspectWindow > AspectViewport){
			WindowLength= WindowLength*AspectWindow/AspectViewport;
		}
		else if(AspectViewport > AspectWindow){
			WindowHigh= WindowHigh*AspectViewport/AspectWindow;
		}
	}

	//Reset pan and zoom to fit the window in the viewport
	CameraZoom= 1;
	CameraPanX= -(wXmin+wXmax)/2;
	CameraPanY= -(wYmin+wYmax)/2;

	//Apply transformation, including pan and zoom
	UpdateWindow();
}


void CWinRf_ViewSketch2D::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	//-------------------------------------------------------
	// CScrollViews must set scroll sizes
	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
	//-------------------------------------------------------

	// Loads the sketch manager
	MouseMode=InactiveMode;

	//Sketch2D toolbar parameters
	DisplayInputSketch= false;
	if (ReferApp.SketchMode && pDoc->DB_InputSketch != NULL)
		DisplayInputSketch= true;
	DisplayWorkingSketch= false;

	DisplayOnLine= ReferApp.GeneralParams.bVectorize_Int;
	DisplayOffLine= ReferApp.GeneralParams.bVectorize_Batch;
	DisplayModelBrep= false;

	SelectedNodes= 0; // 0= original nodes, 1= resampled nodes
	Display_Corners= true;
	Display_Line_Tolerance= true;
	Display_Arc_Tolerance= true;
	Display_Convex_Hull= false;
	Display_Merits_Strokes= false;
	DisplayNodesAndCorners= false;
	DisplaySlidingStrips= false;
	DisplayToleranceBands= false;
	DisplayInBlack= false;

	DisplaySubGraphsInSequence= false;
	NumSubGraphInSequence= 0;
	DisplayPerimeterInSequence= false;
	NumPerimeterInSequence= 0;
	DisplayParallelEdges= false;
	DisplayPEInSequence= false;
	NumPEInSequence= 0;
	DisplayVanishingPoints= false;
	DisplayVPInSequence= false;
	NumVPInSequence= 0;
	DisplayMainAxes= false;
	DisplayFaces= false;
	DisplayFacesInSequence= false;
	NumFaceInSequence= 0;
	DisplayHiddenEdges= false;
	DisplayHiddenArcs= false;
	DisplaySymmetryPlanesInSequence= false;
	NumSymmetryPlaneInSequence= 0;
	DisplaySymmetrySubGraphsInSequence= false;
	NumSymmetrySubGraphInSequence= 0;

	DisplayTree = false;
	SelectedTree= 0; // Axonometric tree
	NumBranches= 0;
}


void CWinRf_ViewSketch2D::OnSetFocus(CWnd* pOldWnd) 
// The framework calls this member function after gaining the input focus
// (pOldWnd contains the CWnd object that loses the input focus)
{
	CScrollView::OnSetFocus(pOldWnd);
}


BOOL CWinRf_ViewSketch2D::OnEraseBkgnd(CDC* pDC)
// Called by invalidate
// Also called before drawing every segment of a new stroke
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect.left, rect.top, rect.right, rect.bottom,
					   ReferApp.m_BackgroundColor);

	// Return TRUE indicates that no further erasing is required
	return true;
}


void CWinRf_ViewSketch2D::OnDraw(CDC* pDC)
{
	//Update window to ensure that pan and zoom
	//are updated after mouse movements
	UpdateWindow();

	//Update scene
	GDI_DrawSketch2D();
}


////////////////////// MOUSE ////////////////////////////



bool CWinRf_ViewSketch2D::LiangBarsky (RECTANGLE2D Limit,   // Rectangle
									   POINT2D src0, POINT2D src1) // Segment
// This function inputs one rectangle and one segment,
// and outputs true if the segment intersects the rectangle, and false otherwise
// (Adapted from: Liang-Barsky function by Daniel White @ http://www.skytopia.com/project/articles/compsci/clipping.html)
{
	double t0 = 0.0;    double t1 = 1.0;
	double xdelta = src1.x-src0.x;
	double ydelta = src1.y-src0.y;
	double p,q;

	for(int edge=0; edge<4; edge++) {   // Traverse through left, right, bottom, top edges.
		if (edge==0) {  p = -xdelta;    q = -(Limit.left-src0.x);  }
		if (edge==1) {  p =  xdelta;    q =  (Limit.right-src0.x); }
		if (edge==2) {  p = -ydelta;    q = -(Limit.bottom-src0.y);}
		if (edge==3) {  p =  ydelta;    q =  (Limit.top-src0.y);   }
		if(p==0 && q<0) return false;   // The rectangle does not touch the segment at all

		double r = q/p;
		if(p<0) {
			if(r>t1) return false;      // The rectangle does not touch the segment at all
			else if(r>t0) t0=r;         // Line is clipped!
		} else if(p>0) {
			if(r<t0) return false;      // The rectangle does not touch the segment at all
			else if(r<t1) t1=r;         // Line is clipped!
		}
	}

	return true;        // The rectangle touches the segment
}


bool CWinRf_ViewSketch2D::SelectStrokes(CPoint point, std::vector<long> &Selection)
// Selects the strokes overlapped by the tolerance rectangle around "point"

// Returns true if any stroke is selected, false otherwise
// Also returns the numbers of the selected strokes
{
	//Tolerance rectangle around "point"
	RECTANGLE2D Limit;
	long Tolerance= 6; //Tolerance rectangle size in Pixels
	Limit.left  = (double) (point.x-Tolerance);
	Limit.right = (double) (point.x+Tolerance);
	Limit.bottom= (double) (point.y-Tolerance);
	Limit.top   = (double) (point.y+Tolerance);

	//Search for strokes overlapped by the rectangle
	for (long istroke= 0; istroke<(long)pDoc->DB_InputSketch->GetNumStrokes(); istroke++){
		CStroke StrokeI= pDoc->DB_InputSketch->GetStroke(istroke);
		CStrokePoint Point1= StrokeI.GetPoint(0);
		POINT2D P1= Point1.GetPosition();
		CStrokePoint Point2= StrokeI.GetPoint(StrokeI.GetNumPoints()-1);
		POINT2D P2= Point2.GetPosition();

		//Transform world into viewport coordinates
		P1 = Transf.GDI_WorldToViewport(P1);
		P2 = Transf.GDI_WorldToViewport(P2);

		if(LiangBarsky (Limit,P1, P2)){
			long Position;
			if(IsValueInVector(istroke, Selection, Position))
				Selection.erase(Selection.begin()+Position);
			else
				Selection.push_back(istroke);
		}
	}
	return (Selection.size()>0);
}


bool CWinRf_ViewSketch2D::SelectStrokes(RECTANGLE2D rect, std::vector<long> &Selection)
// Selects the strokes contained in the rectangle

// Returns true if any stroke is selected, false otherwise
// Also returns the list of the selected strokes
{
	//Search for strokes overlapped by the rectangle
	for (long istroke= 0; istroke<(long)pDoc->DB_InputSketch->GetNumStrokes(); istroke++){
		CStroke StrokeI= pDoc->DB_InputSketch->GetStroke(istroke);
		POINT2D P1= StrokeI.GetPoint(0).GetPosition();
		POINT2D P2= StrokeI.GetPoint(StrokeI.GetNumPoints()-1).GetPosition();

		//RECTANGLE2D S= StrokeI.GetBounds();

		//Transform world into viewport coordinates
		P1 = Transf.GDI_WorldToViewport(P1);
		P2 = Transf.GDI_WorldToViewport(P2);

		//if(LiangBarsky (rect, P1, P2)){
		if(P1.x>=rect.left && P1.x<=rect.right &&
		   P1.y>=rect.bottom && P1.y<=rect.top &&
		   P2.x>=rect.left && P2.x<=rect.right &&
		   P2.y>=rect.bottom && P2.y<=rect.top){
			long Position;
			if(IsValueInVector(istroke, Selection, Position))
				Selection.erase(Selection.begin()+Position);
			else
				Selection.push_back(istroke);
		}
	}
	return (Selection.size()>0);
}


void CWinRf_ViewSketch2D::GetErasedSketch(CPoint EraserLocation)
//Updates erased sketch
{
	//Transform eraser location and size to world coordinates
	POINT2D Eraser= Transf.GDI_ViewportToWorld(EraserLocation);

	CPoint radius(Eraser_Radius, Eraser_Radius);
	POINT2D SizeEraser= Transf.GDI_ViewportToWorld(radius);
	CPoint zero(0, 0);
	POINT2D Zero= Transf.GDI_ViewportToWorld(zero);
	double deltaX= std::abs(SizeEraser.x - Zero.x);
	double deltaY= std::abs(SizeEraser.y - Zero.y);

	//Loop over all the strokes to detect those that are close to the eraser
	for(long i= 0; i<(long)pDoc->DB_InputSketch->GetNumStrokes(); i++){
		RECTANGLE2D rectI= pDoc->DB_InputSketch->GetBounds(i);

		if(Eraser.x+Eraser_Radius > rectI.left &&
		   Eraser.x-Eraser_Radius < rectI.right &&
		   Eraser.y+Eraser_Radius > rectI.bottom &&
		   Eraser.y-Eraser_Radius < rectI.top){
			//The eraser intersects the bounding box of stroke i
			CStroke StrokeI= pDoc->DB_InputSketch->GetStroke(i);

			//Loop over the points of stroke i
			//to detect intersections between the points and the eraser
			long SizeI= StrokeI.GetNumPoints();
			for (long k= SizeI-1; k>=0; k--){
				POINT2D Pk= StrokeI.GetPoint(k).GetPosition();
				if(MODULE_2D(Pk, Eraser) < Eraser_Radius){
					//The eraser intersects the stroke
					if(SizeI == 1){
						//The stroke includes just one point, and must be fully removed
						StrokeI.Clear();
					}
					else{
						if(Eraser_Type== 0 && k!=0 && k!= SizeI-1){
							//Split the sub-stroke into two separate sub-strokes
							//(0,k-1)(k+1,SizeJ-1)
							CStroke Stroke;
							for(long ii= k+1; ii<SizeI-1; ii++)
								Stroke.AddPoint(StrokeI.GetPoint(ii));

							pDoc->DB_InputSketch->AddStroke(Stroke);

							for(long ii= SizeI-1; ii>= k; ii--){
								StrokeI.RemovePoint(ii);
								SizeI--;
							}
						}

						else{
							//Remove point k
							StrokeI.RemovePoint(k);
							SizeI--;
						}
					}
				}
			}

			if(StrokeI.GetNumPoints() > 0){
				//Update the partially erased stroke
				pDoc->DB_InputSketch->SetStroke(i, StrokeI);
			}
			else{
				//Remove the fully erased stroke
				pDoc->DB_InputSketch->erase(i);
			}
		}
	}
}


void CWinRf_ViewSketch2D::ManageEraserMove()
//Updates erased strokes
{
	//Select a pen to simulate the eraser
	CClientDC pDC(this);
	CPen Pluma;
	Pluma.CreatePen(PS_SOLID, Eraser_Radius*2, ReferApp.m_BackgroundColor);
	//Pluma.CreatePen(PS_SOLID, Eraser_Radius*2, RGB(0,0,250));
	pDC.SelectObject(&Pluma);

	//Control windows messages to catch mouse movement
	POINT pPrev, pNew;
	MSG msg;

	bool Done= false;
	CString sMensaje;
	sMensaje= "Procedure in Erase mode:";
	sMensaje+="\r\n    - Left click while moving the cursor";
	sMensaje+="\r\n      over the nodes of the strokes to remove";
	sMensaje+="\r\n                           ";
	sMensaje+="\r\n    - Press ESC when done";
	AfxMessageBox (sMensaje);

	bool eraser= false;
	while (!Done && GetMessage(&msg, NULL, 0, 0)>0) {

		if(msg.message == WM_LBUTTONDOWN){
			GetCursorPos(&pNew);
			ScreenToClient(&pNew);
			pPrev= pNew;
			eraser= true;
			SetCapture();
		}
		else if(eraser && msg.message == WM_MOUSEMOVE){
			//Get new end point
			GetCursorPos(&pNew);
			ScreenToClient(&pNew);

			//Draw the segment from the previous point to the current point
			pDC.MoveTo(pPrev);
			pDC.LineTo(pNew);

			pPrev= pNew;

			//Update sketcth
			//POINT2D P1, P2;
			//P1.x =(double)(pPrev.x)*(CameraZoom * WindowLength/ViewportWide);
			//P1.y =(double)(pPrev.y)*(CameraZoom * WindowHigh/ViewportHigh);
			//P2.x =(double)(pPrev.x)*(CameraZoom * WindowLength/ViewportWide);
			//P2.y =(double)(pPrev.y)*(CameraZoom * WindowHigh/ViewportHigh);

			GetErasedSketch(pNew);
		}
		else if(eraser && msg.message == WM_LBUTTONUP){
			//Save displacement
			GetCursorPos(&pNew);
			ScreenToClient(&pNew);
			eraser= false;
			ReleaseCapture();
		}
		else if(msg.message == WM_KEYUP && msg.wParam == VK_ESCAPE ||
				MouseMode != EraseMode){
			//Done when ESC key is pressed
			Done= true;

			EagerVectorizationUpdate();
		}
	}
}


// DRAWING STROKES


void CWinRf_ViewSketch2D::OnLButtonDown(UINT nFlags, CPoint point) 
//Starts drawing a new stroke
{
	if (ReferApp.SketchMode){
		if(MouseMode == SelectMode){
			firstPoint= lastPoint= point;
			SetCapture();
		}
		else{
			MouseMode=SketchMode;
			lastPoint = point;

			//Starts a new stroke
			CurrentStroke.Clear();

			//Add the current point to the current stroke
			StartTime= 0;
			AddPointToStroke(point, StartTime);
			StartTime = ((double)(clock()) / CLOCKS_PER_SEC);

			SetCapture();
		}
	}

	CScrollView::OnLButtonDown(nFlags, point);
}


void CWinRf_ViewSketch2D::OnLButtonUp(UINT nFlags, CPoint point) 
// Ends drawing the current stroke and updates databases
{
	if (GetCapture() != this) return;

	//Update the selected strokes
	//---------------------------
	if(MouseMode == SelectMode){
		//Detect all selected strokes
		if(point == firstPoint){
			SelectStrokes(point, Selection);
		}
		else{
			RECTANGLE2D rect;
			rect.left= min(firstPoint.x, point.x);
			rect.right= max(firstPoint.x, point.x);
			rect.bottom= min(firstPoint.y, point.y);
			rect.top= max(firstPoint.y, point.y);
			SelectStrokes(rect, Selection);
		}

		ReleaseCapture();
	}

	// Draw the current point and add it to the current stroke
	// -------------------------------------------------------
	else if (ReferApp.SketchMode && MouseMode==SketchMode){
		//Draw a feedback of the cursor movement
			//Create a local device context,
			//to force flushing the drawing commands when it goes out of scope
			CClientDC pDC(this);

			//Select a pen to draw the stroke
			CPen PenSketch;

			PenSketch.CreatePen (PS_SOLID, 5, ColorStroke);
			pDC.SelectObject(PenSketch);

			//Draw the segment from the previous point to the current point
			pDC.MoveTo(lastPoint);
			pDC.LineTo(point);

			// Restore the previous pen
			PenSketch.DeleteObject();

		//Add the current point to the current stroke
		//-------------------------------------------
		double CurrentTime = (((double)(clock()) / CLOCKS_PER_SEC) - StartTime) *1000;
		AddPointToStroke(point, CurrentTime);

////////////////////////////////////////////////
		//This DEBUG CODE saves the current stroke before processing it
		//Is useful to replicate strokes that cause an early crash
		//CDB_Sketch Aux;
		//Aux.AddStroke(*InputStroke[0]);
		//CString NombreFichero = "C:\\REFER 7.0\\Examples\\LastStroke.stk";
		//std::string file= NombreFichero;
		//CInOutSketch* InOutSketch= new CInOutSketch();
		//InOutSketch->writeSketch(file.c_str(), &Aux);
		//delete InOutSketch;
////////////////////////////////////////////////

		//Eager recognition of gestures
		//-----------------------------
		CGestures G;
		if(ReferApp.Ve.ParseGestures &&
		   G.ParseGestures(CurrentStroke,
						   pDoc->DB_InputSketch,
						   ReferApp.Ve.RecognitionApproach,
						   ReferApp.Ve.StrokeCornersApproach)){
			//Update online vectorization after executing the command linked to the gesture
			EagerVectorizationUpdate();
		}

		//Save the stroke and eager vectorize
		//-----------------------------------
		else{
			//Save the stroke into the input sketch
			pDoc->DB_InputSketch->AddStroke(CurrentStroke);

			//Save the stroke into the working sketch, if it is active
			if(DisplayWorkingSketch)
				pDoc->DB_WorkingSketch->AddStroke(CurrentStroke);

			//Iff in online vectorization mode, vectorize the current stroke
			if(ReferApp.GeneralParams.bVectorize_Int){
				EagerVectorizeStroke(CurrentStroke);
			}
		}

		MouseMode=InactiveMode;
		ReleaseCapture();
	}

	Invalidate();

	CScrollView::OnLButtonUp(nFlags, point);
}


void CWinRf_ViewSketch2D::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(GetCapture() != this) return;

	//Create a local device context,
	//to force flushing the drawing commands when it goes out of scope
	CClientDC pDC(this);

	switch (MouseMode){
	case SketchMode:{
		//Draw a feedback of the cursor movement
			//Select a pen to draw the stroke
			CPen PenSketch;
			PenSketch.CreatePen (PS_SOLID, 1, ColorStroke);
									//WARNING! Only thickness 1 works
									//Greater thicknesses prevent the screen from being refreshed
									//until some sort of buffer is filled
									//(This is what happens while drawing very long lines)
									//Invalidate forces redrawing, but produce flickering too
									// Pere, june 2010
			pDC.SelectObject(PenSketch);

			//Draw the segment from the previous point to the current point
			pDC.MoveTo(lastPoint);
			pDC.LineTo(point);

			//Restore the previous pen
			PenSketch.DeleteObject();

		//Update cursor position
		lastPoint = point;

		//Add the current point to the current stroke
		double CurrentTime = (((double)(clock()) / CLOCKS_PER_SEC) - StartTime) *1000;
		AddPointToStroke(point, CurrentTime);
		}

		break;

	case ZoomMode:{
		//Update zoom
		CameraZoom += (double)(point.y-lastPoint.y)/100;

		//Update cursor position
		lastPoint=point;

		//Invalidate to ensure re-painting
		Invalidate();
		}
		break;

	case PanMode:{
		//Update pan;
		CameraPanX +=(double)(point.x-lastPoint.x)*(CameraZoom * WindowLength/ViewportWide);
		CameraPanY +=(double)(lastPoint.y-point.y)*(CameraZoom * WindowHigh/ViewportHigh);

		//Update cursor position
		lastPoint=point;

		//Invalidate to ensure re-painting
		//(and recalculation of viewport/wold transformation)
		Invalidate();
		}
		break;

	case SelectMode:
		if(GetKeyState(VK_LBUTTON)& 0x8000){   //While left button is pressed
			pDC.SetROP2(R2_NOT); //XOR mode to remove by simply repainting
			pDC.SelectObject(GetStockObject(NULL_BRUSH)); //Null brush to prevent filling

			//Remove previous rubberband rectangle
			CRect rectP(firstPoint, lastPoint);
			pDC.Rectangle(rectP);

			//Draw new rubberband rectangle
			lastPoint=point;
			CRect rectN(firstPoint, lastPoint);
			pDC.Rectangle(rectN);
		}

		break;
	}

	CScrollView::OnMouseMove(nFlags, point);
}


void CWinRf_ViewSketch2D::AddPointToStroke(CPoint point, double CurrentTime)
// Adds the current cursor position as a new point in the input stroke
{
	// Transform to world coordinates
	POINT2D World= Transf.GDI_ViewportToWorld(point);

	// Add the point to the current stroke
	CStrokePoint p(World.x, World.y, CurrentTime);
	CurrentStroke.AddPoint(p);
}


// ZOOM AND PAN CONTROL

void CWinRf_ViewSketch2D::OnRButtonDown(UINT nFlags, CPoint point) 
// Starts a pan (or a zoom if MK_CONTROL is clicked too)
// The operations are controlled in "OnMouseMove"
{
	lastPoint = point;

	if (nFlags & MK_CONTROL){
		SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));
		MouseMode=ZoomMode;
	}
	else{
		SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Pan));
		MouseMode=PanMode;
	}

	SetCapture();

	CScrollView::OnRButtonDown(nFlags, point);
}


void CWinRf_ViewSketch2D::OnRButtonUp(UINT nFlags, CPoint point) 
// Ends pan (or zoomn if MK_CONTROL was clicked too)
{
	//if (ReferApp.SketchMode)
	//	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Arrow));
	//else
	//	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));

	MouseMode = InactiveMode;

	ReleaseCapture();
	Invalidate();
	
	CScrollView::OnRButtonUp(nFlags, point);
}


void CWinRf_ViewSketch2D::OnMButtonDown(UINT nFlags, CPoint point) 
// Starts a pan
// The operations are controlled in "OnMouseMove"
{
	lastPoint = point;
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Pan));
	MouseMode=PanMode;

	SetCapture();

	CScrollView::OnMButtonDown(nFlags, point);
}


void CWinRf_ViewSketch2D::OnMButtonUp(UINT nFlags, CPoint point) 
{
	//if (ReferApp.SketchMode)
	//	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Arrow));
	//else
	//	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));

	MouseMode = InactiveMode;

	ReleaseCapture();
	Invalidate();

	CScrollView::OnMButtonUp(nFlags, point);
}


// ZOOM VARIATION BY MOUSE WHEEL ROTATION

BOOL CWinRf_ViewSketch2D::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
//Captures wheel rotation and determines zoom variation

//The zDelta value is expressed in multiples or divisions of WHEEL_DELTA, which is 120
{
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));
	MouseMode=ZoomMode;

	CameraZoom += ((double)zDelta / 1200);

	Invalidate();
	MouseMode= SketchMode;

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CWinRf_ViewSketch2D::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
//The framework calls this member function if mouse input is not captured
//and the mouse causes cursor movement within the CWnd object.

// This function must be overriden
// to select the suitable icon
// and to prevent cursor icon from flickering
{
	if (ReferApp.SketchMode){
		if(MouseMode == SelectMode)
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Select));
		else if(MouseMode == LineMode)
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Line));
		else if(MouseMode == EraseMode)
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Erase));
		else if(MouseMode == PanMode)
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Pan));
		else if(MouseMode == ZoomMode)
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));
		else
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Pencil));
	}
	else{
		if(MouseMode == LineMode)
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Line));
		else
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Arrow));
	}

	return true;
}

////////////////////// END MOUSE ////////////////////////////


//////////////////// KEYBOARD ///////////////////////////


void CWinRf_ViewSketch2D::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==VK_DELETE && Selection.size()>0){
		//Sort the list of selected strokes
		std::vector<long> sorted= SortMinorFirst(Selection);

		//Remove selected strokes
		//(from last to fist, to minimize data updates)
		for(long i= (long)sorted.size()-1; i>=0; i--){
			pDoc->DB_InputSketch->erase(sorted[i]);
		}

		//Delete vectorization
		CVectorization V;
		V.ClearVectorize(pDoc);

		//Update the online vectorization
		EagerVectorizationUpdate();

		Selection.clear();
		Invalidate();
	}

	else if(nChar==VK_ESCAPE && Selection.size()>0){
		Selection.clear();
		Invalidate();
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
//////////////////// END KEYBOARD ///////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewSketch2D drawing


void CWinRf_ViewSketch2D::EllipseToBezier(double left, double right, double bottom, double top,
										  std::vector <POINT2D> &E)
//Auxiliar function that creates points to simulate ellipse using Beziers

//The ellipse is inscribed in rectangle (left, right, bottom, top)

//This is useful to draw rotated circles and ellipses (without filling the interior),
//as Bezier curves are invariant to scaling and rotation

//References:
		//http://www.codeguru.com/cpp/g-m/gdi/article.php/c131/Drawing-Rotated-and-Skewed-Ellipses.htm

		//Feng Yuan. Windows Graphics Programming: Win32 GDI and DirectDraw. Prentice Hall PTR (2000)
		//http://www.fengyuan.com/

		//Riškus A. (2006). "Approximation of a cubic bezier curve by circular arcs and vice versa".
		//Information technology and control, 35 (4): 371–378
{
	//MAGICAL CONSTANT to map ellipse to beziers
	const double kappa = 0.2761423749154; // 2/3*(sqrt(2)-1)   // By Adam Stanislav

	//Use the following line instead for mapping systems where +ve Y is downwards
	//POINT2D offset= ((right-left)*EToBConst, (top-bottom)*EToBConst);
	//Use the following lines instead for mapping systems where +ve Y is upwards
	POINT2D offset((right-left)*kappa, (bottom-top)*kappa);

	POINT2D centre((left+right)/2, (top+bottom)/2);

	E[0].x  =							//-----------/
	E[1].x  =							//
	E[11].x =							// 2___3___4 /
	E[12].x = left;						// 1       5 /
	E[5].x  =							// |       | /
	E[6].x  =							// |       | /
	E[7].x  = right;					// 0,12    6 /
	E[2].x  =							// |       | /
	E[10].x = centre.x - offset.x;		// |       | /
	E[4].x  =							// 11      7 /
	E[8].x  = centre.x + offset.x;		// 10__9___8 /
	E[3].x  =							//
	E[9].x  = centre.x;					//-----------/

	E[2].y  =
	E[3].y  =
	E[4].y  = top;
	E[8].y  =
	E[9].y  =
	E[10].y = bottom;
	E[7].y  =
	E[11].y = centre.y + offset.y;
	E[1].y  =
	E[5].y = centre.y - offset.y;
	E[0].y  =
	E[12].y =
	E[6].y  = centre.y;
}


void CWinRf_ViewSketch2D::UnitAngleArcToBezier(double angleA, double angleB,
											   POINT2D (&quadE)[4])
//The function approximates a circle arc through a Bezier curve
//The arc must span less than 90 degrees
{
	//Define a unit size Bezier centered along the positive x axis
	double x0= cos((angleB-angleA)/2.);
	double y0= sin((angleB-angleA)/2.);
	double tx= (1.-x0)*4./3.;
	double ty= y0 - tx * x0 / y0;

	double xx[4];
	double yy[4];

	xx[0]= x0;
	yy[0]= -y0; 
	xx[1]= x0 + tx;
	yy[1]= -ty;
	xx[2]= x0 + tx;
	yy[2]= ty;
	xx[3]= x0;
	yy[3]= y0;

	//Rotate to the actual orientation
	double s = sin((angleA+angleB)/2.);
	double c = cos((angleA+angleB)/2.);

	for(int i= 0; i<4; ++i){
		quadE[i].x = xx[i] * c - yy[i] * s;
		quadE[i].y = xx[i] * s + yy[i] * c;
	}
}


void CWinRf_ViewSketch2D::EllipseArcToBezier(double Ra, double Rb, 
											 double StartAngle, double EndAngle,
											 std::vector <POINT2D> &E)
//The function approximates an elliptic arc through a Bezier curve

//The arc parameters are:
//		Its centre is at (0, 0)
//		Its main semiaxes are (Ra, Rb)
//		Its axis a is horizontal
//		Starts at an angle "StartAngle"
//		Ends at an angle "EndAngle"

//The approach uses a single Bezier curve to draw the arc
//If the arc is over 90 degress, it's better to break it into multiple pieces

//An ellipse is a circle that's been stretched in one direction,
//and possibly rotated (if the axes aren't parallel to the coordinate system)

//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
{
	//Get a Bezier that fits a unit arc centered around the origin
	double SweepAngle = EndAngle-StartAngle;

	E.clear();
	if(SweepAngle == 0)
		return; //Null arc

	//Get a single Bezier for every quadrant and sew them
	POINT2D quadE[4];
	for(double ax = 0; ax<SweepAngle; ax +=NUMBER_PI/2.){
		if(SweepAngle > ax + NUMBER_PI/2.)
			UnitAngleArcToBezier(ax, ax + NUMBER_PI/2., quadE);
		else
			UnitAngleArcToBezier(ax, SweepAngle, quadE);

		if(E.size() == 0)
			E.push_back(quadE[0]);
		E.push_back(quadE[1]);
		E.push_back(quadE[2]);
		E.push_back(quadE[3]);
	}

	if(E.size() == 0)
		return;  //Null arc

	//Rotate the arc to align it to the polar start angle
	double PolarAngle= StartAngle;

	//double PolarAngle= atan2(Rb*sin(StartAngle), Ra*cos(StartAngle));
	//if(PolarAngle<0.) PolarAngle+= 2*NUMBER_PI;

	double sinAng = sin(PolarAngle);
	double cosAng = cos(PolarAngle);

	for(int i = 0; i<(int)E.size(); ++ i){
		double oldX = E[i].x;
		double oldY = E[i].y;
		E[i].x = oldX*cosAng - oldY*sinAng;
		E[i].y = oldX*sinAng + oldY*cosAng;
	}

	//Resize the unit arc to fit the sizes of the ellipse axes
	for(long i = 0; i <(int)E.size(); ++ i){
		E[i].x= E[i].x * Ra;
		E[i].y= E[i].y * Rb;
	}
}


void CWinRf_ViewSketch2D::DisplayDot(CClientDC *dc, CPoint Position, long size)
//Draws a "fat" dot centered in Position, with elliptical form and radius size (in pixels)
{
	dc->MoveTo(Position.x,Position.y);

	CRect dot;
	dot.left= Position.x-size;
	dot.right= Position.x+size;
	dot.bottom= Position.y-size;
	dot.top= Position.y+size;

	dc->Ellipse(dot);
}


POINT2D CWinRf_ViewSketch2D::GetDisplacedPoint(POINT2D p1, POINT2D p2, double d)
//Returns a point displaced a distance "d" from the midpoint of the segment defined by (p1, p2)
{
	POINT2D disp;

	double xc= (p1.x+p2.x)/2;
	double yc= (p1.y+p2.y)/2;

	if(p1.x == p2.x){
		//Segment vertical
		disp.x= xc + d;
		disp.y= yc;
		return disp;
	}
	if(p1.y == p2.y){
		//Segment horizontal
		disp.x= xc;
		disp.y= yc + d;
		return disp;
	}

	//Line perpendicular to segment (p1, p2) and passing through midpoint
	double m= (p1.x-p2.x)/(p2.y-p1.y);
	double k= 1+ m*m;

	//Point at distance d from midpoint
	disp.x= (k*xc + d*sqrt(k)) / k;
	disp.y= yc + m*(disp.x - xc);

	return disp;
}


long CWinRf_ViewSketch2D::RemoveLines(CPoint point)
// Removes the lines overlapped by the tolerance rectangle around "point"

// Returns the number of removed lines
{
	long removed= 0;

	//Tolerance rectangle around "point"
	RECTANGLE2D Limit;
	long Tolerance= 6; //Tolerance rectangle size in Pixels
	Limit.left  = (double) (point.x-Tolerance);
	Limit.right = (double) (point.x+Tolerance);
	Limit.bottom= (double) (point.y-Tolerance);
	Limit.top   = (double) (point.y+Tolerance);

	//Search for lines overlapped by the rectangle
	for (long iline= 0; iline<(long)pDoc->DB_DrawingOn->GetSizeEdges(); iline++){
		CEdge edgeI= pDoc->DB_DrawingOn->GetEdge(iline);
		CVertex vertex1= pDoc->DB_DrawingOn->GetVertex(edgeI.Head);
		POINT2D P1(vertex1.x, vertex1.y);
		CVertex vertex2= pDoc->DB_DrawingOn->GetVertex(edgeI.Tail);
		POINT2D P2(vertex2.x, vertex2.y);

		//Transform world into viewport coordinates
		P1 = Transf.GDI_WorldToViewport(P1);
		P2 = Transf.GDI_WorldToViewport(P2);

		if(LiangBarsky (Limit,P1, P2)){
			//Update the database
			pDoc->DB_DrawingOn->RemoveEdge(iline, true);

			//Update the screen
			CClientDC pDC(this);
			CPen Pluma;
			Pluma.CreatePen(PS_SOLID, 2, ReferApp.m_BackgroundColor);
			pDC.SelectObject(&Pluma);
			pDC.MoveTo((int)P1.x, (int)P1.y);
			pDC.LineTo((int)P2.x, (int)P2.y);

			iline--;
			removed++;
		}
	}
	return removed;
}


bool CWinRf_ViewSketch2D::GetLine()
//Gains control on windows messages to catch a line and add it to the database

//Removes pointed lines when DELETE is pressed
//Flags the end of the drawing mode when ESC is pressed
{
	//Select a pen to draw the rubberline
	CClientDC pDC(this);
	CPen Pluma;
	Pluma.CreatePen(PS_SOLID, 2, RGB(0, 0, 250));
	pDC.SelectObject(&Pluma);
	pDC.SetROP2(R2_NOTXORPEN); //XOR mode to remove by simply repainting

	//Control windows messages to catch the line
	POINT pFirst, pLast;
	MSG msg;
	bool continuar= false;
	bool rubber= false;
	while (!continuar && GetMessage(&msg, NULL, 0, 0)>0) {

		if(msg.message == WM_LBUTTONDOWN){
			GetCursorPos(&pFirst);
			ScreenToClient(&pFirst);
			pLast= pFirst;
			rubber= true;
		}
		else if(rubber && msg.message == WM_MOUSEMOVE){
			//Remove previous rubberline by repainting
			pDC.MoveTo(pFirst);
			pDC.LineTo(pLast);

			//Get new end point
			GetCursorPos(&pLast);
			ScreenToClient(&pLast);

			//Draw new rubberline
			pDC.MoveTo(pFirst);
			pDC.LineTo(pLast);
		}
		else if(rubber && msg.message == WM_LBUTTONUP){
			//Save displacement
			GetCursorPos(&pLast);
			ScreenToClient(&pLast);
			continuar= true;
		}

		else if(msg.message == WM_KEYUP && msg.wParam == VK_DELETE){
			//Removes the lines pointed by the cursor
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(&point);

			RemoveLines(point);
			return true;
		}
		else if(msg.message == WM_KEYUP && msg.wParam == VK_ESCAPE){
			//Cancel the drawing mode if ESC key is pressed
			return false;
		}
	}

	//Transform the tips to world coordinates
	POINT2D First= Transf.GDI_ViewportToWorld(pFirst);
	POINT2D Last= Transf.GDI_ViewportToWorld(pLast);

	//Update the line-drawing
	pDoc->DB_DrawingOn->AddEdgeByVertices(First.x, First.y, 0, Last.x, Last.y, 0);

	return true;
}


void CWinRf_ViewSketch2D::GDI_DrawBackGroundImage()
// Draws the current background image:
{
	//Load the Image File
	HBITMAP hBmp = (HBITMAP)::LoadImage(NULL, ReferApp.BackgroundBitMapFile,
										IMAGE_BITMAP, 0, 0,
										LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	//Create a bitmap object and attach it to the object
	CBitmap CurrentBMP;
	CurrentBMP.Attach(hBmp);

	CClientDC dc(this);

	//Create a Memory DC and select the BMP to it
	CDC bmDC;
	bmDC.CreateCompatibleDC(&dc);
	CBitmap *pOldbmp = bmDC.SelectObject(&CurrentBMP);

	//Get the BMP Height and Width
	BITMAP BitmapInfo;
	CurrentBMP.GetBitmap(&BitmapInfo);

	//Copy the block of pixels from memoryDC to the screen
	//dc.BitBlt(0, 0, BitmapInfo.bmWidth, BitmapInfo.bmHeight, &bmDC, 0, 0, SRCCOPY);

	//Stretch the block of pixels from memoryDC into the screen
	CPoint P1, P2;
	P1= Transf.GDI_WorldToViewport(-BitmapInfo.bmWidth/2, -BitmapInfo.bmHeight/2);
	P2= Transf.GDI_WorldToViewport(+BitmapInfo.bmWidth/2, +BitmapInfo.bmHeight/2);

	dc.StretchBlt(P1.x, P2.y, (P2.x-P1.x), (P1.y-P2.y),                   //destination
				  &bmDC, 0, 0, BitmapInfo.bmWidth, BitmapInfo.bmHeight,   //origin
				  SRCCOPY);

	//Re-select the old BMP.
	bmDC.SelectObject(pOldbmp);
}


void CWinRf_ViewSketch2D::GDI_Draw2DSketch(CDB_Sketch *DB_Sketch)
// Draws the 2D sketch
{
	//Loop over the strokes
	for (long istroke= 0; istroke<(long)DB_Sketch->GetNumStrokes(); istroke++){
		CClientDC dc(this);
		CPen Pluma;
		if (DisplayInBlack)	//Select color
			Pluma.CreatePen( PS_SOLID, 3, RGB(0, 0, 0));
		else{
			if(IsValueInVector(istroke, Selection))
				Pluma.CreatePen( PS_SOLID, 3, RGB(250,200,100));
			else
				Pluma.CreatePen( PS_SOLID, 3, RGB(200,200,100));
		}

		dc.SelectObject(&Pluma);

		CStroke StrokeI= DB_Sketch->GetStroke(istroke);

		long numP= StrokeI.GetNumPoints();
		//Draws a polyline linking consecutive points of the stroke
		if (numP>0){
			CStrokePoint Point0= StrokeI.GetPoint(0);
			POINT2D c= Point0.GetPosition();
			CPoint Punto = Transf.GDI_WorldToViewport(c.x, c.y);
			dc.MoveTo(Punto.x,Punto.y);

			for (long ipoint=1; ipoint<numP; ipoint++){
				CStrokePoint PointI= StrokeI.GetPoint(ipoint);
				c= PointI.GetPosition();
				Punto = Transf.GDI_WorldToViewport(c.x, c.y);
				dc.LineTo(Punto.x,Punto.y);
			}

///////////////////////////////////////////////////////////////////
			if(Display_Convex_Hull){
				Pluma.DeleteObject();
				Pluma.CreatePen(PS_DOT, 1, RGB(200,0,0));
				dc.SelectObject(&Pluma);
				CStroke Hull= StrokeI;
				Hull.TurnIntoConvexHull();
				c= Hull.GetPoint(0).GetPosition();
				Punto = Transf.GDI_WorldToViewport(c.x, c.y);
				dc.MoveTo(Punto.x,Punto.y);

				for (long ipoint=1; ipoint<Hull.GetNumPoints(); ipoint++){
					CStrokePoint PointI= Hull.GetPoint(ipoint);
					c= PointI.GetPosition();
					Punto = Transf.GDI_WorldToViewport(c.x, c.y);
					dc.LineTo(Punto.x,Punto.y);
				}
			}
///////////////////////////////////////////////////////////////////
		}
	}

	//DRAW THE NODES AND CORNERS
	if(DisplayNodesAndCorners){
		GDI_DrawNodesAndVertices(DB_Sketch);
	}
	//DRAW THE SLIDING STRIPS USED FOR FINDING CORNERS
	if(DisplaySlidingStrips){
		GDI_DrawSlidingStrips(DB_Sketch);
	}
	//DRAW THE TOLERANCE BANDS USED FOR CALCULATING MERITOS OF FITTING STOKES INTO LINES/ARCS
	if(DisplayToleranceBands){
		GDI_DrawToleranceBands(DB_Sketch);
	}
}


void CWinRf_ViewSketch2D::GDI_Draw2DDrawing(CDB_Model *DDBB, int R, int G, int B)
//Draws the vectorized 2D line drawing
{
	int RR= R; int GG= G; int BB= B;
	if (DisplayInBlack)
		{RR= 0; GG= 0; BB= 0;}

	//Loop over the lines
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CClientDC dc(this);
		CPen Pluma;

		//Display hidden edges
		if (DisplayHiddenEdges){
			switch (ReferApp.Cu.Hi.TypesOfEdges[i]){
				case VISIBLE	: Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));  break;
				case HIDDEN		: Pluma.CreatePen(PS_DASH, 1, RGB(RR, GG, BB));  break;
				case UNDEFINED	: Pluma.CreatePen(PS_DOT, 1, RGB(RR, GG, BB));  break;
			};
		}
		else{
			Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));
		}
		dc.SelectObject(&Pluma);

		//Draws a line linking the head and the tail of the stroke
		CEdge E= DDBB->GetEdge(i);
		CVertex HeadVertex= DDBB->GetVertex(E.Head);
		CVertex TailVertex= DDBB->GetVertex(E.Tail);
		CPoint P = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
		dc.MoveTo(P.x,P.y);
		P = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
		dc.LineTo(P.x,P.y);
	}

	//Loop over the arcs
	for (long i= 0; i<(long)DDBB->GetSizeArcs(); i++){
		CClientDC dc(this);
		CPen Pluma;

		CArc Arc= pDoc->DB_DrawingOff->GetArc(i);

		//Display hidden arcs
		if (DisplayHiddenArcs){
			switch (ReferApp.Cu.Hi.TypesOfEdges[i]){
				case VISIBLE	: Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));  break;
				case HIDDEN		: Pluma.CreatePen(PS_DASH, 1, RGB(RR, GG, BB));  break;
				case UNDEFINED	: Pluma.CreatePen(PS_DOT, 1, RGB(RR, GG, BB));  break;
			};
		}
		else{
			Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));
		}
		dc.SelectObject(&Pluma);

		//Get the elliptical arc
		std::vector <POINT2D> E;
		EllipseArcToBezier(Arc.Radius_a, Arc.Radius_b,
						   Arc.StartAngle-Arc.Rotation, Arc.EndAngle-Arc.Rotation, E);

		//Rotate, translate and transform to viewport
		double sinA= sin(Arc.Rotation);
		double cosA= cos(Arc.Rotation);

		long SizeEE= (long)E.size();
		CPoint EE[13];
		for(long i= 0; i<SizeEE; i++){
			EE[i]= Transf.GDI_WorldToViewport((E[i].x*cosA - E[i].y*sinA) + Arc.Centre_x,
											  (E[i].x*sinA + E[i].y*cosA) + Arc.Centre_y);
		}

		//Draw the elliptical bezier
		dc.BeginPath();
		dc.PolyBezier(EE, SizeEE);
		dc.EndPath();
		dc.StrokePath();
	}
}


void CWinRf_ViewSketch2D::GDI_DrawLabels(CDB_Model *DDBB_2D)
// Draws the labels:
//		numbers of strokes (plus merits)
//		numbers of vertices
//		numbers of edges
//		valences of vertices
//		level of each vertex
//		coordinates of vertices
{
	if (!DisplayNumStrokes && !DisplayNumEdges &&
		!DisplayNumVertices && !DisplayMeritsOfVertices &&!DisplayCoordVertices &&
		!DisplayLevels && !DisplayValences)
		return;

	CClientDC dc(this);

	//SET FONT
	CRect rect;
	GetClientRect(&rect);
	long height= (long) (rect.Height()/10 * TextLabelsSizeScale);  //Standard height is set as 1% of the client window height

	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);  //Initializes a CFont with the specified height, measured in tenths of a point, and typeface.
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	CString Texto, Texto1;

	//NUMBER STROKES
	if (DisplayNumStrokes && (DisplayInputSketch || DisplayWorkingSketch)){
		//Calculate offset for sketch labels
		RECTANGLE2D Bounds= pDoc->DB_InputSketch->GetBounds();
		double Offset=0.04* min(Bounds.right-Bounds.left, Bounds.top-Bounds.bottom)/2;

		if (DisplayInBlack)
			dc.SetTextColor(RGB( 0, 0, 0));
		else{
			dc.SetTextColor(TextLabelsColor);
		}
		CPen Pluma;

		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 0, RGB(250,0,0));
		dc.SelectObject( &Pluma );

		long nStrokes;
		if(DisplayWorkingSketch){
			nStrokes= pDoc->DB_WorkingSketch->GetNumStrokes();
		}
		else{ //if(DisplayInputSketch)
			nStrokes= pDoc->DB_InputSketch->GetNumStrokes();
		}

		for (long i=0; i<nStrokes; i++){
			CStroke StrokeI;
			if(DisplayWorkingSketch){
				StrokeI= pDoc->DB_WorkingSketch->GetStroke(i);
			}
			else {//if(DisplayInputSketch)
				StrokeI= pDoc->DB_InputSketch->GetStroke(i);
			}

			CStrokePoint PointM= StrokeI.GetPoint(StrokeI.GetNumPoints()/2);
			POINT2D PM= PointM.GetPosition();  //Mid point

			//Get text
			Texto="";
			Texto1.Format("%i", i);
			Texto+=Texto1;
			if (DisplayWorkingSketch && Display_Merits_Strokes){
				Texto1.Format(" (%4.2f, %4.2f)", StrokeI.MeritLine, StrokeI.MeritArc);
				Texto+=Texto1;
			}
			CSize Tsize= dc.GetTextExtent(Texto);

			//Get positions
			CPoint Base= Transf.GDI_WorldToViewport(PM.x, PM.y);
			CPoint Reference= Transf.GDI_WorldToViewport(PM.x+Offset, PM.y+Offset);

			CRect rect;
			rect.left= Reference.x;
			rect.right= Reference.x+Tsize.cx;
			rect.top= Reference.y;
			rect.bottom= Reference.y-Tsize.cy;

			//Draw extension
			dc.MoveTo(Base.x,Base.y);
			dc.LineTo(Reference.x,Reference.y);

			dc.Rectangle(rect);

			//Draw label
			dc.TextOut(Reference.x,Reference.y-Tsize.cy, Texto);
		}
	}

	//Calculate offset for drawing labels
	//(distance to move the labels appart from their center point)
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	pDoc->DB_DrawingOff->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double Offset=0.04* min(dXmax-dXmin, dYmax-dYmin)/2;

	//NUMBER EDGES
	if (DisplayNumEdges && (DisplayOffLine || DisplayModelBrep)){
		long nEdges;
		if(DisplayModelBrep)
			nEdges= pDoc->DB_Brep_Model->GetSizeEdges();
		else if(DisplayOffLine)
			nEdges= pDoc->DB_DrawingOff->GetSizeEdges();
		else  //if(DisplayOnLine)
			nEdges= pDoc->DB_DrawingOn->GetSizeEdges();

		for (long i=0; i<nEdges; i++){
			//Prioritize the edges to label
			CEdge Arista;
			CVertex HeadVertex;
			CVertex TailVertex;
			if(DisplayModelBrep){
				Arista= DDBB_2D->GetEdge(i);
				HeadVertex= DDBB_2D->GetVertex(Arista.Head);
				TailVertex= DDBB_2D->GetVertex(Arista.Tail);
			}
			else if(DisplayOffLine){
				Arista= pDoc->DB_DrawingOff->GetEdge(i);
				HeadVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Head);
				TailVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Tail);
			}
			else{
				Arista= pDoc->DB_DrawingOff->GetEdge(i);
				HeadVertex= pDoc->DB_DrawingOn->GetVertex(Arista.Head);
				TailVertex= pDoc->DB_DrawingOn->GetVertex(Arista.Tail);
			}

			//Get text
			Texto.Format("%i", i);
			CSize Tsize= dc.GetTextExtent(Texto);

			//Get positions
			CPoint Base= Transf.GDI_WorldToViewport((HeadVertex.x+TailVertex.x)/2, (HeadVertex.y+TailVertex.y)/2);

			POINT2D T(TailVertex.x, TailVertex.y);
			POINT2D H(HeadVertex.x, HeadVertex.y);
			POINT2D p= GetDisplacedPoint(T, H, Offset);
			CPoint Reference= Transf.GDI_WorldToViewport(p.x, p.y);

			CRect rect;
			rect.left= Reference.x;
			rect.right= Reference.x+Tsize.cx;
			rect.top= Reference.y;
			rect.bottom= Reference.y-Tsize.cy;

			//Draw extension
			CPen Pluma;
			if (DisplayInBlack)  //Select color
				Pluma.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
			else
				Pluma.CreatePen(PS_SOLID, 0, RGB(250,0,0));
			dc.SelectObject(&Pluma);

			dc.MoveTo(Base.x,Base.y);
			dc.LineTo(Reference.x,Reference.y);

			dc.Rectangle(rect);

			//Draw label
			dc.TextOut(Reference.x, Reference.y-Tsize.cy, Texto);
		}

		for (long i=0; i<pDoc->DB_DrawingOff->GetSizeArcs(); i++){
			CArc Arc= pDoc->DB_DrawingOff->GetArc(i);

			//Get text
			Texto.Format("%i", i);
			CSize Tsize= dc.GetTextExtent(Texto);

			//Get positions
			CPoint Base= Transf.GDI_WorldToViewport(Arc.Centre_x, Arc.Centre_y);
			CPoint Reference= Base;

			CRect rect;
			rect.left= Reference.x;
			rect.right= Reference.x+Tsize.cx;
			rect.top= Reference.y;
			rect.bottom= Reference.y-Tsize.cy;

			//Draw extension
			CPen Pluma;
			if (DisplayInBlack)	//Select color
				Pluma.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
			else
				Pluma.CreatePen(PS_SOLID, 0, RGB(250,0,0));
			dc.SelectObject(&Pluma);

			dc.MoveTo(Base.x,Base.y);
			dc.LineTo(Reference.x,Reference.y);

			dc.Rectangle(rect);

			//Draw label
			dc.TextOut(Reference.x,Reference.y-Tsize.cy, Texto);
		}
	}

	//NUMBER VERTICES, COORDINATES, LEVELS AND/OR VALENCES
	if ((DisplayNumVertices ||
		 DisplayMeritsOfVertices || 
		 DisplayCoordVertices || 
		 DisplayLevels ||
		 DisplayValences) && (DisplayOnLine ||DisplayOffLine || DisplayModelBrep)){

		if (DisplayInBlack)
			dc.SetTextColor(RGB( 0, 0, 0));
		else{
			dc.SetTextColor(TextLabelsColor);
		}

		CVertex Vertice;
		long nVertices= 0;
		CDB_Model DDBB_MA= *pDoc->DB_Brep_Model; //By default, select the database aligned with the inflation reference system
		if(DisplayModelBrep){
			nVertices= pDoc->DB_Brep_Model->GetSizeVertices();

			//Transformation used to display Main coordinates
			if(!ReferApp.Cu.MA.MainAngles.empty()){
				//If main axes are known, the model is aligned to them, 
				//before outputing the coordinates of its vertices
				CCueMainAxes MainAxes;
				MainAxes.TransformToMainAxes(&DDBB_MA, true);
			}
		}
		else if(DisplayOffLine)
			nVertices= pDoc->DB_DrawingOff->GetSizeVertices();
		else  //if(DisplayOnLine)
			nVertices= pDoc->DB_DrawingOn->GetSizeVertices();

		for (long i=0; i<nVertices; i++){
			//Prioritize the vertices to label
			if(DisplayModelBrep)
				//Vertice= pDoc->DB_Brep_Model->GetVertex(i);
				Vertice= DDBB_2D->GetVertex(i);
			else if(DisplayOffLine)
				Vertice= pDoc->DB_DrawingOff->GetVertex(i);
			else
				Vertice= pDoc->DB_DrawingOn->GetVertex(i);

			Texto="";
			if (DisplayNumVertices){
				Texto1.Format("%i", i);
				Texto+=Texto1;
			}
			if (DisplayLevels && (long)ReferApp.DIL.LevelOfVertex.size()>i){
				Texto1.Format(" (L%i)", ReferApp.DIL.LevelOfVertex[i]);
				Texto+=Texto1;
			}
			if (DisplayMeritsOfVertices){
				Texto1.Format(" (%3.0f%%)", Vertice.merit*100);
				Texto+=Texto1;
			}
			if (DisplayCoordVertices){
				//Switch to display model coordinates relative to main axes
				//(only if main axes are displaned, and the BRep model too
				CVertex VertexMA= Vertice;
				if(DisplayMainAxes && 
				   DisplayModelBrep &&
				   ReferApp.GeneralParams.DetectMainAxes &&
				   !ReferApp.Cu.MA.MainAngles.empty()){
					VertexMA= DDBB_MA.GetVertex(i);
				}
				Texto1.Format(" (%4.2f, %5.2f, %4.2f)", VertexMA.x, VertexMA.y, VertexMA.z);

				Texto+=Texto1;
			}

			if (DisplayValences){
				Texto1.Format(" V%i", ReferApp.Cu.Va.Valences[i]);
				Texto+=Texto1;
			}

			CPoint Punto = Transf.GDI_WorldToViewport((double)Vertice.x+Offset,(double)Vertice.y+Offset);
			dc.TextOut (Punto.x, Punto.y, Texto);
		}
	}
}


void CWinRf_ViewSketch2D::GDI_DrawDashedStrokes()
//Draws dashed strokes
{
	long NumDL= (long)ReferApp.Ve.DashedStrokes.size();
	if(NumDL<=0)
		return;

	CClientDC dc(this);

	//Set font
	CRect rect;
	GetClientRect(&rect);
	long height= rect.Height()/10;     //Standard height is set as 1% of the client window height

	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	CPoint Punto;

	//Calculate offset for sketch labels
	RECTANGLE2D Bounds= pDoc->DB_InputSketch->GetBounds();
	double Offset=0.04* min(Bounds.right-Bounds.left, Bounds.top-Bounds.bottom)/2;

	//Draw dashed lines
	//------------------
	for (long i= 0; i<NumDL; i++){
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		else{
			COLORREF ColorComponente = RGB(0, 0, (i%3)*75+100);
			Pluma.CreatePen(PS_SOLID, 7, ColorComponente);
		}
		dc.SelectObject(&Pluma);

		//Draw the current dashed line
		long NumDashes= (long)ReferApp.Ve.DashedStrokes[i].size();
		for (long j= 0; j<NumDashes; j++){
			CStroke StrokeJ= pDoc->DB_InputSketch->GetStroke(std::abs(ReferApp.Ve.DashedStrokes[i][j])-1);
			long numP= StrokeJ.GetNumPoints();
			//Draws a polyline linking consecutive points of the stroke
			if (numP>0){
				CStrokePoint Point0= StrokeJ.GetPoint(0);
				POINT2D c= Point0.GetPosition();
				Punto = Transf.GDI_WorldToViewport(c.x, c.y);
				dc.MoveTo(Punto.x,Punto.y);

				for (long ipoint=1; ipoint<numP; ipoint++){
					CStrokePoint PointI= StrokeJ.GetPoint(ipoint);
					c= PointI.GetPosition();
					Punto = Transf.GDI_WorldToViewport(c.x, c.y);
					dc.LineTo(Punto.x,Punto.y);
				}
			}
		}

		//Label the current dashed line
		CStroke StrokeO= pDoc->DB_InputSketch->GetStroke(std::abs(ReferApp.Ve.DashedStrokes[i][0])-1);
		CStrokePoint Point0= StrokeO.GetPoint(0);
		POINT2D P0= Point0.GetPosition();  //First point
		CStroke Stroke1= pDoc->DB_InputSketch->GetStroke(std::abs(ReferApp.Ve.DashedStrokes[i][ReferApp.Ve.DashedStrokes[i].size()-1])-1);
		CStrokePoint Point1= Stroke1.GetPoint(Stroke1.GetNumPoints()-1);
		POINT2D P1= Point1.GetPosition();  //Last point
		POINT2D PM((P0.x+P1.x)/2, (P0.y+P1.y)/2);  //Midd point

		//Get text
		CString Texto;
		Texto.Format("%i", i);
		CSize Tsize= dc.GetTextExtent(Texto);

		//Get positions
		CPoint Base= Transf.GDI_WorldToViewport(PM.x, PM.y);
		CPoint Reference= Transf.GDI_WorldToViewport(PM.x+Offset, PM.y+Offset);

		CRect rect;
		rect.left= Reference.x;
		rect.right= Reference.x+Tsize.cx;
		rect.top= Reference.y;
		rect.bottom= Reference.y-Tsize.cy;

		//Draw extension
		CPen PlumaE;
		if (DisplayInBlack)	//Select color
			PlumaE.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		else
			PlumaE.CreatePen(PS_SOLID, 0, RGB(250,0,0));

		dc.SelectObject(&PlumaE);

		dc.MoveTo(Base.x,Base.y);
		dc.LineTo(Reference.x,Reference.y);

		dc.Rectangle(rect);

		//Draw label
		dc.TextOut(Reference.x,Reference.y-Tsize.cy, Texto);
	}
}


void CWinRf_ViewSketch2D::GDI_DrawSubGraphsInSequence()
//Draws the sub-graph NumSubGraphInSequence
{
	if(NumSubGraphInSequence <0 || NumSubGraphInSequence > ReferApp.Cu.SG.NumSubgraphs-1)
		return;

	if(!DisplayOnLine && !DisplayOffLine)
		return;

	CClientDC dc(this);

	CPen Pluma;
	CPoint Punto;

	//Draw the sub-graph
	//------------------
	if (DisplayInBlack)
		Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	else
		Pluma.CreatePen(PS_SOLID, 4, RGB(255, 150, 0));
	dc.SelectObject(&Pluma);

	for (long i=0; i<pDoc->DB_DrawingOff->GetSizeEdges(); i++){
		if (ReferApp.Cu.SG.SubgraphForEdge[i] == NumSubGraphInSequence){
			CEdge Arista= pDoc->DB_DrawingOff->GetEdge(i);
			CVertex HeadVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Head);
			CVertex TailVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Tail);

			Punto = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
			dc.MoveTo(Punto.x,Punto.y);
			Punto = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
			dc.LineTo(Punto.x,Punto.y);
		}
	}
}


void CWinRf_ViewSketch2D::GDI_DrawVertexLabelling()
// Draws the information of vertex labelling:
{
	if(pDoc->DB_DrawingOff->GetSizeVertices() == 0 ||
	   ReferApp.Cu.La.LabelsInVertices.size() ==0)
		return;

	CClientDC dc(this);

	//SET FONT
	CRect rect;
	GetClientRect(&rect);
	long height= (long) (rect.Height()/10 * TextLabelsSizeScale);   //Standard height is set as 1% of the client window height

	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	CString Texto, Texto1;
	CVertex Vertice;

	for (long i=0; i<pDoc->DB_DrawingOff->GetSizeVertices(); i++){
		Vertice= pDoc->DB_DrawingOff->GetVertex(i);

		if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsL)
			Texto= "L";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsW)
			Texto= "W";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsY)
			Texto= "Y";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsT)
			Texto= "T";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsT4)
			Texto= "4";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsZ)
			Texto= "Z";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsX)
			Texto= "X";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsK)
			Texto= "K";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsKplus)
			Texto= "#";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsM)
			Texto= "M";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIs5way)
			Texto= "5";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIs6way)
			Texto= "6";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIs7way)
			Texto= "7";
		else if(ReferApp.Cu.La.LabelsInVertices[i] == JunctionIsHidden)
			Texto= "H";
		else
			Texto= "?";

		//Get text size
		CSize Tsize= dc.GetTextExtent(Texto);

		//Get positions
		CPoint Base= Transf.GDI_WorldToViewport(Vertice.x,Vertice.y);
		CPoint Reference= Base;
		Reference.x += Tsize.cy;
		Reference.y += Tsize.cy;

		CRect rect;
		rect.left= Reference.x;
		rect.right= Reference.x+Tsize.cx;
		rect.top= Reference.y;
		rect.bottom= Reference.y-Tsize.cy;

		//Draw extension
		CPen Pluma;
		if (DisplayInBlack)	//Select color
			Pluma.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 0, RGB(250,0,0));
		dc.SelectObject( &Pluma );

		dc.MoveTo(Base.x,Base.y);
		dc.LineTo(Reference.x,Reference.y);

		dc.Rectangle(rect);

		//Draw label
		dc.TextOut(Reference.x,Reference.y-Tsize.cy, Texto);
	}
}


void DrawRotatedText(CDC* pDC, const CString str, CRect rect, 
					 double angle, UINT nOptions = 0)
// pDC : pointer to your device-context
// str : the text
// rect: the rectangle
// nOptions: can be a combination of ETO_CLIPPED and ETO_OPAQUE
// (see documentation of ExtTextOut for more details)
{
	// convert angle to radian
	double pi = 3.141592654;
	double radian = pi * 2 / 360 * angle;

	// get the center of a not-rotated text
	CSize TextSize = pDC->GetTextExtent(str);
	CPoint center;
	center.x = TextSize.cx / 2;
	center.y = TextSize.cy / 2;

	// now calculate the center of the rotated text
	CPoint rcenter;
	rcenter.x = long(cos(radian) * center.x - sin(radian) * center.y);
	rcenter.y = long(sin(radian) * center.x + cos(radian) * center.y);

	// finally draw the text and move it to the center of the rectangle
	pDC->SetTextAlign(TA_BASELINE);
	pDC->SetBkMode(TRANSPARENT);
	pDC->ExtTextOut(rect.left + rect.Width() / 2 - rcenter.x, 
					rect.top + rect.Height() / 2 + rcenter.y,
					nOptions, rect, str, NULL);
}


void CWinRf_ViewSketch2D::GDI_DrawLabelsEdges()
// Draws the information of edge labelling:
{
	if(pDoc->DB_DrawingOff->GetSizeEdges() == 0 ||
	   ReferApp.Cu.La.LabelsInEdges.size() ==0)
		return;

	//Calculate the distance to move the labels from their center point
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	pDoc->DB_DrawingOff->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double Offset=-0.05* min(dXmax-dXmin, dYmax-dYmin)/2;

	CClientDC dc(this);

	CRect rect;
	GetClientRect(&rect);
	long height= (long) (rect.Height()/10 * TextLabelsSizeScale);  //Standard height is set as 1% of the client window height

	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	CString Texto, Texto1;

	for (long i=0; i<pDoc->DB_DrawingOff->GetSizeEdges(); i++){
		CEdge Arista= pDoc->DB_DrawingOff->GetEdge(i);
		CVertex HeadVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Head);
		CVertex TailVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Tail);

		if(ReferApp.Cu.La.LabelsInEdges[i] == BOUNDARY)
			Texto= ">";
		else if(ReferApp.Cu.La.LabelsInEdges[i] == RBOUNDARY)
			Texto= "<";
		else if(ReferApp.Cu.La.LabelsInEdges[i] == CONCAVE)
			Texto= "-";
		else if(ReferApp.Cu.La.LabelsInEdges[i] == CONVEX)
			Texto= "+";
		else
			Texto= "?";

		//Get text size
		CSize Tsize= dc.GetTextExtent(Texto);

		//Get positions
		CPoint Base= Transf.GDI_WorldToViewport((HeadVertex.x+TailVertex.x)/2, (HeadVertex.y+TailVertex.y)/2);

		//Draw label
		if(ReferApp.Cu.La.LabelsInEdges[i] == BOUNDARY ||
			ReferApp.Cu.La.LabelsInEdges[i] == RBOUNDARY){
			//Alignt the boundary symbol with the line
			LOGFONT lf;
			::ZeroMemory(&lf, sizeof(lf));
			lf.lfHeight = height;
			lf.lfWeight = FW_BOLD;

			//these control the rotation of the text i[0,3600]
			long iangle= (long)(10*RADIANS_TO_DEGREES(atan2((HeadVertex.y-TailVertex.y), (HeadVertex.x-TailVertex.x))));
			if(iangle<0) iangle+= 3600;
			lf.lfEscapement = iangle;
			lf.lfOrientation = iangle;

			::lstrcpy(lf.lfFaceName, _T("Arial"));
			CFont font;
			font.CreatePointFontIndirect(&lf);
			CFont* oldfont = dc.SelectObject(&font);

			//Print rotated text
			dc.TextOut(Base.x,Base.y, Texto);

			dc.SelectObject(oldfont);
		}
		else{
			POINT2D T(TailVertex.x, TailVertex.y);
			POINT2D H(HeadVertex.x, HeadVertex.y);
			POINT2D p= GetDisplacedPoint(T, H, Offset);
			CPoint Reference= Transf.GDI_WorldToViewport(p.x, p.y);

			CRect rect;
			rect.left= Reference.x;
			rect.right= Reference.x+Tsize.cx;
			rect.top= Reference.y;
			rect.bottom= Reference.y-Tsize.cy;

			//Draw extension
			CPen Pluma;
			if (DisplayInBlack)	//Select color
				Pluma.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
			else
				Pluma.CreatePen(PS_SOLID, 0, RGB(250,0,0));

			dc.SelectObject( &Pluma );

			dc.MoveTo(Base.x,Base.y);
			dc.LineTo(Reference.x,Reference.y);

			dc.Rectangle(rect);

			dc.TextOut(Reference.x,Reference.y-Tsize.cy, Texto);
		}
	}
}

void CWinRf_ViewSketch2D::GDI_DrawPerimeterInSequence()
// Highlight the edges of the perimeter
{
	//bool drawEdges= false;
	bool drawVertices= true;


	if(NumPerimeterInSequence <0 ||
		NumPerimeterInSequence > (long)ReferApp.Cu.Pe.PerimeterEdges.size()-1)
		return;

	CClientDC dc(this);

	CPen Pluma;
	if (DisplayInBlack)
		Pluma.CreatePen(PS_SOLID, 6, RGB(0, 0, 0));
	else
		Pluma.CreatePen(PS_SOLID, 6, RGB(255, 150, 0));
	dc.SelectObject(&Pluma);

	//if(drawEdges){
	//	for (long ii=0; ii<(long)ReferApp.Cu.Pe.PerimeterEdges[NumPerimeterInSequence].size(); ii++){
	//		CEdge E= pDoc->DB_DrawingOff->GetEdge(ReferApp.Cu.Pe.PerimeterEdges[NumPerimeterInSequence][ii]);
	//		CVertex HeadVertex= pDoc->DB_DrawingOff->GetVertex(E.Head);
	//		CPoint P = Transf.GDI_WorldToViewport(HeadVertex.x, HeadVertex.y);
	//		dc.MoveTo(P.x, P.y);
	//		CVertex TailVertex= pDoc->DB_DrawingOff->GetVertex(E.Tail);
	//		P = Transf.GDI_WorldToViewport(TailVertex.x, TailVertex.y);
	//		dc.LineTo(P.x, P.y);
	//	}
	//}

	if(drawVertices && ReferApp.Cu.Pe.PerimeterVertices[NumPerimeterInSequence].size()>0){
		CVertex Vertex= pDoc->DB_DrawingOff->GetVertex(ReferApp.Cu.Pe.PerimeterVertices[NumPerimeterInSequence][0]);
		CPoint P = Transf.GDI_WorldToViewport(Vertex.x, Vertex.y);
		dc.MoveTo(P.x, P.y);

		for (long ii=1; ii<(long)ReferApp.Cu.Pe.PerimeterVertices[NumPerimeterInSequence].size(); ii++){
			long NumVertex= ReferApp.Cu.Pe.PerimeterVertices[NumPerimeterInSequence][ii];
			if(NumVertex < 0){
				Vertex.x= ReferApp.Cu.Pe.Tjunction[NumPerimeterInSequence][-NumVertex-1].x;
				Vertex.y= ReferApp.Cu.Pe.Tjunction[NumPerimeterInSequence][-NumVertex-1].y;
			}
			else
				Vertex= pDoc->DB_DrawingOff->GetVertex(NumVertex);

			P = Transf.GDI_WorldToViewport(Vertex.x, Vertex.y);
			dc.LineTo(P.x,P.y);
		}
	}
}


void CWinRf_ViewSketch2D::GDI_DrawParallelEdges()
// Draw the parallel edges
{
	CClientDC dc(this);

	for(long i= 0; i<(long)ReferApp.Cu.PE.ParallelEdges.size(); i++){
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 6, RGB(0, 0, 0));
		else{
			Pluma.CreatePen(PS_SOLID, 6, RGB(0, (i%3)*75+100, 0));
		}
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);
		long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);
		//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

		for (long ii=0; ii<(long)ReferApp.Cu.PE.ParallelEdges[i].size(); ii++){
			CEdge Arista= pDoc->DB_DrawingOff->GetEdge(ReferApp.Cu.PE.ParallelEdges[i][ii]);
			CVertex HeadVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Head);
			CVertex TailVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Tail);
			CPoint p = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
			dc.MoveTo(p.x, p.y);
			p = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
			dc.LineTo(p.x, p.y);
		}
	}
}


void CWinRf_ViewSketch2D::GDI_DrawPEInSequence()
// Displays the parallel edges group NumVPInSequence
{
	if(NumPEInSequence <0 ||
		NumPEInSequence > (long)ReferApp.Cu.PE.ParallelEdges.size()-1)
		return;

	CClientDC dc(this);

	CPen Pluma;
	if (DisplayInBlack)
		Pluma.CreatePen(PS_SOLID, 6, RGB(0, 0, 0));
	else{
		Pluma.CreatePen(PS_SOLID, 6, RGB(0, 255, 0));
	}
	dc.SelectObject(&Pluma);

	CRect rect;
	GetClientRect(&rect);
	long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	for (long ii=0; ii<(long)ReferApp.Cu.PE.ParallelEdges[NumPEInSequence].size(); ii++){
		CEdge Arista= pDoc->DB_DrawingOff->GetEdge(ReferApp.Cu.PE.ParallelEdges[NumPEInSequence][ii]);
		CVertex HeadVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Head);
		CVertex TailVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Tail);
		CPoint p = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
		dc.MoveTo(p.x, p.y);
		p = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
		dc.LineTo(p.x, p.y);
	}
}


void CWinRf_ViewSketch2D::GDI_DrawVanishingPoints()
// Draw the Vanishing Points and its associated information
{
	bool DrawMainVP= true;
	bool DrawCandidateVP= true;
	bool DrawSpokes= false;
	bool DrawFrameVP= true;
	bool DrawCentroid= true;
	bool DrawRing= true;

	CClientDC dc(this);

	CPoint Punto;

	if(DrawCandidateVP){
		//Loop to draw all tentative vanishing points
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 4, RGB(255, 125, 0));
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);
		long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);
		//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

		for (long ii=0; ii<(long)ReferApp.Cu.VP.CandidateVP.size(); ii++){
			CPoint p= Transf.GDI_WorldToViewport(ReferApp.Cu.VP.CandidateVP[ii].x,
												 ReferApp.Cu.VP.CandidateVP[ii].y);
			CRect R;
			long radius= 2;
			R.left  = p.x-radius; R.right = p.x+radius;
			R.bottom= p.y-radius; R.top   = p.y+radius;
			dc.Ellipse(R);

			//Labell the VP
			CString Text;
			Text.Format("%i", ii);
			dc.TextOut (p.x, p.y, Text);
		}
	}

	if(DrawSpokes){
		//Loop to draw spokes passing through all tentative vanishing points
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 0, RGB(255, 125, 0));
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);
		long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);
		//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

		for (long ii=0; ii<(long)ReferApp.Cu.VP.CandidateVP.size(); ii++){
			double xx= ReferApp.Cu.VP.CandidateVP[ii].x - ReferApp.Cu.VP.DrawingCentroid.x;
			double yy= ReferApp.Cu.VP.CandidateVP[ii].y - ReferApp.Cu.VP.DrawingCentroid.y;
			double angle= atan2(yy, xx);
			if(angle<0) angle+= 2*NUMBER_PI;

			double xx0= ReferApp.Cu.VP.InnerRingRadius * cos(angle) + ReferApp.Cu.VP.DrawingCentroid.x;
			double yy0= ReferApp.Cu.VP.InnerRingRadius * sin(angle) + ReferApp.Cu.VP.DrawingCentroid.y;
			CPoint P0= Transf.GDI_WorldToViewport(xx0, yy0);

			double xx1= ReferApp.Cu.VP.OuterRingRadius * cos(angle) + ReferApp.Cu.VP.DrawingCentroid.x;
			double yy1= ReferApp.Cu.VP.OuterRingRadius * sin(angle) + ReferApp.Cu.VP.DrawingCentroid.y;
			CPoint P1= Transf.GDI_WorldToViewport(xx1, yy1);
			dc.MoveTo(P0.x,P0.y);
			dc.LineTo(P1.x,P1.y);
		}
	}

	if(DrawMainVP){
		//Loop to draw all main vanishing points
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 6, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 6, RGB(0, 0, 255));
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);
		long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);
		//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

		for (long ii=0; ii<(long)ReferApp.Cu.VP.MeanVP.size(); ii++){
			CPoint Punto0= Transf.GDI_WorldToViewport(ReferApp.Cu.VP.MeanVP[ii].x,
													  ReferApp.Cu.VP.MeanVP[ii].y);
			CRect R;
			long radius= 4;
			R.left  = Punto0.x-radius; R.right = Punto0.x+radius;
			R.bottom= Punto0.y-radius; R.top   = Punto0.y+radius;
			dc.Ellipse(R);

			//Labell the VP
			CString Texto;
			Texto.Format("%i", ii);
			dc.TextOut (Punto0.x, Punto0.y, Texto);
		}
	}

	if(DrawFrameVP){
		//Draw the bounding box of the vanishing points
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		dc.SelectObject(&Pluma);

		double minVP_X= ReferApp.Cu.VP.CandidateVP[0].x;
		double maxVP_X= ReferApp.Cu.VP.CandidateVP[0].x;
		double minVP_Y= ReferApp.Cu.VP.CandidateVP[0].y;
		double maxVP_Y= ReferApp.Cu.VP.CandidateVP[0].y;

		for (long i=1; i<(long)ReferApp.Cu.VP.CandidateVP.size(); i++){
			minVP_X= min(minVP_X, ReferApp.Cu.VP.CandidateVP[i].x);
			maxVP_X= max(maxVP_X, ReferApp.Cu.VP.CandidateVP[i].x);
			minVP_Y= min(minVP_Y, ReferApp.Cu.VP.CandidateVP[i].y);
			maxVP_Y= max(maxVP_Y, ReferApp.Cu.VP.CandidateVP[i].y);
		}

		CPoint Punto;
		Punto = Transf.GDI_WorldToViewport(minVP_X, minVP_Y);
		dc.MoveTo(Punto.x,Punto.y);
		Punto = Transf.GDI_WorldToViewport(minVP_X, maxVP_Y);
		dc.LineTo(Punto.x,Punto.y);
		Punto = Transf.GDI_WorldToViewport(maxVP_X, maxVP_Y);
		dc.LineTo(Punto.x,Punto.y);
		Punto = Transf.GDI_WorldToViewport(maxVP_X, minVP_Y);
		dc.LineTo(Punto.x,Punto.y);
		Punto = Transf.GDI_WorldToViewport(minVP_X, minVP_Y);
		dc.LineTo(Punto.x,Punto.y);
	}

	if(DrawCentroid){
		//Draw the centroid of the line drawing
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 255));
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);
		long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);

		CPoint Punto0= Transf.GDI_WorldToViewport(ReferApp.Cu.VP.DrawingCentroid.x,
												  ReferApp.Cu.VP.DrawingCentroid.y);
		CRect R;
		long radius= 4;
		R.left  = Punto0.x-radius; R.right = Punto0.x+radius;
		R.bottom= Punto0.y-radius; R.top   = Punto0.y+radius;
		dc.Ellipse(R);

		//Labell the Centroid
		dc.TextOut (Punto0.x, Punto0.y, "Centroid");
	}

	//Draw the ring of vanishing points
	if(DrawRing){
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);
		long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);

		//Inner circle
		std::vector <POINT2D> E1(13);
		EllipseToBezier(ReferApp.Cu.VP.DrawingCentroid.x - ReferApp.Cu.VP.InnerRingRadius, //left
						ReferApp.Cu.VP.DrawingCentroid.x + ReferApp.Cu.VP.InnerRingRadius, //right
						ReferApp.Cu.VP.DrawingCentroid.y - ReferApp.Cu.VP.InnerRingRadius, //bottom
						ReferApp.Cu.VP.DrawingCentroid.y + ReferApp.Cu.VP.InnerRingRadius, //top
						E1);

		//Transform to viewport
		CPoint EE1[13];
		for(long i= 0; i<13; i++){
			EE1[i]= Transf.GDI_WorldToViewport(E1[i].x, E1[i].y);
		}

		//Draw the ellipticall bezier
		dc.BeginPath();
		dc.PolyBezier(EE1, 13);
		dc.EndPath();
		dc.StrokePath();

		//Outer circle
		EllipseToBezier(ReferApp.Cu.VP.DrawingCentroid.x - ReferApp.Cu.VP.OuterRingRadius, //left
						ReferApp.Cu.VP.DrawingCentroid.x + ReferApp.Cu.VP.OuterRingRadius, //right
						ReferApp.Cu.VP.DrawingCentroid.y - ReferApp.Cu.VP.OuterRingRadius, //bottom
						ReferApp.Cu.VP.DrawingCentroid.y + ReferApp.Cu.VP.OuterRingRadius, //top
						E1);

		//Transform to viewport
		for(long i= 0; i<13; i++){
			EE1[i]= Transf.GDI_WorldToViewport(E1[i].x, E1[i].y);
		}

		//Draw the ellipticall bezier
		dc.BeginPath();
		dc.PolyBezier(EE1, 13);
		dc.EndPath();
		dc.StrokePath();
	}
}


void CWinRf_ViewSketch2D::GDI_DrawVPInSequence()
// Displays the VP NumVPInSequence and the candidate VP forming its cluster
{
	if(NumVPInSequence <0 ||
		NumVPInSequence > (long)ReferApp.Cu.VP.MeanVP.size()-1)
		return;

	bool DrawCandidateVP= true;
	bool DrawMainVP= true;
	bool DrawBundleOfEdges= true;

	CClientDC dc(this);

	//Draw its cluster of candidate Vanishing points
	//----------------------------------------------
	if(DrawCandidateVP){
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 4, RGB(255, 125, 05));
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);
		long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);
		//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

		//Loop to draw all the tentative vanishing points in the cluster
		for (long ii=0; ii<(long)ReferApp.Cu.VP.ClusterOfVPs[NumVPInSequence].size(); ii++){
			CPoint p= Transf.GDI_WorldToViewport(ReferApp.Cu.VP.CandidateVP[ReferApp.Cu.VP.ClusterOfVPs[NumVPInSequence][ii]].x,
												 ReferApp.Cu.VP.CandidateVP[ReferApp.Cu.VP.ClusterOfVPs[NumVPInSequence][ii]].y);
			CRect R;
			long radius= 3;
			R.left  = p.x-radius; R.right = p.x+radius;
			R.bottom= p.y-radius; R.top   = p.y+radius;
			dc.Ellipse(R);

			////Labell the VP
			//CString Text;
			//Text.Format("%i", ReferApp.Cu.VP.ClusterOfVPs[NumVPInSequence][ii]);
			//dc.TextOut (Punto0.x, Punto0.y, Text);
		}
	}

	//Draw current main Vanishing point
	//---------------------------------
	if(DrawMainVP){
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		else
			Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 255));
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);
		long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);
		//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

		CPoint p= Transf.GDI_WorldToViewport(ReferApp.Cu.VP.MeanVP[NumVPInSequence].x,
											 ReferApp.Cu.VP.MeanVP[NumVPInSequence].y);
		CRect R;
		long radius= 4;
		R.left  = p.x-radius; R.right = p.x+radius;
		R.bottom= p.y-radius; R.top   = p.y+radius;
		dc.Ellipse(R);

		//Labell the VP
		CString Text;
		Text.Format("%i", NumVPInSequence);
		dc.TextOut (p.x, p.y, Text);
	}


	//Draw its bundle of edges
	//------------------------
	if(DrawBundleOfEdges){
		CPen Pluma;
		if (DisplayInBlack)
			Pluma.CreatePen(PS_SOLID, 6, RGB(0, 0, 0));
		else{
			if(ReferApp.Cu.VP.ParallelGroup[NumVPInSequence])
				Pluma.CreatePen(PS_SOLID, 6, RGB(0, 255, 0));
			else
				Pluma.CreatePen(PS_SOLID, 6, RGB(255, 0, 0));
		}
		dc.SelectObject(&Pluma);

		CRect rect;
		GetClientRect(&rect);  //Standard height is set as 1% of the client window height
		long height= rect.Height()/10;
		CFont Fuente;
		Fuente.CreatePointFont( height, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);
		//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

		//Loop to draw all the edges converging to the vanishing point
		for (long ii=0; ii<(long)ReferApp.Cu.VP.ParallelEdges[NumVPInSequence].size(); ii++){
			CEdge Arista= pDoc->DB_DrawingOff->GetEdge(ReferApp.Cu.VP.ParallelEdges[NumVPInSequence][ii]);
			CVertex HeadVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Head);
			CVertex TailVertex= pDoc->DB_DrawingOff->GetVertex(Arista.Tail);
			CPoint p = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
			dc.MoveTo(p.x, p.y);
			p = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
			dc.LineTo(p.x, p.y);
		}
	}
}


void CWinRf_ViewSketch2D::GDI_DrawMainAxes()
// Draws an icon with the three main axes stored in MainAngles
{
	if(ReferApp.Cu.MA.MainAngles.size() != 3)
		return;

	//Get the rectangle where the icon is located
	CRect rect;
	GetClientRect(&rect);

	//Locate the icon near the bottom/left corner
	CRect rectAxes;
	long fraction= 7;  // Size of the icon is (1/fraction) of the ClientRect
	rectAxes.top= rect.bottom-(rect.bottom-rect.top)/fraction;
	rectAxes.bottom= rect.bottom;
	rectAxes.left= rect.left;
	rectAxes.right= (rect.right-rect.left)/fraction;

	//Centre the origin in the rectangle
	CPoint Origin;
	Origin= rectAxes.CenterPoint();

	//Get proportional lengths
	long Length= rectAxes.Height()/3;
	long LengthPlus= (long)(1.2*Length);

	CClientDC dc(this);

	CFont Fuente;
	Fuente.CreatePointFont((int)(1.4*LengthPlus), "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	//Draw the X axis
	CPoint Punto;
	CPen PlumaX;
	PlumaX.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	dc.SelectObject(&PlumaX);
	dc.MoveTo(Origin.x,Origin.y);
	Punto.x= Origin.x + (long)(Length*cos(ReferApp.Cu.MA.MainAngles[0]));
	Punto.y= Origin.y - (long)(Length*sin(ReferApp.Cu.MA.MainAngles[0]));
	dc.LineTo(Punto.x,Punto.y);
	Punto.x= Origin.x + (long)(LengthPlus*cos(ReferApp.Cu.MA.MainAngles[0]));
	Punto.y= Origin.y - (long)(LengthPlus*sin(ReferApp.Cu.MA.MainAngles[0]));
	dc.TextOut(Punto.x, Punto.y, "X");

	//Draw the Y axis
	CPen PlumaY;
	PlumaY.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	dc.SelectObject(&PlumaY);
	dc.MoveTo(Origin.x,Origin.y);
	Punto.x= Origin.x + (long)(Length*cos(ReferApp.Cu.MA.MainAngles[1]));
	Punto.y= Origin.y - (long)(Length*sin(ReferApp.Cu.MA.MainAngles[1]));
	dc.LineTo(Punto.x,Punto.y);
	Punto.x= Origin.x + (long)(LengthPlus*cos(ReferApp.Cu.MA.MainAngles[1]));
	Punto.y= Origin.y - (long)(LengthPlus*sin(ReferApp.Cu.MA.MainAngles[1]));
	dc.TextOut(Punto.x, Punto.y, "Y");

	//Draw the Z axis
	CPen PlumaZ;
	PlumaZ.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	dc.SelectObject(&PlumaZ);
	dc.MoveTo(Origin.x,Origin.y);
	Punto.x= Origin.x + (long)(Length*cos(ReferApp.Cu.MA.MainAngles[2]));
	Punto.y= Origin.y - (long)(Length*sin(ReferApp.Cu.MA.MainAngles[2]));
	dc.LineTo(Punto.x,Punto.y);
	Punto.x= Origin.x + (long)(LengthPlus*cos(ReferApp.Cu.MA.MainAngles[2]));
	Punto.y= Origin.y - (long)(LengthPlus*sin(ReferApp.Cu.MA.MainAngles[2]));
	dc.TextOut(Punto.x, Punto.y, "Z");
}


void CWinRf_ViewSketch2D::GDI_DrawImplosionFaces()
// Draws "imploded" polygons to visualize the faces in the 2D model
{
	// Calculates the database containing the faces in implosion
	CDB_Model DB_Implosion;
	GetImplodedFaces(pDoc->DB_DrawingOff, &DB_Implosion);

	// Draws the imploded faces
	CClientDC dc(this);
	CPen Pluma;
	if (DisplayInBlack)
		Pluma.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	else
		Pluma.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
	dc.SelectObject(&Pluma);

	for (long i=0; i<DB_Implosion.GetSizeEdges(); i++ ){
		CEdge Arista= DB_Implosion.GetEdge(i);
		CVertex Vertice1= DB_Implosion.GetVertex(Arista.Head);
		CVertex Vertice2= DB_Implosion.GetVertex(Arista.Tail);

		CPoint Punto;
		Punto = Transf.GDI_WorldToViewport((double)Vertice1.x,(double)Vertice1.y);
		dc.MoveTo(Punto.x,Punto.y);
		Punto = Transf.GDI_WorldToViewport((double)Vertice2.x,(double)Vertice2.y);
		dc.LineTo(Punto.x,Punto.y);
	}
}


void CWinRf_ViewSketch2D::GDI_DrawFacesInSequence()
// Highlights the contour of face NumFaceInSequence
{
	if(NumFaceInSequence <0 ||
		NumFaceInSequence > (long)pDoc->DB_DrawingOff->GetSizeFaces()-1)
		return;

	std::vector<long> verticesFaceInSequence= 
		pDoc->DB_DrawingOff->GetAllVerticesInFace(pDoc->DB_DrawingOff->GetFace(NumFaceInSequence));
	long NumVertices= (long)verticesFaceInSequence.size();
	if(NumVertices == 0) return;

	CClientDC dc(this);
	CPen Pluma;
	if (DisplayInBlack)
		Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	else
		Pluma.CreatePen(PS_SOLID, 4, RGB(0, 255, 255));
	dc.SelectObject(&Pluma);

	//Draw the contour of the face
	CVertex Vertice= pDoc->DB_DrawingOff->GetVertex(verticesFaceInSequence[0]);

	CPoint Punto;
	Punto = Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
	dc.MoveTo(Punto.x,Punto.y);

	for(long j=1; j<NumVertices; j++){
		Vertice= pDoc->DB_DrawingOff->GetVertex(verticesFaceInSequence[j]);

		Punto = Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
		dc.LineTo(Punto.x,Punto.y);
	}
	Vertice= pDoc->DB_DrawingOff->GetVertex(verticesFaceInSequence[0]);
	Punto = Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
	dc.LineTo(Punto.x,Punto.y);

	//Display the number of the face
	//------------------------------
	CRect rect;
	GetClientRect(&rect);
	long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	//dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	CString Texto;

	CVertex Centroid= GetFaceCentroid(pDoc->DB_DrawingOff, NumFaceInSequence);
	Punto = Transf.GDI_WorldToViewport((double)Centroid.x, (double)Centroid.y);
	Texto.Format("%i", NumFaceInSequence);
	dc.TextOut (Punto.x, Punto.y, Texto);
}


void CWinRf_ViewSketch2D::GDI_DrawSymmetryAxesInFaces()
// Draws the skewed symmetry axes of the faces
{
	CClientDC dc(this);
	CPen Pluma;
	if (DisplayInBlack)
		Pluma.CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
	else
		Pluma.CreatePen(PS_DASHDOT, 1, RGB(0, 100, 255));
	dc.SelectObject(&Pluma);

	if (ReferApp.Cu.Sy.FaceAxesU.size() != 0){
		for (long i=0; i<(long)ReferApp.Cu.Sy.FaceAxesU.size(); i++){

			//Check if a particular face is selected
			if(!DisplayFacesInSequence || i==NumFaceInSequence){

				// Draw symmetry lines contained in the current face
				if(ReferApp.Cu.Sy.FaceAxesU[i].size() != 0){
					long Eje0, Eje1;
					CEdge Arista0;
					for(long j=0; j<(long)ReferApp.Cu.Sy.FaceAxesU[i].size(); j++){
						// Get the first vertex
						CVertex Vertex0;
						Eje0= ReferApp.Cu.Sy.FaceAxesU[i][j];
						if (Eje0>=0)
							// The vertex of the symmetry line is one vertex of the face
							Vertex0= pDoc->DB_DrawingOff->GetVertex(Eje0);
						else{
							// The vertex of the symmetry line is one midpoint
							Arista0= pDoc->DB_DrawingOff->GetEdge(std::abs(Eje0)-1);
							CVertex VerticeInicio= pDoc->DB_DrawingOff->GetVertex(Arista0.Tail);
							CVertex VerticeFinal= pDoc->DB_DrawingOff->GetVertex(Arista0.Head);
							Vertex0.x=(VerticeInicio.x +VerticeFinal.x)/2;
							Vertex0.y=(VerticeInicio.y +VerticeFinal.y)/2;
						}

						// Get the second vertex
						CVertex Vertex1;
						Eje1= ReferApp.Cu.Sy.FaceAxesV[i][j];
						if (Eje1>=0)
							// The vertex of the symmetry line is one vertex of the face
							Vertex1= pDoc->DB_DrawingOff->GetVertex(Eje1);
						else{
							// The vertex of the symmetry line is one midpoint
							Arista0= pDoc->DB_DrawingOff->GetEdge(std::abs(Eje1)-1);
							CVertex VerticeInicio= pDoc->DB_DrawingOff->GetVertex(Arista0.Tail);
							CVertex VerticeFinal= pDoc->DB_DrawingOff->GetVertex(Arista0.Head);
							Vertex1.x=(VerticeInicio.x +VerticeFinal.x)/2;
							Vertex1.y=(VerticeInicio.y +VerticeFinal.y)/2;
							}

						// Draw the line
						CPoint Punto;
						Punto = Transf.GDI_WorldToViewport((double)Vertex1.x,(double)Vertex1.y);
						dc.MoveTo(Punto.x, Punto.y);
						Punto = Transf.GDI_WorldToViewport((double)Vertex0.x,(double)Vertex0.y);
						dc.LineTo(Punto.x, Punto.y);
					}
				}
			}
		}
	}


}


void CWinRf_ViewSketch2D::GDI_DrawSymmetryPlanes()
//Draws the contour of the intersection between symmetry planes and the object
{
	CClientDC dc(this);
	CPen Pluma;
	if (DisplayInBlack)
		Pluma.CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
	else
		Pluma.CreatePen(PS_DASHDOT, 1, RGB(255, 0, 0));
	dc.SelectObject(&Pluma);

	//Loop over all the subgraphs
	for (long g=0; g<(long)ReferApp.Cu.Sy.SymPlanes.size(); g++){
		//Loop over all the symmetry planes of the current subgraph
		for (long i=0; i<(long)ReferApp.Cu.Sy.SymPlanes[g].size(); i++){
			//Loop over all the lines of the current symmetry plane
			for(long j=0; j<(long)ReferApp.Cu.Sy.SymPlanes[g][i].size()-1; j++){
				//Get the first vertex
				CVertex Vertex1;
				long Eje1= ReferApp.Cu.Sy.SymPlanes[g][i][j];
				if (Eje1>=0)
					//The vertex of the symmetry line is a vertex of the face
					Vertex1= pDoc->DB_DrawingOff->GetVertex(Eje1);
				else{
					//The vertex of the symmetry line is a midpoint
					CEdge Arista1= pDoc->DB_DrawingOff->GetEdge(std::abs(Eje1)-1);
					CVertex VerticeInicio= pDoc->DB_DrawingOff->GetVertex(Arista1.Tail);
					CVertex VerticeFinal= pDoc->DB_DrawingOff->GetVertex(Arista1.Head);
					Vertex1.x=(VerticeInicio.x +VerticeFinal.x)/2;
					Vertex1.y=(VerticeInicio.y +VerticeFinal.y)/2;
				}

				//Get the second vertex
				CVertex Vertex2;
				long Eje2= ReferApp.Cu.Sy.SymPlanes[g][i][j+1];
				if (Eje2>=0)
					//The vertex of the symmetry line is a vertex of the face
					Vertex2= pDoc->DB_DrawingOff->GetVertex(Eje2);
				else{
					//The vertex of the symmetry line is a midpoint
					CEdge Arista1= pDoc->DB_DrawingOff->GetEdge(std::abs(Eje2)-1);
					CVertex VerticeInicio= pDoc->DB_DrawingOff->GetVertex(Arista1.Tail);
					CVertex VerticeFinal= pDoc->DB_DrawingOff->GetVertex(Arista1.Head);
					Vertex2.x=(VerticeInicio.x +VerticeFinal.x)/2;
					Vertex2.y=(VerticeInicio.y +VerticeFinal.y)/2;
				}

				//Draw the line
				CPoint Punto;
				Punto = Transf.GDI_WorldToViewport((double)Vertex2.x,(double)Vertex2.y);
				dc.MoveTo(Punto.x, Punto.y);
				Punto = Transf.GDI_WorldToViewport((double)Vertex1.x,(double)Vertex1.y);
				dc.LineTo(Punto.x, Punto.y);
			}
		}
	}
}


void CWinRf_ViewSketch2D::GDI_DrawSymmetryPlanesInSequence()
//Draws the symmetry plane NumSymmetryPlaneInSequence
//belonging to the current subgraph (NumSubGraphInSequence)
{
	if(NumSubGraphInSequence <0 ||
		NumSubGraphInSequence > (long)ReferApp.Cu.Sy.SymPlanes.size()-1)
		return;
	if(NumSymmetryPlaneInSequence <0 ||
		NumSymmetryPlaneInSequence > (long)ReferApp.Cu.Sy.SymPlanes[NumSubGraphInSequence].size()-1)
		return;

	CClientDC dc(this);
	CPen Pluma;
	if (DisplayInBlack)
		Pluma.CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
	else
		Pluma.CreatePen(PS_DASHDOT, 4, RGB(255, 0, 0));
	dc.SelectObject(&Pluma);

	//Loop over all the lines of the symmetry plane
	std::vector<POINT2D> Polygon;

	//Get the first vertex
	long n= (long)ReferApp.Cu.Sy.SymPlanes[NumSubGraphInSequence][NumSymmetryPlaneInSequence].size();
	CVertex Vertex;
	long Side= ReferApp.Cu.Sy.SymPlanes[NumSubGraphInSequence][NumSymmetryPlaneInSequence][n-1];
	if (Side>=0)
		//The vertex of the symmetry line is a vertex of the face
		Vertex= pDoc->DB_DrawingOff->GetVertex(Side);
	else{
		//The vertex of the symmetry line is a midpoint
		CEdge Arista1= pDoc->DB_DrawingOff->GetEdge(-Side-1);
		CVertex VerticeInicio= pDoc->DB_DrawingOff->GetVertex(Arista1.Tail);
		CVertex VerticeFinal= pDoc->DB_DrawingOff->GetVertex(Arista1.Head);
		Vertex.x=(VerticeInicio.x +VerticeFinal.x)/2;
		Vertex.y=(VerticeInicio.y +VerticeFinal.y)/2;
	}
	CPoint Punto;
	Punto = Transf.GDI_WorldToViewport((double)Vertex.x,(double)Vertex.y);
	dc.MoveTo(Punto.x, Punto.y);
	POINT2D point(Punto.x, Punto.y);

	for(long j=0; j<n; j++){
		//Get the next vertex
		Vertex;
		Side= ReferApp.Cu.Sy.SymPlanes[NumSubGraphInSequence][NumSymmetryPlaneInSequence][j];
		if (Side>=0)
			//The vertex of the symmetry line is a vertex of the face
			Vertex= pDoc->DB_DrawingOff->GetVertex(Side);
		else{
			//The vertex of the symmetry line is a midpoint
			CEdge Arista1= pDoc->DB_DrawingOff->GetEdge(-Side-1);
			CVertex VerticeInicio= pDoc->DB_DrawingOff->GetVertex(Arista1.Tail);
			CVertex VerticeFinal= pDoc->DB_DrawingOff->GetVertex(Arista1.Head);
			Vertex.x=(VerticeInicio.x +VerticeFinal.x)/2;
			Vertex.y=(VerticeInicio.y +VerticeFinal.y)/2;
		}

		//Draw the line
		Punto = Transf.GDI_WorldToViewport((double)Vertex.x,(double)Vertex.y);
		dc.LineTo(Punto.x, Punto.y);

		point.x= Punto.x; point.y= Punto.y;
		Polygon.push_back(point);
	}

	//Display the merit of the symmetry plane
	//---------------------------------------
	CRect rect;
	GetClientRect(&rect);
	long height= rect.Height()/10;  //Standard height is set as 1% of the client window height
	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	CString Texto;

	Texto.Format("%3.2f", ReferApp.Cu.Sy.SymMerits[NumSubGraphInSequence][NumSymmetryPlaneInSequence]);
	POINT2D Centroid= GetCentroid(Polygon);
	dc.TextOut ((long)Centroid.x, (long)Centroid.y, Texto);
}


void CWinRf_ViewSketch2D::GDI_DrawSymmetrySubGraphsInSequence()
//Draws the sub-graph NumSubGraphInSequence
{
	if(NumSymmetrySubGraphInSequence <0 ||
		NumSymmetrySubGraphInSequence > (long)SymmetrySubGraphs.size()-1)
	return;

	CClientDC dc(this);
	CPen Pluma;
	if (DisplayInBlack)
		Pluma.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	else
		Pluma.CreatePen(PS_SOLID, 4, RGB(0, 150, 255));
	dc.SelectObject(&Pluma);

	for (long i=0; i<SymmetrySubGraphs[NumSymmetrySubGraphInSequence].GetSizeEdges(); i++){
		CEdge Arista= SymmetrySubGraphs[NumSymmetrySubGraphInSequence].GetEdge(i);

		CVertex HeadVertex= SymmetrySubGraphs[NumSymmetrySubGraphInSequence].GetVertex(Arista.Head);
		CVertex TailVertex= SymmetrySubGraphs[NumSymmetrySubGraphInSequence].GetVertex(Arista.Tail);
		CPoint Punto = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
		dc.MoveTo(Punto.x,Punto.y);
		Punto = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
		dc.LineTo(Punto.x,Punto.y);
	}
}


void CWinRf_ViewSketch2D::GDI_DrawNormalonLines(CDB_Model *DDBB)
//Draws the normalon lines
{
	int RR, GG, BB;

	//Loop over the lines
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CClientDC dc(this);
		CPen Pluma;

		//Add color labels to edges
		if (DisplayInBlack)
			{RR= 0; GG= 0; BB= 0;}
		else if (ReferApp.DIA.NormalonEdges[i] ==0)
			{RR= 255; GG= 0; BB= 0;}
		else if (ReferApp.DIA.NormalonEdges[i] ==1)
			{RR= 0; GG= 255; BB= 0;}
		else if (ReferApp.DIA.NormalonEdges[i] ==2)
			{RR= 0; GG= 0; BB= 255;}
		else
			{RR= 0; GG= 0; BB= 0;}

		//Display hidden edges
		if (DisplayHiddenEdges){
			switch (ReferApp.Cu.Hi.TypesOfEdges[i]){
				case VISIBLE	: Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));  break;
				case HIDDEN		: Pluma.CreatePen(PS_DASH, 1, RGB(RR, GG, BB));  break;
				case UNDEFINED	: Pluma.CreatePen(PS_DOT, 1, RGB(RR, GG, BB));  break;
			};
		}
		else{
			Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));
		}
		dc.SelectObject(&Pluma);

		//Draws a line linking the head and the tail of the stroke
		CEdge E= DDBB->GetEdge(i);
		CVertex HeadVertex= DDBB->GetVertex(E.Head);
		CVertex TailVertex= DDBB->GetVertex(E.Tail);
		CPoint P = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
		dc.MoveTo(P.x,P.y);
		P = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
		dc.LineTo(P.x,P.y);
	}
}


void CWinRf_ViewSketch2D::GDI_DrawSpanningTree()
//Draws the spanning trees used for axonometric or levels inflation

//Does not always draw the full spanning tree
//It only draws the first "NumBranches"

//Draws the origin as a circle
//Then, draws the first "NumBranches-1" branches
//Finally, the last one is drawn in a differet color
{
	CClientDC dc(this);

	//Set pen
	CPen P, *pPOld;
	if (DisplayInBlack)
		P.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	else
		P.CreatePen(PS_SOLID, 4, RGB(0, 255, 0));
	pPOld = dc.SelectObject(&P);

	//Draw the origin
	CVertex Vertice;
	if(SelectedTree==0)
		Vertice= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIA.AxonometricTreeVertices[0]);
	else
		Vertice= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIL.LevelsTreeVertices[0]);

	CPoint Punto0;
	Punto0= Transf.GDI_WorldToViewport(Vertice.x,Vertice.y);
	CRect R;
	long radius= 4;
	R.left  = Punto0.x-radius; R.right = Punto0.x+radius;
	R.bottom= Punto0.y-radius; R.top   = Punto0.y+radius;
	dc.Ellipse(R);

	//Draw previous branches
	for (long i= 0; i<2*(NumBranches-1); i+=2){
		CVertex Vertice1, Vertice2;
		if(SelectedTree==0){
			Vertice1= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIA.AxonometricTreeVertices[i]);
			Vertice2= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIA.AxonometricTreeVertices[i+1]);
		}
		else{
			Vertice1= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIL.LevelsTreeVertices[i]);
			Vertice2= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIL.LevelsTreeVertices[i+1]);
		}

		CPoint Punto;
		Punto = Transf.GDI_WorldToViewport((double)Vertice1.x,(double)Vertice1.y);
		dc.MoveTo(Punto.x,Punto.y);
		Punto = Transf.GDI_WorldToViewport((double)Vertice2.x,(double)Vertice2.y);
		dc.LineTo(Punto.x,Punto.y);
	}

	//Destroy the pen to free resources
	dc.SelectObject(pPOld);
	P.DeleteObject();

	//Draw current last branch
	if (DisplayInBlack)
		P.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	else
		P.CreatePen(PS_SOLID, 4, RGB(255 ,0, 0));
	pPOld = dc.SelectObject(&P);

	CVertex Vertice1, Vertice2;
	if(SelectedTree==0){
		Vertice1= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIA.AxonometricTreeVertices[2*NumBranches-2]);
		Vertice2= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIA.AxonometricTreeVertices[2*NumBranches-1]);
	}
	else{
		Vertice1= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIL.LevelsTreeVertices[2*NumBranches-2]);
		Vertice2= pDoc->DB_DrawingOff->GetVertex(ReferApp.DIL.LevelsTreeVertices[2*NumBranches-1]);
	}

	CPoint Punto;
	Punto = Transf.GDI_WorldToViewport((double)Vertice1.x,(double)Vertice1.y);
	dc.MoveTo(Punto.x,Punto.y);
	Punto = Transf.GDI_WorldToViewport((double)Vertice2.x,(double)Vertice2.y);
	dc.LineTo(Punto.x,Punto.y);

	//Destroy the pen to free resources
	dc.SelectObject(pPOld);
	P.DeleteObject();
}


void CWinRf_ViewSketch2D::GDI_DrawFeatures(long numFeatures,
										   std::vector<long> Faces,
										   std::vector<double> Merits,
										   std::vector<long> ContainerFaces,
										   CString label,
										   long RedH, long GreenH, long BlueH,
										   long RedL, long GreenL, long BlueL)
// Draws polygons to visualize the faces belonging to different types of features in the 2D model

//	- numFeatures is the number of feartures of the current type
//	- Faces is a list of faces labelled as 0 if they do not belong to the current type of feature,
//	  and labbelled +-i if they belong to the ith feature of the current type.
//	- Merit is a list of the figures of merit of the different features of the current type
//	- Label is a descriptor of the tuype of the current feature.
{
	if(numFeatures == 0)
		//There are none features of the current type
		return;

	CClientDC dc(this);

	//Loop over the set of features of the current type (see "label")
	for(long i=0; i<numFeatures; i++){
		POINT2D Centroid(0, 0);
		long numFacesI= 0;

		//Loop over the faces of the current feature
		for(long j=0; j<(long)Faces.size(); j++){
			if(std::abs(Faces[j]) == i+1){  //Face j belongs to feature i
				CPen Pluma;
				if (DisplayInBlack)
					Pluma.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				else{
					if(Merits[i] > 0.5)
						Pluma.CreatePen(PS_SOLID, 5, RGB(RedH, GreenH, BlueH));  //Bright color for highly meritorious feature
					else
						Pluma.CreatePen(PS_SOLID, 5, RGB(RedL, GreenL, BlueL)); //Dark color for scarcely meritorious feature
				}
				dc.SelectObject(&Pluma);

				//Draw the face
				std::vector<long> verticesJFace= 
					pDoc->DB_DrawingOff->GetAllVerticesInFace(pDoc->DB_DrawingOff->GetFace(j));
				CVertex Vertice= pDoc->DB_DrawingOff->GetVertex(verticesJFace[0]);

				CPoint Punto= Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
				dc.MoveTo(Punto.x,Punto.y);

				for(long k=1; k<(long)verticesJFace.size(); k++){
					Vertice= pDoc->DB_DrawingOff->GetVertex(verticesJFace[k]);

					Punto = Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
					dc.LineTo(Punto.x,Punto.y);
				}
				Vertice= pDoc->DB_DrawingOff->GetVertex(verticesJFace[0]);
				Punto = Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
				dc.LineTo(Punto.x,Punto.y);

				CVertex CentroidJ= GetFaceCentroid(pDoc->DB_DrawingOff, j);
				Centroid.x+= CentroidJ.x;
				Centroid.y+= CentroidJ.y;
				numFacesI ++;
			}
		}

		Centroid.x /= numFacesI;
		Centroid.y /= numFacesI;

		if(DisplayContainerFaces){
			//Display the container faces of the current feature
			//--------------------------------------------------
			for(long j=0; j<(long)ContainerFaces.size(); j++){
				CPen Pluma;
				if (DisplayInBlack)
					Pluma.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				else{
					if(Merits[i] > 0.5)
						Pluma.CreatePen(PS_SOLID, 2, RGB(RedH, GreenH, BlueH));  //Bright color for highly meritorious feature
					else
						Pluma.CreatePen(PS_SOLID, 2, RGB(RedL, GreenL, BlueL)); //Dark color for scarcely meritorious feature
				}
				dc.SelectObject(&Pluma);

				//Draw the face
				std::vector<long> verticesJFace= 
					pDoc->DB_DrawingOff->GetAllVerticesInFace(pDoc->DB_DrawingOff->GetFace(ContainerFaces[j]));
				CVertex Vertice= pDoc->DB_DrawingOff->GetVertex(verticesJFace[0]);

				CPoint Punto= Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
				dc.MoveTo(Punto.x,Punto.y);

				for(long k=1; k<(long)verticesJFace.size(); k++){
					Vertice= pDoc->DB_DrawingOff->GetVertex(verticesJFace[k]);

					Punto = Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
					dc.LineTo(Punto.x,Punto.y);
				}
				Vertice= pDoc->DB_DrawingOff->GetVertex(verticesJFace[0]);
				Punto = Transf.GDI_WorldToViewport(Vertice.x, Vertice.y);
				dc.LineTo(Punto.x,Punto.y);
			}
		}

		//Display the label of the feature
		//-------------------------------
		CRect rect;
		GetClientRect(&rect);
		long Length= (long)(rect.Height()*ScaleFeatureLabels/10);
		CFont Fuente;
		Fuente.CreatePointFont( Length, "Arial", &dc);
		dc.SelectObject(&Fuente);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextAlign(TA_CENTER | TA_BASELINE);

		//Label
		CString Texto, Texto1;
		if(DisplayNamesOfFeatures){
			Texto= label;
			Texto1.Format("%i", i);
			Texto+=Texto1;
		}

		//Add merit to the label
		if(DisplayMeritsOfFeatures){
			Texto+= " (";
			Texto1.Format("%4.3f", Merits[i]);
			Texto+=Texto1;
			Texto+= ")";
		}

		//Display label
		CPoint Punto = Transf.GDI_WorldToViewport((double)Centroid.x,(double)Centroid.y);
		dc.TextOut (Punto.x, Punto.y, Texto);
	}
}


void CWinRf_ViewSketch2D::GDI_DrawRibsAndDividers()
// Draws polygons to visualize the faces belonging to ribs and dividers in the 2D model
{
	if(ReferApp.Fe.RD.NumRibs > 0){
		std::vector<long> ContainerFaces;
		if(DisplayContainerFaces){
			for(long i= 0; i< (long)ReferApp.Fe.RD.ContainerRibFaces.size(); i++){
				for(long j= 0; j< (long)ReferApp.Fe.RD.ContainerRibFaces[i].size(); j++){
					ContainerFaces.push_back(ReferApp.Fe.RD.ContainerRibFaces[i][j]);
				}
			}
		}

		GDI_DrawFeatures(ReferApp.Fe.RD.NumRibs,
						 ReferApp.Fe.RD.Ribs,
						 ReferApp.Fe.RD.MeritRibs,
						 //ReferApp.Fe.RD.ContainerRibFaces[0],
						 ContainerFaces,
						 "Rib ",
						 0, 255, 0,  //Bright green for highly meritorious ribs
						 0, 125, 0); //Dark green for scarcely meritorious ribs
	}

	if(ReferApp.Fe.RD.NumDividers > 0){
		std::vector<long> ContainerFaces;
		if(DisplayContainerFaces){
			for(long i= 0; i< (long)ReferApp.Fe.RD.ContainerDividerFaces.size(); i++){
				for(long j= 0; j< (long)ReferApp.Fe.RD.ContainerDividerFaces[i].size(); j++){
					ContainerFaces.push_back(ReferApp.Fe.RD.ContainerDividerFaces[i][j]);
				}
			}
		}

		GDI_DrawFeatures(ReferApp.Fe.RD.NumDividers,
						 ReferApp.Fe.RD.Dividers,
						 ReferApp.Fe.RD.MeritDividers,
						 //ReferApp.Fe.RD.ContainerDividerFaces[0],
						 ContainerFaces,
						 "Divider ",
						 255, 0, 0,  //Bright red for highly meritorious dividers
						 125, 0, 0); //Dark red for scarcely meritorious dividers
	}
}


void CWinRf_ViewSketch2D::GDI_DrawRailsAndSlots()
// Draws polygons to visualize the faces belonging to the rails and slots in the 2D model
{
	if(ReferApp.Fe.RS.NumRails > 0){
		GDI_DrawFeatures(ReferApp.Fe.RS.NumRails,
						 ReferApp.Fe.RS.Rails,
						 ReferApp.Fe.RS.MeritRails,
						 ReferApp.Fe.RS.ContainerRailFaces,
						 "Rail ",
						 0, 150, 255,  //Bright pale blue for highly meritorious rails
						 0, 75, 125); //Dark pale blue for scarcely meritorious rails
	}

	if(ReferApp.Fe.RS.NumSlots > 0){
		GDI_DrawFeatures(ReferApp.Fe.RS.NumSlots,
						 ReferApp.Fe.RS.Slots,
						 ReferApp.Fe.RS.MeritSlots,
						 ReferApp.Fe.RS.ContainerSlotFaces,
						 "Slot ",
						 255, 0, 255,  //Bright pink for highly meritorious slots
						 125, 0, 125); //Dark pink for scarcely meritorious slots
	}

//	//Raquel: ahora el algoritmo diferencia entre Slot y Rail pasantes y ciegos
	//if(ReferApp.Fe.RS.MeritRails[i] > 0.6){
	//	Texto= "Rail/Slot ";
	//}
	//else{
	//	Texto= "Blind Rail/Slot";
	//}
}


void CWinRf_ViewSketch2D::GDI_DrawStepsAndPockets()
// Visualizes the faces belonging to the steps and pockets in the 2D model
{
	if(ReferApp.Fe.SP.NumSteps > 0){
		GDI_DrawFeatures(ReferApp.Fe.SP.NumSteps,
						 ReferApp.Fe.SP.Steps,
						 ReferApp.Fe.SP.MeritSteps,
						 ReferApp.Fe.SP.ContainerStepFaces,
						 "Step ",
						 0, 0, 255,  //Bright blue for highly meritorious steps
						 0, 0, 125); //Dark blue for scarcely meritorious steps
	}

	if(ReferApp.Fe.SP.NumPockets > 0){
		GDI_DrawFeatures(ReferApp.Fe.SP.NumPockets,
						 ReferApp.Fe.SP.Pockets,
						 ReferApp.Fe.SP.MeritPockets,
						 ReferApp.Fe.SP.ContainerPocketFaces,
						 "Pocket ",
						 125, 0, 255,  //Bright purple for highly meritorious pockets
						 75, 0, 125); //Dark purple for scarcely meritorious pockets
	}
}


void CWinRf_ViewSketch2D::GDI_DrawChamfersAndRounds()    //TBD ChamfersAndRounds
// Visualizes the faces belonging to the chamfers and rounds in the 2D model
{
}


void CWinRf_ViewSketch2D::GDI_DrawNodesAndVertices(CDB_Sketch *DB_Sketch)
// Draws the nodes and vertices of the strokes of a 2D sketch

//Works in two modes:
//		SelectedNodes= 0 draws the original nodes
//		SelectedNodes= 1 draws the resampled nodes

//If Display_Corners flag is true, highlights the corners of the stroke
//different from the first and the last
{
	CPen Pluma;
	CClientDC dc(this);

	//Loop over the strokes
	for (long istroke= 0; istroke<(long)DB_Sketch->GetNumStrokes(); istroke++){
		CStroke StrokeI= DB_Sketch->GetStroke(istroke);

		//Select original or resampled nodes
		std::vector <POINT2D> NewStroke;
		std::vector <POINT2D> NewStroke1;
		if(SelectedNodes == 0){
			for (long ipoint=0; ipoint<StrokeI.GetNumPoints(); ipoint++){
				CStrokePoint PointI= StrokeI.GetPoint(ipoint);
				NewStroke.push_back(PointI.GetPosition());
			}
		}
		else{
			std::vector <POINT2D> Nodes;
			std::vector <long> MappedNodes;  //pcompany
			std::vector <double> Times;
			double TotalTime= 0;
			for (long ipoint=0; ipoint<StrokeI.GetNumPoints(); ipoint++){
				CStrokePoint I= StrokeI.GetPoint(ipoint);
				Nodes.push_back(I.GetPosition());
				double time= I.GetTime();
				Times.push_back(time);
				TotalTime += time;
			}

			CResampleStroke RS;

			switch (ReferApp.Ve.StrokeCornersApproach){
				case 0:{  //Automatic
					if(TotalTime > 0){  //IStraw if time information is available
						std::vector <double> ResampledTimes;
						double meanTime;
						NewStroke= RS.GetResampledStroke(0, Nodes, MappedNodes, true, Times, ResampledTimes, meanTime);
						NewStroke1= RS.GetResampledStroke(1, Nodes, MappedNodes, true, Times, ResampledTimes, meanTime);
					}
					else{  //Sliding Strips otherwise
						NewStroke= RS.GetResampledStroke(Nodes, MappedNodes);
					}
					break;
				}

				case 1:{  //IStraw (or ShortStraw if no time available)
					std::vector <double> ResampledTimes;
					double meanTime;
					NewStroke= RS.GetResampledStroke(0, Nodes, MappedNodes, true, Times, ResampledTimes, meanTime);
					NewStroke1= RS.GetResampledStroke(1, Nodes, MappedNodes, true, Times, ResampledTimes, meanTime);
					break;
				}

				case 2:{  //Silding Strips
					NewStroke= RS.GetResampledStroke(Nodes, MappedNodes);
					break;
				}
			}
		}

		//Loop over all the nodes
		long numP= (long)NewStroke.size();
		if (numP>0){
			//Display nodes
			CBrush BrushPoint;
			if (DisplayInBlack){	//Select color
				Pluma.CreatePen( PS_SOLID, 2, RGB(0, 0, 0));
				BrushPoint.CreateSolidBrush(RGB(0,0,0));
			}
			else{
				Pluma.CreatePen( PS_SOLID, 2, RGB(200,200,100));
				BrushPoint.CreateSolidBrush(RGB(200,200,100));
			}
			dc.SelectObject(&Pluma);
			dc.SelectObject(&BrushPoint);

			for (long ipoint=1; ipoint<numP-1; ipoint++){
				CPoint Punto = Transf.GDI_WorldToViewport(NewStroke[ipoint].x, NewStroke[ipoint].y);
				DisplayDot(&dc, Punto, 3);
			}

			Pluma.DeleteObject();
			BrushPoint.DeleteObject();

			//Second resampled nodes
			if(NewStroke1.size() > 0) {
				CBrush BrushPoint;
				if (DisplayInBlack){	//Select color
					Pluma.CreatePen( PS_SOLID, 2, RGB(0, 0, 0));
					BrushPoint.CreateSolidBrush(RGB(0,0,0));
				}
				else{
					Pluma.CreatePen( PS_SOLID, 1, RGB(200,200,100));
					BrushPoint.CreateSolidBrush(RGB(250,250,150));
				}
				dc.SelectObject(&Pluma);
				dc.SelectObject(&BrushPoint);

				for (long ipoint=1; ipoint<numP-1; ipoint++){
					CPoint Punto = Transf.GDI_WorldToViewport(NewStroke1[ipoint].x, NewStroke1[ipoint].y);
					DisplayDot(&dc, Punto, 3);
				}

				Pluma.DeleteObject();
				BrushPoint.DeleteObject();
			}

			//Display vertices
			if (DisplayInBlack){	//Select color for the initial vertex
				Pluma.CreatePen( PS_SOLID, 3, RGB(0, 0, 0));
				BrushPoint.CreateSolidBrush(RGB(0,0,0));
			}
			else if(DisplayWorkingSketch){
				Pluma.CreatePen( PS_SOLID, 3, RGB(0,255,0));    //Green color
				BrushPoint.CreateSolidBrush(RGB(0,255,0));
			}
			dc.SelectObject( &Pluma );
			dc.SelectObject(&BrushPoint);

			CPoint Punto= Transf.GDI_WorldToViewport(NewStroke[0].x, NewStroke[0].y);
			DisplayDot(&dc, Punto, 4);

			if (DisplayWorkingSketch && !DisplayInBlack){	//Select a different color for the final vertex
				Pluma.DeleteObject();
				BrushPoint.DeleteObject();
				Pluma.CreatePen( PS_SOLID, 3, RGB(255,0,0));    //Red color
				BrushPoint.CreateSolidBrush(RGB(255,0,0));
				dc.SelectObject( &Pluma );
				dc.SelectObject(&BrushPoint);
			}

			Punto = Transf.GDI_WorldToViewport(NewStroke[numP-1].x, NewStroke[numP-1].y);
			DisplayDot(&dc, Punto, 4);

			Pluma.DeleteObject();
			BrushPoint.DeleteObject();
		}

		if(DisplayWorkingSketch && Display_Corners){
			// Detect corners
			if (StrokeI.NumCorners() > 0){
			// Display internal corners
				CBrush BrushPoint;
				if (DisplayInBlack){	//Select color
					Pluma.CreatePen( PS_SOLID, 3, RGB(0, 0, 0));
					BrushPoint.CreateSolidBrush(RGB(0,0,0));
				}
				else{
					Pluma.CreatePen( PS_SOLID, 3, RGB(125,125,0));
					BrushPoint.CreateSolidBrush(RGB(125,125,0));
				}
				dc.SelectObject( &Pluma );
				dc.SelectObject(&BrushPoint);

				for (long i= 1; i <= StrokeI.NumCorners(); i++){
					CStrokePoint I= StrokeI.GetPoint(StrokeI.StrokeCorners[i]);
					POINT2D P= I.GetPosition();

					CPoint Punto = Transf.GDI_WorldToViewport(P.x, P.y);
					DisplayDot(&dc, Punto, 4);
				}

				Pluma.DeleteObject();
				BrushPoint.DeleteObject();
			}
			//else{
			//	AfxMessageBox("Corners not calculated (Configure display of strokes to deselect the option to display corners)");
			//}
		}
	}
}


void CWinRf_ViewSketch2D::GetRectangularStrip(double A, double B, double sinAlpha, double cosAlpha,
											  double CentroidX, double CentroidY,
											  CPoint* P)
//Get viewpoint coordinates of a rectangular strip A x B, rotated Alpha,
//and centered at (CentroidX, CentroidY)
{
	P[0]= Transf.GDI_WorldToViewport((-A*cosAlpha + B*sinAlpha) + CentroidX,
									 (-A*sinAlpha - B*cosAlpha) + CentroidY);
	P[1]= Transf.GDI_WorldToViewport((+A*cosAlpha + B*sinAlpha) + CentroidX,
									 (+A*sinAlpha - B*cosAlpha) + CentroidY);
	P[2]= Transf.GDI_WorldToViewport((+A*cosAlpha - B*sinAlpha) + CentroidX,
									 (+A*sinAlpha + B*cosAlpha) + CentroidY);
	P[3]= Transf.GDI_WorldToViewport((-A*cosAlpha - B*sinAlpha) + CentroidX,
									 (-A*sinAlpha + B*cosAlpha) + CentroidY);
	P[4]= P[0];
}


void CWinRf_ViewSketch2D::GDI_DrawSlidingStrips(CDB_Sketch *DB_Sketch)
//Draw Sliding Strips linked to the Stroke
{
	CClientDC dc(this);
	CPen Pluma;
	if (DisplayInBlack){	//Select color
		Pluma.CreatePen( PS_SOLID, 2, RGB(0, 0, 0));
	}
	else{
		Pluma.CreatePen( PS_SOLID, 2, RGB(100,0,0));
	}
	dc.SelectObject(&Pluma);

	//Loop over the strokes
	for (long istroke= 0; istroke<(long)DB_Sketch->GetNumStrokes(); istroke++){
		CStroke StrokeI= DB_Sketch->GetStroke(istroke);

		//Select original or resampled nodes
		std::vector <POINT2D> Nodes;
		for (long ipoint=0; ipoint<StrokeI.GetNumPoints(); ipoint++){
			CStrokePoint I= StrokeI.GetPoint(ipoint);
			Nodes.push_back(I.GetPosition());
		}

		std::vector <POINT2D> NewStroke;
		if(SelectedNodes ==1){
			std::vector <long> MappedNodes;  //pcompany
			CResampleStroke RS;
			NewStroke= RS.GetResampledStroke(Nodes, MappedNodes);  //Returns ResampledStroke
		}
		else{
			for (long ipoint=0; ipoint<StrokeI.GetNumPoints(); ipoint++){
				CStrokePoint PointI= StrokeI.GetPoint(ipoint);
				NewStroke.push_back(PointI.GetPosition());
			}
		}

		//Loop over all the points of the stroke
		Strips CF;
		for (long ipoint=0; ipoint<(long)NewStroke.size(); ipoint++){
			//Get sliding strip parameters
			double A, B, Angle;
			CF.GetStrip(NewStroke, 1, ipoint, A, B, Angle);
			double sinA= sin(Angle);
			double cosA= cos(Angle);

			//Get the elliptical strip
			std::vector <POINT2D> E1(13);
			EllipseToBezier(-A, A, -B, B, E1);

			//Rotate, translate and transform to viewport
			CPoint EE1[13];
			for(long i= 0; i<13; i++){
				EE1[i]= Transf.GDI_WorldToViewport(( E1[i].x*cosA - E1[i].y*sinA) + NewStroke[ipoint].x,
												   ( E1[i].x*sinA + E1[i].y*cosA) + NewStroke[ipoint].y);
			}

			//Draw the ellipticall strip
			dc.BeginPath();
			dc.PolyBezier(EE1, 13);
			dc.EndPath();
			dc.StrokePath();

			//Get the elliptical strip
			std::vector <POINT2D> E2(13);
			EllipseToBezier(-3*A/4, 3*A/4, -B, B, E2);

			//Rotate, translate and transform to viewport
			CPoint EE2[13];
			for(long i= 0; i<13; i++){
				EE2[i]= Transf.GDI_WorldToViewport(( E2[i].x*cosA - E2[i].y*sinA) + NewStroke[ipoint].x,
												   ( E2[i].x*sinA + E2[i].y*cosA) + NewStroke[ipoint].y);
			}

			//Draw the ellipticall strip
			dc.BeginPath();
			dc.PolyBezier(EE2, 13);
			dc.EndPath();
			dc.StrokePath();

			//Draw the rectangular strip
			CPoint P[5];
			GetRectangularStrip(A/2, B/2, sinA, cosA, NewStroke[ipoint].x, NewStroke[ipoint].y, P);
			dc.Polyline(P, 5);
		}
	}
}


void CWinRf_ViewSketch2D::GDI_DrawToleranceBands(CDB_Sketch *DB_Sketch)
//Draw Tolerance Bands linked to the metits of the fits of the strokes' vectorization
{
	CClientDC dc(this);
	CPen PlumaToleranceIn;
	PlumaToleranceIn.CreatePen(PS_DASH, 1, RGB(200, 125, 0));
	CPen PlumaToleranceOut;
	PlumaToleranceOut.CreatePen(PS_SOLID, 1, RGB(200, 125, 0));

	CPen PlumaEndPoints;
	PlumaEndPoints.CreatePen( PS_SOLID, 2, RGB(255, 0, 0));

	CPen PlumaCentre;
	PlumaCentre.CreatePen( PS_SOLID, 2, RGB(0, 0, 255));

	CPen PlumaWorst;
	PlumaWorst.CreatePen( PS_SOLID, 2, RGB(0, 155, 255));

	if(Display_Line_Tolerance){
		//Loop over the strokes
		for (long i= 0; i<(long)DB_Sketch->GetNumStrokes(); i++){
			CStroke StrokeI= DB_Sketch->GetStroke(i);

			if(StrokeI.MeritLine >= StrokeI.MeritArc){
				//Draws tolerance bands
				double mod= MODULE_2D(StrokeI.Segment.TipBegin, StrokeI.Segment.TipEnd);
				if(mod != 0.){
					POINT2D Centroid((StrokeI.Segment.TipBegin.x+StrokeI.Segment.TipEnd.x)/2. ,
									 (StrokeI.Segment.TipBegin.y+StrokeI.Segment.TipEnd.y)/2.);
					double slope180= StrokeI.Segment.Slope180;  //Slope of the segment that fits the stroke
					CStroke RotatedStroke= StrokeI;
					RotatedStroke.Rotate(Centroid, -slope180);
					RECTANGLE2D bounds= RotatedStroke.GetBounds();
					double Tol= bounds.top-bounds.bottom;

					//Minimum tolerance band
					double delta_t_min= (ReferApp.Ve.LineTolMin*mod - Tol)/2.;
					dc.SelectObject(&PlumaToleranceIn);

					POINT2D T1 (bounds.left, bounds.top + delta_t_min);
					T1= T1.Rotate(Centroid, slope180);
					CPoint Punto = Transf.GDI_WorldToViewport(T1.x, T1.y);
					dc.MoveTo(Punto.x, Punto.y);

					POINT2D H1 (bounds.right, bounds.top + delta_t_min);
					H1= H1.Rotate(Centroid, slope180);
					Punto = Transf.GDI_WorldToViewport(H1.x, H1.y);
					dc.LineTo(Punto.x, Punto.y);

					POINT2D T2 (bounds.left, bounds.bottom - delta_t_min);
					T2= T2.Rotate(Centroid, slope180);
					Punto = Transf.GDI_WorldToViewport(T2.x, T2.y);
					dc.MoveTo(Punto.x, Punto.y);

					POINT2D H2 (bounds.right, bounds.bottom - delta_t_min);
					H2= H2.Rotate(Centroid, slope180);
					Punto = Transf.GDI_WorldToViewport(H2.x, H2.y);
					dc.LineTo(Punto.x, Punto.y);

					//Maximum tolerance band
					double delta_t_max= (ReferApp.Ve.LineTolMax*mod - Tol)/2.;
					dc.SelectObject(&PlumaToleranceOut);

					POINT2D TT1 (bounds.left, bounds.top + delta_t_max);
					TT1= TT1.Rotate(Centroid, slope180);
					Punto = Transf.GDI_WorldToViewport(TT1.x, TT1.y);
					dc.MoveTo(Punto.x, Punto.y);

					POINT2D HH1 (bounds.right, bounds.top + delta_t_max);
					HH1= HH1.Rotate(Centroid, slope180);
					Punto = Transf.GDI_WorldToViewport(HH1.x, HH1.y);
					dc.LineTo(Punto.x, Punto.y);

					POINT2D TT2 (bounds.left, bounds.bottom - delta_t_max);
					TT2= TT2.Rotate(Centroid, slope180);
					Punto = Transf.GDI_WorldToViewport(TT2.x, TT2.y);
					dc.MoveTo(Punto.x, Punto.y);

					POINT2D HH2 (bounds.right, bounds.bottom - delta_t_max);
					HH2= HH2.Rotate(Centroid, slope180);
					Punto = Transf.GDI_WorldToViewport(HH2.x, HH2.y);
					dc.LineTo(Punto.x, Punto.y);
				}
			}
		}
	}

	if(Display_Arc_Tolerance){
		//Loop over the strokes
		for (long i= 0; i<(long)DB_Sketch->GetNumStrokes(); i++){
			CStroke StrokeI= DB_Sketch->GetStroke(i);

			if(StrokeI.MeritArc > StrokeI.MeritLine || StrokeI.MeritLine == 0){
				//Draw tolerance bands
				POINT2D Centre (StrokeI.Arc.Centre_x, StrokeI.Arc.Centre_y);

				std::vector <POINT2D> E;
				double sinA= sin(StrokeI.Arc.Rotation);
				double cosA= cos(StrokeI.Arc.Rotation);
				long SizeEE;
				CPoint EE[13];

				dc.SelectObject(&PlumaToleranceIn);

				double T= (ReferApp.Ve.ArcTolMin * StrokeI.GetLength(10));
				double deltaT= (T - (StrokeI.t_Out+StrokeI.t_In)) / 2.;
				double d= StrokeI.t_Out + deltaT;

				EllipseArcToBezier(StrokeI.Arc.Radius_a+d, StrokeI.Arc.Radius_b+d,
								   StrokeI.Arc.StartAngle-StrokeI.Arc.Rotation, StrokeI.Arc.EndAngle-StrokeI.Arc.Rotation, E);
				SizeEE= (long)E.size();
				for(long i= 0; i<SizeEE; i++){
					EE[i]= Transf.GDI_WorldToViewport((E[i].x*cosA - E[i].y*sinA) + StrokeI.Arc.Centre_x,
													  (E[i].x*sinA + E[i].y*cosA) + StrokeI.Arc.Centre_y);
				}
				dc.BeginPath();
				dc.PolyBezier(EE, SizeEE);
				dc.EndPath();
				dc.StrokePath();

				d= StrokeI.t_In + deltaT;
				EllipseArcToBezier(StrokeI.Arc.Radius_a-d, StrokeI.Arc.Radius_b-d,
								   StrokeI.Arc.StartAngle-StrokeI.Arc.Rotation, StrokeI.Arc.EndAngle-StrokeI.Arc.Rotation, E);
				SizeEE= (long)E.size();
				for(long i= 0; i<SizeEE; i++){
					EE[i]= Transf.GDI_WorldToViewport((E[i].x*cosA - E[i].y*sinA) + StrokeI.Arc.Centre_x,
													  (E[i].x*sinA + E[i].y*cosA) + StrokeI.Arc.Centre_y);
				}
				dc.BeginPath();
				dc.PolyBezier(EE, SizeEE);
				dc.EndPath();
				dc.StrokePath();

				dc.SelectObject(&PlumaToleranceOut);
				T= (ReferApp.Ve.ArcTolMax * StrokeI.GetLength(10));
				deltaT= (T - (StrokeI.t_Out+StrokeI.t_In)) / 2.;
				d= StrokeI.t_Out + deltaT;

				EllipseArcToBezier(StrokeI.Arc.Radius_a+d, StrokeI.Arc.Radius_b+d,
								   StrokeI.Arc.StartAngle-StrokeI.Arc.Rotation, StrokeI.Arc.EndAngle-StrokeI.Arc.Rotation, E);
				SizeEE= (long)E.size();
				for(long i= 0; i<SizeEE; i++){
					EE[i]= Transf.GDI_WorldToViewport((E[i].x*cosA - E[i].y*sinA) + StrokeI.Arc.Centre_x,
													  (E[i].x*sinA + E[i].y*cosA) + StrokeI.Arc.Centre_y);
				}
				dc.BeginPath();
				dc.PolyBezier(EE, SizeEE);
				dc.EndPath();
				dc.StrokePath();

				d= StrokeI.t_In + deltaT;
				EllipseArcToBezier(StrokeI.Arc.Radius_a-d, StrokeI.Arc.Radius_b-d,
								   StrokeI.Arc.StartAngle-StrokeI.Arc.Rotation, StrokeI.Arc.EndAngle-StrokeI.Arc.Rotation, E);
				SizeEE= (long)E.size();
				for(long i= 0; i<SizeEE; i++){
					EE[i]= Transf.GDI_WorldToViewport((E[i].x*cosA - E[i].y*sinA) + StrokeI.Arc.Centre_x,
													  (E[i].x*sinA + E[i].y*cosA) + StrokeI.Arc.Centre_y);
				}
				dc.BeginPath();
				dc.PolyBezier(EE, SizeEE);
				dc.EndPath();
				dc.StrokePath();

				//Draw control points
				//for(long i= 0; i<SizeEE; i++){
				//	DisplayDot(&dc, EE[i], 6);
				//}

				//Draw endpoints
				POINT2D P0 (StrokeI.Arc.Radius_a * cos(StrokeI.Arc.StartAngle-StrokeI.Arc.Rotation), StrokeI.Arc.Radius_b * sin(StrokeI.Arc.StartAngle-StrokeI.Arc.Rotation));
				POINT2D P1 (StrokeI.Arc.Radius_a * cos(StrokeI.Arc.EndAngle-StrokeI.Arc.Rotation), StrokeI.Arc.Radius_b * sin(StrokeI.Arc.EndAngle-StrokeI.Arc.Rotation));
				P0= P0 + Centre;
				P1= P1 + Centre;
				P0= P0.Rotate(Centre, StrokeI.Arc.Rotation);
				P1= P1.Rotate(Centre, StrokeI.Arc.Rotation);

				dc.SelectObject(&PlumaEndPoints);
				CPoint Punto0= Transf.GDI_WorldToViewport(P0.x, P0.y);
				DisplayDot(&dc, Punto0, 4);
				CPoint Punto1= Transf.GDI_WorldToViewport(P1.x, P1.y);
				DisplayDot(&dc, Punto1, 4);

				//Draw centre and foci
				dc.SelectObject(&PlumaCentre);
				CPoint cc= Transf.GDI_WorldToViewport(Centre.x, Centre.y);
				DisplayDot(&dc, cc, 4);
				CPoint F1= Transf.GDI_WorldToViewport(StrokeI.Arc.Focus1_x, StrokeI.Arc.Focus1_y);
				DisplayDot(&dc, F1, 4);
				CPoint F2= Transf.GDI_WorldToViewport(StrokeI.Arc.Focus2_x, StrokeI.Arc.Focus2_y);
				DisplayDot(&dc, F2, 4);

				//Draw the worst points of the stroke
				dc.SelectObject(&PlumaWorst);
				CPoint wwOut= Transf.GDI_WorldToViewport(StrokeI.worstOut_x, StrokeI.worstOut_y);
				DisplayDot(&dc, wwOut, 4);
				CPoint wwIn= Transf.GDI_WorldToViewport(StrokeI.worstIn_x, StrokeI.worstIn_y);
				DisplayDot(&dc, wwIn, 4);
			}
		}
	}

	//Display endopints and midpoints
	//CPen PlumaR;
	//PlumaR.CreatePen( PS_SOLID, 2, RGB(255, 0, 0));
	//dc.SelectObject(&PlumaR);
	//for (long i= 0; i<(long)pDoc->DB_Drawing->GetSizeArcs(); i++){
	//	CArc Arc= pDoc->DB_Drawing->GetArc(i);

		//POINT2D P0 (Arc.T0_x, Arc.T0_y);
		//POINT2D PM (Arc.TM_x, Arc.TM_y);
		//POINT2D P1 (Arc.T1_x, Arc.T1_y);

		//CPoint Punto0= Transf.GDI_WorldToViewport(P0.x, P0.y);
		//DisplayDot(&dc, Punto0, 2);
		//CPoint PuntoM= Transf.GDI_WorldToViewport(PM.x, PM.y);
		//DisplayDot(&dc, PuntoM, 2);
		//CPoint Punto1= Transf.GDI_WorldToViewport(P1.x, P1.y);
		//DisplayDot(&dc, Punto1, 2);
	//}
}


void CWinRf_ViewSketch2D::project3DModel(CDB_Model *DB_Brep_Model_2D)
//Updates the model database, 
//by projecting 3D model as a 2D drawing and translating it to overlap the sketched 2D drawings

//Note that the input 3D model is stored in the inflation reference system
//while the output must be provided in the drawing reference system
{
	//Trivially project orthogonal projections
	//----------------------------------------
	if(ReferApp.Cu.MA.obliqueAngle == -1){
		//Calculate mapping translation

		//Get the current position of the 2D drawing
		double dXmin0= 0, dYmin0= 0, dZmin0= 0, dXmax0= 0, dYmax0= 0, dZmax0= 0;
		pDoc->DB_DrawingOff->GetBoundingBox(dXmin0, dYmin0, dZmin0, dXmax0, dYmax0, dZmax0);

		//Get the current position of the 3D model
		double dXmin1= 0, dYmin1= 0, dZmin1= 0, dXmax1= 0, dYmax1= 0, dZmax1= 0;
		DB_Brep_Model_2D->GetBoundingBox(dXmin1, dYmin1, dZmin1, dXmax1, dYmax1, dZmax1);

		double DeltaX= (dXmax0-dXmin0)/(dXmax1-dXmin1);
		double DeltaY= (dYmax0-dYmin0)/(dYmax1-dYmin1);

		//Project and translate
		for(long i=0; i<DB_Brep_Model_2D->GetSizeVertices(); i++ ) {
			CVertex Vertex= DB_Brep_Model_2D->GetVertex(i);
			Vertex.x = DeltaX*(Vertex.x-dXmin1)+dXmin0;
			Vertex.y = DeltaY*(Vertex.y-dYmin1)+dYmin0;
			//Vertex.z = 0;   //Orthogonal projection results from assigning Z's=0

			DB_Brep_Model_2D->SetVertex(i, Vertex);
		};
	}

	//Or calculate projection for oblique projections
	//-----------------------------------------------
	else if(ReferApp.Cu.MA.obliqueAngle >= 0){
		//Calculate mapping translation
		double DeltaX;
		double DeltaY;

		for(long i=0; i<DB_Brep_Model_2D->GetSizeVertices(); i++ ) {
			CVertex Vertex= DB_Brep_Model_2D->GetVertex(i);
			if(Vertex.z < Accuracy){
				CVertex VertexO= pDoc->DB_DrawingOff->GetVertex(i);
				DeltaX= VertexO.x - Vertex.x;
				DeltaY= VertexO.y - Vertex.y;
				break;
			}
		}

		//Transform 3D oblique model into 2D drawing by translating plus projecting
		if(ReferApp.Cu.MA.obliqueAngle == 0){
			double angle= ReferApp.Cu.MA.AngleXYZ[2];
			for(long i=0; i<DB_Brep_Model_2D->GetSizeVertices(); i++ ) {
				CVertex Vertex= DB_Brep_Model_2D->GetVertex(i);
				if(ReferApp.Cu.MA.obliqueAngle >= 0){
					Vertex.x = (Vertex.x + DeltaX) + Vertex.z * cos(angle) / ReferApp.Cu.MA.CoefE[0];
					Vertex.y = (Vertex.y + DeltaY) - Vertex.z * sin(angle) / ReferApp.Cu.MA.CoefE[0];
				}

				DB_Brep_Model_2D->SetVertex(i, Vertex);
			};
		}

		else if(ReferApp.Cu.MA.obliqueAngle == 1){
			double angle= ReferApp.Cu.MA.AngleXYZ[2];
			for(long i=0; i<DB_Brep_Model_2D->GetSizeVertices(); i++ ) {
				CVertex Vertex= DB_Brep_Model_2D->GetVertex(i);
				if(ReferApp.Cu.MA.obliqueAngle >= 0){
					Vertex.x = (Vertex.x + DeltaX) - Vertex.z * cos(angle) / ReferApp.Cu.MA.CoefE[1];
					Vertex.y = (Vertex.y + DeltaY) - Vertex.z * sin(angle) / ReferApp.Cu.MA.CoefE[1];
				}

				DB_Brep_Model_2D->SetVertex(i, Vertex);
			};
		}

		else if(ReferApp.Cu.MA.obliqueAngle == 2){
			double angle= ReferApp.Cu.MA.AngleXYZ[0];
			for(long i=0; i<DB_Brep_Model_2D->GetSizeVertices(); i++ ) {
				CVertex Vertex= DB_Brep_Model_2D->GetVertex(i);
				if(ReferApp.Cu.MA.obliqueAngle >= 0){
					Vertex.x = (Vertex.x + DeltaX);
					Vertex.y = (Vertex.y + DeltaY) + Vertex.z * sin(angle) / ReferApp.Cu.MA.CoefE[2];
				}

				DB_Brep_Model_2D->SetVertex(i, Vertex);
			};
		}
	}
}


void CWinRf_ViewSketch2D::GDI_DrawSketch2D()
// Draws the 2D sketch
// Draws the 2D drawing
// Draws the projected model

// Draws the labels
// Draws the visualization of some cues
{
	//Create a local device context,
	//to force flushing the drawing commands when it goes out of scope
	CRect rect;
	GetClientRect(&rect);
	CClientDC dc(this);

	//Clean the window
	dc.FillSolidRect(rect, ReferApp.m_BackgroundColor);

	//ADD BACKGROUND IMAGE TO THE DRAWING
	if(ReferApp.BackgroundBitMap)
		GDI_DrawBackGroundImage();

	//DRAW THE DIFFERENT REPRESENTATIONS OF THE MODEL
	if (ReferApp.SketchMode){

		//DRAW THE INPUT 2D SKETCH
		if (DisplayInputSketch && pDoc->DB_InputSketch->GetNumStrokes() > 0){
			GDI_Draw2DSketch(pDoc->DB_InputSketch);
		}

		//DRAW THE WORKING 2D SKETCH
		if (DisplayWorkingSketch && pDoc->DB_WorkingSketch->GetNumStrokes() > 0){
			GDI_Draw2DSketch(pDoc->DB_WorkingSketch);
		}
	}

	//DRAW THE VECTORIZED 2D LINE DRAWING, WITH ONLINE REFINEMENT
	if(DisplayOnLine &&
	   pDoc->DB_DrawingOn->GetSizeEdges()+pDoc->DB_DrawingOn->GetSizeArcs() > 0){
		GDI_Draw2DDrawing(pDoc->DB_DrawingOn, 0, 255, 0);
	}

	//DRAW THE VECTORIZED 2D MODEL, WITH ONLINE AND OFFLINE REFINEMENTS
	if(DisplayOffLine &&
	   pDoc->DB_DrawingOff->GetSizeEdges()+pDoc->DB_DrawingOff->GetSizeArcs() > 0){
		GDI_Draw2DDrawing(pDoc->DB_DrawingOff, 0, 0, 0);
	}

	//--------------------------------------------------------------------------
	//DRAW THE 2D PROJECTION OF THE 3D MODEL
	CDB_Model DB_Brep_Model_2D= *pDoc->DB_Brep_Model;   //Auxiliar database to save the projected model

	if(DisplayModelBrep &&
	   DB_Brep_Model_2D.GetSizeEdges()+DB_Brep_Model_2D.GetSizeArcs() > 0){

		project3DModel(&DB_Brep_Model_2D);

		GDI_Draw2DDrawing(&DB_Brep_Model_2D, 0, 0, 255);
	}

	//ADD LABELS TO THE DRAWING (OR PROJECTED MODEL)
	GDI_DrawLabels(&DB_Brep_Model_2D);  //Uses the projected model to locate the labels linked to vertices
	//--------------------------------------------------------------------------

	//DRAW INFORMATION ABOUT SOME CUES

	//Draw dashed strokes
	if (DisplayDashedStrokes){
		GDI_DrawDashedStrokes();
	}

	//Draw sub-graphs in sequence
	if (DisplaySubGraphsInSequence){
		GDI_DrawSubGraphsInSequence();
	}

	//Draw vertex labelling
	if (DisplayVertexLabelling){
		GDI_DrawVertexLabelling();
	}

	//Draw edge labelling
	if (DisplayLabelsEdges){
		GDI_DrawLabelsEdges();
	}

	//Draw perimeter (External circuit that envelopes the drawing)
	if (DisplayPerimeterInSequence){
		GDI_DrawPerimeterInSequence();
	}

	//Draw parallel edges
	if (DisplayParallelEdges){
		GDI_DrawParallelEdges();
	}

	//Draw parallel edges in sequence
	if (DisplayPEInSequence){
		GDI_DrawPEInSequence();
	}

	//Draw vanishing points
	if (DisplayVanishingPoints){
		GDI_DrawVanishingPoints();
	}

	//Draw vanishing points in sequence
	if (DisplayVPInSequence){
		GDI_DrawVPInSequence();
	}

	//Draw an icon with the main axes
	if (DisplayMainAxes){
		GDI_DrawMainAxes();
	}

	//Draw faces in implosion
	if (DisplayFaces && pDoc->DB_DrawingOff->GetSizeFaces()!= 0){
		GDI_DrawImplosionFaces();
	}

	//Draw faces in sequence
	if (DisplayFacesInSequence && pDoc->DB_DrawingOff->GetSizeFaces()!= 0){
		GDI_DrawFacesInSequence();
	}

	//Draw symmetry axes in faces
	if (DisplaySymmetryAxesInFaces){
		GDI_DrawSymmetryAxesInFaces();
	}

	//Draw symmetry planes
	if (DisplaySymmetryPlanes && ReferApp.Cu.Sy.SymPlanes.size() > 0){
		GDI_DrawSymmetryPlanes();
	}

	//Draw symmetry planes in sequence
	if (DisplaySymmetryPlanesInSequence && ReferApp.Cu.Sy.SymPlanes.size() > 0){
		GDI_DrawSymmetryPlanesInSequence();
	}

	//Draw symmetry sub-graphs in sequence
	if (DisplaySymmetrySubGraphsInSequence && ReferApp.Cu.Sy.SymPlanes.size() > 0){
		GDI_DrawSymmetrySubGraphsInSequence();
	}

	//Display normalon lines
	if (DisplayNormalon){
		GDI_DrawNormalonLines(pDoc->DB_DrawingOff);
	}

	//Draw the spanning tree
	if (DisplayTree){
		GDI_DrawSpanningTree();
	}

	//Draw ribs and dividers
	if (DisplayRibsAndDividers){
		GDI_DrawRibsAndDividers();
	}

	//Draw rails and slots
	if (DisplayRailsAndSlots){
		GDI_DrawRailsAndSlots();
	}

	//Draw steps and pockets
	if (DisplayStepsAndPockets){
		GDI_DrawStepsAndPockets();
	}

	//Draw chamfers and rounds
	if (DisplayChamfersAndRounds){    //TBD ChamfersAndRounds
		GDI_DrawChamfersAndRounds();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CViewModel23D message handlers

//------------------------2D Sketch Toolbar--------------------------------------------------------

void CWinRf_ViewSketch2D::OnDisplayInputSketch()
{
	DisplayInputSketch=!DisplayInputSketch;
	if(DisplayInputSketch)
		DisplayWorkingSketch= false;

		DisplaySlidingStrips= false;
		DisplayToleranceBands= false;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayInputSketch(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(ReferApp.SketchMode && DisplayInputSketch);
	pCmdUI->Enable(ReferApp.SketchMode && pDoc->DB_InputSketch!= NULL);
}


void CWinRf_ViewSketch2D::OnDisplayWorkingSketch()
{
	DisplayWorkingSketch=!DisplayWorkingSketch;
	if(DisplayWorkingSketch)
		DisplayInputSketch= false;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayWorkingSketch(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayWorkingSketch);
	pCmdUI->Enable(ReferApp.SketchMode && pDoc->DB_WorkingSketch->GetNumStrokes()!= 0);
}


void CWinRf_ViewSketch2D::OnDisplayDrawing2DOnLine()
{
	DisplayOnLine=!DisplayOnLine;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayDrawing2DOnLine(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayOnLine);
	pCmdUI->Enable(pDoc->DB_DrawingOn->GetSizeEdges()+pDoc->DB_DrawingOff->GetSizeArcs() != 0);
}


void CWinRf_ViewSketch2D::OnDisplayDrawing2DOffLine()
{
	DisplayOffLine=!DisplayOffLine;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayDrawing2DOffLine(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayOffLine);
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeEdges()+pDoc->DB_DrawingOff->GetSizeArcs() != 0);
}


void CWinRf_ViewSketch2D::OnDisplayModelBrep()
{
	DisplayModelBrep=!DisplayModelBrep;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayModelBrep(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayModelBrep);
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeEdges()+pDoc->DB_DrawingOff->GetSizeArcs() != 0);
}


void CWinRf_ViewSketch2D::OnDisplayNodesAndCorners()
{
	DisplayNodesAndCorners=!DisplayNodesAndCorners;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayNodesAndCorners(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayNodesAndCorners);
	pCmdUI->Enable(ReferApp.SketchMode);
}


void CWinRf_ViewSketch2D::OnDisplaySlidingStrips()
{
	DisplaySlidingStrips=!DisplaySlidingStrips;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplaySlidingStrips(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplaySlidingStrips);
	pCmdUI->Enable(ReferApp.SketchMode && DisplayWorkingSketch);
}


void CWinRf_ViewSketch2D::OnDisplayToleranceBands()
{
	DisplayToleranceBands=!DisplayToleranceBands;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayToleranceBands(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayToleranceBands);
	pCmdUI->Enable(ReferApp.SketchMode && DisplayWorkingSketch);
}


void CWinRf_ViewSketch2D::OnToolsDisplayStrokes()
{
	CDialogToolsDisplayStrokes Dlg;
	Dlg.SetParams(SelectedNodes, Display_Corners,
				  Display_Line_Tolerance, Display_Arc_Tolerance,
				  Display_Convex_Hull, Display_Merits_Strokes);
	if (Dlg.DoModal() == IDOK){
		Dlg.GetParams(SelectedNodes, Display_Corners,
					  Display_Line_Tolerance, Display_Arc_Tolerance,
					  Display_Convex_Hull, Display_Merits_Strokes);
	}
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateToolsDisplayStrokes(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ReferApp.SketchMode);
}


void CWinRf_ViewSketch2D::OnDisplayInBlack()
{
	DisplayInBlack=!DisplayInBlack;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayInBlack(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayInBlack);
	//pCmdUI->Enable(pDoc->SketchMode);
}


void CWinRf_ViewSketch2D::OnZoom100()
// Recalculates Zoom and Pan to obtain a full size view
{
	ResetWindow();
	Invalidate();
}


void CWinRf_ViewSketch2D::OnSelectLabelsColor()
//Opens a dialog to choose the color to be used to display the labels

//Updates the parameter: TextLabelsColor
{
	CColorDialog Dlg(TextLabelsColor);

	if (Dlg.DoModal() == IDOK) {
		TextLabelsColor= Dlg.GetColor();
		Invalidate();
	};
}


void CWinRf_ViewSketch2D::OnSelectLabelsFont()
//Opens a dialog to choose the size to be used to display the labels

//Updates the parameter: TextLabelsSizeScale
{
	CDialogLabelsFont Dlg;
	Dlg.SetParams(TextLabelsSizeScale);
	if (Dlg.DoModal() == IDOK){
		Dlg.GetParams(TextLabelsSizeScale);

		Invalidate();
	}
}


void CWinRf_ViewSketch2D::OnDisplayNumStrokes()
// Switches on/off the show-numbers-of-vertices button
{
	DisplayNumStrokes= !DisplayNumStrokes;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayNumStrokes(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayNumStrokes);
	pCmdUI->Enable(ReferApp.SketchMode);
}


void CWinRf_ViewSketch2D::OnDisplayNumVertices()
// Switches on/off the show-numbers-of-vertices button
{
	DisplayNumVertices= !DisplayNumVertices;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayNumVertices(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayNumVertices);
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeVertices() > 0);
}


void CWinRf_ViewSketch2D::OnDisplayMeritsOfVertices()
// Switches on/off the show-merits-of-vertices button
{
	DisplayMeritsOfVertices= !DisplayMeritsOfVertices;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayMeritsOfVertices(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayMeritsOfVertices);
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeVertices() > 0);
}


void CWinRf_ViewSketch2D::OnDisplayCoordinates()
// Switches on/off the show-coordinates-of-vertices button
{
	DisplayCoordVertices= !DisplayCoordVertices;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayCoordinates(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayCoordVertices);
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeVertices() > 0);
}


void CWinRf_ViewSketch2D::OnDisplayNumedges()
{
	DisplayNumEdges= !DisplayNumEdges;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayNumedges(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayNumEdges);
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeEdges()+pDoc->DB_DrawingOff->GetSizeArcs() > 0);
}


void CWinRf_ViewSketch2D::OnDisplayDashedStrokes()
{
	DisplayDashedStrokes= !DisplayDashedStrokes;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayDashedStrokes(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayDashedStrokes);
	pCmdUI->Enable(ReferApp.Ve.DashedStrokes.size() > 0);
}


void CWinRf_ViewSketch2D::OnDisplayValences()
// Switches on/off the show-valences button
{
	DisplayValences= !DisplayValences;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayValences(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayValences); 

	pCmdUI->Enable(ReferApp.Cu.Va.Valences.size() != 0);
}
void CWinRf_ViewSketch2D::OnDisplayParallelEdges()
{
	DisplayParallelEdges= !DisplayParallelEdges;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayParallelEdges(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayParallelEdges);
	pCmdUI->Enable(ReferApp.Cu.PE.ParallelEdges.size()!=0 && !DisplayPEInSequence);
}


void CWinRf_ViewSketch2D::OnDisplayPEInSequence()
{
	if (!DisplayPEInSequence){
		DisplayPEInSequence= true;
		NumPEInSequence= 0;
		pPrevNextPE = new CDialogPreviousNext();
		pPrevNextPE->SetParams(NumPEInSequence, 0, (long)ReferApp.Cu.PE.ParallelEdges.size()-1,
							  ID_VIEW2D_PreviousPEInSequence, 
							  ID_VIEW2D_NextPEInSequence,
							  0);
	}
	else{
		DisplayPEInSequence= false;
		pPrevNextPE->DestroyWindow();
	}
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayPEInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayPEInSequence);
	pCmdUI->Enable(ReferApp.Cu.PE.ParallelEdges.size()!=0 && !DisplayParallelEdges);
}


void CWinRf_ViewSketch2D::OnDisplayNextPEInSequence()
{
	NumPEInSequence += 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayPreviousPEInSequence()
{
	NumPEInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayVanishingPoints()
{
	DisplayVanishingPoints= !DisplayVanishingPoints;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayVanishingPoints(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayVanishingPoints);
	pCmdUI->Enable(ReferApp.Cu.VP.MeanVP.size()!=0 && !DisplayVPInSequence);
}


void CWinRf_ViewSketch2D::OnDisplayVPInSequence()
{
	if (!DisplayVPInSequence){
		DisplayVPInSequence= true;
		NumVPInSequence= 0;
		pPrevNextVP = new CDialogPreviousNext();
		pPrevNextVP->SetParams(NumVPInSequence, 0, (long)ReferApp.Cu.VP.MeanVP.size()-1,
							  ID_VIEW2D_PreviousVPInSequence, 
							  ID_VIEW2D_NextVPInSequence,
							  0);
	}
	else{
		DisplayVPInSequence= false;
		pPrevNextVP->DestroyWindow();
	}
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayVPInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayVPInSequence);
	pCmdUI->Enable(ReferApp.Cu.VP.MeanVP.size()!=0 && !DisplayVanishingPoints);
}


void CWinRf_ViewSketch2D::OnDisplayNextVPInSequence()
{
	NumVPInSequence += 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayPreviousVPInSequence()
{
	NumVPInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplaySubGraphsInSequence()
{
	if (!DisplaySubGraphsInSequence){
		DisplaySubGraphsInSequence= true;
		if(ReferApp.Cu.SG.NumSubgraphs > 1){
			NumSubGraphInSequence= 0;
			pPrevNextSG = new CDialogPreviousNext();
			pPrevNextSG->SetParams(NumSubGraphInSequence, 0, ReferApp.Cu.SG.NumSubgraphs-1,
								  ID_VIEW2D_PreviousSubGraphInSequence, 
								  ID_VIEW2D_NextSubGraphInSequence,
								  0);
		}
	}
	else{
		DisplaySubGraphsInSequence= false;
		pPrevNextSG->DestroyWindow();
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplaySubGraphsInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplaySubGraphsInSequence);
	pCmdUI->Enable(ReferApp.Cu.SG.NumSubgraphs > 0);
}


void CWinRf_ViewSketch2D::OnDisplayNextSubGraphInSequence()
{
	NumSubGraphInSequence += 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayPreviousSubGraphInSequence()
{
	NumSubGraphInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayPerimeterInSequence()
// Highlights the edges that form the perimeter
{
	if (!DisplayPerimeterInSequence){
		DisplayPerimeterInSequence= true;
		if(ReferApp.Cu.Pe.PerimeterEdges.size() > 1){
			NumPerimeterInSequence= 0;
			pPrevNextPer = new CDialogPreviousNext();
			pPrevNextPer->SetParams(NumPerimeterInSequence, 0, (long)ReferApp.Cu.Pe.PerimeterEdges.size()-1,
								  ID_VIEW2D_PreviousPerimeterInSequence, 
								  ID_VIEW2D_NextPerimeterInSequence,
								  0);
		}
	}
	else{
		DisplayPerimeterInSequence= false;
		pPrevNextPer->DestroyWindow();
	}
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayPerimeterInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayPerimeterInSequence);
	pCmdUI->Enable(ReferApp.Cu.Pe.PerimeterEdges.size()!=0);
}


void CWinRf_ViewSketch2D::OnDisplayNextPerimeterInSequence()
{
	NumPerimeterInSequence += 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayPreviousPerimeterInSequence()
{
	NumPerimeterInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayLabelsVertices()
// Switches on/off the show-labels of vertices button
{
	DisplayVertexLabelling= !DisplayVertexLabelling;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayLabelsVertices(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayVertexLabelling && ReferApp.Cu.La.LabelsInVertices.size() > 0);
	pCmdUI->Enable(ReferApp.Cu.La.LabelsInVertices.size()!=0);
}


void CWinRf_ViewSketch2D::OnDisplayLabelsEdges()
// Switches on/off the show-labels of vertices button
{
	DisplayLabelsEdges= !DisplayLabelsEdges;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayLabelsEdges(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayLabelsEdges && ReferApp.Cu.La.LabelsInEdges.size() > 0);
	pCmdUI->Enable(ReferApp.Cu.La.LabelsInEdges.size()!=0);
}


void CWinRf_ViewSketch2D::OnDisplayMainAxes()
{
	DisplayMainAxes=!DisplayMainAxes;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayMainAxes(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayMainAxes);
	pCmdUI->Enable(ReferApp.Cu.MA.MainAngles.size()>0);

	if(ReferApp.Cu.MA.MainAngles.size()== 0)
		DisplayMainAxes= false;

}


void CWinRf_ViewSketch2D::OnDisplayImplosionFaces()
{
	DisplayFaces = !DisplayFaces;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayImplosionFaces(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayFaces);
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeFaces()!=0 && !DisplayFacesInSequence);
}


void CWinRf_ViewSketch2D::OnDisplayFacesInSequence()
{
	if (!DisplayFacesInSequence){
		DisplayFacesInSequence= true;
		NumFaceInSequence= 0;
		pPrevNextFace = new CDialogPreviousNext();
		pPrevNextFace->SetParams(NumFaceInSequence, 0, pDoc->DB_DrawingOff->GetSizeFaces()-1,
							  ID_VIEW2D_PreviousFaceInSequence, 
							  ID_VIEW2D_NextFaceInSequence,
							  0);
	}
	else{
		DisplayFacesInSequence= false;
		pPrevNextFace->DestroyWindow();
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayFacesInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayFacesInSequence);
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeFaces()!=0 && !DisplayFaces);
}


void CWinRf_ViewSketch2D::OnDisplayNextFaceInSequence()
{
	NumFaceInSequence += 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayPreviousFaceInSequence()
{
	NumFaceInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayHiddenEdges()
{
	DisplayHiddenEdges = !DisplayHiddenEdges;
	DisplayHiddenArcs= DisplayHiddenEdges;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayHiddenEdges(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayHiddenEdges);
	pCmdUI->Enable(ReferApp.Cu.Hi.TypesOfEdges.size() != 0);
}


void CWinRf_ViewSketch2D::OnDisplayLevels()
// Switches on/off the show-levels button
// If the button is on, levels are recalculated
{
	DisplayLevels = !DisplayLevels;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayLevels(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(ReferApp.DIL.LevelOfVertex.size()!=0 && DisplayLevels);
	pCmdUI->Enable(ReferApp.DIL.LevelOfVertex.size()!=0);
}


void CWinRf_ViewSketch2D::OnDisplaySymmetryPlanes() 
{
	DisplaySymmetryPlanes = !DisplaySymmetryPlanes;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplaySymmetryPlanes(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(DisplaySymmetryPlanes);
	pCmdUI->Enable(ReferApp.Cu.Sy.SymPlanes.size() > 0);
}


void CWinRf_ViewSketch2D::OnDisplaySymmetryPlanesInSequence()
{
	if (!DisplaySymmetryPlanesInSequence){
		DisplaySymmetryPlanesInSequence= true;
		NumSymmetryPlaneInSequence= 0;
		pPrevNextSP = new CDialogPreviousNext();
		pPrevNextSP->SetParams(NumSymmetryPlaneInSequence, 0, (long)ReferApp.Cu.Sy.SymPlanes[NumSubGraphInSequence].size()-1,
							  ID_VIEW2D_PreviousSymmetryPlaneInSequence, 
							  ID_VIEW2D_NextSymmetryPlaneInSequence,
							  0);
	}
	else{
		DisplaySymmetryPlanesInSequence= false;
		pPrevNextSP->DestroyWindow();
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplaySymmetryPlanesInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplaySymmetryPlanesInSequence);
	pCmdUI->Enable(ReferApp.Cu.Sy.SymPlanes.size() > 0);
}


void CWinRf_ViewSketch2D::OnDisplayNextSymmetryPlaneInSequence()
{
	NumSymmetryPlaneInSequence += 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayPreviousSymmetryPlaneInSequence()
{
	NumSymmetryPlaneInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplaySymmetrySubGraphsInSequence()
//Calculates and display the subgraphs produced by a symmetry plane

//At this end, the original graph is splitted by the circuit defined by the symmetry plane
{
	if (!DisplaySymmetrySubGraphsInSequence){
		//Recalculate subgraphs
		CDB_Model DDBBAux= *pDoc->DB_DrawingOff;
		DDBBAux.ClearFaces();
		CCueSubGraphs SG;
		SG.SplitByCircuit(&DDBBAux, ReferApp.Cu.Sy.SymPlanes[NumSubGraphInSequence][NumSymmetryPlaneInSequence]);
		SG.GetSubGraphs(&DDBBAux, SymmetrySubGraphs);

		DisplaySymmetrySubGraphsInSequence= true;
		NumSymmetrySubGraphInSequence= 0;

		//Display Prev/Next dialog
		pPrevNextSym = new CDialogPreviousNext();
		pPrevNextSym->SetParams(NumSymmetrySubGraphInSequence, 0, (long)SymmetrySubGraphs.size()-1,
							  ID_VIEW2D_PreviousSymmetrySubGraphInSequence, 
							  ID_VIEW2D_NextSymmetrySubGraphInSequence,
							  15);
	}
	else{
		DisplaySymmetrySubGraphsInSequence= false;
		pPrevNextSym->DestroyWindow();
		SymmetrySubGraphs.clear();
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplaySymmetrySubGraphsInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplaySymmetrySubGraphsInSequence);
	pCmdUI->Enable(DisplaySymmetryPlanesInSequence);
}


void CWinRf_ViewSketch2D::OnDisplayNextSymmetrySubGraphInSequence()
{
	NumSymmetrySubGraphInSequence += 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplayPreviousSymmetrySubGraphInSequence()
{
	NumSymmetrySubGraphInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnDisplaySymmetryAxesInFaces() 
{
	DisplaySymmetryAxesInFaces = !DisplaySymmetryAxesInFaces;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplaySymmetryAxesInFaces(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(DisplaySymmetryAxesInFaces);
	pCmdUI->Enable(ReferApp.GeneralParams.DetectSymmetry);
}


void CWinRf_ViewSketch2D::OnDisplayNormalon()
{
	DisplayNormalon = !DisplayNormalon;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayNormalon(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayNormalon);
	pCmdUI->Enable(ReferApp.DIA.NormalonEdges.size()>0);
}


void CWinRf_ViewSketch2D::OnDisplayTree()
{
	if (!DisplayTree){
		DisplayTree= true;
		long MaxBranches;
		if (SelectedTree==0)
			MaxBranches= (long)(ReferApp.DIA.AxonometricTreeVertices.size()/2);
		else
			MaxBranches= (long)(ReferApp.DIL.LevelsTreeVertices.size()/2);

		NumBranches= MaxBranches;

		pPrevNextTree = new CDialogPreviousNext();
		pPrevNextTree->SetParams(NumBranches, 0, MaxBranches,
							  ID_VIEW2D_TREE_BACKWARDS, 
							  ID_VIEW2D_TREE_FORWARD,
							  0);
	}
	else{
		DisplayTree= false;
		pPrevNextTree->DestroyWindow();
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayTree(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayTree);
	pCmdUI->Enable((SelectedTree==0 && ReferApp.DIA.AxonometricTreeVertices.size()!=0) ||
				   (SelectedTree==1 && ReferApp.DIL.LevelsTreeVertices.size()!=0));
}


void CWinRf_ViewSketch2D::OnTreeForward()
{
	DisplayTree=true;
	NumBranches+= 1;

	if (SelectedTree==0){ //Arbol de Inflado axonometrico
		NumBranches= min(NumBranches, (long)(ReferApp.DIA.AxonometricTreeVertices.size()/2));
	}
	else{
		NumBranches= min(NumBranches, (long)(ReferApp.DIL.LevelsTreeVertices.size()/2));
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnTreeBackward()
{
	if(NumBranches>0) NumBranches-= 1;
	if(NumBranches==0) DisplayTree=false;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnSelectTree()
{
	CDialogSelectTree Dlg;
	Dlg.SetParams(SelectedTree);
	if (Dlg.DoModal() == IDOK){
		Dlg.GetParams(SelectedTree);
	}
}


void CWinRf_ViewSketch2D::OnUpdateSelectTree(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!DisplayTree);
}


//////////////DISPLAY FEATURES//////////////////////////////

void CWinRf_ViewSketch2D::OnDisplayRibsAndDividers()
{
	DisplayRibsAndDividers = !DisplayRibsAndDividers;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayRibsAndDividers(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayRibsAndDividers);
	pCmdUI->Enable(ReferApp.Fe.RD.NumRibs+ReferApp.Fe.RD.NumDividers);
}


void CWinRf_ViewSketch2D::OnDisplayRailsAndSlots()
{
	DisplayRailsAndSlots = !DisplayRailsAndSlots;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayRailsAndSlots(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayRailsAndSlots);
	pCmdUI->Enable(ReferApp.Fe.RS.NumRails + ReferApp.Fe.RS.NumSlots);
}


void CWinRf_ViewSketch2D::OnDisplayStepsAndPockets()
{
	DisplayStepsAndPockets = !DisplayStepsAndPockets;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayStepsAndPockets(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayStepsAndPockets);
	pCmdUI->Enable(ReferApp.Fe.SP.NumSteps+ReferApp.Fe.SP.NumPockets);
}


void CWinRf_ViewSketch2D::OnDisplayChamfersAndRounds()    //TBD ChamfersAndRounds
{
	DisplayChamfersAndRounds = !DisplayChamfersAndRounds;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDisplayChamfersAndRounds(CCmdUI* pCmdUI)    //TBD ChamfersAndRounds
{
	pCmdUI->SetCheck(DisplayChamfersAndRounds);
	pCmdUI->Enable(ReferApp.Fe.ChR.NumChamfers+ReferApp.Fe.ChR.NumRounds);
}


void CWinRf_ViewSketch2D::OnConfigDisplayFeatures()
{
	CDialogConfigDisplayFeatures Dlg;
	Dlg.SetParams(ScaleFeatureLabels, 
				  DisplayNamesOfFeatures, 
				  DisplayMeritsOfFeatures, 
				  DisplayContainerFaces);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ScaleFeatureLabels, 
					  DisplayNamesOfFeatures, 
					  DisplayMeritsOfFeatures, 
					  DisplayContainerFaces);
	}
}


void CWinRf_ViewSketch2D::OnUpdateConfigDisplayFeatures(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewSketch2D diagnostics

#ifdef _DEBUG
//void CWinRf_ViewSketch2D::AssertValid() const
//{
//	CScrollView::AssertValid();
//}
//
//void CWinRf_ViewSketch2D::Dump(CDumpContext& dc) const
//{
//	CScrollView::Dump(dc);
//}

CReferDoc* CWinRf_ViewSketch2D::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CReferDoc)));
	return (CReferDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewSketch2D message handlers


////////////////////// EDIT ////////////////////////////



POINT2D CWinRf_ViewSketch2D::GetDisplacement()
//Gains control on windows messages to catch a displacement
{
	POINT2D distance (0, 0);

	//Select a pen to draw the rubberline
	CClientDC pDC(this);
	CPen Pluma;
	pDC.SelectObject(&Pluma);
	pDC.SetROP2(R2_NOT); //XOR mode to remove by simply repainting

	//Control windows messages to catch displacement
	POINT pIni, pLast;
	MSG msg;
	bool continuar= false;
	bool rubber= false;
	while (!continuar && GetMessage(&msg, NULL, 0, 0)>0) {

		if(msg.message == WM_LBUTTONDOWN){
			GetCursorPos(&pIni);
			ScreenToClient(&pIni);
			pLast= pIni;
			rubber= true;
		}
		else if(rubber && msg.message == WM_MOUSEMOVE){
			//Remove previous rubberline by repainting
			pDC.MoveTo(pIni);
			pDC.LineTo(pLast);

			//Get new end point
			GetCursorPos(&pLast);
			ScreenToClient(&pLast);

			//Draw new rubberline
			pDC.MoveTo(pIni);
			pDC.LineTo(pLast);
		}
		else if(rubber && msg.message == WM_LBUTTONUP){
			//Save displacement
			GetCursorPos(&pLast);
			ScreenToClient(&pLast);
			distance.x =(double)(pLast.x-pIni.x)*(CameraZoom * WindowLength/ViewportWide);
			distance.y =(double)(pIni.y-pLast.y)*(CameraZoom * WindowHigh/ViewportHigh);
			continuar= true;
		}
		else if(msg.message == WM_KEYUP && msg.wParam == VK_ESCAPE){
			//Cancel if ESC key is pressed
			continuar= true;
		}
	}

	return distance;
}


void CWinRf_ViewSketch2D::OnSketchStrokes()
//Switches to draw strokes mode
{
	if(MouseMode!=SelectMode){
		ReferApp.SketchMode=! ReferApp.SketchMode;
	}

	if(ReferApp.SketchMode){
		if(pDoc->DB_InputSketch== NULL){
			pDoc->DB_InputSketch= new CDB_Sketch();
			pDoc->DB_WorkingSketch= new CDB_Sketch();
		}

		DisplayInputSketch= true;
		DisplayWorkingSketch= false;

		MouseMode=InactiveMode;
	}
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateSketchStrokes(CCmdUI* pCmdUI)
{
	//pCmdUI->SetCheck(ReferApp.SketchMode && pDoc->DB_InputSketch &&
	//				 MouseMode != SelectMode && MouseMode != LineMode && MouseMode != EraseMode);
	pCmdUI->SetCheck(ReferApp.SketchMode && pDoc->DB_InputSketch &&
					 MouseMode != SelectMode);
	pCmdUI->Enable(DisplayInputSketch && MouseMode==InactiveMode);
}


void CWinRf_ViewSketch2D::OnSelectStrokes()
//Switches to select/edit mode
{
	static COLORREF fondo;

	//if (DisplayInputSketch && pDoc->DB_InputSketch->GetNumStrokes() > 0){
	if (DisplayInputSketch){
		if(MouseMode!=SelectMode){
			MouseMode= SelectMode;
			Selection.clear();

			//Change the background while in this non-default operating mode
			fondo= ReferApp.m_BackgroundColor;
			ReferApp.m_BackgroundColor= RGB(225, 225, 225);

		}
		else{
			//Back to sketch mode
			MouseMode=InactiveMode;

			//Undo de background change
			ReferApp.m_BackgroundColor= fondo;
		}
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateSelectStrokes(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(MouseMode == SelectMode);
	pCmdUI->Enable(DisplayInputSketch);
}


void CWinRf_ViewSketch2D::OnDrawLines()
//Switches to draw lines mode
{
	MouseMode=LineMode;

	//Change the background while in this non-default operating mode
	COLORREF fondo= ReferApp.m_BackgroundColor;
	ReferApp.m_BackgroundColor= RGB(225, 225, 225);

	////Hide the sketch while drawing lines
	//bool verDIS= DisplayInputSketch;
	//DisplayInputSketch= false;
	//bool verDWS= DisplayWorkingSketch;
	//DisplayWorkingSketch= false;

	Invalidate();

	//Entering line-drawing mode
	CString sMensaje;
	sMensaje= "Procedure in Line-drawing mode:";
	sMensaje+="\r\n    - Draw lines by holding down the left button";
	sMensaje+="\r\n      while moving between the first and last tip";
	sMensaje+="\r\n                           ";
	sMensaje+="\r\n    - Press Delete to remove the line currently";
	sMensaje+="\r\n      pointed by the cursor";
	sMensaje+="\r\n                           ";
	sMensaje+="\r\n    - Press ESC when done";
	AfxMessageBox (sMensaje);

	//Loop to add new lines to the drawing online
	while(GetLine()){
	}

	//Reload the drawing offline
	*pDoc->DB_DrawingOff= *pDoc->DB_DrawingOn;

	//Reload the model database
	*pDoc->DB_Brep_Model= *pDoc->DB_DrawingOff;

	////Restore previous visualization
	//DisplayInputSketch= verDIS;
	//DisplayWorkingSketch= verDWS;

	ReferApp.m_BackgroundColor= fondo;
	MouseMode= InactiveMode;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDrawLines(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(MouseMode == LineMode);
	pCmdUI->Enable(DisplayInputSketch && MouseMode == InactiveMode);
}


void CWinRf_ViewSketch2D::OnEraseStrokes()
//Switches to erase mode
//and starts the erase manager
{
	//Hide the line-drawing, while erasing the sketch
	bool verOn= DisplayOnLine;
	DisplayOnLine= false;
	bool verOff= DisplayOffLine;
	DisplayOffLine= false;
	Invalidate();

	//Change the background while in this non-default operating mode
	COLORREF fondo= ReferApp.m_BackgroundColor;
	ReferApp.m_BackgroundColor= RGB(225, 225, 225);
	Invalidate();

	//Erase
	MouseMode= EraseMode;
	ManageEraserMove();
	MouseMode= InactiveMode;

	//Restore previous visualization
	DisplayOnLine= verOn;
	DisplayOffLine= verOff;
	ReferApp.m_BackgroundColor= fondo;
	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateEraseStrokes(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(MouseMode == EraseMode);
	pCmdUI->Enable(DisplayInputSketch && pDoc->DB_InputSketch->GetNumStrokes() > 0 &&
				   MouseMode != SelectMode);
}


void CWinRf_ViewSketch2D::OnEraseStrokesTools()
//Configures erase mode options
{
	CDialogEraseStrokesTools Dlg;
	Dlg.SetParams(Eraser_Radius, Eraser_Type);
	if(Dlg.DoModal()==IDOK)
		Dlg.GetParams(Eraser_Radius, Eraser_Type);
}


void CWinRf_ViewSketch2D::OnUpdateEraseStrokesTools(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(DisplayInputSketch && pDoc->DB_InputSketch->GetNumStrokes()>0 &&
				   MouseMode != SelectMode);
}


void CWinRf_ViewSketch2D::OnTranslateStrokes()
// Translates the stroke selected by the user
// a distance defined by the user
{
	if (DisplayInputSketch && pDoc->DB_InputSketch->GetNumStrokes() > 0){
		//Select the distance
		POINT2D distance= GetDisplacement();

		//Update the sketch
		for(long i= 0; i<(long)Selection.size(); i++){
			pDoc->DB_InputSketch->Translate(Selection[i], distance);
		}
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateTranslateStrokes(CCmdUI* pCmdUI)
{
	//pCmdUI->SetCheck(MouseMode == SelectMode);
	pCmdUI->Enable(DisplayInputSketch && Selection.size()>0);
}


void CWinRf_ViewSketch2D::OnCopyStrokes()
// Copy the strokes selected by the user
// a distance defined by the user
{
	if (DisplayInputSketch && pDoc->DB_InputSketch->GetNumStrokes() > 0){
		//Select the distance
		POINT2D distance= GetDisplacement();

		//Update the sketch
		for(long i= 0; i<(long)Selection.size(); i++){
			CStroke StrokeI= pDoc->DB_InputSketch->GetStroke(Selection[i]);
			pDoc->DB_InputSketch->AddStroke(StrokeI);
			pDoc->DB_InputSketch->Translate(pDoc->DB_InputSketch->GetNumStrokes()-1, distance);
		}
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateCopyStrokes(CCmdUI* pCmdUI)
{
	//pCmdUI->SetCheck(MouseMode == SelectMode);
	pCmdUI->Enable(DisplayInputSketch && Selection.size()>0);
}


void CWinRf_ViewSketch2D::OnRotateStrokes()
// Rotates the strokes selected by the user
// an angle defined by the user
{
	if (DisplayInputSketch && Selection.size()>0){
		//Select the angle
		double angle= 0.0;
		CDialogRotateAngle Dlg;
		Dlg.SetParams(angle);
		if(Dlg.DoModal()==IDOK)
			Dlg.GetParams(angle);

		POINT2D com= pDoc->DB_InputSketch->GetSubSketchCentroid(Selection);

		//Rotate all selected strokes
		for(long i= 0; i<(long)Selection.size(); i++){
			pDoc->DB_InputSketch->Rotate(Selection[i], com, angle);
		}
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateRotateStrokes(CCmdUI* pCmdUI)
{
	//pCmdUI->SetCheck(MouseMode == SelectMode);
	pCmdUI->Enable(DisplayInputSketch && Selection.size()>0);
}


void CWinRf_ViewSketch2D::OnScaleStrokes()
// Scales the stroke selected by the user
// a scale defined by the user
{
	if (DisplayInputSketch && pDoc->DB_InputSketch!=NULL && pDoc->DB_InputSketch->GetNumStrokes()>0){
		//Select the scale
		POINT2D Scale (1.0, 1.0);
		CDialogScaleStroke Dlg;
		Dlg.SetParams(Scale);
		if(Dlg.DoModal()==IDOK){
			Dlg.GetParams(Scale);

			POINT2D com= pDoc->DB_InputSketch->GetSubSketchCentroid(Selection);

			for(long i= 0; i<(long)Selection.size(); i++){
				pDoc->DB_InputSketch->Scale(Selection[i], com, Scale);
			}
		}
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateScaleStrokes(CCmdUI* pCmdUI)
{
	//pCmdUI->SetCheck(MouseMode == SelectMode);
	pCmdUI->Enable(DisplayInputSketch && Selection.size()>0);
}


void CWinRf_ViewSketch2D::OnFilterStrokes()
// Filters the stroke selected by the user
{
	if (DisplayInputSketch && pDoc->DB_InputSketch!=NULL && pDoc->DB_InputSketch->GetNumStrokes()>0){
		//Select the filter size
		long FilterMode= 0; // 0= filter, 1= Resample
		long FilterFactor= 2;
		long ResampleFactor= 20;
		CDialogFilterStroke Dlg;
		Dlg.SetParams(FilterMode, FilterFactor, ResampleFactor);
		if(Dlg.DoModal()==IDOK){
			Dlg.GetParams(FilterMode, FilterFactor, ResampleFactor);

			//Loop to filter all the selected strokes
			for(long i= 0; i<(long)Selection.size(); i++){
				CStroke StrokeI= pDoc->DB_InputSketch->GetStroke(Selection[i]);
				if(FilterMode == 0)
					StrokeI.Filter(FilterFactor);
				else if(FilterMode == 1)
					StrokeI.Resample(ResampleFactor);

				pDoc->DB_InputSketch->SetStroke(Selection[i], StrokeI);
			}
		}
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateFilterStrokes(CCmdUI* pCmdUI)
{
	//pCmdUI->SetCheck(MouseMode == SelectMode);
	pCmdUI->Enable(DisplayInputSketch && Selection.size()>0);
}


LRESULT CWinRf_ViewSketch2D::OnSetSelection(WPARAM n1, LPARAM n2)
//Labels as selected all strokes in the interval (n1, n2)

//(Used to select strokes added through and add file)
{
	//long nn1= LOWORD(n1); // Macro to get the low-order word
	//long nn2= HIWORD(n2); // Macro to get the high-order word

	MouseMode= SelectMode;
	Selection.clear();

	for(long i= (long)n1; i<(long)n2; i++){
		Selection.push_back(i);
	}

	Invalidate();

	return 0;
}


LRESULT CWinRf_ViewSketch2D::OnGetSelection(WPARAM wparam, LPARAM lparam)
//Returns currently selected strokes

//The list of selected strokes is stored in an external vector
//whose pointer is received through wparam
{
	std::vector<long> *pV = (std::vector<long> *) wparam;

	for(long i= 0; i<(long)Selection.size(); i++){
		pV->push_back(Selection[i]);
	}

	return 0;
}


LRESULT CWinRf_ViewSketch2D::OnGetPixelSize(WPARAM wparam, LPARAM lparam)
//Returns the size of a pixel in world coordinates

//The x size is returned through wparam
//The y size is returned through lparam

//Used to calculate distances in pixels while detecting cues
{
	double *sizeX= (double *) wparam;
	double *sizeY= (double *) lparam;

	CPoint zero(0, 0);
	POINT2D Zero= Transf.GDI_ViewportToWorld(zero);
	CPoint one(1, 1);
	POINT2D One= Transf.GDI_ViewportToWorld(one);

	*sizeX = std::abs(One.x - Zero.x);
	*sizeY = std::abs(One.y - Zero.y);

	return 0;
}


void CWinRf_ViewSketch2D::OnRotate2Ddrawing()
// Rotates the 2D model an angle defined by the user
// Used as an edit tool to introduce variants
{
	double angle= 0.0;

	CDialogRotateAngle Dlg;
	Dlg.SetParams(angle);
	if(Dlg.DoModal()==IDOK)
		Dlg.GetParams(angle);

	if(pDoc->DB_DrawingOff->GetSizeVertices() > 0){
		Rotate2DDrawing(angle);
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateRotate2Ddrawing(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeVertices()>0);
}


void CWinRf_ViewSketch2D::OnDistort2DDrawing()
// Randomly modifies the vertex coordinates of the 2D Model
// Is useful to introduce artificial imperfections in "perfect" line drawings

// Used to test robustness of reconstruction algorithms 
{
	CDialogDistortDrawing Dlg;
	Dlg.SetParams(ReferApp.R2D);
	if(Dlg.DoModal()==IDOK)
		Dlg.GetParams(ReferApp.R2D);

	if(pDoc->DB_DrawingOff->GetSizeVertices() > 0){
		Distort2DDrawing();
	}

	Invalidate();
}


void CWinRf_ViewSketch2D::OnUpdateDistort2DDrawing(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(pDoc->DB_DrawingOff->GetSizeVertices()>0);
}


void CWinRf_ViewSketch2D::Rotate2DSketch(double angle)
// Rotates the 2D Sketch an angle defined by the user

// (Used as an edit tool to introduce variants of sketches)
{
	//Compute the Centre Of Mass of the 2D Sketch
	POINT2D com= pDoc->DB_InputSketch->GetSketchCentroid();

	// Rotate all strokes around the Centre Of Mass of the total sketch
	for (long istroke= 0; istroke<(long)pDoc->DB_InputSketch->GetNumStrokes(); istroke++){
		CStroke StrokeI= pDoc->DB_InputSketch->GetStroke(istroke);
		StrokeI.Rotate(com, angle);
		pDoc->DB_InputSketch->SetStroke(istroke, StrokeI);
	}
}


void CWinRf_ViewSketch2D::Rotate2DDrawing(double angle)
// Rotates the 2D Model an angle defined by the user
// Used as an edit tool to introduce variants
{
	CVertex Vertice;

	//Compute the Centre Of Mass of the vertices of the 2D Model
	long numV= pDoc->DB_DrawingOff->GetSizeVertices();
	double com_x= 0.0;
	double com_y= 0.0;
	for (long i=0; i<numV; i++){
		Vertice= pDoc->DB_DrawingOff->GetVertex(i);
		com_x+= Vertice.x;
		com_y+= Vertice.y;
	}
	com_x= com_x/numV;
	com_y= com_y/numV;

	// Rotate vertices around the Centre Of Mass
	for (long i=0; i<numV; i++){
		Vertice= pDoc->DB_DrawingOff->GetVertex(i);
		double vx= Vertice.x - com_x;
		double vy= Vertice.y - com_y;

		Vertice.x= (vx*cos(angle) - vy*sin(angle)) + com_x;
		Vertice.y= (vx*sin(angle) + vy*cos(angle)) + com_y;
		Vertice.z= 0;
		pDoc->DB_DrawingOff->SetVertex(i, Vertice);
	}
}


void CWinRf_ViewSketch2D::Distort2DDrawing()
// Randomly modifies the vertex coordinates of the 2D Model

// Is useful to introduce artificial imperfections in "perfect" line drawings
{

	//Compute bounding box of vertices
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	pDoc->DB_DrawingOff->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double dRango= max(std::fabs(dXmax-dXmin), std::fabs(dYmax-dYmin));

	// Randomly distort every vertex
	// Random distorsion may affect x, y, or both
	// depending on the value of a random number in the range [0, 4]
	srand( (unsigned)time( NULL ) );  /* Reinicia los numeros aleatorios con el reloj del sistema */

	for (long i=0; i<pDoc->DB_DrawingOff->GetSizeVertices(); i++){
		CVertex Vertice= pDoc->DB_DrawingOff->GetVertex(i);

		double Incremento = (double)rand() / (double)(RAND_MAX+1);
		double Signo = (double)rand() / (double)(RAND_MAX+1);

		if (Incremento<0.25){
			Vertice.x+= 0;
			Vertice.y+= 0;
		}
		else{
			if (Incremento < 0.5){
				if (Signo<0.5){
					Vertice.x-= dRango*ReferApp.R2D.CoeficienteDistorsion;
					Vertice.y+= 0;
				}
				else{
					Vertice.x+= dRango*ReferApp.R2D.CoeficienteDistorsion;
					Vertice.y+= 0;
				}
			}
			else{
				if (Incremento < 0.75){
					if (Signo<0.5){
						Vertice.x+= 0;
						Vertice.y-= dRango*ReferApp.R2D.CoeficienteDistorsion;
					}
					else{
						Vertice.x+= 0;
						Vertice.y+= dRango*ReferApp.R2D.CoeficienteDistorsion;
					}
				}
				else{
					if (Signo<0.5){
						Vertice.x-= dRango*ReferApp.R2D.CoeficienteDistorsion;
						Vertice.y-= dRango*ReferApp.R2D.CoeficienteDistorsion;
					}
					else{
						Vertice.x+= dRango*ReferApp.R2D.CoeficienteDistorsion;
						Vertice.y+= dRango*ReferApp.R2D.CoeficienteDistorsion;
					}
				}
			}
		}

		pDoc->DB_DrawingOff->SetVertex(i, Vertice);
	}
}


void CWinRf_ViewSketch2D::EagerVectorizationUpdate()
//Updates vectorization and 2D refinement

//Should be called after any input sketch edit
{
	if(ReferApp.GeneralParams.bVectorize_Int){
		//Vectorize online
		CVectorization V;
		V.VectorizeSketch(pDoc);

		if(ReferApp.GeneralParams.bRefine2D_OnLine){
			//Refine online
			CRefine2D R2D;
			R2D.GetRefineDrawing(pDoc->DB_DrawingOn, false);
		}
	}
}


void CWinRf_ViewSketch2D::EagerVectorizeStroke(CStroke &InputStroke)
//Adds the line represented by a new stroke to the current vectorization
//and updates 2D refinement

//Should be called after adding any new stroke,
//while in online vectorization
{
	//Vectorize the stroke
	CVectorization V;
	if(V.VectorizeStroke(pDoc->DB_DrawingOn, InputStroke, -1) &&    // -1 -> stroke not yet added to the sketch
	   ReferApp.GeneralParams.bRefine2D_OnLine){
		// Refine online
		CRefine2D R2D;
		R2D.GetRefineDrawing(pDoc->DB_DrawingOn, false);
	}
}
