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

// DialogConfiguracionRefer.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"

#include "DialogSettingsRefer.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogSettingsRefer dialog


CDialogSettingsRefer::CDialogSettingsRefer(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSettingsRefer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSettingsRefer)
	//}}AFX_DATA_INIT
}


void CDialogSettingsRefer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSettingsRefer)
	DDX_Check(pDX, IDC_REFER_Refine_On_Line, m_Refine_On_Line);
	DDX_Check(pDX, IDC_REFER_Refine_Off_Line, m_Refine_Off_Line);
	DDX_Check(pDX, IDC_REFER_Detect_CuesAndFeatures, m_Detect_CuesAndFeatures);

	DDX_Check(pDX, IDC_REFER_Detect_Cues, m_Detect_Cues);
	DDX_Check(pDX, IDC_REFER_Detect_Valences, m_Detect_Valences);
	DDX_Check(pDX, IDC_REFER_Detect_ParallelEdges, m_Detect_ParallelEdges);
	DDX_Check(pDX, IDC_REFER_Detect_VanishingPoints, m_Detect_VanishingPoints);
	DDX_Check(pDX, IDC_REFER_Detect_Graphs, m_Detect_Graphs);
	DDX_Check(pDX, IDC_REFER_Detect_Perimeter, m_Detect_Perimeter);
	DDX_Check(pDX, IDC_REFER_Detect_Labels, m_Detect_Labels);
	DDX_Check(pDX, IDC_REFER_Detect_MainAxes, m_Detect_MainAxes);
	DDX_Check(pDX, IDC_REFER_Detect_Faces, m_Detect_Faces);
	DDX_Check(pDX, IDC_REFER_Detect_Typology, m_Detect_Typology);
	DDX_Check(pDX, IDC_REFER_Detect_HiddenEdges, m_Detect_HiddenEdges);
	DDX_Check(pDX, IDC_REFER_Detect_Symmetry, m_Detect_Symmetry);
	DDX_Check(pDX, IDC_REFER_Detect_Levels, m_Detect_Levels);

	DDX_Check(pDX, IDC_REFER_Detect_Features, m_Detect_Features);
	DDX_Check(pDX, IDC_REFER_Detect_RibsAndDividers, m_Detect_RibsAndDividers);
	DDX_Check(pDX, IDC_REFER_Detect_RailsAndSlots, m_Detect_RailsAndSlots);
	DDX_Check(pDX, IDC_REFER_Detect_StepsAndPockets, m_Detect_StepsAndPockets);

	DDX_Check(pDX, IDC_REFER_InflateBRep, m_InflateBRep);
	DDX_Check(pDX, IDC_REFER_Direct_Inflation, m_Direct_Inflation);
	DDX_Check(pDX, IDC_REFER_Optimization_based_Inflation, m_Optimization_based_Inflation);
	DDX_Check(pDX, IDC_REFER_CSG_Modeling, m_CSG_Modeling);
	DDX_Check(pDX, IDC_REFER_Refine_3D, m_Refine_3D);
	DDX_Check(pDX, IDC_REFER_Tessellate_3D, m_Tessellate_3D);
	DDX_Text(pDX, IDC_REFER_MRU_Files, m_MRU_Files);
	DDX_Radio(pDX, IDC_IGS_TYPE_143, m_IGS_Type_143);
	DDX_Radio(pDX, IDC_IGS_NGON_FACES, m_IGS_NgonFaces);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogSettingsRefer, CDialog)
	//{{AFX_MSG_MAP(CDialogSettingsRefer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogSettingsRefer message handlers

