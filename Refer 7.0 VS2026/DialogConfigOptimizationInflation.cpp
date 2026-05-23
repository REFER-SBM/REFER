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

#include "DialogConfigOptimizationInflation.h"
#include "DialogDetailsHillClimbing.h"
#include "DialogDetailsSimulatedAnnealing.h"
#include "DialogDetailsSimulatedAnnealingMulti.h"
#include "DialogDetailsRegularitiesNature.h"
#include "DialogDetailsRegularitiesUser.h"
#include "DialogDetailsRegularitiesCost.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigInflation dialog


CDialogConfigOptimizationInflation::CDialogConfigOptimizationInflation(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigOptimizationInflation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigOptimizationInflation)
	//}}AFX_DATA_INIT
}


void CDialogConfigOptimizationInflation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigInflation)
	DDX_Radio(pDX, IDC_RADIO_OptimizationAlgorithm, m_OptAlgorithm);
	DDX_Radio(pDX, IDC_RADIO_SelectRegularities, m_SelectRegularities);
	DDX_Text(pDX, IDC_InflationAngleThreshold, m_InflationAngleThreshold);
	DDV_MinMaxDouble(pDX, m_InflationAngleThreshold, 0., 90.);
	DDX_Radio(pDX, IDC_RADIO_NormalizeCosts, m_NormalizeCosts);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigOptimizationInflation, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigInflation)
	ON_BN_CLICKED(IDC_DetailsHillClimbing, OnDetailsHillClimbing)
	ON_BN_CLICKED(IDC_DetailsSimulatedAnnealing, OnDetailsSimulatedAnnealing)
	ON_BN_CLICKED(IDC_DetailsSimulatedAnnealingMulti, OnDetailsSimulatedAnnealingMulti)

	ON_BN_CLICKED(IDC_DetailsRegularitiesNature, OnDetailsRegularitiesNature)
	ON_BN_CLICKED(IDC_DetailsRegularitiesUser, OnDetailsRegularitiesUser)

	ON_BN_CLICKED(IDC_DetailsRegularitiesCost, OnDetailsRegularitiesCost)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigOptimizationInflation message handlers


BOOL CDialogConfigOptimizationInflation::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//if (ReferApp.GeneralParams.DetectTypology==false){
	//	CButton *pBotonRegularidadesAutomatico = (CButton*) GetDlgItem(IDC_RADIO_SelectRegularities);

	//	pBotonRegularidadesAutomatico->EnableWindow(false);
	//	m_SelectRegularities = 0;  //Automatica
	//}

	//if (m_OptAlgorithm == SIMULATED_ANNEALING_MULTI){
	//	CButton *pBoton = (CButton*) GetDlgItem(IDC_RADIO_NormalizeCosts);
	//	pBoton->EnableWindow(false);
	//}

	UpdateData(false);

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogConfigOptimizationInflation::SetParams(CDataInflation In)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_OptAlgorithm = In.iOptAlgorithm;
	m_SelectRegularities = In.iRegularitiesSelection;

	m_InflationAngleThreshold = RADIANS_TO_DEGREES(In.InflationAngleThreshold);

	if (m_OptAlgorithm == SIMULATED_ANNEALING_MULTI)
		m_NormalizeCosts = 1;
	else
		m_NormalizeCosts = In.iNormalizeCosts;

	m_DataOptHC= In.DataOptHC;
	m_DataOptSA= In.DataOptSA;
	m_DataOptSAM= In.DataOptSAM;
}


void CDialogConfigOptimizationInflation::GetParams(CDataInflation &In)
// Updates the general parameters that have been modified in the dialog
{
	In.iOptAlgorithm= m_OptAlgorithm;
	In.iRegularitiesSelection= m_SelectRegularities;
	In.iNormalizeCosts= m_NormalizeCosts;

	In.InflationAngleThreshold= DEGREES_TO_RADIANS(m_InflationAngleThreshold);

	In.DataOptHC= m_DataOptHC;
	In.DataOptSA= m_DataOptSA;
	In.DataOptSAM= m_DataOptSAM;
}


void CDialogConfigOptimizationInflation::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataInflation initial;
	SetParams(initial);

	UpdateData(false);
}


void CDialogConfigOptimizationInflation::OnDetailsHillClimbing() 
{
	bool ok = false;
	CDialogDetailsHillClimbing Dlg;

	while (!ok){
		Dlg.SetParams(m_DataOptHC);
		if (Dlg.DoModal() == IDOK) {
			bool vale = Dlg.GetParams(m_DataOptHC);
			if (!vale)
				AfxMessageBox("A void list of step lengths is not valid!");
			else
				ok = true;
		}
		else
			ok = true;
	}
}

void CDialogConfigOptimizationInflation::OnDetailsSimulatedAnnealing() 
{
	bool ok = false;
	CDialogDetailsSimulatedAnnealing Dlg;

	while(!ok){
		Dlg.SetParams(m_DataOptSA);
		if (Dlg.DoModal() == IDOK) {
			bool vale = Dlg.GetParams(m_DataOptSA);
			if (!vale)
				AfxMessageBox("A void list of step lengths is not valid!");
			else
				ok = true;
		}
		else
			ok =true;
	}
}


void CDialogConfigOptimizationInflation::OnDetailsSimulatedAnnealingMulti() 
{
	bool ok = false;
	CDialogDetailsSimulatedAnnealingMulti Dlg;

	while(!ok){
		Dlg.SetParams(m_DataOptSAM);
		if (Dlg.DoModal() == IDOK) {
			bool vale = Dlg.GetParams(m_DataOptSAM);
			if (!vale)
				AfxMessageBox("A void list of step lengths is not valid!");
			else
				ok = true;
		}
		else
			ok =true;
	}
}


void CDialogConfigOptimizationInflation::OnDetailsRegularitiesNature() 
{
	CDialogDetailsRegularitiesNature Dlg;
	Dlg.SetParams(ReferApp.In);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.In);
	}
}


void CDialogConfigOptimizationInflation::OnDetailsRegularitiesUser() 
{
	CDialogDetailsRegularitiesUser Dlg;
	Dlg.SetParams(ReferApp.In);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.In);
	}
}


void CDialogConfigOptimizationInflation::OnDetailsRegularitiesCost() 
{
	CDialogDetailsRegularitiesCost Dlg;
	Dlg.SetParams(ReferApp.In, this);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.In);
	}
}
