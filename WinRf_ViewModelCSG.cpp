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

// WinRf_ViewCSG.cpp : implementation file
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

#include "WinRf_GDI_Transform.h"
#include "DialogPreviousNextCSG.h"
#include "DialogConfigDisplayCSG.h"

#include "WinRf_TrackBall.h"
#include "WinRf_MainFrame.h"

#include "CueMainAxes.h"

#include "WinRf_OpenGLDevice.h"
#include "WinRf_OpenGLViewModelCSG.h"
#include "WinRf_ViewModelCSG.h"


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelCSG

IMPLEMENT_DYNCREATE(CWinRf_ViewModelCSG, CScrollView)

BEGIN_MESSAGE_MAP(CWinRf_ViewModelCSG, CScrollView)
	//{{AFX_MSG_MAP(CWinRf_ViewModelCSG)
	//Window
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()

	ON_WM_TIMER()

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

	//Toolbar CSG_Display
	ON_COMMAND(ID_CSG_VIEW_FEATURES_IN_SEQUENCE, OnViewCSG_FeaturesInSequence)
	ON_UPDATE_COMMAND_UI(ID_CSG_VIEW_FEATURES_IN_SEQUENCE, OnUpdateViewCSG_FeaturesInSequence)
	ON_COMMAND(ID_CSG_NextFeatureInSequence, OnDisplayNextFeatureInSequence)
	ON_COMMAND(ID_CSG_PreviousFeatureInSequence, OnDisplayPreviousFeatureInSequence)

	ON_COMMAND(ID_CSG_VIEW_MODEL_TREE, OnViewModelCSG_Tree)
	ON_UPDATE_COMMAND_UI(ID_CSG_VIEW_MODEL_TREE, OnUpdateViewModelCSG_Tree)

	ON_COMMAND(ID_CSG_DATUMS, OnDisplayDatums)
	ON_UPDATE_COMMAND_UI(ID_CSG_DATUMS, OnUpdateDisplayDatums)

	ON_COMMAND(ID_CSG_DATUMS_IN_SEQUENCE, OnDisplayDatumsInSequence)
	ON_UPDATE_COMMAND_UI(ID_CSG_DATUMS_IN_SEQUENCE, OnUpdateDisplayDatumsInSequence)
	ON_COMMAND(ID_CSG_NextDatumInSequence, OnDisplayNextDatumInSequence)
	ON_COMMAND(ID_CSG_PreviousDatumInSequence, OnDisplayPreviousDatumInSequence)

	ON_COMMAND(ID_CSG_PROFILES, OnDisplayProfiles)
	ON_UPDATE_COMMAND_UI(ID_CSG_PROFILES, OnUpdateDisplayProfiles)

	ON_COMMAND(ID_CSG_PROFILES_IN_SEQUENCE, OnDisplayProfilesInSequence)
	ON_UPDATE_COMMAND_UI(ID_CSG_PROFILES_IN_SEQUENCE, OnUpdateDisplayProfilesInSequence)
	ON_COMMAND(ID_CSG_NextProfileInSequence, OnDisplayNextProfileInSequence)
	ON_COMMAND(ID_CSG_PreviousProfileInSequence, OnDisplayPreviousProfileInSequence)

	ON_COMMAND(ID_CSG_DECONSTRUCION, OnDisplayDeconstruction)
	ON_UPDATE_COMMAND_UI(ID_CSG_DECONSTRUCION, OnUpdateDisplayDeconstruction)

	ON_COMMAND(ID_CSG_DECONSTRUCION_IN_SEQUENCE, OnDisplayDeconstructionInSequence)
	ON_UPDATE_COMMAND_UI(ID_CSG_DECONSTRUCION_IN_SEQUENCE, OnUpdateDisplayDeconstructionInSequence)
	ON_COMMAND(ID_CSG_NextDeconstructionInSequence, OnDisplayNextDeconstructionInSequence)
	ON_COMMAND(ID_CSG_PreviousDeconstructionInSequence, OnDisplayPreviousDeconstructionInSequence)

	ON_COMMAND(ID_CSG_ZOOM_RESET, OnDisplayZoomReset)
	ON_UPDATE_COMMAND_UI(ID_CSG_ZOOM_RESET, OnUpdateDisplayZoomReset)

	ON_COMMAND(ID_CSG_ConfigDisplayCSG, OnConfigDisplayCSG)
	ON_UPDATE_COMMAND_UI(ID_CSG_ConfigDisplayCSG, OnUpdateConfigDisplayCSG)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelCSG construction/destruction

CWinRf_ViewModelCSG::CWinRf_ViewModelCSG()
{
	dc = NULL;

	OpenGLViewModelCSG = new CWinRf_OpenGLViewModelCSG();

	mode_CSG_Features= false;
	NumFeatureInSequence= 0;

	ViewCSG_Tree= true;
	DisplayDatums= false;
	DisplayDatumsInSequence= false;
	DisplayProfiles= false;
	DisplayProfilesInSequence= false;
	NumProfileInSequence= 0;
	DisplayDatumsInSequence= false;
	NumDatumInSequence= 0;
	DisplayDeconstruction= false;
	DisplayDeconstructionInSequence= false;
	NumDeconstructionInSequence= 0;

	CSG_Tree_ScaleLabels= 1.0;
	CSG_Tree_DisplayNumOfBranches= true;
	CSG_Tree_DisplayNamesOfFeatures= true;
	CSG_Tree_DisplayMeritsOfFeatures= true;
	CSG_Tree_Columns= 2;

	CSG_Datums_ScaleLabels= 1.0;
	CSG_Datums_DisplayGraph= true;
	CSG_Datums_DisplayNumVertices= false;
	CSG_Datums_DisplayCoordsVertices= false;
	CSG_Datums_DisplayNumEdges= false;
	CSG_Datums_DisplayInputDrawing= true;
	CSG_Datums_DisplayNumOfDatums= true;
	CSG_Datums_DisplayDatumSymmetry= true;

	CSG_Dec_ScaleLabels= 1.0;
	CSG_Dec_DisplayLabelDeconstruction= true;
	CSG_Dec_DisplayNumVertices= false;
	CSG_Dec_DisplayCoordsVertices= false;
	CSG_Dec_DisplayNumEdges= false;
	CSG_Dec_Columns= 2;
}


CWinRf_ViewModelCSG::~CWinRf_ViewModelCSG()
{
	if (OpenGLViewModelCSG) delete OpenGLViewModelCSG;
}


void CWinRf_ViewModelCSG::OnDestroy()
{
	CScrollView::OnDestroy();

	// Clean up rendering context stuff
	OpenGLDeviceModelCSG.destroy();
	if (dc) delete dc;

	pDoc->m_pFrameModelCSG = NULL;
	pDoc->m_pViewModelCSG = NULL;

	if(pPrevNextFeature != NULL)
		pPrevNextFeature->DestroyWindow();
	if(pPrevNextDatum != NULL)
		pPrevNextDatum->DestroyWindow();
	if(pPrevNextProfile != NULL)
		pPrevNextProfile->DestroyWindow();
	if(pPrevNextDec != NULL)
		pPrevNextDec->DestroyWindow();
}


BOOL CWinRf_ViewModelCSG::PreCreateWindow(CREATESTRUCT& cs) 
{
	//// Add styles required by OpenGL
	//cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC); // Private-DC constant 
	
	return CScrollView::PreCreateWindow(cs);
}


int CWinRf_ViewModelCSG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Get the handle to the ReferDoc Document
	pDoc= GetDocument();

	//Get the handle to two Device Contexts and link OpenGL with them
	dc = new CClientDC(this);
	OpenGLDeviceModelCSG.create(dc->m_hDC);      //To display the CSG model

	//Make the rendering contexts current, perform initialization, then deselect them
	OpenGLDeviceModelCSG.makeCurrent();
		OpenGLViewModelCSG->GLSetupRC_ModelCSG(dc->m_hDC);
	OpenGLDeviceModelCSG.makeCurrentNull();

	pDoc->m_pViewModelCSG = this;

	return 0;
}


void CWinRf_ViewModelCSG::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	ResetViewport();
	ResetWindow(true);
}


void CWinRf_ViewModelCSG::ResetViewport()
// Updates the Viewport transformation
{
	CRect rect;
	GetClientRect(&rect);

	if (rect.right== 0 || rect.bottom== 0) return;  //no client area

	ViewportWide = rect.right - rect.left;
	ViewportHigh = rect.bottom - rect.top;

	Transf.GDI_SetViewport(rect.left, rect.top, rect.right, rect.bottom);
}


void CWinRf_ViewModelCSG::ResetViewport(long row, long numRows,
										long column, long numColumns)
// Updates the Viewport transformation

