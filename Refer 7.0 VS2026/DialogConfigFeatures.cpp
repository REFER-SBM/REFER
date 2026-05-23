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

// DialogConfigFeatures.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "DialogDetailsMainAxes.h"
#include "DialogDetailsTypology.h"
#include "DialogDetailsFaces.h"
#include "DialogDetailsHiddenEdges.h"

#include "DialogDetailsRibsAndDividers.h"
#include "DialogDetailsRailsAndSlots.h"
#include "DialogDetailsStepsAndPockets.h"
#include "DialogDetailsChamfersAndRounds.h"    //TBD ChamfersAndRounds

#include "DialogConfigFeatures.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigFeatures dialog


CDialogConfigFeatures::CDialogConfigFeatures(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigFeatures::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigFeatures)
	//}}AFX_DATA_INIT
}


void CDialogConfigFeatures::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigFeatures)
	DDX_Check(pDX, IDC_Detect_Features, m_DetectFeatures);
	DDX_Check(pDX, IDC_FEATURE_DetectRibsAndDividers, m_DetectRibsAndDividers);
	DDX_Check(pDX, IDC_FEATURE_DetectRailsAndSlots, m_DetectRailsAndSlots);
	DDX_Check(pDX, IDC_FEATURE_DetectStepsAndPockets, m_DetectStepsAndPockets);
	//DDX_Check(pDX, IDC_FEATURE_DetectChamfersAndRounds, m_DetectChamfersAndRounds);    //TBD ChamfersAndRounds

	DDX_Text(pDX, IDC_FEATURE_SequenceRibsAndDividers, m_SequenceRibsAndDividers);
	DDX_Text(pDX, IDC_FEATURE_SequenceRailsAndSlots, m_SequenceRailsAndSlots);
	DDX_Text(pDX, IDC_FEATURE_SequenceStepsAndPockets, m_SequenceStepsAndPockets);


	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigFeatures, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigFeatures)
	ON_BN_CLICKED(IDC_DetailsRibsAndDividers, OnDetailsRibsAndDividers)
	ON_BN_CLICKED(IDC_DetailsRailsAndSlots, OnDetailsRailsAndSlots)
	ON_BN_CLICKED(IDC_DetailsStepsAndPockets, OnDetailsStepsAndPockets)
	//ON_BN_CLICKED(IDC_DetailsChamfersAndRounds, OnDetailsChamfersAndRounds)    //TBD ChamfersAndRounds
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigFeatures message handlers

BOOL CDialogConfigFeatures::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(false);

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogConfigFeatures::SetParams(CDataRefer GeneralParams, CDataFeatures &Fe)
// Produces local versions of those general parameters 
// that can be modified by this dialog
{
	m_DetectFeatures= GeneralParams.DetectFeatures;

	m_DetectRibsAndDividers= GeneralParams.DetectRibsAndDividers;
	m_DetectRailsAndSlots= GeneralParams.DetectRailsAndSlots;
	m_DetectStepsAndPockets= GeneralParams.DetectStepsAndPockets;
	//m_DetectChamfersAndRounds= GeneralParams.DetectChamfersAndRounds;    //TBD ChamfersAndRounds

	for(long i=0; i<(long)Fe.featuresSequence.size(); i++){
		if(Fe.featuresSequence[i] == RIB)
			m_SequenceRibsAndDividers= i+1;

		else if(Fe.featuresSequence[i] == RAIL)
			m_SequenceRailsAndSlots= i+1;

		else if(Fe.featuresSequence[i] == STEP)
			m_SequenceStepsAndPockets= i+1;

		//else if(Fe.featuresSequence[i] == CHAMFER)
		//	m_SequenceChamfersAndRounds= i+1;
	}
}


void CDialogConfigFeatures::GetParams(CDataRefer &GeneralParams, CDataFeatures &Fe)
// Updates the general parameters that have been modified in the dialog
{
	GeneralParams.DetectFeatures= m_DetectFeatures !=0;

	GeneralParams.DetectRibsAndDividers= m_DetectRibsAndDividers !=0;
	GeneralParams.DetectRailsAndSlots= m_DetectRailsAndSlots !=0;
	GeneralParams.DetectStepsAndPockets= m_DetectStepsAndPockets !=0;
	//GeneralParams.DetectChamfersAndRounds= m_DetectChamfersAndRounds !=0;    //TBD ChamfersAndRounds

	if(m_SequenceRibsAndDividers != m_SequenceRailsAndSlots && 
	   m_SequenceRibsAndDividers != m_SequenceStepsAndPockets &&
	   m_SequenceStepsAndPockets != m_SequenceRailsAndSlots &&
	   (m_SequenceRibsAndDividers+
	    m_SequenceRailsAndSlots+
	    m_SequenceStepsAndPockets) == (1+2+3)){

		Fe.featuresSequence[m_SequenceRibsAndDividers-1] = RIB;
		Fe.featuresSequence[m_SequenceRailsAndSlots-1] = RAIL;
		Fe.featuresSequence[m_SequenceStepsAndPockets-1] = STEP;
		//Fe.featuresSequence[m_SequenceChamfersAndRounds] = CHAMFER;
	}
	else{
		AfxMessageBox("WARNING: Sequence provided to detect features is invalid");
	}
}


void CDialogConfigFeatures::OnDetailsRibsAndDividers() 
{
	CDialogDetailsRibsAndDividers Dlg;
	Dlg.SetParams(ReferApp.Fe.RD);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.Fe.RD);
	}
}


void CDialogConfigFeatures::OnDetailsRailsAndSlots() 
{
	CDialogDetailsRailsAndSlots Dlg;
	Dlg.SetParams(ReferApp.Fe.RS);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.Fe.RS);
	}
}


void CDialogConfigFeatures::OnDetailsStepsAndPockets() 
{
	CDialogDetailsStepsAndPockets Dlg;
	Dlg.SetParams(ReferApp.Fe.SP);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.Fe.SP);
	}
}


//void CDialogConfigFeatures::OnDetailsChamfersAndRounds()    //TBD ChamfersAndRounds 
//{
//	CDialogDetailsChamfersAndRounds Dlg;
//	Dlg.SetParams(ReferApp.Fe.ChR);
//
//	if (Dlg.DoModal() == IDOK) {
//		Dlg.GetParams(ReferApp.Fe.ChR);
//	}
//}
