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

// DialogDetailsMainAxes.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DialogMainAxesRules.h"

#include "WinRf_GDI_Transform.h"
#include "DialogMainAxesManual.h"

#include "DialogDetailsMainAxes.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsMainAxes dialog


CDialogDetailsMainAxes::CDialogDetailsMainAxes(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsMainAxes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsMainAxes)
	//}}AFX_DATA_INIT
}


void CDialogDetailsMainAxes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsMainAxes)
	DDX_Radio(pDX, IDC_ApproachMainAxes, m_ApproachMainAngles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsMainAxes, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsMainAxes)
	ON_BN_CLICKED(IDC_MainAxesRules, OnMainAxesRules)
	ON_BN_CLICKED(IDC_MainAxesManual, OnMainAxesManual)
	ON_BN_CLICKED(IDC_BUTTON_DetailsMainAxesDefault, OnBUTTONDetailsMainAxesDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsMainAxes message handlers


void CDialogDetailsMainAxes::SetParams(CDataMainAxes MA)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ApproachMainAngles = MA.MainAxesApproach;
}


void CDialogDetailsMainAxes::GetParams(CDataMainAxes &MA)
// Updates the general parameters that have been modified in the dialog
{
	MA.MainAxesApproach=m_ApproachMainAngles;
}

void CDialogDetailsMainAxes::OnBUTTONDetailsMainAxesDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataMainAxes initial;
	SetParams(initial);
	UpdateData(false);
}

void CDialogDetailsMainAxes::OnMainAxesRules() 
{
	CDialogMainAxesAutomatic Dlg;
	Dlg.SetParams(ReferApp.Cu.MA);
	if (Dlg.DoModal()==IDOK)
		Dlg.GetParams(ReferApp.Cu.MA);
}


//void CDialogDetailsMainAxes::OnMainAxesManual() 
////Launches a dialog to select three edges parallel to the three main axes
//{
//	CDialogMainAxesManual Dlg;
//	Dlg.SetParams();
//	if (Dlg.DoModal()==IDOK)
//		Dlg.GetParams();
//}


void CDialogDetailsMainAxes::OnMainAxesManual() 
//Sends a message to request the currently selected edges
//If exactly 3 edges are selected, they are considered to be
//parallel to the three main axes

//Otherwise, a warning is displayed, and none axes are selected

//Lines may be selected in advance (by selecting their strokes)
//or may be interactively selected while the dialog is in progress
{
	std::vector<long> Selected_strokes;
	CReferDoc::GetDoc()->m_pFrameSketch2D->GetActiveView()->SendMessage(WM_GET_SELECTION,(WPARAM) &Selected_strokes, 0);

	if (Selected_strokes.size() == 3){
		//This is an alternative code
		//It prevents from needing CDialogMainAxesManual
		//But it selects STROKES instead of EDGES
		ReferApp.Cu.MA.UserMainAxes.clear();
		ReferApp.Cu.MA.UserMainAxes.push_back(Selected_strokes[0]);
		ReferApp.Cu.MA.UserMainAxes.push_back(Selected_strokes[1]);
		ReferApp.Cu.MA.UserMainAxes.push_back(Selected_strokes[2]);
	}

	else{
		//Launches a dialog to select three edges parallel to the three main axes
		CDialogMainAxesManual Dlg;
		Dlg.SetParams();
		if (Dlg.DoModal()==IDOK)
			Dlg.GetParams();
		else
			AfxMessageBox("Main axes not found:\r\n Exactly three strokes must have been selected in advance, or three edges must have been selected interactively! (Vectorize first if line-drawing does not display)");
	}
}