// This overload is used to activate a vignette of the client rectangle,
// which is divided into a regular frame of numRows x numColumns vignettes
// (Each call activates the sub-viewport located in the place (row, column))
{
	if(row <0 || row >= numRows) return; //row out of range
	if(column <0 || column >= numColumns) return; //column out of range

	CRect rect;
	GetClientRect(&rect);

	if (rect.right== 0 || rect.bottom== 0) return;  //no client area

	//Size of a vignette
	ViewportWide = (rect.right - rect.left)/numColumns;
	ViewportHigh = (rect.bottom - rect.top)/numRows;

	//Define the vignette (row,column)
	Transf.GDI_SetViewport((int) (rect.left + ViewportWide*(column)),
						   (int) (rect.top + ViewportHigh*(row)),
						   (int) (rect.left + ViewportWide*(column+1)), 
						   (int) (rect.top + ViewportHigh*(row+1)));
}


void CWinRf_ViewModelCSG::Get2DModelBoundingBox(double &wXmin, double &wYmin,
												double &wXmax, double &wYmax)
// Recalculates the dimensions of the drawing of the 2D model
{
	wXmin= wYmin= wXmax= wYmax= 0;

	//Case of deconstruct
	if (!mode_CSG_Features){

		if(ViewCSG_Tree){
			long numFeatures= pDoc->DB_CSG_Model->TreeFeaturesSize();
			if(numFeatures >0){
				double wZmin= 0, wZmax= 0;
				CDB_Model F2D= pDoc->DB_CSG_Model->GetFeature(numFeatures-1).GetFeature2D();  //Read only
				F2D.GetBoundingBox(wXmin, wYmin, wZmin, wXmax, wYmax, wZmax);
			}
			else if(pDoc->DB_Deconstruct.size() != 0 && pDoc->DB_Deconstruct[0].GetSizeVertices() > 0){
				double wZmin= 0, wZmax= 0;
				pDoc->DB_Deconstruct[0].GetBoundingBox(wXmin, wYmin, wZmin, wXmax, wYmax, wZmax);
			}
		}

		else if(DisplayDeconstruction){
			if(pDoc->DB_Deconstruct.size() != 0 && pDoc->DB_Deconstruct[0].GetSizeVertices() > 0){
				double wZmin= 0, wZmax= 0;
				pDoc->DB_Deconstruct[0].GetBoundingBox(wXmin, wYmin, wZmin, wXmax, wYmax, wZmax);
			}
		}

		else if(DisplayDatums || DisplayDatumsInSequence ||
				DisplayProfiles || DisplayProfilesInSequence){
			if(pDoc->DB_DatumGraph->GetSizeVertices() > 0){
				double wZmin= 0, wZmax= 0;
				pDoc->DB_DatumGraph->GetBoundingBox(wXmin, wYmin, wZmin, wXmax, wYmax, wZmax);
			}
		}

		else if(DisplayDeconstructionInSequence){
			//long core= pDoc->DB_Deconstruct.size()-1;
			//if(pDoc->DB_Deconstruct.size() != 0 && pDoc->DB_Deconstruct[core].GetSizeVertices() > 0){
			if(pDoc->DB_Deconstruct.size() != 0 && pDoc->DB_Deconstruct[0].GetSizeVertices() > 0){
				double wZmin= 0, wZmax= 0;
				//pDoc->DB_Deconstruct[core].GetBoundingBox(wXmin, wYmin, wZmin, wXmax, wYmax, wZmax);
				pDoc->DB_Deconstruct[0].GetBoundingBox(wXmin, wYmin, wZmin, wXmax, wYmax, wZmax);
			}
		}
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


void CWinRf_ViewModelCSG::UpdateWindow()
// Updates the window according to the current pan and zoom
{
	Transf.GDI_SetWorld(-WindowLength/2 * CameraZoom - CameraPanX,
						-WindowHigh/2   * CameraZoom - CameraPanY,
						+WindowLength/2 * CameraZoom - CameraPanX,
						+WindowHigh/2   * CameraZoom - CameraPanY);
}


void CWinRf_ViewModelCSG::ResetWindow(bool resetZoomPan)
// Reset the window, to fit the current 2D model in the current window
{
	//Drawing size
	double wXmin= 0, wYmin= 0, wXmax= 0, wYmax= 0;
	Get2DModelBoundingBox(wXmin, wYmin, wXmax, wYmax);

	//Resize the window to fit the drawing
	WindowLength= 1.2*(wXmax-wXmin);
	WindowHigh  = 1.2*(wYmax-wYmin);

	//Keep its aspect ratio
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
	if(resetZoomPan){
		CameraZoom= 1;
		CameraPanX= -(wXmin+wXmax)/2;
		CameraPanY= -(wYmin+wYmax)/2;
	}

	//Apply transformation, including pan and zoom
	UpdateWindow();
}


void CWinRf_ViewModelCSG::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// CScrollViews must set scroll sizes
	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	//CSG toolbar parameters
	ViewCSG_Tree= true;
	DisplayDatums= false;
	DisplayDatumsInSequence= false;
	DisplayProfiles= false;
	DisplayProfilesInSequence= false;
	DisplayDeconstruction= false;
	DisplayDeconstructionInSequence= false;
	pPrevNextFeature= NULL;
	pPrevNextDatum= NULL;
	pPrevNextProfile= NULL;
	pPrevNextDec= NULL;

	//OpenGL Parameters
	OpenGLViewModelCSG->Camera_CSG.Radius=5;
	OpenGLViewModelCSG->Camera_CSG.Accuracy=5;
	OpenGLViewModelCSG->Camera_CSG.dIncrementAngleX = OpenGLViewModelCSG->Camera_CSG.dIncrementAngleY = 0.0;

	Ctrackball T;
	float q1[4]; float q2[4];  //Get isometric view as a combination of two rotations
	T.calc_quat( q1, 0.0f, 0.50f, 0.0f, 0.0f ); //From half top to front
	T.calc_quat( q2, 0.50f, 0.0f, 0.0f, 0.0f ); //From half side to front
	T.add_quats(q1, q2, OpenGLViewModelCSG->Camera_CSG.curquat);
	OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom = 1.0;

	//Trackball parameters
	m_EstateTimer = Inactive;
	OpenGLViewModelCSG->Camera_CSG.PosMousePrev = OpenGLViewModelCSG->Camera_CSG.PosMouseCurrent = CPoint(0,0);
	OpenGLViewModelCSG->Camera_CSG.IncrementPosMouse = CSize(0,0);
}


void CWinRf_ViewModelCSG::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
}


BOOL CWinRf_ViewModelCSG::OnEraseBkgnd(CDC* pDC) 
// Overrides to prevent default background erase
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect( rect, ReferApp.m_BackgroundColor );

	// Return TRUE indicates that no further erasing is required
	// (this will avoid GDI clearing the OpenGL windows' background)

	return true ;
}


