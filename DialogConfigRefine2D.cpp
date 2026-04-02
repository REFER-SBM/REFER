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

// DialogConfigRefine2D.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "ReferDoc.h"

#include "DialogConfigRefine2D.h"
#include "DialogDetailsMergeVertices.h"
#include "DialogDetailsHillClimbing.h"
#include "DialogRegularities2D.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigRefine2D dialog


CDialogConfigRefine2D::CDialogConfigRefine2D(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigRefine2D::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigRefine2D)
	//}}AFX_DATA_INIT
}


void CDialogConfigRefine2D::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigRefine2D)
	DDX_Check(pDX, IDC_REFINE_ON_LINE, m_Refine_On_Line);
	DDX_Check(pDX, IDC_REFINE_OFF_LINE, m_Refine_Off_Line);

	DDX_Check(pDX, IDC_MergeVertices, m_MergeVertices);

	DDX_Check(pDX, IDC_Merge_T_Vertices, m_Merge_T_Vertices);
	DDX_Check(pDX, IDC_Split_T_Vertices, m_Split_T_Vertices);
	DDX_Text(pDX, IDC_ToleranceSplitTVertices, m_ToleranceSplitTVertices);

	DDX_Check(pDX, IDC_Ignore_ShortEdges, m_IgnoreShortEdges);
	DDX_Text(pDX, IDC_Tolerance_Ignore_ShortEdges, m_ToleranceIgnoreShortEdges);

	DDX_Check(pDX, IDC_MergeCollinearEdges, m_MergeCollinearEdges);
	DDX_Text(pDX, IDC_ToleranceMergeCollinearEdges, m_ToleranceMergeCollinearEdges);

	DDX_Check(pDX, IDC_SplitNon2ManifoldVertices, m_SplitNon2ManifoldVertices);
	DDX_Check(pDX, IDC_SplitNon2ManifoldEdges, m_SplitNon2ManifoldEdges);

	DDX_Check(pDX, IDC_Optimize2D, m_Optimize2D);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigRefine2D, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigRefine2D)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	ON_BN_CLICKED(IDC_DetailsMergeVertices, OnDetailsMergeVertices)
	ON_BN_CLICKED(IDC_DetailsRegularitiesReconstruction2D, OnDetailsRegularitiesReconstruction2D)
	ON_BN_CLICKED(IDC_DetailsHillClimbing2D, OnDetailsHillClimbing2D)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigRefine2D message handlers

void CDialogConfigRefine2D::SetParams(const CDataRefer &GeneralParams,
									  const CDataRefine2D R2D)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Refine_On_Line= GeneralParams.bRefine2D_OnLine;
	m_Refine_Off_Line= GeneralParams.bRefine2D_OffLine;

	m_MergeVertices = R2D.bMergeVertices;

	m_IgnoreShortEdges= R2D.IgnoreShortEdges;
	m_ToleranceIgnoreShortEdges = R2D.ToleranceIgnoreShortEdges*100;

	m_Merge_T_Vertices= R2D.Merge_T_Vertices;
	m_Split_T_Vertices= R2D.Split_T_Vertices;
	m_ToleranceSplitTVertices = R2D.ToleranceSplitTVertices*100;

	m_MergeCollinearEdges= R2D.MergeCollinearEdges;
	m_ToleranceMergeCollinearEdges = RADIANS_TO_DEGREES(R2D.ToleranceMergeCollinearEdges);

	m_SplitNon2ManifoldVertices= R2D.SplitNon2ManifoldVertices;
	m_SplitNon2ManifoldEdges= R2D.SplitNon2ManifoldEdges;

	m_Optimize2D = R2D.bOptimize2D;

	m_DataOptHC_2D= R2D.DataOptHC_2D;
}


void CDialogConfigRefine2D::GetParams(CDataRefer &GeneralParams,
									  CDataRefine2D &R2D)
{
	GeneralParams.bRefine2D_OnLine=m_Refine_On_Line !=0;
	GeneralParams.bRefine2D_OffLine=m_Refine_Off_Line !=0;

	R2D.bMergeVertices = m_MergeVertices !=0;

	R2D.IgnoreShortEdges= m_IgnoreShortEdges !=0;
	R2D.ToleranceIgnoreShortEdges=m_ToleranceIgnoreShortEdges/100;

	R2D.Merge_T_Vertices= m_Merge_T_Vertices !=0;
	R2D.Split_T_Vertices= m_Split_T_Vertices !=0;
	R2D.ToleranceSplitTVertices = m_ToleranceSplitTVertices/100;

	R2D.MergeCollinearEdges= m_MergeCollinearEdges !=0;
	R2D.ToleranceMergeCollinearEdges = DEGREES_TO_RADIANS(m_ToleranceMergeCollinearEdges);

	R2D.SplitNon2ManifoldVertices= m_SplitNon2ManifoldVertices !=0;
	R2D.SplitNon2ManifoldEdges= m_SplitNon2ManifoldEdges !=0;

	R2D.bOptimize2D = m_Optimize2D !=0;

	//Stores updated parameters of the optimization algorithm
	R2D.DataOptHC_2D= m_DataOptHC_2D;
}


void CDialogConfigRefine2D::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataRefer initial1;
	CDataRefine2D initial2;
	SetParams(initial1, initial2);

	UpdateData(false);
}


BOOL CDialogConfigRefine2D::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogConfigRefine2D::OnDetailsMergeVertices() 
{
	CDialogDetailsMergeVertices Dlg;
	Dlg.SetParams(ReferApp.R2D);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.R2D);
}


void CDialogConfigRefine2D::OnDetailsRegularitiesReconstruction2D() 
{
	CDialogRegularities2D Dlg;
	Dlg.SetParams(ReferApp.R2D);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.R2D);
}

void CDialogConfigRefine2D::OnDetailsHillClimbing2D() 
{
	bool ok = false;
	CDialogDetailsHillClimbing Dlg;

	while (!ok){
		Dlg.SetParams(m_DataOptHC_2D);
		if (Dlg.DoModal() == IDOK) {
			bool vale = Dlg.GetParams(m_DataOptHC_2D);
			if (!vale)
				AfxMessageBox("A void list of step lengths is not valid!");
			else
				ok = true;
		}
		else
			ok = true;
	}
}
