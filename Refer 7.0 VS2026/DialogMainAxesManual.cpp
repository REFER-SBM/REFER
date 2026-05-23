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

// DialogMainAxesManual.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "ReferDoc.h"

#include "WinRf_GDI_Transform.h"

#include "DialogMainAxesManual.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogMainAxesManual dialog


CDialogMainAxesManual::CDialogMainAxesManual(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMainAxesManual::IDD, pParent)
{
	//Initialize local database
	DDBB= CReferDoc::GetDoc()->DB_DrawingOff;

	if (DDBB->GetSizeVertices() == 0)
		AfxMessageBox("Cannot select interactively user main axes because sketch was not yet vectorized!");

}


CDialogMainAxesManual::~CDialogMainAxesManual()
{
}


void CDialogMainAxesManual::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogMainAxesManual, CDialog)
	//{{AFX_MSG_MAP(CDialogMainAxesManual)
	ON_WM_PAINT()

	ON_BN_CLICKED(IDC_ClearMainAxes, OnClearSelection)

	//Mouse
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	//ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMainAxesManual message handlers

void CDialogMainAxesManual::SetParams()
// Produces local versions of those general parameters that can be modified by this dialog
{
	Selected_axis= (long)ReferApp.Cu.MA.UserMainAxes.size();
	if (Selected_axis > 3){
		AfxMessageBox("Invalid previous user main directions selection!");
		OnClearSelection();
	}
	else{
		for (long i=0; i<Selected_axis; i++){
			Selection[i]= ReferApp.Cu.MA.UserMainAxes[i];
		}
	}
}


void CDialogMainAxesManual::GetParams()
// Updates the general parameters that have been modified in the dialog
{
	if (Selected_axis<3 && Selected_axis>0)
		AfxMessageBox("Less than 3 axis selected!");

	ReferApp.Cu.MA.UserMainAxes.clear();

	for (long i=0; i<Selected_axis; i++){
		if (Selection[i]!=-1){
			//Add the edge to the list of main directions
			ReferApp.Cu.MA.UserMainAxes.push_back(Selection[i]);
		}
	}
}


void CDialogMainAxesManual::OnClearSelection()
{
	//Reset the list of main directions
	ReferApp.Cu.MA.UserMainAxes.clear();

	//Initialize selection parameters
	for (long i=0; i<3; i++){
		Selection[i]= -1;
	}
	Selected_axis= 0;

	Invalidate();
}


BOOL CDialogMainAxesManual::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Calculates initial Zoom and Pan to obtain a full size view
	ResetViewport();
	ResetWindow();

	return true;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogMainAxesManual::ResetViewport()
// Updates the Viewport transformation
{
	CStatic* pStatic;
	pStatic = (CStatic*) GetDlgItem(IDC_MAM_VIEW);

	CRect rect;

	//Displacement from Dialog into IDC_MAM_VIEW control coordinates
	pStatic->GetWindowRect(&rect);
	ScreenToClient(&rect);
	DeltaX= rect.left;
	DeltaY= rect.top;

	//Viewport coordinates
	pStatic->GetClientRect(&rect);

	if (rect.right== 0 || rect.bottom== 0) return;	 //no client area

	ViewportWide= rect.right - rect.left;
	ViewportHigh  = rect.bottom - rect.top;

	Transf.GDI_SetViewport(rect.left, rect.top, rect.right, rect.bottom);
}


void CDialogMainAxesManual::Get2DModelBoundingBox(double &wXmin, double &wYmin,
												  double &wXmax, double &wYmax)
// Recalculates the dimensions of the drawing, depending on the type of the 2D model
{
	wXmin= wYmin= wXmax= wYmax= 0;

	//Case of dxf 2D line-drawing
	if(DDBB->GetSizeVertices() > 0){
		double wZmin= 0, wZmax= 0;
		DDBB->GetBoundingBox(wXmin, wYmin, wZmin, wXmax, wYmax, wZmax);
	}

	//Case of null 2D model
	if (wXmin-wXmax== 0 && wYmin-wYmax== 0){
		//empty drawing
		CRect rect;
		GetClientRect(&rect);

		//Assign the viewport as window
		wXmin= rect.left;
		wXmax= rect.right;
		wYmin= rect.top;
		wYmax= rect.bottom;
	}
}