void CWinRf_ViewModelCSG::TextInRect(CDC *pDC, CString sText, CRect &rectFont)
// Writes a string sText with a size which fits perfectly in a rectangle rectFont
// Font already attached with device context pDC is scaled
{
	//Gets current font details
	CFont* pFont = pDC->GetCurrentFont();
	LOGFONT LogFont;
	pFont->GetLogFont(&LogFont);

	//Scales the font to fit in the rectangle
	//LogFont.lfHeight = -MulDiv(rectFont.Height(), GetDeviceCaps(*pDC, LOGPIXELSY), 72);
	int H= rectFont.Height();
	int W= rectFont.Width()/sText.GetLength();
	LogFont.lfHeight = -72 * H / pDC->GetDeviceCaps(LOGPIXELSY); //Point refers to a logical size (1/72 of a logical inch)
	if(2*W > 1*H)
		LogFont.lfWidth = 0;   //Preserve aspect 1/2 while text fits
	else
		LogFont.lfWidth  = -72 * W / pDC->GetDeviceCaps(LOGPIXELSX);

	//LogFont.lfQuality = PROOF_QUALITY;

	//Create a font of calculated size
	CFont Font;
	Font.CreateFontIndirect(&LogFont);

	//Rechecks the string width and height
	pFont = pDC->SelectObject(&Font);
	CSize Size = pDC->GetTextExtent(sText);
	pDC->SelectObject(pFont);
	Font.DeleteObject();

	//Fine tunes the font width and height
	LogFont.lfWidth = (int) (LogFont.lfWidth * rectFont.Width() / Size.cx);
	LogFont.lfHeight = (int) (LogFont.lfHeight * rectFont.Height() / Size.cy);

	// Create a font with accurate size and displays string
	Font.CreateFontIndirect(&LogFont);
	pFont = pDC->SelectObject(&Font);
	pDC->DrawText(sText, rectFont, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	pDC->SelectObject(pFont);
	Font.DeleteObject();
}


void CWinRf_ViewModelCSG::PlotModelTree(CDC* pDC)
// Renders an image of the document in the context device pointed by pDC 
// Uses OpenGL to display the model tree of the CSG model
{
	CString sTexto, sAux;

	CRect rect;
	GetClientRect(&rect);

	//Configure text
	long height= (long)(rect.Height()/10 * CSG_Tree_ScaleLabels);   //Standard height is set as 1% of the client window height
	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", pDC); //Initializes a CFont with the specified height, measured in tenths of a point, and typeface.
	pDC->SelectObject(&Fuente);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_LEFT);

	//Rows and columns to divide the vindow in vignettes

	long numFeatures= pDoc->DB_CSG_Model->TreeFeaturesSize();
	long numRows= (numFeatures+(CSG_Tree_Columns-1))/CSG_Tree_Columns;

	double Wide= (rect.right - rect.left)/CSG_Tree_Columns;      //Divide the wide in columns
	double High= (rect.bottom - rect.top)/max(numFeatures/CSG_Tree_Columns, 1); //Divide the high in rows

	//Loop over all the branches
	long iFeat= 0;

	long numBranches= pDoc->DB_CSG_Model->TreeBranchesSize();
	for(long i= (long)numBranches-1; i>=0; i--){ //Follow branches backwards
		//Loop to display all the features in the current branch
		//(from contained to containers)
		long numFeatInBranch= (long)pDoc->DB_CSG_Model->TreeBranchSize(i);
		for (long j= (long)numFeatInBranch-1; j>=0; j--){ //Follow branch features backwards
			//Select column and assing vignette vindow/viewport
			long numColumn= iFeat%CSG_Tree_Columns;
			ResetViewport((iFeat/CSG_Tree_Columns), numRows, 
						  numColumn, CSG_Tree_Columns);
			ResetWindow(false);  //Update the aspect ratio

			CFeature Feature= pDoc->DB_CSG_Model->GetFeature(i, j);

			//Label of current feature
			if(CSG_Tree_ScaleLabels > 0){
				CString toString;

				if(CSG_Tree_DisplayNumOfBranches){
					sTexto= "Branch ";
					toString.Format("%d", numBranches-1-i);
					sTexto+= toString;
				}

				sTexto+= " Feature ";
				toString.Format("%d", iFeat);
				sTexto+= toString;

				if(CSG_Tree_DisplayNamesOfFeatures){
					//sTexto+= "\n";  //Skip line

					switch(Feature.GetType()){
						case CORE:    {sTexto+= " CORE";};    break;
						case NODE:    {sTexto+= " NODE";};    break;
						case RIB:     {sTexto+= " RIB";};     break;
						case DIVIDER: {sTexto+= " DIVIDER";}; break;
						case RAIL:    {sTexto+= " RAIL";};    break;
						case SLOT:    {sTexto+= " SLOT";};    break;
						case STEP:    {sTexto+= " STEP";};    break;
						case POCKET:  {sTexto+= " POCKET";};  break;
						case NONE:    {sTexto+= " NONE";};    break;
					}
				}

				if(CSG_Tree_DisplayMeritsOfFeatures){
					//if(!CSG_Tree_DisplayNamesOfFeatures)
					//	sTexto+= "\n";  //Skip line

					toString.Format("%f", (float)Feature.GetMerit());
					sTexto+= " (Merit " + toString + ")";
				}

				//Text location
				CRect rect;
				//(vignette box)
				rect.left= (int)(Wide*numColumn);
				rect.top= (int)(High*(iFeat/CSG_Tree_Columns));
				rect.right= (int)(Wide*numColumn + Wide);
				rect.bottom= (int)(High*(iFeat/CSG_Tree_Columns) + High);
				rect.DeflateRect((int)(0.05*Wide), (int)(0.05*High));

				//(text box)
				CFont* pFont = pDC->GetCurrentFont();
				LOGFONT LogFont;
				pFont->GetLogFont(&LogFont); //Get the heigth of the current font
				//rect.bottom= (int)(rect.top - 4*LogFont.lfHeight);  //Allocate space for up to three lines
				rect.bottom= (int)(rect.top - 2*LogFont.lfHeight);  //Allocate space for one line

				//Print text in rect
				//pDC->DrawText(sTexto, -1, &rect, DT_TOP | DT_CENTER | DT_WORDBREAK); //Multiple line text
				TextInRect(pDC, sTexto, rect);                                       //Text adjusted to rectangle

				//Drawing of the current feature
				GDI_DrawCSGDrawing(&Feature.GetFeature2D(), 0, 0, 255);

				iFeat++;
			}
		}
	}
}


void CWinRf_ViewModelCSG::DisplayDot(CClientDC *dc, CPoint Position, long size)
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


POINT2D CWinRf_ViewModelCSG::GetDisplacedPoint(POINT2D p1, POINT2D p2, double d)
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


void CWinRf_ViewModelCSG::GDI_DrawMainAxes()
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


void CWinRf_ViewModelCSG::GDI_DrawCSGDatums(CDB_Model *DDBB_Path, CDB_Model *DDBB_Drawing, 
											CDB_CSG *DB_CSG_Model, long NumDatum,
											int R, int G, int B)
//Draws the profile NumProfile over the 2D line drawing
//and the path lines

//If numProfile<0, draws all the profiles
{
	ResetWindow(false);  //Update the aspect ratio

	CClientDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	//Configure text
	long height= (long)(rect.Height()/10 * CSG_Datums_ScaleLabels);    //Standard height is set as 1% of the client window height
	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextAlign(TA_LEFT);

	//Calculate offset for drawing labels
	//(distance to move the labels appart from their center point)
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB_Path->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double Offset=0.04* min(dXmax-dXmin, dYmax-dYmin)/2;

	CString Texto, Texto1;

	//Display graph
	//-------------
	if(CSG_Datums_DisplayGraph){
		//Draw vertices as fat dots
		int RR= 0; int GG= 0; int BB= 255;
		CPen Pluma;
		Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB_Path->GetSizeVertices(); i++){
			CVertex Vertice=DDBB_Path->GetVertex(i);

			CPoint P= Transf.GDI_WorldToViewport(Vertice.x,Vertice.y);
			DisplayDot(&dc, P, 3);
		}
		Pluma.DeleteObject();

		//Display labels of vertices
		RR= 0; GG= 0; BB= 255;
		Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB_Path->GetSizeVertices(); i++){
			CVertex Vertice=DDBB_Path->GetVertex(i);
			if (CSG_Datums_DisplayNumVertices || CSG_Datums_DisplayCoordsVertices){
				Texto="";
				if (CSG_Datums_DisplayNumVertices){
					Texto1.Format("%i", i);
					Texto+=Texto1;
				}
				if (CSG_Datums_DisplayCoordsVertices){
					//Texto1.Format(" (%4.2f, %5.2f, %4.2f)", Vertice.x, Vertice.y, Vertice.z);
					Texto1.Format(" (%4.2f, %5.2f)", Vertice.x, Vertice.y);
					Texto+=Texto1;
				}

				CPoint Punto = Transf.GDI_WorldToViewport((double)Vertice.x+Offset,(double)Vertice.y+Offset);
				dc.TextOut (Punto.x, Punto.y, Texto);
			}
		}
		Pluma.DeleteObject();

		//Display edges of the path
		RR= 0; GG= 0; BB= 255;
		Pluma.CreatePen(PS_DASH, 1, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB_Path->GetSizeEdges(); i++){
			//Draws a line linking the head and the tail of the edge
			CEdge E= DDBB_Path->GetEdge(i);
			CVertex HeadVertex= DDBB_Path->GetVertex(E.Head);
			CVertex TailVertex= DDBB_Path->GetVertex(E.Tail);
			CPoint P = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
			dc.MoveTo(P.x,P.y);
			P = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
			dc.LineTo(P.x,P.y);
		}
		Pluma.DeleteObject();

		//Display labels of edges
		if (CSG_Datums_DisplayNumEdges){
			RR= 255; GG= 0; BB= 0;
			Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
			dc.SelectObject(&Pluma);

			for (long i=0; i<DDBB_Path->GetSizeEdges(); i++){
				CEdge E= DDBB_Path->GetEdge(i);
				CVertex HeadVertex= DDBB_Path->GetVertex(E.Head);
				CVertex TailVertex= DDBB_Path->GetVertex(E.Tail);

				Texto.Format("%i", i);
				/////////////////////////////////////
				Texto1.Format(" (%i, %i)", E.Head, E.Tail);
				Texto+=Texto1;
				/////////////////////////////////////
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
				dc.MoveTo(Base.x,Base.y);
				dc.LineTo(Reference.x,Reference.y);

				dc.Rectangle(rect);

				//Draw label
				dc.TextOut(Reference.x, Reference.y-Tsize.cy, Texto);
			}
			Pluma.DeleteObject();
		}
	}

	//Display input drawing
	//---------------------
	if(CSG_Datums_DisplayInputDrawing){
		int RR= 0; int GG= 0; int BB= 0;
		CPen Pluma;
		Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		//Draw vertices as fat dots
		for (long i=0; i<DDBB_Drawing->GetSizeVertices(); i++){
			CVertex Vertice=DDBB_Drawing->GetVertex(i);

			CPoint P= Transf.GDI_WorldToViewport(Vertice.x,Vertice.y);
			DisplayDot(&dc, P, 4);
		}

		//Loop over the lines of the drawing
		for (long i=0; i<DDBB_Drawing->GetSizeEdges(); i++){
			//Draws a line linking the head and the tail of the stroke
			CEdge E= DDBB_Drawing->GetEdge(i);
			CVertex HeadVertex= DDBB_Drawing->GetVertex(E.Head);
			CVertex TailVertex= DDBB_Drawing->GetVertex(E.Tail);
			CPoint P = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
			dc.MoveTo(P.x,P.y);
			P = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
			dc.LineTo(P.x,P.y);
		}

		Pluma.DeleteObject();
	}

	//Display datums
	//--------------
	int RR= R; int GG= G; int BB= B;
	CPen Pluma;
	Pluma.CreatePen(PS_SOLID, 4, RGB(RR, GG, BB));
	dc.SelectObject(&Pluma);

	//Loop over all the datums
	long numDatums= DB_CSG_Model->DatumPlanesSize();
	for(long i= 0; i<numDatums; i++){
		//In sequence mode, draw only the datum Numdatum
		if(NumDatum >= 0 && NumDatum != i)
			continue;

		long numVertices= DB_CSG_Model->DatumPlaneVerticesSize(i);
		if (numVertices > 0){
			CVertex pvertex= DDBB_Path->GetVertex(DB_CSG_Model->GetDatumPlaneVertex(i, 0));

			//Loop over all the vertices of the current datum
			for (long k= 0; k<numVertices+1; k++){
				CVertex kvertex= DDBB_Path->GetVertex(DB_CSG_Model->GetDatumPlaneVertex(i, k%numVertices));

				CPoint P = Transf.GDI_WorldToViewport((double)pvertex.x,(double)pvertex.y);
				dc.MoveTo(P.x,P.y);
				P = Transf.GDI_WorldToViewport((double)kvertex.x,(double)kvertex.y);
				dc.LineTo(P.x,P.y);

				pvertex= kvertex;
			}
		}
	}

	//Loop over all the origins
	long numOrigins= DB_CSG_Model->DatumOriginsSize();
	std::vector<long> Origins= DB_CSG_Model->GetDatumOrigins();
	for(long i= 0; i<numOrigins; i++){
		int RR= R; int GG= G; int BB= B;
		CPen Pluma;
		Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		//Draw origins as fat dots
		CVertex Vertice=DDBB_Drawing->GetVertex(Origins[i]);

		CPoint P= Transf.GDI_WorldToViewport(Vertice.x,Vertice.y);
		DisplayDot(&dc, P, 6);
	}

	//Display labels
	if(CSG_Datums_DisplayNumOfDatums){
		//TBD
	}

	if(CSG_Datums_DisplayDatumSymmetry){
		//TBD
	}

	Pluma.DeleteObject();

}