void CDialogSettingsRefer::SetParams()
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Refine_On_Line= ReferApp.GeneralParams.bRefine2D_OnLine;
	m_Refine_Off_Line= ReferApp.GeneralParams.bRefine2D_OffLine;

	m_Detect_CuesAndFeatures= ReferApp.GeneralParams.bDetectCuesAndFeatures;

	m_Detect_Cues= ReferApp.GeneralParams.DetectCues;
	m_Detect_Valences= ReferApp.GeneralParams.DetectValences;
	m_Detect_ParallelEdges= ReferApp.GeneralParams.DetectParallelEdges;
	m_Detect_VanishingPoints= ReferApp.GeneralParams.DetectVanishingPoints;
	m_Detect_Graphs= ReferApp.GeneralParams.DetectGraphs;
	m_Detect_Perimeter= ReferApp.GeneralParams.DetectPerimeter;
	m_Detect_Labels= ReferApp.GeneralParams.DetectLabels;
	m_Detect_MainAxes= ReferApp.GeneralParams.DetectMainAxes;
	m_Detect_Faces= ReferApp.GeneralParams.DetectFaces;
	m_Detect_Typology= ReferApp.GeneralParams.DetectTypology;
	m_Detect_HiddenEdges= ReferApp.GeneralParams.DetectHiddenEdges;
	m_Detect_Symmetry= ReferApp.GeneralParams.DetectSymmetry;
	m_Detect_Levels= ReferApp.GeneralParams.DetectLevels;

	m_Detect_Features= ReferApp.GeneralParams.DetectFeatures;
	m_Detect_RibsAndDividers= ReferApp.GeneralParams.DetectRibsAndDividers;
	m_Detect_RailsAndSlots= ReferApp.GeneralParams.DetectRailsAndSlots;
	m_Detect_StepsAndPockets= ReferApp.GeneralParams.DetectStepsAndPockets;

	m_InflateBRep= ReferApp.GeneralParams.bInflateBRep;
	m_Direct_Inflation= ReferApp.GeneralParams.DirectInflation;
	m_Optimization_based_Inflation= ReferApp.GeneralParams.Optimize;
	m_Refine_3D= ReferApp.GeneralParams.bRefine3D;
	m_Tessellate_3D= ReferApp.GeneralParams.bTessellate3D;
	m_CSG_Modeling= ReferApp.GeneralParams.bBuildModelCSG;
	m_MRU_Files= ReferApp.GeneralParams.MRU_Files;
	m_IGS_Type_143= !ReferApp.GeneralParams.bIGS_Type_143;
	m_IGS_NgonFaces= !ReferApp.GeneralParams.bIGS_NgonFaces;
}


void CDialogSettingsRefer::GetParams()
// Updates the general parameters that have been modified in the dialog
{
	ReferApp.GeneralParams.bRefine2D_OnLine= (m_Refine_On_Line != 0);
	ReferApp.GeneralParams.bRefine2D_OffLine= (m_Refine_Off_Line != 0);
	ReferApp.GeneralParams.bDetectCuesAndFeatures= (m_Detect_CuesAndFeatures != 0);

	ReferApp.GeneralParams.DetectCues= (m_Detect_Cues != 0);
	ReferApp.GeneralParams.DetectValences= (m_Detect_Valences != 0);
	ReferApp.GeneralParams.DetectParallelEdges= (m_Detect_ParallelEdges != 0);
	ReferApp.GeneralParams.DetectVanishingPoints= (m_Detect_VanishingPoints != 0);
	ReferApp.GeneralParams.DetectGraphs= (m_Detect_Graphs != 0);
	ReferApp.GeneralParams.DetectPerimeter= (m_Detect_Perimeter != 0);
	ReferApp.GeneralParams.DetectLabels= (m_Detect_Labels != 0);
	ReferApp.GeneralParams.DetectMainAxes= (m_Detect_MainAxes != 0);
	ReferApp.GeneralParams.DetectFaces= (m_Detect_Faces != 0);
	ReferApp.GeneralParams.DetectTypology= (m_Detect_Typology != 0);
	ReferApp.GeneralParams.DetectHiddenEdges= (m_Detect_HiddenEdges != 0);
	ReferApp.GeneralParams.DetectSymmetry= (m_Detect_Symmetry != 0);
	ReferApp.GeneralParams.DetectLevels= (m_Detect_Levels != 0);

	ReferApp.GeneralParams.DetectFeatures= (m_Detect_Features != 0);
	ReferApp.GeneralParams.DetectRibsAndDividers= (m_Detect_RibsAndDividers != 0);
	ReferApp.GeneralParams.DetectRailsAndSlots= (m_Detect_RailsAndSlots != 0);
	ReferApp.GeneralParams.DetectStepsAndPockets= (m_Detect_StepsAndPockets != 0);

	ReferApp.GeneralParams.bInflateBRep= (m_InflateBRep != 0);
	ReferApp.GeneralParams.DirectInflation= (m_Direct_Inflation != 0);
	ReferApp.GeneralParams.Optimize= (m_Optimization_based_Inflation != 0);
	ReferApp.GeneralParams.bRefine3D= (m_Refine_3D != 0);
	ReferApp.GeneralParams.bTessellate3D= (m_Tessellate_3D != 0);
	ReferApp.GeneralParams.bBuildModelCSG= (m_CSG_Modeling != 0);
	ReferApp.GeneralParams.MRU_Files= m_MRU_Files;
	ReferApp.GeneralParams.bIGS_Type_143= (m_IGS_Type_143 == 0);
	ReferApp.GeneralParams.bIGS_NgonFaces= (m_IGS_NgonFaces == 0);
}


BOOL CDialogSettingsRefer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//UpdateData(false);

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
