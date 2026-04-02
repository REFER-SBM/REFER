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

// DialogDetailsSymmetry.cpp : implementation file

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"

#include "DialogDetailsSymmetry.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSymmetry dialog


CDialogDetailsSymmetry::CDialogDetailsSymmetry(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsSymmetry::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsMainAxes)
	//}}AFX_DATA_INIT
}


void CDialogDetailsSymmetry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsMainAxes)
	DDX_Check(pDX, IDC_SYM_DetectNormalCrossing, m_DetectNormalCrossing);
	DDX_Text(pDX, IDC_SYM_MaxParallelCrossing, m_MaxParallelCrossing);
	DDX_Text(pDX, IDC_SYM_MinParallelCrossing, m_MinParallelCrossing);
	DDX_Text(pDX, IDC_SYM_MeritWeight_1, m_MeritWeight_1);

	DDX_Check(pDX, IDC_SYM_DetectDistorted, m_DetectDistorted);
	DDX_Text(pDX, IDC_SYM_MinSymDistortion, m_MinSymDistortion);
	DDX_Text(pDX, IDC_SYM_MaxSymDistortion, m_MaxSymDistortion);
	DDX_Text(pDX, IDC_SYM_MaxParallelFaceSymLines, m_MaxParallelFaceSymLines);
	DDX_Text(pDX, IDC_SYM_MinParallelFaceSymLines, m_MinParallelFaceSymLines);
	DDX_Text(pDX, IDC_SYM_MeritWeight_2, m_MeritWeight_2);

	DDX_Check(pDX, IDC_SYM_SymmetricalEdges, m_SymmetricalEdges);
	DDX_Text(pDX, IDC_SYM_MaxParallelEdgeSymLines, m_MaxParallelEdgeSymLines);
	DDX_Text(pDX, IDC_SYM_MinParallelEdgeSymLines, m_MinParallelEdgeSymLines);
	DDX_Text(pDX, IDC_SYM_MeritWeight_3, m_MeritWeight_3);

	DDX_Check(pDX, IDC_SYM_PrioritizeVertical, m_PrioritizeVertical);
	DDX_Text(pDX, IDC_SYM_minVerticalThreshold, m_minVerticalThreshold);
	DDX_Text(pDX, IDC_SYM_maxVerticalThreshold, m_maxVerticalThreshold);
	DDX_Text(pDX, IDC_SYM_MeritWeight_4, m_MeritWeight_4);

	DDX_Check(pDX, IDC_SYM_RemoveBadMerit, m_RemoveBadMerit);
	DDX_Text(pDX, IDC_SYM_MinMerit, m_MinMerit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsSymmetry, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsMainAxes)
	ON_BN_CLICKED(IDC_BUTTON_DetailsSymmetryDefault, OnBUTTONDetailsSymmetryDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSymmetry message handlers


void CDialogDetailsSymmetry::SetParams(CDataSymmetry SY)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_DetectNormalCrossing = SY.DetectNormalCrossing;
	m_MaxParallelCrossing= RADIANS_TO_DEGREES(SY.MaxParallelCrossing);
	m_MinParallelCrossing= RADIANS_TO_DEGREES(SY.MinParallelCrossing);
	m_MeritWeight_1= 100*SY.MeritWeight_1;

	m_DetectDistorted = SY.DetectDistorted;
	m_MinSymDistortion= 100*SY.MinSymDistortion;
	m_MaxSymDistortion= 100*SY.MaxSymDistortion;
	m_MaxParallelFaceSymLines= RADIANS_TO_DEGREES(SY.MaxParallelFaceSymLines);
	m_MinParallelFaceSymLines= RADIANS_TO_DEGREES(SY.MinParallelFaceSymLines);
	m_MeritWeight_2= 100*SY.MeritWeight_2;

	m_SymmetricalEdges = SY.SymmetricalEdges;
	m_MaxParallelEdgeSymLines= RADIANS_TO_DEGREES(SY.MaxParallelEdgeSymLines);
	m_MinParallelEdgeSymLines= RADIANS_TO_DEGREES(SY.MinParallelEdgeSymLines);
	m_MeritWeight_3= 100*SY.MeritWeight_3;

	m_PrioritizeVertical = SY.PrioritizeVertical;
	m_minVerticalThreshold = RADIANS_TO_DEGREES(SY.minVerticalThreshold);
	m_maxVerticalThreshold = RADIANS_TO_DEGREES(SY.maxVerticalThreshold);
	m_MeritWeight_4= 100*SY.MeritWeight_4;

	m_RemoveBadMerit = SY.RemoveBadMerit;
	m_MinMerit = SY.MinMerit;
}


void CDialogDetailsSymmetry::GetParams(CDataSymmetry &SY)
// Updates the general parameters that have been modified in the dialog
{
	SY.DetectNormalCrossing= m_DetectNormalCrossing !=0;
	SY.MaxParallelCrossing= DEGREES_TO_RADIANS(m_MaxParallelCrossing);
	SY.MinParallelCrossing= DEGREES_TO_RADIANS(m_MinParallelCrossing);
	SY.MeritWeight_1= m_MeritWeight_1/100;

	SY.DetectDistorted= m_DetectDistorted !=0;
	SY.MinSymDistortion= m_MinSymDistortion/100;
	SY.MaxSymDistortion= m_MaxSymDistortion/100;
	SY.MaxParallelFaceSymLines= DEGREES_TO_RADIANS(m_MaxParallelFaceSymLines);
	SY.MinParallelFaceSymLines= DEGREES_TO_RADIANS(m_MinParallelFaceSymLines);
	SY.MeritWeight_2= m_MeritWeight_2/100;

	SY.SymmetricalEdges= m_SymmetricalEdges !=0;
	SY.MaxParallelEdgeSymLines= DEGREES_TO_RADIANS(m_MaxParallelEdgeSymLines);
	SY.MinParallelEdgeSymLines= DEGREES_TO_RADIANS(m_MinParallelEdgeSymLines);
	SY.MeritWeight_3= m_MeritWeight_3/100;

	SY.PrioritizeVertical= m_PrioritizeVertical !=0;
	SY.minVerticalThreshold= DEGREES_TO_RADIANS(m_minVerticalThreshold);
	SY.maxVerticalThreshold= DEGREES_TO_RADIANS(m_maxVerticalThreshold);
	SY.MeritWeight_4= m_MeritWeight_4/100;

	SY.RemoveBadMerit= m_RemoveBadMerit !=0;
	SY.MinMerit= m_MinMerit;
}


void CDialogDetailsSymmetry::OnBUTTONDetailsSymmetryDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataSymmetry Initial;
	SetParams(Initial);

	UpdateData(false);
}