void CDialogMainAxesManual::UpdateWindow()
// Updates the pan and zoom in the current window
{
	Transf.GDI_SetWorld(-WindowLength/2 * CameraZoom - CameraPanX,
						-WindowHigh/2   * CameraZoom - CameraPanY,
						+WindowLength/2 * CameraZoom - CameraPanX,
						+WindowHigh/2   * CameraZoom - CameraPanY);
}


void CDialogMainAxesManual::ResetWindow()
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


void CDialogMainAxesManual::OnPaint()
{
	CPaintDC dc(this); // Removes WM_PAINT message from the queue

	//Update window to ensure that pan and zoom
	//are updated after mouse movements
	UpdateWindow();

	// Draw the model plus the selected and not selected edges
	// inside the control IDC_MAM_VIEW
	GDI_DrawSketch2D();
}


void CDialogMainAxesManual::GDI_DrawSketch2D()
//Draws the 2D sketch and marks the selected edges
//inside the control IDC_MAM_VIEW
{
	//Pointer to the IDC_MAM_VIEW control
	CStatic* pStatic;
	pStatic = (CStatic*) GetDlgItem(IDC_MAM_VIEW);

	//Clear the drawing area
	CRect rect;
	pStatic->GetClientRect(&rect);
	CClientDC dc(pStatic);
	dc.FillSolidRect(rect, ReferApp.m_BackgroundColor);

	//Clip the drawing outside the IDC_MAM_VIEW control
	HRGN hrgn = CreateRectRgn(rect.left, rect.bottom, 
							  rect.right, rect.top); 
	SelectClipRgn(dc, hrgn); 

	//Draw the model, distinguishing between selected and not selected edges
	for (long i=0; i<DDBB->GetSizeEdges(); i++ ){
		CEdge Arista= DDBB->GetEdge(i);

		CVertex Vertice1= DDBB->GetVertex(Arista.Head);
		CVertex Vertice2= DDBB->GetVertex(Arista.Tail);

		//Transform world into viewport coordinates
		CPoint Punto1 = Transf.GDI_WorldToViewport(Vertice1.x, Vertice1.y);
		CPoint Punto2 = Transf.GDI_WorldToViewport(Vertice2.x, Vertice2.y);

		//Select the pen
		CPen Pluma;
		if (i==Selection[0] ||
			i==Selection[1] ||
			i==Selection[2])
			Pluma.CreatePen( PS_SOLID, 4, RGB(0, 0, 255));  // if selected
		else
			Pluma.CreatePen( PS_SOLID, 1, RGB(0, 0, 0));	// if not selected
		dc.SelectObject(&Pluma);

		// Draw the line
		dc.MoveTo( Punto1.x, Punto1.y );
		dc.LineTo( Punto2.x, Punto2.y );
	}
}


////////////////////// MOUSE ////////////////////////////


void CDialogMainAxesManual::OnLButtonDown(UINT nFlags, CPoint point)
// Picks edges to select main axes directions
// (Select edges that will be considered parallel to the desired main axes)
{
	//SetCursor(ReferApp.LoadCursor(IDC_CURSOR_SeleccionMasMenos));

	//Transform point from Dialog into IDC_MAM_VIEW control coordinates
	point.x -= DeltaX;
	point.y -= DeltaY;

	//Finds the edge (if any) crossing a tolerance rectangle around the "point"
	long Selec= -1;
	Selec= SelectEdge(point);

	//If some edge has been selected, add it to the list of selected axes
	if (Selec!=-1){
		//Restart if three edges already existed
		if (Selected_axis>=3)
			OnClearSelection();

		Selection[Selected_axis]= Selec;
		Selected_axis += 1;

		Invalidate();
	}

	CWnd::OnLButtonDown(nFlags, point);
}

long CDialogMainAxesManual::SelectEdge(CPoint point)
// Selects the first edge overlapped by the tolerance rectangle around "point"
// Returns the number of the selected edge, or -1 if nothing is selected
{
	//Tolerance rectangle around "point"
	double LimitLeft, LimitRight, LimitBottom, LimitTop;
	long Tolerance= 6; //Tolerance rectangle size in Pixels
	LimitLeft  = (double) (point.x-Tolerance);
	LimitRight = (double) (point.x+Tolerance);
	LimitBottom= (double) (point.y-Tolerance);
	LimitTop   = (double) (point.y+Tolerance);

	//Search for the first edge overlapped by the rectangle
	CPoint Punto1, Punto2;

	for (long IndiceArista=0; IndiceArista<DDBB->GetSizeEdges(); IndiceArista++ ){
		CEdge Arista= DDBB->GetEdge(IndiceArista);

		CVertex Vertice1= DDBB->GetVertex(Arista.Head);
		CVertex Vertice2= DDBB->GetVertex(Arista.Tail);

		//Transform world into viewport coordinates
		Punto1 = Transf.GDI_WorldToViewport(Vertice1.x, Vertice1.y);
		Punto2 = Transf.GDI_WorldToViewport(Vertice2.x, Vertice2.y);

		if(LiangBarsky (LimitLeft, LimitRight, LimitBottom, LimitTop,
						(double) Punto1.x, (double) Punto1.y,
						(double) Punto2.x, (double) Punto2.y)){
			return IndiceArista;
		}
	}
	return -1; //no selection
}


