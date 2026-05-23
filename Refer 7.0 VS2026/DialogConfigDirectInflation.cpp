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

// DialogConfigDirectInflation.cpp : implementation file

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DialogDetailsPreInflateAutomatic.h"
#include "DialogDetailsPreInflateAxonometric.h"
#include "DialogDetailsLevels.h"
#include "DialogDetailsLevelsHidden.h"

#include "DialogConfigDirectInflation.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDirectInflation dialog


CDialogConfigDirectInflation::CDialogConfigDirectInflation(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigDirectInflation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigDirectInflation)
	//}}AFX_DATA_INIT
}


void CDialogConfigDirectInflation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigDirectInflation)
	DDX_Radio(pDX, IDC_DIRECTINFLATION_AUTOMATIC, m_DirectInflationApproach);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigDirectInflation, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigDirectInflation)
	ON_BN_CLICKED(IDC_DetailsAutomaticDirectInflation, OnDetailsAutomaticDirectInflation)
	ON_BN_CLICKED(IDC_DetailsDirectInflationAxonometric, OnDetailsDirectInflateAxonometric)
	ON_BN_CLICKED(IDC_DetailsDirectInflationLevels, OnDetailsDirectInflateLevels)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDirectInflation message handlers


BOOL CDialogConfigDirectInflation::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(false);

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogConfigDirectInflation::SetParams()
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_DirectInflationApproach = (int)ReferApp.DI.DirectInflationApproach;
}


void CDialogConfigDirectInflation::GetParams()
// Updates the general parameters that have been modified in the dialog
{
	ReferApp.DI.DirectInflationApproach= (DIRINFLATION)m_DirectInflationApproach;
}


void CDialogConfigDirectInflation::OnBUTTONRestoreDefault()
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	SetParams();

	UpdateData(false);
}


void CDialogConfigDirectInflation::OnDetailsAutomaticDirectInflation()
{
	CDialogDetailsPREINFLATEAutomatic Dlg;
	Dlg.SetParams(ReferApp.DI);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.DI);
	}
}


void CDialogConfigDirectInflation::OnDetailsDirectInflateAxonometric()
{
	CDialogDetailsPREINFLATEAxonometric Dlg;
	Dlg.SetParams(ReferApp.DIA);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.DIA);
	}
}


void CDialogConfigDirectInflation::OnDetailsDirectInflateLevels()
{
	UpdateData(true);
	if (!ReferApp.GeneralParams.DetectHiddenEdges){
		CDialogDetailsLevels Dlg;
		Dlg.SetParams(ReferApp.DIL);
		if (Dlg.DoModal() == IDOK)
			Dlg.GetParams(ReferApp.DIL);
	}
	else{
		CDialogDetailsLevelsHidden DlgO;
		DlgO.SetParams(ReferApp.DIL);
		if (DlgO.DoModal() == IDOK)
			DlgO.GetParams(ReferApp.DIL);
	}
}