void CWinRf_ViewModelCSG::GDI_DrawCSGProfiles(CDB_Model *DDBB_Path, 
											  CDB_Model *DDBB_Drawing, 
											  CDB_CSG *DB_CSG_Model, 
											  long NumProfile,
											  int R, int G, int B)
//Draws the profile NumProfile over the 2D line drawing
//and the path lines

//If numProfile<0, draws all the profiles
{
	ResetWindow(false);  //Update the aspect ratio

	CClientDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	//Configure text
	long height= (long)(rect.Height()/10 * CSG_Datums_ScaleLabels);     //Standard height is set as 1% of the client window height
	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextAlign(TA_LEFT);

	//Calculate offset for drawing labels
	//(distance to move the labels appart from their center point)
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB_Path->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double Offset=0.04* min(dXmax-dXmin, dYmax-dYmin)/2;

	CString Texto, Texto1;

	//Display graph
	//-------------
	if(CSG_Datums_DisplayGraph){
		//Draw vertices as fat dots
		int RR= 0; int GG= 0; int BB= 255;
		CPen Pluma;
		Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB_Path->GetSizeVertices(); i++){
			CVertex Vertice=DDBB_Path->GetVertex(i);

			CPoint P= Transf.GDI_WorldToViewport(Vertice.x,Vertice.y);
			DisplayDot(&dc, P, 3);
		}
		Pluma.DeleteObject();

		//Display labels of vertices
		RR= 0; GG= 0; BB= 255;
		Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB_Path->GetSizeVertices(); i++){
			CVertex Vertice=DDBB_Path->GetVertex(i);
			if (CSG_Datums_DisplayNumVertices || CSG_Datums_DisplayCoordsVertices){
				Texto="";
				if (CSG_Datums_DisplayNumVertices){
					Texto1.Format("%i", i);
					Texto+=Texto1;
				}
				if (CSG_Datums_DisplayCoordsVertices){
					//Texto1.Format(" (%4.2f, %5.2f, %4.2f)", Vertice.x, Vertice.y, Vertice.z);
					Texto1.Format(" (%4.2f, %5.2f)", Vertice.x, Vertice.y);
					Texto+=Texto1;
				}

				CPoint Punto = Transf.GDI_WorldToViewport((double)Vertice.x+Offset,(double)Vertice.y+Offset);
				dc.TextOut (Punto.x, Punto.y, Texto);
			}
		}
		Pluma.DeleteObject();

		//Display edges of the path
		RR= 0; GG= 0; BB= 255;
		Pluma.CreatePen(PS_DASH, 1, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB_Path->GetSizeEdges(); i++){
			//Draws a line linking the head and the tail of the edge
			CEdge E= DDBB_Path->GetEdge(i);
			CVertex HeadVertex= DDBB_Path->GetVertex(E.Head);
			CVertex TailVertex= DDBB_Path->GetVertex(E.Tail);
			CPoint P = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
			dc.MoveTo(P.x,P.y);
			P = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
			dc.LineTo(P.x,P.y);
		}
		Pluma.DeleteObject();

		//Display labels of edges
		if (CSG_Datums_DisplayNumEdges){
			RR= 255; GG= 0; BB= 0;
			Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
			dc.SelectObject(&Pluma);

			for (long i=0; i<DDBB_Path->GetSizeEdges(); i++){
				CEdge E= DDBB_Path->GetEdge(i);
				CVertex HeadVertex= DDBB_Path->GetVertex(E.Head);
				CVertex TailVertex= DDBB_Path->GetVertex(E.Tail);

				Texto.Format("%i", i);
				/////////////////////////////////////
				Texto1.Format(" (%i, %i)", E.Head, E.Tail);
				Texto+=Texto1;
				/////////////////////////////////////
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
				dc.MoveTo(Base.x,Base.y);
				dc.LineTo(Reference.x,Reference.y);

				dc.Rectangle(rect);

				//Draw label
				dc.TextOut(Reference.x, Reference.y-Tsize.cy, Texto);
			}
			Pluma.DeleteObject();
		}
	}

	//Display input drawing
	//---------------------
	if(CSG_Datums_DisplayInputDrawing){
		int RR= 0; int GG= 0; int BB= 0;
		CPen Pluma;
		Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		//Draw vertices as fat dots
		for (long i=0; i<DDBB_Drawing->GetSizeVertices(); i++){
			CVertex Vertice=DDBB_Drawing->GetVertex(i);

			CPoint P= Transf.GDI_WorldToViewport(Vertice.x,Vertice.y);
			DisplayDot(&dc, P, 4);
		}

		//Loop over the lines of the drawing
		for (long i=0; i<DDBB_Drawing->GetSizeEdges(); i++){
			//Draws a line linking the head and the tail of the stroke
			CEdge E= DDBB_Drawing->GetEdge(i);
			CVertex HeadVertex= DDBB_Drawing->GetVertex(E.Head);
			CVertex TailVertex= DDBB_Drawing->GetVertex(E.Tail);
			CPoint P = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
			dc.MoveTo(P.x,P.y);
			P = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
			dc.LineTo(P.x,P.y);
		}

		Pluma.DeleteObject();
	}

	//Display profiles
	//----------------
	int RR= R; int GG= G; int BB= B;
	CPen Pluma;
	Pluma.CreatePen(PS_SOLID, 4, RGB(RR, GG, BB));
	dc.SelectObject(&Pluma);

	//Loop over all the branches
	long numBranches= DB_CSG_Model->TreeBranchesSize();
	long iFeat= -1;
	for(long i= 0; i< numBranches; i++){
		//Loop over all the features in the current branch
		long numFeatInBranch= (long)DB_CSG_Model->TreeBranchSize(i);
		for (long j= 0; j<numFeatInBranch; j++){
			iFeat++;

			//In sequence mode, draw only the profile NumProfile
			if(NumProfile >= 0 && NumProfile != iFeat)
				continue;

			CFeature Feature= DB_CSG_Model->GetFeature(i, j);
			long numVertices= (long)Feature.SizeCrossSection();
			if (numVertices > 0){
				CVertex pvertex= DDBB_Path->GetVertex(Feature.GetCrossSection()[0]);

				//Loop over all the vertices of the current profile
				for (long k= 0; k<numVertices+1; k++){
					CVertex kvertex= DDBB_Path->GetVertex(Feature.GetCrossSectionVertex(k%numVertices));

					CPoint P = Transf.GDI_WorldToViewport((double)pvertex.x,(double)pvertex.y);
					dc.MoveTo(P.x,P.y);
					P = Transf.GDI_WorldToViewport((double)kvertex.x,(double)kvertex.y);
					dc.LineTo(P.x,P.y);

					pvertex= kvertex;
				}
			}
		}
	}

	//Display labels
	if(CSG_Datums_DisplayNumOfDatums){
		//TBD
	}

	if(CSG_Datums_DisplayDatumSymmetry){
		//TBD
	}

	Pluma.DeleteObject();

}