bool CDialogMainAxesManual::LiangBarsky (double LimitLeft, double LimitRight,
										 double LimitBottom, double LimitTop,   // Rectangle
										 double x0src, double y0src,
										 double x1src, double y1src)			// Segment
// This function inputs one rectangle and one segment,
// and outputs true if the segment intersects the rectangle, and false otherwise
// (Adapted from: Liang-Barsky function by Daniel White @ http://www.skytopia.com/project/articles/compsci/clipping.html)
{
	double t0 = 0.0;    double t1 = 1.0;
	double xdelta = x1src-x0src;
	double ydelta = y1src-y0src;
	double p,q,r;

	for(int edge=0; edge<4; edge++) {   // Traverse through left, right, bottom, top edges.
		if (edge==0) {  p = -xdelta;    q = -(LimitLeft-x0src);  }
		if (edge==1) {  p = xdelta;     q =  (LimitRight-x0src); }
		if (edge==2) {  p = -ydelta;    q = -(LimitBottom-y0src);}
		if (edge==3) {  p = ydelta;     q =  (LimitTop-y0src);   }   
		if(p==0 && q<0) return false;   // The rectangle does not touch the segment at all

		r = q/p;
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


// Zoom and pan control++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CDialogMainAxesManual::OnRButtonDown(UINT nFlags, CPoint point)
// Starts a pan (or a zoom if MK_CONTROL is clicked too)
// The operations are controlled in "OnMouseMove"
{
	// Transform point from Dialog into IDC_MAM_VIEW control coordinates
	//point.x -= position.x;
	//point.y -= position.y;

	//lastPoint = point-position;
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

	CWnd::OnRButtonDown(nFlags, point);
}


void CDialogMainAxesManual::OnRButtonUp(UINT nFlags, CPoint point) 
// Ends pan (or zoomn if MK_CONTROL was clicked too)
{
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));
	MouseMode = InactiveMode;
	ReleaseCapture();

	CWnd::OnRButtonUp(nFlags, point);
}


void CDialogMainAxesManual::OnMButtonDown(UINT nFlags, CPoint point) 
// Starts a pan
// The operations are controlled in "OnMouseMove"
{
	lastPoint = point;
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_Zoom));
	MouseMode=ZoomMode;
	SetCapture();

	CWnd::OnMButtonDown(nFlags, point);
}


void CDialogMainAxesManual::OnMButtonUp(UINT nFlags, CPoint point) 
// Ends pan
{
	SetCursor(ReferApp.LoadCursor(IDC_CURSOR_HandOpen));
	MouseMode = InactiveMode;
	ReleaseCapture();

	CWnd::OnMButtonUp(nFlags, point);
}


void CDialogMainAxesManual::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(GetCapture() != this)
		return;

	switch (MouseMode){
	case ZoomMode:
		CameraZoom += (double)(point.y-lastPoint.y)/100;
		lastPoint=point;
		break;
	case PanMode:
		CameraPanX +=(double)(point.x-lastPoint.x)*(CameraZoom * WindowLength/ViewportWide);
		CameraPanY +=(double)(lastPoint.y-point.y)*(CameraZoom * WindowHigh/ViewportHigh);
		lastPoint=point;
		break;
	}

	CWnd::OnMouseMove(nFlags, point);
	Invalidate();
}


// ZOOM VARIATION BY MOUSE WHEEL ROTATION

//BOOL CDialogMainAxesManual::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
////Captures wheel rotation and determines zoom variation
//{
//	CameraZoom += ((double)zDelta / 120.0) / 10.0;
//
//	Invalidate();
//	
//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
//}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


////////////////////// MOUSE ////////////////////////////
