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

// DialogConfigInflation.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "ReferDoc.h"

#include "DialogConfigDirectInflation.h"
#include "DialogConfigOptimizationInflation.h"

#include "DialogConfigInflation.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigInflation dialog


CDialogConfigInflation::CDialogConfigInflation(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigInflation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigInflation)
	//}}AFX_DATA_INIT
}


void CDialogConfigInflation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigInflation)
	DDX_Check(pDX, IDC_Direct_Inflation, m_DirectInflation);
	DDX_Check(pDX, IDC_Optimization_Inflation, m_OptimizationInflation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigInflation, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigInflation)
	ON_BN_CLICKED(IDC_Config_Direct_Inflation, OnConfigDirectInflation)
	ON_BN_CLICKED(IDC_Config_Optimization_Inflation, OnConfigOptimizationInflation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigInflation message handlers

BOOL CDialogConfigInflation::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(false);

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogConfigInflation::SetParams(CDataRefer GeneralParams)
// Produces local versions of those general parameters that can be modified by this dialog
{
	// Update local copies of the parameters that can be modified by this dialog
	m_DirectInflation= GeneralParams.DirectInflation;
	m_OptimizationInflation= GeneralParams.Optimize;
}


void CDialogConfigInflation::GetParams(CDataRefer &GeneralParams)
// Updates the general parameters that have been modified in the dialog
{
	// Update parameters modified by this dialog
	GeneralParams.DirectInflation= m_DirectInflation !=0;
	if(!GeneralParams.DirectInflation)
		ReferApp.DI.DirectInflationApproach= NO_DIRINFLATE;

	GeneralParams.Optimize= m_OptimizationInflation !=0;
}


void CDialogConfigInflation::OnConfigDirectInflation() 
{
	CDialogConfigDirectInflation Dlg;
	Dlg.SetParams();

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams();
	}
}


void CDialogConfigInflation::OnConfigOptimizationInflation() 
{
	CDialogConfigOptimizationInflation Dlg;
	Dlg.SetParams(ReferApp.In);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.In);
	}
}
