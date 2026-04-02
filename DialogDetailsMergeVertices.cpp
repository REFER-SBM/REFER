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

// DialogFalseVertices.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"

#include "DialogDetailsMergeVertices.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsMergeVertices dialog


CDialogDetailsMergeVertices::CDialogDetailsMergeVertices(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsMergeVertices::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsMergeVertices)
	//}}AFX_DATA_INIT
}


void CDialogDetailsMergeVertices::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsMergeVertices)
	DDX_Check(pDX, IDC_MERGE_CUBIC_CORNERS, m_MergeCubicCorners);
	DDX_Text(pDX, IDC_DISTANCE_MERGE_CUBIC_CORNERS, m_DistanceMergeCubicCorners);
	DDX_Check(pDX, IDC_MERGE_ALIGNED_CUBIC_CORNERS, m_MergeAlignedCubicCorners);
	DDX_Text(pDX, IDC_COEF_MERGE_ROTATION_CUBIC_CORNERS, m_MergeRotationCubicCorners);

	DDX_Check(pDX, IDC_MERGE_CAREFUL_LINE_TIPS, m_MergeCarefulLineTips);
	DDX_Text(pDX, IDC_COEF_MERGE_VERTICES, m_MergeMaxDistVertices);
	DDV_MinMaxDouble(pDX, m_MergeMaxDistVertices, 0., 100.);
	DDX_Text(pDX, IDC_COEF_MERGE_ROTATION, m_MergeMaxVerticesRotation);
	DDV_MinMaxDouble(pDX, m_MergeMaxVerticesRotation, 0., 90.);

	DDX_Text(pDX, IDC_REIFICATION_MERIT, m_ReificationMerit);
	DDV_MinMaxDouble(pDX, m_ReificationMerit, 0., 1.);

	DDX_Check(pDX, IDC_MERGE_CASUAL_LINE_TIPS, m_MergeCasualLineTips);
	DDX_Text(pDX, IDC_MERGE_CASUAL_INTOL, m_ReMergeCasualInTol);
	DDV_MinMaxDouble(pDX, m_ReMergeCasualInTol, 0., 100.);
	DDX_Text(pDX, IDC_MERGE_CASUAL_OUTTOL, m_ReMergeCasualOutTol);
	DDV_MinMaxDouble(pDX, m_ReMergeCasualOutTol, 0., 100.);
	DDX_Check(pDX, IDC_REMERGE_TRIPLETS, m_ReMergeVerticesTriplets);
	DDX_Text(pDX, IDC_COEF_REMERGE_TRIPLETS, m_ReMergeMaxDistTriplets);
	DDV_MinMaxDouble(pDX, m_ReMergeMaxDistTriplets, 0., 100.);

	DDX_Text(pDX, IDC_EMERGENCE_TRIPLETS_MERIT, m_EmergentTripletsMerit);
	DDV_MinMaxDouble(pDX, m_EmergentTripletsMerit, 0., 1.);

	DDX_Check(pDX, IDC_REMERGE_DANGLING, m_ReMergeVerticesDangling);
	DDX_Text(pDX, IDC_COEF_REMERGE_DANGLING, m_ReMergeMaxDistDangling);
	DDV_MinMaxDouble(pDX, m_ReMergeMaxDistDangling, 0., 100.);

	DDX_Text(pDX, IDC_EMERGENCE_DANGLING_MERIT, m_EmergenceDanglingDeMerit);
	DDV_MinMaxDouble(pDX, m_EmergenceDanglingDeMerit, 0., 1.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsMergeVertices, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsMergeVertices)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_R2D_RestoreDefault, OnRestoreDefaultR2D)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsMergeVertices message handlers

void CDialogDetailsMergeVertices::SetParams(CDataRefine2D R2D)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_MergeCubicCorners= R2D.MergeCubicCorners;
	m_DistanceMergeCubicCorners = R2D.DistanceMergeCubicCorners;
	m_MergeAlignedCubicCorners= R2D.MergeAlignedCubicCorners;
	m_MergeRotationCubicCorners = RADIANS_TO_DEGREES(R2D.MergeRotationCubicCorners);
	m_MergeCarefulLineTips= R2D.MergeCarefulLineTips;
	m_MergeMaxDistVertices = R2D.MergeMaxDistVertices*100;
	m_MergeMaxVerticesRotation= RADIANS_TO_DEGREES(R2D.MergeMaxVerticesRotation);
	m_ReificationMerit= R2D.ReificationMerit;
	m_MergeCasualLineTips= R2D.MergeCasualLineTips;
	m_ReMergeCasualInTol= R2D.ReMergeCasualInTol*100;
	m_ReMergeCasualOutTol= R2D.ReMergeCasualOutTol*100;
	m_ReMergeVerticesTriplets= R2D.ReMergeVerticesTriplets;
	m_ReMergeMaxDistTriplets = R2D.ReMergeMaxDistTriplets*100;
	m_EmergentTripletsMerit = R2D.EmergentTripletsMerit;
	m_ReMergeVerticesDangling= R2D.ReMergeVerticesDangling;
	m_ReMergeMaxDistDangling = R2D.ReMergeMaxDistDangling*100;
	m_EmergenceDanglingDeMerit = R2D.EmergenceDanglingDeMerit;
}

void CDialogDetailsMergeVertices::GetParams(CDataRefine2D &R2D)
// Updates the general parameters that have been modified in the dialog
{
	R2D.MergeCubicCorners= (m_MergeCubicCorners!= 0);
	R2D.DistanceMergeCubicCorners = m_DistanceMergeCubicCorners;
	R2D.MergeAlignedCubicCorners= (m_MergeAlignedCubicCorners!= 0);
	R2D.MergeRotationCubicCorners = DEGREES_TO_RADIANS(m_MergeRotationCubicCorners);
	R2D.MergeCarefulLineTips= (m_MergeCarefulLineTips!= 0);
	R2D.MergeMaxDistVertices = m_MergeMaxDistVertices/100;
	R2D.MergeMaxVerticesRotation= DEGREES_TO_RADIANS(m_MergeMaxVerticesRotation);
	R2D.ReificationMerit= m_ReificationMerit;
	R2D.MergeCasualLineTips= (m_MergeCasualLineTips!= 0);
	R2D.ReMergeCasualInTol= m_ReMergeCasualInTol/100;
	R2D.ReMergeCasualOutTol= m_ReMergeCasualOutTol/100;
	R2D.ReMergeVerticesTriplets= (m_ReMergeVerticesTriplets!= 0);
	R2D.ReMergeMaxDistTriplets = m_ReMergeMaxDistTriplets/100;
	R2D.EmergentTripletsMerit = m_EmergentTripletsMerit;
	R2D.ReMergeVerticesDangling= (m_ReMergeVerticesDangling!= 0);
	R2D.ReMergeMaxDistDangling = m_ReMergeMaxDistDangling/100;
	R2D.EmergenceDanglingDeMerit = m_EmergenceDanglingDeMerit;
}


void CDialogDetailsMergeVertices::OnRestoreDefaultR2D() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
		CDataRefine2D Initial;
	SetParams(Initial);
	UpdateData(false);
}
