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
#include "Refer.h"
#include "resource.h"

#include "DialogDetailsStrokeVectorize.h"
#include "DialogDetailsGroupOvertracedStrokes.h"
#include "DialogDetailsGroupNonSolidStrokes.h"
#include "DialogConfigVectorize2D.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigRefine2D dialog


CDialogConfigVectorize2D::CDialogConfigVectorize2D(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigVectorize2D::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigVectorize2D)
	//}}AFX_DATA_INIT
}


void CDialogConfigVectorize2D::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigVectorize2D)
	DDX_Check(pDX, IDC_Vectorize_Int, m_Vectorize_Int);
	DDX_Check(pDX, IDC_Vectorize_Batch, m_Vectorize_Batch);
	DDX_Check(pDX, IDC_ParseGestures, m_ParseGestures);

	DDX_Radio(pDX, IDC_StrokeCornersApproach, m_StrokeCornersApproach);
	DDX_Radio(pDX, IDC_RecognitionApproach, m_RecognitionApproach);
	DDX_Check(pDX, IDC_GroupOvertracedStrokes, m_GroupOvertracedStrokes);
	DDX_Check(pDX, IDC_GroupNonSolidStrokes, m_GroupNonSolidStrokes);
	DDX_Check(pDX, IDC_SegmentStrokes, m_SegmentStrokes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigVectorize2D, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigVectorize2D)
	ON_BN_CLICKED(IDC_Vectorize_Batch, OnUpdate)
	ON_BN_CLICKED(IDC_Vectorize_Int, OnUpdate)
	ON_BN_CLICKED(IDC_DetailsStrokeVectorize, OnDetailsStrokeVectorize)
	ON_BN_CLICKED(IDC_DetailsGroupOvertracedStrokes, OnDetailsGroupOvertracedStrokes)
	ON_BN_CLICKED(IDC_DetailsGroupNonSolidStrokes, OnDetailsGroupNonSolidStrokes)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigVectorize2D message handlers

void CDialogConfigVectorize2D::SetParams(CDataRefer GeneralParams, CDataVectorize Ve)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Vectorize_Int= GeneralParams.bVectorize_Int;
	m_Vectorize_Batch= GeneralParams.bVectorize_Batch;
	m_ParseGestures= Ve.ParseGestures;
	m_RecognitionApproach= Ve.RecognitionApproach;
	m_StrokeCornersApproach= Ve.StrokeCornersApproach;
	m_GroupOvertracedStrokes= Ve.GroupOvertracedStrokes;
	m_GroupNonSolidStrokes= Ve.GroupNonSolidStrokes;
	m_SegmentStrokes= Ve.SegmentStrokes;
}


void CDialogConfigVectorize2D::GetParams(CDataRefer &GeneralParams, CDataVectorize &Ve)
{
	GeneralParams.bVectorize_Int= m_Vectorize_Int !=0;
	GeneralParams.bVectorize_Batch= m_Vectorize_Batch !=0;
	Ve.ParseGestures= m_ParseGestures !=0;
	Ve.RecognitionApproach= m_RecognitionApproach;
	Ve.StrokeCornersApproach= m_StrokeCornersApproach;
	Ve.GroupOvertracedStrokes= m_GroupOvertracedStrokes !=0;
	Ve.GroupNonSolidStrokes= m_GroupNonSolidStrokes !=0;
	Ve.SegmentStrokes= m_SegmentStrokes !=0;
}


void CDialogConfigVectorize2D::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataRefer initial1;
	CDataVectorize initial2;
	SetParams(initial1, initial2);
	UpdateData(false);
}


BOOL CDialogConfigVectorize2D::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogConfigVectorize2D::OnUpdate() 
{
	UpdateData(true);
}


void CDialogConfigVectorize2D::OnDetailsStrokeVectorize() 
{
	CDialogDetailsStrokeVectorize Dlg;
	Dlg.SetParams(ReferApp.Ve);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.Ve);
	}
}


void CDialogConfigVectorize2D::OnDetailsGroupOvertracedStrokes() 
{
	CDialogDetailsGroupOvertracedStrokes Dlg;
	Dlg.SetParams(ReferApp.Ve);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.Ve);
	}
}


void CDialogConfigVectorize2D::OnDetailsGroupNonSolidStrokes() 
{
	CDialogDetailsGroupNonSolidStrokes Dlg;
	Dlg.SetParams(ReferApp.Ve);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.Ve);
	}
}

