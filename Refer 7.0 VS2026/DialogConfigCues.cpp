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

// DialogConfigCues.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "ReferDoc.h"

#include "DialogDetailsValences.h"
#include "DialogDetailsParallelEdges.h"
#include "DialogDetailsVanishingPoints.h"

#include "DialogDetailsSubGraphs.h"
#include "DialogDetailsPerimeter.h"
#include "DialogDetailsLabels.h"
#include "DialogDetailsMainAxes.h"
#include "DialogDetailsFaces.h"
#include "DialogDetailsTypology.h"
#include "DialogDetailsHiddenEdges.h"
#include "DialogDetailsSymmetry.h"

#include "DialogConfigCues.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigCues dialog


CDialogConfigCues::CDialogConfigCues(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigCues::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigCues)
	//}}AFX_DATA_INIT
}


void CDialogConfigCues::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigCues)
	DDX_Check(pDX, IDC_Detect_Cues, m_DetectCues);
	DDX_Check(pDX, IDC_CUE_DetectValences, m_DetectValences);
	DDX_Check(pDX, IDC_CUE_DetectParallelEdges, m_DetectParallelEdges);
	DDX_Check(pDX, IDC_CUE_DetectVanishingPoints, m_DetectVanishingPoints);

	DDX_Check(pDX, IDC_CUE_DetectGraphs, m_DetectGraphs);
	DDX_Check(pDX, IDC_CUE_DetectPerimeter, m_DetectPerimeter);
	DDX_Check(pDX, IDC_CUE_DetectLabels, m_DetectLabels);
	DDX_Check(pDX, IDC_CUE_DetectMainAxes, m_DetectMainAxes);
	DDX_Check(pDX, IDC_CUE_DetectFaces, m_DetectFaces);
	DDX_Check(pDX, IDC_CUE_DetectTypology, m_DetectTypology);
	DDX_Check(pDX, IDC_CUE_DetectHiddenEdges, m_DetectHiddenEdges);
	DDX_Check(pDX, IDC_CUE_DetectSymmetry, m_DetectSymmetry);

	DDX_Check(pDX, IDC_CUE_DetectLevels, m_DetectLevels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigCues, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigCues)
	ON_BN_CLICKED(IDC_DetailsValences, OnDetailsValences)
	ON_BN_CLICKED(IDC_DetailsParallelEdges, OnDetailsParallelEdges)
	ON_BN_CLICKED(IDC_DetailsVanishingPoints, OnDetailsVanishingPoints)

	ON_BN_CLICKED(IDC_DetailsSubGraphs, OnDetailsSubGraphs)
	ON_BN_CLICKED(IDC_DetailsPerimeter, OnDetailsPerimeter)
	ON_BN_CLICKED(IDC_DetailsLabels, OnDetailsLabels)
	ON_BN_CLICKED(IDC_DetailsMainAxes, OnDetailsMainAxes)
	ON_BN_CLICKED(IDC_DetailsFaces, OnDetailsFaces)
	ON_BN_CLICKED(IDC_DetailsTypology, OnDetailsTypology)
	ON_BN_CLICKED(IDC_DetailsHiddenEdges, OnDetailsHiddenEdges)
	ON_BN_CLICKED(IDC_DetailsSymmetry, OnDetailsSymmetry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigCues message handlers

BOOL CDialogConfigCues::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(false);

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogConfigCues::SetParams(CDataRefer GeneralParams)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_DetectCues= GeneralParams.DetectCues;

	m_DetectValences= GeneralParams.DetectValences;
	m_DetectParallelEdges= GeneralParams.DetectParallelEdges;
	m_DetectVanishingPoints= GeneralParams.DetectVanishingPoints;

	m_DetectGraphs= GeneralParams.DetectGraphs;
	m_DetectPerimeter= GeneralParams.DetectPerimeter;
	m_DetectLabels= GeneralParams.DetectLabels;
	m_DetectMainAxes= GeneralParams.DetectMainAxes;
	m_DetectFaces= GeneralParams.DetectFaces;
	m_DetectTypology= GeneralParams.DetectTypology;
	m_DetectHiddenEdges= GeneralParams.DetectHiddenEdges;
	m_DetectSymmetry= GeneralParams.DetectSymmetry;

	m_DetectLevels= GeneralParams.DetectLevels;
}


void CDialogConfigCues::GetParams(CDataRefer &GeneralParams)
// Updates the general parameters that have been modified in the dialog
{
	GeneralParams.DetectCues= m_DetectCues !=0;

	GeneralParams.DetectValences= m_DetectValences !=0;
	GeneralParams.DetectParallelEdges= m_DetectParallelEdges !=0;
	GeneralParams.DetectVanishingPoints= m_DetectVanishingPoints !=0;

	GeneralParams.DetectGraphs= m_DetectGraphs !=0;
	GeneralParams.DetectPerimeter= m_DetectPerimeter !=0;
	GeneralParams.DetectLabels= m_DetectLabels !=0;
	GeneralParams.DetectMainAxes= m_DetectMainAxes !=0;
	GeneralParams.DetectFaces= m_DetectFaces !=0;
	GeneralParams.DetectTypology= m_DetectTypology !=0;
	GeneralParams.DetectHiddenEdges= m_DetectHiddenEdges !=0;
	GeneralParams.DetectSymmetry= m_DetectSymmetry !=0;

	GeneralParams.DetectLevels= m_DetectLevels !=0;
}


void CDialogConfigCues::OnDetailsValences() 
{
	CDialogDetailsValences Dlg;
	Dlg.SetParams(ReferApp.Cu.Va);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.Va);
}


void CDialogConfigCues::OnDetailsParallelEdges() 
{
	CDialogDetailsParallelEdges Dlg;
	Dlg.SetParams(ReferApp.Cu.PE);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.PE);
}


void CDialogConfigCues::OnDetailsVanishingPoints() 
{
	CDialogDetailsVanishingPoints Dlg;
	Dlg.SetParams(ReferApp.Cu.VP);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.VP);
}


void CDialogConfigCues::OnDetailsSubGraphs() 
{
	CDialogDetailsSubGraphs Dlg;
	Dlg.SetParams(ReferApp.Cu.SG);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.SG);
}


void CDialogConfigCues::OnDetailsPerimeter() 
{
	CDialogDetailsPerimeter Dlg;
	Dlg.SetParams(ReferApp.Cu.Pe);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.Pe);
}


void CDialogConfigCues::OnDetailsLabels() 
{
	CDialogDetailsLabels Dlg;
	Dlg.SetParams(ReferApp.Cu.La);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.La);
}


void CDialogConfigCues::OnDetailsMainAxes() 
{
	CDialogDetailsMainAxes Dlg;
	Dlg.SetParams(ReferApp.Cu.MA);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.MA);
}


void CDialogConfigCues::OnDetailsFaces() 
{
	CDialogDetailsFaces Dlg;
	Dlg.SetParams(ReferApp.Cu);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu);
}


void CDialogConfigCues::OnDetailsTypology() 
{
	CDialogDetailsTypology Dlg;
	Dlg.SetParams(ReferApp.Cu.Ty);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.Ty);
}


void CDialogConfigCues::OnDetailsHiddenEdges() 
{
	CDialogDetailsHiddenEdges Dlg;
	Dlg.SetParams(ReferApp.Cu.Hi);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.Hi);
}


void CDialogConfigCues::OnDetailsSymmetry() 
{
	CDialogDetailsSymmetry Dlg;
	Dlg.SetParams(ReferApp.Cu.Sy);
	if (Dlg.DoModal() == IDOK){
		Dlg.GetParams(ReferApp.Cu.Sy);
	}
}