void CWinRf_ViewModelCSG::PlotModelDeconstruction(CDC* pDC)
// Renders an image of the document in the context device pointed by pDC 
// Uses OpenGL to display the model deconstruction of the CSG model
{
	ResetWindow(false);  //Update the aspect ratio

	CString sTexto, sAux;

	CRect rect;
	GetClientRect(&rect);

	//Configure text
	long height= (long)(rect.Height()/10 * CSG_Dec_ScaleLabels);     //Standard height is set as 1% of the client window height
	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", pDC);  //Initializes a CFont with the specified height, measured in tenths of a point, and typeface.
	pDC->SelectObject(&Fuente);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_LEFT);

	//Rows and columns to divide the vindow in vignettes
	long decSize= pDoc->DB_Deconstruct.size();
	long numRows= (decSize+(CSG_Dec_Columns-1))/CSG_Dec_Columns;

	double Wide= (rect.right - rect.left)/CSG_Dec_Columns;   //Divide the wide in columns
	double High= (rect.bottom - rect.top)/max(decSize/CSG_Dec_Columns, 1);   //Divide the high in rows

	//Loop to display the consecutive deconstructions
	for (long iDec= 0; iDec<decSize; iDec++){
		//Select column and assing vignette vindow/viewport
		long numColumn= iDec%CSG_Dec_Columns;
		ResetViewport((iDec/CSG_Dec_Columns), numRows, 
					  numColumn, CSG_Dec_Columns);
		ResetWindow(false);  //Update the aspect ratio

		//Label of current deconstruct stage
		if(CSG_Dec_ScaleLabels > 0){
			//double vleft, vtop, vright, vbottom;
			//Transf.GDI_GetViewport(vleft, vtop, vright, vbottom);
			//double Wide= (vright - vleft)/DeconstructColumns;                   //Two columns)
			//double High  = (vbottom - vtop)/max(treeSize, 1);  //TreeSize rows

			sTexto= "Deconstruct";

			if(CSG_Dec_DisplayLabelDeconstruction){
				CString toString;
				toString.Format("%d", iDec);
			sTexto+= " stage " + toString;
			}

			//Text location
			CRect rect;
			//(vignette box)
			rect.left= (int)(Wide*numColumn);
			rect.top= (int)(High*(iDec/CSG_Dec_Columns));
			rect.right= (int)(Wide*numColumn + Wide);
			rect.bottom= (int)(High*(iDec/CSG_Dec_Columns) + High);
			rect.DeflateRect((int)(0.05*Wide), (int)(0.05*High));

			//(text box)
			CFont* pFont = pDC->GetCurrentFont();
			LOGFONT LogFont;
			pFont->GetLogFont(&LogFont); //Get the heigth of the current font
			//rect.bottom= (int)(rect.top - 4*LogFont.lfHeight);  //Allocate space for up to three lines
			rect.bottom= (int)(rect.top - 2*LogFont.lfHeight);  //Allocate space for one line

			//Print text in rect
			//pDC->DrawText(sTexto, -1, &rect, DT_TOP | DT_CENTER | DT_WORDBREAK); //Multiple line text
			TextInRect(pDC, sTexto, rect);                                       //Text adjusted to rectangle
		}

		//Drawing of current deconstruct stage
		GDI_DrawCSGDrawing(&pDoc->DB_Deconstruct[iDec], 0, 0, 255);
	}
}


void CWinRf_ViewModelCSG::GDI_DrawCSGDrawing(CDB_Model *DDBB, int R, int G, int B)
//Draws the current deconstruction of the 2D line drawing
{
	ResetWindow(false);  //Update the aspect ratio

	CClientDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	//Configure text
	long height= (long)(rect.Height()/10 * CSG_Dec_ScaleLabels);     //Standard height is set as 1% of the client window height
	CFont Fuente;
	Fuente.CreatePointFont( height, "Arial", &dc);
	dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextAlign(TA_LEFT);

	//Calculate offset for drawing labels
	//(distance to move the labels appart from their center point)
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double Offset=0.04* min(dXmax-dXmin, dYmax-dYmin)/2;

	CString Texto, Texto1;

	//Display drawing
	//---------------
	if(CSG_Datums_DisplayGraph){
		//Draw vertices as fat dots
		int RR= 0; int GG= 0; int BB= 255;
		CPen Pluma;
		Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB->GetSizeVertices(); i++){
			CVertex Vertice=DDBB->GetVertex(i);

			CPoint P= Transf.GDI_WorldToViewport(Vertice.x,Vertice.y);
			DisplayDot(&dc, P, 3);
		}
		Pluma.DeleteObject();

		//Display labels of vertices
		RR= 0; GG= 0; BB= 255;
		Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB->GetSizeVertices(); i++){
			CVertex Vertice=DDBB->GetVertex(i);
			if (CSG_Dec_DisplayNumVertices || CSG_Dec_DisplayCoordsVertices){
				Texto="";
				if (CSG_Dec_DisplayNumVertices){
					Texto1.Format("%i", i);
					Texto+=Texto1;
				}
				if (CSG_Dec_DisplayCoordsVertices){
					//Texto1.Format(" (%4.2f, %5.2f, %4.2f)", Vertice.x, Vertice.y, Vertice.z);
					Texto1.Format(" (%4.2f, %5.2f)", Vertice.x, Vertice.y);
					Texto+=Texto1;
				}

				CPoint Punto = Transf.GDI_WorldToViewport((double)Vertice.x+Offset,(double)Vertice.y+Offset);
				dc.TextOut (Punto.x, Punto.y, Texto);
			}
		}
		Pluma.DeleteObject();

		//Display edges of the path
		RR= R; GG= G; BB= B;
		Pluma.CreatePen(PS_SOLID, 2, RGB(RR, GG, BB));
		dc.SelectObject(&Pluma);

		for (long i=0; i<DDBB->GetSizeEdges(); i++){
			//Draws a line linking the head and the tail of the edge
			CEdge E= DDBB->GetEdge(i);
			CVertex HeadVertex= DDBB->GetVertex(E.Head);
			CVertex TailVertex= DDBB->GetVertex(E.Tail);
			CPoint P = Transf.GDI_WorldToViewport((double)HeadVertex.x,(double)HeadVertex.y);
			dc.MoveTo(P.x,P.y);
			P = Transf.GDI_WorldToViewport((double)TailVertex.x,(double)TailVertex.y);
			dc.LineTo(P.x,P.y);
		}
		Pluma.DeleteObject();

		//Display labels of edges
		if (CSG_Dec_DisplayNumEdges){
			RR= 255; GG= 0; BB= 0;
			Pluma.CreatePen(PS_SOLID, 1, RGB(RR, GG, BB));
			dc.SelectObject(&Pluma);

			for (long i=0; i<DDBB->GetSizeEdges(); i++){
				CEdge E= DDBB->GetEdge(i);
				CVertex HeadVertex= DDBB->GetVertex(E.Head);
				CVertex TailVertex= DDBB->GetVertex(E.Tail);

				Texto.Format("%i", i);
				/////////////////////////////////////
				//Texto1.Format(" (%i, %i)", E.Head, E.Tail);
				//Texto+=Texto1;
				/////////////////////////////////////
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
				dc.MoveTo(Base.x,Base.y);
				dc.LineTo(Reference.x,Reference.y);

				dc.Rectangle(rect);

				//Draw label
				dc.TextOut(Reference.x, Reference.y-Tsize.cy, Texto);
			}
			Pluma.DeleteObject();
		}
	}
}


void CWinRf_ViewModelCSG::OnDrawModelTreeCSG(CDC* pDC)
//Renders an image of the document in the context device pointed by pDC 

//There are three alternative visualizations:
// - Display the deconstructed line drawing,
//   by controlling the deconstruction stage to visualize
// - Display the model tree as a flat diagram
// - Display the 3D model of the currently selected feature 
//   (from the core to the last feature)

//Uses OpenGL to display the CSG model in 3D
{
	//Update viewport
	ResetViewport();

	//Update window to ensure that pan and zoom are updated on each redraw
	UpdateWindow();

	//Invalidate to ensure re-painting
	//(and recalculation of viewport/wold transformation)
	//Invalidate();

	if (ViewCSG_Tree){
		PlotModelTree(pDC);
	}

	else if (DisplayDatums){
		GDI_DrawCSGDatums(pDoc->DB_DatumGraph, 
						  &pDoc->DB_Deconstruct[0],
						  pDoc->DB_CSG_Model, 
						 -1,
						  255, 0, 0);

		GDI_DrawMainAxes();
	}

	else if (DisplayDatumsInSequence){
		GDI_DrawCSGDatums(pDoc->DB_DatumGraph,
						  &pDoc->DB_Deconstruct[0],
						  pDoc->DB_CSG_Model,
						  NumDatumInSequence,
						  255, 0, 0);

		GDI_DrawMainAxes();
	}

	else if (DisplayProfiles){
		GDI_DrawCSGProfiles(pDoc->DB_DatumGraph, 
							&pDoc->DB_Deconstruct[0],
							pDoc->DB_CSG_Model,
							-1,
							255, 0, 0);

		GDI_DrawMainAxes();
	}

	else if (DisplayProfilesInSequence){
		GDI_DrawCSGProfiles(pDoc->DB_DatumGraph, 
							&pDoc->DB_Deconstruct[0],
							pDoc->DB_CSG_Model, 
							NumProfileInSequence,
							255, 0, 0);

		GDI_DrawMainAxes();
	}

	else if (DisplayDeconstruction){
		PlotModelDeconstruction(pDC);
	}

	else if (DisplayDeconstructionInSequence){
		GDI_DrawCSGDrawing(&pDoc->DB_Deconstruct[NumDeconstructionInSequence], 0, 0, 255);
	}
}


