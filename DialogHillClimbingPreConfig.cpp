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

// DialogHillClimbingPreConfig.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "Refer.h"
#include "resource.h"
#include "DialogHillClimbingPreConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogHillClimbingPreConfig dialog


CDialogHillClimbingPreConfig::CDialogHillClimbingPreConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogHillClimbingPreConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogHillClimbingPreConfig)
	//}}AFX_DATA_INIT
}



void CDialogHillClimbingPreConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogHillClimbingPreConfig)
	DDX_Radio(pDX, IDC_RADIO_Velocidad, m_RadioPorDefecto);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogHillClimbingPreConfig, CDialog)
	//{{AFX_MSG_MAP(CDialogHillClimbingPreConfig)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogHillClimbingPreConfig message handlers


void CDialogHillClimbingPreConfig::SetParams(const CDataOptHC Params)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_RadioPorDefecto=Params.iPreConfigured;
}

void CDialogHillClimbingPreConfig::GetParams(CDataOptHC &Params)
// Updates the general parameters that have been modified in the dialog
{
	Params.iPreConfigured = m_RadioPorDefecto;

	switch(Params.iPreConfigured){
		case 0: //Velocidad
			Params.dInitialStep = 0.1;
			Params.dStepReduction = 0.2;
			Params.dFinalStep = 0.005;
			Params.iMaxIterations = 100;
			Params.dThresholdRejectBestCost = 0;
			break;
		case 1: //Accuracy
			Params.dInitialStep = 0.02;
			Params.dStepReduction = 0.4;
			Params.dFinalStep = 0.0006;
			Params.iMaxIterations = 500;
			Params.dThresholdRejectBestCost = 0;
			break;
		case 2: //Funcional
			Params.dInitialStep = 0.025;
			Params.dStepReduction = 0.5;
			Params.dFinalStep = 0.0015;
			Params.iMaxIterations = 100;
			Params.dThresholdRejectBestCost = 0;
			break;
	}

}


