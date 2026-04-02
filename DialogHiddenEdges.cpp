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

// DialogHiddenEdges.cpp : implementation file
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

#include "DialogHiddenEdges.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogHiddenEdges dialog


CDialogHiddenEdges::CDialogHiddenEdges(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogHiddenEdges::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogHiddenEdges)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogHiddenEdges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogHiddenEdges)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogHiddenEdges, CDialog)
	//{{AFX_MSG_MAP(CDialogHiddenEdges)
	ON_BN_CLICKED(IDC_CHECK_Anterior, OnCHECKPrevious)
	ON_BN_CLICKED(IDC_CHECK_Siguiente, OnCHECKNext)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogHiddenEdges message handlers

void CDialogHiddenEdges::SetParams(std::vector<std::vector<bool>> visible,
								   long SelectedVariation)
//Produces local versions of those general parameters that can be modified by this dialog
{
	//Initialize global variables of the class
	m_visible= visible;
	m_SelectedVariation= SelectedVariation;

	//Pointer to the document that contains the model
	CReferDoc* pDoc= CReferDoc::GetDoc();
	DDBB= pDoc->DB_DrawingOff;
}


void CDialogHiddenEdges::GetParams(long &SelectedVariation)
// Updates the general parameters that have been modified in the dialog
{
	SelectedVariation= m_SelectedVariation;
}


BOOL CDialogHiddenEdges::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CButton *pBoton;

	//Deactivates button previous
	pBoton = (CButton*) GetDlgItem(IDC_CHECK_Anterior); //User
	pBoton->EnableWindow(false);

	//Deactivates button next, if only one variation is available
	if ((m_visible.size()==1)){
		pBoton = (CButton*) GetDlgItem(IDC_CHECK_Siguiente);
		pBoton->EnableWindow(false);
	}

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogHiddenEdges::OnCHECKPrevious()
{
	CButton *pBoton;
	m_SelectedVariation--;

	//If current variation is the first variation, deactivates button prev
	if (m_SelectedVariation == 0){
		pBoton = (CButton*) GetDlgItem(IDC_CHECK_Anterior);
		pBoton->EnableWindow(false);
	}

	//Activates the next button (since current variation cannot be the last one)
	pBoton = (CButton*) GetDlgItem(IDC_CHECK_Siguiente);
	pBoton->EnableWindow(true);

	Invalidate();
}


void CDialogHiddenEdges::OnCHECKNext()
{
	CButton *pBoton;
	m_SelectedVariation++;

	//If current variation is the last variation, deactivates button next
	if ((m_visible.size()-1)){
		pBoton = (CButton*) GetDlgItem(IDC_CHECK_Siguiente); //User
		pBoton->EnableWindow(false);
	}

	//Activates the prev button (since current variation cannot be the first one)
	pBoton = (CButton*) GetDlgItem(IDC_CHECK_Anterior); //User
	pBoton->EnableWindow(true);

	Invalidate();
}


void CDialogHiddenEdges::OnPaint()
{
	CPaintDC dc(this); //Removes WM_PAINT message from the queue

	//Draw the model with candidate hidden edges
	GDI_DrawSketch2D();
}


void CDialogHiddenEdges::GDI_DrawSketch2D()
//Draw the model with candidate hidden edges
//inside the control IDC_MODELO_HIDDEN
{
	//Prepare the drawing area
	CStatic* pStatic = (CStatic*) GetDlgItem(IDC_MODELO_HIDDEN);
	CRect rect;
	pStatic->GetClientRect(&rect);
	CClientDC dc(pStatic);

	//Pointer to the document that contains the BackgroundColor
	dc.FillSolidRect(rect, ReferApp.m_BackgroundColor);

	//Prepare the Window-Viewport transformation
	cGDI_Transform Transf;
	Transf.GDI_SetViewport(10, 40, rect.right-10, rect.bottom-10);

	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	Transf.GDI_SetWorld(dXmin, dYmin, dXmax, dYmax);

	//Prepare Pens and Fonts to draw the graphic
	CFont Fuente;
	Fuente.CreatePointFont( 10*10, "Arial", &dc);
	dc.SelectObject(&Fuente);
	if (ReferApp.m_BackgroundColor == RGB(255, 255, 255))
		dc.SetTextColor(RGB( 0, 0, 0));
	else
		dc.SetTextColor(RGB( 255, 255, 255));

	dc.SetTextAlign(TA_CENTER | TA_BASELINE);

	CString sTexto, sTexto1, sTexto2;
	sTexto= "Model ";
	sTexto1.Format( "%i", m_SelectedVariation+1);
	sTexto2.Format( "%i", (long)m_visible.size());
	sTexto+=sTexto1;
	sTexto1= " of ";
	sTexto= sTexto + sTexto1 + sTexto2;
	dc.TextOut( rect.right/2, 15, sTexto );

	//Draw the visible and hidden edges
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge edge= DDBB->GetEdge(i);
		CVertex Vertice1= DDBB->GetVertex(edge.Head);
		CVertex Vertice2= DDBB->GetVertex(edge.Tail);

		//Transform world into viewport coordinates
		CPoint P1 = Transf.GDI_WorldToViewport(Vertice1.x, Vertice1.y);
		CPoint P2 = Transf.GDI_WorldToViewport(Vertice2.x, Vertice2.y);

		//Select the pen
		CPen Pluma;
		if (m_visible[m_SelectedVariation][i])
			Pluma.CreatePen( PS_SOLID, 1, RGB(0, 0, 0));	// if visible
		else
			Pluma.CreatePen( PS_DASH, 1, RGB(0, 255, 0));	// if hidden
		dc.SelectObject( &Pluma );

		//Draw the line
		dc.MoveTo(P1.x, P1.y);
		dc.LineTo(P2.x, P2.y);
	}
}