void CWinRf_ViewModelCSG::DetermineModelCSGVolume(CDB_Model *DB_Aux)
{
	DB_Aux->GetBoundingBox(OpenGLViewModelCSG->VolumeDDBB_CSG.dXmin, 
							OpenGLViewModelCSG->VolumeDDBB_CSG.dYmin, 
							OpenGLViewModelCSG->VolumeDDBB_CSG.dZmin,
							OpenGLViewModelCSG->VolumeDDBB_CSG.dXmax, 
							OpenGLViewModelCSG->VolumeDDBB_CSG.dYmax, 
							OpenGLViewModelCSG->VolumeDDBB_CSG.dZmax );

	// Centre of the model
	OpenGLViewModelCSG->VolumeDDBB_CSG.dMidPointX = (OpenGLViewModelCSG->VolumeDDBB_CSG.dXmax + OpenGLViewModelCSG->VolumeDDBB_CSG.dXmin) / 2.0;
	OpenGLViewModelCSG->VolumeDDBB_CSG.dMidPointY = (OpenGLViewModelCSG->VolumeDDBB_CSG.dYmax + OpenGLViewModelCSG->VolumeDDBB_CSG.dYmin) / 2.0;
	OpenGLViewModelCSG->VolumeDDBB_CSG.dMidPointZ = (OpenGLViewModelCSG->VolumeDDBB_CSG.dZmax + OpenGLViewModelCSG->VolumeDDBB_CSG.dZmin) / 2.0;

	// Intervals (min, max) for every axis
	OpenGLViewModelCSG->VolumeDDBB_CSG.dRangeX = std::fabs(OpenGLViewModelCSG->VolumeDDBB_CSG.dXmax - OpenGLViewModelCSG->VolumeDDBB_CSG.dXmin);
	OpenGLViewModelCSG->VolumeDDBB_CSG.dRangeY = std::fabs(OpenGLViewModelCSG->VolumeDDBB_CSG.dYmax - OpenGLViewModelCSG->VolumeDDBB_CSG.dYmin);
	OpenGLViewModelCSG->VolumeDDBB_CSG.dRangeZ = std::fabs(OpenGLViewModelCSG->VolumeDDBB_CSG.dZmax - OpenGLViewModelCSG->VolumeDDBB_CSG.dZmin);

	// Calculate the coordinates of PlaneXY, which is centered around (0,0,-0.01)
	double dRange= max(OpenGLViewModelCSG->VolumeDDBB_CSG.dRangeX,
					   max(OpenGLViewModelCSG->VolumeDDBB_CSG.dRangeY,
						   OpenGLViewModelCSG->VolumeDDBB_CSG.dRangeZ / 2));

	OpenGLViewModelCSG->VolumeDDBB_CSG.dPlaneXmax = OpenGLViewModelCSG->VolumeDDBB_CSG.dPlaneYmax = (dRange/2.0) * 1.2;
	OpenGLViewModelCSG->VolumeDDBB_CSG.dPlaneXmin = OpenGLViewModelCSG->VolumeDDBB_CSG.dPlaneYmin = - OpenGLViewModelCSG->VolumeDDBB_CSG.dPlaneXmax;
	OpenGLViewModelCSG->VolumeDDBB_CSG.dPlaneZ = - OpenGLViewModelCSG->VolumeDDBB_CSG.dMidPointZ - dRange/200;

	OpenGLViewModelCSG->VolumeDDBB_CSG.dSizeCoordAxes = dRange / 5.0;
}


void CWinRf_ViewModelCSG::OnDrawFeaturesCSG(CDC* pDC)
// Draws 3D BRep models of the features by way of OpenGL

//WARNING: This function is intended to diplay
//         a BRep view of the features of the CSG model,
//         which must have been previously calculated
{
	// Copy the BRep database of the currently selectd feature
	CDB_Model F2D= pDoc->DB_CSG_Model->GetFeature(NumFeatureInSequence).GetFeature2D();;  //Read only

	// Determine visualization volume
	DetermineModelCSGVolume(&F2D);

	// Limits of the visualization window (Replaces OnSize)
	CRect rect;
	GetClientRect(&rect);
	OpenGLViewModelCSG->Camera_CSG.SizeWindow.cx = rect.right;
	OpenGLViewModelCSG->Camera_CSG.SizeWindow.cy = rect.bottom;

	// Make the rendering context current
	OpenGLDeviceModelCSG.makeCurrent();

		// Update the background color
		OpenGLViewModelCSG->GLSetupBackground(ReferApp.m_BackgroundColor);

		// Update sizes to accomodate changes in Zoom and Pan
		OpenGLViewModelCSG->GLResize_ModelCSG(rect.right, rect.bottom);

		// Update scene
		OpenGLViewModelCSG->GLRenderSceneCSG(&F2D);

		// Swap our scene to the front
		SwapBuffers(dc->m_hDC);

	// Deselect the rendering context to allow other rendering contexts to co-exist
	OpenGLDeviceModelCSG.makeCurrentNull();
};


void CWinRf_ViewModelCSG::OnDraw(CDC* pDC)
// Draws 3D model by way of OpenGL
// Distinguishes between two different 3D models: 
//		Features
//		Whole 3D CSG model
{
	// Determine visualization volume
	if (mode_CSG_Features){
		OnDrawFeaturesCSG(pDC);
	}
	else{
		OnDrawModelTreeCSG(pDC);
	}

};


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelCSG diagnostics

#ifdef _DEBUG
//void CWinRf_ViewModelCSG::AssertValid() const
//{
//	CScrollView::AssertValid();
//}
//
//
//void CWinRf_ViewModelCSG::Dump(CDumpContext& dc) const
//{
//	CScrollView::Dump(dc);
//}


CReferDoc* CWinRf_ViewModelCSG::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CReferDoc)));
	return (CReferDoc*)m_pDocument;
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelCSG message handlers


void CWinRf_ViewModelCSG::OnViewCSG_FeaturesInSequence() 
//Switches the parameter that controls
//the visualization of the  CSG model
{
	mode_CSG_Features = !mode_CSG_Features;

	//if(!mode_Features_CSG && !ReferApp.GeneralParams.bInflateBRep){
	//	AfxMessageBox("No inflated 3D BRep model to display");
	//}

	//Ensure that alternative visualizations are inactive
	if(mode_CSG_Features){
		OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom = 1.0;
		Invalidate();

		ViewCSG_Tree = false;
		DisplayDatums= false;
		DisplayDatumsInSequence= false;
		DisplayProfiles= false;
		DisplayProfilesInSequence= false;
		DisplayDeconstruction= false;
		DisplayDeconstructionInSequence= false;
		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();

		if(pDoc->DB_CSG_Model->TreeFeaturesSize() > 1){
			NumFeatureInSequence= 0;
			pPrevNextFeature = new CDialogPreviousNextCSG();
			pPrevNextFeature->SetParams(NumFeatureInSequence, 0, 
										pDoc->DB_CSG_Model->TreeFeaturesSize()-1,
										ID_CSG_PreviousFeatureInSequence, 
										ID_CSG_NextFeatureInSequence,
										0);
		}
	}
	else{
		ResetWindow(true);
		Invalidate();

		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
	}

	Invalidate();
}


void CWinRf_ViewModelCSG::OnUpdateViewCSG_FeaturesInSequence(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(mode_CSG_Features);
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG);
}


void CWinRf_ViewModelCSG::OnDisplayNextFeatureInSequence()
{
	NumFeatureInSequence += 1;
	Invalidate();
}


