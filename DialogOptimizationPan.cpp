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

// DialogOptimizationPan.cpp : implementation file
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

#include "DialogOptimizationPan.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogOptimizationPan dialog


CDialogOptimizationPan::CDialogOptimizationPan(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogOptimizationPan::IDD, pParent)
{
	Create (IDD);

	//{{AFX_DATA_INIT(CDialogOptimizationPan)
	//}}AFX_DATA_INIT
}


void CDialogOptimizationPan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogOptimizationPan)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogOptimizationPan, CDialog)
	//{{AFX_MSG_MAP(CDialogOptimizationPan)
	ON_BN_CLICKED(IDC_BUTTON_DERECHA, OnButtonDerecha)
	ON_BN_CLICKED(IDC_BUTTON_INFERIOR, OnButtonInferior)
	ON_BN_CLICKED(IDC_BUTTON_IZQUIERDA, OnButtonIzquierda)
	ON_BN_CLICKED(IDC_BUTTON_SUPERIOR, OnButtonSuperior)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogOptimizationPan message handlers

void CDialogOptimizationPan::SetParams(CParamsIsolines* pParamsIsolines_External)
// Produces local versions of those general parameters that can be modified by this dialog
{
	pParamsIsolines= pParamsIsolines_External;
}

void CDialogOptimizationPan::OnButtonDerecha() 
{
	double Desplazamiento =std::fabs(pParamsIsolines->XF-pParamsIsolines->X0)/3;

	pParamsIsolines->X0+=Desplazamiento;
	pParamsIsolines->XF+=Desplazamiento;

	CReferDoc::GetDoc()->m_pFrameOptimization->GetActiveView()->Invalidate();  //Message to redraw the window

}

void CDialogOptimizationPan::OnButtonIzquierda() 
{
	double Desplazamiento =std::fabs(pParamsIsolines->XF-pParamsIsolines->X0)/3;

	pParamsIsolines->X0-=Desplazamiento;
	pParamsIsolines->XF-=Desplazamiento;

	CReferDoc::GetDoc()->m_pFrameOptimization->GetActiveView()->Invalidate();  //Message to redraw the window
}

void CDialogOptimizationPan::OnButtonInferior() 
{
	double Desplazamiento =std::fabs(pParamsIsolines->YF-pParamsIsolines->Y0)/3;

	pParamsIsolines->Y0-=Desplazamiento;
	pParamsIsolines->YF-=Desplazamiento;

	CReferDoc::GetDoc()->m_pFrameOptimization->GetActiveView()->Invalidate();  //Message to redraw the window
}


void CDialogOptimizationPan::OnButtonSuperior() 
{
	double Desplazamiento =std::fabs(pParamsIsolines->YF-pParamsIsolines->Y0)/3;

	pParamsIsolines->Y0+=Desplazamiento;
	pParamsIsolines->YF+=Desplazamiento;

	CReferDoc::GetDoc()->m_pFrameOptimization->GetActiveView()->Invalidate();  //Message to redraw the window
}
