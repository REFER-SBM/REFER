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

// DialogDetailsStrokeVectorize.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DataRefer.h"
#include "resource.h"
#include "DialogDetailsStrokeVectorize.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsStrokeVectorize dialog


CDialogDetailsStrokeVectorize::CDialogDetailsStrokeVectorize(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsStrokeVectorize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsStrokeVectorize)
	//}}AFX_DATA_INIT
}


void CDialogDetailsStrokeVectorize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsStrokeVectorize)
	DDX_Check(pDX, IDC_VectorizeLines, m_VectorizeLines);

	DDX_Text(pDX, IDC_EDIT_LineTolMin, m_LineTolMin);
	DDX_Text(pDX, IDC_EDIT_LineTolMax, m_LineTolMax);
	DDX_Text(pDX, IDC_EDIT_LineSmoothingPenalty, m_LineSmoothingPenalty);
	DDX_Text(pDX, IDC_EDIT_MinMeritForLine, m_MinMeritForLine);

	DDX_Radio(pDX, IDC_EDIT_FitLineApproach, m_FitLineApproach);

	DDX_Check(pDX, IDC_EDIT_RemoveTails, m_RemoveTails);
	DDX_Text(pDX, IDC_EDIT_MaxTrimPointsInTail, m_MaxTrimPointsInTail);
	DDX_Text(pDX, IDC_EDIT_MinAngleInTail, m_MinAngleInTail);

	DDX_Check(pDX, IDC_VectorizeArcs, m_VectorizeArcs);

	DDX_Text(pDX, IDC_EDIT_ArcTolMin, m_ArcTolMin);
	DDX_Text(pDX, IDC_EDIT_ArcTolMax, m_ArcTolMax);
	DDX_Text(pDX, IDC_EDIT_ArcSmoothingPenalty, m_ArcSmoothingPenalty);
	DDX_Text(pDX, IDC_EDIT_MinMeritForArc, m_MinMeritForArc);
	DDX_Radio(pDX, IDC_EDIT_FitEllipseApproach, m_FitEllipseApproach);
	DDX_Check(pDX, IDC_EDIT_CheckQuality5P, m_CheckQuality5P);
	DDX_Check(pDX, IDC_EDIT_CheckQualityDIR, m_CheckQualityDIR);
	DDX_Check(pDX, IDC_EDIT_ResampleStrokesDIR, m_ResampleStrokesDIR);
	DDX_Text(pDX, IDC_EDIT_NumResampledPoints, m_NumResampledPoints);
	DDX_Check(pDX, IDC_EDIT_ResampleStrokesGEF, m_ResampleStrokesGEF);
	DDX_Text(pDX, IDC_EDIT_NumResampledPointsGEF, m_NumResampledPointsGEF);
	DDX_Check(pDX, IDC_EDIT_DisplayColors, m_DisplayColors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsStrokeVectorize, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsStrokeVectorize)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsStrokeVectorization message handlers

void CDialogDetailsStrokeVectorize::SetParams(CDataVectorize Ve)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_VectorizeLines= Ve.VectorizeLines;
	m_LineTolMin= Ve.LineTolMin * 100.;
	m_LineTolMax= Ve.LineTolMax * 100.;
	m_LineSmoothingPenalty= Ve.LineSmoothingPenalty * 100.;
	m_MinMeritForLine= Ve.MinMeritForLine;
	m_FitLineApproach= Ve.FitLineApproach;

	m_RemoveTails= Ve.RemoveTails;
	m_MaxTrimPointsInTail= Ve.MaxTrimPointsInTail;
	m_MinAngleInTail= Ve.MinAngleInTail;

	m_VectorizeArcs= Ve.VectorizeArcs;
	m_ArcTolMin= Ve.ArcTolMin * 100.;
	m_ArcTolMax= Ve.ArcTolMax * 100.;
	m_ArcSmoothingPenalty= Ve.ArcSmoothingPenalty * 100.;
	m_MinMeritForArc= Ve.MinMeritForArc;
	m_FitEllipseApproach= Ve.FitEllipseApproach;
	m_CheckQuality5P= Ve.CheckQuality5P;
	m_CheckQualityDIR= Ve.CheckQualityDIR;
	m_ResampleStrokesDIR= Ve.ResampleStrokesDIR;
	m_NumResampledPoints= Ve.NumResampledPoints;
	m_ResampleStrokesGEF= Ve.ResampleStrokesGEF;
	m_NumResampledPointsGEF= Ve.NumResampledPointsGEF;
	m_DisplayColors= Ve.DisplayColors;
}


void CDialogDetailsStrokeVectorize::GetParams(CDataVectorize &Ve)
// Updates the general parameters that have been modified in the dialog
{
	Ve.VectorizeLines= m_VectorizeLines !=0;
	Ve.LineTolMin= m_LineTolMin / 100.;
	Ve.LineTolMax= m_LineTolMax /100.;
	Ve.LineSmoothingPenalty= m_LineSmoothingPenalty / 100.;
	Ve.MinMeritForLine= m_MinMeritForLine;
	Ve.FitLineApproach= m_FitLineApproach;

	Ve.RemoveTails= m_RemoveTails !=0;
	Ve.MaxTrimPointsInTail= m_MaxTrimPointsInTail;
	Ve.MinAngleInTail= m_MinAngleInTail;

	Ve.VectorizeArcs= m_VectorizeArcs !=0;
	Ve.ArcTolMin= m_ArcTolMin / 100.;
	Ve.ArcTolMax= m_ArcTolMax /100.;
	Ve.ArcSmoothingPenalty= m_ArcSmoothingPenalty / 100.;
	Ve.MinMeritForArc= m_MinMeritForArc;
	Ve.FitEllipseApproach= m_FitEllipseApproach;
	Ve.CheckQuality5P= m_CheckQuality5P !=0;
	Ve.CheckQualityDIR= m_CheckQualityDIR !=0;
	Ve.ResampleStrokesDIR= m_ResampleStrokesDIR !=0;
	Ve.NumResampledPoints= m_NumResampledPoints;
	Ve.ResampleStrokesGEF= m_ResampleStrokesGEF !=0;
	Ve.NumResampledPointsGEF= m_NumResampledPointsGEF;
	Ve.DisplayColors= m_DisplayColors !=0;
}


void CDialogDetailsStrokeVectorize::OnBUTTONRestoreDefault()
{
	if(AfxMessageBox(IDSL_BY_DEFAULT, MB_YESNO) == IDNO)
		return;
	CDataVectorize Initial;
	SetParams(Initial);
	UpdateData(false);
}