void CWinRf_ViewModelCSG::OnDisplayPreviousFeatureInSequence()
{
	NumFeatureInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewModelCSG::OnViewModelCSG_Tree() 
//Switches the parameter that controls
//the visualization of the model tree of the CSG model
{
	ViewCSG_Tree = !ViewCSG_Tree;

	//Ensure that alternative visualizations are inactive
	if(ViewCSG_Tree){
		DisplayDatums= false;
		DisplayDatumsInSequence= false;
		DisplayProfiles= false;
		DisplayProfilesInSequence= false;
		DisplayDeconstruction= false;
		DisplayDeconstructionInSequence= false;
		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();
	}

	ResetWindow(true);
	Invalidate();
}


void CWinRf_ViewModelCSG::OnUpdateViewModelCSG_Tree(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( ViewCSG_Tree );
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(!mode_CSG_Features && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG &&
				   pDoc->DB_Deconstruct.size() > 0);
}


void CWinRf_ViewModelCSG::OnDisplayDatums()
{
	DisplayDatums = !DisplayDatums;

	//Ensure that alternative visualizations are inactive
	if(DisplayDatums){
		ViewCSG_Tree= false;
		DisplayDatumsInSequence= false;
		DisplayProfiles= false;
		DisplayProfilesInSequence= false;
		DisplayDeconstruction= false;
		DisplayDeconstructionInSequence= false;
		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();
	}

	ResetWindow(true);
	Invalidate();
}


void CWinRf_ViewModelCSG::OnUpdateDisplayDatums(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( DisplayDatums );
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	//pCmdUI->Enable(mode_Features_CSG && 
	//			   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG &&
	//			   pDoc->DB_CSG_Model->DatumPlanesSize() > 0);
	pCmdUI->Enable(!mode_CSG_Features && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG );
}


void CWinRf_ViewModelCSG::OnDisplayDatumsInSequence()
{
	DisplayDatumsInSequence = !DisplayDatumsInSequence;

	//Ensure that alternative visualizations are inactive
	if (DisplayDatumsInSequence){
		ViewCSG_Tree = false;
		DisplayDatums= false;
		DisplayProfiles= false;
		DisplayProfilesInSequence= false;
		DisplayDeconstruction= false;
		DisplayDeconstructionInSequence= false;
		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();

		if(pDoc->DB_CSG_Model->DatumPlanesSize() > 1){
			NumDatumInSequence= 0;
			pPrevNextDatum = new CDialogPreviousNextCSG();
			pPrevNextDatum->SetParams(NumDatumInSequence, 0, 
									  pDoc->DB_CSG_Model->DatumPlanesSize()-1,
									  ID_CSG_PreviousDatumInSequence, 
									  ID_CSG_NextDatumInSequence,
									  0);
		}
	}
	else{
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
	}

	ResetWindow(true);
	Invalidate();
}


void CWinRf_ViewModelCSG::OnUpdateDisplayDatumsInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayDatumsInSequence);
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(!mode_CSG_Features && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG &&
				   pDoc->DB_CSG_Model->DatumPlanesSize() > 0);
}


void CWinRf_ViewModelCSG::OnDisplayNextDatumInSequence()
{
	NumDatumInSequence += 1;
	Invalidate();
}


void CWinRf_ViewModelCSG::OnDisplayPreviousDatumInSequence()
{
	NumDatumInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewModelCSG::OnDisplayProfiles()
{
	DisplayProfiles = !DisplayProfiles;

	//Ensure that alternative visualizations are inactive
	if(DisplayProfiles){
		ViewCSG_Tree= false;
		DisplayDatums= false;
		DisplayDatumsInSequence= false;
		DisplayProfilesInSequence= false;
		DisplayDeconstruction= false;
		DisplayDeconstructionInSequence= false;
		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();
	}

	ResetWindow(true);
	Invalidate();
}


void CWinRf_ViewModelCSG::OnUpdateDisplayProfiles(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( DisplayProfiles );
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	//pCmdUI->Enable(mode_Features_CSG && 
	//			   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG &&
	//			   pDoc->DB_CSG_Model->TreeBranchesSize() > 0);
	pCmdUI->Enable(!mode_CSG_Features && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG );
}


void CWinRf_ViewModelCSG::OnDisplayProfilesInSequence()
{
	DisplayProfilesInSequence = !DisplayProfilesInSequence;

	//Ensure that alternative visualizations are inactive
	if (DisplayProfilesInSequence){
		DisplayDatums= false;
		DisplayDatumsInSequence= false;
		DisplayProfiles= false;
		ViewCSG_Tree = false;
		DisplayDeconstruction= false;
		DisplayDeconstructionInSequence= false;
		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();

		if(pDoc->DB_CSG_Model->TreeFeaturesSize() > 1){
			NumProfileInSequence= 0;
			pPrevNextProfile = new CDialogPreviousNextCSG();
			pPrevNextProfile->SetParams(NumProfileInSequence, 0, 
										pDoc->DB_CSG_Model->TreeFeaturesSize()-1,
										ID_CSG_PreviousProfileInSequence, 
										ID_CSG_NextProfileInSequence,
										0);
		}
	}
	else{
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
	}

	ResetWindow(true);
	Invalidate();
}


void CWinRf_ViewModelCSG::OnUpdateDisplayProfilesInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayProfilesInSequence);
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(!mode_CSG_Features && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG &&
				   pDoc->DB_CSG_Model->TreeFeaturesSize() > 0);
}


void CWinRf_ViewModelCSG::OnDisplayNextProfileInSequence()
{
	NumProfileInSequence += 1;
	Invalidate();
}


void CWinRf_ViewModelCSG::OnDisplayPreviousProfileInSequence()
{
	NumProfileInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewModelCSG::OnDisplayDeconstruction()
{
	DisplayDeconstruction = !DisplayDeconstruction;

	//Ensure that alternative visualizations are inactive
	if(DisplayDeconstruction){
		ViewCSG_Tree= false;
		DisplayDatums= false;
		DisplayDatumsInSequence= false;
		DisplayProfiles= false;
		DisplayProfilesInSequence= false;
		DisplayDeconstructionInSequence= false;
		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();
	}

	ResetWindow(true);
	Invalidate();
}


void CWinRf_ViewModelCSG::OnUpdateDisplayDeconstruction(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( DisplayDeconstruction );
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(!mode_CSG_Features && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG &&
				   pDoc->DB_Deconstruct.size() > 0);
}


void CWinRf_ViewModelCSG::OnDisplayDeconstructionInSequence()
{
	DisplayDeconstructionInSequence = !DisplayDeconstructionInSequence;

	//Ensure that alternative visualizations are inactive
	if (DisplayDeconstructionInSequence){
		ViewCSG_Tree = false;
		DisplayDatums= false;
		DisplayDatumsInSequence= false;
		DisplayProfiles= false;
		DisplayProfilesInSequence= false;
		DisplayDeconstruction= false;
		if(pPrevNextFeature != NULL)
			pPrevNextFeature->DestroyWindow();
		if(pPrevNextDatum != NULL)
			pPrevNextDatum->DestroyWindow();
		if(pPrevNextProfile != NULL)
			pPrevNextProfile->DestroyWindow();
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();

		if(pDoc->DB_Deconstruct.size() > 1){
			NumDeconstructionInSequence= 0;
			pPrevNextDec = new CDialogPreviousNextCSG();
			pPrevNextDec->SetParams(NumDeconstructionInSequence, 0, 
									pDoc->DB_Deconstruct.size()-1,
									ID_CSG_PreviousDeconstructionInSequence, 
									ID_CSG_NextDeconstructionInSequence,
									0);
		}
	}
	else{
		if(pPrevNextDec != NULL)
			pPrevNextDec->DestroyWindow();
	}

	ResetWindow(true);
	Invalidate();
}


void CWinRf_ViewModelCSG::OnUpdateDisplayDeconstructionInSequence(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DisplayDeconstructionInSequence);
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(!mode_CSG_Features && 
				   pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG &&
				   pDoc->DB_Deconstruct.size() > 0);
}


void CWinRf_ViewModelCSG::OnDisplayNextDeconstructionInSequence()
{
	NumDeconstructionInSequence += 1;
	Invalidate();
}


void CWinRf_ViewModelCSG::OnDisplayPreviousDeconstructionInSequence()
{
	NumDeconstructionInSequence -= 1;
	Invalidate();
}


void CWinRf_ViewModelCSG::OnDisplayZoomReset()
// Recalculates Zoom and Pan to obtain a full size view
{
	if (!mode_CSG_Features){
		ResetWindow(true);
		Invalidate();
	}
	else{
		OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom = 1.0;
		Invalidate();
	}
}


void CWinRf_ViewModelCSG::OnUpdateDisplayZoomReset(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	//pCmdUI->Enable(pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG);
	pCmdUI->Enable(true);
}


void CWinRf_ViewModelCSG::OnConfigDisplayCSG()
// Recalculates Zoom and Pan to obtain a full size view
{
	CDialogConfigDisplayCSG Dlg;
	Dlg.SetParams(CSG_Tree_ScaleLabels,
				  CSG_Tree_DisplayNumOfBranches, 
				  CSG_Tree_DisplayNamesOfFeatures, 
				  CSG_Tree_DisplayMeritsOfFeatures, 
				  CSG_Tree_Columns,
				  CSG_Datums_ScaleLabels,
				  CSG_Datums_DisplayGraph,
				  CSG_Datums_DisplayNumVertices,
				  CSG_Datums_DisplayCoordsVertices,
				  CSG_Datums_DisplayNumEdges,
				  CSG_Datums_DisplayInputDrawing,
				  CSG_Datums_DisplayNumOfDatums,
				  CSG_Datums_DisplayDatumSymmetry,
				  CSG_Dec_ScaleLabels,
				  CSG_Dec_DisplayLabelDeconstruction, 
				  CSG_Dec_DisplayNumVertices,
				  CSG_Dec_DisplayCoordsVertices,
				  CSG_Dec_DisplayNumEdges,
				  CSG_Dec_Columns);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(CSG_Tree_ScaleLabels,
					  CSG_Tree_DisplayNumOfBranches, 
					  CSG_Tree_DisplayNamesOfFeatures, 
					  CSG_Tree_DisplayMeritsOfFeatures, 
					  CSG_Tree_Columns,
					  CSG_Datums_ScaleLabels,
					  CSG_Datums_DisplayGraph,
					  CSG_Datums_DisplayNumVertices,
					  CSG_Datums_DisplayCoordsVertices,
					  CSG_Datums_DisplayNumEdges,
					  CSG_Datums_DisplayInputDrawing,
					  CSG_Datums_DisplayNumOfDatums,
					  CSG_Datums_DisplayDatumSymmetry,
					  CSG_Dec_ScaleLabels,
					  CSG_Dec_DisplayLabelDeconstruction, 
					  CSG_Dec_DisplayNumVertices,
					  CSG_Dec_DisplayCoordsVertices,
					  CSG_Dec_DisplayNumEdges,
					  CSG_Dec_Columns);

		Invalidate();
	}
}


void CWinRf_ViewModelCSG::OnUpdateConfigDisplayCSG(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	pCmdUI->Enable(pFrame->MDIGetActive() == pDoc->m_pFrameModelCSG);
}


////////////////////// MOUSE ////////////////////////////


// TRACKBALL SIMULATED WITH MOUSE/PEN MOVEMENT BETWEEN LEFT BUTTON DOWN AND UP

void CWinRf_ViewModelCSG::OnLButtonDown(UINT nFlags, CPoint point) 
// Starts capture of mouse movement, to determine rotation axis and speed
{
	if (!mode_CSG_Features){
	}

	else{
		SetCapture();
		SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandClosed));

		// Activa un Timer para ver como se mueve el raton
		if (m_EstateTimer != Inactive) {
			BOOL Resultado = KillTimer( m_Timer );
			ASSERT( Resultado != 0 );
		};
		m_Timer = SetTimer(1, 100, NULL);
		ASSERT( m_Timer != 0 );
		m_EstateTimer = CalculatingSpeed;
		m_dwMilliseconds = GetTickCount();

		m_PosMouseFromTimer = point;
		m_PosMouseInitial = point;
		m_PosMousePrev = point;

		OpenGLViewModelCSG->Camera_CSG.PosMousePrev = OpenGLViewModelCSG->Camera_CSG.PosMouseCurrent = point;
		OpenGLViewModelCSG->Camera_CSG.IncrementPosMouse = CSize(0,0);
	}

	CScrollView::OnLButtonDown(nFlags, point);
}


void CWinRf_ViewModelCSG::OnLButtonUp(UINT nFlags, CPoint point) 
// Ends capture of mouse movement, and determines rotation axis and speed
{
	if (!mode_CSG_Features){
	}

	else{
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

				OpenGLViewModelCSG->Camera_CSG.dIncrementAngleX = ((float)(point.y - m_PosMouseInitial.y) / (float)dwTiempo) * 50.0;
				OpenGLViewModelCSG->Camera_CSG.dIncrementAngleY = ((float)(point.x - m_PosMouseInitial.x) / (float)dwTiempo) * 50.0;

				m_EstateTimer = SelfRotation;
				m_Timer = SetTimer(1, 50, NULL);
				ASSERT( m_Timer != 0 );
			}
		};

		ReleaseCapture();
		SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));
	}

	CScrollView::OnLButtonUp(nFlags, point);
}


void CWinRf_ViewModelCSG::OnTimer(UINT nIDEvent) 
{
	if (!mode_CSG_Features){
	}

	else{
		switch (m_EstateTimer) {
		case CalculatingSpeed:
			m_PosMouseFromTimer = m_PosMousePrev;
			break;

		case SelfRotation: 
			//Mandatory to control small movements
			OpenGLViewModelCSG->Camera_CSG.PosMouseCurrent.x += (long)OpenGLViewModelCSG->Camera_CSG.dIncrementAngleY;
			OpenGLViewModelCSG->Camera_CSG.PosMouseCurrent.y += (long)OpenGLViewModelCSG->Camera_CSG.dIncrementAngleX;

			Invalidate();
			break;
		};
	}

	CScrollView::OnTimer(nIDEvent);
}



// ZOOM AND PAN CONTROL


void CWinRf_ViewModelCSG::OnMButtonDown(UINT nFlags, CPoint point) 
// Starts a pan
// The operations are controlled in "OnMouseMove"
{
	if (!mode_CSG_Features){
		lastPoint = point;
		SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Pan));
		MouseModeFeatures=PanModeFeaturesCSG;

		SetCapture();
	}

	CScrollView::OnMButtonDown(nFlags, point);
}


void CWinRf_ViewModelCSG::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if (!mode_CSG_Features){
		//if (ReferApp.SketchMode)
		//	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Arrow));
		//else
		//	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));

		MouseModeFeatures = InactiveModeFeaturesCSG;

		ReleaseCapture();
		Invalidate();
	}

	CScrollView::OnMButtonUp(nFlags, point);
}


void CWinRf_ViewModelCSG::OnRButtonDown(UINT nFlags, CPoint point) 
// Starts a pan (or a zoom if MK_CONTROL is clicked too)
// The operations are controlled in "OnMouseMove"
{
	if (!mode_CSG_Features){
		lastPoint = point;

		if (nFlags & MK_CONTROL){
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));
			MouseModeFeatures=ZoomModeFeaturesCSG;
		}
		else{
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Pan));
			MouseModeFeatures=PanModeFeaturesCSG;
		}

		SetCapture();
	}

	else{
		SetCapture();
		SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));

		m_PosMouseInitial = point;
		m_PosMousePrev = point;
	}

	CScrollView::OnRButtonDown(nFlags, point);
}


void CWinRf_ViewModelCSG::OnRButtonUp(UINT nFlags, CPoint point) 
// Ends pan (or zoomn if MK_CONTROL was clicked too)
{
	if (!mode_CSG_Features){
		//if (ReferApp.SketchMode)
		//	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Arrow));
		//else
		//	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));

		MouseModeFeatures = InactiveModeFeaturesCSG;

		ReleaseCapture();
		Invalidate();
	}

	else{
	ReleaseCapture();
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));
	}

	CScrollView::OnRButtonUp(nFlags, point);
}


// ZOOM VARIATION BY MOUSE WHEEL ROTATION


void CWinRf_ViewModelCSG::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!mode_CSG_Features){
		if(GetCapture() != this) return;

		//Create a local device context,
		//to force flushing the drawing commands when it goes out of scope
		CClientDC pDC(this);

		switch (MouseModeFeatures){

		case ZoomModeFeaturesCSG:{
			//Update zoom
			CameraZoom += (double)(point.y-lastPoint.y)/100;

			//Update cursor position
			lastPoint=point;

			//Invalidate to ensure re-painting
			Invalidate();
			}
			break;

		case PanModeFeaturesCSG:{
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

		}
	}

	else{
		if ( GetCapture() != this ) {
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));
			ShowCursor(true);
			return;
		};
		ShowCursor(true);

		if ( nFlags & MK_RBUTTON ) {
			// Update Zoom following the vertical displacement of the mouse
			long IncrementZoom = (point.y - m_PosMousePrev.y);

			OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom += (float) IncrementZoom / 50.0;
			if ( OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom < 0.2 )
				OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom = 0.2;
			if ( OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom > 2 )
				OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom = 2;
		}
		else {
			//Mandatory to control small movements
			OpenGLViewModelCSG->Camera_CSG.PosMouseCurrent = point;
		};

		m_PosMousePrev = point;
		Invalidate();
	}

	CScrollView::OnMouseMove(nFlags, point);
}


BOOL CWinRf_ViewModelCSG::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
//The framework calls this member function if mouse input is not captured
//and the mouse causes cursor movement within the CWnd object.

// This function must be overriden
// to select the suitable icon
// and to prevent cursor icon from flickering
{
	if (!mode_CSG_Features){
		if(MouseModeFeatures == PanModeFeaturesCSG)
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Pan));
		else if(MouseModeFeatures == ZoomModeFeaturesCSG)
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));
		else
			SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Arrow));
	}

	else{
		//return CScrollView::OnSetCursor(pWnd, nHitTest, message);
	}

	return true;
}


BOOL CWinRf_ViewModelCSG::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
//Captures wheel rotation and determines zoom variation

//The zDelta value is expressed in multiples or divisions of WHEEL_DELTA, which is 120
{
	if (!mode_CSG_Features){
		SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));
		MouseModeFeatures=ZoomModeFeaturesCSG;

		CameraZoom += ((double)zDelta / 1200);
	}
	else{
		OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom += ((double)zDelta / 120.0) / 10.0;
		if ( OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom < 0.2 )  OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom = 0.2;
		if ( OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom > 2 )	 OpenGLViewModelCSG->VolumeDDBB_CSG.dZoom = 2;
	}

	Invalidate();

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

////////////////////// END MOUSE ////////////////////////////
